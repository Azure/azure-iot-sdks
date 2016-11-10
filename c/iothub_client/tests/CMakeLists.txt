#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

cmake_minimum_required(VERSION 2.8.12)

#this is CMakeLists for iothub_client tests folder

add_subdirectory(iothubclient_ll_ut)
if(NOT ${dont_use_uploadtoblob})
add_subdirectory(iothubclient_ll_u2b_ut)
endif()

add_subdirectory(iothubclient_ut)
add_subdirectory(iothubmessage_ut)
add_subdirectory(iothubtransport_ut)
add_subdirectory(blob_ut)

if(${use_http})
    add_subdirectory(iothubtransporthttp_ut)
    if (${run_e2e_tests} OR ${nuget_e2e_tests})
        add_subdirectory(iothubclient_http_e2e)
    endif()
endif()

if(${use_mqtt})
    add_subdirectory(iothubtransportmqtt_ut)
    add_subdirectory(iothubtransport_mqtt_common_ut)
    if (${use_wsio})
        add_subdirectory(iothubtransportmqtt_ws_ut)
    endif()
    if (${run_e2e_tests} OR ${nuget_e2e_tests})
        add_subdirectory(iothubclient_mqtt_e2e)
        add_subdirectory(iothubclient_mqtt_dt_e2e)
        add_subdirectory(iothubclient_mqtt_device_method_e2e)
    endif()
endif()

if(${use_amqp})
    add_subdirectory(uamqp_messaging_ut)
    add_subdirectory(iothubtransport_amqp_common_ut)
    add_subdirectory(iothubtransportamqp_methods_ut)
    add_subdirectory(iothubtransportamqp_ut)
    
    if(${use_wsio})
        add_subdirectory(iothubtransportamqp_ws_ut)
    endif()
    
    if (${run_e2e_tests} OR ${nuget_e2e_tests})
        add_subdirectory(iothubclient_amqp_e2e)
        add_subdirectory(iothubclient_amqp_device_method_e2e)
        if(${use_wsio})
            add_subdirectory(iothubclient_amqp_ws_e2e)
        endif()
    endif()
    
    if (${run_longhaul_tests})
        add_subdirectory(longhaul_tests)
    endif()
endif()

add_subdirectory(version_ut)
