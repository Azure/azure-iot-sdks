#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

#this is CMakeLists.txt for longhaul_tests
cmake_minimum_required(VERSION 2.8.11)

if(NOT ${use_amqp})
	message(FATAL_ERROR "longhaul_tests being generated without amqp support")
endif()

compileAsC99()
set(theseTestsName longhaul_tests)

set(${theseTestsName}_cpp_files
	longhaul_tests.cpp
)

set(${theseTestsName}_c_files
	${SHARED_UTIL_SRC_FOLDER}/crt_abstractions.c
)

set(${theseTestsName}_h_files
)

include_directories(${IOTHUB_TEST_INC_FOLDER})

build_test_artifacts(${theseTestsName} ON)
set_tests_properties(${theseTestsName} PROPERTIES TIMEOUT 1296000)

if(WIN32)
	if(TARGET ${theseTestsName}_dll)
		target_link_libraries(${theseTestsName}_dll
			iothub_test
			iothub_client
			iothub_client_amqp_transport
			aziotsharedutil
			rpcrt4
		)
		linkUAMQP(${theseTestsName}_dll)
	endif()
	
	if(TARGET ${theseTestsName}_exe)
		target_link_libraries(${theseTestsName}_exe
			iothub_test
			iothub_client
			iothub_client_amqp_transport
			aziotsharedutil
			rpcrt4
		)
		linkUAMQP(${theseTestsName}_exe)
	endif()
else()
	if(TARGET ${theseTestsName}_exe)
		target_link_libraries(${theseTestsName}_exe
			iothub_test
			iothub_client
			iothub_client_amqp_transport
			aziotsharedutil
			uuid
		)
		target_link_libraries(${theseTestsName}_exe pthread)
		linkUAMQP(${theseTestsName}_exe)
	endif()
endif()


