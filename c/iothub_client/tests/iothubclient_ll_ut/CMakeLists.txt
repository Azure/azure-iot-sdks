#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

#this is CMakeLists.txt for iothubclient_ll_ut
cmake_minimum_required(VERSION 2.8.11)

compileAsC11()

set(theseTestsName iothubclient_ll_ut)

set(${theseTestsName}_test_files
${theseTestsName}.c
)

set(${theseTestsName}_c_files
../../src/iothub_client_ll.c
real_doublylinkedlist.c
)

set(${theseTestsName}_h_files
)

build_c_test_artifacts(${theseTestsName} ON "tests/UnitTests")