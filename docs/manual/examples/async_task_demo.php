<?php

define('ID_RUN', 1001);
define('ID_CANCEL', 1002);
define('ID_STATUS', 1003);

$main = wb_create_window(NULL, AppWindow, 'Async task demo', WBC_CENTER, WBC_CENTER, 520, 220, WBC_NOTIFY, 0);
$runBtn = wb_create_control($main, PushButton, 'Run task', 16, 16, 110, 28, ID_RUN, 0);
$cancelBtn = wb_create_control($main, PushButton, 'Cancel', 132, 16, 110, 28, ID_CANCEL, 0);
$status = wb_create_control($main, Label, 'Idle', 16, 58, 480, 120, ID_STATUS, WBC_BORDER);

$taskId = 0;
wb_set_handler($main, 'process_main');
wb_main_loop();

function process_main($window, $id, $ctrl, $param1, $param2, $param3)
{
    global $status, $taskId;

    switch ($id) {
        case ID_RUN:
            // Any shell command can be used. estimated_ms enables progress events.
            $taskId = wb_task_run($window, 'ping 127.0.0.1 -n 8 > nul', 7000);
            wb_set_text($status, "Started task #$taskId");
            break;

        case ID_CANCEL:
            if ($taskId > 0) {
                wb_task_cancel($taskId);
            }
            break;

        case IDCLOSE:
            wb_destroy_window($window);
            break;
    }

    // Async task callback notifications arrive as $param1 values.
    if ($param1 == WBC_TASK_PROGRESS) {
        wb_set_text($status, "Task #$param2 running: $param3%");
    } elseif ($param1 == WBC_TASK_COMPLETE) {
        wb_set_text($status, "Task #$param2 completed (exit code $param3)");
    } elseif ($param1 == WBC_TASK_CANCELLED) {
        wb_set_text($status, "Task #$param2 cancelled");
    } elseif ($param1 == WBC_TASK_ERROR) {
        wb_set_text($status, "Task #$param2 failed (code $param3)");
    }
}
