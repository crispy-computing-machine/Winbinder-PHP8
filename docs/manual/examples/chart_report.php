<?php
$main = wb_create_window(NULL, AppWindow, 'Report Chart', WBC_CENTER, WBC_CENTER, 760, 460, 0, 0);
$chart = wb_create_control($main, ChartControl, '', 15, 15, 720, 360, 6001, WBC_NOTIFY, WBC_CHART_POINT_CLICK | WBC_CHART_POINT_HOVER);

$labels = ['Q1', 'Q2', 'Q3', 'Q4'];
$revenue = [120, 140, 160, 190];
$cost = [90, 95, 105, 115];

wb_chart_set_labels($chart, $labels);
wb_chart_set_series($chart, 0, $revenue);
wb_chart_set_series($chart, 1, $cost);
wb_chart_set_colors($chart, [0x2D89EF, 0xE74856]);
wb_chart_set_axis($chart, true, true, 0, 220, false);

function process_report($window, $id, $ctrl, $param1, $param2)
{
    if ($param1 === WBC_CHART_POINT_HOVER) {
        $point = $param2 & 0xFFFF;
        $series = ($param2 >> 16) & 0xFFFF;
        wb_set_text($window, "Report Chart - hover series {$series}, point {$point}");
    }
}

wb_set_handler($main, 'process_report');
wb_main_loop();
