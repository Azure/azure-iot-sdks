#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

cmake_minimum_required(VERSION 2.8)
#this is CMakeLists for iotdm_client

macro(compileAsC99)
  if (CMAKE_VERSION VERSION_LESS "3.1")
    if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
      set (CMAKE_C_FLAGS "--std=c99 ${CMAKE_C_FLAGS}")
    if (NOT IN_OPENWRT)
      set (CMAKE_CXX_FLAGS "--std=c++11 ${CMAKE_CXX_FLAGS}")
    endif()
    endif()
  else()
    set (CMAKE_C_STANDARD 99)
    set (CMAKE_CXX_STANDARD 11)
  endif()
endmacro(compileAsC99)

macro(compileAsC11)
  if (CMAKE_VERSION VERSION_LESS "3.1")
    if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
      set (CMAKE_C_FLAGS "--std=c11 ${CMAKE_C_FLAGS}")
      set (CMAKE_C_FLAGS "-D_POSIX_C_SOURCE=200112L ${CMAKE_C_FLAGS}")
      set (CMAKE_CXX_FLAGS "--std=c++11 ${CMAKE_CXX_FLAGS}")
    endif()
  else()
    set (CMAKE_C_STANDARD 11)
    set (CMAKE_CXX_STANDARD 11)
  endif()
endmacro(compileAsC11)

compileAsC99()

option(no_tls "set no_tls:BOOL=ON to disable TLS in the DM client (OFF by default)" OFF)

add_definitions(-DLWM2M_CLIENT_MODE -DLWM2M_WITH_LOGS -DLWM2M_LITTLE_ENDIAN -DCOAP_TCP -DCOAP_MAX_HEADER_SIZE=370 -DMSFT_LWM2M_CLIENT)

set(LIBLWM2M_DIR ${CMAKE_CURRENT_LIST_DIR}/../wakaama)
set(LWM2M_OBJECTS_DIR ${CMAKE_CURRENT_LIST_DIR}/lwm2m_objects)
set(IOTHUB_CLIENT_SRC_DIR ${CMAKE_CURRENT_LIST_DIR}/src)

set(wakaama_c_files
    ${LIBLWM2M_DIR}/core/bootstrap.c
    ${LIBLWM2M_DIR}/core/json.c
    ${LIBLWM2M_DIR}/core/liblwm2m.c
    ${LIBLWM2M_DIR}/core/list.c
    ${LIBLWM2M_DIR}/core/management.c
    ${LIBLWM2M_DIR}/core/objects.c
    ${LIBLWM2M_DIR}/core/observe.c
    ${LIBLWM2M_DIR}/core/packet.c
    ${LIBLWM2M_DIR}/core/registration.c
    ${LIBLWM2M_DIR}/core/tlv.c
    ${LIBLWM2M_DIR}/core/transaction.c
    ${LIBLWM2M_DIR}/core/uri.c
    ${LIBLWM2M_DIR}/core/utils.c
    ${LIBLWM2M_DIR}/core/er-coap-13/er-coap-13.c
    ${LIBLWM2M_DIR}/platforms/${CMAKE_SYSTEM_NAME}/platform.c
)

set(wakaama_h_files
    ${LIBLWM2M_DIR}/core/internals.h
    ${LIBLWM2M_DIR}/core/liblwm2m.h
    ${LIBLWM2M_DIR}/core/er-coap-13/er-coap-13.h
)

set(iotdm_client_c_files
    ${IOTHUB_CLIENT_SRC_DIR}/iotdm_client.c
    ${IOTHUB_CLIENT_SRC_DIR}/iotdm_connect.c
)

if(${no_tls})
    set(iotdm_client_c_files ${iotdm_client_c_files}
        ./src/dm_create_socketio.c
    )
else()
    set(iotdm_client_c_files ${iotdm_client_c_files}
        ./src/dm_create_tlsio.c
    )
endif()

set(iotdm_client_h_files
    ./inc/iothub_lwm2m.h 
    ${IOTHUB_CLIENT_SRC_DIR}/iotdm_internal.h
)

set(iotdm_lwm2m_objects_c_files
    ${LWM2M_OBJECTS_DIR}/object_global.c
    ${LWM2M_OBJECTS_DIR}/object_security.c
    ${LWM2M_OBJECTS_DIR}/object_server.c
    ${LWM2M_OBJECTS_DIR}/lwm2m_object_list.c
    ${LWM2M_OBJECTS_DIR}/iotdm_dispatchers.c
    ${LWM2M_OBJECTS_DIR}/iotdm_dispatcher_list.c
    ${LWM2M_OBJECTS_DIR}/config_object.c
    ${LWM2M_OBJECTS_DIR}/config_object_x.c
    ${LWM2M_OBJECTS_DIR}/device_object.c
    ${LWM2M_OBJECTS_DIR}/device_object_x.c
    ${LWM2M_OBJECTS_DIR}/firmwareupdate_object.c
    ${LWM2M_OBJECTS_DIR}/firmwareupdate_object_x.c
    ${LWM2M_OBJECTS_DIR}/lwm2mserver_object.c
    ${LWM2M_OBJECTS_DIR}/lwm2mserver_object_x.c
)

set(iotdm_lwm2m_objects_h_files
    ${LWM2M_OBJECTS_DIR}/iotdm_lwm2m_client_apis.h
    ${LWM2M_OBJECTS_DIR}/iotdm_dispatchers.h
    ${LWM2M_OBJECTS_DIR}/config_object.h
    ${LWM2M_OBJECTS_DIR}/device_object.h
    ${LWM2M_OBJECTS_DIR}/firmwareupdate_object.h
    ${LWM2M_OBJECTS_DIR}/lwm2mserver_object.h
)

#these are the include folders
#the following "set" statetement exports across the project a global variable called IOTDM_CLIENT_INC_FOLDER that expands to whatever needs to included when using iotdm_client library
set(IOTDM_CLIENT_INC_FOLDER ${CMAKE_CURRENT_LIST_DIR}/inc CACHE INTERNAL "this is what needs to be included if using iotdm_client lib" FORCE)

include_directories(${LIBLWM2M_DIR}/core)
include_directories(${LIBLWM2m_DIR}/core/er-coap-13)
include_directories(${IOTDM_CLIENT_INC_FOLDER})
include_directories(${SHARED_UTIL_INC_FOLDER})
include_directories(${IOTHUB_CLIENT_INC_FOLDER})
include_directories(${LWM2M_OBJECTS_DIR})
include_directories(${IOTHUB_CLIENT_SRC_DIR})

if(WIN32)
    #add a VC filter.
    source_group("lwm2m_objects" FILES ${iotdm_lwm2m_objects_c_files} ${iotdm_lwm2m_objects_h_files})
    source_group("wakaama" FILES ${wakaama_c_files} ${wakaama_h_files})

    #windows needs this define
    add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_DEPRECATE)
    add_definitions(-DGB_MEASURE_MEMORY_FOR_THIS -DGB_DEBUG_ALLOC)
else()
    add_definitions(-D_POSIX_C_SOURCE=200112L)
    add_definitions("-Wno-unused-result")
    add_definitions("-Wno-implicit-function-declaration")
    add_definitions("-Wno-int-to-pointer-cast")
    add_definitions("-std=c99")
endif(WIN32)

add_library(iotdm_client
    ${iotdm_client_c_files}
    ${iotdm_client_h_files}
    ${iotdm_lwm2m_objects_c_files}
    ${iotdm_lwm2m_objects_h_files}
    ${wakaama_c_files}
)

add_subdirectory(samples)

if ("${CMAKE_PROJECT_NAME}" STREQUAL "azure_iot_sdks")
    add_subdirectory(tests)
endif()

