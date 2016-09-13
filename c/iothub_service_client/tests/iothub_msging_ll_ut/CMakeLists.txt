#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

#this is CMakeLists.txt for iothub_messaging_ll_ut
cmake_minimum_required(VERSION 2.8.11)

compileAsC11()

if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /bigobj")
endif()

set(theseTestsName iothub_messaging_ll_ut)

set(${theseTestsName}_test_files
iothub_messaging_ll_ut.c
)

set(${theseTestsName}_c_files
../../src/iothub_messaging_ll.c
)

set(${theseTestsName}_h_files
)

build_c_test_artifacts(${theseTestsName} ON "tests/UnitTests")