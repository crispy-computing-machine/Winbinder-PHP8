<?php

$win = wb_create_window(0, AppWindow, "ScintillaEdit API demo", WBC_CENTER, WBC_CENTER, 1040, 820);

$editor = wb_create_control(
    $win,
    ScintillaEdit,
    "",
    10,
    10,
    780,
    740,
    101,
    WBC_VISIBLE | WBC_BORDER | WBC_NOTIFY,
    WBC_SCN_MODIFIED | WBC_SCN_UPDATEUI | WBC_SCN_MARGINCLICK | WBC_SCN_CHARADDED
);

$status = wb_create_control($win, Label, "Ready", 10, 764, 1010, 40, 102, WBC_VISIBLE | WBC_BORDER | WBC_MULTILINE);

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

// Build broad autocomplete sources for all WinBinder functions/constants.
$wbFunctions = array_values(array_filter(get_defined_functions()['internal'], fn($f) => stripos($f, 'wb_') === 0));
$wbConstants = array_keys(array_filter(get_defined_constants(), fn($k) => stripos($k, 'WBC_') === 0 || stripos($k, 'AppWindow') === 0 || stripos($k, 'ScintillaEdit') === 0, ARRAY_FILTER_USE_KEY));
$GLOBALS['wb_scintilla_autocomplete_list'] = implode(' ', array_unique(array_merge($wbFunctions, $wbConstants)));

$demoText = <<<'PHP'
<?php

class Demo
{
    public function greet(string $name): void
    {
        echo "Hello {$name}\n";
    }
}

$demo = new Demo();
$demo->greet('Scintilla');

// Type wb_ or WBC_ then '.' for broad autocomplete.
PHP;

wb_scintilla_set_text($editor, $demoText);
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

    $ro = wb_get_value(wb_get_control($window, 201)) ? 'ON' : 'OFF';
    $ln = wb_get_value(wb_get_control($window, 202)) ? 'ON' : 'OFF';
    $ig = wb_get_value(wb_get_control($window, 203)) ? 'ON' : 'OFF';
    $ut = wb_get_value(wb_get_control($window, 204)) ? 'ON' : 'OFF';
    $ws = wb_get_value(wb_get_control($window, 205)) ? 'ON' : 'OFF';
    $eol = wb_get_value(wb_get_control($window, 206)) ? 'ON' : 'OFF';

    wb_set_text(
        wb_get_control($window, 102),
        "Lines: {$lines}, Pos: {$pos}, Sel: {$selStart}-{$selEnd}\nRO={$ro} LN={$ln} IG={$ig} TABS={$ut} WS={$ws} EOL={$eol}"
    );
}

function process_main($window, $id, $ctrl = 0, $param1 = 0, $param2 = 0, $param3 = 0)
{
    $editor = wb_get_control($window, 101);

    if ($id === IDCANCEL || $id === IDCLOSE) {
        wb_destroy_window($window);
        return;
    }

    switch ($id) {
        case 201:
            wb_scintilla_set_readonly($editor, wb_get_value($ctrl) ? true : false);
            update_status($window, $editor, " (readonly toggled)");
            return;
        case 202:
            wb_scintilla_set_line_numbers($editor, wb_get_value($ctrl) ? true : false, 56);
            update_status($window, $editor, " (line numbers toggled)");
            return;
        case 203:
            wb_scintilla_set_indent_guides($editor, wb_get_value($ctrl) ? true : false);
            update_status($window, $editor, " (indent guides toggled)");
            return;
        case 204:
            wb_scintilla_set_use_tabs($editor, wb_get_value($ctrl) ? true : false);
            update_status($window, $editor, " (tabs toggled)");
            return;
        case 205:
            wb_scintilla_set_whitespace_view($editor, wb_get_value($ctrl) ? true : false);
            update_status($window, $editor, " (whitespace toggled)");
            return;
        case 206:
            wb_scintilla_set_eol_view($editor, wb_get_value($ctrl) ? true : false);
            update_status($window, $editor, " (EOL toggled)");
            return;
    }

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
        $text = wb_scintilla_get_text($editor);
        $len = strlen($text);
        $ch = $len > 0 ? $text[$len - 1] : '';

        if ($ch === '$' || $ch === ':' || $ch === '>') {
            wb_scintilla_show_php_autocomplete($editor, $ch);
        }

        if ($ch === '.') {
            wb_scintilla_autocomplete_show($editor, $GLOBALS['wb_scintilla_autocomplete_list'], 0);
        } elseif ($ch === '(') {
            wb_scintilla_calltip_show($editor, "wb_function_name(arg1, arg2)\nConstants: WBC_* | Classes: AppWindow..ScintillaEdit");
        } elseif ($ch === ')') {
            wb_scintilla_calltip_cancel($editor);
            wb_scintilla_autocomplete_cancel($editor);
        }

        update_status($window, $editor);
    }
}
