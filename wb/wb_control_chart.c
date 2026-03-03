#include "wb.h"

#define CHART_MAGIC 0x43485254
#define CHART_MAX_SERIES 16

typedef struct
{
	DWORD dwMagic;
	int nSeriesCount;
	int nPointCount;
	int nLabelCount;
	double *pSeries[CHART_MAX_SERIES];
	TCHAR **ppszLabels;
	COLORREF colors[CHART_MAX_SERIES];
	BOOL bShowAxis;
	BOOL bShowGrid;
	BOOL bAutoRange;
	double dAxisMin;
	double dAxisMax;
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
			int series = -1, point = -1;
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
				wsprintf(szAxis, TEXT("%.2f"), pData->dAxisMax);
				TextOut(hdc, 4, rcPlot.top - 6, szAxis, (int)_tcslen(szAxis));
				wsprintf(szAxis, TEXT("%.2f"), pData->dAxisMin);
				TextOut(hdc, 4, rcPlot.bottom - 10, szAxis, (int)_tcslen(szAxis));
			}

			if (pData->ppszLabels && pData->nLabelCount > 0)
			{
				for (i = 0; i < pData->nLabelCount && i < pData->nPointCount; i++)
				{
					int x, y, nLabelX;
					SIZE sz;
					if (!pData->ppszLabels[i])
						continue;
					ChartPointToScreen(&rcPlot, pData, i, pData->dAxisMin, &x, &y);
					GetTextExtentPoint32(hdc, pData->ppszLabels[i], (int)_tcslen(pData->ppszLabels[i]), &sz);
					nLabelX = x - (sz.cx / 2);
					if (nLabelX < rcClient.left + 2)
						nLabelX = rcClient.left + 2;
					if (nLabelX + sz.cx > rcClient.right - 2)
						nLabelX = (rcClient.right - 2) - sz.cx;
					TextOut(hdc, nLabelX, rcPlot.bottom + 4, pData->ppszLabels[i], (int)_tcslen(pData->ppszLabels[i]));
				}
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
	int i;
	if (!pData || nCount < 0)
		return FALSE;
	if (pData->ppszLabels)
	{
		for (i = 0; i < pData->nLabelCount; i++)
			if (pData->ppszLabels[i])
				wbFree(pData->ppszLabels[i]);
		wbFree(pData->ppszLabels);
		pData->ppszLabels = NULL;
		pData->nLabelCount = 0;
	}
	if (nCount == 0 || !ppszLabels)
		return TRUE;
	pData->ppszLabels = wbCalloc(nCount, sizeof(TCHAR *));
	if (!pData->ppszLabels)
		return FALSE;
	pData->nLabelCount = nCount;
	for (i = 0; i < nCount; i++)
		if (ppszLabels[i])
		{
			pData->ppszLabels[i] = ChartDupUtf8Label(ppszLabels[i]);
			if (!pData->ppszLabels[i])
			{
				int j;
				for (j = 0; j < i; j++)
					if (pData->ppszLabels[j])
						wbFree(pData->ppszLabels[j]);
				wbFree(pData->ppszLabels);
				pData->ppszLabels = NULL;
				pData->nLabelCount = 0;
				return FALSE;
			}
		}
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
	if (pData->ppszLabels)
	{
		for (i = 0; i < pData->nLabelCount; i++)
			if (pData->ppszLabels[i])
				wbFree(pData->ppszLabels[i]);
		wbFree(pData->ppszLabels);
	}
	pData->dwMagic = 0;
	wbFree(pData);
	pwbo->lparam = 0;
	return TRUE;
}
