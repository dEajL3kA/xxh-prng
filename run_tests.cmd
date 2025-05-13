@echo off
setlocal EnableDelayedExpansion

cd /d "%~dp0"

if not exist "%CD%\bin\x64\Release\xxh_rand.exe" (
	echo Error: Must build xxh_rand.exe before running the tests ^^!^^!^^!
	pause && goto:eof
)

"%CD%\bin\x64\Release\xxh_rand.exe" | "%CD%\etc\tests\tools\win32\dieharder_x64.exe" -g 200 -a -k 2 -Y 1

echo.
echo Test finished.

pause
