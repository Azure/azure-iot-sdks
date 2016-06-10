#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

function(add_sample_directory whatIsBuilding)
	add_subdirectory(${whatIsBuilding})

	set_target_properties(${whatIsBuilding}
			   PROPERTIES
			   FOLDER "IoTHub_Samples")
endfunction()

add_sample_directory(iothub_messaging_ll_sample)
add_sample_directory(iothub_registrymanager_sample)
add_sample_directory(iothub_service_client_sample)
