<?php

wb_set_theme(WBT_THEME_DEFAULT);

$win = wb_create_window(null, AppWindow, "Theme toggle demo", WBC_CENTER, WBC_CENTER, 760, 520);
$label = wb_create_control($win, Label, "Theme: default", 16, 16, 300, 24, 101, WBC_VISIBLE);
$btnDefault = wb_create_control($win, PushButton, "Default", 16, 52, 90, 28, 201, WBC_VISIBLE);
$btnLight = wb_create_control($win, PushButton, "Light", 116, 52, 90, 28, 202, WBC_VISIBLE);
$btnDark = wb_create_control($win, PushButton, "Dark", 216, 52, 90, 28, 203, WBC_VISIBLE);
$list = wb_create_control($win, ListView, ["Control", "Status"], 16, 96, 340, 210, 102, WBC_VISIBLE | WBC_LINES | WBC_BORDER);
$edit = wb_create_control($win, ScintillaEdit, "", 370, 16, 370, 450, 103, WBC_VISIBLE | WBC_BORDER);

if ($edit) {
    wb_scintilla_set_text($edit, "<?php\n// Toggle theme buttons and notice Scintilla + controls update.\n");
    wb_scintilla_apply_php_preset($edit);
}

wb_create_listview_item($list, ["Label", "Auto themed"]);
wb_create_listview_item($list, ["ListView", "SetWindowTheme + redraw"]);
wb_create_listview_item($list, ["ScintillaEdit", "Preset follows wb_get_theme()"]);

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
    }
}
