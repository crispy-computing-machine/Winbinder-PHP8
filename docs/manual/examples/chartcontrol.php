<?php
$win = wb_create_window(NULL, AppWindow, "ChartControl demo", WBC_CENTER, WBC_CENTER, 760, 480, WBC_NOTIFY);
$chart = wb_create_control($win, ChartControl, "", 20, 20, 720, 380, 201, WBC_BORDER, WBC_CHART_POINT_HOVER | WBC_CHART_POINT_CLICK);

wb_chart_set_data($chart, [
    [
        'name' => 'Revenue',
        'type' => 'line',
        'points' => [
            ['x' => 'Jan', 'y' => 21, 'label' => 'January'],
            ['x' => 'Feb', 'y' => 25, 'label' => 'February'],
            ['x' => 'Mar', 'y' => 18, 'label' => 'March'],
            ['x' => 'Apr', 'y' => 30, 'label' => 'April'],
        ],
    ],
    [
        'name' => 'Orders',
        'type' => 'scatter',
        'points' => [ [0, 10], [1, 16], [2, 12], [3, 20] ],
    ],
]);

wb_chart_set_options($chart, [
    'title' => 'Quarter snapshot',
    'x_label' => 'Month',
    'y_label' => 'Value',
    'padding' => 30,
]);

wb_chart_set_colors($chart, [RGB(33, 150, 243), RGB(255, 99, 71), RGB(76, 175, 80)]);
wb_chart_redraw($chart);

function process_main($window, $id, $ctrl, $param1, $param2)
{
    if ($id === 201 && ($param1 === WBC_CHART_POINT_HOVER || $param1 === WBC_CHART_POINT_CLICK)) {
        // point metadata is delivered as encoded indices in $param2 (series, point)
    }
    if ($id === IDCLOSE) {
        wb_destroy_window($window);
    }
}

wb_set_handler($win, "process_main");
wb_main_loop();
