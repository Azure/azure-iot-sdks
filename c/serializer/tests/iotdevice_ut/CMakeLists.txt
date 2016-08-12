#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

#this is CMakeLists.txt for iotdevice_ut
cmake_minimum_required(VERSION 2.8.11)

compileAsC99()
set(theseTestsName iotdevice_ut)

set(${theseTestsName}_cpp_files
${theseTestsName}.cpp
)

set(${theseTestsName}_c_files
../../src/iotdevice.c
${SHARED_UTIL_SRC_FOLDER}/gballoc.c
${LOCK_C_FILE}
${SHARED_UTIL_SRC_FOLDER}/crt_abstractions.c
)

build_test_artifacts(${theseTestsName} ON)