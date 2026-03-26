<?php

/**
 * VLCPlayer full demo for WinBinder.
 *
 * Requirements:
 * - VLC runtime installed and libvlc.dll available in PATH (or next to php.exe).
 * - Extension build that includes VLCPlayer + wb_vlc_* APIs.
 */

define('ID_VLC', 1001);
define('ID_OPEN', 1002);
define('ID_PLAY', 1003);
define('ID_PAUSE', 1004);
define('ID_STOP', 1005);
define('ID_SEEK', 1006);
define('ID_VOLUME', 1007);
define('ID_STATUS', 1008);
define('ID_TIMER', 1009);

define('SEEK_MAX', 1000);

$window = wb_create_window(0, AppWindow, 'VLCPlayer full demo', WBC_CENTER, WBC_CENTER, 920, 640, WBC_NOTIFY, 0);

$vlc = wb_create_control($window, VLCPlayer, '', 12, 12, 896, 460, ID_VLC, WBC_BORDER);

$openBtn  = wb_create_control($window, PushButton, 'Open…', 12, 486, 100, 30, ID_OPEN);
$playBtn  = wb_create_control($window, PushButton, 'Play', 118, 486, 80, 30, ID_PLAY);
$pauseBtn = wb_create_control($window, PushButton, 'Pause', 204, 486, 80, 30, ID_PAUSE);
$stopBtn  = wb_create_control($window, PushButton, 'Stop', 290, 486, 80, 30, ID_STOP);

$seek = wb_create_control($window, Slider, '', 12, 526, 700, 30, ID_SEEK, WBC_LINES);
$volume = wb_create_control($window, Slider, '', 720, 526, 188, 30, ID_VOLUME, WBC_LINES);

$status = wb_create_control($window, Label, 'Ready. Click Open… to choose a media file.', 12, 566, 896, 58, ID_STATUS, WBC_BORDER | WBC_MULTILINE);

wb_set_range($seek, 0, SEEK_MAX);
wb_set_value($seek, 0);

wb_set_range($volume, 0, 200);
wb_set_value($volume, 100);
wb_vlc_set_volume($vlc, 100);

$GLOBALS['app'] = [
    'vlc' => $vlc,
    'seek' => $seek,
    'volume' => $volume,
    'status' => $status,
    'draggingSeek' => false,
    'mediaPath' => '',
];

wb_create_timer($window, ID_TIMER, 250);
wb_set_handler($window, 'process_main');
wb_main_loop();

function process_main($window, $id, $ctrl = 0, $param1 = 0, $param2 = 0, $param3 = 0)
{
    if ($id === IDCLOSE || $id === IDCANCEL) {
        wb_destroy_window($window);
        return;
    }

    switch ($id) {
        case ID_OPEN:
            open_media_file($window);
            break;

        case ID_PLAY:
            if (!wb_vlc_play($GLOBALS['app']['vlc'])) {
                set_status("Play failed. Did you load media and install VLC runtime (libvlc.dll)?");
            }
            break;

        case ID_PAUSE:
            wb_vlc_pause($GLOBALS['app']['vlc']);
            break;

        case ID_STOP:
            wb_vlc_stop($GLOBALS['app']['vlc']);
            wb_set_value($GLOBALS['app']['seek'], 0);
            break;

        case ID_SEEK:
            on_seek_changed();
            break;

        case ID_VOLUME:
            $volume = wb_get_value($GLOBALS['app']['volume']);
            wb_vlc_set_volume($GLOBALS['app']['vlc'], $volume);
            set_status('Volume: ' . $volume . '%');
            break;

        case ID_TIMER:
            refresh_playback_ui();
            break;
    }
}

function open_media_file($window)
{
    $filter = [
        'Media files (*.mp4;*.mkv;*.avi;*.mov;*.mp3;*.flac;*.wav;*.m4a;*.ogg)' => '*.mp4;*.mkv;*.avi;*.mov;*.mp3;*.flac;*.wav;*.m4a;*.ogg',
        'All files (*.*)' => '*.*',
    ];

    $file = wb_sys_dlg_open($window, 'Open media file', $filter, '', 0);
    if (!$file || !is_string($file)) {
        return;
    }

    $ok = wb_vlc_set_media($GLOBALS['app']['vlc'], $file, true);
    if (!$ok) {
        set_status("Could not load media:\n$file\nCheck libvlc.dll availability and path.");
        return;
    }

    $GLOBALS['app']['mediaPath'] = $file;
    wb_set_value($GLOBALS['app']['seek'], 0);
    set_status("Loaded and playing:\n$file");
}

function on_seek_changed()
{
    $lengthMs = wb_vlc_get_length($GLOBALS['app']['vlc']);
    if ($lengthMs <= 0) {
        return;
    }

    $raw = wb_get_value($GLOBALS['app']['seek']);
    $targetMs = (int)round(($raw / SEEK_MAX) * $lengthMs);
    wb_vlc_seek($GLOBALS['app']['vlc'], $targetMs);
}

function refresh_playback_ui()
{
    $vlc = $GLOBALS['app']['vlc'];
    $lengthMs = wb_vlc_get_length($vlc);
    $timeMs = wb_vlc_get_time($vlc);
    $isPlaying = wb_vlc_is_playing($vlc);

    if ($lengthMs > 0 && $timeMs >= 0) {
        $slider = (int)round(($timeMs / max(1, $lengthMs)) * SEEK_MAX);
        wb_set_value($GLOBALS['app']['seek'], max(0, min(SEEK_MAX, $slider)));
    }

    $state = $isPlaying ? 'Playing' : 'Paused/Stopped';
    $name = $GLOBALS['app']['mediaPath'] !== '' ? basename($GLOBALS['app']['mediaPath']) : 'No media';

    set_status(sprintf(
        "%s\n%s | %s / %s",
        $name,
        $state,
        format_ms($timeMs),
        format_ms($lengthMs)
    ));
}

function format_ms($ms)
{
    if (!is_int($ms) && !is_float($ms)) {
        return '00:00';
    }
    if ($ms < 0) {
        return '00:00';
    }
    $total = (int)floor($ms / 1000);
    $h = (int)floor($total / 3600);
    $m = (int)floor(($total % 3600) / 60);
    $s = $total % 60;

    if ($h > 0) {
        return sprintf('%d:%02d:%02d', $h, $m, $s);
    }
    return sprintf('%02d:%02d', $m, $s);
}

function set_status($text)
{
    wb_set_text($GLOBALS['app']['status'], $text);
}
