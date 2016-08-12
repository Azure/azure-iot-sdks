@REM Copyright (c) Microsoft. All rights reserved.
@REM Licensed under the MIT license. See LICENSE file in the project root for full license information.

@setlocal EnableDelayedExpansion
@echo off

cd %~dp0\release_npm
set root=../../..

@REM encourage user to specify a fork of azure-iot-sdks
if "%1"=="" goto usage
for /f "tokens=1,2,3" %%i in ('git remote -v') do (
    if "%%k"=="(fetch)" if "%%i"=="%1" (
        set candidate_uri=%%j
        if "!candidate_uri:~-20!"=="Azure/azure-iot-sdks" goto not-a-fork
        if "!candidate_uri:~-21!"=="Azure/azure-iot-sdks/" goto not-a-fork
        if "!candidate_uri:~-24!"=="Azure/azure-iot-sdks.git" goto not-a-fork
    )
)

@REM install script dependencies
call npm rm -g prepare-aziot-release
if not !ERRORLEVEL!==0 exit /b !ERRORLEVEL!

call npm install -g
if not !ERRORLEVEL!==0 exit /b !ERRORLEVEL!

prepare-aziot-release %1 %root%/node/common/core %root%/node/common/transport/amqp %root%/node/common/transport/http %root%/node/common/transport/mqtt %root%/node/device/core %root%/node/device/transport/amqp %root%/node/device/transport/amqp-ws %root%/node/device/transport/http %root%/node/device/transport/mqtt %root%/node/device/node-red %root%/node/service %root%/tools/iothub-explorer %root%/tools/iothub-diagnostics

goto :eof

:not-a-fork
echo.
echo '%1' points to the azure-iot-sdks repo. Please specify a *fork* of azure-iot-sdks instead
goto usage

:usage
echo.
echo release_npm.cmd ^<fork-name^>
echo   ^<fork-name^> the name of a remote that preferably points to a fork of azure-iot-sdks
goto :eof