@REM Copyright (c) Microsoft. All rights reserved.
@REM Licensed under the MIT license. See LICENSE file in the project root for full license information.

@setlocal EnableExtensions EnableDelayedExpansion
@echo off

set build-root=%~dp0..\..
cd %build-root%\node

rem -----------------------------------------------------------------------------
rem -- Generate JS docs
rem -----------------------------------------------------------------------------
set jsdoc=node_modules\.bin\jsdoc.cmd
if not exist %jsdoc% (
	echo "jsdoc" was not found. Please make sure you've run "npm install" in %build-root%\node.
	exit /b 1
)

%jsdoc% -c ..\build\docs\jsdoc.conf.json -d apidocs