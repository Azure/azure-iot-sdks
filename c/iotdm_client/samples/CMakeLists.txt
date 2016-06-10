#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

#this is CMakeLists.txt for samples. There's noithig here, except redirections to 
#individual samples

Option(build_edison_sample "set build_edison_sample to OFF to skip the edison sample" ON)

function(add_sample_directory whatIsBuilding)
	add_subdirectory(${whatIsBuilding})

	set_target_properties(${whatIsBuilding}
			   PROPERTIES
			   FOLDER "IotHub_DM_Samples")
endfunction()


add_sample_directory(iotdm_simple_sample)

if(${build_edison_sample} AND (NOT WIN32))
    add_subdirectory(iotdm_edison_sample)
endif()

