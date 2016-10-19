@REM Copyright (c) Microsoft. All rights reserved.
@REM Licensed under the MIT license. See LICENSE file in the project root for full license information.

@setlocal EnableExtensions EnableDelayedExpansion
@echo off

set build-root=%~dp0..\..
cd %build-root%\build\release

set nuget_feed=%1

if not defined nuget_feed (
	choice /C yn /M "No feed specified. Are you sure you want to publish to Nuget.org and MBED.org?"
	if not !errorlevel!==1 goto :eof
)

rem -----------------------------------------------------------------------------
rem -- Bump version numbers across the repo
rem -----------------------------------------------------------------------------
call bump_version.cmd

rem -----------------------------------------------------------------------------
rem -- Build all windows binaries
rem -----------------------------------------------------------------------------
cd %build-root%\build
call windows_slow.cmd
if errorlevel 1 goto :eof
cd %build-root%\build\release


rem -----------------------------------------------------------------------------
rem -- Publish C nuget packages
rem -----------------------------------------------------------------------------
if not defined nuget_feed (
	call release_c_nuget.cmd
	if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!
) else (
	call release_c_nuget.cmd %nuget_feed%
	if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!
)


rem -----------------------------------------------------------------------------
rem -- Publish .NET nuget packages
rem -----------------------------------------------------------------------------
if not defined nuget_feed (
	call release_csharp_nuget.cmd
	if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!
) else (
	call release_csharp_nuget.cmd %nuget_feed%
	if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!
)

rem -----------------------------------------------------------------------------
rem -- Auto-Sign DeviceExplorer
rem -----------------------------------------------------------------------------
call auto_sign_deviceexplorer.cmd

rem -----------------------------------------------------------------------------
rem -- Publish node npm packages
rem -----------------------------------------------------------------------------

rem call release_npm.cmd
