@REM Copyright (c) Microsoft. All rights reserved.
@REM Licensed under the MIT license. See LICENSE file in the project root for full license information.

@setlocal EnableExtensions EnableDelayedExpansion
@echo off

set build-root=%~dp0\..\..
for %%i in ("%build-root%") do set build-root=%%~fi


set nuget_feed=%1

if not defined nuget_feed (
	choice /C yn /M "No feed specified. Are you sure you want to publish to Nuget.org"
	if not !errorlevel!==1 goto :eof
)

rem -----------------------------------------------------------------------------
rem -- Build all native client configurations & platforms
rem -----------------------------------------------------------------------------
call %build-root%\c\build_all\windows\build_client.cmd --platform Win32
if not !ERRORLEVEL!==0 exit /b !ERRORLEVEL!
call %build-root%\c\build_all\windows\build_client.cmd --platform x64
if not !ERRORLEVEL!==0 exit /b !ERRORLEVEL!

rem -----------------------------------------------------------------------------
rem -- Create NuGet Packages
rem -----------------------------------------------------------------------------
echo Y | call %build-root%\c\build_all\packaging\windows\rebuild_nugets.cmd
if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!

echo Y | call %build-root%\c\azure-c-shared-utility\build_all\packaging\windows\rebuild_nugets.cmd
if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!

echo Y | call %build-root%\c\azure-uamqp-c\build_all\packaging\windows\rebuild_nugets.cmd
if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!

echo Y | call %build-root%\c\azure-umqtt-c\build_all\packaging\windows\rebuild_nugets.cmd
if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!


rem -----------------------------------------------------------------------------
rem -- Publish C NuGet Packages
rem -----------------------------------------------------------------------------
if not defined nuget_feed (
	echo Y | call %build-root%\build\release\push_nugets.cmd --path %build-root%\c\azure-c-shared-utility\build_all\packaging\windows
	if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!
	echo Y | call %build-root%\build\release\push_nugets.cmd --path %build-root%\c\azure-uamqp-c\build_all\packaging\windows
	if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!
	echo Y | call %build-root%\build\release\push_nugets.cmd --path %build-root%\c\azure-umqtt-c\build_all\packaging\windows
	if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!
	echo Y | call %build-root%\build\release\push_nugets.cmd --path %build-root%\c\build_all\packaging\windows
	if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!
) else (
	echo Y Y | call %build-root%\build\release\push_nugets.cmd --path %build-root%\c\azure-c-shared-utility\build_all\packaging\windows --feed %nuget_feed%
	if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!
	echo Y Y | call %build-root%\build\release\push_nugets.cmd --path %build-root%\c\azure-uamqp-c\build_all\packaging\windows --feed %nuget_feed%
	if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!
	echo Y Y | call %build-root%\build\release\push_nugets.cmd --path %build-root%\c\azure-umqtt-c\build_all\packaging\windows --feed %nuget_feed%
	if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!
	echo Y Y | call %build-root%\build\release\push_nugets.cmd --path %build-root%\c\build_all\packaging\windows --feed %nuget_feed%
	if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!
)