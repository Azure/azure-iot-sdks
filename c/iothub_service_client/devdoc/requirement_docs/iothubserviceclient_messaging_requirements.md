# IoTHubMessaging Requirements

## Overview

IoTHubMessaging is a module that extends the IoTHubMessaging_LL module with 2 features:
-scheduling the work for the IoTHubServiceClient from a thread, so that the user does not need to create its own thread.
-Thread-safe APIs
Underlaying layer in the following requirements refers to IoTHubMessaging_LL.

## Exposed API
```c
```

## IoTHubMessaging_Create

```c 
extern IOTHUB_MESSAGING_CLIENT_HANDLE IoTHubMessaging_Create(IOTHUB_SERVICE_CLIENT_AUTH_HANDLE serviceClientHandle);
```
**SRS_IOTHUBMESSAGING_12_001: [** `IoTHubMessaging_Create` shall verify the serviceClientHandle input parameter and if it is `NULL` then return `NULL`. **]**

**SRS_IOTHUBMESSAGING_12_002: [** `IoTHubMessaging_Create` shall allocate a new `IoTHubMessagingClient` instance. **]**

**SRS_IOTHUBMESSAGING_12_003: [** If allocating memory for the new `IoTHubMessagingClient` instance fails, then `IoTHubMessaging_Create` shall return `NULL`. **]**

**SRS_IOTHUBMESSAGING_12_004: [** `IoTHubMessaging_Create` shall create a lock object to be used later for serializing `IoTHubMessagingClient` calls. **]**

**SRS_IOTHUBMESSAGING_12_005: [** If creating the lock fails, then `IoTHubMessaging_Create` shall return NULL. **]**

**SRS_IOTHUBMESSAGING_12_006: [** `IoTHubMessaging_Create` shall instantiate a new `IoTHubMessaging_LL` instance by calling `IoTHubMessaging_LL_Create` and passing the `serviceClientHandle` argument. **]**

**SRS_IOTHUBMESSAGING_12_007: [** If `IoTHubMessaging_LL_Create` fails, then `IoTHubMessaging_Create` shall return `NULL`. **]**

**SRS_IOTHUBMESSAGING_12_008: [** If `IoTHubMessaging_Create` fails, all resources allocated by it shall be freed. **]**


## IoTHubMessaging_Destroy

```c
extern void IoTHubMessaging_Destroy(IOTHUB_MESSAGING_CLIENT_HANDLE messagingClientHandle);
```
**SRS_IOTHUBMESSAGING_12_009: [** `IoTHubMessaging_Destroy` shall do nothing if parameter `messagingClientHandle` is `NULL`. **]**

**SRS_IOTHUBMESSAGING_12_010: [** `IoTHubMessaging_Destroy` shall lock the serializing lock and signal the worker thread (if any) to end. **]**

**SRS_IOTHUBMESSAGING_12_011: [** `IoTHubMessaging_Destroy` shall destroy `IoTHubMessagingHandle` by call `IoTHubMessaging_LL_Destroy`. **]**

**SRS_IOTHUBMESSAGING_12_012: [** `IoTHubMessaging_Destroy` shall unlock the serializing lock. **]**

**SRS_IOTHUBMESSAGING_12_013: [** The thread created as part of executing `IoTHubMessaging_SendAsync` shall be joined. **]**

**SRS_IOTHUBMESSAGING_12_014: [** If the lock was allocated in `IoTHubMessaging_Create`, it shall be also freed. **]**


## IoTHubMessaging_Open
```c
extern IOTHUB_MESSAGING_RESULT IoTHubMessaging_Open(IOTHUB_MESSAGING_CLIENT_HANDLE messagingClientHandle, IOTHUB_OPEN_COMPLETE_CALLBACK openCompleteCallback, void* userContextCallback);
```
**SRS_IOTHUBMESSAGING_12_015: [** If `messagingClientHandle` is `NULL`, `IoTHubMessaging_Open` shall return `IOTHUB_MESSAGING_INVALID_ARG`. **]**

**SRS_IOTHUBMESSAGING_12_016: [** `IoTHubMessaging_Open` shall be made thread-safe by using the lock created in `IoTHubMessaging_Create`. **]**

**SRS_IOTHUBMESSAGING_12_017: [** If acquiring the lock fails, `IoTHubMessaging_Open` shall return `IOTHUB_MESSAGING_ERROR`. **]**

**SRS_IOTHUBMESSAGING_12_018: [** `IoTHubMessaging_Open` shall call `IoTHubMessaging_LL_Open`, while passing the `IOTHUB_MESSAGING_HANDLE` handle created by `IoTHubMessaging_Create` and the parameters `openCompleteCallback` and `userContextCallback`. **]**

**SRS_IOTHUBMESSAGING_12_019: [** When `IoTHubMessaging_LL_Open` is called, `IoTHubMessaging_Open` shall return the result of `IoTHubMessaging_LL_Open`. **]**

**SRS_IOTHUBMESSAGING_12_020: [** `IoTHubMessaging_Open` shall be made thread-safe by using the lock created in `IoTHubMessaging_Create`. **]**


## IoTHubMessaging_Close
```c
extern void IoTHubMessaging_Close(IOTHUB_MESSAGING_CLIENT_HANDLE messagingClientHandle)
```
**SRS_IOTHUBMESSAGING_12_021: [** If `messagingClientHandle` is `NULL`, `IoTHubMessaging_Close` shall do nothing. **]**

**SRS_IOTHUBMESSAGING_12_022: [** `IoTHubMessaging_Close` shall be made thread-safe by using the lock created in `IoTHubMessaging_Create`. **]**

**SRS_IOTHUBMESSAGING_12_023: [** If acquiring the lock fails, `IoTHubMessaging_Close` shall return `IOTHUB_MESSAGING_ERROR`. **]**

**SRS_IOTHUBMESSAGING_12_024: [** `IoTHubMessaging_Close` shall call `IoTHubMessaging_LL_Close`, while passing the `IOTHUB_MESSAGING_HANDLE` handle created by `IoTHubMessaging_Create` **]**

**SRS_IOTHUBMESSAGING_12_025: [** When `IoTHubMessaging_LL_Close` is called, `IoTHubMessaging_Close` shall return the result of `IoTHubMessaging_LL_Close`. **]**

**SRS_IOTHUBMESSAGING_12_026: [** `IoTHubMessaging_Close` shall be made thread-safe by using the lock created in `IoTHubMessaging_Create`. **]**


## IoTHubMessaging_SetFeedbackMessageCallback
```c
extern IOTHUB_MESSAGING_RESULT IoTHubMessaging_SetFeedbackMessageCallback(IOTHUB_MESSAGING_CLIENT_HANDLE messagingClientHandle, IOTHUB_FEEDBACK_MESSAGE_RECEIVED_CALLBACK feedbackMessageReceivedCallback, void* userContextCallback);
```
**SRS_IOTHUBMESSAGING_12_027: [** If `messagingClientHandle` is `NULL`, `IoTHubMessaging_SetFeedbackMessageCallback` shall return `IOTHUB_MESSAGING_INVALID_ARG`. **]**

**SRS_IOTHUBMESSAGING_12_028: [** `IoTHubMessaging_SetFeedbackMessageCallback` shall be made thread-safe by using the lock created in `IoTHubMessaging_Create`. **]**

**SRS_IOTHUBMESSAGING_12_029: [** If acquiring the lock fails, `IoTHubMessaging_SetFeedbackMessageCallback` shall return `IOTHUB_MESSAGING_ERROR`. **]**

**SRS_IOTHUBMESSAGING_12_030: [** `IoTHubMessaging_SetFeedbackMessageCallback` shall call `IoTHubMessaging_LL_SetFeedbackMessageCallback`, while passing the `IOTHUB_MESSAGING_HANDLE` handle created by `IoTHubMessaging_Create`, `feedbackMessageReceivedCallback` and `userContextCallback` **]**

**SRS_IOTHUBMESSAGING_12_031: [** When `IoTHubMessaging_LL_SetFeedbackMessageCallback` is called, `IoTHubMessaging_SetFeedbackMessageCallback` shall return the result of `IoTHubMessaging_LL_SetFeedbackMessageCallback`. **]**

**SRS_IOTHUBMESSAGING_12_032: [** `IoTHubMessaging_SetFeedbackMessageCallback` shall be made thread-safe by using the lock created in `IoTHubMessaging_Create`. **]**


## IoTHubMessaging_SendAsync
```c
extern IOTHUB_MESSAGING_RESULT IoTHubMessaging_SendAsync(IOTHUB_MESSAGING_CLIENT_HANDLE messagingClientHandle, const char* deviceId, IOTHUB_MESSAGE_HANDLE message, IOTHUB_SEND_COMPLETE_CALLBACK sendCompleteCallback, void* userContextCallback)
```

**SRS_IOTHUBMESSAGING_12_033: [** If `messagingClientHandle` is `NULL`, `IoTHubMessaging_SendAsync` shall return `IOTHUB_MESSAGING_INVALID_ARG`. **]**

**SRS_IOTHUBMESSAGING_12_034: [** `IoTHubMessaging_SendAsync` shall be made thread-safe by using the lock created in `IoTHubMessaging_Create`. **]**

**SRS_IOTHUBMESSAGING_12_035: [** If acquiring the lock fails, `IoTHubMessaging_SendAsync` shall return `IOTHUB_MESSAGING_ERROR`. **]**

**SRS_IOTHUBMESSAGING_12_036: [** `IoTHubMessaging_SendAsync` shall start the worker thread if it was not previously started. **]**

**SRS_IOTHUBMESSAGING_12_037: [** If starting the thread fails, `IoTHubMessaging_SendAsync` shall return `IOTHUB_CLIENT_ERROR`. **]**

**SRS_IOTHUBMESSAGING_12_038: [** `IoTHubMessaging_SendAsync` shall call `IoTHubMessaging_LL_Send`, while passing the `IOTHUB_MESSAGING_HANDLE` handle created by `IoTHubClient_Create` and the parameters `deviceId`, `message`, `sendCompleteCallback` and `userContextCallback`.

**SRS_IOTHUBMESSAGING_12_039: [** When `IoTHubMessaging_LL_Send` is called, `IoTHubMessaging_SendAsync` shall return the result of `IoTHubMessaging_LL_Send`. **]**

**SRS_IOTHUBMESSAGING_12_040: [** `IoTHubClient_SendEventAsync` shall be made thread-safe by using the lock created in `IoTHubClient_Create`. **]**


### Scheduling work

**SRS_IOTHUBMESSAGING_12_041: [** The thread shall exit when all IoTHubServiceClients using the thread have had `IoTHubMessaging_Destroy` called. **]**

**SRS_IOTHUBMESSAGING_12_042: [** The thread created by `IoTHubMessaging_SendAsync` shall call `IoTHubMessaging_LL_DoWork` every 1 ms. **]**

**SRS_IOTHUBMESSAGING_12_043: [** All calls to `IoTHubMessaging_LL_DoWork` shall be protected by the lock created in `IoTHubMessaging_Create`. **]**

**SRS_IOTHUBMESSAGING_12_044: [** If acquiring the lock fails, `IoTHubMessaging_LL_DoWork` shall not be called. **]**
