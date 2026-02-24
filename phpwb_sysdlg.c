/*******************************************************************************

 WINBINDER - The native Windows binding for PHP for PHP

 Copyright  Hypervisual - see LICENSE.TXT for details
 Author: Rubem Pechansky (https://github.com/crispy-computing-machine/Winbinder)

 ZEND wrapper for common dialog boxes

*******************************************************************************/

//----------------------------------------------------------------- DEPENDENCIES

#include "phpwb.h"
#include <ctype.h>
#include <stdio.h>
//#include <shlobj.h>
//#include <stdio.h>

typedef struct _phpwb_filter_buffer {
	char *data;
	size_t len;
	size_t cap;
} phpwb_filter_buffer;

static int phpwb_filterbuf_grow(phpwb_filter_buffer *buf, size_t extra)
{
	size_t needed = buf->len + extra;
	char *new_data;
	size_t new_cap;

	if (needed <= buf->cap){
		return 1;
	}

	new_cap = buf->cap ? buf->cap : 128;
	while (new_cap < needed) {
		new_cap *= 2;
	}

	new_data = buf->data ? wbRealloc(buf->data, new_cap) : wbMalloc(new_cap);
	if (!new_data){
		return 0;
	}
	buf->data = new_data;
	buf->cap = new_cap;
	return 1;
}

static int phpwb_filterbuf_append_bytes(phpwb_filter_buffer *buf, const char *src, size_t src_len)
{
	if (!phpwb_filterbuf_grow(buf, src_len)){
		return 0;
	}
	memcpy(buf->data + buf->len, src, src_len);
	buf->len += src_len;
	return 1;
}

static int phpwb_filterbuf_append_segment(phpwb_filter_buffer *buf, const char *src)
{
	size_t src_len = src ? strlen(src) : 0;
	if (!phpwb_filterbuf_append_bytes(buf, src ? src : "", src_len)){
		return 0;
	}
	return phpwb_filterbuf_append_bytes(buf, "\0", 1);
}

static TCHAR *phpwb_utf8_to_wide_buffer(const char *src, size_t src_len, BOOL ensure_double_null)
{
	int wlen;
	TCHAR *dst;

	if (!src){
		return NULL;
	}
	wlen = MultiByteToWideChar(CP_UTF8, 0, src, (int)src_len, NULL, 0);
	if (wlen <= 0){
		return NULL;
	}
	dst = wbMalloc(sizeof(TCHAR) * (wlen + (ensure_double_null ? 2 : 1)));
	if (!dst){
		return NULL;
	}
	MultiByteToWideChar(CP_UTF8, 0, src, (int)src_len, dst, wlen);
	dst[wlen] = L'\0';
	if (ensure_double_null){
		dst[wlen + 1] = L'\0';
	}
	return dst;
}

static char *phpwb_normalize_pattern_list(const char *pattern)
{
	size_t in_len, pos = 0;
	phpwb_filter_buffer out = {0};

	if (!pattern || !*pattern){
		return wbStrDup("*.*");
	}

	in_len = strlen(pattern);
	while (pos < in_len)
	{
		size_t start, end, tok_len;
		size_t i;
		int has_wildcard;

		while (pos < in_len && (pattern[pos] == ';' || isspace((unsigned char)pattern[pos]))) {
			pos++;
		}
		if (pos >= in_len){
			break;
		}
		start = pos;
		while (pos < in_len && pattern[pos] != ';') {
			pos++;
		}
		end = pos;
		while (end > start && isspace((unsigned char)pattern[end - 1])) {
			end--;
		}
		while (start < end && isspace((unsigned char)pattern[start])) {
			start++;
		}
		if (start >= end){
			continue;
		}

		tok_len = end - start;
		has_wildcard = 0;
		for (i = 0; i < tok_len; i++) {
			if (pattern[start + i] == '*' || pattern[start + i] == '?') {
				has_wildcard = 1;
				break;
			}
		}

		if (out.len > 0 && !phpwb_filterbuf_append_bytes(&out, ";", 1)){
			goto fail;
		}

		if (has_wildcard)
		{
			if (!phpwb_filterbuf_append_bytes(&out, pattern + start, tok_len)){
				goto fail;
			}
		}
		else
		{
			if (!phpwb_filterbuf_append_bytes(&out, "*.", 2)){
				goto fail;
			}
			if (pattern[start] == '.') {
				start++;
				tok_len--;
			}
			if (tok_len == 0 || !phpwb_filterbuf_append_bytes(&out, pattern + start, tok_len)){
				goto fail;
			}
		}
	}

	if (out.len == 0){
		wbFree(out.data);
		return wbStrDup("*.*");
	}
	if (!phpwb_filterbuf_append_bytes(&out, "\0", 1)){
		goto fail;
	}
	return out.data;

fail:
	wbFree(out.data);
	return NULL;
}

static TCHAR *phpwb_build_filter_wide(zval *filter_zv)
{
	phpwb_filter_buffer utf8 = {0};
	BOOL has_entries = FALSE;
	zval *entry;
	TCHAR *wfilter;

	if (!filter_zv || Z_TYPE_P(filter_zv) == IS_NULL){
		return NULL;
	}
	if (Z_TYPE_P(filter_zv) == IS_STRING){
		return phpwb_utf8_to_wide_buffer(Z_STRVAL_P(filter_zv), Z_STRLEN_P(filter_zv), TRUE);
	}
	if (Z_TYPE_P(filter_zv) != IS_ARRAY){
		return NULL;
	}

	ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(filter_zv), entry)
	{
		zval *desc, *pattern;
		char *normalized = NULL;
		char desc_buf[256];

		if (Z_TYPE_P(entry) == IS_STRING)
		{
			normalized = phpwb_normalize_pattern_list(Z_STRVAL_P(entry));
			if (!normalized){
				goto fail;
			}
			snprintf(desc_buf, sizeof(desc_buf), "%s files (%s)", Z_STRVAL_P(entry), normalized);
			if (!phpwb_filterbuf_append_segment(&utf8, desc_buf) || !phpwb_filterbuf_append_segment(&utf8, normalized)){
				wbFree(normalized);
				goto fail;
			}
			wbFree(normalized);
			has_entries = TRUE;
			continue;
		}

		if (Z_TYPE_P(entry) != IS_ARRAY){
			continue;
		}

		desc = zend_hash_index_find(Z_ARRVAL_P(entry), 0);
		pattern = zend_hash_index_find(Z_ARRVAL_P(entry), 1);
		if (!desc || !pattern || Z_TYPE_P(desc) != IS_STRING || Z_TYPE_P(pattern) != IS_STRING){
			continue;
		}

		normalized = phpwb_normalize_pattern_list(Z_STRVAL_P(pattern));
		if (!normalized){
			goto fail;
		}
		if (!phpwb_filterbuf_append_segment(&utf8, Z_STRVAL_P(desc)) || !phpwb_filterbuf_append_segment(&utf8, normalized)){
			wbFree(normalized);
			goto fail;
		}
		wbFree(normalized);
		has_entries = TRUE;
	}
	ZEND_HASH_FOREACH_END();

	if (!has_entries){
		wbFree(utf8.data);
		return NULL;
	}
	if (!phpwb_filterbuf_append_bytes(&utf8, "\0", 1)){
		goto fail;
	}

	wfilter = phpwb_utf8_to_wide_buffer(utf8.data, utf8.len, TRUE);
	if (!wfilter){
		goto fail;
	}
	wbFree(utf8.data);
	return wfilter;

fail:
	wbFree(utf8.data);
	return NULL;
}

//----------------------------------------------------------- EXPORTED FUNCTIONS

ZEND_FUNCTION(wb_sys_dlg_open)
{
	zend_long pwboParent = (LONG_PTR)NULL;
	char *title = "", *path = "";
	size_t title_len = 0, path_len = 0;
	int fileCount = 0;
	zend_long style = 0;
	TCHAR szFile[MAX_PATH_BUFFER] = {0};
	TCHAR szDir[MAX_PATH] = {0};

	char *file = 0;
	int file_len = 0;

	TCHAR *szTitle = 0;
	TCHAR *szFilter = 0;
	TCHAR *szPath = 0;
	TCHAR fullPath[MAX_PATH * 2];
	zval *zfilter = NULL;
	zval *zstyle = NULL;

	// if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l|sssl", &pwboParent, &title, &title_len, &filter, &filter_len, &path, &path_len, &style) == FAILURE)
	ZEND_PARSE_PARAMETERS_START(1, 5)
		Z_PARAM_LONG(pwboParent)
		Z_PARAM_OPTIONAL
		Z_PARAM_STRING_OR_NULL(title, title_len)
		Z_PARAM_ZVAL(zfilter)
		Z_PARAM_STRING_OR_NULL(path, path_len)
		Z_PARAM_ZVAL(zstyle)
	ZEND_PARSE_PARAMETERS_END();

	if (pwboParent && !wbIsWBObj((void *)pwboParent, TRUE)){
		RETURN_NULL();
	}
	if (zstyle && Z_TYPE_P(zstyle) != IS_NULL){
		style = zval_get_long(zstyle);
	}
	szTitle = Utf82WideChar(title, title_len);
	szFilter = phpwb_build_filter_wide(zfilter);
	szPath = Utf82WideChar(path, path_len);

	wbSysDlgOpen((PWBOBJ)pwboParent, szTitle, szFilter, szPath, szFile, style, MAX_PATH_BUFFER);

	if (*szFile)
	{
		if (!BITTEST(style, WBC_MULTISELECT))
		{
			file = WideChar2Utf8(szFile, &file_len);
			RETVAL_STRINGL(file, file_len);
			goto cleanup;
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
			wbFree(file);
			file = NULL;
			fileCount++;
		}
		if (fileCount == 0)
		{
			file = WideChar2Utf8(szDir, &file_len);
			add_next_index_stringl(return_value, file, file_len);
			wbFree(file);
			file = NULL;
		}
	}
	else
		RETVAL_STRING("");

cleanup:
	wbFree(szTitle);
	wbFree(szFilter);
	wbFree(szPath);
	wbFree(file);
}

ZEND_FUNCTION(wb_sys_dlg_save)
{
	zend_long pwboParent = (LONG_PTR)NULL;
	const char *title = "", *path = "", *file = "", *defext = "";
	size_t title_len = 0, path_len = 0, file_len = 0, defext_len = 0;
	TCHAR szFile[MAX_PATH] = TEXT("");
	TCHAR *szDefExt = 0;

	TCHAR *szTitle = 0;
	TCHAR *szFilter = 0;
	TCHAR *szPath = 0;
	zval *zfilter = NULL;

	// if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l|sssss", &pwboParent, &title, &title_len, &filter, &filter_len, &path, &path_len, &file, &file_len, &defext, &defext_len) == FAILURE)
	ZEND_PARSE_PARAMETERS_START(1, 6)
		Z_PARAM_LONG(pwboParent)
		Z_PARAM_OPTIONAL
		Z_PARAM_STRING_OR_NULL(title,title_len)
		Z_PARAM_ZVAL(zfilter)
		Z_PARAM_STRING_OR_NULL(path,path_len)
		Z_PARAM_STRING_OR_NULL(file, file_len)
		Z_PARAM_STRING_OR_NULL(defext, defext_len)
	ZEND_PARSE_PARAMETERS_END();

	if (pwboParent && !wbIsWBObj((void *)pwboParent, TRUE)){
		RETURN_NULL();
	}
	if (file && *file){
		Utf82WideCharCopy((const char *)file, file_len, szFile, MAX_PATH);
		szFile[MAX_PATH - 1] = L'\0';
	}

	if (defext && *defext){
		//		strcpy(szDefExt, defext);
		szDefExt = Utf82WideChar(defext, defext_len);
	}

	szTitle = Utf82WideChar(title, title_len);
	szFilter = phpwb_build_filter_wide(zfilter);
	szPath = Utf82WideChar(path, path_len);

	wbSysDlgSave((PWBOBJ)pwboParent, szTitle, szFilter, szPath, szFile, MAX_PATH, szDefExt);

	if (*szFile)
	{
		int out_len = 0;
		file = WideChar2Utf8(szFile, &out_len);
		RETVAL_STRINGL(file, out_len);
		wbFree((void *)file);
	}
	else{
		RETVAL_STRING("");
	}

	wbFree(szDefExt);
	wbFree(szTitle);
	wbFree(szFilter);
	wbFree(szPath);
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
	size_t title_len = 0, name_len = 0;
	zend_long height = 12, color = 0, flags = 0;
	int font = 0;
	zend_bool height_isnull, color_isnull, flags_isnull;
	TCHAR *wTitle = NULL;
	TCHAR *wName = NULL;
	FONT initFont;
	PFONT pInitFont = NULL;


	// if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|lsslll", &pwbparent, &title, &title_len, &name, &name_len, &height, &color, &flags) == FAILURE)
	ZEND_PARSE_PARAMETERS_START(1, 6)
		Z_PARAM_LONG(pwbparent)
		Z_PARAM_STRING_OR_NULL(title, title_len)
	        Z_PARAM_OPTIONAL
		Z_PARAM_STRING_OR_NULL(name, name_len)
		Z_PARAM_LONG_OR_NULL(height, height_isnull)
		Z_PARAM_LONG_OR_NULL(color, color_isnull)
		Z_PARAM_LONG_OR_NULL(flags, flags_isnull)
	ZEND_PARSE_PARAMETERS_END();

	if (pwbparent && !wbIsWBObj((void *)pwbparent, TRUE)){
		RETURN_NULL();
	}

	wTitle = Utf82WideChar(title, title_len);

	if (name && name_len > 0)
	{
		ZeroMemory(&initFont, sizeof(FONT));
		wName = Utf82WideChar(name, name_len);
		initFont.pszName = wName;
		initFont.nHeight = height;
		initFont.color = (COLORREF)color;
		initFont.dwFlags = (DWORD)flags;
		pInitFont = &initFont;
	}

	font = wbSysDlgFont((PWBOBJ)pwbparent, wTitle, pInitFont);

	if (wTitle)
		wbFree(wTitle);
	if (wName)
		wbFree(wName);

	RETURN_LONG(font);
}

//------------------------------------------------------------------ END OF FILE
