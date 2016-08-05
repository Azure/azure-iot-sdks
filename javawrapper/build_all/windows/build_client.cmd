@REM Copyright (c) Microsoft. All rights reserved.
@REM Licensed under the MIT license. See LICENSE file in the project root for full license information.

@setlocal EnableExtensions EnableDelayedExpansion
@echo off

set build-platform=Win32
set use-websockets=OFF

set build-root=%~dp0
rem // resolve to fully qualified path
for %%i in ("%build-root%") do set build-root=%%~fi

call :checkExists mvn
if not !ERRORLEVEL!==0 exit /b !ERRORLEVEL!

REM -- C --
cd ..\..\..\c\build_all\windows

:args-loop
if "%1" equ "" goto args-done
if "%1" equ "--platform" goto arg-build-platform
if "%1" equ "--use-websockets" goto arg-use-websockets
call :usage && exit /b 1

:arg-build-platform
shift
if "%1" equ "" call :usage && exit /b 1
set build-platform=%1
shift
goto args-loop

:arg-use-websockets
set use-websockets=ON
shift
goto args-loop

:args-done
if %use-websockets% == ON (
	call build_client.cmd --build-javawrapper --platform %build-platform% --use-websockets %1 %2 %3
	goto delete
) else (
	call build_client.cmd --platform %build-platform% --build-javawrapper %1 %2 %3
	goto args-continue
)

:delete
del ..\..\..\javawrapper\device\iothub_client_javawrapper\windows\iothub_client_javawrapper_temp.def

:args-continue
if not !ERRORLEVEL!==0 goto :eof
cd %build-root%

set PATH=%PATH%;%userprofile%\cmake_%build-platform%\javawrapper\Debug

REM -- compile javawrapper --
cd ..\..\device
call mvn install

copy %userprofile%\cmake_%build-platform%\javawrapper\Debug\iothub_client_java.dll %cd%\samples\direct_call_of_wrapped_functions\target
copy %userprofile%\cmake_%build-platform%\javawrapper\Debug\iothub_client_java.dll %cd%\samples\send-receive-sample\target
copy %userprofile%\cmake_%build-platform%\javawrapper\Debug\iothub_client_java.dll %cd%\samples\send-receive-sample-x509\target
copy %userprofile%\cmake_%build-platform%\javawrapper\Debug\iothub_client_java.dll %cd%\samples\send-event-sample\target
copy %userprofile%\cmake_%build-platform%\javawrapper\Debug\iothub_client_java.dll %cd%\samples\send-serialized-event\target
copy %userprofile%\cmake_%build-platform%\javawrapper\Debug\iothub_client_mock.dll %cd%\test
cd %build-root%

goto :eof

:usage
echo build_client.cmd [options]
echo options:
echo  --use-websockets   use amqp over web sockets
echo  --platform ^<value^>    [Win32] build platform (e.g. Win32, x64, ...)
goto :eof

rem -----------------------------------------------------------------------------
rem -- helper subroutines
rem -----------------------------------------------------------------------------
:checkExists
where %~1 >nul 2>nul
if not !ERRORLEVEL!==0 (
    echo "%~1" not found. Please make sure that "%~1" is installed and available in the path.
    exit /b !ERRORLEVEL!
)
goto :eof
