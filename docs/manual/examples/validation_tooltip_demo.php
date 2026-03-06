<?php

$win = wb_create_window(NULL, AppWindow, "Validation tooltip demo", WBC_CENTER, WBC_CENTER, 420, 180);
$label = wb_create_control($win, Label, "Email", 14, 20, 70, 20, 0, WBC_RIGHT);
$input = wb_create_control($win, EditBox, "", 90, 16, 300, 24, 1001);
$ok = wb_create_control($win, PushButton, "Validate", 90, 56, 90, 28, 1002);
$close = wb_create_control($win, PushButton, "Close", 190, 56, 90, 28, 1003);

wb_attach_tooltip($input, "Enter a valid e-mail address.");
wb_set_handler($win, "process_main");
wb_main_loop();

function process_main($window, $id, $ctrl)
{
    global $input;

    switch ($id) {
        case 1002:
            $email = trim(wb_get_text($input));
            if ($email === "") {
                wb_show_tooltip_balloon($input, "Email is required.", "error", "Validation error");
                return;
            }
            if (!filter_var($email, FILTER_VALIDATE_EMAIL)) {
                wb_show_tooltip_balloon($input, "This e-mail format looks invalid.", WBT_SEVERITY_WARN, "Check value");
                return;
            }
            wb_hide_tooltip($input);
            wb_show_tooltip_balloon($input, "Looks good.", WBT_SEVERITY_INFO, "Validation");
            break;

        case 1003:
        case IDCLOSE:
            wb_destroy_window($window);
            break;
    }
}
