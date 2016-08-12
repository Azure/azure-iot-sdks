#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

#this is CMakeLists.txt for iothubtransporthttp_ut
cmake_minimum_required(VERSION 2.8.11)

if(NOT ${use_http})
	message(FATAL_ERROR "iothubtransporthttp_ut being generated without HTTP support")
endif()

compileAsC99()
set(theseTestsName iothubtransporthttp_ut)
set(${theseTestsName}_cpp_files
${theseTestsName}.cpp
)

set(${theseTestsName}_c_files
../../src/iothubtransporthttp.c
${SHARED_UTIL_SRC_FOLDER}/crt_abstractions.c
)

set(${theseTestsName}_h_files
)

build_test_artifacts(${theseTestsName} ON)