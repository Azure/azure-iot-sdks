#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

cmake_minimum_required(VERSION 2.8.11)

compileAsC99()
set(theseTestsName methodreturn_ut)

include_directories(${SERIALIZER_INC_FOLDER})

set(${theseTestsName}_test_files
${theseTestsName}.c
)

set(${theseTestsName}_c_files
    ../../src/methodreturn.c
    real_strings.c
)

set(${theseTestsName}_h_files
    real_strings.h
)

build_c_test_artifacts(${theseTestsName} ON "tests/UnitTests")
