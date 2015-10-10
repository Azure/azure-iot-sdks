#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

#this is CMakeLists.txt for serializer_e2etests
cmake_minimum_required(VERSION 3.0)

if(NOT (${use_amqp} AND  ${use_http}) )
	message(FATAL_ERROR "serializer_e2etests being generated without amqp or http support")
endif()

compileAsC99()
set(theseTestsName serializer_e2etests)

set(${theseTestsName}_cpp_files
	${theseTestsName}.cpp
)

set(${theseTestsName}_c_files
)

set(${theseTestsName}_h_files
	MacroE2EModelAction.h
)

include_directories(${IOTHUB_TEST_INC_FOLDER} ${IOTHUB_CLIENT_INC_FOLDER} ${SERIALIZER_INC_FOLDER})

build_test_artifacts(${theseTestsName} ON)

target_link_libraries(${theseTestsName}
	serializer
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


linkProton(${theseTestsName})
linkHttp(${theseTestsName})