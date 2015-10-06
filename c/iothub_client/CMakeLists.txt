#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

cmake_minimum_required(VERSION 3.0)
#this is CMakeLists for iothub_client

set(iothub_client_all_transport_c_files
./src/version.c
./src/iothub_message.c
./src/iothub_client_ll.c
)

set(iothub_client_all_transport_h_files
./inc/iothub_message.h
./inc/iothub_client_ll.h
./inc/version.h
)

set(iothub_client_c_files
./src/iothub_client.c
./src/version.c
../common/adapters/threadapi_c11.c
../common/adapters/lock_c11.c
)

set(iothub_client_h_files
./inc/iothub_client.h
../common/inc/threadapi.h
../common/inc/lock.h
./inc/version.h
)

set(iothub_client_http_transport_c_files
${iothub_client_all_transport_c_files}
./src/iothubtransporthttp.c
../common/adapters/httpapi_winhttp.c
)

set(iothub_client_http_transport_h_files
${iothub_client_all_transport_h_files}
./inc/iothubtransporthttp.h
../common/inc/httpapi.h
)

set(iothub_client_amqp_transport_c_files
${iothub_client_all_transport_c_files}
./src/iothubtransportamqp.c
)

set(iothub_client_amqp_transport_h_files
${iothub_client_all_transport_h_files}
./inc/iothubtransportamqp.h
)

set(iothub_client_mqtt_transport_c_files
${iothub_client_all_transport_c_files}
./src/iothubtransportmqtt.c
./adapters/mqttapi_paho.c
)

set(iothub_client_mqtt_transport_h_files
${iothub_client_all_transport_h_files}
./inc/iothubtransportmqtt.h
../common/inc/mqttapi.h
)

#these are the include folders
#the following "set" statetement exports across the project a global variable called COMMON_INC_FOLDER that expands to whatever needs to included when using COMMON library
set(IOTHUB_CLIENT_HTTP_TRANSPORT_INC_FOLDER ${CMAKE_CURRENT_LIST_DIR}/inc CACHE INTERNAL "this is what needs to be included if using iothub_client_http_transport lib" FORCE)
set(IOTHUB_CLIENT_AMQP_TRANSPORT_INC_FOLDER ${CMAKE_CURRENT_LIST_DIR}/inc CACHE INTERNAL "this is what needs to be included if using iothub_client_amqp_transport lib" FORCE)
set(IOTHUB_CLIENT_MQTT_TRANSPORT_INC_FOLDER ${CMAKE_CURRENT_LIST_DIR}/inc CACHE INTERNAL "this is what needs to be included if using iothub_client_mqtt_transport lib" FORCE)
set(IOTHUB_CLIENT_INC_FOLDER ${CMAKE_CURRENT_LIST_DIR}/inc CACHE INTERNAL "this is what needs to be included if using iothub_client lib" FORCE)


include_directories(${IOTHUB_CLIENT_HTTP_TRANSPORT_INC_FOLDER} ${COMMON_INC_FOLDER})
include_directories(${IOTHUB_CLIENT_AMQP_TRANSPORT_INC_FOLDER} ${COMMON_INC_FOLDER} $ENV{proton_path}/qpid-proton/proton-c/include)
include_directories(${IOTHUB_CLIENT_MQTT_TRANSPORT_INC_FOLDER} ${COMMON_INC_FOLDER} $ENV{paho_path}/org.eclipse.paho.mqtt.c/src)
include_directories(${IOTHUB_CLIENT_INC_FOLDER} ${COMMON_INC_FOLDER})

IF(WIN32)
	#windows needs this define
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
ENDIF(WIN32)

add_library(
iothub_client_http_transport ${iothub_client_http_transport_c_files} ${iothub_client_http_transport_h_files}
)

add_library(
iothub_client_amqp_transport ${iothub_client_amqp_transport_c_files} ${iothub_client_amqp_transport_h_files}
)

add_library(
iothub_client_mqtt_transport ${iothub_client_mqtt_transport_c_files} ${iothub_client_mqtt_transport_h_files}
)

add_library(
iothub_client ${iothub_client_c_files} ${iothub_client_h_files}
)

add_subdirectory(samples)
add_subdirectory(tests)