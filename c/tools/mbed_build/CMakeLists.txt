#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

cmake_minimum_required(VERSION 2.8.11)
project(mbed_iothub_client_lib)
set(HG_COMMIT_MSG "Automatic Build Commit" CACHE STRING "Passed to the mercurial commit")

set(MBED_CMAKE_LIB)

### Functions
function(verifyRequiredParameters)
	if(NOT DEFINED ENV{MBED_HG_USER})
		message(FATAL_ERROR "MBED_HG_USER environment variable not defined.")
	endif()

	if(NOT DEFINED ENV{MBED_USER})
		message(FATAL_ERROR "MBED_USER environment variable not defined.")
	endif()
	
	if(NOT DEFINED ENV{MBED_PWD})
		message(FATAL_ERROR "MBED_PWD environment variable not defined.")
	endif()
endfunction(verifyRequiredParameters)


function(verifyTarIsInstalled)
	execute_process(COMMAND tar "--version" WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} RESULT_VARIABLE result_code OUTPUT_QUIET)
	
	if(WIN32)		
		if(${result_code} MATCHES "The system cannot find the file specified")
			message(FATAL_ERROR "Please install 'tar' and make it available on $PATH.")
		endif()
	endif()
endfunction(verifyTarIsInstalled)


function(verifyMercurialIsInstalled)
	execute_process(COMMAND hg "--version" WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} RESULT_VARIABLE result_code OUTPUT_QUIET)
	
	if(WIN32)
		if(${result_code} MATCHES "The system cannot find the file specified")
			message(FATAL_ERROR "Please install 'Mercurial' and make it available on $PATH.")
		endif()
	endif()
endfunction(verifyMercurialIsInstalled)


function(verifyCompileMbedTool)
	if(WIN32)
		file(GLOB expanded_compile_mbed_tool ${compile_mbed_tool})
		if (NOT EXISTS ${expanded_compile_mbed_tool})
			message(FATAL_ERROR "Required tool " ${expanded_compile_mbed_tool} " could not be found. Please compile it first.")
		endif()
	endif()
endfunction(verifyCompileMbedTool)


function(verifyRequiredTools)
	verifyMercurialIsInstalled()
	verifyCompileMbedTool()
endfunction(verifyRequiredTools)


function(cloneMBEDRepository)
	set(mbed_repo_address https://$ENV{MBED_USER}:$ENV{MBED_PWD}@developer.mbed.org/users/$ENV{MBED_USER}/code/${mbed_repo_name}/)
	
	message(STATUS "Trying to delete " ${local_repo_path})
	
	if(EXISTS ${local_repo_path})
		message(STATUS "Removing existing cloned directory.")
		file(REMOVE_RECURSE ${local_repo_path})
		
		if(EXISTS ${local_repo_path})
			message(FATAL_ERROR "Failed removing existing repository clone.")
		endif()
	endif()
	
	
	message(STATUS "Cloning MBED code.")
	execute_process(COMMAND hg "clone" "--insecure" ${mbed_repo_address} WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} RESULT_VARIABLE result_code)
	
	if(NOT ${result_code} EQUAL 0)
		message(FATAL_ERROR "Failed cloning MBED repository '" ${mbed_repo_name} "' (error code " ${result_code} ")")
	endif()
endfunction(cloneMBEDRepository)


function(updateMBEDSourceFiles)
	set(source_file_destination ${local_repo_path})
	
	message(STATUS "Copying common files.")
	foreach(file_item ${common_files})
		file(GLOB expanded_file_item ${file_item})
		file(COPY ${expanded_file_item} DESTINATION ${source_file_destination})
	endforeach()

	message(STATUS "Copying target project files.")
	foreach(file_item ${project_files})
		file(GLOB expanded_file_item ${file_item})
		file(COPY ${expanded_file_item} DESTINATION ${source_file_destination})
	endforeach()
	
	message(STATUS "Removing excluded files.")
	foreach(file_item ${exclude_files})
		file(GLOB expanded_file_item ${source_file_destination}/${file_item})
		
		if (NOT "${expanded_file_item}" STREQUAL "")
			file(REMOVE ${expanded_file_item})
		endif()
	endforeach()	
endfunction(updateMBEDSourceFiles)


function(pushMBEDRepository)
	
	set(mbed_repo_address https://$ENV{MBED_USER}:$ENV{MBED_PWD}@developer.mbed.org/users/$ENV{MBED_USER}/code/${mbed_repo_name}/)

	message(STATUS "Commiting changes to local repository.")

	execute_process(COMMAND hg "addremove" WORKING_DIRECTORY ${local_repo_path} RESULT_VARIABLE result_code)
	if(NOT ${result_code} EQUAL 0)
		message(FATAL_ERROR "Failed adding/removing local changes (error code " ${result_code} ")")
	endif()

	execute_process(COMMAND hg "commit" "-u" $ENV{MBED_HG_USER} "-m ${HG_COMMIT_MSG}" WORKING_DIRECTORY ${local_repo_path} RESULT_VARIABLE result_code)
	if(NOT ${result_code} EQUAL 0 AND NOT ${result_code} EQUAL 1)
		message(FATAL_ERROR "Failed commiting changes to local repository (error code " ${result_code} ")")
	endif()

	
	message(STATUS "Pushing changes to repo " ${mbed_repo_name})
	execute_process(COMMAND hg "push" "--insecure" ${mbed_repo_address} WORKING_DIRECTORY ${local_repo_path} RESULT_VARIABLE result_code)
	
	if(NOT ${result_code} EQUAL 0 AND NOT ${result_code} EQUAL 1)
		message(FATAL_ERROR "Failed pushing changes to remote MBED repository '" ${mbed_repo_name} "' (error code " ${result_code} ")")
	endif()
endfunction(pushMBEDRepository)


function(compileMBEDProject)
	set(mbed_repo_address http://developer.mbed.org/users/$ENV{MBED_USER}/code/${mbed_repo_name}/)
	
	file(GLOB expanded_compile_mbed_tool ${compile_mbed_tool})
	if (DEFINED mbed_output_bin_path)
		execute_process(COMMAND ${expanded_compile_mbed_tool} -un $ENV{MBED_USER} -pwd $ENV{MBED_PWD} -r ${mbed_repo_address} -plat ${mbed_platform} -o ${mbed_output_bin_path} WORKING_DIRECTORY . RESULT_VARIABLE result_code)
	else()
		execute_process(COMMAND ${expanded_compile_mbed_tool} -un $ENV{MBED_USER} -pwd $ENV{MBED_PWD} -r ${mbed_repo_address} -plat ${mbed_platform} WORKING_DIRECTORY . RESULT_VARIABLE result_code)
	endif()
	
	if(NOT ${result_code} EQUAL 0)
		message(FATAL_ERROR "Failed compiling MBED repository '" ${mbed_repo_name} "' (error code " ${result_code} ")")
	endif()
endfunction(compileMBEDProject)

function(releaseMBEDRepository)
	verifyRequiredParameters()
	verifyMercurialIsInstalled()
	cloneMBEDRepository()
	updateMBEDSourceFiles()
	pushMBEDRepository()
endfunction(releaseMBEDRepository)

function(compileMBEDRepository)
    releaseMBEDRepository()
	compileMBEDProject()
endfunction(compileMBEDRepository)


