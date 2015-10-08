@REM Copyright (c) Microsoft. All rights reserved.
@REM Licensed under the MIT license. See LICENSE file in the project root for full license information.

setlocal

set build-root=%~dp0..
rem // resolve to fully qualified path
for %%i in ("%build-root%") do set build-root=%%~fi

REM -- Node.js --
cd %build-root%\node

cd build
@call :npm link
@call :build
if errorlevel 1 goto :unlink

cd ..\common
@call :npm link azure-iot-build
@call :npm link
@call :build lintAndAllTests
if errorlevel 1 goto :unlink

cd ..\device
@call :npm link azure-iot-build
@call :npm link azure-iot-common
@call :npm link
@call :build lintAndAllTests
if errorlevel 1 goto :unlink

cd ..\service
@call :npm link azure-iot-build
@call :npm link azure-iot-common
@call :npm link
@call :build lintAndAllTests
if errorlevel 1 goto :unlink

cd ..\..\tools\iothub-explorer
@call :npm link azure-iothub
@call :npm install
@call :npm test
if errorlevel 1 goto :unlink

:unlink
@REM The 'npm link' commands in this script create symlinks to tracked repo
@REM files from ignored locations (under ./node_modules). This means a call to
@REM 'git clean -xdf' will delete tracked files from the repo's working
@REM directory. To avoid any complications, we'll unlink everything before
@REM exiting.

cd %build-root%\tools\iothub-explorer
@call :npm rm azure-iothub

cd %build-root%\node\service
@call :npm rm
@call :npm rm azure-iot-build
@call :npm rm azure-iot-common

cd ..\device
@call :npm rm
@call :npm rm azure-iot-build
@call :npm rm azure-iot-common

cd ..\common
@call :npm rm
@call :npm rm azure-iot-build

cd ..\build
@call :npm rm

goto :eof

:npm
@REM npm.cmd turns echo off, but doesn't restore it. So we'll restore it here.
call npm %*
echo on
@goto :eof

:build
@REM Something in our Node.js build pipeline isn't restoring echo, so we'll do
@REM it here.
call build.cmd %*
echo on
@goto :eof
