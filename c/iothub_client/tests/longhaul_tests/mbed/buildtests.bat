@REM  Copyright (c) Microsoft. All rights reserved.
@REM  Licensed under the MIT license. See LICENSE file in the project root for full license information.

@setlocal EnableExtensions EnableDelayedExpansion
@echo off

for %%i in ("%~dp0..\..\..\..") do set "root_path=%%~fi"

set base=iothubclientlonghaultests

set "tests_path=%~dp0"

set "mbed_bin_path=%~1"

if "%mbed_bin_path%"=="" (
	set "mbed_bin_path=.\%base%.bin"
)

call %root_path%\tools\mbed_build\buildtests.cmd "%tests_path%" "%base%_bld" %base% FRDM-K64F "%mbed_bin_path%"
