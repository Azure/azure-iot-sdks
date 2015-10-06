#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

#this is CMakeLists for micromock unittests
cmake_minimum_required(VERSION 3.0)

set(micromock_unittests_cpp_files
stdafx.cpp
cmockvalueunittests.cpp
micromockcallcomparisonunittests.cpp
micromocktest.cpp
micromockvalidateargumentbufferunittests.cpp
nullargsstringificationunittests.cpp
#timediscretemicromocktestvoidvoid.cpp
#timediscretemicromocktest_stim.cpp
#timediscretemicromockwithreturnandparameters.cpp
)

set(micromock_unittests_c_files
#hypothetic_module.c
#micromocktest_with_ctest.c
)

set(micromock_unittests_h_files
#hypothetic_module.h
hypothetic_module_interfaces.h
stdafx.h
)

include_directories(${MICROMOCK_INC_FOLDER} ${TESTRUNNERSWITCHER_INC_FOLDER} ${CTEST_INC_FOLDER} $ENV{VCInstallDir}UnitTest/include)
link_directories($ENV{VCInstallDir}UnitTest/lib)

IF(WIN32)
	#windows needs this define
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	add_definitions(-DCPP_UNITTEST)
ENDIF(WIN32)


add_library(
micromock_unittests SHARED ${micromock_unittests_cpp_files} ${micromock_unittests_c_files} ${micromock_unittests_h_files}
)

target_link_libraries(micromock_unittests micromock ctest)