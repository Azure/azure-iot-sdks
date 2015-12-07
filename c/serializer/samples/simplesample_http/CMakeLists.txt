#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

#this is CMakeLists.txt for simplesample_http sample

if(NOT ${use_http})
	message(FATAL_ERROR "simplesample_http being generated without HTTP support")
endif()

compileAsC99()

set(simplesample_http_c_files
simplesample_http.c
)

if(WIN32)
	set(simplesample_http_c_files ${simplesample_http_c_files} ./windows/main.c)
else()
	set(simplesample_http_c_files ${simplesample_http_c_files} ./linux/main.c)
endif()


set(simplesample_http_h_files
	simplesample_http.h
)

IF(WIN32)
	#windows needs this define
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
ENDIF(WIN32)

include_directories(. ${SHARED_UTIL_INC_FOLDER} ${IOTHUB_CLIENT_INC_FOLDER})
link_directories(${whatIsBuilding}_dll ${SHARED_UTIL_LIB_DIR})

add_executable(simplesample_http ${simplesample_http_c_files} ${simplesample_http_h_files})

target_link_libraries(simplesample_http 
	serializer 
	iothub_client 
	iothub_client_http_transport 
)

linkSharedUtil(simplesample_http)
linkHttp(simplesample_http)