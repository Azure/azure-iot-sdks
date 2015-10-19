@REM Copyright (c) Microsoft. All rights reserved.
@REM Licensed under the MIT license. See LICENSE file in the project root for full license information.

@setlocal EnableExtensions EnableDelayedExpansion
@echo off

set build-root=%~dp0..\..
cd %build-root%

rem -----------------------------------------------------------------------------
rem -- While checking out branches below this batch file can get nuked. So
rem -- we copy real_update_gh_pages.cmd to a temp location and run that script
rem -- instead.
rem -----------------------------------------------------------------------------
set update_script=%temp%\real_update_gh_pages.cmd
if exist %update_script% del %update_script%
copy build\release\real_update_gh_pages.cmd %update_script%
%update_script% "%build-root%"
