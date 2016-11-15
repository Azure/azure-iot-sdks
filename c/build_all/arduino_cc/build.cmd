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

set work_root=%build_root%\arduino_cc_Work
set test_root=%build_root%\arduino_cc_Work
set tests_list_file=tests.lst
set compiler_path=%build_root%\%IOTHUB_ARDUINO_VERSION%

set compiler_hardware_path=%compiler_path%\hardware
set compiler_tools_builder_path=%compiler_path%\tools-builder
set compiler_tools_processor_path=%compiler_path%\hardware\tools\avr
set compiler_libraries_path=%compiler_path%\libraries

set user_hardware_path=%work_root%\arduino\hardware
set user_libraries_path=%work_root%\arduino\libraries
set user_packages_path=%build_root%\arduino15\packages

rem -----------------------------------------------------------------------------
rem -- parse script arguments
rem -----------------------------------------------------------------------------
set build_clean=OFF
set build_test=ON
set run_test=OFF
set clone_test=ON

:args_loop
if "%1" equ "" goto args_done
if "%1" equ "-c" goto arg_build_clean
if "%1" equ "--clean" goto arg_build_clean
if "%1" equ "--root" goto arg_test_root
if "%1" equ "--tests" goto arg_test_tests
if "%1" equ "-t" goto arg_test_tests
if "%1" equ "-b" goto arg_build_only
if "%1" equ "--build-only" goto arg_build_only
if "%1" equ "-r" goto arg_run_only
if "%1" equ "--run-only" goto arg_run_only
if "%1" equ "-e2e" goto arg_run_e2e_tests
if "%1" equ "--run-e2e-tests" goto arg_run_e2e_tests
call :usage && exit /b 1

:arg_build_clean
set build_clean=ON
goto args_continue

:arg_test_root
set clone_test=OFF
set test_root=%2
shift
goto args_continue

:arg_test_tests
set tests_list_file=%2
shift
goto args_continue

:arg_build_only
set build_test=ON
goto args_continue

:arg_run_only
set build_test=OFF
set run_test=ON
goto args_continue

:arg_run_e2e_tests
set build_test=ON
set run_test=ON
goto args_continue

:args_continue
shift
goto args_loop

:args_done

echo.
echo Start compilation for Arduino with the follow parameters:
echo.
echo   build_clean = %build_clean%
echo   build_test  = %build_test%
echo   run_test    = %run_test%
echo   clone_test  = %clone_test%
echo.
echo   current_path = %current_path%
echo   build_root   = %build_root%
echo   work_root    = %work_root%
echo   test_root    = %test_root%
echo.
echo   compiler_path                 = %compiler_path%
echo   compiler_hardware_path        = %compiler_hardware_path%
echo   compiler_tools_builder_path   = %compiler_tools_builder_path%
echo   compiler_tools_processor_path = %compiler_tools_processor_path%
echo   compiler_libraries_path       = %compiler_libraries_path%
echo.
echo   user_hardware_path  = %user_hardware_path%
echo   user_libraries_path = %user_libraries_path%
echo   user_packages_path  = %user_packages_path%
echo.
echo.

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
rem -- Execute all tests
rem -----------------------------------------------------------------------------

call :ExecuteAllTests

set finalResult=0

if %build_test%==ON (
    echo.
    echo.
    echo Build results:

    for /F "tokens=2* delims=.=" %%A in ('set __errolevel_build.') do (
        @echo Build %%A %%B
        if "%%B"=="FAILED" (
            set finalResult=1
        )
    )
)

if %run_test%==ON (
    echo.
    echo.
    echo Execution results:

    for /F "tokens=2* delims=.=" %%A in ('set __errolevel_run.') do (
        @echo Run %%A %%B
        if "%%B"=="FAILED" (
            set finalResult=1
        )
    )
)

echo.
if !finalResult!==0 (
   echo TEST SUCCEED
) else (
   echo TEST FAILED
)

exit /b !finalResult!


rem -----------------------------------------------------------------------------
rem -- subroutines
rem -----------------------------------------------------------------------------

:usage
echo build.cmd [options]
echo options:
echo  -c, --clean             delete artifacts from previous build before building
echo  -b, --build-only        only build the project (default)
echo  -r, --run-only          only run the test
echo  -e2e, --run-e2e-tests   run end-to-end test
echo  --root <test_path>      determine the root of the test tree
goto :eof


rem -----------------------------------------------------------------------------
rem -- helper subroutines
rem -----------------------------------------------------------------------------

REM Parser tests.lst
:ExecuteAllTests
set testName=false
set projectName=

for /F "tokens=*" %%F in (%tests_list_file%) do (
    if /i "%%F"=="" (
        REM ignore empty line.
    ) else ( 
        set command=%%F
        if /i "!command:~0,1!"=="#" (
            echo Comment=!command:~1!
        ) else ( if /i "!command:~0,1!"=="[" (
            if %build_test%==ON (
                call :BuildTest
            )
            if %run_test%==ON (
                call :RunTest
            )
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


REM Build each test in the Tests.lst
:BuildTest
if not "!projectName!"=="" (
    echo.
    echo Build !projectName!
    echo   test_root=%test_root%
    echo   work_root=%work_root%
    echo   Target=!Target!
    echo   RelativePath=!RelativePath!
    echo   RelativeWorkingDir=!RelativeWorkingDir!
    echo   SerialPort=!SerialPort!
    echo   MaxAllowedDurationSeconds=!MaxAllowedDurationSeconds!
    echo   CloneURL=!CloneURL!
    echo   Categories=!Categories!
    echo   Hardware=!Hardware!
    echo   CPUParameters=!CPUParameters!
    echo   Build=!Build!

    if !CloneURL!=="" (
        set final_test_root=%work_root%
    ) else (
        set final_test_root=%test_root%
    )
    echo   final_test_root=!final_test_root!

    if "!Build!"=="Disable" (
        set __errolevel_build.!projectName!=DISABLED
        echo ** Build for !projectName! is disable. **
        goto :eof
    )

    if %clone_test%==ON (
        if not !CloneURL!=="" (
            pushd %work_root%
            git clone !CloneURL!
            popd
        )
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
    set parameters=-logger=machine !hardware_parameters! !tools_parameters! !libraries_parameters! !CPUParameters! -build-path "%build_root%!RelativeWorkingDir!" -warnings=none -prefs=build.warn_data_percentage=75 -verbose !final_test_root!!RelativePath!\!Target!

    echo Dump Arduino preferences:
    echo  !compiler_name! -dump-prefs !parameters!
    call !compiler_name! -dump-prefs !parameters!
    
    echo Build !RelativePath!:
    echo  !compiler_name! -compile !parameters!
    call !compiler_name! -compile !parameters!

    if "!errorlevel!"=="0" (
        set __errolevel_build.!projectName!=SUCCEED
    ) else (
        set __errolevel_build.!projectName!=FAILED
    )
)
goto :eof


REM Run each test in the Tests.lst
:RunTest
if not "!projectName!"=="" (
    echo.
    echo Run !projectName!
    echo   Target=!Target!
    echo   RelativeWorkingDir=!RelativeWorkingDir!
    echo   LogLines=!LogLines!
    echo   MinimumHeap=!MinimumHeap!
    echo   Execution=!Execution!

    if "!Execution!"=="Disable" (
        set __errolevel_run.!projectName!=DISABLED
        echo ** Execution for !projectName! is disable. **
        goto :eof
    )
    
    call powershell.exe -NoProfile -NonInteractive -ExecutionPolicy unrestricted -Command .\execute.ps1 -binaryPath:%build_root%!RelativeWorkingDir!\!Target!.bin -serialPort:!SerialPort! -esptool:%build_root%\arduino15\packages\esp8266\tools\esptool\0.4.8\esptool.exe -logLines:!LogLines! -minimumHeap:!MinimumHeap!

    if "!errorlevel!"=="0" (
        set __errolevel_run.!projectName!=SUCCEED
    ) else (
        set __errolevel_run.!projectName!=FAILED
    )

)
goto :eof
