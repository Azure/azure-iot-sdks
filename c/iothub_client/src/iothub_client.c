// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h> 
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#include "gballoc.h"

#include <stdlib.h>
#include <signal.h>
#include "crt_abstractions.h"
#include "iothub_client.h"
#include "iothub_client_ll.h"
#include "threadapi.h"
#include "lock.h"
#include "iot_logging.h"

typedef struct IOTHUB_CLIENT_INSTANCE_TAG
{
    IOTHUB_CLIENT_LL_HANDLE IoTHubClientLLHandle;
    THREAD_HANDLE ThreadHandle;
    LOCK_HANDLE LockHandle;
    sig_atomic_t StopThread;
} IOTHUB_CLIENT_INSTANCE;

static int ScheduleWork_Thread(void* threadArgument)
{
    IOTHUB_CLIENT_INSTANCE* iotHubClientInstance = (IOTHUB_CLIENT_INSTANCE*)threadArgument;
    
    /* Codes_SRS_IOTHUBCLIENT_01_038: [The thread shall exit when IoTHubClient_Destroy is called.] */
    while (!iotHubClientInstance->StopThread)
    {
        /* Codes_SRS_IOTHUBCLIENT_01_039: [All calls to IoTHubClient_LL_DoWork shall be protected by the lock created in IotHubClient_Create.] */
        /* Codes_SRS_IOTHUBCLIENT_01_040: [If acquiring the lock fails, IoTHubClient_LL_DoWork shall not be called.] */
        if (Lock(iotHubClientInstance->LockHandle) == LOCK_OK)
        {
            /* Codes_SRS_IOTHUBCLIENT_01_037: [The thread created by IoTHubClient_SendEvent or IoTHubClient_SetMessageCallback shall call IoTHubClient_LL_DoWork every 1 ms.] */
            IoTHubClient_LL_DoWork(iotHubClientInstance->IoTHubClientLLHandle);
            /* Codes_SRS_IOTHUBCLIENT_01_039: [All calls to IoTHubClient_LL_DoWork shall be protected by the lock created in IotHubClient_Create.] */
            Unlock(iotHubClientInstance->LockHandle);
        }

        ThreadAPI_Sleep(1);
    }

    return 0;
}

static void StartWorkerThreadIfNeeded(IOTHUB_CLIENT_INSTANCE* iotHubClientInstance)
{
    if (iotHubClientInstance->ThreadHandle == NULL)
    {
        iotHubClientInstance->StopThread = 0;
        if (ThreadAPI_Create(&iotHubClientInstance->ThreadHandle, ScheduleWork_Thread, iotHubClientInstance) != THREADAPI_OK)
        {
            iotHubClientInstance->ThreadHandle = NULL;
        }
    }
}

IOTHUB_CLIENT_HANDLE IoTHubClient_CreateFromConnectionString(const char* connectionString, IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol)
{
    IOTHUB_CLIENT_INSTANCE* result = NULL;

    /* Codes_SRS_IOTHUBCLIENT_12_003: [IoTHubClient_CreateFromConnectionString shall verify the input parameter and if it is NULL then return NULL] */
    if (connectionString == NULL)
    {
        LogError("Input parameter is NULL: connectionString\r\n");
    }
    else if (protocol == NULL)
    {
        LogError("Input parameter is NULL: protocol\r\n");
    }
    else
    {
        /* Codes_SRS_IOTHUBCLIENT_12_004: [IoTHubClient_CreateFromConnectionString shall allocate a new IoTHubClient instance.] */
        result = malloc(sizeof(IOTHUB_CLIENT_INSTANCE));

        /* Codes_SRS_IOTHUBCLIENT_12_011: [If the allocation failed, IoTHubClient_CreateFromConnectionString returns NULL] */
        if (result == NULL)
        {
            LogError("Malloc failed\r\n");
        }
        else
        {
            /* Codes_SRS_IOTHUBCLIENT_12_005: [IoTHubClient_CreateFromConnectionString shall create a lock object to be used later for serializing IoTHubClient calls] */
            result->LockHandle = Lock_Init();
            if (result->LockHandle == NULL)
            {
                /* Codes_SRS_IOTHUBCLIENT_12_009: [If lock creation failed, IoTHubClient_CreateFromConnectionString shall do clean up and return NULL] */
                free(result);
                result = NULL;
                LogError("Lock_Init failed\r\n");
            }
            else
            {
                /* Codes_SRS_IOTHUBCLIENT_12_006: [IoTHubClient_CreateFromConnectionString shall instantiate a new IoTHubClient_LL instance by calling IoTHubClient_LL_CreateFromConnectionString and passing the connectionString] */
                result->IoTHubClientLLHandle = IoTHubClient_LL_CreateFromConnectionString(connectionString, protocol);
                if (result->IoTHubClientLLHandle == NULL)
                {
                    /* Codes_SRS_IOTHUBCLIENT_12_010: [If IoTHubClient_LL_CreateFromConnectionString fails then IoTHubClient_CreateFromConnectionString shall do clean - up and return NULL] */
                    Lock_Deinit(result->LockHandle);
                    free(result);
                    result = NULL;
                    LogError("IoTHubClient_LL_CreateFromConnectionString failed\r\n");
                }
                else
                { 
                    result->ThreadHandle = NULL;
                }
            }
        }
    }
    return result;
}


IOTHUB_CLIENT_HANDLE IoTHubClient_Create(const IOTHUB_CLIENT_CONFIG* config)
{
    /* Codes_SRS_IOTHUBCLIENT_01_001: [IoTHubClient_Create shall allocate a new IoTHubClient instance and return a non-NULL handle to it.] */
    IOTHUB_CLIENT_INSTANCE* result = (IOTHUB_CLIENT_INSTANCE*)malloc(sizeof(IOTHUB_CLIENT_INSTANCE));

    /* Codes_SRS_IOTHUBCLIENT_01_004: [If allocating memory for the new IoTHubClient instance fails, then IoTHubClient_Create shall return NULL.] */
    if (result != NULL)
    {
        result->ThreadHandle = NULL;

        /* Codes_SRS_IOTHUBCLIENT_01_029: [IoTHubClient_Create shall create a lock object to be used later for serializing IoTHubClient calls.] */
        result->LockHandle = Lock_Init();
        if (result->LockHandle == NULL)
        {
            /* Codes_SRS_IOTHUBCLIENT_01_030: [If creating the lock fails, then IoTHubClient_Create shall return NULL.] */
            /* Codes_SRS_IOTHUBCLIENT_01_031: [If IoTHubClient_Create fails, all resources allocated by it shall be freed.] */
            free(result);
            result = NULL;
        }
        else
        {
            /* Codes_SRS_IOTHUBCLIENT_01_002: [IoTHubClient_Create shall instantiate a new IoTHubClient_LL instance by calling IoTHubClient_LL_Create and passing the config argument.] */
            result->IoTHubClientLLHandle = IoTHubClient_LL_Create(config);
            if (result->IoTHubClientLLHandle == NULL)
            {
                /* Codes_SRS_IOTHUBCLIENT_01_003: [If IoTHubClient_LL_Create fails, then IoTHubClient_Create shall return NULL.] */
                /* Codes_SRS_IOTHUBCLIENT_01_031: [If IoTHubClient_Create fails, all resources allocated by it shall be freed.] */
                Lock_Deinit(result->LockHandle);
                free(result);
                result = NULL;
            }
        }
    }

    return result;
}

/* Codes_SRS_IOTHUBCLIENT_01_005: [IoTHubClient_Destroy shall free all resources associated with the iotHubClientHandle instance.] */
void IoTHubClient_Destroy(IOTHUB_CLIENT_HANDLE iotHubClientHandle)
{
    /* Codes_SRS_IOTHUBCLIENT_01_008: [IoTHubClient_Destroy shall do nothing if parameter iotHubClientHandle is NULL.] */
    if (iotHubClientHandle != NULL)
    {
        IOTHUB_CLIENT_INSTANCE* iotHubClientInstance = (IOTHUB_CLIENT_INSTANCE*)iotHubClientHandle;

        /* Codes_SRS_IOTHUBCLIENT_01_007: [The thread created as part of executing IoTHubClient_SendEventAsync or IoTHubClient_SetMessageCallback shall be joined.] */
        if (iotHubClientInstance->ThreadHandle != NULL)
        {
            int res;
            iotHubClientInstance->StopThread = 1;
            if (ThreadAPI_Join(iotHubClientInstance->ThreadHandle, &res) != THREADAPI_OK)
            {
                LogError("ThreadAPI_Join failed\r\n");
            }
        }

        /* Codes_SRS_IOTHUBCLIENT_01_006: [That includes destroying the IoTHubClient_LL instance by calling IoTHubClient_LL_Destroy.] */
        IoTHubClient_LL_Destroy(iotHubClientInstance->IoTHubClientLLHandle);

        /* Codes_SRS_IOTHUBCLIENT_01_032: [The lock allocated in IoTHubClient_Create shall be also freed.] */
        Lock_Deinit(iotHubClientInstance->LockHandle);
        free(iotHubClientInstance);
    }
}

IOTHUB_CLIENT_RESULT IoTHubClient_SendEventAsync(IOTHUB_CLIENT_HANDLE iotHubClientHandle, IOTHUB_MESSAGE_HANDLE eventMessageHandle, IOTHUB_CLIENT_EVENT_CONFIRMATION_CALLBACK eventConfirmationCallback, void* userContextCallback)
{
    IOTHUB_CLIENT_RESULT result;

    if (iotHubClientHandle == NULL)
    {
        /* Codes_SRS_IOTHUBCLIENT_01_011: [If iotHubClientHandle is NULL, IoTHubClient_SendEventAsync shall return IOTHUB_CLIENT_INVALID_ARG.] */
        result = IOTHUB_CLIENT_INVALID_ARG;
        LogError("NULL iothubClientHandle\r\n");
    }
    else
    {
        IOTHUB_CLIENT_INSTANCE* iotHubClientInstance = (IOTHUB_CLIENT_INSTANCE*)iotHubClientHandle;

        /* Codes_SRS_IOTHUBCLIENT_01_025: [IoTHubClient_SendEventAsync shall be made thread-safe by using the lock created in IoTHubClient_Create.] */
        if (Lock(iotHubClientInstance->LockHandle) != LOCK_OK)
        {
            /* Codes_SRS_IOTHUBCLIENT_01_026: [If acquiring the lock fails, IoTHubClient_SendEventAsync shall return IOTHUB_CLIENT_ERROR.] */
            result = IOTHUB_CLIENT_ERROR;
            LogError("Could not acquire lock\r\n");
        }
        else
        {
            /* Codes_SRS_IOTHUBCLIENT_01_009: [IoTHubClient_SendEventAsync shall start the worker thread if it was not previously started.] */
            StartWorkerThreadIfNeeded(iotHubClientInstance);

            if (iotHubClientInstance->ThreadHandle == NULL)
            {
                /* Codes_SRS_IOTHUBCLIENT_01_010: [If starting the thread fails, IoTHubClient_SendEventAsync shall return IOTHUB_CLIENT_ERROR.] */
                result = IOTHUB_CLIENT_ERROR;
                LogError("Could not start worker thread\r\n");
            }
            else
            {
                /* Codes_SRS_IOTHUBCLIENT_01_012: [IoTHubClient_SendEventAsync shall call IoTHubClient_LL_SendEventAsync, while passing the IoTHubClient_LL handle created by IoTHubClient_Create and the parameters eventMessageHandle, eventConfirmationCallback and userContextCallback.] */
                /* Codes_SRS_IOTHUBCLIENT_01_013: [When IoTHubClient_LL_SendEventAsync is called, IoTHubClient_SendEventAsync shall return the result of IoTHubClient_LL_SendEventAsync.] */
                result = IoTHubClient_LL_SendEventAsync(iotHubClientInstance->IoTHubClientLLHandle, eventMessageHandle, eventConfirmationCallback, userContextCallback);
            }

            /* Codes_SRS_IOTHUBCLIENT_01_025: [IoTHubClient_SendEventAsync shall be made thread-safe by using the lock created in IoTHubClient_Create.] */
            (void)Unlock(iotHubClientInstance->LockHandle);
        }
    }

    return result;
}

IOTHUB_CLIENT_RESULT IoTHubClient_GetSendStatus(IOTHUB_CLIENT_HANDLE iotHubClientHandle, IOTHUB_CLIENT_STATUS *iotHubClientStatus)
{
    IOTHUB_CLIENT_RESULT result;

    if (iotHubClientHandle == NULL)
    {
        /* Codes_SRS_IOTHUBCLIENT_01_023: [If iotHubClientHandle is NULL, IoTHubClient_ GetSendStatus shall return IOTHUB_CLIENT_INVALID_ARG.] */
        result = IOTHUB_CLIENT_INVALID_ARG;
        LogError("NULL iothubClientHandle\r\n");
    }
    else
    {
        IOTHUB_CLIENT_INSTANCE* iotHubClientInstance = (IOTHUB_CLIENT_INSTANCE*)iotHubClientHandle;

        /* Codes_SRS_IOTHUBCLIENT_01_033: [IoTHubClient_GetSendStatus shall be made thread-safe by using the lock created in IoTHubClient_Create.] */
        if (Lock(iotHubClientInstance->LockHandle) != LOCK_OK)
        {
            /* Codes_SRS_IOTHUBCLIENT_01_034: [If acquiring the lock fails, IoTHubClient_GetSendStatus shall return IOTHUB_CLIENT_ERROR.] */
            result = IOTHUB_CLIENT_ERROR;
            LogError("Could not acquire lock\r\n");
        }
        else
        {
            /* Codes_SRS_IOTHUBCLIENT_01_022: [IoTHubClient_GetSendStatus shall call IoTHubClient_LL_GetSendStatus, while passing the IoTHubClient_LL handle created by IoTHubClient_Create and the parameter iotHubClientStatus.] */
            /* Codes_SRS_IOTHUBCLIENT_01_024: [Otherwise, IoTHubClient_GetSendStatus shall return the result of IoTHubClient_LL_GetSendStatus.] */
            result = IoTHubClient_LL_GetSendStatus(iotHubClientInstance->IoTHubClientLLHandle, iotHubClientStatus);

            /* Codes_SRS_IOTHUBCLIENT_01_033: [IoTHubClient_GetSendStatus shall be made thread-safe by using the lock created in IoTHubClient_Create.] */
            (void)Unlock(iotHubClientInstance->LockHandle);
        }
    }

    return result;
}

IOTHUB_CLIENT_RESULT IoTHubClient_SetMessageCallback(IOTHUB_CLIENT_HANDLE iotHubClientHandle, IOTHUB_CLIENT_MESSAGE_CALLBACK_ASYNC messageCallback, void* userContextCallback)
{
    IOTHUB_CLIENT_RESULT result;

    if (iotHubClientHandle == NULL)
    {
        /* Codes_SRS_IOTHUBCLIENT_01_016: [If iotHubClientHandle is NULL, IoTHubClient_SetMessageCallback shall return IOTHUB_CLIENT_INVALID_ARG.] */
        result = IOTHUB_CLIENT_INVALID_ARG;
        LogError("NULL iothubClientHandle\r\n");
    }
    else
    {
        IOTHUB_CLIENT_INSTANCE* iotHubClientInstance = (IOTHUB_CLIENT_INSTANCE*)iotHubClientHandle;

        /* Codes_SRS_IOTHUBCLIENT_01_027: [IoTHubClient_SetMessageCallback shall be made thread-safe by using the lock created in IoTHubClient_Create.] */
        if (Lock(iotHubClientInstance->LockHandle) != LOCK_OK)
        {
            /* Codes_SRS_IOTHUBCLIENT_01_028: [If acquiring the lock fails, IoTHubClient_SetMessageCallback shall return IOTHUB_CLIENT_ERROR.] */
            result = IOTHUB_CLIENT_ERROR;
            LogError("Could not acquire lock\r\n");
        }
        else
        {
            /* Codes_SRS_IOTHUBCLIENT_01_014: [IoTHubClient_SetMessageCallback shall start the worker thread if it was not previously started.] */
            StartWorkerThreadIfNeeded(iotHubClientInstance);

            if (iotHubClientInstance->ThreadHandle == NULL)
            {
                /* Codes_SRS_IOTHUBCLIENT_01_015: [If starting the thread fails, IoTHubClient_SetMessageCallback shall return IOTHUB_CLIENT_ERROR.] */
                result = IOTHUB_CLIENT_ERROR;
                LogError("Could not start worker thread\r\n");
            }
            else
            {
                /* Codes_SRS_IOTHUBCLIENT_01_017: [IoTHubClient_SetMessageCallback shall call IoTHubClient_LL_SetMessageCallback, while passing the IoTHubClient_LL handle created by IoTHubClient_Create and the parameters messageCallback and userContextCallback.] */
                result = IoTHubClient_LL_SetMessageCallback(iotHubClientInstance->IoTHubClientLLHandle, messageCallback, userContextCallback);
            }

            /* Codes_SRS_IOTHUBCLIENT_01_027: [IoTHubClient_SetMessageCallback shall be made thread-safe by using the lock created in IoTHubClient_Create.] */
            Unlock(iotHubClientInstance->LockHandle);
        }
    }

    return result;
}

IOTHUB_CLIENT_RESULT IoTHubClient_GetLastMessageReceiveTime(IOTHUB_CLIENT_HANDLE iotHubClientHandle, time_t* lastMessageReceiveTime)
{
    IOTHUB_CLIENT_RESULT result;

    if (iotHubClientHandle == NULL)
    {
        /* Codes_SRS_IOTHUBCLIENT_01_020: [If iotHubClientHandle is NULL, IoTHubClient_GetLastMessageReceiveTime shall return IOTHUB_CLIENT_INVALID_ARG.] */
        result = IOTHUB_CLIENT_INVALID_ARG;
        LogError("NULL iothubClientHandle\r\n");
    }
    else
    {
        IOTHUB_CLIENT_INSTANCE* iotHubClientInstance = (IOTHUB_CLIENT_INSTANCE*)iotHubClientHandle;

        /* Codes_SRS_IOTHUBCLIENT_01_035: [IoTHubClient_GetLastMessageReceiveTime shall be made thread-safe by using the lock created in IoTHubClient_Create.] */
        if (Lock(iotHubClientInstance->LockHandle) != LOCK_OK)
        {
            /* Codes_SRS_IOTHUBCLIENT_01_036: [If acquiring the lock fails, IoTHubClient_GetLastMessageReceiveTime shall return IOTHUB_CLIENT_ERROR.] */
            result = IOTHUB_CLIENT_ERROR;
            LogError("Could not acquire lock\r\n");
        }
        else
        {
            /* Codes_SRS_IOTHUBCLIENT_01_019: [IoTHubClient_GetLastMessageReceiveTime shall call IoTHubClient_LL_GetLastMessageReceiveTime, while passing the IoTHubClient_LL handle created by IoTHubClient_Create and the parameter lastMessageReceiveTime.] */
            /* Codes_SRS_IOTHUBCLIENT_01_021: [Otherwise, IoTHubClient_GetLastMessageReceiveTime shall return the result of IoTHubClient_LL_GetLastMessageReceiveTime.] */
            result = IoTHubClient_LL_GetLastMessageReceiveTime(iotHubClientInstance->IoTHubClientLLHandle, lastMessageReceiveTime);

            /* Codes_SRS_IOTHUBCLIENT_01_035: [IoTHubClient_GetLastMessageReceiveTime shall be made thread-safe by using the lock created in IoTHubClient_Create.] */
            Unlock(iotHubClientInstance->LockHandle);
        }
    }

    return result;
}

IOTHUB_CLIENT_RESULT IoTHubClient_SetOption(IOTHUB_CLIENT_HANDLE iotHubClientHandle, const char* optionName, const void* value)
{
    IOTHUB_CLIENT_RESULT result;
    /*Codes_SRS_IOTHUBCLIENT_02_034: [If parameter iotHubClientHandle is NULL then IoTHubClient_SetOption shall return IOTHUB_CLIENT_INVALID_ARG.] */
    if (
        (iotHubClientHandle == NULL) ||
        (optionName == NULL) ||
        (value == NULL)
        )
    {
        result = IOTHUB_CLIENT_INVALID_ARG;
        LogError("invalid arg (NULL)r\n");
    }
    else
    {
        IOTHUB_CLIENT_INSTANCE* iotHubClientInstance = (IOTHUB_CLIENT_INSTANCE*)iotHubClientHandle;
        /*Codes_SRS_IOTHUBCLIENT_02_038: [If optionName doesn't match one of the options handled by this module then IoTHubClient_SetOption shall call IoTHubClient_LL_SetOption passing the same parameters and return what IoTHubClient_LL_SetOption returns.] */
        result = IoTHubClient_LL_SetOption(iotHubClientInstance->IoTHubClientLLHandle, optionName, value);

        if (result != IOTHUB_CLIENT_OK)
        {
            LogError("IoTHubClient_LL_SetOption failed\r\n");
        }
    }
    return result;
}
