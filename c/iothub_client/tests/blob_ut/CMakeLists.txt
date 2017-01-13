#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

#this is CMakeLists.txt for blob_ut
cmake_minimum_required(VERSION 2.8.11)

compileAsC11()

add_definitions(-DGB_MEASURE_MEMORY_FOR_THIS -DGB_DEBUG_ALLOC)

set(theseTestsName blob_ut )

set(${theseTestsName}_test_files
${theseTestsName}.c
)

set(${theseTestsName}_c_files
    ../../src/blob.c
)

set(${theseTestsName}_h_files
)

build_c_test_artifacts(${theseTestsName} OFF "tests/UnitTests")
