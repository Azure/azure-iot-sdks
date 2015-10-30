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

rem -----------------------------------------
rem - Specify the Azure SDK client build root
rem -----------------------------------------
set current-path=%~dp0
set current-path=%current-path:~0,-1%
set client-build-root=%current-path%\..\..
for %%i in ("%client-build-root%") do set client-build-root=%%~fi
echo Client Build root is %client-build-root%

rem -- Clear directories before starting process
del /F /Q %client-build-root%\build\tosign
del /F /Q %client-build-root%\build\signed

rem -- Build the DeviceExplorerWithInstaller.sln to ensure both projects are built
devenv %client-build-root%\tools\DeviceExplorer\DeviceExplorerWithInstaller.sln /Build "Release"
echo ErrorLevel after devenv is %errorlevel%
if not %errorlevel%==0 exit /b %errorlevel%

rem -- Copy the DeviceExplorer.exe file to the "tosign" folder
xcopy /q /y /R %client-build-root%\tools\DeviceExplorer\DeviceExplorer\bin\Release\DeviceExplorer.exe %client-build-root%\build\tosign\
if %errorlevel% neq 0 exit /b %errorlevel%

rem -- Auto-sign the DeviceExplorer.exe
csu.exe /s=True /w=True /i=%client-build-root%\build\tosign /o=%client-build-root%\build\signed /c1=400 /d="Signing DeviceExplorer.exe"
if %errorlevel% neq 0 exit /b %errorlevel%

rem -- Copy back the DeviceExplorer.exe to the build output directory
xcopy /q /y /R %client-build-root%\build\signed\DeviceExplorer.exe %client-build-root%\tools\DeviceExplorer\DeviceExplorer\bin\Release\
if %errorlevel% neq 0 exit /b %errorlevel%

xcopy /q /y /R %client-build-root%\build\signed\DeviceExplorer.exe %client-build-root%\tools\DeviceExplorer\DeviceExplorer\obj\Release\
if %errorlevel% neq 0 exit /b %errorlevel%

rem -- Clear directories
del /F /Q %client-build-root%\build\tosign
del /F /Q %client-build-root%\build\signed

rem -- Build the DeviceExplorerWithInstaller.sln. Only the SetupDeviceExplorer project will build since both were built before
devenv %client-build-root%\tools\DeviceExplorer\DeviceExplorerWithInstaller.sln /Build "Release"
echo ErrorLevel after devenv is %errorlevel%
if not %errorlevel%==0 exit /b %errorlevel%

rem -- Copy the .msi to the "tosign" folder
xcopy /q /y /R %client-build-root%\tools\DeviceExplorer\SetupDeviceExplorer\Release\SetupDeviceExplorer.msi %client-build-root%\build\tosign\
if %errorlevel% neq 0 exit /b %errorlevel%

rem -- Sign the .msi
csu.exe /s=True /w=True /i=%client-build-root%\build\tosign /o=%client-build-root%\build\signed /c1=400 /d="Signing SetupDeviceExplorer.msi"
if %errorlevel% neq 0 exit /b %errorlevel%

rem -- Copy back the .msi to the build output directory
xcopy /q /y /R %client-build-root%\build\signed\SetupDeviceExplorer.msi %client-build-root%\tools\DeviceExplorer\SetupDeviceExplorer\Release\
if %errorlevel% neq 0 exit /b %errorlevel%

rem -- Clean directories
del /F /Q %client-build-root%\build\tosign
del /F /Q %client-build-root%\build\signed
