﻿#IoTHubClient Requirements
 

 
##Overview

IoTHubClient is a module that extends the IoTHubCLient_LL module with 2 features:
-	scheduling the work for the IoTHubCLient from a thread, so that the user does not need to create its own thread.
-	Thread-safe APIs
Undelaying layer in the following requirements refers to IoTHubClient_LL.
 
##Exposed API

```c
typedef void* IOTHUB_CLIENT_HANDLE;
extern const char* IoTHubClient_GetVersionString(void);
extern IOTHUB_CLIENT_HANDLE IoTHubClient_CreateFromConnectionString(const char* connectionString, IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol);
extern IOTHUB_CLIENT_HANDLE IoTHubClient_Create(const IOTHUB_CLIENT_CONFIG* config);
extern IOTHUB_CLIENT_HANDLE IoTHubClient_CreateWithTransport(TRANSPORT_HANDLE transportHandle, const IOTHUB_CLIENT_CONFIG* config);
extern void IoTHubClient_Destroy(IOTHUB_CLIENT_HANDLE iotHubClientHandle);

extern IOTHUB_CLIENT_RESULT IoTHubClient_SendEventAsync(IOTHUB_CLIENT_HANDLE iotHubClientHandle, IOTHUB_MESSAGE_HANDLE eventMessageHandle, IOTHUB_CLIENT_EVENT_CONFIRMATION_CALLBACK eventConfirmationCallback, void* userContextCallback);
    extern IOTHUB_CLIENT_RESULT IoTHubClient_SetMessageCallback(IOTHUB_CLIENT_HANDLE iotHubClientHandle, IOTHUB_CLIENT_MESSAGE_CALLBACK_ASYNC messageCallback, void* userContextCallback);

    extern IOTHUB_CLIENT_RESULT IoTHubClient_GetLastMessageReceiveTime(IOTHUB_CLIENT_HANDLE iotHubClientHandle, time_t* lastMessageReceiveTime);
extern IOTHUB_CLIENT_RESULT IoTHubClient_SetOption(IOTHUB_CLIENT_HANDLE iotHubClientHandle, const char* optionName, const void* value);
```

## IoTHubClient_GetVersionString
```c
extern const char* IoTHubClient_GetVersionString(void);
```

**SRS_IOTHUBCLIENT_05_001: [** IoTHubClient_GetVersionString shall return a pointer to a constant string which indicates the version of IoTHubClient API. **]**



## IoTHubClient_CreateFromConnectionString
```c
extern IOTHUB_CLIENT_HANDLE IoTHubClient_CreateFromConnectionString(const char* connectionString, IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol);
``` 
 
**SRS_IOTHUBCLIENT_12_003: [** IoTHubClient_CreateFromConnectionString shall verify the input parameters and if any of them NULL then return NULL  **]**

**SRS_IOTHUBCLIENT_12_004: [** IoTHubClient_CreateFromConnectionString shall allocate a new IoTHubClient instance.  **]**

**SRS_IOTHUBCLIENT_12_011: [** If the allocation failed, IoTHubClient_CreateFromConnectionString returns NULL  **]**

**SRS_IOTHUBCLIENT_02_039: [** IoTHubClient_CreateFromConnectionString shall create a condition variable object to be used later for stopping the worker thread. **]**

**SRS_IOTHUBCLIENT_02_040: [** If condition variable creation fails, then IoTHubClient_CreateFromConnectionString shall fail and return NULL. **]**

**SRS_IOTHUBCLIENT_12_005: [** IoTHubClient_CreateFromConnectionString shall create a lock object to be used later for serializing IoTHubClient calls **]**

**SRS_IOTHUBCLIENT_12_009: [** If lock creation failed, IoTHubClient_CreateFromConnectionString shall do clean up and return NULL **]**

**SRS_IOTHUBCLIENT_12_006: [** IoTHubClient_CreateFromConnectionString shall instantiate a new IoTHubClient_LL instance by calling IoTHubClient_LL_CreateFromConnectionString and passing the connectionString **]**

**SRS_IOTHUBCLIENT_12_010: [** If IoTHubClient_LL_CreateFromConnectionString fails then IoTHubClient_CreateFromConnectionString shall do clean-up and return NULL **]**


 
## IoTHubClient_Create
```c 
extern IOTHUB_CLIENT_HANDLE IoTHubClient_Create(const IOTHUB_CLIENT_CONFIG* config);
```

**SRS_IOTHUBCLIENT_01_001: [** IoTHubClient_Create shall allocate a new IoTHubClient instance and return a non-NULL handle to it. **]**

**SRS_IOTHUBCLIENT_01_002: [** IoTHubClient_Create shall instantiate a new IoTHubClient_LL instance by calling IoTHubClient_LL_Create and passing the config argument. **]**

**SRS_IOTHUBCLIENT_01_003: [** If IoTHubClient_LL_Create fails, then IoTHubClient_Create shall return NULL. **]**

**SRS_IOTHUBCLIENT_01_004: [** If allocating memory for the new IoTHubClient instance fails, then IoTHubClient_Create shall return NULL. **]**

**SRS_IOTHUBCLIENT_02_041: [** IoTHubClient_Create shall create a condition variable object to be used later for stopping the worker thread. **]**

**SRS_IOTHUBCLIENT_02_042: [** If condition variable creation fails, then IoTHubClient_Create shall fail and return NULL. **]**

**SRS_IOTHUBCLIENT_01_029: [** IoTHubClient_Create shall create a lock object to be used later for serializing IoTHubClient calls. **]**

**SRS_IOTHUBCLIENT_01_030: [** If creating the lock fails, then IoTHubClient_Create shall return NULL. **]**

**SRS_IOTHUBCLIENT_01_031: [** If IoTHubClient_Create fails, all resources allocated by it shall be freed. **]**

## IoTHubClient_CreateWithTransport
```c
extern IOTHUB_CLIENT_HANDLE IoTHubClient_CreateWithTransport(TRANSPORT_HANDLE transportHandle, const IOTHUB_CLIENT_CONFIG* config);
```

Create an IoTHubClient using an existing connection.

**SRS_IOTHUBCLIENT_17_013: [** IoTHubClient_CreateWithTransport shall return NULL if transportHandle is NULL. **]**

**SRS_IOTHUBCLIENT_17_014: [** IoTHubClient_CreateWithTransport shall return NULL if config is NULL. **]**

**SRS_IOTHUBCLIENT_17_001: [** IoTHubClient_CreateWithTransport shall allocate a new IoTHubClient instance and return a non-NULL handle to it.
 **]**
 
 **SRS_IOTHUBCLIENT_17_002: [** If allocating memory for the new IoTHubClient instance fails, then IoTHubClient_CreateWithTransport shall return NULL. **]**
 
**SRS_IOTHUBCLIENT_17_003: [** IoTHubClient_CreateWithTransport shall call IoTHubTransport_GetLLTransport on transportHandle to get lower layer transport. **]**

**SRS_IOTHUBCLIENT_17_004: [** If IoTHubTransport_GetLLTransport fails, then IoTHubClient_CreateWithTransport shall return NULL. **]**

**SRS_IOTHUBCLIENT_17_005: [** IoTHubClient_CreateWithTransport shall call IoTHubTransport_GetLock to get the transport lock to be used later for serializing IoTHubClient calls. **]**

**SRS_IOTHUBCLIENT_17_006: [** If IoTHubTransport_GetLock fails, then IoTHubClient_CreateWithTransport shall return NULL. **]**

**SRS_IOTHUBCLIENT_17_007: [** IoTHubClient_CreateWithTransport shall instantiate a new IoTHubClient_LL instance by calling IoTHubClient_LL_CreateWithTransport and passing the lower layer transport and config argument. **]**

**SRS_IOTHUBCLIENT_17_008: [** If IoTHubClient_LL_CreateWithTransport fails, then IoTHubClient_Create shall return NULL. **]**

**SRS_IOTHUBCLIENT_17_009: [** If IoTHubClient_LL_CreateWithTransport fails, all resources allocated by it shall be freed. **]**



## IoTHubClient_Destroy
```c
extern void IoTHubClient_Destroy(IOTHUB_CLIENT_HANDLE iotHubClientHandle);
```
**SRS_IOTHUBCLIENT_01_005: [** IoTHubClient_Destroy shall free all resources associated with the iotHubClientHandle instance. **]**

**SRS_IOTHUBCLIENT_01_006: [** That includes destroying the IoTHubClient_LL instance by calling IoTHubClient_LL_Destroy. **]**

**SRS_IOTHUBCLIENT_02_043: [** IoTHubClient_Destroy shall lock the serializing lock and signal the worker thread (if any) to end **]**

**SRS_IOTHUBCLIENT_02_045: [** IoTHubClient_Destroy shall unlock the serializing lock. **]**

**SRS_IOTHUBCLIENT_01_007: [** The thread created as part of executing IoTHubClient_SendEventAsync or IoTHubClient_SetNotificationMessageCallback shall be joined. **]**

**SRS_IOTHUBCLIENT_02_046: [** the condition variable shall be detroyed. **]**

**SRS_IOTHUBCLIENT_01_032: [** If the lock was allocated in IoTHubClient_Create, it shall be also freed. **]**

**SRS_IOTHUBCLIENT_01_008: [** IoTHubClient_Destroy shall do nothing if parameter iotHubClientHandle is NULL. **]**


## IoTHubClient_SendEventAsync 
```c 
extern IOTHUB_CLIENT_RESULT IoTHubClient_SendEventAsync(IOTHUB_CLIENT_HANDLE iotHubClientHandle, IOTHUB_MESSAGE_HANDLE eventMessageHandle, IOTHUB_CLIENT_EVENT_CONFIRMATION_CALLBACK eventConfirmationCallback, void* userContextCallback);
```

**SRS_IOTHUBCLIENT_01_009: [** IoTHubClient_SendEventAsync shall start the worker thread if it was not previously started. **]**

**SRS_IOTHUBCLIENT_17_012: [** If the transport connection is shared, the thread shall be started by calling IoTHubTransport_StartWorkerThread. **]**

**SRS_IOTHUBCLIENT_01_010: [** If starting the thread fails, IoTHubClient_SendEventAsync shall return IOTHUB_CLIENT_ERROR. **]**

**SRS_IOTHUBCLIENT_01_011: [** If iotHubClientHandle is NULL, IoTHubClient_SendEventAsync shall return IOTHUB_CLIENT_INVALID_ARG. **]**



**SRS_IOTHUBCLIENT_01_012: [** IoTHubClient_SendEventAsync shall call IoTHubClient_LL_SendEventAsync, while passing the IoTHubClient_LL handle created by IoTHubClient_Create and the parameters eventMessageHandle, eventConfirmationCallback and userContextCallback. **]**


**SRS_IOTHUBCLIENT_01_013: [** When IoTHubClient_LL_SendEventAsync is called, IoTHubClient_SendEventAsync shall return the result of IoTHubClient_LL_SendEventAsync. **]**

**SRS_IOTHUBCLIENT_01_025: [** IoTHubClient_SendEventAsync shall be made thread-safe by using the lock created in IoTHubClient_Create. **]**

**SRS_IOTHUBCLIENT_01_026: [** If acquiring the lock fails, IoTHubClient_SendEventAsync shall return IOTHUB_CLIENT_ERROR. **]**


## IoTHubClient_SetMessageCallback
```c
extern IOTHUB_CLIENT_RESULT IoTHubClient_SetMessageCallback(IOTHUB_CLIENT_HANDLE iotHubClientHandle, IOTHUB_CLIENT_MESSAGE_CALLBACK_ASYNC messageCallback, void* userContextCallback);
```

**SRS_IOTHUBCLIENT_01_014: [** IoTHubClient_SetMessageCallback shall start the worker thread if it was not previously started. **]**

**SRS_IOTHUBCLIENT_17_011: [** If the transport connection is shared, the thread shall be started by calling IoTHubTransport_StartWorkerThread. **]**

**SRS_IOTHUBCLIENT_01_015: [** If starting the thread fails, IoTHubClient_SetMessageCallback shall return IOTHUB_CLIENT_ERROR. **]**

**SRS_IOTHUBCLIENT_01_016: [** If iotHubClientHandle is NULL, IoTHubClient_SetMessageCallback shall return IOTHUB_CLIENT_INVALID_ARG. **]**


**SRS_IOTHUBCLIENT_01_017: [** IoTHubClient_SetMessageCallback shall call IoTHubClient_LL_SetMessageCallback, while passing the IoTHubClient_LL handle created by IoTHubClient_Create and the parameters messageCallback and userContextCallback. **]**


**SRS_IOTHUBCLIENT_01_018: [** When IoTHubClient_LL_SetMessageCallback is called, IoTHubClient_SetMessageCallback shall return the result of IoTHubClient_LL_SetMessageCallback. **]**

**SRS_IOTHUBCLIENT_01_027: [** IoTHubClient_SetMessageCallback shall be made thread-safe by using the lock created in IoTHubClient_Create. **]**

**SRS_IOTHUBCLIENT_01_028: [** If acquiring the lock fails, IoTHubClient_SetMessageCallback shall return IOTHUB_CLIENT_ERROR. **]**



## IoTHubClient_GetLastMessageReceiveTime 
```c
extern IOTHUB_CLIENT_RESULT IoTHubClient_GetLastMessageReceiveTime(IOTHUB_CLIENT_HANDLE iotHubClientHandle, time_t* lastMessageReceiveTime);
```

**SRS_IOTHUBCLIENT_01_019: [** IoTHubClient_GetLastMessageReceiveTime shall call IoTHubClient_LL_GetLastMessageReceiveTime, while passing the IoTHubClient_LL handle created by IoTHubClient_Create and the parameter lastMessageReceiveTime. **]**

**SRS_IOTHUBCLIENT_01_020: [** If iotHubClientHandle is NULL, IoTHubClient_GetLastMessageReceiveTime shall return IOTHUB_CLIENT_INVALID_ARG. **]**

**SRS_IOTHUBCLIENT_01_021: [** Otherwise, IoTHubClient_GetLastMessageReceiveTime shall return the result of IoTHubClient_LL_GetLastMessageReceiveTime. **]**

**SRS_IOTHUBCLIENT_01_035: [** IoTHubClient_GetLastMessageReceiveTime shall be made thread-safe by using the lock created in IoTHubClient_Create. **]**

**SRS_IOTHUBCLIENT_01_036: [** If acquiring the lock fails, IoTHubClient_GetLastMessageReceiveTime shall return IOTHUB_CLIENT_ERROR. **]**



## IoTHubClient_GetSendStatus

```c
extern IOTHUB_CLIENT_RESULT IoTHubClient_GetSendStatus(IOTHUB_CLIENT_HANDLE iotHubClientHandle, IOTHUB_CLIENT_STATUS *iotHubClientStatus);
```

**SRS_IOTHUBCLIENT_01_022: [** IoTHubClient_GetSendStatus shall call IoTHubClient_LL_GetSendStatus, while passing the IoTHubClient_LL handle created by IoTHubClient_Create and the parameter iotHubClientStatus. **]**

**SRS_IOTHUBCLIENT_01_023: [** If iotHubClientHandle is NULL, IoTHubClient_ GetSendStatus shall return IOTHUB_CLIENT_INVALID_ARG. **]**

**SRS_IOTHUBCLIENT_01_024: [** Otherwise, IoTHubClient_GetSendStatus shall return the result of IoTHubClient_LL_GetSendStatus. **]**

**SRS_IOTHUBCLIENT_01_033: [** IoTHubClient_GetSendStatus shall be made thread-safe by using the lock created in IoTHubClient_Create. **]**

**SRS_IOTHUBCLIENT_01_034: [** If acquiring the lock fails, IoTHubClient_GetSendStatus shall return IOTHUB_CLIENT_ERROR. **]**


###Scheduling work
**SRS_IOTHUBCLIENT_01_037: [** The thread created by IoTHubClient_SendEvent or IoTHubClient_SetMessageCallback shall call IoTHubClient_LL_DoWork every 1 ms. **]**

**SRS_IOTHUBCLIENT_01_038: [** The thread shall exit when all IoTHubClients using the thread have had IoTHubClient_Destroy called. **]**

**SRS_IOTHUBCLIENT_01_039: [** All calls to IoTHubClient_LL_DoWork shall be protected by the lock created in IotHubClient_Create. **]**

**SRS_IOTHUBCLIENT_01_040: [** If acquiring the lock fails, IoTHubClient_LL_DoWork shall not be called. **]**



## IoTHubClient_SetOption
```c
extern IOTHUB_CLIENT_RESULT IoTHubClient_SetOption(IOTHUB_CLIENT_HANDLE iotHubClientHandle, const char* optionName, const void* value);
```

IoTHubClient_SetOption allows run-time changing of settings of the IoTHubClient. 

**SRS_IOTHUBCLIENT_02_034: [** If parameter iotHubClientHandle is NULL then IoTHubClient_SetOption shall return IOTHUB_CLIENT_INVALID_ARG. **]**

**SRS_IOTHUBCLIENT_02_035: [** If parameter optionName is NULL then IoTHubClient_SetOption shall return IOTHUB_CLIENT_INVALID_ARG. **]**

**SRS_IOTHUBCLIENT_02_036: [** If parameter value is NULL then IoTHubClient_SetOption shall return IOTHUB_CLIENT_INVALID_ARG. **]**

**SRS_IOTHUBCLIENT_02_037: [** If optionName matches one of the option handled by IoTHubClient, then the pointer value shall be dereferenced (by convention) to the data type for that option and option specific code shall be executed. **]**

**SRS_IOTHUBCLIENT_02_038: [** If optionName doesn't match one of the options handled by this module then IoTHubClient_SetOption shall call IoTHubClient_LL_SetOption passing the same parameters and return what IoTHubClient_LL_SetOption returns. **]**

**SRS_IOTHUBCLIENT_01_041: [** IoTHubClient_SetOption shall be made thread-safe by using the lock created in IoTHubClient_Create. **]**
**SRS_IOTHUBCLIENT_01_042: [** If acquiring the lock fails, IoTHubClient_GetLastMessageReceiveTime shall return IOTHUB_CLIENT_ERROR. **]**


Options handled by IoTHubClient_SetOption:
-none.
