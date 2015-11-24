@REM Copyright (c) Microsoft. All rights reserved.
@REM Licensed under the MIT license. See LICENSE file in the project root for full license information.

@setlocal EnableExtensions EnableDelayedExpansion
@echo off

cd %~dp0..\..\node
if exist out rd /s /q out

rem -----------------------------------------------------------------------------
rem -- Generate Node.js Device SDK docs
rem -----------------------------------------------------------------------------
call :jsdoc "Microsoft Azure IoT Device SDK for Node.js" "--package device/package.json -c ../build/docs/jsdoc-device.conf.json"

rem -----------------------------------------------------------------------------
rem -- Generate Node.js Service SDK docs
rem -----------------------------------------------------------------------------
call :jsdoc "Microsoft Azure IoT Service SDK for Node.js" "--readme service/README.md --package service/package.json -c ../build/docs/jsdoc-service.conf.json"

goto :eof

:jsdoc
cmd /c "set "JSDOC_TITLE=%~1" && jsdoc %~2"
goto :eof
