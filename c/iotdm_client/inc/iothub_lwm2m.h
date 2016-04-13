// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef IOTHUB_LWM2M_H
#define IOTHUB_LWM2M_H

#include "iothub_client.h"
#include "iothub_client_ll.h"

#include "threadapi.h"
#include "iot_logging.h"

typedef void *IOTHUB_CHANNEL_HANDLE;
typedef enum IOTHUB_TRANSPORT_TAG
{
    COAP_TCPIP = 0
}IOTHUB_TRANSPORT;

typedef void(*ON_DM_CONNECT_COMPLETE)(IOTHUB_CLIENT_RESULT result, void* context);

IOTHUB_CHANNEL_HANDLE IoTHubClient_DM_Open(const char *connectionString, IOTHUB_TRANSPORT transport);
void IoTHubClient_DM_Close(IOTHUB_CHANNEL_HANDLE client);

IOTHUB_CLIENT_RESULT IoTHubClient_DM_CreateDefaultObjects(IOTHUB_CHANNEL_HANDLE h);
    
IOTHUB_CLIENT_RESULT IoTHubClient_DM_AddNewObject(IOTHUB_CHANNEL_HANDLE h, unsigned int objectID);

IOTHUB_CLIENT_RESULT IoTHubClient_DM_Start(IOTHUB_CHANNEL_HANDLE client);

IOTHUB_CLIENT_RESULT IoTHubClient_DM_Connect(IOTHUB_CHANNEL_HANDLE h, ON_DM_CONNECT_COMPLETE onComplete, void* callbackContext);
bool IoTHubClient_DM_DoWork(IOTHUB_CHANNEL_HANDLE h);

#endif /* IOTHUB_LWM2M_H */
