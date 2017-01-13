@REM Copyright (c) Microsoft. All rights reserved.
@REM Licensed under the MIT license. See LICENSE file in the project root for full license information.

@setlocal EnableExtensions EnableDelayedExpansion
@echo off

:args-loop
if "%1" equ "" goto args-done
if "%1" equ "--path" goto packages-path
if "%1" equ "--feed" goto nuget-feed
call :usage && exit /b 1

:packages-path
shift
if "%1" equ "" call :usage && exit /b 1
set nuget_packages_path=%1
rem // remove trailing slash
IF %nuget_packages_path:~-1%==\ SET nuget_packages_path=%nuget_packages_path:~0,-1%
echo Packages Source: %nuget_packages_path%
goto args-continue

:nuget-feed
shift
if "%1" equ "" call :usage && exit /b 1
set nuget_feed_path=%1
rem // remove trailing slash
IF %nuget_feed_path:~-1%==\ SET nuget_feed_path=%nuget_feed_path:~0,-1%
echo Publishing to: %nuget_feed_path%
goto args-continue

:args-continue
shift
goto args-loop

:args-done

if not defined NUGET_API_KEY (
	echo Need to define environment variable NUGET_API_KEY.
	exit /b 1
) else (
	rem Ensure nuget.exe exists
	where /q nuget.exe
	if not !errorlevel! == 0 (
	@Echo Azure IoT SDK needs to download nuget.exe from https://www.nuget.org/nuget.exe 
	@Echo https://www.nuget.org
	choice /C yn /M "Do you want to download and run nuget.exe"
	if not !errorlevel!==1 goto :eof
	rem if nuget.exe is not found, then ask user
	Powershell.exe wget -outf nuget.exe https://nuget.org/nuget.exe
		if not exist .\nuget.exe (
			echo nuget does not exist
			exit /b 1
		)
	)
	
	if not defined nuget_feed_path (
		choice /C yn /M "No NuGet feed specified. Do you want to publish to Nuget.org"
		if not !errorlevel!==1 goto :eof
		echo Publishing to Nuget.org with ApiKey: %NUGET_API_KEY%
		nuget push %nuget_packages_path%\*.nupkg -ApiKey %NUGET_API_KEY% -NonInteractive
		if not !ERRORLEVEL!==0 exit /b !ERRORLEVEL!
	) else (
		nuget push %nuget_packages_path%\*.nupkg -ApiKey %NUGET_API_KEY% -Source %nuget_feed_path% -NonInteractive
		if not !ERRORLEVEL!==0 exit /b !ERRORLEVEL!
	)
)

echo All NuGet packages have been published successfully!
goto :eof
