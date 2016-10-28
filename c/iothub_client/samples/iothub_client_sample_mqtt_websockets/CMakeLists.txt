#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

#this is CMakeLists.txt for iothub_client_sample_mqtt

if(NOT ${use_mqtt})
    message(FATAL_ERROR "iothub_client_sample_mqtt being generated without mqtt support")
endif()

compileAsC99()

set(iothub_client_sample_mqtt_c_files
    iothub_client_sample_mqtt_ws.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../certs/certs.c
)

set(iothub_client_sample_mqtt_h_files
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../certs/certs.h
)

IF(WIN32)
    #windows needs this define
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    add_definitions(-DGB_MEASURE_MEMORY_FOR_THIS -DGB_DEBUG_ALLOC)
ENDIF(WIN32)

include_directories(.)

add_executable(iothub_client_sample_mqtt_websockets ${iothub_client_sample_mqtt_c_files} ${iothub_client_sample_mqtt_h_files})

target_link_libraries(iothub_client_sample_mqtt_websockets  
    iothub_client_mqtt_ws_transport
    iothub_client 
)

linkSharedUtil(iothub_client_sample_mqtt_websockets)
linkMqttLibrary(iothub_client_sample_mqtt_websockets)

