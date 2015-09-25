@REM Copyright (c) Microsoft. All rights reserved.
@REM Licensed under the MIT license. See LICENSE file in the project root for full license information.

@echo off
if not defined NUGET_API_KEY (
	echo script need to find an environment variable called NUGET_API_KEY.
	exit /b 1
) else (
	Powershell.exe wget -outf nuget.exe https://nuget.org/nuget.exe
	if not exist .\nuget.exe (
		echo nuget does not exist
		exit /b 1
	)
	nuget push *.nupkg %NUGET_API_KEY% -NonInteractive
	if not %errorlevel%==0 exit /b %errorlevel%
)
echo all nugets have been pushed correctly