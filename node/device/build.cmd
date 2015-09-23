@REM Copyright (c) Microsoft. All rights reserved.
@REM Licensed under the MIT license. See LICENSE file in the project root for full license information.

@echo off
setlocal
cd %~dp0
if not exist node_modules\.bin\jake.cmd (
  echo This script depends on packages it cannot find. Please run 'npm install' from %~dp0, then try again.
  echo.
  exit /b 1 
)

node_modules\.bin\jake %*
