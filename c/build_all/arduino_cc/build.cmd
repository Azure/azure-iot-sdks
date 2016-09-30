@REM Copyright (c) Microsoft. All rights reserved.
@REM Licensed under the MIT license. See LICENSE file in the project root for full license information.

@setlocal EnableExtensions EnableDelayedExpansion
@echo off

set current_path=%~dp0
rem // remove trailing slash
set current_path=%current_path:~0,-1%

set build_root=%current_path%\..\..\..\..
rem // resolve to fully qualified path
for %%i in ("%build_root%") do set build_root=%%~fi

set work_root=%build_root%\Work
set compiler_path=%build_root%\%IOTHUB_ARDUINO_VERSION%

set compiler_hardware_path=%compiler_path%\hardware
set compiler_tools_builder_path=%compiler_path%\tools-builder
set compiler_tools_processor_path=%compiler_path%\hardware\tools\avr
set compiler_libraries_path=%compiler_path%\libraries

set user_hardware_path=%work_root%\arduino\hardware
set user_libraries_path=%work_root%\arduino\libraries
set user_packages_path=%build_root%\arduino15\packages

set test_path=%build_root%\iot-hub-c-huzzah-getstartedkit\remote_monitoring\remote_monitoring.ino

rem -----------------------------------------------------------------------------
rem -- parse script arguments
rem -----------------------------------------------------------------------------
set build_clean=OFF
set build_test=OFF
set run_test=OFF

:args_loop
if "%1" equ "" goto args_done
if "%1" equ "-c" goto arg_build_clean
if "%1" equ "--clean" goto arg_build_clean
if "%1" equ "-b" goto arg_build_only
if "%1" equ "--build-only" goto arg_build_only
if "%1" equ "-r" goto arg_run_e2e_tests
if "%1" equ "--run-e2e-tests" goto arg_run_e2e_tests
call :usage && exit /b 1

:arg_build_clean
set build_clean=ON
goto args_continue

:arg_build_only
set build_test=ON
goto args_continue

:arg_run_e2e_tests
set build_test=ON
set run_test=ON
goto args_continue

:args_continue
shift
goto args_loop

:args_done

rem -----------------------------------------------------------------------------
rem -- clean solutions
rem -----------------------------------------------------------------------------

if %build_clean%==ON (
    rmdir /S /Q %work_root%
)

rem -----------------------------------------------------------------------------
rem -- download arduino compiler
rem -----------------------------------------------------------------------------
call download_blob.cmd -directory %build_root% -file %IOTHUB_ARDUINO_VERSION% -check arduino-builder.exe

rem -----------------------------------------------------------------------------
rem -- create test directories
rem -----------------------------------------------------------------------------
mkdir %work_root%

rem -----------------------------------------------------------------------------
rem -- download arduino libraries
rem -----------------------------------------------------------------------------
mkdir %user_libraries_path%
pushd %user_libraries_path%
git clone https://github.com/adafruit/Adafruit_Sensor
git clone https://github.com/adafruit/Adafruit_DHT_Unified
git clone https://github.com/adafruit/DHT-sensor-library
git clone https://github.com/adafruit/Adafruit_BME280_Library
git clone https://github.com/adafruit/Adafruit_WINC1500
git clone https://github.com/arduino-libraries/RTCZero
popd

call make_sdk.cmd %work_root%

rem -----------------------------------------------------------------------------
rem -- download arduino hardware
rem -----------------------------------------------------------------------------
set hardware_path=%work_root%\arduino\hardware\esp8266com

mkdir %hardware_path%
pushd %hardware_path%
git clone https://github.com/esp8266/Arduino
rename %hardware_path%\Arduino esp8266
popd

call download_blob.cmd -directory %user_packages_path% -file esp8266 -check hardware\esp8266\2.2.0\libraries
call download_blob.cmd -directory %user_packages_path% -file adafruit -check hardware\samd\1.0.9\libraries
call download_blob.cmd -directory %user_packages_path% -file arduino -check hardware\samd\1.6.6\libraries

rem -----------------------------------------------------------------------------
rem -- build solution
rem -----------------------------------------------------------------------------

if %build_test%==ON (
    call :ParserTestLst
)

rem -----------------------------------------------------------------------------
rem -- upload and execute solution
rem -----------------------------------------------------------------------------

rem Step 3, upload to the test tool (not implemented):
rem Ex: C:\Users\iottestuser\AppData\Local\Arduino15\packages\esp8266\tools\esptool\0.4.8/esptool.exe -vv -cd nodemcu -cb 115200 -cp COM4 -ca 0x00000 -cf C:\Users\iottestuser\AppData\Local\Temp\build505ec6e3f4000475ae6da076f93e5ffe.tmp/remote_monitoring.ino.bin
rem
rem Step 4, execute the test invoking features in the test tool (not defined and implemented):

rem if %run_test%==ON (
rem )

popd
goto :eof


rem -----------------------------------------------------------------------------
rem -- subroutines
rem -----------------------------------------------------------------------------

:usage
echo build.cmd [options]
echo options:
echo  -c, --clean           delete artifacts from previous build before building
echo  -b, --build-only      only build the project
echo  -r, --run-test        run end-to-end test
goto :eof


rem -----------------------------------------------------------------------------
rem -- helper subroutines
rem -----------------------------------------------------------------------------

REM Parser tests.lst
:ParserTestLst
set buildlog=tests.lst
set testName=false
set projectName=

for /F "tokens=*" %%F in (%buildlog%) do (
    if /i "%%F"=="" (
        REM ignore empty line.
    ) else ( 
        set command=%%F
        if /i "!command:~0,1!"=="#" (
            echo Comment=!command:~1!
        ) else ( if /i "!command:~0,1!"=="[" (
            call :ExecuteTest
            set projectName=!command:~1,-1!
            if /i "!projectName!"=="End" goto :eof
        ) else (
            for /f "tokens=1 delims==" %%A in ("!command!") do set key=%%A
            call set value=%%command:!key!=%%
            set value=!value:~1!
            set !key!=!value!
        ) ) ) 
    ) 
)
goto :eof


REM Execute each test in the Tests.lst
:ExecuteTest
if not "!projectName!"=="" (
    echo.
    echo Execute !projectName!
    echo   RelativePath=!RelativePath!
    echo   RelativeWorkingDir=!RelativeWorkingDir!
    echo   MaxAllowedDurationSeconds=!MaxAllowedDurationSeconds!
    echo   Categories=!Categories!
    echo   Hardware=!Hardware!
    echo   CPUParameters=!CPUParameters!
    echo   Build=!Build!
    echo   Installation=!Installation!
    echo   Execution=!Execution!

    if "!Build!"=="Disable" (
        echo ** Build for !projectName! is disable. **
        goto :eof
    )
    
    mkdir %build_root%!RelativeWorkingDir!

rem Step 1, build dump preferences:
rem Ex: arduino-builder 
rem                        -dump-prefs 
rem                     -logger=machine 
rem                     -hardware "C:\Program Files (x86)\Arduino\hardware" 
rem                     -hardware "C:\Users\iottestuser\AppData\Local\Arduino15\packages" 
rem                     -hardware "C:\Users\iottestuser\Documents\Arduino\hardware" 
rem                     -tools "C:\Program Files (x86)\Arduino\tools-builder" 
rem                     -tools "C:\Program Files (x86)\Arduino\hardware\tools\avr" 
rem                     -tools "C:\Users\iottestuser\AppData\Local\Arduino15\packages" 
rem                     -built-in-libraries "C:\Program Files (x86)\Arduino\libraries" 
rem                     -libraries "C:\Users\iottestuser\Documents\Arduino\libraries" 
rem                     -fqbn=esp8266:esp8266:huzzah:CpuFrequency=80,UploadSpeed=115200,FlashSize=4M3M 
rem                     -ide-version=10609 
rem                     -build-path "C:\Users\iottestuser\AppData\Local\Temp\build505ec6e3f4000475ae6da076f93e5ffe.tmp" 
rem                     -warnings=none 
rem                     -prefs=build.warn_data_percentage=75 
rem                     -verbose 
rem                     "F:\Azure\IoT\SDKs\iot-hub-c-huzzah-getstartedkit-master\remote_monitoring\remote_monitoring.ino"
rem
rem Step 2, compile the project:
rem Ex: arduino-builder  
rem                     -compile  
rem                     -logger=machine  
rem                     -hardware "C:\Program Files (x86)\Arduino\hardware"  
rem                     -hardware "C:\Users\iottestuser\AppData\Local\Arduino15\packages"  
rem                     -hardware "C:\Users\iottestuser\Documents\Arduino\hardware"  
rem                     -tools "C:\Program Files (x86)\Arduino\tools-builder"  
rem                     -tools "C:\Program Files (x86)\Arduino\hardware\tools\avr"  
rem                     -tools "C:\Users\iottestuser\AppData\Local\Arduino15\packages"  
rem                     -built-in-libraries "C:\Program Files (x86)\Arduino\libraries"  
rem                     -libraries "C:\Users\iottestuser\Documents\Arduino\libraries"  
rem                     -fqbn=esp8266:esp8266:huzzah:CpuFrequency=80,UploadSpeed=115200,FlashSize=4M3M  
rem                     -ide-version=10609  
rem                     -build-path "C:\Users\iottestuser\AppData\Local\Temp\build505ec6e3f4000475ae6da076f93e5ffe.tmp"  
rem                     -warnings=none  
rem                     -prefs=build.warn_data_percentage=75  
rem                     -verbose  
rem                     "F:\Azure\IoT\SDKs\iot-hub-c-huzzah-getstartedkit-master\remote_monitoring\remote_monitoring.ino"

    set compiler_name=%compiler_path%\arduino-builder.exe

    set hardware_parameters=-hardware "%compiler_hardware_path%" -hardware "%user_hardware_path%" -hardware "%user_packages_path%" -hardware "%user_libraries_path%"
    set tools_parameters=-tools "%compiler_tools_builder_path%" -tools "%compiler_tools_processor_path%" -tools "%user_packages_path%"
    set libraries_parameters=-built-in-libraries "%compiler_libraries_path%" -libraries "%user_libraries_path%"
    set parameters=-logger=machine !hardware_parameters! !tools_parameters! !libraries_parameters! !CPUParameters! -build-path "%build_root%!RelativeWorkingDir!" -warnings=none -prefs=build.warn_data_percentage=75 -verbose %build_root%!RelativePath!

    echo Dump Arduino preferences:
    echo  !compiler_name! -dump-prefs !parameters!
    call !compiler_name! -dump-prefs !parameters!
    
    echo Build !RelativePath!:
    echo  !compiler_name! -compile !parameters!
    call !compiler_name! -compile !parameters!

)
goto :eof
