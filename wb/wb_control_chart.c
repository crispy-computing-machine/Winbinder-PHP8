#include "wb.h"

#define CHART_MAGIC 0x43485254
#define CHART_MAX_SERIES 16

typedef struct
{
	DWORD dwMagic;
	int nSeriesCount;
	int nPointCount;
	int nXLabelCount;
	int nYLabelCount;
	double *pSeries[CHART_MAX_SERIES];
	TCHAR **ppszXLabels;
	TCHAR **ppszYLabels;
	COLORREF colors[CHART_MAX_SERIES];
	BOOL bShowAxis;
	BOOL bShowGrid;
	BOOL bAutoRange;
	double dAxisMin;
	double dAxisMax;
	BOOL bPopupEnabled;
	COLORREF clPopupBack;
	COLORREF clPopupText;
	COLORREF clPopupBorder;
	BOOL bMouseTracking;
	int nHoverSeries;
	int nHoverPoint;
} CHARTDATA, *PCHARTDATA;

static PCHARTDATA ChartGetData(PWBOBJ pwbo)
{
	if (!pwbo || pwbo->uClass != ChartControl || !pwbo->lparam)
		return NULL;
	if (((PCHARTDATA)pwbo->lparam)->dwMagic != CHART_MAGIC)
		return NULL;
	return (PCHARTDATA)pwbo->lparam;
}

static TCHAR *ChartDupUtf8Label(const char *pszLabel)
{
	if (!pszLabel)
		return NULL;

#ifdef UNICODE
	{
		int nChars = MultiByteToWideChar(CP_UTF8, 0, pszLabel, -1, NULL, 0);
		TCHAR *pszWide;
		if (nChars <= 0)
			return NULL;
		pszWide = wbMalloc(sizeof(TCHAR) * nChars);
		if (!pszWide)
			return NULL;
		if (!MultiByteToWideChar(CP_UTF8, 0, pszLabel, -1, pszWide, nChars))
		{
			wbFree(pszWide);
			return NULL;
		}
		return pszWide;
	}
#else
	return wbStrDup(pszLabel);
#endif
}

static void ChartFreeLabels(TCHAR ***pppszLabels, int *pnCount)
{
	int i;
	if (!pppszLabels || !pnCount || !*pppszLabels)
	{
		if (pnCount)
			*pnCount = 0;
		return;
	}

	for (i = 0; i < *pnCount; i++)
	{
		if ((*pppszLabels)[i])
			wbFree((*pppszLabels)[i]);
	}

	wbFree(*pppszLabels);
	*pppszLabels = NULL;
	*pnCount = 0;
}

static void ChartFormatDouble(TCHAR *pszOut, int nOutChars, double value)
{
	if (!pszOut || nOutChars <= 0)
		return;

#ifdef UNICODE
	_snwprintf(pszOut, nOutChars - 1, L"%.2f", value);
#else
	_snprintf(pszOut, nOutChars - 1, "%.2f", value);
#endif
	pszOut[nOutChars - 1] = TEXT('\0');
}

static BOOL ChartSetLabelSet(TCHAR ***pppszTarget, int *pnTargetCount, const char **ppszLabels, int nCount)
{
	int i;
	TCHAR **ppszNew;

	if (!pppszTarget || !pnTargetCount || nCount < 0)
		return FALSE;

	ChartFreeLabels(pppszTarget, pnTargetCount);

	if (!ppszLabels || nCount == 0)
		return TRUE;

	ppszNew = wbCalloc(nCount, sizeof(TCHAR *));
	if (!ppszNew)
		return FALSE;

	for (i = 0; i < nCount; i++)
	{
		if (ppszLabels[i])
		{
			ppszNew[i] = ChartDupUtf8Label(ppszLabels[i]);
			if (!ppszNew[i])
			{
				int j;
				for (j = 0; j < i; j++)
					if (ppszNew[j])
						wbFree(ppszNew[j]);
				wbFree(ppszNew);
				return FALSE;
			}
		}
	}

	*pppszTarget = ppszNew;
	*pnTargetCount = nCount;
	return TRUE;
}

static const TCHAR *ChartGetXLabel(PCHARTDATA pData, int nPoint, TCHAR *pszFallback, int nFallbackChars)
{
	if (pData->ppszXLabels && nPoint >= 0 && nPoint < pData->nXLabelCount && pData->ppszXLabels[nPoint])
		return pData->ppszXLabels[nPoint];

	if (pszFallback && nFallbackChars > 1)
	{
#ifdef UNICODE
		_snwprintf(pszFallback, nFallbackChars - 1, L"%d", nPoint);
#else
		_snprintf(pszFallback, nFallbackChars - 1, "%d", nPoint);
#endif
		pszFallback[nFallbackChars - 1] = TEXT('\0');
		return pszFallback;
	}

	return TEXT("");
}

static const TCHAR *ChartGetYLabel(PCHARTDATA pData, double value, TCHAR *pszFallback, int nFallbackChars)
{
	if (pData->ppszYLabels && pData->nYLabelCount > 0)
	{
		double span = pData->dAxisMax - pData->dAxisMin;
		int idx = 0;
		if (span > 0)
		{
			double t = (value - pData->dAxisMin) / span;
			if (t < 0)
				t = 0;
			if (t > 1)
				t = 1;
			idx = (int)(t * (pData->nYLabelCount - 1) + 0.5);
		}
		if (idx >= 0 && idx < pData->nYLabelCount && pData->ppszYLabels[idx])
			return pData->ppszYLabels[idx];
	}

	ChartFormatDouble(pszFallback, nFallbackChars, value);
	return pszFallback;
}

static void ChartDrawPopup(HDC hdc, RECT *prcClient, RECT *prcPlot, PCHARTDATA pData)
{
	RECT rcPopup;
	SIZE szLine1, szLine2, szLine3;
	TCHAR szLine1[256], szLine2[256], szLine3[256];
	TCHAR szXFallback[64], szYFallback[64], szValue[64];
	const TCHAR *pszX;
	const TCHAR *pszY;
	int px, py;
	int nW, nH;
	double value;
	HBRUSH hbr;
	HPEN hPen, hOldPen;
	HFONT hFont, hOldFont;

	if (!pData->bPopupEnabled || pData->nHoverSeries < 0 || pData->nHoverPoint < 0)
		return;
	if (pData->nHoverSeries >= pData->nSeriesCount || !pData->pSeries[pData->nHoverSeries])
		return;
	if (pData->nHoverPoint >= pData->nPointCount)
		return;

	value = pData->pSeries[pData->nHoverSeries][pData->nHoverPoint];
	pszX = ChartGetXLabel(pData, pData->nHoverPoint, szXFallback, NUMITEMS(szXFallback));
	pszY = ChartGetYLabel(pData, value, szYFallback, NUMITEMS(szYFallback));
	ChartFormatDouble(szValue, NUMITEMS(szValue), value);

#ifdef UNICODE
	_snwprintf(szLine1, NUMITEMS(szLine1) - 1, L"X: %ls", pszX);
	_snwprintf(szLine2, NUMITEMS(szLine2) - 1, L"Y: %ls", pszY);
	_snwprintf(szLine3, NUMITEMS(szLine3) - 1, L"Value: %ls", szValue);
#else
	_snprintf(szLine1, NUMITEMS(szLine1) - 1, "X: %s", pszX);
	_snprintf(szLine2, NUMITEMS(szLine2) - 1, "Y: %s", pszY);
	_snprintf(szLine3, NUMITEMS(szLine3) - 1, "Value: %s", szValue);
#endif
	szLine1[NUMITEMS(szLine1) - 1] = TEXT('\0');
	szLine2[NUMITEMS(szLine2) - 1] = TEXT('\0');
	szLine3[NUMITEMS(szLine3) - 1] = TEXT('\0');

	GetTextExtentPoint32(hdc, szLine1, (int)_tcslen(szLine1), &szLine1);
	GetTextExtentPoint32(hdc, szLine2, (int)_tcslen(szLine2), &szLine2);
	GetTextExtentPoint32(hdc, szLine3, (int)_tcslen(szLine3), &szLine3);

	nW = MAX(MAX(szLine1.cx, szLine2.cx), szLine3.cx) + 12;
	nH = szLine1.cy + szLine2.cy + szLine3.cy + 12;

	ChartPointToScreen(prcPlot, pData, pData->nHoverPoint, value, &px, &py);
	rcPopup.left = px + 10;
	rcPopup.top = py - nH - 10;
	rcPopup.right = rcPopup.left + nW;
	rcPopup.bottom = rcPopup.top + nH;

	if (rcPopup.right > prcClient->right - 2)
	{
		rcPopup.right = prcClient->right - 2;
		rcPopup.left = rcPopup.right - nW;
	}
	if (rcPopup.left < prcClient->left + 2)
	{
		rcPopup.left = prcClient->left + 2;
		rcPopup.right = rcPopup.left + nW;
	}
	if (rcPopup.top < prcClient->top + 2)
	{
		rcPopup.top = py + 10;
		rcPopup.bottom = rcPopup.top + nH;
	}
	if (rcPopup.bottom > prcClient->bottom - 2)
	{
		rcPopup.bottom = prcClient->bottom - 2;
		rcPopup.top = rcPopup.bottom - nH;
	}

	hbr = CreateSolidBrush(pData->clPopupBack);
	FillRect(hdc, &rcPopup, hbr);
	DeleteObject(hbr);

	hPen = CreatePen(PS_SOLID, 1, pData->clPopupBorder);
	hOldPen = (HPEN)SelectObject(hdc, hPen);
	Rectangle(hdc, rcPopup.left, rcPopup.top, rcPopup.right, rcPopup.bottom);
	SelectObject(hdc, hOldPen);
	DeleteObject(hPen);

	SetBkMode(hdc, TRANSPARENT);
	SetTextColor(hdc, pData->clPopupText);
	hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	hOldFont = (HFONT)SelectObject(hdc, hFont);
	TextOut(hdc, rcPopup.left + 6, rcPopup.top + 4, szLine1, (int)_tcslen(szLine1));
	TextOut(hdc, rcPopup.left + 6, rcPopup.top + 4 + szLine1.cy, szLine2, (int)_tcslen(szLine2));
	TextOut(hdc, rcPopup.left + 6, rcPopup.top + 4 + szLine1.cy + szLine2.cy, szLine3, (int)_tcslen(szLine3));
	SelectObject(hdc, hOldFont);
}

static void ChartUpdateRange(PCHARTDATA pData)
{
	int i, j;
	double minV = 0, maxV = 0;
	BOOL first = TRUE;
	if (!pData || !pData->bAutoRange)
		return;
	for (i = 0; i < pData->nSeriesCount; i++)
	{
		if (!pData->pSeries[i])
			continue;
		for (j = 0; j < pData->nPointCount; j++)
		{
			double v = pData->pSeries[i][j];
			if (first)
			{
				minV = maxV = v;
				first = FALSE;
			}
			else
			{
				if (v < minV)
					minV = v;
				if (v > maxV)
					maxV = v;
			}
		}
	}
	if (first)
	{
		minV = 0;
		maxV = 1;
	}
	if (minV == maxV)
		maxV = minV + 1;
	pData->dAxisMin = minV;
	pData->dAxisMax = maxV;
}

static void ChartPointToScreen(RECT *prcPlot, PCHARTDATA pData, int nPoint, double v, int *px, int *py)
{
	double span = pData->dAxisMax - pData->dAxisMin;
	if (span <= 0)
		span = 1;
	if (pData->nPointCount <= 1)
		*px = prcPlot->left;
	else
		*px = prcPlot->left + (nPoint * (prcPlot->right - prcPlot->left)) / (pData->nPointCount - 1);
	*py = prcPlot->bottom - (int)(((v - pData->dAxisMin) * (prcPlot->bottom - prcPlot->top)) / span);
}

static BOOL ChartHitTest(PCHARTDATA pData, RECT *prcPlot, int x, int y, int *pnSeries, int *pnPoint)
{
	int i, j;
	for (i = 0; i < pData->nSeriesCount; i++)
	{
		if (!pData->pSeries[i])
			continue;
		for (j = 0; j < pData->nPointCount; j++)
		{
			int px, py;
			ChartPointToScreen(prcPlot, pData, j, pData->pSeries[i][j], &px, &py);
			if (abs(px - x) <= 5 && abs(py - y) <= 5)
			{
				*pnSeries = i;
				*pnPoint = j;
				return TRUE;
			}
		}
	}
	return FALSE;
}

static LRESULT CALLBACK ChartControlProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	PWBOBJ pwbo = (PWBOBJ)GetWindowLongPtr(hwnd, GWLP_USERDATA);
	PCHARTDATA pData = ChartGetData(pwbo);
	RECT rcClient, rcPlot;

	switch (msg)
	{
	case WM_ERASEBKGND:
		return 1;
	case WM_MOUSEMOVE:
		if (pwbo && pData)
		{
			TRACKMOUSEEVENT tme;
			int series = -1, point = -1;
			if (!pData->bMouseTracking)
			{
				tme.cbSize = sizeof(TRACKMOUSEEVENT);
				tme.dwFlags = TME_LEAVE;
				tme.hwndTrack = hwnd;
				tme.dwHoverTime = 0;
				TrackMouseEvent(&tme);
				pData->bMouseTracking = TRUE;
			}
			GetClientRect(hwnd, &rcClient);
			rcPlot = rcClient;
			rcPlot.left += 40;
			rcPlot.right -= 10;
			rcPlot.top += 10;
			rcPlot.bottom -= 25;
			if (ChartHitTest(pData, &rcPlot, LOWORD(lParam), HIWORD(lParam), &series, &point))
			{
				if (series != pData->nHoverSeries || point != pData->nHoverPoint)
				{
					pData->nHoverSeries = series;
					pData->nHoverPoint = point;
					InvalidateRect(hwnd, NULL, FALSE);
					if (BITTEST(pwbo->style, WBC_NOTIFY) && BITTEST(pwbo->lparam, WBC_CHART_POINT_HOVER) && pwbo->parent && pwbo->parent->pszCallBackFn && *pwbo->parent->pszCallBackFn)
						wbCallUserFunction(pwbo->parent->pszCallBackFn, pwbo->parent->pszCallBackObj, pwbo->parent, pwbo, pwbo->id, WBC_CHART_POINT_HOVER, MAKELPARAM(point, series), 0);
				}
			}
			else if (pData->nHoverSeries != -1 || pData->nHoverPoint != -1)
			{
				pData->nHoverSeries = -1;
				pData->nHoverPoint = -1;
				InvalidateRect(hwnd, NULL, FALSE);
			}
		}
		break;
	case WM_MOUSELEAVE:
		if (pData)
		{
			pData->bMouseTracking = FALSE;
			if (pData->nHoverSeries != -1 || pData->nHoverPoint != -1)
			{
				pData->nHoverSeries = -1;
				pData->nHoverPoint = -1;
				InvalidateRect(hwnd, NULL, FALSE);
			}
		}
		break;
	case WM_LBUTTONDOWN:
		if (pwbo && pData && BITTEST(pwbo->style, WBC_NOTIFY) && BITTEST(pwbo->lparam, WBC_CHART_POINT_CLICK) && pwbo->parent && pwbo->parent->pszCallBackFn && *pwbo->parent->pszCallBackFn)
		{
			int series = -1, point = -1;
			GetClientRect(hwnd, &rcClient);
			rcPlot = rcClient;
			rcPlot.left += 40;
			rcPlot.right -= 10;
			rcPlot.top += 10;
			rcPlot.bottom -= 25;
			if (ChartHitTest(pData, &rcPlot, LOWORD(lParam), HIWORD(lParam), &series, &point))
				wbCallUserFunction(pwbo->parent->pszCallBackFn, pwbo->parent->pszCallBackObj, pwbo->parent, pwbo, pwbo->id, WBC_CHART_POINT_CLICK, MAKELPARAM(point, series), 0);
		}
		break;
	case WM_PAINT:
		if (pData)
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);
			HPEN hAxisPen = CreatePen(PS_SOLID, 1, RGB(90, 90, 90));
			HBRUSH hBack = CreateSolidBrush(RGB(255, 255, 255));
			int i, j;
			GetClientRect(hwnd, &rcClient);
			FillRect(hdc, &rcClient, hBack);
			DeleteObject(hBack);
			rcPlot = rcClient;
			rcPlot.left += 40;
			rcPlot.right -= 10;
			rcPlot.top += 10;
			rcPlot.bottom -= 25;
			ChartUpdateRange(pData);
			if (pData->bShowGrid)
			{
				HPEN hGrid = CreatePen(PS_DOT, 1, RGB(220, 220, 220));
				HPEN hOldGrid = (HPEN)SelectObject(hdc, hGrid);
				for (i = 1; i < 5; i++)
				{
					int y = rcPlot.top + ((rcPlot.bottom - rcPlot.top) * i / 5);
					MoveToEx(hdc, rcPlot.left, y, NULL);
					LineTo(hdc, rcPlot.right, y);
				}
				SelectObject(hdc, hOldGrid);
				DeleteObject(hGrid);
			}
			if (pData->bShowAxis)
			{
				HPEN hOld = (HPEN)SelectObject(hdc, hAxisPen);
				TCHAR szAxis[64];
				MoveToEx(hdc, rcPlot.left, rcPlot.top, NULL);
				LineTo(hdc, rcPlot.left, rcPlot.bottom);
				LineTo(hdc, rcPlot.right, rcPlot.bottom);
				SelectObject(hdc, hOld);

				SetBkMode(hdc, TRANSPARENT);
				ChartFormatDouble(szAxis, NUMITEMS(szAxis), pData->dAxisMax);
				TextOut(hdc, 4, rcPlot.top - 6, szAxis, (int)_tcslen(szAxis));
				ChartFormatDouble(szAxis, NUMITEMS(szAxis), pData->dAxisMin);
				TextOut(hdc, 4, rcPlot.bottom - 10, szAxis, (int)_tcslen(szAxis));

			}
			for (i = 0; i < pData->nSeriesCount; i++)
			{
				HPEN hPen;
				HPEN hOld;
				if (!pData->pSeries[i])
					continue;
				hPen = CreatePen(PS_SOLID, 2, pData->colors[i]);
				hOld = (HPEN)SelectObject(hdc, hPen);
				for (j = 0; j < pData->nPointCount; j++)
				{
					int x, y;
					ChartPointToScreen(&rcPlot, pData, j, pData->pSeries[i][j], &x, &y);
					if (j == 0)
						MoveToEx(hdc, x, y, NULL);
					else
						LineTo(hdc, x, y);
					Ellipse(hdc, x - 2, y - 2, x + 2, y + 2);
					if (i == pData->nHoverSeries && j == pData->nHoverPoint)
						Ellipse(hdc, x - 5, y - 5, x + 5, y + 5);
				}
				SelectObject(hdc, hOld);
				DeleteObject(hPen);
			}
			ChartDrawPopup(hdc, &rcClient, &rcPlot, pData);
			DeleteObject(hAxisPen);
			EndPaint(hwnd, &ps);
			return 0;
		}
		break;
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

BOOL RegisterChartControlClass(void)
{
	WNDCLASS wc;
	memset(&wc, 0, sizeof(WNDCLASS));
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = (WNDPROC)ChartControlProc;
	wc.hInstance = hAppInstance;
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszClassName = CHART_CONTROL_CLASS;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	if (!RegisterClass(&wc) && GetLastError() != ERROR_CLASS_ALREADY_EXISTS)
		return FALSE;
	return TRUE;
}

BOOL wbChartInitControl(PWBOBJ pwbo)
{
	PCHARTDATA pData;
	if (!pwbo || pwbo->uClass != ChartControl)
		return FALSE;
	pData = wbCalloc(1, sizeof(CHARTDATA));
	if (!pData)
		return FALSE;
	pData->dwMagic = CHART_MAGIC;
	pData->bShowAxis = TRUE;
	pData->bShowGrid = TRUE;
	pData->bAutoRange = TRUE;
	pData->dAxisMin = 0;
	pData->dAxisMax = 100;
	pData->bPopupEnabled = TRUE;
	pData->clPopupBack = RGB(255, 255, 225);
	pData->clPopupText = RGB(0, 0, 0);
	pData->clPopupBorder = RGB(90, 90, 90);
	pData->bMouseTracking = FALSE;
	pData->nHoverSeries = -1;
	pData->nHoverPoint = -1;
	pData->colors[0] = RGB(0, 122, 204);
	pData->colors[1] = RGB(220, 53, 69);
	pData->colors[2] = RGB(40, 167, 69);
	pData->colors[3] = RGB(255, 193, 7);
	pwbo->lparam = (LPARAM)pData;
	return TRUE;
}

BOOL wbChartSetSeries(PWBOBJ pwbo, int nSeries, const double *pValues, int nCount)
{
	PCHARTDATA pData = ChartGetData(pwbo);
	if (!pData || nSeries < 0 || nSeries >= CHART_MAX_SERIES || nCount <= 0 || !pValues)
		return FALSE;
	if (pData->pSeries[nSeries])
		wbFree(pData->pSeries[nSeries]);
	pData->pSeries[nSeries] = wbMalloc(sizeof(double) * nCount);
	if (!pData->pSeries[nSeries])
		return FALSE;
	memcpy(pData->pSeries[nSeries], pValues, sizeof(double) * nCount);
	pData->nPointCount = nCount;
	if (nSeries + 1 > pData->nSeriesCount)
		pData->nSeriesCount = nSeries + 1;
	ChartUpdateRange(pData);
	return wbChartRefresh(pwbo);
}

BOOL wbChartSetLabels(PWBOBJ pwbo, const char **ppszLabels, int nCount)
{
	PCHARTDATA pData = ChartGetData(pwbo);
	if (!pData || nCount < 0)
		return FALSE;
	if (!ChartSetLabelSet(&pData->ppszXLabels, &pData->nXLabelCount, ppszLabels, nCount))
		return FALSE;
	return wbChartRefresh(pwbo);
}

BOOL wbChartSetYLabels(PWBOBJ pwbo, const char **ppszLabels, int nCount)
{
	PCHARTDATA pData = ChartGetData(pwbo);
	if (!pData || nCount < 0)
		return FALSE;
	if (!ChartSetLabelSet(&pData->ppszYLabels, &pData->nYLabelCount, ppszLabels, nCount))
		return FALSE;
	return wbChartRefresh(pwbo);
}

BOOL wbChartSetColors(PWBOBJ pwbo, const COLORREF *pColors, int nCount)
{
	PCHARTDATA pData = ChartGetData(pwbo);
	int i;
	if (!pData || !pColors)
		return FALSE;
	for (i = 0; i < nCount && i < CHART_MAX_SERIES; i++)
		pData->colors[i] = pColors[i];
	return wbChartRefresh(pwbo);
}

BOOL wbChartSetAxis(PWBOBJ pwbo, BOOL bShowAxis, BOOL bShowGrid, double dMin, double dMax, BOOL bAutoRange)
{
	PCHARTDATA pData = ChartGetData(pwbo);
	if (!pData)
		return FALSE;
	pData->bShowAxis = bShowAxis;
	pData->bShowGrid = bShowGrid;
	pData->bAutoRange = bAutoRange;
	if (!bAutoRange)
	{
		pData->dAxisMin = dMin;
		pData->dAxisMax = (dMax > dMin) ? dMax : dMin + 1;
	}
	ChartUpdateRange(pData);
	return wbChartRefresh(pwbo);
}

BOOL wbChartSetPopup(PWBOBJ pwbo, BOOL bEnabled, COLORREF clBack, COLORREF clText, COLORREF clBorder)
{
	PCHARTDATA pData = ChartGetData(pwbo);
	if (!pData)
		return FALSE;
	pData->bPopupEnabled = bEnabled;
	pData->clPopupBack = clBack;
	pData->clPopupText = clText;
	pData->clPopupBorder = clBorder;
	if (!bEnabled)
	{
		pData->nHoverSeries = -1;
		pData->nHoverPoint = -1;
	}
	return wbChartRefresh(pwbo);
}

BOOL wbChartRefresh(PWBOBJ pwbo)
{
	if (!pwbo || !IsWindow(pwbo->hwnd))
		return FALSE;
	InvalidateRect(pwbo->hwnd, NULL, TRUE);
	UpdateWindow(pwbo->hwnd);
	return TRUE;
}

BOOL wbChartDestroy(PWBOBJ pwbo)
{
	PCHARTDATA pData = ChartGetData(pwbo);
	int i;
	if (!pData)
		return FALSE;
	for (i = 0; i < CHART_MAX_SERIES; i++)
		if (pData->pSeries[i])
			wbFree(pData->pSeries[i]);
	ChartFreeLabels(&pData->ppszXLabels, &pData->nXLabelCount);
	ChartFreeLabels(&pData->ppszYLabels, &pData->nYLabelCount);
	pData->dwMagic = 0;
	wbFree(pData);
	pwbo->lparam = 0;
	return TRUE;
}
