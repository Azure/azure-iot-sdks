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
set hg_commit_message="Release 1.0.tony"

rmdir /s /q hg 2>nul
mkdir hg
cd hg

echo Using cmake to release '%project_name%'
cmake -DHG_COMMIT_MSG:string=%hg_commit_message% %project_path%\mbedrelease
