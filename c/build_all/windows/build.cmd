@REM Copyright (c) Microsoft. All rights reserved.
@REM Licensed under the MIT license. See LICENSE file in the project root for full license information.

@setlocal EnableExtensions EnableDelayedExpansion
@echo off

set current-path=%~dp0
rem // remove trailing slash
set current-path=%current-path:~0,-1%

set build-root=%current-path%\..\..
rem // resolve to fully qualified path
for %%i in ("%build-root%") do set build-root=%%~fi

rem -----------------------------------------------------------------------------
rem -- check prerequisites
rem -----------------------------------------------------------------------------

rem // some of our projects expect PROTON_PATH to be defined
if not defined PROTON_PATH (
    set PROTON_PATH=%~d0\proton
)

if not exist %PROTON_PATH% (
    echo ERROR: PROTON_PATH must point to the root of your QPID Proton installation, but
    echo "%PROTON_PATH%" does not exist. Exiting...
    exit /b 1
)

rem ensure nuget.exe exists
where /q nuget.exe
if not !errorlevel! == 0 (
@Echo Azure IoT SDK needs to download nuget.exe from https://www.nuget.org/nuget.exe 
@Echo https://www.nuget.org 
choice /C yn /M "Do you want to download and run nuget.exe?" 
if not !errorlevel!==1 goto :eof
rem if nuget.exe is not found, then ask user
Powershell.exe wget -outf nuget.exe https://nuget.org/nuget.exe
	if not exist .\nuget.exe (
		echo nuget does not exist
		exit /b 1
	)
)
rem -----------------------------------------------------------------------------
rem -- parse script arguments
rem -----------------------------------------------------------------------------

rem // default build options
set build-clean=0
set build-config=Debug
set build-platform=Win32
set skip-tests=0
set skip-e2e-tests=0
set run-longhaul-tests=0

:args-loop
if "%1" equ "" goto args-done
if "%1" equ "-c" goto arg-build-clean
if "%1" equ "--clean" goto arg-build-clean
if "%1" equ "--config" goto arg-build-config
if "%1" equ "--platform" goto arg-build-platform
if "%1" equ "--skip-tests" goto arg-skip-tests
if "%1" equ "--skip-e2e-tests" goto arg-skip-e2e-tests
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

:arg-skip-tests
set skip-tests=1
goto args-continue

:arg-skip-e2e-tests
set skip-e2e-tests=1
goto args-continue

:arg-longhaul-tests
set run-longhaul-tests=1
goto args-continue

:args-continue
shift
goto args-loop

:args-done

rem -----------------------------------------------------------------------------
rem -- build device explorer
rem -----------------------------------------------------------------------------

call nuget restore "%build-root%\..\tools\deviceexplorer\deviceexplorer.sln"
if %build-clean%==1 (
    call :clean-a-solution "%build-root%\..\tools\deviceexplorer\deviceexplorer.sln" "Release" "Any CPU"
    if not !errorlevel!==0 exit /b !errorlevel!
)
call :build-a-solution "%build-root%\..\tools\deviceexplorer\deviceexplorer.sln" "Release" "Any CPU"
if not %errorlevel%==0 exit /b %errorlevel%

rem -----------------------------------------------------------------------------
rem -- build device explorer with installer
rem -----------------------------------------------------------------------------

call nuget restore "%build-root%\..\tools\deviceexplorer\deviceexplorerwithinstaller.sln"
if %build-clean%==1 (
    call :clean-a-solution "%build-root%\..\tools\deviceexplorer\deviceexplorerwithinstaller.sln" "Release" "Any CPU"
    if not !errorlevel!==0 exit /b !errorlevel!
)
call :build-a-solution "%build-root%\..\tools\deviceexplorer\deviceexplorerwithinstaller.sln" "Release" "Any CPU"
if not %errorlevel%==0 exit /b %errorlevel%

rem -----------------------------------------------------------------------------
rem -- clean solutions
rem -----------------------------------------------------------------------------

if %build-clean%==1 (
    call :clean-a-solution "%build-root%\common\build\windows\common.sln"
    if not %errorlevel%==0 exit /b %errorlevel%
    
    call :clean-a-solution "%build-root%\testtools\micromock\build\windows\micromock.sln"
    if not %errorlevel%==0 exit /b %errorlevel%
    
    call nuget restore "%build-root%\iothub_client\build\windows\iothub_client.sln"
    call :clean-a-solution "%build-root%\iothub_client\build\windows\iothub_client.sln"
    if not %errorlevel%==0 exit /b %errorlevel%

    call nuget restore "%build-root%\iothub_client\build\windows\iothub_client_dev.sln"
    call :clean-a-solution "%build-root%\iothub_client\build\windows\iothub_client_dev.sln"
    if not %errorlevel%==0 exit /b %errorlevel%

    call nuget restore "%build-root%\iothub_client\samples\iothub_client_sample_amqp\windows\iothub_client_sample_amqp.sln"
    call :clean-a-solution "%build-root%\iothub_client\samples\iothub_client_sample_amqp\windows\iothub_client_sample_amqp.sln"
    if not %errorlevel%==0 exit /b %errorlevel%

    call nuget restore "%build-root%\iothub_client\samples\iothub_client_sample_http\windows\iothub_client_sample_http.sln"
    call :clean-a-solution "%build-root%\iothub_client\samples\iothub_client_sample_http\windows\iothub_client_sample_http.sln"
    if not %errorlevel%==0 exit /b %errorlevel%

    call nuget restore "%build-root%\iothub_client\samples\iothub_client_sample_mqtt\windows\iothub_client_sample_mqtt.sln"
    call :clean-a-solution "%build-root%\iothub_client\samples\iothub_client_sample_mqtt\windows\iothub_client_sample_mqtt.sln"
    if not %errorlevel%==0 exit /b %errorlevel%
    
    call nuget restore "%build-root%\serializer\build\windows\serializer.sln"
    call :clean-a-solution "%build-root%\serializer\build\windows\serializer.sln"
    if not %errorlevel%==0 exit /b %errorlevel%

    call nuget restore "%build-root%\serializer\build\windows\serializer_dev.sln"
    call :clean-a-solution "%build-root%\serializer\build\windows\serializer_dev.sln"
    if not %errorlevel%==0 exit /b %errorlevel%
    
    call :clean-a-solution "%build-root%\serializer\samples\simplesample_http\windows\simplesample_http.sln"
    rem if not %errorlevel%==0 exit /b %errorlevel%
    
    call :clean-a-solution "%build-root%\serializer\samples\simplesample_amqp\windows\simplesample_amqp.sln"
    if not %errorlevel%==0 exit /b %errorlevel%

    call :clean-a-solution "%build-root%\serializer\samples\simplesample_mqtt\windows\simplesample_mqtt.sln"
    if not %errorlevel%==0 exit /b %errorlevel%
)
rem -----------------------------------------------------------------------------
rem -- restore packages for solutions
rem -----------------------------------------------------------------------------

call nuget restore -config "%current-path%\NuGet.Config" "%build-root%\common\build\windows\common.sln"
call nuget restore -config "%current-path%\NuGet.Config" "%build-root%\iothub_client\build\windows\iothub_client.sln"
call nuget restore -config "%current-path%\NuGet.Config" "%build-root%\iothub_client\build\windows\iothub_client_dev.sln"
call nuget restore -config "%current-path%\NuGet.Config" "%build-root%\iothub_client\samples\iothub_client_sample_amqp\windows\iothub_client_sample_amqp.sln"
call nuget restore -config "%current-path%\NuGet.Config" "%build-root%\iothub_client\samples\iothub_client_sample_http\windows\iothub_client_sample_http.sln"
call nuget restore -config "%current-path%\NuGet.Config" "%build-root%\iothub_client\samples\iothub_client_sample_mqtt\windows\iothub_client_sample_mqtt.sln"
call nuget restore -config "%current-path%\NuGet.Config" "%build-root%\serializer\build\windows\serializer.sln"
call nuget restore -config "%current-path%\NuGet.Config" "%build-root%\serializer\build\windows\serializer_dev.sln"
call nuget restore -config "%current-path%\NuGet.Config" "%build-root%\serializer\samples\simplesample_http\windows\simplesample_http.sln"
call nuget restore -config "%current-path%\NuGet.Config" "%build-root%\serializer\samples\simplesample_amqp\windows\simplesample_amqp.sln"
call nuget restore -config "%current-path%\NuGet.Config" "%build-root%\serializer\samples\simplesample_mqtt\windows\simplesample_mqtt.sln"


rem -----------------------------------------------------------------------------
rem -- build solutions
rem -----------------------------------------------------------------------------

call :build-a-solution "%build-root%\common\build\windows\common.sln"
if not %errorlevel%==0 exit /b %errorlevel%

call :build-a-solution "%build-root%\iothub_client\build\windows\iothub_client.sln"
if not %errorlevel%==0 exit /b %errorlevel%

call :build-a-solution "%build-root%\iothub_client\build\windows\iothub_client_dev.sln"
if not %errorlevel%==0 exit /b %errorlevel%

call :build-a-solution "%build-root%\iothub_client\samples\iothub_client_sample_amqp\windows\iothub_client_sample_amqp.sln"
rem if not %errorlevel%==0 exit /b %errorlevel%

call :build-a-solution "%build-root%\iothub_client\samples\iothub_client_sample_http\windows\iothub_client_sample_http.sln"
rem if not %errorlevel%==0 exit /b %errorlevel%

call :build-a-solution "%build-root%\iothub_client\samples\iothub_client_sample_mqtt\windows\iothub_client_sample_mqtt.sln"
rem if not %errorlevel%==0 exit /b %errorlevel%

call :build-a-solution "%build-root%\serializer\build\windows\serializer.sln"
if not %errorlevel%==0 exit /b %errorlevel%

call :build-a-solution "%build-root%\serializer\build\windows\serializer_dev.sln"
if not %errorlevel%==0 exit /b %errorlevel%

call :build-a-solution "%build-root%\serializer\samples\simplesample_amqp\windows\simplesample_amqp.sln"
if not %errorlevel%==0 exit /b %errorlevel%

call :build-a-solution "%build-root%\serializer\samples\simplesample_http\windows\simplesample_http.sln"
rem if not %errorlevel%==0 exit /b %errorlevel%

call :build-a-solution "%build-root%\serializer\samples\simplesample_mqtt\windows\simplesample_mqtt.sln"
if not %errorlevel%==0 exit /b %errorlevel%

rem -----------------------------------------------------------------------------
rem -- run unit tests
rem -----------------------------------------------------------------------------

if %skip-tests%==1 (
    echo Skipping unit tests...
) else (
    call :run-unit-tests "common"
    if not %errorlevel%==0 exit /b %errorlevel%

    call :run-unit-tests "iothub_client"
    if not %errorlevel%==0 exit /b %errorlevel%

    call :run-unit-tests "serializer"
    if not %errorlevel%==0 exit /b %errorlevel%
)

rem -----------------------------------------------------------------------------
rem -- run end-to-end tests
rem -----------------------------------------------------------------------------

set __skip=1
if %skip-tests%==0 if %skip-e2e-tests%==0 set __skip=0

if %__skip%==1 (
    echo Skipping end-to-end tests...
) else (
    call :run-e2e-tests "iothub_client"
    if not %errorlevel%==0 exit /b %errorlevel%

    call :run-e2e-tests "serializer"
    if not %errorlevel%==0 exit /b %errorlevel%
)

rem -----------------------------------------------------------------------------
rem -- run long-haul tests
rem -----------------------------------------------------------------------------

set __skip=1
if %skip-tests%==0 if %run-longhaul-tests%==1 set __skip=0

if %__skip%==1 (
    echo Skipping long-haul tests...
) else (
    call :run-longhaul-tests "iothub_client"
    if not %errorlevel%==0 exit /b %errorlevel%
)

rem -----------------------------------------------------------------------------
rem -- done
rem -----------------------------------------------------------------------------

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
echo  --skip-tests          build only, do not run any tests
echo  --skip-e2e-tests      do not run end-to-end tests
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

msbuild /m %build-target% "/p:Configuration=%build-config%;Platform=%build-platform%" %2
if not %errorlevel%==0 exit /b %errorlevel%
goto :eof


:_run-tests
rem // discover tests
set test-dlls-list=
set test-dlls-path=%build-root%\%~1\build\windows\%build-platform%\%build-config%
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
