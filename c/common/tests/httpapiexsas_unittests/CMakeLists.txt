#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

#this is CMakeLists.txt for httpapiexsas_unittests
cmake_minimum_required(VERSION 3.0)

if(NOT ${use_http})
	message(FATAL_ERROR "httpapiexsas_unittests being generated without HTTP support")
endif()

compileAsC99()
set(theseTestsName httpapiexsas_unittests)

set(${theseTestsName}_cpp_files
${theseTestsName}.cpp
)

set(${theseTestsName}_c_files
../../src/httpapiexsas.c
../../src/crt_abstractions.c
)

set(${theseTestsName}_h_files
)

build_test_artifacts(${theseTestsName} ON)


