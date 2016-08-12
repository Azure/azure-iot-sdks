#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

#this is CMakeLists.txt for iothubclient_ut
cmake_minimum_required(VERSION 2.8.11)

compileAsC99()
set(theseTestsName iothub_service_client_auth_ut)
set(${theseTestsName}_cpp_files
${theseTestsName}.cpp
)

set(${theseTestsName}_c_files
../../src/iothub_service_client_auth.c
)

set(${theseTestsName}_h_files
)

include_directories(${SHARED_UTIL_INC_FOLDER})

build_test_artifacts(${theseTestsName} ON)