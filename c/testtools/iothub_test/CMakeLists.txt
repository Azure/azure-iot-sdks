#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

#this is CMakeLists for iothub_test

compileAsC99()

set(iothub_test_c_files
./src/iothubtest.c
./src/iothub_account.c
)

set(iothub_test_h_files
./inc/iothubtest.h
./inc/iothub_account.h
)

#these are the include folders
#the following "set" statetement exports across the project a global variable called IOTHUB_TEST_INC_FOLDER that expands to whatever needs to included when using iothub_test library
set(IOTHUB_TEST_INC_FOLDER ${CMAKE_CURRENT_LIST_DIR}/inc CACHE INTERNAL "this is what needs to be included if using iothub_test" FORCE)

include_directories(${IOTHUB_TEST_INC_FOLDER} ${SHARED_UTIL_INC_FOLDER} ${UAMQP_INC_FOLDER})
include_directories(${CMAKE_CURRENT_LIST_DIR}/../../iothub_client/inc)
include_directories(${IOTHUB_SERVICE_CLIENT_INC_FOLDER})

IF(WIN32)
	#windows needs this define
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	add_definitions(-DGB_MEASURE_MEMORY_FOR_THIS -DGB_DEBUG_ALLOC)
ENDIF(WIN32)

add_library(iothub_test ${iothub_test_c_files} ${iothub_test_h_files})

if(NOT ${nuget_e2e_tests})
    target_link_libraries(iothub_test uamqp iothub_service_client)
else()
    target_link_libraries(iothub_test iothub_service_client)
endif()