
@setlocal EnableExtensions EnableDelayedExpansion
@echo off

set current-path=%~dp0
rem // remove trailing slash
set current-path=%current-path:~0,-1%

set build-root=%current-path%\..\..
rem // resolve to fully qualified path
for %%i in ("%build-root%") do set build-root=%%~fi

rem -----------------------------------------------------------------------------
rem -- "Release" all the libraries that are used by the clients
rem -----------------------------------------------------------------------------

call %build-root%\..\..\build\release\release_mbed_project.cmd %build-root%\build\iothub_amqp_transport
if not %errorlevel%==0 exit /b %errorlevel%

call %build-root%\..\..\build\release\release_mbed_project.cmd %build-root%\build\iothub_http_transport
if not %errorlevel%==0 exit /b %errorlevel%

call %build-root%\..\..\build\release\release_mbed_project.cmd %build-root%\build\iothub_mqtt_transport
if not %errorlevel%==0 exit /b %errorlevel%

call %build-root%\..\..\build\release\release_mbed_project.cmd %build-root%\build\iothub_client
if not %errorlevel%==0 exit /b %errorlevel%

call %build-root%\..\..\build\release\release_mbed_project.cmd %build-root%\..\azure-uamqp-c\build
if not %errorlevel%==0 exit /b %errorlevel%

call %build-root%\..\..\build\release\release_mbed_project.cmd %build-root%\..\azure-umqtt-c\build
if not %errorlevel%==0 exit /b %errorlevel%

call %build-root%\..\..\build\release\release_mbed_project.cmd %build-root%\..\azure-c-shared-utility\build
if not %errorlevel%==0 exit /b %errorlevel%

call %build-root%\..\..\build\release\release_mbed_project.cmd %build-root%\..\serializer\build
if not %errorlevel%==0 exit /b %errorlevel%

rem -----------------------------------------------------------------------------
rem -- build iothub client samples
rem -----------------------------------------------------------------------------

call :compile iothub_client_sample_amqp %build-root%\samples\iothub_client_sample_amqp\mbed
if not %errorlevel%==0 exit /b %errorlevel%

call :compile iothub_client_sample_http %build-root%\samples\iothub_client_sample_http\mbed
if not %errorlevel%==0 exit /b %errorlevel%

call :compile iothub_client_sample_mqtt %build-root%\samples\iothub_client_sample_mqtt\mbed
if not %errorlevel%==0 exit /b %errorlevel%

rem -----------------------------------------------------------------------------
rem -- build a serializer client sample
rem -----------------------------------------------------------------------------

call :compile simplesample_amqp %build-root%\..\serializer\samples\simplesample_amqp\mbed
if not %errorlevel%==0 exit /b %errorlevel%

rem -----------------------------------------------------------------------------
rem -- build iothub longhaul tests (FOR NOW DON'T DO THIS)
rem -----------------------------------------------------------------------------

rem call :compile longhaul_tests %build-root%\tests\longhaul_tests\mbed
rem if not %errorlevel%==0 exit /b %errorlevel%

goto:eof

rem -----------------------------------------------------------------------------
rem -- helper subroutines
rem -----------------------------------------------------------------------------

:compile
setlocal EnableExtensions
set "project_name=%~1"
set "project_path=%~2"
set "cmake_project_bin_path=%project_name%_cmake_build"

mkdir %cmake_project_bin_path%
cd %cmake_project_bin_path%
cmake -Dmbed_repo_name:string=%project_name% %project_path%
set CMAKE_ERROR_CODE=%ERRORLEVEL%
cd ..
exit /b %CMAKE_ERROR_CODE%
goto:eof

