/*******************************************************************************

 WINBINDER - The native Windows binding for PHP

 Copyright  Hypervisual - see LICENSE.TXT for details
 Author: Rubem Pechansky (https://github.com/crispy-computing-machine/Winbinder)

 Tab control

*******************************************************************************/

//----------------------------------------------------------------- DEPENDENCIES

#include "wb.h"

#define WB_TAB_FLAG_CLOSABLE 0x00000001
#define WB_TAB_FLAG_PINNED 0x00000002

static BOOL wbTabGetItemData(PWBOBJ pwboTab, int nItem, LRESULT *pFlags)
{
	TC_ITEM tcitem = {0};

	if (!pwboTab || !pwboTab->hwnd || !IsWindow(pwboTab->hwnd))
		return FALSE;

	if (nItem < 0 || nItem >= TabCtrl_GetItemCount(pwboTab->hwnd))
		return FALSE;

	tcitem.mask = TCIF_PARAM;
	if (!TabCtrl_GetItem(pwboTab->hwnd, nItem, &tcitem))
		return FALSE;

	if (pFlags)
		*pFlags = tcitem.lParam;

	return TRUE;
}

static BOOL wbTabSetItemData(PWBOBJ pwboTab, int nItem, LRESULT flags)
{
	TC_ITEM tcitem = {0};

	if (!pwboTab || !pwboTab->hwnd || !IsWindow(pwboTab->hwnd))
		return FALSE;

	tcitem.mask = TCIF_PARAM;
	tcitem.lParam = flags;

	return TabCtrl_SetItem(pwboTab->hwnd, nItem, &tcitem);
}

static BOOL wbTabShouldNotify(PWBOBJ pwboTab, DWORD dwEvent)
{
	PWBOBJ pwboParent;

	if (!pwboTab || !pwboTab->parent)
		return FALSE;

	pwboParent = pwboTab->parent;
	if (!(pwboParent->style & WBC_NOTIFY))
		return FALSE;

	return BITTEST(pwboParent->lparam, dwEvent);
}

static UINT64 wbTabNotify(PWBOBJ pwboTab, DWORD dwEvent, LPARAM lParam1, LPARAM lParam2)
{
	if (!wbTabShouldNotify(pwboTab, dwEvent))
		return 0;

	if (pwboTab->pszCallBackFn && *pwboTab->pszCallBackFn)
		return wbCallUserFunction(pwboTab->pszCallBackFn, pwboTab->pszCallBackObj, pwboTab->parent, pwboTab, pwboTab->id, dwEvent, lParam1, lParam2);

	if (pwboTab->parent && pwboTab->parent->pszCallBackFn && *pwboTab->parent->pszCallBackFn)
		return wbCallUserFunction(pwboTab->parent->pszCallBackFn, pwboTab->parent->pszCallBackObj, pwboTab->parent, pwboTab, pwboTab->id, dwEvent, lParam1, lParam2);

	return 0;
}

//------------------------------------------------------------- PUBLIC FUNCTIONS

/* Create tabs according to the tab control title */

BOOL wbSetTabControlText(PWBOBJ pwboTab, LPCTSTR pszText)
{
	TCHAR *szTitle = _wcsdup(pszText);
	TCHAR *ptr = NULL;
	TCHAR *next = NULL;

	if (!pwboTab || !pwboTab->hwnd || !IsWindow(pwboTab->hwnd))
		return FALSE;

	if (!pszText || !*pszText)
		return FALSE;

	SendMessage(pwboTab->hwnd, TCM_DELETEALLITEMS, 0, 0);
	ptr = wcstok(szTitle, TEXT("\r\n,"), &next);
	while (ptr)
	{
		wbCreateTabItem(pwboTab, ptr);
		ptr = wcstok(NULL, TEXT("\r\n,"), &next);
	}
	return TRUE;
}

/* Create a tab in a tab control */

BOOL wbCreateTabItem(PWBOBJ pwboTab, LPCTSTR pszItem)
{
	TC_ITEM tcitem;
	int nItems;
	PTABDATA pTabData;
	UINT64 i;
	RECT rc;

	if (!pwboTab || !pwboTab->hwnd || !IsWindow(pwboTab->hwnd))
		return FALSE;

	if (!pszItem || !*pszItem)
		return FALSE;

	tcitem.mask = TCIF_TEXT | TCIF_PARAM;
	tcitem.cchTextMax = 0;
	tcitem.iImage = 0;
	tcitem.lParam = WB_TAB_FLAG_CLOSABLE;
	tcitem.pszText = (LPTSTR)pszItem;

	// Create the tab

	nItems = SendMessage(pwboTab->hwnd, TCM_GETITEMCOUNT, 0, 0);
	SendMessage(pwboTab->hwnd, TCM_INSERTITEM, nItems, (LPARAM)&tcitem);

	// Create the container page

	pTabData = (PTABDATA)pwboTab->lparam;

	pTabData->page[pTabData->nPages].hwnd = CreateWindow(
		TAB_PAGE_CLASS,
		TEXT(""),
		//			WS_CHILD | WS_TABSTOP | DS_3DLOOK | DS_SETFONT | DS_CONTROL,
		WS_CHILD,
		0, 0, CW_USEDEFAULT, CW_USEDEFAULT,
		pwboTab->hwnd,
		NULL,
		hAppInstance,
		NULL);

	GetClientRect(pwboTab->hwnd, &rc);
	TabCtrl_AdjustRect(pwboTab->hwnd, FALSE, &rc);

	// Adjust page size and visibility

	SetWindowPos(pTabData->page[pTabData->nPages].hwnd, NULL,
				 rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top,
				 SWP_NOACTIVATE | SWP_NOCOPYBITS | SWP_NOOWNERZORDER | SWP_NOREDRAW);

	ShowWindow(pTabData->page[pTabData->nPages].hwnd, pTabData->nPages == 0 ? SW_SHOW : SW_HIDE);

	// Requests all existing child controls that belong to this tab / page

	for (i = 0; i < pTabData->nCtrls; i++)
	{
		if (pTabData->ctrl[i].nTab == pTabData->nPages)
		{
			SetParent(pTabData->ctrl[i].hwnd, pTabData->page[pTabData->nPages].hwnd);
		}
	}

	//	if(!pTabData->nPages)
	//		SetFocus(pTabData->page[0].hwnd);

	pTabData->nPages++;

	return TRUE;
}

/* Selects a tab, showing or displaying the appropriate page and controls */

BOOL wbSelectTab(PWBOBJ pwboTab, int nItem)
{
	int nSelTab = 0;
	UINT64 i;
	PTABDATA pTabData;

	if (!pwboTab || !pwboTab->hwnd || !IsWindow(pwboTab->hwnd))
		return FALSE;

	nSelTab = TabCtrl_GetCurSel(pwboTab->hwnd);

	if (nSelTab != nItem)
		TabCtrl_SetCurSel(pwboTab->hwnd, nItem);

	nSelTab = TabCtrl_GetCurSel(pwboTab->hwnd); // New tab index

	// Show / hide pages

	pTabData = (PTABDATA)pwboTab->lparam;
	if (!pTabData)
		return FALSE;
	for (i = 0; i < pTabData->nPages; i++)
		ShowWindow(pTabData->page[i].hwnd, (int)i == nSelTab ? SW_SHOW : SW_HIDE);

	return TRUE;
}

BOOL wbSetTabClosable(PWBOBJ pwboTab, int nItem, BOOL bClosable)
{
	LRESULT flags;

	if (!wbTabGetItemData(pwboTab, nItem, &flags))
		return FALSE;

	if (bClosable)
		flags |= WB_TAB_FLAG_CLOSABLE;
	else
		flags &= ~WB_TAB_FLAG_CLOSABLE;

	return wbTabSetItemData(pwboTab, nItem, flags);
}

BOOL wbMoveTab(PWBOBJ pwboTab, int nFrom, int nTo, BOOL bNotify)
{
	PTABDATA pTabData;
	TC_ITEM tcitem = {0};
	TCHAR szText[MAX_ITEM_STRING];
	int nCount;
	int nSel;
	HWND hPage;
	UINT64 i;
	BYTE oldIndex;

	if (!pwboTab || !pwboTab->hwnd || !IsWindow(pwboTab->hwnd))
		return FALSE;

	nCount = TabCtrl_GetItemCount(pwboTab->hwnd);
	if (nFrom < 0 || nTo < 0 || nFrom >= nCount || nTo >= nCount)
		return FALSE;

	if (nFrom == nTo)
		return TRUE;

	pTabData = (PTABDATA)pwboTab->lparam;
	if (!pTabData)
		return FALSE;

	tcitem.mask = TCIF_TEXT | TCIF_IMAGE | TCIF_PARAM;
	tcitem.pszText = szText;
	tcitem.cchTextMax = NUMITEMS(szText);
	if (!TabCtrl_GetItem(pwboTab->hwnd, nFrom, &tcitem))
		return FALSE;

	if (!TabCtrl_DeleteItem(pwboTab->hwnd, nFrom))
		return FALSE;

	if (nFrom < nTo)
		nTo--;

	if (TabCtrl_InsertItem(pwboTab->hwnd, nTo, &tcitem) < 0)
		return FALSE;

	hPage = pTabData->page[nFrom].hwnd;
	if (nFrom < nTo)
	{
		for (i = nFrom; i < (UINT64)nTo; i++)
			pTabData->page[i].hwnd = pTabData->page[i + 1].hwnd;
	}
	else
	{
		for (i = nFrom; i > (UINT64)nTo; i--)
			pTabData->page[i].hwnd = pTabData->page[i - 1].hwnd;
	}
	pTabData->page[nTo].hwnd = hPage;

	for (i = 0; i < pTabData->nCtrls; i++)
	{
		oldIndex = pTabData->ctrl[i].nTab;
		if (oldIndex == nFrom)
			pTabData->ctrl[i].nTab = (BYTE)nTo;
		else if (nFrom < nTo && oldIndex > nFrom && oldIndex <= nTo)
			pTabData->ctrl[i].nTab--;
		else if (nFrom > nTo && oldIndex >= nTo && oldIndex < nFrom)
			pTabData->ctrl[i].nTab++;
	}

	nSel = TabCtrl_GetCurSel(pwboTab->hwnd);
	if (nSel >= 0)
		wbSelectTab(pwboTab, nSel);

	if (bNotify)
		wbTabNotify(pwboTab, WBC_TAB_REORDERED, nFrom, nTo);

	return TRUE;
}

BOOL wbPinTab(PWBOBJ pwboTab, int nItem, BOOL bPinned, BOOL bNotify)
{
	LRESULT flags;
	int nCount;
	int nTarget;
	int i;
	BOOL bMoved = FALSE;

	if (!wbTabGetItemData(pwboTab, nItem, &flags))
		return FALSE;

	if (bPinned)
		flags |= WB_TAB_FLAG_PINNED;
	else
		flags &= ~WB_TAB_FLAG_PINNED;

	if (!wbTabSetItemData(pwboTab, nItem, flags))
		return FALSE;

	nCount = TabCtrl_GetItemCount(pwboTab->hwnd);
	nTarget = nItem;

	if (bPinned)
	{
		nTarget = 0;
		for (i = 0; i < nCount; i++)
		{
			LRESULT f = 0;
			if (i == nItem)
				continue;
			if (!wbTabGetItemData(pwboTab, i, &f))
				continue;
			if (f & WB_TAB_FLAG_PINNED)
				nTarget = i + 1;
		}
	}
	else
	{
		nTarget = nCount - 1;
		for (i = 0; i < nCount; i++)
		{
			LRESULT f = 0;
			if (i == nItem)
				continue;
			if (!wbTabGetItemData(pwboTab, i, &f))
				continue;
			if (!(f & WB_TAB_FLAG_PINNED))
			{
				nTarget = i;
				break;
			}
		}
	}

	if (nTarget != nItem)
		bMoved = wbMoveTab(pwboTab, nItem, nTarget, FALSE);

	if (bNotify)
		wbTabNotify(pwboTab, WBC_TAB_PINNED, bMoved ? nTarget : nItem, bPinned);

	return TRUE;
}

BOOL wbCloseTab(PWBOBJ pwboTab, int nItem, BOOL bNotify)
{
	PTABDATA pTabData;
	LRESULT flags = 0;
	int nCount;
	int nSel;
	int nNextSel;
	UINT64 i;

	if (!pwboTab || !pwboTab->hwnd || !IsWindow(pwboTab->hwnd))
		return FALSE;

	nCount = TabCtrl_GetItemCount(pwboTab->hwnd);
	if (nItem < 0 || nItem >= nCount)
		return FALSE;

	if (!wbTabGetItemData(pwboTab, nItem, &flags))
		return FALSE;

	if (!(flags & WB_TAB_FLAG_CLOSABLE))
		return FALSE;

	if (bNotify && wbTabNotify(pwboTab, WBC_TAB_CLOSING, nItem, nCount))
		return FALSE;

	pTabData = (PTABDATA)pwboTab->lparam;
	if (!pTabData)
		return FALSE;

	nSel = TabCtrl_GetCurSel(pwboTab->hwnd);
	if (!TabCtrl_DeleteItem(pwboTab->hwnd, nItem))
		return FALSE;

	DestroyWindow(pTabData->page[nItem].hwnd);
	for (i = nItem; i + 1 < pTabData->nPages; i++)
		pTabData->page[i].hwnd = pTabData->page[i + 1].hwnd;
	if (pTabData->nPages)
		pTabData->nPages--;

	for (i = 0; i < pTabData->nCtrls; i++)
	{
		if (pTabData->ctrl[i].nTab == nItem)
			pTabData->ctrl[i].nTab = 0;
		else if (pTabData->ctrl[i].nTab > nItem)
			pTabData->ctrl[i].nTab--;
	}

	nCount = TabCtrl_GetItemCount(pwboTab->hwnd);
	if (nCount > 0)
	{
		nNextSel = (nSel > nItem) ? nSel - 1 : nSel;
		if (nNextSel >= nCount)
			nNextSel = nCount - 1;
		if (nNextSel < 0)
			nNextSel = 0;
		wbSelectTab(pwboTab, nNextSel);
	}

	if (bNotify)
		wbTabNotify(pwboTab, WBC_TAB_CLOSED, nItem, nCount);

	return TRUE;
}

/* Sets an image list for tab control */
BOOL wbCreateTabControlImageList(PWBOBJ pwbo, HBITMAP hbmImage, int nImages, COLORREF clTransparent)
{
	HBITMAP hbmMask;
	static HIMAGELIST hi;
	BITMAP bm;

	if (!pwbo || !pwbo->hwnd || !IsWindow(pwbo->hwnd))
		return FALSE;

	if (hbmImage && nImages)
	{

		GetObject(hbmImage, sizeof(BITMAP), (LPSTR)&bm);
		if ((bm.bmWidth == 0) | (bm.bmHeight == 0))
			return FALSE;

		nImages = MAX(1, MIN(nImages, MIN(bm.bmWidth, MAX_IMAGELIST_IMAGES)));

		if (clTransparent != NOCOLOR)
		{
			hbmMask = wbCreateMask(hbmImage, clTransparent);
			hi = ImageList_Create(bm.bmWidth / nImages, bm.bmHeight, ILC_COLORDDB | ILC_MASK, nImages, 0);
			ImageList_Add(hi, hbmImage, hbmMask);
			TabCtrl_SetImageList(pwbo->hwnd, hi);
			DeleteObject(hbmMask);
		}
		else
		{
			hi = ImageList_Create(bm.bmWidth / nImages, bm.bmHeight, ILC_COLORDDB, nImages, 0);
			ImageList_Add(hi, hbmImage, NULL);
			TabCtrl_SetImageList(pwbo->hwnd, hi);
		}
	}
	else
	{
		TabCtrl_SetImageList(pwbo->hwnd, NULL);
		ImageList_Destroy(hi);
	}
	return TRUE;
}

/* Change the two images of the specified node */

BOOL wbSetTabControlItemImages(PWBOBJ pwbo, int item, int nImageIndex)
{
	TC_ITEM tabItemToUpdate = {0};
	LONG_PTR lStyle;

	if (!pwbo || !pwbo->hwnd || !IsWindow(pwbo->hwnd))
		return FALSE;
	if (item < 0 || item >= TabCtrl_GetItemCount(pwbo->hwnd))
		return FALSE;

	tabItemToUpdate.mask = TCIF_IMAGE;
	tabItemToUpdate.iImage = nImageIndex; // image index

	if (nImageIndex >= 0)
	{
		lStyle = GetWindowLongPtr(pwbo->hwnd, GWL_STYLE);
		if ((lStyle & (TCS_FIXEDWIDTH | TCS_FORCEICONLEFT)) != (TCS_FIXEDWIDTH | TCS_FORCEICONLEFT))
		{
			SetWindowLongPtr(pwbo->hwnd, GWL_STYLE, lStyle | TCS_FIXEDWIDTH | TCS_FORCEICONLEFT);
			SetWindowPos(pwbo->hwnd, NULL, 0, 0, 0, 0,
				SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
		}
	}

	TabCtrl_SetItem(pwbo->hwnd, item, &tabItemToUpdate);

	return TRUE;
}

/*LRESULT CALLBACK TabProc(HWND hwnd, UINT64 msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg) {

		case WM_COMMAND:		// Passes commands to parent window
		case WM_NOTIFY:
		case WM_HSCROLL:		// For scroll bars
		case WM_VSCROLL:
			{
				HWND hwndParent = GetParent(hwnd);

				if(hwndParent)
					SendMessage(hwndParent, msg, wParam, lParam);
			}
			break;
	}
	return CallWindowProc(lpfnTabProcOld, hwnd, msg, wParam, lParam);
}*/

//------------------------------------------- FUNCTIONS PUBLIC TO WINBINDER ONLY

// Insert the control data in its parent's TABDATA structure

BOOL RegisterControlInTab(PWBOBJ pwboParent, PWBOBJ pwbo, UINT64 id, UINT64 nTab)
{
	PWBOBJ pwboTab;
	PTABDATA pTabData;

	if (pwboParent == pwbo)
		return FALSE;

	pwboTab = (PWBOBJ)GetWindowLongPtr(pwboParent->hwnd, GWLP_USERDATA);
	if (!pwboTab)
		return FALSE;

	if (pwboTab == pwbo)
		return FALSE;

	pTabData = (PTABDATA)pwboTab->lparam;
	if (!pTabData)
		return FALSE;

	pTabData->ctrl[pTabData->nCtrls].hwnd = pwbo->hwnd;
	pTabData->ctrl[pTabData->nCtrls].id = id;
	pTabData->ctrl[pTabData->nCtrls].nTab = MIN(MIN((UINT64)nTab, pTabData->nPages - 1), MAX_TABS);
	pTabData->nCtrls++;

	return TRUE;
}

// Look for children tab controls and assign the handler to them

PWBOBJ AssignHandlerToTabs(HWND hwndParent, LPDWORD pszObj, LPCTSTR pszHandler)
{
	HWND hChild = NULL;
	PWBOBJ poChild = NULL;
	HWND hTabPage;

	// Find the first tab control, if any

	while ((hChild = FindWindowEx(hwndParent, hChild, WC_TABCONTROL, NULL)) != NULL)
	{

		// Found a tab control: assign the handler to it

		poChild = wbGetWBObj(hChild);
		if (!poChild)
			break;

		// An object name was passed

		if (pszObj != NULL)
		{
			poChild->pszCallBackObj = pszObj;
		}
		else
			poChild->pszCallBackObj = NULL;

		poChild->pszCallBackFn = (LPTSTR)pszHandler;

		//		printf("> %08X\n", poChild);

		// Now, find its child window (must be a TAB_PAGE_CLASS)

		hTabPage = FindWindowEx(poChild->hwnd, NULL, TAB_PAGE_CLASS, NULL);
		if (!hTabPage)
			return NULL;

		// ...and call this function recursively

		AssignHandlerToTabs(hTabPage, pszObj, pszHandler);
	}
	return poChild;
}

//------------------------------------------------------------------ END OF FILE
