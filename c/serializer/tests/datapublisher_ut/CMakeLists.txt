#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

#this is CMakeLists.txt for datapublisher_ut
cmake_minimum_required(VERSION 2.8.11)

compileAsC99()
set(theseTestsName datapublisher_ut)

set(${theseTestsName}_test_files
${theseTestsName}.c
)

set(${theseTestsName}_c_files
../../src/datapublisher.c
)

set(${theseTestsName}_h_files
)

#datapublisher is special and needs some data type definitions from iothub_client
#but shouldn't...
include_directories(${IOTHUB_CLIENT_INC_FOLDER})

build_c_test_artifacts(${theseTestsName} ON "tests/UnitTests")