<?php
<<<<<<< codex/build-custom-splitter-control-in-winbinder-cu4dlm
$mainwin = wb_create_window(NULL, AppWindow, "Splitter demo", WBC_CENTER, WBC_CENTER, 900, 600, WBC_NOTIFY, WBC_RESIZE);
$split = wb_create_control($mainwin, Splitter, NULL, 0, 0, 900, 600, 1001, WBC_SPLIT_VERTICAL, 500);
$left = wb_create_control($split, EditBox, "Left pane", 0, 0, 100, 100, 1002, WBC_MULTILINE | WBC_BORDER);
$right = wb_create_control($split, EditBox, "Right pane", 0, 0, 100, 100, 1003, WBC_MULTILINE | WBC_BORDER);

wb_set_splitter_panes($split, $left, $right);
wb_set_splitter_minsize($split, 140, 180);
wb_set_splitter_position($split, 500, TRUE);
wb_set_handler($mainwin, "process_main");
wb_set_visible($mainwin, TRUE);

wb_main_loop();

function process_main($window, $id, $ctrl = 0, $param = 0)
{
    global $split;

    if ($param == WBC_RESIZE) {
        $size = wb_get_size($window);
        wb_set_size($split, $size[0], $size[1]);
    }
=======
$win = wb_create_window(NULL, AppWindow, "Splitter demo", WBC_NOTIFY, 100, 100, 900, 600);
$split = wb_create_control($win, Splitter, NULL, 0, 0, 900, 600, 1001, WBC_SPLIT_VERTICAL);
$left = wb_create_control($split, EditBox, "Left pane", 0, 0, 100, 100, 1002, WBC_MULTILINE);
$right = wb_create_control($split, ListView, "", 0, 0, 100, 100, 1003, WBC_BORDER);
wb_set_splitter_panes($split, $left, $right);
wb_set_splitter_minsize($split, 140, 180);
wb_set_splitter_position($split, 450);

while (wb_wait()) {
>>>>>>> dev
}
