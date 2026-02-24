<?php

$win = wb_create_window(NULL, AppWindow, "ScintillaEdit MVP", WBC_CENTER, WBC_CENTER, 900, 640);

$editor = wb_create_control(
    $win,
    ScintillaEdit,
    "",
    10,
    10,
    870,
    580,
    101,
    WBC_VISIBLE | WBC_BORDER | WBC_NOTIFY,
    WBC_SCN_MODIFIED | WBC_SCN_UPDATEUI | WBC_SCN_MARGINCLICK | WBC_SCN_CHARADDED
);

if (!$editor) {
    wb_message_box($win, "ScintillaEdit could not be created. Ensure SciLexer.dll is available.", "Scintilla runtime missing", WBC_STOP);
    wb_destroy_window($win);
    return;
}

wb_scintilla_set_text($editor, "<?php\n\n// ScintillaEdit Phase 2 sample\nfunction greet(string $name): void\n{\n    echo \"Hello {$name}\\n\";\n}\n\ngreet('Scintilla');\n");


wb_scintilla_apply_php_preset($editor);
wb_scintilla_set_whitespace_view($editor, false);
wb_scintilla_set_eol_view($editor, false);

// Optional fine-tuning after preset
wb_scintilla_set_line_numbers($editor, true, 56);
wb_scintilla_set_style($editor, WBC_SC_STYLE_LINENUMBER, DARKGRAY, LIGHTGRAY);

wb_set_handler($win, "process_main");
wb_main_loop();

function process_main($window, $id, $ctrl = 0, $param1 = 0, $param2 = 0, $param3 = 0)
{
    switch ($id) {
        case IDOK:
        case IDCANCEL:
            wb_destroy_window($window);
            return;
    }

    // Scintilla Phase 4 notifications
    if ($id === 101) {
        if ($param1 === WBC_SCN_MODIFIED) {
            wb_set_text($window, "ScintillaEdit MVP* (modified)");
        } elseif ($param1 === WBC_SCN_MARGINCLICK) {
            // Placeholder hook for fold/margin interactions
        } elseif ($param1 === WBC_SCN_CHARADDED) {
            // Minimal Phase 5 autocomplete/calltip trigger demo
            $ch = chr((int)$param2);
            if ($ch === '$' || $ch === ':' || $ch === '>') {
                wb_scintilla_show_php_autocomplete($ctrl, $ch);
            } elseif ($ch === '(') {
                wb_scintilla_calltip_show($ctrl, "functionName(arg1, arg2)");
            }
        }
    }
}
