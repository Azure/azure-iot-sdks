#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

#this is CMakeLists.txt for codefirst_withstructs_cpp_ut
cmake_minimum_required(VERSION 2.8.11)

compileAsC99()
set(theseTestsName codefirst_withstructs_cpp_ut)
set(${theseTestsName}_test_files
${theseTestsName}.cpp
)

set(${theseTestsName}_c_files
../../src/codefirst.c
${SHARED_UTIL_SRC_FOLDER}/gballoc.c
${LOCK_C_FILE}
)

set(${theseTestsName}_h_files
)

#CPP compiler on windows likes to complain about unused local function removed (C4505)
#C compiler doesn't like to complain about the same thing
if(WIN32)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4505")
endif()

build_c_test_artifacts(${theseTestsName} ON "tests/UnitTests")