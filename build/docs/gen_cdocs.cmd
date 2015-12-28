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

