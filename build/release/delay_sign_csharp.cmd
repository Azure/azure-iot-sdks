@REM Copyright (c) Microsoft. All rights reserved.
@REM Licensed under the MIT license. See LICENSE file in the project root for full license information.

@setlocal EnableExtensions EnableDelayedExpansion
@echo off

rem - Specify the Azure SDK client build root
set current-path=%~dp0
set current-path=%current-path:~0,-1%
set build-root=%current-path%\..\..
for %%i in ("%build-root%") do set build-root=%%~fi
echo Client Build root is %build-root%

rem -- Build csharp client for Delay Signing
set build_error=0
echo Y | call %build-root%\csharp\device\build\build.cmd --config Release_Delay_Sign
if !ERRORLEVEL! neq 0 set build_error=1

echo Y | call %build-root%\csharp\service\build\build.cmd --config Release_Delay_Sign
if !ERRORLEVEL! neq 0 set build_error=1

echo Charp Build Error: %build_error%

if %build_error% neq 0 exit /b 1
