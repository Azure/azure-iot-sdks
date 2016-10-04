#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

#this is CMakeLists.txt for iothub_devicemethod_ut
cmake_minimum_required(VERSION 2.8.11)

compileAsC11()

set(theseTestsName iothub_devicemethod_ut)

set(${theseTestsName}_test_files
iothub_devicemethod_ut.c
)


set(${theseTestsName}_c_files
../../src/iothub_devicemethod.c
)

set(${theseTestsName}_h_files
)

build_c_test_artifacts(${theseTestsName} ON "tests/UnitTests")
