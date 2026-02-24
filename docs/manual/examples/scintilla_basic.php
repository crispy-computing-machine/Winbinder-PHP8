<?php

$win = wb_create_window(NULL, AppWindow, "ScintillaEdit API demo", WBC_CENTER, WBC_CENTER, 1040, 760);

$editor = wb_create_control(
    $win,
    ScintillaEdit,
    "",
    10,
    10,
    780,
    700,
    101,
    WBC_VISIBLE | WBC_BORDER | WBC_NOTIFY,
    WBC_SCN_MODIFIED | WBC_SCN_UPDATEUI | WBC_SCN_MARGINCLICK | WBC_SCN_CHARADDED
);

$status = wb_create_control($win, Label, "Ready", 10, 714, 1010, 26, 102, WBC_VISIBLE | WBC_BORDER);

$chkReadonly     = wb_create_control($win, CheckBox, "Read-only",          810,  20, 200, 24, 201, WBC_VISIBLE);
$chkLineNumbers  = wb_create_control($win, CheckBox, "Line numbers",       810,  50, 200, 24, 202, WBC_VISIBLE);
$chkIndentGuides = wb_create_control($win, CheckBox, "Indent guides",      810,  80, 200, 24, 203, WBC_VISIBLE);
$chkUseTabs      = wb_create_control($win, CheckBox, "Use tabs",           810, 110, 200, 24, 204, WBC_VISIBLE);
$chkWhitespace   = wb_create_control($win, CheckBox, "Show whitespace",    810, 140, 200, 24, 205, WBC_VISIBLE);
$chkEol          = wb_create_control($win, CheckBox, "Show EOL",           810, 170, 200, 24, 206, WBC_VISIBLE);

if (!$editor) {
    wb_message_box($win, "ScintillaEdit could not be created. Ensure SciLexer.dll is available.", "Scintilla runtime missing", WBC_STOP);
    wb_destroy_window($win);
    return;
}

// Core setup and API walkthrough
wb_scintilla_set_text($editor, "<?php\n\nclass Demo\n{\n    public function greet(string $name): void\n    {\n        echo \"Hello {$name}\\n\";\n    }\n}\n\n$demo = new Demo();\n$demo->greet('Scintilla');\n");
wb_scintilla_append_text($editor, "\n// Try typing $, :, >, ., (, ) and clicking margins\n");

wb_scintilla_apply_php_preset($editor);
wb_scintilla_set_lexer($editor, WBC_SCLEX_HTML);
wb_scintilla_set_keywords($editor, 0, "class function public private protected return if else foreach while try catch throw");
wb_scintilla_set_style($editor, WBC_SC_STYLE_LINENUMBER, DARKGRAY, LIGHTGRAY);

wb_scintilla_set_tab_width($editor, 4);
wb_scintilla_set_line_numbers($editor, true, 56);
wb_scintilla_set_indent_guides($editor, true);
wb_scintilla_set_use_tabs($editor, false);
wb_scintilla_set_whitespace_view($editor, false);
wb_scintilla_set_eol_view($editor, false);

// Set checkbox defaults to match initial editor configuration
wb_set_state($chkLineNumbers, 0, true);
wb_set_state($chkIndentGuides, 0, true);

wb_set_handler($win, "process_main");
wb_main_loop();

function update_status($window, $editor, $prefix = "")
{
    $pos = wb_scintilla_get_current_pos($editor);
    $selStart = wb_scintilla_get_selection_start($editor);
    $selEnd = wb_scintilla_get_selection_end($editor);
    $lines = wb_scintilla_get_line_count($editor);
    wb_set_text($window, "ScintillaEdit API demo" . $prefix);
    wb_set_text(wb_get_control($window, 102), "Lines: {$lines}, Pos: {$pos}, Sel: {$selStart}-{$selEnd}");
}

function process_main($window, $id, $ctrl = 0, $param1 = 0, $param2 = 0, $param3 = 0)
{
    $editor = wb_get_control($window, 101);

    if ($id === IDCANCEL || $id === IDOK) {
        wb_destroy_window($window);
        return;
    }

    // Checkbox-driven configuration tests
    switch ($id) {
        case 201:
            wb_scintilla_set_readonly($editor, wb_get_value($ctrl) ? true : false);
            break;
        case 202:
            wb_scintilla_set_line_numbers($editor, wb_get_value($ctrl) ? true : false, 56);
            break;
        case 203:
            wb_scintilla_set_indent_guides($editor, wb_get_value($ctrl) ? true : false);
            break;
        case 204:
            wb_scintilla_set_use_tabs($editor, wb_get_value($ctrl) ? true : false);
            break;
        case 205:
            wb_scintilla_set_whitespace_view($editor, wb_get_value($ctrl) ? true : false);
            break;
        case 206:
            wb_scintilla_set_eol_view($editor, wb_get_value($ctrl) ? true : false);
            break;
    }

    // Scintilla notifications
    if ($id !== 101) {
        update_status($window, $editor);
        return;
    }

    if ($param1 === WBC_SCN_MODIFIED) {
        update_status($window, $editor, "* (modified)");
    } elseif ($param1 === WBC_SCN_UPDATEUI) {
        update_status($window, $editor);
    } elseif ($param1 === WBC_SCN_MARGINCLICK) {
        update_status($window, $editor, " (margin click)");
    } elseif ($param1 === WBC_SCN_CHARADDED) {
        // Fallback on current char-at-caret to avoid dependency on notification payload
        $text = wb_scintilla_get_text($editor);
        $len = strlen($text);
        $ch = $len > 0 ? $text[$len - 1] : '';

        if ($ch === '$' || $ch === ':' || $ch === '>') {
            wb_scintilla_show_php_autocomplete($editor, $ch);
        }

        if ($ch === '.') {
            wb_scintilla_autocomplete_show($editor, "count current date diff filter format join map merge reduce split", 0);
        } elseif ($ch === '(') {
            wb_scintilla_calltip_show($editor, "functionName(arg1, arg2)");
        } elseif ($ch === ')') {
            wb_scintilla_calltip_cancel($editor);
            wb_scintilla_autocomplete_cancel($editor);
        }

        update_status($window, $editor);
    }
}
