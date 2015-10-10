#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

#this is CMakeLists.txt for longhaul_tests
cmake_minimum_required(VERSION 3.0)

if(NOT ${use_amqp})
	message(FATAL_ERROR "iothubtransportamqp_unittests being generated without amqp support")
endif()

compileAsC99()
set(theseTestsName longhaul_tests)

set(${theseTestsName}_cpp_files
longhaul_tests.cpp
)

set(${theseTestsName}_c_files

)

set(${theseTestsName}_h_files
)

include_directories(${IOTHUB_TEST_INC_FOLDER})

build_test_artifacts(${theseTestsName} ON)

target_link_libraries(${theseTestsName}
	iothub_test
	iothub_client
	iothub_client_amqp_transport
	common)

if(WIN32)
else()
	target_link_libraries(${theseTestsName} pthread)
endif()

linkProton(${theseTestsName})