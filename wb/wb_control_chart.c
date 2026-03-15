#include "wb.h"
#include <math.h>

#define CHART_MAGIC 0x43485254
#define CHART_MAX_TOOLTIP 256

typedef struct
{
	DWORD magic;
	LONG_PTR notifyMask;
	int seriesCount;
	WBCHARTSERIES *series;
	LPTSTR title;
	LPTSTR xAxis;
	LPTSTR yAxis;
	int padding;
	BOOL hasMinX;
	BOOL hasMaxX;
	BOOL hasMinY;
	BOOL hasMaxY;
	double minX;
	double maxX;
	double minY;
	double maxY;
	COLORREF bgColor;
	COLORREF gridColor;
	COLORREF axisTextColor;
	COLORREF *palette;
	int paletteCount;
	int hoverSeries;
	int hoverPoint;
	HWND hTooltip;
	TCHAR tooltipText[CHART_MAX_TOOLTIP];
} WBCHARTDATA;

static void wbChartFreeString(LPTSTR psz)
{
	if (psz)
		wbFree(psz);
}

static LPTSTR wbChartDup(LPCTSTR psz)
{
	size_t n;
	LPTSTR out;
	if (!psz)
		psz = TEXT("");
	n = wcslen(psz);
	out = wbMalloc((n + 1) * sizeof(TCHAR));
	wcscpy(out, psz);
	return out;
}

static void wbChartClearSeries(WBCHARTDATA *cd)
{
	if (!cd || !cd->series)
		return;
	wbChartFreeSeriesBuffer(cd->series, cd->seriesCount);
	cd->series = NULL;
	cd->seriesCount = 0;
}

void wbChartFreeSeriesBuffer(WBCHARTSERIES *series, int count)
{
	int i, j;
	if (!series)
		return;
	for (i = 0; i < count; i++)
	{
		wbChartFreeString(series[i].name);
		if (series[i].points)
		{
			for (j = 0; j < series[i].pointCount; j++)
			{
				wbChartFreeString(series[i].points[j].xLabel);
				wbChartFreeString(series[i].points[j].label);
			}
			wbFree(series[i].points);
		}
	}
	wbFree(series);
}

static WBCHARTDATA *wbChartGetData(PWBOBJ pwbo)
{
	if (!pwbo || pwbo->uClass != ChartControl || !pwbo->lparam)
		return NULL;
	if (((WBCHARTDATA *)pwbo->lparam)->magic != CHART_MAGIC)
		return NULL;
	return (WBCHARTDATA *)pwbo->lparam;
}

static COLORREF wbChartGetSeriesColor(WBCHARTDATA *cd, int i)
{
	static COLORREF defaults[] = {
		RGB(0x1f, 0x77, 0xb4), RGB(0xff, 0x7f, 0x0e), RGB(0x2c, 0xa0, 0x2c),
		RGB(0xd6, 0x27, 0x28), RGB(0x94, 0x67, 0xbd), RGB(0x8c, 0x56, 0x4b)};
	if (cd->paletteCount > 0 && cd->palette)
		return cd->palette[i % cd->paletteCount];
	return defaults[i % 6];
}

static void wbChartComputeBounds(WBCHARTDATA *cd, double *minX, double *maxX, double *minY, double *maxY)
{
	int i, j;
	BOOL hasPoint = FALSE;
	*minX = 0;
	*maxX = 1;
	*minY = 0;
	*maxY = 1;
	for (i = 0; i < cd->seriesCount; i++)
	{
		for (j = 0; j < cd->series[i].pointCount; j++)
		{
			double x = cd->series[i].points[j].x;
			double y = cd->series[i].points[j].y;
			if (!hasPoint)
			{
				*minX = *maxX = x;
				*minY = *maxY = y;
				hasPoint = TRUE;
			}
			else
			{
				*minX = MIN(*minX, x);
				*maxX = MAX(*maxX, x);
				*minY = MIN(*minY, y);
				*maxY = MAX(*maxY, y);
			}
		}
	}
	if (!hasPoint)
		return;
	if (*maxX == *minX)
		*maxX = *minX + 1;
	if (*maxY == *minY)
		*maxY = *minY + 1;
	if (cd->hasMinX)
		*minX = cd->minX;
	if (cd->hasMaxX)
		*maxX = cd->maxX;
	if (cd->hasMinY)
		*minY = cd->minY;
	if (cd->hasMaxY)
		*maxY = cd->maxY;
}

static int wbChartMapX(double x, double minX, double maxX, RECT *plot)
{
	return plot->left + (int)((x - minX) / (maxX - minX) * (plot->right - plot->left));
}

static int wbChartMapY(double y, double minY, double maxY, RECT *plot)
{
	return plot->bottom - (int)((y - minY) / (maxY - minY) * (plot->bottom - plot->top));
}

static void wbChartDraw(WBCHARTDATA *cd, HDC hdc, RECT *rc)
{
	RECT plot = *rc;
	int i, j;
	double minX, maxX, minY, maxY;
	HBRUSH hbg = CreateSolidBrush(cd->bgColor);
	FillRect(hdc, rc, hbg);
	DeleteObject(hbg);

	plot.left += cd->padding;
	plot.top += cd->padding + 20;
	plot.right -= cd->padding;
	plot.bottom -= cd->padding;
	if (plot.right <= plot.left || plot.bottom <= plot.top)
		return;

	wbChartComputeBounds(cd, &minX, &maxX, &minY, &maxY);

	SetBkMode(hdc, TRANSPARENT);
	SetTextColor(hdc, cd->axisTextColor);
	if (cd->title)
		TextOut(hdc, rc->left + cd->padding, rc->top + 4, cd->title, (int)wcslen(cd->title));

	{
		HPEN hGrid = CreatePen(PS_SOLID, 1, cd->gridColor);
		HPEN old = SelectObject(hdc, hGrid);
		for (i = 0; i <= 5; i++)
		{
			int gy = plot.top + (plot.bottom - plot.top) * i / 5;
			MoveToEx(hdc, plot.left, gy, NULL);
			LineTo(hdc, plot.right, gy);
		}
		for (i = 0; i <= 5; i++)
		{
			int gx = plot.left + (plot.right - plot.left) * i / 5;
			MoveToEx(hdc, gx, plot.top, NULL);
			LineTo(hdc, gx, plot.bottom);
		}
		SelectObject(hdc, old);
		DeleteObject(hGrid);
	}

	{
		HPEN hAxis = CreatePen(PS_SOLID, 1, cd->axisTextColor);
		HPEN old = SelectObject(hdc, hAxis);
		MoveToEx(hdc, plot.left, plot.bottom, NULL);
		LineTo(hdc, plot.right, plot.bottom);
		MoveToEx(hdc, plot.left, plot.top, NULL);
		LineTo(hdc, plot.left, plot.bottom);
		SelectObject(hdc, old);
		DeleteObject(hAxis);
	}

	for (i = 0; i < cd->seriesCount; i++)
	{
		WBCHARTSERIES *s = &cd->series[i];
		COLORREF clr = s->lineColor == NOCOLOR ? wbChartGetSeriesColor(cd, i) : s->lineColor;
		if (s->type == 2)
		{
			for (j = 0; j < s->pointCount; j++)
			{
				int px = wbChartMapX(s->points[j].x, minX, maxX, &plot);
				int py = wbChartMapY(s->points[j].y, minY, maxY, &plot);
				HBRUSH b = CreateSolidBrush(clr);
				HBRUSH ob = SelectObject(hdc, b);
				Ellipse(hdc, px - 3, py - 3, px + 4, py + 4);
				SelectObject(hdc, ob);
				DeleteObject(b);
			}
		}
		else if (s->type == 1)
		{
			int bw = MAX(3, (plot.right - plot.left) / MAX(1, s->pointCount * 2));
			HBRUSH b = CreateSolidBrush(s->fillColor == NOCOLOR ? clr : s->fillColor);
			HBRUSH ob = SelectObject(hdc, b);
			for (j = 0; j < s->pointCount; j++)
			{
				int px = wbChartMapX(s->points[j].x, minX, maxX, &plot);
				int py = wbChartMapY(s->points[j].y, minY, maxY, &plot);
				Rectangle(hdc, px - bw / 2, py, px + bw / 2, plot.bottom);
			}
			SelectObject(hdc, ob);
			DeleteObject(b);
		}
		else
		{
			HPEN p = CreatePen(PS_SOLID, 2, clr);
			HPEN op = SelectObject(hdc, p);
			for (j = 0; j < s->pointCount; j++)
			{
				int px = wbChartMapX(s->points[j].x, minX, maxX, &plot);
				int py = wbChartMapY(s->points[j].y, minY, maxY, &plot);
				if (j == 0)
					MoveToEx(hdc, px, py, NULL);
				else
					LineTo(hdc, px, py);
				Ellipse(hdc, px - 2, py - 2, px + 3, py + 3);
			}
			SelectObject(hdc, op);
			DeleteObject(p);
		}
	}
}

static BOOL wbChartHitTest(WBCHARTDATA *cd, RECT *rc, int mx, int my, int *outSeries, int *outPoint)
{
	double minX, maxX, minY, maxY;
	RECT plot = *rc;
	int i, j;
	double best = 1e20;
	plot.left += cd->padding;
	plot.top += cd->padding + 20;
	plot.right -= cd->padding;
	plot.bottom -= cd->padding;
	wbChartComputeBounds(cd, &minX, &maxX, &minY, &maxY);
	*outSeries = -1;
	*outPoint = -1;
	for (i = 0; i < cd->seriesCount; i++)
	{
		for (j = 0; j < cd->series[i].pointCount; j++)
		{
			int px = wbChartMapX(cd->series[i].points[j].x, minX, maxX, &plot);
			int py = wbChartMapY(cd->series[i].points[j].y, minY, maxY, &plot);
			double d = sqrt((double)((px - mx) * (px - mx) + (py - my) * (py - my)));
			if (d < best)
			{
				best = d;
				*outSeries = i;
				*outPoint = j;
			}
		}
	}
	return (best <= 12.0 && *outSeries >= 0);
}

static void wbChartUpdateTooltip(PWBOBJ pwbo, WBCHARTDATA *cd, int s, int p)
{
	TOOLINFO ti;
	if (!cd->hTooltip)
		return;
	if (s < 0 || p < 0)
	{
		SendMessage(cd->hTooltip, TTM_TRACKACTIVATE, FALSE, 0);
		return;
	}
	wsprintf(cd->tooltipText, TEXT("%s\nx=%g\ny=%g"),
		cd->series[s].points[p].label && *cd->series[s].points[p].label ? cd->series[s].points[p].label : cd->series[s].name,
		cd->series[s].points[p].x,
		cd->series[s].points[p].y);
	ZeroMemory(&ti, sizeof(ti));
	ti.cbSize = sizeof(ti);
	ti.uFlags = TTF_IDISHWND | TTF_TRACK;
	ti.hwnd = pwbo->hwnd;
	ti.uId = (UINT_PTR)pwbo->hwnd;
	ti.lpszText = cd->tooltipText;
	SendMessage(cd->hTooltip, TTM_UPDATETIPTEXT, 0, (LPARAM)&ti);
	SendMessage(cd->hTooltip, TTM_TRACKACTIVATE, TRUE, (LPARAM)&ti);
}

LRESULT CALLBACK ChartControlProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	PWBOBJ pwbo = (PWBOBJ)GetWindowLongPtr(hwnd, GWLP_USERDATA);
	WBCHARTDATA *cd = wbChartGetData(pwbo);
	switch (msg)
	{
	case WM_PAINT:
		if (cd)
		{
			PAINTSTRUCT ps;
			RECT rc;
			HDC hdc = BeginPaint(hwnd, &ps);
			GetClientRect(hwnd, &rc);
			wbChartDraw(cd, hdc, &rc);
			EndPaint(hwnd, &ps);
			return 0;
		}
		break;
	case WM_MOUSEMOVE:
		if (cd && pwbo)
		{
			RECT rc;
			int s, p;
			POINT pt = {LOWORD(lParam), HIWORD(lParam)};
			GetClientRect(hwnd, &rc);
			if (wbChartHitTest(cd, &rc, pt.x, pt.y, &s, &p))
			{
				if (cd->hoverSeries != s || cd->hoverPoint != p)
				{
					cd->hoverSeries = s;
					cd->hoverPoint = p;
					wbChartUpdateTooltip(pwbo, cd, s, p);
					if (pwbo->pszCallBackFn && *pwbo->pszCallBackFn && BITTEST(cd->notifyMask, WBC_CHART_POINT_HOVER))
						wbCallUserFunction(pwbo->pszCallBackFn, pwbo->pszCallBackObj, pwbo->parent ? pwbo->parent : pwbo, pwbo, pwbo->id, WBC_CHART_POINT_HOVER, MAKELPARAM(s, p), 0);
				}
			}
			else
			{
				cd->hoverSeries = -1;
				cd->hoverPoint = -1;
				wbChartUpdateTooltip(pwbo, cd, -1, -1);
			}
		}
		break;
	case WM_LBUTTONDOWN:
		if (cd && pwbo && cd->hoverSeries >= 0 && pwbo->pszCallBackFn && *pwbo->pszCallBackFn && BITTEST(cd->notifyMask, WBC_CHART_POINT_CLICK))
		{
			wbCallUserFunction(pwbo->pszCallBackFn, pwbo->pszCallBackObj, pwbo->parent ? pwbo->parent : pwbo, pwbo, pwbo->id, WBC_CHART_POINT_CLICK, MAKELPARAM(cd->hoverSeries, cd->hoverPoint), 0);
		}
		break;
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

BOOL RegisterChartControlClass(void)
{
	WNDCLASS wc;
	ZeroMemory(&wc, sizeof(wc));
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = (WNDPROC)ChartControlProc;
	wc.hInstance = hAppInstance;
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszClassName = CHART_CONTROL_CLASS;
	wc.hCursor = LoadCursor(NULL, IDC_CROSS);
	wc.cbWndExtra = DLGWINDOWEXTRA;
	if (!RegisterClass(&wc))
		return FALSE;
	return TRUE;
}

BOOL wbChartInit(PWBOBJ pwbo, LONG_PTR notifyMask)
{
	WBCHARTDATA *cd;
	if (!pwbo)
		return FALSE;
	cd = wbMalloc(sizeof(WBCHARTDATA));
	ZeroMemory(cd, sizeof(WBCHARTDATA));
	cd->magic = CHART_MAGIC;
	cd->notifyMask = notifyMask;
	cd->padding = 28;
	cd->bgColor = RGB(255, 255, 255);
	cd->gridColor = RGB(230, 230, 230);
	cd->axisTextColor = RGB(40, 40, 40);
	cd->hoverSeries = -1;
	cd->hoverPoint = -1;
	cd->hTooltip = CreateWindow(TOOLTIPS_CLASS, NULL, WS_POPUP | TTS_ALWAYSTIP,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		pwbo->hwnd, NULL, hAppInstance, NULL);
	if (cd->hTooltip)
	{
		TOOLINFO ti;
		ZeroMemory(&ti, sizeof(ti));
		ti.cbSize = sizeof(ti);
		ti.uFlags = TTF_IDISHWND | TTF_TRACK;
		ti.hwnd = pwbo->hwnd;
		ti.uId = (UINT_PTR)pwbo->hwnd;
		ti.lpszText = cd->tooltipText;
		SendMessage(cd->hTooltip, TTM_ADDTOOL, 0, (LPARAM)&ti);
	}
	pwbo->lparam = (LPARAM)cd;
	return TRUE;
}

BOOL wbChartDestroy(PWBOBJ pwbo)
{
	WBCHARTDATA *cd = wbChartGetData(pwbo);
	if (!cd)
		return FALSE;
	wbChartClearSeries(cd);
	wbChartFreeString(cd->title);
	wbChartFreeString(cd->xAxis);
	wbChartFreeString(cd->yAxis);
	if (cd->palette)
		wbFree(cd->palette);
	if (cd->hTooltip)
		DestroyWindow(cd->hTooltip);
	cd->magic = 0;
	wbFree(cd);
	pwbo->lparam = 0;
	return TRUE;
}

BOOL wbChartSetData(PWBOBJ pwbo, WBCHARTSERIES *series, int count)
{
	WBCHARTDATA *cd = wbChartGetData(pwbo);
	int i, j;
	if (!cd)
		return FALSE;
	wbChartClearSeries(cd);
	if (count <= 0)
	{
		InvalidateRect(pwbo->hwnd, NULL, TRUE);
		return TRUE;
	}

	cd->series = wbCalloc(count, sizeof(WBCHARTSERIES));
	if (!cd->series)
		return FALSE;
	cd->seriesCount = count;

	for (i = 0; i < count; i++)
	{
		cd->series[i].name = wbChartDup(series[i].name);
		cd->series[i].type = series[i].type;
		cd->series[i].lineColor = series[i].lineColor;
		cd->series[i].fillColor = series[i].fillColor;
		cd->series[i].pointColor = series[i].pointColor;
		cd->series[i].pointCount = series[i].pointCount;

		if (series[i].pointCount > 0)
		{
			cd->series[i].points = wbCalloc(series[i].pointCount, sizeof(WBCHARTPOINT));
			if (!cd->series[i].points)
			{
				wbChartClearSeries(cd);
				return FALSE;
			}
			for (j = 0; j < series[i].pointCount; j++)
			{
				cd->series[i].points[j].x = series[i].points[j].x;
				cd->series[i].points[j].y = series[i].points[j].y;
				cd->series[i].points[j].xLabel = wbChartDup(series[i].points[j].xLabel);
				cd->series[i].points[j].label = wbChartDup(series[i].points[j].label);
			}
		}
	}

	InvalidateRect(pwbo->hwnd, NULL, TRUE);
	return TRUE;
}

BOOL wbChartSetOptions(PWBOBJ pwbo, LPCTSTR title, LPCTSTR xAxis, LPCTSTR yAxis, int padding,
	BOOL hasMinX, double minX, BOOL hasMaxX, double maxX, BOOL hasMinY, double minY, BOOL hasMaxY, double maxY)
{
	WBCHARTDATA *cd = wbChartGetData(pwbo);
	if (!cd)
		return FALSE;
	wbChartFreeString(cd->title);
	wbChartFreeString(cd->xAxis);
	wbChartFreeString(cd->yAxis);
	cd->title = wbChartDup(title);
	cd->xAxis = wbChartDup(xAxis);
	cd->yAxis = wbChartDup(yAxis);
	cd->padding = MAX(8, padding);
	cd->hasMinX = hasMinX;
	cd->hasMaxX = hasMaxX;
	cd->hasMinY = hasMinY;
	cd->hasMaxY = hasMaxY;
	cd->minX = minX;
	cd->maxX = maxX;
	cd->minY = minY;
	cd->maxY = maxY;
	InvalidateRect(pwbo->hwnd, NULL, TRUE);
	return TRUE;
}

BOOL wbChartSetColors(PWBOBJ pwbo, COLORREF *palette, int count)
{
	WBCHARTDATA *cd = wbChartGetData(pwbo);
	if (!cd)
		return FALSE;
	if (cd->palette)
	{
		wbFree(cd->palette);
		cd->palette = NULL;
		cd->paletteCount = 0;
	}
	if (count > 0)
	{
		cd->palette = wbCalloc(count, sizeof(COLORREF));
		memcpy(cd->palette, palette, sizeof(COLORREF) * count);
		cd->paletteCount = count;
	}
	InvalidateRect(pwbo->hwnd, NULL, TRUE);
	return TRUE;
}

BOOL wbChartRedraw(PWBOBJ pwbo)
{
	WBCHARTDATA *cd = wbChartGetData(pwbo);
	if (!cd)
		return FALSE;
	InvalidateRect(pwbo->hwnd, NULL, TRUE);
	UpdateWindow(pwbo->hwnd);
	return TRUE;
}
