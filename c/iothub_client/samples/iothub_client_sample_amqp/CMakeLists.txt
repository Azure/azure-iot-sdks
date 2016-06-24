#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

#this is CMakeLists.txt for iothub_client_sample_amqp

compileAsC99()

if(NOT ${use_amqp})
	message(FATAL_ERROR "iothub_client_sample_amqp being generated without uamqp support")
endif()

set(iothub_client_sample_amqp_c_files
	iothub_client_sample_amqp.c
)

set(iothub_client_sample_amqp_h_files
	iothub_client_sample_amqp.h
)

IF(WIN32)
	#windows needs this define
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	add_definitions(-DGB_MEASURE_MEMORY_FOR_THIS -DGB_DEBUG_ALLOC)
ENDIF(WIN32)

include_directories(.)

add_executable(iothub_client_sample_amqp ${iothub_client_sample_amqp_c_files} ${iothub_client_sample_amqp_h_files})

target_link_libraries(iothub_client_sample_amqp
		iothub_client
		iothub_client_amqp_transport)

linkSharedUtil(iothub_client_sample_amqp)
linkUAMQP(iothub_client_sample_amqp)
