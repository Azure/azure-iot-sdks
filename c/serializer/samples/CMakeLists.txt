#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

#this is CMakeLists.txt for serializer samples. There's nothing here, except redirections to 
#individual samples

if(${use_amqp})
	add_subdirectory(remote_monitoring)
	add_subdirectory(simplesample_amqp)
endif()

if(${use_http})
	add_subdirectory(simplesample_http)
	add_subdirectory(temp_sensor_anomaly)
endif()

if(${use_mqtt})
	add_subdirectory(simplesample_mqtt)
endif()

