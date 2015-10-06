#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

cmake_minimum_required(VERSION 3.0)
#this is CMakeLists for iothub_client tests folder

add_subdirectory(iothubclient_e2etests)
add_subdirectory(iothubclient_ll_unittests)
add_subdirectory(iothubclient_unittests)
add_subdirectory(iothubmessage_unittests)
add_subdirectory(iothubtransportamqp_unittests)
add_subdirectory(iothubtransporthttp_unittests)
add_subdirectory(iothubtransportmqtt_unittests)
add_subdirectory(longhaul_tests)
add_subdirectory(mqttapi_paho_unittests)
add_subdirectory(version_unittests)