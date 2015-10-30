@REM  Copyright (c) Microsoft. All rights reserved.
@REM  Licensed under the MIT license. See LICENSE file in the project root for full license information.

setlocal

@REM  Script arguments:
@REM  %1 Name of the project to release
@REM  %2 Local path to the project to be released.
@REM  %3 File list for the project that is being released
@REM  %4 Platform to build for

ECHO Releasing project %1

set project_name=%1
set project_path=%2
set filelist=%project_path%\%3
set mbed_plat=%4

rmdir /s /q hg 2>nul
mkdir hg
cd hg

echo.
echo Cloning repo '%project_name%'...

set mbed_creds_defined=false
if defined MBED_HG_USER if defined MBED_USER if defined MBED_PWD set mbed_creds_defined=true
if not "%mbed_creds_defined%"=="true" (
    echo ** MBED_HG_USER, MBED_USER, or MBED_PWD not defined in your environment **
    exit /b 1
)

hg clone --insecure https://%MBED_USER%:%MBED_PWD%@developer.mbed.org/users/%MBED_USER%/code/%project_name%/
if %errorlevel% neq 0 goto :eof

md temp

cd ..

copy hg\%project_name%\*.lib hg\temp\*.lib
copy hg\%project_name%\*.bld hg\temp\*.bld
del /q hg\%project_name%\*.*

echo Copying files ...

for /f "tokens=*" %%i in (%filelist%) DO (
    xcopy /y "%project_path%\%%i" "hg\%project_name%"
    if %errorlevel% neq 0 goto :eof
)
echo.

copy /y hg\temp\*.lib hg\%project_name%\*.lib
copy /y hg\temp\*.bld hg\%project_name%\*.bld
rmdir /q /s hg\temp

cd hg\%project_name%
if %errorlevel% neq 0 goto :eof

echo Pushing changes to repo '%project_name%'...

hg addremove
if %errorlevel% neq 0 goto :eof

hg commit -u "%MBED_HG_USER%" -m "v1.0.0-preview.3"

hg push --insecure https://%MBED_USER%:%MBED_PWD%@developer.mbed.org/users/%MBED_USER%/code/%project_name%/

@REM  Instruct mbed online compiler to build from the repo

if not defined mbed_plat goto :eof
if %mbed_plat%=="" goto :eof

echo ..\..\..\c\tools\compilembed\bin\release\compilembed.exe -un %MBED_USER% -pwd **** -r http://developer.mbed.org/users/%MBED_USER%/code/%project_name%/ -plat %mbed_plat%
..\..\..\c\tools\compilembed\bin\release\compilembed.exe -un %MBED_USER% -pwd %MBED_PWD% -r http://developer.mbed.org/users/%MBED_USER%/code/%project_name%/ -plat %mbed_plat%
