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
rem -- Delay Sign csharp client
rem -----------------------------------------------------------------------------
call %build-root%\build\release\delay_sign_csharp.cmd
if %errorlevel% neq 0 exit /b %errorlevel%

rem -----------------------------------------------------------------------------
rem -- Auto Sign csharp client with Strong Name & Authenticode
rem -----------------------------------------------------------------------------
call %build-root%\build\release\auto_sign_csharp_client.cmd
if %errorlevel% neq 0 exit /b %errorlevel%

rem -----------------------------------------------------------------------------
rem -- Create NuGet Package
rem -----------------------------------------------------------------------------
pushd %build-root%\csharp\device\nuget\
powershell.exe %build-root%\csharp\device\nuget\make_nuget_package.ps1
if %errorlevel% neq 0 exit /b %errorlevel%
popd

rem -----------------------------------------------------------------------------
rem -- Publish csharp NuGet Package
rem -----------------------------------------------------------------------------
if not defined nuget_feed (
	echo Y | call %build-root%\build\release\push_nugets.cmd --path %build-root%\csharp\device\nuget\
	if %errorlevel% neq 0 exit /b %errorlevel%
) else (
	echo Y | call %build-root%\build\release\push_nugets.cmd --path %build-root%\csharp\device\nuget\ --feed %nuget_feed%
	if %errorlevel% neq 0 exit /b %errorlevel%
)
