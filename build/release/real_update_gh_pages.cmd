@REM Copyright (c) Microsoft. All rights reserved.
@REM Licensed under the MIT license. See LICENSE file in the project root for full license information.

@setlocal EnableExtensions EnableDelayedExpansion
@echo off

set build-root=%1
cd %build-root%

rem -----------------------------------------------------------------------------
rem -- Make sure current git branch is 'develop'. If it isn't check out that
rem -- branch.
rem -----------------------------------------------------------------------------
for /f "delims=" %%i in ('git rev-parse --abbrev-ref HEAD') do set current_git_branch=%%i
if "%current_git_branch%" neq "develop" (
	echo Switching git branch to 'develop'
	git checkout develop
	if %errorlevel% neq 0 exit /b %errorlevel%
)

rem -----------------------------------------------------------------------------
rem -- Call gen_docs.cmd to generate API docs afresh
rem -----------------------------------------------------------------------------
call %build-root%\build\gen_docs.cmd

rem -----------------------------------------------------------------------------
rem -- First, copy all the docs out to a temp folder.
rem -----------------------------------------------------------------------------
set temp_doc_dir=%temp%\azure-iot-sdks
if exist %temp_doc_dir% rd /s /q %temp_doc_dir%
if %errorlevel% neq 0 exit /b %errorlevel%
mkdir %temp_doc_dir%
if %errorlevel% neq 0 exit /b %errorlevel%

rem -----------------------------------------------------------------------------
rem -- Copy C SDK API docs
rem -----------------------------------------------------------------------------
mkdir %temp_doc_dir%\c\api_reference
xcopy /q /e /y c\doc\api_reference\html %temp_doc_dir%\c\api_reference
if %errorlevel% neq 0 exit /b %errorlevel%

rem -----------------------------------------------------------------------------
rem -- Copy Java Device SDK API docs
rem -----------------------------------------------------------------------------
mkdir %temp_doc_dir%\java\api_reference
xcopy /q /e /y java\device\doc\api_reference %temp_doc_dir%\java\api_reference
if %errorlevel% neq 0 exit /b %errorlevel%

rem -----------------------------------------------------------------------------
rem -- Copy Node.js Device & Service SDK API docs
rem -----------------------------------------------------------------------------
mkdir %temp_doc_dir%\node\api_reference
xcopy /q /e /y node\out %temp_doc_dir%\node\api_reference
if %errorlevel% neq 0 exit /b %errorlevel%

rem -----------------------------------------------------------------------------
rem -- Checkout gh-pages branch
rem -----------------------------------------------------------------------------
git checkout gh-pages
if %errorlevel% neq 0 exit /b %errorlevel%

rem -----------------------------------------------------------------------------
rem -- Copy the docs over from temp to current folder
rem -----------------------------------------------------------------------------
rd /s /q %build-root%\c
rd /s /q %build-root%\java
rd /s /q %build-root%\node
xcopy /e /y %temp_doc_dir% %build-root%
if %errorlevel% neq 0 exit /b %errorlevel%

echo Docs have been updated in gh-pages. You're in gh-pages branch now. Review the changes
echo and commit them if everything looks good.
