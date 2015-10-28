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

del *.nupkg

rem -- Copy all Win32 files from cmake build directory to the repo directory
xcopy /q /y /R %USERPROFILE%\cmake_Win32\iothub_client\Debug\*.* %client-root%\build_output\c\win32\debug\*.*
if %errorlevel% neq 0 exit /b %errorlevel%

xcopy /q /y /R %USERPROFILE%\cmake_Win32\common\Debug\*.* %client-root%\build_output\c\win32\debug\*.*
if %errorlevel% neq 0 exit /b %errorlevel%

xcopy /q /y /R %USERPROFILE%\cmake_Win32\serializer\Debug\*.* %client-root%\build_output\c\win32\debug\*.*
if %errorlevel% neq 0 exit /b %errorlevel%

xcopy /q /y /R %USERPROFILE%\cmake_Win32\iothub_client\Release\*.* %client-root%\build_output\c\win32\release\*.*
if %errorlevel% neq 0 exit /b %errorlevel%

xcopy /q /y /R %USERPROFILE%\cmake_Win32\common\Release\*.* %client-root%\build_output\c\win32\release\*.*
if %errorlevel% neq 0 exit /b %errorlevel%

xcopy /q /y /R %USERPROFILE%\cmake_Win32\serializer\Release\*.* %client-root%\build_output\c\win32\release\*.*
if %errorlevel% neq 0 exit /b %errorlevel%

rem -- Copy all x64 files from cmake build directory to the repo directory
xcopy /q /y /R %USERPROFILE%\cmake_x64\iothub_client\Debug\*.* %client-root%\build_output\c\x64\debug\*.*
if %errorlevel% neq 0 exit /b %errorlevel%

xcopy /q /y /R %USERPROFILE%\cmake_x64\common\Debug\*.* %client-root%\build_output\c\x64\debug\*.*
if %errorlevel% neq 0 exit /b %errorlevel%

xcopy /q /y /R %USERPROFILE%\cmake_x64\serializer\Debug\*.* %client-root%\build_output\c\x64\debug\*.*
if %errorlevel% neq 0 exit /b %errorlevel%

xcopy /q /y /R %USERPROFILE%\cmake_x64\iothub_client\Release\*.* %client-root%\build_output\c\x64\release\*.*
if %errorlevel% neq 0 exit /b %errorlevel%

xcopy /q /y /R %USERPROFILE%\cmake_x64\common\Release\*.* %client-root%\build_output\c\x64\release\*.*
if %errorlevel% neq 0 exit /b %errorlevel%

xcopy /q /y /R %USERPROFILE%\cmake_x64\serializer\Release\*.* %client-root%\build_output\c\x64\release\*.*
if %errorlevel% neq 0 exit /b %errorlevel%

rem -- Package Nuget
nuget pack Microsoft.Azure.IoTHub.HttpTransport.nuspec
nuget pack Microsoft.Azure.IoTHub.AmqpTransport.nuspec
nuget pack Microsoft.Azure.IoTHub.IoTHubClient.nuspec
nuget pack Microsoft.Azure.IoTHub.Common.nuspec

rem because nuget cannot access absolute files given by environment variables
mkdir paho_outputs
cd paho_outputs
mkdir Win32
mkdir x64
mkdir include
cd Win32
mkdir Release
mkdir Debug
cd ..
cd x64
mkdir Release
mkdir Debug
cd ..
cd ..

rem -- Copy Win32 Paho Files.
copy "%PAHO_PATH%\org.eclipse.paho.mqtt.c\Windows Build\Debug\paho-mqtt3cs.lib" paho_outputs\Win32\Debug
copy "%PAHO_PATH%\org.eclipse.paho.mqtt.c\Windows Build\Debug\paho-mqtt3cs.pdb" paho_outputs\Win32\Debug
copy "%PAHO_PATH%\org.eclipse.paho.mqtt.c\Windows Build\Debug\paho-mqtt3cs.dll" paho_outputs\Win32\Debug

copy "%PAHO_PATH%\org.eclipse.paho.mqtt.c\build\output\paho-mqtt3cs.lib" paho_outputs\Win32\Release
copy "%PAHO_PATH%\org.eclipse.paho.mqtt.c\build\output\paho-mqtt3cs.dll" paho_outputs\Win32\Release

rem -- Copy 64 bits Paho Files.
copy "%PAHO_PATH%\org.eclipse.paho.mqtt.c\Windows Build\x64\Debug\paho-mqtt3cs.lib" paho_outputs\x64\Debug
copy "%PAHO_PATH%\org.eclipse.paho.mqtt.c\Windows Build\x64\Debug\paho-mqtt3cs.pdb" paho_outputs\x64\Debug
copy "%PAHO_PATH%\org.eclipse.paho.mqtt.c\Windows Build\x64\Debug\paho-mqtt3cs.dll" paho_outputs\x64\Debug

copy "%PAHO_PATH%\org.eclipse.paho.mqtt.c\Windows Build\x64\Release\paho-mqtt3cs.lib" paho_outputs\x64\Release
copy "%PAHO_PATH%\org.eclipse.paho.mqtt.c\Windows Build\x64\Release\paho-mqtt3cs.dll" paho_outputs\x64\Release

rem -- Copy Header Files
copy "%PAHO_PATH%\org.eclipse.paho.mqtt.c\src\*.h" paho_outputs\include


mkdir openssl_outputs
cd openssl_outputs
mkdir Win32
cd Win32
mkdir Debug
cd ..
mkdir x64
cd x64
mkdir Debug
cd ..
cd ..

copy "%OpenSSLDir%\out32dllForNuget\libeay32.dll" openssl_outputs\Win32\Debug
copy "%OpenSSLDir%\out32dllForNuget\libeay32.lib" openssl_outputs\Win32\Debug
copy "%OpenSSLDir%\out32dllForNuget\libeay32.exp" openssl_outputs\Win32\Debug
copy "%OpenSSLDir%\out32dllForNuget\libeay32.pdb" openssl_outputs\Win32\Debug
copy "%OpenSSLDir%\out32dllForNuget\ssleay32.dll" openssl_outputs\Win32\Debug
copy "%OpenSSLDir%\out32dllForNuget\ssleay32.lib" openssl_outputs\Win32\Debug
copy "%OpenSSLDir%\out32dllForNuget\ssleay32.exp" openssl_outputs\Win32\Debug
copy "%OpenSSLDir%\out32dllForNuget\ssleay32.pdb" openssl_outputs\Win32\Debug

copy "%OpenSSLDir%\out64dll\libeay32.dll" openssl_outputs\x64\Debug
copy "%OpenSSLDir%\out64dll\libeay32.lib" openssl_outputs\x64\Debug
copy "%OpenSSLDir%\out64dll\libeay32.exp" openssl_outputs\x64\Debug
copy "%OpenSSLDir%\out64dll\libeay32.pdb" openssl_outputs\x64\Debug
copy "%OpenSSLDir%\out64dll\ssleay32.dll" openssl_outputs\x64\Debug
copy "%OpenSSLDir%\out64dll\ssleay32.lib" openssl_outputs\x64\Debug
copy "%OpenSSLDir%\out64dll\ssleay32.exp" openssl_outputs\x64\Debug
copy "%OpenSSLDir%\out64dll\ssleay32.pdb" openssl_outputs\x64\Debug

nuget pack Eclipse.Paho-C.paho-mqtt3cs.nuspec


nuget pack Microsoft.Azure.IoTHub.MqttTransport.nuspec

rmdir paho_outputs /S /Q
rmdir openssl_outputs /S /Q

nuget pack Microsoft.Azure.IoTHub.Serializer.nuspec

rmdir %client-root%\build_output /S /Q

popd
