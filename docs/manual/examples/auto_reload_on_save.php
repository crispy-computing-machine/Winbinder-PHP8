<?php

$main = wb_create_window(0, AppWindow, "Auto-reload on save", WBC_NOTIFY, 100, 100, 640, 360);
$log = wb_create_control($main, EditBox, "Watching...", 10, 10, 620, 280, 1001, WBC_READONLY | WBC_MULTILINE | WBC_BORDER);

$watchPath = __DIR__;
$watchId = wb_watch_path($watchPath, true, 250);
if (!$watchId) {
    wb_message_box($main, "Failed to watch path: $watchPath", "Watcher", WBC_OK | WBC_WARNING);
}

function app_window_handler($window, $id, $ctrl, $lparam1, $lparam2)
{
    global $log, $watchId;

    if ($id === IDCLOSE) {
        if ($watchId) {
            wb_unwatch_path($watchId);
        }
        wb_destroy_window($window);
        return;
    }

    if ($id === 9001) {
        $events = wb_watch_poll(0);
        if (!$events) {
            return;
        }

        foreach ($events as $event) {
            $line = sprintf("[%d] %s\\%s (event=%d)", $event['watch_id'], $event['base_path'], $event['path'], $event['event']);
            $existing = wb_get_text($log);
            wb_set_text($log, $existing . PHP_EOL . $line);

            if ($event['event'] === WBE_FILE_MODIFIED && preg_match('/\\.php$/i', $event['path'])) {
                wb_set_text($log, wb_get_text($log) . PHP_EOL . " -> changed PHP file, reload action can be triggered here");
            }
        }
    }
}

wb_set_handler($main, 'app_window_handler');
wb_create_timer($main, 9001, 200);
wb_main_loop();
