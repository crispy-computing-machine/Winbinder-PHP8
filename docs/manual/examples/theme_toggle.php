<?php

wb_set_theme(WBT_THEME_DEFAULT);
wb_set_theme_color(WBT_THEME_CUSTOM, WBT_COLOR_ROLE_TEXT, 0xF0F0F0);
wb_set_theme_color(WBT_THEME_CUSTOM, WBT_COLOR_ROLE_BACKGROUND, 0x202428);
wb_set_theme_color(WBT_THEME_CUSTOM, WBT_COLOR_ROLE_ACCENT, 0x50C0FF);

$win = wb_create_window(0, AppWindow, "Theme toggle demo", WBC_CENTER, WBC_CENTER, 980, 620);
$label = wb_create_control($win, Label, "Theme: default", 16, 16, 400, 24, 101, WBC_VISIBLE);
$btnDefault = wb_create_control($win, PushButton, "Default", 16, 52, 90, 28, 201, WBC_VISIBLE);
$btnLight = wb_create_control($win, PushButton, "Light", 116, 52, 90, 28, 202, WBC_VISIBLE);
$btnDark = wb_create_control($win, PushButton, "Dark", 216, 52, 90, 28, 203, WBC_VISIBLE);
$btnCustom = wb_create_control($win, PushButton, "Custom", 316, 52, 90, 28, 204, WBC_VISIBLE);

$editBox = wb_create_control($win, EditBox, "Sample EditBox text", 16, 96, 260, 24, 301, WBC_VISIBLE | WBC_BORDER);
$check = wb_create_control($win, CheckBox, "Enable option", 16, 128, 180, 24, 302, WBC_VISIBLE);
$radio = wb_create_control($win, RadioButton, "Choice A", 16, 156, 180, 24, 303, WBC_VISIBLE | WBC_GROUP);
$listBox = wb_create_control($win, ListBox, "", 16, 188, 260, 130, 304, WBC_VISIBLE | WBC_BORDER);
$combo = wb_create_control($win, ComboBox, "", 16, 328, 260, 200, 305, WBC_VISIBLE | WBC_BORDER);
$link = wb_create_control($win, HyperLink, "https://winbinder.org", 16, 362, 260, 24, 306, WBC_VISIBLE);

$list = wb_create_control($win, ListView, ["Control", "Status"], 290, 96, 320, 260, 102, WBC_VISIBLE | WBC_LINES | WBC_BORDER);
$edit = wb_create_control($win, ScintillaEdit, "", 620, 16, 340, 560, 103, WBC_VISIBLE | WBC_BORDER);

wb_create_item($listBox, "ListBox item 1");
wb_create_item($listBox, "ListBox item 2");
wb_create_item($combo, "Combo item 1");
wb_create_item($combo, "Combo item 2");

wb_create_item($list, "Window");
wb_set_text($list, "WM_ERASEBKGND + palette brush", 0);
wb_create_item($list, "List/Tree/Toolbar");
wb_set_text($list, "Theme colors applied where supported", 1);
wb_create_item($list, "ScintillaEdit");
wb_set_text($list, "Preset follows wb_get_theme() + palette", 2);

if ($edit) {
    wb_scintilla_set_text($edit, "<?php
// Toggle theme buttons and preview many controls.
");
    wb_scintilla_apply_php_preset($edit);
}

wb_set_handler($win, "process_theme_demo");
wb_main_loop();

function apply_theme($window, $theme, $name)
{
    wb_set_theme($window, $theme);
    wb_set_text(wb_get_control($window, 101), "Theme: {$name}");
    $editor = wb_get_control($window, 103);
    if ($editor) {
        wb_scintilla_apply_php_preset($editor);
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
