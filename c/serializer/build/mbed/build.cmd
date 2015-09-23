
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

call %build-root%\samples\temp_sensor_anomaly\mbed\buildsample.bat
if not %errorlevel%==0 exit /b %errorlevel%

call %build-root%\samples\simplesample_amqp\mbed\buildsample.bat
if not %errorlevel%==0 exit /b %errorlevel%

call %build-root%\samples\simplesample_http\mbed\buildsample.bat
if not %errorlevel%==0 exit /b %errorlevel%

call %build-root%\samples\remote_monitoring\mbed\buildsample.bat
if not %errorlevel%==0 exit /b %errorlevel%



goto :eof
