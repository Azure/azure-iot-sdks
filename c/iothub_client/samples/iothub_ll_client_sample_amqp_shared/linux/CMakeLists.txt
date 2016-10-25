#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

#this is CMakeLists.txt for iothub_ll_client_sample_amqp_shared
cmake_minimum_required(VERSION 2.8.11)

if(WIN32)
    message(FATAL_ERROR "This CMake file is only support Linux builds!")
endif()

set(AZUREIOT_INC_FOLDER ".." "/usr/include/azureiot")

include_directories(${AZUREIOT_INC_FOLDER})

set(iothub_ll_client_sample_amqp_shared_c_files
    ../iothub_ll_client_sample_amqp_shared.c
)

set(iothub_ll_client_sample_amqp_shared_h_files
    ../iothub_ll_client_sample_amqp_shared.h
)

add_executable(iothub_ll_client_sample_amqp_shared ${iothub_ll_client_sample_amqp_shared_c_files} ${iothub_ll_client_sample_amqp_shared_h_files})

target_link_libraries(iothub_ll_client_sample_amqp_shared
    iothub_client
    iothub_client_amqp_transport
    aziotsharedutil
    uamqp
    pthread
    curl
    ssl
    crypto
)
