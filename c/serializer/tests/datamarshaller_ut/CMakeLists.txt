#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

#this is CMakeLists.txt for datamarshaller_ut
cmake_minimum_required(VERSION 2.8.11)

compileAsC99()
set(theseTestsName datamarshaller_ut)

set(${theseTestsName}_test_files
${theseTestsName}.c
)

set(${theseTestsName}_c_files
real_parson.c
real_vector.c
real_crt_abstractions.c
real_strings.c
../../src/datamarshaller.c
${LOCK_C_FILE}
)

set(${theseTestsName}_h_files
real_parson.h
real_vector.h
real_crt_abstractions.h
real_strings.h
)

if(MSVC)
    set_source_files_properties(real_parson.c PROPERTIES COMPILE_FLAGS "/wd4244 /wd4232")
endif()


build_c_test_artifacts(${theseTestsName} ON "tests/UnitTests")