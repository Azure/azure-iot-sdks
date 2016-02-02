@REM Copyright (c) Microsoft. All rights reserved.
@REM Licensed under the MIT license. See LICENSE file in the project root for full license information.

@setlocal EnableExtensions EnableDelayedExpansion
@echo off

rem - Specify the Azure SDK client build root
set current-path=%~dp0
set current-path=%current-path:~0,-1%
set client-root=%current-path%\..\..\..\..
for %%i in ("%client-root%") do set client-root=%%~fi
echo Client root is %client-root%

pushd %client-root%\c\build_all\packaging\windows\

where /q nuget.exe
if not !errorlevel! == 0 (
@Echo Azure IoT SDK needs to download nuget.exe from https://www.nuget.org/nuget.exe 
@Echo https://www.nuget.org 
choice /C yn /M "Do you want to download and run nuget.exe?" 
if not !errorlevel!==1 goto :eof
rem if nuget.exe is not found, then ask user
Powershell.exe wget -outf nuget.exe https://nuget.org/nuget.exe
	if not exist .\nuget.exe (
		echo nuget does not exist
		exit /b 1
	)
)

echo Deleting any existing .nupkg files...
del *.nupkg

rem -- Copy all Win32 files from cmake build directory to the repo directory
xcopy /q /y /R %USERPROFILE%\cmake_Win32\iothub_client\Debug\*.* %client-root%\build_output\c\win32\debug\*.*
if %errorlevel% neq 0 exit /b %errorlevel%

xcopy /q /y /R %USERPROFILE%\cmake_Win32\serializer\Debug\*.* %client-root%\build_output\c\win32\debug\*.*
if %errorlevel% neq 0 exit /b %errorlevel%

xcopy /q /y /R %USERPROFILE%\cmake_Win32\iothub_client\Release\*.* %client-root%\build_output\c\win32\release\*.*
if %errorlevel% neq 0 exit /b %errorlevel%

xcopy /q /y /R %USERPROFILE%\cmake_Win32\serializer\Release\*.* %client-root%\build_output\c\win32\release\*.*
if %errorlevel% neq 0 exit /b %errorlevel%

rem -- Copy all x64 files from cmake build directory to the repo directory
xcopy /q /y /R %USERPROFILE%\cmake_x64\iothub_client\Debug\*.* %client-root%\build_output\c\x64\debug\*.*
if %errorlevel% neq 0 exit /b %errorlevel%

xcopy /q /y /R %USERPROFILE%\cmake_x64\serializer\Debug\*.* %client-root%\build_output\c\x64\debug\*.*
if %errorlevel% neq 0 exit /b %errorlevel%

xcopy /q /y /R %USERPROFILE%\cmake_x64\iothub_client\Release\*.* %client-root%\build_output\c\x64\release\*.*
if %errorlevel% neq 0 exit /b %errorlevel%

xcopy /q /y /R %USERPROFILE%\cmake_x64\serializer\Release\*.* %client-root%\build_output\c\x64\release\*.*
if %errorlevel% neq 0 exit /b %errorlevel%

rem -- Package Nuget
nuget pack Microsoft.Azure.IoTHub.HttpTransport.nuspec
nuget pack Microsoft.Azure.IoTHub.AmqpTransport.nuspec
nuget pack Microsoft.Azure.IoTHub.IoTHubClient.nuspec

nuget pack Microsoft.Azure.IoTHub.MqttTransport.nuspec

nuget pack Microsoft.Azure.IoTHub.Serializer.nuspec

rmdir %client-root%\build_output /S /Q

popd
