#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

#this is CMakeLists for sal. It only defines a global include 
cmake_minimum_required(VERSION 3.0)

#these are the include folders
#the following "set" statetement exports across the project a global variable called SAL_INC_FOLDER that expands to whatever needs to included when using sal header
set(SAL_INC_FOLDER ${CMAKE_CURRENT_LIST_DIR}/inc CACHE INTERNAL "this is what needs to be included if using sal header" FORCE)