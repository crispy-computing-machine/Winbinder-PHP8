/*******************************************************************************

 WINBINDER - The native Windows binding for PHP

 Copyright  Hypervisual - see LICENSE.TXT for details
 Author: Rubem Pechansky (https://github.com/crispy-computing-machine/Winbinder)

 Window system functions

*******************************************************************************/

//----------------------------------------------------------------- DEPENDENCIES

#include "wb.h"
#include <shellapi.h> // For ShellExecute()
#include <shlobj.h>   // For SHGetSpecialFolderLocation()
#include <io.h>		  // For access()
#include <stdio.h>	// For printf() -- beep
#include <string.h>

#include <shlobj.h>			// For uxtheme.h
#include <uxtheme.h>		// For theme functions
#include <psapi.h>			// For memory information functions
#include <ole2.h>			// For OleInitialize()
#include <mmsystem.h>		// For PlaySound()
#include <VersionHelpers.h> // For VERSIONHELPERAPI

//-------------------------------------------------------------------- CONSTANTS

#define OBJ_COLORSPACE 14

//----------------------------------------------------------------------- MACROS

#define INCRMAX(v, mx) ((v) = (v) < (mx) ? (v) + 1 : (mx)) // Increment to a ceiling
#define DECRMIN(v, mn) ((v) = (v) > (mn) ? (v)-1 : (mn))   // Decrement to a floor
#define WHITESPACES " \t\n\r\f\v"

//-------------------------------------------------------------------- VARIABLES

// Global

HFONT hIconFont = NULL;
HWND hCurrentDlg = NULL;

// Local

static HACCEL hAccelTable = NULL;		 // Accelerator table
static HWND hAccelWnd = NULL;			 // Accelerator table window
static HCURSOR hClassCursor[NUMCLASSES]; // Table of mouse cursors, one for each class

typedef struct _wb_async_event
{
	HWND hwndTarget;
	UINT64 taskId;
	int eventCode;
	int progress;
	DWORD value;
	struct _wb_async_event *next;
} WB_ASYNC_EVENT;

typedef struct _wb_async_task
{
	UINT64 id;
	HWND hwndTarget;
	TCHAR *command;
	UINT64 estimatedMs;
	HANDLE thread;
	HANDLE cancelEvent;
	volatile LONG status;
	volatile LONG progress;
	DWORD exitCode;
	DWORD errorCode;
	struct _wb_async_task *next;
} WB_ASYNC_TASK;

static CRITICAL_SECTION g_taskLock;
static BOOL g_taskLockInit = FALSE;
static WB_ASYNC_TASK *g_taskList = NULL;
static UINT64 g_nextTaskId = 1;

static CRITICAL_SECTION g_eventLock;
static BOOL g_eventLockInit = FALSE;
static WB_ASYNC_EVENT *g_eventHead = NULL;
static WB_ASYNC_EVENT *g_eventTail = NULL;

//---------------------------------------------------------- FUNCTION PROTOTYPES

// Private

static int _GetUserObjects(void);
static int _GetGDIObjects(void);
static HKEY _GetKeyFromString(LPCTSTR pszKey);
static BOOL _GetOSVersionString(LPTSTR pszString);
static HCURSOR GetSysCursor(LPCTSTR pszCursor);
static char *GetTokenExt(const char *pszBuffer, int nToken, const char *pszSep, char chGroup, BOOL fBlock, char *pszToken, int nTokLen);
static char *GetToken(const char *pszBuffer, int nToken, char *pszToken, int nTokLen);
static void wbTaskPostEvent(WB_ASYNC_TASK *task, int eventCode, int progress, DWORD value);
static DWORD WINAPI wbTaskThreadProc(LPVOID lpParam);
static void wbTaskCleanup(void);
BOOL wbTaskDequeueEvent(HWND hwndTarget, UINT64 *taskId, int *eventCode, int *progress, DWORD *value);

BOOL bScintillaAvailable = FALSE;

// Public to wb_* modules

LPTSTR MakeWinPath(LPTSTR pszPath);

// External

extern BOOL RegisterClasses(void);
extern char *WideChar2Utf8(LPCTSTR wcs, int *len);


#define MAX_WB_WATCHERS 32
#define WB_WATCH_BUFFER_SIZE 65536
#define MAX_WB_WATCH_EVENTS 512

typedef struct _WBWATCH
{
	int id;
	BOOL in_use;
	BOOL recursive;
	DWORD debounce_ms;
	HANDLE dir_handle;
	HANDLE event_handle;
	OVERLAPPED overlapped;
	BYTE buffer[WB_WATCH_BUFFER_SIZE];
	TCHAR base_path[MAX_PATH_BUFFER];
} WBWATCH;

typedef struct _WBWATCHEVENT
{
	int watch_id;
	int event_type;
	DWORD tick_count;
	TCHAR base_path[MAX_PATH_BUFFER];
	TCHAR rel_path[MAX_PATH_BUFFER];
} WBWATCHEVENT;

static WBWATCH g_watchers[MAX_WB_WATCHERS];
static WBWATCHEVENT g_watch_events[MAX_WB_WATCH_EVENTS];
static int g_watch_event_count = 0;
static int g_next_watch_id = 1;

static DWORD WatchNotifyFilter(void)
{
	return FILE_NOTIFY_CHANGE_FILE_NAME |
			   FILE_NOTIFY_CHANGE_DIR_NAME |
			   FILE_NOTIFY_CHANGE_LAST_WRITE |
			   FILE_NOTIFY_CHANGE_CREATION |
			   FILE_NOTIFY_CHANGE_SIZE;
}

static void WatchNormalizePath(LPTSTR dst, UINT dstLen, LPCTSTR src)
{
	if (!dst || dstLen == 0)
		return;

	if (!src)
	{
		dst[0] = 0;
		return;
	}

	lstrcpyn(dst, src, dstLen);
	MakeWinPath(dst);

	while (*dst)
	{
		UINT len = lstrlen(dst);
		if (len <= 3)
			break;
		if (dst[len - 1] != TEXT('\\'))
			break;
		dst[len - 1] = 0;
	}
}

static BOOL WatchArm(WBWATCH *watch)
{
	DWORD bytes = 0;
	if (!watch || !watch->in_use)
		return FALSE;

	ResetEvent(watch->event_handle);
	ZeroMemory(&watch->overlapped, sizeof(OVERLAPPED));
	watch->overlapped.hEvent = watch->event_handle;

	if (ReadDirectoryChangesW(
			watch->dir_handle,
			watch->buffer,
			sizeof(watch->buffer),
			watch->recursive,
			WatchNotifyFilter(),
			&bytes,
			&watch->overlapped,
			NULL))
	{
		return TRUE;
	}

	return GetLastError() == ERROR_IO_PENDING;
}

static WBWATCH *WatchById(int watchId)
{
	int i;
	for (i = 0; i < MAX_WB_WATCHERS; i++)
	{
		if (g_watchers[i].in_use && g_watchers[i].id == watchId)
			return &g_watchers[i];
	}
	return NULL;
}

static int MapWatchAction(DWORD action)
{
	switch (action)
	{
	case FILE_ACTION_ADDED:
		return WBE_FILE_CREATED;
	case FILE_ACTION_REMOVED:
		return WBE_FILE_DELETED;
	case FILE_ACTION_MODIFIED:
		return WBE_FILE_MODIFIED;
	case FILE_ACTION_RENAMED_OLD_NAME:
		return WBE_FILE_RENAMED_OLD;
	case FILE_ACTION_RENAMED_NEW_NAME:
		return WBE_FILE_RENAMED_NEW;
	}
	return 0;
}

static BOOL WatchEventEquals(const WBWATCHEVENT *a, const WBWATCHEVENT *b)
{
	return a->watch_id == b->watch_id &&
		   a->event_type == b->event_type &&
		   lstrcmpi(a->rel_path, b->rel_path) == 0;
}

static void PushWatchEvent(const WBWATCHEVENT *evt, DWORD debounceMs)
{
	int i;
	if (!evt || !evt->event_type)
		return;

	for (i = g_watch_event_count - 1; i >= 0; i--)
	{
		DWORD delta;
		if (!WatchEventEquals(&g_watch_events[i], evt))
			continue;
		delta = evt->tick_count - g_watch_events[i].tick_count;
		if (delta <= debounceMs)
		{
			g_watch_events[i].tick_count = evt->tick_count;
			return;
		}
		break;
	}

	if (g_watch_event_count >= MAX_WB_WATCH_EVENTS)
	{
		memmove(&g_watch_events[0], &g_watch_events[1], sizeof(g_watch_events[0]) * (MAX_WB_WATCH_EVENTS - 1));
		g_watch_event_count = MAX_WB_WATCH_EVENTS - 1;
	}

	g_watch_events[g_watch_event_count++] = *evt;
}

static void ParseWatchBuffer(WBWATCH *watch)
{
	DWORD bytes = 0;
	BYTE *cursor;

	if (!watch)
		return;

	if (!GetOverlappedResult(watch->dir_handle, &watch->overlapped, &bytes, FALSE))
		return;
	if (bytes == 0)
		return;

	cursor = watch->buffer;
	while (cursor)
	{
		FILE_NOTIFY_INFORMATION *fni = (FILE_NOTIFY_INFORMATION *)cursor;
		int eventType = MapWatchAction(fni->Action);
		WBWATCHEVENT evt;
		int charLen = (int)(fni->FileNameLength / sizeof(WCHAR));
		int copyLen = MIN(charLen, MAX_PATH_BUFFER - 1);

		ZeroMemory(&evt, sizeof(evt));
		evt.watch_id = watch->id;
		evt.event_type = eventType;
		evt.tick_count = GetTickCount();
		lstrcpyn(evt.base_path, watch->base_path, MAX_PATH_BUFFER);
		if (copyLen > 0)
		{
			memcpy(evt.rel_path, fni->FileName, copyLen * sizeof(WCHAR));
			evt.rel_path[copyLen] = 0;
		}

		PushWatchEvent(&evt, watch->debounce_ms ? watch->debounce_ms : 200);

		if (fni->NextEntryOffset == 0)
			break;
		cursor += fni->NextEntryOffset;
	}
}

static int CollectWatchHandles(HANDLE *handles, int maxHandles)
{
	int i, count = 0;
	if (!handles || maxHandles <= 0)
		return 0;
	for (i = 0; i < MAX_WB_WATCHERS && count < maxHandles; i++)
	{
		if (g_watchers[i].in_use && g_watchers[i].event_handle)
			handles[count++] = g_watchers[i].event_handle;
	}
	return count;
}

static void PumpWatchNotifications(DWORD timeoutMs)
{
	HANDLE handles[MAX_WB_WATCHERS];
	int count, i;
	DWORD waitMs = timeoutMs;

	count = CollectWatchHandles(handles, MAX_WB_WATCHERS);
	if (count <= 0)
		return;

	while (1)
	{
		DWORD wr = WaitForMultipleObjects(count, handles, FALSE, waitMs);
		if (wr < WAIT_OBJECT_0 || wr >= WAIT_OBJECT_0 + (DWORD)count)
			break;

		for (i = 0; i < MAX_WB_WATCHERS; i++)
		{
			if (!g_watchers[i].in_use || !g_watchers[i].event_handle)
				continue;
			if (WaitForSingleObject(g_watchers[i].event_handle, 0) == WAIT_OBJECT_0)
			{
				ParseWatchBuffer(&g_watchers[i]);
				WatchArm(&g_watchers[i]);
			}
		}

		waitMs = 0;
	}
}

//------------------------------------------------------------- PUBLIC FUNCTIONS

/* Module initialization */

BOOL wbInit(void)
{
	//	int nClass;
	LOGFONT lfIcon;
	OSVERSIONINFO ovInfo;
	INITCOMMONCONTROLSEX icex;
	HMIDIOUT hmo;

	// Create the interface font

	SystemParametersInfo(SPI_GETICONTITLELOGFONT, sizeof(LOGFONT), &lfIcon, 0);
	hIconFont = CreateFontIndirect(&lfIcon);

	// Load the common control DLLs

	InitCommonControls();
	if (!LoadLibrary(TEXT("RICHED20.DLL"))) // This is version 2.0 of the DLL
		LoadLibrary(TEXT("RICHED32.DLL"));
	bScintillaAvailable = (LoadLibrary(TEXT("SciLexer.dll")) != NULL);
	if (!bScintillaAvailable)
		wbError(TEXT(__FUNCTION__), MB_ICONWARNING, TEXT("Scintilla runtime (SciLexer.dll) not found. ScintillaEdit controls will be unavailable."));
	icex.dwSize = sizeof(icex);
	icex.dwICC = ICC_DATE_CLASSES; // Load date and time picker control class
	InitCommonControlsEx(&icex);
	if (OleInitialize(NULL) != S_OK) // Initialize the OLE interface
		return FALSE;

	// Get the handle of the file used to create this process

	hAppInstance = GetModuleHandle(NULL);

	// Set the background color for tab controls
	if (IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WINXP), LOBYTE(_WIN32_WINNT_WINXP), 2))
	{ //ovInfo.dwMajorVersion >= 5 && ovInfo.dwMinorVersion >= 1) {	// Windows XP only */

		// Check if a manifest file exists

		OFSTRUCT ofs;
		char szApp[MAX_PATH];
		TCHAR *szCmd = GetCommandLine();
		char *cmd = WideChar2Utf8(szCmd, NULL);

		GetToken(cmd, 0, szApp, MAX_PATH - 1);
		strcat(szApp, ".manifest");
		// 2011_11_24 - Stefan Loewe: line below commented out, because if it fails with Windows 7 32 bit, with PHP 5.4 RC1
		//free(cmd);

		ofs.cBytes = sizeof(OFSTRUCT);
		if (OpenFile(szApp, &ofs, OF_EXIST) != HFILE_ERROR)
		{

			// Check if themes are enabled

			HANDLE hLib;
			HTHEME hTheme;

			//			THEMEAPI_(HTHEME)(*w_OpenThemeData)(HWND, LPCWSTR);
			//			THEMEAPI (*w_CloseThemeData)(HTHEME);

			//			HTHEME (DECLSPEC_IMPORT STDAPICALLTYPE *w_OpenThemeData)(HWND, LPCWSTR);
			//			HRESULT (DECLSPEC_IMPORT STDAPICALLTYPE *w_CloseThemeData)(HTHEME);
			HTHEME(STDAPICALLTYPE * w_OpenThemeData)
			(HWND, LPCWSTR);
			HRESULT(STDAPICALLTYPE * w_CloseThemeData)
			(HTHEME);

			hLib = LoadLibrary(TEXT("UxTheme.dll"));

			if (hLib)
			{ // Is the DLL present?
				w_OpenThemeData = (void *)GetProcAddress(hLib, "OpenThemeData");
				w_CloseThemeData = (void *)GetProcAddress(hLib, "CloseThemeData");
				hTheme = w_OpenThemeData(GetDesktopWindow(), L"Tab");

				if (hTheme)
				{ // Is there an active theme?

					clrTabs = 0xFCFCFE; // This is the tab color for all standard XP themes, but of course
										// it should not be hardcoded. Couldn't find a method of obtaining
										// this color. GetThemeColor() with TABP_PANE/TIS_NORMAL/TMT_FILLCOLOR
										// should work but it doesn't, maybe because WinBinder doesn't use
										// class #32770 (dialog class) for the tab pages.
					hbrTabs = CreateSolidBrush(clrTabs);
					w_CloseThemeData(hTheme);
				}
			}
		}
	}

	if (!RegisterClasses())
		return FALSE;

	//	for(nClass = 0; nClass < NUMCLASSES; nClass++)
	//		wbSetCursor((PWBOBJ)Calendar, NULL);

	// Turn off any hanging MIDI notes

	//midiOutOpen(&hmo, MIDI_MAPPER, 0, 0, CALLBACK_NULL);
	//midiOutReset(hmo);
	//midiOutClose(hmo);

	// Initializes mouse cursor for some control classes

	wbSetCursor((PWBOBJ)EditBox, TEXT("ibeam"), NULL);
	wbSetCursor((PWBOBJ)InvisibleArea, TEXT("arrow"), NULL);

	if (!g_taskLockInit)
	{
		InitializeCriticalSection(&g_taskLock);
		g_taskLockInit = TRUE;
	}
	if (!g_eventLockInit)
	{
		InitializeCriticalSection(&g_eventLock);
		g_eventLockInit = TRUE;
	}

	return TRUE;
}

/* Module end */

BOOL wbEnd(void)
{
	int i;
	for (i = 0; i < MAX_WB_WATCHERS; i++)
	{
		if (g_watchers[i].in_use)
			wbUnwatchPath(g_watchers[i].id);
	}
	wbWatchClearEvents();

	if (hIconFont)
		DeleteObject(hIconFont);
	if (hbrTabs)
		DeleteObject(hbrTabs);
	if (hAccelTable)
		DestroyAcceleratorTable(hAccelTable);
	wbTaskCleanup();
	if (g_eventLockInit)
	{
		DeleteCriticalSection(&g_eventLock);
		g_eventLockInit = FALSE;
	}
	if (g_taskLockInit)
	{
		DeleteCriticalSection(&g_taskLock);
		g_taskLockInit = FALSE;
	}
	OleUninitialize();

	return TRUE;
}

int wbWatchPath(LPCTSTR pszPath, BOOL bRecursive, DWORD dwDebounceMs)
{
	int i;
	WBWATCH *watch = NULL;
	TCHAR normalized[MAX_PATH_BUFFER];

	WatchNormalizePath(normalized, MAX_PATH_BUFFER, pszPath);
	if (!normalized[0])
		return 0;

	for (i = 0; i < MAX_WB_WATCHERS; i++)
	{
		if (!g_watchers[i].in_use)
		{
			watch = &g_watchers[i];
			break;
		}
	}
	if (!watch)
		return 0;

	ZeroMemory(watch, sizeof(*watch));
	watch->event_handle = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (!watch->event_handle)
		return 0;

	watch->dir_handle = CreateFile(
		normalized,
		FILE_LIST_DIRECTORY,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
		NULL);
	if (watch->dir_handle == INVALID_HANDLE_VALUE)
	{
		CloseHandle(watch->event_handle);
		watch->event_handle = NULL;
		return 0;
	}

	watch->id = g_next_watch_id++;
	watch->in_use = TRUE;
	watch->recursive = bRecursive;
	watch->debounce_ms = dwDebounceMs;
	lstrcpyn(watch->base_path, normalized, MAX_PATH_BUFFER);

	if (!WatchArm(watch))
	{
		wbUnwatchPath(watch->id);
		return 0;
	}

	return watch->id;
}

BOOL wbUnwatchPath(int nWatchId)
{
	WBWATCH *watch = WatchById(nWatchId);
	if (!watch)
		return FALSE;

	CancelIo(watch->dir_handle);
	if (watch->dir_handle && watch->dir_handle != INVALID_HANDLE_VALUE)
		CloseHandle(watch->dir_handle);
	if (watch->event_handle)
		CloseHandle(watch->event_handle);
	ZeroMemory(watch, sizeof(*watch));
	return TRUE;
}

UINT64 wbWatchPoll(DWORD dwTimeoutMs, void (*event_cb)(int watchId, int eventType, const TCHAR *basePath, const TCHAR *relativePath, DWORD tickCount, void *ctx), void *ctx)
{
	UINT64 i;
	PumpWatchNotifications(dwTimeoutMs);
	if (!event_cb)
		return (UINT64)g_watch_event_count;

	for (i = 0; i < (UINT64)g_watch_event_count; i++)
	{
		event_cb(g_watch_events[i].watch_id,
				 g_watch_events[i].event_type,
				 g_watch_events[i].base_path,
				 g_watch_events[i].rel_path,
				 g_watch_events[i].tick_count,
				 ctx);
	}
	return (UINT64)g_watch_event_count;
}

BOOL wbWatchGetEvent(int watchEventIndex, int *watchId, int *eventType, LPCTSTR *basePath, LPCTSTR *relativePath, DWORD *tickCount)
{
	if (watchEventIndex < 0 || watchEventIndex >= g_watch_event_count)
		return FALSE;
	if (watchId)
		*watchId = g_watch_events[watchEventIndex].watch_id;
	if (eventType)
		*eventType = g_watch_events[watchEventIndex].event_type;
	if (basePath)
		*basePath = g_watch_events[watchEventIndex].base_path;
	if (relativePath)
		*relativePath = g_watch_events[watchEventIndex].rel_path;
	if (tickCount)
		*tickCount = g_watch_events[watchEventIndex].tick_count;
	return TRUE;
}

void wbWatchClearEvents(void)
{
	g_watch_event_count = 0;
}

/*

References:

http://groups.google.com/groups?hl=en&lr=&frame=right&th=d1c76898d0731052&seekm=1KE29.8484%24wh1.7226%40news01.bloor.is.net.cable.rogers.com#link6
http://code.glowdot.com/tutorials/win32tutorial.php?page=10

Notice that TranslateAccelerator is called before IsDialogMessage. if they are called
in the reverse order, and the focus is on one of our edit controls, IsDialogMessage
will take our shortcut combo and process it as a dialog message.

*/

WPARAM wbMainLoop(void)
{
	MSG msg;
	HANDLE watchHandles[MAX_WB_WATCHERS];

	while (1)
	{
		int watchCount = CollectWatchHandles(watchHandles, MAX_WB_WATCHERS);
		DWORD waitResult = MsgWaitForMultipleObjects(watchCount, watchHandles, FALSE, INFINITE, QS_ALLINPUT);

		if (waitResult >= WAIT_OBJECT_0 && waitResult < WAIT_OBJECT_0 + (DWORD)watchCount)
		{
			PumpWatchNotifications(0);
			continue;
		}

		if (waitResult == WAIT_OBJECT_0 + (DWORD)watchCount)
		{
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				if (msg.message == WM_QUIT)
					return msg.wParam;

				if (!TranslateAccelerator(hAccelWnd, hAccelTable, &msg))
				{
					if (!hCurrentDlg || !IsDialogMessage(hCurrentDlg, &msg))
					{
						TranslateMessage(&msg);
						DispatchMessage(&msg);
					}
				}
			}

			PumpWatchNotifications(0);
		}
	}
}

// Used in wb_wait()
// Reference: API help, "Examining a Message Queue"

UINT64 wbCheckInput(PWBOBJ pwbo, DWORD dwFlags, DWORD dwTimeout)
{
	HWND hwnd;
	MSG msg;
	DWORD dwMs;

	if (!pwbo)
		hwnd = GetForegroundWindow();
	else
	{
		if (!pwbo || !pwbo->hwnd || !IsWindow(pwbo->hwnd))
			return FALSE;
		hwnd = pwbo->hwnd;
	}

	// Ensure that messages will go to this window

	SetFocus(hwnd);
	// Peek message
	dwMs = GetTickCount();

	//	dwTimeout = MAX(1, dwTimeout);
	while (TRUE)
	{
		if (PeekMessage(&msg, hwnd, 0, 0, PM_REMOVE))
		{

			switch (msg.message)
			{

			case WM_LBUTTONDOWN:
				if (dwFlags & WBC_MOUSEDOWN)
					return MAKELONG(WBC_MOUSEDOWN, 1);
				break;

			case WM_MBUTTONDOWN:
				if (dwFlags & WBC_MOUSEDOWN)
					return MAKELONG(WBC_MOUSEDOWN, 2);
				break;

			case WM_RBUTTONDOWN:
				if (dwFlags & WBC_MOUSEDOWN)
					return MAKELONG(WBC_MOUSEDOWN, 3);
				break;

			case WM_LBUTTONUP:
				if (dwFlags & WBC_MOUSEUP)
					return MAKELONG(WBC_MOUSEUP, 1);
				break;

			case WM_MBUTTONUP:
				if (dwFlags & WBC_MOUSEUP)
					return MAKELONG(WBC_MOUSEUP, 2);
				break;

			case WM_RBUTTONUP:
				if (dwFlags & WBC_MOUSEUP)
					return MAKELONG(WBC_MOUSEUP, 3);
				break;

			case WM_KEYDOWN:
				if (dwFlags & WBC_KEYDOWN)
					return MAKELONG(msg.wParam, 0);
				break;

			case WM_KEYUP:
				if (dwFlags & WBC_KEYUP)
					return MAKELONG(msg.wParam, 0);
				break;
			}

			// "Eat" this message
			TranslateMessage(&msg);
			DispatchMessage(&msg);

		} // ~if

        // No timeout
		if (dwTimeout == 0)
			break;

        // Elapsed timeout, break
		if (GetTickCount() >= dwMs + dwTimeout) // Time's out
			break;

	} // ~while

	return 0;
}


// Set system, class or control mouse cursor
// ******* It seems that all child controls inherit the mouse cursor style
// ******* from their parent window, and it's not clear if this
// ******* behavior is standard in Windows (seems so) or some WinBinder bug
BOOL wbSetCursor(PWBOBJ pwbo, LPCTSTR pszCursor, HANDLE handle)
{
	HCURSOR hCursor;
	TCHAR szFile[MAX_PATH]; // 256

	if (!pwbo)
	{

		// Set system cursor
		if (handle) {// Cursor handle
			hCursor = handle;
		}

        // pszCursor is NULL (reset cursor)
		if (!pszCursor || !*pszCursor) {
			hCursor = GetSysCursor(TEXT("arrow"));
		} else {
            wcsncpy(szFile, pszCursor, MAX_PATH - 1);
            szFile[MAX_PATH - 1] = TEXT('\0');
            if(wbFindFile(szFile, MAX_PATH)) {
                // Assume it's a file path for a custom cursor
                hCursor = LoadCursorFromFile(szFile);
                if (!hCursor)
                {
                    // If loading from file fails, fall back to arrow cursor
                    hCursor = GetSysCursor(TEXT("arrow"));
                }
            } else {
                // System Cursor name
                hCursor = GetSysCursor(pszCursor);
            }
        }

        // If loading from file/system fails, fall back to arrow cursor
        if (!hCursor)
        {
            hCursor = GetSysCursor(TEXT("arrow"));
        }

		if (hCursor){
			SetCursor(hCursor == (HCURSOR)-1 ? 0 : hCursor);
		}

		// Must NOT use the value returned from from SetCursor()
		return (hCursor != 0);

	} else if (wbIsValidClass((UINT64)pwbo)) {

		// Stores class ((UINT64)pwbo) mouse cursor in array hClassCursor
		// to be set in main loop later

        // Cursor handle
		if (handle){
			hCursor = handle;
		} else if (!pszCursor || !*pszCursor) {
		    // pszCursor is NULL (reset cursor)
			hCursor = GetSysCursor(TEXT("arrow"));
		} else {
            wcsncpy(szFile, pszCursor, MAX_PATH - 1);
            szFile[MAX_PATH - 1] = TEXT('\0');
            if(wbFindFile(szFile, MAX_PATH)) {
                // Assume it's a file path for a custom cursor
                hCursor = LoadCursorFromFile(szFile);
            } else {
                // System cursor name
                hCursor = GetSysCursor(pszCursor);
            }
        }

        // If loading from file/system fails, fall back to arrow cursor
        if (!hCursor)
        {
            hCursor = GetSysCursor(TEXT("arrow"));
        }

		hClassCursor[(UINT64)pwbo] = hCursor;
		return TRUE;
	}
	else
	{

		// Stores mouse cursor handle in control param (M_nMouseCursor)
        // to be set in main loop later
		if (!pwbo->hwnd || !IsWindow(pwbo->hwnd)){
			return FALSE;
        }

        // Cursor handle
		if (handle) {
			hCursor = handle;
		} else if (!pszCursor || !*pszCursor) {
		    // pszCursor is NULL
			hCursor = hClassCursor[pwbo->uClass];
		} else {
            wcsncpy(szFile, pszCursor, MAX_PATH - 1);
            szFile[MAX_PATH - 1] = TEXT('\0');
            if(wbFindFile(szFile, MAX_PATH)) {
                // Assume it's a file path for a custom cursor
                hCursor = LoadCursorFromFile(szFile);
                if (!hCursor)
                {
                    // If loading from file fails, fall back to arrow cursor
                    hCursor = GetSysCursor(TEXT("arrow"));
                }
            } else {
                // Cursor name
                hCursor = GetSysCursor(pszCursor);
            }
        }

        // If loading from file/system fails, fall back to arrow cursor
        if (!hCursor)
        {
            hCursor = GetSysCursor(TEXT("arrow"));
        }

		M_nMouseCursor = (LONG_PTR)hCursor;
		return (hCursor != 0);
	}
}

UINT64 wbTaskRun(PWBOBJ pwboTarget, LPCTSTR pszCommand, UINT64 estimatedMs)
{
	WB_ASYNC_TASK *task;
	SIZE_T len;

	if (!pwboTarget || !pwboTarget->hwnd || !pszCommand || !*pszCommand || !g_taskLockInit)
		return 0;

	task = (WB_ASYNC_TASK *)calloc(1, sizeof(WB_ASYNC_TASK));
	if (!task)
		return 0;

	len = (wcslen(pszCommand) + 1) * sizeof(TCHAR);
	task->command = (TCHAR *)malloc(len);
	if (!task->command)
	{
		free(task);
		return 0;
	}
	wcscpy(task->command, pszCommand);
	task->hwndTarget = pwboTarget->hwnd;
	task->estimatedMs = estimatedMs;
	task->status = WB_TASK_STATUS_PENDING;
	task->progress = 0;
	task->cancelEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (!task->cancelEvent)
	{
		free(task->command);
		free(task);
		return 0;
	}

	EnterCriticalSection(&g_taskLock);
	task->id = g_nextTaskId++;
	task->next = g_taskList;
	g_taskList = task;
	LeaveCriticalSection(&g_taskLock);

	task->thread = CreateThread(NULL, 0, wbTaskThreadProc, task, 0, NULL);
	if (!task->thread)
	{
		EnterCriticalSection(&g_taskLock);
		if (g_taskList == task)
			g_taskList = task->next;
		LeaveCriticalSection(&g_taskLock);
		CloseHandle(task->cancelEvent);
		free(task->command);
		free(task);
		return 0;
	}

	return task->id;
}

BOOL wbTaskPoll(UINT64 taskId, int *pStatus, int *pProgress, DWORD *pExitCode, DWORD *pErrorCode)
{
	WB_ASYNC_TASK *task;
	BOOL found = FALSE;

	if (!g_taskLockInit || taskId == 0)
		return FALSE;

	EnterCriticalSection(&g_taskLock);
	for (task = g_taskList; task; task = task->next)
	{
		if (task->id == taskId)
		{
			if (pStatus)
				*pStatus = (int)task->status;
			if (pProgress)
				*pProgress = (int)task->progress;
			if (pExitCode)
				*pExitCode = task->exitCode;
			if (pErrorCode)
				*pErrorCode = task->errorCode;
			found = TRUE;
			break;
		}
	}
	LeaveCriticalSection(&g_taskLock);

	return found;
}

BOOL wbTaskCancel(UINT64 taskId)
{
	WB_ASYNC_TASK *task;

	if (!g_taskLockInit || taskId == 0)
		return FALSE;

	EnterCriticalSection(&g_taskLock);
	for (task = g_taskList; task; task = task->next)
	{
		if (task->id == taskId)
		{
			SetEvent(task->cancelEvent);
			LeaveCriticalSection(&g_taskLock);
			return TRUE;
		}
	}
	LeaveCriticalSection(&g_taskLock);

	return FALSE;
}

BOOL wbTaskDequeueEvent(HWND hwndTarget, UINT64 *taskId, int *eventCode, int *progress, DWORD *value)
{
	WB_ASYNC_EVENT *current;
	WB_ASYNC_EVENT *prev = NULL;

	if (!g_eventLockInit)
		return FALSE;

	EnterCriticalSection(&g_eventLock);
	for (current = g_eventHead; current; prev = current, current = current->next)
	{
		if (!hwndTarget || current->hwndTarget == hwndTarget)
		{
			if (prev)
				prev->next = current->next;
			else
				g_eventHead = current->next;
			if (g_eventTail == current)
				g_eventTail = prev;
			break;
		}
	}
	LeaveCriticalSection(&g_eventLock);

	if (!current)
		return FALSE;

	if (taskId)
		*taskId = current->taskId;
	if (eventCode)
		*eventCode = current->eventCode;
	if (progress)
		*progress = current->progress;
	if (value)
		*value = current->value;

	free(current);
	return TRUE;
}

// Returns TRUE if uClass is a valid WinBinder class

BOOL wbIsValidClass(UINT64 uClass)
{
	switch (uClass)
	{

	case AppWindow:
	case ModalDialog:
	case ModelessDialog:
	case NakedWindow:
	case PopupWindow:
	case ResizableWindow:
	case ToolDialog:
		return TRUE;

	case Accel:
	case Calendar:
	case CheckBox:
	case ComboBox:
	case EditBox:
	case Frame:
	case Gauge:
	case HTMLControl:
	case HyperLink:
	case ImageButton:
	case InvisibleArea:
	case Label:
	case ListBox:
	case ListView:
	case Menu:
	case PushButton:
	case RTFEditBox:
	case RadioButton:
	case ScrollBar:
	case Slider:
	case Spinner:
	case StatusBar:
	case TabControl:
	case ToolBar:
	case TreeView:
	case ScintillaEdit:
		return TRUE;
	}
	return FALSE;
}

/*
	Gets the accelerator from a string like "Ctrl+S", "Alt+Ctrl+Shift+F12", "F1", "J", etc.
	Returns the key code in the LOWORD and the accelerator flags in the HIWORD.
*/

DWORD wbMakeAccelFromString(const char *pszAccel)
{
	WORD key = 0, fVirt = 0;
	char szAux[100]; // Should be enough
	char *ptr;
	int i;

	if (!pszAccel || !*pszAccel) // Ignore null or empty strings
		return 0;

	strcpy(szAux, pszAccel);
	ptr = strtok(szAux, "+");
	if (ptr && *ptr)
	{
		for (i = 0; ptr && *ptr && (i < 4); i++)
		{

			// Normal key

			if (ptr && *ptr)
				key = *ptr;

			// Up to three modifiers: ctrl, alt, shift

			if (!stricmp(ptr, "ctrl"))
				fVirt |= FCONTROL | FVIRTKEY;
			else if (!stricmp(ptr, "alt"))
				fVirt |= FALT | FVIRTKEY;
			else if (!stricmp(ptr, "shift"))
				fVirt |= FSHIFT | FVIRTKEY;

			// Special keys

			else
				fVirt |= FVIRTKEY;

			/* else */ if (!stricmp(ptr, "enter"))
				key = VK_RETURN;
			else if (!stricmp(ptr, "escape") || !stricmp(ptr, "esc"))
				key = VK_ESCAPE;
			else if (!stricmp(ptr, "tab"))
				key = VK_TAB;
			else if (!stricmp(ptr, "backspace"))
				key = VK_BACK;
			else if (!stricmp(ptr, "space"))
				key = VK_SPACE;
			else if (!stricmp(ptr, "left"))
				key = VK_LEFT;
			else if (!stricmp(ptr, "up"))
				key = VK_UP;
			else if (!stricmp(ptr, "right"))
				key = VK_RIGHT;
			else if (!stricmp(ptr, "down"))
				key = VK_DOWN;
			else if (!stricmp(ptr, "pagedown") || !stricmp(ptr, "pgdn"))
				key = VK_NEXT;
			else if (!stricmp(ptr, "pageup") || !stricmp(ptr, "pgup"))
				key = VK_PRIOR;
			else if (!stricmp(ptr, "home"))
				key = VK_HOME;
			else if (!stricmp(ptr, "end"))
				key = VK_END;
			else if (!stricmp(ptr, "insert") || !stricmp(ptr, "ins"))
				key = VK_INSERT;
			else if (!stricmp(ptr, "delete") || !stricmp(ptr, "del"))
				key = VK_DELETE;
			else if (!stricmp(ptr, "plus"))
				key = fVirt ? VK_ADD : '+';

			// F-keys

			else if ((*ptr == 'F' || *ptr == 'f') && *(ptr + 1))
			{
				key = VK_F1 - 1 + atoi(ptr + 1);
				fVirt |= FVIRTKEY;
			}

			ptr = strtok(NULL, "+");
		}
	}
	else
	{
		return 0;
	}
	//	printf("ACCEL: [%s], key %d, fVirt %d\n", pszAccel, key, fVirt);

	return MAKELONG(key, fVirt);
}

BOOL wbSetAccelerators(PWBOBJ pwbo, LPACCEL paccels, int nCount)
{
	if (hAccelTable)
		DestroyAcceleratorTable(hAccelTable);

	if (paccels && nCount)
	{
		hAccelTable = CreateAcceleratorTable(paccels, nCount);
		if (!hAccelTable)
			return FALSE;
		hAccelWnd = pwbo->hwnd;
		return TRUE;
	}
	else
		return TRUE;
}

/*

Return values:

IDNO		-1
IDCANCEL	 0
IDIGNORE	 0
IDABORT		 0
Others		 0
IDOK		 1
IDRETRY		 1
IDYES		 1

Error		-2

*/
static int wbNormalizeMessageBoxReturn(int nRet)
{
	switch (nRet)
	{
	case IDNO:
		return -1;

	case IDTIMEOUT:
		return IDTIMEOUT;

	case IDCANCEL:
	case IDIGNORE:
	case IDABORT:
	default:
		return 0;

	case IDOK:
	case IDYES:
	case IDRETRY:
		return 1;
	}
}

int wbMessageBox(PWBOBJ pwboParent, LPCTSTR pszText, LPCTSTR pszCaption, UINT64 nStyle)
{
	HWND hwndParent = NULL;
	int nRet;

	if (pwboParent)
	{
		if (!pwboParent->hwnd || !IsWindow(pwboParent->hwnd))
			return -2;
		hwndParent = pwboParent->hwnd;
	}

	nRet = MessageBox(hwndParent, pszText, pszCaption, nStyle);

	return wbNormalizeMessageBoxReturn(nRet);
}

int wbQuietMessageBox(PWBOBJ pwboParent, LPCTSTR pszText, LPCTSTR pszCaption, UINT64 nStyle, DWORD timeoutMs)
{
	typedef int(WINAPI *PFNMESSAGEBOXTIMEOUTW)(HWND, LPCWSTR, LPCWSTR, UINT, WORD, DWORD);
	static PFNMESSAGEBOXTIMEOUTW pMessageBoxTimeoutW = NULL;
	static BOOL bMessageBoxTimeoutInitialized = FALSE;
	HWND hwndParent = NULL;
	int nRet;

	if (pwboParent)
	{
		if (!pwboParent->hwnd || !IsWindow(pwboParent->hwnd))
			return -2;
		hwndParent = pwboParent->hwnd;
	}

	if (!timeoutMs)
		return wbMessageBox(pwboParent, pszText, pszCaption, nStyle);

	if (!bMessageBoxTimeoutInitialized)
	{
		HMODULE hUser32 = GetModuleHandle(TEXT("user32.dll"));
		if (hUser32)
			pMessageBoxTimeoutW = (PFNMESSAGEBOXTIMEOUTW)GetProcAddress(hUser32, "MessageBoxTimeoutW");
		bMessageBoxTimeoutInitialized = TRUE;
	}

	if (!pMessageBoxTimeoutW)
		return wbMessageBox(pwboParent, pszText, pszCaption, nStyle);

	nRet = pMessageBoxTimeoutW(hwndParent, (LPCWSTR)pszText, (LPCWSTR)pszCaption, (UINT)nStyle, 0, timeoutMs);
	if (!nRet)
		return -2;

	return wbNormalizeMessageBoxReturn(nRet);
}

//-------------------------------------------------------------- SOUND FUNCTIONS

BOOL wbPlaySound(LPCTSTR pszFileName, LPCTSTR pszCommand)
{
	// Open and/or control a media file

	PMEDIA pMd;

	pMd = wbOpenMedia(pszFileName, 0);
	if (!pMd)
	{
		return FALSE;
	}

	return wbControlMedia(pMd, pszCommand, 0);
}

BOOL wbStopSound(LPCTSTR pszCommand)
{
	if (!pszCommand || !*pszCommand || !lstrcmpi(pszCommand, TEXT("all")))
	{

		MEDIA md;

		md.wFormat = SNF_WAV;
		wbControlMedia(&md, TEXT("stop"), 0);
		md.wFormat = SNF_MIDI;
		return wbControlMedia(&md, TEXT("stop"), 0);
	}
	else if (!lstrcmpi(pszCommand, TEXT("wav")) || !lstrcmpi(pszCommand, TEXT("wave")))
	{

		MEDIA md;

		md.wFormat = SNF_WAV;
		return wbControlMedia(&md, TEXT("stop"), 0);
	}
	else if (!lstrcmpi(pszCommand, TEXT("mid")) || !lstrcmpi(pszCommand, TEXT("midi")))
	{

		MEDIA md;

		md.wFormat = SNF_MIDI;
		return wbControlMedia(&md, TEXT("stop"), 0);
	}
	else
		return FALSE;
}

// dwFlags is not used

PMEDIA wbOpenMedia(LPCTSTR pszFileName, DWORD dwFlags)
{
	PMEDIA pMd;
	HANDLE hFile;
	DWORD nBytesToRead;
	DWORD nBytesRead;
	BOOL bResult;

	if (!pszFileName || !*pszFileName)
		return NULL;

	pMd = wbMalloc(sizeof(MEDIA));

	// Determine file format according to file extension

	if (wbStriStr(pszFileName, TEXT(".wav")))
	{
		pMd->wFormat = SNF_WAV;
	}
	else if (wbStriStr(pszFileName, TEXT(".mid")))
	{
		pMd->wFormat = SNF_MIDI;
	}
	else if (wbStriStr(pszFileName, TEXT(".midi")))
	{
		pMd->wFormat = SNF_MIDI;
		//	} else if(wbStriStr(pszFileName, ".rmi")) {
		//		pMd->wFormat = SNF_MIDI;
	}
	else if (wbStriStr(pszFileName, TEXT(".avi")))
	{
		pMd->wFormat = SNF_AVI;
	}
	else
	{
		pMd->wFormat = SNF_UNKNOWN;
	}

	// For a MIDI

	switch (pMd->wFormat)
	{

	case SNF_MIDI:

		if (!EXISTFILE(pszFileName))
			return NULL;
		pMd->pvMedia = wbMalloc(sizeof(TCHAR) * (wcslen(pszFileName) + 1));
		wcscpy(pMd->pvMedia, pszFileName);
		return pMd;

	case SNF_WAV:

		// Open sound file

		hFile = CreateFile(pszFileName, GENERIC_READ, FILE_SHARE_READ,
						   (LPSECURITY_ATTRIBUTES)NULL, OPEN_EXISTING,
						   FILE_ATTRIBUTE_NORMAL,
						   (HANDLE)NULL);
		if (!hFile)
			return NULL;

		// Determine file size

		nBytesToRead = GetFileSize(hFile, NULL);
		if (!nBytesToRead)
		{
			CloseHandle(hFile);
			return NULL;
		}
		pMd->pvMedia = wbMalloc(nBytesToRead);

		// Read contents

		bResult = ReadFile(hFile, pMd->pvMedia, nBytesToRead, &nBytesRead, NULL);
		if (!bResult)
		{
			CloseHandle(hFile);
			return NULL;
		}

		CloseHandle(hFile);
		return pMd;

	default:
		return NULL;
	} // ~switch
}

// dwFlags is not used

BOOL wbControlMedia(PMEDIA pMd, LPCTSTR pszCommand, DWORD dwFlags)
{
	static BOOL bMIDIPlaying = FALSE;
	TCHAR szStr[MAX_PATH + 50];

	if (!pMd)
		return FALSE;

	switch (pMd->wFormat)
	{

	case SNF_WAV:
		if (!*pszCommand || !lstrcmpi(pszCommand, TEXT("play")))
			PlaySound(pMd->pvMedia, NULL, SND_ASYNC | SND_NOSTOP | SND_NOWAIT | SND_MEMORY);
		else if (!lstrcmpi(pszCommand, TEXT("sync")))
			PlaySound(pMd->pvMedia, NULL, SND_NOWAIT | SND_MEMORY);
		else if (!lstrcmpi(pszCommand, TEXT("loop")))
			PlaySound(pMd->pvMedia, NULL, SND_LOOP | SND_ASYNC | SND_NOSTOP | SND_NOWAIT | SND_MEMORY);
		else if (!lstrcmpi(pszCommand, TEXT("stop")))
			PlaySound(NULL, NULL, SND_ASYNC);
		//			else if(!stricmp(pszCommand, "purge"))
		//				PlaySound(NULL, NULL, SND_ASYNC | SND_PURGE);
		else
			return FALSE;
		break;

	case SNF_MIDI:

		//			if(!stricmp(pszCommand, "play")) {
		if (!*pszCommand || !lstrcmpi(pszCommand, TEXT("play")))
		{
			if (bMIDIPlaying)
			{
				mciSendString(TEXT("stop synth"), NULL, 0, NULL);
				mciSendString(TEXT("close synth"), NULL, 0, NULL);
				bMIDIPlaying = FALSE;
			}
			if (*(LPCTSTR)pMd->pvMedia)
			{
				wsprintf(szStr, TEXT("open sequencer!%s alias synth"), (LPCTSTR)pMd->pvMedia);
				mciSendString(szStr, NULL, 0, NULL);
				mciSendString(TEXT("play synth from 0"), NULL, 0, NULL);
				bMIDIPlaying = TRUE;
			}
		}
		else if (!lstrcmpi(pszCommand, TEXT("stop")))
		{

			if (bMIDIPlaying)
			{
				mciSendString(TEXT("stop synth"), NULL, 0, NULL);
				mciSendString(TEXT("close synth"), NULL, 0, NULL);
				bMIDIPlaying = FALSE;
			}
		}
		else
			return FALSE;
		break;
	}
	return TRUE;
}

BOOL wbCloseMedia(PMEDIA pMd)
{
	if (!pMd)
		return FALSE;

	if (pMd->pvMedia)
		wbFree(pMd->pvMedia);

	wbFree(pMd);

	return TRUE;
}

BOOL wbPlaySystemSound(int nStyle)
{
	switch (nStyle)
	{

	case MB_OK:
	case MB_ICONINFORMATION:
	case MB_ICONWARNING:
	case MB_ICONSTOP:
	case MB_ICONQUESTION:
		break; // nStyle is valid

	case WBC_BEEP:
		nStyle = printf("\x07"); // Beep speaker (0xFFFFFFFF doesn't work)
		return TRUE;

	default:
		nStyle = MB_OK;
	}

	return MessageBeep(nStyle);
}

//---------------------------------------------------------- END SOUND FUNCTIONS

DWORD wbExec(LPCTSTR pszPgm, LPCTSTR pszParm, BOOL bShowWindow)
{
	DWORD bRet;

	// Is pszPgm a WinBinder script? PHPW
	if (wcsstr(pszPgm, TEXT(".") WB_EXTENSION) == pszPgm + wcslen(pszPgm) - sizeof(WB_EXTENSION))
	{

		TCHAR szApp[1024];

		GetModuleFileName(NULL, szApp, 1023);

		if (pszParm && (*pszParm == 'o' || *pszParm == 'O'))
		{				  // User wants to open the file (which will usually run it, except if the user has changed the .phpw association)
			goto Execute; // Sometimes a good ol' GOTO can be useful
		}
		else if (pszParm && (*pszParm == 'c' || *pszParm == 'C'))
		{	 // User wants console mode
			; // do nothing
		}
		else if (pszParm && (*pszParm == 'w' || *pszParm == 'W'))
		{ // User wants windowed mode
			*wcsstr(szApp, TEXT(".exe")) = '\0';
			*wcscat(szApp, TEXT("-win.exe")); // PHP-specific
		}
		else
		{
			TCHAR szTitle[MAX_PATH + 1];

			// Determine console mode according to the calling program
			if (!GetConsoleTitle(szTitle, MAX_PATH))
			{
				*wcsstr(szApp, TEXT(".exe")) = '\0';
				*wcscat(szApp, TEXT("-win.exe")); // PHP-specific
			}
		}

		// Shell execute
		// If the function succeeds, it returns a value greater than 32.
		bRet = (DWORD)(ULONG_PTR)ShellExecute(GetActiveWindow(), TEXT("open"), szApp, pszPgm, NULL, bShowWindow ? SW_SHOWNORMAL : SW_HIDE);
	}
	else
	{

	Execute:
		// Shell execute
		// If the function succeeds, return PID
		SHELLEXECUTEINFO ShExInfo = {0};
		ShExInfo.cbSize = sizeof(SHELLEXECUTEINFO);
		ShExInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
		ShExInfo.hwnd = GetActiveWindow();
		ShExInfo.lpVerb = TEXT("open");
		ShExInfo.lpFile = pszPgm;
		ShExInfo.lpParameters = pszParm;
		ShExInfo.nShow = bShowWindow ? SW_SHOWNORMAL : SW_HIDE;
		bRet = ShellExecuteEx(&ShExInfo);
		if (bRet)
		{
			// The process was launched successfully
			// Retrieve the process ID (PID)
			DWORD dwPid = GetProcessId(ShExInfo.hProcess);
			// Do something with the PID...
			bRet = dwPid;

		}
		else
		{
			// Failed to launch the process
			bRet = 0;
		}
	}

	if (!bRet)
		wbError(TEXT(__FUNCTION__), MB_ICONWARNING, TEXT("Could not run application %s"), pszPgm);

	return bRet;
}

BOOL wbIsRunning(LONG pid)
{
	HANDLE phnd;
	DWORD exitCode;

	phnd = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	if(!phnd) return FALSE;

	if(!GetExitCodeProcess(phnd, &exitCode)) {
		CloseHandle(phnd);
		return FALSE;
	}
	CloseHandle(phnd);

	return exitCode == STILL_ACTIVE;
}

BOOL wbShowLastError(LPCTSTR pszCaption, BOOL bMessageBox)
{
	LPVOID lpMsgBuf;

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR)&lpMsgBuf, 0, NULL);
	if (bMessageBox)
		MessageBox(NULL, (LPCTSTR)lpMsgBuf, pszCaption, MB_OK | MB_ICONINFORMATION);
	else
		wprintf(TEXT("%s - %s\n"), pszCaption, (LPCTSTR)lpMsgBuf);
	LocalFree(lpMsgBuf);
	return TRUE;
}

/*

If pszFile exists, do nothing and return TRUE.
If not, look for pszFile in the Windows directory, then in the System directory.
If it exists in one of those, put the complete path in pszFile and return TRUE.
If not, do nothing and return FALSE.

NOTE: Do not use SearchPath(), it may crash PHP (why?)

*/

BOOL wbFindFile(LPTSTR pszFile, UINT64 uLen)
{
	TCHAR szPath[MAX_PATH * 2];

	if (EXISTFILE(pszFile))
		return TRUE;

	if (uLen > MAX_PATH)
		uLen = MAX_PATH;

	// Look for the file in the Windows directory

	GetWindowsDirectory(szPath, MAX_PATH - 1);
	if (!szPath[wcslen(szPath) - 1] != '\\')
		wcscat(szPath, TEXT("\\"));
	wcscat(szPath, pszFile);
	if (EXISTFILE(szPath))
	{
		wcscpy(pszFile, szPath);
		return TRUE;
	}

	// Look for the file in the System directory

	GetSystemDirectory(szPath, MAX_PATH - 1);
	if (!szPath[wcslen(szPath) - 1] != '\\')
		wcscat(szPath, TEXT("\\"));
	wcscat(szPath, pszFile);
	if (EXISTFILE(szPath))
	{
		wcscpy(pszFile, szPath);
		return TRUE;
	}

	// Found nothing
	//wbError(TEXT(__FUNCTION__), MB_ICONWARNING, TEXT("File %s was not found."), pszFile);

	return FALSE;
}

BOOL wbReadRegistryKey(LPCTSTR pszKey, LPTSTR pszSubKey, LPTSTR pszEntry, LPTSTR pszValue, int *pnBufLen)
{
	HKEY hMaster;
	HKEY hKey;
	DWORD dwType;

	// Get hMaster from pszKey

	hMaster = _GetKeyFromString(pszKey);
	if (!hMaster)
	{
		*pszValue = '\0';
		return FALSE;
	}

	// Convert any '/'s to '\\'s

	if (!pszEntry || !*pszEntry) // No entry?
		pszEntry = NULL;
	else
		MakeWinPath(pszEntry);
	MakeWinPath(pszSubKey);

	//printf("%s\\%s\\%s\n", pszKey, pszSubKey, pszEntry);

	// Read the data

	if (RegOpenKeyEx(hMaster, pszSubKey, 0, KEY_READ, &hKey) != ERROR_SUCCESS)
	{

		// The entry does not exist

		*pszValue = '\0';
		return FALSE;
	}

	// The entry exists

	if (RegQueryValueEx(hKey, pszEntry, NULL, &dwType, (LPBYTE)pszValue, (LPDWORD)pnBufLen) != ERROR_SUCCESS)
	{

		// The entry exists but is empty

		*pszValue = '\0';
		RegCloseKey(hKey);
		return TRUE;
	}

	RegCloseKey(hKey);

	//printf("[%d] %s\n", dwType, pszValue);
	switch (dwType)
	{

	case REG_DWORD:
	{
		DWORD dw = *((DWORD *)pszValue);

		_ltow(dw, pszValue, 10);
	}
		return TRUE;

	case REG_SZ:
	case REG_MULTI_SZ:
	case REG_EXPAND_SZ:
		return TRUE;

	case REG_BINARY:
	case REG_NONE:
	case REG_DWORD_BIG_ENDIAN:
	default:
		*pszValue = '\0';
		return FALSE;
	}
}

// If pszValue is NULL and bString is TRUE, deletes the value

BOOL wbWriteRegistryKey(LPCTSTR pszKey, LPTSTR pszSubKey, LPTSTR pszEntry, LPCTSTR pszValue, DWORD dwValue, BOOL bString)
{
	HKEY hMaster;
	HKEY hKey;
	DWORD dwDisp;

	// Get hMaster from pszKey

	hMaster = _GetKeyFromString(pszKey);
	if (!hMaster)
		return FALSE;

	// Convert any '/'s to '\\'s

	if (!pszEntry || !*pszEntry) // No entry?
		pszEntry = NULL;
	else
		MakeWinPath(pszEntry);
	MakeWinPath(pszSubKey);

	//	printf("%s %s (%d %d)\n", pszEntry, pszValue, dwValue, bString);

	// Opens or creates the entry

	if (RegCreateKeyEx(hMaster, pszSubKey, 0, NULL, REG_OPTION_NON_VOLATILE,
					   KEY_WRITE, NULL, &hKey, &dwDisp))
		return FALSE;

	if (bString && !pszValue)
	{ // Delete value
		if (RegDeleteValue(hKey, pszEntry) != ERROR_SUCCESS)
			return FALSE;
	}
	else if (bString && pszValue)
	{ // Create a string value
		/* Win32 expects REG_SZ size in bytes, including the trailing NUL terminator. */
		if (RegSetValueEx(hKey, pszEntry, 0, REG_SZ, (BYTE *)pszValue, (DWORD)((wcslen(pszValue) + 1) * sizeof(WCHAR))) != ERROR_SUCCESS)
			return FALSE;
	}
	else
	{ // Create a DWORD value
		if (RegSetValueEx(hKey, pszEntry, 0, REG_DWORD, (BYTE *)&dwValue, sizeof(DWORD)) != ERROR_SUCCESS)
			return FALSE;
	}

	if (RegCloseKey(hKey) != ERROR_SUCCESS)
		return FALSE;

	return TRUE;
}

/*
  Returns various system data (LONG_PTR or string)

  To get the value, must check the value of pbString:

  If FALSE, the value is a LONG_PTR returned by the function;
  If TRUE, the value is contained in pszString.
*/

LONG_PTR wbGetSystemInfo(LPCTSTR pszInfo, BOOL *pbIsString, LPTSTR pszString, UINT uLen)
{
	*pbIsString = FALSE;

	if (!lstrcmpi(pszInfo, L"gdiobjects"))
	{

		*pbIsString = FALSE;
		return _GetGDIObjects();
	}
	else if (!lstrcmpi(pszInfo, L"userobjects"))
	{

		*pbIsString = FALSE;
		return _GetUserObjects();
	}
	else if (!lstrcmpi(pszInfo, L"instance"))
	{

		*pbIsString = FALSE;
		return (DWORD)(ULONG_PTR)hAppInstance;
	}
	else if (!lstrcmpi(pszInfo, L"ospath"))
	{

		GetWindowsDirectory(pszString, uLen);
		*pbIsString = TRUE;
		if (*(pszString + wcslen(pszString) - 1) != '\\')
			wcscat(pszString, L"\\");
		return 0;
	}
	else if (!lstrcmpi(pszInfo, L"systempath"))
	{

		GetSystemDirectory(pszString, uLen);
		*pbIsString = TRUE;
		if (*(pszString + wcslen(pszString) - 1) != '\\')
			wcscat(pszString, L"\\");
		return 0;
	}
	else if (!lstrcmpi(pszInfo, L"fontpath"))
	{

		// Taken from comp.os.ms-windows.programmer.tools.mfc

		LPITEMIDLIST pidlFontDir;

		SHGetSpecialFolderLocation(NULL, CSIDL_FONTS, &pidlFontDir);
		SHGetPathFromIDList((LPCITEMIDLIST)pidlFontDir, pszString);
		*pbIsString = TRUE;
		if (*(pszString + wcslen(pszString) - 1) != '\\')
			wcscat(pszString, L"\\");
		return 0;
	}
	else if (!lstrcmpi(pszInfo, L"pgmpath"))
	{

		// Taken from comp.os.ms-windows.programmer.tools.mfc

		LPITEMIDLIST pidlFontDir;

		SHGetSpecialFolderLocation(NULL, CSIDL_PROGRAM_FILES, &pidlFontDir);
		SHGetPathFromIDList((LPCITEMIDLIST)pidlFontDir, pszString);
		*pbIsString = TRUE;
		if (*(pszString + wcslen(pszString) - 1) != '\\')
			wcscat(pszString, L"\\");
		return 0;
	}
	else if (!lstrcmpi(pszInfo, L"exepath"))
	{

		wcsncpy(pszString, GetCommandLine(), uLen);
		if (wcschr(pszString, ' '))
			*wcschr(pszString, ' ') = '\0';
		if (wcschr(pszString, '\\'))
			*(wcsrchr(pszString, '\\') + 1) = '\0';
		*pbIsString = TRUE;
		return 0;
	}
	else if (!lstrcmpi(pszInfo, L"diskdrives"))
	{

		int drive;
		TCHAR szAux[5];
		DWORD dwDrives = GetLogicalDrives();

		// Build a string with the drive letters

		for (*pszString = '\0', drive = 0; drive < 32; drive++)
		{
			if (((dwDrives >> drive) & 0x01) == 0x01)
			{
				wsprintf(szAux, L"%s%c:", (*pszString ? L" " : L""), 'A' + drive);
				wcscat(pszString, szAux);
			}
		}

		*pbIsString = TRUE;
		return 0;
	}
	else if (!lstrcmpi(pszInfo, L"commandline"))
	{

		wcsncpy(pszString, GetCommandLine(), uLen);
		*pbIsString = TRUE;
		return 0;
	}
	else if (!lstrcmpi(pszInfo, L"osversion"))
	{

		_GetOSVersionString(pszString);
		*pbIsString = TRUE;
		return 0;
	}
	else if (!lstrcmpi(pszInfo, L"osnumber"))
	{
		OSVERSIONINFO ovInfo;

		ovInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		GetVersionEx(&ovInfo);

		wsprintf(pszString, L"%d.%d",
				 ovInfo.dwMajorVersion,
				 ovInfo.dwMinorVersion);
		*pbIsString = TRUE;
		return 0;
	}
	else if (!lstrcmpi(pszInfo, L"consolemode"))
	{

		TCHAR title[MAX_PATH + 1];

		*pbIsString = FALSE;
		return GetConsoleTitle(title, MAX_PATH) ? TRUE : FALSE;
	}
	else if (!lstrcmpi(pszInfo, L"computername"))
	{

		GetComputerName(pszString, &uLen);
		*pbIsString = TRUE;
		return 0;
	}
	else if (!lstrcmpi(pszInfo, L"username"))
	{

		GetUserName(pszString, &uLen);
		*pbIsString = TRUE;
		return 0;
	}
	else if (!lstrcmpi(pszInfo, L"temppath"))
	{

		GetTempPath(uLen, pszString);
		*pbIsString = TRUE;
		return 0;
	}
	else if (!lstrcmpi(pszInfo, L"screenarea"))
	{

		wsprintf(pszString, L"%d %d %d %d", 0, 0,
				 GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
		*pbIsString = TRUE;
		return 0;
	}
	else if (!lstrcmpi(pszInfo, L"workarea"))
	{

		RECT rc;

		SystemParametersInfo(SPI_GETWORKAREA, sizeof(LOGFONT), &rc, 0);
		wsprintf(pszString, L"%d %d %d %d", rc.left, rc.top,
				 rc.right - rc.left, rc.bottom - rc.top);
		*pbIsString = TRUE;
		return 0;
	}
	else if (!lstrcmpi(pszInfo, L"colordepth"))
	{

		HDC hdcScreen;
		HWND hwndMain;
		int bpp;

		*pbIsString = FALSE;

		hwndMain = GetForegroundWindow();
		hdcScreen = GetDC(hwndMain);
		bpp = GetDeviceCaps(hdcScreen, BITSPIXEL) * GetDeviceCaps(hdcScreen, PLANES);
		ReleaseDC(hwndMain, hdcScreen);
		return bpp;
	}
	else if (!lstrcmpi(pszInfo, L"systemfont"))
	{

		LOGFONT lfIcon;
		HDC hdc;

		SystemParametersInfo(SPI_GETICONTITLELOGFONT, sizeof(LOGFONT), &lfIcon, 0);
		hdc = CreateCompatibleDC(NULL);
		wsprintf(pszString, L"%s %d %s%s", lfIcon.lfFaceName,
				 (int)((float)72 * (float)-lfIcon.lfHeight / GetDeviceCaps(hdc, LOGPIXELSY)),
				 (lfIcon.lfWeight >= FW_SEMIBOLD ? L"Bold" : L"Regular"),
				 (lfIcon.lfItalic ? L" Italic" : L""));
		DeleteDC(hdc);
		*pbIsString = TRUE;
		return 0;
	}
	else if (!lstrcmpi(pszInfo, L"appmemory"))
	{
		/*
		OSVERSIONINFO ovInfo;

		*pbIsString = FALSE;

		ovInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		GetVersionEx(&ovInfo);
		if(ovInfo.dwPlatformId != VER_PLATFORM_WIN32_NT)	// Only NT-based
			return 0;
		else { */

		HANDLE hProcess;
		HMODULE hLib = NULL;
		//			BOOL WINAPI (*w_GetProcessMemoryInfo)(HANDLE Process, PPROCESS_MEMORY_COUNTERS ppsmemCounters, DWORD cb);
		BOOL(WINAPI * w_GetProcessMemoryInfo)
		(HANDLE Process, PPROCESS_MEMORY_COUNTERS ppsmemCounters, DWORD cb);
		PROCESS_MEMORY_COUNTERS pmc;

		hLib = LoadLibrary(L"psapi.dll");
		if (hLib)
		{ // Is the DLL present?
			w_GetProcessMemoryInfo = (void *)GetProcAddress(hLib, "GetProcessMemoryInfo");
			if (!w_GetProcessMemoryInfo)
				return 0;
		}

		hProcess = GetCurrentProcess();
		if (!hProcess)
			return 0;

		if (w_GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc)))
		{
			//				wsprintf(pszString, "%d", pmc.WorkingSetSize);
			_ltow(pmc.WorkingSetSize, pszString, 10);
			CloseHandle(hProcess);
			return pmc.WorkingSetSize;
		}
		return 0;
		//		}
	}
	else if (!lstrcmpi(pszInfo, L"totalmemory"))
	{

		MEMORYSTATUS mst;

		mst.dwLength = sizeof(MEMORYSTATUS);
		GlobalMemoryStatus(&mst);
		*pbIsString = FALSE;
		return mst.dwTotalPhys;
	}
	else if (!lstrcmpi(pszInfo, L"freememory"))
	{

		MEMORYSTATUS mst;

		mst.dwLength = sizeof(MEMORYSTATUS);
		GlobalMemoryStatus(&mst);
		*pbIsString = FALSE;
		return mst.dwAvailPhys;
	}
	else if (!lstrcmpi(pszInfo, L"backgroundcolor"))
	{

		*pbIsString = FALSE;
		return GetSysColor(COLOR_BTNFACE);
	}
	else
	{

		wbError(TEXT(__FUNCTION__), MB_ICONWARNING, L"Unrecognized parameter \"%s\".", pszInfo);
		*pbIsString = TRUE;
		*pszString = '\0';
		return -1;
	}
}

/* Transforms a UNIX or mixed DOS/UNIX path into a Windows path. */

LPTSTR MakeWinPath(LPTSTR pszPath)
{
	if (pszPath && *pszPath)
	{
		UINT64 i;

		for (i = 0; i < wcslen(pszPath); i++)
			if (*(pszPath + i) == '/')
				*(pszPath + i) = '\\';
	}
	return pszPath;
}

//------------------------------------------------------------ PRIVATE FUNCTIONS

// Returns the hKey that corresponds to pszKey

static HKEY _GetKeyFromString(LPCTSTR pszKey)
{
	if (!lstrcmpi(pszKey, L"HKCU") || !lstrcmpi(pszKey, L"HKEY_CURRENT_USER"))
		return HKEY_CURRENT_USER;
	else if (!lstrcmpi(pszKey, L"HKLM") || !lstrcmpi(pszKey, L"HKEY_LOCAL_MACHINE"))
		return HKEY_LOCAL_MACHINE;
	else if (!lstrcmpi(pszKey, L"HKCR") || !lstrcmpi(pszKey, L"HKEY_CLASSES_ROOT"))
		return HKEY_CLASSES_ROOT;
	else if (!lstrcmpi(pszKey, L"HKU") || !lstrcmpi(pszKey, L"HKEY_USERS"))
		return HKEY_USERS;
	else if (!lstrcmpi(pszKey, L"HKCC") || !lstrcmpi(pszKey, L"HKEY_CURRENT_CONFIG"))
		return HKEY_CURRENT_CONFIG;
	else if (!lstrcmpi(pszKey, L"HKD") || !lstrcmpi(pszKey, L"HKEY_DYN_DATA"))
		return HKEY_DYN_DATA;
	else
		return NULL;
}

/* Counts User objects. */

static int _GetUserObjects(void)
{
	/*
	OSVERSIONINFO ovInfo;

	ovInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&ovInfo);

	if(ovInfo.dwPlatformId == VER_PLATFORM_WIN32_NT) { */
	return GetGuiResources(GetCurrentProcess(), GR_USEROBJECTS);
	/*	} else
		return 0; */
}

/*
Counts GDI objects.
*/

static int _GetGDIObjects(void)
{
	return GetGuiResources(GetCurrentProcess(), GR_GDIOBJECTS);
	/*
	OSVERSIONINFO ovInfo;

	ovInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&ovInfo);

	if(ovInfo.dwPlatformId == VER_PLATFORM_WIN32_NT) {
		return GetGuiResources(GetCurrentProcess(), GR_GDIOBJECTS);
	} else {

		int nObjCount = 0;
		DWORD dwObjType;
		DWORD dwhGdi;

		// Test all possible handle values

		for(dwhGdi = 0; dwhGdi < 0xFFFF; dwhGdi++) {
			dwObjType = GetObjectType((HGDIOBJ)dwhGdi);
			switch(dwObjType) {

				// 16-bit GDI heap

				case OBJ_BITMAP:
				case OBJ_BRUSH:
				case OBJ_PAL:
				case OBJ_PEN:
				case OBJ_DC:
				case OBJ_MEMDC:
				case OBJ_EXTPEN:
				case OBJ_METADC:
				case OBJ_ENHMETADC:
					if((dwhGdi & 2) && ((dwhGdi % 2) == 0) && (dwhGdi <= 0x6000))
						nObjCount++;
					break;

				// Undocumented special cases

				case OBJ_METAFILE:
				case OBJ_ENHMETAFILE:
				case OBJ_COLORSPACE:
					nObjCount++;
					break;

				// 32-bit GDI heap

				case OBJ_FONT:
				case OBJ_REGION:
					if((dwhGdi % 4) == 0)
						nObjCount++;
					break;
			}
		}

		return nObjCount;
	}
	*/
}

// This code is adapted from MSDN article 'Getting the system version':
// http://msdn.microsoft.com/library/default.asp?url=/library/en-us/sysinfo/base/getting_the_system_version.asp

static BOOL _GetOSVersionString(LPTSTR pszString)
{
	TCHAR szOS[256] = L"";
	TCHAR szVer[256] = L"";
	TCHAR szSP[256] = L"";
	OSVERSIONINFOEX osvi;
	BOOL bOsVersionInfoEx;

	// Try calling GetVersionEx using the OSVERSIONINFOEX structure.
	// if that fails, try using the OSVERSIONINFO structure.

	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	if (!(bOsVersionInfoEx = GetVersionEx((OSVERSIONINFO *)&osvi)))
	{
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		if (!GetVersionEx((OSVERSIONINFO *)&osvi))
			return FALSE;
	}

	switch (osvi.dwPlatformId)
	{

		// Test for the Windows NT product family.

	case VER_PLATFORM_WIN32_NT:

		// Test for the specific product.
		if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2)
			wcscpy(szOS, L"Microsoft Windows Server 2003,");

		if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1)
			wcscpy(szOS, L"Microsoft Windows XP");

		if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0)
			wcscpy(szOS, L"Microsoft Windows 2000");

		if (osvi.dwMajorVersion <= 4)
			wcscpy(szOS, L"Microsoft Windows NT");

		if (bOsVersionInfoEx)
		{ // Test for specific product on Windows NT 4.0 SP6 and later.

			// Test for the workstation type.

			if (osvi.wProductType == VER_NT_WORKSTATION)
			{

				if (osvi.dwMajorVersion == 4)
					wcscpy(szVer, L"Workstation 4.0");
				else if (osvi.wSuiteMask & VER_SUITE_PERSONAL)
					wcscpy(szVer, L"Home Edition");
				else
					wcscpy(szVer, L"Professional");
			}
			else if (osvi.wProductType == VER_NT_SERVER || osvi.wProductType == VER_NT_DOMAIN_CONTROLLER)
			{

				// Test for the server type.

				if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2)
				{
					if (osvi.wSuiteMask & VER_SUITE_DATACENTER)
						wcscpy(szVer, L"Datacenter Edition");
					else if (osvi.wSuiteMask & VER_SUITE_ENTERPRISE)
						wcscpy(szVer, L"Enterprise Edition");
					else if (osvi.wSuiteMask == VER_SUITE_BLADE)
						wcscpy(szVer, L"Web Edition");
					else
						wcscpy(szVer, L"Standard Edition");
				}
				else if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0)
				{
					if (osvi.wSuiteMask & VER_SUITE_DATACENTER)
						wcscpy(szVer, L"Datacenter Server");
					else if (osvi.wSuiteMask & VER_SUITE_ENTERPRISE)
						wcscpy(szVer, L"Advanced Server");
					else
						wcscpy(szVer, L"Server");
				}
				else
				{ // Windows NT 4.0
					if (osvi.wSuiteMask & VER_SUITE_ENTERPRISE)
						wcscpy(szVer, L"Server 4.0, Enterprise Edition");
					else
						wcscpy(szVer, L"Server 4.0");
				}
			}
		}
		else
		{ // Test for specific product on Windows NT 4.0 SP5 and earlier

			HKEY hKey;
			TCHAR szProductType[80];
			DWORD dwBufLen = 80;
			LONG_PTR lRet;

			lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Control\\ProductOptions", 0, KEY_QUERY_VALUE, &hKey);
			if (lRet != ERROR_SUCCESS)
				return FALSE;

			lRet = RegQueryValueEx(hKey, L"ProductType", NULL, NULL, (LPBYTE)szProductType, &dwBufLen);
			if ((lRet != ERROR_SUCCESS) || (dwBufLen > 80))
				return FALSE;

			RegCloseKey(hKey);

			if (lstrcmpi(L"WINNT", szProductType) == 0)
				wcscpy(szVer, L"Workstation");
			if (lstrcmpi(L"LANMANNT", szProductType) == 0)
				wcscpy(szVer, L"Server");
			if (lstrcmpi(L"SERVERNT", szProductType) == 0)
				wcscpy(szVer, L"Advanced Server");

			wsprintf(szSP, L"%d.%d", osvi.dwMajorVersion, osvi.dwMinorVersion);
		}

		// Display service pack (if any) and build number.

		if (osvi.dwMajorVersion == 4 && lstrcmpi(osvi.szCSDVersion, L"Service Pack 6") == 0)
		{
			HKEY hKey;
			LONG_PTR lRet;

			// Test for SP6 versus SP6a.
			lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Hotfix\\Q246009", 0, KEY_QUERY_VALUE, &hKey);
			if (lRet == ERROR_SUCCESS)
				wsprintf(szSP, L"Service Pack 6a (Build %d)", osvi.dwBuildNumber & 0xFFFF);
			else
			{
				// Windows NT 4.0 prior to SP6a
				wsprintf(szSP, L"%s (Build %d)", osvi.szCSDVersion, osvi.dwBuildNumber & 0xFFFF);
			}

			RegCloseKey(hKey);
		}
		else
		{
			// not Windows NT 4.0
			wsprintf(szSP, L"%s (Build %d)", osvi.szCSDVersion, osvi.dwBuildNumber & 0xFFFF);
		}

		break;

		// Test for Windows Me/98/95.

	case VER_PLATFORM_WIN32_WINDOWS:

		if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 0)
		{
			wcscpy(szOS, L"Microsoft Windows 95");
			if (osvi.szCSDVersion[1] == 'C' || osvi.szCSDVersion[1] == 'B')
				wsprintf(pszString, L"OSR2");
		}

		if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 10)
		{
			wcscpy(szOS, L"Microsoft Windows 98");
			if (osvi.szCSDVersion[1] == 'A')
				wsprintf(pszString, L"SE");
		}

		if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 90)
		{
			wcscpy(szOS, L"Microsoft Windows Millennium Edition");
		}
		break;

	case VER_PLATFORM_WIN32s:

		wcscpy(szOS, L"Microsoft Win32s");
		break;
	}

	wsprintf(pszString, L"%s %s %s", szOS, szVer, szSP);
	return TRUE;
}

static HCURSOR GetSysCursor(LPCTSTR pszCursor)
{
	LPTSTR pszCursorName;

	// Selects a system cursor

	if (!(pszCursor && *pszCursor))
		pszCursorName = IDC_ARROW;
	else if (!lstrcmpi(pszCursor, L"arrow"))
		pszCursorName = IDC_ARROW;
	else if (!lstrcmpi(pszCursor, L"wait"))
		pszCursorName = IDC_WAIT;
	else if (!lstrcmpi(pszCursor, L"cross"))
		pszCursorName = IDC_CROSS;
	else if (!lstrcmpi(pszCursor, L"help"))
		pszCursorName = IDC_HELP;
	else if (!lstrcmpi(pszCursor, L"waitarrow"))
		pszCursorName = IDC_APPSTARTING;
	else if (!lstrcmpi(pszCursor, L"ibeam"))
		pszCursorName = IDC_IBEAM;
	else if (!lstrcmpi(pszCursor, L"help"))
		pszCursorName = IDC_ARROW;
	else if (!lstrcmpi(pszCursor, L"finger"))
		pszCursorName = IDC_HAND;
	else if (!lstrcmpi(pszCursor, L"sizeall"))
		pszCursorName = IDC_SIZEALL;
	else if (!lstrcmpi(pszCursor, L"sizens"))
		pszCursorName = IDC_SIZENS;
	else if (!lstrcmpi(pszCursor, L"sizewe"))
		pszCursorName = IDC_SIZEWE;
	else if (!lstrcmpi(pszCursor, L"sizenesw"))
		pszCursorName = IDC_SIZENESW;
	else if (!lstrcmpi(pszCursor, L"sizenwse"))
		pszCursorName = IDC_SIZENWSE;
	else if (!lstrcmpi(pszCursor, L"uparrow"))
		pszCursorName = IDC_UPARROW;
	else if (!lstrcmpi(pszCursor, L"forbidden"))
		pszCursorName = IDC_NO;
	else if (!lstrcmpi(pszCursor, L"none"))
		return (HCURSOR)-1;
	else
	{
		wbError(TEXT(__FUNCTION__), MB_ICONWARNING, L"Cursor style \"%s\" does not exist", pszCursor);
		return NULL;
	}
	return LoadCursor(NULL, pszCursorName);
}

/*
// DEBUGGING FUNCTION
void printthem(void)
{
	int nClass;

	for(nClass = 0; nClass < NUMCLASSES; nClass++)
		printf("%04X ", hClassCursor[nClass]);
	printf("\n");
}
*/

static void wbTaskPostEvent(WB_ASYNC_TASK *task, int eventCode, int progress, DWORD value)
{
	WB_ASYNC_EVENT *evt;

	if (!task || !g_eventLockInit)
		return;

	evt = (WB_ASYNC_EVENT *)calloc(1, sizeof(WB_ASYNC_EVENT));
	if (!evt)
		return;

	evt->hwndTarget = task->hwndTarget;
	evt->taskId = task->id;
	evt->eventCode = eventCode;
	evt->progress = progress;
	evt->value = value;

	EnterCriticalSection(&g_eventLock);
	if (g_eventTail)
		g_eventTail->next = evt;
	else
		g_eventHead = evt;
	g_eventTail = evt;
	LeaveCriticalSection(&g_eventLock);

	PostMessage(task->hwndTarget, WBWM_TASK, 0, 0);
}

static DWORD WINAPI wbTaskThreadProc(LPVOID lpParam)
{
	WB_ASYNC_TASK *task = (WB_ASYNC_TASK *)lpParam;
	STARTUPINFO si = {0};
	PROCESS_INFORMATION pi = {0};
	TCHAR cmdBuffer[2048];
	DWORD waitResult;
	DWORD elapsed = 0;

	if (!task)
		return 0;

	task->status = WB_TASK_STATUS_RUNNING;
	si.cb = sizeof(si);
	_snwprintf(cmdBuffer, 2047, TEXT("cmd.exe /C %s"), task->command);
	cmdBuffer[2047] = 0;

	if (!CreateProcess(NULL, cmdBuffer, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi))
	{
		task->status = WB_TASK_STATUS_FAILED;
		task->errorCode = GetLastError();
		wbTaskPostEvent(task, WBC_TASK_ERROR, task->progress, task->errorCode);
		return 0;
	}

	while (1)
	{
		if (WaitForSingleObject(task->cancelEvent, 0) == WAIT_OBJECT_0)
		{
			TerminateProcess(pi.hProcess, 1);
			task->status = WB_TASK_STATUS_CANCELLED;
			task->exitCode = 1;
			task->progress = 0;
			wbTaskPostEvent(task, WBC_TASK_CANCELLED, task->progress, task->exitCode);
			break;
		}

		waitResult = WaitForSingleObject(pi.hProcess, 200);
		if (waitResult == WAIT_OBJECT_0)
		{
			GetExitCodeProcess(pi.hProcess, &task->exitCode);
			if (task->exitCode == 0)
			{
				task->status = WB_TASK_STATUS_COMPLETED;
				task->progress = 100;
				wbTaskPostEvent(task, WBC_TASK_COMPLETE, task->progress, task->exitCode);
			}
			else
			{
				task->status = WB_TASK_STATUS_FAILED;
				wbTaskPostEvent(task, WBC_TASK_ERROR, task->progress, task->exitCode);
			}
			break;
		}

		if (task->estimatedMs > 0)
		{
			elapsed += 200;
			task->progress = (LONG)min(99, (elapsed * 100) / task->estimatedMs);
			wbTaskPostEvent(task, WBC_TASK_PROGRESS, task->progress, 0);
		}
	}

	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
	return 0;
}

static void wbTaskCleanup(void)
{
	WB_ASYNC_TASK *task;
	WB_ASYNC_TASK *next;
	WB_ASYNC_EVENT *evt;
	WB_ASYNC_EVENT *nextEvt;

	if (g_taskLockInit)
	{
		EnterCriticalSection(&g_taskLock);
		task = g_taskList;
		g_taskList = NULL;
		LeaveCriticalSection(&g_taskLock);

		while (task)
		{
			next = task->next;
			if (task->cancelEvent)
				SetEvent(task->cancelEvent);
			if (task->thread)
			{
				WaitForSingleObject(task->thread, 500);
				CloseHandle(task->thread);
			}
			if (task->cancelEvent)
				CloseHandle(task->cancelEvent);
			if (task->command)
				free(task->command);
			free(task);
			task = next;
		}
	}

	if (g_eventLockInit)
	{
		EnterCriticalSection(&g_eventLock);
		evt = g_eventHead;
		g_eventHead = NULL;
		g_eventTail = NULL;
		LeaveCriticalSection(&g_eventLock);

		while (evt)
		{
			nextEvt = evt->next;
			free(evt);
			evt = nextEvt;
		}
	}
}

//------------------------------------------------------------ PRIVATE FUNCTIONS

/* Given a pszBuffer string, returns the nToken'th token in pszToken. Tokens are
  separated with any character from pszSep. chGroup is generally a quote or double-
  quote used to bypass pszSep. */

static char *GetTokenExt(const char *pszBuffer, int nToken, const char *pszSep, char chGroup, BOOL fBlock, char *pszToken, int nTokLen)
{
	int nBufPtr, nSepPtr, nTokPtr, nToks;

	// For each character in the buffer

	for (nBufPtr = 0, nTokPtr = 0, nToks = 0; *(pszBuffer + nBufPtr); nBufPtr++)
	{

		if (chGroup && *(pszBuffer + nBufPtr) == chGroup)
		{

			// Start of group: waituntil it ends

			nBufPtr++;

			if (nToks == nToken)
			{
				for (; *(pszBuffer + nBufPtr) && *(pszBuffer + nBufPtr) != chGroup; nBufPtr++)
				{

					// Build token

					*(pszToken + nTokPtr) = *(pszBuffer + nBufPtr);
					INCRMAX(nTokPtr, nTokLen - 1);
				}
				break;
			}
			else
				for (; *(pszBuffer + nBufPtr) && *(pszBuffer + nBufPtr) != chGroup; nBufPtr++)
					;
		}

		// Test separator characters

		for (nSepPtr = 0; *(pszSep + nSepPtr); nSepPtr++)
		{
			if (*(pszBuffer + nBufPtr) == *(pszSep + nSepPtr))
			{

				// Found a separator

				if (fBlock)

					// If fBlock is TRUE, skip next separators

					for (nSepPtr = 0; *(pszSep + nSepPtr); nSepPtr++)
						while (*(pszBuffer + nBufPtr + 1) == *(pszSep + nSepPtr))
							nBufPtr++;

				if (nToks++ > nToken)
				{

					// Token is finished, may end loop

					*(pszToken + nTokPtr) = '\0';
					return pszToken;
				}
				goto HHH; // The good ol' goto still has its uses
			}
		}

		// Build token

		if (nToks == nToken)
		{
			*(pszToken + nTokPtr) = *(pszBuffer + nBufPtr);
			INCRMAX(nTokPtr, nTokLen - 1);
		}
	HHH:;

	} // ~for

	// Reached the end of the loop

	*(pszToken + nTokPtr) = '\0';
	return pszToken;
}

// Return a whitepsace-separated token. Tokens may be grouped with double-quotes

static char *GetToken(const char *pszBuffer, int nToken, char *pszToken, int nTokLen)
{
	return GetTokenExt(pszBuffer, nToken, WHITESPACES, '\"', TRUE, pszToken, nTokLen);
}

BOOL UTF8ToUnicode16(CHAR *in_Src, WCHAR *out_Dst, INT in_MaxLen)
{
	INT lv_Len;

	if (in_MaxLen <= 0)
		return FALSE;
	lv_Len = MultiByteToWideChar(CP_UTF8, 0, in_Src, -1, out_Dst, in_MaxLen);
	if (lv_Len < 0)
		lv_Len = 0;

	if (lv_Len < in_MaxLen)
		out_Dst[lv_Len] = 0;
	else if (out_Dst[in_MaxLen - 1])
		out_Dst[0] = 0;

	return TRUE;
}
//------------------------------------------------------------------ END OF FILE
