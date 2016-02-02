#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

#this is CMakeLists.txt for iothubtransportamqp_unittests
cmake_minimum_required(VERSION 3.0)

if(NOT ${use_uamqp})
	message(FATAL_ERROR "iothubtransportuamqp_unittests being generated without uAMQP support")
endif()

compileAsC99()
set(theseTestsName iothubtransportuamqp_unittests)

set(${theseTestsName}_cpp_files
${theseTestsName}.cpp
)

set(${theseTestsName}_c_files
../../src/iothubtransportuamqp.c
)

set(${theseTestsName}_h_files
)

build_test_artifacts(${theseTestsName} ON)