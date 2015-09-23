@REM Copyright (c) Microsoft. All rights reserved.
@REM Licensed under the MIT license. See LICENSE file in the project root for full license information.

@setlocal EnableExtensions EnableDelayedExpansion
@echo off

set build-root=%~dp0..\..
cd %build-root%\build\docs

rem -----------------------------------------------------------------------------
rem -- Generate C API docs
rem -----------------------------------------------------------------------------
doxygen

rem -----------------------------------------------------------------------------
rem -- Fix up the line feeds in the generated C docs so that just line
rem -- feed changes aren't flagged as changes by git. We don't care if this
rem -- fails though.
rem -----------------------------------------------------------------------------
echo Fixing line feeds in generated docs...
node fix-crlf.js
