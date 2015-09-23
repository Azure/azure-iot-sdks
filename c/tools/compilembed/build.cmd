@setlocal EnableExtensions EnableDelayedExpansion
@echo off

set "current-path=%~dp0"
set build-clean=0

if "%2" neq "" call :usage && exit /b 1
if "%1" equ "" goto args-done
if "%1" equ "-c" goto arg-build-clean
if "%1" equ "--clean" goto arg-build-clean
call :usage && exit /b 1

:arg-build-clean
set build-clean=1

:args-done

set "msbuild-args=/m "/p:Configuration=Release;Platform=Any CPU" "%current-path%\compilembed.sln""

if %build-clean%==1 (
    msbuild /t:Clean %msbuild-args%
    if not %errorlevel%==0 exit /b %errorlevel%
)

msbuild %msbuild-args%
if not %errorlevel%==0 exit /b %errorlevel%

goto :eof

:usage
echo Build compilembed.exe. Usage:
echo   build.cmd [options]
echo   options:
echo     -c, --clean           delete artifacts from previous build before building
goto :eof
