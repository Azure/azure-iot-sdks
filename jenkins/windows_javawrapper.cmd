@REM Copyright (c) Microsoft. All rights reserved.
@REM Licensed under the MIT license. See LICENSE file in the project root for full license information.

@setlocal EnableExtensions EnableDelayedExpansion

set build-root=%~dp0..
rem // resolve to fully qualified path
for %%i in ("%build-root%") do set build-root=%%~fi

REM -- JavaWrapper --
cd %build-root%\javawrapper\build_all\windows
call build_client.cmd %*
if not !ERRORLEVEL!==0 exit /b !ERRORLEVEL!
cd %build-root%
