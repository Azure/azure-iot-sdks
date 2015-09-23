@REM  Copyright (c) Microsoft. All rights reserved.
@REM  Licensed under the MIT license. See LICENSE file in the project root for full license information.

@REM  Script arguments:
@REM  %1 Dependency name (e.g., "My Dependency")
@REM  %2 Dependency file (e.g., mydep.exe)
@REM  %3 Path to dependency's default install directory (e.g., %ProgramFiles%\mydep)

echo.
echo Looking for dependency "%~1"...

@REM  Use where.exe to find dependency in PATH
where %~2 2>nul
if %ERRORLEVEL% equ 0 goto :eof

@REM  Didn't find dependency in PATH; look in the default install path
path %~3;%PATH%
where %~2 2>nul
if %ERRORLEVEL% equ 0 goto :eof

echo ** Didn't find dependency "%~1" **
exit /b 1
