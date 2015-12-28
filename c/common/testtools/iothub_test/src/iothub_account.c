// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include <stdio.h>
#include "string.h"
#include "string_tokenizer.h"
#include "iothub_account.h"

#ifdef MBED_BUILD_TIMESTAMP
#define MBED_PARAM_MAX_LENGTH 256
#endif

const char *DEFAULT_CONSUMER_GROUP = "$Default";
const int DEFAULT_PARTITION_COUNT = 16;

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
	static char iothub_name[128];

	char *iothub_connection_string;

	iothub_connection_string = IoTHubAccount_GetIoTHubConnString();

	if (iothub_connection_string != NULL)
	{
		sscanf(iothub_connection_string, "HostName=%[^.]", iothub_name);
	}

	return iothub_name;
}

const char* IoTHubAccount_GetIoTHubSuffix(void)
{
	static char iothub_suffix[256];
	char *iothub_connection_string;

	iothub_connection_string = IoTHubAccount_GetIoTHubConnString();

	if (iothub_connection_string != NULL)
	{
		sscanf(iothub_connection_string, "HostName=%*[^.].%[^;/]", iothub_suffix);
	}

	return iothub_suffix;
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

const char* IoTHubAccount_GetProtocolGatewayHostName(void)
{
#ifdef MBED_BUILD_TIMESTAMP
	static char* value[MBED_PARAM_MAX_LENGTH];
	(void)mbed_log("ProtocolGatewayHostName?\r\n");
	(void)scanf("%s", &value);
	(void)mbed_log("Received '%s'\r\n", value);
	return value;
#else
	return getenv("IOTHUB_PROTOCOL_GATEWAY_HOSTNAME");
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
	char *iothub_connection_string;
	static char access_key[128];

	if ((iothub_connection_string = IoTHubAccount_GetIoTHubConnString()) != NULL)
	{
		STRING_HANDLE iothub_connection_string_str;

		if((iothub_connection_string_str = STRING_construct(iothub_connection_string)) != NULL)
		{
			STRING_TOKENIZER_HANDLE tokenizer;
		
			if ((tokenizer = STRING_TOKENIZER_create(iothub_connection_string_str)) != NULL)
			{
				STRING_HANDLE tokenString;

				if ((tokenString = STRING_new()) != NULL)
				{
					STRING_HANDLE valueString;

					if ((valueString = STRING_new()) != NULL)
					{
						while ((STRING_TOKENIZER_get_next_token(tokenizer, tokenString, "=") == 0))
						{
							char tokenValue[128];
							strcpy(tokenValue, STRING_c_str(tokenString));

							if (STRING_TOKENIZER_get_next_token(tokenizer, tokenString, ";") != 0)
							{
								break;
							}

							if (strcmp(tokenValue, "SharedAccessKey") == 0)
							{
								strcpy(access_key, STRING_c_str(tokenString));
								break;
							}
						}
					
						STRING_delete(valueString);
					}

					STRING_delete(tokenString);
				}

				STRING_TOKENIZER_destroy(tokenizer);
			}
			
			STRING_delete(iothub_connection_string_str);
		}
	} 

	return access_key;
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
	static char consumerGroup[64];

	char *envVarValue = getenv("IOTHUB_EVENTHUB_CONSUMER_GROUP");

	if (envVarValue != NULL)
	{
		strcpy(consumerGroup, envVarValue);
	}
	else
	{ 
		strcpy(consumerGroup, DEFAULT_CONSUMER_GROUP);
	}

    return consumerGroup;
#endif
}

const size_t IoTHubAccount_GetIoTHubPartitionCount(void)
{
	int value;

#ifdef MBED_BUILD_TIMESTAMP
	(void)mbed_log("EventHubPartitionCount?\r\n");
	(void)scanf("%i", &value);
	(void)mbed_log("Received '%i'\r\n", value);
#else
	char *envVarValue = getenv("IOTHUB_PARTITION_COUNT");

	if (envVarValue != NULL)
	{
		value = atoi(envVarValue);
	}
	else
	{
		value = DEFAULT_PARTITION_COUNT;
	}
#endif

	return (size_t)value;
}