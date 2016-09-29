#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

#this is CMakeLists.txt for forrester
cmake_minimum_required(VERSION 2.8.11)

compileAsC99()


set(AZUREIOT_INC_FOLDER ".." ${SERIALIZER_INC_FOLDER} ${SHARED_UTIL_INC_FOLDER})

include_directories(${AZUREIOT_INC_FOLDER})

set(forrester_c_files
    ../forrester.c
    pi.c
)

set(forrester_h_files
    ../forrester.h
)

add_executable(forrester ${forrester_c_files} ${forrester_h_files})

target_link_libraries(forrester
    iothub_client
    iothub_client_mqtt_transport
    aziotsharedutil
    serializer
    umqtt
)
