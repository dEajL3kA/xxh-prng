@echo off
setlocal EnableDelayedExpansion

cd /d "%~dp0"

if "%PROCESSOR_ARCHITECTURE%" == "AMD64" (
	set "EXE_FILE=bin\x64\Release\xxh_rand-x64.exe"
) else (
	if "%PROCESSOR_ARCHITECTURE%" == "ARM64" (
		set "EXE_FILE=bin\ARM64\Release\xxh_rand-a64.exe"
	) else (
		set "EXE_FILE=bin\Win32\Release\xxh_rand-x86.exe"
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
			if "%%~s" == "1" set EXPECTED=c2123cbdb8a7e3d515b33be1960a4594b4e61a49a3aca8569fec1c78855344ba3ea7e348f6cf13ec51392a3886535d7aa2c2706f89c92b69b15db38cf62018bb
			if "%%~s" == "2" set EXPECTED=f06f5a0670150e13195c19081e0611fd07590cc97f96f351b286a9213497cad283fa2492810f4c3f2d075e258f7bc5d72fffbeb91de70c528cc6830fb54ad2fa
		) else (
			if "%%~s" == "1" set EXPECTED=abbf0374e11212fe96ab1999980c453ad34426947585d1cdd748cb830d5960da1a664ff565126db322f8edd298cd96ce4292507e318e09c8088b9c3f29d6bef9
			if "%%~s" == "2" set EXPECTED=aa6a83138bd4e85c6ab1b8fc1afd9d9579f219125733bbe4cac121ae07e58b00f05c8b66ff4feb869f7038c20e03196cf1ef38b06589c6a73fb23752846dad46
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
