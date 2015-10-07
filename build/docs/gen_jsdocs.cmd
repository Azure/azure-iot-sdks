@REM Copyright (c) Microsoft. All rights reserved.
@REM Licensed under the MIT license. See LICENSE file in the project root for full license information.

@setlocal EnableExtensions EnableDelayedExpansion
@echo off

set build-root=%~dp0..\..
cd %build-root%\node

rem -----------------------------------------------------------------------------
rem -- Generate device JS docs
rem -----------------------------------------------------------------------------
set doc-target-dir=%build-root%\node\device\doc\api_reference
if exist %doc-target-dir% rd /s /q %doc-target-dir%
mkdir %doc-target-dir%
jsdoc -c ..\build\docs\jsdoc-device.conf.json -d %doc-target-dir%

