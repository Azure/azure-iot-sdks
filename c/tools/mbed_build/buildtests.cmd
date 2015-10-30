@REM  Copyright (c) Microsoft. All rights reserved.
@REM  Licensed under the MIT license. See LICENSE file in the project root for full license information.

setlocal

@REM  Script arguments:
@REM  %1 Local path to tests code directory. This script assumes other source code dependencies are in specific
@REM       locations relative to this directory.
@REM  %2 Name of Mercurial repository at http://developer.mbed.org/users/<user>/code/.
@REM  %3 Name of root folder in repository. This is where the source code lives.
@REM  %4 Name of mbed platform to pass to compiler

set "tests_path=%~1"
set "repo_name=%~2"
set "folder_name=%~3"
set "mbed_plat=%~4"
set "mbed_bin_path=%~5"

if not exist "%tests_path%" (
    echo ** "%sample_path%" not found **
    goto :eof
)

@REM  Verify our dependencies

call "%~dp0\checkdep.cmd" Mercurial hg.exe "%ProgramFiles%\TortoiseHG"
if %errorlevel% neq 0 goto :eof

@REM  Build compilembed.exe (practically a no-op if it already exists)

call "%~dp0\..\compilembed\build.cmd"
if %errorlevel% neq 0 goto :eof

@REM  Clone the mbed repo

cd "%tests_path%"

rmdir /s /q hg 2>nul
mkdir hg
cd hg

echo.
echo Cloning repo '%repo_name%'...

set mbed_creds_defined=false
if defined MBED_HG_USER if defined MBED_USER if defined MBED_PWD set mbed_creds_defined=true
if not "%mbed_creds_defined%"=="true" (
    echo ** MBED_HG_USER, MBED_USER, or MBED_PWD not defined in your environment **
    exit /b 1
)

hg clone --insecure https://%MBED_USER%:%MBED_PWD%@developer.mbed.org/users/%MBED_USER%/code/%repo_name%/

mkdir %repo_name%\%folder_name%
cd %repo_name%\%folder_name%
if %errorlevel% neq 0 goto :eof

del /q *.*
if %errorlevel% neq 0 goto :eof

echo.
echo Overwriting repository files with new files...

SET outdir=%tests_path%\hg\%repo_name%\%folder_name%
SET root_path=%tests_path%\..\..\..\..\

@REM  Delete any residual files from previous batch file runs
RMDIR /S/Q %outdir% 2>nul
DEL /F /Q *.zip 2>nul

echo.
echo Gathering files ...

@REM  Copy common code
COPY %root_path%\common\src\*.c %outdir%
COPY %root_path%\common\inc\*.h %outdir%

@REM  Copy test tools code
COPY %root_path%\testtools\testrunnerswitcher\inc\*.h %outdir%
COPY %root_path%\testtools\micromock\inc\*.h %outdir%
COPY %root_path%\testtools\micromock\src\*.c* %outdir%
COPY %root_path%\testtools\sal\inc\*.h %outdir%
COPY %root_path%\testtools\ctest\inc\*.h %outdir%
COPY %root_path%\testtools\ctest\src\*.c %outdir%

@REM  Copy tests
COPY %tests_path%\main.cpp %outdir%
COPY %tests_path%\*.c %outdir%
COPY %tests_path%\*.h %outdir%
COPY %tests_path%\..\*.cpp %outdir%

@REM  Copy E2E test extras
COPY %root_path%\common\testtools\iothub_test\inc\*.h %outdir%
COPY %root_path%\common\testtools\iothub_test\src\*.c %outdir%

@REM  Copy mbed-specific common code
COPY /Y %root_path%\common\adapters\*mbed.c %outdir%
COPY /Y %root_path%\common\adapters\*mbed.cpp %outdir%

@REM  Copy iothub_client code
COPY %root_path%\iothub_client\src\*.c %outdir%
COPY %root_path%\iothub_client\inc\*.h %outdir%

@REM  Copy serializer code if necessary
if not "%tests_path%"=="%tests_path:serializer=iothub_client%" (
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
DEL /F /Q %outdir%\*http*.*
DEL /F /Q %outdir%\detours*.*
DEL /F /Q %outdir%\*_2*.*
DEL /F /Q %outdir%\*mqtt*.*

@REM  Commit changes and push result back to mbed

echo.
echo Pushing changes to repo '%repo_name%'...

hg addremove
if %errorlevel% neq 0 goto :eof

hg commit -u "%MBED_HG_USER%" -m "Automatic build commit"

hg push --insecure https://%MBED_USER%:%MBED_PWD%@developer.mbed.org/users/%MBED_USER%/code/%repo_name%/

echo.

@REM  Instruct mbed online compiler to build from the repo

%~dp0\..\compilembed\bin\release\compilembed.exe -un %MBED_USER% -pwd %MBED_PWD% -r http://developer.mbed.org/users/%MBED_USER%/code/%repo_name%/ -plat %mbed_plat% -o %mbed_bin_path%
