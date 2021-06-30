/*******************************************************************************

 WINBINDER - The native Windows binding for PHP for PHP

 Copyright  Hypervisual - see LICENSE.TXT for details
 Author: Rubem Pechansky (http://winbinder.org/contact.php)

 ZEND wrapper for drawing functions

*******************************************************************************/

//----------------------------------------------------------------- DEPENDENCIES

#include "phpwb.h"

//----------------------------------------------------------- EXPORTED FUNCTIONS

/*
source can be a WinBinder object, a bitmap handle, a window handle or a DC.
Returns NOCOLOR (0xFFFFFFFF) if error
*/

ZEND_FUNCTION(wb_get_pixel)
{
	if (ZEND_NUM_ARGS() == 3)
	{
		zend_long handle, x, y;

		//if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lll", &handle, &x, &y) == FAILURE)
		// ZEND_PARSE_PARAMETERS_START() takes two arguments minimal and maximal parameters count.
		ZEND_PARSE_PARAMETERS_START(3, 3)
			Z_PARAM_LONG(handle)
			Z_PARAM_LONG(x)
			Z_PARAM_LONG(y)
		ZEND_PARSE_PARAMETERS_END();


		RETURN_LONG(wbGetPixel((HANDLE)handle, x, y));
	}
	else
	{

		// With four parameters: call wbGetPixelDirect() for faster performance (not stable)

		zend_long x, y;
		unsigned char *pixdata;
		int pixdata_len;
		BOOL compress4to3 = FALSE;

		// if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "slll", &pixdata, &pixdata_len, &x, &y, &compress4to3) == FAILURE)
		// ZEND_PARSE_PARAMETERS_START() takes two arguments minimal and maximal parameters count.
		ZEND_PARSE_PARAMETERS_START(4, 4)
			Z_PARAM_STR(pixdata)
			Z_PARAM_LONG(x)
			Z_PARAM_LONG(y)
			Z_PARAM_LONG(compress4to3)
		ZEND_PARSE_PARAMETERS_END(compress4to3);

		RETURN_LONG(wbGetPixelDirect(pixdata, x, y, compress4to3));
	}
}

/*
source can be a WinBinder object, a bitmap handle, a window handle or a DC.
*/

ZEND_FUNCTION(wb_draw_point)
{
	zend_long handle, x, y, color;

	// if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "llll", &handle, &x, &y, &color) == FAILURE)
	// ZEND_PARSE_PARAMETERS_START() takes two arguments minimal and maximal parameters count.
	ZEND_PARSE_PARAMETERS_START(4, 4)
		Z_PARAM_LONG(handle)
		Z_PARAM_LONG(x)
		Z_PARAM_LONG(y)
		Z_PARAM_LONG(color)
	ZEND_PARSE_PARAMETERS_END();

	RETURN_BOOL(wbSetPixel((HANDLE)handle, x, y, color));
}

ZEND_FUNCTION(wb_draw_line)
{
	zend_long handle, x0, y0, x1, y1, color, linewidth = 0, linestyle = 0;

	// if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "llllll|ll", &handle, &x0, &y0, &x1, &y1, &color, &linewidth, &linestyle) == FAILURE)
	// ZEND_PARSE_PARAMETERS_START() takes two arguments minimal and maximal parameters count.
	ZEND_PARSE_PARAMETERS_START(6, 8)
		Z_PARAM_LONG(handle)
		Z_PARAM_LONG(x0)
		Z_PARAM_LONG(y0)
		Z_PARAM_LONG(x1)
		Z_PARAM_LONG(y1)
		Z_PARAM_LONG(color)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(linewidth)
		Z_PARAM_LONG(linestyle)
	ZEND_PARSE_PARAMETERS_END();

	RETURN_BOOL(wbDrawLine((HANDLE)handle, x0, y0, x1, y1, color, linewidth, linestyle));
}

ZEND_FUNCTION(wb_draw_rect)
{
	zend_long handle, x, y, width, height, color, filled = TRUE, linewidth = 0, linestyle = 0;

	// if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "llllll|lll", &handle, &x, &y, &width, &height, &color, &filled, &linewidth, &linestyle) == FAILURE)
	// ZEND_PARSE_PARAMETERS_START() takes two arguments minimal and maximal parameters count.
	ZEND_PARSE_PARAMETERS_START(6, 9)
		Z_PARAM_LONG(handle)
		Z_PARAM_LONG(x)
		Z_PARAM_LONG(y)
		Z_PARAM_LONG(width)
		Z_PARAM_LONG(height)
		Z_PARAM_LONG(color)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(filled)
		Z_PARAM_LONG(linewidth)
		Z_PARAM_LONG(linestyle)
	ZEND_PARSE_PARAMETERS_END();

	RETURN_BOOL(wbDrawRect((HANDLE)handle, x, y, width, height, color, filled, linewidth, linestyle));
}

ZEND_FUNCTION(wb_draw_ellipse)
{
	zend_long handle, x, y, width, height, color, filled = TRUE, linewidth = 0, linestyle = 0;

	// if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "llllll|lll", &handle, &x, &y, &width, &height, &color, &filled, &linewidth, &linestyle) == FAILURE)
	// ZEND_PARSE_PARAMETERS_START() takes two arguments minimal and maximal parameters count.
	ZEND_PARSE_PARAMETERS_START(6, 9)
		Z_PARAM_LONG(handle)
		Z_PARAM_LONG(x)
		Z_PARAM_LONG(y)
		Z_PARAM_LONG(width)
		Z_PARAM_LONG(height)
		Z_PARAM_LONG(color)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(filled)
		Z_PARAM_LONG(linewidth)
		Z_PARAM_LONG(linestyle)
	ZEND_PARSE_PARAMETERS_END();

	RETURN_BOOL(wbDrawEllipse((HANDLE)handle, x, y, width, height, color, filled, linewidth, linestyle));
}

ZEND_FUNCTION(wb_draw_text)
{
	zend_long handle, x, y, width = 0, height = 0, nfont = -1, flags = 0;
	char *text;
	int text_len;
	int nargs;

	TCHAR *wcs = 0;

	nargs = ZEND_NUM_ARGS();
	// if (zend_parse_parameters(nargs TSRMLS_CC, "lsll|llll", &handle, &text, &text_len, &x, &y, &width, &height, &nfont, &flags) == FAILURE)
	// ZEND_PARSE_PARAMETERS_START() takes two arguments minimal and maximal parameters count.
	ZEND_PARSE_PARAMETERS_START(4, 8)
		Z_PARAM_LONG(handle)
		Z_PARAM_STR(text)
		Z_PARAM_LONG(x)
		Z_PARAM_LONG(y)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(width)
		Z_PARAM_LONG(height)
		Z_PARAM_LONG(nfont)
		Z_PARAM_LONG(flags)
	ZEND_PARSE_PARAMETERS_END();

	switch (nargs)
	{

	case 4:
		nfont = -1;
		break;

	case 5:
		nfont = width;
		break;
	}

	// UGLY, DIRTY HACK that is necessary to change the unnatural default parameters of wbDrawText.
	// Must change the internal defaults in font functions from the API layer.

	nfont = (nfont == 0 ? -1 : (nfont < 0 ? 0 : nfont));

	wcs = Utf82WideChar(text, text_len);
	RETURN_BOOL(wbDrawText((HANDLE)handle, wcs, x, y, width, height, nfont, flags));
}

ZEND_FUNCTION(wb_draw_image)
{
	zend_long handle, hbm, x = 0, y = 0, w = 0, h = 0, cx = 0, cy = 0;
	COLORREF transpcolor = NOCOLOR;

	// if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll|lllllll", &handle, &hbm, &x, &y, &w, &h, &transpcolor, &cx, &cy) == FAILURE)
	ZEND_PARSE_PARAMETERS_START(2, 9)
		Z_PARAM_LONG(handle)
		Z_PARAM_LONG(hbm)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(x)
		Z_PARAM_LONG(y)
		Z_PARAM_LONG(w)
		Z_PARAM_LONG(h)
		Z_PARAM_LONG(transpcolor)
		Z_PARAM_LONG(cx)
		Z_PARAM_LONG(cy)
	ZEND_PARSE_PARAMETERS_END();
	RETURN_BOOL((LONG)wbDrawBitmap((HANDLE)handle, (HBITMAP)hbm, x, y, w, h, cx, cy, transpcolor));
}

//------------------------------------------------------------------ END OF FILE
