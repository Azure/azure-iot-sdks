#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

#this is CMakeLists.txt for iothub_client_sample_http

compileAsC99()

if(NOT ${use_http})
	message(FATAL_ERROR "iothub_client_sample_http being generated without HTTP support")
endif()

set(iothub_client_sample_http_c_files
iothub_client_sample_http.c
)

set(iothub_client_sample_http_h_files
iothub_client_sample_http.h
)

IF(WIN32)
	#windows needs this define
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	add_definitions(-DGB_MEASURE_MEMORY_FOR_THIS -DGB_DEBUG_ALLOC)

if (WINCE) # Use C++ as mitigation for C99
  SET_SOURCE_FILES_PROPERTIES(iothub_client_sample_http.c PROPERTIES LANGUAGE CXX)
ENDIF()

ENDIF(WIN32)

include_directories(.)

add_executable(iothub_client_sample_http ${iothub_client_sample_http_c_files} ${iothub_client_sample_http_h_files})

target_link_libraries(iothub_client_sample_http 
#iothubclient is here only because locking... in gballoc no less.
	iothub_client
	iothub_client_http_transport
)

linkSharedUtil(iothub_client_sample_http)
linkHttp(iothub_client_sample_http)