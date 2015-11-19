@REM Copyright (c) Microsoft. All rights reserved.
@REM Licensed under the MIT license. See LICENSE file in the project root for full license information.

@setlocal EnableExtensions EnableDelayedExpansion
echo off

set current-path=%~dp0
rem // remove trailing slash
set current-path=%current-path:~0,-1%

set build-root=%current-path%\..\..
rem // resolve to fully qualified path
for %%i in ("%build-root%") do set build-root=%%~fi

rem -----------------------------------------------------------------------------
rem -- check prerequisites
rem -----------------------------------------------------------------------------

rem // check that SDK is installed

rem -----------------------------------------------------------------------------
rem -- parse script arguments
rem -----------------------------------------------------------------------------

rem // default build options
set build-clean=0
set build-config=Debug
set build-platform=WindowsCE
set CMAKE_run_e2e_tests=OFF
set CMAKE_run_longhaul_tests=OFF

:args-loop
if "%1" equ "" goto args-done
if "%1" equ "-c" goto arg-build-clean
if "%1" equ "--clean" goto arg-build-clean
if "%1" equ "--config" goto arg-build-config
if "%1" equ "--platform" goto arg-build-platform
if "%1" equ "--run-e2e-tests" goto arg-run-e2e-tests
if "%1" equ "--run-longhaul-tests" goto arg-longhaul-tests
call :usage && exit /b 1

:arg-build-clean
set build-clean=1
goto args-continue

:arg-build-config
shift
if "%1" equ "" call :usage && exit /b 1
set build-config=%1
goto args-continue

:arg-build-platform
shift
if "%1" equ "" call :usage && exit /b 1
set build-platform=%1
goto args-continue

:arg-run-e2e-tests
set CMAKE_run_e2e_tests=ON
goto args-continue

:arg-longhaul-tests
set CMAKE_run_longhaul_tests=ON
goto args-continue

:args-continue
shift
goto args-loop

:args-done

rem -----------------------------------------------------------------------------
rem -- build with CMAKE and run tests
rem -----------------------------------------------------------------------------

rmdir /s/q "%USERPROFILE%\cmake_ce8"
rem no error checking

mkdir "%USERPROFILE%\cmake_ce8"
rem no error checking

pushd "%USERPROFILE%\cmake_ce8"

rem if you plan to use a different SDK you need to change SDKNAME to the name of your SDK. Ensure that this is also changed in the sample solutions iothub_client_sample_http, simplesample_http and remote_monitoring
set SDKNAME=TORADEX_CE800
set PROCESSOR=arm

cmake -DWINCE=TRUE -DCMAKE_SYSTEM_NAME=WindowsCE -DCMAKE_SYSTEM_VERSION=8.0 -DCMAKE_SYSTEM_PROCESSOR=%PROCESSOR% -DCMAKE_GENERATOR_TOOLSET=CE800 -DCMAKE_GENERATOR_PLATFORM=%SDKNAME% -Drun_longhaul_tests:BOOL=%CMAKE_run_longhaul_tests% -Drun_e2e_tests:BOOL=%CMAKE_run_e2e_tests% -Duse_amqp=FALSE -Duse_mqtt=FALSE %build-root%

if not %errorlevel%==0 exit /b %errorlevel%

msbuild /m azure_iot_sdks.sln
if not %errorlevel%==0 exit /b %errorlevel%

rem -- Copy all libraries to the repo directory to be able to build solutions
xcopy /q /y /R "%USERPROFILE%\cmake_ce8\iothub_client\Debug\*.*" "%build-root%\build_output\c\wec2013\debug\*.*"
if %errorlevel% neq 0 exit /b %errorlevel%

xcopy /q /y /R "%USERPROFILE%\cmake_ce8\common\Debug\*.*" "%build-root%\build_output\c\wec2013\debug\*.*"
if %errorlevel% neq 0 exit /b %errorlevel%

xcopy /q /y /R "%USERPROFILE%\cmake_ce8\serializer\Debug\*.*" "%build-root%\build_output\c\wec2013\debug\*.*"
if %errorlevel% neq 0 exit /b %errorlevel%

rem currently only debug binaries are built
xcopy /q /y /R "%USERPROFILE%\cmake_ce8\iothub_client\Release\*.*" "%build-root%\build_output\c\wec2013\release\*.*"
xcopy /q /y /R "%USERPROFILE%\cmake_ce8\common\Release\*.*" "%build-root%\build_output\c\wec2013\release\*.*"
xcopy /q /y /R "%USERPROFILE%\cmake_ce8\serializer\Release\*.*" "%build-root%\build_output\c\wec2013\release\*.*"

rem Tests can't be executed on the local PC, must run on remote target
rem ctest -C "debug" -V
rem if not %errorlevel%==0 exit /b %errorlevel%

if %build-clean%==1 (
	
    call :clean-a-solution "%build-root%\iothub_client\samples\iothub_client_sample_http\windowsce\iothub_client_sample_http.sln"
    if not %errorlevel%==0 exit /b %errorlevel%
  
    call :clean-a-solution "%build-root%\serializer\samples\simplesample_http\windowsce\simplesample_http.sln"
    rem if not %errorlevel%==0 exit /b %errorlevel%
    	
	  call :clean-a-solution "%build-root%\serializer\samples\remote_monitoring\windowsce\remote_monitoring.sln"
    if not %errorlevel%==0 exit /b %errorlevel%
	
	  call :clean-a-solution "%build-root%\serializer\samples\temp_sensor_anomaly\windowsce\temp_sensor_anomaly.sln"
    if not %errorlevel%==0 exit /b %errorlevel%

)



rem -----------------------------------------------------------------------------
rem -- build solutions
rem -----------------------------------------------------------------------------


call :build-a-solution "%build-root%\iothub_client\samples\iothub_client_sample_http\windowsce\iothub_client_sample_http.sln"
 if not %errorlevel%==0 exit /b %errorlevel%

call :build-a-solution "%build-root%\serializer\samples\simplesample_http\windowsce\simplesample_http.sln"
if not %errorlevel%==0 exit /b %errorlevel%

call :build-a-solution "%build-root%\serializer\samples\remote_monitoring\windowsce\remote_monitoring.sln"
if not %errorlevel%==0 exit /b %errorlevel%

popd
goto :eof


rem -----------------------------------------------------------------------------
rem -- subroutines
rem -----------------------------------------------------------------------------

:clean-a-solution
call :_run-msbuild "Clean" %1 %2 %3
goto :eof


:build-a-solution
call :_run-msbuild "Build" %1 %2 %3
goto :eof


:run-unit-tests
call :_run-tests %1 "UnitTests"
goto :eof


:run-e2e-tests
call :_run-tests %1 "e2eTests"
goto :eof

:run-longhaul-tests
call :_run-tests %1 "longhaul"
goto :eof

:usage
echo build.cmd [options]
echo options:
echo  -c, --clean           delete artifacts from previous build before building
echo  --config ^<value^>      [Debug] build configuration (e.g. Debug, Release)
echo  --platform ^<value^>    [Win32] build platform (e.g. Win32, x64, ...)
echo  --run-e2e-tests       run end-to-end tests
echo  --run-longhaul-tests  run long-haul tests
goto :eof


rem -----------------------------------------------------------------------------
rem -- helper subroutines
rem -----------------------------------------------------------------------------

:_run-msbuild
rem // optionally override configuration|platform
setlocal EnableExtensions
set build-target=
if "%~1" neq "Build" set "build-target=/t:%~1"
if "%~3" neq "" set build-config=%~3
if "%~4" neq "" set build-platform=%~4

msbuild /m %build-target% "/p:Configuration=%build-config%;Platform=%SDKNAME%" %2
if not %errorlevel%==0 exit /b %errorlevel%
goto :eof


:_run-tests
rem // discover tests
set test-dlls-list=
set test-dlls-path=%build-root%\%~1\build\windows\%SDKNAME%\%build-config%
for /f %%i in ('dir /b %test-dlls-path%\*%~2*.dll') do set test-dlls-list="%test-dlls-path%\%%i" !test-dlls-list!

if "%test-dlls-list%" equ "" (
    echo No unit tests found in %test-dlls-path%
    exit /b 1
)

rem // run tests
echo Test DLLs: %test-dlls-list%
echo.
vstest.console.exe %test-dlls-list%

if not %errorlevel%==0 exit /b %errorlevel%
goto :eof
