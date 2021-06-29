@echo off
setlocal enableextensions enabledelayedexpansion

	rem change into bild folder
	cd /D %APPVEYOR_BUILD_FOLDER%
	if %errorlevel% neq 0 exit /b 3

	rem Set DEPS_DIR
	set STABILITY=staging
	set DEPS_DIR=%PHP_BUILD_CACHE_BASE_DIR%\deps-%PHP_REL%-%PHP_REL%-%ARCHITECTURES%
	echo %DEPS_DIR%

	cd /d %PHP_BUILD_CACHE_SDK_DIR%\bin\

	rem SDK is cached, deps info is cached as well
	echo Updating dependencies in %DEPS_DIR%
	cmd /c phpsdk_deps.bat --update --no-backup --branch %PHP_REL% --stability %STABILITY% --deps %DEPS_DIR% --crt %PHP_BUILD_CRT%
	if %errorlevel% neq 0 exit /b 3

	rem Something went wrong, most likely when concurrent builds were to fetch deps
	rem updates. It might be, that some locking mechanism is needed.
	if not exist "%DEPS_DIR%" (
		cmd /c phpsdk_deps.bat --update --force --no-backup --branch %PHP_REL% --stability %STABILITY% --deps %DEPS_DIR% --crt %PHP_BUILD_CRT%
	)
	if %errorlevel% neq 0 exit /b 3

	rem New PHP 8 starter batch file
	cmd /c phpsdk-%PHP_REL%-%PHP_SDK_ARCH%.bat
	if %errorlevel% neq 0 exit /b 3

	rem Build php 8 build directory structure
	cmd /c phpsdk_buildtree.bat.bat phpmaster
	if %errorlevel% neq 0 exit /b 3

	rem normal buildconf
	cmd /c buildconf.bat --force
	if %errorlevel% neq 0 exit /b 3

	rem normal configure with module enabled
	cmd /c configure.bat --disable-all --with-mp=auto --enable-cli --%ZTS_STATE%-zts --with-winbinder=shared --enable-object-out-dir=%PHP_BUILD_OBJ_DIR% --with-config-file-scan-dir=%APPVEYOR_BUILD_FOLDER%\build\modules.d --with-prefix=%APPVEYOR_BUILD_FOLDER%\build --with-php-build=%DEPS_DIR%
	if %errorlevel% neq 0 exit /b 3

	nmake /NOLOGO
	if %errorlevel% neq 0 exit /b 3

	nmake install
	if %errorlevel% neq 0 exit /b 3

    if %errorlevel% == 0 echo BUILD SUCCESS!!!

	cd /d %APPVEYOR_BUILD_FOLDER%

	if not exist "%APPVEYOR_BUILD_FOLDER%\build\ext\php_winbinder.dll" exit /b 3

	rem xcopy %APPVEYOR_BUILD_FOLDER%\LICENSE %APPVEYOR_BUILD_FOLDER%\php_winbinder-%PHP_REL%-!ZTS_SHORT!-%PHP_BUILD_CRT%-%PHP_SDK_ARCH%\ /y /f
	xcopy %APPVEYOR_BUILD_FOLDER%\docs %APPVEYOR_BUILD_FOLDER%\php_winbinder-%PHP_REL%-!ZTS_SHORT!-%PHP_BUILD_CRT%-%PHP_SDK_ARCH%\docs\ /y /f
	xcopy %APPVEYOR_BUILD_FOLDER%\php %APPVEYOR_BUILD_FOLDER%\php_winbinder-%PHP_REL%-!ZTS_SHORT!-%PHP_BUILD_CRT%-%PHP_SDK_ARCH%\examples\ /y /f
	rem xcopy %APPVEYOR_BUILD_FOLDER%\build\ext\php_winbinder.dll %APPVEYOR_BUILD_FOLDER%\php_winbinder-%PHP_REL%-!ZTS_SHORT!-%PHP_BUILD_CRT%-%PHP_SDK_ARCH%\ /y /f
	xcopy %APPVEYOR_BUILD_FOLDER%\build\ext\*.dll %APPVEYOR_BUILD_FOLDER%\php_winbinder-%PHP_REL%-!ZTS_SHORT!-%PHP_BUILD_CRT%-%PHP_SDK_ARCH%\ /y /f
	7z a php_winbinder-%PHP_REL%-!ZTS_SHORT!-%PHP_BUILD_CRT%-%PHP_SDK_ARCH%.zip %APPVEYOR_BUILD_FOLDER%\php_winbinder-%PHP_REL%-!ZTS_SHORT!-%PHP_BUILD_CRT%-%PHP_SDK_ARCH%\*
	appveyor PushArtifact php_winbinder-%PHP_REL%-!ZTS_SHORT!-%PHP_BUILD_CRT%-%PHP_SDK_ARCH%.zip -FileName php_winbinder-%APPVEYOR_REPO_TAG_NAME%-%PHP_REL%-!ZTS_SHORT!-%PHP_BUILD_CRT%-%PHP_SDK_ARCH%.zip
	move build\ext\php_winbinder.dll artifacts\php_winbinder-%PHP_REL%-!ZTS_SHORT!-%PHP_BUILD_CRT%-%PHP_SDK_ARCH%.dll

	if %errorlevel% == 0 echo ASSET READY FOR DOWNLOAD!!!
endlocal
