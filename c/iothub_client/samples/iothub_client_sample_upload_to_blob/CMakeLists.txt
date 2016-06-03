#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

#this is CMakeLists.txt for iothub_client_sample_upload_to_blob

compileAsC99()

if(NOT ${use_http})
	message(FATAL_ERROR "iothub_client_sample_upload_to_blob being generated without HTTP support")
endif()

set(iothub_client_sample_upload_to_blob_c_files
iothub_client_sample_upload_to_blob.c
)

if(WIN32)
	set(iothub_client_sample_upload_to_blob_c_files ${iothub_client_sample_upload_to_blob_c_files} ./windows/main.c)
else()
	set(iothub_client_sample_upload_to_blob_c_files ${iothub_client_sample_upload_to_blob_c_files} ./linux/main.c)
endif()


set(iothub_client_sample_upload_to_blob_h_files
iothub_client_sample_upload_to_blob.h
)

IF(WIN32)
	#windows needs this define
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	add_definitions(-DGB_MEASURE_MEMORY_FOR_THIS -DGB_DEBUG_ALLOC)
ENDIF(WIN32)

include_directories(.)

add_executable(iothub_client_sample_upload_to_blob ${iothub_client_sample_upload_to_blob_c_files} ${iothub_client_sample_upload_to_blob_h_files})

target_link_libraries(iothub_client_sample_upload_to_blob 
#iothubclient is here only because locking... in gballoc no less.
	iothub_client
	iothub_client_http_transport
)

linkSharedUtil(iothub_client_sample_upload_to_blob)
linkHttp(iothub_client_sample_upload_to_blob)