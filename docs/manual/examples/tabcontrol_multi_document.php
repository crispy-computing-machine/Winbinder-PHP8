<?php

include '../../include/winbinder.php';

define('ID_TAB', 101);
define('ID_STATUS', 102);
define('ID_CLOSE', 103);
define('ID_PIN', 104);
define('ID_MOVE', 105);

$documents = array('README.md', 'notes.txt', 'todo.php');

$win = wb_create_window(NULL, AppWindow, 'TabControl Modern UX Demo', WBC_CENTER, WBC_CENTER, 720, 420, WBC_NOTIFY,
    WBC_HEADERSEL | WBC_TAB_CLOSING | WBC_TAB_CLOSED | WBC_TAB_REORDERED | WBC_TAB_PINNED);
wb_set_handler($win, 'process_main');

$tab = wb_create_control($win, TabControl, implode("\n", $documents), 8, 8, 700, 320, ID_TAB, WBC_VISIBLE);
$status = wb_create_control($win, StatusBar, '', 0, 0, 0, 0, ID_STATUS);

$close = wb_create_control($win, PushButton, 'Close active tab', 8, 336, 130, 26, ID_CLOSE, WBC_VISIBLE);
$pin = wb_create_control($win, PushButton, 'Toggle pin', 148, 336, 100, 26, ID_PIN, WBC_VISIBLE);
$move = wb_create_control($win, PushButton, 'Move right', 258, 336, 100, 26, ID_MOVE, WBC_VISIBLE);

foreach ($documents as $i => $name) {
    $editor = wb_create_control($tab, EditBox, "Editing {$name}", 12, 18, 670, 270, 1000 + $i, WBC_VISIBLE | WBC_MULTILINE, 0, $i);
    wb_tab_set_closable($tab, $i, $i > 0); // keep the first one fixed
}

wb_main_loop();

function process_main($window, $id, $ctrl, $lparam1 = 0, $lparam2 = 0, $lparam3 = 0)
{
    global $tab, $status;

    switch ($id) {
        case ID_TAB:
            if ($lparam1 & WBC_HEADERSEL) {
                wb_set_text($status, "Selected tab #{$lparam2}");
            } elseif ($lparam1 & WBC_TAB_CLOSING) {
                // Return non-zero to cancel closing tab #0.
                if ($lparam2 == 0) {
                    wb_set_text($status, 'Prevented closing pinned tab #0');
                    return 1;
                }
            } elseif ($lparam1 & WBC_TAB_CLOSED) {
                wb_set_text($status, "Closed tab #{$lparam2}");
            } elseif ($lparam1 & WBC_TAB_REORDERED) {
                wb_set_text($status, "Moved tab {$lparam2} -> {$lparam3}");
            } elseif ($lparam1 & WBC_TAB_PINNED) {
                wb_set_text($status, "Pin changed on tab #{$lparam2}");
            }
            break;

        case ID_CLOSE:
            wb_tab_close($tab, wb_get_selected($tab));
            break;

        case ID_PIN:
            wb_tab_pin($tab, wb_get_selected($tab));
            break;

        case ID_MOVE:
            $cur = wb_get_selected($tab);
            wb_tab_move($tab, $cur, $cur + 1);
            break;
    }

    return 0;
}
