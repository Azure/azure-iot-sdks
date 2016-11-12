@REM Copyright (c) Microsoft. All rights reserved.
@REM Licensed under the MIT license. See LICENSE file in the project root for full license information.

@setlocal EnableExtensions EnableDelayedExpansion
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
set wheel=0
set platname=win32
set use-websockets=OFF

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
if "%1" equ "--wheel" goto arg-build-wheel
if "%1" equ "--use-websockets" goto arg-use-websockets

call :usage && exit /b 1

:arg-build-config
shift
if "%1" equ "" call :usage && exit /b 1
set build-config=%1
goto args-continue

:arg-build-wheel
set wheel=1
goto args-continue

:arg-use-websockets
set use-websockets=ON
goto args-continue

:args-continue
shift
goto args-loop

:args-done

:build

set cmake-output=cmake_%build-platform%

REM -- C --
cd %build-root%..\..\..\c\build_all\windows

if %use-websockets% == ON (
call build_client.cmd --platform %build-platform% --buildpython %build-python% --config %build-config% --use-websockets
) else (
call build_client.cmd --platform %build-platform% --buildpython %build-python% --config %build-config%
)

if not !ERRORLEVEL!==0 exit /b !ERRORLEVEL!
cd %build-root%

@Echo CMAKE Output Path: %USERPROFILE%\%cmake-output%\python

copy %USERPROFILE%\%cmake-output%\python\src\%build-config%\iothub_client.pyd ..\..\device\samples
if not !ERRORLEVEL!==0 exit /b !ERRORLEVEL!
copy %USERPROFILE%\%cmake-output%\python\test\%build-config%\iothub_client_mock.pyd ..\..\device\tests
if not !ERRORLEVEL!==0 exit /b !ERRORLEVEL!

cd ..\..\device\tests
@Echo python iothub_client_ut.py
python iothub_client_ut.py
if not !ERRORLEVEL!==0 exit /b !ERRORLEVEL!
@Echo python iothub_client_map_test.py
python iothub_client_map_test.py
if not !ERRORLEVEL!==0 exit /b !ERRORLEVEL!
echo Python unit test PASSED
cd %build-root%

rem -----------------------------------------------------------------------------
rem -- create PyPi wheel
rem -----------------------------------------------------------------------------

if "%build-platform%"=="x64" (
    set platname=win-amd64
)

if %wheel%==1 (
    echo Copy iothub_client.pyd to %build-root%\build_all\windows\iothub_client for Python wheel generation
    copy %USERPROFILE%\%cmake-output%\python\src\%build-config%\iothub_client.pyd ..\..\build_all\windows\iothub_client
    if not !ERRORLEVEL!==0 exit /b !ERRORLEVEL!
    cd %build-root%\build_all\windows
    echo update Python packages
    python -m pip install -U pip setuptools wheel twine
    echo create Python wheel: 
    echo "python setup.py bdist_wheel --plat-name %platname%"
    python setup.py bdist_wheel --plat-name "%platname%"
    if not !ERRORLEVEL!==0 exit /b !ERRORLEVEL!
    dir dist
    echo Yet another Python wheel done
)
goto :eof

:usage
echo build_client.cmd [options]
echo options:
echo  --config ^<value^>         [Debug] build configuration (e.g. Debug, Release)
echo  --platform ^<value^>       [Win32] build platform (e.g. Win32, x64, ...)
echo  --buildpython ^<value^>    [2.7]   build python extension (e.g. 2.7, 3.4, ...)
echo  --no-logging               Disable logging
echo  --use-websockets           Enable websocket support for AMQP and MQTT
goto :eof
