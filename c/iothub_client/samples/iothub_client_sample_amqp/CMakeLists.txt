#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

#this is CMakeLists.txt for iothub_client_sample_amqp

compileAsC99()

set(iothub_client_sample_amqp_c_files
	iothub_client_sample_amqp.c
)

if(WIN32)
	set(iothub_client_sample_amqp_c_files ${iothub_client_sample_amqp_c_files} ./windows/main.c)
else()
	set(iothub_client_sample_amqp_c_files ${iothub_client_sample_amqp_c_files} ./linux/main.c)
endif()

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

linkProton(iothub_client_sample_amqp)

if(WIN32)
	target_link_libraries(iothub_client_sample_amqp common iothub_client iothub_client_amqp_transport)
else()
	#target_link_libraries(iothub_client_sample_amqp pthread common iothub_client iothub_client_amqp_transport)
	target_link_libraries(iothub_client_sample_amqp iothub_client_amqp_transport common iothub_client)
endif()

