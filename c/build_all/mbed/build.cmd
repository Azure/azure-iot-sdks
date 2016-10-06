@REM Copyright (c) Microsoft. All rights reserved.
@REM Licensed under the MIT license. See LICENSE file in the project root for full license information.

@setlocal EnableExtensions EnableDelayedExpansion
@echo off

set current-path=%~dp0
rem // remove trailing slash
set current-path=%current-path:~0,-1%

set repo-build-root=%current-path%\..\..
rem // resolve to fully qualified path
for %%i in ("%repo-build-root%") do set repo-build-root=%%~fi

rem -----------------------------------------------------------------------------
rem -- build (clean) compilembed tool
rem -----------------------------------------------------------------------------

call "%repo-build-root%\c-utility\tools\compilembed\build.cmd" --clean
if not !ERRORLEVEL!==0 exit /b !ERRORLEVEL!

rem -----------------------------------------------------------------------------
rem -- build iothub client samples
rem -----------------------------------------------------------------------------

call %repo-build-root%\iothub_client\build\mbed\build.cmd %*
if not !ERRORLEVEL!==0 exit /b !ERRORLEVEL!

goto :eof
