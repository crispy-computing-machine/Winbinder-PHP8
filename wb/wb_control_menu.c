/*******************************************************************************

 WINBINDER - The native Windows binding for PHP

 Copyright  Hypervisual - see LICENSE.TXT for details
 Author: Rubem Pechansky (https://github.com/crispy-computing-machine/Winbinder)

 Menu functions

*******************************************************************************/

//----------------------------------------------------------------- DEPENDENCIES

#include "wb.h"

//------------------------------------------------------------- LOCAL FUNCTIONS

static HMENU wbBuildMenuFromItems(PWBITEM pitem[], int nItems, BOOL bPopup)
{
	int i;
	HMENU hMenu, hPopup = NULL;
	LPCTSTR pszLastPopup = NULL;

	hMenu = bPopup ? CreatePopupMenu() : CreateMenu();
	if (!hMenu)
		return NULL;

	for (i = 0; i < nItems; i++)
	{
		if (!pitem[i])
		{
			if (hPopup)
				AppendMenu(hPopup, MF_SEPARATOR, 0, NULL);
		}
		else if (!pitem[i]->id)
		{
			if (pitem[i]->pszCaption && *pitem[i]->pszCaption)
			{
				if (hPopup && pszLastPopup)
					AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hPopup, pszLastPopup);
				hPopup = CreatePopupMenu();
				pszLastPopup = pitem[i]->pszCaption;
			}
			else if (hPopup)
			{
				AppendMenu(hPopup, MF_SEPARATOR, 0, NULL);
			}
		}
		else
		{
			if (pitem[i]->pszCaption && *pitem[i]->pszCaption)
			{
				HMENU hTarget = hPopup ? hPopup : hMenu;
				AppendMenu(hTarget, MF_STRING | ((pitem[i]->lparam & WBC_DISABLED) ? MF_GRAYED : 0), pitem[i]->id, pitem[i]->pszCaption);
				if (pitem[i]->pszImage && *pitem[i]->pszImage)
				{
					HBITMAP hImage = wbLoadImage(pitem[i]->pszImage, 0, 0);
					if (hImage)
						SetMenuItemBitmaps(hTarget, pitem[i]->id, MF_BYCOMMAND, hImage, hImage);
				}
			}
		}
	}

	if (hPopup && pszLastPopup)
		AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hPopup, pszLastPopup);

	return hMenu;
}

static PWBOBJ wbCreateMenuObject(PWBOBJ pwboParent, HMENU hMenu)
{
	PWBOBJ pwbo;
	MENUITEMINFO mi;

	if (!hMenu)
		return NULL;

	pwbo = wbMalloc(sizeof(WBOBJ));
	pwbo->hwnd = (HWND)hMenu;
	pwbo->id = 0;
	pwbo->uClass = Menu;
	pwbo->item = -1;
	pwbo->subitem = -1;
	pwbo->style = 0;
	pwbo->pszCallBackFn = NULL;
	pwbo->pszCallBackObj = NULL;
	pwbo->lparam = 0;
	pwbo->parent = pwboParent;

	ZeroMemory(&mi, sizeof(MENUITEMINFO));
	mi.cbSize = sizeof(MENUITEMINFO);
	mi.fMask = MIIM_DATA;
	mi.dwItemData = (DWORD_PTR)pwbo;
	SetMenuItemInfo((HMENU)pwbo->hwnd, 0, TRUE, &mi);

	return pwbo;
}

//----------------------------------------------------------- EXPORTED FUNCTIONS

/*
 Receive an array of WBITEM items
 Today it shows only the "checked" image,
 with 13 x 13 bitmaps only. Whites get transparent, colors are faded.
 Solution: use owner-drawn menus
*/

PWBOBJ wbCreateMenu(PWBOBJ pwboParent, PWBITEM pitem[], int nItems)
{
	HMENU hMenu;
	PWBOBJ pwbo;

	if (!pwboParent || !pwboParent->hwnd || !IsWindow(pwboParent->hwnd))
		return NULL;

	hMenu = wbBuildMenuFromItems(pitem, nItems, FALSE);
	if (!hMenu)
		return NULL;

	pwbo = wbCreateMenuObject(pwboParent, hMenu);
	if (!pwbo)
	{
		DestroyMenu(hMenu);
		return NULL;
	}

	SetMenu(pwboParent->hwnd, hMenu);
	return pwbo;
}

PWBOBJ wbCreatePopupMenu(PWBITEM pitem[], int nItems)
{
	HMENU hMenu;

	hMenu = wbBuildMenuFromItems(pitem, nItems, TRUE);
	if (!hMenu)
		return NULL;

	return wbCreateMenuObject(NULL, hMenu);
}

DWORD wbTrackPopupMenu(PWBOBJ pwboMenu, PWBOBJ pwboParent, int xPos, int yPos, DWORD dwFlags)
{
	POINT pt;
	HWND hwndParent;
	DWORD dwRet;

	if (!pwboMenu || !pwboMenu->hwnd || !IsMenu((HMENU)pwboMenu->hwnd))
		return 0;
	if (!pwboParent || !pwboParent->hwnd || !IsWindow(pwboParent->hwnd))
		return 0;

	hwndParent = pwboParent->hwnd;
	pt.x = xPos;
	pt.y = yPos;
	if (xPos < 0 || yPos < 0)
	{
		GetCursorPos(&pt);
	}
	else
	{
		ClientToScreen(hwndParent, &pt);
	}

	SetForegroundWindow(hwndParent);
	dwRet = TrackPopupMenu((HMENU)pwboMenu->hwnd,
		dwFlags | TPM_LEFTBUTTON | TPM_RIGHTBUTTON | TPM_RETURNCMD,
		pt.x,
		pt.y,
		0,
		hwndParent,
		NULL);
	PostMessage(hwndParent, WM_NULL, 0, 0);

	if (dwRet)
	{
		PWBOBJ pwbobj = wbGetWBObj(hwndParent);
		if (pwbobj && pwbobj->pszCallBackFn && *pwbobj->pszCallBackFn)
			wbCallUserFunction(pwbobj->pszCallBackFn, pwbobj->pszCallBackObj, pwbobj, pwbobj, dwRet, 0, 0, 0);
	}

	return dwRet;
}

/* Sest the text of a menu item */

BOOL wbSetMenuItemText(PWBOBJ pwbo, LPCTSTR pszText)
{
	if (!pwbo || !pwbo->hwnd || !IsMenu((HMENU)pwbo->hwnd))
		return FALSE;

	if (pszText && *pszText)
		return ModifyMenu((HMENU)pwbo->hwnd, pwbo->id, MF_BYCOMMAND | MF_STRING, pwbo->id, pszText);
	else
		return FALSE;
}

BOOL wbGetMenuItemChecked(PWBOBJ pwbo)
{
	MENUITEMINFO mi;

	if (!pwbo || !pwbo->hwnd || !IsMenu((HMENU)pwbo->hwnd))
		return FALSE;

	mi.cbSize = sizeof(MENUITEMINFO);
	mi.fMask = MIIM_STATE;

	GetMenuItemInfo((HMENU)pwbo->hwnd, pwbo->id, FALSE, &mi);
	return mi.fState ? TRUE : FALSE;
}

BOOL wbSetMenuItemChecked(PWBOBJ pwbo, BOOL bState)
{
	if (!pwbo || !pwbo->hwnd || !IsMenu((HMENU)pwbo->hwnd))
		return FALSE;

	return (CheckMenuItem((HMENU)pwbo->hwnd, pwbo->id, bState ? MF_CHECKED : MF_UNCHECKED) != 0xFFFFFFFF);
}

BOOL wbSetMenuItemSelected(PWBOBJ pwbo)
{
	if (!pwbo || !pwbo->hwnd || !IsMenu((HMENU)pwbo->hwnd))
		return FALSE;

	return CheckMenuRadioItem((HMENU)pwbo->hwnd, pwbo->id, pwbo->id, pwbo->id, MF_BYCOMMAND);
}

/* Insert an image on a menu

 Today it shows only the "checked" image,
 with 13 x 13 bitmaps only. Whites get transparent, colors are faded.
 Solution: use owner-drawn menus
*/

BOOL wbSetMenuItemImage(PWBOBJ pwbo, HANDLE hImage)
{
	if (!pwbo || !pwbo->hwnd || !IsMenu((HMENU)pwbo->hwnd))
		return FALSE;

	if (hImage)
		return SetMenuItemBitmaps((HMENU)pwbo->hwnd, pwbo->id, MF_BYCOMMAND, (HBITMAP)hImage, (HBITMAP)hImage);
	else
		return FALSE;
}

//------------------------------------------------------------------ END OF FILE
