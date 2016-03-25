# IoTHubTransport Requirements

## Overview

IoTHubTransport is a module that extends the Lower Layer transport function, allowing the transport to be successfully shared between IoTHubClients.  Features:
  - creates a single thread for all communication on this connection.
  - creates the lock for thread safety between IoTHubClients.
  - creates a Lower Layer Transport suitable for managing multiple IoTHubClients.
  
## Exposed API

```c
typedef TRANSPORT_HANDLE_DATA_TAG* TRANSPORT_HANDLE;

extern TRANSPORT_HANDLE		IoTHubTransport_Create(IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol, const char* iotHubName, const char* iotHubSuffix);
extern void					IoTHubTransport_Destroy(TRANSPORT_HANDLE transportHlHandle);
extern LOCK_HANDLE			IoTHubTransport_GetLock(TRANSPORT_HANDLE transportHlHandle);
extern TRANSPORT_LL_HANDLE	IoTHubTransport_GetLLTransport(TRANSPORT_HANDLE transportHlHandle);
extern IOTHUB_CLIENT_RESULT IoTHubTransport_StartWorkerThread(TRANSPORT_HANDLE transportHlHandle, IOTHUB_CLIENT_HANDLE clientHandle);
extern bool					IoTHubTransport_SignalEndWorkerThread(TRANSPORT_HANDLE transportHlHandle, IOTHUB_CLIENT_HANDLE clientHandle);
extern void					IoTHubTransport_JoinWorkerThread(TRANSPORT_HANDLE transportHlHandle, IOTHUB_CLIENT_HANDLE clientHandle);
```

## IoTHubTransport_Create
```c
extern TRANSPORT_HANDLE IoTHubTransport_Create(IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol, const char* iotHubName, const char* iotHubSuffix);
```

**SRS_IOTHUBTRANSPORT_17_001: [** IoTHubTransport_Create shall return a non-NULL handle on success.**]**

**SRS_IOTHUBTRANSPORT_17_002: [** If protocol is NULL, this function shall return NULL. **]**   
**SRS_IOTHUBTRANSPORT_17_003: [** If iotHubName is NULL, this function shall return NULL. **]**   
**SRS_IOTHUBTRANSPORT_17_004: [** If iotHubSuffix is NULL, this function shall return NULL. **]**   

**SRS_IOTHUBTRANSPORT_17_032: [** IoTHubTransport_Create shall allocate memory for the transport data. **]**

**SRS_IOTHUBTRANSPORT_17_040: [** If memory allocation fails, IoTHubTransport_Create shall return NULL. **]**

**SRS_IOTHUBTRANSPORT_17_005: [** IoTHubTransport_Create shall create the lower layer transport by calling the protocol's IoTHubTransport_Create function. **]**

**SRS_IOTHUBTRANSPORT_17_006: [** If the creation of the transport fails, IoTHubTransport_Create shall return NULL. **]**

**SRS_IOTHUBTRANSPORT_17_007: [** IoTHubTransport_Create shall create the transport lock by Calling Lock_Init. **]**

**SRS_IOTHUBTRANSPORT_17_008: [** If the lock creation fails, IoTHubTransport_Create shall return NULL. **]**

**SRS_IOTHUBTRANSPORT_17_038: [** IoTHubTransport_Create shall call VECTOR_Create to make a list of IOTHUB_CLIENT_HANDLE using this transport. **]**

**SRS_IOTHUBTRANSPORT_17_039: [** If the Vector creation fails, IoTHubTransport_Create shall return NULL. **]**

**SRS_IOTHUBTRANSPORT_17_009: [** IoTHubTransport_Create shall clean up any resources it creates if the function does not succeed. **]**


## IoTHubTransport_Destroy
```c
extern void					IoTHubTransport_Destroy(TRANSPORT_HANDLE transportHlHandle);
```

**SRS_IOTHUBTRANSPORT_17_033: [** IoTHubTransport_Destroy shall lock the transport lock. **]**

**SRS_IOTHUBTRANSPORT_17_010: [** IoTHubTransport_Destroy shall free all resources. **]**

IoTHubTransport_Destroy shall close the worker thread if worker thread is running.

**SRS_IOTHUBTRANSPORT_17_011: [** IoTHubTransport_Destroy shall do nothing if transportHlHandle is NULL. **]**

## IoTHubTransport_GetLock
```c
extern LOCK_HANDLE			IoTHubTransport_GetLock(TRANSPORT_HANDLE transportHlHandle);
```

**SRS_IOTHUBTRANSPORT_17_012: [** IoTHubTransport_GetLock shall return a handle to the transport lock. **]**

**SRS_IOTHUBTRANSPORT_17_013: [** If transportHlHandle is NULL, IoTHubTransport_GetLock shall return NULL. **]**

## IoTHubTransport_GetLLTransport
```c
extern TRANSPORT_LL_HANDLE		IoTHubTransport_GetLLTransport(TRANSPORT_HANDLE transportHlHandle);
```

**SRS_IOTHUBTRANSPORT_17_014: [** IoTHubTransport_GetLLTransport shall return a handle to the lower layer transport. **]**

**SRS_IOTHUBTRANSPORT_17_015: [** If transportHlHandle is NULL, IoTHubTransport_GetLLTransport shall return NULL. **]**

## IoTHubTransport_StartWorkerThread
```c
extern IOTHUB_CLIENT_RESULT IoTHubTransport_StartWorkerThread(TRANSPORT_HANDLE transportHlHandle, IOTHUB_CLIENT_HANDLE clientHandle);
```

IoTHubTransport_StartWorkerThread ensures a single thread for all device communication is created.  It also tracks which IoTHubClients have requested the thread to start, so that it know when the thread may be stopped.

**SRS_IOTHUBTRANSPORT_17_016: [** If transportHlHandle is NULL, IoTHubTransport_StartWorkerThread shall return IOTHUB_CLIENT_INVALID_ARG. **]**

**SRS_IOTHUBTRANSPORT_17_017: [** If clientHandle is NULL, IoTHubTransport_StartWorkerThread shall return IOTHUB_CLIENT_INVALID_ARG. **]**

**SRS_IOTHUBTRANSPORT_17_018: [** If the worker thread does not exist, IoTHubTransport_StartWorkerThread shall start the thread using ThreadAPI_Create. **]**

**SRS_IOTHUBTRANSPORT_17_019: [** If thread creation fails, IoTHubTransport_StartWorkerThread shall return IOTHUB_CLIENT_ERROR. **]**

**SRS_IOTHUBTRANSPORT_17_020: [** IoTHubTransport_StartWorkerThread shall search for IoTHubClient clientHandle in the list of IoTHubClient handles. **]**

**SRS_IOTHUBTRANSPORT_17_021: [** If handle is not found, then clientHandle shall be added to the list.  **]**

**SRS_IOTHUBTRANSPORT_17_042: [** If Adding to the client list fails, IoTHubTransport_StartWorkerThread shall return IOTHUB_CLIENT_ERROR. **]**

**SRS_IOTHUBTRANSPORT_17_022: [** Upon success, IoTHubTransport_StartWorkerThread shall return IOTHUB_CLIENT_OK. **]**

## IoTHubTransport_SignalEndWorkerThread
```c
extern bool IoTHubTransport_SignalEndWorkerThread(TRANSPORT_HANDLE transportHlHandle, IOTHUB_CLIENT_HANDLE clientHandle);
```

This function will signal the transport worker thread to end.  It will return true if
the thread has been successfully signalled to end and is ready to terminate.

**SRS_IOTHUBTRANSPORT_17_023: [** If transportHlHandle is NULL, IoTHubTransport_SignalEndWorkerThread shall return false. **]**

**SRS_IOTHUBTRANSPORT_17_024: [** If clientHandle is NULL, IoTHubTransport_SignalEndWorkerThread shall return false. **]**

**SRS_IOTHUBTRANSPORT_17_043: [** IoTHubTransport_SignalEndWorkerThread shall signal the worker thread to end. **]**

**SRS_IOTHUBTRANSPORT_17_025: [** If the worker thread does not exist, then IoTHubTransport_SignalEndWorkerThread shall return false. **]**

**SRS_IOTHUBTRANSPORT_17_026: [** IoTHubTransport_SignalEndWorkerThread shall remove clientHandlehandle from handle list. **]**


## IoTHubTransport_JoinWorkerThread
```c
extern void	IoTHubTransport_JoinWorkerThread(TRANSPORT_HANDLE transportHlHandle, IOTHUB_CLIENT_HANDLE clientHandle);
```

**SRS_IOTHUBTRANSPORT_17_044: [** If transportHlHandle is NULL, IoTHubTransport_JoinWorkerThread shall do nothing. **]**

**SRS_IOTHUBTRANSPORT_17_045: [** If clientHandle is NULL, IoTHubTransport_JoinWorkerThread shall do nothing. **]**

**SRS_IOTHUBTRANSPORT_17_027: [** The worker thread shall be joined.  **]**

## Worker Thread

**SRS_IOTHUBTRANSPORT_17_028: [** The thread shall exit when IoTHubTransport_EndWorkerThread has been called for each clientHandle which invoked IoTHubTransport_StartWorkerThread. **]**

**SRS_IOTHUBTRANSPORT_17_029: [** The thread shall call lower layer transport DoWork every 1 ms. **]**

**SRS_IOTHUBTRANSPORT_17_030: [** All calls to lower layer transport DoWork shall be protected by the lock created in IoTHubTransport_Create. **]**
 
**SRS_IOTHUBTRANSPORT_17_031: [** If acquiring the lock fails, lower layer transport DoWork shall not be called. **]**
