<?php
require_once __DIR__ . '/chart_wrapper.php';

$main = wb_create_window(NULL, AppWindow, 'Live Metrics', WBC_CENTER, WBC_CENTER, 720, 420, 0, 0);
$chart = WBChart::create($main, ['x'=>10, 'y'=>10, 'w'=>690, 'h'=>320, 'id'=>5001]);

$labels = [];
$series = [];
for ($i = 0; $i < 30; $i++) {
    $labels[] = date('H:i:s');
    $series[] = 40 + sin($i / 2) * 20;
}

$chart->setLabels($labels)
    ->setYLabels(['0', '25', '50', '75', '100'])
    ->setSeries(0, $series)
    ->setColors([0x00AA66])
    ->setAxis(true, true, 0, 100, true)
    ->setPopup(true, 0xE1FFFF, 0x000000, 0x4D4D4D);

wb_create_timer($main, 1, 1000);

function process_main($window, $id, $ctrl, $param1, $param2)
{
    global $chart, $series, $labels;
    if ($id === 1) {
        array_shift($series);
        $series[] = 40 + mt_rand(-25, 25);
        array_shift($labels);
        $labels[] = date('H:i:s');

        $chart->setLabels($labels)
            ->setSeries(0, $series)
            ->refresh();
    } elseif ($param1 === WBC_CHART_POINT_CLICK) {
        $point = WBChart::pointFromEvent($param2);
        wb_set_text($window, 'Live Metrics - clicked point ' . $point);
    }
}

wb_set_handler($main, 'process_main');
wb_main_loop();
