<?php

// Popup menu demo for wb_create_popup_menu() + wb_track_popup_menu()
// - Right-click the window to open the popup at the mouse x/y position
// - Demonstrates separators, checked items, enable/disable, and icon assignment

const ID_POP_COPY = 1001;
const ID_POP_PASTE = 1002;
const ID_POP_TOGGLE_CHECK = 1003;
const ID_POP_TOGGLE_ENABLED = 1004;
const ID_POP_EXIT = 1005;

$win = wb_create_window(NULL, AppWindow, "Popup menu demo", 120, 120, 520, 320, WBC_NOTIFY, WBC_MOUSEDOWN);
wb_create_control($win, Label, "Right-click anywhere in this window.", 14, 16, 360, 20, 0);
wb_create_control($win, Label, "Use 'Toggle Paste enabled' and 'Toggle checked item' to test state updates.", 14, 40, 470, 20, 0);

$popup = wb_create_popup_menu(array(
    array(ID_POP_COPY, "&Copy\tCtrl+C", "Copy selection", ""),
    array(ID_POP_PASTE, "&Paste\tCtrl+V", "Paste from clipboard", ""),
    null,
    array(ID_POP_TOGGLE_CHECK, "Toggle &checked item", "Toggle checked state", ""),
    array(ID_POP_TOGGLE_ENABLED, "Toggle &Paste enabled", "Enable/disable Paste", ""),
    null,
    array(ID_POP_EXIT, "E&xit", "Close the demo", "")
));

// Optional icon: if the image exists, assign it to Copy
$iconPath = __DIR__ . '/../images/icon_note.png';
if (file_exists($iconPath)) {
    $himg = wb_load_image($iconPath);
    if ($himg) {
        wb_set_menu_item_image($popup, ID_POP_COPY, $himg);
    }
}

$menuState = array(
    'pasteEnabled' => true,
    'checked' => false,
);

wb_set_handler($win, 'process_main');
wb_main_loop();

function process_main($window, $id, $ctrl, $param1 = 0, $param2 = 0)
{
    global $popup, $menuState;

    switch ($id) {
        // Open popup on right mouse down at cursor x/y from callback payload
        case 0:
            $isMouseDown = (($param1 & WBC_MOUSEDOWN) === WBC_MOUSEDOWN);
            $isRightButton = (($param1 & WBC_RBUTTON) === WBC_RBUTTON);

            if ($isMouseDown && $isRightButton) {
                $x = $param2 & 0xFFFF;
                $y = ($param2 >> 16) & 0xFFFF;

                wb_track_popup_menu($popup, $window, $x, $y);
            }
            return;

        case ID_POP_COPY:
            wb_message_box($window, "Copy clicked", "Popup menu");
            return;

        case ID_POP_PASTE:
            wb_message_box($window, "Paste clicked", "Popup menu");
            return;

        case ID_POP_TOGGLE_CHECK:
            $menuState['checked'] = !$menuState['checked'];
            wb_set_menu_item_checked($popup, ID_POP_TOGGLE_CHECK, $menuState['checked'] ? 1 : 0);
            return;

        case ID_POP_TOGGLE_ENABLED:
            $menuState['pasteEnabled'] = !$menuState['pasteEnabled'];
            $pasteItem = wb_get_control($popup, ID_POP_PASTE);
            wb_set_enabled($pasteItem, $menuState['pasteEnabled']);
            return;

        case ID_POP_EXIT:
        case IDCLOSE:
            wb_destroy_window($window);
            return;
    }
}
