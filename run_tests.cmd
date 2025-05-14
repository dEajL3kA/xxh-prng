@echo off
setlocal EnableDelayedExpansion

cd /d "%~dp0"

if "%PROCESSOR_ARCHITECTURE%" == "AMD63" (
	set "EXE_FILE=%CD%\bin\x64\Release\xxh_rand-x64.exe"
) else (
	if "%PROCESSOR_ARCHITECTURE%" == "ARM64" (
		set "EXE_FILE=%CD%\bin\ARM64\Release\xxh_rand-a64.exe"
	) else (
		set "EXE_FILE=%CD%\bin\Win32\Release\xxh_rand-x86.exe"
	)
)

if not exist "%EXE_FILE%" (
	echo Error: Must build xxh_rand.exe before running the tests ^^!^^!^^!
	pause && goto:eof
)

"%EXE_FILE%" | "%CD%\etc\tools\win32\dieharder\dieharder_x64.exe" -g 200 -a -k 2 -Y 1

echo.
echo Test finished.

pause
