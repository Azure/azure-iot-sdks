@REM Copyright (c) Microsoft. All rights reserved.
@REM Licensed under the MIT license. See LICENSE file in the project root for full license information.

@setlocal EnableExtensions EnableDelayedExpansion
@echo off
if not defined NUGET_API_KEY (
	echo script need to find an environment variable called NUGET_API_KEY.
	exit /b 1
) else (
	rem ensure nuget.exe exists
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
	nuget push *.nupkg %NUGET_API_KEY% -NonInteractive
	if not %errorlevel%==0 exit /b %errorlevel%
)
echo all nugets have been pushed correctly