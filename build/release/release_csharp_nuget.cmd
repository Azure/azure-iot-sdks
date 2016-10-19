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
rem -- Delay Sign csharp device & service clients
rem -----------------------------------------------------------------------------
call %build-root%\build\release\delay_sign_csharp.cmd
if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!

rem -----------------------------------------------------------------------------
rem -- Auto Sign csharp client with Strong Name & Authenticode
rem -----------------------------------------------------------------------------
call %build-root%\build\release\auto_sign_csharp_client.cmd
if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!

rem -----------------------------------------------------------------------------
rem -- Create Device Client NuGet Package
rem -----------------------------------------------------------------------------
pushd %build-root%\csharp\device\nuget\
powershell.exe %build-root%\csharp\device\nuget\make_nuget_package.ps1
if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!
popd

rem -----------------------------------------------------------------------------
rem -- Create Service Client NuGet Package
rem -----------------------------------------------------------------------------
pushd %build-root%\csharp\service\nuget\
powershell.exe %build-root%\csharp\service\nuget\make_nuget_package.ps1
if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!
popd

rem -----------------------------------------------------------------------------
rem -- Publish csharp NuGet Package
rem -----------------------------------------------------------------------------
if not defined nuget_feed (
	echo Y | call %build-root%\build\release\push_nugets.cmd --path %build-root%\csharp\device\nuget\
	if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!

	echo Y | call %build-root%\build\release\push_nugets.cmd --path %build-root%\csharp\service\nuget\
	if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!
) else (
	echo Y | call %build-root%\build\release\push_nugets.cmd --path %build-root%\csharp\device\nuget\ --feed %nuget_feed%
	if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!

	echo Y | call %build-root%\build\release\push_nugets.cmd --path %build-root%\csharp\service\nuget\ --feed %nuget_feed%
	if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!
)
