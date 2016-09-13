// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>
#include <sys/reboot.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include "azure_c_shared_utility/threadapi.h"
#include "azure_c_shared_utility/platform.h"

#include "iothub_client_sample_mqtt_dm.h"


pid_t child_download = -1;
pid_t child_update = -1;
pid_t child_factory_reset = -1;

#define TEMP_ZIP_LOCATION "/home/root/nf.zip"
#define DOWNLOAD_DESTINATION "/home/root/newFirmware.zip"

static void _system(const char *command)
{
    LogInfo("running [%s]", command);
    int i = system(command);
    LogInfo("Command [%s] returned %d", command, i);
}

static bool is_process_running(pid_t *p)
{
    if (*p <= 0)
    {
        return false;
    }
    else
    {
        int status;
        pid_t result = waitpid(*p, &status, WNOHANG);
        if (result == 0)
        {
            return true;
        }
        else
        {
            *p = -1;
            return false;
        }
    }
}

static bool is_download_happening(void)
{
    return is_process_running(&child_download);
}

static bool is_update_happening(void)
{
    return is_process_running(&child_update);
}

static bool is_factory_reset_happening(void)
{
    return is_process_running(&child_factory_reset);
}

static bool was_file_successfully_downloaded(void)
{
    struct stat s;
    int status = stat(DOWNLOAD_DESTINATION, &s);
    return (status == 0);
}

static bool is_any_child_process_running(void)
{
    bool ret = false;
    if (is_download_happening())
    {
        LogInfo("download is in progress");
        ret = true;
    }
    if (is_update_happening())
    {
        LogInfo("update is in progress");
        ret = true;
    }
    if (is_factory_reset_happening())
    {
        LogInfo("factory reset is in progress");
        ret = true;
    }
    return ret;
}

static char *read_string_from_file(const char *filename)
{
    char *sn = NULL;
    FILE *fd = fopen(filename, "r");

    if (NULL != fd)
    {
        fseek(fd, 0, SEEK_END);
        size_t size = ftell(fd);
        sn = (char *)malloc(size * sizeof(char));
        if (NULL != sn)
        {
            fseek(fd, 0L, SEEK_SET);
            int count = fread(sn, sizeof(char), size, fd);
            sn[count - 1] = '\0';
        }
        fclose(fd);
    }
    return sn;
}

static bool use_wget(const char *uri)
{
    bool ret = false;
    if (uri[0] == 'h' && uri[1] == 't' && uri[2] == 't' && uri[3] == 'p')
    {
        ret = true;
    }
    else if (uri[0] == 'f' && uri[1] == 't' && uri[2] == 'p')
    {
        ret = true;
    }
    return ret;
}

static const char *remove_file_prefix(const char *uri)
{
    if (uri[0] == 'f' && uri[1] == 'i' && uri[2] == 'l' && uri[3] == 'e' && uri[4] == ':' && uri[5] == '/' && uri[6] == '/')
    {
        return  uri + 7;
    }
    else
    {
        return uri;
    }
}


//----------------------------------------------------------------------------------------------------------------------------
void device_get_name(char *&out)
{
    out = NULL;
}

void device_get_time(time_t &out)
{
    out = time(NULL);
}

void device_get_tz(char *&out)
{
    out = NULL;
}

void device_get_serial_number(char *&out)
{
    out = read_string_from_file("/factory/serial_number");
}

void device_get_firmware_version(char *&out)
{
    out = read_string_from_file("/etc/version");
}

FIRMWARE_UPDATE_STATE device_get_firmware_update_state(void)
{
	FIRMWARE_UPDATE_STATE rv = FIRMWARE_UPDATE_STATE_NONE;
	struct stat s;
	int    status = stat(TEMP_ZIP_LOCATION, &s);
	if (status == 0)
	{
		rv = FIRMWARE_UPDATE_STATE_DOWNLOADING_IMAGE;
	}
	else
	{
		status = stat(DOWNLOAD_DESTINATION, &s);
		if (status == 0)
		{
			rv = FIRMWARE_UPDATE_STATE_IMAGE_DOWNLOADED;
		}
	}
	return rv;
}

bool device_reboot(void)
{
    bool ret = false;
    LogInfo("\n\t REBOOT\n");

    // if the server tells us to reboot, we reboot. This protects us against
    // child processes that are hung.
    if (is_any_child_process_running() == true)
    {
        LogInfo("Reboot requested while another process is running.  Continuing");
    }

    pid_t child = fork();
    if (child == 0)
    {
        ThreadAPI_Sleep(500);

        setuid(0);
        sync();

        LogInfo("\n\t REBOOT - fork()\n");
        reboot(RB_AUTOBOOT);
    }
    else if (child > 0)
    {
        ret = true;
    }
    else
    {
        LogError("fork error - %d",child);
    }
    return ret;
}

bool device_factoryreset(void)
{
    bool ret = false;
    LogInfo("\n\t FACTORY RESET\n");

    if (is_any_child_process_running() == true)
    {
        LogError("Cannot factory reset while another process is running");
    }
    else
    {
        pid_t child = fork();

        if (child == 0)
        {
            LogInfo("** Reset To Factory started");

            _system("/usr/bin/wget https://downloadmirror.intel.com/25028/eng/edison-image-ww25.5-15.zip -O /home/root/factory.zip");
            ThreadAPI_Sleep(500);
            ThreadAPI_Sleep(500);
            ThreadAPI_Sleep(500);

            setuid(0);

            _system("mkdir -p /update");
            _system("systemctl stop clloader");
            _system("echo on > /sys/devices/pci0000:00/0000:00:11.0/dwc3-device.1/power/control");
            _system("rmmod g_multi");
            _system("losetup -o 8192 /dev/loop0 /dev/disk/by-partlabel/update");
            _system("mount /dev/loop0 /update");
            _system("rm -rf /update/* /update/.[!.]* /update/..?*");
            _system("unzip -o /home/root/factory.zip -d /update");

            ThreadAPI_Sleep(500);
            ThreadAPI_Sleep(500);
            ThreadAPI_Sleep(500);
            unlink("/home/root/factory.zip");

            _system("reboot ota");
            exit(0);
        }
        else if (child > 0)
        {
            child_factory_reset = child;
            ret = true;
        }
        else
        {
            child_factory_reset = -1;
            LogError("fork error - %d", child);
        }
    }
    return ret;
}

bool device_download_firmware(const char *uri)
{
    bool ret = false;
    LogInfo("Downloading [%s]", uri);

    if (is_any_child_process_running() == true)
    {
        LogError("Cannot download while another process is running");
    }
    else
    {
        unlink(TEMP_ZIP_LOCATION);
        unlink(DOWNLOAD_DESTINATION);

        pid_t child = fork();
        if (child == 0)
        {
            char buffer[1024];

            if (use_wget(uri))
            {
                sprintf(buffer, "/usr/bin/wget \"%s\" -O %s", uri, TEMP_ZIP_LOCATION);
                _system(buffer);
                
                sprintf(buffer, "mv %s \"%s\"", TEMP_ZIP_LOCATION, DOWNLOAD_DESTINATION);
                _system(buffer);
            }
            else
            {
                const char *src = remove_file_prefix(uri);
                sprintf(buffer, "cp \"%s\" \"%s\"", src, DOWNLOAD_DESTINATION);
                _system(buffer);
            }
            
            LogInfo("** Download complete");
            exit(0);
        }
        else if (child > 0)
        {
            child_download = child;
            ret = true;
        }
        else
        {
            child_download = -1;
            LogError("fork error - %d", child);
        }
    }
    return ret;
}

bool device_update_firmware(void)
{
    bool ret = false;
    LogInfo("\n\t FIRMWARE UPDATE\n");

    if (is_any_child_process_running() == true)
    {
        LogError("Cannot factory reset while another process is running");
    }
    else
    {
        pid_t child = fork();

        if (child == 0)
        {
            LogInfo("** Update started");

            // Give the client time to repsond to the service.  
            ThreadAPI_Sleep(500);
            ThreadAPI_Sleep(500);
            ThreadAPI_Sleep(500);

            setuid(0);

            _system("mkdir -p /update");
            _system("systemctl stop clloader");
            _system("echo on > /sys/devices/pci0000:00/0000:00:11.0/dwc3-device.1/power/control");
            _system("rmmod g_multi");
            _system("losetup -o 8192 /dev/loop0 /dev/disk/by-partlabel/update");
            _system("mount /dev/loop0 /update");
            _system("rm -rf /update/* /update/.[!.]* /update/..?*");
            _system("unzip -o /home/root/newFirmware.zip -d /update");

            ThreadAPI_Sleep(500);
            ThreadAPI_Sleep(500);
            ThreadAPI_Sleep(500);
            unlink("/home/root/newFirmware.zip");

            LogInfo("** Update complete");
            _system("reboot ota");
            exit(0);
        }
        else if (child > 0)
        {
            child_update = child;
            ret = true;
        }
        else
        {
            child_update = -1;
            LogError("fork error - %d ", child);
        }
    }
    return ret;
}

bool device_run_service(void)
{
    LogInfo("Running as service");

    int ret = chdir("/");
	if (ret == 0)
	{
		ret = daemon(0, 1);
	}
	return (ret == 0);
}
