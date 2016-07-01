#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

#this is CMakeLists for service

compileAsC99()

if(${use_http})

set(PARSON_INC_FOLDER ${CMAKE_CURRENT_LIST_DIR}/iothub_service_client/parson CACHE INTERNAL "this is what needs to be included if using parson" FORCE)
include_directories(${PARSON_INC_FOLDER})

set(IOTHUB_CLIENT_INC_FOLDER ${CMAKE_CURRENT_LIST_DIR}/../iothub_client/inc CACHE INTERNAL "this is what needs to be included if using iothub_message" FORCE)
include_directories(${IOTHUB_CLIENT_INC_FOLDER})

set(IOTHUB_SERVICE_CLIENT_INC_FOLDER ${CMAKE_CURRENT_LIST_DIR}/iothub_service_client/inc CACHE INTERNAL "this is what needs to be included if using iothub_service_client_auth, iothub_registrymanager, iothub_messaging" FORCE)
include_directories(${IOTHUB_SERVICE_CLIENT_INC_FOLDER})

add_subdirectory(iothub_service_client)
endif()