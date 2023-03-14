/*******************************************************************************

 WINBINDER - The native Windows binding for PHP for PHP

 Copyright  Hypervisual - see LICENSE.TXT for details
 Author: Rubem Pechansky (http://winbinder.org/contact.php)

 ZEND wrapper for low-level functions

*******************************************************************************/

//----------------------------------------------------------------- DEPENDENCIES

#include "phpwb.h"

//-------------------------------------------------------------------- CONSTANTS

#define param09 param[0], param[1], param[2], param[3], param[4], param[5], param[6], param[7], param[8], param[9]

//----------------------------------------------------------- EXPORTED FUNCTIONS

ZEND_FUNCTION(wb_send_message)
{
	zend_long msg, w, l;
	zend_long pwbo;
	zend_bool w_isnull, l_isnull;

	// low level functions disabled?
	if (INI_INT("winbinder.low_level_functions") == 0)
	{
		wbError(TEXT("wb_send_message"), MB_ICONWARNING, TEXT("Low level functions disabled via ini configuration"));
		return;
	}

	// if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll|ll", &pwbo, &msg, &w, &l) == FAILURE)
	// ZEND_PARSE_PARAMETERS_START() takes two arguments minimal and maximal parameters count.
	ZEND_PARSE_PARAMETERS_START(2, 4)
		Z_PARAM_LONG(pwbo)
		Z_PARAM_LONG(msg)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG_OR_NULL(w, w_isnull)
		Z_PARAM_LONG_OR_NULL(l, l_isnull)
	ZEND_PARSE_PARAMETERS_END();

	RETURN_LONG(wbSendMessage((PWBOBJ)pwbo, (UINT64)msg, (WPARAM)w, (LPARAM)l));
}

/* Get the contents of a memory area */

ZEND_FUNCTION(wb_peek)
{
	zend_long address, bytes = 0;
	char *ptr;
	zend_bool bytes_isnull;

	// low level functions disabled?
	if (INI_INT("winbinder.low_level_functions") == 0)
	{
		wbError(TEXT("wb_peek"), MB_ICONWARNING, TEXT("Low level functions disabled via ini configuration"));
		return;
	}

	// if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l|l", &address, &bytes) == FAILURE)
	// ZEND_PARSE_PARAMETERS_START() takes two arguments minimal and maximal parameters count.
	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_LONG(address)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG_OR_NULL(bytes, bytes_isnull)
	ZEND_PARSE_PARAMETERS_END();

	if (!address){
		RETURN_NULL();
	}
	bytes = max(0, bytes);
	ptr = (char *)address;
	if (bytes == 0)
	{ // Want a zero-terminated string?
		if (!IsBadStringPtr(((LPCWSTR)ptr), 32767))
			RETURN_STRINGL(ptr, strlen(ptr));
	}
	else
	{ // No, want a memory dump
		if (!IsBadReadPtr(ptr, bytes)){
			RETURN_STRINGL(ptr, bytes);
		}
	}
	wbError(TEXT("wb_peek"), MB_ICONWARNING, TEXT("Cannot read from location %d"), (__int64)ptr);
	RETURN_NULL();
}

/* Change the contents of a memory area */

ZEND_FUNCTION(wb_poke)
{
	zend_long address, bytes = 0;
	char *contents;
	size_t contents_len;
	void *ptr;
	zend_bool bytes_isnull;

	// low level functions disabled?
	if (INI_INT("winbinder.low_level_functions") == 0)
	{
		wbError(TEXT("wb_poke"), MB_ICONWARNING, TEXT("Low level functions disabled via ini configuration"));
		return;
	}

	// if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ls|l", &address, &contents, &contents_len, &bytes) == FAILURE)
	ZEND_PARSE_PARAMETERS_START(1, 3)
		Z_PARAM_LONG(address)
		Z_PARAM_STRING(contents, contents_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG_OR_NULL(bytes, bytes_isnull)
	ZEND_PARSE_PARAMETERS_END();

	if (!address)
	{
		wbError(TEXT("wb_poke"), MB_ICONWARNING, TEXT("Invalid address"));
		RETURN_BOOL(FALSE);
	}

	if (!contents_len)
	{
		wbError(TEXT("wb_poke"), MB_ICONWARNING, TEXT("Zero length contents"));
		RETURN_BOOL(FALSE);
	}

	if (!bytes){
		bytes = contents_len;
	}
	ptr = (void *)address;

	if (IsBadWritePtr(ptr, bytes))
	{
		wbError(TEXT("wb_poke"), MB_ICONWARNING, TEXT("Cannot write to location %d"), (__int64)ptr);
		RETURN_BOOL(FALSE);
	}

	memcpy(ptr, contents, bytes);

	RETURN_BOOL(TRUE);
}

/*

The parameter must by reference for php 7

Get the address of a string, double or integer
*/

ZEND_FUNCTION(wb_get_address)
{
	zval *source;

	// low level functions disabled?
	if (INI_INT("winbinder.low_level_functions") == 0)
	{
		wbError(TEXT("wb_get_address"), MB_ICONWARNING, TEXT("Low level functions disabled via ini configuration"));
		return;
	}

	// if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z/", &source) == FAILURE)
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(source)
	ZEND_PARSE_PARAMETERS_END();

	zend_uchar sourcetype = Z_TYPE_P(source);
	if (sourcetype == IS_LONG)
	{
		RETURN_LONG((LONG_PTR)(void *)&Z_LVAL_P(source));
	}
	else if (sourcetype == IS_TRUE)
	{
		RETURN_LONG((LONG_PTR)(void *)&Z_LVAL_P(source));
	}
	else if (sourcetype == IS_FALSE)
	{
		RETURN_LONG((LONG_PTR)(void *)&Z_LVAL_P(source));
	}
	else if (sourcetype == IS_DOUBLE)
	{
		RETURN_LONG((LONG_PTR)(void *)&Z_DVAL_P(source));
	}
	else if (sourcetype == IS_STRING)
	{
		RETURN_LONG((LONG_PTR)(void *)Z_STRVAL_P(source));
	}
	else{
		RETURN_LONG((LONG_PTR)(void *)source);
	}
}

ZEND_FUNCTION(wb_load_library)
{
	char *lib;
	size_t lib_len;
	LONG_PTR hlib;

	// low level functions disabled?
	if (INI_INT("winbinder.low_level_functions") == 0)
	{
		wbError(TEXT("wb_load_library"), MB_ICONWARNING, TEXT("Low level functions disabled via ini configuration"));
		return;
	}

	// if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &lib, &lib_len) == FAILURE)
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STRING(lib,lib_len)
	ZEND_PARSE_PARAMETERS_END();

	TCHAR *wcs = 0;
	wcs = Utf82WideChar(lib, lib_len);
	hlib = (LONG_PTR)wbLoadLibrary(wcs);
	//hlib = (LONG_PTR)wbLoadLibrary(lib);

	if (hlib){
		RETURN_LONG(hlib);
	}else
	{
		wbError(TEXT("wb_load_library"), MB_ICONWARNING, TEXT("Unable to locate library %s"), wcs);
		RETURN_NULL();
	}
}

ZEND_FUNCTION(wb_release_library)
{
	zend_long hlib;

	// low level functions disabled?
	if (INI_INT("winbinder.low_level_functions") == 0)
	{
		wbError(TEXT("wb_release_library"), MB_ICONWARNING, TEXT("Low level functions disabled via ini configuration"));
		return;
	}

	// if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &hlib) == FAILURE)
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(hlib)
	ZEND_PARSE_PARAMETERS_END();


	// Is the library handle valid?

	if (IsBadCodePtr((FARPROC)hlib))
	{
		wbError(TEXT("wb_release_library"), MB_ICONWARNING, TEXT("Invalid library address %d"), hlib);
		RETURN_NULL();
	}

	RETURN_BOOL(wbReleaseLibrary((HMODULE)hlib));
}

ZEND_FUNCTION(wb_get_function_address)
{
	char *fun;
	size_t fun_len;
	zend_long addr, hlib = (LONG_PTR)NULL;
	zend_bool hlib_isnull;

	// low level functions disabled?
	if (INI_INT("winbinder.low_level_functions") == 0)
	{
		wbError(TEXT("wb_get_function_address"), MB_ICONWARNING, TEXT("Low level functions disabled via ini configuration"));
		return;
	}

	// if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|l", &fun, &fun_len, &hlib) == FAILURE)
	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STRING(fun, fun_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG_OR_NULL(hlib, hlib_isnull)
	ZEND_PARSE_PARAMETERS_END();

	// Is the library handle valid?

	if (IsBadCodePtr((FARPROC)hlib))
	{
		wbError(TEXT("wb_get_function_address"), MB_ICONWARNING, TEXT("Invalid library address %d"), hlib);
		RETURN_NULL();
	}

	addr = (LONG_PTR)wbGetLibraryFunction((HMODULE)hlib, fun);

	if (addr){
		RETURN_LONG(addr);
	}else
	{
		wbError(TEXT("wb_get_function_address"), MB_ICONWARNING, TEXT("Unable to locate function %s() in library"), fun);
		RETURN_NULL();
	}
}

ZEND_FUNCTION(wb_call_function)
{
	zend_long addr;
	LONG_PTR retval = 0;
	DWORD *param = NULL;
	zval *array = NULL, *entry = NULL;
	int i, nelem = 0;
	HashTable *target_hash;

	// low level functions disabled?
	if (INI_INT("winbinder.low_level_functions") == 0)
	{
		wbError(TEXT("wb_call_function"), MB_ICONWARNING, TEXT("Low level functions disabled via ini configuration"));
		return;
	}

	// if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l|a!", &addr, &array) == FAILURE)
	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_LONG(addr)
		Z_PARAM_OPTIONAL
		Z_PARAM_ARRAY_OR_NULL(array)
	ZEND_PARSE_PARAMETERS_END();

	// Is the address valid?

	if (IsBadCodePtr((FARPROC)addr))
	{
		wbError(TEXT("wb_call_function"), MB_ICONWARNING, TEXT("Invalid address %d"), addr);
		RETURN_NULL();
	}

	// Does the function have arguments?

	if (array)
	{
		target_hash = HASH_OF(array);
		if (target_hash)
		{

			nelem = zend_hash_num_elements(target_hash);
			zend_hash_internal_pointer_reset(target_hash);

			// Allocate memory for argument array

			param = emalloc(nelem * sizeof(DWORD));
			memset(param, nelem, 0);

			// Loop to read array items

			for (i = 0; i < nelem; i++)
			{
				if ((entry = zend_hash_get_current_data(target_hash)) == NULL)
				{
					wbError(TEXT("wb_call_function"), MB_ICONWARNING, TEXT("Could not retrieve element %d from array"), i);
					RETURN_NULL();
				}
				switch (Z_TYPE_P(entry))
				{

				case IS_ARRAY: // Invalid types
				case IS_OBJECT:
				case IS_RESOURCE:
					param[i] = (LONG_PTR)NULL;
					break;

				case IS_NULL:
					param[i] = (LONG_PTR)NULL;
					break;

				case IS_STRING:
					param[i] = (LONG_PTR)Z_STRVAL_P(entry);
					break;

				case IS_DOUBLE:
					param[i] = (DWORD)Z_DVAL_P(entry);
					break;

				case IS_LONG:
				case IS_TRUE:
				case IS_FALSE:
					param[i] = Z_LVAL_P(entry);
					break;
				}

				if (i < nelem - 1){
					zend_hash_move_forward(target_hash);
				}
			}
		}
	}

	// This ugly code calls the function with parameters. Maximum is 20 parameters which should be
	// enough for everyone.
	// Should I have to use assembly for an elegant solution?

	switch (nelem)
	{
	case 0:
		retval = ((FARPROC)addr)();
		break;
	case 1:
		retval = ((FARPROC)addr)(param[0]);
		break;
	case 2:
		retval = ((FARPROC)addr)(param[0], param[1]);
		break;
	case 3:
		retval = ((FARPROC)addr)(param[0], param[1], param[2]);
		break;
	case 4:
		retval = ((FARPROC)addr)(param[0], param[1], param[2], param[3]);
		break;
	case 5:
		retval = ((FARPROC)addr)(param[0], param[1], param[2], param[3], param[4]);
		break;
	case 6:
		retval = ((FARPROC)addr)(param[0], param[1], param[2], param[3], param[4], param[5]);
		break;
	case 7:
		retval = ((FARPROC)addr)(param[0], param[1], param[2], param[3], param[4], param[5], param[6]);
		break;
	case 8:
		retval = ((FARPROC)addr)(param[0], param[1], param[2], param[3], param[4], param[5], param[6], param[7]);
		break;
	case 9:
		retval = ((FARPROC)addr)(param[0], param[1], param[2], param[3], param[4], param[5], param[6], param[7], param[8]);
		break;
	case 10:
		retval = ((FARPROC)addr)(param09);
		break;
	case 11:
		retval = ((FARPROC)addr)(param09, param[10]);
		break;
	case 12:
		retval = ((FARPROC)addr)(param09, param[10], param[11]);
		break;
	case 13:
		retval = ((FARPROC)addr)(param09, param[10], param[11], param[12]);
		break;
	case 14:
		retval = ((FARPROC)addr)(param09, param[10], param[11], param[12], param[13]);
		break;
	case 15:
		retval = ((FARPROC)addr)(param09, param[10], param[11], param[12], param[13], param[14]);
		break;
	case 16:
		retval = ((FARPROC)addr)(param09, param[10], param[11], param[12], param[13], param[14], param[15]);
		break;
	case 17:
		retval = ((FARPROC)addr)(param09, param[10], param[11], param[12], param[13], param[14], param[15], param[16]);
		break;
	case 18:
		retval = ((FARPROC)addr)(param09, param[10], param[11], param[12], param[13], param[14], param[15], param[16], param[17]);
		break;
	case 19:
		retval = ((FARPROC)addr)(param09, param[10], param[11], param[12], param[13], param[14], param[15], param[16], param[17], param[18]);
		break;
	case 20:
		retval = ((FARPROC)addr)(param09, param[10], param[11], param[12], param[13], param[14], param[15], param[16], param[17], param[18], param[19]);
		break;
	default:
		retval = ((FARPROC)addr)(param09, param[10], param[11], param[12], param[13], param[14], param[15], param[16], param[17], param[18], param[19]);
		wbError(TEXT("wb_call_function"), MB_ICONWARNING, TEXT("Foreign function: maximum allowed is 20 parameters, some arguments ignored in function"));
		break;
	}

	// Cannot cleanup here
	efree(param);

	RETURN_LONG(retval);
}

PWBOBJ pwndMain;

// Midi callback
void CALLBACK wbMidiInProc(
	DWORD_PTR hMidiIn,
	UINT64 wMsg,
	DWORD_PTR dwInstance,
	DWORD_PTR dwParam1,
	DWORD_PTR dwParam2)
{

	DWORD param[4];
	//DWORD *ptrParam;
	//pwbo = (PWBOBJ)dwInstance;

	param[0] = wMsg;
	param[1] = dwParam1;
	param[2] = dwParam2;
	param[3] = dwInstance;

	/*if (!pwbo && pwndMain)
	{	pwbo = pwndMain;
	}*/

	if (pwndMain)
	{
		//SendMessage(pwbo->hwnd, wMsg, dwParam1,dwParam2);
		SendMessage(pwndMain->hwnd, WBWM_MIDI, hMidiIn, (LPARAM)&param);
	}

	return;
}

ZEND_FUNCTION(wb_get_midi_callback)
{
	// wbMidiInProc;
	//return 0;
}

// Enum callback
void CALLBACK wbEnumProc(
	HWND hwnd,
	LPARAM lParam)
{

	if (pwndMain)
	{
		//SendMessage(pwbo->hwnd, wMsg, dwParam1,dwParam2);
		SendMessage(pwndMain->hwnd, WBWM_ENUM, (WPARAM)hwnd, lParam);
	}

	return;
}

ZEND_FUNCTION(wb_get_enum_callback)
{
	//return 0;
	// wbEnumProc;
}

DWORD WINAPI wbHookProc(
	int code,
	WPARAM wParam,
	LPARAM lParam)
{
	DWORD param[2];
	//DWORD *ptrParam;
	//pwbo = (PWBOBJ)dwInstance;

	param[0] = wParam;
	param[1] = lParam;

	if (pwndMain)
	{
		//SendMessage(pwbo->hwnd, wMsg, dwParam1,dwParam2);
		SendMessage(pwndMain->hwnd, WBWM_HOOK, code, (LPARAM)&param);
	}
	if (code < 0)
	{
		CallNextHookEx(0, code, wParam, lParam);
	}

	return 0;
}

ZEND_FUNCTION(wb_get_hook_callback)
{
	//wbHookProc;
}

//------------------------------------------------------------------ END OF FILE
