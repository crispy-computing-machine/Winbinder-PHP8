<?php
require_once __DIR__ . '/chart_wrapper.php';

$main = wb_create_window(NULL, AppWindow, 'Report Chart', WBC_CENTER, WBC_CENTER, 760, 460, 0, 0);
$chart = WBChart::create($main, ['x'=>15, 'y'=>15, 'w'=>720, 'h'=>360, 'id'=>6001]);

$labels = ['Q1', 'Q2', 'Q3', 'Q4'];
$revenue = [120, 140, 160, 190];
$cost = [90, 95, 105, 115];

$chart->setLabels($labels)
    ->setSeries(0, $revenue)
    ->setSeries(1, $cost)
    ->setColors([0x2D89EF, 0xE74856])
    ->setAxis(true, true, 0, 220, false)
    ->refresh();

function process_report($window, $id, $ctrl, $param1, $param2)
{
    if ($param1 === WBC_CHART_POINT_HOVER) {
        $point = WBChart::pointFromEvent($param2);
        $series = WBChart::seriesFromEvent($param2);
        wb_set_text($window, "Report Chart - hover series {$series}, point {$point}");
    }
}

wb_set_handler($main, 'process_report');
wb_main_loop();
