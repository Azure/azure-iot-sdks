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

set repo_root=%build-root%\..\..
rem // resolve to fully qualified path
for %%i in ("%repo_root%") do set repo_root=%%~fi

echo Build Root: %build-root%
echo Repo Root: %repo_root%

set cmake-root=%build-root%
rem -----------------------------------------------------------------------------
rem -- check prerequisites
rem -----------------------------------------------------------------------------

rem -----------------------------------------------------------------------------
rem -- parse script arguments
rem -----------------------------------------------------------------------------

rem // default build options
set build-clean=0
set build-config=Debug
set build-platform=Win32
set CMAKE_run_e2e_tests=OFF
set CMAKE_run_longhaul_tests=OFF
set CMAKE_skip_unittests=OFF
set CMAKE_use_wsio=OFF
set MAKE_NUGET_PKG=no
set CMAKE_DIR=iotsdk_win32
set build-samples=yes
set make=yes
set build_traceabilitytool=0

:args-loop
if "%1" equ "" goto args-done
if "%1" equ "-c" goto arg-build-clean
if "%1" equ "--clean" goto arg-build-clean
if "%1" equ "--config" goto arg-build-config
if "%1" equ "--platform" goto arg-build-platform
if "%1" equ "--run-e2e-tests" goto arg-run-e2e-tests
if "%1" equ "--run-longhaul-tests" goto arg-longhaul-tests
if "%1" equ "--skip-unittests" goto arg-skip-unittests
if "%1" equ "--use-websockets" goto arg-use-websockets
if "%1" equ "--make_nuget" goto arg-build-nuget
if "%1" equ "--cmake-root" goto arg-cmake-root
if "%1" equ "--no-make" goto arg-no-make
if "%1" equ "--build-traceabilitytool" goto arg-build-traceabilitytool
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
if %build-platform% == x64 (
    set CMAKE_DIR=iotsdk_x64
) else if %build-platform% == arm (
    set CMAKE_DIR=iotsdk_arm
)
goto args-continue

:arg-run-e2e-tests
set CMAKE_run_e2e_tests=ON
goto args-continue

:arg-longhaul-tests
set CMAKE_run_longhaul_tests=ON
goto args-continue

:arg-skip-unittests
set CMAKE_skip_unittests=ON
goto args-continue

:arg-use-websockets
set CMAKE_use_wsio=ON
goto args-continue

:arg-build-nuget
shift
if "%1" equ "" call :usage && exit /b 1
set MAKE_NUGET_PKG=%1
set CMAKE_skip_unittests=ON
set build-samples=no
goto args-continue

:arg-cmake-root
shift
if "%1" equ "" call :usage && exit /b 1
set cmake-root=%1
goto args-continue

:arg-no-make
set make=no
goto args-continue

:arg-build-traceabilitytool
set build_traceabilitytool=1
goto args-continue

:args-continue
shift
goto args-loop

:args-done

if %make% == no (
	rem No point running tests if we are not building the code
	set CMAKE_run_e2e_tests=OFF
	set CMAKE_run_longhaul_tests=OFF
	set CMAKE_skip_unittests=ON
	set build-samples=no
)

rem -----------------------------------------------------------------------------
rem -- restore packages for solutions
rem -----------------------------------------------------------------------------

if %build-samples%==yes (
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

	rem call nuget restore -config "%current-path%\NuGet.Config" "%build-root%\iothub_client\samples\iothub_client_sample_amqp\windows\iothub_client_sample_amqp.sln"
	if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!
	call nuget restore -config "%current-path%\NuGet.Config" "%build-root%\iothub_client\samples\iothub_client_sample_http\windows\iothub_client_sample_http.sln"
	if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!
	call nuget restore -config "%current-path%\NuGet.Config" "%build-root%\iothub_client\samples\iothub_client_sample_mqtt\windows\iothub_client_sample_mqtt.sln"
	if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!

	call nuget restore -config "%current-path%\NuGet.Config" "%build-root%\serializer\samples\simplesample_http\windows\simplesample_http.sln"
	if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!
	call nuget restore -config "%current-path%\NuGet.Config" "%build-root%\serializer\samples\simplesample_amqp\windows\simplesample_amqp.sln"
	if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!
	call nuget restore -config "%current-path%\NuGet.Config" "%build-root%\serializer\samples\simplesample_mqtt\windows\simplesample_mqtt.sln"
	if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!
	call nuget restore -config "%current-path%\NuGet.Config" "%build-root%\serializer\samples\remote_monitoring\windows\remote_monitoring.sln"
	if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!
	call nuget restore -config "%current-path%\NuGet.Config" "%build-root%\serializer\samples\temp_sensor_anomaly\windows\temp_sensor_anomaly.sln"
	if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!
)

rem -----------------------------------------------------------------------------
rem -- clean solutions
rem -----------------------------------------------------------------------------

if %build-clean%==1 (
	if %build-samples%==yes (
		rem call nuget restore "%build-root%\iothub_client\samples\iothub_client_sample_amqp\windows\iothub_client_sample_amqp.sln"
		rem call :clean-a-solution "%build-root%\iothub_client\samples\iothub_client_sample_amqp\windows\iothub_client_sample_amqp.sln"
		rem if not !ERRORLEVEL!==0 exit /b !ERRORLEVEL!

		call nuget restore "%build-root%\iothub_client\samples\iothub_client_sample_http\windows\iothub_client_sample_http.sln"
		call :clean-a-solution "%build-root%\iothub_client\samples\iothub_client_sample_http\windows\iothub_client_sample_http.sln"
		if not !ERRORLEVEL!==0 exit /b !ERRORLEVEL!

		rem call nuget restore "%build-root%\iothub_client\samples\iothub_client_sample_mqtt\windows\iothub_client_sample_mqtt.sln"
		rem call :clean-a-solution "%build-root%\iothub_client\samples\iothub_client_sample_mqtt\windows\iothub_client_sample_mqtt.sln"
		rem if not !ERRORLEVEL!==0 exit /b !ERRORLEVEL!
		
		call :clean-a-solution "%build-root%\serializer\samples\simplesample_http\windows\simplesample_http.sln"
		if not !ERRORLEVEL!==0 exit /b !ERRORLEVEL!
		
		call :clean-a-solution "%build-root%\serializer\samples\simplesample_amqp\windows\simplesample_amqp.sln"
		if not !ERRORLEVEL!==0 exit /b !ERRORLEVEL!

		call :clean-a-solution "%build-root%\serializer\samples\simplesample_mqtt\windows\simplesample_mqtt.sln"
		if not !ERRORLEVEL!==0 exit /b !ERRORLEVEL!
		
		call :clean-a-solution "%build-root%\serializer\samples\remote_monitoring\windows\remote_monitoring.sln"
		if not !ERRORLEVEL!==0 exit /b !ERRORLEVEL!
		
		call :clean-a-solution "%build-root%\serializer\samples\temp_sensor_anomaly\windows\temp_sensor_anomaly.sln"
		if not !ERRORLEVEL!==0 exit /b !ERRORLEVEL!
	)
)

rem -----------------------------------------------------------------------------
rem -- build solutions
rem -----------------------------------------------------------------------------

if %build-samples%==yes (
	rem call :build-a-solution "%build-root%\iothub_client\samples\iothub_client_sample_amqp\windows\iothub_client_sample_amqp.sln"
	rem if not !ERRORLEVEL!==0 exit /b !ERRORLEVEL!

	call :build-a-solution "%build-root%\iothub_client\samples\iothub_client_sample_http\windows\iothub_client_sample_http.sln"
	if not !ERRORLEVEL!==0 exit /b !ERRORLEVEL!

	rem call :build-a-solution "%build-root%\iothub_client\samples\iothub_client_sample_mqtt\windows\iothub_client_sample_mqtt.sln"
	rem if not !ERRORLEVEL!==0 exit /b !ERRORLEVEL!

	rem call :build-a-solution "%build-root%\serializer\samples\simplesample_amqp\windows\simplesample_amqp.sln"
	rem if not !ERRORLEVEL!==0 exit /b !ERRORLEVEL!

	call :build-a-solution "%build-root%\serializer\samples\simplesample_http\windows\simplesample_http.sln"
	if not !ERRORLEVEL!==0 exit /b !ERRORLEVEL!

	rem call :build-a-solution "%build-root%\serializer\samples\simplesample_mqtt\windows\simplesample_mqtt.sln"
	rem if not !ERRORLEVEL!==0 exit /b !ERRORLEVEL!

	rem call :build-a-solution "%build-root%\serializer\samples\remote_monitoring\windows\remote_monitoring.sln"
	rem if not !ERRORLEVEL!==0 exit /b !ERRORLEVEL!

	rem call :build-a-solution "%build-root%\serializer\samples\temp_sensor_anomaly\windows\temp_sensor_anomaly.sln"
	rem if not !ERRORLEVEL!==0 exit /b !ERRORLEVEL!
)

rem -----------------------------------------------------------------------------
rem -- build with CMAKE and run tests
rem -----------------------------------------------------------------------------

if %CMAKE_use_wsio% == ON (
	echo WebSockets support only available for x86 platform.
)

if EXIST %cmake-root%\cmake\%CMAKE_DIR% (
    rmdir /s/q %cmake-root%\cmake\%CMAKE_DIR%
    rem no error checking
)

echo CMAKE Output Path: %cmake-root%\cmake\%CMAKE_DIR%
mkdir %cmake-root%\cmake\%CMAKE_DIR%
rem no error checking
pushd %cmake-root%\cmake\%CMAKE_DIR%

if %MAKE_NUGET_PKG% == yes (
    echo ***Running CMAKE for Win32***
    cmake %build-root% -Drun_longhaul_tests:BOOL=%CMAKE_run_longhaul_tests% -Drun_e2e_tests:BOOL=%CMAKE_run_e2e_tests% -Dskip_unittests:BOOL=%CMAKE_skip_unittests% -Duse_wsio:BOOL=%CMAKE_use_wsio%
    if not !ERRORLEVEL!==0 exit /b !ERRORLEVEL!
    popd
	
    echo ***Running CMAKE for Win64***
    if EXIST %cmake-root%\cmake\iotsdk_x64 (
        rmdir /s/q %cmake-root%\cmake\iotsdk_x64
    )
	mkdir %cmake-root%\cmake\iotsdk_x64
	rem no error checking

	pushd %cmake-root%\cmake\iotsdk_x64
	cmake -Drun_longhaul_tests:BOOL=%CMAKE_run_longhaul_tests% -Drun_e2e_tests:BOOL=%CMAKE_run_e2e_tests% -Dskip_unittests:BOOL=%CMAKE_skip_unittests% -Duse_wsio:BOOL=%CMAKE_use_wsio% %build-root%  -G "Visual Studio 14 Win64"
	if not !ERRORLEVEL!==0 exit /b !ERRORLEVEL!
    popd

    echo ***Running CMAKE for ARM***
    if EXIST %cmake-root%\cmake\iotsdk_arm (
        rmdir /s/q %cmake-root%\cmake\iotsdk_arm
    )
	mkdir %cmake-root%\cmake\iotsdk_arm
	rem no error checking

	pushd %cmake-root%\cmake\iotsdk_arm
	cmake -Drun_longhaul_tests:BOOL=%CMAKE_run_longhaul_tests% -Drun_e2e_tests:BOOL=%CMAKE_run_e2e_tests% -Dskip_unittests:BOOL=%CMAKE_skip_unittests% -Duse_wsio:BOOL=%CMAKE_use_wsio% %build-root%  -G "Visual Studio 14 ARM"
	if not !ERRORLEVEL!==0 exit /b !ERRORLEVEL!

) else if %build-platform% == x64 (
	echo ***Running CMAKE for Win64***
	cmake -Drun_longhaul_tests:BOOL=%CMAKE_run_longhaul_tests% -Drun_e2e_tests:BOOL=%CMAKE_run_e2e_tests% -Dskip_unittests:BOOL=%CMAKE_skip_unittests% -Duse_wsio:BOOL=%CMAKE_use_wsio% %build-root%  -G "Visual Studio 14 Win64"
	if not !ERRORLEVEL!==0 exit /b !ERRORLEVEL!
) else if %build-platform% == arm (
	echo ***Running CMAKE for ARM***
	cmake -Drun_longhaul_tests:BOOL=%CMAKE_run_longhaul_tests% -Drun_e2e_tests:BOOL=%CMAKE_run_e2e_tests% -Dskip_unittests:BOOL=%CMAKE_skip_unittests% -Duse_wsio:BOOL=%CMAKE_use_wsio% %build-root%  -G "Visual Studio 14 ARM"
	if not !ERRORLEVEL!==0 exit /b !ERRORLEVEL!
) else (
	echo ***Running CMAKE for Win32***
	cmake -Drun_longhaul_tests:BOOL=%CMAKE_run_longhaul_tests% -Drun_e2e_tests:BOOL=%CMAKE_run_e2e_tests% -Dskip_unittests:BOOL=%CMAKE_skip_unittests% -Duse_wsio:BOOL=%CMAKE_use_wsio% %build-root% 
	if not !ERRORLEVEL!==0 exit /b !ERRORLEVEL!
)

if %MAKE_NUGET_PKG% == yes (
	if %make%==yes (
		echo ***Building all configurations***
		msbuild /m %cmake-root%\cmake\iotsdk_win32\azure_iot_sdks.sln /p:Configuration=Release
		if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!
		msbuild /m %cmake-root%\cmake\iotsdk_win32\azure_iot_sdks.sln /p:Configuration=Debug
		if not !ERRORLEVEL!==0 exit /b !ERRORLEVEL!

		msbuild /m %cmake-root%\cmake\iotsdk_x64\azure_iot_sdks.sln /p:Configuration=Release
		if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!
		msbuild /m %cmake-root%\cmake\iotsdk_x64\azure_iot_sdks.sln /p:Configuration=Debug
		if not !ERRORLEVEL!==0 exit /b !ERRORLEVEL!

		msbuild /m %cmake-root%\cmake\iotsdk_arm\azure_iot_sdks.sln /p:Configuration=Release
		if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!
		msbuild /m %cmake-root%\cmake\iotsdk_arm\azure_iot_sdks.sln /p:Configuration=Debug
		if not !ERRORLEVEL!==0 exit /b !ERRORLEVEL!
	)
) else (
	if %make%==yes (
		msbuild /m azure_iot_sdks.sln
		if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!

		if %build-platform% neq arm (
 			ctest -C "debug" -V
			if not !ERRORLEVEL!==0 exit /b !ERRORLEVEL!
		)
	)
)

popd

if %build_traceabilitytool%==1 (
	rem invoke the traceabilitytool here instead of the second build step in Jenkins windows_c job
	msbuild /m %build-root%\tools\traceabilitytool\traceabilitytool.sln
	if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!
)

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

:usage
echo build.cmd [options]
echo options:
echo  -c, --clean               delete artifacts from previous build before building
echo  --config ^<value^>        [Debug] build configuration (e.g. Debug, Release)
echo  --platform ^<value^>      [Win32] build platform (e.g. Win32, x64, arm, ...)
echo  --make_nuget ^<value^>    [no] generates the binaries to be used for nuget packaging (e.g. yes, no)
echo  --run-e2e-tests           run end-to-end tests
echo  --run-longhaul-tests      run long-haul tests
echo  --use-websockets          Enables the support for AMQP over WebSockets.
echo  --cmake-root			    Directory to place the cmake files used for building the project
echo  --no-make                 Surpress building the code
echo  --build-traceabilitytool  Builds an internal tool (traceabilitytool) to check for requirements/code/test consistency
echo  --skip-unittests          Skips building and executing unit tests (not advisable)

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
if not !ERRORLEVEL!==0 exit /b !ERRORLEVEL!
goto :eof