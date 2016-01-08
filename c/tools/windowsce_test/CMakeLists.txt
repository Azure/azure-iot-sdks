#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

#this is CMakeLists.txt for iothubtransporthttp_unittests
cmake_minimum_required(VERSION 2.8.11)

if(NOT ${WINCE})
	message(FATAL_ERROR "This application must be built for Windows CE and executed on a Windows CE device")
endif()

compileAsC99()
set(targetName ctestsetup)
set(${targetName}_cpp_files
${targetName}.cpp
)

build_test_artifacts(${targetName} ON)
