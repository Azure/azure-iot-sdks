#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

#this is CMakeLists.txt for iothubtransportmqtt_ut
cmake_minimum_required(VERSION 2.8.11)

if(NOT ${use_mqtt})
	message(FATAL_ERROR "iothubtransportmqtt_ut being generated without mqtt support")
endif()

compileAsC99()
set(theseTestsName iothubtransportmqtt_ut)

set(${theseTestsName}_cpp_files
${theseTestsName}.cpp
)

set(${theseTestsName}_c_files
../../src/iothubtransportmqtt.c
)

set(${theseTestsName}_h_files
)

build_test_artifacts(${theseTestsName} ON)
