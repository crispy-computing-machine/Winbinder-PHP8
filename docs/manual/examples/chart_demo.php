<?php

$win = wb_create_window(0, AppWindow, 'Chart control demo', WBC_CENTER, WBC_CENTER, 760, 520);

$chart = wb_create_control(
    $win,
    Chart,
    array('Sample', 'Value'),
    16,
    16,
    720,
    360,
    101,
    WBC_VISIBLE | WBC_BORDER
);

$btnLine = wb_create_control($win, PushButton, 'Line',    16, 392, 110, 28, 201, WBC_VISIBLE);
$btnBar = wb_create_control($win, PushButton, 'Bar',      136, 392, 110, 28, 202, WBC_VISIBLE);
$btnScatter = wb_create_control($win, PushButton, 'Scatter', 256, 392, 110, 28, 203, WBC_VISIBLE);
$btnAvg = wb_create_control($win, PushButton, 'Avg: OFF', 376, 392, 110, 28, 205, WBC_VISIBLE);
$btnRefresh = wb_create_control($win, PushButton, 'Randomize', 496, 392, 130, 28, 204, WBC_VISIBLE);
$status = wb_create_control($win, Label, 'Hover points/bars to see tooltips', 16, 430, 720, 56, 301, WBC_VISIBLE | WBC_BORDER | WBC_MULTILINE);

$GLOBALS['chart_x'] = array(1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
$GLOBALS['chart_y'] = array(14, 20, 17, 26, 24, 31, 29, 36, 33, 40);
$GLOBALS['chart_type'] = WBC_CHART_LINE;
$GLOBALS['show_avg'] = false;

if (!$chart) {
    wb_message_box($win, 'Chart control could not be created.', 'Chart demo', WBC_STOP);
    wb_destroy_window($win);
    return;
}

apply_chart($chart, $GLOBALS['chart_type']);
wb_set_handler($win, 'process_main');
wb_main_loop();

function apply_chart($chart, $chartType)
{
    $ok = wb_set_chart_data($chart, $GLOBALS['chart_x'], $GLOBALS['chart_y'], $chartType, $GLOBALS['show_avg']);
    if ($ok) {
        wb_refresh($chart);
    }

    $typeLabel = 'Line';
    if ($chartType == WBC_CHART_BAR) {
        $typeLabel = 'Bar';
    } elseif ($chartType == WBC_CHART_SCATTER) {
        $typeLabel = 'Scatter';
    }

    $avgLabel = $GLOBALS['show_avg'] ? 'ON' : 'OFF';
    wb_set_text(wb_get_control(wb_get_parent($chart), 205), "Avg: {$avgLabel}");
    wb_set_text(wb_get_control(wb_get_parent($chart), 301), "Type: {$typeLabel}\nAverage line: {$avgLabel}. Use Randomize to load fresh data.");
}

function randomize_data()
{
    $y = array();
    for ($i = 0; $i < count($GLOBALS['chart_x']); $i++) {
        $y[] = rand(10, 50);
    }
    $GLOBALS['chart_y'] = $y;
}

function process_main($window, $id, $ctrl = 0)
{
    $chart = wb_get_control($window, 101);

    switch ($id) {
        case IDCLOSE:
        case IDCANCEL:
            wb_destroy_window($window);
            return;

        case 201:
            $GLOBALS['chart_type'] = WBC_CHART_LINE;
            apply_chart($chart, $GLOBALS['chart_type']);
            return;

        case 202:
            $GLOBALS['chart_type'] = WBC_CHART_BAR;
            apply_chart($chart, $GLOBALS['chart_type']);
            return;

        case 203:
            $GLOBALS['chart_type'] = WBC_CHART_SCATTER;
            apply_chart($chart, $GLOBALS['chart_type']);
            return;

        case 204:
            randomize_data();
            apply_chart($chart, $GLOBALS['chart_type']);
            return;

        case 205:
            $GLOBALS['show_avg'] = !$GLOBALS['show_avg'];
            apply_chart($chart, $GLOBALS['chart_type']);
            return;
    }
}
