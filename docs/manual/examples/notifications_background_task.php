<?php

$window = wb_create_window(0, AppWindow, "Notification API demo", WBC_CENTER, WBC_CENTER, 500, 220, WBC_VISIBLE);
$status = wb_create_control($window, Label, "Click Start to run a background task.", 10, 20, 470, 40, 101, WBC_VISIBLE | WBC_MULTILINE);
$start = wb_create_control($window, PushButton, "Start", 10, 80, 100, 32, 201, WBC_VISIBLE);

wb_set_handler($window, "process_main");
wb_main_loop();

function process_main($window, $id)
{
    if ($id === IDCANCEL || $id === IDOK) {
        wb_destroy_window($window);
        return;
    }

    if ($id !== 201) {
        return;
    }

    wb_set_text(wb_get_control($window, 101), "Running background task...");

    // Simulate a non-blocking background workload.
    wb_wait(0, 1500);

    wb_notify([
        'parent' => $window,
        'title' => 'Import finished',
        'body' => 'Background task completed successfully.',
        'icon' => 'info',
        'duration' => 3500,
    ]);

    wb_set_text(wb_get_control($window, 101), "Task done. Notification dispatched.");
}

