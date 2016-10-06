#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

#this is CMakeLists.txt for iothub_devicemethod_sample

compileAsC99()

set(iothub_devicemethod_sample_c_files
iothub_devicemethod_sample.c
)

if(WIN32)
	set(iothub_devicemethod_sample_c_files ${iothub_devicemethod_sample_c_files} ./windows/main.c)
elseif(LINUX)
	set(iothub_devicemethod_sample_c_files ${iothub_devicemethod_sample_c_files} ./linux/main.c)
endif()


set(iothub_devicemethod_sample_h_files
iothub_devicemethod_sample.h
)

IF(WIN32)
	#windows needs this define
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	add_definitions(-DGB_MEASURE_MEMORY_FOR_THIS -DGB_DEBUG_ALLOC)
ENDIF(WIN32)

include_directories(.)

add_executable(iothub_devicemethod_sample ${iothub_devicemethod_sample_c_files} ${iothub_devicemethod_sample_h_files})

target_link_libraries(iothub_devicemethod_sample 
	iothub_service_client
)

linkSharedUtil(iothub_devicemethod_sample)
