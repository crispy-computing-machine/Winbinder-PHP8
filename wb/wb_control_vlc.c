/*******************************************************************************

 WINBINDER - The native Windows binding for PHP

 Copyright  Hypervisual - see LICENSE.TXT for details
 Author: Rubem Pechansky (https://github.com/crispy-computing-machine/Winbinder)

 VLC media player integration for WinBinder controls

*******************************************************************************/

#include "wb.h"
#include <string.h>
#include <stdio.h>

typedef struct libvlc_instance_t libvlc_instance_t;
typedef struct libvlc_media_t libvlc_media_t;
typedef struct libvlc_media_player_t libvlc_media_player_t;
typedef struct libvlc_event_manager_t libvlc_event_manager_t;

typedef struct
{
	int type;
	void *object;
	union
	{
		struct
		{
			float new_position;
		} media_player_position_changed;
	} u;
} libvlc_event_t;

typedef void (*libvlc_callback_t)(const libvlc_event_t *, void *);

enum
{
	WB_LIBVLC_EVENT_PLAYING = 260,
	WB_LIBVLC_EVENT_PAUSED = 261,
	WB_LIBVLC_EVENT_END_REACHED = 265,
	WB_LIBVLC_EVENT_ENCOUNTERED_ERROR = 266
};

typedef libvlc_instance_t *(*PFN_libvlc_new)(int, const char *const *);
typedef void (*PFN_libvlc_release)(libvlc_instance_t *);
typedef libvlc_media_t *(*PFN_libvlc_media_new_location)(libvlc_instance_t *, const char *);
typedef libvlc_media_t *(*PFN_libvlc_media_new_path)(libvlc_instance_t *, const char *);
typedef void (*PFN_libvlc_media_release)(libvlc_media_t *);
typedef libvlc_media_player_t *(*PFN_libvlc_media_player_new)(libvlc_instance_t *);
typedef void (*PFN_libvlc_media_player_release)(libvlc_media_player_t *);
typedef void (*PFN_libvlc_media_player_set_media)(libvlc_media_player_t *, libvlc_media_t *);
typedef int (*PFN_libvlc_media_player_play)(libvlc_media_player_t *);
typedef void (*PFN_libvlc_media_player_pause)(libvlc_media_player_t *);
typedef void (*PFN_libvlc_media_player_stop)(libvlc_media_player_t *);
typedef int (*PFN_libvlc_audio_set_volume)(libvlc_media_player_t *, int);
typedef void (*PFN_libvlc_media_player_set_position)(libvlc_media_player_t *, float);
typedef void (*PFN_libvlc_media_player_set_hwnd)(libvlc_media_player_t *, void *);
typedef libvlc_event_manager_t *(*PFN_libvlc_media_player_event_manager)(libvlc_media_player_t *);
typedef int (*PFN_libvlc_event_attach)(libvlc_event_manager_t *, int, libvlc_callback_t, void *);
typedef void (*PFN_libvlc_event_detach)(libvlc_event_manager_t *, int, libvlc_callback_t, void *);

typedef struct
{
	HMODULE hLib;
	BOOL bLoadTried;
	BOOL bAvailable;
	PFN_libvlc_new libvlc_new;
	PFN_libvlc_release libvlc_release;
	PFN_libvlc_media_new_location libvlc_media_new_location;
	PFN_libvlc_media_new_path libvlc_media_new_path;
	PFN_libvlc_media_release libvlc_media_release;
	PFN_libvlc_media_player_new libvlc_media_player_new;
	PFN_libvlc_media_player_release libvlc_media_player_release;
	PFN_libvlc_media_player_set_media libvlc_media_player_set_media;
	PFN_libvlc_media_player_play libvlc_media_player_play;
	PFN_libvlc_media_player_pause libvlc_media_player_pause;
	PFN_libvlc_media_player_stop libvlc_media_player_stop;
	PFN_libvlc_audio_set_volume libvlc_audio_set_volume;
	PFN_libvlc_media_player_set_position libvlc_media_player_set_position;
	PFN_libvlc_media_player_set_hwnd libvlc_media_player_set_hwnd;
	PFN_libvlc_media_player_event_manager libvlc_media_player_event_manager;
	PFN_libvlc_event_attach libvlc_event_attach;
	PFN_libvlc_event_detach libvlc_event_detach;
} WBVLCLIB;

typedef struct
{
	DWORD dwMagic;
	PWBOBJ pwboHost;
	libvlc_instance_t *pInstance;
	libvlc_media_player_t *pPlayer;
	libvlc_event_manager_t *pEventMgr;
	BOOL bAttached;
} WBVLCPLAYER;

#define WB_VLC_PLAYER_MAGIC 0x564C4350 /* 'VLCP' */

static WBVLCLIB g_vlc = {0};

static BOOL WbVlcResolveSymbol(FARPROC *ppfn, const char *pszName)
{
	*ppfn = GetProcAddress(g_vlc.hLib, pszName);
	return *ppfn != NULL;
}

static BOOL WbVlcEnsureLoaded(void)
{
	if (g_vlc.bLoadTried)
		return g_vlc.bAvailable;

	g_vlc.bLoadTried = TRUE;
	g_vlc.hLib = LoadLibrary(TEXT("libvlc.dll"));
	if (!g_vlc.hLib)
		return FALSE;

	if (!WbVlcResolveSymbol((FARPROC *)&g_vlc.libvlc_new, "libvlc_new") ||
		!WbVlcResolveSymbol((FARPROC *)&g_vlc.libvlc_release, "libvlc_release") ||
		!WbVlcResolveSymbol((FARPROC *)&g_vlc.libvlc_media_new_location, "libvlc_media_new_location") ||
		!WbVlcResolveSymbol((FARPROC *)&g_vlc.libvlc_media_new_path, "libvlc_media_new_path") ||
		!WbVlcResolveSymbol((FARPROC *)&g_vlc.libvlc_media_release, "libvlc_media_release") ||
		!WbVlcResolveSymbol((FARPROC *)&g_vlc.libvlc_media_player_new, "libvlc_media_player_new") ||
		!WbVlcResolveSymbol((FARPROC *)&g_vlc.libvlc_media_player_release, "libvlc_media_player_release") ||
		!WbVlcResolveSymbol((FARPROC *)&g_vlc.libvlc_media_player_set_media, "libvlc_media_player_set_media") ||
		!WbVlcResolveSymbol((FARPROC *)&g_vlc.libvlc_media_player_play, "libvlc_media_player_play") ||
		!WbVlcResolveSymbol((FARPROC *)&g_vlc.libvlc_media_player_pause, "libvlc_media_player_pause") ||
		!WbVlcResolveSymbol((FARPROC *)&g_vlc.libvlc_media_player_stop, "libvlc_media_player_stop") ||
		!WbVlcResolveSymbol((FARPROC *)&g_vlc.libvlc_audio_set_volume, "libvlc_audio_set_volume") ||
		!WbVlcResolveSymbol((FARPROC *)&g_vlc.libvlc_media_player_set_position, "libvlc_media_player_set_position") ||
		!WbVlcResolveSymbol((FARPROC *)&g_vlc.libvlc_media_player_set_hwnd, "libvlc_media_player_set_hwnd") ||
		!WbVlcResolveSymbol((FARPROC *)&g_vlc.libvlc_media_player_event_manager, "libvlc_media_player_event_manager") ||
		!WbVlcResolveSymbol((FARPROC *)&g_vlc.libvlc_event_attach, "libvlc_event_attach") ||
		!WbVlcResolveSymbol((FARPROC *)&g_vlc.libvlc_event_detach, "libvlc_event_detach"))
	{
		FreeLibrary(g_vlc.hLib);
		ZeroMemory(&g_vlc, sizeof(g_vlc));
		g_vlc.bLoadTried = TRUE;
		return FALSE;
	}

	g_vlc.bAvailable = TRUE;
	return TRUE;
}

static WBVLCPLAYER *WbVlcGetPlayer(void *pvPlayer)
{
	WBVLCPLAYER *pPlayer = (WBVLCPLAYER *)pvPlayer;
	if (!pPlayer || pPlayer->dwMagic != WB_VLC_PLAYER_MAGIC)
		return NULL;
	return pPlayer;
}


static BOOL WbVlcBuildPluginArg(char *pszBuffer, size_t nBuffer)
{
	TCHAR szDllPath[MAX_PATH_BUFFER];
	TCHAR *pszLastSlash;
	DWORD dwAttr;
	char *pszUtf8;

	if (!pszBuffer || nBuffer < 32 || !g_vlc.hLib)
		return FALSE;

	if (!GetModuleFileName(g_vlc.hLib, szDllPath, MAX_PATH_BUFFER))
		return FALSE;

	pszLastSlash = wcsrchr(szDllPath, TEXT('\\'));
	if (!pszLastSlash)
		return FALSE;
	*pszLastSlash = 0;

	if ((wcslen(szDllPath) + wcslen(TEXT("\\plugins")) + 1) >= MAX_PATH_BUFFER)
		return FALSE;
	wcscat(szDllPath, TEXT("\\plugins"));

	dwAttr = GetFileAttributes(szDllPath);
	if (dwAttr == INVALID_FILE_ATTRIBUTES || !(dwAttr & FILE_ATTRIBUTE_DIRECTORY))
		return FALSE;

	pszUtf8 = WideChar2Utf8(szDllPath, NULL);
	if (!pszUtf8)
		return FALSE;

	snprintf(pszBuffer, nBuffer, "--plugin-path=%s", pszUtf8);
	efree(pszUtf8);
	return TRUE;
}

static void WbVlcPostState(WBVLCPLAYER *pPlayer, LPARAM state)
{
	HWND hwndParent;
	if (!pPlayer || !pPlayer->pwboHost || !pPlayer->pwboHost->parent)
		return;

	hwndParent = pPlayer->pwboHost->parent->hwnd;
	if (!hwndParent || !IsWindow(hwndParent))
		return;

	PostMessage(hwndParent, WBWM_VLCSTATE, (WPARAM)pPlayer->pwboHost->id, state);
}

static void WbVlcEventCallback(const libvlc_event_t *pEvent, void *pUserData)
{
	WBVLCPLAYER *pPlayer = (WBVLCPLAYER *)pUserData;
	if (!pPlayer || !pEvent)
		return;

	switch (pEvent->type)
	{
	case WB_LIBVLC_EVENT_PLAYING:
		WbVlcPostState(pPlayer, WBC_VLC_PLAYING);
		break;
	case WB_LIBVLC_EVENT_PAUSED:
		WbVlcPostState(pPlayer, WBC_VLC_PAUSED);
		break;
	case WB_LIBVLC_EVENT_END_REACHED:
		WbVlcPostState(pPlayer, WBC_VLC_ENDED);
		break;
	case WB_LIBVLC_EVENT_ENCOUNTERED_ERROR:
		WbVlcPostState(pPlayer, WBC_VLC_ERROR);
		break;
	}
}

BOOL wbVlcIsAvailable(void)
{
	return WbVlcEnsureLoaded();
}

void *wbVlcCreatePlayer(PWBOBJ pwboHost)
{
	const char *args[2];
	char szPluginArg[MAX_PATH_BUFFER + 32];
	int nArgs = 0;
	WBVLCPLAYER *pPlayer;

	if (!pwboHost || !pwboHost->hwnd || pwboHost->uClass != VlcMediaControl)
		return NULL;

	if (!WbVlcEnsureLoaded())
		return NULL;

	pPlayer = wbCalloc(1, sizeof(WBVLCPLAYER));
	if (!pPlayer)
		return NULL;

	pPlayer->dwMagic = WB_VLC_PLAYER_MAGIC;
	pPlayer->pwboHost = pwboHost;

	args[nArgs++] = "--no-video-title-show";
	if (WbVlcBuildPluginArg(szPluginArg, sizeof(szPluginArg)))
		args[nArgs++] = szPluginArg;

	pPlayer->pInstance = g_vlc.libvlc_new(nArgs, args);
	if (!pPlayer->pInstance)
	{
		wbFree(pPlayer);
		return NULL;
	}

	pPlayer->pPlayer = g_vlc.libvlc_media_player_new(pPlayer->pInstance);
	if (!pPlayer->pPlayer)
	{
		g_vlc.libvlc_release(pPlayer->pInstance);
		wbFree(pPlayer);
		return NULL;
	}

	g_vlc.libvlc_media_player_set_hwnd(pPlayer->pPlayer, (void *)pwboHost->hwnd);
	pPlayer->pEventMgr = g_vlc.libvlc_media_player_event_manager(pPlayer->pPlayer);
	if (pPlayer->pEventMgr)
	{
		if (g_vlc.libvlc_event_attach(pPlayer->pEventMgr, WB_LIBVLC_EVENT_PLAYING, WbVlcEventCallback, pPlayer) == 0)
			pPlayer->bAttached = TRUE;
		g_vlc.libvlc_event_attach(pPlayer->pEventMgr, WB_LIBVLC_EVENT_PAUSED, WbVlcEventCallback, pPlayer);
		g_vlc.libvlc_event_attach(pPlayer->pEventMgr, WB_LIBVLC_EVENT_END_REACHED, WbVlcEventCallback, pPlayer);
		g_vlc.libvlc_event_attach(pPlayer->pEventMgr, WB_LIBVLC_EVENT_ENCOUNTERED_ERROR, WbVlcEventCallback, pPlayer);
	}

	pwboHost->lparams[0] = (LONG_PTR)pPlayer;
	return pPlayer;
}

BOOL wbVlcDestroyPlayer(void *pvPlayer)
{
	WBVLCPLAYER *pPlayer = WbVlcGetPlayer(pvPlayer);
	if (!pPlayer)
		return FALSE;

	if (g_vlc.bAvailable && pPlayer->pEventMgr)
	{
		g_vlc.libvlc_event_detach(pPlayer->pEventMgr, WB_LIBVLC_EVENT_PLAYING, WbVlcEventCallback, pPlayer);
		g_vlc.libvlc_event_detach(pPlayer->pEventMgr, WB_LIBVLC_EVENT_PAUSED, WbVlcEventCallback, pPlayer);
		g_vlc.libvlc_event_detach(pPlayer->pEventMgr, WB_LIBVLC_EVENT_END_REACHED, WbVlcEventCallback, pPlayer);
		g_vlc.libvlc_event_detach(pPlayer->pEventMgr, WB_LIBVLC_EVENT_ENCOUNTERED_ERROR, WbVlcEventCallback, pPlayer);
	}

	if (g_vlc.bAvailable && pPlayer->pPlayer)
	{
		g_vlc.libvlc_media_player_stop(pPlayer->pPlayer);
		g_vlc.libvlc_media_player_release(pPlayer->pPlayer);
	}
	if (g_vlc.bAvailable && pPlayer->pInstance)
		g_vlc.libvlc_release(pPlayer->pInstance);

	if (pPlayer->pwboHost && pPlayer->pwboHost->lparams[0] == (LONG_PTR)pPlayer)
		pPlayer->pwboHost->lparams[0] = 0;

	pPlayer->dwMagic = 0;
	wbFree(pPlayer);
	return TRUE;
}

BOOL wbVlcSetMedia(void *pvPlayer, const char *pszMedia)
{
	WBVLCPLAYER *pPlayer = WbVlcGetPlayer(pvPlayer);
	libvlc_media_t *pMedia;
	if (!pPlayer || !pszMedia || !*pszMedia || !g_vlc.bAvailable)
		return FALSE;

	if (strstr(pszMedia, "://"))
		pMedia = g_vlc.libvlc_media_new_location(pPlayer->pInstance, pszMedia);
	else
		pMedia = g_vlc.libvlc_media_new_path(pPlayer->pInstance, pszMedia);

	if (!pMedia)
		return FALSE;

	g_vlc.libvlc_media_player_set_media(pPlayer->pPlayer, pMedia);
	if (pPlayer->pwboHost && pPlayer->pwboHost->hwnd && IsWindow(pPlayer->pwboHost->hwnd))
		g_vlc.libvlc_media_player_set_hwnd(pPlayer->pPlayer, (void *)pPlayer->pwboHost->hwnd);
	g_vlc.libvlc_media_release(pMedia);
	return TRUE;
}

BOOL wbVlcPlay(void *pvPlayer)
{
	WBVLCPLAYER *pPlayer = WbVlcGetPlayer(pvPlayer);
	if (!pPlayer || !g_vlc.bAvailable)
		return FALSE;
	if (pPlayer->pwboHost && pPlayer->pwboHost->hwnd && IsWindow(pPlayer->pwboHost->hwnd))
		g_vlc.libvlc_media_player_set_hwnd(pPlayer->pPlayer, (void *)pPlayer->pwboHost->hwnd);
	return g_vlc.libvlc_media_player_play(pPlayer->pPlayer) == 0;
}

BOOL wbVlcPause(void *pvPlayer)
{
	WBVLCPLAYER *pPlayer = WbVlcGetPlayer(pvPlayer);
	if (!pPlayer || !g_vlc.bAvailable)
		return FALSE;
	g_vlc.libvlc_media_player_pause(pPlayer->pPlayer);
	return TRUE;
}

BOOL wbVlcStop(void *pvPlayer)
{
	WBVLCPLAYER *pPlayer = WbVlcGetPlayer(pvPlayer);
	if (!pPlayer || !g_vlc.bAvailable)
		return FALSE;
	g_vlc.libvlc_media_player_stop(pPlayer->pPlayer);
	return TRUE;
}

BOOL wbVlcSetVolume(void *pvPlayer, int nVolume)
{
	WBVLCPLAYER *pPlayer = WbVlcGetPlayer(pvPlayer);
	if (!pPlayer || !g_vlc.bAvailable)
		return FALSE;
	if (nVolume < 0)
		nVolume = 0;
	if (nVolume > 200)
		nVolume = 200;
	return g_vlc.libvlc_audio_set_volume(pPlayer->pPlayer, nVolume) == 0;
}

BOOL wbVlcSetPosition(void *pvPlayer, float fPosition)
{
	WBVLCPLAYER *pPlayer = WbVlcGetPlayer(pvPlayer);
	if (!pPlayer || !g_vlc.bAvailable)
		return FALSE;
	if (fPosition < 0.0f)
		fPosition = 0.0f;
	if (fPosition > 1.0f)
		fPosition = 1.0f;
	g_vlc.libvlc_media_player_set_position(pPlayer->pPlayer, fPosition);
	return TRUE;
}

BOOL wbVlcDetachControl(PWBOBJ pwboHost)
{
	if (!pwboHost || pwboHost->uClass != VlcMediaControl)
		return FALSE;
	if (!pwboHost->lparams[0])
		return TRUE;
	return wbVlcDestroyPlayer((void *)pwboHost->lparams[0]);
}

void wbVlcShutdown(void)
{
	if (g_vlc.hLib)
		FreeLibrary(g_vlc.hLib);
	ZeroMemory(&g_vlc, sizeof(g_vlc));
}
