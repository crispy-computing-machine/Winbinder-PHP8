WinBinder
========================
WinBinder - The native Windows binding for PHP 8.

[![Build status](https://ci.appveyor.com/api/projects/status/github/crispy-computing-machine/Winbinder?svg=true&branch=php8)](https://ci.appveyor.com/api/projects/status/github/crispy-computing-machine/Winbinder?svg=true&branch=php8) PHP 8 x64


What is WinBinder?
========================

WinBinder is an open source extension to PHP, the script programming language. It allows PHP programmers to easily build native Windows applications, producing quick and rewarding results with minimum effort. Even short scripts with a few dozen lines can generate a useful program, thanks to the power and flexibility of PHP.

[Docs](https://crispy-computing-machine.github.io/Winbinder-Docs/)

[Examples](https://github.com/crispy-computing-machine/Winbinder-Examples)

[IDE Stub File](https://gist.github.com/crispy-computing-machine/ee4e94ba5e4870a7907535185af14ba0)


PHP Build Compatibility
========================
  - PHP 7.* ❌ (unsupported)
  - PHP 8.4 ✅
  
(For older PHP version compatibility please see PECL website for old .dll's)

Latest Features & Updates
========================

[Wiki - New features!](https://github.com/crispy-computing-machine/Winbinder/wiki/New-Features-Functions)

Reporting Bugs
========================

Report them via github [issues](https://github.com/crispy-computing-machine/Winbinder/issues).

About
========================
Forked from [Wagy](https://github.com/wagy/WinBinder) for PHP7 support

**Updated for PHP 8 compatibility - I have no idea what I am doing, but it seems to work!**

# ⚠️ DO NOT USE IN PRODUCTION!
# ⚠️ No warranty provided!

VLC / libVLC integration (VlcMediaControl)
========================

WinBinder now includes a `VlcMediaControl` host control class for embedding libVLC video output.

Runtime requirements:
- `libvlc.dll` must be available at runtime (PATH, app folder, or system search path).
- VLC plugin/runtime DLLs from the same VLC distribution must also be present.
- Keep a `plugins` directory next to `libvlc.dll` (or ensure VLC plugin-path is discoverable), otherwise video output modules may not load.
- Architecture must match (`php.exe`, `php_winbinder.dll`, and VLC runtime all x86 or all x64).

Graceful fallback behavior:
- `wb_create_control(..., VlcMediaControl, ...)` still creates a plain host window even if VLC is missing.
- `wb_vlc_is_available()` reports whether libVLC symbols were loaded successfully.
- `wb_vlc_create_player()` returns `NULL` when VLC is unavailable; all other VLC APIs return `FALSE` for invalid/missing runtime.

Minimal API:
- `wb_vlc_is_available()`
- `wb_vlc_create_player($vlcHostControl)`
- `wb_vlc_destroy_player($vlcPlayer)`
- `wb_vlc_set_media($vlcPlayer, $pathOrUrl)`
- `wb_vlc_play($vlcPlayer)`, `wb_vlc_pause($vlcPlayer)`, `wb_vlc_stop($vlcPlayer)`
- `wb_vlc_set_volume($vlcPlayer, $volume)` (`0..200`)
- `wb_vlc_set_position($vlcPlayer, $position)` (`0.0..1.0`)

Callbacks / events:
- VLC player state changes are dispatched to the window callback using control id + event code.
- Event codes: `WBC_VLC_PLAYING`, `WBC_VLC_PAUSED`, `WBC_VLC_ENDED`, `WBC_VLC_ERROR`.

Example usage (PHP)
-------------------

```php
<?php

function main_handler($window, $id, $ctrl, $lparam1, $lparam2, $lparam3) {
    global $vlcHost;

    if ($id === wb_get_id($vlcHost)) {
        switch ($lparam1) {
            case WBC_VLC_PLAYING:
                echo "VLC: playing\n";
                break;
            case WBC_VLC_PAUSED:
                echo "VLC: paused\n";
                break;
            case WBC_VLC_ENDED:
                echo "VLC: ended\n";
                break;
            case WBC_VLC_ERROR:
                echo "VLC: error\n";
                break;
        }
    }

    if ($id === IDOK) {
        wb_destroy_window($window);
    }

    return 0;
}

$main = wb_create_window(NULL, AppWindow, "WinBinder + VLC", 100, 100, 900, 600, 0, WBC_NOTIFY);
$vlcHost = wb_create_control($main, VlcMediaControl, "", 10, 10, 860, 500, 1001, 0, 0);
$btnStop = wb_create_control($main, PushButton, "Stop", 10, 525, 80, 28, IDOK, 0);

if (!wb_vlc_is_available()) {
    wb_message_box($main, "VLC runtime not available (libvlc.dll missing or invalid).", "VLC", MB_ICONWARNING);
    wb_main_loop();
    exit;
}

$player = wb_vlc_create_player($vlcHost);
if ($player) {
    wb_vlc_set_media($player, "C:/media/sample.mp4"); // Or URL: "https://example.com/video.mp4"
    wb_vlc_set_volume($player, 90);
    wb_vlc_play($player);
}

wb_set_handler($main, "main_handler");
wb_main_loop();

if ($player) {
    wb_vlc_destroy_player($player);
}
```

Notes:
- Replace `C:/media/sample.mp4` with an existing file on your machine.
- For URLs, VLC/network support must be present in your VLC runtime package.
- If VLC is unavailable, the host control still exists but VLC APIs return `NULL`/`FALSE`.
- If you only copied `libvlc.dll` + `libvlccore.dll`, also copy the matching `plugins` folder or video may not render.
