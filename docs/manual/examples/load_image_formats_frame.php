<?php
$win = wb_create_window(0, AppWindow, "wb_load_image() raster format demo", WBC_CENTER, WBC_CENTER, 760, 220, WBC_NOTIFY);
$label = wb_create_control($win, Label, "Loaded images into WBC_IMAGE frames:", 10, 10, 720, 20, 1, WBC_VISIBLE);

$formats = ['png', 'jpg', 'gif', 'bmp', 'wbmp'];
$x = 10;
$images = [];
$workdir = __DIR__ . DIRECTORY_SEPARATOR . "generated_images";
foreach ($formats as $i => $ext) {
    $path = $workdir . DIRECTORY_SEPARATOR . "sample.$ext";
    $frame = wb_create_control($win, Frame, strtoupper($ext), $x, 40, 130, 130, 100 + $i, WBC_IMAGE | WBC_BORDER | WBC_VISIBLE);
    $img = wb_load_image($path);

    if ($img) {
        wb_set_image($frame, $img, NOCOLOR);
        $images[] = $img;
    } else {
        wb_set_text($frame, strtoupper($ext) . " failed");
    }

    $x += 145;
}

$GLOBALS['demo_images'] = $images;
wb_set_handler($win, 'process_main');
wb_main_loop();

function process_main($window, $id)
{
    if ($id == IDCLOSE || $id == IDCANCEL || $id == IDOK) {
        foreach ($GLOBALS['demo_images'] as $hImage) {
            if ($hImage) {
                wb_destroy_image($hImage);
            }
        }
        wb_destroy_window($window);
    }
}
