#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

#this is CMakeLists.txt for iothub_client_sample_mqtt
set(iothub_client_sample_mqtt_c_files
iothub_client_sample_mqtt.c
./windows/main.c
)

set(iothub_client_sample_mqtt_h_files
iothub_client_sample_mqtt.h
)

IF(WIN32)
	#windows needs this define
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
ENDIF(WIN32)

include_directories(.)

add_executable(iothub_client_sample_mqtt ${iothub_client_sample_mqtt_c_files} ${iothub_client_sample_mqtt_h_files})

#TODO: debug folder is hard coded and horrible. Should be something like configuration
target_link_libraries(iothub_client_sample_mqtt common iothub_client_mqtt_transport iothub_client "$ENV{paho_path}/org.eclipse.paho.mqtt.c/Windows Build/paho-mqtt3cs/Debug/paho-mqtt3cs.lib")
file(COPY "$ENV{paho_path}/org.eclipse.paho.mqtt.c/Windows Build/paho-mqtt3cs/Debug/paho-mqtt3cs.dll" DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug)
file(COPY "$ENV{openssldir}/bin/libeay32.dll" DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug)
file(COPY "$ENV{openssldir}/bin/ssleay32.dll" DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug)