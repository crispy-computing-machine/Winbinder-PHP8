@echo off
setlocal enableextensions enabledelayedexpansion
	cmd /c %APPVEYOR_BUILD_FOLDER%\appveyor\build_task.cmd
	if %errorlevel% neq 0 exit /b 3
endlocal