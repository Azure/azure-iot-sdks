@REM Copyright (c) Microsoft. All rights reserved.
@REM Licensed under the MIT license. See LICENSE file in the project root for full license information.

@setlocal EnableExtensions EnableDelayedExpansion
@echo off

rem // default build options
set build-clean=0
set run-ut=0
set build-config=Release
set build-platform=x86

set current-path=%~dp0
rem // remove trailing slash
set current-path=%current-path:~0,-1%

set build-root=%current-path%\..\..
rem // resolve to fully qualified path
for %%i in ("%build-root%") do set build-root=%%~fi

rem -----------------------------------------------------------------------------
rem -- check prerequisites
rem -----------------------------------------------------------------------------

rem ensure python.exe exists
where /q python.exe
if errorlevel 1 goto :NeedPython

rem -----------------------------------------------------------------------------
rem -- detect Python x86 or x64 version, select build target accordingly
rem -----------------------------------------------------------------------------
python python_version_check.py >pyenv.bat
if errorlevel 1 goto :NeedPython
call pyenv.bat
@Echo Using Python found in: %PYTHON_PATH%, building %build-platform% platform extension
goto :nuget

:NeedPython
@Echo Azure IoT SDK needs Python from 
@Echo https://www.python.org/downloads/
exit /b 1

:nuget
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

rem -----------------------------------------------------------------------------
rem -- parse script arguments
rem -----------------------------------------------------------------------------

:args-loop
if "%1" equ "" goto args-done
if "%1" equ "-c" goto arg-build-clean
if "%1" equ "--clean" goto arg-build-clean
if "%1" equ "--config" goto arg-build-config
if "%1" equ "--run-ut" goto arg-build-run-ut
call :usage && exit /b 1

:arg-build-clean
set build-clean=1
goto args-continue

:arg-build-config
shift
if "%1" equ "" call :usage && exit /b 1
set build-config=%1
goto args-continue

:arg-build-run-ut
set run-ut=1
goto args-continue

:args-continue
shift
goto args-loop

:args-done

rem -----------------------------------------------------------------------------
rem -- restore packages for solutions
rem -----------------------------------------------------------------------------

set PYTHON_SOLUTION_PATH=%build-root%\device\iothub_client_python\windows
set PYTHON_SOLUTION="%PYTHON_SOLUTION_PATH%\iothub_client_python.sln"

if "%build-platform%"=="x64" (
    set PYTHON_SOLUTION_PATH=%PYTHON_SOLUTION_PATH%\x64
)

call nuget restore -config "%current-path%\NuGet.Config" %PYTHON_SOLUTION%

rem -----------------------------------------------------------------------------
rem -- clean solutions
rem -----------------------------------------------------------------------------

if %build-clean%==1 (
    call nuget restore %PYTHON_SOLUTION%
    call :clean-a-solution %PYTHON_SOLUTION%
    if not %errorlevel%==0 exit /b %errorlevel%
)

rem -----------------------------------------------------------------------------
rem -- build solutions
rem -----------------------------------------------------------------------------

call :build-a-solution %PYTHON_SOLUTION%
if not %errorlevel%==0 exit /b %errorlevel%

rem -----------------------------------------------------------------------------
rem -- Copy Python extension to sample and test folder
rem -----------------------------------------------------------------------------

@echo Python extension release folder: %PYTHON_SOLUTION_PATH%\%build-config%
@echo Copy iothub_client.pyd to %build-root%\device\samples
copy /Y %PYTHON_SOLUTION_PATH%\%build-config%\iothub_client.pyd  %build-root%\device\samples\ 
if not %errorlevel%==0 exit /b %errorlevel%
@echo Copy iothub_client_mock.pyd to %build-root%\device\tests
copy /Y %PYTHON_SOLUTION_PATH%\%build-config%\iothub_client_mock.pyd  %build-root%\device\tests\ 
if not %errorlevel%==0 exit /b %errorlevel%

rem -----------------------------------------------------------------------------
rem -- Python library unit test
rem -----------------------------------------------------------------------------

if %run-ut%==1 (
    cd %build-root%\device\tests
    python iothub_client_ut.py
    if ERRORLEVEL 1 exit /b 1
    echo Python unit test PASSED
)

rem -----------------------------------------------------------------------------
rem -- build done
rem -----------------------------------------------------------------------------

goto :eof

rem -----------------------------------------------------------------------------
rem -- subroutines
rem -----------------------------------------------------------------------------

:clean-a-solution
call :_run-msbuild "Clean" %1 %2 %3
goto :eof


:build-a-solution
call :_run-msbuild "Build" %1 %2 %3
goto :eof

:usage
echo build.cmd [options]
echo options:
echo  -c, --clean             delete artifacts from previous build before building
echo  --config ^<value^>      [Release] build configuration (e.g. Debug, Release)
echo  --run-ut                run the unit test after build
goto :eof

rem -----------------------------------------------------------------------------
rem -- helper subroutines
rem -----------------------------------------------------------------------------

:_run-msbuild
rem // optionally override configuration|platform
setlocal EnableExtensions
set build-target=
if "%~1" neq "Build" set "build-target=/t:%~1"
if "%~3" neq "" set build-config=%~3
if "%~4" neq "" set build-platform=%~4

msbuild /m %build-target% "/p:Configuration=%build-config%;Platform=%build-platform%" %2
if not %errorlevel%==0 exit /b %errorlevel%
goto :eof

