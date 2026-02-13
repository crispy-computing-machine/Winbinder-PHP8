<?php
$win = wb_create_window(NULL, AppWindow, "Splitter demo", WBC_NOTIFY, 100, 100, 900, 600);
$split = wb_create_control($win, Splitter, NULL, 0, 0, 900, 600, 1001, WBC_SPLIT_VERTICAL);
$left = wb_create_control($split, EditBox, "Left pane", 0, 0, 100, 100, 1002, WBC_MULTILINE);
$right = wb_create_control($split, ListView, "", 0, 0, 100, 100, 1003, WBC_BORDER);
wb_set_splitter_panes($split, $left, $right);
wb_set_splitter_minsize($split, 140, 180);
wb_set_splitter_position($split, 450);

while (wb_wait()) {
}
