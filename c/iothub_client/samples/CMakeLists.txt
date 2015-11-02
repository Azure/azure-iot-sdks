#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

#this is CMakeLists.txt for samples. There's noithig here, except redirections to 
#individual protocol samples

if(${use_amqp})
	add_subdirectory(iothub_client_sample_amqp)
endif()

if(${use_http})
	add_subdirectory(iothub_client_sample_http)
endif()

if(${use_mqtt})
	add_subdirectory(iothub_client_sample_mqtt)
endif()
