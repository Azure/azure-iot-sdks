#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

#this is CMakeLists for iothub_test

compileAsC99()

set(iothub_service_client_c_files
./parson/parson.c
./src/connection_string_parser.c
./src/iothub_service_client_auth.c
./src/iothub_registrymanager.c
)

set(iothub_service_client_h_files
./parson/parson.h
./inc/connection_string_parser.h
./inc/iothub_registrymanager.h
./inc/iothub_messaging_ll.h
./inc/iothub_service_client_auth.h
)

#these are the include folders
#the following "set" statetement exports across the project a global variable called IOTHUB_SERVICE_CLIENT_INC_FOLDER that expands to whatever needs to included when using iothub_service_client library
set(IOTHUB_SERVICE_CLIENT_INC_FOLDER ${CMAKE_CURRENT_LIST_DIR}/inc CACHE INTERNAL "this is what needs to be included if using iothub_service_client_auth, iothub_registrymanager, iothub_messaging" FORCE)

include_directories(${SHARED_UTIL_INC_FOLDER})

include_directories(${IOTHUB_SERVICE_CLIENT_INC_FOLDER})

include_directories(${UAMQP_INC_FOLDER})

include_directories(${PARSON_INC_FOLDER})

IF(WIN32)
	#windows needs this define
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	add_definitions(-DGB_MEASURE_MEMORY_FOR_THIS -DGB_DEBUG_ALLOC)
ENDIF(WIN32)

add_library(iothub_service_client ${iothub_service_client_c_files} ${iothub_service_client_h_files})

target_link_libraries(iothub_service_client uamqp)

if (NOT ${ARCHITECTURE} STREQUAL "ARM")
	add_subdirectory(samples)
endif()

if(NOT IN_OPENWRT)
# Disable tests for OpenWRT
add_subdirectory(tests)
endif()

