#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

#this is CMakeLists.txt for iothubtransportamqp_ut
cmake_minimum_required(VERSION 2.8)

if(NOT ${use_amqp})
	message(FATAL_ERROR "iothubtransportamqp_ut being generated without AMQP support")
endif()

compileAsC99()
set(theseTestsName iothubtransportamqp_ut)

set(${theseTestsName}_cpp_files
${theseTestsName}.cpp
)

set(${theseTestsName}_c_files
../../src/iothubtransportamqp.c
)

set(${theseTestsName}_h_files
)

build_test_artifacts(${theseTestsName} ON)