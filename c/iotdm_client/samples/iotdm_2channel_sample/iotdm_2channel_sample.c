

// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

//
// LWM2M/HTTP sample.  
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#if defined(WIN32)
#include <windows.h>
#include <tchar.h>
#endif

#include "iothub_lwm2m.h"
#include "iotdm_lwm2m_client_apis.h"
#include "liblwm2m.h"
#include "iothub_client_ll.h"
#include "iothub_message.h"
#include "crt_abstractions.h"
#include "iothubtransporthttp.h"


static const char *lwm2mConnectionString = "[device connection string]";

static const char *httpConnectionString = "[device connection string]";

IOTHUB_CLIENT_RESULT register_lwm2mserver_object(IOTHUB_CHANNEL_HANDLE h);
IOTHUB_CLIENT_RESULT register_device_object(IOTHUB_CHANNEL_HANDLE h);
IOTHUB_CLIENT_RESULT register_firmwareupdate_object(IOTHUB_CHANNEL_HANDLE h);
IOTHUB_CLIENT_RESULT register_config_object(IOTHUB_CHANNEL_HANDLE h);

IOTHUB_CLIENT_LL_HANDLE g_httpHandle = NULL;

DEFINE_ENUM_STRINGS(IOTHUB_CLIENT_CONFIRMATION_RESULT, IOTHUB_CLIENT_CONFIRMATION_RESULT_VALUES);

typedef struct confirmation_callback_context
{
    IOTHUB_MESSAGE_HANDLE messageHandle;
    int callbackCounter;
} confirmation_callback_context;

static void SendConfirmationCallback(IOTHUB_CLIENT_CONFIRMATION_RESULT result, void* userContextCallback)
{
    confirmation_callback_context* ctx = (confirmation_callback_context*)userContextCallback;
    LogInfo("Confirmation[%d] received for last message with result = %s\r\n", ctx->callbackCounter, ENUM_TO_STRING(IOTHUB_CLIENT_CONFIRMATION_RESULT, result));
    ctx->callbackCounter++;
    IoTHubMessage_Destroy(ctx->messageHandle);
}

IOTHUB_CLIENT_STATUS send_text_over_http(IOTHUB_CLIENT_LL_HANDLE httpHandle, const char *format, ...)
{
    IOTHUB_CLIENT_STATUS result = IOTHUB_CLIENT_OK;
    static char msgText[1024];

    va_list args;
    va_start(args, format);
    vsnprintf(msgText, sizeof(msgText), format, args);
    va_end(args);
    msgText[sizeof(msgText) - 1] = 0;

    IOTHUB_MESSAGE_HANDLE messageHandle = IoTHubMessage_CreateFromByteArray((const unsigned char*)msgText, strlen(msgText));
    if (messageHandle == NULL)
    {
        LogError("IoTHubMessage_CreateFromByteArray FAILED!\r\n");
        result = IOTHUB_CLIENT_ERROR;
    }
    else
    {
        result = IoTHubClient_LL_SendEventAsync(httpHandle, messageHandle, SendConfirmationCallback, messageHandle);
        if (result != IOTHUB_CLIENT_OK)
        {
            LogError("IoTHubClient_LL_SendEventAsync FAILED!\r\n");
        }
        else
        {
            LogInfo("IoTHubClient_LL_SendEventAsync accepted message [%s] for transmission to IoT Hub.\r\n", msgText);
        }
    }
    return result;

}

void on_dm_connect_complete(IOTHUB_CLIENT_RESULT result, void* context)
{
    bool* connectCompleted = (bool*)context;

    if (result == IOTHUB_CLIENT_OK)
    {
        *connectCompleted = true;
    }
    else
    {
        LogError("IoTHubClient_DM_Connect invoked callback with result=%d\r\n", result);
    }
}


/**********************************************************************************
 * MAIN LOOP
 *
 **********************************************************************************/
int main(int argc, char *argv[])
{
    bool connectedCompleted = false;

    // Create an LWM2M Channel
    IOTHUB_CHANNEL_HANDLE lwm2mHandle = IoTHubClient_DM_Open(lwm2mConnectionString, COAP_TCPIP);
    if (NULL == lwm2mHandle)
    {
        LogError("Failure to start LWM2M!\r\n");
        return -1;
    }

    LogInfo("Lwm2m Handle: %p\r\n", lwm2mHandle);

    LogInfo("prepare the LWM2M Server Object\r\n");
    if (IOTHUB_CLIENT_OK != register_lwm2mserver_object(lwm2mHandle))
    {
        LogError("failure to add the LWM2M Server object for client: %p\r\n", lwm2mHandle);

        return -1;
    }

    LogInfo("prepare the Device Object\r\n");
    if (IOTHUB_CLIENT_OK != register_device_object(lwm2mHandle))
    {
        LogError("failure to add the Device object for client: %p\r\n", lwm2mHandle);

        return -1;
    }

    LogInfo("prepare the Firmware Update Object\r\n");
    if (IOTHUB_CLIENT_OK != register_firmwareupdate_object(lwm2mHandle))
    {
        LogError("failure to add the Firmware Update object for client: %p\r\n", lwm2mHandle);

        return -1;
    }

    LogInfo("prepare the Config Object\r\n");
    if (IOTHUB_CLIENT_OK != register_config_object(lwm2mHandle))
    {
        LogError("failure to add the Config object for client: %p\r\n", lwm2mHandle);

        return -1;
    }

    if (IOTHUB_CLIENT_OK != IoTHubClient_DM_Connect(lwm2mHandle, on_dm_connect_complete, &connectedCompleted))
    {
        LogError("failure to start the LWM2M client: %p\r\n", lwm2mHandle);
        return -1;
    }

    while (!connectedCompleted)
    {
        ThreadAPI_Sleep(100);
    }

    IOTHUB_CLIENT_LL_HANDLE httpHandle = IoTHubClient_LL_CreateFromConnectionString(httpConnectionString, HTTP_Protocol);
    if (httpHandle == NULL)
    {
        LogError("failure to start HTTP\r\n");
        return -1;
    }
    g_httpHandle = httpHandle;

    send_text_over_http(httpHandle, "iotdm_2channel_sample init complete - %p, %p", lwm2mHandle, httpHandle);

    while (true)
    {
        IoTHubClient_LL_DoWork(httpHandle);
        IoTHubClient_DM_DoWork(lwm2mHandle);
        ThreadAPI_Sleep(500);
    }

    /* Disconnect and cleanup */
    IoTHubClient_LL_Destroy(httpHandle);
    IoTHubClient_DM_Close(lwm2mHandle);
}
