@echo off
setlocal EnableDelayedExpansion

cd /d "%~d0"

if "%VS2022INSTALLDIR%" == "" (
	set "VS2022INSTALLDIR=C:\Program Files\Microsoft Visual Studio\2022\Community"
)

if not exist "%VS2022INSTALLDIR%\VC\Auxiliary\Build\vcvars64.bat" (
	echo Microsoft Visual C++ not found ^^!^^!^^!
	pause && goto:eof
)

call "%VS2022INSTALLDIR%\VC\Auxiliary\Build\vcvars64.bat" x64

for %%d in (bin obj) do (
	if exist "%CD%/%%d/" rmdir /S /Q "%CD%/%%d"
)

for %%p in (x86 x64) do (
	msbuild /property:Platform=%%p /property:Configuration=Release /target:Rebuild "%CD%\xxh_rand.sln"
	if !ERRORLEVEL! neq 0 (
		echo.
		echo Build process has failed ^^!^^!^^!
		pause && goto:eof
	)
)

echo.
echo Build successfully completed.

pause
