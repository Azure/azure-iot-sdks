#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

cmake_minimum_required(VERSION 3.0)
#this is CMakeLists for iothub_client tests folder

add_subdirectory(iothubclient_ll_unittests)
add_subdirectory(iothubclient_unittests)
add_subdirectory(iothubmessage_unittests)

if(${use_amqp})
	add_subdirectory(iothubtransportamqp_unittests)
	add_subdirectory(longhaul_tests)
endif()

if(${use_amqp} AND ${use_http} AND ${use_mqtt})
	add_subdirectory(iothubclient_e2etests)
endif()

if(${use_http})
	add_subdirectory(iothubtransporthttp_unittests)
endif()

if(${use_mqtt})
	add_subdirectory(iothubtransportmqtt_unittests)
	add_subdirectory(mqttapi_paho_unittests)
endif()

add_subdirectory(version_unittests)