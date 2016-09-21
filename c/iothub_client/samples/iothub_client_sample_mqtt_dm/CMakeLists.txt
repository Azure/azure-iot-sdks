#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

#this is CMakeLists.txt for iothub_client_sample_mqtt_dm
Option(edison "set edison to ON to build the DM edison sample" OFF)

function(add_sample_directory whatIsBuilding)
    add_subdirectory(${whatIsBuilding})

    set_target_properties(${whatIsBuilding}
               PROPERTIES
               FOLDER "IoTHub_Samples")
endfunction()

if(NOT ${use_mqtt})
    message(FATAL_ERROR "iothub_client_sample_mqtt_dm being generated without mqtt support")
else()
    if (${edison})
        add_subdirectory(Edison)
    endif()
    if (${pi})
        add_subdirectory(Pi)
    endif()
endif()
