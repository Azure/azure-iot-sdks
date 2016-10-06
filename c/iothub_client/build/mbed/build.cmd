@REM  Copyright (c) Microsoft. All rights reserved.
@REM  Licensed under the MIT license. See LICENSE file in the project root for full license information.

@setlocal EnableExtensions EnableDelayedExpansion

rem -----------------------------------------------------------------------------
rem -- parse script arguments
rem -----------------------------------------------------------------------------

set current-path=%~dp0
rem // remove trailing slash
set current-path=%current-path:~0,-1%

set build-root=%current-path%\..\..
rem // resolve to fully qualified path
for %%i in ("%build-root%") do set build-root=%%~fi

rem // default build options
set "longhaul_tests_download_bin_path="
set skip_samples=0

:args-loop
if "%1" equ "" goto args-done
if "%1" equ "--longhaul_tests_download_bin_path" goto longhaul_tests_download_bin_path
if "%1" equ "--skip-samples" goto skip_samples
goto args-done

:longhaul_tests_download_bin_path
shift
if NOT "%1" equ "" (
	set longhaul_tests_download_bin_path=%1
)
goto args-continue

:skip_samples
set skip_samples=1
goto args-continue

:args-continue
shift
goto args-loop

:args-done


rem -----------------------------------------------------------------------------
rem -- "Release" all the libraries that are used by the clients
rem -----------------------------------------------------------------------------

call %build-root%\..\c-utility\tools\mbed_build_scripts\release_mbed_project.cmd %build-root%\build\iothub_amqp_transport
if not !ERRORLEVEL!==0 exit /b !ERRORLEVEL!

call %build-root%\..\c-utility\tools\mbed_build_scripts\release_mbed_project.cmd %build-root%\build\iothub_http_transport
if not !ERRORLEVEL!==0 exit /b !ERRORLEVEL!

call %build-root%\..\c-utility\tools\mbed_build_scripts\release_mbed_project.cmd %build-root%\build\iothub_mqtt_transport
if not !ERRORLEVEL!==0 exit /b !ERRORLEVEL!

call %build-root%\..\c-utility\tools\mbed_build_scripts\release_mbed_project.cmd %build-root%\build\iothub_client
if not !ERRORLEVEL!==0 exit /b !ERRORLEVEL!

call %build-root%\..\c-utility\tools\mbed_build_scripts\release_mbed_project.cmd %build-root%\..\uamqp\build_all
if not !ERRORLEVEL!==0 exit /b !ERRORLEVEL!

call %build-root%\..\c-utility\tools\mbed_build_scripts\release_mbed_project.cmd %build-root%\..\umqtt\build_all
if not !ERRORLEVEL!==0 exit /b !ERRORLEVEL!

call %build-root%\..\c-utility\tools\mbed_build_scripts\release_mbed_project.cmd %build-root%\..\c-utility\build_all
if not !ERRORLEVEL!==0 exit /b !ERRORLEVEL!

call %build-root%\..\c-utility\tools\mbed_build_scripts\release_mbed_project.cmd %build-root%\..\serializer\build
if not !ERRORLEVEL!==0 exit /b !ERRORLEVEL!

if %skip_samples%==0 (
	rem -----------------------------------------------------------------------------
	rem -- build iothub client samples
	rem -----------------------------------------------------------------------------

	call :compile iothub_client_sample_amqp %build-root%\samples\iothub_client_sample_amqp\mbed
	if not !ERRORLEVEL!==0 exit /b !ERRORLEVEL!

	call :compile iothub_client_sample_http %build-root%\samples\iothub_client_sample_http\mbed
	if not !ERRORLEVEL!==0 exit /b !ERRORLEVEL!

	call :compile iothub_client_sample_mqtt %build-root%\samples\iothub_client_sample_mqtt\mbed
	if not !ERRORLEVEL!==0 exit /b !ERRORLEVEL!

	rem -----------------------------------------------------------------------------
	rem -- build a serializer client sample
	rem -----------------------------------------------------------------------------

	call :compile simplesample_amqp %build-root%\..\serializer\samples\simplesample_amqp\mbed
	if not !ERRORLEVEL!==0 exit /b !ERRORLEVEL!
)

rem -----------------------------------------------------------------------------
rem -- build iothub longhaul tests
rem -----------------------------------------------------------------------------

rem call :compile longhaul_tests %build-root%\tests\longhaul_tests\mbed %longhaul_tests_download_bin_path%
rem if not !ERRORLEVEL!==0 exit /b !ERRORLEVEL!

goto:eof

rem -----------------------------------------------------------------------------
rem -- helper subroutines
rem -----------------------------------------------------------------------------

:compile
setlocal EnableExtensions
set "project_name=%~1"
set "project_path=%~2"
set "download_bin_path=%~3"
set "cmake_project_bin_path=%project_name%_cmake_build"

mkdir %cmake_project_bin_path%
cd %cmake_project_bin_path%
cmake -Dmbed_repo_name:string=%project_name% -Dmbed_output_bin_path:string=%download_bin_path% %project_path%
set CMAKE_ERROR_CODE=!ERRORLEVEL!
cd ..
exit /b %CMAKE_ERROR_CODE%
goto:eof

