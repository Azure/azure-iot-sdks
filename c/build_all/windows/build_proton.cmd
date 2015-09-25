@setlocal EnableExtensions EnableDelayedExpansion
@echo off

set proton-repo=https://github.com/Azure/qpid-proton.git
set proton-branch=0.9-IoTClient
set package-id=Apache.QPID.Proton.AzureIot
set package-root=%~dp0\..\packaging\windows

rem -----------------------------------------------------------------------------
rem -- use PROTON_PATH if specified, otherwise <CurrentDrive>:\proton\qpid-proton
rem -----------------------------------------------------------------------------

set proton-root=%~d0\proton
if defined PROTON_PATH set proton-root=%PROTON_PATH%
set build-root=%proton-root%\qpid-proton
echo Proton is located at %proton-root%
echo Build root is %build-root%
echo Nuget Package files at %package-root%

rem -----------------------------------------------------------------------------
rem -- Make sure they know this is a third party component
rem -----------------------------------------------------------------------------

@Echo Azure IoT SDK has a dependency on apache qpid-proton-c
@Echo https://github.com/apache/qpid-proton/blob/master/LICENSE

choice /C yn /M "Do you want to install the component?" 
if not %errorlevel%==1 goto :eof

rem -----------------------------------------------------------------------------
rem -- sync the Proton source code
rem -----------------------------------------------------------------------------

if Exist %build-root% rmdir %build-root% /s /q
git clone -b %proton-branch% %proton-repo% %build-root%

if not %errorlevel%==0 exit /b %errorlevel%

rem -----------------------------------------------------------------------------
rem -- build
rem -----------------------------------------------------------------------------

if not defined VCINSTALLDIR (
rem take a best guess
echo "taking a best guess at where Visual Studio 2015 might be installed: C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC"
echo if visual studio tools are not to be found, then run this batch file from a visual studio command prompt.
set "VCINSTALLDIR=C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC"
)

rem build proton x86
CALL "%VCINSTALLDIR%\vcvarsall.bat" x86
if not exist %build-root%\build (
    md %build-root%\build
    if not %errorlevel%==0 exit /b %errorlevel%
)
pushd %build-root%\build

cmake -G "Visual Studio 14 2015" .. 
if not %errorlevel%==0 exit /b %errorlevel%

msbuild /m proton.sln /property:Configuration=Debug
if not %errorlevel%==0 exit /b %errorlevel%

msbuild /m proton.sln /property:Configuration=Release
if not %errorlevel%==0 exit /b %errorlevel%

rem build proton x64
CALL "%VCINSTALLDIR%\vcvarsall.bat" x64
if not exist %build-root%\build_x64 (
    md %build-root%\build_x64
    if not %errorlevel%==0 exit /b %errorlevel%
)
pushd %build-root%\build_x64

cmake -G "Visual Studio 14 2015 Win64" .. 
if not %errorlevel%==0 exit /b %errorlevel%

msbuild /m proton.sln /property:Configuration=Debug
if not %errorlevel%==0 exit /b %errorlevel%

msbuild /m proton.sln /property:Configuration=Release
if not %errorlevel%==0 exit /b %errorlevel%

rem build proton arm
rem by means of copy & paste from the previous platform, but crypt32.lib doesn't exist and proton then doesn't link.

rem -----------------------------------------------------------------------------
rem -- Create the nuget package
rem -----------------------------------------------------------------------------

copy %package-root%\%package-id%.* %proton-root%
if not %errorlevel%==0 exit /b %errorlevel%

pushd %proton-root%

if not exist %package-id%.nuspec (
    echo nuspec does not exist
    exit /b 1
)
if not exist %package-id%.targets (
    echo targets does not exist
    exit /b 1
)

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

nuget.exe pack %package-id%.nuspec
if not %errorlevel%==0 exit /b %errorlevel%
