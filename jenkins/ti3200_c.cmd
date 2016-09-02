@REM Copyright (c) Microsoft. All rights reserved.
@REM Licensed under the MIT license. See LICENSE file in the project root for full license information.

@echo off

setlocal

set installers-path=c:\ti
IF [%installers-path%]==[] goto:installerspath-not-set

set install-root=c:\\ti
if not exist "%install-root%" mkdir %install-root%

set sdkversion=1.1.0

rem // This Script can run for multiple TI SDK versions based on the value of the enviornment variable.
rem // Check for enviornment variable defining CC3200 SDK version. If defined use the SDK version
if defined CC3200_SDK_VERSION set sdkversion=%CC3200_SDK_VERSION%

set cc3200sdk=CC3200SDK_%sdkversion%

echo ********** INSTALLING CC3200 SDK **********
if not exist "%install-root%\%cc3200sdk%" start /wait %installers-path%\CC3200SDK-%sdkversion%-windows-installer.exe --mode unattended --prefix %install-root%\%cc3200sdk%

echo ********** INSTALLING Tirtos Simplelink **********
if not exist "%install-root%\tirtos_simplelink_2_14_01_20" start /wait %installers-path%\tirtos_simplelink_setupwin32_2_14_01_20.exe --mode unattended --prefix %install-root%

echo ********** EXTRACTING ns **********
if not exist "%install-root%\ns_1_10_00_00_eng" powershell.exe -nologo -noprofile -command "& { Add-Type -A 'System.IO.Compression.FileSystem'; [IO.Compression.ZipFile]::ExtractToDirectory('%installers-path%\ns_1_10_00_00_eng.zip', '%install-root%'); }"

echo ********** INSTALLING TI ARM Compiler **********
if not exist "%install-root%\ti-cgt-arm_5.2.5" start /wait %installers-path%\ti_cgt_tms470_5.2.5_windows_installer.exe --mode unattended --prefix %install-root%

set build-root=%~dp0..
rem // resolve to fully qualified path
for %%i in ("%build-root%") do set build-root=%%~fi

REM -- C --
cd /d %build-root%\c\build_all\tirtos

@echo off
echo ********** UPDATING products.mak **********
call:DoReplace "c:\\ti\\xdctools_3_31_01_33_core" "%install-root%\\xdctools_3_31_01_33_core" products.mak
call:DoReplace "c:\\ti/tirtos_simplelink_2_14_01_20" "%install-root%/tirtos_simplelink_2_14_01_20" products.mak
call:DoReplace "bios_6_42_02_29" "bios_6_42_02_29" products.mak
call:DoReplace "c:\\ti/ns_1_10_00_00_eng" "%install-root%/ns_1_10_00_00_eng" products.mak
call:DoReplace "c:\\ti/CC3200SDK_1.1.0/cc3200-sdk" "%install-root%/%cc3200sdk%/cc3200-sdk" products.mak
call:DoReplace "c:\\ti/ti-cgt-arm_5.2.5" "%install-root%/ti-cgt-arm_5.2.5" products.mak
call:DoReplace "CC3200SDK_VERSION = 1.1.0" "CC3200SDK_VERSION = %sdkversion%" products.mak

@echo on
call %install-root%\\xdctools_3_31_01_33_core\\gmake.exe clean
call %install-root%\\xdctools_3_31_01_33_core\\gmake.exe all

cd /d %build-root%\c\serializer\samples\simplesample_http\tirtos\cc3200	

@echo off
echo ********** UPDATING %build-root%\c\serializer\samples\simplesample_http\simplesample_http.c **********
call:DoReplace "[device connection string]" "%DEVICE_CONNECTION_STRING%" %build-root%\c\serializer\samples\simplesample_http\simplesample_http.c

@echo off
echo ********** UPDATING %build-root%\c\serializer\samples\simplesample_http\tirtos\cc3200\main.c **********
for /F "tokens=1-4 delims=/ " %%a in ('echo %DATE%') do (
set month=%%b
set day=%%c
set year=%%d
)
for /F "tokens=1-4 delims=:. " %%a in ('echo %TIME%') do (
set hour=%%a
set minute=%%b
set second=%%c
)
call:DoReplace "#define DAY     15" "#define DAY     %day%" %build-root%\c\serializer\samples\simplesample_http\tirtos\cc3200\main.c
call:DoReplace "#define MONTH   9" "#define MONTH   %month%" %build-root%\c\serializer\samples\simplesample_http\tirtos\cc3200\main.c
call:DoReplace "#define YEAR    2015" "#define YEAR    %year%" %build-root%\c\serializer\samples\simplesample_http\tirtos\cc3200\main.c
call:DoReplace "#define HOUR    6" "#define HOUR    %hour%" %build-root%\c\serializer\samples\simplesample_http\tirtos\cc3200\main.c
call:DoReplace "#define MINUTE  21" "#define MINUTE  %minute%" %build-root%\c\serializer\samples\simplesample_http\tirtos\cc3200\main.c
call:DoReplace "#define SECOND  0" "#define SECOND  %second%" %build-root%\c\serializer\samples\simplesample_http\tirtos\cc3200\main.c

echo ********** DOWNLOADING elf2cc32.exe **********
if not exist "%build-root%\c\serializer\samples\simplesample_http\tirtos\cc3200\tools" mkdir %build-root%\c\serializer\samples\simplesample_http\tirtos\cc3200\tools
powershell.exe -nologo -noprofile -command "& {Add-Type -A 'System.Net'; (new-object System.Net.WebClient).DownloadFile('https://github.com/tisb-vikram/azure-iot-sdks/blob/7da24633b2c4af3bc779998e9950146f061a8a10/c/serializer/samples/simplesample_http/tirtos/cc3200/tools/elf2cc32.exe?raw=true','%build-root%\c\serializer\samples\simplesample_http\tirtos\cc3200\tools\elf2cc32.exe'); }"
if not exist "%install-root%\ns_1_10_00_00_eng\tools" mkdir %install-root%\ns_1_10_00_00_eng\tools
powershell.exe -nologo -noprofile -command "& {Add-Type -A 'System.Net'; (new-object System.Net.WebClient).DownloadFile('https://github.com/tisb-vikram/azure-iot-sdks/blob/7da24633b2c4af3bc779998e9950146f061a8a10/c/serializer/samples/simplesample_http/tirtos/cc3200/tools/elf2cc32.exe?raw=true','%install-root%\ns_1_10_00_00_eng\tools\elf2cc32.exe'); }"

set PATH=%PATH%;%install-root%\xdctools_3_31_01_33_core;%install-root%\xdctools_3_31_01_33_core\bin

@echo on
call %install-root%\xdctools_3_31_01_33_core\gmake.exe clean
call %install-root%\xdctools_3_31_01_33_core\gmake.exe all

@echo off
if errorlevel 1 goto :eof
cd %build-root%
goto:eof

:DoReplace
echo ^(Get-Content "%3"^).replace^(%1, %2^) ^| Set-Content %3>Rep.ps1
Powershell.exe -executionpolicy ByPass -File Rep.ps1
if exist Rep.ps1 del Rep.ps1
goto:eof

:installerspath-not-set
echo installers-path variable not set in script
goto:eof
