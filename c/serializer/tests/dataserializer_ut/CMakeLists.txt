#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

#this is CMakeLists.txt for dataserializer_ut
cmake_minimum_required(VERSION 2.8.11)

compileAsC99()
set(theseTestsName dataserializer_ut)

set(${theseTestsName}_cpp_files
${theseTestsName}.cpp
)

set(${theseTestsName}_c_files
../../src/dataserializer.c
${SHARED_UTIL_SRC_FOLDER}/gballoc.c
${LOCK_C_FILE}
)

set(${theseTestsName}_h_files
)

