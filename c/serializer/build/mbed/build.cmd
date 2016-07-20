
@setlocal EnableExtensions EnableDelayedExpansion
@echo off

set current-path=%~dp0
rem // remove trailing slash
set current-path=%current-path:~0,-1%

set build-root=%current-path%\..\..
rem // resolve to fully qualified path
for %%i in ("%build-root%") do set build-root=%%~fi

rem -----------------------------------------------------------------------------
rem -- build iothub schema client samples
rem -----------------------------------------------------------------------------

call :compile temp_sensor_anomaly %build-root%\samples\temp_sensor_anomaly\mbed
if not !ERRORLEVEL!==0 exit /b !ERRORLEVEL!

call :compile simplesample_amqp %build-root%\samples\simplesample_amqp\mbed
if not !ERRORLEVEL!==0 exit /b !ERRORLEVEL!

call :compile simplesample_http %build-root%\samples\simplesample_http\mbed
if not !ERRORLEVEL!==0 exit /b !ERRORLEVEL!

call :compile remote_monitoring %build-root%\samples\remote_monitoring\mbed
if not !ERRORLEVEL!==0 exit /b !ERRORLEVEL!

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
cmake %project_path%
set CMAKE_ERROR_CODE=!ERRORLEVEL!
cd ..
exit /b %CMAKE_ERROR_CODE%
goto:eof
