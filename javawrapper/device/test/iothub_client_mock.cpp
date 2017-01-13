// Copyright(c) Microsoft.All rights reserved.
// Licensed under the MIT license.See LICENSE file in the project root for full license information.

#define  IMPORT_NAME iothub_client_mock

#include <string> 
#include <vector>
#include <list>
#include <stdlib.h>

#include "platform.h"
#include "map.h"
#include "iothub_client.h"
#include "iothub_client_version.h"
#include "iothub_message.h"
#include "iothubtransporthttp.h"
#include "iothubtransportamqp.h"
#include "iothubtransportmqtt.h"


int platform_init(void)
{
	return 0;
}

void platform_deinit(void)
{
}

// "map.h"

MAP_HANDLE mockMapHandle = (MAP_HANDLE)0x12345678;

#define MOCKMAPSIZE 128
#define MOCKARRAYSIZE 8
MAP_FILTER_CALLBACK mockFilterFunc = NULL;
char mockKey[MOCKMAPSIZE] = { 0 };
char *mockKeyArray[MOCKARRAYSIZE] = { mockKey };
char mockValue[MOCKMAPSIZE] = { 0 };
char *mockValueArray[MOCKARRAYSIZE] = { mockValue };

MAP_HANDLE Map_Create(MAP_FILTER_CALLBACK mapFilterFunc)
{
	// reset mock buffers
	memset(mockKey, 0, sizeof(mockKey));
	memset(mockValue, 0, sizeof(mockValue));
	mockFilterFunc = mapFilterFunc;
	return mockMapHandle;
}

void Map_Destroy(MAP_HANDLE handle)
{
    (void)handle;
	// mockFilterFunc = NULL;
}

MAP_HANDLE Map_Clone(MAP_HANDLE handle)
{
    (void)handle;
	return Map_Create(mockFilterFunc);
}

MAP_RESULT Map_Add(MAP_HANDLE handle, const char* key, const char* value)
{
    (void)handle;
	MAP_RESULT result = MAP_OK;

	if ((key == NULL) || (value == NULL))
	{
		return result = MAP_INVALIDARG;
	}

	if (mockFilterFunc != NULL)
	{
		result = mockFilterFunc(key, value) ? MAP_FILTER_REJECT : MAP_OK;
	}

	if (result == MAP_OK)
	{
		if (strcmp(mockKey, key) == 0)
		{
			result = MAP_KEYEXISTS;
		}
		else
		{
			strcpy(mockKey, key);
			strcpy(mockValue, value);
		}
	}
	return result;
}

MAP_RESULT Map_AddOrUpdate(MAP_HANDLE handle, const char* key, const char* value)
{
    (void)handle;
	MAP_RESULT result = MAP_OK;

	if ((key == NULL) || (value == NULL) || (handle == NULL))
	{
		return result = MAP_INVALIDARG;
	}
	
	if (mockFilterFunc != NULL)
	{
		result = mockFilterFunc(key, value) ? MAP_FILTER_REJECT : MAP_OK;
	}

	if (result == MAP_OK)
	{
		strcpy(mockKey, key);
		strcpy(mockValue, value);
	}

	return result;
}

MAP_RESULT Map_Delete(MAP_HANDLE handle, const char* key)
{
	MAP_RESULT result = MAP_KEYNOTFOUND;

	if ((key == NULL) || (handle == NULL))
	{
		return result = MAP_INVALIDARG;
	}

	if (strcmp(mockKey, key) == 0)
	{
		memset(mockKey, 0, sizeof(mockKey));
		memset(mockValue, 0, sizeof(mockValue));
		result = MAP_OK;
	}

	return result;
}

MAP_RESULT Map_ContainsKey(MAP_HANDLE handle, const char* key, bool* keyExists)
{
	if ((key == NULL) || (handle == NULL))
	{
		return MAP_INVALIDARG;
	}
	
	*keyExists = strcmp(mockKey, key) == 0;
	return MAP_OK;
}

MAP_RESULT Map_ContainsValue(MAP_HANDLE handle, const char* value, bool* valueExists)
{
	if ((value == NULL) || (handle == NULL))
	{
		return MAP_INVALIDARG;
	}
	
	*valueExists = strcmp(mockValue, value) == 0;
	return MAP_OK;
}

const char* Map_GetValueFromKey(MAP_HANDLE handle, const char* key)
{
	if ((key == NULL) || (handle == NULL))
	{
		return NULL;
	}
	
	if (strcmp(mockKey, key) == 0)
	{
		return mockValue;
	}
	return NULL;
}

MAP_RESULT Map_GetInternals(MAP_HANDLE handle, const char*const** keys, const char*const** values, size_t* count)
{
	*count = 0;
	*keys = mockKeyArray;
	*values = mockValueArray;
	(void)handle;

	if (*mockKey != '\0')
	{
		*count = 1;
	}

	return MAP_OK;
}

// "iothub_client.h"

IOTHUB_CLIENT_HANDLE mockClientHandle = (IOTHUB_CLIENT_HANDLE)0x12345678;
IOTHUB_CLIENT_CONFIG* mockConfig = NULL;

IOTHUB_CLIENT_HANDLE IoTHubClient_CreateFromConnectionString(const char* connectionString, IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol)
{
    (void)protocol;
	if (connectionString == NULL)
	{
		return NULL;
	}
	
	return mockClientHandle;
}

IOTHUB_CLIENT_HANDLE IoTHubClient_Create(const IOTHUB_CLIENT_CONFIG* config)
{
    (void)config;
	return mockClientHandle;
}

void IoTHubClient_Destroy(IOTHUB_CLIENT_HANDLE iotHubClientHandle)
{
    (void)iotHubClientHandle;
}

IOTHUB_CLIENT_RESULT IoTHubClient_SendEventAsync(IOTHUB_CLIENT_HANDLE iotHubClientHandle, IOTHUB_MESSAGE_HANDLE eventMessageHandle, IOTHUB_CLIENT_EVENT_CONFIRMATION_CALLBACK eventConfirmationCallback, void* userContextCallback)
{
    (void)eventMessageHandle;
    (void)eventConfirmationCallback;
    (void)userContextCallback;
	if (iotHubClientHandle == NULL)
	{
		return IOTHUB_CLIENT_INVALID_ARG;
	}
	
	return IOTHUB_CLIENT_OK;
}

IOTHUB_CLIENT_RESULT IoTHubClient_GetSendStatus(IOTHUB_CLIENT_HANDLE iotHubClientHandle, IOTHUB_CLIENT_STATUS *iotHubClientStatus)
{
    (void)iotHubClientStatus;
	if (iotHubClientHandle == NULL)
	{
		return IOTHUB_CLIENT_INVALID_ARG;
	}
	
	return IOTHUB_CLIENT_OK;
}

IOTHUB_CLIENT_RESULT IoTHubClient_SetMessageCallback(IOTHUB_CLIENT_HANDLE iotHubClientHandle, IOTHUB_CLIENT_MESSAGE_CALLBACK_ASYNC messageCallback, void* userContextCallback)
{
    (void)messageCallback;
    (void)userContextCallback;
	if (iotHubClientHandle == NULL)
	{
		return IOTHUB_CLIENT_INVALID_ARG;
	}
	
	return IOTHUB_CLIENT_OK;
}

IOTHUB_CLIENT_RESULT IoTHubClient_GetLastMessageReceiveTime(IOTHUB_CLIENT_HANDLE iotHubClientHandle, time_t* lastMessageReceiveTime)
{
    (void)lastMessageReceiveTime;
	if (iotHubClientHandle == NULL)
	{
		return IOTHUB_CLIENT_INVALID_ARG;
	}
	
	return IOTHUB_CLIENT_INDEFINITE_TIME;
}

IOTHUB_CLIENT_RESULT IoTHubClient_SetOption(IOTHUB_CLIENT_HANDLE iotHubClientHandle, const char* optionName, const void* value)
{
	if ((iotHubClientHandle == NULL) || (optionName == NULL) || (value == NULL))
	{
		return IOTHUB_CLIENT_INVALID_ARG;
	}
	
	return IOTHUB_CLIENT_OK;
}

IOTHUB_CLIENT_RESULT IoTHubClient_UploadToBlobAsync(IOTHUB_CLIENT_HANDLE iotHubClientHandle, const char* destinationFileName, const unsigned char* source, size_t size, IOTHUB_CLIENT_FILE_UPLOAD_CALLBACK iotHubClientFileUploadCallback, void* context)
{
    (void)size;
    (void)iotHubClientFileUploadCallback;
    (void)context;

    if ((iotHubClientHandle == NULL) || (destinationFileName == NULL) || (source == NULL))
    {
        return IOTHUB_CLIENT_INVALID_ARG;
    }

    return IOTHUB_CLIENT_OK;
}


// "iothub_client_version.h"

const char* IoTHubClient_GetVersionString(void)
{
	return IOTHUB_SDK_VERSION;
}

// "iothub_message.h"

IOTHUB_MESSAGE_HANDLE mockMessageHandle = (IOTHUB_MESSAGE_HANDLE)0x12345678;
#define MOCKMESSAGESIZE 128
char mockBuffer[MOCKMESSAGESIZE] = { 0 };
char mockMessageId[MOCKMESSAGESIZE] = { 0 };
char mockCorrelationId[MOCKMESSAGESIZE] = { 0 };
size_t mockSize = 0;
bool mockString = false;

IOTHUB_MESSAGE_HANDLE IoTHubMessage_CreateFromByteArray(const unsigned char* byteArray, size_t size)
{
	if (byteArray == NULL)
	{
		return NULL;
	}
	
	mockSize = size > MOCKMESSAGESIZE ? MOCKMESSAGESIZE : size;
	memcpy(mockBuffer, byteArray, mockSize);
	mockString = false;
	return mockMessageHandle;
}

IOTHUB_MESSAGE_HANDLE IoTHubMessage_CreateFromString(const char* source)
{
	if (source == NULL)
	{
		return NULL;
	}
	
	mockString = true;
	strncpy(mockBuffer, source, MOCKMESSAGESIZE);
	return mockMessageHandle;
}

IOTHUB_MESSAGE_HANDLE IoTHubMessage_Clone(IOTHUB_MESSAGE_HANDLE iotHubMessageHandle)
{
	if (iotHubMessageHandle == NULL)
	{
		return NULL;
	}

	return mockMessageHandle;
}

IOTHUB_MESSAGE_RESULT IoTHubMessage_GetByteArray(IOTHUB_MESSAGE_HANDLE iotHubMessageHandle, const unsigned char** buffer, size_t* size)
{
	if (iotHubMessageHandle == NULL)
	{
		return IOTHUB_MESSAGE_INVALID_ARG;
	}
	
	if (mockString)
	{
		return IOTHUB_MESSAGE_INVALID_TYPE;
	}
	*buffer = (unsigned char*)mockBuffer;
	*size = mockSize;
	return IOTHUB_MESSAGE_OK;
}

const char* IoTHubMessage_GetString(IOTHUB_MESSAGE_HANDLE iotHubMessageHandle)
{
	if (iotHubMessageHandle == NULL)
	{
		return NULL;
	}
	
	if (!mockString)
	{
		return NULL;
	}
	return mockBuffer;
}

IOTHUBMESSAGE_CONTENT_TYPE IoTHubMessage_GetContentType(IOTHUB_MESSAGE_HANDLE iotHubMessageHandle)
{
	if (iotHubMessageHandle == NULL)
	{
		return IOTHUBMESSAGE_UNKNOWN;
	}
	
	return mockString ? IOTHUBMESSAGE_STRING : IOTHUBMESSAGE_BYTEARRAY;
}

MAP_HANDLE IoTHubMessage_Properties(IOTHUB_MESSAGE_HANDLE iotHubMessageHandle)
{
	if (iotHubMessageHandle == NULL)
	{
		return NULL;
	}

	return mockMapHandle;
}

const char* IoTHubMessage_GetMessageId(IOTHUB_MESSAGE_HANDLE iotHubMessageHandle)
{
	if (iotHubMessageHandle == NULL)
	{
		return NULL;
	}
	
	if (*mockMessageId == '\0')
	{
		return NULL;
	}
	return mockMessageId;
}

IOTHUB_MESSAGE_RESULT IoTHubMessage_SetMessageId(IOTHUB_MESSAGE_HANDLE iotHubMessageHandle, const char* messageId)
{
	if ((iotHubMessageHandle == NULL) || (messageId == NULL))
	{
		return IOTHUB_MESSAGE_INVALID_ARG;
	}
	
	strncpy(mockMessageId, messageId, MOCKMESSAGESIZE);
	return IOTHUB_MESSAGE_OK;
}

const char* IoTHubMessage_GetCorrelationId(IOTHUB_MESSAGE_HANDLE iotHubMessageHandle)
{
	if (iotHubMessageHandle == NULL)
	{
		return NULL;
	}
	
	if (*mockCorrelationId == '\0')
	{
		return NULL;
	}
	return mockCorrelationId;
}

IOTHUB_MESSAGE_RESULT IoTHubMessage_SetCorrelationId(IOTHUB_MESSAGE_HANDLE iotHubMessageHandle, const char* correlationId)
{
	if ((iotHubMessageHandle == NULL) || (correlationId == NULL))
	{
		return IOTHUB_MESSAGE_INVALID_ARG;
	}
	
	strncpy(mockCorrelationId, correlationId, MOCKMESSAGESIZE);
	return IOTHUB_MESSAGE_OK;
}

void IoTHubMessage_Destroy(IOTHUB_MESSAGE_HANDLE iotHubMessageHandle)
{
    (void)iotHubMessageHandle;
}

// "iothubtransporthttp.h"
TRANSPORT_PROVIDER *mockProtocol = (TRANSPORT_PROVIDER *)0x12345678;
const TRANSPORT_PROVIDER* HTTP_Protocol(void)
{
	return mockProtocol;
}

// "iothubtransportamqp.h"

const TRANSPORT_PROVIDER* AMQP_Protocol(void)
{
	return mockProtocol;
}

// "iothubtransportmqtt.h"

const TRANSPORT_PROVIDER* MQTT_Protocol(void)
{
	return mockProtocol;
}

// "iothubtransportamqp_websockets.h"

const TRANSPORT_PROVIDER* AMQP_Protocol_over_WebSocketsTls(void)
{
	return mockProtocol;
}