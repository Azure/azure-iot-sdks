@REM Copyright (c) Microsoft. All rights reserved.
@REM Licensed under the MIT license. See LICENSE file in the project root for full license information.

@setlocal EnableExtensions EnableDelayedExpansion
@echo off

set build-root=%~dp0..\..
cd %build-root%\build\release

rem -----------------------------------------------------------------------------
rem -- Bump version numbers across the repo
rem -----------------------------------------------------------------------------
call bump_version.cmd

rem -----------------------------------------------------------------------------
rem -- Release mbed code
rem -----------------------------------------------------------------------------
call release_mbed.cmd

rem -----------------------------------------------------------------------------
rem -- Publish C nuget packages
rem -----------------------------------------------------------------------------
rem call release_c_nuget.cmd

rem -----------------------------------------------------------------------------
rem -- Publish .NET nuget packages
rem -----------------------------------------------------------------------------
rem call release_csharp_nuget.cmd

rem -----------------------------------------------------------------------------
rem -- Publish node npm packages
rem -----------------------------------------------------------------------------
rem call release_npm.cmd

rem -----------------------------------------------------------------------------
rem -- Generate API docs
rem -----------------------------------------------------------------------------
call ..\gen_docs.cmd
