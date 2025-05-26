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
			if "%%~s" == "1" set EXPECTED=c8bb566c40b78f82d3cd693d1f3f4dbe0d33b0c5e4c842621a3f3494b910e77193f20c5c947edfeaf68bb80b766398c4d42f7508eb3803af02d9e715140f9f1c
			if "%%~s" == "2" set EXPECTED=1441493d403335481de5dcce203d622a2ab158389809deaae96659a752973476cbd01f9753f60e1af580930ec67e8227b8747dcebbd310cbd0f7f8af3cadfff3
		) else (
			if "%%~s" == "1" set EXPECTED=c3b859699c69271ac8c28dfeef270b336dfde71b9c9c441b6364a189baa9710b231f950cb5663bdf2f58a9c07ad7a411783082a5e7711b08544fa7333b32210a
			if "%%~s" == "2" set EXPECTED=397c3f14b786b1256faa16f8805584ccbf879370d57fc1561de43eb623677dfae35017440e71591f99f69a0cdbf9b7d8eab5e4c68ab2fda0a7e048d2b2204c1a
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
