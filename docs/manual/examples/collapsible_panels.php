<?php

$win = wb_create_window(NULL, AppWindow, 'Settings', 80, 80, 520, 500, WBC_NOTIFY, WBC_RESIZE);

$groups = [];
$groups[] = create_group($win, 101, 'General', '⚙', [
    ['Label', 'Language'],
    ['EditBox', 'English (US)'],
    ['CheckBox', 'Check for updates automatically'],
]);

$groups[] = create_group($win, 102, 'Editor', '📝', [
    ['CheckBox', 'Use soft tabs'],
    ['Label', 'Tab width'],
    ['EditBox', '4'],
    ['CheckBox', 'Show line numbers'],
]);

$groups[] = create_group($win, 103, 'Advanced', '🧪', [
    ['CheckBox', 'Enable experimental features'],
    ['CheckBox', 'Verbose logs'],
    ['EditBox', 'Custom arguments...'],
]);

wb_set_handler($win, 'process_main');
wb_main_loop();

function create_group($parent, $id, $title, $icon, $rows)
{
    static $nextY = 16;

    $height = 44 + (count($rows) * 26);
    $panel = wb_create_control($parent, Frame, $title, 14, $nextY, 490, $height, $id, WBC_PANEL);
    wb_panel_set_header($panel, $title, $icon);

    $y = 28;
    foreach ($rows as $row) {
        if ($row[0] === 'Label') {
            wb_create_control($panel, Label, $row[1], 16, $y + 4, 220, 16, 0);
        } elseif ($row[0] === 'EditBox') {
            wb_create_control($panel, EditBox, $row[1], 220, $y, 230, 22, 0);
        } elseif ($row[0] === 'CheckBox') {
            wb_create_control($panel, CheckBox, $row[1], 16, $y, 434, 22, 0);
        }
        $y += 26;
    }

    $nextY += $height + 8;
    return $panel;
}

function process_main($window, $id, $ctrl, $param1, $param2)
{
    if ($id === IDCLOSE) {
        wb_destroy_window($window);
        return;
    }

    if ($param1 === WBC_PANEL_EXPANDED || $param1 === WBC_PANEL_COLLAPSED) {
        $state = ($param1 === WBC_PANEL_EXPANDED) ? 'expanded' : 'collapsed';
        wb_set_text($window, "Group #{$id} {$state}");
        return;
    }

    if ($param1 === WBC_RESIZE) {
        // Re-anchor all panels to fill the current width.
        for ($i = 101; $i <= 103; $i++) {
            $panel = wb_get_control($window, $i);
            if ($panel) {
                $size = wb_get_size($window);
                wb_set_size($panel, $size[0] - 30, null);
            }
        }
    }
}
