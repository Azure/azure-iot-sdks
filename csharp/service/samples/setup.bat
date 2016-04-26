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

ECHO Building service samples...

msbuild ..\GetStartedWithIoTHubDM\GetStartedWithIoTHubDM.sln /p:OutputPath=%BINPATH% /fl /flp:logfile=samplesBuild.log
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