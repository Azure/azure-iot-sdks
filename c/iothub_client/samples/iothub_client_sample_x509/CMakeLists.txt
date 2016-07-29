#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

#this is CMakeLists.txt for iothub_client_sample_x509

compileAsC99()

set(iothub_client_sample_x509_c_files
iothub_client_sample_x509.c
)

IF(WIN32)
    #windows needs this define
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    add_definitions(-DGB_MEASURE_MEMORY_FOR_THIS -DGB_DEBUG_ALLOC)
ENDIF(WIN32)

include_directories(.)

add_executable(iothub_client_sample_x509 ${iothub_client_sample_x509_c_files})

target_link_libraries(iothub_client_sample_x509 
#iothubclient is here only because locking... in gballoc no less.
    iothub_client
)

if(${use_http})
    target_link_libraries(iothub_client_sample_x509 
        iothub_client_http_transport
    )
    linkSharedUtil(iothub_client_sample_x509)
    linkHttp(iothub_client_sample_x509)
    add_definitions(-DUSE_HTTP)
endif()

if(${use_amqp})
    target_link_libraries(iothub_client_sample_x509 
    #iothubclient is here only because locking... in gballoc no less.
        iothub_client_amqp_transport
    )
    linkUAMQP(iothub_client_sample_x509)
    add_definitions(-DUSE_AMQP)
endif()

if(${use_mqtt})
    target_link_libraries(iothub_client_sample_x509 
        iothub_client_mqtt_transport
    )
    linkMqttLibrary(iothub_client_sample_x509)
    add_definitions(-DUSE_MQTT)
endif()
