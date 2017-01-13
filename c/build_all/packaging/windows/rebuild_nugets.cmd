@REM Copyright (c) Microsoft. All rights reserved.
@REM Licensed under the MIT license. See LICENSE file in the project root for full license information.

@setlocal EnableExtensions EnableDelayedExpansion
@echo off

rem - Specify the Azure SDK client build root
set current-path=%~dp0
set current-path=%current-path:~0,-1%
set build-root=%current-path%\..\..\..\..
for %%i in ("%build-root%") do set build-root=%%~fi
echo Build root is %build-root%

pushd %build-root%\c\build_all\packaging\windows\

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

set build-path=%build-root%\c\cmake

if exist %build-path%\iot_sdk_output (
	rmdir /s/q %build-path%\iot_sdk_output
	rem no error checking
)

echo Deleting any existing .nupkg files...
if exist *.nupkg (
	del *.nupkg
)

rem -- Copy all Win32 files from cmake build directory to the repo directory
xcopy /q /y /R %build-path%\iotsdk_win32\iothub_client\Debug\*.* %build-path%\iot_sdk_output\c\win32\debug\*.*
if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!

xcopy /q /y /R %build-path%\iotsdk_win32\serializer\Debug\*.* %build-path%\iot_sdk_output\c\win32\debug\*.*
if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!

xcopy /q /y /R %build-path%\iotsdk_win32\iothub_client\Release\*.* %build-path%\iot_sdk_output\c\win32\release\*.*
if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!

xcopy /q /y /R %build-path%\iotsdk_win32\serializer\Release\*.* %build-path%\iot_sdk_output\c\win32\release\*.*
if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!

rem -- Copy all x64 files from cmake build directory to the repo directory
xcopy /q /y /R %build-path%\iotsdk_x64\iothub_client\Debug\*.* %build-path%\iot_sdk_output\c\x64\debug\*.*
if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!

xcopy /q /y /R %build-path%\iotsdk_x64\serializer\Debug\*.* %build-path%\iot_sdk_output\c\x64\debug\*.*
if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!

xcopy /q /y /R %build-path%\iotsdk_x64\iothub_client\Release\*.* %build-path%\iot_sdk_output\c\x64\release\*.*
if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!

xcopy /q /y /R %build-path%\iotsdk_x64\serializer\Release\*.* %build-path%\iot_sdk_output\c\x64\release\*.*
if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!

rem -- Copy all arm files from cmake build directory to the repo directory
xcopy /q /y /R %build-path%\iotsdk_arm\iothub_client\Debug\*.* %build-path%\iot_sdk_output\c\arm\debug\*.*
if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!

xcopy /q /y /R %build-path%\iotsdk_arm\serializer\Debug\*.* %build-path%\iot_sdk_output\c\arm\debug\*.*
if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!

xcopy /q /y /R %build-path%\iotsdk_arm\iothub_client\Release\*.* %build-path%\iot_sdk_output\c\arm\release\*.*
if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!

xcopy /q /y /R %build-path%\iotsdk_arm\serializer\Release\*.* %build-path%\iot_sdk_output\c\arm\release\*.*
if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!

rem -- Package Nuget
nuget pack Microsoft.Azure.IoTHub.HttpTransport.nuspec
nuget pack Microsoft.Azure.IoTHub.AmqpTransport.nuspec
nuget pack Microsoft.Azure.IoTHub.IoTHubClient.nuspec
nuget pack Microsoft.Azure.IoTHub.MqttTransport.nuspec
nuget pack Microsoft.Azure.IoTHub.Serializer.nuspec

rmdir %client-root%\iot_sdk_output /S /Q

popd
