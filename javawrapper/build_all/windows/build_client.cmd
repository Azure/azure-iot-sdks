@REM Copyright (c) Microsoft. All rights reserved.
@REM Licensed under the MIT license. See LICENSE file in the project root for full license information.

@echo off

setlocal

set build-root=%~dp0
rem // resolve to fully qualified path
for %%i in ("%build-root%") do set build-root=%%~fi

REM -- C --
cd ..\..\..\c\build_all\windows

if "%1" equ "" goto args-done
if "%1" equ "--use-websockets" goto arg-use-websockets
call :usage && exit /b 1

:arg-use-websockets
call build_client.cmd --build-javawrapper --use-websockets %1 %2 %3
goto args-continue

:args-done
call build_client.cmd --build-javawrapper %1 %2 %3

:args-continue
if errorlevel 1 goto :eof
cd %build-root%

REM -- compile java samples --
cd ..\..\device\samples\direct_call_of_wrapped_functions
call mvn install
cd %build-root%

cd ..\..\device\samples\send-receive-sample
call mvn install
cd %build-root%

cd ..\..\device\samples\send-sample
call mvn install
cd %build-root%

cd ..\..\device\samples\send-serialized-event
call mvn install
cd %build-root%

goto :eof

:usage
echo build_client.cmd [options]
echo options:
echo  --use-websockets   use amqp over web sockets
goto :eof