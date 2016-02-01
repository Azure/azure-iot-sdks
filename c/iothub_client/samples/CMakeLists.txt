#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

#this is CMakeLists.txt for samples. There's noithig here, except redirections to 
#individual protocol samples

function(add_sample_directory whatIsBuilding)
	add_subdirectory(${whatIsBuilding})

	set_target_properties(${whatIsBuilding}
			   PROPERTIES
			   FOLDER "IotHub_Samples")
endfunction()

if(${use_http})
	add_sample_directory(iothub_client_sample_http)
endif()

if(${use_mqtt})
	add_sample_directory(iothub_client_sample_mqtt)
endif()

if(${use_amqp})
	add_sample_directory(iothub_client_sample_amqp)
endif()