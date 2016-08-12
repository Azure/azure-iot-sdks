#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

#this is CMakeLists.txt for codefirst_cpp_ut
cmake_minimum_required(VERSION 2.8.11)

compileAsC99()
set(theseTestsName codefirst_cpp_ut)

set(${theseTestsName}_cpp_files
${theseTestsName}.cpp
)

set(${theseTestsName}_c_files
    ../../src/codefirst.c
    ./c_bool_size.c
    ${SHARED_UTIL_SRC_FOLDER}/gballoc.c
    ${LOCK_C_FILE}
)

set(${theseTestsName}_h_files
    ./c_bool_size.h
)

build_c_test_artifacts(${theseTestsName} ON "UnitTests")
