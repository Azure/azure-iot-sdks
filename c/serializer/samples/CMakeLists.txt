#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

#this is CMakeLists.txt for serializer samples. There's nothing here, except redirections to 
#individual samples

function(add_sample_directory whatIsBuilding)
	add_subdirectory(${whatIsBuilding})

	set_target_properties(${whatIsBuilding}
			   PROPERTIES
			   FOLDER "Serializer_Samples")
endfunction()

if(${use_amqp})
	add_sample_directory(remote_monitoring)
	add_sample_directory(simplesample_amqp)
endif()

if(${use_http})
	add_sample_directory(simplesample_http)
	add_sample_directory(temp_sensor_anomaly)
endif()

if(${use_mqtt})
	add_sample_directory(simplesample_mqtt)
endif()

