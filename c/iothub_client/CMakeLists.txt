#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

cmake_minimum_required(VERSION 2.8.11)
#this is CMakeLists for iothub_client

compileAsC99()

set(iothub_client_ll_transport_c_files
./src/version.c
./src/iothub_message.c
./src/iothub_client_ll.c
)

set(iothub_client_ll_transport_h_files
./inc/iothub_message.h
./inc/iothub_client_ll.h
./inc/version.h
)

set(iothub_client_c_files
./src/iothub_client.c
./src/version.c
)

set(iothub_client_h_files
./inc/iothub_client.h
./inc/version.h
)

if(${use_http})
	set(iothub_client_http_transport_c_files
		${iothub_client_ll_transport_c_files}
		./src/iothubtransporthttp.c
	)

	set(iothub_client_http_transport_h_files
		${iothub_client_ll_transport_h_files}
		./inc/iothubtransporthttp.h
		./inc/iothub_client_private.h
	)
endif()

if(${use_amqp})
	set(iothub_client_amqp_transport_c_files
		${iothub_client_ll_transport_c_files}
		./src/iothubtransportamqp.c
	)

	set(iothub_client_amqp_transport_h_files
		${iothub_client_ll_transport_h_files}
		./inc/iothubtransportamqp.h
	)
endif()

if(${use_mqtt})
	set(iothub_client_mqtt_transport_c_files
		${iothub_client_ll_transport_c_files}
		./src/iothubtransportmqtt.c
	)
	
	set(iothub_client_mqtt_transport_h_files
		${iothub_client_ll_transport_h_files}
		./inc/iothubtransportmqtt.h
	)
endif()


#these are the include folders
#the following "set" statetement exports across the project a global variable called SHARED_UTIL_INC_FOLDER that expands to whatever needs to included when using COMMON library

if(${use_http})
	set(IOTHUB_CLIENT_HTTP_TRANSPORT_INC_FOLDER ${CMAKE_CURRENT_LIST_DIR}/inc CACHE INTERNAL "this is what needs to be included if using iothub_client_http_transport lib" FORCE)
endif()

if(${use_amqp})
	set(IOTHUB_CLIENT_AMQP_TRANSPORT_INC_FOLDER ${CMAKE_CURRENT_LIST_DIR}/inc CACHE INTERNAL "this is what needs to be included if using iothub_client_amqp_transport lib" FORCE)
endif()

if(${use_mqtt})
	set(IOTHUB_CLIENT_MQTT_TRANSPORT_INC_FOLDER ${CMAKE_CURRENT_LIST_DIR}/inc CACHE INTERNAL "this is what needs to be included if using iothub_client_mqtt_transport lib" FORCE)
endif()

set(IOTHUB_CLIENT_INC_FOLDER ${CMAKE_CURRENT_LIST_DIR}/inc CACHE INTERNAL "this is what needs to be included if using iothub_client lib" FORCE)

include_directories(${SHARED_UTIL_INC_FOLDER})

if(${use_http})
	include_directories(${IOTHUB_CLIENT_HTTP_TRANSPORT_INC_FOLDER})
	add_library(iothub_client_http_transport 
		${iothub_client_http_transport_c_files} 
		${iothub_client_http_transport_h_files}
	)
	linkSharedUtil(iothub_client_http_transport)
endif()

if(${use_amqp})
	include_directories(${IOTHUB_CLIENT_AMQP_TRANSPORT_INC_FOLDER} ${PROTON_C_INCLUDES})
	includeProton()

	add_definitions(-Dqpid_proton_EXPORTS)
	add_library(iothub_client_amqp_transport 
		${iothub_client_amqp_transport_c_files} 
		${iothub_client_amqp_transport_h_files}
	)
	linkSharedUtil(iothub_client_amqp_transport)
endif()

if(${use_mqtt})
	include_directories(${IOTHUB_CLIENT_MQTT_TRANSPORT_INC_FOLDER} ${MQTT_INC_FOLDER})
	add_library(iothub_client_mqtt_transport 
		${iothub_client_mqtt_transport_c_files} 
		${iothub_client_mqtt_transport_h_files}
	)
	linkSharedUtil(iothub_client_mqtt_transport)
	linkMqttLibrary(iothub_client_mqtt_transport)
endif()

include_directories(${IOTHUB_CLIENT_INC_FOLDER})

IF(WIN32)
	#windows needs this define
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	add_definitions(-DGB_MEASURE_MEMORY_FOR_THIS -DGB_DEBUG_ALLOC)
ENDIF(WIN32)

add_library(iothub_client 
	${iothub_client_c_files} 
	${iothub_client_h_files}
)
linkSharedUtil(iothub_client)

add_subdirectory(samples)

if(NOT IN_OPENWRT)
# Disable tests for OpenWRT
add_subdirectory(tests)
endif()
