# IoTHubClient_LL Requirements

## Overview

IoTHubClient_LL is a module allowing a user (usually a device) to communicate with an Azure IoTHub. It can send event messages and receive messages. At any given moment in time there can only be at most 1 message sink function.
Undelaying layer in the following requirements refers either to AMQP or HTTP.
Exposed API

```c
#define IOTHUB_CLIENT_RESULT_VALUES          \
    IOTHUB_CLIENT_OK,                        \
    IOTHUB_CLIENT_INVALID_ARG,               \
    IOTHUB_CLIENT_ERROR,                     \

DEFINE_ENUM(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_RESULT_VALUES);

#define IOTHUB_CLIENT_CONFIRMATION_RESULT_VALUES     \
    IOTHUB_CLIENT_CONFIRMATION_OK,                   \
    IOTHUB_CLIENT_CONFIRMATION_TIMEOUT,              \
    IOTHUB_CLIENT_CONFIRMATION_ERROR                 \

DEFINE_ENUM(IOTHUB_CLIENT_CONFIRMATION_RESULT, IOTHUB_CLIENT_CONFIRMATION_RESULT_VALUES);

#define IOTHUB_CLIENT_STATUS_VALUES       \
	IOTHUB_CLIENT_SEND_STATUS_IDLE,       \
	IOTHUB_CLIENT_SEND_STATUS_BUSY        \

DEFINE_ENUM(IOTHUB_CLIENT_STATUS, IOTHUB_CLIENT_STATUS_VALUES);

#define IOTHUB_CLIENT_CONNECTION_STATUS_VALUES                                     \
    IOTHUB_CLIENT_CONNECTION_AUTHENTICATED,                \
    IOTHUB_CLIENT_CONNECTION_UNAUTHENTICATED               \

    DEFINE_ENUM(IOTHUB_CLIENT_CONNECTION_STATUS, IOTHUB_CLIENT_CONNECTION_STATUS_VALUES);

#define IOTHUB_CLIENT_CONNECTION_STATUS_REASON_VALUES                               \
    IOTHUB_CLIENT_CONNECTION_EXPIRED_SAS_TOKEN,            \
    IOTHUB_CLIENT_CONNECTION_DEVICE_DISABLED,              \
    IOTHUB_CLIENT_CONNECTION_BAD_CREDENTIAL,               \
    IOTHUB_CLIENT_CONNECTION_RETRY_EXPIRED,                \
    IOTHUB_CLIENT_CONNECTION_NO_NETWORK,                   \
    IOTHUB_CLIENT_CONNECTION_OK                            \

    DEFINE_ENUM(IOTHUB_CLIENT_CONNECTION_STATUS_REASON, IOTHUB_CLIENT_CONNECTION_STATUS_REASON_VALUES);

#define IOTHUB_CLIENT_RETRY_POLICY_VALUES     \
    IOTHUB_CLIENT_RETRY_NONE,                   \
    IOTHUB_CLIENT_RETRY_IMMEDIATE,                  \
    IOTHUB_CLIENT_RETRY_INTERVAL,      \
    IOTHUB_CLIENT_RETRY_LINEAR_BACKOFF,      \
    IOTHUB_CLIENT_RETRY_EXPONENTIAL_BACKOFF,                 \
    IOTHUB_CLIENT_RETRY_EXPONENTIAL_BACKOFF_WITH_JITTER,                 \
    IOTHUB_CLIENT_RETRY_RANDOM

DEFINE_ENUM(IOTHUB_CLIENT_RETRY_POLICY, IOTHUB_CLIENT_RETRY_POLICY_VALUES);

#define IOTHUBMESSAGE_DISPOSITION_RESULT_VALUES \
    IOTHUBMESSAGE_ACCEPTED, \
    IOTHUBMESSAGE_REJECTED, \
    IOTHUBMESSAGE_ABANDONED

DEFINE_ENUM(IOTHUBMESSAGE_DISPOSITION_RESULT, IOTHUBMESSAGE_DISPOSITION_RESULT_VALUES);

typedef void* IOTHUB_CLIENT_HANDLE;
typedef void(*IOTHUB_CLIENT_EVENT_CONFIRMATION_CALLBACK)(IOTHUB_CLIENT_CONFIRMATION_RESULT result, void* userContextCallback);
typedef void(*IOTHUB_CLIENT_CONNECTION_STATUS_CALLBACK)(IOTHUB_CLIENT_CONNECTION_STATUS result, IOTHUB_CLIENT_CONNECTION_STATUS_REASON reason, void* userContextCallback);
typedef int(*IOTHUB_CLIENT_MESSAGE_CALLBACK_ASYNC)(IOTHUB_CLIENT_HANDLE iotHubClientHandle, IOTHUB_MESSAGE_HANDLE message, void* userContextCallback);
typedef void*(*IOTHUB_CLIENT_TRANSPORT_PROVIDER)(void);
 
typedef struct IOTHUB_CLIENT_CONFIG_TAG
{
    IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol;
    const char* deviceId;
    const char* deviceKey;
    const char* deviceSasToken;
    const char* iotHubName;
    const char* iotHubSuffix;
    const char* protocolGatewayHostName;
} IOTHUB_CLIENT_CONFIG;

typedef struct IOTHUB_CLIENT_DEVICE_CONFIG_TAG
{
	IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol;
	void * transportHandle;
	const char* deviceId;
	const char* deviceKey;
  const char* deviceSasToken;
} IOTHUB_CLIENT_DEVICE_CONFIG;


extern IOTHUB_CLIENT_LL_HANDLE IoTHubClient_LL_CreateFromConnectionString(const char* connectionString, IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol);

extern IOTHUB_CLIENT_LL_HANDLE IoTHubClient_LL_Create(const IOTHUB_CLIENT_CONFIG* config);
extern  IOTHUB_CLIENT_LL_HANDLE IoTHubClient_LL_CreateWithTransport(IOTHUB_CLIENT_DEVICE_CONFIG * config);

extern void IoTHubClient_LL_Destroy(IOTHUB_CLIENT_HANDLE iotHubClientHandle);
 
extern IOTHUB_CLIENT_RESULT IoTHubClient_LL_SendEventAsync(IOTHUB_CLIENT_HANDLE iotHubClientHandle, IOTHUB_MESSAGE_HANDLE eventMessageHandle, IOTHUB_CLIENT_EVENT_CONFIRMATION_CALLBACK eventConfirmationCallback, void* userContextCallback);
extern void IoTHubClient_LL_DoWork(IOTHUB_CLIENT_HANDLE iotHubClientHandle);
extern IOTHUB_CLIENT_RESULT IoTHubClient_LL_SetMessageCallback(IOTHUB_CLIENT_HANDLE iotHubClientHandle, IOTHUB_CLIENT_MESSAGE_CALLBACK_ASYNC messageCallback, void* userContextCallback);
extern IOTHUB_CLIENT_RESULT IoTHubClient_LL_SetConnectionStatusCallback(IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle, IOTHUB_CLIENT_CONNECTION_STATUS_CALLBACK connectionStatusCallback, void* userContextCallback);
extern IOTHUB_CLIENT_RESULT IoTHubClient_LL_SetRetryPolicy(IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle, IOTHUB_CLIENT_RETRY_POLICY retryPolicy, size_t retryTimeoutLimit);
extern IOTHUB_CLIENT_RESULT IoTHubClient_LL_GetRetryPolicy(IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle, IOTHUB_CLIENT_RETRY_POLICY* retryPolicy, size_t* retryTimeoutLimit);
extern IOTHUB_CLIENT_RESULT IoTHubClient_LL_GetSendStatus(IOTHUB_CLIENT_HANDLE iotHubClientHandle, IOTHUB_CLIENT_STATUS *iotHubClientStatus);
extern IOTHUB_CLIENT_RESULT IoTHubClient_LL_GetLastMessageReceiveTime(IOTHUB_CLIENT_HANDLE iotHubClientHandle, time_t* lastMessageReceiveTime);
extern IOTHUB_CLIENT_RESULT IoTHubClient_LL_SetOption(IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle, const char* optionName, const void* value);
extern IOTHUB_CLIENT_RESULT IoTHubClient_LL_UploadToBlob(IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle, const char* destinationFileName, const unsigned char* source, size_t size);

## DeviceTwin
extern IOTHUB_CLIENT_RESULT IoTHubClient_LL_SetDeviceTwinCallback(IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle, IOTHUB_CLIENT_DEVICE_TWIN_CALLBACK deviceTwinCallback, void* userContextCallback);
extern IOTHUB_CLIENT_RESULT IoTHubClient_LL_SendReportedState(IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle, const unsigned char* reportedState, size_t size, uint32_t reportedVersion, uint32_t lastSeenDesiredVersion, IOTHUB_CLIENT_REPORTED_STATE_CALLBACK reportedStateCallback, void* userContextCallback);
```

## IoTHubClient_LL_CreateFromConnectionString

```c
extern IOTHUB_CLIENT_LL_HANDLE IoTHubClient_LL_CreateFromConnectionString(const char* connectionString, IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol);
```

**SRS_IOTHUBCLIENT_LL_05_001: [** `IoTHubClient_LL_CreateFromConnectionString` shall obtain the version string by a call to `IoTHubClient_GetVersionString`.** ]**

**SRS_IOTHUBCLIENT_LL_05_002: [** `IoTHubClient_LL_CreateFromConnectionString` shall print the version string to standard output.** ]**

**SRS_IOTHUBCLIENT_LL_12_003: [** `IoTHubClient_LL_CreateFromConnectionString` shall verify the input parameters and if any of them `NULL` then return `NULL`.** ]**

**SRS_IOTHUBCLIENT_LL_12_004: [** `IoTHubClient_LL_CreateFromConnectionString` shall allocate `IOTHUB_CLIENT_CONFIG` structure.** ]**

**SRS_IOTHUBCLIENT_LL_12_012: [** If the allocation failed `IoTHubClient_LL_CreateFromConnectionString`  returns `NULL`.** ]**

**SRS_IOTHUBCLIENT_LL_12_005: [** `IoTHubClient_LL_CreateFromConnectionString` shall try to parse the `connectionString` input parameter for the following structure: "Key1=value1;key2=value2;key3=value3... ." ]**

**SRS_IOTHUBCLIENT_LL_12_013: [** If the parsing failed `IoTHubClient_LL_CreateFromConnectionString` returns `NULL`.** ]**

**SRS_IOTHUBCLIENT_LL_12_006: [** `IoTHubClient_LL_CreateFromConnectionString` shall verify the existence of the following Key/Value pairs in the connection string: `HostName`, `DeviceId`, `SharedAccessKey`, `SharedAccessSignature` or `x509`.** ]**

**SRS_IOTHUBCLIENT_LL_12_014: [** If either of key is missing or x509 is not set to `"true"` then `IoTHubClient_LL_CreateFromConnectionString` returns `NULL`.** ]**

**SRS_IOTHUBCLIENT_LL_02_092: [** `IoTHubClient_LL_CreateFromConnectionString` shall create a `IOTHUB_CLIENT_LL_UPLOADTOBLOB_HANDLE` from `IOTHUB_CLIENT_CONFIG`.** ]**

**SRS_IOTHUBCLIENT_LL_02_093: [** If creating the `IOTHUB_CLIENT_LL_UPLOADTOBLOB_HANDLE` fails then `IoTHubClient_LL_CreateFromConnectionString` shall fail and return `NULL`.** ]**

**SRS_IOTHUBCLIENT_LL_12_009: [** `IoTHubClient_LL_CreateFromConnectionString` shall split the value of HostName to Name and Suffix using the first "." as a separator.** ]**

**SRS_IOTHUBCLIENT_LL_12_015: [** If the string split failed, `IoTHubClient_LL_CreateFromConnectionString` returns `NULL`.** ]**

**SRS_IOTHUBCLIENT_LL_12_010: [** `IoTHubClient_LL_CreateFromConnectionString` shall fill up the `IOTHUB_CLIENT_CONFIG` structure using the following mapping: iotHubName = Name, iotHubSuffix = Suffix, deviceId = DeviceId, deviceKey = SharedAccessKey or deviceSasToken = SharedAccessSignature.** ]**

**SRS_IOTHUBCLIENT_LL_03_010: [** `IoTHubClient_LL_CreateFromConnectionString` shall return `NULL` if both a `deviceKey` & a `deviceSasToken` are specified.** ]**

**SRS_IOTHUBCLIENT_LL_12_011: [** `IoTHubClient_LL_CreateFromConnectionString` shall call into the `IoTHubClient_LL_Create` API with the current structure and returns with the return value of it.** ]**

**SRS_IOTHUBCLIENT_LL_12_016: [** `IoTHubClient_LL_CreateFromConnectionString` shall return `NULL` if `IoTHubClient_LL_Create` call fails.** ]**

**SRS_IOTHUBCLIENT_LL_04_001: [** `IoTHubClient_LL_CreateFromConnectionString` shall verify the existence of key/value pair GatewayHostName. If it does exist it shall pass the value to IoTHubClient_LL_Create API.** ]**

**SRS_IOTHUBCLIENT_LL_04_002: [** If it does not, it shall pass the `protocolGatewayHostName` `NULL`.** ]** 



## IoTHubClient_LL_Create

```c
extern IOTHUB_CLIENT_LL_HANDLE IoTHubClient_LL_Create(const IOTHUB_CLIENT_CONFIG* config);
```

**SRS_IOTHUBCLIENT_LL_02_001: [** `IoTHubClient_LL_Create` shall return `NULL` if config parameter is `NULL` or protocol field is `NULL`.** ]**

**SRS_IOTHUBCLIENT_LL_02_094: [** `IoTHubClient_LL_Create` shall create a `IOTHUB_CLIENT_LL_UPLOADTOBLOB_HANDLE` from `IOTHUB_CLIENT_CONFIG`.** ]**

**SRS_IOTHUBCLIENT_LL_02_095: [** If creating the `IOTHUB_CLIENT_LL_UPLOADTOBLOB_HANDLE` fails then `IoTHubClient_LL_Create` shall fail and return `NULL`.** ]**

**SRS_IOTHUBCLIENT_LL_02_045: [** Otherwise `IoTHubClient_LL_Create` shall create a new `TICK_COUNTER_HANDLE`.** ]**

**SRS_IOTHUBCLIENT_LL_02_046: [** If creating the `TICK_COUNTER_HANDLE` fails then `IoTHubClient_LL_Create` shall fail and return `NULL`.** ]**

**SRS_IOTHUBCLIENT_LL_02_004: [** Otherwise `IoTHubClient_LL_Create` shall initialize a new DLIST (further called "waitingToSend") containing records with fields of the following types: IOTHUB_MESSAGE_HANDLE, IOTHUB_CLIENT_EVENT_CONFIRMATION_CALLBACK, void*.** ]**

**SRS_IOTHUBCLIENT_LL_02_006: [** `IoTHubClient_LL_Create` shall populate a structure of type `IOTHUBTRANSPORT_CONFIG` with the information from config parameter and the previous DLIST and shall pass that to the underlying layer _Create function.** ]**

**SRS_IOTHUBCLIENT_LL_02_007: [** If the underlaying layer _Create function fails them `IoTHubClient_LL_Create` shall fail and return `NULL`.** ]**

**SRS_IOTHUBCLIENT_LL_17_008: [** `IoTHubClient_LL_Create` shall call the transport _Register function with a populated structure of type `IOTHUB_DEVICE_CONFIG` and waitingToSend list.** ]**

**SRS_IOTHUBCLIENT_LL_17_009: [** If the _Register function fails, this function shall fail and return `NULL`.** ]**

**SRS_IOTHUBCLIENT_LL_02_008: [** Otherwise, `IoTHubClient_LL_Create` shall succeed and return a `non-NULL` handle.** ]** 

**SRS_IOTHUBCLIENT_LL_25_124: [** `IoTHubClient_LL_Create` shall set the default retry policy as Exponential backoff with jitter and if succeed and return a `non-NULL` handle.** ]**



## IoTHubClient_LL_CreateWithTransport

```c
extern  IOTHUB_CLIENT_LL_HANDLE IoTHubClient_LL_CreateWithTransport(IOTHUB_CLIENT_DEVICE_CONFIG * config);
```

**SRS_IOTHUBCLIENT_LL_17_001: [** `IoTHubClient_LL_CreateWithTransport` shall return `NULL` if `config` parameter is `NULL`, or `protocol` field is `NULL` or `transportHandle` is `NULL`.** ]**

**SRS_IOTHUBCLIENT_LL_02_098: [** `IoTHubClient_LL_CreateWithTransport` shall fail and return `NULL` if both `config->deviceKey` AND `config->deviceSasToken` are `NULL`.** ]**

**SRS_IOTHUBCLIENT_LL_17_002: [** `IoTHubClient_LL_CreateWithTransport` shall allocate data for the `IOTHUB_CLIENT_LL_HANDLE`.** ]**

**SRS_IOTHUBCLIENT_LL_17_003: [** If allocation fails, the function shall fail and return `NULL`.** ]**

**SRS_IOTHUBCLIENT_LL_02_096: [** `IoTHubClient_LL_CreateWithTransport` shall create the data structures needed to instantiate a `IOTHUB_CLIENT_LL_UPLOADTOBLOB_HANDLE`.** ]**

**SRS_IOTHUBCLIENT_LL_02_097: [** If creating the data structures fails or instantiating the `IOTHUB_CLIENT_LL_UPLOADTOBLOB_HANDLE` fails then `IoTHubClient_LL_CreateWithTransport` shall fail and return `NULL`.** ]**

**SRS_IOTHUBCLIENT_LL_02_047: [** `IoTHubClient_LL_CreateWithTransport` shall create a `TICK_COUNTER_HANDLE`.** ]**

**SRS_IOTHUBCLIENT_LL_02_048: [** If creating the handle fails, then `IoTHubClient_LL_CreateWithTransport` shall fail and return `NULL`.** ]**

**SRS_IOTHUBCLIENT_LL_17_004: [** `IoTHubClient_LL_CreateWithTransport` shall initialize a new DLIST (further called "waitingToSend") containing records with fields of the following types: IOTHUB_MESSAGE_HANDLE, IOTHUB_CLIENT_EVENT_CONFIRMATION_CALLBACK, void\*.** ]**

**SRS_IOTHUBCLIENT_LL_17_005: [** `IoTHubClient_LL_CreateWithTransport` shall save the transport handle and mark this transport as shared.** ]**

**SRS_IOTHUBCLIENT_LL_17_006: [** `IoTHubClient_LL_CreateWithTransport` shall call the transport _Register function with the `IOTHUB_DEVICE_CONFIG` populated structure and waitingToSend list.** ]**

**SRS_IOTHUBCLIENT_LL_17_007: [** If the _Register function fails, this function shall fail and return `NULL`.** ]** 

**SRS_IOTHUBCLIENT_LL_25_125: [** `IoTHubClient_LL_CreateWithTransport` shall set the default retry policy as Exponential backoff with jitter and if succeed and return a `non-NULL` handle.** ]**



## IoTHubClient_LL_Destroy

```c
extern void IoTHubClient_LL_Destroy(IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle);
```

**SRS_IOTHUBCLIENT_LL_02_009: [** `IoTHubClient_LL_Destroy` shall do nothing if parameter `iotHubClientHandle` is `NULL`.** ]**

**SRS_IOTHUBCLIENT_LL_02_033: [** Otherwise, `IoTHubClient_LL_Destroy` shall complete all the event message callbacks that are in the waitingToSend list with the result IOTHUB_CLIENT_CONFIRMATION_BECAUSE_DESTROY.** ]**

**SRS_IOTHUBCLIENT_LL_17_010: [** `IoTHubClient_LL_Destroy`  shall call the underlaying layer's _Unregister function.** ]**

**SRS_IOTHUBCLIENT_LL_02_010: [** If `iotHubClientHandle` was not created by `IoTHubClient_LL_CreateWithTransport`, `IoTHubClient_LL_Destroy`  shall call the underlaying layer's _Destroy function. and shall free the resources allocated by `IoTHubClient` (if any).** ]**

**SRS_IOTHUBCLIENT_LL_17_011: [** `IoTHubClient_LL_Destroy` shall free the resources allocated by `IoTHubClient` (if any).** ]** 

**SRS_IOTHUBCLIENT_LL_07_007: [** `IoTHubClient_LL_Destroy` shall iterate the device twin queues and destroy any remaining items. **]**


## IoTHubClient_LL_SendEventAsync

```c 
extern IOTHUB_CLIENT_RESULT IoTHubClient_LL_SendEventAsync(IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle, IOTHUB_MESSAGE_HANDLE eventMessageHandle, IOTHUB_CLIENT_EVENT_CONFIRMATION_CALLBACK eventConfirmationCallback, void* userContextCallback);
```

**SRS_IOTHUBCLIENT_LL_02_011: [** `IoTHubClient_LL_SendEventAsync` shall fail and return `IOTHUB_CLIENT_INVALID_ARG` if parameter `iotHubClientHandle` or `eventMessageHandle` is `NULL`.** ]**

**SRS_IOTHUBCLIENT_LL_02_012: [** `IoTHubClient_LL_SendEventAsync` shall fail and return `IOTHUB_CLIENT_INVALID_ARG` if parameter `eventConfirmationCallback` is `NULL` and userContextCallback is not `NULL`.** ]**

**SRS_IOTHUBCLIENT_LL_02_013: [** `IotHubClient_SendEventAsync` shall add the DLIST waitingToSend a new record cloning the information from `eventMessageHandle`, `eventConfirmationCallback`, `userContextCallback`.** ]**

**SRS_IOTHUBCLIENT_LL_02_014: [** If cloning and/or adding the information fails for any reason, `IoTHubClient_LL_SendEventAsync` shall fail and return `IOTHUB_CLIENT_ERROR`.** ]**

**SRS_IOTHUBCLIENT_LL_02_015: [** Otherwise `IoTHubClient_LL_SendEventAsync` shall succeed and return `IOTHUB_CLIENT_OK`.** ]** 



## IoTHubClient_LL_SetMessageCallback

```c
extern IOTHUB_CLIENT_RESULT IoTHubClient_LL_SetMessageCallback(IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle, IOTHUB_CLIENT_MESSAGE_CALLBACK_ASYNC messageCallback, void* userContextCallback);
```

**SRS_IOTHUBCLIENT_LL_02_016: [** `IoTHubClient_LL_SetMessageCallback` shall fail and return `IOTHUB_CLIENT_INVALID_ARG` if parameter `iotHubClientHandle` is `NULL`.** ]**

**SRS_IOTHUBCLIENT_LL_02_017: [** If parameter `messageCallback` is `non-NULL` then `IoTHubClient_LL_SetMessageCallback` shall call the underlying layer's _Subscribe function.** ]**

**SRS_IOTHUBCLIENT_LL_02_018: [** If the underlying layer's _Subscribe function fails, then `IoTHubClient_LL_SetMessageCallback` shall fail and return `IOTHUB_CLIENT_ERROR`. Otherwise `IoTHubClient_LL_SetMessageCallback` shall succeed and return `IOTHUB_CLIENT_OK`.** ]**

**SRS_IOTHUBCLIENT_LL_02_019: [** If parameter `messageCallback` is `NULL` then `IoTHubClient_LL_SetMessageCallback` shall call the underlying layer's _Unsubscribe function and return `IOTHUB_CLIENT_OK`.** ]** 



## IoTHubClient_LL_DoWork

```c
extern void IoTHubClient_LL_DoWork(IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle);
```

**SRS_IOTHUBCLIENT_LL_02_020: [** If parameter `iotHubClientHandle` is `NULL` then `IoTHubClient_LL_DoWork` shall not perform any action.** ]**

**SRS_IOTHUBCLIENT_LL_02_021: [** Otherwise, `IoTHubClient_LL_DoWork` shall invoke the underlaying layer's _DoWork function.** ]** 

**SRS_IOTHUBCLIENT_LL_07_008: [** `IoTHubClient_LL_DoWork` shall iterate the message queue and execute the underlying transports `IoTHubTransport_ProcessItem` function for each item.** ]** 

**SRS_IOTHUBCLIENT_LL_07_010: [** If 'IoTHubTransport_ProcessItem' returns IOTHUB_PROCESS_CONTINUE or IOTHUB_PROCESS_NOT_CONNECTED `IoTHubClient_LL_DoWork` shall continue on to call the underlaying layer's _DoWork function.** ]**  

**SRS_IOTHUBCLIENT_LL_07_011: [** If 'IoTHubTransport_ProcessItem' returns IOTHUB_PROCESS_OK `IoTHubClient_LL_DoWork` shall add the `IOTHUB_QUEUE_DATA_ITEM` to the ack queue.** ]**

**SRS_IOTHUBCLIENT_LL_07_012: [** If 'IoTHubTransport_ProcessItem' returns any other value `IoTHubClient_LL_DoWork` shall destroy the `IOTHUB_QUEUE_DATA_ITEM` item. **]**

## IoTHubClient_LL_SendComplete

```c
void IoTHubClient_LL_SendComplete(IOTHUB_CLIENT_LL_HANDLE handle, PDLIST_ENTRY completed, IOTHUB_BATCHSTATE result)
```

**SRS_IOTHUBCLIENT_LL_02_022: [** If parameter `completed` is `NULL` or parameter `handle` is `NULL` then `IoTHubClient_LL_SendComplete` shall return.** ]** 

#### IoTHubClient_LL_SendComplete is a function that is only called by the lower layers. Completed is a PDLIST containing records of the same type as those created in the IoTHubClient _Create function. Result is a parameter that indicates that the result of the sending the batch.

**SRS_IOTHUBCLIENT_LL_02_025: [** If parameter `result` is `IOTHUB_BATCHSTATE_SUCCESS` then `IoTHubClient_LL_SendComplete` shall call all the `non-NULL` callbacks with the result parameter set to `IOTHUB_CLIENT_CONFIRMATION_OK` and the context set to the context passed originally in the `SendEventAsync` call.** ]**

**SRS_IOTHUBCLIENT_LL_02_026: [** If any callback is `NULL` then there shall not be a callback call.** ]**

**SRS_IOTHUBCLIENT_LL_02_027: [** If parameter result is `IOTHUB_BACTCHSTATE_FAILED` then `IoTHubClient_LL_SendComplete` shall call all the `non-NULL` callbacks with the result parameter set to `IOTHUB_CLIENT_CONFIRMATION_ERROR` and the context set to the context passed originally in the `SendEventAsync` call.** ]**



## IoTHubClient_LL_MessageCallback

```c
extern IOTHUBMESSAGE_DISPOSITION_RESULT IoTHubClient_LL_MessageCallback(IOTHUB_CLIENT_LL_HANDLE handle, IOTHUB_MESSAGE_HANDLE message);
```

This function is only called by the lower layers upon receiving a message from IoTHub.
**SRS_IOTHUBCLIENT_LL_02_029: [** If parameter handle is `NULL` then `IoTHubClient_LL_MessageCallback` shall return `IOTHUBMESSAGE_ABANDONED`.** ]**

**SRS_IOTHUBCLIENT_LL_02_030: [** `IoTHubClient_LL_MessageCallback` shall invoke the last callback function (the parameter `messageCallback` to `IoTHubClient_LL_SetMessageCallback`) passing the message and the passed `userContextCallback`.** ]**

**SRS_IOTHUBCLIENT_LL_02_031: [** Then `IoTHubClient_LL_MessageCallback` shall return what the user function returns.** ]**

**SRS_IOTHUBCLIENT_LL_02_032: [** If the last callback function was `NULL`, then `IoTHubClient_LL_MessageCallback`  shall return `IOTHUBMESSAGE_ABANDONED`.** ]** 



## IoTHubClient_LL_GetSendStatus

```c
extern IOTHUB_CLIENT_RESULT IoTHubClient_LL_GetSendStatus(IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle, IOTHUB_CLIENT_STATUS *iotHubClientStatus);
```

**SRS_IOTHUBCLIENT_LL_09_007: [** `IoTHubClient_LL_GetSendStatus` shall return `IOTHUB_CLIENT_INVALID_ARG` if called with `NULL` parameter.** ]**

**SRS_IOTHUBCLIENT_LL_09_008: [** `IoTHubClient_LL_GetSendStatus` shall return `IOTHUB_CLIENT_OK` and status `IOTHUB_CLIENT_SEND_STATUS_IDLE` if there is currently no items to be sent.** ]**

**SRS_IOTHUBCLIENT_LL_09_009: [** `IoTHubClient_LL_GetSendStatus` shall return `IOTHUB_CLIENT_OK` and status `IOTHUB_CLIENT_SEND_STATUS_BUSY` if there are currently items to be sent.** ]** 

###IoTHubClient_LL_SetConnectionStatusCallback
```c
extern IOTHUB_CLIENT_RESULT IoTHubClient_LL_SetConnectionStatusCallback(IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle, IOTHUB_CLIENT_CONNECTION_STATUS_CALLBACK connectionStatusCallback, void* userContextCallback);
```
**SRS_IOTHUBCLIENT_LL_25_111: [**IoTHubClient_LL_SetConnectionStatusCallback shall return IOTHUB_CLIENT_INVALID_ARG if called with NULL parameter iotHubClientHandle**]**

**SRS_IOTHUBCLIENT_LL_25_112: [**IoTHubClient_LL_SetConnectionStatusCallback shall return IOTHUB_CLIENT_OK and save the callback and userContext as a member of the handle.**]**

###IotHubClient_LL_ConnectionStatusCallBack
```c
extern void IotHubClient_LL_ConnectionStatusCallBack(IOTHUB_CLIENT_LL_HANDLE handle, IOTHUB_CLIENT_CONNECTION_STATUS connectionStatus, IOTHUB_CLIENT_CONNECTION_STATUS_REASON reason);
```
**SRS_IOTHUBCLIENT_LL_25_113: [**If parameter connectionStatus is NULL or parameter handle is NULL then IotHubClient_LL_ConnectionStatusCallBack shall return.**]**

IotHubClient_LL_ConnectionStatusCallBack is a function that is only called by the lower layers. connectionStatus represents the authentication state of the client to the IOTHUB with reason for change.

**SRS_IOTHUBCLIENT_LL_25_114: [**IotHubClient_LL_ConnectionStatusCallBack shall call non-callback set by the user from IoTHubClient_LL_SetConnectionStatusCallback passing the status, reason and the passed userContextCallback.**]**

###IoTHubClient_LL_SetRetryPolicy
```c
extern IOTHUB_CLIENT_RESULT IoTHubClient_LL_SetRetryPolicy(IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle, IOTHUB_CLIENT_RETRY_POLICY retryPolicy, size_t retryTimeoutLimitinSeconds);
```
**SRS_IOTHUBCLIENT_LL_25_116: [**IoTHubClient_LL_SetRetryPolicy shall return IOTHUB_CLIENT_INVALID_ARG if called with NULL iotHubClientHandle**]**

**SRS_IOTHUBCLIENT_LL_25_118: [**IoTHubClient_LL_SetRetryPolicy shall save connection retry policies specified by the user to retryPolicy in struct IOTHUB_CLIENT_LL_HANDLE_DATA**]**

**SRS_IOTHUBCLIENT_LL_25_119: [**IoTHubClient_LL_SetRetryPolicy shall save retryTimeoutLimitinSeconds in seconds to retryTimeout in struct IOTHUB_CLIENT_LL_HANDLE_DATA**]**

###IoTHubClient_LL_GetRetryPolicy
```c
extern IOTHUB_CLIENT_RESULT IoTHubClient_LL_GetRetryPolicy(IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle, IOTHUB_CLIENT_RETRY_POLICY* retryPolicy, size_t* retryTimeoutLimitinSeconds);
```
**SRS_IOTHUBCLIENT_LL_25_120: [**IoTHubClient_LL_GetRetryPolicy shall return IOTHUB_CLIENT_INVALID_ARG if called with NULL iotHubClientHandle or retryPolicy or retryTimeoutLimitinSeconds parameters**]**

**SRS_IOTHUBCLIENT_LL_25_121: [**IoTHubClient_LL_GetRetryPolicy shall retrieve connection retry policy from retryPolicy in struct IOTHUB_CLIENT_LL_HANDLE_DATA**]**

**SRS_IOTHUBCLIENT_LL_25_122: [**IoTHubClient_LL_GetRetryPolicy shall retrieve retryTimeoutLimit in seconds from retryTimeoutinSeconds in struct IOTHUB_CLIENT_LL_HANDLE_DATA**]**

**SRS_IOTHUBCLIENT_LL_25_123: [**If user did not set the policy and timeout values by calling IoTHubClient_LL_SetRetryPolicy then IoTHubClient_LL_GetRetryPolicy shall return default values**]**


## IoTHubClient_LL_GetLastMessageReceiveTime

```c
extern IOTHUB_CLIENT_RESULT IoTHubClient_LL_GetLastMessageReceiveTime(IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle, time_t* lastMessageReceiveTime);
```

**SRS_IOTHUBCLIENT_LL_09_001: [** `IoTHubClient_LL_GetLastMessageReceiveTime` shall return `IOTHUB_CLIENT_INVALID_ARG` if any of the arguments is `NULL`.** ]**

**SRS_IOTHUBCLIENT_LL_09_002: [** `IoTHubClient_LL_GetLastMessageReceiveTime` shall return `IOTHUB_CLIENT_INDEFINITE_TIME` - and not set `lastMessageReceiveTime` - if it is unable to provide the time for the last commands.** ]**

**SRS_IOTHUBCLIENT_LL_09_003: [** `IoTHubClient_LL_GetLastMessageReceiveTime` shall return `IOTHUB_CLIENT_OK` if it wrote in the `lastMessageReceiveTime` the time when the last command was received.** ]**

**SRS_IOTHUBCLIENT_LL_09_004: [** `IoTHubClient_LL_GetLastMessageReceiveTime` shall return `lastMessageReceiveTime` in localtime.** ]** 



## IoTHubClient_LL_SetOption

```c
extern IOTHUB_CLIENT_RESULT IoTHubClient_LL_SetOption(IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle, const char* optionName, const void* value);
```

IoTHubClient_LL_SetOption sets the runtime option "optionName" to the value pointed to by value.

**SRS_IOTHUBCLIENT_LL_02_034: [** If `iotHubClientHandle` is `NULL` then `IoTHubClient_LL_SetOption` shall return `IOTHUB_CLIENT_INVALID_ARG`.** ]**

**SRS_IOTHUBCLIENT_LL_02_035: [** If `optionName` is `NULL` then `IoTHubClient_LL_SetOption` shall return `IOTHUB_CLIENT_INVALID_ARG`.** ]**

**SRS_IOTHUBCLIENT_LL_02_036: [** If `value` is `NULL` then `IoTHubClient_LL_SetOption` shall return `IOTHUB_CLIENT_INVALID_ARG`.** ]**  

### Options that shall be handled by IoTHubClient_LL:

-**SRS_IOTHUBCLIENT_LL_02_039: [** "messageTimeout" - once `IoTHubClient_LL_SendEventAsync` is called the message shall timeout after `*value` miliseconds. value is a pointer to a uint64.** ]**

-**SRS_IOTHUBCLIENT_LL_02_041: [** If more than \*value miliseconds have passed since the call to `IoTHubClient_LL_SendEventAsync` then the message callback shall be called with a status code of `IOTHUB_CLIENT_CONFIRMATION_TIMEOUT`.** ]**

-**SRS_IOTHUBCLIENT_LL_02_042: [** By default, messages shall not timeout.** ]**

-**SRS_IOTHUBCLIENT_LL_02_043: [** Calling `IoTHubClient_LL_SetOption` with \*value set to "0" shall disable the timeout mechanism for all new messages.** ]**

-**SRS_IOTHUBCLIENT_LL_02_044: [** Messages already delivered to `IoTHubClient_LL` shall not have their timeouts modified by a new call to `IoTHubClient_LL_SetOption`.** ]**

 **SRS_IOTHUBCLIENT_LL_02_099: [** `IoTHubClient_LL_SetOption` shall return according to the table below  ]**

- | IoTHubClient_UploadToBlob_SetOption   | Transport_SetOption       | Return value
- |---------------------------------------|---------------------------|
- | IOTHUB_CLIENT_OK                      | IOTHUB_CLIENT_OK          | IOTHUB_CLIENT_OK                                     
- | IOTHUB_CLIENT_OK                      | IOTHUB_CLIENT_ERROR       | IOTHUB_CLIENT_ERROR
- | IOTHUB_CLIENT_OK                      | IOTHUB_CLIENT_INVALID_ARG | IOTHUB_CLIENT_OK
- | IOTHUB_CLIENT_ERROR                   | ANY value                 | IOTHUB_CLIENT_ERROR                                     
- | IOTHUB_CLIENT_ERRROR                  | IOTHUB_CLIENT_ERROR       | IOTHUB_CLIENT_ERROR
- | IOTHUB_CLIENT_INVALID_ARG             | value "X"                 | "X"                                     



## IoTHubClient_LL_UploadToBlob

```c
extern IOTHUB_CLIENT_RESULT IoTHubClient_LL_UploadToBlob(IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle, const char* destinationFileName, const unsigned char* source, size_t size);
```

### `IoTHubClient_LL_UploadToBlob` calls `IoTHubClient_LL_UploadToBlob_Impl` to synchronously uploads the data pointed to by `source` having the size `size` to a blob called `destinationFileName` in Azure Blob Storage.

**SRS_IOTHUBCLIENT_LL_02_061: [** If `iotHubClientHandle` is `NULL` then `IoTHubClient_LL_UploadToBlob` shall fail and return `IOTHUB_CLIENT_INVALID_ARG`.** ]**

**SRS_IOTHUBCLIENT_LL_02_062: [** If `destinationFileName` is `NULL` then `IoTHubClient_LL_UploadToBlob` shall fail and return `IOTHUB_CLIENT_INVALID_ARG`.** ]**

**SRS_IOTHUBCLIENT_LL_02_063: [** If `source` is `NULL` and size is greater than 0 then `IoTHubClient_LL_UploadToBlob` shall fail and return `IOTHUB_CLIENT_INVALID_ARG`.** ]**

These are the 3 steps that are required to upload a file to Azure Blob Storage using IoTHub: 
step 1: get the SasUri components from IoTHub service
step 2: upload using the SasUri.
step 3: inform IoTHub that the upload has finished. 

### step 1: get the SasUri components from IoTHub service.

**SRS_IOTHUBCLIENT_LL_02_064: [** `IoTHubClient_LL_UploadToBlob` shall create an `HTTPAPIEX_HANDLE` to the IoTHub hostname.** ]**

**SRS_IOTHUBCLIENT_LL_02_065: [** If creating the `HTTPAPIEX_HANDLE` fails then `IoTHubClient_LL_UploadToBlob` shall fail and return `IOTHUB_CLIENT_ERROR`.** ]**

**SRS_IOTHUBCLIENT_LL_02_066: [** `IoTHubClient_LL_UploadToBlob` shall create an HTTP relative path formed from "/devices/" + deviceId + "/files/" + destinationFileName + "?api-version=API_VERSION".** ]**

**SRS_IOTHUBCLIENT_LL_02_067: [** If creating the relativePath fails then `IoTHubClient_LL_UploadToBlob` shall fail and return `IOTHUB_CLIENT_ERROR`.** ]**

**SRS_IOTHUBCLIENT_LL_02_068: [** `IoTHubClient_LL_UploadToBlob` shall create an HTTP responseContent BUFFER_HANDLE.** ]**

**SRS_IOTHUBCLIENT_LL_02_069: [** If creating the HTTP response buffer handle fails then `IoTHubClient_LL_UploadToBlob` shall fail and return `IOTHUB_CLIENT_ERROR`.** ]**

**SRS_IOTHUBCLIENT_LL_02_070: [** `IoTHubClient_LL_UploadToBlob` shall create request HTTP headers.** ]**

**SRS_IOTHUBCLIENT_LL_02_071: [** If creating the HTTP headers fails then `IoTHubClient_LL_UploadToBlob` shall fail and return `IOTHUB_CLIENT_ERROR`.** ]**

**SRS_IOTHUBCLIENT_LL_02_072: [** `IoTHubClient_LL_UploadToBlob` shall add the following name:value to request HTTP headers:  ]**
"Content-Type": "application/json"
"Accept": "application/json"
"User-Agent": "iothubclient/" IOTHUB_SDK_VERSION

**SRS_IOTHUBCLIENT_LL_02_073: [** If the credentials used to create `iotHubClientHandle` have "sasToken" then `IoTHubClient_LL_UploadToBlob` shall add 
the following HTTP request headers:  ]**

### "Authorization": original SAS token

**SRS_IOTHUBCLIENT_LL_02_074: [** If adding "Authorization" fails then `IoTHubClient_LL_UploadToBlob` shall fail and return `IOTHUB_CLIENT_ERROR`.  ]**

**SRS_IOTHUBCLIENT_LL_02_075: [** `IoTHubClient_LL_UploadToBlob` shall execute `HTTPAPIEX_ExecuteRequest` passing the following information for arguments:  ]**

- HTTPAPIEX_HANDLE handle - the handle created at the beginning of `IoTHubClient_LL_UploadToBlob`
- HTTPAPI_REQUEST_TYPE requestType - HTTPAPI_REQUEST_GET
- const char* relativePath - the HTTP relative path
- HTTP_HEADERS_HANDLE requestHttpHeadersHandle - request HTTP headers
- BUFFER_HANDLE requestContent - NULL
- unsigned int* statusCode - the address of an unsigned int that will contain the HTTP status code
- HTTP_HEADERS_HANDLE responseHttpHeadersHandl - NULL
- BUFFER_HANDLE responseContent - the HTTP response BUFFER_HANDLE

**SRS_IOTHUBCLIENT_LL_02_076: [** If `HTTPAPIEX_ExecuteRequest` call fails then `IoTHubClient_LL_UploadToBlob` shall fail and return `IOTHUB_CLIENT_ERROR`.** ]**

**SRS_IOTHUBCLIENT_LL_02_077: [** If HTTP statusCode is greater than or equal to 300 then `IoTHubClient_LL_UploadToBlob` shall fail and return `IOTHUB_CLIENT_ERROR`.** ]**

**SRS_IOTHUBCLIENT_LL_02_078: [** If the credentials used to create `iotHubClientHandle` have "deviceKey" then `IoTHubClient_LL_UploadToBlob` shall create an `HTTPAPIEX_SAS_HANDLE` passing as arguments:  ]**

- STRING_HANDLE key - deviceKey
- STRING_HANDLE uriResource - "/devices/" + deviceId
- STRING_HANDLE keyName - "" (empty string)

**SRS_IOTHUBCLIENT_LL_02_089: [** If creating the `HTTPAPIEX_SAS_HANDLE` fails then `IoTHubClient_LL_UploadToBlob` shall fail and return `IOTHUB_CLIENT_ERROR`.** ]**

**SRS_IOTHUBCLIENT_LL_02_090: [** `IoTHubClient_LL_UploadToBlob` shall call `HTTPAPIEX_SAS_ExecuteRequest` passing as arguments:  ]**

- HTTPAPIEX_SAS_HANDLE sasHandle - the created HTTPAPIEX_SAS_HANDLE
- HTTPAPIEX_HANDLE handle - the created HTTPAPIEX_HANDLE
- HTTPAPI_REQUEST_TYPE requestType - HTTPAPI_REQUEST_GET
- const char* relativePath - the HTTP relative path
- HTTP_HEADERS_HANDLE requestHttpHeadersHandle - request HTTP headers
- BUFFER_HANDLE requestContent - NULL
- unsigned int* statusCode - the address of an unsigned int that will contain the HTTP status code
- HTTP_HEADERS_HANDLE responseHeadersHandle - NULL
- BUFFER_HANDLE responseContent - the HTTP response BUFFER_HANDLE

**SRS_IOTHUBCLIENT_LL_02_079: [** If `HTTPAPIEX_SAS_ExecuteRequest` fails then `IoTHubClient_LL_UploadToBlob` shall fail and return `IOTHUB_CLIENT_ERROR`.** ]**

**SRS_IOTHUBCLIENT_LL_02_080: [** If status code is greater than or equal to 300 then `IoTHubClient_LL_UploadToBlob` shall fail and return `IOTHUB_CLIENT_ERROR`.** ]**

If the credentials used to create `iotHubClientHandle` do not have "deviceKey" or "deviceSasToken" then
**SRS_IOTHUBCLIENT_LL_02_106: [** - `x509certificate` and `x509privatekey` saved options shall be passed on the HTTPAPIEX_SetOption.** ]**

**SRS_IOTHUBCLIENT_LL_02_107: [** - "Authorization" header shall not be build.** ]**

**SRS_IOTHUBCLIENT_LL_02_108: [** `IoTHubClient_LL_UploadToBlob` shall execute HTTPAPIEX_ExecuteRequest passing the following information for arguments:  ]**

- HTTPAPIEX_HANDLE handle - the handle created at the beginning of `IoTHubClient_LL_UploadToBlob`
- HTTPAPI_REQUEST_TYPE requestType - HTTPAPI_REQUEST_GET
- const char* relativePath - the HTTP relative path
- HTTP_HEADERS_HANDLE requestHttpHeadersHandle - request HTTP headers
- BUFFER_HANDLE requestContent - NULL
- unsigned int* statusCode - the address of an unsigned int that will contain the HTTP status code
- HTTP_HEADERS_HANDLE responseHttpHeadersHandl - NULL
- BUFFER_HANDLE responseContent - the HTTP response BUFFER_HANDLE

**SRS_IOTHUBCLIENT_LL_02_081: [** Otherwise, `IoTHubClient_LL_UploadToBlob` shall use parson to extract and save the following information from the response buffer: correlationID and SasUri.** ]**

**SRS_IOTHUBCLIENT_LL_02_082: [** If extracting and saving the correlationId or SasUri fails then `IoTHubClient_LL_UploadToBlob` shall fail and return `IOTHUB_CLIENT_ERROR`.** ]**

### step 2: upload using the SasUri

**SRS_IOTHUBCLIENT_LL_02_083: [** `IoTHubClient_LL_UploadToBlob` shall call `Blob_UploadFromSasUri` and capture the HTTP return code and HTTP body.** ]**

**SRS_IOTHUBCLIENT_LL_02_084: [** If `Blob_UploadFromSasUri` fails then `IoTHubClient_LL_UploadToBlob` shall fail and return `IOTHUB_CLIENT_ERROR`.** ]**

### step 3: inform IoTHub that the upload has finished

**SRS_IOTHUBCLIENT_LL_02_085: [** `IoTHubClient_LL_UploadToBlob` shall use the same authorization as step 1. to prepare and perform a HTTP request with the following parameters:  ]**

- HTTPAPI_REQUEST_TYPE requestType - HTTPAPI_REQUEST_POST
- const char* relativePath - "/devices" + deviceId + "/files/notifications/" + correlationId + "?api-version" API_VERSION
- HTTP message body: a JSON encoding of an object having the following structure:

```json
{
    "statusCode": what_Blob_UploadFromSasUri_returned_as_HTTP_code,
    "statusDescription": "what_Blob_UploadFromSasUri_returned_as_HTTP_message_body"
}
```

**SRS_IOTHUBCLIENT_LL_02_091: [** If step 2 fails without establishing an HTTP dialogue, then the HTTP message body shall look like:  ]**

```json
{
    "statusCode": -1
    "statusDescription": "client not able to connect with the server"
}
```

**SRS_IOTHUBCLIENT_LL_02_086: [** If performing the HTTP request fails then `IoTHubClient_LL_UploadToBlob` shall fail and return `IOTHUB_CLIENT_ERROR`.** ]**

**SRS_IOTHUBCLIENT_LL_02_087: [** If the statusCode of the HTTP request is greater than or equal to 300 then `IoTHubClient_LL_UploadToBlob` shall fail and return `IOTHUB_CLIENT_ERROR`.** ]**

**SRS_IOTHUBCLIENT_LL_02_088: [** Otherwise, `IoTHubClient_LL_UploadToBlob` shall succeed and return `IOTHUB_CLIENT_OK`.** ]**



## IoTHubClient_LL_UploadToBlob_SetOption

```c
extern IOTHUB_CLIENT_RESULT IoTHubClient_LL_UploadToBlob_SetOption(IOTHUB_CLIENT_LL_UPLOADTOBLOB_HANDLE handle, const char* optionName, const void* value)
```

IoTHubClient_LL_UploadToBlob_SetOption sets an option for UploadToBlob.

**SRS_IOTHUBCLIENT_LL_02_110: [** If parameter `handle` is `NULL` then `IoTHubClient_LL_UploadToBlob_SetOption` shall fail and return `IOTHUB_CLIENT_ERROR`.** ]**

Handled options are

**SRS_IOTHUBCLIENT_LL_02_100: [** `x509certificate` - then `value` then is a null terminated string that contains the x509 certificate.** ]**

**SRS_IOTHUBCLIENT_LL_02_101: [** `x509privatekey` - then `value` is a null terminated string that contains the x509 privatekey.** ]**

**SRS_IOTHUBCLIENT_LL_02_102: [** If an unknown option is presented then `IoTHubClient_LL_UploadToBlob_SetOption` shall return `IOTHUB_CLIENT_INVALID_ARG`.** ]**

**SRS_IOTHUBCLIENT_LL_02_109: [** If the authentication scheme is NOT x509 then `IoTHubClient_LL_UploadToBlob_SetOption` shall return `IOTHUB_CLIENT_INVALID_ARG`.** ]**

**SRS_IOTHUBCLIENT_LL_02_103: [** The options shall be saved.** ]**

**SRS_IOTHUBCLIENT_LL_02_104: [** If saving fails, then `IoTHubClient_LL_UploadToBlob_SetOption` shall fail and return `IOTHUB_CLIENT_ERROR`.** ]**

**SRS_IOTHUBCLIENT_LL_02_105: [** Otherwise `IoTHubClient_LL_UploadToBlob_SetOption` shall succeed and return `IOTHUB_CLIENT_OK`.** ]**



## IoTHubClient_LL_SetDeviceTwinCallback

```c
extern IOTHUB_CLIENT_RESULT IoTHubClient_LL_SetDeviceTwinCallback(IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle, IOTHUB_CLIENT_DEVICE_TWIN_CALLBACK deviceTwinCallback, void* userContextCallback);
```

**SRS_IOTHUBCLIENT_LL_10_001: [** `IoTHubClient_LL_SetDeviceTwinCallback` shall fail and return `IOTHUB_CLIENT_INVALID_ARG` if parameter `iotHubClientHandle` is `NULL`.** ]**

**SRS_IOTHUBCLIENT_LL_10_002: [** If `deviceTwinCallback` is not `NULL`, then `IoTHubClient_LL_SetDeviceTwinCallback` shall call the underlying layer's `_Subscribe` function.** ]**

**SRS_IOTHUBCLIENT_LL_10_003: [** If the underlying layer's `_Subscribe` function fails, then `IoTHubClient_LL_SetDeviceTwinCallback` shall fail and return `IOTHUB_CLIENT_ERROR`.** ]**

**SRS_IOTHUBCLIENT_LL_10_005: [** Otherwise `IoTHubClient_LL_SetDeviceTwinCallback` shall succeed and return `IOTHUB_CLIENT_OK`.** ]**

**SRS_IOTHUBCLIENT_LL_10_006: [** If `deviceTwinCallback` is `NULL`, then `IoTHubClient_LL_SetDeviceTwinCallback` shall call the underlying layer's `_Unsubscribe` function and return `IOTHUB_CLIENT_OK`.** ]**



## IoTHubClient_LL_SendReportedState

```c
extern IOTHUB_CLIENT_RESULT IoTHubClient_LL_SendReportedState(IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle, const unsigned char* reportedState, size_t size, IOTHUB_CLIENT_REPORTED_STATE_CALLBACK reportedStateCallback, void* userContextCallback);
```

**SRS_IOTHUBCLIENT_LL_10_012: [** `IoTHubClient_LL_SendReportedState` shall fail and return `IOTHUB_CLIENT_INVALID_ARG` if parameter `iotHubClientHandle` is `NULL`.** ]**

**SRS_IOTHUBCLIENT_LL_10_013: [** `IoTHubClient_LL_SendReportedState` shall fail and return `IOTHUB_CLIENT_INVALID_ARG` if parameter `reportedState` is `NULL`]**

**SRS_IOTHUBCLIENT_LL_07_005: [** `IoTHubClient_LL_SendReportedState` shall fail and return `IOTHUB_CLIENT_INVALID_ARG` if parameter `size` is equal to 0.** ]**

**SRS_IOTHUBCLIENT_LL_10_014: [** `IoTHubClient_LL_SendReportedState` shall construct a Device_Twin structure containing reportedState data.** ]**

**SRS_IOTHUBCLIENT_LL_07_001: [** `IoTHubClient_LL_SendReportedState` shall queue the constructed reportedState data to be consumed by the targeted transport.** ]**

**SRS_IOTHUBCLIENT_LL_10_015: [** If any error is encountered `IoTHubClient_LL_SendReportedState` shall return `IOTHUB_CLIENT_ERROR`.** ]**

**SRS_IOTHUBCLIENT_LL_10_016: [** Otherwise `IoTHubClient_LL_SendReportedState` shall succeed and return `IOTHUB_CLIENT_OK`.** ]**

**SRS_IOTHUBCLIENT_LL_10_017: [** If parameter `reportedStateCallback` is `NULL`, `IoTHubClient_LL_SendReportedState` shall send the reported state without any notification upon the message reaching the iothub.** ]**



## IoTHubClient_LL_ReportedStateComplete

```c
void IoTHubClient_LL_ReportedStateComplete(IOTHUB_CLIENT_LL_HANDLE handle, uint32_t item_id, int status_code)
```

IoTHubClient_LL_ReportedStateComplete is a function only called by the transport that signals the completed transmission of a device_twin message to iothub, or an error.  

**SRS_IOTHUBCLIENT_LL_07_002: [** If handle is `NULL` then `IoTHubClient_LL_ReportedStateComplete` shall do nothing.** ]**

**SRS_IOTHUBCLIENT_LL_07_003: [** `IoTHubClient_LL_ReportedStateComplete` shall enumerate through the `IOTHUB_QUEUE_DATA_ITEM` structures in queue_handle.** ]**

**SRS_IOTHUBCLIENT_LL_07_004: [** If the `IOTHUB_QUEUE_DATA_ITEM`'s `reported_state_callback` variable is non-`NULL` then `IoTHubClient_LL_ReportedStateComplete` shall call the function.** ]**  

**SRS_IOTHUBCLIENT_LL_07_009: [** `IoTHubClient_LL_ReportedStateComplete` shall remove the `IOTHUB_QUEUE_DATA_ITEM` item from the ack queue.]** 

## IoTHubClient_LL_RetrievePropertyComplete

```c
void IoTHubClient_LL_RetrievePropertyComplete(IOTHUB_CLIENT_LL_HANDLE handle, DEVICE_TWIN_UPDATE_STATE update_state, const unsigned char* payLoad, size_t size)
```

**SRS_IOTHUBCLIENT_LL_07_013: [** If `handle` is `NULL` then `IoTHubClient_LL_RetrievePropertyComplete` shall return.**]**

**SRS_IOTHUBCLIENT_LL_07_014: [** If `deviceTwinCallback` is `NULL` then `IoTHubClient_LL_RetrievePropertyComplete` shall return.**]**

**SRS_IOTHUBCLIENT_LL_07_015: [** If the the `update_state` parameter is `DEVICE_TWIN_UPDATE_PARTIAL` and a `DEVICE_TWIN_UPDATE_COMPLETE` message has not been previously received then `IoTHubClient_LL_RetrievePropertyComplete` shall return.**]** 

**SRS_IOTHUBCLIENT_LL_07_016: [** If `deviceTwinCallback` is set and `DEVICE_TWIN_UPDATE_COMPLETE` has been encountered then `IoTHubClient_LL_RetrievePropertyComplete` shall call `deviceTwinCallback`.**]**

## IoTHubClient_LL_SetDeviceMethodCallback

```c
extern IOTHUB_CLIENT_RESULT IoTHubClient_LL_SetDeviceMethodCallback(IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle, IOTHUB_CLIENT_METHOD_CALLBACK_ASYNC deviceMethodCallback, void* userContextCallback)
```

**SRS_IOTHUBCLIENT_LL_12_017: [** `IoTHubClient_LL_SetDeviceMethodCallback` shall fail and return `IOTHUB_CLIENT_INVALID_ARG` if parameter `iotHubClientHandle` is `NULL`. **]**

**SRS_IOTHUBCLIENT_LL_12_018: [** If `deviceMethodCallback` is `NULL`, then `IoTHubClient_LL_SetDeviceMethodCallback` shall call the underlying layer's `IoTHubTransport_Unsubscribe_DeviceMethod` function and return `IOTHUB_CLIENT_OK`. **]**

**SRS_IOTHUBCLIENT_LL_12_019: [** If `deviceMethodCallback` is not `NULL`, then `IoTHubClient_LL_SetDeviceMethodCallback` shall call the underlying layer's `IoTHubTransport_Subscribe_DeviceMethod` function. **]**

**SRS_IOTHUBCLIENT_LL_12_020: [** If the underlying layer's `IoTHubTransport_Subscribe_DeviceMethod` function fails, then `IoTHubClient_LL_SetDeviceMethodCallback` shall fail and return `IOTHUB_CLIENT_ERROR`. **]**

**SRS_IOTHUBCLIENT_LL_12_021: [** If adding the information fails for any reason, `IoTHubClient_LL_SetDeviceMethodCallback` shall fail and return `IOTHUB_CLIENT_ERROR`. **]**

**SRS_IOTHUBCLIENT_LL_12_022: [** Otherwise `IoTHubClient_LL_SetDeviceMethodCallback` shall succeed and return `IOTHUB_CLIENT_OK`. **]**

## IoTHubClient_LL_DeviceMethodComplete

```c
int IoTHubClient_LL_DeviceMethodComplete(IOTHUB_CLIENT_LL_HANDLE handle, const unsigned char* payLoad, size_t size, BUFFER_HANDLE response)
```

**SRS_IOTHUBCLIENT_LL_07_017: [** If `handle` or response is NULL then `IoTHubClient_LL_DeviceMethodComplete` shall return 500. **]**

**SRS_IOTHUBCLIENT_LL_07_018: [** If `deviceMethodCallback` is not NULL `IoTHubClient_LL_DeviceMethodComplete` shall execute `deviceMethodCallback` and return the status. **]**

**SRS_IOTHUBCLIENT_LL_07_019: [** If `deviceMethodCallback` is NULL `IoTHubClient_LL_DeviceMethodComplete` shall return 404. **]**

**SRS_IOTHUBCLIENT_LL_07_020: [** `deviceMethodCallback` shall buil the BUFFER_HANDLE with the response payload from the `IOTHUB_CLIENT_DEVICE_METHOD_CALLBACK_ASYNC` callback. **]**
