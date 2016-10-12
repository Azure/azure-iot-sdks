#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

Option(edison "set edison to ON to build the DM edison sample" OFF)

cmake_minimum_required(VERSION 2.8.11)

project(iothub_client_sample_firmware_update)

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

compileAsC99()

if(WIN32)  
	message(FATAL_ERROR "iothub_client_sample_firmware_update can not be generated on windows")
endif()  

set(AZUREIOT_INC_FOLDER "pi_device" "." "/usr/include/azureiot")

include_directories(${AZUREIOT_INC_FOLDER})

set(iothub_client_sample_mqtt_dm_c_files
	iothub_client_sample_mqtt_dm.c
	pi_device/pi.c
)
set(iothub_client_sample_mqtt_dm_h_files
	iothub_client_sample_mqtt_dm.h
)

add_executable(firmware_update ${iothub_client_sample_mqtt_dm_c_files} ${iothub_client_sample_mqtt_dm_h_files})

target_link_libraries(firmware_update
	iothub_client
	iothub_client_mqtt_transport
	serializer
	aziotsharedutil
	umqtt
	pthread
	curl
	ssl
	crypto
	m
)

set(iothub_install_files pi_device/iothub_client_sample_firmware_update.service .device_connection_string)

install (TARGETS firmware_update DESTINATION "/usr/share/iothub_client_sample")
install (FILES ${iothub_install_files} DESTINATION "/usr/share/iothub_client_sample")

