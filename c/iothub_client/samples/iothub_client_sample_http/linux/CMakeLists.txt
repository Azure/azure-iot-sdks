#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

#this is CMakeLists.txt for iothub_client_sample_mqtt
cmake_minimum_required(VERSION 2.8.11)

if(WIN32)
    message(FATAL_ERROR "This CMake file is only support Linux builds!")
endif()

set(AZUREIOT_INC_FOLDER ".." "/usr/include/azureiot")

include_directories(${AZUREIOT_INC_FOLDER})

set(iothub_client_sample_http_c_files
    ../iothub_client_sample_http.c
    ./main.c
)

set(iothub_client_sample_http_h_files
    ../iothub_client_sample_http.h
)

add_executable(iothub_client_sample_http ${iothub_client_sample_http_c_files} ${iothub_client_sample_http_h_files})

target_link_libraries(iothub_client_sample_http
    curl
    ssl
    crypto
    pthread
    iothub_client
    pthread
    ssl
    crypto
    aziotsharedutil
    iothub_client_http_transport
    iothub_client
)