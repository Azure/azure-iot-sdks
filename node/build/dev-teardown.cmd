@REM Copyright (c) Microsoft. All rights reserved.
@REM Licensed under the MIT license. See LICENSE file in the project root for full license information.

@setlocal
@echo off

set node-root=%~dp0..
REM // resolve to fully qualified path
for %%i in ("%node-root%") do set node-root=%%~fi

cd %node-root%\build\tools
echo .
echo -- Removing links for build tools --
call npm rm azure-iothub

cd %node-root%\..\tools\iothub-explorer
echo -- Removing links for %cd% --
call npm rm azure-iothub
call npm rm azure-iot-device
call npm rm azure-iot-common

cd %node-root%\e2etests
echo.
echo -- Removing links for %cd% --
call npm rm azure-iothub
call npm rm azure-iot-device-mqtt
call npm rm azure-iot-device-http
call npm rm azure-iot-device-amqp
call npm rm azure-iot-device
call npm rm azure-iot-common

cd %node-root%\device\transport\mqtt
echo -- Removing links for %cd% --
call npm rm
call npm rm azure-iothub
call npm rm azure-iot-device
call npm rm azure-iot-common

cd %node-root%\device\transport\http
echo -- Removing links for %cd% --
call npm rm
call npm rm azure-iothub
call npm rm azure-iot-device
call npm rm azure-iot-common
call npm rm azure-iot-http-base

cd %node-root%\device\transport\amqp
echo -- Removing links for %cd% --
call npm rm
call npm rm azure-iothub
call npm rm azure-iot-device
call npm rm azure-iot-common
call npm rm azure-iot-amqp-base

cd %node-root%\service
echo -- Removing links for %cd% --
call npm rm
call npm rm azure-iot-http-base
call npm rm azure-iot-amqp-base
call npm rm azure-iot-common

cd %node-root%\device\core
echo -- Removing links for %cd% --
call npm rm
call npm rm azure-iot-common
call npm rm azure-iot-http-base

cd %node-root%\common\transport\http
echo -- Removing links for %cd% --
call npm rm
call npm rm azure-iot-common

cd %node-root%\common\transport\amqp
echo -- Removing links for %cd% --
call npm rm
call npm rm azure-iot-common

cd %node-root%\common\core
echo -- Removing links for %cd% --
call npm rm