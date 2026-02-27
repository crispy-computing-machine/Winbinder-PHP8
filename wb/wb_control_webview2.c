#include "wb.h"

/*
 * Minimal WebView2 ABI declarations to avoid compile-time dependency on WebView2 SDK headers.
 * Interfaces/method order mirror the WebView2 COM ABI used by this host implementation.
 */

typedef struct ICoreWebView2Environment ICoreWebView2Environment;
typedef struct ICoreWebView2Controller ICoreWebView2Controller;
typedef struct ICoreWebView2 ICoreWebView2;
typedef struct ICoreWebView2Settings ICoreWebView2Settings;
typedef struct ICoreWebView2WebMessageReceivedEventArgs ICoreWebView2WebMessageReceivedEventArgs;
typedef struct ICoreWebView2NavigationCompletedEventArgs ICoreWebView2NavigationCompletedEventArgs;

typedef struct EventRegistrationToken
{
	INT64 value;
} EventRegistrationToken;

typedef struct ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandlerVtbl
{
	HRESULT(STDMETHODCALLTYPE *QueryInterface)(void *This, REFIID riid, void **ppvObject);
	ULONG(STDMETHODCALLTYPE *AddRef)(void *This);
	ULONG(STDMETHODCALLTYPE *Release)(void *This);
	HRESULT(STDMETHODCALLTYPE *Invoke)(void *This, HRESULT result, ICoreWebView2Environment *createdEnvironment);
} ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandlerVtbl;

typedef struct ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler
{
	const ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandlerVtbl *lpVtbl;
} ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler;

typedef struct ICoreWebView2CreateCoreWebView2ControllerCompletedHandlerVtbl
{
	HRESULT(STDMETHODCALLTYPE *QueryInterface)(void *This, REFIID riid, void **ppvObject);
	ULONG(STDMETHODCALLTYPE *AddRef)(void *This);
	ULONG(STDMETHODCALLTYPE *Release)(void *This);
	HRESULT(STDMETHODCALLTYPE *Invoke)(void *This, HRESULT result, ICoreWebView2Controller *createdController);
} ICoreWebView2CreateCoreWebView2ControllerCompletedHandlerVtbl;

typedef struct ICoreWebView2CreateCoreWebView2ControllerCompletedHandler
{
	const ICoreWebView2CreateCoreWebView2ControllerCompletedHandlerVtbl *lpVtbl;
} ICoreWebView2CreateCoreWebView2ControllerCompletedHandler;

typedef struct ICoreWebView2WebMessageReceivedEventHandlerVtbl
{
	HRESULT(STDMETHODCALLTYPE *QueryInterface)(void *This, REFIID riid, void **ppvObject);
	ULONG(STDMETHODCALLTYPE *AddRef)(void *This);
	ULONG(STDMETHODCALLTYPE *Release)(void *This);
	HRESULT(STDMETHODCALLTYPE *Invoke)(void *This, ICoreWebView2 *sender, ICoreWebView2WebMessageReceivedEventArgs *args);
} ICoreWebView2WebMessageReceivedEventHandlerVtbl;

typedef struct ICoreWebView2WebMessageReceivedEventHandler
{
	const ICoreWebView2WebMessageReceivedEventHandlerVtbl *lpVtbl;
} ICoreWebView2WebMessageReceivedEventHandler;

typedef struct ICoreWebView2NavigationCompletedEventHandlerVtbl
{
	HRESULT(STDMETHODCALLTYPE *QueryInterface)(void *This, REFIID riid, void **ppvObject);
	ULONG(STDMETHODCALLTYPE *AddRef)(void *This);
	ULONG(STDMETHODCALLTYPE *Release)(void *This);
	HRESULT(STDMETHODCALLTYPE *Invoke)(void *This, ICoreWebView2 *sender, ICoreWebView2NavigationCompletedEventArgs *args);
} ICoreWebView2NavigationCompletedEventHandlerVtbl;

typedef struct ICoreWebView2NavigationCompletedEventHandler
{
	const ICoreWebView2NavigationCompletedEventHandlerVtbl *lpVtbl;
} ICoreWebView2NavigationCompletedEventHandler;

typedef struct ICoreWebView2ExecuteScriptCompletedHandlerVtbl
{
	HRESULT(STDMETHODCALLTYPE *QueryInterface)(void *This, REFIID riid, void **ppvObject);
	ULONG(STDMETHODCALLTYPE *AddRef)(void *This);
	ULONG(STDMETHODCALLTYPE *Release)(void *This);
	HRESULT(STDMETHODCALLTYPE *Invoke)(void *This, HRESULT errorCode, LPCWSTR resultObjectAsJson);
} ICoreWebView2ExecuteScriptCompletedHandlerVtbl;

typedef struct ICoreWebView2ExecuteScriptCompletedHandler
{
	const ICoreWebView2ExecuteScriptCompletedHandlerVtbl *lpVtbl;
} ICoreWebView2ExecuteScriptCompletedHandler;

typedef struct ICoreWebView2EnvironmentVtbl
{
	HRESULT(STDMETHODCALLTYPE *QueryInterface)(ICoreWebView2Environment *This, REFIID riid, void **ppvObject);
	ULONG(STDMETHODCALLTYPE *AddRef)(ICoreWebView2Environment *This);
	ULONG(STDMETHODCALLTYPE *Release)(ICoreWebView2Environment *This);
	HRESULT(STDMETHODCALLTYPE *CreateCoreWebView2Controller)(ICoreWebView2Environment *This, HWND parentWindow, ICoreWebView2CreateCoreWebView2ControllerCompletedHandler *handler);
} ICoreWebView2EnvironmentVtbl;

struct ICoreWebView2Environment
{
	const ICoreWebView2EnvironmentVtbl *lpVtbl;
};

typedef struct ICoreWebView2ControllerVtbl
{
	HRESULT(STDMETHODCALLTYPE *QueryInterface)(ICoreWebView2Controller *This, REFIID riid, void **ppvObject);
	ULONG(STDMETHODCALLTYPE *AddRef)(ICoreWebView2Controller *This);
	ULONG(STDMETHODCALLTYPE *Release)(ICoreWebView2Controller *This);
	HRESULT(STDMETHODCALLTYPE *get_IsVisible)(ICoreWebView2Controller *This, BOOL *isVisible);
	HRESULT(STDMETHODCALLTYPE *put_IsVisible)(ICoreWebView2Controller *This, BOOL isVisible);
	HRESULT(STDMETHODCALLTYPE *get_Bounds)(ICoreWebView2Controller *This, RECT *bounds);
	HRESULT(STDMETHODCALLTYPE *put_Bounds)(ICoreWebView2Controller *This, RECT bounds);
	HRESULT(STDMETHODCALLTYPE *get_ZoomFactor)(ICoreWebView2Controller *This, double *zoomFactor);
	HRESULT(STDMETHODCALLTYPE *put_ZoomFactor)(ICoreWebView2Controller *This, double zoomFactor);
	HRESULT(STDMETHODCALLTYPE *add_ZoomFactorChanged)(ICoreWebView2Controller *This, void *eventHandler, EventRegistrationToken *token);
	HRESULT(STDMETHODCALLTYPE *remove_ZoomFactorChanged)(ICoreWebView2Controller *This, EventRegistrationToken token);
	HRESULT(STDMETHODCALLTYPE *SetBoundsAndZoomFactor)(ICoreWebView2Controller *This, RECT bounds, double zoomFactor);
	HRESULT(STDMETHODCALLTYPE *MoveFocus)(ICoreWebView2Controller *This, int reason);
	HRESULT(STDMETHODCALLTYPE *add_MoveFocusRequested)(ICoreWebView2Controller *This, void *eventHandler, EventRegistrationToken *token);
	HRESULT(STDMETHODCALLTYPE *remove_MoveFocusRequested)(ICoreWebView2Controller *This, EventRegistrationToken token);
	HRESULT(STDMETHODCALLTYPE *add_GotFocus)(ICoreWebView2Controller *This, void *eventHandler, EventRegistrationToken *token);
	HRESULT(STDMETHODCALLTYPE *remove_GotFocus)(ICoreWebView2Controller *This, EventRegistrationToken token);
	HRESULT(STDMETHODCALLTYPE *add_LostFocus)(ICoreWebView2Controller *This, void *eventHandler, EventRegistrationToken *token);
	HRESULT(STDMETHODCALLTYPE *remove_LostFocus)(ICoreWebView2Controller *This, EventRegistrationToken token);
	HRESULT(STDMETHODCALLTYPE *add_AcceleratorKeyPressed)(ICoreWebView2Controller *This, void *eventHandler, EventRegistrationToken *token);
	HRESULT(STDMETHODCALLTYPE *remove_AcceleratorKeyPressed)(ICoreWebView2Controller *This, EventRegistrationToken token);
	HRESULT(STDMETHODCALLTYPE *NotifyParentWindowPositionChanged)(ICoreWebView2Controller *This);
	HRESULT(STDMETHODCALLTYPE *Close)(ICoreWebView2Controller *This);
	HRESULT(STDMETHODCALLTYPE *get_CoreWebView2)(ICoreWebView2Controller *This, ICoreWebView2 **coreWebView2);
} ICoreWebView2ControllerVtbl;

struct ICoreWebView2Controller
{
	const ICoreWebView2ControllerVtbl *lpVtbl;
};

typedef struct ICoreWebView2Vtbl
{
	HRESULT(STDMETHODCALLTYPE *QueryInterface)(ICoreWebView2 *This, REFIID riid, void **ppvObject);
	ULONG(STDMETHODCALLTYPE *AddRef)(ICoreWebView2 *This);
	ULONG(STDMETHODCALLTYPE *Release)(ICoreWebView2 *This);
	HRESULT(STDMETHODCALLTYPE *add_NavigationStarting)(ICoreWebView2 *This, void *eventHandler, EventRegistrationToken *token);
	HRESULT(STDMETHODCALLTYPE *remove_NavigationStarting)(ICoreWebView2 *This, EventRegistrationToken token);
	HRESULT(STDMETHODCALLTYPE *add_ContentLoading)(ICoreWebView2 *This, void *eventHandler, EventRegistrationToken *token);
	HRESULT(STDMETHODCALLTYPE *remove_ContentLoading)(ICoreWebView2 *This, EventRegistrationToken token);
	HRESULT(STDMETHODCALLTYPE *add_SourceChanged)(ICoreWebView2 *This, void *eventHandler, EventRegistrationToken *token);
	HRESULT(STDMETHODCALLTYPE *remove_SourceChanged)(ICoreWebView2 *This, EventRegistrationToken token);
	HRESULT(STDMETHODCALLTYPE *add_HistoryChanged)(ICoreWebView2 *This, void *eventHandler, EventRegistrationToken *token);
	HRESULT(STDMETHODCALLTYPE *remove_HistoryChanged)(ICoreWebView2 *This, EventRegistrationToken token);
	HRESULT(STDMETHODCALLTYPE *add_NavigationCompleted)(ICoreWebView2 *This, ICoreWebView2NavigationCompletedEventHandler *eventHandler, EventRegistrationToken *token);
	HRESULT(STDMETHODCALLTYPE *remove_NavigationCompleted)(ICoreWebView2 *This, EventRegistrationToken token);
	HRESULT(STDMETHODCALLTYPE *add_FrameNavigationStarting)(ICoreWebView2 *This, void *eventHandler, EventRegistrationToken *token);
	HRESULT(STDMETHODCALLTYPE *remove_FrameNavigationStarting)(ICoreWebView2 *This, EventRegistrationToken token);
	HRESULT(STDMETHODCALLTYPE *add_FrameNavigationCompleted)(ICoreWebView2 *This, void *eventHandler, EventRegistrationToken *token);
	HRESULT(STDMETHODCALLTYPE *remove_FrameNavigationCompleted)(ICoreWebView2 *This, EventRegistrationToken token);
	HRESULT(STDMETHODCALLTYPE *add_ScriptDialogOpening)(ICoreWebView2 *This, void *eventHandler, EventRegistrationToken *token);
	HRESULT(STDMETHODCALLTYPE *remove_ScriptDialogOpening)(ICoreWebView2 *This, EventRegistrationToken token);
	HRESULT(STDMETHODCALLTYPE *add_PermissionRequested)(ICoreWebView2 *This, void *eventHandler, EventRegistrationToken *token);
	HRESULT(STDMETHODCALLTYPE *remove_PermissionRequested)(ICoreWebView2 *This, EventRegistrationToken token);
	HRESULT(STDMETHODCALLTYPE *add_ProcessFailed)(ICoreWebView2 *This, void *eventHandler, EventRegistrationToken *token);
	HRESULT(STDMETHODCALLTYPE *remove_ProcessFailed)(ICoreWebView2 *This, EventRegistrationToken token);
	HRESULT(STDMETHODCALLTYPE *AddScriptToExecuteOnDocumentCreated)(ICoreWebView2 *This, LPCWSTR javaScript, void *handler);
	HRESULT(STDMETHODCALLTYPE *RemoveScriptToExecuteOnDocumentCreated)(ICoreWebView2 *This, LPCWSTR id);
	HRESULT(STDMETHODCALLTYPE *ExecuteScript)(ICoreWebView2 *This, LPCWSTR javaScript, ICoreWebView2ExecuteScriptCompletedHandler *handler);
	HRESULT(STDMETHODCALLTYPE *CapturePreview)(ICoreWebView2 *This, int imageFormat, void *stream, void *handler);
	HRESULT(STDMETHODCALLTYPE *Reload)(ICoreWebView2 *This);
	HRESULT(STDMETHODCALLTYPE *PostWebMessageAsJson)(ICoreWebView2 *This, LPCWSTR webMessageAsJson);
	HRESULT(STDMETHODCALLTYPE *PostWebMessageAsString)(ICoreWebView2 *This, LPCWSTR webMessageAsString);
	HRESULT(STDMETHODCALLTYPE *add_WebMessageReceived)(ICoreWebView2 *This, ICoreWebView2WebMessageReceivedEventHandler *handler, EventRegistrationToken *token);
	HRESULT(STDMETHODCALLTYPE *remove_WebMessageReceived)(ICoreWebView2 *This, EventRegistrationToken token);
	HRESULT(STDMETHODCALLTYPE *CallDevToolsProtocolMethod)(ICoreWebView2 *This, LPCWSTR methodName, LPCWSTR parametersAsJson, void *handler);
	HRESULT(STDMETHODCALLTYPE *get_BrowserProcessId)(ICoreWebView2 *This, UINT32 *value);
	HRESULT(STDMETHODCALLTYPE *get_CanGoBack)(ICoreWebView2 *This, BOOL *canGoBack);
	HRESULT(STDMETHODCALLTYPE *get_CanGoForward)(ICoreWebView2 *This, BOOL *canGoForward);
	HRESULT(STDMETHODCALLTYPE *GoBack)(ICoreWebView2 *This);
	HRESULT(STDMETHODCALLTYPE *GoForward)(ICoreWebView2 *This);
	HRESULT(STDMETHODCALLTYPE *Navigate)(ICoreWebView2 *This, LPCWSTR uri);
	HRESULT(STDMETHODCALLTYPE *NavigateToString)(ICoreWebView2 *This, LPCWSTR htmlContent);
	HRESULT(STDMETHODCALLTYPE *get_Source)(ICoreWebView2 *This, LPWSTR *uri);
	HRESULT(STDMETHODCALLTYPE *get_DocumentTitle)(ICoreWebView2 *This, LPWSTR *title);
	HRESULT(STDMETHODCALLTYPE *add_DocumentTitleChanged)(ICoreWebView2 *This, void *eventHandler, EventRegistrationToken *token);
	HRESULT(STDMETHODCALLTYPE *remove_DocumentTitleChanged)(ICoreWebView2 *This, EventRegistrationToken token);
	HRESULT(STDMETHODCALLTYPE *get_Settings)(ICoreWebView2 *This, ICoreWebView2Settings **settings);
} ICoreWebView2Vtbl;

struct ICoreWebView2
{
	const ICoreWebView2Vtbl *lpVtbl;
};

typedef struct ICoreWebView2SettingsVtbl
{
	HRESULT(STDMETHODCALLTYPE *QueryInterface)(ICoreWebView2Settings *This, REFIID riid, void **ppvObject);
	ULONG(STDMETHODCALLTYPE *AddRef)(ICoreWebView2Settings *This);
	ULONG(STDMETHODCALLTYPE *Release)(ICoreWebView2Settings *This);
	HRESULT(STDMETHODCALLTYPE *get_IsScriptEnabled)(ICoreWebView2Settings *This, BOOL *enabled);
	HRESULT(STDMETHODCALLTYPE *put_IsScriptEnabled)(ICoreWebView2Settings *This, BOOL enabled);
	HRESULT(STDMETHODCALLTYPE *get_IsWebMessageEnabled)(ICoreWebView2Settings *This, BOOL *enabled);
	HRESULT(STDMETHODCALLTYPE *put_IsWebMessageEnabled)(ICoreWebView2Settings *This, BOOL enabled);
} ICoreWebView2SettingsVtbl;

struct ICoreWebView2Settings
{
	const ICoreWebView2SettingsVtbl *lpVtbl;
};

typedef struct ICoreWebView2WebMessageReceivedEventArgsVtbl
{
	HRESULT(STDMETHODCALLTYPE *QueryInterface)(ICoreWebView2WebMessageReceivedEventArgs *This, REFIID riid, void **ppvObject);
	ULONG(STDMETHODCALLTYPE *AddRef)(ICoreWebView2WebMessageReceivedEventArgs *This);
	ULONG(STDMETHODCALLTYPE *Release)(ICoreWebView2WebMessageReceivedEventArgs *This);
	HRESULT(STDMETHODCALLTYPE *TryGetWebMessageAsString)(ICoreWebView2WebMessageReceivedEventArgs *This, LPWSTR *webMessageAsString);
} ICoreWebView2WebMessageReceivedEventArgsVtbl;

struct ICoreWebView2WebMessageReceivedEventArgs
{
	const ICoreWebView2WebMessageReceivedEventArgsVtbl *lpVtbl;
};

typedef struct ICoreWebView2NavigationCompletedEventArgsVtbl
{
	HRESULT(STDMETHODCALLTYPE *QueryInterface)(ICoreWebView2NavigationCompletedEventArgs *This, REFIID riid, void **ppvObject);
	ULONG(STDMETHODCALLTYPE *AddRef)(ICoreWebView2NavigationCompletedEventArgs *This);
	ULONG(STDMETHODCALLTYPE *Release)(ICoreWebView2NavigationCompletedEventArgs *This);
	HRESULT(STDMETHODCALLTYPE *get_IsSuccess)(ICoreWebView2NavigationCompletedEventArgs *This, BOOL *isSuccess);
} ICoreWebView2NavigationCompletedEventArgsVtbl;

struct ICoreWebView2NavigationCompletedEventArgs
{
	const ICoreWebView2NavigationCompletedEventArgsVtbl *lpVtbl;
};

typedef HRESULT(STDAPICALLTYPE *PFN_CreateCoreWebView2EnvironmentWithOptions)(PCWSTR, PCWSTR, void *, ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler *);

static HMODULE s_webview2Loader = NULL;
static PFN_CreateCoreWebView2EnvironmentWithOptions s_createEnvironment = NULL;

typedef struct
{
	ICoreWebView2Controller *controller;
	ICoreWebView2 *webview;
	ICoreWebView2WebMessageReceivedEventHandler *messageHandler;
	ICoreWebView2NavigationCompletedEventHandler *navHandler;
	EventRegistrationToken messageToken;
	EventRegistrationToken navToken;
} WBWEBVIEW2DATA;

typedef struct
{
	ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler iface;
	LONG ref;
	PWBOBJ pwbo;
	HANDLE doneEvent;
	HRESULT hr;
} ENV_HANDLER;

typedef struct
{
	ICoreWebView2CreateCoreWebView2ControllerCompletedHandler iface;
	LONG ref;
	PWBOBJ pwbo;
	HANDLE doneEvent;
	HRESULT hr;
} CTRL_HANDLER;

typedef struct
{
	ICoreWebView2WebMessageReceivedEventHandler iface;
	LONG ref;
	PWBOBJ pwbo;
} MSG_HANDLER;

typedef struct
{
	ICoreWebView2NavigationCompletedEventHandler iface;
	LONG ref;
	PWBOBJ pwbo;
} NAV_HANDLER;

typedef struct
{
	ICoreWebView2ExecuteScriptCompletedHandler iface;
	LONG ref;
} SCRIPT_HANDLER;

static HRESULT STDMETHODCALLTYPE WBQI_NotSupported(void *This, REFIID riid, void **ppvObject)
{
	UNREFERENCED_PARAMETER(This);
	UNREFERENCED_PARAMETER(riid);
	if (ppvObject)
		*ppvObject = NULL;
	return E_NOINTERFACE;
}

static ULONG STDMETHODCALLTYPE Env_AddRef(void *This)
{
	return (ULONG)InterlockedIncrement(&((ENV_HANDLER *)This)->ref);
}

static ULONG STDMETHODCALLTYPE Env_Release(void *This)
{
	LONG r = InterlockedDecrement(&((ENV_HANDLER *)This)->ref);
	if (!r)
		wbFree(This);
	return (ULONG)r;
}

static ULONG STDMETHODCALLTYPE Ctrl_AddRef(void *This)
{
	return (ULONG)InterlockedIncrement(&((CTRL_HANDLER *)This)->ref);
}

static ULONG STDMETHODCALLTYPE Ctrl_Release(void *This)
{
	LONG r = InterlockedDecrement(&((CTRL_HANDLER *)This)->ref);
	if (!r)
		wbFree(This);
	return (ULONG)r;
}

static ULONG STDMETHODCALLTYPE Msg_AddRef(void *This)
{
	return (ULONG)InterlockedIncrement(&((MSG_HANDLER *)This)->ref);
}

static ULONG STDMETHODCALLTYPE Msg_Release(void *This)
{
	LONG r = InterlockedDecrement(&((MSG_HANDLER *)This)->ref);
	if (!r)
		wbFree(This);
	return (ULONG)r;
}

static ULONG STDMETHODCALLTYPE Nav_AddRef(void *This)
{
	return (ULONG)InterlockedIncrement(&((NAV_HANDLER *)This)->ref);
}

static ULONG STDMETHODCALLTYPE Nav_Release(void *This)
{
	LONG r = InterlockedDecrement(&((NAV_HANDLER *)This)->ref);
	if (!r)
		wbFree(This);
	return (ULONG)r;
}


static ULONG STDMETHODCALLTYPE Script_AddRef(void *This)
{
	return (ULONG)InterlockedIncrement(&((SCRIPT_HANDLER *)This)->ref);
}

static ULONG STDMETHODCALLTYPE Script_Release(void *This)
{
	LONG r = InterlockedDecrement(&((SCRIPT_HANDLER *)This)->ref);
	if (!r)
		wbFree(This);
	return (ULONG)r;
}

static HRESULT STDMETHODCALLTYPE Script_Invoke(void *This, HRESULT errorCode, LPCWSTR resultObjectAsJson)
{
	UNREFERENCED_PARAMETER(This);
	UNREFERENCED_PARAMETER(errorCode);
	UNREFERENCED_PARAMETER(resultObjectAsJson);
	return S_OK;
}

static void wbWebView2Dispatch(PWBOBJ pwbo, LPARAM eventType, LPARAM payload)
{
	if (!pwbo || !pwbo->parent)
		return;

	if ((!pwbo->pszCallBackFn || !*pwbo->pszCallBackFn) && (!pwbo->parent->pszCallBackFn || !*pwbo->parent->pszCallBackFn))
		return;

	wbCallUserFunction(pwbo->parent->pszCallBackFn, pwbo->parent->pszCallBackObj, pwbo->parent, pwbo, pwbo->id, eventType, payload, 0);
}

static HRESULT STDMETHODCALLTYPE Msg_Invoke(void *This, ICoreWebView2 *sender, ICoreWebView2WebMessageReceivedEventArgs *args)
{
	LPWSTR msg = NULL;
	PWBOBJ pwbo = ((MSG_HANDLER *)This)->pwbo;
	UNREFERENCED_PARAMETER(sender);

	if (args && args->lpVtbl->TryGetWebMessageAsString && SUCCEEDED(args->lpVtbl->TryGetWebMessageAsString(args, &msg)) && msg)
	{
		SIZE_T len = wcslen(msg);
		LPTSTR copy = wbMalloc((len + 1) * sizeof(TCHAR));
		if (copy)
		{
			wcscpy(copy, msg);
			wbWebView2Dispatch(pwbo, WBC_WEBVIEW2_SCRIPT_MESSAGE, (LPARAM)copy);
			wbFree(copy);
		}
		CoTaskMemFree(msg);
	}
	else
	{
		wbWebView2Dispatch(pwbo, WBC_WEBVIEW2_SCRIPT_MESSAGE, (LPARAM)TEXT(""));
	}

	return S_OK;
}

static HRESULT STDMETHODCALLTYPE Nav_Invoke(void *This, ICoreWebView2 *sender, ICoreWebView2NavigationCompletedEventArgs *args)
{
	BOOL ok = TRUE;
	PWBOBJ pwbo = ((NAV_HANDLER *)This)->pwbo;
	UNREFERENCED_PARAMETER(sender);

	if (args && args->lpVtbl->get_IsSuccess)
		args->lpVtbl->get_IsSuccess(args, &ok);

	wbWebView2Dispatch(pwbo, WBC_WEBVIEW2_NAV_COMPLETED, (LPARAM)ok);
	return S_OK;
}

static const ICoreWebView2WebMessageReceivedEventHandlerVtbl g_msgVt = {
	WBQI_NotSupported,
	Msg_AddRef,
	Msg_Release,
	Msg_Invoke};

static const ICoreWebView2NavigationCompletedEventHandlerVtbl g_navVt = {
	WBQI_NotSupported,
	Nav_AddRef,
	Nav_Release,
	Nav_Invoke};


static const ICoreWebView2ExecuteScriptCompletedHandlerVtbl g_scriptVt = {
	WBQI_NotSupported,
	Script_AddRef,
	Script_Release,
	Script_Invoke};


static BOOL wbBuildWebView2UserDataDir(LPWSTR pathBuffer, DWORD cchPathBuffer)
{
	WCHAR tempPath[MAX_PATH];
	WCHAR baseDir[MAX_PATH];
	DWORD n;

	if (!pathBuffer || cchPathBuffer == 0)
		return FALSE;

	n = GetTempPathW(MAX_PATH, tempPath);
	if (!n || n >= MAX_PATH)
		return FALSE;

	if (_snwprintf(baseDir, MAX_PATH, L"%lsWinBinder", tempPath) < 0)
		return FALSE;

	CreateDirectoryW(baseDir, NULL);

	if (_snwprintf(pathBuffer, cchPathBuffer, L"%ls\\WebView2Data", baseDir) < 0)
		return FALSE;

	CreateDirectoryW(pathBuffer, NULL);
	return TRUE;
}

static BOOL wbEnsureWebView2Loader(void)
{
	if (s_createEnvironment)
		return TRUE;

	if (!s_webview2Loader)
		s_webview2Loader = LoadLibraryW(L"WebView2Loader.dll");

	if (!s_webview2Loader)
		return FALSE;

	s_createEnvironment = (PFN_CreateCoreWebView2EnvironmentWithOptions)GetProcAddress(s_webview2Loader, "CreateCoreWebView2EnvironmentWithOptions");
	return s_createEnvironment != NULL;
}

static ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandlerVtbl g_envVtMutable;
static ICoreWebView2CreateCoreWebView2ControllerCompletedHandlerVtbl g_ctrlVtMutable;

static HRESULT STDMETHODCALLTYPE Env_Invoke(void *This, HRESULT result, ICoreWebView2Environment *createdEnvironment)
{
	ENV_HANDLER *h = (ENV_HANDLER *)This;
	CTRL_HANDLER *c;
	HRESULT hr;

	h->hr = result;
	if (FAILED(result) || !createdEnvironment)
	{
		SetEvent(h->doneEvent);
		return S_OK;
	}

	c = (CTRL_HANDLER *)wbMalloc(sizeof(CTRL_HANDLER));
	if (!c)
	{
		h->hr = E_OUTOFMEMORY;
		SetEvent(h->doneEvent);
		return S_OK;
	}
	memset(c, 0, sizeof(*c));
	c->iface.lpVtbl = &g_ctrlVtMutable;
	c->ref = 1;
	c->pwbo = h->pwbo;
	c->doneEvent = h->doneEvent;
	c->hr = E_FAIL;

	hr = createdEnvironment->lpVtbl->CreateCoreWebView2Controller(createdEnvironment, h->pwbo->hwnd, &c->iface);
	if (FAILED(hr))
	{
		h->hr = hr;
		Ctrl_Release(c);
		SetEvent(h->doneEvent);
	}
	createdEnvironment->lpVtbl->Release(createdEnvironment);
	return S_OK;
}

static HRESULT STDMETHODCALLTYPE Ctrl_Invoke(void *This, HRESULT result, ICoreWebView2Controller *createdController)
{
	CTRL_HANDLER *h = (CTRL_HANDLER *)This;
	WBWEBVIEW2DATA *data;
	RECT rc;

	h->hr = result;
	if (FAILED(result) || !createdController)
	{
		SetEvent(h->doneEvent);
		return S_OK;
	}

	data = (WBWEBVIEW2DATA *)wbMalloc(sizeof(WBWEBVIEW2DATA));
	if (!data)
	{
		h->hr = E_OUTOFMEMORY;
		SetEvent(h->doneEvent);
		return S_OK;
	}
	memset(data, 0, sizeof(*data));
	data->controller = createdController;
	createdController->lpVtbl->AddRef(createdController);

	if (FAILED(createdController->lpVtbl->get_CoreWebView2(createdController, &data->webview)) || !data->webview)
	{
		createdController->lpVtbl->Release(createdController);
		wbFree(data);
		h->hr = E_FAIL;
		SetEvent(h->doneEvent);
		return S_OK;
	}

	GetClientRect(h->pwbo->hwnd, &rc);
	createdController->lpVtbl->put_Bounds(createdController, rc);

	if (data->webview->lpVtbl->get_Settings)
	{
		ICoreWebView2Settings *settings = NULL;
		if (SUCCEEDED(data->webview->lpVtbl->get_Settings(data->webview, &settings)) && settings)
		{
			if (settings->lpVtbl->put_IsWebMessageEnabled)
				settings->lpVtbl->put_IsWebMessageEnabled(settings, TRUE);
			settings->lpVtbl->Release(settings);
		}
	}

	h->pwbo->lparams[0] = (LONG_PTR)data;
	SetEvent(h->doneEvent);
	return S_OK;
}

static ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandlerVtbl g_envVtMutable = {
	WBQI_NotSupported,
	Env_AddRef,
	Env_Release,
	Env_Invoke};

static ICoreWebView2CreateCoreWebView2ControllerCompletedHandlerVtbl g_ctrlVtMutable = {
	WBQI_NotSupported,
	Ctrl_AddRef,
	Ctrl_Release,
	Ctrl_Invoke};

static BOOL wbWebView2HookEvents(PWBOBJ pwbo)
{
	WBWEBVIEW2DATA *data = (WBWEBVIEW2DATA *)pwbo->lparams[0];
	MSG_HANDLER *mh;
	NAV_HANDLER *nh;

	if (!data || !data->webview)
		return FALSE;

	mh = (MSG_HANDLER *)wbMalloc(sizeof(MSG_HANDLER));
	nh = (NAV_HANDLER *)wbMalloc(sizeof(NAV_HANDLER));
	if (!mh || !nh)
	{
		if (mh)
			wbFree(mh);
		if (nh)
			wbFree(nh);
		return FALSE;
	}
	memset(mh, 0, sizeof(*mh));
	memset(nh, 0, sizeof(*nh));
	mh->iface.lpVtbl = &g_msgVt;
	mh->ref = 1;
	mh->pwbo = pwbo;
	nh->iface.lpVtbl = &g_navVt;
	nh->ref = 1;
	nh->pwbo = pwbo;

	if (FAILED(data->webview->lpVtbl->add_WebMessageReceived(data->webview, &mh->iface, &data->messageToken)))
	{
		Msg_Release(mh);
		Nav_Release(nh);
		return FALSE;
	}
	if (FAILED(data->webview->lpVtbl->add_NavigationCompleted(data->webview, &nh->iface, &data->navToken)))
	{
		data->webview->lpVtbl->remove_WebMessageReceived(data->webview, data->messageToken);
		Msg_Release(mh);
		Nav_Release(nh);
		return FALSE;
	}

	data->messageHandler = (ICoreWebView2WebMessageReceivedEventHandler *)mh;
	data->navHandler = (ICoreWebView2NavigationCompletedEventHandler *)nh;
	return TRUE;
}

BOOL wbWebView2RuntimeAvailable(void)
{
	return wbEnsureWebView2Loader();
}

BOOL wbWebView2InitControl(PWBOBJ pwbo)
{
	ENV_HANDLER *env;
	HANDLE doneEvent;
	DWORD wait;
	MSG msg;
	WCHAR userDataDir[MAX_PATH];
	LPCWSTR pUserDataDir = NULL;

	if (!wbIsWBObj(pwbo, TRUE) || pwbo->uClass != WebView2Control)
		return FALSE;

	if (!wbEnsureWebView2Loader())
		return FALSE;

	doneEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (!doneEvent)
		return FALSE;

	env = (ENV_HANDLER *)wbMalloc(sizeof(ENV_HANDLER));
	if (!env)
	{
		CloseHandle(doneEvent);
		return FALSE;
	}
	memset(env, 0, sizeof(*env));
	env->iface.lpVtbl = &g_envVtMutable;
	env->ref = 1;
	env->pwbo = pwbo;
	env->doneEvent = doneEvent;
	env->hr = E_FAIL;

	if (wbBuildWebView2UserDataDir(userDataDir, MAX_PATH))
		pUserDataDir = userDataDir;

	if (FAILED(s_createEnvironment(NULL, pUserDataDir, NULL, &env->iface)))
	{
		Env_Release(env);
		CloseHandle(doneEvent);
		return FALSE;
	}

	for (;;)
	{
		wait = MsgWaitForMultipleObjects(1, &doneEvent, FALSE, 3000, QS_ALLINPUT);
		if (wait == WAIT_OBJECT_0)
			break;
		if (wait == WAIT_OBJECT_0 + 1)
		{
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			continue;
		}
		break;
	}

	Env_Release(env);
	CloseHandle(doneEvent);

	if (!pwbo->lparams[0])
		return FALSE;

	if (!wbWebView2HookEvents(pwbo))
	{
		wbWebView2Destroy(pwbo);
		return FALSE;
	}

	return TRUE;
}

void wbWebView2Resize(PWBOBJ pwbo, int width, int height)
{
	WBWEBVIEW2DATA *data;
	RECT rc;

	if (!pwbo || pwbo->uClass != WebView2Control)
		return;

	data = (WBWEBVIEW2DATA *)pwbo->lparams[0];
	if (!data || !data->controller)
		return;

	rc.left = 0;
	rc.top = 0;
	rc.right = width;
	rc.bottom = height;
	data->controller->lpVtbl->put_Bounds(data->controller, rc);
}

void wbWebView2Destroy(PWBOBJ pwbo)
{
	WBWEBVIEW2DATA *data;

	if (!pwbo)
		return;

	data = (WBWEBVIEW2DATA *)pwbo->lparams[0];
	if (!data)
		return;

	if (data->webview)
	{
		if (data->messageToken.value && data->webview->lpVtbl->remove_WebMessageReceived)
			data->webview->lpVtbl->remove_WebMessageReceived(data->webview, data->messageToken);
		if (data->navToken.value && data->webview->lpVtbl->remove_NavigationCompleted)
			data->webview->lpVtbl->remove_NavigationCompleted(data->webview, data->navToken);
	}

	if (data->messageHandler)
		data->messageHandler->lpVtbl->Release(data->messageHandler);
	if (data->navHandler)
		data->navHandler->lpVtbl->Release(data->navHandler);
	if (data->webview)
		data->webview->lpVtbl->Release(data->webview);
	if (data->controller)
	{
		if (data->controller->lpVtbl->Close)
			data->controller->lpVtbl->Close(data->controller);
		data->controller->lpVtbl->Release(data->controller);
	}

	wbFree(data);
	pwbo->lparams[0] = 0;
}

BOOL wbWebView2Navigate(PWBOBJ pwbo, LPCTSTR url)
{
	WBWEBVIEW2DATA *data = (WBWEBVIEW2DATA *)pwbo->lparams[0];
	if (!wbIsWBObj(pwbo, TRUE) || pwbo->uClass != WebView2Control || !url || !data || !data->webview)
		return FALSE;
	return SUCCEEDED(data->webview->lpVtbl->Navigate(data->webview, url));
}

BOOL wbWebView2SetHtml(PWBOBJ pwbo, LPCTSTR html)
{
	WBWEBVIEW2DATA *data = (WBWEBVIEW2DATA *)pwbo->lparams[0];
	if (!wbIsWBObj(pwbo, TRUE) || pwbo->uClass != WebView2Control || !html || !data || !data->webview)
		return FALSE;
	return SUCCEEDED(data->webview->lpVtbl->NavigateToString(data->webview, html));
}

BOOL wbWebView2ExecuteScript(PWBOBJ pwbo, LPCTSTR script)
{
	WBWEBVIEW2DATA *data = (WBWEBVIEW2DATA *)pwbo->lparams[0];
	if (!wbIsWBObj(pwbo, TRUE) || pwbo->uClass != WebView2Control || !script || !data || !data->webview)
		return FALSE;
	SCRIPT_HANDLER *sh = (SCRIPT_HANDLER *)wbMalloc(sizeof(SCRIPT_HANDLER));
	BOOL ok;
	if (!sh)
		return FALSE;
	memset(sh, 0, sizeof(*sh));
	sh->iface.lpVtbl = &g_scriptVt;
	sh->ref = 1;
	ok = SUCCEEDED(data->webview->lpVtbl->ExecuteScript(data->webview, script, &sh->iface));
	Script_Release(sh);
	return ok;
}

BOOL wbWebView2DispatchScriptMessage(PWBOBJ pwbo, LPCTSTR message)
{
	WBWEBVIEW2DATA *data = (WBWEBVIEW2DATA *)pwbo->lparams[0];
	if (!wbIsWBObj(pwbo, TRUE) || pwbo->uClass != WebView2Control || !message || !data || !data->webview)
		return FALSE;
	return SUCCEEDED(data->webview->lpVtbl->PostWebMessageAsString(data->webview, message));
}
