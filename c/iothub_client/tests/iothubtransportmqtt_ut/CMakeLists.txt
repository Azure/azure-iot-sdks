#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

cmake_minimum_required(VERSION 2.8.11)

if(NOT ${use_mqtt})
	message(FATAL_ERROR "iothubtransportmqtt_ws_ut being generated without mqtt support")
endif()

set(theseTestsName iothubtransportmqtt_ut)

set(${theseTestsName}_test_files
${theseTestsName}.c
)

set(${theseTestsName}_c_files
../../src/iothubtransportmqtt.c
)

set(${theseTestsName}_h_files
)

build_c_test_artifacts(${theseTestsName} ON "tests/UnitTests")
