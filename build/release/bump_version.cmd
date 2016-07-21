@REM Copyright (c) Microsoft. All rights reserved.
@REM Licensed under the MIT license. See LICENSE file in the project root for full license information.

@setlocal EnableExtensions EnableDelayedExpansion
@echo off

set build-root=%~dp0..\..

cd %build-root%\build\release\bump_version

call npm install
if not !ERRORLEVEL!==0 exit /b !ERRORLEVEL!

node app.js
