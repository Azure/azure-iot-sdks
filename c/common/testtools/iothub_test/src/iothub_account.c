// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include <stdio.h>
#include "iothub_account.h"

#ifdef MBED_BUILD_TIMESTAMP
#define MBED_PARAM_MAX_LENGTH 256
#endif

const char* IoTHubAccount_GetEventHubConnectionString(void)
{
#ifdef MBED_BUILD_TIMESTAMP
    static char* value[MBED_PARAM_MAX_LENGTH];
    (void)mbed_log("EventHubConnectionString?\r\n");
    (void)scanf("%s", &value);
    (void)mbed_log("Received '%s'\r\n", value);
    return value;
#else
    return getenv("IOTHUB_EVENTHUB_CONNECTION_STRING");
#endif
}

const char* IoTHubAccount_GetIoTHubName(void)
{
#ifdef MBED_BUILD_TIMESTAMP
    static char* value[MBED_PARAM_MAX_LENGTH];
    (void)mbed_log("IoTHubName?\r\n");
    (void)scanf("%s", &value);
    (void)mbed_log("Received '%s'\r\n", value);
    return value;
#else
    return getenv("IOTHUB_NAME");
#endif
}

const char* IoTHubAccount_GetIoTHubSuffix(void)
{
#ifdef MBED_BUILD_TIMESTAMP
    static char* value[MBED_PARAM_MAX_LENGTH];
    (void)mbed_log("IoTHubSuffix?\r\n");
    (void)scanf("%s", &value);
    (void)mbed_log("Received '%s'\r\n", value);
    return value;
#else
    return getenv("IOTHUB_SUFFIX");
#endif
}

const char* IoTHubAccount_GetEventhubListenName(void)
{
#ifdef MBED_BUILD_TIMESTAMP
    static char* value[MBED_PARAM_MAX_LENGTH];
    (void)mbed_log("EventhubListenName?\r\n");
    (void)scanf("%s", &value);
    (void)mbed_log("Received '%s'\r\n", value);
    return value;
#else
    return getenv("IOTHUB_EVENTHUB_LISTEN_NAME");
#endif
}

const char* IoTHubAccount_GetDeviceId(void)
{
#ifdef MBED_BUILD_TIMESTAMP
    static char* value[MBED_PARAM_MAX_LENGTH];
    (void)mbed_log("DeviceId?\r\n");
    (void)scanf("%s", &value);
    (void)mbed_log("Received '%s'\r\n", value);
    return value;
#else
    return getenv("IOTHUB_DEVICE_ID");
#endif
}

const char* IoTHubAccount_GetDeviceKey(void)
{
#ifdef MBED_BUILD_TIMESTAMP
    static char* value[MBED_PARAM_MAX_LENGTH];
    (void)mbed_log("DeviceKey?\r\n");
    (void)scanf("%s", &value);
    (void)mbed_log("Received '%s'\r\n", value);
    return value;
#else
    return getenv("IOTHUB_DEVICE_KEY");
#endif
}

const char* IoTHubAccount_GetIoTHubConnString(void)
{
#ifdef MBED_BUILD_TIMESTAMP
    static char* value[MBED_PARAM_MAX_LENGTH];
    (void)mbed_log("IoTHubConnString?\r\n");
    (void)scanf("%s", &value);
    (void)mbed_log("Received '%s'\r\n", value);
    return value;
#else
    return getenv("IOTHUB_CONNECTION_STRING");
#endif
}

const char* IoTHubAccount_GetSharedAccessSignature(void)
{
#ifdef MBED_BUILD_TIMESTAMP
    char tempValue[MBED_PARAM_MAX_LENGTH];
    static char value[MBED_PARAM_MAX_LENGTH];

    (void)mbed_log("SharedAccessSignature?\r\n");
    (void)scanf("%s", &tempValue);
    (void)sprintf(value, "SharedAccessSignature %s", tempValue);
    (void)mbed_log("Received '%s'\r\n", value);
    return &value;
#else
    return getenv("IOTHUB_SHARED_ACCESS_SIGNATURE");
#endif
}

const char* IoTHubAccount_GetEventhubAccessKey(void)
{
#ifdef MBED_BUILD_TIMESTAMP
    static char* value[MBED_PARAM_MAX_LENGTH];
    (void)mbed_log("EventhubAccessKey?\r\n");
    (void)scanf("%s", &value);
    (void)mbed_log("Received '%s'\r\n", value);
    return value;
#else
    return getenv("IOTHUB_EVENTHUB_ACCESS_KEY");
#endif
}

const char* IoTHubAccount_GetEventhubConsumerGroup(void)
{
#ifdef MBED_BUILD_TIMESTAMP
    static char* value[MBED_PARAM_MAX_LENGTH];
    (void)mbed_log("EventhubConsumerGroup?\r\n");
    (void)scanf("%s", &value);
    (void)mbed_log("Received '%s'\r\n", value);
    return value;
#else
    return getenv("IOTHUB_EVENTHUB_CONSUMER_GROUP");
#endif
}
