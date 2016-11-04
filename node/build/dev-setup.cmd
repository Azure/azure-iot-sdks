@REM Copyright (c) Microsoft. All rights reserved.
@REM Licensed under the MIT license. See LICENSE file in the project root for full license information.

@setlocal
@echo off

set node-root=%~dp0..
REM // resolve to fully qualified path
for %%i in ("%node-root%") do set node-root=%%~fi

cd %node-root%\common\core
echo.
echo -- Creating links for %cd% --
call npm link

cd %node-root%\common\transport\amqp
echo.
echo -- Creating links for %cd% --
call npm link azure-iot-common
call npm link

cd %node-root%\common\transport\http
echo.
echo -- Creating links for %cd% --
call npm link azure-iot-common
call npm link

cd %node-root%\device\core
echo.
echo -- Creating links for %cd% --
call npm link azure-iot-http-base
call npm link azure-iot-common
call npm link

cd %node-root%\service
echo.
echo -- Creating links for %cd% --
call npm link azure-iot-common
call npm link azure-iot-amqp-base
call npm link azure-iot-http-base
call npm link

cd %node-root%\device\transport\amqp
echo.
echo -- Creating links for %cd% --
call npm link azure-iot-amqp-base
call npm link azure-iot-common
call npm link azure-iot-device
call npm link azure-iothub
call npm link

cd %node-root%\device\transport\http
echo.
echo -- Creating links for %cd% --
call npm link azure-iot-http-base
call npm link azure-iot-common
call npm link azure-iot-device
call npm link azure-iothub
call npm link

cd %node-root%\device\transport\mqtt
echo.
echo -- Creating links for %cd% --
call npm link azure-iot-common
call npm link azure-iot-device
call npm link azure-iothub
call npm link

cd %node-root%\e2etests
echo.
echo -- Creating links for %cd% --
call npm link azure-iot-common
call npm link azure-iot-device
call npm link azure-iot-device-amqp
call npm link azure-iot-device-http
call npm link azure-iot-device-mqtt
call npm link azure-iothub
call npm install

cd %node-root%\..\tools\iothub-explorer
echo.
echo -- Creating links for %cd% --
call npm link azure-iot-common
call npm link azure-iot-device
call npm link azure-iothub
call npm install

cd %node-root%\build\tools
echo .
echo -- Setting up links for build tools --
call npm link azure-iothub
call npm install