#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

#this is CMakeLists.txt for remote_monitoring sample
set(remote_monitoring_c_files
remote_monitoring.c
./windows/main.c
)

set(remote_monitoring_h_files
remote_monitoring.h
)

IF(WIN32)
	#windows needs this define
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
ENDIF(WIN32)

include_directories(. ${IOTHUB_CLIENT_INC_FOLDER})

add_executable(remote_monitoring ${remote_monitoring_c_files} ${remote_monitoring_h_files})
#TODO: debug folder is hard coded and horrible. Should be something like configuration
#TODO: this should be a global function "install_proton" or something
target_link_libraries(remote_monitoring serializer iothub_client iothub_client_amqp_transport common $ENV{proton_path}/qpid-proton/build/proton-c/Debug/qpid-protond.lib)
file(COPY $ENV{proton_path}/qpid-proton/build/proton-c/Debug/qpid-protond.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug)