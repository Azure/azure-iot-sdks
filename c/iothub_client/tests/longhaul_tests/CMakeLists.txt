#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

#this is CMakeLists.txt for longhaul_tests

cmake_minimum_required(VERSION 2.8.11)

if(NOT ${use_amqp})
	message(FATAL_ERROR "longhaul_tests being generated without amqp support")
endif()

compileAsC99()

set(longhaul_tests_c_files
	main.c
	longhaul_tests.c
)

set(longhaul_tests_cpp_files
)

set(longhaul_tests_h_files
	longhaul_tests.h
)

IF(WIN32)
	#windows needs this define
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	add_definitions(-DGB_MEASURE_MEMORY_FOR_THIS -DGB_DEBUG_ALLOC)
ENDIF(WIN32)

include_directories(${IOTHUB_TEST_INC_FOLDER})

set(sharedutil_include_directories ${SAL_INC_FOLDER} ${SHARED_UTIL_INC_FOLDER} ${SHARED_UTIL_SRC_FOLDER})
if(WIN32)
else()
	include_directories(${sharedutil_include_directories})
endif()

add_executable(longhaul_tests ${longhaul_tests_c_files} ${longhaul_tests_cpp_files} ${longhaul_tests_h_files})

if(WIN32)
	target_link_libraries(
		longhaul_tests
		iothub_test
		iothub_client
		iothub_client_amqp_transport
		aziotsharedutil
		rpcrt4)
else()
	target_link_libraries(
		longhaul_tests
		iothub_test
		iothub_client
		iothub_client_amqp_transport
		aziotsharedutil
		pthread 
		uuid
		m)
endif()
	
linkSharedUtil(longhaul_tests)
linkUAMQP(longhaul_tests)

add_test(NAME longhaul_tests COMMAND $<TARGET_FILE:longhaul_tests>)
set_tests_properties(longhaul_tests PROPERTIES TIMEOUT 86400)
