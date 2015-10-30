@REM Copyright (c) Microsoft. All rights reserved.
@REM Licensed under the MIT license. See LICENSE file in the project root for full license information.

@setlocal
@echo off

set node-root=%~dp0..
REM // resolve to fully qualified path
for %%i in ("%node-root%") do set node-root=%%~fi

cd %node-root%\..\tools\iothub-explorer
echo.
echo -- Removing links for %cd% --
call npm rm azure-iothub

cd %node-root%\service
echo.
echo -- Removing links for %cd% --
call npm rm
call npm rm azure-iot-build
call npm rm azure-iot-common

cd ..\device
echo.
echo -- Removing links for %cd% --
call npm rm
call npm rm azure-iot-build
call npm rm azure-iot-common

cd ..\common
echo.
echo -- Removing links for %cd% --
call npm rm
call npm rm azure-iot-build

cd ..\build
echo.
echo -- Removing links for %cd% --
call npm rm
