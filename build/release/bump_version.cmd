@REM Copyright (c) Microsoft. All rights reserved.
@REM Licensed under the MIT license. See LICENSE file in the project root for full license information.

@setlocal EnableExtensions EnableDelayedExpansion
@echo off

set build-root=%~dp0..

rem -----------------------------------------------------------------------------
rem -- Update link to DeviceExplorer MSI in tools\DeviceExplorer\doc\how_to_use_device_explorer.md
rem -----------------------------------------------------------------------------
rem node update_de_link.js %SDK_VERSION%

rem -----------------------------------------------------------------------------
rem -- Update version string in build\docs\Doxyfile
rem -----------------------------------------------------------------------------
rem node update_doxyfile.js %SDK_VERSION%
