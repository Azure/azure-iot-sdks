#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

if(${use_installed_dependencies})
    #These need to be set for the functions included by c-utility 
    set(SHARED_UTIL_SRC_FOLDER "${CMAKE_CURRENT_LIST_DIR}/c-utility/src" CACHE INTERNAL "")
    set(SHARED_UTIL_ADAPTER_FOLDER "${CMAKE_CURRENT_LIST_DIR}/c-utility/adapters")
    set(SHARED_UTIL_FOLDER "${CMAKE_CURRENT_LIST_DIR}/c-utility")
    set_platform_files("${CMAKE_CURRENT_LIST_DIR}/c-utility")
    if(NOT umock_c_FOUND)
        find_package(umock_c REQUIRED CONFIG)
        include_directories(${UMOCK_C_INCLUDES})
    endif()
endif()
