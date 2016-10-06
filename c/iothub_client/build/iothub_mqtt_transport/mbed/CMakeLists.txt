#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

cmake_minimum_required(VERSION 2.8.11)

set(shared_util_base_path ../../../../c-utility)
set(mbed_project_base "iothub_mqtt_transport" CACHE STRING "The item being built")
include (${shared_util_base_path}/tools/mbed_build_scripts/mbedbldtemplate.txt)
