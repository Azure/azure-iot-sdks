#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

cmake_minimum_required(VERSION 2.8.11)
#this is CMakeLists for iothub_client

compileAsC99()

set(iothub_client_ll_transport_c_files
./src/version.c
./src/iothub_message.c
./src/iothub_client_ll.c
./src/blob.c
)

if(NOT ${dont_use_uploadtoblob})
    set(iothub_client_ll_transport_c_files 
        ${iothub_client_ll_transport_c_files}
        ../parson/parson.c
        ./src/iothub_client_ll_uploadtoblob.c
        )

    if(MSVC)
        set_source_files_properties(../parson/parson.c PROPERTIES COMPILE_FLAGS "/wd4244 /wd4232")
    endif()
endif()


set(iothub_client_ll_transport_h_files
./inc/iothub_message.h
./inc/iothub_client_ll.h
./inc/iothub_client_version.h
./inc/iothub_transport_ll.h
./inc/blob.h
)

if(NOT ${dont_use_uploadtoblob})
    set(iothub_client_ll_transport_h_files 
        ${iothub_client_ll_transport_h_files}
        ../parson/parson.h
        ./inc/iothub_client_ll_uploadtoblob.h
    )
endif()

set(iothub_client_c_files
./src/iothub_client.c
./src/version.c
./src/iothubtransport.c
)

set(iothub_client_h_files
./inc/iothub_client.h
./inc/iothub_client_options.h
./inc/iothub_client_version.h
./inc/iothubtransport.h
./inc/iothub_client_private.h
)

set(iothub_client_h_install_files
    ${iothub_client_ll_transport_h_files}
    ${iothub_client_h_files}
)

set(iothub_client_libs
iothub_client
)

if(${use_http})
    set(iothub_client_http_transport_c_files
        ${iothub_client_ll_transport_c_files}
        ./src/iothubtransporthttp.c
    )

    set(iothub_client_http_transport_h_files
        ${iothub_client_ll_transport_h_files}
        ./inc/iothubtransporthttp.h
        ./inc/iothub_transport_ll.h
    )
    
    set(iothub_client_h_install_files
        ${iothub_client_h_install_files}
        ${iothub_client_http_transport_h_files}
    )
endif()

if(${use_amqp})
    set(iothub_client_amqp_transport_common_c_files
        ${iothub_client_ll_transport_c_files}
        ./src/iothubtransport_amqp_common.c
        ./src/iothubtransportamqp_auth.c
        ./src/iothubtransport_amqp_connection.c
        ./src/iothubtransportamqp_methods.c
        ./src/uamqp_messaging.c
    )

    set(iothub_client_amqp_transport_common_h_files
        ${iothub_client_ll_transport_h_files}
        ./inc/iothubtransport_amqp_common.h
        ./inc/iothubtransportamqp_auth.h
        ./inc/iothubtransport_amqp_connection.h
        ./inc/iothubtransportamqp_methods.h
        ./inc/uamqp_messaging.h
    )

    set(iothub_client_amqp_transport_c_files
        ${iothub_client_amqp_transport_common_c_files}
        ./src/iothubtransportamqp.c
    )

    set(iothub_client_amqp_transport_h_files
        ${iothub_client_amqp_transport_common_h_files}
        ./inc/iothubtransportamqp.h
    )    
    
    set(iothub_client_h_install_files
        ${iothub_client_h_install_files}
        ${iothub_client_amqp_transport_h_files}
    )

    if (${use_wsio})
        set(iothub_client_amqp_ws_transport_c_files
            ${iothub_client_amqp_transport_common_c_files}
            ./src/iothubtransportamqp_websockets.c
        )
        
        set(iothub_client_amqp_ws_transport_h_files
            ${iothub_client_amqp_transport_common_h_files}
            ./inc/iothubtransportamqp_websockets.h
        )
        
        set(iothub_client_h_install_files
            ${iothub_client_h_install_files}
            ./inc/iothubtransportamqp_websockets.h
        )
    endif()
endif()

if(${use_mqtt})
    if (${use_wsio})
        set(iothub_client_mqtt_ws_transport_c_files
            ${iothub_client_ll_transport_c_files}
            ./src/iothubtransport_mqtt_common.c
            ./src/iothubtransportmqtt_websockets.c
        )
        set(iothub_client_mqtt_ws_transport_h_files
            ${iothub_client_ll_transport_h_files}
            ./inc/iothubtransport_mqtt_common.h
            ./inc/iothubtransportmqtt_websockets.h
        )
    endif()

    set(iothub_client_mqtt_transport_c_files
        ${iothub_client_ll_transport_c_files}
        ./src/iothubtransport_mqtt_common.c
        ./src/iothubtransportmqtt.c
    )
    
    set(iothub_client_mqtt_transport_h_files
        ${iothub_client_ll_transport_h_files}
        ./inc/iothubtransport_mqtt_common.h
        ./inc/iothubtransportmqtt.h
    )
    
    set(iothub_client_h_install_files
        ${iothub_client_h_install_files}
        ${iothub_client_mqtt_transport_h_files}
    )
endif()

#these are the include folders
#the following "set" statetement exports across the project a global variable called SHARED_UTIL_INC_FOLDER that expands to whatever needs to included when using COMMON library

if(${use_http})
    set(IOTHUB_CLIENT_HTTP_TRANSPORT_INC_FOLDER ${CMAKE_CURRENT_LIST_DIR}/inc CACHE INTERNAL "this is what needs to be included if using iothub_client_http_transport lib" FORCE)
endif()

if(${use_mqtt})
    set(IOTHUB_CLIENT_MQTT_TRANSPORT_INC_FOLDER ${CMAKE_CURRENT_LIST_DIR}/inc CACHE INTERNAL "this is what needs to be included if using iothub_client_mqtt_transport lib" FORCE)
endif()

if(${use_amqp})
    set(IOTHUB_CLIENT_AMQP_TRANSPORT_INC_FOLDER ${CMAKE_CURRENT_LIST_DIR}/inc CACHE INTERNAL "this is what needs to be included if using iothub_client_amqp_transport lib" FORCE)
endif()

set(IOTHUB_CLIENT_INC_FOLDER ${CMAKE_CURRENT_LIST_DIR}/inc CACHE INTERNAL "this is what needs to be included if using iothub_client lib" FORCE)

if(NOT ${dont_use_uploadtoblob})
    include_directories(../parson)
endif()

include_directories(${AZURE_C_SHARED_UTILITY_INCLUDES})
include_directories(${SHARED_UTIL_INC_FOLDER})

if (WINCE)
include_directories(${SHARED_UTIL_INC_FOLDER}/azure_c_shared_utility/windowsce) #windowsce SDK doesn't have stdbool.h
ENDIF()


include_directories(${IOTHUB_CLIENT_INC_FOLDER})

IF(WIN32)
    #windows needs this define
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    add_definitions(-DGB_MEASURE_MEMORY_FOR_THIS -DGB_DEBUG_ALLOC)
  
  if (WINCE) # Be lax with WEC 2013 compiler
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W3")
    SET_SOURCE_FILES_PROPERTIES(src/iothub_client.c src/iothubtransport.c src/iothub_client_ll.c src/iothubtransporthttp.c src/blob.c PROPERTIES LANGUAGE CXX)
  ENDIF(WINCE)
ENDIF(WIN32)

if(${use_http})
    include_directories(${IOTHUB_CLIENT_HTTP_TRANSPORT_INC_FOLDER})
    add_library(iothub_client_http_transport 
        ${iothub_client_http_transport_c_files} 
        ${iothub_client_http_transport_h_files}
    )
    linkSharedUtil(iothub_client_http_transport)
    target_link_libraries(iothub_client_http_transport)
    set(iothub_client_libs
        ${iothub_client_libs}
        iothub_client_http_transport
    )
endif()

if(${use_amqp})
    include_directories(${UAMQP_INCLUDES})
    include_directories(${IOTHUB_CLIENT_AMQP_TRANSPORT_INC_FOLDER} ${UAMQP_INC_FOLDER})
    add_library(iothub_client_amqp_transport 
        ${iothub_client_amqp_transport_c_files} 
        ${iothub_client_amqp_transport_h_files}
    )
    linkSharedUtil(iothub_client_amqp_transport)
    target_link_libraries(iothub_client_amqp_transport)
    set(iothub_client_libs
        ${iothub_client_libs}
        iothub_client_amqp_transport
    )
	
	if (${use_wsio})
	    include_directories(${iothub_client_amqp_ws_transport_INC_FOLDER} ${UAMQP_INC_FOLDER})
		add_library(iothub_client_amqp_ws_transport 
			${iothub_client_amqp_ws_transport_c_files} 
			${iothub_client_amqp_ws_transport_h_files}
		)
		linkSharedUtil(iothub_client_amqp_ws_transport)
		linkWebSockets(iothub_client_amqp_ws_transport)
		target_link_libraries(iothub_client_amqp_ws_transport)
		set(iothub_client_libs
			${iothub_client_libs}
			iothub_client_amqp_ws_transport
		)
	endif()
endif()

if(${use_mqtt})
    include_directories(${UMQTT_INCLUDES})
    include_directories(${IOTHUB_CLIENT_MQTT_TRANSPORT_INC_FOLDER} ${MQTT_INC_FOLDER})
    add_library(iothub_client_mqtt_transport 
        ${iothub_client_mqtt_transport_c_files} 
        ${iothub_client_mqtt_transport_h_files}
    )
    linkSharedUtil(iothub_client_mqtt_transport)
    linkMqttLibrary(iothub_client_mqtt_transport)
    target_link_libraries(iothub_client_mqtt_transport)
    set(iothub_client_libs
        ${iothub_client_libs}
        iothub_client_mqtt_transport
    )
    if (${use_wsio})
        include_directories(${IOTHUB_CLIENT_MQTT_TRANSPORT_INC_FOLDER} ${MQTT_INC_FOLDER})
        add_library(iothub_client_mqtt_ws_transport 
            ${iothub_client_mqtt_ws_transport_c_files} 
            ${iothub_client_mqtt_ws_transport_h_files}
        )
        linkSharedUtil(iothub_client_mqtt_ws_transport)
        linkMqttLibrary(iothub_client_mqtt_ws_transport)
        linkWebSockets(iothub_client_mqtt_ws_transport)
        target_link_libraries(iothub_client_mqtt_ws_transport)
        set(iothub_client_libs
            ${iothub_client_libs}
            iothub_client_mqtt_ws_transport
        )
    endif()

endif()

include_directories(${IOTHUB_CLIENT_INC_FOLDER})

IF(WIN32)
    #windows needs this define
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    add_definitions(-DGB_MEASURE_MEMORY_FOR_THIS -DGB_DEBUG_ALLOC)
ENDIF(WIN32)

add_library(iothub_client 
    ${iothub_client_c_files} 
    ${iothub_client_h_files}
)
linkSharedUtil(iothub_client)

# Don't build samples under Win32 ARM for now
if(NOT ${skip_samples})
if(WIN32)
# Build samples for WEC 2013 for both ARM and x86 processsor types
  if(WINCE) 
    add_subdirectory(samples)
  else()
    if (NOT ${ARCHITECTURE} STREQUAL "ARM")
    add_subdirectory(samples)
    endif()
  endif()
    
else()
    add_subdirectory(samples)
endif()
endif()

if(NOT IN_OPENWRT)
    # Disable tests for OpenWRT
    if(NOT ${skip_unittests})
        add_subdirectory(tests)
    endif()
endif()

if(${use_installed_dependencies})
    #Set CMAKE_INSTALL_LIBDIR if not defined
    include(GNUInstallDirs)

    if(NOT DEFINED CMAKE_INSTALL_LIBDIR)
        set(CMAKE_INSTALL_LIBDIR "lib")
    endif()

    install(TARGETS ${iothub_client_libs} EXPORT azure_iot_sdksTargets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}/../bin
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/azureiot 
    )
    install(FILES ${iothub_client_h_install_files} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/azureiot)

else()
    message(WARNING "This package may only be installed when 'use_installed_dependencies' is ON")
endif()