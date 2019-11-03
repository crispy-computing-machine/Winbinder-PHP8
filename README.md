WinBinder
=========
WinBinder - The native Windows binding for PHP

What is WinBinder?
==================

WinBinder is an open source extension to PHP, the script programming language. It allows PHP programmers to easily build native Windows applications, producing quick and rewarding results with minimum effort. Even short scripts with a few dozen lines can generate a useful program, thanks to the power and flexibility of PHP.

What is needed to start building WinBinder applications?
========================================================

You will need the source code of PHP plus the WinBinder source code.

On which PHP versions has it been tested on?
  - PHP 7.0 
  - PHP 7.2

Prerequisites:
--------------
  1. Get Microsoft Visual Studio (Express version will do)
  2. Get PHP source code, e.g. from http://windows.php.net/download/
  3. Compile PHP by following this guide https://wiki.php.net/internals/windows/stepbystepbuild
  4. Clone this repo into the folder ext/winbinder
  5. compile PHP with your options
    1. add <code>--with-winbinder</code> to compile WinBinder statically into php.exe<br/>
      <code>configure.bat --enable-snapshot-build --enable-debug-pack --with-winbinder</code>
    2. add <code>--with-winbinder=shared</code> to compile WinBinder shared, i.e., to get a *.dll<br/>
      <code>configure.bat --enable-snapshot-build --enable-debug-pack --with-winbinder=shared</code>

Where to get further information?
=================================

See the [wiki](https://github.com/crispy-computing-machine/Winbinder/wiki)

Reporting Bugs
========================

Report them via github [issues](https://github.com/crispy-computing-machine/Winbinder/issues).

About
========================
Forked from [Wagy](https://github.com/wagy/WinBinder) for PHP7 support