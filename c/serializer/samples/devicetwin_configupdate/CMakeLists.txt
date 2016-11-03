#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

#this is CMakeLists.txt for devicetwin_configupdate

compileAsC99()

if(NOT ${use_mqtt})
    message(FATAL_ERROR "devicetwin_configupdate being generated without mqtt support")
endif()

set(devicetwin_configupdate_c_files
    devicetwin_configupdate.c
)

set(devicetwin_configupdate_h_files
)

IF(WIN32)
    #windows needs this define
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
ENDIF(WIN32)

include_directories(. ${SERIALIZER_INC_FOLDER} ${SHARED_UTIL_INC_FOLDER} ${IOTHUB_CLIENT_INC_FOLDER})

add_executable(devicetwin_configupdate ${devicetwin_configupdate_c_files} ${devicetwin_configupdate_h_files})
target_link_libraries(devicetwin_configupdate iothub_client_mqtt_transport serializer iothub_client)

linkSharedUtil(devicetwin_configupdate)
