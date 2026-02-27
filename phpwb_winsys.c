/*******************************************************************************

 WINBINDER - The native Windows binding for PHP for PHP

 Copyright  Hypervisual - see LICENSE.TXT for details
 Author: Rubem Pechansky (https://github.com/crispy-computing-machine/Winbinder)

 ZEND wrapper for Windows system functions

*******************************************************************************/

//----------------------------------------------------------------- DEPENDENCIES

#include "phpwb.h"
#include "php_ini.h" // for cfg_get_string()
#include <shellapi.h>

//-------------------------------------------------------------------- VARIABLES

// Private

static char szAppName[] = APPNAME;

// Public to wb_* modules

extern LPTSTR MakeWinPath(LPTSTR pszPath);

//----------------------------------------------------------- EXPORTED FUNCTIONS

// How can a function that is so important be so small?

ZEND_FUNCTION(wb_main_loop)
{
	RETURN_LONG(wbMainLoop());
}

ZEND_FUNCTION(wb_set_accel_table)
{
	int i, nelem;
	zval *zarray = NULL, *entry = NULL;
	HashTable *target_hash;
	char *str_accel;
	ACCEL accel[MAX_ACCELS];
	DWORD dwacc;
	int naccel = 0;
	zend_long pwbo;

	// Get function parameters

	// if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lz!", &pwbo, &zarray) == FAILURE)
	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_LONG(pwbo)
		Z_PARAM_ZVAL(zarray)
	ZEND_PARSE_PARAMETERS_END();
	if (Z_TYPE_P(zarray) == IS_ARRAY)
	{

		target_hash = HASH_OF(zarray);
		if (!target_hash){
			RETURN_BOOL(FALSE);
		}
		nelem = zend_hash_num_elements(target_hash);
		zend_hash_internal_pointer_reset(target_hash);

		// Loop to read array items

		for (i = 0; i < nelem; i++)
		{
			if ((entry = zend_hash_get_current_data(target_hash)) == NULL)
			{
				wbError(TEXT("wb_set_accel_table"), MB_ICONWARNING, TEXT("Could not retrieve element %d from array in function"), i);
				RETURN_BOOL(FALSE);
			}

			switch (Z_TYPE_P(entry))
			{

			case IS_ARRAY: // An accelerator item is an array inside an array
				parse_array(entry, "ls", &accel[naccel].cmd, &str_accel);
				if (str_accel && *str_accel && naccel < MAX_ACCELS)
				{
					dwacc = wbMakeAccelFromString(str_accel);
					accel[naccel].key = LOWORD(dwacc);
					accel[naccel].fVirt = (BYTE)HIWORD(dwacc);
					//						printf("> %d %d %d %s\n", accel[naccel].key, accel[naccel].fVirt, accel[naccel].cmd, str_accel);
					naccel++;
				}
				break;

			default:
				wbError(TEXT("wb_set_accel_table"), MB_ICONWARNING, TEXT("Accelerator table must be an array of arrays with two elements"));
				RETURN_BOOL(FALSE);
				break;
			}

			if (i < nelem - 1){
				zend_hash_move_forward(target_hash);
			}
		}

		// Create accelerator table

		RETURN_BOOL(wbSetAccelerators((PWBOBJ)pwbo, accel, naccel));
	}
	else
	{
		RETURN_BOOL(wbSetAccelerators((PWBOBJ)pwbo, NULL, 0));
	}
}

ZEND_FUNCTION(wb_set_cursor)
{
	zend_long pwbo;
	zval *source = NULL;
	zend_uchar sourcetype;
	HANDLE hCursor;
	LPTSTR pszCursorName;

	// if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lz!", &pwbo, &source) == FAILURE)
	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_LONG(pwbo)
		Z_PARAM_ZVAL(source)
	ZEND_PARSE_PARAMETERS_END();

	sourcetype = Z_TYPE_P(source);

	if (!source)
	{

		hCursor = NULL;
		pszCursorName = NULL;
	}
	else if (sourcetype == IS_LONG)
	{

		hCursor = (HANDLE)source->value.lval;
		pszCursorName = NULL;
	}
	else if (sourcetype == IS_STRING)
	{

		hCursor = NULL;
		pszCursorName = Utf82WideChar(Z_STRVAL_P(source), Z_STRLEN_P(source));
	}
	else
	{
		wbError(TEXT("wb_set_cursor"), MB_ICONWARNING, TEXT("Invalid parameter type passed to function"));
		RETURN_BOOL(FALSE);
	}

	RETURN_BOOL(wbSetCursor((PWBOBJ)pwbo, pszCursorName, hCursor));
}

/*
ZEND_FUNCTION(wb_load_media)
{
	int filename_len;
	LONG_PTR flags;
	char *filename;
    BOOL bRet;

    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
	  "sl", &cmd, &filename_len, &flags) == FAILURE)
		return;

	if(!filename || !*filename)
		RETURN_NULL();

		PMEDIA pMd;

		MakeWinPath(source->value.str.val);
		pMd = wbOpenMedia(source->value.str.val, TRUE);
		if(!pMd) {
			zend_error(E_WARNING, "Could not open media file %s in function %s()",
			  source->value.str.val, get_active_function_name(TSRMLS_C));
			RETURN_NULL();
		}
		bRet = wbControlMedia(pMd, cmd, 0, 1);
		wbCloseMedia(pMd);
		RETURN_BOOL(bRet);
}

ZEND_FUNCTION(wb_destroy_media)
{
}
*/

ZEND_FUNCTION(wb_play_sound)
{
	size_t cmd_len;
	char *cmd = "";
	zval *source;

	TCHAR *szPath = 0;
	TCHAR *szCmd = 0;
	BOOL ret;

	// if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|s", &source, &cmd, &cmd_len) == FAILURE)
	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_ZVAL(source)
		Z_PARAM_OPTIONAL
		Z_PARAM_STRING_OR_NULL(cmd,cmd_len)
	ZEND_PARSE_PARAMETERS_END();

	if (!source){
		RETURN_NULL();
	}
	if (Z_TYPE_P(source) == IS_LONG)
	{ // It's an integer: Play system sound

		RETURN_BOOL(wbPlaySystemSound(source->value.lval));
	}
	else if (Z_TYPE_P(source) == IS_STRING)
	{ // It's an empty string or filename

		if (*Z_STRVAL_P(source))
		{
			szPath = Utf82WideChar(Z_STRVAL_P(source), Z_STRLEN_P(source));
			MakeWinPath(szPath);
			if (!EXISTFILE(szPath))
			{
				wbFree(szPath);
				wbError(TEXT("wb_play_sound"), MB_ICONWARNING, TEXT("Could not open media file %s in function"), Z_STRVAL_P(source));
				RETURN_BOOL(FALSE);
				return;
			}

			szCmd = Utf82WideChar(cmd, cmd_len);
		}

		ret = wbPlaySound(szPath, szCmd);

		if (ret)
		{
			RETURN_BOOL(TRUE);
		}
		else
		{
			wbError(TEXT("wb_play_sound"), MB_ICONWARNING, TEXT("Unknown command '%s' in function"), cmd);
			RETURN_BOOL(FALSE);
		}
	}
	else
	{
		wbError(TEXT("wb_play_sound"), MB_ICONWARNING, TEXT("Invalid parameter type passed to function"), cmd);
		RETURN_BOOL(FALSE);
	}
}

ZEND_FUNCTION(wb_stop_sound)
{
	size_t cmd_len;
	char *cmd = "";

	TCHAR *wcs = 0;

	// if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s", &cmd, &cmd_len) == FAILURE)
	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_STRING_OR_NULL(cmd, cmd_len)
	ZEND_PARSE_PARAMETERS_END();

	wcs = Utf82WideChar(cmd, cmd_len);
	RETURN_BOOL(wbStopSound(wcs));
}

static UINT wbNotifyIconFlagFromValue(zval *icon)
{
	if (!icon)
		return NIIF_INFO;

	if (Z_TYPE_P(icon) == IS_LONG)
		return (UINT)Z_LVAL_P(icon);

	if (Z_TYPE_P(icon) == IS_STRING)
	{
		char *name = Z_STRVAL_P(icon);

		if (!name)
			return NIIF_INFO;
		if (!stricmp(name, "error") || !stricmp(name, "danger"))
			return NIIF_ERROR;
		if (!stricmp(name, "warning") || !stricmp(name, "warn"))
			return NIIF_WARNING;
		if (!stricmp(name, "none"))
			return NIIF_NONE;
	}

	return NIIF_INFO;
}

ZEND_FUNCTION(wb_notify)
{
	zval *options;
	zval *zParent = NULL, *zTitle = NULL, *zBody = NULL, *zIcon = NULL, *zDuration = NULL;
	PWBOBJ pwboParent = NULL;
	TCHAR *szTitle = NULL, *szBody = NULL;
	DWORD durationMs = 5000;
	UINT iconFlags = NIIF_INFO;
	BOOL usedSystemNotification = FALSE;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY(options)
	ZEND_PARSE_PARAMETERS_END();

	zParent = zend_hash_str_find(Z_ARRVAL_P(options), "parent", sizeof("parent") - 1);
	zTitle = zend_hash_str_find(Z_ARRVAL_P(options), "title", sizeof("title") - 1);
	zBody = zend_hash_str_find(Z_ARRVAL_P(options), "body", sizeof("body") - 1);
	zIcon = zend_hash_str_find(Z_ARRVAL_P(options), "icon", sizeof("icon") - 1);
	zDuration = zend_hash_str_find(Z_ARRVAL_P(options), "duration", sizeof("duration") - 1);

	if (zParent && Z_TYPE_P(zParent) == IS_LONG)
		pwboParent = (PWBOBJ)Z_LVAL_P(zParent);

	if (pwboParent && !wbIsWBObj((void *)pwboParent, TRUE))
		RETURN_FALSE;

	if (zDuration)
	{
		zval tmp;

		ZVAL_COPY(&tmp, zDuration);
		convert_to_long(&tmp);
		if (Z_LVAL(tmp) > 0)
			durationMs = (DWORD)Z_LVAL(tmp);
		zval_ptr_dtor(&tmp);
	}

	if (zTitle && Z_TYPE_P(zTitle) == IS_STRING && Z_STRLEN_P(zTitle) > 0)
		szTitle = Utf82WideChar(Z_STRVAL_P(zTitle), Z_STRLEN_P(zTitle));
	else
		szTitle = Utf82WideChar("WinBinder notification", sizeof("WinBinder notification") - 1);

	if (zBody && Z_TYPE_P(zBody) == IS_STRING && Z_STRLEN_P(zBody) > 0)
		szBody = Utf82WideChar(Z_STRVAL_P(zBody), Z_STRLEN_P(zBody));
	else
		szBody = Utf82WideChar("", 0);

	iconFlags = wbNotifyIconFlagFromValue(zIcon);

	if (pwboParent && pwboParent->hwnd && IsWindow(pwboParent->hwnd))
	{
		NOTIFYICONDATA nid;

		ZeroMemory(&nid, sizeof(nid));
		nid.cbSize = sizeof(nid);
		nid.hWnd = pwboParent->hwnd;
		nid.uID = 100;
		nid.uFlags = NIF_INFO;
		nid.dwInfoFlags = iconFlags;
		nid.uTimeout = durationMs;
		lstrcpyn(nid.szInfoTitle, szTitle, sizeof(nid.szInfoTitle) / sizeof(TCHAR));
		lstrcpyn(nid.szInfo, szBody, sizeof(nid.szInfo) / sizeof(TCHAR));

		usedSystemNotification = Shell_NotifyIcon(NIM_MODIFY, &nid);
	}

	if (!usedSystemNotification)
	{
		FLASHWINFO fw;
		UINT beepType = MB_ICONINFORMATION;

		if (iconFlags == NIIF_ERROR)
			beepType = MB_ICONERROR;
		else if (iconFlags == NIIF_WARNING)
			beepType = MB_ICONWARNING;

		wbPlaySystemSound(beepType);

		if (pwboParent && pwboParent->hwnd && IsWindow(pwboParent->hwnd))
		{
			ZeroMemory(&fw, sizeof(fw));
			fw.cbSize = sizeof(fw);
			fw.hwnd = pwboParent->hwnd;
			fw.dwFlags = FLASHW_TRAY | FLASHW_TIMERNOFG;
			fw.uCount = 2;
			fw.dwTimeout = durationMs / 2;
			FlashWindowEx(&fw);
		}
	}

	array_init(return_value);
	add_assoc_bool(return_value, "delivered", TRUE);
	add_assoc_string(return_value, "backend", usedSystemNotification ? "windows_tray_balloon" : "in_app_banner");
	add_assoc_bool(return_value, "system_supported", usedSystemNotification);
	add_assoc_bool(return_value, "click_callback_supported", 0);

	if (szTitle)
		wbFree(szTitle);
	if (szBody)
		wbFree(szBody);
}

ZEND_FUNCTION(wb_message_box)
{
	char *msg, *title = NULL;
	zend_long pwbo, style = MB_OK;
	size_t msg_len, title_len = 0;
	int ret;

	TCHAR *szMsg = 0;
	TCHAR *szTitle = 0;

	// if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ls|sl", &pwbo, &msg, &msg_len, &title, &title_len, &style) == FAILURE)
	ZEND_PARSE_PARAMETERS_START(2, 4)
		Z_PARAM_LONG(pwbo)
		Z_PARAM_STRING(msg, msg_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_STRING(title, title_len)
		Z_PARAM_LONG(style)
	ZEND_PARSE_PARAMETERS_END();

	if (pwbo && !wbIsWBObj((void *)pwbo, TRUE))
	{
		RETURN_NULL();
	}
	if (!title || !*title)
	{
		title = szAppName;
		title_len = strlen(szAppName);
	}

	szMsg = Utf82WideChar(msg, msg_len);
	szTitle = Utf82WideChar(title, title_len);
	ret = wbMessageBox((PWBOBJ)pwbo, szMsg, szTitle, style);

	switch (ret)
	{
	case -2: // Error
		RETURN_NULL();
	case -1: // IDNO
		RETURN_LONG(0);
	case 0: // Cancel, etc.
		RETURN_BOOL(FALSE);
	case 1: // OK, etc.
		RETURN_BOOL(TRUE);
	default:
		RETURN_BOOL(FALSE);
	}
}

ZEND_FUNCTION(wb_quiet_message_box)
{
	char *msg, *title = NULL;
	zend_long pwbo, style = MB_OK, timeout = 0;
	size_t msg_len, title_len = 0;
	int ret;

	TCHAR *szMsg = 0;
	TCHAR *szTitle = 0;

	ZEND_PARSE_PARAMETERS_START(2, 5)
		Z_PARAM_LONG(pwbo)
		Z_PARAM_STRING(msg, msg_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_STRING(title, title_len)
		Z_PARAM_LONG(style)
		Z_PARAM_LONG(timeout)
	ZEND_PARSE_PARAMETERS_END();

	if (pwbo && !wbIsWBObj((void *)pwbo, TRUE))
	{
		RETURN_NULL();
	}
	if (!title || !*title)
	{
		title = szAppName;
		title_len = strlen(szAppName);
	}
	if (timeout < 0)
		timeout = 0;

	szMsg = Utf82WideChar(msg, msg_len);
	szTitle = Utf82WideChar(title, title_len);
	ret = wbQuietMessageBox((PWBOBJ)pwbo, szMsg, szTitle, style, (DWORD)timeout);

	switch (ret)
	{
	case -2: // Error
		RETURN_NULL();
	case -1: // IDNO
		RETURN_LONG(0);
	case 0: // Cancel, etc.
		RETURN_BOOL(FALSE);
	case 1: // OK, etc.
		RETURN_BOOL(TRUE);
	case IDTIMEOUT: // Auto-close timeout
		RETURN_LONG(IDTIMEOUT);
	default:
		RETURN_BOOL(FALSE);
	}
}

ZEND_FUNCTION(wb_exec)
{
	char *pgm, *parm = NULL;
	size_t pgm_len, parm_len = 0;
	zend_bool show;
	TCHAR *szPgm = 0;
	TCHAR *szParm = 0;
	zend_bool show_isnull = TRUE;

	// if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|sl", &pgm, &pgm_len, &parm, &parm_len, &show) == FAILURE)
	ZEND_PARSE_PARAMETERS_START(1, 3)
		Z_PARAM_STRING(pgm, pgm_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_STRING_OR_NULL(parm, parm_len)
		Z_PARAM_BOOL_OR_NULL(show, show_isnull)
	ZEND_PARSE_PARAMETERS_END();

	szPgm = Utf82WideChar(pgm, pgm_len);
	szParm = Utf82WideChar(parm, parm_len);
	RETURN_LONG(wbExec(szPgm, szParm, show));
}

ZEND_FUNCTION(wb_is_running)
{
	zend_long pid;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(pid)
	ZEND_PARSE_PARAMETERS_END();

	RETURN_BOOL(wbIsRunning(pid));
}


ZEND_FUNCTION(wb_get_system_info)
{
	char *s;
	size_t s_len;
	BOOL isstr;
	LONG_PTR res = -1;
	char strval[1024];

	TCHAR szVal[1024];
	TCHAR *wcs = 0;

	//if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &s, &s_len) == FAILURE)
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STRING_OR_NULL(s,s_len)
	ZEND_PARSE_PARAMETERS_END();

	if (!s){
		RETURN_NULL();
	}

	if (!stricmp(s, "extensionpath"))
	{

		// Calls Zend function cfg_get_string()

		char *value;
		TCHAR *szValue = 0;

		if (cfg_get_string("extension_dir", &value) == FAILURE){
			RETURN_BOOL(FALSE);
		}
		szValue = Utf82WideChar(value, strlen(value));

		// Assemble the final string
		wbGetSystemInfo(TEXT("exepath"), &isstr, szVal, 1023);
		if (wcslen(szVal) + wcslen(szValue) >= 1023){
			wbFree(szValue);
			RETURN_NULL();
		}
		wcsncat(szVal, szValue, 1023 - wcslen(szVal));
		MakeWinPath(szVal);
		wbFree(szValue);

		WideCharCopy(szVal, strval, 1024);
		isstr = TRUE;
	}
	else
	{
		wcs = Utf82WideChar(s, s_len);
		// Calls the API (low-level) WinBinder layer
		res = wbGetSystemInfo(wcs, &isstr, szVal, 1023);
		WideCharCopy(szVal, strval, 1024);
	}

	if (isstr)
	{
		if (!*strval && (res == -1))
		{ // Unrecognized parameter
			RETURN_NULL();
		}
		else
		{
			RETURN_STRING(strval);
		}
	}
	else
	{
		RETURN_LONG(res);
	}
}

/*

Search for a file in the Windows and System directories, in this order. If the file exists,
return the complete path to it. If not, return filename.

*/

ZEND_FUNCTION(wb_find_file)
{
	char *s;
	size_t s_len;
	char path[MAX_PATH * 4];

	TCHAR *szPath = 0;

	// if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &s, &s_len) == FAILURE)
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STRING(s,s_len)
	ZEND_PARSE_PARAMETERS_END();

	szPath = Utf82WideChar(s, s_len);
	wbFindFile(szPath, MAX_PATH * 4);

	WideCharCopy(szPath, path, MAX_PATH * 4);
	RETURN_STRING(path);
}

/*

Reads a registry key as a string. If it is a DWORD, convert to a string
NOTE: maximum string is 1024 characters
*/

ZEND_FUNCTION(wb_get_registry_key)
{
	char *key, *subkey, *entry = NULL;
	size_t key_len, subkey_len, entry_len;
	int buflen = 1024;
	char sval[1024];
	TCHAR szVal[1024];

	TCHAR *szKey = 0;
	TCHAR *szSubKey = 0;
	TCHAR *szEntry = 0;
	BOOL ret;

	// if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|s", &key, &key_len, &subkey, &subkey_len, &entry, &entry_len) == FAILURE)
	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_STRING(key, key_len)
		Z_PARAM_STRING(subkey, subkey_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_STRING_OR_NULL(entry, entry_len)
	ZEND_PARSE_PARAMETERS_END();

	szKey = Utf82WideChar(key, key_len);
	szSubKey = Utf82WideChar(subkey, subkey_len);
	szEntry = Utf82WideChar(entry, entry_len);

	ret = wbReadRegistryKey(szKey, szSubKey, szEntry, szVal, &buflen);

	if (ret)
	{
		if (*szVal)
		{
			WideCharCopy(szVal, sval, buflen);
			RETURN_STRING(sval);
		}
		else
			RETURN_STRING("");
	}
	else
	{
		RETURN_NULL();
	}
}

ZEND_FUNCTION(wb_set_registry_key)
{
	char *key, *subkey, *entry;
	size_t key_len, subkey_len, entry_len;
	zval *source = NULL;

	TCHAR *szKey = 0;
	TCHAR *szSubKey = 0;
	TCHAR *szEntry = 0;
	TCHAR *szVal = 0;
	BOOL ret;

	// if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|sz!", &key, &key_len, &subkey, &subkey_len, &entry, &entry_len, &source) == FAILURE)
	ZEND_PARSE_PARAMETERS_START(2, 4)
		Z_PARAM_STRING(key, key_len)
		Z_PARAM_STRING(subkey, subkey_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_STRING_OR_NULL(entry, entry_len)
		Z_PARAM_ZVAL_OR_NULL(source)
	ZEND_PARSE_PARAMETERS_END();

	if (!source)
	{
		szKey = Utf82WideChar(key, key_len);
		szSubKey = Utf82WideChar(subkey, subkey_len);
		szEntry = Utf82WideChar(entry, entry_len);

		ret = wbWriteRegistryKey(szKey, szSubKey, szEntry, NULL, 0, TRUE);

		RETURN_BOOL(ret);
		// 2016_08_12 - Jared Allard: no more IS_BOOL, use IS_TRUE/IS_FALSE
	}
	else
	{
		zend_uchar sourcetype = Z_TYPE_P(source);

		if (sourcetype == IS_LONG || (sourcetype == IS_FALSE || sourcetype == IS_TRUE))
		{
			szKey = Utf82WideChar(key, key_len);
			szSubKey = Utf82WideChar(subkey, subkey_len);
			szEntry = Utf82WideChar(entry, entry_len);

			ret = wbWriteRegistryKey(szKey, szSubKey, szEntry, NULL, source->value.lval, FALSE);

			RETURN_BOOL(ret);
		}
		else if (sourcetype == IS_DOUBLE)
		{
			TCHAR szAux[50];
			swprintf(szAux, sizeof(szAux) / sizeof(szAux[0]), TEXT("%20.20f"), source->value.dval);

			szKey = Utf82WideChar(key, key_len);
			szSubKey = Utf82WideChar(subkey, subkey_len);
			szEntry = Utf82WideChar(entry, entry_len);

			ret = wbWriteRegistryKey(szKey, szSubKey, szEntry, szAux, 0, TRUE);

			RETURN_BOOL(ret);
		}
		else if (sourcetype == IS_STRING)
		{
			szKey = Utf82WideChar(key, key_len);
			szSubKey = Utf82WideChar(subkey, subkey_len);
			szEntry = Utf82WideChar(entry, entry_len);
			szVal = Utf82WideChar(Z_STRVAL_P(source), Z_STRLEN_P(source));

			ret = wbWriteRegistryKey(szKey, szSubKey, szEntry, szVal, 0, TRUE);

			RETURN_BOOL(ret);
		}
		else
		{
			wbError(TEXT("wb_set_registry_key"), MB_ICONWARNING, TEXT("Invalid parameter type passed to function"));
			RETURN_NULL();
		}
	}
}

ZEND_FUNCTION(wb_create_timer)
{
	zend_long pwbo;
	zend_long id, ms;

	// if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lll", &pwbo, &id, &ms) == FAILURE)
	ZEND_PARSE_PARAMETERS_START(3, 3)
		Z_PARAM_LONG(pwbo)
		Z_PARAM_LONG(id)
		Z_PARAM_LONG(ms)
	ZEND_PARSE_PARAMETERS_END();

	if (!wbIsWBObj((void *)pwbo, TRUE)){
		RETURN_BOOL(FALSE);
	}
	RETURN_BOOL(wbSetTimer((PWBOBJ)pwbo, id, MAX(1, ms)));
}

ZEND_FUNCTION(wb_destroy_timer)
{
	zend_long pwbo;
	zend_long id;

	// if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &pwbo, &id) == FAILURE)
	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_LONG(pwbo)
		Z_PARAM_LONG(id)
	ZEND_PARSE_PARAMETERS_END();

	if (!wbIsWBObj((void *)pwbo, TRUE)){
		RETURN_BOOL(FALSE);
	}
	RETURN_BOOL(wbSetTimer((PWBOBJ)pwbo, id, 0));
}

ZEND_FUNCTION(wb_wait)
{
	zend_long pwbo = 0, pause = 0, flags = WBC_KEYDOWN;
	zend_bool pwbo_isnull, pause_isnull, flags_isnull;

	// if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|lll", &pwbo, &pause, &flags) == FAILURE)
	ZEND_PARSE_PARAMETERS_START(0, 3)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG_OR_NULL(pwbo, pwbo_isnull)
		Z_PARAM_LONG_OR_NULL(pause, pause_isnull)
		Z_PARAM_LONG_OR_NULL(flags, flags_isnull)
	ZEND_PARSE_PARAMETERS_END();

	if (pwbo != 0 && !wbIsWBObj((void *)pwbo, TRUE)){
		RETURN_NULL();
	}
	RETURN_LONG(wbCheckInput((PWBOBJ)pwbo, flags, pause));
}

ZEND_FUNCTION(wb_is_obj)
{
	zend_long pwbo;

	// if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &pwbo) == FAILURE)
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(pwbo)
	ZEND_PARSE_PARAMETERS_END();

	RETURN_BOOL(wbIsWBObj((void *)pwbo, FALSE));
}

/*
ZEND_FUNCTION(wb_get_clipboard)
{
	char tcopy[4096];
	//char *wText;
	char *wGlobal;
	HANDLE hdata;
	size_t blen;

	BOOL success = FALSE;

	if (OpenClipboard(NULL))
	{
		hdata = GetClipboardData(CF_TEXT);
		if (hdata)
		{
			wGlobal = (wchar_t *)GlobalLock(hdata);
			if (wGlobal != NULL)
			{
				blen = GlobalSize(hdata);
				if (blen > 4095)
					blen = 4095;
				memcpy(tcopy, wGlobal, blen);
				GlobalUnlock(hdata);
				tcopy[blen] = 0;
				success = TRUE;
			}
		}
	}
	CloseClipboard();
	if (!success)
		RETURN_NULL();
	RETURN_STRING(tcopy, TRUE);
}
*/

ZEND_FUNCTION(wb_get_clipboard)
{
	HANDLE hData = NULL;
	char *buffer = NULL;
	SIZE_T data_size = 0;
	SIZE_T data_len = 0;

	if (!OpenClipboard(NULL)){
		RETURN_NULL();
	}

	hData = GetClipboardData(CF_TEXT);
	if (!hData){
		CloseClipboard();
		RETURN_NULL();
	}

	buffer = (char *)GlobalLock(hData);
	if (!buffer){
		CloseClipboard();
		RETURN_NULL();
	}

	data_size = GlobalSize(hData);
	while (data_len < data_size && buffer[data_len]){
		data_len++;
	}

	RETVAL_STRINGL(buffer, data_len);
	GlobalUnlock(hData);
	CloseClipboard();
}

ZEND_FUNCTION(wb_set_clipboard)
{
	char *clip;
	WCHAR *wclip;
	size_t size;
	BOOL success = FALSE;

	HGLOBAL hdata;
	HGLOBAL hwdata;
	LPTSTR clipcopy;
	LPTSTR wclipcopy;

	// if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &clip, &size) == FAILURE)
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STRING(clip, size)
	ZEND_PARSE_PARAMETERS_END();

	if (OpenClipboard(NULL))
	{
		if (!EmptyClipboard()){
			CloseClipboard();
			RETURN_BOOL(FALSE);
		}

		hdata = GlobalAlloc(GMEM_MOVEABLE, size + 1);
		if (!hdata){
			CloseClipboard();
			RETURN_BOOL(FALSE);
		}

		clipcopy = (LPTSTR)GlobalLock(hdata);
		if (!clipcopy){
			GlobalFree(hdata);
			CloseClipboard();
			RETURN_BOOL(FALSE);
		}

		memcpy(clipcopy, clip, size);
		clipcopy[size] = '\0';
		GlobalUnlock(hdata);

		if (SetClipboardData(CF_TEXT, hdata))
		{
			size = size * 2;
			wclip = (WCHAR *)emalloc(size + 2);
			if (!wclip){
				CloseClipboard();
				RETURN_BOOL(FALSE);
			}

			if (!UTF8ToUnicode16(clip, wclip, size + 2)){
				efree(wclip);
				CloseClipboard();
				RETURN_BOOL(FALSE);
			}
			hwdata = GlobalAlloc(GMEM_MOVEABLE, size + 2);
			if (!hwdata){
				efree(wclip);
				CloseClipboard();
				RETURN_BOOL(FALSE);
			}

			wclipcopy = (LPTSTR)GlobalLock(hwdata);
			if (!wclipcopy){
				GlobalFree(hwdata);
				efree(wclip);
				CloseClipboard();
				RETURN_BOOL(FALSE);
			}

			memcpy(wclipcopy, wclip, size);
			((WCHAR *)wclipcopy)[size / sizeof(WCHAR)] = L'\0';
			GlobalUnlock(hwdata);

			if (!SetClipboardData(CF_UNICODETEXT, hwdata)){
				GlobalFree(hwdata);
				efree(wclip);
				CloseClipboard();
				RETURN_BOOL(FALSE);
			}

			efree(wclip);
			success = TRUE;
		}
		else
		{
			GlobalFree(hdata);
		}
	}
	CloseClipboard();
	RETURN_BOOL(success);
}

ZEND_FUNCTION(wb_empty_clipboard)
{
	BOOL success = FALSE;
	if (OpenClipboard(NULL))
	{
		if (EmptyClipboard()){
			success = TRUE;
		}
		CloseClipboard();
	}
	RETURN_BOOL(success);
}

// get mouse pos else return false
// https://learn.microsoft.com/en-us/windows/win32/inputdev/using-mouse-input?source=recommendations#tracking-the-mouse-cursor
ZEND_FUNCTION(wb_get_mouse_pos)
{
	POINT cursor;

	zend_long pwbo;
	zend_long id;
	zend_bool pwbo_isnull;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG_OR_NULL(pwbo, pwbo_isnull)
	ZEND_PARSE_PARAMETERS_END();


    // New return array
    array_init(return_value);

	// https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getcursorpos
    if (GetCursorPos(&cursor)) {

		// A handle to the window whose client area will be used for the conversion.
		if(!pwbo_isnull && ((PWBOBJ)pwbo)->hwnd){
			// converts the screen coordinates of a specified point on the screen to client-area coordinates
			// https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-screentoclients
			ScreenToClient(((PWBOBJ)pwbo)->hwnd, &cursor);	
		}

        add_index_long(return_value, 0, cursor.x);
        add_index_long(return_value, 1, cursor.y);
        return;
    }
    RETURN_BOOL(0);
}

ZEND_FUNCTION(wb_wmi_query)
{
	char *wquery;
	size_t wquery_size;

	
	HRESULT hr;
	BSTR PropName = NULL;
	char *propn;
	char *propv;
	int wslen;
	BSTR query;
	long nCount;
	char err[256];


	// COM interface pointers
	IWbemLocator         *locator  = NULL;
	IWbemServices        *services = NULL;
	IEnumWbemClassObject *results  = NULL;

	// BSTR strings we'll use (http://msdn.microsoft.com/en-us/library/ms221069.aspx)
	BSTR resource = SysAllocString(L"ROOT\\CIMV2");
	BSTR language = SysAllocString(L"WQL");

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STRING_OR_NULL(wquery, wquery_size)
	ZEND_PARSE_PARAMETERS_END();
	
	wslen = MultiByteToWideChar(CP_ACP, 0, wquery, wquery_size, 0, 0);
    query = SysAllocStringLen(0, wslen);
    MultiByteToWideChar(CP_ACP, 0, wquery, wquery_size, query, wslen);

	// initialize COM
	hr = CoInitializeEx(0, COINIT_MULTITHREADED);
	hr = CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_DEFAULT, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL);

	// connect to WMI
	hr = CoCreateInstance(&CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, &IID_IWbemLocator, (LPVOID *) &locator);
	hr = locator->lpVtbl->ConnectServer(locator, resource, NULL, NULL, NULL, 0, NULL, NULL, &services);

	// issue a WMI query
	hr = services->lpVtbl->ExecQuery(services, language, query, WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY | WBEM_FLAG_DIRECT_READ, NULL, &results);

	// list the query results
	if (results != NULL) {
		IWbemClassObject *result = NULL;
		ULONG returnedCount = 0;
		
		array_init(return_value);

		// enumerate the retrieved objects
		while((hr = results->lpVtbl->Next(results, WBEM_INFINITE, 1, &result, &returnedCount)) == S_OK) {
			VARIANT val;
			VARTYPE vt = 0;
			LPSAFEARRAY pFieldArray = NULL;
			zval subarray;

			array_init(&subarray);

			hr = result->lpVtbl->GetNames(result, NULL, WBEM_FLAG_ALWAYS | WBEM_FLAG_NONSYSTEM_ONLY, NULL, &pFieldArray);
			
			for(nCount = 0; nCount < pFieldArray->rgsabound->cElements; nCount++) {
			
				hr = SafeArrayGetElement(pFieldArray, &nCount, &PropName);
				hr = result->lpVtbl->Get(result, PropName, 0, &val, 0, 0);
				propn = ConvertBSTRToLPSTR(PropName);
				
				//https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-oaut/3fe7db9f-5803-4dc4-9d14-5425d3f5461f
				switch(val.vt){
					case VT_NULL:
						add_assoc_null(&subarray, propn);
						break;
					case VT_BOOL:
						add_assoc_bool(&subarray, propn, val.boolVal);
						break;
					case VT_BSTR:
						propv = ConvertBSTRToLPSTR(val.bstrVal);
						add_assoc_string(&subarray, propn, propv);
						free(propv);
						break;
					case VT_I4:
						add_assoc_long(&subarray, propn, val.intVal);
						break;
				
					default:
						sprintf(err, "(Variant type 0x%04x not supported)", val.vt);
						add_assoc_string(&subarray, propn, err);
				
				}
				free(propn);
			}
			add_next_index_zval(return_value, &subarray);
			result->lpVtbl->Release(result);
		}
	}

	// release WMI COM interfaces
	results->lpVtbl->Release(results);
	services->lpVtbl->Release(services);
	locator->lpVtbl->Release(locator);

	// unwind everything else we've allocated
	CoUninitialize();

	SysFreeString(query);
	SysFreeString(language);
	SysFreeString(resource);
}


// https://msdn.microsoft.com/en-ca/library/windows/desktop/ms724385(v=vs.85).aspx
ZEND_FUNCTION(wb_get_system_metric)
{
	zend_long idx;
	int val;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(idx)
	ZEND_PARSE_PARAMETERS_END();
	
	val = GetSystemMetrics(idx);
	RETURN_LONG((long)val);
}


ZEND_FUNCTION(wb_get_system_timezone)
{
	char tz[16];
	get_system_timezone(tz);
	RETURN_STRING(tz);
}


ZEND_FUNCTION(wb_expand_env)
{
	char *path;
	size_t path_size;
	char buffer[MAXPATHLEN];

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STRING_OR_NULL(path, path_size)
	ZEND_PARSE_PARAMETERS_END();

	if(!ExpandEnvironmentStringsA(path, buffer, MAXPATHLEN-1)) RETURN_BOOL(FALSE);
	RETURN_STRING(buffer);
}


ZEND_FUNCTION(wb_send_key)
{
	zval *keys = NULL;
	zval *pzval;
	INPUT *inputs;
	UINT uSent;
	int kcount, i = 0;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(keys)
	ZEND_PARSE_PARAMETERS_END();

	if(Z_TYPE_P(keys) == IS_LONG){
		inputs = malloc(sizeof(INPUT)*2);
		ZeroMemory(inputs, sizeof(INPUT)*2);
		inputs[0].type = INPUT_KEYBOARD;
		inputs[0].ki.wVk = Z_LVAL_P(keys);
		inputs[1].type = INPUT_KEYBOARD;
		inputs[1].ki.wVk = Z_LVAL_P(keys);
		inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;
		uSent = SendInput(2, inputs, sizeof(INPUT));
		RETURN_BOOL(uSent == 2);
	} else if(Z_TYPE_P(keys) == IS_ARRAY){
		kcount = zend_hash_num_elements(Z_ARRVAL_P(keys));
		inputs = malloc(sizeof(INPUT)*kcount*2);
		ZeroMemory(inputs, sizeof(INPUT)*kcount*2);
		for(zend_hash_internal_pointer_reset(Z_ARRVAL_P(keys)); zend_hash_has_more_elements(Z_ARRVAL_P(keys)) == SUCCESS; zend_hash_move_forward(Z_ARRVAL_P(keys)), i++){
			pzval = zend_hash_get_current_data(Z_ARRVAL_P(keys));
			inputs[i].type = INPUT_KEYBOARD;
			inputs[i].ki.wVk = Z_LVAL_P(pzval);
		}
		for(zend_hash_internal_pointer_end(Z_ARRVAL_P(keys)); zend_hash_has_more_elements(Z_ARRVAL_P(keys)) == SUCCESS; zend_hash_move_backwards(Z_ARRVAL_P(keys)), i++){
			pzval = zend_hash_get_current_data(Z_ARRVAL_P(keys));
			inputs[i].type = INPUT_KEYBOARD;
			inputs[i].ki.wVk = Z_LVAL_P(pzval);
			inputs[i].ki.dwFlags = KEYEVENTF_KEYUP;
		}
		uSent = SendInput(kcount*2, inputs, sizeof(INPUT));
		RETURN_BOOL(uSent == (kcount*2));
	} else {
		php_error_docref(NULL, E_WARNING, "Parameter 1 should be a an long or an array of longs");
		RETURN_BOOL(FALSE);
	}
}
//------------------------------------------------------------------ END OF FILE
