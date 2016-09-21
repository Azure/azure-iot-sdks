#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

#this is CMakeLists.txt for iothub_client_sample_mqtt_dm
cmake_minimum_required(VERSION 2.8.11)

compileAsC99()


set(AZUREIOT_INC_FOLDER ".." ${SERIALIZER_INC_FOLDER} ${SHARED_UTIL_INC_FOLDER})

include_directories(${AZUREIOT_INC_FOLDER})

set(iothub_client_sample_mqtt_dm_c_files
    ../iothub_client_sample_mqtt_dm.cpp
    template.cpp
)

set(iothub_client_sample_mqtt_dm_h_files
    ../iothub_client_sample_mqtt_dm.h
)

add_executable(iothub_client_sample_mqtt_dm ${iothub_client_sample_mqtt_dm_c_files} ${iothub_client_sample_mqtt_dm_h_files})

target_link_libraries(iothub_client_sample_mqtt_dm
    iothub_client
    iothub_client_mqtt_transport
    aziotsharedutil
    serializer
    umqtt
)
