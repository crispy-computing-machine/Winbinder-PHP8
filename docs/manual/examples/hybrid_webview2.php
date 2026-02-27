<?php
$main = wb_create_window(null, AppWindow, "Hybrid WebView2 Sample", WBC_CENTER, WBC_CENTER, 900, 620, WBC_NOTIFY);
$address = wb_create_control($main, EditBox, "https://example.com", 10, 10, 620, 24, 1001);
$go = wb_create_control($main, PushButton, "Go", 640, 10, 60, 24, 1002);
$js = wb_create_control($main, PushButton, "JS", 710, 10, 60, 24, 1003);
$msg = wb_create_control($main, PushButton, "Msg", 780, 10, 60, 24, 1004);
$status = wb_create_control($main, Label, "", 10, 580, 860, 20, 1005);
$web = wb_create_control($main, WebView2Control, "", 10, 40, 860, 530, 2001);

if (!wb_webview2_available()) {
    wb_set_text($status, "WebView2 runtime not detected. Install WebView2 Runtime.");
}

function process_main($window, $id, $ctrl = null, $param1 = null, $param2 = null) {
    global $address, $web, $status;

    switch ($id) {
        case IDCLOSE:
            wb_destroy_window($window);
            return;

        case 1002:
            wb_webview2_navigate($web, wb_get_text($address));
            return;

        case 1003:
            wb_webview2_execute_script($web, "document.body.style.background='#f5f7ff';");
            return;

        case 1004:
            wb_webview2_dispatch_script_message($web, "Hello from native WinBinder");
            return;

        case 2001:
            if ($param1 == WBC_WEBVIEW2_SCRIPT_MESSAGE) {
                wb_set_text($status, "Script message: " . (string)$param2);
            }
            return;
    }
}

wb_set_handler($main, 'process_main');
wb_main_loop();
?>
