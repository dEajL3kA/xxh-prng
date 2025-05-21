@echo off
setlocal EnableDelayedExpansion

cd /d "%~dp0"

if "%PROCESSOR_ARCHITECTURE%" == "AMD64" (
	set EXE_FILE=bin\x64\Release\xxh_rand-x64.exe
) else (
	if "%PROCESSOR_ARCHITECTURE%" == "ARM64" (
		set EXE_FILE=bin\ARM64\Release\xxh_rand-a64.exe
	) else (
		set EXE_FILE=bin\Win32\Release\xxh_rand-x86.exe
	)
)

if not exist "%EXE_FILE%" (
	echo Error: Must build xxh_rand.exe before running the tests ^^!^^!^^!
	pause && goto:eof
)

for %%h in ("" "--hex") do (
	for %%s in (1 2) do (
		echo Computing checksum, please wait...
		set EXPECTED=00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
		set COMPUTED=ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff
		if "%%~h" == "--hex" (
			if "%%~s" == "1" set EXPECTED=59212d699c852a1fc8e5a729114eaab5abc1a4ef3e38c9b814fb34918e6544423d98eca133b0d7e657f74abdfea14858a1f0c406d8fd192d2358b605f527f063
			if "%%~s" == "2" set EXPECTED=1e95b1ba9870730e548cf4431ff155f619e3ad07c269234f90aef9c465ee9fe4e521f00471d4b34039872aa666daa6438b89e18cfdd2c23239b19a11c96beb5a
		) else (
			if "%%~s" == "1" set EXPECTED=95958f9d5db86b9ca9dbb4ed4655b40844e1c4405743b18e0296162d4879b8d67ae5c4f51e62638b2d0167473c962414c0903a4c17a1af1b54c85c4c71743083
			if "%%~s" == "2" set EXPECTED=29757303dadd627bb757b7feb40994143562eba70cd95ac0d32244fb0bebeb70ab942fe56afde15c7969988bb30539d8a47c156e28dc4f43df9eedf297dff800
		)
		for /f "usebackq delims= " %%i in (`%EXE_FILE% %%~h -- %%~s 1073741824 ^| etc\tools\win32\rhash\rhash.exe --sha512 -`) do (
			set "COMPUTED=%%~i"
		)
		if "!COMPUTED!" == "!EXPECTED!" (
			echo Success.
			echo.
		) else (
			echo Test has failed: Checksum mismatch error ^^!^^!^^!
			echo ^> Computed digest: !COMPUTED!
			echo ^> Expected digest: !EXPECTED!
			pause && goto:eof
		)
	)
)

%EXE_FILE% | etc\tools\win32\dieharder\dieharder_x64.exe -g 200 -a -k 2 -Y 1

echo.
echo All tests completed.

pause
