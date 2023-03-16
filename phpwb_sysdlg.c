/*******************************************************************************

 WINBINDER - The native Windows binding for PHP for PHP

 Copyright  Hypervisual - see LICENSE.TXT for details
 Author: Rubem Pechansky (http://winbinder.org/contact.php)

 ZEND wrapper for common dialog boxes

*******************************************************************************/

//----------------------------------------------------------------- DEPENDENCIES

#include "phpwb.h"
//#include <shlobj.h>
//#include <stdio.h>

//----------------------------------------------------------- EXPORTED FUNCTIONS

ZEND_FUNCTION(wb_sys_dlg_open)
{
	zend_long pwboParent = (LONG_PTR)NULL;
	char *title = "", *filter = "", *path = "";
	size_t title_len = 0, filter_len = 0, path_len = 0;
	int fileCount = 0;
	zend_long style;
	TCHAR szFile[MAX_PATH_BUFFER] = {0};
	TCHAR szDir[MAX_PATH] = {0};

	char *file = 0;
	int file_len = 0;

	TCHAR *szTitle = 0;
	TCHAR *szFilter = 0;
	TCHAR *szPath = 0;
	TCHAR thisOne[MAX_PATH], fullPath[MAX_PATH * 2];
	zend_bool style_isnull;

	// if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l|sssl", &pwboParent, &title, &title_len, &filter, &filter_len, &path, &path_len, &style) == FAILURE)
	ZEND_PARSE_PARAMETERS_START(1, 5)
		Z_PARAM_LONG(pwboParent)
		Z_PARAM_OPTIONAL
		Z_PARAM_STRING_OR_NULL(title, title_len)
		Z_PARAM_STRING_OR_NULL(filter, filter_len)
		Z_PARAM_STRING_OR_NULL(path, path_len)
		Z_PARAM_LONG_OR_NULL(style, style_isnull)
	ZEND_PARSE_PARAMETERS_END();

	if (pwboParent && !wbIsWBObj((void *)pwboParent, TRUE)){
		RETURN_NULL();
	}
	szTitle = Utf82WideChar(title, title_len);
	szFilter = Utf82WideChar(filter, filter_len);
	szPath = Utf82WideChar(path, path_len);

	wbSysDlgOpen((PWBOBJ)pwboParent, szTitle, szFilter, szPath, szFile, style, MAX_PATH_BUFFER);

	if (*szFile)
	{
		if (!BITTEST(style, WBC_MULTISELECT))
		{
			file = WideChar2Utf8(szFile, &file_len);
			RETURN_STRINGL(file, file_len);
		}
		array_init(return_value);

		TCHAR *ptr = szFile;
		wcscpy(szDir, ptr);
		ptr += wcslen(szDir) + 1;
		while (*ptr)
		{
			if (fileCount == 0){
				wcscat(szDir, L"\\");
			}
			wcscpy(fullPath, szDir);
			wcscat(fullPath, ptr);
			ptr += (wcslen(ptr) + 1);
			file = WideChar2Utf8(fullPath, &file_len);
			add_next_index_stringl(return_value, file, file_len);
			fileCount++;
		}
		if (fileCount == 0)
		{
			file = WideChar2Utf8(szDir, &file_len);
			add_next_index_stringl(return_value, file, file_len);
		}
	}
	else
		RETURN_STRING("");
}

ZEND_FUNCTION(wb_sys_dlg_save)
{
	zend_long pwboParent = (LONG_PTR)NULL;
	char *title = "", *filter = "", *path = "", *file = "", *defext = "";
	size_t title_len = 0, filter_len = 0, path_len = 0, file_len = 0, defext_len = 0;
	TCHAR szFile[MAX_PATH] = TEXT("");
	TCHAR *szDefExt = 0;

	TCHAR *szTitle = 0;
	TCHAR *szFilter = 0;
	TCHAR *szPath = 0;

	// if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l|sssss", &pwboParent, &title, &title_len, &filter, &filter_len, &path, &path_len, &file, &file_len, &defext, &defext_len) == FAILURE)
	ZEND_PARSE_PARAMETERS_START(1, 6)
		Z_PARAM_LONG(pwboParent)
		Z_PARAM_OPTIONAL
		Z_PARAM_STRING_OR_NULL(title,title_len)
		Z_PARAM_STRING_OR_NULL(filter,filter_len)
		Z_PARAM_STRING_OR_NULL(path,path_len)
		Z_PARAM_STRING_OR_NULL(file, file_len)
		Z_PARAM_STRING_OR_NULL(defext, defext_len)
	ZEND_PARSE_PARAMETERS_END();

	if (pwboParent && !wbIsWBObj((void *)pwboParent, TRUE)){
		RETURN_NULL();
	}
	if (*file){
		Utf82WideCharCopy((const char *)file, file_len, szFile, strlen(szFile));
	}

	if (*defext){
		//		strcpy(szDefExt, defext);
		szDefExt = Utf82WideChar(defext, defext_len);
	}

	szTitle = Utf82WideChar(title, title_len);
	szFilter = Utf82WideChar(filter, filter_len);
	szPath = Utf82WideChar(path, path_len);

	wbSysDlgSave((PWBOBJ)pwboParent, szTitle, szFilter, szPath, szFile, szDefExt);

	if (*szFile)
	{
		file = WideChar2Utf8((const char *)szFile, strlen(szFile));
		RETURN_STRINGL(file, strlen(file));
	}
	else{
		RETURN_STRING("");
	}
}

ZEND_FUNCTION(wb_sys_dlg_path)
{
	zend_long pwboParent = (LONG_PTR)NULL;
	char *title = "", *path = "";
	size_t title_len = 0, path_len = 0;
	TCHAR szSelPath[MAX_PATH] = TEXT("");

	TCHAR *szTitle = 0;
	TCHAR *szPath = 0;
	char *selPath = 0;
	int sel_len = 0;

	// if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l|ss", &pwboParent, &title, &title_len, &path, &path_len) == FAILURE)
	ZEND_PARSE_PARAMETERS_START(1, 3)
		Z_PARAM_LONG(pwboParent)
		Z_PARAM_OPTIONAL
		Z_PARAM_STRING_OR_NULL(title,title_len)
		Z_PARAM_STRING_OR_NULL(path,path_len)
	ZEND_PARSE_PARAMETERS_END();

	if (pwboParent && !wbIsWBObj((void *)pwboParent, TRUE)){
		RETURN_NULL();
	}
	szTitle = Utf82WideChar(title, title_len);
	szPath = Utf82WideChar(path, path_len);

	wbSysDlgPath((PWBOBJ)pwboParent, szTitle, szPath, szSelPath);

	if (*szSelPath)
	{
		selPath = WideChar2Utf8(szSelPath, &sel_len);
		RETURN_STRINGL(selPath, sel_len);
	}
	else{
		RETURN_STRING("");
	}
}

ZEND_FUNCTION(wb_sys_dlg_color)
{
	zend_long pwboParent = (LONG_PTR)NULL;
	zend_long color = NOCOLOR;
	char *title = "";
	size_t title_len = 0;
	TCHAR *szTitle = 0;
	zend_bool color_isnull;
	
	// if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l|sl", &pwboParent, &title, &title_len, &color) == FAILURE)
	ZEND_PARSE_PARAMETERS_START(1, 3)
		Z_PARAM_LONG(pwboParent)
		Z_PARAM_OPTIONAL
		Z_PARAM_STRING_OR_NULL(title, title_len)
		Z_PARAM_LONG_OR_NULL(color, color_isnull)
	ZEND_PARSE_PARAMETERS_END();

	if (pwboParent && !wbIsWBObj((void *)pwboParent, TRUE)){
		RETURN_NULL();
	}
	szTitle = Utf82WideChar(title, title_len);
	RETURN_LONG(wbSysDlgColor((PWBOBJ)pwboParent, szTitle, (COLORREF)color));
}

ZEND_FUNCTION(wb_sys_dlg_font)
{
	LONG_PTR pwbparent = (LONG_PTR)NULL;
	char *title = "";
	char *name = "";
	__int64 height = 0, color = 0, flags = 0;
	size_t title_len = 0, name_len = 0;
	int font = 0;
	zend_bool pwbparent_isnull, height_isnull, color_isnull, flags_isnull;


	// if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|lsslll", &pwbparent, &title, &title_len, &name, &name_len, &height, &color, &flags) == FAILURE)
	ZEND_PARSE_PARAMETERS_START(0, 6)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG_OR_NULL(pwbparent, pwbparent_isnull)
		Z_PARAM_STRING_OR_NULL(title, title_len)
		Z_PARAM_STRING_OR_NULL(name,name_len)
		Z_PARAM_LONG_OR_NULL(height, height_isnull)
		Z_PARAM_LONG_OR_NULL(color, color_isnull)
		Z_PARAM_LONG_OR_NULL(flags, flags_isnull)
	ZEND_PARSE_PARAMETERS_END();

	if (pwbparent && !wbIsWBObj((void *)pwbparent, TRUE)){
		RETURN_NULL();
	}

	font = wbCreateFont((LPCTSTR)name, height, color, flags);
	RETURN_LONG(
		(LONG_PTR)wbSysDlgFont(
			(PWBOBJ)pwbparent,
			 (LPTSTR)title,
			  0
			  )
		);
}

//------------------------------------------------------------------ END OF FILE
