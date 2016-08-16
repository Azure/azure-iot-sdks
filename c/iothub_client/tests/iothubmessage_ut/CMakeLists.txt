#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

#this is CMakeLists.txt for iothubmessage_ut
cmake_minimum_required(VERSION 2.8.11)

compileAsC99()
set(theseTestsName iothubmessage_ut)
set(${theseTestsName}_cpp_files
${theseTestsName}.cpp
)

set(${theseTestsName}_c_files
../../src/iothub_message.c
)

set(${theseTestsName}_h_files
)

build_test_artifacts(${theseTestsName} ON)