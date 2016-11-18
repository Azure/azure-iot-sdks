#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

#this is CMakeLists.txt for remote_monitoring sample

compileAsC99()

if(NOT ${use_amqp})
	message(FATAL_ERROR "remote_monitoring being generated without amqp support")
endif()

set(remote_monitoring_c_files
	remote_monitoring.c
	${CMAKE_CURRENT_SOURCE_DIR}/../../../certs/certs.c
)

if(WIN32)
	set(remote_monitoring_c_files ${remote_monitoring_c_files})
else()
	set(remote_monitoring_c_files ${remote_monitoring_c_files})
endif()

set(remote_monitoring_h_files
	remote_monitoring.h
	${CMAKE_CURRENT_SOURCE_DIR}/../../../certs/certs.h
)

IF(WIN32)
	#windows needs this define
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
ENDIF(WIN32)

include_directories(. ${IOTHUB_CLIENT_INC_FOLDER})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../certs)

link_directories(${whatIsBuilding}_dll ${SHARED_UTIL_LIB_DIR})

add_executable(remote_monitoring ${remote_monitoring_c_files} ${remote_monitoring_h_files})
target_link_libraries(remote_monitoring serializer iothub_client iothub_client_amqp_transport)

linkSharedUtil(remote_monitoring)
linkUAMQP(remote_monitoring)
