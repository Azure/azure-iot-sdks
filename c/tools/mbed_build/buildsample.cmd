@REM  Copyright (c) Microsoft. All rights reserved.
@REM  Licensed under the MIT license. See LICENSE file in the project root for full license information.

setlocal

@REM  Script arguments:
@REM  %1 Local path to sample code directory. This script assumes other source code dependencies are in specific
@REM       locations relative to this directory.
@REM  %2 Local path to zip file containing source code to compile. Use mkmbedzip.cmd tool to create it.
@REM  %3 Name of Mercurial repository at http://developer.mbed.org/users/<user>/code/.
@REM  %4 Name of root folder in repository. This is where the source code lives.
@REM  %5 Name of mbed platform to pass to compiler

set "sample_path=%~1"
set "zip_path=%~2"
set "repo_name=%~3"
set "folder_name=%~4"
set "mbed_plat=%~5"

if not exist "%sample_path%" (
    echo ** "%sample_path%" not found **
    goto :eof
)

if not exist "%zip_path%" (
    echo ** "%zip_path%" not found **
    goto :eof
)

@REM  Verify our dependencies

call "%~dp0\checkdep.cmd" 7-Zip 7z.exe "%ProgramFiles%\7-Zip"
if %errorlevel% neq 0 goto :eof

call "%~dp0\checkdep.cmd" Mercurial hg.exe "%ProgramFiles%\TortoiseHG"
if %errorlevel% neq 0 goto :eof

@REM  Build compilembed.exe (practically a no-op if it already exists)

call "%~dp0\..\compilembed\build.cmd"
if %errorlevel% neq 0 goto :eof

@REM  Clone the mbed repo

cd "%sample_path%"

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

@REM  Replace repo contents with files from zip archive

cd %repo_name%\%folder_name%
if %errorlevel% neq 0 goto :eof

del /q *.*
if %errorlevel% neq 0 goto :eof

echo.
echo Overwriting repository files with zipped files...

call 7z x "%zip_path%" *.* -y
if %errorlevel% neq 0 goto :eof

@REM  Commit changes and push result back to mbed

echo.
echo Pushing changes to repo '%repo_name%'...

hg addremove
if %errorlevel% neq 0 goto :eof

hg commit -u "%MBED_HG_USER%" -m "Automatic build commit"

hg push --insecure https://%MBED_USER%:%MBED_PWD%@developer.mbed.org/users/%MBED_USER%/code/%repo_name%/

echo.

@REM  Instruct mbed online compiler to build from the repo

echo %~dp0\..\compilembed\bin\release\compilembed.exe -un %MBED_USER% -pwd **** -r http://developer.mbed.org/users/%MBED_USER%/code/%repo_name%/ -plat %mbed_plat%
%~dp0\..\compilembed\bin\release\compilembed.exe -un %MBED_USER% -pwd %MBED_PWD% -r http://developer.mbed.org/users/%MBED_USER%/code/%repo_name%/ -plat %mbed_plat%
