/*******************************************************************************

 WINBINDER - The native Windows binding for PHP

 Copyright  Hypervisual - see LICENSE.TXT for details
 Author: Rubem Pechansky (https://github.com/crispy-computing-machine/Winbinder)

 Toolbar control

*******************************************************************************/

//----------------------------------------------------------------- DEPENDENCIES

#include "wb.h"

//---------------------------------------------------------- FUNCTION PROTOTYPES

// Static

static HWND CreateToolbar(HWND hwndParent, int nButtons, int nBtnWidth, int nBtnHeight, HBITMAP hbm);
static BOOL CreateToolbarButton(HWND hwnd, int id, int nIndex, LPCTSTR pszHint);

typedef struct _WBSPLITBTN
{
	HWND hwndToolbar;
	int nButtonId;
	HMENU hMenu;
	int nDefaultCommand;
	struct _WBSPLITBTN *pNext;
} WBSPLITBTN, *PWBSPLITBTN;

static PWBSPLITBTN g_pSplitButtons = NULL;

static PWBSPLITBTN FindSplitButton(HWND hwndToolbar, int nButtonId);
static PWBSPLITBTN FindOrCreateSplitButton(HWND hwndToolbar, int nButtonId);

// External

extern void SetToolBarHandle(HWND hCtrl);
extern BOOL IsBitmap(HANDLE handle);

//----------------------------------------------------------- EXPORTED FUNCTIONS

PWBOBJ wbCreateToolbar(PWBOBJ pwboParent, PWBITEM pitem[], int nItems, int nBtnWidth, int nBtnHeight, HBITMAP hbm)
{
	int i;
	PWBOBJ pwbo;
	HWND hToolbar;

	if (!pwboParent || !pwboParent->hwnd || !IsWindow(pwboParent->hwnd))
		return NULL;

	// Create the toolbar

	hToolbar = CreateToolbar((HWND)pwboParent->hwnd, nItems, nBtnWidth, nBtnHeight, hbm);
	if (!hToolbar)
	{
		wbError(TEXT(__FUNCTION__), MB_ICONWARNING, TEXT("Could not create toolbar"));
		return NULL;
	}

	pwbo = wbMalloc(sizeof(WBOBJ));
	pwbo->hwnd = hToolbar;
	pwbo->id = 0;
	pwbo->uClass = ToolBar;
	pwbo->item = -1;
	pwbo->subitem = -1;
	pwbo->style = 0;
	pwbo->pszCallBackFn = NULL;
	pwbo->pszCallBackObj = NULL;
	pwbo->lparam = 0;
	pwbo->parent = pwboParent;

	for (i = 0; i < nItems; i++)
	{
		if (!pitem[i] || !pitem[i]->id)
			CreateToolbarButton(hToolbar, 0, i, NULL); // Separator
		else
			CreateToolbarButton(hToolbar, pitem[i]->id, pitem[i]->index, pitem[i]->pszHint);
	}

	SetWindowLongPtr(pwbo->hwnd, GWLP_USERDATA, (LONG_PTR)pwbo);

	return pwbo;
}

//------------------------------------------------------------ PRIVATE FUNCTIONS

static HWND CreateToolbar(HWND hwndParent, int nButtons, int nBtnWidth, int nBtnHeight, HBITMAP hbm)
{
	HWND hTBWnd; // Handle of toolbar window
	HIMAGELIST imageList;

	// Cria a toolbar

	hTBWnd = CreateWindowEx(0,
							TOOLBARCLASSNAME, TEXT(""),
							WS_CHILD | WS_VISIBLE | CCS_TOP | TBSTYLE_TOOLTIPS | TBSTYLE_FLAT,
							0, 0, 0, 0,
							hwndParent, NULL, hAppInstance, NULL);

	if (!hTBWnd)
		return NULL;

	SetToolBarHandle(hTBWnd);
	SendMessage(hTBWnd, TB_SETEXTENDEDSTYLE, 0, TBSTYLE_EX_DRAWDDARROWS);

	// Create an ImageList with transparent bitmaps

	if (hbm && IsBitmap(hbm))
	{

		nButtons = MAX(1, MIN(nButtons, MIN(nBtnWidth, MAX_IMAGELIST_IMAGES)));

		imageList = ImageList_Create(nBtnWidth, nBtnHeight, ILC_COLORDDB | ILC_MASK, nButtons, 0);
		ImageList_AddMasked(imageList, hbm, RGB(0, 255, 0)); // hardcoded to green....
		DeleteObject(hbm);
		SendMessage(hTBWnd, TB_SETIMAGELIST, 0, (LPARAM)imageList);
	}

	return hTBWnd;
}

static BOOL CreateToolbarButton(HWND hwnd, int id, int nIndex, LPCTSTR pszHint)
{
	TBBUTTON tbb = {0};
	BOOL bRet;

	tbb.fsState = TBSTATE_ENABLED;

	if (id == 0)
	{ // Separator
		tbb.idCommand = 0;
		tbb.fsState = 0;
		tbb.fsStyle = TBSTYLE_SEP;
		tbb.iBitmap = 0;
		tbb.iString = 0;
	}
	else
	{ // Button
		tbb.idCommand = id;
		tbb.fsState = TBSTATE_ENABLED;
		tbb.fsStyle = TBSTYLE_BUTTON;
		tbb.iBitmap = nIndex;
		tbb.iString = 0;
		if (pszHint && *pszHint)
		{
		    // Convert UTF-8 to wide char using Windows API
			int len = MultiByteToWideChar(CP_UTF8, 0, (LPCCH)pszHint, -1, NULL, 0);
			if (len > 0)
			{
				LPWSTR pszWideHint = (LPWSTR)wbMalloc(len * sizeof(WCHAR));
				MultiByteToWideChar(CP_UTF8, 0, (LPCCH)pszHint, -1, pszWideHint, len);
				tbb.dwData = (DWORD_PTR)pszWideHint;
			}
			else
			{
				tbb.dwData = 0;
			}
		}
		else {
			tbb.dwData = 0;
		}
	}

	// Insert the button

	bRet = SendMessage(hwnd, TB_INSERTBUTTON, 32767, (LPARAM)&tbb);
	if (!bRet)
	{
		if (id)
			wbError(TEXT(__FUNCTION__), MB_ICONWARNING, TEXT("Could not create item # %d in toolbar"), id);
		else
			wbError(TEXT(__FUNCTION__), MB_ICONWARNING, TEXT("Could not create separator in toolbar"));
	}
	return bRet;
}



BOOL wbToolbarAttachSplitMenu(PWBOBJ pwboToolbar, int nButtonId, PWBOBJ pwboMenu)
{
	TBBUTTONINFO tbbi = {0};
	PWBSPLITBTN pSplit;

	if (!pwboToolbar || pwboToolbar->uClass != ToolBar || !IsWindow(pwboToolbar->hwnd))
		return FALSE;

	if (!pwboMenu || pwboMenu->uClass != Menu || !pwboMenu->hwnd || !IsMenu((HMENU)pwboMenu->hwnd))
		return FALSE;

	pSplit = FindOrCreateSplitButton(pwboToolbar->hwnd, nButtonId);
	if (!pSplit)
		return FALSE;

	pSplit->hMenu = (HMENU)pwboMenu->hwnd;
	if (pSplit->nDefaultCommand == 0)
		pSplit->nDefaultCommand = nButtonId;

	tbbi.cbSize = sizeof(TBBUTTONINFO);
	tbbi.dwMask = TBIF_STYLE;
	tbbi.fsStyle = TBSTYLE_BUTTON | TBSTYLE_DROPDOWN;

	if (!SendMessage(pwboToolbar->hwnd, TB_SETBUTTONINFO, nButtonId, (LPARAM)&tbbi))
		return FALSE;

	return TRUE;
}

BOOL wbToolbarSetSplitDefault(PWBOBJ pwboToolbar, int nButtonId, int nDefaultCommand)
{
	PWBSPLITBTN pSplit;

	if (!pwboToolbar || pwboToolbar->uClass != ToolBar || !IsWindow(pwboToolbar->hwnd))
		return FALSE;

	pSplit = FindSplitButton(pwboToolbar->hwnd, nButtonId);
	if (!pSplit)
		return FALSE;

	pSplit->nDefaultCommand = nDefaultCommand;
	return TRUE;
}

BOOL wbToolbarGetSplitInfo(HWND hwndToolbar, int nButtonId, HMENU *phMenu, int *pnDefaultCommand)
{
	PWBSPLITBTN pSplit = FindSplitButton(hwndToolbar, nButtonId);
	if (!pSplit || !pSplit->hMenu)
		return FALSE;

	if (phMenu)
		*phMenu = pSplit->hMenu;
	if (pnDefaultCommand)
		*pnDefaultCommand = pSplit->nDefaultCommand;
	return TRUE;
}

static PWBSPLITBTN FindSplitButton(HWND hwndToolbar, int nButtonId)
{
	PWBSPLITBTN pItem = g_pSplitButtons;
	while (pItem)
	{
		if (pItem->hwndToolbar == hwndToolbar && pItem->nButtonId == nButtonId)
			return pItem;
		pItem = pItem->pNext;
	}
	return NULL;
}

static PWBSPLITBTN FindOrCreateSplitButton(HWND hwndToolbar, int nButtonId)
{
	PWBSPLITBTN pItem = FindSplitButton(hwndToolbar, nButtonId);
	if (pItem)
		return pItem;

	pItem = wbMalloc(sizeof(WBSPLITBTN));
	if (!pItem)
		return NULL;

	pItem->hwndToolbar = hwndToolbar;
	pItem->nButtonId = nButtonId;
	pItem->hMenu = NULL;
	pItem->nDefaultCommand = nButtonId;
	pItem->pNext = g_pSplitButtons;
	g_pSplitButtons = pItem;
	return pItem;
}

//------------------------------------------------------------------ END OF FILE
