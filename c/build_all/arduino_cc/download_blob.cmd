@REM Copyright (c) Microsoft. All rights reserved.
@REM Licensed under the MIT license. See LICENSE file in the project root for full license information.

@setlocal EnableExtensions EnableDelayedExpansion
@echo off

if %AZURE_CONTAINER_SASTOKEN%=="" (
    echo Fatal: Invalid container SAS token.
    echo.
    goto :usage
)
if "%AZURE_CONTAINER_URL%"=="" (
    echo Fatal: Invalid container URL.
    echo.
    goto :usage
)

:args_loop
if "%1" equ "" goto args_done
if "%1" equ "-directory" goto arg_directory
if "%1" equ "-zip_file_name" goto arg_zip_file_name
if "%1" equ "-file" goto arg_zip_file_name
if "%1" equ "-check_path" goto arg_check_path
if "%1" equ "-check" goto arg_check_path
echo Fatal: Unknown paramenter %1.
echo.
goto :usage

:arg_directory
shift
set directory=%1
goto args_continue

:arg_zip_file_name
shift
set zip_file_name=%1
goto args_continue

:arg_check_path
shift
set check_path=%1
goto args_continue

:args_continue
shift
goto args_loop

:usage
@echo Download test artifacts from an Azure Storage Account. The artifact must be a single zip file stored in a Blobs container. This is a simple way to store and retrieve files that you need to execute tests in the cloud that, for some reason, you cannot make it public. For example, one test that needs a dll that belongs to a third part.
@echo.
@echo To use this script, you must:
@echo.
@echo    1) Have an Azure Storage Account (https://azure.microsoft.com/en-us/documentation/articles/storage-create-storage-account/) 
@echo    2) Create a Container to store your artifacts, for example 'myproductcontainer'.
@echo    3) Compress the files and directories, for example 'contosodlls.zip'.
@echo    4) Using Microsoft Azure Storage Explorer (http://storageexplorer.com/), generate a SasToken (Shared Access Signature) for the Container, copy the 'URL' and the 'Query string'.
@echo    5) Set container environment variables:
@echo        a. AZURE_CONTAINER_SASTOKEN="Query string". Use the Query string generated in the step 4 with double quotes to avoid issues with special characters on it.
@echo        b. AZURE_CONTAINER_URL="URL". Got from Microsoft Azure Storage Explorer, it must be something like https://contoso.blob.core.windows.net/myproductcontainer
@echo    6) For each zip file, call download_blob.cmd with the parameters
@echo        a. -file "file name without extension". Required parameter with zip file name
@echo        b. -directory "directory path". Destination directory, if not provided, it will use the current directory.
@echo        c. -check "file or directory path". File or directory to check, if it exists, do not download and unzip the file. If not provided, this script will download and unzip anyway.

exit /b 1

:args_done

if "%zip_file_name%"=="" (
    echo Fatal: Required parameter -file missed.
    echo.
    goto :usage
)

if "%directory%"=="" set directory=%CD%

set sasToken=!AZURE_CONTAINER_SASTOKEN:%%3A=:!
set sasToken=!sasToken:%"=!

set containerUrl=!AZURE_CONTAINER_URL:%"=!
if "%containerUrl:~-1%"=="/" (set containerUrl=%containerUrl:~0,-1%)

mkdir %directory%

set compiler_fullZipName=%directory%\%zip_file_name%.zip

if "%check_path%"=="" goto :download

if exist %directory%\%zip_file_name%\%check_path% (
    echo ***do not download %zip_file_name% because %check_path% already exist.***
    goto :eof
)

:download
if exist %compiler_fullZipName% ( 
    echo ***do not download %zip_file_name% because it already exist, only unzip it.*** 
) else (
    echo Downloading %containerUrl%/%zip_file_name%.zip to %compiler_fullZipName%
    powershell.exe -nologo -noprofile -command "& { iwr '%containerUrl%/%zip_file_name%.zip%sasToken%' -OutFile '%compiler_fullZipName%'; }"
)

echo Uncompressing %compiler_fullZipName% to %directory%
powershell.exe -nologo -noprofile -command "& { Add-Type -A 'System.IO.Compression.FileSystem'; [IO.Compression.ZipFile]::ExtractToDirectory('%compiler_fullZipName%', '%directory%'); }"

echo Deleting %compiler_fullZipName%
del %compiler_fullZipName%
