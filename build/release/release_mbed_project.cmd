@REM  Copyright (c) Microsoft. All rights reserved.
@REM  Licensed under the MIT license. See LICENSE file in the project root for full license information.

setlocal

@REM  Script arguments:
@REM  %1 Local path to the project to be released.

set project_path=%1
set hg_commit_message="1.0.11"
set project_name_override=""

if  NOT "%2" equ "" (
set project_name_override="-Dmbed_repo_name:string=%2"
)

rmdir /s /q hg 2>nul
mkdir hg
cd hg

cmake -DHG_COMMIT_MSG:string=%hg_commit_message% -Drelease_the_project:bool=ON %project_name_override% %project_path%\mbed
