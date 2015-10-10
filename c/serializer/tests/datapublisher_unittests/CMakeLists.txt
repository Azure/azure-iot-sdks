#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

#this is CMakeLists.txt for datapublisher_unittests
cmake_minimum_required(VERSION 3.0)

compileAsC99()
set(theseTestsName datapublisher_unittests)

set(${theseTestsName}_cpp_files
${theseTestsName}.cpp
)

set(${theseTestsName}_c_files
../../src/datapublisher.c

../../../common/src/gballoc.c
${LOCK_C_FILE}

../../../common/src/crt_abstractions.c
)

set(${theseTestsName}_h_files
)

#datapublisher is special and needs some data type definitions from iothub_client
#but shouldn't...
include_directories(${IOTHUB_CLIENT_INC_FOLDER})

build_test_artifacts(${theseTestsName} ON)