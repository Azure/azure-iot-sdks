// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h> 
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#include "azure_c_shared_utility/gballoc.h"

#include <stdlib.h>
#include <signal.h>
#include <stddef.h>
#include "azure_c_shared_utility/crt_abstractions.h"
#include "iothub_client.h"
#include "iothub_client_ll.h"
#include "iothubtransport.h"
#include "azure_c_shared_utility/threadapi.h"
#include "azure_c_shared_utility/lock.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/singlylinkedlist.h"

typedef struct IOTHUB_CLIENT_INSTANCE_TAG
{
    IOTHUB_CLIENT_LL_HANDLE IoTHubClientLLHandle;
    TRANSPORT_HANDLE TransportHandle;
    THREAD_HANDLE ThreadHandle;
    LOCK_HANDLE LockHandle;
    sig_atomic_t StopThread;
#ifndef DONT_USE_UPLOADTOBLOB
    SINGLYLINKEDLIST_HANDLE savedDataToBeCleaned; /*list containing UPLOADTOBLOB_SAVED_DATA*/
#endif
} IOTHUB_CLIENT_INSTANCE;

#ifndef DONT_USE_UPLOADTOBLOB
typedef struct UPLOADTOBLOB_SAVED_DATA_TAG
{
    unsigned char* source;
    size_t size;
    char* destinationFileName;
    IOTHUB_CLIENT_FILE_UPLOAD_CALLBACK iotHubClientFileUploadCallback;
    void* context;
    THREAD_HANDLE uploadingThreadHandle;
    IOTHUB_CLIENT_HANDLE iotHubClientHandle;
    LOCK_HANDLE lockGarbage;
    int canBeGarbageCollected; /*flag indicating that the UPLOADTOBLOB_SAVED_DATA structure can be freed because the thread deadling with it finished*/
}UPLOADTOBLOB_SAVED_DATA;
#endif

/*used by unittests only*/
const size_t IoTHubClient_ThreadTerminationOffset = offsetof(IOTHUB_CLIENT_INSTANCE, StopThread);

#ifndef DONT_USE_UPLOADTOBLOB
/*this function is called from _Destroy and from ScheduleWork_Thread to join finished blobUpload threads and free that memory*/
static void garbageCollectorImpl(IOTHUB_CLIENT_INSTANCE* iotHubClientInstance)
{
    /*see if any savedData structures can be disposed of*/
    /*Codes_SRS_IOTHUBCLIENT_02_072: [ All threads marked as disposable (upon completion of a file upload) shall be joined and the data structures build for them shall be freed. ]*/
    LIST_ITEM_HANDLE item = singlylinkedlist_get_head_item(iotHubClientInstance->savedDataToBeCleaned);
    while (item != NULL)
    {
        const UPLOADTOBLOB_SAVED_DATA* savedData = (const UPLOADTOBLOB_SAVED_DATA*)singlylinkedlist_item_get_value(item);
        LIST_ITEM_HANDLE old_item = item;
        item = singlylinkedlist_get_next_item(item);

        if (Lock(savedData->lockGarbage) != LOCK_OK)
        {
            LogError("unable to Lock");
        }
        else
        {
            if (savedData->canBeGarbageCollected == 1)
            {
                int notUsed;
                if (ThreadAPI_Join(savedData->uploadingThreadHandle, &notUsed) != THREADAPI_OK)
                {
                    LogError("unable to ThreadAPI_Join");
                }
                (void)singlylinkedlist_remove(iotHubClientInstance->savedDataToBeCleaned, old_item);
                free((void*)savedData->source);
                free((void*)savedData->destinationFileName);

                if (Unlock(savedData->lockGarbage) != LOCK_OK)
                {
                    LogError("unable to unlock after locking");
                }
                (void)Lock_Deinit(savedData->lockGarbage);
                free((void*)savedData);
            }
            else
            {
                if (Unlock(savedData->lockGarbage) != LOCK_OK)
                {
                    LogError("unable to unlock after locking");
                }
            }
        }
    }
}
#endif

static int ScheduleWork_Thread(void* threadArgument)
{
    IOTHUB_CLIENT_INSTANCE* iotHubClientInstance = (IOTHUB_CLIENT_INSTANCE*)threadArgument;

    while (1)
    {
        if (Lock(iotHubClientInstance->LockHandle) == LOCK_OK)
        {
            /*Codes_SRS_IOTHUBCLIENT_01_038: [ The thread shall exit when IoTHubClient_Destroy is called. ]*/
            if (iotHubClientInstance->StopThread)
            {
                (void)Unlock(iotHubClientInstance->LockHandle);
                break; /*gets out of the thread*/
            }
            else
            {
                /* Codes_SRS_IOTHUBCLIENT_01_037: [The thread created by IoTHubClient_SendEvent or IoTHubClient_SetMessageCallback shall call IoTHubClient_LL_DoWork every 1 ms.] */
                /* Codes_SRS_IOTHUBCLIENT_01_039: [All calls to IoTHubClient_LL_DoWork shall be protected by the lock created in IotHubClient_Create.] */
                IoTHubClient_LL_DoWork(iotHubClientInstance->IoTHubClientLLHandle);

#ifndef DONT_USE_UPLOADTOBLOB
                garbageCollectorImpl(iotHubClientInstance);
#endif
                (void)Unlock(iotHubClientInstance->LockHandle);
            }
        }
        else
        {
            /*Codes_SRS_IOTHUBCLIENT_01_040: [If acquiring the lock fails, IoTHubClient_LL_DoWork shall not be called.]*/
            /*no code, shall retry*/
        }
        (void)ThreadAPI_Sleep(1);
    }

    return 0;
}

static IOTHUB_CLIENT_RESULT StartWorkerThreadIfNeeded(IOTHUB_CLIENT_INSTANCE* iotHubClientInstance)
{
    IOTHUB_CLIENT_RESULT result;
    if (iotHubClientInstance->TransportHandle == NULL)
    {
        if (iotHubClientInstance->ThreadHandle == NULL)
        {
            iotHubClientInstance->StopThread = 0;
            if (ThreadAPI_Create(&iotHubClientInstance->ThreadHandle, ScheduleWork_Thread, iotHubClientInstance) != THREADAPI_OK)
            {
                iotHubClientInstance->ThreadHandle = NULL;
                result = IOTHUB_CLIENT_ERROR;
            }
            else
            {
                result = IOTHUB_CLIENT_OK;
            }
        }
        else
        {
            result = IOTHUB_CLIENT_OK;
        }
    }
    else
    {
        /*Codes_SRS_IOTHUBCLIENT_17_012: [ If the transport connection is shared, the thread shall be started by calling IoTHubTransport_StartWorkerThread. ]*/
        /*Codes_SRS_IOTHUBCLIENT_17_011: [ If the transport connection is shared, the thread shall be started by calling IoTHubTransport_StartWorkerThread*/

        result = IoTHubTransport_StartWorkerThread(iotHubClientInstance->TransportHandle, iotHubClientInstance);
    }
    return result;
}

IOTHUB_CLIENT_HANDLE IoTHubClient_CreateFromConnectionString(const char* connectionString, IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol)
{
    IOTHUB_CLIENT_INSTANCE* result = NULL;

    /* Codes_SRS_IOTHUBCLIENT_12_003: [IoTHubClient_CreateFromConnectionString shall verify the input parameter and if it is NULL then return NULL] */
    if (connectionString == NULL)
    {
        LogError("Input parameter is NULL: connectionString");
    }
    else if (protocol == NULL)
    {
        LogError("Input parameter is NULL: protocol");
    }
    else
    {
        /* Codes_SRS_IOTHUBCLIENT_12_004: [IoTHubClient_CreateFromConnectionString shall allocate a new IoTHubClient instance.] */
        result = (IOTHUB_CLIENT_INSTANCE *) malloc(sizeof(IOTHUB_CLIENT_INSTANCE));

        /* Codes_SRS_IOTHUBCLIENT_12_011: [If the allocation failed, IoTHubClient_CreateFromConnectionString returns NULL] */
        if (result == NULL)
        {
            LogError("Malloc failed");
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
                LogError("Lock_Init failed");
            }
            else
            {
#ifndef DONT_USE_UPLOADTOBLOB
                /*Codes_SRS_IOTHUBCLIENT_02_059: [ IoTHubClient_CreateFromConnectionString shall create a SINGLYLINKEDLIST_HANDLE containing THREAD_HANDLE (created by future calls to IoTHubClient_UploadToBlobAsync). ]*/
                if ((result->savedDataToBeCleaned = singlylinkedlist_create()) == NULL)
                {
                    /*Codes_SRS_IOTHUBCLIENT_02_070: [ If creating the SINGLYLINKEDLIST_HANDLE fails then IoTHubClient_CreateFromConnectionString shall fail and return NULL]*/
                    LogError("unable to singlylinkedlist_create");
                    Lock_Deinit(result->LockHandle);
                    free(result);
                    result = NULL;
                }
                else
#endif
                {
                    /* Codes_SRS_IOTHUBCLIENT_12_006: [IoTHubClient_CreateFromConnectionString shall instantiate a new IoTHubClient_LL instance by calling IoTHubClient_LL_CreateFromConnectionString and passing the connectionString] */
                    result->IoTHubClientLLHandle = IoTHubClient_LL_CreateFromConnectionString(connectionString, protocol);
                    if (result->IoTHubClientLLHandle == NULL)
                    {
                        /* Codes_SRS_IOTHUBCLIENT_12_010: [If IoTHubClient_LL_CreateFromConnectionString fails then IoTHubClient_CreateFromConnectionString shall do clean - up and return NULL] */
#ifndef DONT_USE_UPLOADTOBLOB
                        singlylinkedlist_destroy(result->savedDataToBeCleaned);
#endif
                        Lock_Deinit(result->LockHandle);
                        free(result);
                        result = NULL;
                        LogError("IoTHubClient_LL_CreateFromConnectionString failed");
                    }
                    else
                    {
                        result->ThreadHandle = NULL;
                        result->TransportHandle = NULL;
                    }
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
#ifndef DONT_USE_UPLOADTOBLOB
            /*Codes_SRS_IOTHUBCLIENT_02_060: [ IoTHubClient_Create shall create a SINGLYLINKEDLIST_HANDLE containing THREAD_HANDLE (created by future calls to IoTHubClient_UploadToBlobAsync). ]*/
            if ((result->savedDataToBeCleaned = singlylinkedlist_create()) == NULL)
            {
                /*Codes_SRS_IOTHUBCLIENT_02_061: [ If creating the SINGLYLINKEDLIST_HANDLE fails then IoTHubClient_Create shall fail and return NULL. ]*/
                LogError("unable to singlylinkedlist_create");
                Lock_Deinit(result->LockHandle);
                free(result);
                result = NULL;
            }
            else
#endif
            {
                /* Codes_SRS_IOTHUBCLIENT_01_002: [IoTHubClient_Create shall instantiate a new IoTHubClient_LL instance by calling IoTHubClient_LL_Create and passing the config argument.] */
                result->IoTHubClientLLHandle = IoTHubClient_LL_Create(config);
                if (result->IoTHubClientLLHandle == NULL)
                {
                    /* Codes_SRS_IOTHUBCLIENT_01_003: [If IoTHubClient_LL_Create fails, then IoTHubClient_Create shall return NULL.] */
                    /* Codes_SRS_IOTHUBCLIENT_01_031: [If IoTHubClient_Create fails, all resources allocated by it shall be freed.] */
                    Lock_Deinit(result->LockHandle);
#ifndef DONT_USE_UPLOADTOBLOB
                    singlylinkedlist_destroy(result->savedDataToBeCleaned);
#endif
                    free(result);
                    result = NULL;
                }
                else
                {
                    result->TransportHandle = NULL;
                    result->ThreadHandle = NULL;
                }
            }
        }
    }

    return result;
}

IOTHUB_CLIENT_HANDLE IoTHubClient_CreateWithTransport(TRANSPORT_HANDLE transportHandle, const IOTHUB_CLIENT_CONFIG* config)
{
    IOTHUB_CLIENT_INSTANCE* result;
    /*Codes_SRS_IOTHUBCLIENT_17_013: [ IoTHubClient_CreateWithTransport shall return NULL if transportHandle is NULL. ]*/
    /*Codes_SRS_IOTHUBCLIENT_17_014: [ IoTHubClient_CreateWithTransport shall return NULL if config is NULL. ]*/
    if (transportHandle == NULL || config == NULL)
    {
        LogError("invalid parameter TRANSPORT_HANDLE transportHandle=%p, const IOTHUB_CLIENT_CONFIG* config=%p", transportHandle, config);
        result = NULL;
    }
    else
    {
        /*Codes_SRS_IOTHUBCLIENT_17_001: [ IoTHubClient_CreateWithTransport shall allocate a new IoTHubClient instance and return a non-NULL handle to it. ]*/
        result = (IOTHUB_CLIENT_INSTANCE*)malloc(sizeof(IOTHUB_CLIENT_INSTANCE));
        /*Codes_SRS_IOTHUBCLIENT_17_002: [ If allocating memory for the new IoTHubClient instance fails, then IoTHubClient_CreateWithTransport shall return NULL. ]*/
        if (result == NULL)
        {
            LogError("unable to malloc");
            /*return as is*/
        }
        else
        {
#ifndef DONT_USE_UPLOADTOBLOB
            /*Codes_SRS_IOTHUBCLIENT_02_073: [ IoTHubClient_CreateWithTransport shall create a SINGLYLINKEDLIST_HANDLE that shall be used by IoTHubClient_UploadToBlobAsync. ]*/
            if ((result->savedDataToBeCleaned = singlylinkedlist_create()) == NULL)
            {
                /*Codes_SRS_IOTHUBCLIENT_02_074: [ If creating the SINGLYLINKEDLIST_HANDLE fails then IoTHubClient_CreateWithTransport shall fail and return NULL. ]*/
                LogError("unable to singlylinkedlist_create");
                free(result);
                result = NULL;
            }
            else
#endif
            {
                result->ThreadHandle = NULL;
                result->TransportHandle = transportHandle;
                /*Codes_SRS_IOTHUBCLIENT_17_005: [ IoTHubClient_CreateWithTransport shall call IoTHubTransport_GetLock to get the transport lock to be used later for serializing IoTHubClient calls. ]*/
                LOCK_HANDLE transportLock = IoTHubTransport_GetLock(transportHandle);
                result->LockHandle = transportLock;
                if (result->LockHandle == NULL)
                {
                    LogError("unable to IoTHubTransport_GetLock");
                    /*Codes_SRS_IOTHUBCLIENT_17_006: [ If IoTHubTransport_GetLock fails, then IoTHubClient_CreateWithTransport shall return NULL. ]*/
#ifndef DONT_USE_UPLOADTOBLOB
                    singlylinkedlist_destroy(result->savedDataToBeCleaned);
#endif
                    free(result);
                    result = NULL;
                }
                else
                {
                    IOTHUB_CLIENT_DEVICE_CONFIG deviceConfig;
                    deviceConfig.deviceId = config->deviceId;
                    deviceConfig.deviceKey = config->deviceKey;
                    deviceConfig.protocol = config->protocol;
                    deviceConfig.deviceSasToken = config->deviceSasToken;
                    deviceConfig.protocol = config->protocol;

                    /*Codes_SRS_IOTHUBCLIENT_17_003: [ IoTHubClient_CreateWithTransport shall call IoTHubTransport_GetLLTransport on transportHandle to get lower layer transport. ]*/
                    deviceConfig.transportHandle = IoTHubTransport_GetLLTransport(transportHandle);

                    if (deviceConfig.transportHandle == NULL)
                    {
                        LogError("unable to IoTHubTransport_GetLLTransport");
                        /*Codes_SRS_IOTHUBCLIENT_17_004: [ If IoTHubTransport_GetLLTransport fails, then IoTHubClient_CreateWithTransport shall return NULL. ]*/
#ifndef DONT_USE_UPLOADTOBLOB
                        singlylinkedlist_destroy(result->savedDataToBeCleaned);
#endif
                        free(result);
                        result = NULL;
                    }
                    else
                    {
                        if (Lock(transportLock) != LOCK_OK)
                        {
                            LogError("unable to Lock");
#ifndef DONT_USE_UPLOADTOBLOB
                            singlylinkedlist_destroy(result->savedDataToBeCleaned);
#endif
                            free(result);
                            result = NULL;
                        }
                        else
                        {
                            /*Codes_SRS_IOTHUBCLIENT_17_007: [ IoTHubClient_CreateWithTransport shall instantiate a new IoTHubClient_LL instance by calling IoTHubClient_LL_CreateWithTransport and passing the lower layer transport and config argument. ]*/
                            result->IoTHubClientLLHandle = IoTHubClient_LL_CreateWithTransport(&deviceConfig);
                            if (result->IoTHubClientLLHandle == NULL)
                            {
                                LogError("unable to IoTHubClient_LL_CreateWithTransport");
                                /*Codes_SRS_IOTHUBCLIENT_17_008: [ If IoTHubClient_LL_CreateWithTransport fails, then IoTHubClient_Create shall return NULL. ]*/
                                /*Codes_SRS_IOTHUBCLIENT_17_009: [ If IoTHubClient_LL_CreateWithTransport fails, all resources allocated by it shall be freed. ]*/
#ifndef DONT_USE_UPLOADTOBLOB
                                singlylinkedlist_destroy(result->savedDataToBeCleaned);
#endif
                                free(result);
                                result = NULL;
                            }

                            if (Unlock(transportLock) != LOCK_OK)
                            {
                                LogError("unable to Unlock");
                            }
                        }
                    }
                }
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
        bool okToJoin;

        IOTHUB_CLIENT_INSTANCE* iotHubClientInstance = (IOTHUB_CLIENT_INSTANCE*)iotHubClientHandle;

        /*Codes_SRS_IOTHUBCLIENT_02_043: [ IoTHubClient_Destroy shall lock the serializing lock and signal the worker thread (if any) to end ]*/
        if (Lock(iotHubClientInstance->LockHandle) != LOCK_OK)
        {
            LogError("unable to Lock - - will still proceed to try to end the thread without locking");
        }

#ifndef DONT_USE_UPLOADTOBLOB
        /*Codes_SRS_IOTHUBCLIENT_02_069: [ IoTHubClient_Destroy shall free all data created by IoTHubClient_UploadToBlobAsync ]*/
        /*wait for all uploading threads to finish*/
        while (singlylinkedlist_get_head_item(iotHubClientInstance->savedDataToBeCleaned) != NULL)
        {
            garbageCollectorImpl(iotHubClientInstance);
        }
#endif
        if (iotHubClientInstance->ThreadHandle != NULL)
        {
            iotHubClientInstance->StopThread = 1;
            okToJoin = true;
        }
        else
        {
            okToJoin = false;
        }

        if (iotHubClientInstance->TransportHandle != NULL)
        {
            /*Codes_SRS_IOTHUBCLIENT_01_007: [ The thread created as part of executing IoTHubClient_SendEventAsync or IoTHubClient_SetNotificationMessageCallback shall be joined. ]*/
            okToJoin = IoTHubTransport_SignalEndWorkerThread(iotHubClientInstance->TransportHandle, iotHubClientHandle);
        }

        /* Codes_SRS_IOTHUBCLIENT_01_006: [That includes destroying the IoTHubClient_LL instance by calling IoTHubClient_LL_Destroy.] */
        IoTHubClient_LL_Destroy(iotHubClientInstance->IoTHubClientLLHandle);

#ifndef DONT_USE_UPLOADTOBLOB
        if (iotHubClientInstance->savedDataToBeCleaned != NULL)
        {
            singlylinkedlist_destroy(iotHubClientInstance->savedDataToBeCleaned);
        }
#endif

        /*Codes_SRS_IOTHUBCLIENT_02_045: [ IoTHubClient_Destroy shall unlock the serializing lock. ]*/
        if (Unlock(iotHubClientInstance->LockHandle) != LOCK_OK)
        {
            LogError("unable to Unlock");
        }

        if (okToJoin == true)
        {
            if (iotHubClientInstance->ThreadHandle != NULL)
            {
                int res;
                /*Codes_SRS_IOTHUBCLIENT_01_007: [ The thread created as part of executing IoTHubClient_SendEventAsync or IoTHubClient_SetNotificationMessageCallback shall be joined. ]*/
                if (ThreadAPI_Join(iotHubClientInstance->ThreadHandle, &res) != THREADAPI_OK)
                {
                    LogError("ThreadAPI_Join failed");
                }
            }
            if (iotHubClientInstance->TransportHandle != NULL)
            {
                /*Codes_SRS_IOTHUBCLIENT_01_007: [ The thread created as part of executing IoTHubClient_SendEventAsync or IoTHubClient_SetNotificationMessageCallback shall be joined. ]*/
                IoTHubTransport_JoinWorkerThread(iotHubClientInstance->TransportHandle, iotHubClientHandle);
            }
        }

        if (iotHubClientInstance->TransportHandle == NULL)
        {
            /* Codes_SRS_IOTHUBCLIENT_01_032: [If the lock was allocated in IoTHubClient_Create, it shall be also freed..] */
            Lock_Deinit(iotHubClientInstance->LockHandle);
        }

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
        LogError("NULL iothubClientHandle");
    }
    else
    {
        IOTHUB_CLIENT_INSTANCE* iotHubClientInstance = (IOTHUB_CLIENT_INSTANCE*)iotHubClientHandle;

        /* Codes_SRS_IOTHUBCLIENT_01_025: [IoTHubClient_SendEventAsync shall be made thread-safe by using the lock created in IoTHubClient_Create.] */
        if (Lock(iotHubClientInstance->LockHandle) != LOCK_OK)
        {
            /* Codes_SRS_IOTHUBCLIENT_01_026: [If acquiring the lock fails, IoTHubClient_SendEventAsync shall return IOTHUB_CLIENT_ERROR.] */
            result = IOTHUB_CLIENT_ERROR;
            LogError("Could not acquire lock");
        }
        else
        {
            /* Codes_SRS_IOTHUBCLIENT_01_009: [IoTHubClient_SendEventAsync shall start the worker thread if it was not previously started.] */
            if ((result = StartWorkerThreadIfNeeded(iotHubClientInstance)) != IOTHUB_CLIENT_OK)
            {
                /* Codes_SRS_IOTHUBCLIENT_01_010: [If starting the thread fails, IoTHubClient_SendEventAsync shall return IOTHUB_CLIENT_ERROR.] */
                result = IOTHUB_CLIENT_ERROR;
                LogError("Could not start worker thread");
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
        LogError("NULL iothubClientHandle");
    }
    else
    {
        IOTHUB_CLIENT_INSTANCE* iotHubClientInstance = (IOTHUB_CLIENT_INSTANCE*)iotHubClientHandle;

        /* Codes_SRS_IOTHUBCLIENT_01_033: [IoTHubClient_GetSendStatus shall be made thread-safe by using the lock created in IoTHubClient_Create.] */
        if (Lock(iotHubClientInstance->LockHandle) != LOCK_OK)
        {
            /* Codes_SRS_IOTHUBCLIENT_01_034: [If acquiring the lock fails, IoTHubClient_GetSendStatus shall return IOTHUB_CLIENT_ERROR.] */
            result = IOTHUB_CLIENT_ERROR;
            LogError("Could not acquire lock");
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
        LogError("NULL iothubClientHandle");
    }
    else
    {
        IOTHUB_CLIENT_INSTANCE* iotHubClientInstance = (IOTHUB_CLIENT_INSTANCE*)iotHubClientHandle;

        /* Codes_SRS_IOTHUBCLIENT_01_027: [IoTHubClient_SetMessageCallback shall be made thread-safe by using the lock created in IoTHubClient_Create.] */
        if (Lock(iotHubClientInstance->LockHandle) != LOCK_OK)
        {
            /* Codes_SRS_IOTHUBCLIENT_01_028: [If acquiring the lock fails, IoTHubClient_SetMessageCallback shall return IOTHUB_CLIENT_ERROR.] */
            result = IOTHUB_CLIENT_ERROR;
            LogError("Could not acquire lock");
        }
        else
        {
            /* Codes_SRS_IOTHUBCLIENT_01_014: [IoTHubClient_SetMessageCallback shall start the worker thread if it was not previously started.] */
            if ((result = StartWorkerThreadIfNeeded(iotHubClientInstance)) != IOTHUB_CLIENT_OK)
            {
                /* Codes_SRS_IOTHUBCLIENT_01_015: [If starting the thread fails, IoTHubClient_SetMessageCallback shall return IOTHUB_CLIENT_ERROR.] */
                result = IOTHUB_CLIENT_ERROR;
                LogError("Could not start worker thread");
            }
            else
            {
                /* Codes_SRS_IOTHUBCLIENT_01_017: [IoTHubClient_SetMessageCallback shall call IoTHubClient_LL_SetMessageCallback, while passing the IoTHubClient_LL handle created by IoTHubClient_Create and the parameters messageCallback and userContextCallback.] */
                result = IoTHubClient_LL_SetMessageCallback(iotHubClientInstance->IoTHubClientLLHandle, messageCallback, userContextCallback);
            }

            /* Codes_SRS_IOTHUBCLIENT_01_027: [IoTHubClient_SetMessageCallback shall be made thread-safe by using the lock created in IoTHubClient_Create.] */
            (void)Unlock(iotHubClientInstance->LockHandle);
        }
    }

    return result;
}

IOTHUB_CLIENT_RESULT IoTHubClient_SetConnectionStatusCallback(IOTHUB_CLIENT_HANDLE iotHubClientHandle, IOTHUB_CLIENT_CONNECTION_STATUS_CALLBACK connectionStatusCallback, void * userContextCallback)
{
    IOTHUB_CLIENT_RESULT result;

    if (iotHubClientHandle == NULL)
    {
        /* Codes_SRS_IOTHUBCLIENT_25_076: [** If `iotHubClientHandle` is `NULL`, `IoTHubClient_SetRetryPolicy` shall return `IOTHUB_CLIENT_INVALID_ARG`. ] */
        result = IOTHUB_CLIENT_INVALID_ARG;
        LogError("NULL iothubClientHandle");
    }
    else
    {
        IOTHUB_CLIENT_INSTANCE* iotHubClientInstance = (IOTHUB_CLIENT_INSTANCE*)iotHubClientHandle;

        /* Codes_SRS_IOTHUBCLIENT_25_087: [ `IoTHubClient_SetConnectionStatusCallback` shall be made thread-safe by using the lock created in `IoTHubClient_Create`. ] */
        if (Lock(iotHubClientInstance->LockHandle) != LOCK_OK)
        {
            /* Codes_SRS_IOTHUBCLIENT_25_088: [ If acquiring the lock fails, `IoTHubClient_SetConnectionStatusCallback` shall return `IOTHUB_CLIENT_ERROR`. ]*/
            result = IOTHUB_CLIENT_ERROR;
            LogError("Could not acquire lock");
        }
        else
        {
            /* Codes_SRS_IOTHUBCLIENT_25_081: [ `IoTHubClient_SetConnectionStatusCallback` shall start the worker thread if it was not previously started. ]*/
            if ((result = StartWorkerThreadIfNeeded(iotHubClientInstance)) != IOTHUB_CLIENT_OK)
            {
                /* Codes_SRS_IOTHUBCLIENT_25_083: [ If starting the thread fails, `IoTHubClient_SetConnectionStatusCallback` shall return `IOTHUB_CLIENT_ERROR`. ]*/
                result = IOTHUB_CLIENT_ERROR;
                LogError("Could not start worker thread");
            }
            else
            {
                /* Codes_SRS_IOTHUBCLIENT_25_085: [ `IoTHubClient_SetConnectionStatusCallback` shall call `IoTHubClient_LL_SetConnectionStatusCallback`, while passing the `IoTHubClient_LL` handle created by `IoTHubClient_Create` and the parameters `connectionStatusCallback` and `userContextCallback`. ]*/
                result = IoTHubClient_LL_SetConnectionStatusCallback(iotHubClientInstance->IoTHubClientLLHandle, connectionStatusCallback, userContextCallback);
            }

            (void)Unlock(iotHubClientInstance->LockHandle);
        }
    }

    return result;

}

IOTHUB_CLIENT_RESULT IoTHubClient_SetRetryPolicy(IOTHUB_CLIENT_HANDLE iotHubClientHandle, IOTHUB_CLIENT_RETRY_POLICY retryPolicy, size_t retryTimeoutLimitInSeconds)
{
    IOTHUB_CLIENT_RESULT result;

    if (iotHubClientHandle == NULL)
    {
        /* Codes_SRS_IOTHUBCLIENT_25_076: [** If `iotHubClientHandle` is `NULL`, `IoTHubClient_SetRetryPolicy` shall return `IOTHUB_CLIENT_INVALID_ARG`. ] */
        result = IOTHUB_CLIENT_INVALID_ARG;
        LogError("NULL iothubClientHandle");
    }
    else
    {
        IOTHUB_CLIENT_INSTANCE* iotHubClientInstance = (IOTHUB_CLIENT_INSTANCE*)iotHubClientHandle;

        /* Codes_SRS_IOTHUBCLIENT_25_079: [ `IoTHubClient_SetRetryPolicy` shall be made thread-safe by using the lock created in `IoTHubClient_Create`.] */
        if (Lock(iotHubClientInstance->LockHandle) != LOCK_OK)
        {
            /* Codes_SRS_IOTHUBCLIENT_25_080: [ If acquiring the lock fails, `IoTHubClient_SetRetryPolicy` shall return `IOTHUB_CLIENT_ERROR`. ]*/
            result = IOTHUB_CLIENT_ERROR;
            LogError("Could not acquire lock");
        }
        else
        {
            /* Codes_SRS_IOTHUBCLIENT_25_073: [ `IoTHubClient_SetRetryPolicy` shall start the worker thread if it was not previously started. ] */
            if ((result = StartWorkerThreadIfNeeded(iotHubClientInstance)) != IOTHUB_CLIENT_OK)
            {
                /* Codes_SRS_IOTHUBCLIENT_25_075: [ If starting the thread fails, `IoTHubClient_SetRetryPolicy` shall return `IOTHUB_CLIENT_ERROR`. ]*/
                result = IOTHUB_CLIENT_ERROR;
                LogError("Could not start worker thread");
            }
            else
            {
                /* Codes_SRS_IOTHUBCLIENT_25_077: [ `IoTHubClient_SetRetryPolicy` shall call `IoTHubClient_LL_SetRetryPolicy`, while passing the `IoTHubClient_LL` handle created by `IoTHubClient_Create` and the parameters `retryPolicy` and `retryTimeoutLimitinSeconds`.]*/
                result = IoTHubClient_LL_SetRetryPolicy(iotHubClientInstance->IoTHubClientLLHandle, retryPolicy, retryTimeoutLimitInSeconds);
            }

            (void)Unlock(iotHubClientInstance->LockHandle);
        }
    }

    return result;
}

IOTHUB_CLIENT_RESULT IoTHubClient_GetRetryPolicy(IOTHUB_CLIENT_HANDLE iotHubClientHandle, IOTHUB_CLIENT_RETRY_POLICY * retryPolicy, size_t * retryTimeoutLimitInSeconds)
{
    IOTHUB_CLIENT_RESULT result;

    if (iotHubClientHandle == NULL)
    {
        /* Codes_SRS_IOTHUBCLIENT_25_092: [ If `iotHubClientHandle` is `NULL`, `IoTHubClient_GetRetryPolicy` shall return `IOTHUB_CLIENT_INVALID_ARG`. ]*/
        result = IOTHUB_CLIENT_INVALID_ARG;
        LogError("NULL iothubClientHandle");
    }
    else
    {
        IOTHUB_CLIENT_INSTANCE* iotHubClientInstance = (IOTHUB_CLIENT_INSTANCE*)iotHubClientHandle;

        /* Codes_SRS_IOTHUBCLIENT_25_095: [ `IoTHubClient_GetRetryPolicy` shall be made thread-safe by using the lock created in `IoTHubClient_Create`. ]*/
        if (Lock(iotHubClientInstance->LockHandle) != LOCK_OK)
        {
            /* Codes_SRS_IOTHUBCLIENT_25_096: [ If acquiring the lock fails, `IoTHubClient_GetRetryPolicy` shall return `IOTHUB_CLIENT_ERROR`. ]*/
            result = IOTHUB_CLIENT_ERROR;
            LogError("Could not acquire lock");
        }
        else
        {
            /* Codes_SRS_IOTHUBCLIENT_25_089: [ `IoTHubClient_GetRetryPolicy` shall start the worker thread if it was not previously started.]*/
            if ((result = StartWorkerThreadIfNeeded(iotHubClientInstance)) != IOTHUB_CLIENT_OK)
            {
                /* Codes_SRS_IOTHUBCLIENT_25_091: [ If starting the thread fails, `IoTHubClient_GetRetryPolicy` shall return `IOTHUB_CLIENT_ERROR`.]*/
                result = IOTHUB_CLIENT_ERROR;
                LogError("Could not start worker thread");
            }
            else
            {
                /* Codes_SRS_IOTHUBCLIENT_25_093: [ `IoTHubClient_GetRetryPolicy` shall call `IoTHubClient_LL_GetRetryPolicy`, while passing the `IoTHubClient_LL` handle created by `IoTHubClient_Create` and the parameters `connectionStatusCallback` and `userContextCallback`.]*/
                result = IoTHubClient_LL_GetRetryPolicy(iotHubClientInstance->IoTHubClientLLHandle, retryPolicy, retryTimeoutLimitInSeconds);
            }

            (void)Unlock(iotHubClientInstance->LockHandle);
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
        LogError("NULL iothubClientHandle");
    }
    else
    {
        IOTHUB_CLIENT_INSTANCE* iotHubClientInstance = (IOTHUB_CLIENT_INSTANCE*)iotHubClientHandle;

        /* Codes_SRS_IOTHUBCLIENT_01_035: [IoTHubClient_GetLastMessageReceiveTime shall be made thread-safe by using the lock created in IoTHubClient_Create.] */
        if (Lock(iotHubClientInstance->LockHandle) != LOCK_OK)
        {
            /* Codes_SRS_IOTHUBCLIENT_01_036: [If acquiring the lock fails, IoTHubClient_GetLastMessageReceiveTime shall return IOTHUB_CLIENT_ERROR.] */
            result = IOTHUB_CLIENT_ERROR;
            LogError("Could not acquire lock");
        }
        else
        {
            /* Codes_SRS_IOTHUBCLIENT_01_019: [IoTHubClient_GetLastMessageReceiveTime shall call IoTHubClient_LL_GetLastMessageReceiveTime, while passing the IoTHubClient_LL handle created by IoTHubClient_Create and the parameter lastMessageReceiveTime.] */
            /* Codes_SRS_IOTHUBCLIENT_01_021: [Otherwise, IoTHubClient_GetLastMessageReceiveTime shall return the result of IoTHubClient_LL_GetLastMessageReceiveTime.] */
            result = IoTHubClient_LL_GetLastMessageReceiveTime(iotHubClientInstance->IoTHubClientLLHandle, lastMessageReceiveTime);

            /* Codes_SRS_IOTHUBCLIENT_01_035: [IoTHubClient_GetLastMessageReceiveTime shall be made thread-safe by using the lock created in IoTHubClient_Create.] */
            (void)Unlock(iotHubClientInstance->LockHandle);
        }
    }

    return result;
}

IOTHUB_CLIENT_RESULT IoTHubClient_SetOption(IOTHUB_CLIENT_HANDLE iotHubClientHandle, const char* optionName, const void* value)
{
    IOTHUB_CLIENT_RESULT result;
    /*Codes_SRS_IOTHUBCLIENT_02_034: [If parameter iotHubClientHandle is NULL then IoTHubClient_SetOption shall return IOTHUB_CLIENT_INVALID_ARG.] */
    /*Codes_SRS_IOTHUBCLIENT_02_035: [ If parameter optionName is NULL then IoTHubClient_SetOption shall return IOTHUB_CLIENT_INVALID_ARG. ]*/
    /*Codes_SRS_IOTHUBCLIENT_02_036: [ If parameter value is NULL then IoTHubClient_SetOption shall return IOTHUB_CLIENT_INVALID_ARG. ]*/
    if (
        (iotHubClientHandle == NULL) ||
        (optionName == NULL) ||
        (value == NULL)
        )
    {
        result = IOTHUB_CLIENT_INVALID_ARG;
        LogError("invalid arg (NULL)");
    }
    else
    {
        IOTHUB_CLIENT_INSTANCE* iotHubClientInstance = (IOTHUB_CLIENT_INSTANCE*)iotHubClientHandle;

        /* Codes_SRS_IOTHUBCLIENT_01_041: [ IoTHubClient_SetOption shall be made thread-safe by using the lock created in IoTHubClient_Create. ]*/
        if (Lock(iotHubClientInstance->LockHandle) != LOCK_OK)
        {
            /* Codes_SRS_IOTHUBCLIENT_01_042: [ If acquiring the lock fails, IoTHubClient_GetLastMessageReceiveTime shall return IOTHUB_CLIENT_ERROR. ]*/
            result = IOTHUB_CLIENT_ERROR;
            LogError("Could not acquire lock");
        }
        else
        {
            /*Codes_SRS_IOTHUBCLIENT_02_038: [If optionName doesn't match one of the options handled by this module then IoTHubClient_SetOption shall call IoTHubClient_LL_SetOption passing the same parameters and return what IoTHubClient_LL_SetOption returns.] */
            result = IoTHubClient_LL_SetOption(iotHubClientInstance->IoTHubClientLLHandle, optionName, value);
            if (result != IOTHUB_CLIENT_OK)
            {
                LogError("IoTHubClient_LL_SetOption failed");
            }

            (void)Unlock(iotHubClientInstance->LockHandle);
        }
    }
    return result;
}

IOTHUB_CLIENT_RESULT IoTHubClient_SetDeviceTwinCallback(IOTHUB_CLIENT_HANDLE iotHubClientHandle, IOTHUB_CLIENT_DEVICE_TWIN_CALLBACK deviceTwinCallback, void* userContextCallback)
{
    IOTHUB_CLIENT_RESULT result;

    /*Codes_SRS_IOTHUBCLIENT_10_001: [** `IoTHubClient_SetDeviceTwinCallback` shall fail and return `IOTHUB_CLIENT_INVALID_ARG` if parameter `iotHubClientHandle` is `NULL`. ]*/
    if (iotHubClientHandle == NULL)
    {
        result = IOTHUB_CLIENT_INVALID_ARG;
        LogError("invalid arg (NULL)");
    }
    else
    {
        IOTHUB_CLIENT_INSTANCE* iotHubClientInstance = (IOTHUB_CLIENT_INSTANCE*)iotHubClientHandle;

        /*Codes_SRS_IOTHUBCLIENT_10_020: [** `IoTHubClient_SetDeviceTwinCallback` shall be made thread - safe by using the lock created in IoTHubClient_Create. ]*/
        if (Lock(iotHubClientInstance->LockHandle) != LOCK_OK)
        {
            /*Codes_SRS_IOTHUBCLIENT_10_002: [** If acquiring the lock fails, `IoTHubClient_SetDeviceTwinCallback` shall return `IOTHUB_CLIENT_ERROR`. ]*/
            result = IOTHUB_CLIENT_ERROR;
            LogError("Could not acquire lock");
        }
        else
        {
            /*Codes_SRS_IOTHUBCLIENT_10_003: [** If the transport connection is shared, the thread shall be started by calling `IoTHubTransport_StartWorkerThread`. ]*/
            if ((result = StartWorkerThreadIfNeeded(iotHubClientInstance)) != IOTHUB_CLIENT_OK)
            {
                /*Codes_SRS_IOTHUBCLIENT_10_004: [** If starting the thread fails, `IoTHubClient_SetDeviceTwinCallback` shall return `IOTHUB_CLIENT_ERROR`. ]*/
                result = IOTHUB_CLIENT_ERROR;
                LogError("Could not start worker thread");
            }
            else
            {
                /*Codes_SRS_IOTHUBCLIENT_10_005: [** `IoTHubClient_LL_SetDeviceTwinCallback` shall call `IoTHubClient_LL_SetDeviceTwinCallback`, while passing the `IoTHubClient_LL handle` created by `IoTHubClient_LL_Create` along with the parameters `reportedStateCallback` and `userContextCallback`. ]*/
                result = IoTHubClient_LL_SetDeviceTwinCallback(iotHubClientInstance->IoTHubClientLLHandle, deviceTwinCallback, userContextCallback);
                if (result != IOTHUB_CLIENT_OK)
                {
                    LogError("IoTHubClient_LL_SetDeviceTwinCallback failed");
                }
            }

            (void)Unlock(iotHubClientInstance->LockHandle);
        }
    }
    return result;
}

IOTHUB_CLIENT_RESULT IoTHubClient_SendReportedState(IOTHUB_CLIENT_HANDLE iotHubClientHandle, const unsigned char* reportedState, size_t size, IOTHUB_CLIENT_REPORTED_STATE_CALLBACK reportedStateCallback, void* userContextCallback)
{
    IOTHUB_CLIENT_RESULT result;

    /*Codes_SRS_IOTHUBCLIENT_10_013: [** If `iotHubClientHandle` is `NULL`, `IoTHubClient_SendReportedState` shall return `IOTHUB_CLIENT_INVALID_ARG`. ]*/
    if (iotHubClientHandle == NULL)
    {
        result = IOTHUB_CLIENT_INVALID_ARG;
        LogError("invalid arg (NULL)");
    }
    else
    {
        IOTHUB_CLIENT_INSTANCE* iotHubClientInstance = (IOTHUB_CLIENT_INSTANCE*)iotHubClientHandle;

        /*Codes_SRS_IOTHUBCLIENT_10_021: [** `IoTHubClient_SendReportedState` shall be made thread-safe by using the lock created in IoTHubClient_Create. ]*/
        if (Lock(iotHubClientInstance->LockHandle) != LOCK_OK)
        {
            /*Codes_SRS_IOTHUBCLIENT_10_014: [** If acquiring the lock fails, `IoTHubClient_SendReportedState` shall return `IOTHUB_CLIENT_ERROR`. ]*/
            result = IOTHUB_CLIENT_ERROR;
            LogError("Could not acquire lock");
        }
        else
        {
            /*Codes_SRS_IOTHUBCLIENT_10_015: [** If the transport connection is shared, the thread shall be started by calling `IoTHubTransport_StartWorkerThread`. ]*/
            if ((result = StartWorkerThreadIfNeeded(iotHubClientInstance)) != IOTHUB_CLIENT_OK)
            {
                /*Codes_SRS_IOTHUBCLIENT_10_016: [** If starting the thread fails, `IoTHubClient_SendReportedState` shall return `IOTHUB_CLIENT_ERROR`. ]*/
                result = IOTHUB_CLIENT_ERROR;
                LogError("Could not start worker thread");
            }
            else
            {
                /*Codes_SRS_IOTHUBCLIENT_10_017: [** `IoTHubClient_SendReportedState` shall call `IoTHubClient_LL_SendReportedState`, while passing the `IoTHubClient_LL handle` created by `IoTHubClient_LL_Create` along with the parameters `reportedState`, `size`, `reportedStateCallback`, and `userContextCallback`. ]*/
                /*Codes_SRS_IOTHUBCLIENT_10_018: [** When `IoTHubClient_LL_SendReportedState` is called, `IoTHubClient_SendReportedState` shall return the result of `IoTHubClient_LL_SendReportedState`. **]*/
                result = IoTHubClient_LL_SendReportedState(iotHubClientInstance->IoTHubClientLLHandle, reportedState, size, reportedStateCallback, userContextCallback);
                if (result != IOTHUB_CLIENT_OK)
                {
                    LogError("IoTHubClient_LL_SendReportedState failed");
                }
            }

            (void)Unlock(iotHubClientInstance->LockHandle);
        }
    }
    return result;
}

IOTHUB_CLIENT_RESULT IoTHubClient_SetDeviceMethodCallback(IOTHUB_CLIENT_HANDLE iotHubClientHandle, IOTHUB_CLIENT_DEVICE_METHOD_CALLBACK_ASYNC deviceMethodCallback, void* userContextCallback)
{
    IOTHUB_CLIENT_RESULT result;

    /*Codes_SRS_IOTHUBCLIENT_12_012: [ If iotHubClientHandle is NULL, IoTHubClient_SetDeviceMethodCallback shall return IOTHUB_CLIENT_INVALID_ARG. ]*/ 
    if (iotHubClientHandle == NULL)
    {
        result = IOTHUB_CLIENT_INVALID_ARG;
        LogError("invalid arg (NULL)");
    }
    else
    {
        IOTHUB_CLIENT_INSTANCE* iotHubClientInstance = (IOTHUB_CLIENT_INSTANCE*)iotHubClientHandle;

        /*Codes_SRS_IOTHUBCLIENT_12_018: [ IoTHubClient_SetDeviceMethodCallback shall be made thread-safe by using the lock created in IoTHubClient_Create. ]*/
        if (Lock(iotHubClientInstance->LockHandle) != LOCK_OK)
        {
            /*Codes_SRS_IOTHUBCLIENT_12_013: [ If acquiring the lock fails, IoTHubClient_SetDeviceMethodCallback shall return IOTHUB_CLIENT_ERROR. ]*/
            result = IOTHUB_CLIENT_ERROR;
            LogError("Could not acquire lock");
        }
        else
        {
            /*Codes_SRS_IOTHUBCLIENT_12_014: [ If the transport handle is null and the worker thread is not initialized, the thread shall be started by calling IoTHubTransport_StartWorkerThread. ]*/
            if ((result = StartWorkerThreadIfNeeded(iotHubClientInstance)) != IOTHUB_CLIENT_OK)
            {
                /*Codes_SRS_IOTHUBCLIENT_12_015: [ If starting the thread fails, IoTHubClient_SetDeviceMethodCallback shall return IOTHUB_CLIENT_ERROR. ]*/
                result = IOTHUB_CLIENT_ERROR;
                LogError("Could not start worker thread");
            }
            else
            {
                /*Codes_SRS_IOTHUBCLIENT_12_016: [ IoTHubClient_SetDeviceMethodCallback shall call IoTHubClient_LL_SetDeviceMethodCallback, while passing the IoTHubClient_LL_handle created by IoTHubClient_LL_Create along with the parameters deviceMethodCallback and userContextCallback. ]*/
                /*Codes_SRS_IOTHUBCLIENT_12_017: [ When IoTHubClient_LL_SetDeviceMethodCallback is called, IoTHubClient_SetDeviceMethodCallback shall return the result of IoTHubClient_LL_SetDeviceMethodCallback. ]*/
                result = IoTHubClient_LL_SetDeviceMethodCallback(iotHubClientInstance->IoTHubClientLLHandle, deviceMethodCallback, userContextCallback);
                if (result != IOTHUB_CLIENT_OK)
                {
                    LogError("IoTHubClient_LL_SetDeviceMethodCallback failed");
                }
            }

            (void)Unlock(iotHubClientInstance->LockHandle);
        }
    }
    return result;
}


#ifndef DONT_USE_UPLOADTOBLOB
static int uploadingThread(void *data)
{
    UPLOADTOBLOB_SAVED_DATA* savedData = (UPLOADTOBLOB_SAVED_DATA*)data;

    /*it so happens that IoTHubClient_LL_UploadToBlob is thread-safe because there's no saved state in the handle and there are no globals, so no need to protect it*/
    /*not having it protected means multiple simultaneous uploads can happen*/
    /*Codes_SRS_IOTHUBCLIENT_02_054: [ The thread shall call IoTHubClient_LL_UploadToBlob passing the information packed in the structure. ]*/
    if (IoTHubClient_LL_UploadToBlob(savedData->iotHubClientHandle->IoTHubClientLLHandle, savedData->destinationFileName, savedData->source, savedData->size) != IOTHUB_CLIENT_OK)
    {
        LogError("unable to IoTHubClient_LL_UploadToBlob");
        /*call the callback*/
        if (savedData->iotHubClientFileUploadCallback != NULL)
        {
            /*Codes_SRS_IOTHUBCLIENT_02_055: [ If IoTHubClient_LL_UploadToBlob fails then the thread shall call iotHubClientFileUploadCallbackInternal passing as result FILE_UPLOAD_ERROR and as context the structure from SRS IOTHUBCLIENT 02 051. ]*/
            savedData->iotHubClientFileUploadCallback(FILE_UPLOAD_ERROR, savedData->context);
        }
    }
    else
    {
        if (savedData->iotHubClientFileUploadCallback != NULL)
        {
            /*Codes_SRS_IOTHUBCLIENT_02_056: [ Otherwise the thread iotHubClientFileUploadCallbackInternal passing as result FILE_UPLOAD_OK and the structure from SRS IOTHUBCLIENT 02 051. ]*/
            savedData->iotHubClientFileUploadCallback(FILE_UPLOAD_OK, savedData->context);
        }
    }

    /*Codes_SRS_IOTHUBCLIENT_02_071: [ The thread shall mark itself as disposable. ]*/
    if (Lock(savedData->lockGarbage) != LOCK_OK)
    {
        LogError("unable to Lock - trying anyway");
        savedData->canBeGarbageCollected = 1;
    }
    else
    {
        savedData->canBeGarbageCollected = 1;

        if (Unlock(savedData->lockGarbage) != LOCK_OK)
        {
            LogError("unable to Unlock after locking");
        }
    }
    return 0;
}
#endif

#ifndef DONT_USE_UPLOADTOBLOB
IOTHUB_CLIENT_RESULT IoTHubClient_UploadToBlobAsync(IOTHUB_CLIENT_HANDLE iotHubClientHandle, const char* destinationFileName, const unsigned char* source, size_t size, IOTHUB_CLIENT_FILE_UPLOAD_CALLBACK iotHubClientFileUploadCallback, void* context)
{
    IOTHUB_CLIENT_RESULT result;
    /*Codes_SRS_IOTHUBCLIENT_02_047: [ If iotHubClientHandle is NULL then IoTHubClient_UploadToBlobAsync shall fail and return IOTHUB_CLIENT_INVALID_ARG. ]*/
    /*Codes_SRS_IOTHUBCLIENT_02_048: [ If destinationFileName is NULL then IoTHubClient_UploadToBlobAsync shall fail and return IOTHUB_CLIENT_INVALID_ARG. ]*/
    /*Codes_SRS_IOTHUBCLIENT_02_049: [ If source is NULL and size is greated than 0 then IoTHubClient_UploadToBlobAsync shall fail and return IOTHUB_CLIENT_INVALID_ARG. ]*/
    if (
        (iotHubClientHandle == NULL) ||
        (destinationFileName == NULL) ||
        ((source == NULL) && (size > 0))
        )
    {
        LogError("invalid parameters IOTHUB_CLIENT_HANDLE iotHubClientHandle = %p , const char* destinationFileName = %s, const unsigned char* source= %p, size_t size = %zu, IOTHUB_CLIENT_FILE_UPLOAD_CALLBACK iotHubClientFileUploadCallback = %p, void* context = %p",
            iotHubClientHandle,
            destinationFileName,
            source,
            size,
            iotHubClientFileUploadCallback,
            context
        );
        result = IOTHUB_CLIENT_INVALID_ARG;
    }
    else
    {
        /*Codes_SRS_IOTHUBCLIENT_02_051: [IoTHubClient_UploadToBlobAsync shall copy the souce, size, iotHubClientFileUploadCallback, context into a structure.]*/
        UPLOADTOBLOB_SAVED_DATA *savedData = (UPLOADTOBLOB_SAVED_DATA *)malloc(sizeof(UPLOADTOBLOB_SAVED_DATA));
        if (savedData == NULL)
        {
            /*Codes_SRS_IOTHUBCLIENT_02_053: [ If copying to the structure or spawning the thread fails, then IoTHubClient_UploadToBlobAsync shall fail and return IOTHUB_CLIENT_ERROR. ]*/
            LogError("unable to malloc - oom");
            result = IOTHUB_CLIENT_ERROR;
        }
        else
        {
            if (mallocAndStrcpy_s((char**)&savedData->destinationFileName, destinationFileName) != 0)
            {
                /*Codes_SRS_IOTHUBCLIENT_02_053: [ If copying to the structure or spawning the thread fails, then IoTHubClient_UploadToBlobAsync shall fail and return IOTHUB_CLIENT_ERROR. ]*/
                LogError("unable to mallocAndStrcpy_s");
                free(savedData);
                result = IOTHUB_CLIENT_ERROR;
            }
            else
            {
                savedData->size = size;
                int sourceCloned;
                if (size == 0)
                {
                    savedData->source = NULL;
                    sourceCloned = 1;
                }
                else
                {
                    savedData->source = (unsigned char*)malloc(size);
                    if (savedData->source == NULL)
                    {
                        /*Codes_SRS_IOTHUBCLIENT_02_053: [ If copying to the structure or spawning the thread fails, then IoTHubClient_UploadToBlobAsync shall fail and return IOTHUB_CLIENT_ERROR. ]*/
                        LogError("unable to malloc - oom");
                        free(savedData->destinationFileName);
                        free(savedData);
                        sourceCloned = 0;

                    }
                    else
                    {
                        sourceCloned = 1;
                    }
                }

                if (sourceCloned == 0)
                {
                    result = IOTHUB_CLIENT_ERROR;
                }
                else
                {
                    savedData->iotHubClientFileUploadCallback = iotHubClientFileUploadCallback;
                    savedData->context = context;
                    memcpy(savedData->source, source, size);
                    IOTHUB_CLIENT_INSTANCE* iotHubClientHandleData = (IOTHUB_CLIENT_INSTANCE*)iotHubClientHandle;
                    if (Lock(iotHubClientHandleData->LockHandle) != LOCK_OK) /*locking because the next statement is changing blobThreadsToBeJoined*/
                    {
                        LogError("unable to lock");
                        free(savedData->source);
                        free(savedData->destinationFileName);
                        free(savedData);
                        result = IOTHUB_CLIENT_ERROR;
                    }
                    else
                    {
                        if ((result = StartWorkerThreadIfNeeded(iotHubClientHandleData)) != IOTHUB_CLIENT_OK)
                        {
                            free(savedData->source);
                            free(savedData->destinationFileName);
                            free(savedData);
                            result = IOTHUB_CLIENT_ERROR;
                            LogError("Could not start worker thread");
                        }
                        else
                        {
                            /*Codes_SRS_IOTHUBCLIENT_02_058: [ IoTHubClient_UploadToBlobAsync shall add the structure to the list of structures that need to be cleaned once file upload finishes. ]*/
                            LIST_ITEM_HANDLE item = singlylinkedlist_add(iotHubClientHandleData->savedDataToBeCleaned, savedData);
                            if (item == NULL)
                            {
                                LogError("unable to singlylinkedlist_add");
                                free(savedData->source);
                                free(savedData->destinationFileName);
                                free(savedData);
                                result = IOTHUB_CLIENT_ERROR;
                            }
                            else
                            {
                                savedData->iotHubClientHandle = iotHubClientHandle;
                                savedData->canBeGarbageCollected = 0;
                                if ((savedData->lockGarbage = Lock_Init()) == NULL)
                                {
                                    (void)singlylinkedlist_remove(iotHubClientHandleData->savedDataToBeCleaned, item);
                                    free(savedData->source);
                                    free(savedData->destinationFileName);
                                    free(savedData);
                                    result = IOTHUB_CLIENT_ERROR;
                                    LogError("unable to Lock_Init");
                                }
                                else
                                {
                                    /*Codes_SRS_IOTHUBCLIENT_02_052: [ IoTHubClient_UploadToBlobAsync shall spawn a thread passing the structure build in SRS IOTHUBCLIENT 02 051 as thread data.]*/
                                    if (ThreadAPI_Create(&savedData->uploadingThreadHandle, uploadingThread, savedData) != THREADAPI_OK)
                                    {
                                        /*Codes_SRS_IOTHUBCLIENT_02_053: [ If copying to the structure or spawning the thread fails, then IoTHubClient_UploadToBlobAsync shall fail and return IOTHUB_CLIENT_ERROR. ]*/
                                        LogError("unablet to ThreadAPI_Create");
                                        (void)Lock_Deinit(savedData->lockGarbage);
                                        (void)singlylinkedlist_remove(iotHubClientHandleData->savedDataToBeCleaned, item);
                                        free(savedData->source);
                                        free(savedData->destinationFileName);
                                        free(savedData);
                                        result = IOTHUB_CLIENT_ERROR;
                                    }
                                    else
                                    {

                                        result = IOTHUB_CLIENT_OK;
                                    }
                                }
                            }
                        }
                        (void)Unlock(iotHubClientHandleData->LockHandle);
                    }
                }
            }
        }
    }
    return result;
}
#endif /*DONT_USE_UPLOADTOBLOB*/
