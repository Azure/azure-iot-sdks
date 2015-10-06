#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

#this is CMakeLists.txt for simplesample_mqtt sample
set(simplesample_mqtt_c_files
simplesample_mqtt.c
./windows/main.c
)

set(simplesample_mqtt_h_files
simplesample_mqtt.h
)

IF(WIN32)
	#windows needs this define
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
ENDIF(WIN32)

include_directories(. ${COMMON_INC_FOLDER} ${IOTHUB_CLIENT_INC_FOLDER})

add_executable(simplesample_mqtt ${simplesample_mqtt_c_files} ${simplesample_mqtt_h_files})
#TODO: debug folder is hard coded and horrible. Should be something like configuration
#TODO: this should be a global function "install_proton" or something
target_link_libraries(simplesample_mqtt serializer iothub_client iothub_client_mqtt_transport common "$ENV{paho_path}/org.eclipse.paho.mqtt.c/Windows Build/paho-mqtt3cs/Debug/paho-mqtt3cs.lib")

file(COPY "$ENV{paho_path}/org.eclipse.paho.mqtt.c/Windows Build/paho-mqtt3cs/Debug/paho-mqtt3cs.dll" DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug)
file(COPY "$ENV{openssldir}/bin/libeay32.dll" DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug)
file(COPY "$ENV{openssldir}/bin/ssleay32.dll" DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug)