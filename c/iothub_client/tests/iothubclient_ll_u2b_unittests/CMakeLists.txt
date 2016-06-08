#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

#this is CMakeLists.txt for iothub_client_ll_uploadtoblob_unittests
cmake_minimum_required(VERSION 2.8.11)

if(NOT ${use_http})
	message(FATAL_ERROR "iothub_client_ll_uploadtoblob being generated without HTTP support")
endif()

compileAsC11()
set(theseTestsName iothub_client_ll_u2b_unittests )

set(${theseTestsName}_test_files
${theseTestsName}.c
)

set(${theseTestsName}_c_files
../../src/iothub_client_ll_uploadtoblob.c
)

set(${theseTestsName}_h_files
)

build_c_test_artifacts(${theseTestsName} ON "UnitTests")