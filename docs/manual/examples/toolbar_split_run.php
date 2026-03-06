<?php

include "../phpwb.php";

define('ID_RUN', 1001);
define('ID_RUN_DEFAULT', 1100);
define('ID_RUN_SAFE', 1101);
define('ID_RUN_VERBOSE', 1102);

$win = wb_create_window(null, AppWindow, 'Split toolbar demo', WBC_CENTER, WBC_CENTER, 520, 220, 0);

$menu = wb_create_menu($win, [
    ['Run', ID_RUN_DEFAULT],
    ['Run safely', ID_RUN_SAFE],
    ['Run with verbose logging', ID_RUN_VERBOSE],
]);

$toolbarItems = [
    [ID_RUN, '', 'Run', 0],
];

$toolbar = wb_create_toolbar($win, $toolbarItems, 16, 16, null);
wb_toolbar_attach_split_menu($toolbar, ID_RUN, $menu);
wb_toolbar_set_split_default($toolbar, ID_RUN, ID_RUN_DEFAULT);

$status = wb_create_control($win, Label, '', 10, 60, 490, 90, 5000, 0, 0);
wb_set_text($status, "Click Run for default action, or the arrow for options.");

wb_set_handler($win, 'process_main');
wb_main_loop();

function process_main($window, $id, $ctrl = null, $lparam1 = null, $lparam2 = null)
{
    if ($id == ID_RUN) {
        if ($lparam1 == WBC_SPLIT_PRIMARY) {
            wb_message_box($window, "Primary click -> command {$lparam2}", "Run");
        } elseif ($lparam1 == WBC_SPLIT_DROPDOWN) {
            if ($lparam2 > 0) {
                wb_toolbar_set_split_default($ctrl, ID_RUN, $lparam2);
                wb_message_box($window, "Dropdown click -> selected {$lparam2}.\nDefault updated.", "Run With Options");
            }
        }
    }

    if ($id == IDCLOSE) {
        wb_destroy_window($window);
    }
}
