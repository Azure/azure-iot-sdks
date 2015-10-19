#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

#this is CMakeLists.txt for gballoc_without_init_unittests
cmake_minimum_required(VERSION 3.0)

set(theseTestsName gballoc_without_init_unittests)

set(${theseTestsName}_cpp_files
${theseTestsName}.cpp
)

set(${theseTestsName}_c_files
gballoc_undertest.c
)

set(${theseTestsName}_h_files
)

build_test_artifacts(${theseTestsName} ON)
