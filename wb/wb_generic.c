/*******************************************************************************

 WINBINDER - The native Windows binding for PHP

 Copyright  Hypervisual - see LICENSE.TXT for details
 Author: Rubem Pechansky (https://github.com/crispy-computing-machine/Winbinder)

 General-purpose functions

*******************************************************************************/

//----------------------------------------------------------------- DEPENDENCIES

#include "wb.h"

//----------------------------------------------------------- EXPORTED FUNCTIONS

/* Return TRUE if pwbo is a valid WinBinder object */

BOOL wbIsWBObj(void *pwbo, BOOL bShowErrors)
{

    //printf("wbIsWBObj 1\n");
	if (!pwbo)
	{
		if (bShowErrors){
			wbError(TEXT(__FUNCTION__), MB_ICONWARNING, TEXT("NULL WinBinder object"));
		}
		return FALSE;
	}

    //printf("wbIsWBObj 2\n");
	// Is pwbo a valid memory address?
	if (IsBadReadPtr(pwbo, sizeof(WBOBJ)))
	{

		if (bShowErrors)
			wbError(TEXT(__FUNCTION__), MB_ICONWARNING, TEXT("Invalid memory address"));
		//		printf("%d\n", pwbo);
		return FALSE;
	}

	//printf("wbIsWBObj 3\n");
	// A Windows or menu handle is not a WinBinder object
	if (IsWindow(pwbo) || IsMenu(pwbo))
	{
		if (bShowErrors)
			wbError(TEXT(__FUNCTION__), MB_ICONWARNING, TEXT("Not a WinBinder object"));
		return FALSE;
	}

	// Does it have a valid handle?
	//printf("wbIsWBObj 4\n");
    HWND hwnd = ((PWBOBJ)pwbo)->hwnd;
    if (!hwnd || !IsWindow(hwnd)) {
        if (bShowErrors)
            wbError(TEXT(__FUNCTION__), MB_ICONWARNING, TEXT("Invalid WinBinder object handle"));
        return FALSE;
    }

    //printf("wbIsWBObj 5\n");
	if (IsMenu((HMENU)((PWBOBJ)pwbo)->hwnd))
		return TRUE;

    //printf("wbIsWBObj 6\n");
	if (IsWindow((HWND)((PWBOBJ)pwbo)->hwnd))
		return TRUE;

	if (bShowErrors)
		wbError(TEXT(__FUNCTION__), MB_ICONWARNING, TEXT("Invalid WinBinder object"));

    //printf("wbIsWBObj 7\n");
	return FALSE;
}

PWBOBJ wbGetWBObj(HANDLE hwnd)
{
	if (IsWindow(hwnd))
		return (PWBOBJ)GetWindowLongPtr((HWND)hwnd, GWLP_USERDATA);
	else
		return NULL;
}

/* Convert line breaks for Windows use. Allocs memory for ppszTarget. */

BOOL wbConvertLineBreaks(TCHAR **ppszTarget, const TCHAR *pszSource)
{
	int i, src, tgt, nLen;

	if (!pszSource || !*pszSource)
	{ // Zero-length or NULL source

		nLen = 1;
		*ppszTarget = wbMalloc(sizeof(TCHAR));
		**ppszTarget = NUL;
		return FALSE;
	}
	else
	{

		nLen = wcslen(pszSource);
		*ppszTarget = wbMalloc((nLen * 2 + 1) * sizeof(TCHAR)); // At the maximum, converts all chars

		for (i = 0, src = 0, tgt = 0; i < nLen; src++, i++)
		{
			switch (*(pszSource + src))
			{

			case '\r':
				break;

			case '\n':
				*(*ppszTarget + tgt) = '\r';
				tgt++;
				*(*ppszTarget + tgt) = '\n';
				tgt++;
				break;

			default:
				*(*ppszTarget + tgt) = *(pszSource + src);
				tgt++;
				break;
			}
		}

		*(*ppszTarget + tgt) = NUL;
		return TRUE;
	}
}

/* Local stristr() replacement to solve MSVC compilation problems
   Thanks to the SNIPPETS C source code archives (http://c.snippets.org/) */

LPTSTR wbStriStr(LPCTSTR String, LPCTSTR Pattern)
{
	TCHAR *pptr, *sptr, *start;

	for (start = (TCHAR *)String; *start != NUL; start++)
	{

		/* Find start of pattern in string */

		for (; ((*start != NUL) && (towupper(*start) != towupper(*Pattern))); start++)
			;

		if (NUL == *start)
			return NULL;

		pptr = (TCHAR *)Pattern;
		sptr = start;

		while (towupper(*sptr) == towupper(*pptr))
		{
			sptr++;
			pptr++;

			/* If end of pattern then pattern was found */

			if (NUL == *pptr)
				return (start);
		}
	}
	return NULL;
}

//------------------------------------------------------------------ END OF FILE
