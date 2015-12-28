#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

#this is CMakeLists.txt for commanddecoder_unittests
cmake_minimum_required(VERSION 3.0)

compileAsC99()
set(theseTestsName commanddecoder_unittests)

set(${theseTestsName}_cpp_files
${theseTestsName}.cpp
)

set(${theseTestsName}_c_files
../../src/commanddecoder.c
)

set(${theseTestsName}_h_files
)

build_test_artifacts(${theseTestsName} ON)