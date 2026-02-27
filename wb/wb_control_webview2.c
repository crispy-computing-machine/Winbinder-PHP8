#include "wb.h"

static HMODULE s_webview2Loader = NULL;
static FARPROC s_createEnvironment = NULL;

extern BOOL DisplayHTMLPage(PWBOBJ pwbo, LPCTSTR pszWebPageName);
extern BOOL DisplayHTMLString(PWBOBJ pwbo, LPCTSTR string);
extern BOOL ExecuteHTMLScript(PWBOBJ pwbo, LPCTSTR pszScript);

static BOOL wbEnsureWebView2Loader(void)
{
	if (s_createEnvironment)
		return TRUE;

	if (!s_webview2Loader)
		s_webview2Loader = LoadLibraryW(L"WebView2Loader.dll");

	if (!s_webview2Loader)
		return FALSE;

	s_createEnvironment = GetProcAddress(s_webview2Loader, "CreateCoreWebView2EnvironmentWithOptions");
	return s_createEnvironment != NULL;
}

BOOL wbWebView2RuntimeAvailable(void)
{
	return wbEnsureWebView2Loader();
}

BOOL wbWebView2Navigate(PWBOBJ pwbo, LPCTSTR url)
{
	if (!wbIsWBObj(pwbo, TRUE) || pwbo->uClass != WebView2Control)
		return FALSE;

	if (!wbEnsureWebView2Loader())
		return FALSE;

	return DisplayHTMLPage(pwbo, url);
}

BOOL wbWebView2SetHtml(PWBOBJ pwbo, LPCTSTR html)
{
	if (!wbIsWBObj(pwbo, TRUE) || pwbo->uClass != WebView2Control)
		return FALSE;

	if (!wbEnsureWebView2Loader())
		return FALSE;

	return DisplayHTMLString(pwbo, html);
}

BOOL wbWebView2ExecuteScript(PWBOBJ pwbo, LPCTSTR script)
{
	if (!wbIsWBObj(pwbo, TRUE) || pwbo->uClass != WebView2Control)
		return FALSE;

	if (!wbEnsureWebView2Loader())
		return FALSE;

	return ExecuteHTMLScript(pwbo, script);
}

BOOL wbWebView2DispatchScriptMessage(PWBOBJ pwbo, LPCTSTR message)
{
	if (!wbIsWBObj(pwbo, TRUE) || pwbo->uClass != WebView2Control || !message)
		return FALSE;

	if (!pwbo->parent || !pwbo->parent->hwnd)
		return FALSE;

	if ((!pwbo->pszCallBackFn || !*pwbo->pszCallBackFn) && (!pwbo->parent->pszCallBackFn || !*pwbo->parent->pszCallBackFn))
		return FALSE;

	wbCallUserFunction(pwbo->parent->pszCallBackFn, pwbo->parent->pszCallBackObj, pwbo->parent, pwbo, pwbo->id, WBC_WEBVIEW2_SCRIPT_MESSAGE, (LPARAM)message, 0);
	return TRUE;
}
