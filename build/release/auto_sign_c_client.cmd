@REM Copyright (c) Microsoft. All rights reserved.
@REM Licensed under the MIT license. See LICENSE file in the project root for full license information.

@REM -----------------------------------------------------------------------------------------------
@REM -- NOTE: THESE INSTRUCTIONS ARE FOR MICROSOFT INTERNAL USE ONLY FOR SIGNING DLLs
@REM -- Running this script requires the following:
@REM -- 	1. Must be logged into the team's Employee Approval Bypass Authorization account (EABA)
@REM --		2. Machine must be setup for CodeSign
@REM --		3. CodeSignUtility (csu.exe) installed and added to system path
@REM --	Refer to the Microsoft internal site http://codesigninfo for details
@REM -----------------------------------------------------------------------------------------------

@setlocal EnableExtensions EnableDelayedExpansion
@echo off

rem **********************************
rem * Specify the Build Root Folders *
rem **********************************

rem - Specify the Azure SDK client build root
set current-path=%~dp0
set current-path=%current-path:~0,-1%
set client-build-root=%current-path%\..\..
for %%i in ("%client-build-root%") do set client-build-root=%%~fi
echo Client Build root is %client-build-root%

rem -- Specify the OpenSSL build root
set openSSL-build-root=%OpenSSLDir%
echo OpenSSL Build root is %openSSL-build-root%

rem -- Clear directories before starting process
del /F /Q %client-build-root%\build\tosign
del /F /Q %client-build-root%\build\signed


rem ********************************
rem * Sign the native "win32" dlls *
rem ********************************

rem -- Copy the native "win32" dlls to the "tosign" Folder for signing

xcopy /q /y /R %openSSL-build-root%\bin\libeay32.dll %client-build-root%\build\tosign\
if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!

xcopy /q /y /R %openSSL-build-root%\bin\ssleay32.dll %client-build-root%\build\tosign\
if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!

rem -- Auto-sign the native "win32" dlls placed in the "tosign" Folder
csu.exe /s=True /w=True /i=%client-build-root%\build\tosign /o=%client-build-root%\build\signed /c1=401 /d="Signing Azure IoT Native Client binaries"
if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!

rem -- Copy the signed native "win32" dlls back to their respective build output directories

xcopy /q /y /R %client-build-root%\build\signed\libeay32.dll %openSSL-build-root%\bin\
if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!

xcopy /q /y /R  %client-build-root%\build\signed\ssleay32.dll %openSSL-build-root%\bin\
if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!

rem -- Clean directories
del /F /Q %client-build-root%\build\tosign
del /F /Q %client-build-root%\build\signed
