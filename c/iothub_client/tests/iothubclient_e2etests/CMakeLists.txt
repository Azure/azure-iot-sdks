#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

#this is CMakeLists.txt for iothubclient_e2etests
cmake_minimum_required(VERSION 3.0)

if(NOT (${use_http} AND ${use_amqp}))
	message(FATAL_ERROR "iothubclient_e2etests being generated without HTTP and AMQP support")
endif()

compileAsC99()
set(theseTestsName iothubclient_e2etests)

set(${theseTestsName}_cpp_files
	${theseTestsName}.cpp
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
	iothub_client_http_transport
	common
	iothub_client_http_transport
)

if(WIN32)
else()
	target_link_libraries(${theseTestsName} pthread)
endif()

linkHttp(${theseTestsName})
linkProton(${theseTestsName})