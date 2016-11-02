#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

#this is CMakeLists.txt for iothub_client_sample_device_method

if(NOT ${use_mqtt})
    message(FATAL_ERROR "iothub_client_sample_device_method being generated without mqtt support")
endif()

compileAsC99()

set(iothub_client_sample_device_method_c_files
    iothub_client_sample_device_method.c
)

set(iothub_client_sample_device_method_h_files
)

IF(WIN32)
    #windows needs this define
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    add_definitions(-DGB_MEASURE_MEMORY_FOR_THIS -DGB_DEBUG_ALLOC)
ENDIF(WIN32)

include_directories(.)

add_executable(iothub_client_sample_device_method ${iothub_client_sample_device_method_c_files} ${iothub_client_sample_device_method_h_files})

target_link_libraries(iothub_client_sample_device_method  
    iothub_client_mqtt_transport
    iothub_client 
)

