@REM Copyright (c) Microsoft. All rights reserved.
@REM Licensed under the MIT license. See LICENSE file in the project root for full license information.

setlocal

set build-root=%~dp0
rem // resolve to fully qualified path
for %%i in ("%build-root%") do set build-root=%%~fi


REM keep in sync with c build
set build-platform=Win32
set cmake-output=cmake_%build-platform%

REM -- C --
cd %build-root%..\..\..\c\build_all\windows
call build_client.cmd --platform %build-platform% --buildpython %1 %2 %3
if errorlevel 1 exit /b 1
cd %build-root%

echo CMAKE Output Path: %USERPROFILE%\%cmake-output%\python

copy %USERPROFILE%\%cmake-output%\python\src\Release\iothub_client.pyd ..\..\device\samples
copy %USERPROFILE%\%cmake-output%\python\test\Release\iothub_client_mock.pyd ..\..\device\tests

cd ..\..\device\tests
python iothub_client_ut.py
if ERRORLEVEL 1 exit /b 1
echo Python unit test PASSED
cd %build-root%