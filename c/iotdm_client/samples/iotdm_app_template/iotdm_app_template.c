// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

//
// APPLICATION TEMPLATE FOR IoTHub LWM2M client code
//

#include "iothub_lwm2m.h"

static const char *connectionString = "[device connection string]";

/**********************************************************************************
 * MAIN LOOP
 *
 **********************************************************************************/
int main(int argc, char *argv[])
{
    const char *cs;

    if (argc == 2) cs = argv[1];
    else cs = connectionString;

    IOTHUB_CHANNEL_HANDLE IoTHubChannel = IoTHubClient_DM_Open(cs, COAP_TCPIP);
    if (NULL == IoTHubChannel)
    {
        LogError("IoTHubClientHandle is NULL!\r\n");

        return -1;
    }

    LogInfo("IoTHubClientHandle: %p\r\n", IoTHubChannel);

    LogInfo("prepare LWM2M objects");
    if (IOTHUB_CLIENT_OK != IoTHubClient_DM_CreateDefaultObjects(IoTHubChannel))
    {
        LogError("failure to create LWM2M objects for client: %p\r\n", IoTHubChannel);

        return -1;
    }
    
    if (IOTHUB_CLIENT_OK != IoTHubClient_DM_Start(IoTHubChannel))
    {
        LogError("failure to start the client: %p\r\n", IoTHubChannel);

        return -1;
    }

    /* Disconnect and cleanup */
    IoTHubClient_DM_Close(IoTHubChannel);
}

