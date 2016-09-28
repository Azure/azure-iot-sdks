// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <ctype.h>
#include <time.h>
#include <unistd.h>
#include <sys/reboot.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/utsname.h>

#include "azure_c_shared_utility/threadapi.h"
#include "azure_c_shared_utility/platform.h"

#include "iothub_client_sample_mqtt_dm.h"


#define TEMP_ZIP_LOCATION "/root/nf.zip"
#define NEW_FW_ARCHIVE "/root/newfirmware.zip"


static int _system(const char *command)
{
    int retValue = system(command);
    LogInfo("Command [%s] returned %d", command, retValue);
    return retValue;
}

static int install_zip_package(void)
{
    int retValue = _system("/usr/bin/apt-get update");
    if (retValue != 0)
    {
        LogError("failed to update the package cahce");
    }
    else
    {
        retValue = _system("/usr/bin/apt-get install zip --assume-yes");
        if (retValue != 0)
        {
            LogError("failed to install the zip package");
        }
    }
    return retValue;
}

static int prepare_to_flash(const char *fileName)
{
    int retValue = install_zip_package();
    if (retValue != 0)
    {
        LogError("failed to install zip utility");
    }
    else
    {
        if (_system("mkdir /update") != 0)
        {
            LogError("failed to create the update directory");
        }
        else
        {
            retValue = _system("mkfs.vfat -F32 -I /dev/mmcblk0p1");
            if ((retValue != 0) && (retValue != 256 /* filesystem was prestaged */))
            {
                LogError("failed to prepare the mount point. mkfs.vfat returned: %d", retValue);
            }
            else
            {
                retValue = _system("mount -t vfat /dev/mmcblk0p1 /update");
                if (retValue != 0)
                {
                    LogError("failed to mount the update partition. mount returned %d", retValue);
                }
                else
                {
                    retValue = _system("rm -rf /update/* /update/.[!.]* /update/..?*");
                    if (retValue != 0)
                    {
                        LogError("failed to clean the update partirion");
                    }
                    else
                    {
                        size_t  length = snprintf(NULL, 0, "/usr/bin/unzip -o %s -d /update", fileName);
                        char   *buffer = malloc(length + 1);
                        if (buffer == NULL)
                        {
                            LogError("failed to allocate memory for system command");
                        }
                        else
                        {
                            sprintf(buffer, "/usr/bin/unzip -o %s -d /update", fileName);
                            retValue = _system(buffer);
                            free(buffer);
                            if (unlink(fileName) == 0)
                            {
                                LogError("failed to delete flash file '%s'", fileName);
                            }

                            if (retValue != 0)
                            {
                                LogError("failed to inflate the firmware package '%s'", fileName);
                            }
                            else
                            {
                                retValue = _system("echo \"silentinstall lang=en keyboard=us runinstaller quiet ramdisk_size=32768 root=/dev/ram0 init=/init vt.cur_default=1 elevator=deadline\" > /update/recovery.cmdline");
                                if (retValue != 0)
                                {
                                    LogError("failed to prepare the command file for auto-flash");
                                }
                                else
                                {
                                    retValue = reboot(RB_AUTOBOOT);
                                    if (retValue != 0)
                                    {
                                        LogError("failed to reboot the device");
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return retValue;
}

//----------------------------------------------------------------------------------------------------------------------------
char *device_get_firmware_version(void)
{
    char *retValue;
    struct utsname data;
    if (uname(&data) != 0)
    {
        LogError("failed to 'uname'");
        retValue = NULL;
    }
    else
    {
        size_t size = strlen(data.release) + 1;
        retValue = malloc(size * sizeof(char));
        if (retValue == NULL)
        {
            LogError("failed to allocate string for firmware version");
        }
        else
        {
            memcpy(retValue, data.release, size);
        }
    }
    return retValue;
}

char* device_read_string_from_file(const char *fileName)
{
    char *retValue;
    FILE *fd = fopen(fileName, "rx");

    if (fd == NULL)
    {
        LogError("failed to open file '%s'", fileName);
        retValue = NULL;
    }
    else
    {
        if (fseek(fd, 0L, SEEK_END) != 0)
        {
            LogError("failed to fined the end of file('%s')", fileName);
            retValue = NULL;
        }
        else
        {
            long size = ftell(fd);
            if ((size <= 0) || (fseek(fd, 0L, SEEK_SET) != 0))
            {
                LogError("failed to rewind the file");
                retValue = NULL;
            }
            else
            {
                retValue = malloc((size + 1) * sizeof(char));
                if (retValue == NULL)
                {
                    LogError("failed to allocate buffer for data");
                }
                else
                {
                    retValue = fgets(retValue, size, fd);
                }
            }
        }
        fclose(fd);
    }
    return retValue;
}

FIRMWARE_UPDATE_STATUS device_get_firmware_update_status(void)
{
    FIRMWARE_UPDATE_STATUS retValue;
    struct stat s;
    int    status = stat(TEMP_ZIP_LOCATION, &s);
    if (status == 0)
    {
        retValue = downloading;
    }
    else
    {
        status = stat(NEW_FW_ARCHIVE, &s);
        if (status == 0)
        {
            retValue = downloadComplete;
        }
        else
        {
            retValue = waiting;
        }
    }
    return retValue;
}

bool device_download_firmware(const char *uri)
{
    LogInfo("Downloading [%s]", uri);

    unlink(TEMP_ZIP_LOCATION);
    unlink(NEW_FW_ARCHIVE);

    int result;

    size_t  length = snprintf(NULL, 0, "/usr/bin/wget \"%s\" -O %s", uri, TEMP_ZIP_LOCATION);
    char   *buffer = malloc(length + 1);
    if (buffer == NULL)
    {
        result = -1;
        LogError("failed to allocate memory for the download command");
    }
    else
    {
        sprintf(buffer, "/usr/bin/wget \"%s\" -O %s", uri, TEMP_ZIP_LOCATION);
        result = _system(buffer);
        free(buffer);

        if (result != 0)
        {
            LogError("failed to download from '%s'", uri);
        }
        else
        {
            length = snprintf(NULL, 0, "mv %s \"%s\"", TEMP_ZIP_LOCATION, NEW_FW_ARCHIVE);
            buffer = malloc(length + 1);
            if (buffer == NULL)
            {
                result = -1;
                LogError("failed to allocate memory for the move file command");
            }
            else
            {
                sprintf(buffer, "mv %s \"%s\"", TEMP_ZIP_LOCATION, NEW_FW_ARCHIVE);
                result = _system(buffer);
                free(buffer);
                if (result != 0)
                {
                    LogError("failed to stage the firmware package");
                }
            }
        }
    }
    return (result == 0);
}

bool device_update_firmware(void)
{
    LogInfo("\n\t FIRMWARE UPDATE\n");

    int result = setuid(0);
    if (result != 0)
    {
        LogError("failed to sudo");
    }
    else
    {
        result = prepare_to_flash(NEW_FW_ARCHIVE);
        if (result != 0)
        {
            LogError("failed to prepare the flash device");
        }
        else
        {
            result = reboot(RB_AUTOBOOT);
            if (result != 0)
            {
                LogError("failed to reboot the device");
            }
        }
    }
    return (result == 0);
}

bool device_run_service(void)
{
    int  pid, sid;
    bool retValue;
        
    /* Fork off the parent process */
    pid = fork();
    if (pid < 0)
    {
        LogError("failed to fork from parent process: %p\n", pid);
        retValue = false;
    }
    else
    {
        if (pid > 0)
        {
            /* exit the parent process */
            exit(EXIT_SUCCESS);

            /* Change the file mode mask */
            umask(0);       

            /* Create a new SID for the child process */
            sid = setuid(0);
            if (sid < 0)
            {
                retValue = false;
                LogError("failed to set sudo");
            }
            else
            {
                /* Change the current working directory */
                if ((chdir("/")) < 0)
                {
                    LogError("failed to chdir to '/'");
                    retValue = false;
                }
                else
                {
                    /* Close out the standard file descriptors */
                    close(STDIN_FILENO);
                    close(STDOUT_FILENO);
                    close(STDERR_FILENO);

                    retValue = true;
                }
            }
        }
        else
        {
            LogError("Failed to fork with a good process ID");
            retValue = false;
        }
    }
    return retValue;
}
