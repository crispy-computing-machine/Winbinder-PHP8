@echo off
setlocal enableextensions enabledelayedexpansion
	cinst wget

    echo INSTALLING PHP VERSION: %PHP_REL%
	if not exist "%PHP_BUILD_CACHE_BASE_DIR%" (
		echo Creating %PHP_BUILD_CACHE_BASE_DIR%
		mkdir "%PHP_BUILD_CACHE_BASE_DIR%"
	)

	if not exist "%PHP_BUILD_OBJ_DIR%" (
		echo Creating %PHP_BUILD_OBJ_DIR%
		mkdir "%PHP_BUILD_OBJ_DIR%"
	)

	if not exist "%PHP_BUILD_CACHE_SDK_DIR%" (
		echo Cloning remote SDK repository
		rem git clone -q --depth=1 --branch %SDK_BRANCH% %SDK_REMOTE% "%PHP_BUILD_CACHE_SDK_DIR%" 2>&1
		git clone --branch %SDK_BRANCH% %SDK_REMOTE% "%PHP_BUILD_CACHE_SDK_DIR%" -v
	) else (
		echo Fetching remote SDK repository
		git --git-dir="%PHP_BUILD_CACHE_SDK_DIR%\.git" --work-tree="%PHP_BUILD_CACHE_SDK_DIR%" fetch --prune origin -v
		echo Checkout SDK repository branch
		git --git-dir="%PHP_BUILD_CACHE_SDK_DIR%\.git" --work-tree="%PHP_BUILD_CACHE_SDK_DIR%" checkout --force %SDK_BRANCH% -v
	)

	rem if "%PHP_REL%"=="master" (
	rem 	echo git clone -q --depth=1 --branch=%PHP_REL% https://github.com/php/php-src C:\projects\php-src
	rem 	git clone -q --depth=1 --branch=%PHP_REL% https://github.com/php/php-src C:\projects\php-src
	rem ) else (
	rem 	echo git clone -q --depth=1 --branch=PHP-%PHP_REL% https://github.com/php/php-src C:\projects\php-src
	rem 	git clone -q --depth=1 --branch=PHP-%PHP_REL% https://github.com/php/php-src C:\projects\php-src
	rem )

	rem powershell -Command "(New-Object Net.WebClient).DownloadFile('http://windows.php.net/downloads/releases/', 'C:\projects\php-src\php8.zip')"
	echo Downloading deps from php website...
	if not exist "%PHP_BUILD_PRJ_DIR%" (
        echo Creating %PHP_BUILD_PRJ_DIR%
        mkdir "%PHP_BUILD_PRJ_DIR%"
    )
	if not exist "%PHP_BUILD_SRC_DIR%" (
        echo Creating %PHP_BUILD_SRC_DIR%
        mkdir "%PHP_BUILD_SRC_DIR%"
    )
	echo https://windows.php.net/downloads/releases/archives/php-devel-pack-%PHP_VER%!ZTS_SHORT!-Win32-%PHP_BUILD_CRT%-x86.zip
	rem http://windows.php.net/downloads/releases/php-devel-pack-8.0.3-Win32-vs16-x86.zip
	powershell -Command "Invoke-WebRequest http://windows.php.net/downloads/releases/archives/php-devel-pack-%PHP_VER%!ZTS_SHORT!-Win32-%PHP_BUILD_CRT%-x86.zip -OutFile C:\projects\php-src\php8.zip"
	7z x %PHP_BUILD_SRC_DIR%\php8.zip -o%PHP_BUILD_SRC_DIR% -r -y -bb
	echo Listing src directory...
	tree %PHP_BUILD_SRC_DIR%
	xcopy %APPVEYOR_BUILD_FOLDER% %PHP_BUILD_SRC_DIR%\ext\winbinder\ /s /e /y /f

	rem xcopy %APPVEYOR_BUILD_FOLDER%\LICENSE %APPVEYOR_BUILD_FOLDER%\artifacts\ /y /f

	if "%APPVEYOR%" equ "True" rmdir /s /q C:\cygwin >NUL 2>NUL
	if %errorlevel% neq 0 exit /b 3
	if "%APPVEYOR%" equ "True" rmdir /s /q C:\cygwin64 >NUL 2>NUL
	if %errorlevel% neq 0 exit /b 3
	if "%APPVEYOR%" equ "True" rmdir /s /q C:\mingw >NUL 2>NUL
	if %errorlevel% neq 0 exit /b 3
	if "%APPVEYOR%" equ "True" rmdir /s /q C:\mingw-w64 >NUL 2>NUL
	if %errorlevel% neq 0 exit /b 3

	if "%APPVEYOR_REPO_TAG_NAME%"=="" (
		set APPVEYOR_REPO_TAG_NAME=%APPVEYOR_REPO_BRANCH%-%APPVEYOR_REPO_COMMIT:~0,8%
		for /f "delims=" %%l in (wb\wb_version.h) do (
			if not "%%l"=="" (
				set line=%%l
				if "!line:~8,17!"=="WINBINDER_VERSION" (
					set APPVEYOR_REPO_TAG_NAME=!line:~29,-1!-%APPVEYOR_REPO_BRANCH%-%APPVEYOR_REPO_COMMIT:~0,8%
				)
			)
		)

		appveyor SetVariable -Name APPVEYOR_REPO_TAG_NAME -Value !APPVEYOR_REPO_TAG_NAME!
	)
endlocal