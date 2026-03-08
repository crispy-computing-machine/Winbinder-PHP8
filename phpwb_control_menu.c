/*******************************************************************************

 WINBINDER - The native Windows binding for PHP for PHP

 Copyright  Hypervisual - see LICENSE.TXT for details
 Author: Rubem Pechansky (https://github.com/crispy-computing-machine/Winbinder)

 ZEND wrapper for menu functions

*******************************************************************************/

//----------------------------------------------------------------- DEPENDENCIES

#include "phpwb.h"

//------------------------------------------------------------- LOCAL FUNCTIONS

static BOOL wbBuildMenuItemsFromArray(zval *zarray, PWBITEM **ppitem, int *pnitems, ACCEL accel[], int *pnaccel)
{
	int i, nelem;
	zval *entry = NULL;
	HashTable *target_hash;
	PWBITEM *pitem = NULL;
	char *str_accel = NULL;
	DWORD dwacc;
	int naccel = 0;

	if (Z_TYPE_P(zarray) != IS_ARRAY)
	{
		*ppitem = NULL;
		*pnitems = 0;
		*pnaccel = 0;
		return TRUE;
	}

	target_hash = HASH_OF(zarray);
	if (!target_hash)
		return FALSE;

	nelem = zend_hash_num_elements(target_hash);
	zend_hash_internal_pointer_reset(target_hash);

	pitem = emalloc(nelem * sizeof(PWBITEM));

	for (i = 0; i < nelem; i++)
	{
		if ((entry = zend_hash_get_current_data(target_hash)) == NULL)
		{
			wbError(TEXT("wb_create_menu"), MB_ICONWARNING, TEXT("Could not retrieve element %d from array in function"), i);
			efree(pitem);
			return FALSE;
		}

		pitem[i] = emalloc(sizeof(WBITEM));
		ZeroMemory(pitem[i], sizeof(WBITEM));

		switch (Z_TYPE_P(entry))
		{
		case IS_ARRAY:
			parse_array(entry, "lssssl", &pitem[i]->id, &pitem[i]->pszCaption, &pitem[i]->pszHint, &pitem[i]->pszImage, &str_accel, &pitem[i]->lparam);
			pitem[i]->pszCaption = Utf82WideChar((const char *)pitem[i]->pszCaption, 0);
			pitem[i]->pszHint = Utf82WideChar((const char *)pitem[i]->pszHint, 0);
			pitem[i]->pszImage = Utf82WideChar((const char *)pitem[i]->pszImage, 0);

			if (str_accel && *str_accel && naccel < MAX_ACCELS)
			{
				dwacc = wbMakeAccelFromString(str_accel);
				accel[naccel].key = LOWORD(dwacc);
				accel[naccel].fVirt = (BYTE)HIWORD(dwacc);
				accel[naccel].cmd = pitem[i]->id;
				naccel++;
			}
			break;

		case IS_NULL:
			efree(pitem[i]);
			pitem[i] = NULL;
			break;

		case IS_STRING:
			pitem[i]->id = 0;
			pitem[i]->index = 0;
			pitem[i]->pszCaption = Utf82WideChar(Z_STRVAL_P(entry), Z_STRLEN_P(entry));
			pitem[i]->pszHint = NULL;
			pitem[i]->pszImage = NULL;
			break;

		default:
			efree(pitem[i]);
			pitem[i] = NULL;
			break;
		}

		if (i < nelem - 1)
			zend_hash_move_forward(target_hash);
	}

	*ppitem = pitem;
	*pnitems = nelem;
	*pnaccel = naccel;
	return TRUE;
}


static void wbFreeBuiltMenuItems(PWBITEM *pitem, int nitems)
{
	int i;
	if (!pitem)
		return;
	for (i = 0; i < nitems; i++)
	{
		if (!pitem[i])
			continue;
		if (pitem[i]->pszCaption)
			efree((void *)pitem[i]->pszCaption);
		if (pitem[i]->pszHint)
			efree((void *)pitem[i]->pszHint);
		if (pitem[i]->pszImage)
			efree((void *)pitem[i]->pszImage);
		efree(pitem[i]);
	}
	efree(pitem);
}

//----------------------------------------------------------- EXPORTED FUNCTIONS

ZEND_FUNCTION(wb_create_menu)
{
	zval *zarray = NULL;
	zend_long pwboParent;
	PWBITEM *pitem = NULL;
	LONG_PTR l;
	ACCEL accel[MAX_ACCELS];
	int nitems = 0;
	int naccel = 0;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_LONG(pwboParent)
		Z_PARAM_ZVAL(zarray)
	ZEND_PARSE_PARAMETERS_END();

	if (!wbIsWBObj((void *)pwboParent, TRUE))
		RETURN_NULL();

	if (!wbBuildMenuItemsFromArray(zarray, &pitem, &nitems, accel, &naccel))
		RETURN_NULL();

	wbSetAccelerators((PWBOBJ)pwboParent, accel, naccel);

	l = (LONG_PTR)wbCreateMenu((PWBOBJ)pwboParent, pitem, nitems);

	wbFreeBuiltMenuItems(pitem, nitems);
	RETURN_LONG(l);
}

ZEND_FUNCTION(wb_create_popup_menu)
{
	zval *zarray = NULL;
	PWBITEM *pitem = NULL;
	LONG_PTR l;
	ACCEL accel[MAX_ACCELS];
	int nitems = 0;
	int naccel = 0;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(zarray)
	ZEND_PARSE_PARAMETERS_END();

	if (!wbBuildMenuItemsFromArray(zarray, &pitem, &nitems, accel, &naccel))
		RETURN_NULL();

	l = (LONG_PTR)wbCreatePopupMenu(pitem, nitems);

	wbFreeBuiltMenuItems(pitem, nitems);
	RETURN_LONG(l);
}

ZEND_FUNCTION(wb_track_popup_menu)
{
	zend_long pwboMenu, pwboParent;
	zend_long x = -1, y = -1;
	zend_long flags = 0;

	ZEND_PARSE_PARAMETERS_START(2, 5)
		Z_PARAM_LONG(pwboMenu)
		Z_PARAM_LONG(pwboParent)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(x)
		Z_PARAM_LONG(y)
		Z_PARAM_LONG(flags)
	ZEND_PARSE_PARAMETERS_END();

	RETURN_LONG(wbTrackPopupMenu((PWBOBJ)pwboMenu, (PWBOBJ)pwboParent, (int)x, (int)y, (DWORD)flags));
}

ZEND_FUNCTION(wb_get_menu_item_checked)
{
	zend_long id;
	zend_long pwbo;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_LONG(pwbo)
		Z_PARAM_LONG(id)
	ZEND_PARSE_PARAMETERS_END();

	((PWBOBJ)pwbo)->id = id;

	RETURN_BOOL(wbGetMenuItemChecked((PWBOBJ)pwbo));
}

ZEND_FUNCTION(wb_get_menu_item_enabled)
{
	zend_long id;
	zend_long pwbo;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_LONG(pwbo)
		Z_PARAM_LONG(id)
	ZEND_PARSE_PARAMETERS_END();

	((PWBOBJ)pwbo)->id = id;

	RETURN_BOOL(wbGetEnabled((PWBOBJ)pwbo));
}

ZEND_FUNCTION(wb_set_menu_item_enabled)
{
	zend_long id, b;
	zend_long pwbo;

	ZEND_PARSE_PARAMETERS_START(3, 3)
		Z_PARAM_LONG(pwbo)
		Z_PARAM_LONG(id)
		Z_PARAM_LONG(b)
	ZEND_PARSE_PARAMETERS_END();

	((PWBOBJ)pwbo)->id = id;

	RETURN_BOOL(wbSetEnabled((PWBOBJ)pwbo, b));
}

ZEND_FUNCTION(wb_set_menu_item_checked)
{
	zend_long id, b;
	zend_long pwbo;

	ZEND_PARSE_PARAMETERS_START(3, 3)
		Z_PARAM_LONG(pwbo)
		Z_PARAM_LONG(id)
		Z_PARAM_LONG(b)
	ZEND_PARSE_PARAMETERS_END();

	((PWBOBJ)pwbo)->id = id;

	RETURN_BOOL(wbSetMenuItemChecked((PWBOBJ)pwbo, b));
}

/*
	Insert a bullet
	NOTE: state is not used, must be 1
*/

ZEND_FUNCTION(wb_set_menu_item_selected)
{
	zend_long pwbo, id, state;

	ZEND_PARSE_PARAMETERS_START(3, 3)
		Z_PARAM_LONG(pwbo)
		Z_PARAM_LONG(id)
		Z_PARAM_LONG(state)
	ZEND_PARSE_PARAMETERS_END();

	((PWBOBJ)pwbo)->id = id;

	RETURN_BOOL(wbSetMenuItemSelected((PWBOBJ)pwbo));
}

ZEND_FUNCTION(wb_set_menu_item_image)
{
	zend_long id, handle;
	zend_long pwbo;

	ZEND_PARSE_PARAMETERS_START(3, 3)
		Z_PARAM_LONG(pwbo)
		Z_PARAM_LONG(id)
		Z_PARAM_LONG(handle)
	ZEND_PARSE_PARAMETERS_END();

	((PWBOBJ)pwbo)->id = id;

	RETURN_BOOL(wbSetMenuItemImage((PWBOBJ)pwbo, (HANDLE)handle));
}

//------------------------------------------------------------------ END OF FILE
