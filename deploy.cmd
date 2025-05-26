@echo off
setlocal EnableDelayedExpansion

cd /d "%~d0"

for /f "usebackq delims=" %%i in (`etc\tools\win32\vswhere\vswhere.exe -version 17 -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -latest -property installationPath`) do (
	if exist "%%~i\VC\Auxiliary\Build\vcvars64.bat" (
		set "VS2022INSTALLDIR=%%~i"
		goto:vs_found
	)
)

echo Microsoft Visual C++ 2022 not found ^^!^^!^^!
pause && goto:eof

:vs_found

call "%VS2022INSTALLDIR%\VC\Auxiliary\Build\vcvars64.bat"

if "%PROCESSOR_ARCHITECTURE%" == "AMD64" (
	set PreferredToolArchitecture=x64
)

for %%d in (bin obj) do (
	if exist "%CD%/%%d/" rmdir /S /Q "%CD%/%%d"
	if exist "%CD%/%%d/" (
		echo Failed to clean up intermediate files ^^!^^!^^!
		pause && goto:eof
	)
)

for %%p in (x86 x64 ARM64) do (
	msbuild /property:Platform=%%p /property:Configuration=Release /target:Rebuild "%CD%\xxh_rand.sln"
	if !ERRORLEVEL! neq 0 (
		echo.
		echo Build process has failed ^^!^^!^^!
		pause && goto:eof
	)
)

echo.
echo Build completed.

if not "%NON_INTERACTIVE%" == "1" (
	pause
)
