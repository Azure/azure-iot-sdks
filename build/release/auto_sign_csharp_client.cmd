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

set no-sign=%1

rem **********************************
rem * Specify the Build Root Folders *
rem **********************************

rem - Specify the Azure SDK client build root
set current-path=%~dp0
set current-path=%current-path:~0,-1%
set client-build-root=%current-path%\..\..
for %%i in ("%client-build-root%") do set client-build-root=%%~fi
echo Client Build root is %client-build-root%

rem -- Clear directories before starting process
del /F /Q %client-build-root%\build\tosign
del /F /Q %client-build-root%\build\signed

rem ******************************************************************
rem * Sign the Managed DLLs with both "Authenticode" & "Strong Name" *
rem ******************************************************************

rem -- Copy the managed dlls to the "tosign" Folder for signing
xcopy /q /y /R %client-build-root%\csharp\device\Microsoft.Azure.Devices.Client\bin\Release_Delay_Sign\Microsoft.Azure.Devices.Client.dll %client-build-root%\build\tosign\
if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!

xcopy /q /y /R %client-build-root%\csharp\device\Microsoft.Azure.Devices.Client.PCL\bin\Release_Delay_Sign\Microsoft.Azure.Devices.Client.PCL.dll %client-build-root%\build\tosign\
if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!

xcopy /q /y /R %client-build-root%\csharp\device\Microsoft.Azure.Devices.Client.WinRT\bin\Release_Delay_Sign\Microsoft.Azure.Devices.Client.winmd %client-build-root%\build\tosign\
if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!

xcopy /q /y /R %client-build-root%\csharp\service\Microsoft.Azure.Devices\bin\Release_Delay_Sign\Microsoft.Azure.Devices.dll %client-build-root%\build\tosign\
if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!

xcopy /q /y /R %client-build-root%\csharp\service\Microsoft.Azure.Devices.Uwp\bin\Release_Delay_Sign\Microsoft.Azure.Devices.Uwp.dll %client-build-root%\build\tosign\
if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!

if not defined no-sign (
    rem -- Auto-sign the managed dlls placed in the "tosign" Folder
    csu.exe /s=True /w=True /i=%client-build-root%\build\tosign /o=%client-build-root%\build\signed /c1=72 /c2=401 /d="Signing Azure IoT Managed Client binaries"
    if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!

    rem -- Copy the signed managed dlls back to their respective build output directories
    xcopy /q /y /R %client-build-root%\build\signed\Microsoft.Azure.Devices.Client.dll %client-build-root%\csharp\device\Microsoft.Azure.Devices.Client\bin\Release_Delay_Sign\
    if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!

    xcopy /q /y /R %client-build-root%\build\signed\Microsoft.Azure.Devices.Client.dll %client-build-root%\csharp\device\Microsoft.Azure.Devices.Client\obj\Release_Delay_Sign\
    if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!

    xcopy /q /y /R %client-build-root%\build\signed\Microsoft.Azure.Devices.Client.PCL.dll %client-build-root%\csharp\device\Microsoft.Azure.Devices.Client.PCL\bin\Release_Delay_Sign\
    if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!

    xcopy /q /y /R %client-build-root%\build\signed\Microsoft.Azure.Devices.Client.PCL.dll %client-build-root%\csharp\device\Microsoft.Azure.Devices.Client.PCL\obj\Release_Delay_Sign\
    if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!

    xcopy /q /y /R %client-build-root%\build\signed\Microsoft.Azure.Devices.Client.winmd %client-build-root%\csharp\device\Microsoft.Azure.Devices.Client.WinRT\bin\Release_Delay_Sign\
    if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!

    xcopy /q /y /R %client-build-root%\build\signed\Microsoft.Azure.Devices.Client.winmd %client-build-root%\csharp\device\Microsoft.Azure.Devices.Client.WinRT\obj\Release_Delay_Sign\
    if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!

    xcopy /q /y /R %client-build-root%\build\signed\Microsoft.Azure.Devices.dll %client-build-root%\csharp\service\Microsoft.Azure.Devices\bin\Release_Delay_Sign\
    if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!

    xcopy /q /y /R %client-build-root%\build\signed\Microsoft.Azure.Devices.dll %client-build-root%\csharp\service\Microsoft.Azure.Devices\obj\Release_Delay_Sign\
    if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!

    xcopy /q /y /R %client-build-root%\build\signed\Microsoft.Azure.Devices.Uwp.dll %client-build-root%\csharp\service\Microsoft.Azure.Devices.Uwp\bin\Release_Delay_Sign\
    if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!

    xcopy /q /y /R %client-build-root%\build\signed\Microsoft.Azure.Devices.Uwp.dll %client-build-root%\csharp\service\Microsoft.Azure.Devices.Uwp\obj\Release_Delay_Sign\
    if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!

    rem -- Clean directories
    del /F /Q %client-build-root%\build\tosign
    del /F /Q %client-build-root%\build\signed
)