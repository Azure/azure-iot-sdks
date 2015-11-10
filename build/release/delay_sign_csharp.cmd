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

rem -- Retain copy of original AssemblyInfo.cs
rem -- The echo F piping to xcopy in order to affirm to xcopy the destination to be create is for a "file"
echo F | xcopy /q /y /R %build-root%\csharp\device\Microsoft.Azure.Devices.Client\Properties\AssemblyInfo.cs %build-root%\csharp\device\Microsoft.Azure.Devices.Client\Properties\Original_AssemblyInfo.cs
if %errorlevel% neq 0 exit /b %errorlevel%

echo F | xcopy /q /y /R %build-root%\csharp\device\Microsoft.Azure.Devices.Client.WinRT\Properties\AssemblyInfo.cs %build-root%\csharp\device\Microsoft.Azure.Devices.Client.WinRT\Properties\Original_AssemblyInfo.cs
if %errorlevel% neq 0 exit /b %errorlevel%

rem -- Add the Attributes for Delay-Signing to the AssemblyInfo.cs file(s)
echo [assembly: AssemblyDelaySignAttribute(true)] >> %build-root%\csharp\device\Microsoft.Azure.Devices.Client\Properties\AssemblyInfo.cs
echo [assembly: AssemblyKeyFileAttribute("35MSSharedLib1024.snk")] >> %build-root%\csharp\device\Microsoft.Azure.Devices.Client\Properties\AssemblyInfo.cs

echo [assembly: AssemblyDelaySignAttribute(true)] >> %build-root%\csharp\device\Microsoft.Azure.Devices.Client.WinRT\Properties\AssemblyInfo.cs
echo [assembly: AssemblyKeyFileAttribute("35MSSharedLib1024.snk")] >> %build-root%\csharp\device\Microsoft.Azure.Devices.Client.WinRT\Properties\AssemblyInfo.cs

rem -- Build csharp client with updated AssemblyInfo.cs
set build_error=0
echo Y | call %build-root%\csharp\device\build\build.cmd
if %errorlevel% neq 0 set build_error=1

rem -- Restore the original AssemblyInfo.cs
xcopy /q /y /R %build-root%\csharp\device\Microsoft.Azure.Devices.Client\Properties\Original_AssemblyInfo.cs %build-root%\csharp\device\Microsoft.Azure.Devices.Client\Properties\AssemblyInfo.cs
if %errorlevel% neq 0 exit /b %errorlevel%

xcopy /q /y /R %build-root%\csharp\device\Microsoft.Azure.Devices.Client.WinRT\Properties\Original_AssemblyInfo.cs %build-root%\csharp\device\Microsoft.Azure.Devices.Client.WinRT\Properties\AssemblyInfo.cs
if %errorlevel% neq 0 exit /b %errorlevel%

rem -- Delete unwanted files
del /F /Q %build-root%\csharp\device\Microsoft.Azure.Devices.Client\Properties\Original_AssemblyInfo.cs
del /F /Q %build-root%\csharp\device\Microsoft.Azure.Devices.Client.WinRT\Properties\Original_AssemblyInfo.cs

echo Charp Build Error: %build_error%

if %build_error% neq 0 exit /b 1
