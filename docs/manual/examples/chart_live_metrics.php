<?php
$main = wb_create_window(NULL, AppWindow, 'Live Metrics', WBC_CENTER, WBC_CENTER, 720, 420, 0, 0);
$chart = wb_create_control($main, ChartControl, '', 10, 10, 690, 320, 5001, WBC_NOTIFY, WBC_CHART_POINT_CLICK | WBC_CHART_POINT_HOVER);

$labels = [];
$series = [];
for ($i = 0; $i < 30; $i++) {
    $labels[] = date('H:i:s');
    $series[] = 40 + sin($i / 2) * 20;
}

wb_chart_set_labels($chart, $labels);
wb_chart_set_series($chart, 0, $series);
wb_chart_set_colors($chart, [0x00AA66]);
wb_chart_set_axis($chart, true, true, 0, 100, true);
wb_create_timer($main, 1, 1000);

function process_main($window, $id, $ctrl, $param1, $param2)
{
    global $chart, $series, $labels;
    if ($id === 1) {
        array_shift($series);
        $series[] = 40 + mt_rand(-25, 25);
        array_shift($labels);
        $labels[] = date('H:i:s');
        wb_chart_set_labels($chart, $labels);
        wb_chart_set_series($chart, 0, $series);
        wb_chart_refresh($chart);
    } elseif ($param1 === WBC_CHART_POINT_CLICK) {
        $point = $param2 & 0xFFFF;
        wb_set_text($window, 'Live Metrics - clicked point ' . $point);
    }
}

wb_set_handler($main, 'process_main');
wb_main_loop();
