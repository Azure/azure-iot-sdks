#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

cmake_minimum_required(VERSION 2.8.11)
project(mbed_iothub_client_lib)
set(HG_COMMIT_MSG "Automatic Build Commit" CACHE STRING "Passed to the mercurial commit")

set(MBED_CMAKE_LIB)

### Functions
function(verifyRequiredParameters)
    set(ALL_PARAMETERS_PROVIDED TRUE)

    if(NOT DEFINED ENV{MBED_HG_USER})
        message(WARNING "MBED_HG_USER environment variable not defined.")
        set(ALL_PARAMETERS_PROVIDED FALSE)
    endif()

    if(NOT DEFINED ENV{MBED_USER})
        message(WARNING "MBED_USER environment variable not defined.")
        set(ALL_PARAMETERS_PROVIDED FALSE)
    endif()

    if(NOT DEFINED ENV{MBED_PWD})
        message(WARNING "MBED_PWD environment variable not defined.")
        set(ALL_PARAMETERS_PROVIDED FALSE)
    endif()

    if (NOT ${ALL_PARAMETERS_PROVIDED})
        message(FATAL_ERROR "Not all expected parameters provided. Please provide the parameters listed above.")
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
    message(STATUS "Trying to delete " ${local_sublib_dir})
    message(STATUS "Trying to clone " ${mbed_repo_address})

    if (EXISTS ${local_sublib_dir})
        message(STATUS "Removing old sublib directory: " ${local_sublib_dir})
        file(REMOVE_RECURSE ${local_sublib_dir})

        if(EXISTS ${local_subdir_path})
            message(FATAL_ERROR "Failed removing sublib directory: " ${local_sublib_dir})
        endif()
    endif()
    file(MAKE_DIRECTORY  ${local_sublib_dir})
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
    set(exported_file_destination ${local_repo_path}/${mbed_project_base})
    #
    # go through and delete every non .lib .hg .bld file in the directory. Don't delete the exported file directory though!!!
    #
    file(GLOB existingFiles ${local_repo_path}/*)
    foreach (file_item ${existingFiles})
        STRING(FIND ${file_item} ".lib" string_with_lib)
        STRING(FIND ${file_item} ".bld" string_with_bld)
        STRING(FIND ${file_item} ".hg" string_with_hg)
        #
        # We try to see if the we have the subdirectory of the exported files.  We don't want to delete that directory
        # basically we are detecting if the subdirectory name is the very last thing in the file name.
        #
        STRING(FIND ${file_item} ${mbed_project_base} string_with_base REVERSE)
        if (NOT (${string_with_base} EQUAL -1))
            STRING(LENGTH ${file_item} file_item_length)
            STRING(LENGTH ${mbed_project_base} mbed_project_base_length)
            math(EXPR offset_of_end_of_base "${string_with_base} + ${mbed_project_base_length}")
            if (NOT (${offset_of_end_of_base} EQUAL ${file_item_length}))
                set(string_with_base -1)
            endif()
        endif()
        if ((${string_with_lib} EQUAL -1) AND
            (${string_with_hg} EQUAL -1) AND
            (${string_with_bld} EQUAL -1) AND
            (${string_with_base} EQUAL -1))
            file(REMOVE ${file_item})
        endif()
    endforeach()
    #
    # go through and delete every non .lib .hg .bld file in the exported file directory
    #
    file(GLOB existingFiles ${local_repo_path}/${mbed_project_base}/*)
    foreach (file_item ${existingFiles})
        STRING(FIND ${file_item} ".lib" string_with_lib)
        STRING(FIND ${file_item} ".bld" string_with_bld)
        STRING(FIND ${file_item} ".hg" string_with_hg)
        STRING(FIND ${file_item} ${mbed_project_base} string_with_base REVERSE)
        #
        # We try to see if the we have the subdirectory of the exported files.  We don't want to delete that directory
        # basically we are detecting if the subdirectory name is the very last thing in the file name.
        #
        if (NOT (${string_with_base} EQUAL -1))
            STRING(LENGTH ${file_item} file_item_length)
            STRING(LENGTH ${mbed_project_base} mbed_project_base_length)
            math(EXPR offset_of_end_of_base "${string_with_base} + ${mbed_project_base_length}")
            if (NOT (${offset_of_end_of_base} EQUAL ${file_item_length}))
                set(string_with_base -1)
            endif()
        endif()
        if ((${string_with_lib} EQUAL -1) AND
            (${string_with_hg} EQUAL -1) AND
            (${string_with_bld} EQUAL -1))
            file(REMOVE ${file_item})
        endif()
    endforeach()
    message(STATUS "Copying target project files.")
    foreach(file_item ${mbed_project_files})
        file(GLOB expanded_file_item ${file_item})
        message(STATUS ${expanded_file_item})
        file(COPY ${expanded_file_item} DESTINATION ${source_file_destination})
    endforeach()
    foreach(file_item ${mbed_exported_project_files})
        file(GLOB expanded_file_item ${file_item})
        message(STATUS ${expanded_file_item})
        file(COPY ${expanded_file_item} DESTINATION ${exported_file_destination})
    endforeach()

endfunction(updateMBEDSourceFiles)

function(updateMBEDLibraries)

    file(GLOB libfiles ${local_repo_path}/*.lib)
    foreach(file_item ${libfiles})

        #
        # We go through all of the .lib files.  We only want to update what we own.
        #
        file (READ ${file_item} fileContents)
        STRING(FIND ${fileContents} /$ENV{MBED_USER}/ matchResult)
        if (NOT ${matchResult} EQUAL -1)

            #
            # Found one.  We want to get the the clone path of the library.  Go up to the slash and pound sign.  (The sha follows that.)
            #
            STRING(FIND ${fileContents} /\# uriEnd)
            if (NOT ${uriEnd} EQUAL -1)
                STRING(SUBSTRING ${fileContents} 0 ${uriEnd} libraryUri)

                #
                # Get the sha that exists in the file.  We'll compare after the clone to see if we need to change the file.
                # This is because even if we have the same sha, we could change the whitespace and this will cause hg to think the file has
                # changed.

                math(EXPR beginSha "${uriEnd} + 2}")
                STRING(SUBSTRING ${fileContents} ${beginSha} -1 rawOldSha)
                STRING(STRIP ${rawOldSha} oldSha)

                #
                # Get the name that the clone will go to.  (This is the last part of the uri).
                #
                STRING(FIND ${libraryUri} / finalSlash REVERSE)
                math(EXPR finalSlash "${finalSlash} + 1")
                STRING(SUBSTRING ${libraryUri} ${finalSlash} -1 libraryName)

                #
                # Our compile sandboxes are actually private.  So we need to pass the username and password to do the clone.
                # So we actually form the address all over again.  Sigh.
                #
                set(subLibraryCloningUri https://$ENV{MBED_USER}:$ENV{MBED_PWD}@developer.mbed.org/users/$ENV{MBED_USER}/code/${libraryName}/)

                #
                # We got the parts we need.  Clone the lib and then go get the sha.
                #
                execute_process(COMMAND hg "clone" "--insecure" ${subLibraryCloningUri} WORKING_DIRECTORY ${local_sublib_dir} RESULT_VARIABLE result_code)
                if(NOT ${result_code} EQUAL 0)
                    message(FATAL_ERROR "Failed to clone the sub library " ${libraryName} " (error code " ${result_code} ")")
                endif()
                execute_process(COMMAND hg "id" "-i" WORKING_DIRECTORY ${local_sublib_dir}/${libraryName} RESULT_VARIABLE result_code OUTPUT_VARIABLE outputSha OUTPUT_STRIP_TRAILING_WHITESPACE)
                if(NOT ${result_code} EQUAL 0)
                    message(FATAL_ERROR "Failed to obtain the sha of the sublibrary" ${libraryName} "(error code " ${result_code} ")")
                endif()

                if (NOT ${outputSha} STREQUAL ${oldSha})
                    message(STATUS "Need to update the reference to libary at " ${libraryUri})
                    file(WRITE ${file_item} ${libraryUri} /\# ${outputSha})
                endif()
            else()
                message(FATAL "Improperly formed lib file: " ${file_item})
            endif()
        endif()
    endforeach()
endfunction(updateMBEDLibraries)

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
    if (DEFINED mbed_output_bin_path AND NOT ${mbed_output_bin_path} EQUAL "")
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
    updateMBEDLibraries()
    updateMBEDSourceFiles()
    pushMBEDRepository()
endfunction(releaseMBEDRepository)

function(compileMBEDRepository)
    releaseMBEDRepository()
    compileMBEDProject()
endfunction(compileMBEDRepository)


