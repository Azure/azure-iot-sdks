#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

cmake_minimum_required(VERSION 2.8.12)

#this is CMakeLists for iothub_client tests folder

add_subdirectory(iothubclient_ll_unittests)
if(NOT ${dont_use_uploadtoblob})

add_subdirectory(iothubclient_ll_u2b_unittests)
endif()

add_subdirectory(iothubclient_unittests)
add_subdirectory(iothubmessage_unittests)
add_subdirectory(iothubtransport_unittests)
add_subdirectory(blob_unittests)

if(${use_http})
	add_subdirectory(iothubtransporthttp_unittests)
	if (${run_e2e_tests})
		add_subdirectory(iothubclient_http_e2etests)
	endif()
endif()

if(${use_mqtt})
	add_subdirectory(iothubtransportmqtt_unittests)
	if (${run_e2e_tests})
		add_subdirectory(iothubclient_mqtt_e2etests)
	endif()
endif()

if(${use_amqp})
	add_subdirectory(iothubtransportamqp_unittests)
	if (${run_e2e_tests})
		add_subdirectory(iothubclient_amqp_e2etests)
	endif()
	
	if (${run_longhaul_tests})
		add_subdirectory(longhaul_tests)
	endif()
endif()

add_subdirectory(version_unittests)