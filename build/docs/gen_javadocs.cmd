@REM Copyright (c) Microsoft. All rights reserved.
@REM Licensed under the MIT license. See LICENSE file in the project root for full license information.

@setlocal EnableExtensions EnableDelayedExpansion
@echo off

set build-root=%~dp0..\..

rem -----------------------------------------------------------------------------
rem -- Generate Java docs for Device SDK
rem -----------------------------------------------------------------------------
echo Generating Java Device SDK docs...
cd %build-root%\java\device\iothub-java-client
call mvn -q javadoc:javadoc
if not !errorlevel!==0 (
    echo Generating java docs for iothub-java-client failed.
    exit /b %errorlevel%
)

rem Move the generated docs to %build-root%\java\device\doc\api_reference
set doc-target-dir=%build-root%\java\device\doc\api_reference
echo Copying Java API docs to %doc-target-dir%
if exist %doc-target-dir% rd /s /q %doc-target-dir%
mkdir %doc-target-dir%
xcopy /q /e /y target\site\apidocs %doc-target-dir%

rem -----------------------------------------------------------------------------
rem -- Generate Java docs for Service SDK
rem -----------------------------------------------------------------------------
echo Generating Java Service SDK docs... 
cd %build-root%\java\service\iothub-service-sdk
call mvn -q javadoc:javadoc
if not !errorlevel!==0 (
    echo Generating java docs for iothub-service-sdk failed.
    exit /b !errorlevel!
)

rem Move the generated docs to %build-root%\java\service\doc\api_reference
set doc-target-dir=%build-root%\java\service\doc\api_reference
echo Copying Java API docs to %doc-target-dir%
if exist %doc-target-dir% rd /s /q %doc-target-dir%
mkdir %doc-target-dir%
xcopy /q /e /y target\site\apidocs %doc-target-dir%
