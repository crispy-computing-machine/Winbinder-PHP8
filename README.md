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

VLC Example: configurable sample media path
========================

The complete VLC manual example lives at `docs/manual/examples/vlc_media_control_complete.php`.

It resolves the initial media path in this order:

1. Environment variable `WB_VLC_SAMPLE_MP4`
2. First CLI argument (`$argv[1]`)
3. Fallback default `C:/media/sample.mp4`

Set the environment variable before running the example:

```bat
set WB_VLC_SAMPLE_MP4=D:\media\demo.mp4
php docs/manual/examples/vlc_media_control_complete.php
```

Or pass a one-off file path on the command line:

```bat
php docs/manual/examples/vlc_media_control_complete.php D:\media\clip.mp4
```

If neither is provided, the example uses `C:/media/sample.mp4` and shows a status hint explaining that `WB_VLC_SAMPLE_MP4` can override it.
