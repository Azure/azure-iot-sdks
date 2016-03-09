@REM Copyright (c) Microsoft. All rights reserved.
@REM Licensed under the MIT license. See LICENSE file in the project root for full license information.

setlocal

set build-root=%~dp0
rem // resolve to fully qualified path
for %%i in ("%build-root%") do set build-root=%%~fi


REM -- C --
cd ..\..\..\c\build_all\windows
call build_client.cmd --build-javawrapper %1 %2 %3
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