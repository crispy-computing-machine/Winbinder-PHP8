<?php

/*
 * Demonstrates wb_load_image() compatibility formats by creating a small
 * set of test images on disk, loading them, and showing them in Frame
 * controls created with the WBC_IMAGE flag.
 */

$workdir = __DIR__ . DIRECTORY_SEPARATOR . "generated_images";
if (!is_dir($workdir)) {
    mkdir($workdir, 0777, true);
}

$fixtures = [
    'png'  => 'iVBORw0KGgoAAAANSUhEUgAAAAEAAAABCAQAAAC1HAwCAAAAC0lEQVR42mP8/x8AAwMCAO5m4i4AAAAASUVORK5CYII=',
    'jpg'  => '/9j/4AAQSkZJRgABAQAAAQABAAD/2wCEAAkGBxAQEBUQEBAVFhUVFRUVFRUVFRUVFRUVFRUXFhUVFRUYHSggGBolGxUVITEhJSkrLi4uFx8zODMsNygtLisBCgoKDg0OGhAQGi0fHyUtLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLf/AABEIAAEAAQMBIgACEQEDEQH/xAAZAAEBAQEBAQAAAAAAAAAAAAAAAQIDBAX/xAAUEAEAAAAAAAAAAAAAAAAAAAAA/8QAFgEBAQEAAAAAAAAAAAAAAAAAAgAB/8QAFBEBAAAAAAAAAAAAAAAAAAAAAP/aAAwDAQACEQMRAD8A8gA7gH//2Q==',
    'gif'  => 'R0lGODlhAQABAIAAAP///////ywAAAAAAQABAAACAkQBADs=',
    'bmp'  => 'Qk06AAAAAAAAADYAAAAoAAAAAQAAAAEAAAABABgAAAAAAAQAAAAAAAAAAAAAAAAAAAAAAAAAAP///w==',
    'wbmp' => 'AAABAQA=',
];

foreach ($fixtures as $ext => $base64) {
    $bin = base64_decode($base64, true);
    if ($bin === false) {
        echo "Failed to decode fixture: $ext\n";
        continue;
    }

    file_put_contents($workdir . DIRECTORY_SEPARATOR . "sample.$ext", $bin);
}

$win = wb_create_window(NULL, AppWindow, "wb_load_image() raster format demo", WBC_CENTER, WBC_CENTER, 760, 220, WBC_NOTIFY);
$label = wb_create_control($win, Label, "Loaded images into WBC_IMAGE frames:", 10, 10, 720, 20, 1, WBC_VISIBLE);

$formats = ['png', 'jpg', 'gif', 'bmp', 'wbmp'];
$x = 10;
$images = [];

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
