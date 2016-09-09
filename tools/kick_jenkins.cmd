@REM Copyright (c) Microsoft. All rights reserved.
@REM Licensed under the MIT license. See LICENSE file in the project root for full license information.

@setlocal EnableExtensions EnableDelayedExpansion
@echo off

set current-path=%~dp0

rem // remove trailing slash
set current-path=%current-path:~0,-1%

set build-root=%current-path%\..
rem // resolve to fully qualified path
for %%i in ("%build-root%") do set build-root=%%~fi

REM check that we have a valid argument
if "%1" equ "" (
  set jenkins_job=_integrate-into-develop
) else if "%1" equ "c" (
  set jenkins_job=_integrate-into-develop-c-and-wrappers
) else if "%1" equ "csharp" (
  set jenkins_job=_integrate-into-develop-csharp
) else if "%1" equ "java" (
  set jenkins_job=_integrate-into-develop-java
) else if "%1" equ "node" (
  set jenkins_job=_integrate-into-develop-node
) else (
  echo Usage:
  echo kick_jenkins
  echo     or
  echo "kick_jenkins c | csharp | java | node"
  exit /b 1
)

REM check that we have java handy
call :checkExists java
if not !ERRORLEVEL!==0 exit /b !ERRORLEVEL!

REM check that jenkins-cli.jar is in the repository's tools folder
if not exist %build-root%\tools\jenkins-cli.jar (
	echo jenkins-cli does not exist
	echo Downloading from repository:
	echo http://azure-iot-sdks-ci.westus.cloudapp.azure.com:8080/jnlpJars/jenkins-cli.jar
	
	powershell -Command "Invoke-WebRequest http://azure-iot-sdks-ci.westus.cloudapp.azure.com:8080/jnlpJars/jenkins-cli.jar -OutFile %build-root%\tools\jenkins-cli.jar"
	
	if not !ERRORLEVEL!==0 (
		echo Failed downloading jenkins-cli.jar
		echo Use the following link to manually download it into the tools folder of your repository:
		echo http://azure-iot-sdks-ci.westus.cloudapp.azure.com:8080/jnlpJars/jenkins-cli.jar
		exit /b 1
	)
)

REM find current branch
for /f "usebackq tokens=*" %%i in (`git symbolic-ref -q HEAD`) do set "current_branch_ref=%%i"
if defined current_branch_ref set "current_branch=%current_branch_ref:refs/heads/=%"

REM handle detached HEAD
if not defined current_branch (
    echo You're not on any branch! Aborting...
    goto :eof
)

REM Must be on a topic branch when running this script
set __exit=1
if not "%current_branch%"=="master" if not "%current_branch%"=="develop" set __exit=0
if %__exit%==1 (
    echo You cannot call this script from 'develop' or 'master'. Change to a topic branch first.  Aborting...
    goto :eof
)

REM find tracking branch
for /f "usebackq tokens=*" %%i in (`git rev-parse --symbolic-full-name --abbrev-ref @{u}`) do set "tracking_branch=%%i"
if not defined tracking_branch (
    echo Branch ^'%current_branch%^' is not tracking a remote branch! First try ^'git branch -u ^<remote^>/^<branch^>^' to set tracking info. Aborting...
    goto :eof
)

for /f "usebackq tokens=*" %%i in (`git config branch.%current_branch%.remote`) do set "remote=%%i"
if not defined remote (
    REM should never happen...
    echo Cannot isolate remote name for tracking branch ^'%tracking_branch%^'! Aborting...
    goto :eof
)

REM get the tracking branch name only e.g., origin/topic -> topic
for /f "usebackq tokens=*" %%i in (`call echo %%tracking_branch:%remote%/^=%%`) do set "tracking_name=%%i"

REM find repo url
for /f "usebackq tokens=*" %%i in (`git ls-remote --heads --get-url %remote%`) do set "repo_url=%%i" 
if not defined repo_url (
    echo Cannot find the remote repository URL! Aborting...
    goto :eof
)

echo ****************************************************************
echo commit_id: %current_branch%
echo repo_url:  %repo_url%
echo remote:    %remote%
echo trackingN: %tracking_name%
echo jenkinJob: %jenkins_job%
echo ****************************************************************


REM kick off the build!
java -jar "%build-root%"\tools\jenkins-cli.jar -s http://azure-iot-sdks-ci.westus.cloudapp.azure.com:8080/ build %jenkins_job% -p COMMIT_ID=%tracking_name% -p AZURE_REPO=%repo_url% -p BRANCH_TO_MERGE_TO=develop -s -v

rem -----------------------------------------------------------------------------
rem -- done
rem -----------------------------------------------------------------------------
goto :eof

rem -----------------------------------------------------------------------------
rem -- helper subroutines
rem -----------------------------------------------------------------------------
:checkExists
where %~1 >nul 2>nul
if not !ERRORLEVEL!==0 (
    echo "%~1" not found. Please make sure that "%~1" is installed and available in the path.
    exit /b !ERRORLEVEL!
)
goto :eof

