#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

#this is CMakeLists.txt for samples. There's noithig here, except redirections to 
#individual samples

add_subdirectory(iotdm_simple_sample)

if(NOT WIN32)
    add_subdirectory(iotdm_edison_sample)
endif()

