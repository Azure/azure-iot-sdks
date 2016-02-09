#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

#this is CMakeLists.txt for simplesample_amqp sample

compileAsC99()

if(NOT ${use_amqp})
	message(FATAL_ERROR "simplesample_amqp being generated without amqp support")
endif()

set(simplesample_amqp_c_files
simplesample_amqp.c
)

if(WIN32)
	set(simplesample_amqp_c_files ${simplesample_amqp_c_files} ./windows/main.c)
else()
	set(simplesample_amqp_c_files ${simplesample_amqp_c_files} ./linux/main.c)
endif()

set(simplesample_amqp_h_files
simplesample_amqp.h
)

IF(WIN32)
	#windows needs this define
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
ENDIF(WIN32)

include_directories(. ${SHARED_UTIL_INC_FOLDER} ${IOTHUB_CLIENT_INC_FOLDER})
link_directories(${whatIsBuilding}_dll ${SHARED_UTIL_LIB_DIR})

add_executable(simplesample_amqp ${simplesample_amqp_c_files} ${simplesample_amqp_h_files})

target_link_libraries(simplesample_amqp serializer iothub_client iothub_client_amqp_transport)

linkSharedUtil(simplesample_amqp)
linkUAMQP(simplesample_amqp)
