@setlocal EnableExtensions EnableDelayedExpansion
@echo off

:args-loop
if "%1" equ "" goto args-done
if "%1" equ "-b" (
	set "mbed_bin_path=%2"
	goto args-continue
)
if "%1" equ "-d" (
	set "mbed_usb_drive_letter=%2"
	goto args-continue
)
if "%1" equ "-p" (
	set "mbed_serial_port=%2"
	goto args-continue
)

if "%1" equ "-deps" (
	set "dependency_paths=%~2"
	goto args-continue
)

:args-continue
shift
goto args-loop

:args-done


if "%mbed_bin_path%"=="" (
	powershell -Command ".\Run-TestOnMBED.ps1 -Port %mbed_serial_port%" -DependencyPaths "%dependency_paths%"
) else (
	powershell -Command ".\Run-TestOnMBED.ps1 -TestBinaryFilePath %mbed_bin_path% -MBEDUsbDriveLetter %mbed_usb_drive_letter% -Port %mbed_serial_port%" -DependencyPaths "%dependency_paths%"
)

exit /b %LASTEXITCODE%