@REM Copyright (c) Microsoft. All rights reserved.
@REM Licensed under the MIT license. See LICENSE file in the project root for full license information.

@setlocal EnableExtensions EnableDelayedExpansion
@echo off

@rem  // default options
set user-name=%username%
set machine-name=%computername%

:args-loop
if "%1" equ "" goto args-done
if "%1" equ "--user" goto arg-user-name
if "%1" equ "--machine" goto arg-machine-name
goto usage

:arg-user-name
shift
if "%1" equ "" goto usage
set user-name=%1
goto args-continue

:arg-machine-name
shift
if "%1" equ "" goto usage
set machine-name=%1
goto args-continue

:args-continue
shift
goto args-loop

:args-done
runas /noprofile /savecred /user:%machine-name%\%user-name% "REG ADD HKEY_CURRENT_USER\Software\Microsoft\VisualStudio\12.0_Config\MSBuild /v EnableOutOfProcBuild /t REG_DWORD /d 0 /f"
runas /noprofile /savecred /user:%machine-name%\%user-name% "REG ADD HKEY_CURRENT_USER\Software\Microsoft\VisualStudio\14.0_Config\MSBuild /v EnableOutOfProcBuild /t REG_DWORD /d 0 /f"
goto :eof

:usage
echo enable_outofproc_msbuild.cmd [options]
echo options:
echo  --user jenkins    Optional argument, %user-name% is used by dafault
echo  --machine win02   Optional argument, %computername% is used by dafault 
exit /b 1