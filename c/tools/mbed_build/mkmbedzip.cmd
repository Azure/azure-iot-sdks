@REM  Copyright (c) Microsoft. All rights reserved.
@REM  Licensed under the MIT license. See LICENSE file in the project root for full license information.

setlocal

@REM  Script arguments:
@REM  %1 Local path to sample code directory. This script assumes other source code dependencies are in specific
@REM       locations relative to this directory.
@REM  %2 Local path to zip file to create.
@REM  %3 (optional) Wildcard pattern that describes files to delete from staging directory before zipping.
@REM       E.g., *ampq*.*

set "sample_path=%~1"
set "zip_path=%~2"
set "exclude_pattern=%~3"
set "exclude_pattern2=%~4"
set "exclude_pattern3=%~5"

if not exist "%sample_path%" (
    echo *************************************
    echo ** ERROR: "%sample_path%" not found.
    echo *************************************
    goto :eof
)

for %%i in ("%sample_path%\..\..\..\..") do set "root_path=%%~fi"

for %%i in ("%sample_path%\..\..\..\..\..\..") do set "repo_root=%%~fi"

call "%~dp0\checkdep.cmd" 7-Zip 7z.exe "%ProgramFiles%\7-Zip"
if %ERRORLEVEL% neq 0 goto :eof

cd "%sample_path%"

SET outdir=mbed_out

@REM  Delete any residual files from previous batch file runs
RMDIR /S/Q %outdir% 2>nul
DEL /F /Q *.zip 2>nul

@REM  Create a directory for temporary file storage
MKDIR %outdir%

echo.
echo Gathering files to zip...

@REM  Copy shared-Util code
COPY %repo_root%\azure-c-shared-utility\c\src\*.c %outdir%
COPY %repo_root%\azure-c-shared-utility\c\inc\*.h %outdir%

@REM  Copy mbed-specific common code
COPY /Y %repo_root%\azure-c-shared-utility\c\adapters\*mbed.c %outdir%
COPY /Y %repo_root%\azure-c-shared-utility\c\adapters\*mbed.cpp %outdir%
COPY /Y %repo_root%\azure-c-shared-utility\c\adapters\wolfssl_connection.* %outdir%

@REM  Copy iothub_client code
COPY %root_path%\iothub_client\src\*.c %outdir%
COPY %root_path%\iothub_client\inc\*.h %outdir%

@REM  Copy serializer code if necessary
if not "%sample_path%"=="%sample_path:serializer=iothub_client%" (
    COPY %root_path%\serializer\src\*.c %outdir%
    COPY %root_path%\serializer\inc\*.h %outdir%
)

@REM  Copy sample code
COPY main.cpp %outdir%
COPY ..\*.cpp %outdir% 2>nul
COPY ..\*.c %outdir%
COPY ..\*.h %outdir%

@REM  agenttime.c isn't used on mbed
DEL /F /Q %outdir%\agenttime.c

@REM  exclude requested files from staging directory before zipping
if not "%exclude_pattern%"=="" (
    DEL /F /Q %outdir%\%exclude_pattern%
)

@REM  exclude requested files from staging directory before zipping 
if not "%exclude_pattern2%"=="" (
    DEL /F /Q %outdir%\%exclude_pattern2%
)

@REM  exclude requested files from staging directory before zipping 
if not "%exclude_pattern3%"=="" (
    DEL /F /Q %outdir%\%exclude_pattern3%
)

@REM  Put all files into a zip file
PUSHD %outdir%
7z a -r "%zip_path%" *.*
POPD

RMDIR /S/Q %outdir%
