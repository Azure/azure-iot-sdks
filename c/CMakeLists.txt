#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

cmake_minimum_required(VERSION 3.0)
project(everything)

enable_testing()

add_subdirectory(testtools)

add_subdirectory(common)

add_subdirectory(iothub_client)
add_subdirectory(serializer)