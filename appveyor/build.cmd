@echo off
setlocal enableextensions enabledelayedexpansion
	for %%a in (%ARCHITECTURES%) do (
		set ARCH=%%a

		cmd /c !SDK_RUNNER! -t %APPVEYOR_BUILD_FOLDER%\appveyor\build_task.cmd
		if %errorlevel% neq 0 exit /b 3

	)
endlocal