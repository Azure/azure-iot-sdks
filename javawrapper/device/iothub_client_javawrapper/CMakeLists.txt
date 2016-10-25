#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

#this is CMakeLists.txt for iothub_client_javawrapper

if(NOT ${use_mqtt})
    message(FATAL_ERROR "iothub_client_javawrapper needs mqtt support")
endif()
if(NOT ${use_amqp})
    message(FATAL_ERROR "iothub_client_javawrapper needs amqp support")
endif()
if(NOT ${use_http})
    message(FATAL_ERROR "iothub_client_javawrapper needs http support")
endif()

compileAsC99()

if(${use_wsio})
    set(iothub_client_c_files
    ../../../c/iothub_client/src/iothub_client.c
    ../../../c/iothub_client/src/iothub_client_ll.c
    ../../../c/iothub_client/src/iothub_message.c
    ../../../c/iothub_client/src/iothubtransportamqp_websockets.c
    ../../../c/iothub_client/src/iothubtransportamqp_auth.c
    ../../../c/iothub_client/src/iothubtransporthttp.c
    ../../../c/iothub_client/src/iothubtransportmqtt.c
    ../../../c/iothub_client/src/version.c
    )
else()
    set(iothub_client_c_files
    ../../../c/iothub_client/src/iothub_client.c
    ../../../c/iothub_client/src/iothub_client_ll.c
    ../../../c/iothub_client/src/iothub_message.c
    ../../../c/iothub_client/src/iothubtransportamqp.c
	../../../c/iothub_client/src/iothubtransportamqp_auth.c
    ../../../c/iothub_client/src/iothubtransporthttp.c
    ../../../c/iothub_client/src/iothubtransportmqtt.c
    ../../../c/iothub_client/src/version.c
    )
endif()

set(iothub_client_mock_c_files 
     ../test/iothub_client_mock.cpp 
 ) 


if(${use_wsio})
    file(READ ./windows/iothub_client_javawrapper.def def_file)
    file(WRITE ./windows/iothub_client_javawrapper_temp.def "${def_file}")
    file(APPEND ./windows/iothub_client_javawrapper_temp.def "\nAMQP_Protocol_over_WebSocketsTls")
endif()

if(WIN32)
    if(${use_wsio})
        set(iothub_client_c_files ${iothub_client_c_files} ./windows/dllmain.c ./windows/iothub_client_javawrapper_temp.def)
    else()
        set(iothub_client_c_files ${iothub_client_c_files} ./windows/dllmain.c ./windows/iothub_client_javawrapper.def)
    endif()
    set(iothub_client_mock_c_files ${iothub_client_mock_c_files} ./windows/dllmain.c ./windows/iothub_client_mock_javawrapper.def)
else()
    set(iothub_client_c_files ${iothub_client_c_files} ) 
endif()

set(iothub_client_sample_h_files
    
)

include_directories(${SHARED_UTIL_INC_FOLDER})
include_directories(${IOTHUB_CLIENT_INC_FOLDER})
if(${use_http})
    include_directories(${IOTHUB_CLIENT_HTTP_TRANSPORT_INC_FOLDER})
endif()
if(${use_amqp})
    include_directories(${IOTHUB_CLIENT_AMQP_TRANSPORT_INC_FOLDER} ${UAMQP_INC_FOLDER})
endif()
if(${use_mqtt})
    include_directories(${IOTHUB_CLIENT_MQTT_TRANSPORT_INC_FOLDER} ${MQTT_INC_FOLDER})
endif()

include_directories(.)


add_library( iothub_client_javawrapper SHARED
${iothub_client_c_files} 
${iothub_client_h_files})

add_library( iothub_client_mock SHARED 
     ${iothub_client_mock_c_files}  
 ) 


SET_TARGET_PROPERTIES(iothub_client_javawrapper PROPERTIES OUTPUT_NAME "iothub_client_java")

target_link_libraries(  
    iothub_client_javawrapper
    iothub_client_mqtt_transport
    iothub_client_http_transport
    iothub_client_amqp_transport
    iothub_client 
    uamqp
)

target_link_libraries(   
     iothub_client_mock 
)

linkSharedUtil(iothub_client_javawrapper)
linkUAMQP(iothub_client_javawrapper)
