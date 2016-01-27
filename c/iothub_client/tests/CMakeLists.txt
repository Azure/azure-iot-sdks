#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

cmake_minimum_required(VERSION 2.8.11)

#this is CMakeLists for iothub_client tests folder

add_subdirectory(iothubclient_ll_unittests)
add_subdirectory(iothubclient_unittests)
add_subdirectory(iothubmessage_unittests)

if(${use_amqp})
	add_subdirectory(iothubtransportamqp_unittests)
	add_subdirectory(iothubclient_amqp_e2etests)
	add_subdirectory(longhaul_tests)
endif()

if(${use_http})
	add_subdirectory(iothubtransporthttp_unittests)
	if(${use_amqp})
		add_subdirectory(iothubclient_http_e2etests)
	endif()
endif()

if(${use_mqtt})
	add_subdirectory(iothubtransportmqtt_unittests)
	if(${use_amqp})
		add_subdirectory(iothubclient_mqtt_e2etests)
	endif()
endif()

add_subdirectory(version_unittests)