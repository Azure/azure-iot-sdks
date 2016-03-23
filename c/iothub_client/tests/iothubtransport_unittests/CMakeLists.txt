#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

#this is CMakeLists.txt for iothubtransport_unittests)
cmake_minimum_required(VERSION 2.8.11)

compileAsC99()
set(theseTestsName iothubtransport_unittests)
set(${theseTestsName}_cpp_files
${theseTestsName}.cpp
)

set(${theseTestsName}_c_files
../../src/iothubtransport.c
)

set(${theseTestsName}_h_files
)

build_test_artifacts(${theseTestsName} ON)