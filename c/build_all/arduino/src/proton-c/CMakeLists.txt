#
# Licensed to the Apache Software Foundation (ASF) under one
# or more contributor license agreements.  See the NOTICE file
# distributed with this work for additional information
# regarding copyright ownership.  The ASF licenses this file
# to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance
# with the License.  You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing,
# software distributed under the License is distributed on an
# "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
# KIND, either express or implied.  See the License for the
# specific language governing permissions and limitations
# under the License.
#

include(CheckLibraryExists)
include(CheckSymbolExists)

include(soversion.cmake)

SET(IN_OPENWRT $ENV{IN_OPENWRT})
	
	#IF (IN_OPENWRT)
	        ADD_DEFINITIONS("$ENV{TARGET_LDFLAGS}" "$ENV{TARGET_CPPFLAGS}" "$ENV{TARGET_CFLAGS}")
	        INCLUDE_DIRECTORIES("$ENV{TOOLCHAIN_DIR}/usr/include" "$ENV{TARGET_LDFLAGS}" "$ENV{TARGET_CPPFLAGS}" "$ENV{TARGET_CFLAGS}")
	        INCLUDE_DIRECTORIES("$ENV{TOOLCHAIN_DIR}/usr/include/openssl")
	        INCLUDE_DIRECTORIES("$ENV{TOOLCHAIN_DIR}/usr/include/uuid")
	#ENDIF(IN_OPENWRT)

if(WIN32 AND NOT CYGWIN)
  # linking against Windows native libraries, including mingw
  set (PN_WINAPI TRUE)
  set (PLATFORM_LIBS ws2_32 Rpcrt4)
endif(WIN32 AND NOT CYGWIN)

# Can't use ${CMAKE_VERSION) as it is not available in all versions of cmake 2.6
if ("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION}" VERSION_LESS "2.8.0")
  # OPTIONAL does not exist in install before 2.8 so always make docs and install
  set (OPTIONAL_ARG "")
  add_custom_target(docs ALL)
  # There are bugs in the OpenSSL detection that mean -lcrypto is missed from the link line
  # so turn off unknown symbol warnings
  set (NOENABLE_UNDEFINED_ERROR ON)
else()
  set (OPTIONAL_ARG OPTIONAL)
  add_custom_target(docs)
endif()


# Set the default SSL/TLS implementation
find_package(OpenSSL)

find_package (PythonInterp REQUIRED)

if(PN_WINAPI)
  set(ssl_impl schannel)
  set(ssl_providers "'none','schannel','openssl'")
else(PN_WINAPI)
  set(ssl_impl, none)
  set(ssl_providers "'none','openssl'")
endif(PN_WINAPI)
if (OPENSSL_FOUND)
  set(ssl_impl openssl)
endif (OPENSSL_FOUND)
set(SSL_IMPL ${ssl_impl} CACHE STRING "Library to use for SSL/TLS support. Valid values: ${ssl_providers}")
mark_as_advanced (SSL_IMPL)

configure_file (
  "${CMAKE_CURRENT_SOURCE_DIR}/include/proton/version.h.in"
  "${CMAKE_CURRENT_BINARY_DIR}/include/proton/version.h"
  )

include_directories ("${CMAKE_CURRENT_BINARY_DIR}/src")
include_directories ("${CMAKE_CURRENT_BINARY_DIR}/include")
include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/src")
include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/include")

#include_directories ("/home/ming/target/usr/include")

# TODO: This is only needed because the proton executable can use getopt on windows
# if/when this executable gets sorted out remove
include_directories ("${CMAKE_SOURCE_DIR}/examples/include")

add_custom_command (
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/src/encodings.h
  COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/env.py PYTHONPATH=${CMAKE_CURRENT_SOURCE_DIR} ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/src/codec/encodings.h.py > ${CMAKE_CURRENT_BINARY_DIR}/src/encodings.h
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/codec/encodings.h.py
  )

add_custom_command (
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/src/protocol.h
  COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/env.py PYTHONPATH=${CMAKE_CURRENT_SOURCE_DIR} ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/src/protocol.h.py > ${CMAKE_CURRENT_BINARY_DIR}/src/protocol.h
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/protocol.h.py
  )

# Select driver
if(PN_WINAPI)
  set (pn_io_impl src/windows/io.c src/windows/iocp.c src/windows/write_pipeline.c)
  set (pn_selector_impl src/windows/selector.c)
  set (pn_driver_impl src/windows/driver.c)
else(PN_WINAPI)
  set (pn_io_impl src/posix/io.c)
  set (pn_selector_impl src/posix/selector.c)
  set (pn_driver_impl src/posix/driver.c)
endif(PN_WINAPI)

# Link in openssl if present
if (SSL_IMPL STREQUAL openssl)
  set (pn_driver_ssl_impl src/ssl/openssl.c)
  include_directories ("${OPENSSL_INCLUDE_DIR}")
  set (SSL_LIB ${OPENSSL_LIBRARIES})
else (SSL_IMPL STREQUAL openssl)
  if (SSL_IMPL STREQUAL schannel)
    set (pn_driver_ssl_impl src/windows/schannel.c)
    set (SSL_LIB Crypt32.lib Secur32.lib)
  else (SSL_IMPL STREQUAL schannel)
    set (pn_driver_ssl_impl src/ssl/ssl_stub.c)
  endif (SSL_IMPL STREQUAL schannel)
endif (SSL_IMPL STREQUAL openssl)

# First check whether we get clock_gettime without any special library linked
CHECK_SYMBOL_EXISTS(clock_gettime "time.h" CLOCK_GETTIME_IN_LIBC)
if (CLOCK_GETTIME_IN_LIBC)
  list(APPEND PLATFORM_DEFINITIONS "USE_CLOCK_GETTIME")
else (CLOCK_GETTIME_IN_LIBC)
  CHECK_LIBRARY_EXISTS (rt clock_gettime "" CLOCK_GETTIME_IN_RT)
  if (CLOCK_GETTIME_IN_RT)
    set (TIME_LIB rt)
    list(APPEND PLATFORM_DEFINITIONS "USE_CLOCK_GETTIME")
  else (CLOCK_GETTIME_IN_RT)
    CHECK_SYMBOL_EXISTS(GetSystemTimeAsFileTime "windows.h" WINDOWS_FILETIME)
    if (WINDOWS_FILETIME)
      list(APPEND PLATFORM_DEFINITIONS "USE_WIN_FILETIME")
    else (WINDOWS_FILETIME)
      list(APPEND PLATFORM_DEFINITIONS "USE_GETTIMEOFDAY")
    endif (WINDOWS_FILETIME)
  endif (CLOCK_GETTIME_IN_RT)
endif (CLOCK_GETTIME_IN_LIBC)

CHECK_SYMBOL_EXISTS(uuid_generate "uuid/uuid.h" UUID_GENERATE_IN_LIBC)
if (UUID_GENERATE_IN_LIBC)
  list(APPEND PLATFORM_DEFINITIONS "USE_UUID_GENERATE")
else (UUID_GENERATE_IN_LIBC)
  CHECK_LIBRARY_EXISTS (uuid uuid_generate "" UUID_GENERATE_IN_UUID)
  if (UUID_GENERATE_IN_UUID)
    set (UUID_LIB uuid)
    list(APPEND PLATFORM_DEFINITIONS "USE_UUID_GENERATE")
  else (UUID_GENERATE_IN_UUID)
    CHECK_SYMBOL_EXISTS(uuid_create "uuid.h" UUID_CREATE_IN_LIBC)
    if (UUID_CREATE_IN_LIBC)
      list(APPEND PLATFORM_DEFINITIONS "USE_UUID_CREATE")
    else (UUID_CREATE_IN_LIBC)
      CHECK_SYMBOL_EXISTS(UuidToString "rpc.h" WIN_UUID)
      if (WIN_UUID)
        list(APPEND PLATFORM_DEFINITIONS "USE_WIN_UUID")
      else (WIN_UUID)
        message(FATAL_ERROR "No Uuid API found")
      endif (WIN_UUID)
    endif (UUID_CREATE_IN_LIBC)
  endif (UUID_GENERATE_IN_UUID)
endif (UUID_GENERATE_IN_LIBC)

if (PN_WINAPI)
  CHECK_SYMBOL_EXISTS(strerror_s "string.h" STRERROR_S_IN_WINAPI)
  if (STRERROR_S_IN_WINAPI)
    list(APPEND PLATFORM_DEFINITIONS "USE_STRERROR_S")
  else (STRERROR_S_IN_WINAPI)
    if (MINGW)
      message (STATUS, "NOTE: your MinGW version lacks a thread safe strerror")
      list(APPEND PLATFORM_DEFINITIONS "USE_OLD_STRERROR")
    endif (MINGW)
  endif (STRERROR_S_IN_WINAPI)
else (PN_WINAPI)
  CHECK_SYMBOL_EXISTS(strerror_r "string.h" STRERROR_R_IN_LIBC)
  if (STRERROR_R_IN_LIBC)
    list(APPEND PLATFORM_DEFINITIONS "USE_STRERROR_R")
  endif (STRERROR_R_IN_LIBC)
endif (PN_WINAPI)

CHECK_SYMBOL_EXISTS(atoll "stdlib.h" C99_ATOLL)
if (C99_ATOLL)
  list(APPEND PLATFORM_DEFINITIONS "USE_ATOLL")
else (C99_ATOLL)
  CHECK_SYMBOL_EXISTS(_atoi64 "stdlib.h" WINAPI_ATOI64)
  if (WINAPI_ATOI64)
    list(APPEND PLATFORM_DEFINITIONS "USE_ATOI64")
  else (WINAPI_ATOI64)
    message(FATAL_ERROR "No atoll API found")
  endif (WINAPI_ATOI64)
endif (C99_ATOLL)

# Try to keep any platform specific overrides together here:

# MacOS has a bunch of differences in build tools and process and so we have to turn some things
# off if building there:
if (APPLE)
  set (NOBUILD_PHP ON)
  set (NOENABLE_WARNING_ERROR ON)
  set (NOENABLE_UNDEFINED_ERROR ON)
endif (APPLE)

# Add options here called <whatever> they will turn into "ENABLE_<whatever" and can be
# overridden on a platform specific basis above by NOENABLE_<whatever>
set (OPTIONS WARNING_ERROR UNDEFINED_ERROR)

foreach (OPTION ${OPTIONS})
  if (NOT "NOENABLE_${OPTION}")
    set ("DEFAULT_${OPTION}" ON)
  endif (NOT "NOENABLE_${OPTION}")
endforeach (OPTION)

# And add the option here too with help text
option(ENABLE_WARNING_ERROR "Consider compiler warnings to be errors" ${DEFAULT_WARNING_ERROR})
option(ENABLE_UNDEFINED_ERROR "Check for unresolved library symbols" ${DEFAULT_UNDEFINED_ERROR})

# Set any additional compiler specific flags
if (CMAKE_COMPILER_IS_GNUCC)
  if (ENABLE_WARNING_ERROR)
    set (WERROR "-Werror")
  endif (ENABLE_WARNING_ERROR)
  set (COMPILE_WARNING_FLAGS "${WERROR} -Wall -pedantic-errors")
  if (NOT BUILD_WITH_CXX)
    set (COMPILE_WARNING_FLAGS "${COMPILE_WARNING_FLAGS} -Wstrict-prototypes")
    set (COMPILE_LANGUAGE_FLAGS "-std=c99")
    set (COMPILE_PLATFORM_FLAGS "-std=gnu99")

    execute_process(COMMAND ${CMAKE_C_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION
      OUTPUT_STRIP_TRAILING_WHITESPACE)
    if (${GCC_VERSION} VERSION_LESS "4.3.0")
      # Only a concern if contibuting code back.
      message (STATUS "Old gcc version detected.  C++ compatibility checks disabled")
    else (${GCC_VERSION} VERSION_LESS "4.3.0")
      set (COMPILE_WARNING_FLAGS "${COMPILE_WARNING_FLAGS} -Wc++-compat -Wvla -Wsign-compare -Wwrite-strings")
    endif (${GCC_VERSION} VERSION_LESS "4.3.0")
  else (NOT BUILD_WITH_CXX)
    # allow "%z" format specifier
    set (COMPILE_WARNING_FLAGS "${COMPILE_WARNING_FLAGS} -Wno-format")
  endif (NOT BUILD_WITH_CXX)

  if (ENABLE_UNDEFINED_ERROR)
    set (CATCH_UNDEFINED "-Wl,--no-undefined")
    set (ALLOW_UNDEFINED "-Wl,--allow-shlib-undefined")
  endif (ENABLE_UNDEFINED_ERROR)
endif (CMAKE_COMPILER_IS_GNUCC)

if (MSVC)
    set(CMAKE_DEBUG_POSTFIX "d")
    add_definitions(
        /wd4244
        /wd4267
        /wd4800
        /wd4996
    )
endif (MSVC)

macro (pn_absolute_install_dir NAME VALUE PREFIX)
  if(IS_ABSOLUTE ${VALUE})
    set(${NAME} "${VALUE}")
  elseif(IS_ABSOLUTE ${PREFIX})
    set(${NAME} "${PREFIX}/${VALUE}")
  else()
    set(${NAME} "${CMAKE_BINARY_DIR}/${PREFIX}/${VALUE}")
  endif(IS_ABSOLUTE ${VALUE})
  get_filename_component(${NAME} ${${NAME}} ABSOLUTE)
endmacro()

find_package(SWIG)
if (SWIG_FOUND)
  add_subdirectory(bindings)
endif (SWIG_FOUND)

add_subdirectory(docs/api)
add_subdirectory(docs/man)
add_subdirectory(../tests/tools/apps/c ../tests/tools/apps/c)

set (qpid-proton-platform
  ${pn_io_impl}
  ${pn_selector_impl}
  ${pn_driver_impl}
  src/platform.c
  ${pn_driver_ssl_impl}
  )

set (qpid-proton-core
  src/object/object.c
  src/object/list.c
  src/object/map.c
  src/object/string.c
  src/object/iterator.c

  src/util.c
  src/url.c
  src/error.c
  src/buffer.c
  src/parser.c
  src/scanner.c
  src/types.c

  src/framing/framing.c

  src/codec/codec.c
  src/codec/decoder.c
  src/codec/encoder.c

  src/dispatcher/dispatcher.c
  src/engine/engine.c
  src/events/event.c
  src/transport/transport.c
  src/message/message.c
  src/sasl/sasl.c

  src/messenger/messenger.c
  src/messenger/subscription.c
  src/messenger/store.c
  src/messenger/transform.c
  src/selectable.c

  ${CMAKE_CURRENT_BINARY_DIR}/src/encodings.h
  ${CMAKE_CURRENT_BINARY_DIR}/src/protocol.h
  )

set_source_files_properties (
  ${qpid-proton-core}
  PROPERTIES
  COMPILE_FLAGS "${COMPILE_WARNING_FLAGS} ${COMPILE_LANGUAGE_FLAGS}"
  )

set_source_files_properties (
  ${qpid-proton-platform}
  PROPERTIES
  COMPILE_FLAGS "${COMPILE_WARNING_FLAGS} ${COMPILE_PLATFORM_FLAGS}"
  COMPILE_DEFINITIONS "${PLATFORM_DEFINITIONS}"
  )

add_library (
  qpid-proton SHARED

  ${qpid-proton-core}
  ${qpid-proton-platform}
  )

target_link_libraries (qpid-proton ${UUID_LIB} ${SSL_LIB} ${TIME_LIB} ${PLATFORM_LIBS})

set_target_properties (
  qpid-proton
  PROPERTIES
  VERSION   "${PN_LIB_SOMAJOR}.${PN_LIB_SOMINOR}"
  SOVERSION "${PN_LIB_SOMAJOR}"
  LINK_FLAGS "${CATCH_UNDEFINED}"
  )

if (MSVC)
  # guard against use of C99 violating functions on Windows
  include(WindowsC99CheckDef)
endif(MSVC)

add_executable (proton src/proton.c)
target_link_libraries (proton qpid-proton)

add_executable (proton-dump src/proton-dump.c)
target_link_libraries (proton-dump qpid-proton)

set_target_properties (
  proton proton-dump
  PROPERTIES
  COMPILE_FLAGS "${COMPILE_WARNING_FLAGS} ${COMPILE_PLATFORM_FLAGS}"
  )

macro(pn_c_files)
  foreach (src_file ${ARGN})
    if (${src_file} MATCHES "^.*[.]c$")
      if (BUILD_WITH_CXX)
        # tell CMake to use C++ for proton source files ending in ".c"
        set_source_files_properties (${src_file} PROPERTIES LANGUAGE CXX)
      endif (BUILD_WITH_CXX)
    endif (${src_file} MATCHES "^.*[.]c$")
  endforeach (src_file)
endmacro(pn_c_files)

pn_c_files (${qpid-proton-core} ${qpid-proton-platform} src/proton.c src/proton-dump.c)

# Install executables and libraries
install (TARGETS proton proton-dump
  RUNTIME DESTINATION bin
  ARCHIVE DESTINATION ${LIB_INSTALL_DIR}
  LIBRARY DESTINATION ${LIB_INSTALL_DIR})
install (TARGETS qpid-proton
  EXPORT  proton
  RUNTIME DESTINATION bin
  ARCHIVE DESTINATION ${LIB_INSTALL_DIR}
  LIBRARY DESTINATION ${LIB_INSTALL_DIR})

# Install windows qpid-proton pdb files
if (MSVC)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/Debug/qpid-proton${CMAKE_DEBUG_POSTFIX}.pdb
    DESTINATION bin 
    CONFIGURATIONS Debug
    OPTIONAL)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/RelWithDebInfo/qpid-proton.pdb
    DESTINATION bin 
    CONFIGURATIONS RelWithDebInfo
    OPTIONAL)
endif (MSVC)

# Install header files
file(GLOB headers "include/proton/*.[hi]")
install (FILES ${headers} DESTINATION ${INCLUDE_INSTALL_DIR}/proton)
install (FILES  ${CMAKE_CURRENT_BINARY_DIR}/include/proton/version.h
         DESTINATION ${INCLUDE_INSTALL_DIR}/proton)

pn_absolute_install_dir(PREFIX "." ${CMAKE_INSTALL_PREFIX})
pn_absolute_install_dir(EXEC_PREFIX "." ${CMAKE_INSTALL_PREFIX})
pn_absolute_install_dir(LIBDIR ${LIB_INSTALL_DIR} ${CMAKE_INSTALL_PREFIX})
pn_absolute_install_dir(INCLUDEDIR ${INCLUDE_INSTALL_DIR} ${CMAKE_INSTALL_PREFIX})

# Pkg config file
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/src/libqpid-proton.pc.in
  ${CMAKE_CURRENT_BINARY_DIR}/libqpid-proton.pc @ONLY)
install (FILES 
  ${CMAKE_CURRENT_BINARY_DIR}/libqpid-proton.pc
  DESTINATION ${LIB_INSTALL_DIR}/pkgconfig)

if (DEFINED CMAKE_IMPORT_LIBRARY_PREFIX)
set(PROTONLIB ${CMAKE_IMPORT_LIBRARY_PREFIX}qpid-proton${CMAKE_IMPORT_LIBRARY_SUFFIX})
set(PROTONLIBDEBUG ${CMAKE_IMPORT_LIBRARY_PREFIX}qpid-proton${CMAKE_DEBUG_POSTFIX}${CMAKE_IMPORT_LIBRARY_SUFFIX})
else ()
set(PROTONLIB ${CMAKE_SHARED_LIBRARY_PREFIX}qpid-proton${CMAKE_SHARED_LIBRARY_SUFFIX})
set(PROTONLIBDEBUG ${CMAKE_SHARED_LIBRARY_PREFIX}qpid-proton${CMAKE_DEBUG_POSTFIX}${CMAKE_SHARED_LIBRARY_SUFFIX})
endif ()

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/src/ProtonConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/ProtonConfig.cmake @ONLY)
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/src/ProtonConfigVersion.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/ProtonConfigVersion.cmake @ONLY)
install (FILES
  ${CMAKE_CURRENT_BINARY_DIR}/ProtonConfig.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/ProtonConfigVersion.cmake
  DESTINATION ${LIB_INSTALL_DIR}/cmake/Proton)

# CTest

  # suffix for Visual Studio targets.  Only support RelWithDebInfo for now.
if (MSVC)
  set (bld_suffix "/RelWithDebInfo")
endif(MSVC)

if (CMAKE_SYSTEM_NAME STREQUAL Windows)

  # Substitute windows separators in path and remove empty entries.
  # Replace necessary colons after windows drive letters.
  function(set_path result path)
    string (REGEX REPLACE ":" ";" path "${path}")
    string (REGEX REPLACE ";;" ";" path "${path}")
    string (REGEX REPLACE "^;" "" path "${path}")
    string (REGEX REPLACE "\\\\" "/" path "${path}")
    string (REGEX REPLACE ";([a-zA-Z]);" ";\\1:" path "${path}")
    string (REGEX REPLACE "^([a-zA-Z]);" "\\1:" path "${path}")
    set (${result} ${path} PARENT_SCOPE)
  endfunction()

else (CMAKE_SYSTEM_NAME STREQUAL Windows)

  # Set variable result to path, with empty entries removed
  function(set_path result path)
    string (REGEX REPLACE "^:" "" path "${path}")
    string (REGEX REPLACE "::" ":" path "${path}")
    set (${result} ${path} PARENT_SCOPE)
  endfunction()

endif (CMAKE_SYSTEM_NAME STREQUAL Windows)


set (env_py "${CMAKE_CURRENT_SOURCE_DIR}/env.py" )

find_program(VALGRIND_EXE valgrind DOC "Location of the valgrind program")
option(ENABLE_VALGRIND "Use valgrind to detect run-time problems" ON)
if (ENABLE_VALGRIND)
  if (NOT VALGRIND_EXE)
    message(STATUS "Can't locate the valgrind command; no run-time error detection")
  else ()
    set (VALGRIND_ENV "VALGRIND=${VALGRIND_EXE}")
  endif ()
endif (ENABLE_VALGRIND)

mark_as_advanced (VALGRIND_EXE)

# c tests:

add_subdirectory(src/tests)

# python test: tests/python/proton-test
if (BUILD_PYTHON)
  set (py_root "${pn_test_root}/python")
  set (py_src "${CMAKE_CURRENT_SOURCE_DIR}/bindings/python")
  set (py_bin "${CMAKE_CURRENT_BINARY_DIR}/bindings/python")
  set (py_bld "${CMAKE_CURRENT_BINARY_DIR}${bld_suffix}") # For windows
  set (app_path "${pn_test_bin}/tools/apps/c${bld_suffix}")
  set (app_path "${app_path}:${pn_test_root}/tools/apps/python")
  set_path (py_path "${py_bin}:${py_bld}:${app_path}:$ENV{PATH}")
  set_path (py_pythonpath "${py_root}:${py_src}:${py_bin}:${py_bld}|$ENV{PYTHONPATH}")
  if (CMAKE_SYSTEM_NAME STREQUAL Windows)
    set_path (py_pythonpath "${py_bin}${bld_suffix}:${py_pythonpath}")
  endif (CMAKE_SYSTEM_NAME STREQUAL Windows)
  add_test (python-test ${PYTHON_EXECUTABLE} ${env_py}
            "PATH=${py_path}" "PYTHONPATH=${py_pythonpath}" ${VALGRIND_ENV}
            ${PYTHON_EXECUTABLE} "${py_root}/proton-test")
  set_tests_properties(python-test PROPERTIES PASS_REGULAR_EXPRESSION "Totals: .* 0 failed")
endif (BUILD_PYTHON)

find_program(RUBY_EXE "ruby")
if (RUBY_EXE)
  set (rb_root "${pn_test_root}/ruby")
  set (rb_src "${CMAKE_CURRENT_SOURCE_DIR}/bindings/ruby")
  set (rb_lib "${CMAKE_CURRENT_SOURCE_DIR}/bindings/ruby/lib")
  set (rb_bin "${CMAKE_CURRENT_BINARY_DIR}/bindings/ruby")
  set (rb_bld "${CMAKE_CURRENT_BINARY_DIR}${bld_suffix}")
  set (rb_path "$ENV{PATH}:${rb_bin}:${rb_bld}")
  set (rb_rubylib "${rb_root}:${rb_src}:${rb_bin}:${rb_bld}:${rb_lib}")

  # ruby unit tests:  tests/ruby/proton-test
  # only enable the tests if the Ruby gem dependencies were found
  if (DEFAULT_RUBY_TESTING)
    add_test (ruby-unit-test ${PYTHON_EXECUTABLE} ${env_py} "PATH=${rb_path}" "RUBYLIB=${rb_rubylib}"
      "${rb_root}/proton-test")

    # ruby spec tests
    find_program(RSPEC_EXE rspec)
    if (RSPEC_EXE)
      add_test (NAME ruby-spec-test
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bindings/ruby
                COMMAND ${PYTHON_EXECUTABLE} ${env_py} "PATH=${rb_path}" "RUBYLIB=${rb_rubylib}"
                        ${RSPEC_EXE})

    else(RSPEC_EXE)
      message (STATUS "Cannot find rspec, skipping rspec tests")
    endif(RSPEC_EXE)
  else (DEFAULT_RUBY_TESTING)
    message(STATUS "Skipping Ruby tests: missing dependencies")
  endif (DEFAULT_RUBY_TESTING)
else (RUBY_EXE)
  message (STATUS "Cannot find ruby, skipping ruby tests")
endif (RUBY_EXE)

mark_as_advanced (RUBY_EXE RSPEC_EXE)

# build examples to make sure they still work
add_subdirectory(../examples ../examples)
