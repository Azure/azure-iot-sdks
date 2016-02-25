@REM Copyright (c) Microsoft. All rights reserved.
@REM Licensed under the MIT license. See LICENSE file in the project root for full license information.

@setlocal EnableExtensions EnableDelayedExpansion
@echo off

set current-path=%~dp0
rem // remove trailing slash
set current-path=%current-path:~0,-1%

set build-root=%current-path%\..\..
rem // resolve to fully qualified path
for %%i in ("%build-root%") do set build-root=%%~fi

rem -----------------------------------------------------------------------------
rem -- check prerequisites
rem -----------------------------------------------------------------------------

rem -----------------------------------------------------------------------------
rem -- parse script arguments
rem -----------------------------------------------------------------------------

rem // default build options
set build-clean=0
set build-config=
set build-platform=Win32
set CMAKE_use_wsio=OFF

:args-loop
if "%1" equ "" goto args-done
if "%1" equ "--config" goto arg-build-config
if "%1" equ "--platform" goto arg-build-platform
if "%1" equ "--use-websockets" goto arg-use-websockets
call :usage && exit /b 1

:arg-build-config
shift
if "%1" equ "" call :usage && exit /b 1
set build-config=%1
goto args-continue

:arg-build-platform
shift
if "%1" equ "" call :usage && exit /b 1
set build-platform=%1
goto args-continue

:arg-use-websockets
shift
set CMAKE_use_wsio=ON
goto args-continue

:args-continue
shift
goto args-loop

:args-done
set cmake-output=cmake_%build-platform%

rem -----------------------------------------------------------------------------
rem -- build with CMAKE
rem -----------------------------------------------------------------------------

if %CMAKE_use_wsio% == ON (
	echo WebSockets support only available for x86 platform.
)

echo CMAKE Output Path: %USERPROFILE%\%cmake-output%

rmdir /s/q %USERPROFILE%\%cmake-output%
rem no error checking

mkdir %USERPROFILE%\%cmake-output%
rem no error checking

pushd %USERPROFILE%\%cmake-output%

if %build-platform% == Win32 (
	echo ***Running CMAKE for Win32***
	cmake %build-root% -Duse_wsio:BOOL=%CMAKE_use_wsio%
	if not %errorlevel%==0 exit /b %errorlevel%	
) else (
	echo ***Running CMAKE for Win64***
	cmake %build-root% -G "Visual Studio 14 Win64"
	if not %errorlevel%==0 exit /b %errorlevel%	
)

if not defined build-config (
	echo ***Building both configurations***
	msbuild /m azure_iot_sdks.sln /p:Configuration=Release
	msbuild /m azure_iot_sdks.sln /p:Configuration=Debug
) else (
	echo ***Building %build-config% only***
	msbuild /m azure_iot_sdks.sln /p:Configuration=%build-config%
)

popd
goto :eof


:usage
echo build.cmd [options]
echo options:
echo  --config ^<value^>      [Debug] build configuration (e.g. Debug, Release)
echo  --platform ^<value^>    [Win32] build platform (e.g. Win32, x64, ...)
goto :eof
