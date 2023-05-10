/*******************************************************************************

 WINBINDER - The native Windows binding for PHP for PHP

 Copyright  Hypervisual - see LICENSE.TXT for details
 Author: Rubem Pechansky (https://github.com/crispy-computing-machine/Winbinder)

 ZEND wrapper for font functions

*******************************************************************************/

//----------------------------------------------------------------- DEPENDENCIES

#include "phpwb.h"

//----------------------------------------------------------- EXPORTED FUNCTIONS

// Creates a font and stores it in the font cache

ZEND_FUNCTION(wb_create_font)
{
	zend_long height = 10, color = 0x000000, flags = 0;
	char *name;
	size_t name_len;
	TCHAR *wcs = 0;
	zend_bool color_isnull;
	zend_bool flags_isnull;

	// if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sl|ll", &name, &name_len, &height, &color, &flags) == FAILURE)
	// ZEND_PARSE_PARAMETERS_START() takes two arguments minimal and maximal parameters count.
	ZEND_PARSE_PARAMETERS_START(2, 4)
		Z_PARAM_STRING(name,name_len)
		Z_PARAM_LONG(height)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG_OR_NULL(color, color_isnull)
		Z_PARAM_LONG_OR_NULL(flags, flags_isnull)
	ZEND_PARSE_PARAMETERS_END();

	wcs = Utf82WideChar(name, name_len);
	RETURN_LONG(wbCreateFont(wcs, height, color, flags));
}

// Destroys a font or all created fonts

ZEND_FUNCTION(wb_destroy_font)
{
	zend_long nfont = 0;
	zend_bool nfont_isnull = 0;

	// if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|l", &nfont) == FAILURE)
	// ZEND_PARSE_PARAMETERS_START() takes two arguments minimal and maximal parameters count.
	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG_OR_NULL(nfont, nfont_isnull)
	ZEND_PARSE_PARAMETERS_END();

	RETURN_BOOL(wbDestroyFont(nfont));
}

// Applies a font to a control

ZEND_FUNCTION(wb_set_font)
{
	zend_long pwbo, nfont = 0, redraw = 1;
	zend_bool nfont_isnull, redraw_isnull;

	// if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l|ll", &pwbo, &nfont, &redraw) == FAILURE)
	// ZEND_PARSE_PARAMETERS_START() takes two arguments minimal and maximal parameters count.
	ZEND_PARSE_PARAMETERS_START(1, 3)
		Z_PARAM_LONG(pwbo)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG_OR_NULL(nfont, nfont_isnull)
		Z_PARAM_LONG_OR_NULL(redraw, redraw_isnull)
	ZEND_PARSE_PARAMETERS_END();

	RETURN_BOOL(wbSetControlFont((PWBOBJ)pwbo, nfont, redraw));
}

//------------------------------------------------------------------ END OF FILE
