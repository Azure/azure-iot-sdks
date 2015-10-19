@REM Copyright (c) Microsoft. All rights reserved.
@REM Licensed under the MIT license. See LICENSE file in the project root for full license information.

@setlocal
@echo off

set build-root=%~dp0..
REM Resolve to fully qualified path
for %%i in ("%build-root%") do set build-root=%%~fi

cd %build-root%\node

REM Set up links in the npm cache to ensure we're exercising all the code in
REM the repo, rather than downloading released versions of our packages from
REM npm.

call build\dev-setup.cmd

echo.
echo -- Linting and running tests --
echo.

cd build
call build
if errorlevel 1 goto :unlink

cd ..\common
call build lintAndAllTests
if errorlevel 1 goto :unlink

cd ..\device
call build lintAndAllTests
if errorlevel 1 goto :unlink

cd ..\service
call build lintAndAllTests
if errorlevel 1 goto :unlink

cd ..\..\tools\iothub-explorer
call npm test
if errorlevel 1 goto :unlink

:unlink
REM The 'npm link' commands in this script create symlinks to tracked repo
REM files from ignored locations (under ./node_modules). This means a call to
REM 'git clean -xdf' will delete tracked files from the repo's working
REM directory. To avoid any complications, we'll unlink everything before
REM exiting.

call %build-root%\node\build\dev-teardown.cmd
goto :eof
