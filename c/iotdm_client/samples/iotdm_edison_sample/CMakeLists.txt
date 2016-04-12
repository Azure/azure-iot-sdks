#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

cmake_minimum_required(VERSION 2.8)

set (SAMPLE_NAME iotdm_edison_sample)

set(my_c_files
    ${SAMPLE_NAME}.c
    edison.c
)

set(my_h_files
)

if(WIN32)
    #windows needs this define
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    add_definitions(-DGB_MEASURE_MEMORY_FOR_THIS -DGB_DEBUG_ALLOC)
endif(WIN32)

include_directories(.)

add_executable(${SAMPLE_NAME}
    ${my_h_files}
    ${my_c_files}
)

target_link_libraries(${SAMPLE_NAME}
    iotdm_client)
    
linkSharedUtil(${SAMPLE_NAME})