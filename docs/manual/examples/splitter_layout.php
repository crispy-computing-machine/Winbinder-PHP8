<?php
$mainwin = wb_create_window(NULL, AppWindow, "Splitter example app", WBC_CENTER, WBC_CENTER, 980, 640, WBC_NOTIFY, WBC_RESIZE);
$split = wb_create_control($mainwin, Splitter, "", 0, 0, 980, 640, 1001, WBC_SPLIT_VERTICAL, 360);

$paneLeft = wb_create_control($split, InvisibleArea, "", 0, 0, 320, 640, 1100, WBC_BORDER);
$paneRight = wb_create_control($split, InvisibleArea, "", 0, 0, 640, 640, 1200, WBC_BORDER);

// Left pane: simple item manager controls
$lblSearch = wb_create_control($paneLeft, Label, "Search", 12, 12, 70, 20, 1101, 0);
$txtSearch = wb_create_control($paneLeft, EditBox, "", 86, 10, 220, 22, 1102, WBC_BORDER);
$lstItems = wb_create_control($paneLeft, ListBox, "", 12, 42, 294, 420, 1103, WBC_BORDER);
$chkCase = wb_create_control($paneLeft, CheckBox, "Match case", 12, 468, 120, 22, 1104, 0);
$btnAdd = wb_create_control($paneLeft, PushButton, "Add", 12, 500, 90, 28, 1105, 0);
$btnRemove = wb_create_control($paneLeft, PushButton, "Remove", 108, 500, 90, 28, 1106, 0);
$btnClear = wb_create_control($paneLeft, PushButton, "Clear", 204, 500, 90, 28, 1107, 0);

wb_create_item($lstItems, "Alpha");
wb_create_item($lstItems, "Beta");
wb_create_item($lstItems, "Gamma");

// Right pane: details/editor controls
$lblTitle = wb_create_control($paneRight, Label, "Details", 12, 12, 120, 24, 1201, 0);
$edtDetails = wb_create_control($paneRight, EditBox, "Select an item to view/edit details.", 12, 42, 610, 230, 1202, WBC_MULTILINE | WBC_BORDER);
$lblNotes = wb_create_control($paneRight, Label, "Notes", 12, 284, 120, 20, 1203, 0);
$edtNotes = wb_create_control($paneRight, EditBox, "", 12, 308, 610, 220, 1204, WBC_MULTILINE | WBC_BORDER);
$btnSave = wb_create_control($paneRight, PushButton, "Save", 532, 536, 90, 28, 1205, 0);

wb_set_splitter_panes($split, $paneLeft, $paneRight);
wb_set_splitter_minsize($split, 280, 420);
wb_set_splitter_position($split, 360);

// Regression check: calling wb_get_position() with clientarea=TRUE on a main window must be safe.
$mainPosClient = wb_get_position($mainwin, TRUE);
if (!is_array($mainPosClient) || count($mainPosClient) < 2) {
    wb_message_box($mainwin, "Unexpected wb_get_position() result for main window.", "Regression check", WBC_INFO);
}

wb_set_handler($mainwin, "process_main");
wb_set_visible($mainwin, TRUE);
wb_main_loop();

function process_main($window, $id, $ctrl = 0, $param = 0)
{
    global $split, $lstItems, $txtSearch, $edtDetails;

    if ($param == WBC_RESIZE) {
        $size = wb_get_size($window);
        wb_set_size($split, $size[0], $size[1]);
        return;
    }

    switch ($id) {
        case 1105: // Add
            $newText = trim(wb_get_text($txtSearch));
            if ($newText != "") {
                wb_create_item($lstItems, $newText);
                wb_set_text($txtSearch, "");
            }
            break;

        case 1106: // Remove selected
            wb_delete_items($lstItems, wb_get_selected($lstItems));
            break;

        case 1107: // Clear all
            wb_delete_items($lstItems);
            break;

        case 1103: // List selection changed
            $selected = wb_get_selected($lstItems);
            if ($selected >= 0) {
                $text = wb_get_text($lstItems, $selected);
                wb_set_text($edtDetails, "Item: " . $text . "\n\nEditable details go here.");
            }
            break;
    }
}
