

// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

//
// Simple implementation of IoTHub LWM2M Device object
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#if defined(WIN32)
#include <windows.h>
#include <tchar.h>
#endif

#include "iothub_lwm2m.h"
#include "iotdm_lwm2m_client_apis.h"
#include "liblwm2m.h"

IOTHUB_CLIENT_STATUS send_text_over_http(IOTHUB_CLIENT_LL_HANDLE HttpHandle, const char *format, ...);
extern IOTHUB_CLIENT_LL_HANDLE g_httpHandle;

/**********************************************************************************
 * Callback prototypes
 *
 **********************************************************************************/

IOTHUB_CLIENT_RESULT on_read_device_manufacturer(STRING_HANDLE *value);
IOTHUB_CLIENT_RESULT on_read_device_modelnumber(STRING_HANDLE *value);
IOTHUB_CLIENT_RESULT on_read_device_serialnumber(STRING_HANDLE *value);
IOTHUB_CLIENT_RESULT on_read_device_firmwareversion(STRING_HANDLE *value);
IOTHUB_CLIENT_RESULT on_execute_device_reboot();
IOTHUB_CLIENT_RESULT on_execute_device_factoryreset();
IOTHUB_CLIENT_RESULT on_read_device_batterylevel(int *value);
IOTHUB_CLIENT_RESULT on_read_device_memoryfree(int *value);
IOTHUB_CLIENT_RESULT on_read_device_currenttime(int *value);
IOTHUB_CLIENT_RESULT on_write_device_currenttime(int value);
IOTHUB_CLIENT_RESULT on_read_device_utcoffset(STRING_HANDLE *value);
IOTHUB_CLIENT_RESULT on_write_device_utcoffset(STRING_HANDLE value);
IOTHUB_CLIENT_RESULT on_read_device_timezone(STRING_HANDLE *value);
IOTHUB_CLIENT_RESULT on_write_device_timezone(STRING_HANDLE value);
IOTHUB_CLIENT_RESULT on_read_device_devicetype(STRING_HANDLE *value);
IOTHUB_CLIENT_RESULT on_read_device_hardwareversion(STRING_HANDLE *value);
IOTHUB_CLIENT_RESULT on_read_device_batterystatus(int *value);
IOTHUB_CLIENT_RESULT on_read_device_memorytotal(int *value);

/**********************************************************************************
 * Default property implmentations
 *
 **********************************************************************************/
STRING_HANDLE propval_device_manufacturer = NULL;
STRING_HANDLE propval_device_modelnumber = NULL;
STRING_HANDLE propval_device_serialnumber = NULL;
STRING_HANDLE propval_device_firmwareversion = NULL;
int propval_device_batterylevel = 0;
int propval_device_memoryfree = 0;
int propval_device_currenttime = 0;
STRING_HANDLE propval_device_utcoffset = NULL;
STRING_HANDLE propval_device_timezone = NULL;
STRING_HANDLE propval_device_devicetype = NULL;
STRING_HANDLE propval_device_hardwareversion = NULL;
int propval_device_batterystatus = 0;
int propval_device_memorytotal = 0;

void set_default_device_values()
{
	propval_device_manufacturer = STRING_construct("Microsoft");
	propval_device_modelnumber = STRING_construct("IOTDM 2ChannelSample");
	propval_device_serialnumber = STRING_construct("8675309");
	propval_device_firmwareversion = STRING_construct("0.99.0");
	propval_device_batterylevel = 100;
	propval_device_memoryfree = 1024;
	propval_device_currenttime = -1;
	propval_device_utcoffset = STRING_construct("-08:00");
	propval_device_timezone = STRING_construct("America/Los_Angeles");
	propval_device_devicetype = STRING_construct("2ChannelSample");
	propval_device_hardwareversion = STRING_construct("1.0");
	propval_device_batterystatus = -1;
	propval_device_memorytotal = -1;
}

/**********************************************************************************
 * CALLBACK REGISTRATION
 *
 **********************************************************************************/
IOTHUB_CLIENT_RESULT register_all_device_callbacks(IOTHUB_CHANNEL_HANDLE h)
{
    if (
        (IOTHUB_CLIENT_OK != IotHubClient_Device_Manufacturer_SetReadCallback(h, on_read_device_manufacturer)) ||
        (IOTHUB_CLIENT_OK != IotHubClient_Device_ModelNumber_SetReadCallback(h, on_read_device_modelnumber)) ||
        (IOTHUB_CLIENT_OK != IotHubClient_Device_SerialNumber_SetReadCallback(h, on_read_device_serialnumber)) ||
        (IOTHUB_CLIENT_OK != IotHubClient_Device_FirmwareVersion_SetReadCallback(h, on_read_device_firmwareversion)) ||
        (IOTHUB_CLIENT_OK != IotHubClient_Device_Reboot_SetExecuteCallback(h, on_execute_device_reboot)) ||
        (IOTHUB_CLIENT_OK != IotHubClient_Device_FactoryReset_SetExecuteCallback(h, on_execute_device_factoryreset)) ||
        (IOTHUB_CLIENT_OK != IotHubClient_Device_BatteryLevel_SetReadCallback(h, on_read_device_batterylevel)) ||
        (IOTHUB_CLIENT_OK != IotHubClient_Device_MemoryFree_SetReadCallback(h, on_read_device_memoryfree)) ||
        (IOTHUB_CLIENT_OK != IotHubClient_Device_CurrentTime_SetReadCallback(h, on_read_device_currenttime)) ||
        (IOTHUB_CLIENT_OK != IotHubClient_Device_CurrentTime_SetWriteCallback(h, on_write_device_currenttime)) ||
        (IOTHUB_CLIENT_OK != IotHubClient_Device_UtcOffset_SetReadCallback(h, on_read_device_utcoffset)) ||
        (IOTHUB_CLIENT_OK != IotHubClient_Device_UtcOffset_SetWriteCallback(h, on_write_device_utcoffset)) ||
        (IOTHUB_CLIENT_OK != IotHubClient_Device_Timezone_SetReadCallback(h, on_read_device_timezone)) ||
        (IOTHUB_CLIENT_OK != IotHubClient_Device_Timezone_SetWriteCallback(h, on_write_device_timezone)) ||
        (IOTHUB_CLIENT_OK != IotHubClient_Device_DeviceType_SetReadCallback(h, on_read_device_devicetype)) ||
        (IOTHUB_CLIENT_OK != IotHubClient_Device_HardwareVersion_SetReadCallback(h, on_read_device_hardwareversion)) ||
        (IOTHUB_CLIENT_OK != IotHubClient_Device_BatteryStatus_SetReadCallback(h, on_read_device_batterystatus)) ||
        (IOTHUB_CLIENT_OK != IotHubClient_Device_MemoryTotal_SetReadCallback(h, on_read_device_memorytotal)) ||
        false
        )
    {
        LogError("Failure setting callback in Device object\r\n");

        return IOTHUB_CLIENT_ERROR;
    }

    return IOTHUB_CLIENT_OK;
}

/**********************************************************************************
 * OBJECT REGISTRATION
 *
 **********************************************************************************/
IOTHUB_CLIENT_RESULT register_device_object(IOTHUB_CLIENT_HANDLE h)
{
    set_default_device_values();

    if (IOTHUB_CLIENT_OK != register_all_device_callbacks(h))
    {
        LogError("setting default values for Device object\r\n");

        return IOTHUB_CLIENT_ERROR;
    }

    if (IOTHUB_CLIENT_OK != IoTHubClient_DM_AddNewObject(h, LWM2M_DEVICE_OBJECT_ID))
    {
        LogError("Failure to add the Device object for client: %p\r\n", h);

        return IOTHUB_CLIENT_ERROR;
    }

    return IOTHUB_CLIENT_OK;
}

/**********************************************************************************
 * CALLBACK HANDLERS
 *
 **********************************************************************************/
IOTHUB_CLIENT_RESULT on_read_device_manufacturer(STRING_HANDLE *value)
{
    *value = propval_device_manufacturer;
    send_text_over_http(g_httpHandle, "Read Device_Manufacturer returns [%s]", STRING_c_str(*value));

    return IOTHUB_CLIENT_OK;
}

IOTHUB_CLIENT_RESULT on_read_device_modelnumber(STRING_HANDLE *value)
{
    *value = propval_device_modelnumber;
    send_text_over_http(g_httpHandle, "Read Device_ModelNumber returns [%s]", STRING_c_str(*value));

    return IOTHUB_CLIENT_OK;
}

IOTHUB_CLIENT_RESULT on_read_device_serialnumber(STRING_HANDLE *value)
{
    *value = propval_device_serialnumber;
    send_text_over_http(g_httpHandle, "Read Device_SerialNumber returns [%s]", STRING_c_str(*value));

    return IOTHUB_CLIENT_OK;
}

IOTHUB_CLIENT_RESULT on_read_device_firmwareversion(STRING_HANDLE *value)
{
    *value = propval_device_firmwareversion;
    send_text_over_http(g_httpHandle, "Read Device_FirmawareVersion returns [%s]", STRING_c_str(*value));

    return IOTHUB_CLIENT_OK;
}

IOTHUB_CLIENT_RESULT on_execute_device_reboot()
{
    send_text_over_http(g_httpHandle, "Device_Reboot called");
    return IOTHUB_CLIENT_OK;
}

IOTHUB_CLIENT_RESULT on_execute_device_factoryreset()
{
    send_text_over_http(g_httpHandle, "Device_FactoryReset called");
    return IOTHUB_CLIENT_OK;
}

IOTHUB_CLIENT_RESULT on_read_device_batterylevel(int *value)
{

	LogInfo("inside reader for Device_BatteryLevel\r\n"); 


	// Simulate a battery that drops by 1% every time you ask
	if (--propval_device_batterylevel == 0)
	{
		propval_device_batterylevel = 100;
	}

	LogInfo("returning %d\r\n", propval_device_batterylevel);
	send_text_over_http(g_httpHandle, "Read Device_BatteryLevel returns [%d]", propval_device_batterylevel);
	*value = propval_device_batterylevel;

    return IOTHUB_CLIENT_OK;
}

IOTHUB_CLIENT_RESULT on_read_device_memoryfree(int *value)
{
    *value = propval_device_memoryfree;
    send_text_over_http(g_httpHandle, "Read Device_BatteryLevel returns [%d]", *value);

    return IOTHUB_CLIENT_OK;
}

IOTHUB_CLIENT_RESULT on_read_device_currenttime(int *value)
{
    *value = propval_device_currenttime;
    send_text_over_http(g_httpHandle, "Read Device_CurrentTime returns [%d]", *value);

    return IOTHUB_CLIENT_OK;
}

IOTHUB_CLIENT_RESULT on_write_device_currenttime(int value)
{
    propval_device_currenttime = value;
    send_text_over_http(g_httpHandle, "Device_CurrentTime set to [%d]", value);

    return IOTHUB_CLIENT_OK;
}

IOTHUB_CLIENT_RESULT on_read_device_utcoffset(STRING_HANDLE *value)
{
    *value = propval_device_utcoffset;
    send_text_over_http(g_httpHandle, "Read Device_UtcOffset returns [%s]", STRING_c_str(*value));

    return IOTHUB_CLIENT_OK;
}

IOTHUB_CLIENT_RESULT on_write_device_utcoffset(STRING_HANDLE value)
{
    STRING_copy(propval_device_utcoffset, STRING_c_str(value));
    send_text_over_http(g_httpHandle, "Device_UtcOffset set to [%s]", STRING_c_str(value));

    return IOTHUB_CLIENT_OK;
}

IOTHUB_CLIENT_RESULT on_read_device_timezone(STRING_HANDLE *value)
{
    *value = propval_device_timezone;
    send_text_over_http(g_httpHandle, "Read Device_TimeZone returns [%s]", STRING_c_str(*value));

    return IOTHUB_CLIENT_OK;
}

IOTHUB_CLIENT_RESULT on_write_device_timezone(STRING_HANDLE value)
{
    STRING_copy(propval_device_timezone, STRING_c_str(value));
    send_text_over_http(g_httpHandle, "Device_TimeZone set to [%d]", value);

    return IOTHUB_CLIENT_OK;
}

IOTHUB_CLIENT_RESULT on_read_device_devicetype(STRING_HANDLE *value)
{
    *value = propval_device_devicetype;
    send_text_over_http(g_httpHandle, "Read Device_Type returns [%s]", STRING_c_str(*value));

    return IOTHUB_CLIENT_OK;
}

IOTHUB_CLIENT_RESULT on_read_device_hardwareversion(STRING_HANDLE *value)
{
    *value = propval_device_hardwareversion;
    send_text_over_http(g_httpHandle, "Read Device_HardwareVersion returns [%s]", STRING_c_str(*value));

    return IOTHUB_CLIENT_OK;
}

IOTHUB_CLIENT_RESULT on_read_device_batterystatus(int *value)
{
    *value = propval_device_batterystatus;
    send_text_over_http(g_httpHandle, "Read Device_BatteryStatus returns [%d]", *value);

    return IOTHUB_CLIENT_OK;
}

IOTHUB_CLIENT_RESULT on_read_device_memorytotal(int *value)
{
    *value = propval_device_memorytotal;
    send_text_over_http(g_httpHandle, "Read Device_MemoryTotal returns [%d]", *value);

    return IOTHUB_CLIENT_OK;
}
