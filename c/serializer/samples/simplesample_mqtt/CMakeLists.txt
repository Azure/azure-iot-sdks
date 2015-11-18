#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

#this is CMakeLists.txt for simplesample_mqtt sample

if(NOT ${use_mqtt})
	message(FATAL_ERROR "simplesample_mqtt being generated without mqtt support")
endif()

compileAsC99()

set(simplesample_mqtt_c_files
	simplesample_mqtt.c
)

if(WIN32)
	set(simplesample_mqtt_c_files ${simplesample_mqtt_c_files} ./windows/main.c)
else()
	set(simplesample_mqtt_c_files ${simplesample_mqtt_c_files} ./linux/main.c)
endif()

set(simplesample_mqtt_h_files
	simplesample_mqtt.h
)

IF(WIN32)
	#windows needs this define
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
ENDIF(WIN32)

include_directories(. ${COMMON_INC_FOLDER} ${IOTHUB_CLIENT_INC_FOLDER})

add_executable(simplesample_mqtt ${simplesample_mqtt_c_files} ${simplesample_mqtt_h_files})

target_link_libraries(simplesample_mqtt 
	serializer 
	iothub_client
	iothub_client_mqtt_transport 
	common
	iothub_client_mqtt_transport
	iothub_client
	)
linkPaho(simplesample_mqtt)

if(WIN32)
else()
	target_link_libraries(simplesample_mqtt pthread)
endif()