<?php

$win = wb_create_window(NULL, AppWindow, "ScintillaEdit API demo", WBC_CENTER, WBC_CENTER, 980, 700);

$editor = wb_create_control(
    $win,
    ScintillaEdit,
    "",
    10,
    10,
    950,
    610,
    101,
    WBC_VISIBLE | WBC_BORDER | WBC_NOTIFY,
    WBC_SCN_MODIFIED | WBC_SCN_UPDATEUI | WBC_SCN_MARGINCLICK | WBC_SCN_CHARADDED
);

$status = wb_create_control($win, Label, "Ready", 10, 628, 950, 24, 102, WBC_VISIBLE | WBC_BORDER);

if (!$editor) {
    wb_message_box($win, "ScintillaEdit could not be created. Ensure SciLexer.dll is available.", "Scintilla runtime missing", WBC_STOP);
    wb_destroy_window($win);
    return;
}

// -----------------------------------------------------------------------------
// Phase 1..5 API walkthrough (using all currently added Scintilla helpers)
// -----------------------------------------------------------------------------

// Core text APIs
wb_scintilla_set_text($editor, "<?php\n\nclass Demo\n{\n    public function greet(string $name): void\n    {\n        echo \"Hello {$name}\\n\";\n    }\n}\n\n$demo = new Demo();\n$demo->greet('Scintilla');\n");
wb_scintilla_append_text($editor, "\n// Appended with wb_scintilla_append_text()\n");
$allText = wb_scintilla_get_text($editor);

// Selection/caret/navigation APIs
wb_scintilla_goto_line($editor, 0);
$lineCount = wb_scintilla_get_line_count($editor);
$currentPos = wb_scintilla_get_current_pos($editor);
wb_scintilla_set_selection($editor, 0, min(20, strlen($allText)));
$selStart = wb_scintilla_get_selection_start($editor);
$selEnd = wb_scintilla_get_selection_end($editor);
wb_set_text($status, "Lines: {$lineCount}, Pos: {$currentPos}, Sel: {$selStart}-{$selEnd}");

// Edit command APIs
wb_scintilla_copy($editor);
wb_scintilla_cut($editor);
wb_scintilla_paste($editor);
wb_scintilla_undo($editor);
wb_scintilla_redo($editor);

// Readonly toggle API
wb_scintilla_set_readonly($editor, true);
wb_scintilla_set_readonly($editor, false);

// Preset + lexer/style/keywords APIs
wb_scintilla_apply_php_preset($editor);
wb_scintilla_set_lexer($editor, WBC_SCLEX_HTML);
wb_scintilla_set_keywords($editor, 0, "class function public private protected return if else foreach while try catch throw");
wb_scintilla_set_style($editor, WBC_SC_STYLE_LINENUMBER, DARKGRAY, LIGHTGRAY);

// Indentation/layout APIs
wb_scintilla_set_tab_width($editor, 4);
wb_scintilla_set_use_tabs($editor, false);
wb_scintilla_set_indent_guides($editor, true);
wb_scintilla_set_line_numbers($editor, true, 56);

// Visibility APIs
wb_scintilla_set_whitespace_view($editor, false);
wb_scintilla_set_eol_view($editor, false);

// Clear + restore demo for wb_scintilla_clear_all
$backup = wb_scintilla_get_text($editor);
wb_scintilla_clear_all($editor);
wb_scintilla_set_text($editor, $backup);

wb_set_handler($win, "process_main");
wb_main_loop();

function process_main($window, $id, $ctrl = 0, $param1 = 0, $param2 = 0, $param3 = 0)
{
    // Close window
    if ($id === IDCANCEL || $id === IDOK) {
        wb_destroy_window($window);
        return;
    }

    if ($id !== 101) {
        return;
    }

    // Scintilla notifications
    if ($param1 === WBC_SCN_MODIFIED) {
        wb_set_text($window, "ScintillaEdit API demo* (modified)");
    } elseif ($param1 === WBC_SCN_MARGINCLICK) {
        wb_set_text($window, "ScintillaEdit API demo (margin click)");
    } elseif ($param1 === WBC_SCN_CHARADDED) {
        $ch = chr((int)$param2);

        // PHP-specific convenience autocomplete helper
        if ($ch === '$' || $ch === ':' || $ch === '>') {
            wb_scintilla_show_php_autocomplete($ctrl, $ch);
        }

        // Generic autocomplete/calltip helpers
        if ($ch === '.') {
            wb_scintilla_autocomplete_show($ctrl, "count current date diff filter format join map merge reduce split", 0);
        } elseif ($ch === '(') {
            wb_scintilla_calltip_show($ctrl, "functionName(arg1, arg2)");
        } elseif ($ch === ')') {
            wb_scintilla_calltip_cancel($ctrl);
            wb_scintilla_autocomplete_cancel($ctrl);
        }
    }
}
