#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

#this is CMakeLists.txt for temp_sensor_anomaly sample
if(NOT ${use_http})
	message(FATAL_ERROR "temp_sensor_anomaly being generated without HTTP support")
endif()

compileAsC99()

set(temp_sensor_anomaly_c_files
./windows/main.c
)

set(temp_sensor_anomaly_h_files
)

IF(WIN32)
	#windows needs this define
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
ENDIF(WIN32)

include_directories(. ${SHARED_UTIL_INC_FOLDER} ${IOTHUB_CLIENT_INC_FOLDER})
link_directories(${whatIsBuilding}_dll ${SHARED_UTIL_LIB_DIR})

add_executable(temp_sensor_anomaly ${temp_sensor_anomaly_c_files} ${temp_sensor_anomaly_h_files})

target_link_libraries(temp_sensor_anomaly 
	serializer 
	iothub_client 
	iothub_client_http_transport)
	
linkSharedUtil(temp_sensor_anomaly)
linkHttp(temp_sensor_anomaly)
