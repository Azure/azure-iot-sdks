#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

#this is CMakeLists.txt for samples. There's noithig here, except redirections to 
#individual samples

add_subdirectory(iotdm_simple_sample)
add_subdirectory(iotdm_app_template)

if(NOT WIN32)
    add_subdirectory(iotdm_edison_sample)
endif()

# This sample depends on the iothub SDK components.  Only include it if we're building from way up there
if ("${CMAKE_PROJECT_NAME}" STREQUAL "azure_iot_sdks")
#        add_subdirectory(iotdm_2channel_sample)
endif()
