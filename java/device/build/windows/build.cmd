@REM Copyright (c) Microsoft. All rights reserved.
@REM Licensed under the MIT license. See LICENSE file in the project root for full license information.

setlocal

set build-root=%~dp0..\..\..\..
rem // resolve to fully qualified path
for %%i in ("%build-root%") do set build-root=%%~fi

REM -- Java --
cd %build-root%\java\device
call mvn verify -DskipITs=false
if errorlevel 1 goto :eof
cd %build-root%
