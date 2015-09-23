@setlocal EnableExtensions EnableDelayedExpansion
@echo off

if not defined PAHO_PATH goto :errorPAHOPATHnotDefined
if not defined OpenSSLDir goto :errorOPenSSLDirnotDefined

goto :continueWithInstallation


:errorPAHOPATHnotDefined
@Echo PAHO_PATH not defined. Please, Define PAHO_PATH on Environment Variables, restart command prompt and the script.
goto :eof

:errorOPenSSLDirnotDefined
@Echo OpenSSLDir not defined. Please, Define OpenSSLDir on Environment Variables and restart the script.
goto :eof

:continueWithInstallation
rem -----------------------------------------------------------------------------
rem -- use OpenSSLDir
rem -----------------------------------------------------------------------------
set openSSL-root=%OpenSSLDir%
set openSSL-repo=https://github.com/openssl/openssl.git
set openSSL-build-root=%openSSL-root%
set openSSL-branch=OpenSSL_1_0_1-stable

rem -----------------------------------------------------------------------------
rem -- use PAHO_PATH
rem -----------------------------------------------------------------------------

set paho-root=%PAHO_PATH%
set paho-repo=https://git.eclipse.org/r/paho/org.eclipse.paho.mqtt.c.git
set package-root=%~dp0\..\packaging\windows
set paho-build-root=%paho-root%\org.eclipse.paho.mqtt.c


rem -----------------------------------------------------------------------------
rem -- Make sure they know this is a third party component
rem -----------------------------------------------------------------------------

echo OpenSSL will be located at %openSSL-root%
echo.
echo Paho will be located at %paho-root%
echo.

@Echo Azure IoT SDK has a dependency on OpenSSL and Paho MQTT
@Echo https://github.com/openssl/openssl/blob/OpenSSL_1_0_1-stable/LICENSE
@Echo http://git.eclipse.org/c/paho/org.eclipse.paho.mqtt.c.git/tree/notice.html
choice /C yn /M "Do you want to install these components?" 
if not %errorlevel%==1 goto :eof

rem -----------------------------------------------------------------------------
rem -Check Perl Version
@Echo This is your version of Perl. We require version 5.20 or higher.
perl -v
perl -v | findstr /i "(v5\.2*" > nul
IF %ERRORLEVEL% EQU 1 goto :wrongPerlVersion
goto :continueInstallingOpenSSL
:wrongPerlVersion
@Echo Seems like you have a wrong Perl Version. 
@Echo Check your perl installation and make sure you install version 5.20 or higher and restart the script.
@Echo If you have multiple perl versions, make sure you set a supported one first on the PATH environment variable.
goto :eof
rem -----------------------------------------------------------------------------

rem -----------------------------------------------------------------------------
rem -- sync the OpenSSL source code
rem -----------------------------------------------------------------------------
:continueInstallingOpenSSL
if Exist %openSSL-root% rmdir %openSSL-root% /s /q
git clone -b %openSSL-branch% %openSSL-repo% %openSSL-build-root%
if not %errorlevel%==0 exit /b %errorlevel%

rem -----------------------------------------------------------------------------
rem -- build OpenSSL
rem -----------------------------------------------------------------------------

call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" x86 
if not %errorlevel%==0 exit /b %errorlevel%

pushd %openSSL-build-root%
perl Configure VC-WIN32 no-asm --prefix=%openSSL-build-root%
if not %errorlevel%==0 exit /b %errorlevel%

call ms\do_ms.bat
if not %errorlevel%==0 exit /b %errorlevel%

call nmake -f ms\ntdll.mak
if not %errorlevel%==0 exit /b %errorlevel%

call nmake -f ms\ntdll.mak install
if not %errorlevel%==0 exit /b %errorlevel%

rem -----------------------------------------------------------------------------
rem -- sync the paho source code
rem -----------------------------------------------------------------------------

if Exist %paho-root% rmdir %paho-root% /s /q
git clone %paho-repo% %paho-build-root%
if not %errorlevel%==0 exit /b %errorlevel%

rem -----------------------------------------------------------------------------
rem -- build paho
rem -----------------------------------------------------------------------------

pushd %paho-build-root%
if exist "%programfiles(x86)%\MSBuild\14.0\\." (
msbuild ".\Windows Build\paho-mqtt3cs\paho-mqtt3cs.vcxproj" /p:Configuration=Debug;PlatformToolset=v140 
goto paho_build_done
)
if exist "%programfiles(x86)%\MSBuild\12.0\\." (
msbuild ".\Windows Build\paho-mqtt3cs\paho-mqtt3cs.vcxproj" /p:Configuration=Debug
goto paho_build_done
)
@Echo Paho MQTT needs Visual Studio 2013 or higher in order to build.
exit /b 1
:paho_build_done
if not %errorlevel%==0 exit /b %errorlevel%
