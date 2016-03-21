// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h> 
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "gballoc.h"

#include <stdlib.h>
#include <signal.h>
#include <stddef.h>
#include "crt_abstractions.h"
#include "iothubtransport.h"
#include "iothub_client.h"
#include "iothub_client_private.h"
#include "threadapi.h"
#include "lock.h"
#include "iot_logging.h"
#include "vector.h"

typedef struct TRANSPORT_HL_HANDLE_DATA_TAG
{
	TRANSPORT_LL_HANDLE transportLLHandle;
    THREAD_HANDLE workerThreadHandle;
    LOCK_HANDLE lockHandle;
    sig_atomic_t stopThread;
	TRANSPORT_PROVIDER_FIELDS;
	VECTOR_HANDLE clients;
} TRANSPORT_HANDLE_DATA;

/* Used for Unit test */
const size_t IoTHubTransport_ThreadTerminationOffset = offsetof(TRANSPORT_HANDLE_DATA, stopThread);

TRANSPORT_HANDLE  IoTHubTransport_Create(IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol, const char* iotHubName, const char* iotHubSuffix)
{
	TRANSPORT_HANDLE_DATA * result;

	if (protocol == NULL || iotHubName == NULL || iotHubSuffix == NULL)
	{
		LogError("Invalid NULL argument, protocol [%p], name [%p], suffix [%p].\r\n", protocol, iotHubName, iotHubSuffix);
		result = NULL;
	}
	else
	{
		result = malloc(sizeof(TRANSPORT_HANDLE_DATA));
		if (result == NULL)
		{
			LogError("Transport handle was not allocated.\r\n");
		}
		else
		{
			TRANSPORT_PROVIDER * transportProtocol = (TRANSPORT_PROVIDER*)protocol();
			IOTHUB_CLIENT_CONFIG upperConfig;
			upperConfig.deviceId = NULL;
			upperConfig.deviceKey = NULL;
			upperConfig.iotHubName = iotHubName;
			upperConfig.iotHubSuffix = iotHubSuffix;
			upperConfig.protocol = protocol;
			upperConfig.protocolGatewayHostName = NULL;

			IOTHUBTRANSPORT_CONFIG transportLLConfig;
			transportLLConfig.upperConfig = &upperConfig;
			transportLLConfig.waitingToSend = NULL;

			result->transportLLHandle = transportProtocol->IoTHubTransport_Create(&transportLLConfig);
			if (result->transportLLHandle == NULL)
			{
				LogError("Lower Layer transport not created.\r\n");
				free(result);
				result = NULL;
			}
			else
			{
				result->lockHandle = Lock_Init();
				if (result->lockHandle == NULL)
				{
					LogError("transport Lock not created.\r\n");
					transportProtocol->IoTHubTransport_Destroy(result->transportLLHandle);
					free(result);
					result = NULL;
				}
				else
				{
					result->clients = VECTOR_create(sizeof(IOTHUB_CLIENT_HANDLE));
					if (result->clients == NULL)
					{
						LogError("clients list not created.\r\n");
						Lock_Deinit(result->lockHandle);
						transportProtocol->IoTHubTransport_Destroy(result->transportLLHandle);
						free(result);
						result = NULL;
					}
					else
					{
						result->stopThread = 1;
						result->workerThreadHandle = NULL; /* create thread when work needs to be done */
						result->IoTHubTransport_SetOption = transportProtocol->IoTHubTransport_SetOption;
						result->IoTHubTransport_Create = transportProtocol->IoTHubTransport_Create;
						result->IoTHubTransport_Destroy = transportProtocol->IoTHubTransport_Destroy;
						result->IoTHubTransport_Register = transportProtocol->IoTHubTransport_Register;
						result->IoTHubTransport_Unregister = transportProtocol->IoTHubTransport_Unregister;
						result->IoTHubTransport_Subscribe = transportProtocol->IoTHubTransport_Subscribe;
						result->IoTHubTransport_Unsubscribe = transportProtocol->IoTHubTransport_Unsubscribe;
						result->IoTHubTransport_DoWork = transportProtocol->IoTHubTransport_DoWork;
						result->IoTHubTransport_GetSendStatus = transportProtocol->IoTHubTransport_GetSendStatus;
					}
				}
			}
		}
	}
	
	return result;
}

static int transport_worker_thread(void* threadArgument)
{
	TRANSPORT_HANDLE_DATA* transportData = (TRANSPORT_HANDLE_DATA*)threadArgument;

	while (1)
	{
		if (Lock(transportData->lockHandle) == LOCK_OK)
		{
			if (transportData->stopThread)
			{
				(void)Unlock(transportData->lockHandle);
				break;
			}
			else
			{
				(transportData->IoTHubTransport_DoWork)(transportData->transportLLHandle, NULL);
				(void)Unlock(transportData->lockHandle);
			}
		}
		ThreadAPI_Sleep(1);
	}

	return 0;
}

static bool find_by_handle(const void* element, const void* value)
{
	bool result;
	/* data stored at element is device handle */
	const IOTHUB_CLIENT_HANDLE * guess = (const IOTHUB_CLIENT_HANDLE *)element;
	const IOTHUB_CLIENT_HANDLE match = (const IOTHUB_CLIENT_HANDLE)value;
	result = (*guess == match) ? true : false;
	return result;
}

static void start_worker_if_needed(TRANSPORT_HANDLE_DATA * transportData, IOTHUB_CLIENT_HANDLE clientHandle)
{
	if (transportData->workerThreadHandle == NULL)
	{
		transportData->stopThread = 0;
		if (ThreadAPI_Create(&transportData->workerThreadHandle, transport_worker_thread, transportData) != THREADAPI_OK)
		{
			transportData->workerThreadHandle = NULL;
		}
	}
	if (transportData->workerThreadHandle != NULL)
	{
		bool addToList = ((VECTOR_size(transportData->clients) == 0) || (VECTOR_find_if(transportData->clients, find_by_handle, clientHandle) == NULL));
		if (addToList)
		{
			VECTOR_push_back(transportData->clients, &clientHandle, 1);
		}
	}
}

static void stop_worker_thread(TRANSPORT_HANDLE_DATA * transportData)
{
	int res;
	transportData->stopThread = 1;
	if (ThreadAPI_Join(transportData->workerThreadHandle, &res) != THREADAPI_OK)
	{
		LogError("ThreadAPI_Join failed\r\n");
	}
}

static void end_worker_thread(TRANSPORT_HANDLE_DATA * transportData, IOTHUB_CLIENT_HANDLE clientHandle)
{
	void* element = VECTOR_find_if(transportData->clients, find_by_handle, clientHandle);
	if (element != NULL)
	{
		VECTOR_erase(transportData->clients, element, 1);
	}

	if (transportData->workerThreadHandle != NULL)
	{
		if (VECTOR_size(transportData->clients) == 0)
		{
			stop_worker_thread(transportData);
		}
	}
}

void IoTHubTransport_Destroy(TRANSPORT_HANDLE transportHlHandle)
{
	
	if (transportHlHandle != NULL)
	{
		TRANSPORT_HANDLE_DATA * transportData = (TRANSPORT_HANDLE_DATA*)transportHlHandle;
		if (Lock(transportData->lockHandle) != LOCK_OK)
		{
			LogError("Unable to lock - will still attempt to end thread without thread safety");
			stop_worker_thread(transportData);
		}
		else
		{
			stop_worker_thread(transportData);
			(void)Unlock(transportData->lockHandle);
		}
		Lock_Deinit(transportData->lockHandle);
		(transportData->IoTHubTransport_Destroy)(transportData->transportLLHandle);
		VECTOR_destroy(transportData->clients);
		free(transportHlHandle);
	}
}

LOCK_HANDLE IoTHubTransport_GetLock(TRANSPORT_HANDLE transportHlHandle)
{
	LOCK_HANDLE lock;
	if (transportHlHandle == NULL)
	{
		lock = NULL;
	}
	else
	{
		TRANSPORT_HANDLE_DATA * transportData = (TRANSPORT_HANDLE_DATA*)transportHlHandle;
		lock = transportData->lockHandle;
	}
	return lock;
}

TRANSPORT_LL_HANDLE IoTHubTransport_GetLLTransport(TRANSPORT_HANDLE transportHlHandle)
{
	TRANSPORT_LL_HANDLE llTransport;
	if (transportHlHandle == NULL)
	{
		llTransport = NULL;
	}
	else
	{
		TRANSPORT_HANDLE_DATA * transportData = (TRANSPORT_HANDLE_DATA*)transportHlHandle;
		llTransport = transportData->transportLLHandle;
	}
	return llTransport;
}

IOTHUB_CLIENT_RESULT IoTHubTransport_StartWorkerThread(TRANSPORT_HANDLE transportHlHandle, IOTHUB_CLIENT_HANDLE clientHandle)
{
	IOTHUB_CLIENT_RESULT result;
	if (transportHlHandle == NULL || clientHandle == NULL)
	{
		result = IOTHUB_CLIENT_INVALID_ARG;
	}
	else
	{
		TRANSPORT_HANDLE_DATA * transportData = (TRANSPORT_HANDLE_DATA*)transportHlHandle;
		if (Lock(transportData->lockHandle) != LOCK_OK)
		{
			LogError("Unable to start thread safely");
			result = IOTHUB_CLIENT_ERROR;
		}
		else
		{
			start_worker_if_needed(transportData, clientHandle);
			if (transportData->workerThreadHandle == NULL)
			{
				result = IOTHUB_CLIENT_ERROR;
			}
			else
			{
				result = IOTHUB_CLIENT_OK;
			}
			(void)Unlock(transportData->lockHandle);
		}
	}
	return result;
}

void IoTHubTransport_EndWorkerThread(TRANSPORT_HANDLE transportHlHandle, IOTHUB_CLIENT_HANDLE clientHandle)
{
	if (!(transportHlHandle == NULL || clientHandle == NULL))
	{
		TRANSPORT_HANDLE_DATA * transportData = (TRANSPORT_HANDLE_DATA*)transportHlHandle;
		if (Lock(transportData->lockHandle) != LOCK_OK)
		{
			LogError("Unable to acquire lock - abandoning the attempt to end the thread");
		}
		else
		{
			end_worker_thread(transportData, clientHandle);
			(void)Unlock(transportData->lockHandle);
		}
	}
	
}