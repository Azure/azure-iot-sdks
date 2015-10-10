#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

#this is CMakeLists.txt for httpapiex_unittests
cmake_minimum_required(VERSION 3.0)

if(NOT ${use_http})
	message(FATAL_ERROR "httpapiex_unittests being generated without HTTP support")
endif()

compileAsC99()
set(theseTestsName httpapiex_unittests )

set(${theseTestsName}_cpp_files
${theseTestsName}.cpp
)

set(${theseTestsName}_c_files
../../src/httpapiex.c
../../src/crt_abstractions.c
)

set(${theseTestsName}_h_files
)

build_test_artifacts(${theseTestsName} ON)