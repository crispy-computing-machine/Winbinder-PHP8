/*******************************************************************************

 WINBINDER - The native Windows binding for PHP for PHP

 Copyright  Hypervisual - see LICENSE.TXT for details
 Author: Rubem Pechansky (https://github.com/crispy-computing-machine/Winbinder)

 ZEND wrapper for ListView control

*******************************************************************************/

//----------------------------------------------------------------- DEPENDENCIES

#include "phpwb.h"

//-------------------------------------------------------------------- CONSTANTS

#define MAXITEMSTR 1024

//----------------------------------------------------------- EXPORTED FUNCTIONS

ZEND_FUNCTION(wb_create_listview_item)
{
	zend_long pwbo, nitem, nimage;
	char *s;
	size_t s_len;
	int newitem;

	TCHAR *wcs = 0;

	// if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "llls", &pwbo, &nitem, &nimage, &s, &s_len) == FAILURE)
	// ZEND_PARSE_PARAMETERS_START() takes two arguments minimal and maximal parameters count.
	ZEND_PARSE_PARAMETERS_START(4, 4)
		Z_PARAM_LONG(pwbo)
		Z_PARAM_LONG(nitem)
		Z_PARAM_LONG(nimage)
		Z_PARAM_STRING(s,s_len)
	ZEND_PARSE_PARAMETERS_END();

	if (!wbIsWBObj((void *)pwbo, TRUE)){
		RETURN_NULL();
}
	wcs = Utf82WideChar(s, s_len);
	newitem = wbCreateListViewItem((PWBOBJ)pwbo, nitem, nimage, wcs);

	if (newitem == -1)
	{
		wbError(TEXT("wb_create_listview_item"), MB_ICONWARNING, TEXT("Cannot insert item #%ld ('%s') in ListView in function"), nitem, s);
		RETURN_LONG(-1);
		return;
	}
	else{
		RETURN_LONG(newitem);
		}
}

ZEND_FUNCTION(wb_set_listview_item_text)
{
	zend_long pwbo, item, sub;
	char *s;
	size_t s_len;
	TCHAR *wcs = 0;

	// if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "llls", &pwbo, &item, &sub, &s, &s_len) == FAILURE)
	// ZEND_PARSE_PARAMETERS_START() takes two arguments minimal and maximal parameters count.
	ZEND_PARSE_PARAMETERS_START(4, 4)
		Z_PARAM_LONG(pwbo)
		Z_PARAM_LONG(item)
		Z_PARAM_LONG(sub)
		Z_PARAM_STRING(s,s_len)
	ZEND_PARSE_PARAMETERS_END();

	if (!wbIsWBObj((void *)pwbo, TRUE)){
		RETURN_BOOL(FALSE);
	}
	wcs = Utf82WideChar(s, s_len); /// ahhhhhhh
	RETURN_BOOL(wbSetListViewItemText((PWBOBJ)pwbo, item, sub, wcs));
}

ZEND_FUNCTION(wb_set_listview_item_checked)
{
	zend_long pwbo, item, checked;

	// if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lll", &pwbo, &item, &checked) == FAILURE)
	// ZEND_PARSE_PARAMETERS_START() takes two arguments minimal and maximal parameters count.
	ZEND_PARSE_PARAMETERS_START(3, 3)
		Z_PARAM_LONG(pwbo)
		Z_PARAM_LONG(item)
		Z_PARAM_LONG(checked)
	ZEND_PARSE_PARAMETERS_END();

	if (!wbIsWBObj((void *)pwbo, TRUE)){
		RETURN_BOOL(FALSE);
	}

	RETURN_BOOL(wbSetListViewItemChecked((PWBOBJ)pwbo, item, checked));
}

/* If w is negative, calculate width automatically */

ZEND_FUNCTION(wb_create_listview_column)
{
	zend_long pwbo, ncol, w = 0, align = 0;
	char *s;
	size_t s_len;

	TCHAR *wcs = 0;

	// if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "llsll", &pwbo, &ncol, &s, &s_len, &w, &align) == FAILURE)
	// ZEND_PARSE_PARAMETERS_START() takes two arguments minimal and maximal parameters count.
	ZEND_PARSE_PARAMETERS_START(5, 5)
		Z_PARAM_LONG(pwbo)
		Z_PARAM_LONG(ncol)
		Z_PARAM_STRING(s,s_len)
		Z_PARAM_LONG(w)
		Z_PARAM_LONG(align)
	ZEND_PARSE_PARAMETERS_END();

	if (!wbIsWBObj((void *)pwbo, TRUE)){
		RETURN_BOOL(FALSE);
	}
	wcs = Utf82WideChar(s, s_len);
	RETURN_BOOL(wbCreateListViewColumn((PWBOBJ)pwbo, ncol, wcs, w, align));
}

/* Returns an array with the column widths of control pwbo */
/*
ZEND_FUNCTION(wb_get_listview_column_widths)
{
	int i, cols;
	LONG_PTR pwbo;
	int pwidths[MAX_LISTVIEWCOLS];

    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
	  "l", &pwbo) == FAILURE)
		return;

	if(!wbIsWBObj((void *)pwbo, TRUE))
		RETURN_NULL()

	// Build the array

	cols = wbGetListViewColumnWidths((PWBOBJ)pwbo, pwidths);
	array_init(return_value);
	for(i = 0; i < cols; i++)
		add_next_index_long(return_value, pwidths[i]);
}
*/

/*ZEND_FUNCTION(wb_set_listview_column_widths)
{
	int i, nelem;
	LONG_PTR pwbo;
	int pwidths[MAX_LISTVIEWCOLS];
	zval *array, *entry;
	HashTable *target_hash;

    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
	  "la", &pwbo, &array) == FAILURE)
		return;

	if(!wbIsWBObj((void *)pwbo, TRUE))
		RETURN_NULL()

	target_hash = HASH_OF(array);
	if(!target_hash)
		RETURN_NULL();

	nelem = zend_hash_num_elements(target_hash);
	zend_hash_internal_pointer_reset(target_hash);

	// Loop to read array items

	for(i = 0; i < nelem; i++) {
		if((entry = zend_hash_get_current_data(target_hash)) == NULL) {
			zend_error(E_WARNING, "%s(): Could not retrieve element %d from array",
			  get_active_function_name(), i);
			RETURN_NULL();
		}
		switch(Z_TYPE_P(entry)) {

			case IS_LONG:
				break;

			case IS_STRING:
			case IS_DOUBLE:
				convert_to_long_ex(entry);
				break;

			default:
				zend_error(E_WARNING, "Wrong data type in array in function %s()",
				  get_active_function_name());
				RETURN_NULL();
		}

		pwidths[i] = Z_LVAL_P(entry);
		zend_hash_move_forward(target_hash);
	}

	RETURN_BOOL(wbSetListViewColumnWidths((PWBOBJ)pwbo, pwidths));
}*/

/* Returns the number of columns of control pwbo */

ZEND_FUNCTION(wb_get_listview_columns)
{
	zend_long pwbo;

	// if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &pwbo) == FAILURE)
	// ZEND_PARSE_PARAMETERS_START() takes two arguments minimal and maximal parameters count.
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(pwbo)
	ZEND_PARSE_PARAMETERS_END();

	if (!wbIsWBObj((void *)pwbo, TRUE)){
		RETURN_NULL();
	}
	RETURN_LONG(wbGetListViewColumnWidths((PWBOBJ)pwbo, NULL));
}
/*
ZEND_FUNCTION(wb_set_listview_item_image)
{
	LONG_PTR pwbo, nitem, ncol, nimage;

    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
	  "llll", &pwbo, &nitem, &ncol, &nimage) == FAILURE)
		RETURN_NULL();

	if(!wbIsWBObj((void *)pwbo, TRUE))
		RETURN_NULL()

	RETURN_BOOL(wbSetListViewItemImage((PWBOBJ)pwbo, nitem, ncol, nimage));
}
*/

/* Return TRUE if the item's checkbox is checked */

ZEND_FUNCTION(wb_get_listview_item_checked)
{
	zend_long pwbo, item;

	//if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &pwbo, &item) == FAILURE)
	// ZEND_PARSE_PARAMETERS_START() takes two arguments minimal and maximal parameters count.
	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_LONG(pwbo)
		Z_PARAM_LONG(item)
	ZEND_PARSE_PARAMETERS_END();

	if (!wbIsWBObj((void *)pwbo, TRUE)){
		RETURN_BOOL(FALSE);
	}
	RETURN_BOOL(wbGetListViewItemChecked((PWBOBJ)pwbo, item));
}

ZEND_FUNCTION(wb_clear_listview_columns)
{
	zend_long pwbo;

	// if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &pwbo) == FAILURE)
	// ZEND_PARSE_PARAMETERS_START() takes two arguments minimal and maximal parameters count.
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(pwbo)
	ZEND_PARSE_PARAMETERS_END();


	if (!wbIsWBObj((void *)pwbo, TRUE)){
		RETURN_BOOL(FALSE);
	}
	RETURN_BOOL(wbClearListViewColumns((PWBOBJ)pwbo));
}

ZEND_FUNCTION(wb_select_listview_item)
{
	zend_long pwbo, nitem, state;

	//if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lll", &pwbo, &nitem, &state) == FAILURE)
	// ZEND_PARSE_PARAMETERS_START() takes two arguments minimal and maximal parameters count.
	ZEND_PARSE_PARAMETERS_START(3, 3)
		Z_PARAM_LONG(pwbo)
		Z_PARAM_LONG(nitem)
		Z_PARAM_LONG(state)
	ZEND_PARSE_PARAMETERS_END();

	if (!wbIsWBObj((void *)pwbo, TRUE)){
		RETURN_BOOL(FALSE);
	 }
	RETURN_BOOL(wbSelectListViewItem((PWBOBJ)pwbo, nitem, state));
}

/* Selects / deselects all items */

ZEND_FUNCTION(wb_select_all_listview_items)
{
	zend_long pwbo, state;

	// if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &pwbo, &state) == FAILURE)
	// ZEND_PARSE_PARAMETERS_START() takes two arguments minimal and maximal parameters count.
	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_LONG(pwbo)
		Z_PARAM_LONG(state)
	ZEND_PARSE_PARAMETERS_END();

	if (!wbIsWBObj((void *)pwbo, TRUE)){
		RETURN_BOOL(FALSE);
	 }
	RETURN_BOOL(wbSelectAllListViewItems((PWBOBJ)pwbo, state));
}

/* Returns an array of strings */

ZEND_FUNCTION(wb_get_listview_text)
{
	int ncols = 0, nitems = 0;
	zend_bool nitems_isnull, ncol_isnull;
	zend_long pwbo, nitem = -1, ncol = -1;
	TCHAR szItem[MAX_ITEM_STRING];

	char *str = 0;
	int len = 0;

	// if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l|ll", &pwbo, &nitem, &ncol) == FAILURE)
	// ZEND_PARSE_PARAMETERS_START() takes two arguments minimal and maximal parameters count.
	ZEND_PARSE_PARAMETERS_START(1, 3)
		Z_PARAM_LONG(pwbo)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG_OR_NULL(nitem, nitems_isnull)
		Z_PARAM_LONG_OR_NULL(ncol, ncol_isnull)
	ZEND_PARSE_PARAMETERS_END();

	if (!wbIsWBObj((void *)pwbo, TRUE)){
		RETURN_NULL();
	 }
	if (nitem >= 0)
	{

		if (ncol >= 0)
		{ // A single cell
			if (wbGetListViewItemText((PWBOBJ)pwbo, nitem, ncol, szItem, MAX_ITEM_STRING - 1)){
				RETURN_STRINGL(WideChar2Utf8(szItem, &len), len);
			}else{
				RETURN_STRING("");
			}
		}
		else
		{ // The entire row

			ncols = wbGetListViewColumnWidths((PWBOBJ)pwbo, NULL);

			array_init(return_value);
			for (ncol = 0; ncol < ncols; ncol++)
			{
				if (wbGetListViewItemText((PWBOBJ)pwbo, nitem, ncol, szItem, MAX_ITEM_STRING - 1))
				{
					str = WideChar2Utf8(szItem, &len);
					add_next_index_stringl(return_value, str, len);
				}
				else{
					add_next_index_stringl(return_value, "", 0);
				}
			}
		}
	}
	else
	{

		if (ncol >= 0)
		{ // The entire column

			nitems = wbGetListViewItemCount((PWBOBJ)pwbo);

			array_init(return_value);
			for (nitem = 0; nitem < nitems; nitem++)
			{
				if (wbGetListViewItemText((PWBOBJ)pwbo, nitem, ncol, szItem, MAX_ITEM_STRING - 1))
				{
					str = WideChar2Utf8(szItem, &len);
					add_next_index_stringl(return_value, str, len);
				}
				else{
					add_next_index_stringl(return_value, "", 0);
				}
			}
		}
		else
		{ // All cells

			nitems = wbGetListViewItemCount((PWBOBJ)pwbo);
			ncols = wbGetListViewColumnWidths((PWBOBJ)pwbo, NULL);

			array_init(return_value);
			for (nitem = 0; nitem < nitems; nitem++)
			{
				for (ncol = 0; ncol < ncols; ncol++)
				{
					if (wbGetListViewItemText((PWBOBJ)pwbo, nitem, ncol, szItem, MAX_ITEM_STRING - 1))
					{
						str = WideChar2Utf8(szItem, &len);
						add_next_index_stringl(return_value, str, len);
					}
					else{
						add_next_index_stringl(return_value, "", 0);
					}
				}
			}
		}
	}
}

//------------------------------------------------------------------ END OF FILE
