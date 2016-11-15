# IoTHubMessaging_LL Requirements

## Overview

The IoTHubMessaging module provides the user the ability to send messages to a device and receive feedback messages from all devices in the IotHub.

## References

([IoT Hub SDK.doc](https://microsoft.sharepoint.com/teams/Azure_IoT/_layouts/15/WopiFrame.aspx?sourcedoc={9A552E4B-EC00-408F-AE9A-D8C2C37E904F}&file=IoT%20Hub%20SDK.docx&action=default))

## Exposed API
```c
#define IOTHUB_FEEDBACK_STATUS_CODE_VALUES               \
    IOTHUB_FEEDBACK_STATUS_CODE_SUCCESS,                 \
    IOTHUB_FEEDBACK_STATUS_CODE_EXPIRED,                 \
    IOTHUB_FEEDBACK_STATUS_CODE_DELIVER_COUNT_EXCEEDED,  \
    IOTHUB_FEEDBACK_STATUS_CODE_REJECTED,                \
    IOTHUB_FEEDBACK_STATUS_CODE_UNKNOWN                  \

DEFINE_ENUM(IOTHUB_FEEDBACK_STATUS_CODE, IOTHUB_FEEDBACK_STATUS_CODE_VALUES);

#define IOTHUB_MESSAGE_SEND_STATE_VALUES                 \
    IOTHUB_MESSAGE_SEND_STATE_NOT_SENT,                  \
    IOTHUB_MESSAGE_SEND_STATE_SEND_IN_PROGRESS,          \
    IOTHUB_MESSAGE_SEND_STATE_SENT_OK,                   \
    IOTHUB_MESSAGE_SEND_STATE_SEND_FAILED                \

DEFINE_ENUM(IOTHUB_MESSAGE_SEND_STATE, IOTHUB_MESSAGE_SEND_STATE_VALUES);

#define IOTHUB_MESSAGING_RESULT_VALUES       \
    IOTHUB_MESSAGING_OK,                     \
    IOTHUB_MESSAGING_INVALID_ARG,            \
    IOTHUB_MESSAGING_ERROR,                  \
    IOTHUB_MESSAGING_INVALID_JSON,           \
    IOTHUB_MESSAGING_DEVICE_EXIST,           \
    IOTHUB_MESSAGING_CALLBACK_NOT_SET        \

DEFINE_ENUM(IOTHUB_MESSAGING_RESULT, IOTHUB_MESSAGING_RESULT_VALUES);

typedef struct IOTHUB_SERVICE_FEEDBACK_RECORD_TAG
{
    const char* deviceId;
    const char* correlationId;
    const char* description;
    const char* generationId;
    time_t enqueuedTimeUtc;
    IOTHUB_FEEDBACK_STATUS_CODE statusCode;
} IOTHUB_SERVICE_FEEDBACK_RECORD;

typedef struct IOTHUB_SERVICE_FEEDBACK_BATCH_TAG
{
    const char* userId;
    const char* lockToken;
    SINGLYLINKEDLIST_HANDLE feedbackRecordList;
} IOTHUB_SERVICE_FEEDBACK_BATCH;

typedef struct IOTHUB_MESSAGING_TAG* IOTHUB_MESSAGING_HANDLE;

typedef void(*IOTHUB_OPEN_COMPLETE_CALLBACK)(void);
typedef void(*IOTHUB_SEND_COMPLETE_CALLBACK)(void* context, IOTHUB_MESSAGE_HANDLE message);
typedef void(*IOTHUB_FEEDBACK_MESSAGE_RECEIVED_CALLBACK)(IOTHUB_SERVICE_FEEDBACK_BATCH* feedbackBatch);

extern IOTHUB_MESSAGING_HANDLE IoTHubMessaging_LL_Create(IOTHUB_MESSAGING_AUTH_HANDLE serviceClientHandle);
extern void IoTHubMessaging_LL_Destroy(IOTHUB_MESSAGING_HANDLE messagingHandle);

extern IOTHUB_MESSAGING_RESULT IoTHubMessaging_LL_Open(IOTHUB_MESSAGING_HANDLE messagingHandle, IOTHUB_OPEN_COMPLETE_CALLBACK openCompleteCallback, void* userContextCallback);
extern void IoTHubMessaging_LL_Close(IOTHUB_MESSAGING_HANDLE messagingHandle);

extern IOTHUB_MESSAGING_RESULT IoTHubMessaging_LL_Send(IOTHUB_MESSAGING_HANDLE messagingHandle, const char* deviceId, IOTHUB_MESSAGE_HANDLE message, IOTHUB_SEND_COMPLETE_CALLBACK sendCompleteCallback, void* userContextCallback);

extern IOTHUB_MESSAGING_RESULT IoTHubMessaging_LL_SetFeedbackMessageCallback(IOTHUB_MESSAGING_HANDLE messagingHandle, IOTHUB_FEEDBACK_MESSAGE_RECEIVED_CALLBACK feedbackMessageReceivedCallback, void* userContextCallback);

extern void IoTHubMessaging_LL_DoWork(void);
```


## IoTHubMessaging_LL_Create
```c
extern IOTHUB_MESSAGING_HANDLE IoTHubMessaging_LL_Create(IOTHUB_SERVICECLIENT_HANDLE serviceClientHandle);
```
**SRS_IOTHUBMESSAGING_12_001: [** If the serviceClientHandle input parameter is NULL IoTHubMessaging_LL_Create shall return NULL **]**

**SRS_IOTHUBMESSAGING_12_064: [** If any member of the serviceClientHandle input parameter is NULL IoTHubMessaging_LL_Create shall return NULL **]**

**SRS_IOTHUBMESSAGING_12_002: [** IoTHubMessaging_LL_Create shall allocate memory for a new messaging instance **]**

**SRS_IOTHUBMESSAGING_12_003: [** If the allocation failed, IoTHubMessaging_LL_Create shall return NULL **]**

**SRS_IOTHUBMESSAGING_12_004: [** If the allocation and creation is successful, IoTHubMessaging_LL_Create shall return with the messaging instance, a non-NULL value **]**

**SRS_IOTHUBMESSAGING_12_065: [** IoTHubMessaging_LL_Create shall allocate memory and copy hostName to result->hostName by calling mallocAndStrcpy_s **]**

**SRS_IOTHUBMESSAGING_12_066: [** If the mallocAndStrcpy_s fails, IoTHubMessaging_LL_Create shall do clean up and return NULL **]**

**SRS_IOTHUBMESSAGING_12_067: [** IoTHubMessaging_LL_Create shall allocate memory and copy iothubName to result->iothubName by calling mallocAndStrcpy_s **]**

**SRS_IOTHUBMESSAGING_12_068: [** If the mallocAndStrcpy_s fails, IoTHubMessaging_LL_Create shall do clean up and return NULL **]**

**SRS_IOTHUBMESSAGING_12_069: [** IoTHubMessaging_LL_Create shall allocate memory and copy iothubSuffix to result->iothubSuffix by calling mallocAndStrcpy_s **]**

**SRS_IOTHUBMESSAGING_12_070: [** If the mallocAndStrcpy_s fails, IoTHubMessaging_LL_Create shall do clean up and return NULL **]**

**SRS_IOTHUBMESSAGING_12_071: [** IoTHubMessaging_LL_Create shall allocate memory and copy sharedAccessKey to result->sharedAccessKey by calling mallocAndStrcpy_s **]**

**SRS_IOTHUBMESSAGING_12_072: [** If the mallocAndStrcpy_s fails, IoTHubMessaging_LL_Create shall do clean up and return NULL **]**

**SRS_IOTHUBMESSAGING_12_073: [** IoTHubMessaging_LL_Create shall allocate memory and copy keyName to result->keyName by calling mallocAndStrcpy_s **]**

**SRS_IOTHUBMESSAGING_12_075: [** IoTHubMessaging_LL_Create shall set messaging isOpened flag to false **]**

**SRS_IOTHUBMESSAGING_12_074: [** If the mallocAndStrcpy_s fails, IoTHubMessaging_LL_Create shall do clean up and return NULL **]**

**SRS_IOTHUBMESSAGING_12_076: [** If create is successfull IoTHubMessaging_LL_Create shall save the callback data return the valid messaging handle **]**

## IoTHubMessaging_LL_Destroy
```c
extern void IoTHubMessaging_LL_Destroy(IOTHUB_MESSAGING_HANDLE messagingHandle);
```
**SRS_IOTHUBMESSAGING_12_005: [** If the messagingHandle input parameter is NULL IoTHubMessaging_LL_Destroy shall return **]**

**SRS_IOTHUBMESSAGING_12_006: [** If the messagingHandle input parameter is not NULL IoTHubMessaging_LL_Destroy shall free all resources (memory) allocated by IoTHubMessaging_LL_Create **]**


## IoTHubMessaging_LL_Open
```c
extern IOTHUB_MESSAGING_RESULT IoTHubMessaging_LL_Open(IOTHUB_MESSAGING_HANDLE messagingHandle, IOTHUB_OPEN_COMPLETE_CALLBACK openCompleteCallback, void* userContextCallback);
```
**SRS_IOTHUBMESSAGING_12_007: [** If the messagingHandle input parameter is NULL IoTHubMessaging_LL_Open shall return IOTHUB_MESSAGING_INVALID_ARG **]**

**SRS_IOTHUBMESSAGING_12_077: [** If the messagingHandle->hostname input parameter is NULL IoTHubMessaging_LL_Open shall return IOTHUB_MESSAGING_INVALID_ERROR **]**

**SRS_IOTHUBMESSAGING_12_008: [** If messaging is already opened IoTHubMessaging_LL_Open return shall IOTHUB_MESSAGING_OK **]**

**SRS_IOTHUBMESSAGING_12_009: [** IoTHubMessaging_LL_Open shall get uAMQP SASL PLAIN interface by calling saslplain_get_interface **]**

**SRS_IOTHUBMESSAGING_12_010: [** IoTHubMessaging_LL_Open shall create uAMQP PLAIN SASL mechanism by calling saslmechanism_create with the sasl plain interface **]**

**SRS_IOTHUBMESSAGING_12_011: [** IoTHubMessaging_LL_Open shall create uAMQP TLSIO by calling the xio_create **]**

**SRS_IOTHUBMESSAGING_12_012: [** IoTHubMessaging_LL_Open shall create uAMQP SASL IO by calling the xio_create with the previously created SASL mechanism and TLSIO**]**

**SRS_IOTHUBMESSAGING_12_013: [** IoTHubMessaging_LL_Open shall create uAMQP connection by calling the connection_create with the previously created SASL IO **]**

**SRS_IOTHUBMESSAGING_12_014: [** IoTHubMessaging_LL_Open shall create uAMQP session by calling the session_create **]**

**SRS_IOTHUBMESSAGING_12_015: [** IoTHubMessaging_LL_Open shall set the AMQP incoming window to UINT32 maximum value by calling session_set_incoming_window **]**

**SRS_IOTHUBMESSAGING_12_016: [** IoTHubMessaging_LL_Open shall set the AMQP outgoing window to UINT32 maximum value by calling session_set_outgoing_window **]**

**SRS_IOTHUBMESSAGING_12_018: [** IoTHubMessaging_LL_Open shall create uAMQP sender link by calling the link_create **]**

**SRS_IOTHUBMESSAGING_06_001: [** IoTHubMessaging_LL_Open shall add the version property to the sender link by calling the link_set_attach_properties **]**

**SRS_IOTHUBMESSAGING_12_019: [** IoTHubMessaging_LL_Open shall set the AMQP sender link settle mode to sender_settle_mode_unsettled  by calling link_set_snd_settle_mode **]**

**SRS_IOTHUBMESSAGING_12_021: [** IoTHubMessaging_LL_Open shall create uAMQP messaging source for sender by calling the messaging_create_source **]**

**SRS_IOTHUBMESSAGING_12_022: [** IoTHubMessaging_LL_Open shall create uAMQP messaging target for sender by calling the messaging_create_target **]**

**SRS_IOTHUBMESSAGING_12_023: [** IoTHubMessaging_LL_Open shall create uAMQP message sender by calling the messagesender_create with the created sender link and the local IoTHubMessaging_LL_SenderStateChanged callback **]**

**SRS_IOTHUBMESSAGING_12_024: [** IoTHubMessaging_LL_Open shall create uAMQP receiver link by calling the link_create **]**

**SRS_IOTHUBMESSAGING_06_002: [** IoTHubMessaging_LL_Open shall add the version property to the receiver by calling the link_set_attach_properties **]**

**SRS_IOTHUBMESSAGING_12_025: [** IoTHubMessaging_LL_Open shall set the AMQP receiver link settle mode to receiver_settle_mode_first by calling link_set_rcv_settle_mode **]**

**SRS_IOTHUBMESSAGING_12_027: [** IoTHubMessaging_LL_Open shall create uAMQP messaging source for receiver by calling the messaging_create_source **]**

**SRS_IOTHUBMESSAGING_12_028: [** IoTHubMessaging_LL_Open shall create uAMQP messaging target for receiver by calling the messaging_create_target **]**

**SRS_IOTHUBMESSAGING_12_029: [** IoTHubMessaging_LL_Open shall create uAMQP message receiver by calling the messagereceiver_create with the created sender link and the local IoTHubMessaging_LL_ReceiverStateChanged callback **]**

**SRS_IOTHUBMESSAGING_12_030: [** If any of the uAMQP call fails IoTHubMessaging_LL_Open shall return IOTHUB_MESSAGING_ERROR **]**

**SRS_IOTHUBMESSAGING_12_031: [** If all of the uAMQP call return 0 (success) IoTHubMessaging_LL_Open shall return IOTHUB_MESSAGING_OK **]**


## IoTHubMessaging_LL_Close
```c
extern void IoTHubMessaging_LL_Close(IOTHUB_MESSAGING_HANDLE messagingHandle);
```
**SRS_IOTHUBMESSAGING_12_032: [** If the messagingHandle input parameter is NULL IoTHubMessaging_LL_Close shall return IOTHUB_MESSAGING_INVALID_ARG **]**

**SRS_IOTHUBMESSAGING_12_033: [** IoTHubMessaging_LL_Close destroy the AMQP transportconnection by calling link_destroy, session_destroy, connection_destroy, xio_destroy, saslmechanism_destroy **]**



## IoTHubMessaging_LL_Send
```c
extern IOTHUB_MESSAGING_RESULT IoTHubMessaging_LL_Send(IOTHUB_MESSAGING_HANDLE messagingHandle, const char* deviceId, IOTHUB_MESSAGE_HANDLE message, IOTHUB_SEND_COMPLETE_CALLBACK sendCompleteCallback, void* userContextCallback);
```
**SRS_IOTHUBMESSAGING_12_034: [** IoTHubMessaging_LL_SendMessage shall verify the messagingHandle, deviceId, message input parameters and if any of them are NULL then return NULL **]**

**SRS_IOTHUBMESSAGING_12_035: [** IoTHubMessaging_LL_SendMessage shall verify if the AMQP messaging has been established by a successfull call to _Open and if it is not then return IOTHUB_MESSAGING_ERROR **]**

**SRS_IOTHUBMESSAGING_12_036: [** IoTHubMessaging_LL_SendMessage shall create a uAMQP message by calling message_create **]**

**SRS_IOTHUBMESSAGING_12_037: [** IoTHubMessaging_LL_SendMessage shall set the uAMQP message body to the given message content by calling message_add_body_amqp_data **]**

**SRS_IOTHUBMESSAGING_12_038: [** IoTHubMessaging_LL_SendMessage shall set the uAMQP message properties to the given message properties by calling message_set_properties **]**

**SRS_IOTHUBMESSAGING_12_039: [** IoTHubMessaging_LL_SendMessage shall call uAMQP messagesender_send with the created message with IoTHubMessaging_LL_SendMessageComplete callback by which IoTHubMessaging is notified of completition of send **]**

**SRS_IOTHUBMESSAGING_12_040: [** If any of the uAMQP call fails IoTHubMessaging_LL_SendMessage shall return IOTHUB_MESSAGING_ERROR **]**

**SRS_IOTHUBMESSAGING_12_041: [** If all uAMQP call return 0 then IoTHubMessaging_LL_SendMessage shall return IOTHUB_MESSAGING_OK  **]**



## IoTHubMessaging_LL_SetFeedbackMessageCallback
```c
extern IOTHUB_MESSAGING_RESULT IoTHubMessaging_LL_SetFeedbackMessageCallback(IOTHUB_MESSAGING_HANDLE messagingHandle, IOTHUB_FEEDBACK_MESSAGE_RECEIVED_CALLBACK feedbackMessageReceivedCallback, void* userContextCallback);
```
**SRS_IOTHUBMESSAGING_12_042: [** IoTHubMessaging_LL_SetCallbacks shall verify the messagingHandle input parameter and if it is NULL then return NULL **]**

**SRS_IOTHUBMESSAGING_12_043: [** IoTHubMessaging_LL_SetCallbacks shall save the given feedbackMessageReceivedCallback to use them in local callbacks **]**

**SRS_IOTHUBMESSAGING_12_044: [** IoTHubMessaging_LL_Open shall return IOTHUB_MESSAGING_OK after the callbacks have been set **]**



## IoTHubMessaging_LL_DoWork
```c
extern void IoTHubMessaging_LL_DoWork();
```
**SRS_IOTHUBMESSAGING_12_045: [** IoTHubMessaging_LL_DoWork shall verify if uAMQP transport has been initialized and if it is not then return immediately **]**

**SRS_IOTHUBMESSAGING_12_046: [** IoTHubMessaging_LL_DoWork shall call uAMQP connection_dowork **]**

**SRS_IOTHUBMESSAGING_12_047: [** IoTHubMessaging_LL_SendMessageComplete callback given to messagesender_send will be called with MESSAGE_SEND_RESULT **]**

**SRS_IOTHUBMESSAGING_12_048: [** If message has been received the IoTHubMessaging_LL_FeedbackMessageReceived callback given to messagesender_receive will be called with the received MESSAGE_HANDLE **]**


## IoTHubMessaging_LL_SenderStateChanged
```c
static void IoTHubMessaging_LL_SenderStateChanged(void* context, MESSAGE_SENDER_STATE new_state, MESSAGE_SENDER_STATE previous_state);
```
**SRS_IOTHUBMESSAGING_12_049: [** IoTHubMessaging_LL_SenderStateChanged shall save the new_state to local variable **]**

**SRS_IOTHUBMESSAGING_12_050: [** If both sender and receiver state is open IoTHubMessaging_LL_SenderStateChanged shall set the isOpened local variable to true **]**

**SRS_IOTHUBMESSAGING_12_051: [** If neither sender_state nor receiver_state is open IoTHubMessaging_LL_SenderStateChanged shall set the local isOpened variable to false **]**



## IoTHubMessaging_LL_ReceiverStateChanged
```c
static void IoTHubMessaging_LL_ReceiverStateChanged(const void* context, MESSAGE_RECEIVER_STATE new_state, MESSAGE_RECEIVER_STATE previous_state);
```
**SRS_IOTHUBMESSAGING_12_052: [** IoTHubMessaging_LL_ReceiverStateChanged shall save the new_state to local variable **]**

**SRS_IOTHUBMESSAGING_12_053: [** If both sender and receiver state is open IoTHubMessaging_LL_ReceiverStateChanged shall set the isOpened local variable to true **]**

**SRS_IOTHUBMESSAGING_12_054: [** If neither sender_state nor receiver_state is open IoTHubMessaging_LL_ReceiverStateChanged shall set the local isOpened variable to false **]**



## IoTHubMessaging_LL_SendMessageComplete
```c
static void IoTHubMessaging_LL_SendMessageComplete(void* context, MESSAGE_SEND_RESULT send_result);
```
**SRS_IOTHUBMESSAGING_12_055: [** If context is not NULL and IoTHubMessaging_LL_SendMessageComplete shall call user callback with user context and messaging result **]**

**SRS_IOTHUBMESSAGING_12_056: [** If context is NULL IoTHubMessaging_LL_SendMessageComplete shall return **]**


## IoTHubMessaging_LL_FeedbackMessageReceived
```c
static AMQP_VALUE IoTHubMessaging_LL_FeedbackMessageReceived(const void* context, MESSAGE_HANDLE message);
```
**SRS_IOTHUBMESSAGING_12_057: [** If context is NULL IoTHubMessaging_LL_FeedbackMessageReceived shall do nothing and return delivery_accepted **]**

**SRS_IOTHUBMESSAGING_12_058: [** If context is not NULL IoTHubMessaging_LL_FeedbackMessageReceived shall get the content string of the message by calling message_get_body_amqp_data **]**

**SRS_IOTHUBMESSAGING_12_059: [** IoTHubMessaging_LL_FeedbackMessageReceived shall parse the response JSON to IOTHUB_SERVICE_FEEDBACK_BATCH struct **]**

**SRS_IOTHUBMESSAGING_12_060: [** IoTHubMessaging_LL_FeedbackMessageReceived shall use the following parson APIs to parse the response string: json_parse_string, json_value_get_object, json_object_get_string, json_object_dotget_string  **]**

**SRS_IOTHUBMESSAGING_12_061: [** If any of the parson API fails, IoTHubMessaging_LL_FeedbackMessageReceived shall return IOTHUB_MESSAGING_INVALID_JSON **]**

**SRS_IOTHUBMESSAGING_12_062: [** If context is not NULL IoTHubMessaging_LL_FeedbackMessageReceived shall call IOTHUB_FEEDBACK_MESSAGE_RECEIVED_CALLBACK with the received IOTHUB_SERVICE_FEEDBACK_BATCH **]**

**SRS_IOTHUBMESSAGING_12_078: [** IoTHubMessaging_LL_FeedbackMessageReceived shall do clean up before exits **]**