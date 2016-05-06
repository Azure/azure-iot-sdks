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
rem -- Auto Sign csharp client with Strong Name & Authenticode
rem -----------------------------------------------------------------------------
call %build-root%\build\release\auto_sign_c_client.cmd
if %errorlevel% neq 0 exit /b %errorlevel%

rem -----------------------------------------------------------------------------
rem -- Build all native client configurations & platforms
rem -----------------------------------------------------------------------------
call %build-root%\c\build_all\windows\build_client.cmd --platform Win32
call %build-root%\c\build_all\windows\build_client.cmd --platform x64

rem -----------------------------------------------------------------------------
rem -- Create NuGet Packages
rem -----------------------------------------------------------------------------
echo Y | call %build-root%\c\build_all\packaging\windows\rebuild_nugets.cmd
if %errorlevel% neq 0 exit /b %errorlevel%

echo Y | call %build-root%\c\azure-c-shared-utility\build_all\packaging\windows\rebuild_nugets.cmd
if %errorlevel% neq 0 exit /b %errorlevel%

echo Y | call %build-root%\c\azure-uamqp-c\build_all\packaging\windows\rebuild_nugets.cmd
if %errorlevel% neq 0 exit /b %errorlevel%

echo Y | call %build-root%\c\azure-umqtt-c\build_all\packaging\windows\rebuild_nugets.cmd
if %errorlevel% neq 0 exit /b %errorlevel%


rem -----------------------------------------------------------------------------
rem -- Publish csharp NuGet Packages
rem -----------------------------------------------------------------------------
if not defined nuget_feed (
	echo Y | call %build-root%\build\release\push_nugets.cmd --path %build-root%\c\azure-c-shared-utility\build_all\packaging\windows
	if %errorlevel% neq 0 exit /b %errorlevel%
	echo Y | call %build-root%\build\release\push_nugets.cmd --path %build-root%\c\azure-uamqp-c\build_all\packaging\windows
	if %errorlevel% neq 0 exit /b %errorlevel%
	echo Y | call %build-root%\build\release\push_nugets.cmd --path %build-root%\c\azure-umqtt-c\build_all\packaging\windows
	if %errorlevel% neq 0 exit /b %errorlevel%
	echo Y | call %build-root%\build\release\push_nugets.cmd --path %build-root%\c\build_all\packaging\windows
	if %errorlevel% neq 0 exit /b %errorlevel%
) else (
	echo Y Y | call %build-root%\build\release\push_nugets.cmd --path %build-root%\c\azure-c-shared-utility\build_all\packaging\windows --feed %nuget_feed%
	if %errorlevel% neq 0 exit /b %errorlevel%
	echo Y Y | call %build-root%\build\release\push_nugets.cmd --path %build-root%\c\azure-uamqp-c\build_all\packaging\windows --feed %nuget_feed%
	if %errorlevel% neq 0 exit /b %errorlevel%
	echo Y Y | call %build-root%\build\release\push_nugets.cmd --path %build-root%\c\azure-umqtt-c\build_all\packaging\windows --feed %nuget_feed%
	if %errorlevel% neq 0 exit /b %errorlevel%
	echo Y Y | call %build-root%\build\release\push_nugets.cmd --path %build-root%\c\build_all\packaging\windows --feed %nuget_feed%
	if %errorlevel% neq 0 exit /b %errorlevel%
)