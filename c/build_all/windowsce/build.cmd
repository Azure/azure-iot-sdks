@REM Copyright (c) Microsoft. All rights reserved.
@REM Licensed under the MIT license. See LICENSE file in the project root for full license information.

@setlocal EnableExtensions EnableDelayedExpansion
echo off

set current-path=%~dp0
rem // remove trailing slash
set current-path=%current-path:~0,-1%

set build-root=%current-path%\..\..
rem // resolve to fully qualified path
for %%i in ("%build-root%") do set build-root=%%~fi

rem -----------------------------------------------------------------------------
rem -- check prerequisites
rem -----------------------------------------------------------------------------

rem // check that SDK is installed

rem -----------------------------------------------------------------------------
rem -- parse script arguments
rem -----------------------------------------------------------------------------

rem // default build options
set build-clean=0
set build-config=Debug
set build-platform=WindowsCE
set CMAKE_run_e2e_tests=OFF
set CMAKE_run_longhaul_tests=OFF

rem -----------------------------------------------------------------------------
rem -- build solution with CMake
rem -----------------------------------------------------------------------------

rmdir /s/q "%USERPROFILE%\cmake_ce8"
rem no error checking

mkdir "%USERPROFILE%\cmake_ce8"
rem no error checking

pushd "%USERPROFILE%\cmake_ce8"

rem if you plan to use a different SDK you need to change SDKNAME to the name of your SDK. Ensure that this is also changed in the sample solutions iothub_client_sample_http, simplesample_http and remote_monitoring
set SDKNAME=TORADEX_CE800
set PROCESSOR=arm

cmake -DWINCE=TRUE -DCMAKE_SYSTEM_NAME=WindowsCE -DCMAKE_SYSTEM_VERSION=8.0 -DCMAKE_SYSTEM_PROCESSOR=%PROCESSOR% -DCMAKE_GENERATOR_TOOLSET=CE800 -DCMAKE_GENERATOR_PLATFORM=%SDKNAME% -Drun_longhaul_tests:BOOL=%CMAKE_run_longhaul_tests% -Drun_e2e_tests:BOOL=%CMAKE_run_e2e_tests% -Duse_amqp=FALSE -Duse_mqtt=FALSE %build-root%

if not %errorlevel%==0 exit /b %errorlevel%

rem Currently, only building of HTTP sample is supported
msbuild "%USERPROFILE%\cmake_ce8\iothub_client\samples\iothub_client_sample_http\iothub_client_sample_http.vcxproj
if not %errorlevel%==0 exit /b %errorlevel%



