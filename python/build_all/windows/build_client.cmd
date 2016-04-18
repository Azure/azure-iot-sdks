@REM Copyright (c) Microsoft. All rights reserved.
@REM Licensed under the MIT license. See LICENSE file in the project root for full license information.

@setlocal
@echo off

set build-root=%~dp0
rem // resolve to fully qualified path
for %%i in ("%build-root%") do set build-root=%%~fi
cd %build-root%

rem -----------------------------------------------------------------------------
rem -- check prerequisites
rem -----------------------------------------------------------------------------

rem ensure python.exe exists
where /q python.exe
if errorlevel 1 goto :NeedPython

rem -----------------------------------------------------------------------------
rem -- detect Python x86 or x64 version, select build target accordingly
rem -----------------------------------------------------------------------------

REM target may be set to 64 bit build if a Python x64 detected
set build-platform=Win32
set build-config=Release
set build-python=2.7

python python_version_check.py >pyenv.bat
if errorlevel 1 goto :NeedPython
call pyenv.bat
@Echo Using Python found in: %PYTHON_PATH%, building Python %build-python% %build-platform% extension
goto :args-loop

:NeedPython
@Echo Azure IoT SDK needs Python 2.7 or Python 3.4 from 
@Echo https://www.python.org/downloads/
exit /b 1

:args-loop
if "%1" equ "" goto args-done
if "%1" equ "--config" goto arg-build-config
call :usage && exit /b 1

:arg-build-config
shift
if "%1" equ "" call :usage && exit /b 1
set build-config=%1
goto args-continue

:args-continue
shift
goto args-loop

:args-done

:build

set cmake-output=cmake_%build-platform%

REM -- C --
cd %build-root%..\..\..\c\build_all\windows
call build_client.cmd --platform %build-platform% --buildpython %build-python% --config %build-config%
if errorlevel 1 exit /b 1
cd %build-root%

@Echo CMAKE Output Path: %USERPROFILE%\%cmake-output%\python

copy %USERPROFILE%\%cmake-output%\python\src\%build-config%\iothub_client.pyd ..\..\device\samples
if not %errorlevel%==0 exit /b %errorlevel%
copy %USERPROFILE%\%cmake-output%\python\test\%build-config%\iothub_client_mock.pyd ..\..\device\tests
if not %errorlevel%==0 exit /b %errorlevel%

cd ..\..\device\tests
@Echo python iothub_client_ut.py
python iothub_client_ut.py
if ERRORLEVEL 1 exit /b 1
echo Python unit test PASSED
cd %build-root%