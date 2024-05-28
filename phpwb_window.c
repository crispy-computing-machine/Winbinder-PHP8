/*******************************************************************************

 WINBINDER - The native Windows binding for PHP for PHP

 Copyright  Hypervisual - see LICENSE.TXT for details
 Author: Rubem Pechansky (https://github.com/crispy-computing-machine/Winbinder)

 ZEND wrapper for window creation and manipulation functions

*******************************************************************************/

//----------------------------------------------------------------- DEPENDENCIES

#include "phpwb.h"
#include <shellapi.h>

//----------------------------------------------------------- EXPORTED FUNCTIONS

ZEND_FUNCTION(wb_create_window)
{
	zend_long pwboparent, pwbo;
	zend_long wbclass, x = WBC_CENTER, y = WBC_CENTER, w = CW_USEDEFAULT, h = CW_USEDEFAULT, style = 0, lparam = 0;
	int nargs;
	zval *zcaption;
	char *caption = "";
	char *tooltip = "";
	TCHAR *wcsCaption = 0;
	TCHAR *wcsTooltip = 0;
	nargs = ZEND_NUM_ARGS();

	zend_long x_len ,y_len ,w_len ,h_len ,style_len ,lparam_len;

	// if (zend_parse_parameters(nargs TSRMLS_CC, "ll|zllllll", &pwboparent, &wbclass, &zcaption, &x, &y, &w, &h, &style, &lparam) == FAILURE)
	ZEND_PARSE_PARAMETERS_START(2, 9)
		Z_PARAM_LONG(pwboparent)
		Z_PARAM_LONG(wbclass)
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL_OR_NULL(zcaption)
		Z_PARAM_LONG_OR_NULL(x, x_len)
		Z_PARAM_LONG_OR_NULL(y, y_len)
		Z_PARAM_LONG_OR_NULL(w, w_len)
		Z_PARAM_LONG_OR_NULL(h, h_len)
		Z_PARAM_LONG_OR_NULL(style, style_len)
		Z_PARAM_LONG_OR_NULL(lparam, lparam_len)
	ZEND_PARSE_PARAMETERS_END();

	if (pwboparent && !wbIsWBObj((void *)pwboparent, TRUE)){
		RETURN_NULL();
	}
	
	if (nargs == 5)
	{
		w = x;
		h = y;
		x = WBC_CENTER;
		y = WBC_CENTER;
	}

	switch (Z_TYPE_P(zcaption))
	{

	case IS_ARRAY:
		parse_array(zcaption, "ss", &caption, &tooltip);
		wcsCaption = Utf82WideChar(caption, strlen(caption));
		wcsTooltip = Utf82WideChar(tooltip, strlen(tooltip));
		break;

	case IS_STRING:
		wcsCaption = Utf82WideChar(Z_STRVAL_P(zcaption), Z_STRLEN_P(zcaption));
		break;

	case IS_NULL:
		*caption = '\0';
		break;
	}

	pwbo = (LONG_PTR)wbCreateWindow((PWBOBJ)pwboparent, wbclass, wcsCaption, wcsTooltip, x, y, w, h, 0, style, lparam);

	if (!pwbo)
	{
		wbError(TEXT("wb_create_window"), MB_ICONWARNING, TEXT("Error creating window"));
		RETURN_NULL();
	}

	RETURN_LONG(pwbo);
}

ZEND_FUNCTION(wb_destroy_window)
{
	zend_long pwbo;

	// if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &pwbo) == FAILURE)
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(pwbo)
	ZEND_PARSE_PARAMETERS_END();

	if (!wbIsWBObj((void *)pwbo, TRUE)){
		RETURN_BOOL(FALSE);
	 }
	RETURN_BOOL(wbDestroyWindow((PWBOBJ)pwbo));
}

ZEND_FUNCTION(wb_get_instance)
{
	char *caption = "";
	size_t caption_len = 0;
	zend_bool bringtofront = FALSE;
	TCHAR *szCaption = 0;
	zend_long bringtofront_len;

	// if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|l", &caption, &caption_len, &bringtofront) == FAILURE)
	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STRING(caption, caption_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL_OR_NULL(bringtofront, bringtofront_len)
	ZEND_PARSE_PARAMETERS_END();

	// This function could return the window handler instead of a BOOL,
	// but it wouldn't be useful. Windows wouldn't let the second PHP process to
	// use this handle directly to, say, change the window caption

	szCaption = Utf82WideChar(caption, caption_len);
	RETURN_BOOL(wbGetRequestedAppWindow(szCaption, bringtofront) != NULL);
}

/* Returns an array with the width and height of a control, window, image or image file,
  or an array with ListView column widths */

ZEND_FUNCTION(wb_get_size)
{
	zval *source;
	DWORD size;
	zend_long lparam = 0;
	zend_long lparam_len;
	PWBOBJ pwbo;

	TCHAR *wcs = 0;

	// if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|l", &source, &lparam) == FAILURE)
	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_ZVAL(source)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG_OR_NULL(lparam, lparam_len)
	ZEND_PARSE_PARAMETERS_END();

	if (!source){
		RETURN_NULL();
	}

	if (Z_TYPE_P(source) == IS_LONG)
	{ // It's an integer: PWBO, HBITMAP or HICON

		if (!source->value.lval){
			RETURN_NULL();
		}
		//Test first is image
		size = wbGetImageDimensions((HBITMAP)source->value.lval);
		if (size == 0) //if not image then get windo or listview size
		{
			pwbo = (PWBOBJ)(void *)source->value.lval;
			if (wbIsWBObj(pwbo, FALSE))
			{
				// lparam here means "column widths"
				if (pwbo->uClass == ListView && lparam)
				{
					int i, cols;
					int pwidths[MAX_LISTVIEWCOLS];

					// Build the array
					cols = wbGetListViewColumnWidths(pwbo, pwidths);
					array_init(return_value);
					for (i = 0; i < cols; i++)
						add_next_index_long(return_value, pwidths[i]);
					return;
				}
				else
				{
					// lparam here means "client area"
					size = wbGetWindowSize(pwbo, lparam);
				}
			}
			else
			{
				RETURN_NULL();
			}
		}
	}
	else if (Z_TYPE_P(source) == IS_STRING)
	{

		// Is source an image file?
		char* const source_str_val = Z_STRVAL_P(source);
		if (strchr(source_str_val, '.') && (strstr(source_str_val, ".bmp") || strstr(source_str_val, ".ico") || strstr(source_str_val, ".icl") ||
											strstr(source_str_val, ".dll") || strstr(source_str_val, ".exe")))
		{

			HBITMAP hbm;

			wcs = Utf82WideChar(source_str_val, Z_STRLEN_P(source));
			hbm = wbLoadImage(wcs, 0, 0);
			if (hbm){
				//size = wbGetImageDimensions(wbLoadImage(source->value.str.val, 0, 0));
				size = wbGetImageDimensions(hbm);
			}
		}
		else
		{

			SIZE siz;
			wcs = Utf82WideChar(source_str_val, Z_STRLEN_P(source));
			if (wbGetTextSize(&siz, wcs, lparam)){
				size = (DWORD)MAKELONG(siz.cx, siz.cy);
				}
			else{
				size = 0;
				}
		}
	}
	else
	{
		wbError(TEXT("wb_get_size"), MB_ICONWARNING, TEXT("Invalid parameter type passed to function"));
		RETURN_NULL();
	}

	// Build the array

	switch (size)
	{
	case (DWORD)MAKELONG(WBC_CENTER, WBC_MINIMIZED):
		RETURN_LONG(WBC_MINIMIZED);
		//		case (DWORD)MAKELONG(WBC_CENTER, WBC_MAXIMIZED):
		//			RETURN_LONG(WBC_MAXIMIZED);
	case (DWORD)MAKELONG(WBC_CENTER, WBC_CENTER):
		RETURN_NULL();
	}

	array_init(return_value);
	add_next_index_long(return_value, LOWORD(size));
	add_next_index_long(return_value, HIWORD(size));
}

ZEND_FUNCTION(wb_set_size)
{
	zend_long pwbo;
	zend_long h = 65535;
	int nargs;
	zval *zparm = NULL;
	zend_long h_len;
	nargs = ZEND_NUM_ARGS();

	// if (zend_parse_parameters(nargs TSRMLS_CC, "lz|l", &pwbo, &zparm, &h) == FAILURE)
	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_LONG(pwbo)
		Z_PARAM_ZVAL(zparm)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG_OR_NULL(h, h_len)
	ZEND_PARSE_PARAMETERS_END();

	if (!wbIsWBObj((void *)pwbo, TRUE)){
		RETURN_BOOL(FALSE);
	 }
	if ((Z_TYPE_P(zparm) == IS_ARRAY) && ((PWBOBJ)pwbo)->uClass == ListView)
	{

		int i, nelem;
		int pwidths[MAX_LISTVIEWCOLS];
		HashTable *target_hash;
		zval *entry = NULL;

		target_hash = HASH_OF(zparm);
		if (!target_hash){
			RETURN_BOOL(FALSE);
		}
		nelem = zend_hash_num_elements(target_hash);
		zend_hash_internal_pointer_reset(target_hash);

		// Loop to read zparm items

		for (i = 0; i < nelem; i++)
		{
			if ((entry = zend_hash_get_current_data(target_hash)) == NULL)
			{
				wbError(TEXT("wb_set_size"), MB_ICONWARNING, TEXT("Could not retrieve element %d from zparm"), i);
				RETURN_BOOL(FALSE);
			}
			switch (Z_TYPE_P(entry))
			{

			case IS_LONG:
				pwidths[i] = Z_LVAL_P(entry);
				break;

			case IS_STRING:
			case IS_DOUBLE:
				convert_to_long_ex(entry);
				pwidths[i] = Z_LVAL_P(entry);
				break;

			case IS_NULL:
				convert_to_long_ex(entry);
				pwidths[i] = 65535;
				break;

			default:
				wbError(TEXT("wb_set_size"), MB_ICONWARNING, TEXT("Wrong data type in array in function"));
				RETURN_BOOL(FALSE);
			}

			zend_hash_move_forward(target_hash);
		}

		RETURN_BOOL(wbSetListViewColumnWidths((PWBOBJ)pwbo, pwidths));
	}
	else
	{

		if (Z_TYPE_P(zparm) != IS_LONG)
		{
			wbError(TEXT("wb_set_size"), MB_ICONWARNING, TEXT("Wrong data type in width in function"));
			RETURN_BOOL(FALSE);
		}

		if (h != 65535){
			RETURN_BOOL(wbSetWindowSize((PWBOBJ)pwbo, zparm->value.lval, h, -1));
			}
		else{
			RETURN_BOOL(wbSetWindowSize((PWBOBJ)pwbo, 0, 0, zparm->value.lval));
		}
	}
}

/* Returns an array with the x and y positions */

ZEND_FUNCTION(wb_get_position)
{
	zend_long pwbo;
	DWORD pos;
	zend_bool clientarea = FALSE;
	zend_long clientarea_len;
	
	// if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l|l", &pwbo, &clientarea) == FAILURE)
	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_LONG(pwbo)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL_OR_NULL(clientarea, clientarea_len)
	ZEND_PARSE_PARAMETERS_END();

	if (!wbIsWBObj((void *)pwbo, TRUE)){
		RETURN_NULL();
	 }

	// Build the array x, y
	pos = wbGetWindowPosition((PWBOBJ)pwbo, NULL, clientarea);
	array_init(return_value);
	add_next_index_long(return_value, LOWORD(pos));
	add_next_index_long(return_value, HIWORD(pos));
}

ZEND_FUNCTION(wb_set_position)
{
	zend_long pwbo, x, y;

	// Default parameter values
	zend_long x_len, y_len;

	x = WBC_CENTER;
	y = WBC_CENTER;

	// if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l|ll", &pwbo, &x, &y) == FAILURE)
	ZEND_PARSE_PARAMETERS_START(1, 3)
		Z_PARAM_LONG(pwbo)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG_OR_NULL(x, x_len)
		Z_PARAM_LONG_OR_NULL(y, y_len)
	ZEND_PARSE_PARAMETERS_END();

	if (!wbIsWBObj((void *)pwbo, TRUE)){
		RETURN_BOOL(FALSE);
	 }
	RETURN_BOOL(wbSetWindowPosition((PWBOBJ)pwbo, x, y, NULL));
}

ZEND_FUNCTION(wb_set_area)
{
	int nargs;
	zend_long pwbo, type, x, y, w, h;

	// Default parameter values

	x = -1;
	y = -1;
	w = 0;
	h = 0;
	zend_long x_len, y_len, w_len, h_len;
	nargs = ZEND_NUM_ARGS();

	// if (zend_parse_parameters(nargs TSRMLS_CC, "ll|llll", &pwbo, &type, &x, &y, &w, &h) == FAILURE)
	ZEND_PARSE_PARAMETERS_START(2, 6)
		Z_PARAM_LONG(pwbo)
		Z_PARAM_LONG(type)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG_OR_NULL(x, x_len)
		Z_PARAM_LONG_OR_NULL(y, y_len)
		Z_PARAM_LONG_OR_NULL(w, w_len)
		Z_PARAM_LONG_OR_NULL(h, h_len)
	ZEND_PARSE_PARAMETERS_END();

	// x, y, w, h must be supplied together
	switch (nargs)
	{
	case 3:
	case 4:
	case 5:
		wbError(TEXT("wb_set_area"), MB_ICONWARNING, TEXT("Invalid number of parameters passed to function"));
		RETURN_NULL();
	}

	if (!wbIsWBObj((void *)pwbo, TRUE)){
		RETURN_NULL();
	 }
	RETURN_BOOL(wbSetWindowArea((PWBOBJ)pwbo, type, x, y, w, h));
}

ZEND_FUNCTION(wb_set_handler)
{
	zend_long pwbo;
	zval *obj = NULL;
	zend_string *fname = NULL;
	zval name = {0}, *zparam = NULL;
	char *handler = "";

	TCHAR *wcsHandler = 0;

	// if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lz", &pwbo, &zparam) == FAILURE)
	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_LONG(pwbo)
		Z_PARAM_ZVAL(zparam)
	ZEND_PARSE_PARAMETERS_END();

	if (!wbIsWBObj((void *)pwbo, TRUE)){
		RETURN_BOOL(FALSE);
	}
	switch (Z_TYPE_P(zparam))
	{
	case IS_ARRAY:
		parse_array(zparam, "ls", &obj, &handler);
		break;
	case IS_STRING:
		handler = Z_STRVAL_P(zparam);
		break;
	default:
		wbError(TEXT("wb_set_handler"), MB_ICONWARNING, TEXT("Wrong data type in function"));
		RETURN_BOOL(FALSE);
	}

	// Error checking
	if (!zend_is_callable(zparam, 0, &fname))
	{
		wbError(TEXT("wb_set_handler"), MB_ICONWARNING, TEXT("%s handler is not a function or cannot be called"), fname);
		RETURN_BOOL(FALSE);
	}
	else
	{

		wcsHandler = Utf82WideChar(handler, 0);
		RETURN_BOOL(wbSetWindowHandler((PWBOBJ)pwbo, (LPDWORD)obj, wcsHandler));
	}
}

ZEND_FUNCTION(wb_get_item_list)
{
	zend_long pwboparent;
	PWBOBJ *plist;
	int nctrls, i;

	// if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &pwboparent) == FAILURE)
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(pwboparent)
	ZEND_PARSE_PARAMETERS_END();

	if (!wbIsWBObj((void *)pwboparent, TRUE)){
		RETURN_NULL();
	}
	// Build the array
	array_init(return_value);

	nctrls = wbGetControlList((PWBOBJ)pwboparent, NULL, 32767);
	if (nctrls)
	{
		plist = emalloc(nctrls * sizeof(PWBOBJ));
		wbGetControlList((PWBOBJ)pwboparent, plist, nctrls);
		for (i = 0; i < nctrls; i++)
			add_next_index_long(return_value, (LONG_PTR)plist[i]);
		efree(plist);
	}
}


ZEND_FUNCTION(wb_set_window_accept_drop)
{
	zend_long pwbo;
	zend_bool accept = TRUE;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_LONG(pwbo)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL(accept)
	ZEND_PARSE_PARAMETERS_END();
	
	if(!wbIsWBObj((void *)pwbo, TRUE)) RETURN_BOOL(FALSE);

	if(accept) {
		ChangeWindowMessageFilterEx(((PWBOBJ)pwbo)->hwnd, WM_DROPFILES, MSGFLT_ALLOW, NULL);
		ChangeWindowMessageFilterEx(((PWBOBJ)pwbo)->hwnd, WM_COPYDATA, MSGFLT_ALLOW, NULL);
		ChangeWindowMessageFilterEx(((PWBOBJ)pwbo)->hwnd, 0x0049, MSGFLT_ALLOW, NULL);
	}

	DragAcceptFiles(((PWBOBJ)pwbo)->hwnd, accept);
	RETURN_BOOL(TRUE);
}


ZEND_FUNCTION(wb_get_drop_files)
{
	zend_long wparam;
	zend_bool isShort;

	HDROP *pHDrop;
	char buffer[2048];
	char shortpath[2048];
	long shortpath_size;

	int fcount, i;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_LONG(wparam)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL(isShort)
	ZEND_PARSE_PARAMETERS_END();

	pHDrop = (HDROP *)wparam;
	fcount = DragQueryFileA(pHDrop, 0xFFFFFFFF, buffer, 2048);	
	if(!fcount) RETURN_NULL();

	array_init(return_value);
	for(i = 0; i < fcount; i++)
	{
		DragQueryFileA(pHDrop, i, buffer, 2048);
		if(isShort) {
			shortpath_size = GetShortPathNameA(buffer, shortpath, 2048);
			if(shortpath_size == 0) {
				add_next_index_string(return_value, buffer);
			} else {
				add_next_index_string(return_value, shortpath);
			}
		} else {
			add_next_index_string(return_value, buffer);
		}
	}
}


ZEND_FUNCTION(wb_bring_to_front)
{
	zend_long pwbo;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(pwbo)
	ZEND_PARSE_PARAMETERS_END();

	if(!IsWindowVisible(((PWBOBJ)pwbo)->hwnd)) RETURN_BOOL(FALSE);
	if(IsIconic(((PWBOBJ)pwbo)->hwnd) && !ShowWindow(((PWBOBJ)pwbo)->hwnd, SW_RESTORE)) RETURN_BOOL(FALSE);
	if(!SetWindowPos(((PWBOBJ)pwbo)->hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW)) RETURN_BOOL(FALSE);
	if(!SetWindowPos(((PWBOBJ)pwbo)->hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW)) RETURN_BOOL(FALSE);
	RETURN_BOOL(BringWindowToTop(((PWBOBJ)pwbo)->hwnd));
}


ZEND_FUNCTION(wb_get_window_buffer)
{
	long pwbo;
	
	ZEND_PARSE_PARAMETERS_NONE();

	if(!wbIsWBObj((void *)pwbo, TRUE)) RETURN_BOOL(FALSE);

	RETURN_LONG(((PWBOBJ)pwbo)->pbuffer);
}


ZEND_FUNCTION(wb_get_window_handle)
{
	long pwbo;
	
	ZEND_PARSE_PARAMETERS_NONE();

	if(!wbIsWBObj((void *)pwbo, TRUE)) RETURN_BOOL(FALSE);

	RETURN_LONG(((PWBOBJ)pwbo)->hwnd);
}