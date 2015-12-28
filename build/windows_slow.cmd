@REM Copyright (c) Microsoft. All rights reserved.
@REM Licensed under the MIT license. See LICENSE file in the project root for full license information.

setlocal

set build-root=%~dp0..
rem // resolve to fully qualified path
for %%i in ("%build-root%") do set build-root=%%~fi

REM -- C# --
cd %build-root%\csharp\device\build
echo Y | call build.cmd
if errorlevel 1 goto :eof
cd %build-root%

REM -- Java --
cd %build-root%\java\device
call mvn verify
if errorlevel 1 goto :eof
cd %build-root%

REM -- C --
echo Y | call %build-root%\c\build_all\windows\build.cmd --run-e2e-tests
if errorlevel 1 goto :eof
cd %build-root%

REM -- Node.js --
call %build-root%\build\windows_node.cmd
if errorlevel 1 goto :eof
