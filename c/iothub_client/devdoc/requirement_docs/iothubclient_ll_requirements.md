#IoTHubClient_LL Requirements

##Overview
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

#define IOTHUBMESSAGE_DISPOSITION_RESULT_VALUES \
    IOTHUBMESSAGE_ACCEPTED, \
    IOTHUBMESSAGE_REJECTED, \
    IOTHUBMESSAGE_ABANDONED
 
DEFINE_ENUM(IOTHUBMESSAGE_DISPOSITION_RESULT, IOTHUBMESSAGE_DISPOSITION_RESULT_VALUES);

typedef void* IOTHUB_CLIENT_HANDLE;
typedef void(*IOTHUB_CLIENT_EVENT_CONFIRMATION_CALLBACK)(IOTHUB_CLIENT_HANDLE iotHubClientHandle, IOTHUB_CLIENT_CONFIRMATION_RESULT result, void* userContextCallback);
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


extern IOTHUB_CLIENT_HANDLE IoTHubClient_LL_CreateFromConnectionString(const char* connectionString, IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol);
  
extern IOTHUB_CLIENT_HANDLE IoTHubClient_LL_Create(const IOTHUB_CLIENT_CONFIG* config);
extern  IOTHUB_CLIENT_LL_HANDLE IoTHubClient_LL_CreateWithTransport(IOTHUB_CLIENT_DEVICE_CONFIG * config);

extern void IoTHubClient_LL_Destroy(IOTHUB_CLIENT_HANDLE iotHubClientHandle);
 
extern IOTHUB_CLIENT_RESULT IoTHubClient_LL_SendEventAsync(IOTHUB_CLIENT_HANDLE iotHubClientHandle, IOTHUB_MESSAGE_HANDLE eventMessageHandle, IOTHUB_CLIENT_EVENT_CONFIRMATION_CALLBACK eventConfirmationCallback, void* userContextCallback);
extern void IoTHubClient_LL_DoWork(IOTHUB_CLIENT_HANDLE iotHubClientHandle);
extern IOTHUB_CLIENT_RESULT IoTHubClient_LL_SetMessageCallback(IOTHUB_CLIENT_HANDLE iotHubClientHandle, IOTHUB_CLIENT_MESSAGE_CALLBACK_ASYNC messageCallback, void* userContextCallback);
extern IOTHUB_CLIENT_RESULT IoTHubClient_LL_GetSendStatus(IOTHUB_CLIENT_HANDLE iotHubClientHandle, IOTHUB_CLIENT_STATUS *iotHubClientStatus);
extern IOTHUB_CLIENT_RESULT IoTHubClient_LL_GetLastMessageReceiveTime(IOTHUB_CLIENT_HANDLE iotHubClientHandle, time_t* lastMessageReceiveTime);
extern IOTHUB_CLIENT_RESULT IoTHubClient_LL_SetOption(IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle, const char* optionName, const void* value);
```

###IoTHubClient_LL_CreateFromConnectionString
```c
extern IOTHUB_CLIENT_HANDLE IoTHubClient_LL_CreateFromConnectionString(const char* connectionString, IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol);
```
**SRS_IOTHUBCLIENT_LL_05_001: [**IoTHubClient_LL_CreateFromConnectionString shall obtain the version string by a call to IoTHubClient_GetVersionString.**]**  
**SRS_IOTHUBCLIENT_LL_05_002: [**IoTHubClient_LL_CreateFromConnectionString shall print the version string to standard output.**]**
**SRS_IOTHUBCLIENT_LL_12_003: [**IoTHubClient_LL_CreateFromConnectionString shall verify the input parameters and if any of them NULL then return NULL.**]** 
**SRS_IOTHUBCLIENT_LL_12_004: [**IoTHubClient_LL_CreateFromConnectionString shall allocate IOTHUB_CLIENT_CONFIG structure.**]** 
**SRS_IOTHUBCLIENT_LL_12_012: [**If the allocation failed IoTHubClient_LL_CreateFromConnectionString  returns NULL.**]**  
**SRS_IOTHUBCLIENT_LL_12_005: [**IoTHubClient_LL_CreateFromConnectionString shall try to parse the connectionString input parameter for the following structure: "Key1=value1;key2=value2;key3=value3..."**]** 
**SRS_IOTHUBCLIENT_LL_12_013: [**If the parsing failed IoTHubClient_LL_CreateFromConnectionString returns NULL**]**  
**SRS_IOTHUBCLIENT_LL_12_006: [**IoTHubClient_LL_CreateFromConnectionString shall verify the existence of the following Key/Value pairs in the connection string: HostName, DeviceId, SharedAccessKey or SharedAccessSignature.**]**  
**SRS_IOTHUBCLIENT_LL_12_014: [**If either of key is missing then IoTHubClient_LL_CreateFromConnectionString returns NULL **]** 
**SRS_IOTHUBCLIENT_LL_12_009: [**IoTHubClient_LL_CreateFromConnectionString shall split the value of HostName to Name and Suffix using the first "." as a separator**]** 
**SRS_IOTHUBCLIENT_LL_12_015: [**If the string split failed, IoTHubClient_LL_CreateFromConnectionString returns NULL **]** 
**SRS_IOTHUBCLIENT_LL_12_010: [**IoTHubClient_LL_CreateFromConnectionString shall fill up the IOTHUB_CLIENT_CONFIG structure using the following mapping: iotHubName = Name, iotHubSuffix = Suffix, deviceId = DeviceId, deviceKey = SharedAccessKey or deviceSasToken = SharedAccessSignature**]** 
**SRS_IOTHUBCLIENT_LL_12_011: [**IoTHubClient_LL_CreateFromConnectionString shall call into the IoTHubClient_LL_Create API with the current structure and returns with the return value of it**]** 
**SRS_IOTHUBCLIENT_LL_12_016: [**IoTHubClient_LL_CreateFromConnectionString shall return NULL if IoTHubClient_LL_Create call fails**]** 
**SRS_IOTHUBCLIENT_LL_04_001: [**IoTHubClient_LL_CreateFromConnectionString shall verify the existence of key/value pair GatewayHostName. If it does exist it shall pass the value to IoTHubClient_LL_Create API.**]** 
**SRS_IOTHUBCLIENT_LL_04_002: [**If it does not, it shall pass the protocolGatewayHostName NULL.**]** 

###IoTHubClient_LL_Create
```c
extern IOTHUB_CLIENT_HANDLE IoTHubClient_LL_Create(const IOTHUB_CLIENT_CONFIG* config);
```
**SRS_IOTHUBCLIENT_LL_02_001: [**IoTHubClient_LL_Create shall return NULL if config parameter is NULL or protocol field is NULL.**]**
**SRS_IOTHUBCLIENT_LL_02_045: [** Otherwise IoTHubClient_LL_Create shall create a new `TICK_COUNTER_HANDLE` **]**
**SRS_IOTHUBCLIENT_LL_02_046: [** If creating the `TICK_COUNTER_HANDLE` fails then `IoTHubClient_LL_Create` shall fail and return NULL. **]**
**SRS_IOTHUBCLIENT_LL_02_004: [**Otherwise IoTHubClient_LL_Create shall initialize a new DLIST (further called "waitingToSend") containing records with fields of the following types: IOTHUB_MESSAGE_HANDLE, IOTHUB_CLIENT_EVENT_CONFIRMATION_CALLBACK, void*.**]** 
**SRS_IOTHUBCLIENT_LL_02_006: [**IoTHubClient_LL_Create shall populate a structure of type IOTHUBTRANSPORT_CONFIG with the information from config parameter and the previous DLIST and shall pass that to the underlying layer _Create function.**]** 
**SRS_IOTHUBCLIENT_LL_02_007: [**If the underlaying layer _Create function fails them IoTHubClient_LL_Create shall fail and return NULL.**]**
**SRS_IOTHUBCLIENT_LL_17_008: [**IoTHubClient_LL_Create shall call the transport _Register function with a populated structure of type IOTHUB_DEVICE_CONFIG and waitingToSend list.**]** 
**SRS_IOTHUBCLIENT_LL_17_009: [**If the _Register function fails, this function shall fail and return NULL.**]** 
**SRS_IOTHUBCLIENT_LL_02_008: [**Otherwise, IoTHubClient_LL_Create shall succeed and return a non-NULL handle.**]** 

###IoTHubClient_LL_CreateWithTransport
```c
extern  IOTHUB_CLIENT_LL_HANDLE IoTHubClient_LL_CreateWithTransport(IOTHUB_CLIENT_DEVICE_CONFIG * config);
```
**SRS_IOTHUBCLIENT_LL_17_001: [**IoTHubClient_LL_CreateWithTransport shall return NULL if config parameter is NULL, or protocol field is NULL or transportHandle is NULL.**]** 
**SRS_IOTHUBCLIENT_LL_17_002: [**IoTHubClient_LL_CreateWithTransport shall allocate data for the IOTHUB_CLIENT_LL_HANDLE.**]** 
**SRS_IOTHUBCLIENT_LL_17_003: [**If allocation fails, the function shall fail and return NULL.**]** 
**SRS_IOTHUBCLIENT_LL_02_047: [** IoTHubClient_LL_CreateWithTransport shall create a TICK_COUNTER_HANDLE. **]**
**SRS_IOTHUBCLIENT_LL_02_048: [** If creating the handle fails, then IoTHubClient_LL_CreateWithTransport shall fail and return NULL **]**
**SRS_IOTHUBCLIENT_LL_17_004: [**IoTHubClient_LL_CreateWithTransport shall initialize a new DLIST (further called "waitingToSend") containing records with fields of the following types: IOTHUB_MESSAGE_HANDLE, IOTHUB_CLIENT_EVENT_CONFIRMATION_CALLBACK, void*.**]** 
**SRS_IOTHUBCLIENT_LL_17_005: [**IoTHubClient_LL_CreateWithTransport shall save the transport handle and mark this transport as shared.**]** 
**SRS_IOTHUBCLIENT_LL_17_006: [**IoTHubClient_LL_CreateWithTransport shall call the transport _Register function with the IOTHUB_DEVICE_CONFIG populated structure and waitingToSend list.**]** 
**SRS_IOTHUBCLIENT_LL_17_007: [**If the _Register function fails, this function shall fail and return NULL.**]** 

###IoTHubClient_LL_Destroy
```c
void IoTHubClient_LL_Destroy(IOTHUB_CLIENT_HANDLE iotHubClientHandle);
```
**SRS_IOTHUBCLIENT_LL_02_009: [**IoTHubClient_LL_Destroy shall do nothing if parameter iotHubClientHandle is NULL.**]** 
**SRS_IOTHUBCLIENT_LL_02_033: [**Otherwise, IoTHubClient_LL_Destroy shall complete all the event message callbacks that are in the waitingToSend list with the result IOTHUB_CLIENT_CONFIRMATION_BECAUSE_DESTROY.**]** 
**SRS_IOTHUBCLIENT_LL_17_010: [**IoTHubClient_LL_Destroy  shall call the underlaying layer's _Unregister function**]** 
**SRS_IOTHUBCLIENT_LL_02_010: [**If iotHubClientHandle was not created by IoTHubClient_LL_CreateWithTransport, IoTHubClient_LL_Destroy  shall call the underlaying layer's _Destroy function. and shall free the resources allocated by IoTHubClient (if any).**]** 
**SRS_IOTHUBCLIENT_LL_17_011: [**IoTHubClient_LL_Destroy  shall free the resources allocated by IoTHubClient (if any).**]** 

###IoTHubClient_LL_SendEventAsync
```c 
extern IOTHUB_CLIENT_RESULT IoTHubClient_LL_SendEventAsync(IOTHUB_CLIENT_HANDLE iotHubClientHandle, IOTHUB_MESSAGE_HANDLE eventMessageHandle, IOTHUB_CLIENT_EVENT_CONFIRMATION_CALLBACK eventConfirmationCallback, void* userContextCallback);
```
**SRS_IOTHUBCLIENT_LL_02_011: [**IoTHubClient_LL_SendEventAsync shall fail and return IOTHUB_CLIENT_INVALID_ARG if parameter iotHubClientHandle or eventMessageHandle is NULL.**]** 
**SRS_IOTHUBCLIENT_LL_02_012: [**IoTHubClient_LL_SendEventAsync shall fail and return IOTHUB_CLIENT_INVALID_ARG if parameter eventConfirmationCallback is NULL and userContextCallback is not NULL.**]** 
**SRS_IOTHUBCLIENT_LL_02_013: [**IotHubClient_SendEventAsync shall add the DLIST waitingToSend a new record cloning the information from eventMessageHandle, eventConfirmationCallback, userContextCallback.**]** 
**SRS_IOTHUBCLIENT_LL_02_014: [**If cloning and/or adding the information fails for any reason, IoTHubClient_LL_SendEventAsync shall fail and return IOTHUB_CLIENT_ERROR.**]** 
**SRS_IOTHUBCLIENT_LL_02_015: [**Otherwise IoTHubClient_LL_SendEventAsync shall succeed and return IOTHUB_CLIENT_OK.**]** 

###IoTHubClient_LL_SetMessageCallback
```c
extern IOTHUB_CLIENT_RESULT IoTHubClient_LL_SetMessageCallback(IOTHUB_CLIENT_HANDLE iotHubClientHandle, IOTHUB_CLIENT_MESSAGE_CALLBACK_ASYNC messageCallback, void* userContextCallback);
```
**SRS_IOTHUBCLIENT_LL_02_016: [**IoTHubClient_LL_SetMessageCallback shall fail and return IOTHUB_CLIENT_INVALID_ARG if parameter iotHubClientHandle is NULL.**]** 
**SRS_IOTHUBCLIENT_LL_02_017: [**If parameter messageCallback is non-NULL then IoTHubClient_LL_SetMessageCallback shall call the underlying layer's _Subscribe function.**]** 
**SRS_IOTHUBCLIENT_LL_02_018: [**If the underlying layer's _Subscribe function fails, then IoTHubClient_LL_SetMessageCallback shall fail and return IOTHUB_CLIENT_ERROR. Otherwise IoTHubClient_LL_SetMessageCallback shall succeed and return IOTHUB_CLIENT_OK.**]** 
**SRS_IOTHUBCLIENT_LL_02_019: [**If parameter messageCallback is NULL then IoTHubClient_LL_SetMessageCallback shall call the underlying layer's _Unsubscribe function and return IOTHUB_CLIENT_OK.**]** 

###IoTHubClient_LL_DoWork
```c
void IoTHubClient_LL_DoWork(IOTHUB_CLIENT_HANDLE iotHubClientHandle);
```
**SRS_IOTHUBCLIENT_LL_02_020: [**If parameter iotHubClientHandle is NULL then IoTHubClient_LL_DoWork shall not perform any action.**]** 
**SRS_IOTHUBCLIENT_LL_02_021: [**Otherwise, IoTHubClient_LL_DoWork shall invoke the underlaying layer's _DoWork function.**]** 

###IoTHubClient_LL_SendComplete
```c
void IoTHubClient_LL_SendComplete(IOTHUB_CLIENT_HANDLE handle, PDLIST_ENTRY completed, IOTHUB_BATCHSTATE result)
```
**SRS_IOTHUBCLIENT_LL_02_022: [**If parameter completed is NULL or parameter handle is NULL then IoTHubClient_LL_SendComplete shall return.**]** 

IoTHubClient_LL_SendComplete is a function that is only called by the lower layers. Completed is a PDLIST containing records of the same type as those created in the IoTHubClient _Create function. Result is a parameter that indicates that the result of the sending the batch.

**SRS_IOTHUBCLIENT_LL_02_025: [**If parameter result is IOTHUB_BATCHSTATE_SUCCESS then IoTHubClient_LL_SendComplete shall call all the non-NULL callbacks with the result parameter set to IOTHUB_CLIENT_CONFIRMATION_OK and the context set to the context passed originally in the SendEventAsync call.**]** 
**SRS_IOTHUBCLIENT_LL_02_026: [**If any callback is NULL then there shall not be a callback call.**]** 
**SRS_IOTHUBCLIENT_LL_02_027: [**If parameter result is IOTHUB_BACTCHSTATE_FAILED then IoTHubClient_LL_SendComplete shall call all the non-NULL callbacks with the result parameter set to IOTHUB_CLIENT_CONFIRMATION_ERROR and the context set to the context passed originally in the SendEventAsync call.**]** 
**SRS_IOTHUBCLIENT_LL_02_028: [**If any callback is NULL then there shall not be a callback call.**]** 

###IoTHubClient_LL_MessageCallback
```c
IOTHUBMESSAGE_DISPOSITION_RESULT IoTHubClient_LL_MessageCallback(IOTHUB_CLIENT_HANDLE handle, IOTHUB_MESSAGE_HANDLE message);
```
This function is only called by the lower layers upon receiving a message from IoTHub.
**SRS_IOTHUBCLIENT_LL_02_029: [**If parameter handle is NULL then IoTHubClient_LL_MessageCallback shall return IOTHUBMESSAGE_ABANDONED.**]** 
**SRS_IOTHUBCLIENT_LL_02_030: [**IoTHubClient_LL_MessageCallback shall invoke the last callback function (the parameter messageCallback to IoTHubClient_LL_SetMessageCallback) passing the message and the passed userContextCallback.**]** 
**SRS_IOTHUBCLIENT_LL_02_031: [**Then IoTHubClient_LL_MessageCallback shall return what the user function returns.**]** 
**SRS_IOTHUBCLIENT_LL_02_032: [**If the last callback function was NULL, then IoTHubClient_LL_MessageCallback  shall return IOTHUBMESSAGE_ABANDONED.**]** 

###IoTHubClient_LL_GetSendStatus
```c
extern IOTHUB_CLIENT_RESULT IoTHubClient_LL_GetSendStatus(IOTHUB_CLIENT_HANDLE iotHubClientHandle, IOTHUB_CLIENT_STATUS *iotHubClientStatus);
```
**SRS_IOTHUBCLIENT_LL_09_007: [**IoTHubClient_LL_GetSendStatus shall return IOTHUB_CLIENT_INVALID_ARG if called with NULL parameter**]** 
**SRS_IOTHUBCLIENT_LL_09_008: [**IoTHubClient_LL_GetSendStatus shall return IOTHUB_CLIENT_OK and status IOTHUB_CLIENT_SEND_STATUS_IDLE if there is currently no items to be sent**]** 
**SRS_IOTHUBCLIENT_LL_09_009: [**IoTHubClient_LL_GetSendStatus shall return IOTHUB_CLIENT_OK and status IOTHUB_CLIENT_SEND_STATUS_BUSY if there are currently items to be sent**]** 

###IoTHubClient_LL_GetLastMessageReceiveTime
```c
extern IOTHUB_CLIENT_RESULT IoTHubClient_LL_GetLastMessageReceiveTime(IOTHUB_CLIENT_HANDLE iotHubClientHandle, time_t* lastMessageReceiveTime);
```

**SRS_IOTHUBCLIENT_LL_09_001: [**IoTHubClient_LL_GetLastMessageReceiveTime shall return IOTHUB_CLIENT_INVALID_ARG if any of the arguments is NULL**]** 
**SRS_IOTHUBCLIENT_LL_09_002: [**IoTHubClient_LL_GetLastMessageReceiveTime shall return IOTHUB_CLIENT_INDEFINITE_TIME - and not set ‘lastMessageReceiveTime’ - if it is unable to provide the time for the last commands**]**
**SRS_IOTHUBCLIENT_LL_09_003: [**IoTHubClient_LL_GetLastMessageReceiveTime shall return IOTHUB_CLIENT_OK if it wrote in the lastMessageReceiveTime the time when the last command was received**]**
**SRS_IOTHUBCLIENT_LL_09_004: [**IoTHubClient_LL_GetLastMessageReceiveTime shall return lastMessageReceiveTime in localtime**]** 


###IoTHubClient_LL_SetOption
```c
extern IOTHUB_CLIENT_RESULT IoTHubClient_LL_SetOption(IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle, const char* optionName, const void* value);
```
IoTHubClient_LL_SetOption sets the runtime option "optionName" to the value pointed to by value.
**SRS_IOTHUBCLIENT_LL_02_034: [**If iotHubClientHandle is NULL then IoTHubClient_LL_SetOption shall return IOTHUB_CLIENT_INVALID_ARG.**]** 
**SRS_IOTHUBCLIENT_LL_02_035: [**If optionName is NULL then IoTHubClient_LL_SetOption shall return IOTHUB_CLIENT_INVALID_ARG.**]** 
**SRS_IOTHUBCLIENT_LL_02_036: [**If value is NULL then IoTHubClient_LL_SetOption shall return IOTHUB_CLIENT_INVALID_ARG.**]** 
**SRS_IOTHUBCLIENT_LL_02_037: [**If optionName is an option that is handled by IoTHubClient_LL then it shall be set.**]** 
**SRS_IOTHUBCLIENT_LL_02_038: [**Otherwise, IoTHubClient_LL shall call the function _SetOption of the underlying transport and return what that function is returning.**]** 

Options currently handled by IoTHubClient_LL: 
-	**SRS_IOTHUBCLIENT_LL_02_039: [** "messageTimeout" - once `IoTHubClient_LL_SendEventAsync` is called the message shall timeout after `*value` miliseconds. value is a pointer to a uint64. **]**
    **SRS_IOTHUBCLIENT_LL_02_041: [** If more than *value miliseconds have passed since the call to `IoTHubClient_LL_SendEventAsync` then the message callback shall be called with a status code of IOTHUB_CLIENT_CONFIRMATION_TIMEOUT. **]**
    **SRS_IOTHUBCLIENT_LL_02_042: [** By default, messages shall not timeout. **]** 
    **SRS_IOTHUBCLIENT_LL_02_043: [** Calling `IoTHubClient_LL_SetOption` with *value set to "0" shall disable the timeout mechanism for all new messages. **]**
    **SRS_IOTHUBCLIENT_LL_02_044: [** Messages already delivered to IoTHubClient_LL shall not have their timeouts modified by a new call to IoTHubClient_LL_SetOption. **]**

