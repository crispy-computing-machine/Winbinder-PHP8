/*******************************************************************************

 WINBINDER - The native Windows binding for PHP for PHP

 Copyright  Hypervisual - see LICENSE.TXT for details
 Author: Rubem Pechansky (https://github.com/crispy-computing-machine/Winbinder)

 ZEND wrapper for toolbar control

*******************************************************************************/

//----------------------------------------------------------------- DEPENDENCIES

#include "phpwb.h"

//----------------------------------------------------------- EXPORTED FUNCTIONS

ZEND_FUNCTION(wb_create_toolbar)
{
	zend_long i, nelem; 
	size_t s_len = 0;
	zval *zarray = NULL, *entry = NULL;
	HashTable *target_hash;
	HANDLE hImage = NULL;
	LONG_PTR l;
	zend_long pwboParent, width = 0, height = 0;
	char *s = "";
	PWBITEM *pitem;
	TCHAR *wcs = 0;
	zend_bool width_isnull, height_isnull;

	// Get function parameters

	//if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lz!|lls", &pwboParent, &zarray, &width, &height, &s, &s_len) == FAILURE)
	// ZEND_PARSE_PARAMETERS_START() takes two arguments minimal and maximal parameters count.
	ZEND_PARSE_PARAMETERS_START(2, 5)
		Z_PARAM_LONG(pwboParent)
		Z_PARAM_ZVAL_OR_NULL(zarray)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG_OR_NULL(width, width_isnull)
		Z_PARAM_LONG_OR_NULL(height, height_isnull)
		Z_PARAM_STRING_OR_NULL(s,s_len)

	ZEND_PARSE_PARAMETERS_END();

	if (!wbIsWBObj((void *)pwboParent, TRUE)){
		RETURN_NULL();
	}
	if (Z_TYPE_P(zarray) == IS_ARRAY)
	{

		target_hash = HASH_OF(zarray);
		if (!target_hash){
			RETURN_NULL();
		}
		nelem = zend_hash_num_elements(target_hash);
		zend_hash_internal_pointer_reset(target_hash);

		// Allocate memory for toolbar pointers

		pitem = emalloc(nelem * sizeof(PWBITEM));

		// Loop to read array items
		for (i = 0; i < nelem; i++)
		{

			if ((entry = zend_hash_get_current_data(target_hash)) == NULL)
			{
				wbError(TEXT("wb_create_toolbar"), MB_ICONWARNING, TEXT("Could not retrieve element %d from array in function"), i);
				efree(pitem);
				RETURN_NULL();
			}

			// Allocate memory for item description
			pitem[i] = emalloc(sizeof(WBITEM));

			switch (Z_TYPE_P(entry))
			{

			case IS_ARRAY: // Toolbar button
				parse_array(entry, "lssl", &pitem[i]->id, &pitem[i]->pszCaption, &pitem[i]->pszHint, &pitem[i]->index);
				//pitem[i]->pszCaption = Utf82WideChar((const char *)pitem[i]->pszCaption, 0);
				//pitem[i]->pszHint = Utf82WideChar((const char *)pitem[i]->pszHint, 0);

				pitem[i]->pszCaption = (LPCTSTR)pitem[i]->pszCaption; // LPCTSTR?
				pitem[i]->pszHint = (LPCTSTR)pitem[i]->pszHint; // LPCTSTR?

				break;

			case IS_NULL: // Separator
				pitem[i] = NULL;
				break;

			default:
				wbError(TEXT("wb_create_toolbar"), MB_ICONWARNING, TEXT("Invalid element type in array: must be an array or null in function"));
				efree(pitem);
				RETURN_NULL();
			}

			if (i < nelem - 1){
				zend_hash_move_forward(target_hash);
			}
		}

		wcs = Utf82WideChar(s, s_len);
		hImage = wbLoadImage(wcs, 0, 0);
		wbFree(wcs);
		if (!hImage)
		{
			wbError(TEXT("wb_create_toolbar"), MB_ICONWARNING, TEXT("%s is an invalid image file or has an unrecognizable format in function"), s);
		}
	}
	else
	{
		nelem = 0;
		pitem = NULL;
	}

	// Create toolbar and attach it to window

	l = (LONG_PTR)wbCreateToolbar((PWBOBJ)pwboParent, pitem, nelem, width, height, hImage);

	if (pitem){
		efree(pitem);
	}
	RETURN_LONG(l);
}

//------------------------------------------------------------------ END OF FILE
