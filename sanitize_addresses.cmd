@echo off
setlocal EnableDelayedExpansion

cd /d "%~dp0"

if not exist "%DR_MEMORY_PATH%\drmemory.exe" (
	echo Error: Dr. Memory executable file could not be found ^^!^^!^^!
	pause && goto:eof
)

if "%PROCESSOR_ARCHITECTURE%" == "AMD64" (
	set EXE_FILE=bin\x64\Debug\xxh_rand-x64.exe
) else (
	if "%PROCESSOR_ARCHITECTURE%" == "ARM64" (
		set EXE_FILE=bin\ARM64\Debug\xxh_rand-a64.exe
	) else (
		set EXE_FILE=bin\Win32\Debug\xxh_rand-x86.exe
	)
)

if not exist "%EXE_FILE%" (
	echo Error: Must build xxh_rand.exe before running the tests ^^!^^!^^!
	pause && goto:eof
)

for %%h in ("" "--hex") do (
	"%DR_MEMORY_PATH%\drmemory.exe" -- %EXE_FILE% %%~h - 8192 > NUL
)

echo.
echo All tests completed.

pause
