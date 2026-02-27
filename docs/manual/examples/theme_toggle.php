<?php

wb_set_theme(WBT_THEME_DEFAULT);
wb_set_theme_color(WBT_THEME_CUSTOM, WBT_COLOR_ROLE_TEXT, 0xF0F0F0);
wb_set_theme_color(WBT_THEME_CUSTOM, WBT_COLOR_ROLE_BACKGROUND, 0x202428);
wb_set_theme_color(WBT_THEME_CUSTOM, WBT_COLOR_ROLE_ACCENT, 0x50C0FF);

$win = wb_create_window(null, AppWindow, "Theme toggle demo", WBC_CENTER, WBC_CENTER, 820, 560);
$label = wb_create_control($win, Label, "Theme: default", 16, 16, 400, 24, 101, WBC_VISIBLE);
$btnDefault = wb_create_control($win, PushButton, "Default", 16, 52, 90, 28, 201, WBC_VISIBLE);
$btnLight = wb_create_control($win, PushButton, "Light", 116, 52, 90, 28, 202, WBC_VISIBLE);
$btnDark = wb_create_control($win, PushButton, "Dark", 216, 52, 90, 28, 203, WBC_VISIBLE);
$btnCustom = wb_create_control($win, PushButton, "Custom", 316, 52, 90, 28, 204, WBC_VISIBLE);
$list = wb_create_control($win, ListView, ["Control", "Status"], 16, 96, 380, 250, 102, WBC_VISIBLE | WBC_LINES | WBC_BORDER);
$edit = wb_create_control($win, ScintillaEdit, "", 410, 16, 390, 500, 103, WBC_VISIBLE | WBC_BORDER);

if ($edit) {
    wb_scintilla_set_text($edit, "<?php\n// Toggle theme buttons and notice controls update coherently.\n");
    wb_scintilla_apply_php_preset($edit);
}

wb_create_listview_item($list, ["Window", "WM_ERASEBKGND + palette brush"]);
wb_create_listview_item($list, ["Menu/Toolbar", "Theme refresh + redraw"]);
wb_create_listview_item($list, ["ScintillaEdit", "Preset follows wb_get_theme() + palette"]);

wb_set_handler($win, "process_theme_demo");
wb_main_loop();

function apply_theme($window, $theme, $name)
{
    wb_set_theme($window, $theme);
    wb_set_text(wb_get_control($window, 101), "Theme: {$name}");

    $edit = wb_get_control($window, 103);
    if ($edit) {
        wb_scintilla_apply_php_preset($edit);
    }
}

function process_theme_demo($window, $id)
{
    if ($id === IDCANCEL) {
        wb_destroy_window($window);
        return;
    }

    if ($id === 201) {
        apply_theme($window, WBT_THEME_DEFAULT, "default");
    } elseif ($id === 202) {
        apply_theme($window, WBT_THEME_LIGHT, "light");
    } elseif ($id === 203) {
        apply_theme($window, WBT_THEME_DARK, "dark");
    } elseif ($id === 204) {
        apply_theme($window, WBT_THEME_CUSTOM, "custom");
    }
}
