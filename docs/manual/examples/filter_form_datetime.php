<?php

$win = wb_create_window(0, AppWindow, "Filter form with DateTimePicker", WBC_CENTER, WBC_CENTER, 520, 250, WBC_NOTIFY, 0);

wb_create_control($win, Label, "From:", 20, 24, 80, 22, 101, WBC_VISIBLE);
wb_create_control($win, Label, "To:", 20, 64, 80, 22, 102, WBC_VISIBLE);

$dtFrom = wb_create_control($win, DateTimePicker, "", 110, 20, 220, 24, 201, WBC_VISIBLE, WBC_DTP_ISO);
$dtTo = wb_create_control($win, DateTimePicker, "", 110, 60, 220, 24, 202, WBC_VISIBLE, WBC_DTP_ISO);

wb_set_datetime_format($dtFrom, "yyyy-MM-dd HH:mm");
wb_set_datetime_format($dtTo, "yyyy-MM-dd HH:mm");

$now = time();
$weekAgo = strtotime("-7 days", $now);

wb_set_datetime($dtFrom, $weekAgo);
wb_set_datetime($dtTo, $now);
wb_set_datetime_bounds($dtFrom, strtotime("-1 year"), $now);
wb_set_datetime_bounds($dtTo, strtotime("-1 year"), strtotime("+1 year"));

$btnApply = wb_create_control($win, PushButton, "Apply filter", 350, 20, 130, 28, 301, WBC_VISIBLE);
$btnClear = wb_create_control($win, PushButton, "Clear dates", 350, 56, 130, 28, 302, WBC_VISIBLE);
$status = wb_create_control($win, Label, "", 20, 110, 460, 90, 401, WBC_VISIBLE | WBC_BORDER | WBC_MULTILINE);

wb_set_handler($win, 'process_filter_form');
wb_main_loop();

function process_filter_form($window, $id, $ctrl = null, $param1 = 0, $param2 = 0)
{
    $from = wb_get_control($window, 201);
    $to = wb_get_control($window, 202);
    $status = wb_get_control($window, 401);

    if ($id === IDCANCEL || $id === IDCLOSE) {
        wb_destroy_window($window);
        return;
    }

    if ($id === 302) {
        wb_set_datetime_empty($from, true);
        wb_set_datetime_empty($to, true);
        wb_set_text($status, "Date filter cleared (null range)");
        return;
    }

    if ($id === 301 || $id === 201 || $id === 202) {
        $fromIso = wb_get_datetime($from, true);
        $toIso = wb_get_datetime($to, true);
        $bounds = wb_get_datetime_bounds($from, true);

        wb_set_text(
            $status,
            "From: " . ($fromIso ?: '(empty)') . "\n" .
            "To: " . ($toIso ?: '(empty)') . "\n" .
            "From min/max: " . json_encode($bounds) . "\n" .
            "Last callback payload: param1=" . $param1 . ", empty=" . $param2
        );
    }
}
