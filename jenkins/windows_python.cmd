@REM Copyright (c) Microsoft. All rights reserved.
@REM Licensed under the MIT license. See LICENSE file in the project root for full license information.

@echo off
setlocal

set build-root=%~dp0..
rem // resolve to fully qualified path
for %%i in ("%build-root%") do set build-root=%%~fi

REM -- Python --
cd %build-root%\python\build_all\windows
echo Building client using cmake
call build_client.cmd %2 %3

if errorlevel 1 exit /b 1
cd %build-root%
