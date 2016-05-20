#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

#this is CMakeLists for service

compileAsC99()

set(IOTHUB_SERVICE_CLIENT_DEPS_PARSON_FOLDER ${CMAKE_CURRENT_LIST_DIR}/deps/parson CACHE INTERNAL "this is what needs to be included if using iothub_registrymanager, iothub_messaging" FORCE)
include_directories(${IOTHUB_SERVICE_CLIENT_DEPS_PARSON_FOLDER})

set(IOTHUB_CLIENT_INC_FOLDER ${CMAKE_CURRENT_LIST_DIR}/../iothub_client/inc CACHE INTERNAL "this is what needs to be included if using iothub_message" FORCE)
include_directories(${IOTHUB_CLIENT_INC_FOLDER})

add_subdirectory(iothub_service_client)
