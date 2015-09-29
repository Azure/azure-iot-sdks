@REM Copyright (c) Microsoft. All rights reserved.
@REM Licensed under the MIT license. See LICENSE file in the project root for full license information.

setlocal

set build-root=%~dp0..
rem // resolve to fully qualified path
for %%i in ("%build-root%") do set build-root=%%~fi

REM -- Java --
cd %build-root%\java\device
call mvn verify
if errorlevel 1 goto :eof
cd %build-root%

REM -- C --
call c\build_all\windows\build.cmd
if errorlevel 1 goto :eof

REM -- Node.js --
cd %build-root%\node

cd build
call npm install
call build.cmd
if errorlevel 1 goto :eof

cd ..\common
call npm install
call build.cmd lintAndAllTests
if errorlevel 1 goto :eof

cd ..\device
call npm install
call build.cmd lintAndAllTests
if errorlevel 1 goto :eof

cd ..\service
call npm install
call build.cmd lintAndAllTests
if errorlevel 1 goto :eof

cd ..\..\tools\iothub-explorer
call npm install
call npm test
