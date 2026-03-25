<?php

define('ID_MEDIA_PATH', 1001);
define('ID_STATUS', 1002);
define('ID_LOAD', 1003);
define('ID_PLAY', 1004);
define('ID_PAUSE', 1005);
define('ID_STOP', 1006);

// Bootstrap configuration for initial media path (env -> CLI -> fallback).
$fallbackMediaPath = 'C:/media/sample.mp4';
$defaultMediaPath = getenv('WB_VLC_SAMPLE_MP4');
$usedFallbackDefault = false;

if ($defaultMediaPath === false || trim($defaultMediaPath) === '') {
    if (isset($argv[1]) && trim((string) $argv[1]) !== '') {
        $defaultMediaPath = (string) $argv[1];
    } else {
        $defaultMediaPath = $fallbackMediaPath;
        $usedFallbackDefault = true;
    }
}

$main = wb_create_window(null, AppWindow, 'VLC Media Control (Complete)', WBC_CENTER, WBC_CENTER, 760, 180, WBC_NOTIFY, 0);
$edtMedia = wb_create_control($main, EditBox, $defaultMediaPath, 12, 14, 590, 24, ID_MEDIA_PATH, WBC_BORDER);
$btnLoad = wb_create_control($main, PushButton, 'Load', 612, 14, 130, 24, ID_LOAD, 0);
$btnPlay = wb_create_control($main, PushButton, 'Play', 12, 48, 90, 24, ID_PLAY, 0);
$btnPause = wb_create_control($main, PushButton, 'Pause', 112, 48, 90, 24, ID_PAUSE, 0);
$btnStop = wb_create_control($main, PushButton, 'Stop', 212, 48, 90, 24, ID_STOP, 0);
$statusText = $usedFallbackDefault
    ? 'using default sample path; set WB_VLC_SAMPLE_MP4 to override'
    : 'Ready';
$lblStatus = wb_create_control($main, Label, $statusText, 12, 84, 730, 52, ID_STATUS, WBC_BORDER);

$vlcHandle = null;
wb_set_handler($main, 'process_main');
wb_main_loop();

function process_main($window, $id)
{
    switch ($id) {
        case ID_LOAD:
            load_current_media();
            break;

        case ID_PLAY:
            if (function_exists('wb_vlc_play')) {
                wb_vlc_play();
            }
            wb_set_text(wb_get_control($window, ID_STATUS), 'Playback started');
            break;

        case ID_PAUSE:
            if (function_exists('wb_vlc_pause')) {
                wb_vlc_pause();
            }
            wb_set_text(wb_get_control($window, ID_STATUS), 'Playback paused');
            break;

        case ID_STOP:
            if (function_exists('wb_vlc_stop')) {
                wb_vlc_stop();
            }
            wb_set_text(wb_get_control($window, ID_STATUS), 'Playback stopped');
            break;

        case IDCLOSE:
            wb_destroy_window($window);
            break;
    }
}

function load_current_media()
{
    global $main;

    $path = trim((string) wb_get_text(wb_get_control($main, ID_MEDIA_PATH)));
    if ($path === '') {
        wb_set_text(wb_get_control($main, ID_STATUS), 'Please provide a media file path first.');
        return;
    }

    wb_vlc_set_media($path);
    wb_set_text(wb_get_control($main, ID_STATUS), "Loaded: {$path}");
}
