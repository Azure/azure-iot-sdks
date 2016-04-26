@echo off

IF "%*"=="" (
  ECHO Please pass a connection string
  ECHO Usage:
  ECHO "%0 <Connection String>"
  GOTO END
)

msbuild /ver
IF %ERRORLEVEL% NEQ 0 (
   ECHO Error : cannot find msbuild. Please run this script from a Visual Studio Tools command prompt.
   GOTO END
)

rem ensure nuget.exe exists
where /q nuget.exe
if not %errorlevel% == 0 (
@Echo Azure IoT SDK needs to download nuget.exe from https://www.nuget.org/nuget.exe 
@Echo https://www.nuget.org 
choice /C yn /M "Do you want to download and run nuget.exe?" 
if not %errorlevel%==1 goto :eof
rem if nuget.exe is not found, then ask user
Powershell.exe wget -outf nuget.exe https://nuget.org/nuget.exe
     if not exist .\nuget.exe (
           echo nuget does not exist
           exit /b 1
     )
)

set OUTPUTPATH=%~dp0
set BINPATH=%OUTPUTPATH%\bin

@setlocal
set CMAKEOUTPUTDIR=%OUTPUTPATH%\cmake
set "IOTHUB_CONNECTION_STRING=%*"

pushd %OUTPUTPATH%

IF NOT EXIST %CMAKEOUTPUTDIR% mkdir %CMAKEOUTPUTDIR%

pushd  %CMAKEOUTPUTDIR%
ECHO Creating build files...
cmake ..\..\..\..\c
IF %ERRORLEVEL% NEQ 0 (
   ECHO Error creating build files with CMAKE.
   GOTO END
)

ECHO Building device client...
msbuild iotdm_client\samples\iotdm_simple_sample\iotdm_simple_sample.vcxproj /p:OutputPath=%BINPATH% /fl /flp:logfile=clientBuild.log
IF %ERRORLEVEL% NEQ 0 (
   ECHO Error building device client.
   GOTO END
)

popd
ECHO Building service samples...
set SOLUTIONFILE=.\GetStartedWithIoTHubDM\GetStartedWithIoTHubDM.sln
nuget restore %SOLUTIONFILE%
msbuild %SOLUTIONFILE% /p:OutputPath=%BINPATH% /fl /flp:logfile=samplesBuild.log
IF %ERRORLEVEL% NEQ 0 (
   ECHO Error building samples.
   GOTO END
)

ECHO Provisioning devices...
pushd %BINPATH%
GenerateDevices.exe %IOTHUB_CONNECTION_STRING%

@endlocal

cd %BINPATH%
ECHO All done!
ECHO You are now in a folder containing the sample applications.
ECHO You can run each sample by calling it with the connection string as an argument.

:END
@echo on