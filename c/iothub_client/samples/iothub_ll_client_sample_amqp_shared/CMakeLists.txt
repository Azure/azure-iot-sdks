#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

#this is CMakeLists.txt for iothub_ll_client_sample_amqp_shared

compileAsC99()

if(NOT ${use_amqp})
	message(FATAL_ERROR "iothub_ll_client_sample_amqp_shared being generated without uamqp support")
endif()

set(iothub_ll_client_sample_amqp_shared_c_files
	iothub_ll_client_sample_amqp_shared.c
)

set(iothub_ll_client_sample_amqp_shared_h_files
	iothub_ll_client_sample_amqp_shared.h
)

IF(WIN32)
	#windows needs this define
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	add_definitions(-DGB_MEASURE_MEMORY_FOR_THIS -DGB_DEBUG_ALLOC)
ENDIF(WIN32)

include_directories(.)

add_executable(iothub_ll_client_sample_amqp_shared ${iothub_ll_client_sample_amqp_shared_c_files} ${iothub_ll_client_sample_amqp_shared_h_files})

target_link_libraries(iothub_ll_client_sample_amqp_shared
		iothub_client
		iothub_client_amqp_transport)

linkSharedUtil(iothub_ll_client_sample_amqp_shared)
linkUAMQP(iothub_ll_client_sample_amqp_shared)
