#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

#this is CMakeLists.txt for iothub_client_sample_amqp_websockets

compileAsC99()

if(NOT ${use_amqp})
	message(FATAL_ERROR "iothub_client_sample_amqp_websockets being generated without amqp support")
endif()

set(iothub_client_sample_amqp_websockets_c_files
	iothub_client_sample_amqp_websockets.c
	${CMAKE_CURRENT_SOURCE_DIR}/../../../certs/certs.c
)

if(WIN32)
	set(iothub_client_sample_amqp_websockets_c_files ${iothub_client_sample_amqp_websockets_c_files} ./windows/main.c)
else()
	set(iothub_client_sample_amqp_websockets_c_files ${iothub_client_sample_amqp_websockets_c_files} ./linux/main.c)
endif()

set(iothub_client_sample_amqp_websockets_h_files
	iothub_client_sample_amqp_websockets.h
	${CMAKE_CURRENT_SOURCE_DIR}/../../../certs/certs.h
)

IF(WIN32)
	#windows needs this define
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	add_definitions(-DGB_MEASURE_MEMORY_FOR_THIS -DGB_DEBUG_ALLOC)
ENDIF(WIN32)

include_directories(.)

add_executable(iothub_client_sample_amqp_websockets ${iothub_client_sample_amqp_websockets_c_files} ${iothub_client_sample_amqp_websockets_h_files})

target_link_libraries(iothub_client_sample_amqp_websockets
		iothub_client
		iothub_client_amqp_ws_transport)

linkSharedUtil(iothub_client_sample_amqp_websockets)
linkUAMQP(iothub_client_sample_amqp_websockets)
