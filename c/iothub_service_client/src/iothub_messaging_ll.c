// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include <ctype.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/tlsio.h"
#include "azure_c_shared_utility/platform.h"
#include "azure_c_shared_utility/sastoken.h"

#include "azure_uamqp_c/connection.h"
#include "azure_uamqp_c/message_receiver.h"
#include "azure_uamqp_c/message_sender.h"
#include "azure_uamqp_c/messaging.h"
#include "azure_uamqp_c/sasl_mechanism.h"
#include "azure_uamqp_c/saslclientio.h"
#include "azure_uamqp_c/sasl_plain.h"
#include "azure_uamqp_c/cbs.h"

#include "parson.h"

#include "iothub_messaging_ll.h"

typedef struct CALLBACK_DATA_TAG
{
    IOTHUB_OPEN_COMPLETE_CALLBACK openCompleteCompleteCallback;
    IOTHUB_SEND_COMPLETE_CALLBACK sendCompleteCallback;
    IOTHUB_FEEDBACK_MESSAGE_RECEIVED_CALLBACK feedbackMessageCallback;
    void* openUserContext;
    void* sendUserContext;
    void* feedbackUserContext;
} CALLBACK_DATA;

typedef struct IOTHUB_MESSAGING_TAG
{
    int isOpened;
    char* hostname;
    char* iothubName;
    char* iothubSuffix;
    char* sharedAccessKey;
    char* keyName;

    MESSAGE_SENDER_HANDLE message_sender;
    MESSAGE_RECEIVER_HANDLE message_receiver;
    CONNECTION_HANDLE connection;
    SESSION_HANDLE session;
    LINK_HANDLE sender_link;
    LINK_HANDLE receiver_link;
    SASL_MECHANISM_HANDLE sasl_mechanism_handle;
    SASL_PLAIN_CONFIG sasl_plain_config;
    XIO_HANDLE tls_io;
    XIO_HANDLE sasl_io;

    MESSAGE_SENDER_STATE message_sender_state;
    MESSAGE_RECEIVER_STATE message_receiver_state;

    CALLBACK_DATA* callback_data;
} IOTHUB_MESSAGING;

static const char* FEEDBACK_RECORD_KEY_DEVICE_ID = "deviceId";
static const char* FEEDBACK_RECORD_KEY_DEVICE_GENERATION_ID = "deviceGenerationId";
static const char* FEEDBACK_RECORD_KEY_DESCRIPTION = "description";
static const char* FEEDBACK_RECORD_KEY_ENQUED_TIME_UTC = "enqueuedTimeUtc";
static const char* FEEDBACK_RECORD_KEY_ORIGINAL_MESSAGE_ID = "originalMessageId";

static char* createSasToken(IOTHUB_MESSAGING_HANDLE messagingHandle)
{
    char* result;

    char* buffer = NULL;
    if (messagingHandle->sharedAccessKey == NULL)
    {
        LogError("createSasPlainConfig failed - sharedAccessKey cannot be NULL");
        result = NULL;
    }
    else if (messagingHandle->hostname == NULL)
    {
        LogError("createSasPlainConfig failed - hostname cannot be NULL");
        result = NULL;
    }
    else if (messagingHandle->keyName == NULL)
    {
        LogError("createSasPlainConfig failed - keyName cannot be NULL");
        result = NULL;
    }
    else
    {
        STRING_HANDLE hostName = NULL;
        STRING_HANDLE sharedAccessKey = NULL;
        STRING_HANDLE keyName = NULL;

        if ((hostName = STRING_construct(messagingHandle->hostname)) == NULL)
        {
            LogError("STRING_construct failed for hostName");
            result = NULL;
        }
        else if ((sharedAccessKey = STRING_construct(messagingHandle->sharedAccessKey)) == NULL)
        {
            LogError("STRING_construct failed for sharedAccessKey");
            result = NULL;
        }
        else if ((keyName = STRING_construct(messagingHandle->keyName)) == NULL)
        {
            LogError("STRING_construct failed for keyName");
            result = NULL;
        }
        else
        {
            time_t currentTime = time(NULL);
            size_t expiry_time = (size_t)(currentTime + (365 * 24 * 60 * 60));
            const char* c_buffer = NULL;

            STRING_HANDLE sasHandle = SASToken_Create(sharedAccessKey, hostName, keyName, expiry_time);
            if (sasHandle == NULL)
            {
                LogError("SASToken_Create failed");
                result = NULL;
            }
            else if ((c_buffer = (const char*)STRING_c_str(sasHandle)) == NULL)
            {
                LogError("STRING_c_str returned NULL");
                result = NULL;
            }
            else if (mallocAndStrcpy_s(&buffer, c_buffer) != 0)
            {
                LogError("mallocAndStrcpy_s failed for sharedAccessToken");
                result = NULL;
            }
            else
            {
                result = buffer;
            }
            STRING_delete(sasHandle);
        }
        STRING_delete(keyName);
        STRING_delete(sharedAccessKey);
        STRING_delete(hostName);
    }
    return result;
}

static char* createAuthCid(IOTHUB_MESSAGING_HANDLE messagingHandle)
{
    char* result;

    char* buffer = NULL;
    if (messagingHandle->iothubName == NULL)
    {
        LogError("createSasPlainConfig failed - iothubName cannot be NULL");
        result = NULL;
    }
    else
    {
        const char* AMQP_SEND_AUTHCID_FMT = "iothubowner@sas.root.%s";
        size_t authCidLen = strlen(AMQP_SEND_AUTHCID_FMT) + strlen(messagingHandle->iothubName);

        if ((buffer = (char*)malloc(authCidLen + 1)) == NULL)
        {
            LogError("Malloc failed for authCid.");
            result = NULL;
        }
        else if ((snprintf(buffer, authCidLen + 1, AMQP_SEND_AUTHCID_FMT, messagingHandle->iothubName)) < 0)
        {
            LogError("sprintf_s failed for authCid.");
            free(buffer);
            result = NULL;
        }
        else
        {
            result = buffer;
        }
    }
    return result;
}

static char* createReceiveTargetAddress(IOTHUB_MESSAGING_HANDLE messagingHandle)
{
    char* result;

    char* buffer = NULL;
    if (messagingHandle->hostname == NULL)
    {
        LogError("createSendTargetAddress failed - hostname cannot be NULL");
        result = NULL;
    }
    else
    {
        const char* AMQP_SEND_TARGET_ADDRESS_FMT = "amqps://%s/messages/servicebound/feedback";
        size_t addressLen = strlen(AMQP_SEND_TARGET_ADDRESS_FMT) + strlen(messagingHandle->hostname);

        if ((buffer = (char*)malloc(addressLen + 1)) == NULL)
        {
            LogError("Malloc failed for receiveTargetAddress");
            result = NULL;
        }
        else if ((snprintf(buffer, addressLen + 1, AMQP_SEND_TARGET_ADDRESS_FMT, messagingHandle->hostname)) < 0)
        {
            LogError("sprintf_s failed for receiveTargetAddress.");
            free((char*)buffer);
            result = NULL;
        }
        else
        {
            result = buffer;
        }
    }
    return result;
}

static char* createSendTargetAddress(IOTHUB_MESSAGING_HANDLE messagingHandle)
{
    char* result;

    char* buffer = NULL;
    if (messagingHandle->hostname == NULL)
    {
        /*Codes_SRS_IOTHUBMESSAGING_12_077: [ If the messagingHandle->hostname input parameter is NULL IoTHubMessaging_LL_Open shall return IOTHUB_MESSAGING_INVALID_ARG ] */
        LogError("createSendTargetAddress failed - hostname cannot be NULL");
        result = NULL;
    }
    else
    {
        const char* AMQP_SEND_TARGET_ADDRESS_FMT = "amqps://%s/messages/deviceBound";
        size_t addressLen = strlen(AMQP_SEND_TARGET_ADDRESS_FMT) + strlen(messagingHandle->hostname);

        if ((buffer = (char*)malloc(addressLen + 1)) == NULL)
        {
            LogError("Malloc failed for sendTargetAddress");
            result = NULL;
        }
        else if ((snprintf(buffer, addressLen + 1, AMQP_SEND_TARGET_ADDRESS_FMT, messagingHandle->hostname)) < 0)
        {
            LogError("sprintf_s failed for sendTargetAddress.");
            free((char*)buffer);
            result = NULL;
        }
        else
        {
            result = buffer;
        }
    }
    return result;
}

static char* createDeviceDestinationString(const char* deviceId)
{
    char* result;

    if (deviceId == NULL)
    {
        LogError("createDeviceDestinationString failed - deviceId cannot be NULL");
        result = NULL;
    }
    else
    {
        const char* AMQP_ADDRESS_PATH_FMT = "/devices/%s/messages/deviceBound";
        size_t deviceDestLen = strlen(AMQP_ADDRESS_PATH_FMT) + strlen(deviceId) + 1;

        char* buffer = (char*)malloc(deviceDestLen + 1);
        if (buffer == NULL)
        {
            LogError("Could not create device destination string.");
            result = NULL;
        }
        else if ((snprintf(buffer, deviceDestLen + 1, AMQP_ADDRESS_PATH_FMT, deviceId)) < 0)
        {
            LogError("sprintf_s failed for deviceDestinationString.");
            free((char*)buffer);
            result = NULL;
        }
        else
        {
            result = buffer;
        }
    }
    return result;
}

static void IoTHubMessaging_LL_SenderStateChanged(void* context, MESSAGE_SENDER_STATE new_state, MESSAGE_SENDER_STATE previous_state)
{
    (void)previous_state;
    if (context != NULL)
    {
        /*Codes_SRS_IOTHUBMESSAGING_12_049: [ IoTHubMessaging_LL_SenderStateChanged shall save the new_state to local variable ] */
        IOTHUB_MESSAGING* messagingData = (IOTHUB_MESSAGING*)context;
        messagingData->message_sender_state = new_state;

        if ((messagingData->message_sender_state == MESSAGE_SENDER_STATE_OPEN) && (messagingData->message_receiver_state == MESSAGE_RECEIVER_STATE_OPEN))
        {
            /*Codes_SRS_IOTHUBMESSAGING_12_050: [ If both sender and receiver state is open IoTHubMessaging_LL_SenderStateChanged shall set the isOpened local variable to true ] */
            messagingData->isOpened = true;
            if (messagingData->callback_data->openCompleteCompleteCallback != NULL)
            {
                (messagingData->callback_data->openCompleteCompleteCallback)(messagingData->callback_data->openUserContext);
            }
        }
        else
        {
            /*Codes_SRS_IOTHUBMESSAGING_12_051: [ If neither sender_state nor receiver_state is open IoTHubMessaging_LL_SenderStateChanged shall set the local isOpened variable to false ] */
            messagingData->isOpened = false;
        }
    }
}

static void IoTHubMessaging_LL_ReceiverStateChanged(const void* context, MESSAGE_RECEIVER_STATE new_state, MESSAGE_RECEIVER_STATE previous_state)
{
    (void)previous_state;
    if (context != NULL)
    {
        /*Codes_SRS_IOTHUBMESSAGING_12_052: [ IoTHubMessaging_LL_ReceiverStateChanged shall save the new_state to local variable ] */
        IOTHUB_MESSAGING* messagingData = (IOTHUB_MESSAGING*)context;
        messagingData->message_receiver_state = new_state;

        if ((messagingData->message_sender_state == MESSAGE_SENDER_STATE_OPEN) && (messagingData->message_receiver_state == MESSAGE_RECEIVER_STATE_OPEN))
        {
            /*Codes_SRS_IOTHUBMESSAGING_12_053: [ If both sender and receiver state is open IoTHubMessaging_LL_ReceiverStateChanged shall set the isOpened local variable to true ] */
            messagingData->isOpened = true;
            if (messagingData->callback_data->openCompleteCompleteCallback != NULL)
            {
                (messagingData->callback_data->openCompleteCompleteCallback)(messagingData->callback_data->openUserContext);
            }
        }
        else
        {
            /*Codes_SRS_IOTHUBMESSAGING_12_054: [ If neither sender_state nor receiver_state is open IoTHubMessaging_LL_ReceiverStateChanged shall set the local isOpened variable to false ] */
            messagingData->isOpened = false;
        }
    }
}

static void IoTHubMessaging_LL_SendMessageComplete(void* context, IOTHUB_MESSAGING_RESULT send_result)
{
    /*Codes_SRS_IOTHUBMESSAGING_12_056: [ If context is NULL IoTHubMessaging_LL_SendMessageComplete shall return ] */
    if (context != NULL)
    {
        /*Codes_SRS_IOTHUBMESSAGING_12_055: [ If context is not NULL and IoTHubMessaging_LL_SendMessageComplete shall call user callback with user context and messaging result ] */
        IOTHUB_MESSAGING* messagingData = (IOTHUB_MESSAGING*)context;
        if (messagingData->callback_data->sendCompleteCallback != NULL)
        {
            (messagingData->callback_data->sendCompleteCallback)(messagingData->callback_data->sendUserContext, send_result);
        }
    }
}

static AMQP_VALUE IoTHubMessaging_LL_FeedbackMessageReceived(const void* context, MESSAGE_HANDLE message)
{
    AMQP_VALUE result;

    /*Codes_SRS_IOTHUBMESSAGING_12_057: [ If context is NULL IoTHubMessaging_LL_FeedbackMessageReceived shall do nothing and return delivery_accepted ] */
    if (context == NULL)
    {
        result = messaging_delivery_accepted();
    }
    else
    {
        IOTHUB_MESSAGING* messagingData = (IOTHUB_MESSAGING*)context;

        BINARY_DATA binary_data;
        JSON_Value* root_value = NULL;
        JSON_Object* feedback_object = NULL;
        JSON_Array* feedback_array = NULL;

        /*Codes_SRS_IOTHUBMESSAGING_12_058: [ If context is not NULL IoTHubMessaging_LL_FeedbackMessageReceived shall get the content string of the message by calling message_get_body_amqp_data ] */
        /*Codes_SRS_IOTHUBMESSAGING_12_059: [ IoTHubMessaging_LL_FeedbackMessageReceived shall parse the response JSON to IOTHUB_SERVICE_FEEDBACK_BATCH struct ] */
        /*Codes_SRS_IOTHUBMESSAGING_12_060: [ IoTHubMessaging_LL_FeedbackMessageReceived shall use the following parson APIs to parse the response string: json_parse_string, json_value_get_object, json_object_get_string, json_object_dotget_string  ] */
        if (message_get_body_amqp_data(message, 0, &binary_data) != 0)
        {
            /*Codes_SRS_IOTHUBMESSAGING_12_061: [ If any of the parson API fails, IoTHubMessaging_LL_FeedbackMessageReceived shall return IOTHUB_MESSAGING_INVALID_JSON ] */
            LogError("Cannot get message data");
            result = messaging_delivery_rejected("Rejected due to failure reading AMQP message", "Failed reading message body");
        }
        else if ((root_value = json_parse_string((const char*)binary_data.bytes)) == NULL)
        {
            /*Codes_SRS_IOTHUBMESSAGING_12_061: [ If any of the parson API fails, IoTHubMessaging_LL_FeedbackMessageReceived shall return IOTHUB_MESSAGING_INVALID_JSON ] */
            LogError("json_parse_string failed");
            result = messaging_delivery_rejected("Rejected due to failure reading AMQP message", "Failed parsing json root");
        }
        else if ((feedback_array = json_value_get_array(root_value)) == NULL)
        {
            /*Codes_SRS_IOTHUBMESSAGING_12_061: [ If any of the parson API fails, IoTHubMessaging_LL_FeedbackMessageReceived shall return IOTHUB_MESSAGING_INVALID_JSON ] */
            LogError("json_parse_string failed");
            result = messaging_delivery_rejected("Rejected due to failure reading AMQP message", "Failed parsing json array");
        }
        else if (json_array_get_count(feedback_array) == 0)
        {
            /*Codes_SRS_IOTHUBMESSAGING_12_061: [ If any of the parson API fails, IoTHubMessaging_LL_FeedbackMessageReceived shall return IOTHUB_MESSAGING_INVALID_JSON ] */
            LogError("json_array_get_count failed");
            result = messaging_delivery_rejected("Rejected due to failure reading AMQP message", "json_array_get_count failed");
        }
        else
        {
            IOTHUB_SERVICE_FEEDBACK_BATCH* feedbackBatch;

            if ((feedbackBatch = (IOTHUB_SERVICE_FEEDBACK_BATCH*)malloc(sizeof(IOTHUB_SERVICE_FEEDBACK_BATCH))) == NULL)
            {
                /*Codes_SRS_IOTHUBMESSAGING_12_061: [ If any of the parson API fails, IoTHubMessaging_LL_FeedbackMessageReceived shall return IOTHUB_MESSAGING_INVALID_JSON ] */
                LogError("json_parse_string failed");
                result = messaging_delivery_rejected("Rejected due to failure reading AMQP message", "Failed to allocate memory for feedback batch");
            }
            else
            {
                size_t array_count = 0;
                if ((array_count = json_array_get_count(feedback_array)) <= 0)
                {
                    /*Codes_SRS_IOTHUBMESSAGING_12_061: [ If any of the parson API fails, IoTHubMessaging_LL_FeedbackMessageReceived shall return IOTHUB_MESSAGING_INVALID_JSON ] */
                    LogError("json_array_get_count failed");
                    free(feedbackBatch);
                    result = messaging_delivery_rejected("Rejected due to failure reading AMQP message", "json_array_get_count failed");
                }
                else if ((feedbackBatch->feedbackRecordList = list_create()) == NULL)
                {
                    /*Codes_SRS_IOTHUBMESSAGING_12_061: [ If any of the parson API fails, IoTHubMessaging_LL_FeedbackMessageReceived shall return IOTHUB_MESSAGING_INVALID_JSON ] */
                    LogError("list_create failed");
                    free(feedbackBatch);
                    result = messaging_delivery_rejected("Rejected due to failure reading AMQP message", "list_create failed");
                }
                else
                {
                    bool isLoopFailed = false;
                    for (size_t i = 0; i < array_count; i++)
                    {
                        if ((feedback_object = json_array_get_object(feedback_array, i)) == NULL)
                        {
                            isLoopFailed = true;
                            break;
                        }
                        else
                        {
                            IOTHUB_SERVICE_FEEDBACK_RECORD* feedbackRecord;
                            if ((feedbackRecord = (IOTHUB_SERVICE_FEEDBACK_RECORD*)malloc(sizeof(IOTHUB_SERVICE_FEEDBACK_RECORD))) == NULL)
                            {
                                isLoopFailed = true;
                                break;
                            }
                            else
                            {
                                feedbackRecord->deviceId = (char*)json_object_get_string(feedback_object, FEEDBACK_RECORD_KEY_DEVICE_ID);
                                feedbackRecord->generationId = (char*)json_object_get_string(feedback_object, FEEDBACK_RECORD_KEY_DEVICE_GENERATION_ID);
                                feedbackRecord->description = (char*)json_object_get_string(feedback_object, FEEDBACK_RECORD_KEY_DESCRIPTION);
                                feedbackRecord->enqueuedTimeUtc = (char*)json_object_get_string(feedback_object, FEEDBACK_RECORD_KEY_ENQUED_TIME_UTC);
                                feedbackRecord->originalMessageId = (char*)json_object_get_string(feedback_object, FEEDBACK_RECORD_KEY_ORIGINAL_MESSAGE_ID);
                                feedbackRecord->correlationId = "";

                                if (feedbackRecord->description == NULL)
                                {
                                    feedbackRecord->statusCode = IOTHUB_FEEDBACK_STATUS_CODE_UNKNOWN;
                                }
                                else
                                {
                                    size_t j;
                                    for (j = 0; feedbackRecord->description[j]; j++)
                                    {
                                        feedbackRecord->description[j] = (char)tolower(feedbackRecord->description[j]);
                                    }

                                    if (strcmp(feedbackRecord->description, "success") == 0)
                                    {
                                        feedbackRecord->statusCode = IOTHUB_FEEDBACK_STATUS_CODE_SUCCESS;
                                    }
                                    else if (strcmp(feedbackRecord->description, "expired") == 0)
                                    {
                                        feedbackRecord->statusCode = IOTHUB_FEEDBACK_STATUS_CODE_EXPIRED;
                                    }
                                    else if (strcmp(feedbackRecord->description, "deliverycountexceeded") == 0)
                                    {
                                        feedbackRecord->statusCode = IOTHUB_FEEDBACK_STATUS_CODE_DELIVER_COUNT_EXCEEDED;
                                    }
                                    else if (strcmp(feedbackRecord->description, "rejected") == 0)
                                    {
                                        feedbackRecord->statusCode = IOTHUB_FEEDBACK_STATUS_CODE_REJECTED;
                                    }
                                    else
                                    {
                                        feedbackRecord->statusCode = IOTHUB_FEEDBACK_STATUS_CODE_UNKNOWN;
                                    }
                                }
                                list_add(feedbackBatch->feedbackRecordList, feedbackRecord);
                            }
                        }
                    }
                    feedbackBatch->lockToken = "";
                    feedbackBatch->userId = "";

                    if (isLoopFailed)
                    {
                        LogError("Failed to read feedback records");
                        result = messaging_delivery_rejected("Rejected due to failure reading AMQP message", "Failed to read feedback records");
                    }
                    else
                    {
                        if (messagingData->callback_data->feedbackMessageCallback != NULL)
                        {
                            /*Codes_SRS_IOTHUBMESSAGING_12_062: [ If context is not NULL IoTHubMessaging_LL_FeedbackMessageReceived shall call IOTHUB_FEEDBACK_MESSAGE_RECEIVED_CALLBACK with the received IOTHUB_SERVICE_FEEDBACK_BATCH ] */
                            (messagingData->callback_data->feedbackMessageCallback)(messagingData->callback_data->feedbackUserContext, feedbackBatch);
                        }
                        result = messaging_delivery_accepted();
                    }

                    /*Codes_SRS_IOTHUBMESSAGING_12_078: [** IoTHubMessaging_LL_FeedbackMessageReceived shall do clean up before exits ] */
                    LIST_ITEM_HANDLE feedbackRecord = list_get_head_item(feedbackBatch->feedbackRecordList);
                    while (feedbackRecord != NULL)
                    {
                        IOTHUB_SERVICE_FEEDBACK_RECORD* feedback = (IOTHUB_SERVICE_FEEDBACK_RECORD*)list_item_get_value(feedbackRecord);
                        feedbackRecord = list_get_next_item(feedbackRecord);
                        free(feedback);
                    }
                    list_destroy(feedbackBatch->feedbackRecordList);
                    free(feedbackBatch);
                }
            }
        }
        json_array_clear(feedback_array);
        json_object_clear(feedback_object);
        json_value_free(root_value);
    }
    return result;
}

IOTHUB_MESSAGING_HANDLE IoTHubMessaging_LL_Create(IOTHUB_SERVICE_CLIENT_AUTH_HANDLE serviceClientHandle)
{
    IOTHUB_MESSAGING_HANDLE result;
    CALLBACK_DATA* callback_data;

    /*Codes_SRS_IOTHUBMESSAGING_12_001: [ If the serviceClientHandle input parameter is NULL IoTHubMessaging_LL_Create shall return NULL ] */
    if (serviceClientHandle == NULL)
    {
        LogError("serviceClientHandle input parameter cannot be NULL");
        result = NULL;
    }
    else
    {
        /*Codes_SRS_IOTHUBMESSAGING_12_064: [ If any member of the serviceClientHandle input parameter is NULL IoTHubMessaging_LL_Create shall return NULL ] */
        IOTHUB_SERVICE_CLIENT_AUTH* serviceClientAuth = (IOTHUB_SERVICE_CLIENT_AUTH*)serviceClientHandle;

        if (serviceClientAuth->hostname == NULL)
        {
            LogError("authInfo->hostName input parameter cannot be NULL");
            result = NULL;
        }
        else if (serviceClientAuth->iothubName == NULL)
        {
            LogError("authInfo->iothubName input parameter cannot be NULL");
            result = NULL;
        }
        else if (serviceClientAuth->iothubSuffix == NULL)
        {
            LogError("authInfo->iothubSuffix input parameter cannot be NULL");
            result = NULL;
        }
        else if (serviceClientAuth->keyName == NULL)
        {
            LogError("authInfo->keyName input parameter cannot be NULL");
            result = NULL;
        }
        else if (serviceClientAuth->sharedAccessKey == NULL)
        {
            LogError("authInfo->sharedAccessKey input parameter cannot be NULL");
            result = NULL;
        }
        else
        {
            /*Codes_SRS_IOTHUBMESSAGING_12_002: [ IoTHubMessaging_LL_Create shall allocate memory for a new messaging instance ] */
            if ((result = (IOTHUB_MESSAGING*)malloc(sizeof(IOTHUB_MESSAGING))) == NULL)
            {
                /*Codes_SRS_IOTHUBMESSAGING_12_003: [ If the allocation failed, IoTHubMessaging_LL_Create shall return NULL ] */
                LogError("Malloc failed for IOTHUB_REGISTRYMANAGER");
            }
            /*Codes_SRS_IOTHUBMESSAGING_12_004: [ If the allocation and creation is successful, IoTHubMessaging_LL_Create shall return with the messaging instance, a non-NULL value ] */
            /*Codes_SRS_IOTHUBMESSAGING_12_065: [ IoTHubMessaging_LL_Create shall allocate memory and copy hostName to result->hostName by calling mallocAndStrcpy_s ] */
            else if (mallocAndStrcpy_s(&result->hostname, serviceClientAuth->hostname) != 0)
            {
                /*Codes_SRS_IOTHUBMESSAGING_12_066: [ If the mallocAndStrcpy_s fails, IoTHubMessaging_LL_Create shall do clean up and return NULL ] */
                LogError("mallocAndStrcpy_s failed for hostName");
                free(result);
                result = NULL;
            }
            /*Codes_SRS_IOTHUBMESSAGING_12_067: [ IoTHubMessaging_LL_Create shall allocate memory and copy iothubName to result->iothubName by calling mallocAndStrcpy_s ] */
            else if (mallocAndStrcpy_s(&result->iothubName, serviceClientAuth->iothubName) != 0)
            {
                /*Codes_SRS_IOTHUBMESSAGING_12_068: [ If the mallocAndStrcpy_s fails, IoTHubMessaging_LL_Create shall do clean up and return NULL ] */
                LogError("mallocAndStrcpy_s failed for iothubName");
                free(result->hostname);
                free(result);
                result = NULL;
            }
            /*Codes_SRS_IOTHUBMESSAGING_12_069: [ IoTHubMessaging_LL_Create shall allocate memory and copy iothubSuffix to result->iothubSuffix by calling mallocAndStrcpy_s ] */
            else if (mallocAndStrcpy_s(&result->iothubSuffix, serviceClientAuth->iothubSuffix) != 0)
            {
                /*Codes_SRS_IOTHUBMESSAGING_12_070: [ If the mallocAndStrcpy_s fails, IoTHubMessaging_LL_Create shall do clean up and return NULL ] */
                LogError("mallocAndStrcpy_s failed for iothubSuffix");
                free(result->hostname);
                free(result->iothubName);
                free(result);
                result = NULL;
            }
            /*Codes_SRS_IOTHUBMESSAGING_12_071: [ IoTHubMessaging_LL_Create shall allocate memory and copy sharedAccessKey to result->sharedAccessKey by calling mallocAndStrcpy_s ] */
            else if (mallocAndStrcpy_s(&result->sharedAccessKey, serviceClientAuth->sharedAccessKey) != 0)
            {
                /*Codes_SRS_IOTHUBMESSAGING_12_072: [ If the mallocAndStrcpy_s fails, IoTHubMessaging_LL_Create shall do clean up and return NULL ] */
                LogError("mallocAndStrcpy_s failed for sharedAccessKey");
                free(result->hostname);
                free(result->iothubName);
                free(result->iothubSuffix);
                free(result);
                result = NULL;
            }
            /*Codes_SRS_IOTHUBMESSAGING_12_073: [ IoTHubMessaging_LL_Create shall allocate memory and copy keyName to result->keyName by calling mallocAndStrcpy_s ] */
            else if (mallocAndStrcpy_s(&result->keyName, serviceClientAuth->keyName) != 0)
            {
                /*Codes_SRS_IOTHUBMESSAGING_12_074: [ If the mallocAndStrcpy_s fails, IoTHubMessaging_LL_Create shall do clean up and return NULL ] */
                LogError("mallocAndStrcpy_s failed for keyName");
                free(result->hostname);
                free(result->iothubName);
                free(result->iothubSuffix);
                free(result->sharedAccessKey);
                free(result);
                result = NULL;
            }
            /*Codes_SRS_IOTHUBMESSAGING_12_075: [ IoTHubMessaging_LL_Create shall set messaging isOpened flag to false ] */
            else if ((callback_data = (CALLBACK_DATA*)malloc(sizeof(CALLBACK_DATA))) == NULL)
            {
                /*Codes_SRS_IOTHUBMESSAGING_12_074: [ If the mallocAndStrcpy_s fails, IoTHubMessaging_LL_Create shall do clean up and return NULL ] */
                LogError("Malloc failed for callback_data");
                free(result->hostname);
                free(result->iothubName);
                free(result->iothubSuffix);
                free(result->sharedAccessKey);
                free(result->keyName);
                free(result);
                result = NULL;
            }
            else
            {
                /*Codes_SRS_IOTHUBMESSAGING_12_076: [ If create successfull IoTHubMessaging_LL_Create shall save the callback data return the valid messaging handle ] */
                callback_data->openCompleteCompleteCallback = NULL;
                callback_data->sendCompleteCallback = NULL;
                callback_data->feedbackMessageCallback = NULL;
                callback_data->openUserContext = NULL;
                callback_data->sendUserContext = NULL;
                callback_data->feedbackUserContext = NULL;

                result->callback_data = callback_data;
                result->isOpened = false;
            }
        }
    }
    return result;
}

void IoTHubMessaging_LL_Destroy(IOTHUB_MESSAGING_HANDLE messagingHandle)
{
    /*Codes_SRS_IOTHUBMESSAGING_12_005: [ If the messagingHandle input parameter is NULL IoTHubMessaging_LL_Destroy shall return ] */
    if (messagingHandle != NULL)
    {
        /*Codes_SRS_IOTHUBMESSAGING_12_006: [ If the messagingHandle input parameter is not NULL IoTHubMessaging_LL_Destroy shall free all resources (memory) allocated by IoTHubMessaging_LL_Create ] */
        IOTHUB_MESSAGING* messHandle = (IOTHUB_MESSAGING*)messagingHandle;

        free(messHandle->callback_data);
        free(messHandle->hostname);
        free(messHandle->iothubName);
        free(messHandle->iothubSuffix);
        free(messHandle->sharedAccessKey);
        free(messHandle->keyName);
        free(messHandle);
    }
}

IOTHUB_MESSAGING_RESULT IoTHubMessaging_LL_Open(IOTHUB_MESSAGING_HANDLE messagingHandle, IOTHUB_OPEN_COMPLETE_CALLBACK openCompleteCallback, void* userContextCallback)
{
    IOTHUB_MESSAGING_RESULT result;

    char* send_target_address = NULL;
    char* receive_target_address = NULL;

    TLSIO_CONFIG tls_io_config;
    SASLCLIENTIO_CONFIG sasl_io_config;

    AMQP_VALUE sendSource = NULL;
    AMQP_VALUE sendTarget = NULL;

    AMQP_VALUE receiveSource = NULL;
    AMQP_VALUE receiveTarget = NULL;

    /*Codes_SRS_IOTHUBMESSAGING_12_007: [ If the messagingHandle input parameter is NULL IoTHubMessaging_LL_Open shall return IOTHUB_MESSAGING_INVALID_ARG ] */
    if (messagingHandle == NULL)
    {
        LogError("Input parameter cannot be NULL");
        result = IOTHUB_MESSAGING_INVALID_ARG;
    }
    else
    {
        messagingHandle->message_sender = NULL;
        messagingHandle->connection = NULL;
        messagingHandle->session = NULL;
        messagingHandle->sender_link = NULL;
        messagingHandle->sasl_plain_config.authzid = NULL;
        messagingHandle->sasl_mechanism_handle = NULL;
        messagingHandle->tls_io = NULL;
        messagingHandle->sasl_io = NULL;

        /*Codes_SRS_IOTHUBMESSAGING_12_008: [ If messaging is already opened IoTHubMessaging_LL_Open return shall IOTHUB_MESSAGING_OK ] */
        if (messagingHandle->isOpened != 0)
        {
            LogError("Messaging is already opened");
            result = IOTHUB_MESSAGING_OK;
        }
        /*Codes_SRS_IOTHUBMESSAGING_12_022: [ IoTHubMessaging_LL_Open shall create uAMQP messaging target for sender by calling the messaging_create_target ] */
        else if ((send_target_address = createSendTargetAddress(messagingHandle)) == NULL)
        {
            /*Codes_SRS_IOTHUBMESSAGING_12_077: [ If the messagingHandle->hostname input parameter is NULL IoTHubMessaging_LL_Open shall return IOTHUB_MESSAGING_ERROR ] */
            LogError("Could not create sendTargetAddress");
            result = IOTHUB_MESSAGING_ERROR;
        }
        /*Codes_SRS_IOTHUBMESSAGING_12_027: [ IoTHubMessaging_LL_Open shall create uAMQP messaging source for receiver by calling the messaging_create_source ] */
        else if ((receive_target_address = createReceiveTargetAddress(messagingHandle)) == NULL)
        {
            /*Codes_SRS_IOTHUBMESSAGING_12_030: [ If any of the uAMQP call fails IoTHubMessaging_LL_Open shall return IOTHUB_MESSAGING_ERROR ] */
            LogError("Could not create receiveTargetAddress");
            result = IOTHUB_MESSAGING_ERROR;
        }
        /*Codes_SRS_IOTHUBMESSAGING_12_010: [ IoTHubMessaging_LL_Open shall create uAMQP PLAIN SASL mechanism by calling saslmechanism_create with the sasl plain interface ] */
        else if ((messagingHandle->sasl_plain_config.authcid = createAuthCid(messagingHandle)) == NULL)
        {
            /*Codes_SRS_IOTHUBMESSAGING_12_030: [ If any of the uAMQP call fails IoTHubMessaging_LL_Open shall return IOTHUB_MESSAGING_ERROR ] */
            LogError("Could not create authCid");
            result = IOTHUB_MESSAGING_ERROR;
        }
        /*Codes_SRS_IOTHUBMESSAGING_12_010: [ IoTHubMessaging_LL_Open shall create uAMQP PLAIN SASL mechanism by calling saslmechanism_create with the sasl plain interface ] */
        else if ((messagingHandle->sasl_plain_config.passwd = createSasToken(messagingHandle)) == NULL)
        {
            /*Codes_SRS_IOTHUBMESSAGING_12_030: [ If any of the uAMQP call fails IoTHubMessaging_LL_Open shall return IOTHUB_MESSAGING_ERROR ] */
            LogError("Could not create sasToken");
            free((char*)messagingHandle->sasl_plain_config.authcid);
            result = IOTHUB_MESSAGING_ERROR;
        }
        else
        {
            const SASL_MECHANISM_INTERFACE_DESCRIPTION* sasl_mechanism_interface;

            /*Codes_SRS_IOTHUBMESSAGING_12_010: [ IoTHubMessaging_LL_Open shall create uAMQP PLAIN SASL mechanism by calling saslmechanism_create with the sasl plain interface ] */
            if ((sasl_mechanism_interface = saslplain_get_interface()) == NULL)
            {
                /*Codes_SRS_IOTHUBMESSAGING_12_030: [ If any of the uAMQP call fails IoTHubMessaging_LL_Open shall return IOTHUB_MESSAGING_ERROR ] */
                LogError("Could not get SASL plain mechanism interface.");
                free((char*)messagingHandle->sasl_plain_config.authcid);
                free((char*)messagingHandle->sasl_plain_config.passwd);
                result = IOTHUB_MESSAGING_ERROR;
            }
            /*Codes_SRS_IOTHUBMESSAGING_12_010: [ IoTHubMessaging_LL_Open shall create uAMQP PLAIN SASL mechanism by calling saslmechanism_create with the sasl plain interface ] */
            else if ((messagingHandle->sasl_mechanism_handle = saslmechanism_create(sasl_mechanism_interface, &messagingHandle->sasl_plain_config)) == NULL)
            {
                /*Codes_SRS_IOTHUBMESSAGING_12_030: [ If any of the uAMQP call fails IoTHubMessaging_LL_Open shall return IOTHUB_MESSAGING_ERROR ] */
                LogError("Could not create SASL plain mechanism.");
                free((char*)messagingHandle->sasl_plain_config.authcid);
                free((char*)messagingHandle->sasl_plain_config.passwd);
                result = IOTHUB_MESSAGING_ERROR;
            }
            else
            {
                tls_io_config.hostname = messagingHandle->hostname;
                tls_io_config.port = 5671;

                const IO_INTERFACE_DESCRIPTION* tlsio_interface;

                /*Codes_SRS_IOTHUBMESSAGING_12_011: [ IoTHubMessaging_LL_Open shall create uAMQP TLSIO by calling the xio_create ] */
                if ((tlsio_interface = platform_get_default_tlsio()) == NULL)
                {
                    /*Codes_SRS_IOTHUBMESSAGING_12_030: [ If any of the uAMQP call fails IoTHubMessaging_LL_Open shall return IOTHUB_MESSAGING_ERROR ] */
                    LogError("Could not get default TLS IO interface.");
                    free((char*)messagingHandle->sasl_plain_config.authcid);
                    free((char*)messagingHandle->sasl_plain_config.passwd);
                    result = IOTHUB_MESSAGING_ERROR;
                }
                /*Codes_SRS_IOTHUBMESSAGING_12_011: [ IoTHubMessaging_LL_Open shall create uAMQP TLSIO by calling the xio_create ] */
                else if ((messagingHandle->tls_io = xio_create(tlsio_interface, &tls_io_config)) == NULL)
                {
                    /*Codes_SRS_IOTHUBMESSAGING_12_030: [ If any of the uAMQP call fails IoTHubMessaging_LL_Open shall return IOTHUB_MESSAGING_ERROR ] */
                    LogError("Could not create TLS IO.");
                    free((char*)messagingHandle->sasl_plain_config.authcid);
                    free((char*)messagingHandle->sasl_plain_config.passwd);
                    result = IOTHUB_MESSAGING_ERROR;
                }
                else
                {
                    messagingHandle->callback_data->openCompleteCompleteCallback = openCompleteCallback;
                    messagingHandle->callback_data->openUserContext = userContextCallback;

                    sasl_io_config.sasl_mechanism = messagingHandle->sasl_mechanism_handle;
                    sasl_io_config.underlying_io = messagingHandle->tls_io;

                    const IO_INTERFACE_DESCRIPTION* saslclientio_interface;

                    /*Codes_SRS_IOTHUBMESSAGING_12_012: [ IoTHubMessaging_LL_Open shall create uAMQP SASL IO by calling the xio_create with the previously created SASL mechanism and TLSIO] */
                    if ((saslclientio_interface = saslclientio_get_interface_description()) == NULL)
                    {
                        /*Codes_SRS_IOTHUBMESSAGING_12_030: [ If any of the uAMQP call fails IoTHubMessaging_LL_Open shall return IOTHUB_MESSAGING_ERROR ] */
                        LogError("Could not create get SASL IO interface description.");
                        free((char*)messagingHandle->sasl_plain_config.authcid);
                        free((char*)messagingHandle->sasl_plain_config.passwd);
                        result = IOTHUB_MESSAGING_ERROR;
                    }
                    /*Codes_SRS_IOTHUBMESSAGING_12_012: [ IoTHubMessaging_LL_Open shall create uAMQP SASL IO by calling the xio_create with the previously created SASL mechanism and TLSIO] */
                    else if ((messagingHandle->sasl_io = xio_create(saslclientio_interface, &sasl_io_config)) == NULL)
                    {
                        /*Codes_SRS_IOTHUBMESSAGING_12_030: [ If any of the uAMQP call fails IoTHubMessaging_LL_Open shall return IOTHUB_MESSAGING_ERROR ] */
                        LogError("Could not create SASL IO.");
                        free((char*)messagingHandle->sasl_plain_config.authcid);
                        free((char*)messagingHandle->sasl_plain_config.passwd);
                        result = IOTHUB_MESSAGING_ERROR;
                    }
                    /*Codes_SRS_IOTHUBMESSAGING_12_013: [ IoTHubMessaging_LL_Open shall create uAMQP connection by calling the connection_create with the previously created SASL IO ] */
                    else if ((messagingHandle->connection = connection_create(messagingHandle->sasl_io, messagingHandle->hostname, "some", NULL, NULL)) == NULL)
                    {
                        /*Codes_SRS_IOTHUBMESSAGING_12_030: [ If any of the uAMQP call fails IoTHubMessaging_LL_Open shall return IOTHUB_MESSAGING_ERROR ] */
                        LogError("Could not create connection.");
                        free((char*)messagingHandle->sasl_plain_config.authcid);
                        free((char*)messagingHandle->sasl_plain_config.passwd);
                        result = IOTHUB_MESSAGING_ERROR;
                    }
                    /*Codes_SRS_IOTHUBMESSAGING_12_014: [ IoTHubMessaging_LL_Open shall create uAMQP session by calling the session_create ] */
                    else if ((messagingHandle->session = session_create(messagingHandle->connection, NULL, NULL)) == NULL)
                    {
                        /*Codes_SRS_IOTHUBMESSAGING_12_030: [ If any of the uAMQP call fails IoTHubMessaging_LL_Open shall return IOTHUB_MESSAGING_ERROR ] */
                        LogError("Could not create session.");
                        free((char*)messagingHandle->sasl_plain_config.authcid);
                        free((char*)messagingHandle->sasl_plain_config.passwd);
                        result = IOTHUB_MESSAGING_ERROR;
                    }
                    /*Codes_SRS_IOTHUBMESSAGING_12_015: [ IoTHubMessaging_LL_Open shall set the AMQP incoming window to UINT32 maximum value by calling session_set_incoming_window ] */
                    else if (session_set_incoming_window(messagingHandle->session, 2147483647) != 0)
                    {
                        /*Codes_SRS_IOTHUBMESSAGING_12_030: [ If any of the uAMQP call fails IoTHubMessaging_LL_Open shall return IOTHUB_MESSAGING_ERROR ] */
                        LogError("Could not set incoming window.");
                        free((char*)messagingHandle->sasl_plain_config.authcid);
                        free((char*)messagingHandle->sasl_plain_config.passwd);
                        result = IOTHUB_MESSAGING_ERROR;
                    }
                    /*Codes_SRS_IOTHUBMESSAGING_12_016: [ IoTHubMessaging_LL_Open shall set the AMQP outgoing window to UINT32 maximum value by calling session_set_outgoing_window ] */
                    else if (session_set_outgoing_window(messagingHandle->session, 255 * 1024) != 0)
                    {
                        /*Codes_SRS_IOTHUBMESSAGING_12_030: [ If any of the uAMQP call fails IoTHubMessaging_LL_Open shall return IOTHUB_MESSAGING_ERROR ] */
                        LogError("Could not set outgoing window.");
                        free((char*)messagingHandle->sasl_plain_config.authcid);
                        free((char*)messagingHandle->sasl_plain_config.passwd);
                        result = IOTHUB_MESSAGING_ERROR;
                    }
                    /*Codes_SRS_IOTHUBMESSAGING_12_021: [ IoTHubMessaging_LL_Open shall create uAMQP messaging source for sender by calling the messaging_create_source ] */
                    else if ((sendSource = messaging_create_source("ingress")) == NULL)
                    {
                        /*Codes_SRS_IOTHUBMESSAGING_12_030: [ If any of the uAMQP call fails IoTHubMessaging_LL_Open shall return IOTHUB_MESSAGING_ERROR ] */
                        LogError("Could not create source for link.");
                        free((char*)messagingHandle->sasl_plain_config.authcid);
                        free((char*)messagingHandle->sasl_plain_config.passwd);
                        result = IOTHUB_MESSAGING_ERROR;
                    }
                    /*Codes_SRS_IOTHUBMESSAGING_12_022: [ IoTHubMessaging_LL_Open shall create uAMQP messaging target for sender by calling the messaging_create_target ] */
                    else if ((sendTarget = messaging_create_target(send_target_address)) == NULL)
                    {
                        /*Codes_SRS_IOTHUBMESSAGING_12_030: [ If any of the uAMQP call fails IoTHubMessaging_LL_Open shall return IOTHUB_MESSAGING_ERROR ] */
                        LogError("Could not create target for link.");
                        free((char*)messagingHandle->sasl_plain_config.authcid);
                        free((char*)messagingHandle->sasl_plain_config.passwd);
                        result = IOTHUB_MESSAGING_ERROR;
                    }
                    /*Codes_SRS_IOTHUBMESSAGING_12_018: [ IoTHubMessaging_LL_Open shall create uAMQP sender link by calling the link_create ] */
                    else if ((messagingHandle->sender_link = link_create(messagingHandle->session, "sender-link", role_sender, sendSource, sendTarget)) == NULL)
                    {
                        /*Codes_SRS_IOTHUBMESSAGING_12_030: [ If any of the uAMQP call fails IoTHubMessaging_LL_Open shall return IOTHUB_MESSAGING_ERROR ] */
                        LogError("Could not create link.");
                        free((char*)messagingHandle->sasl_plain_config.authcid);
                        free((char*)messagingHandle->sasl_plain_config.passwd);
                        result = IOTHUB_MESSAGING_ERROR;
                    }
                    /*Codes_SRS_IOTHUBMESSAGING_12_019: [ IoTHubMessaging_LL_Open shall set the AMQP sender link settle mode to sender_settle_mode_unsettled  by calling link_set_snd_settle_mode ] */
                    else if (link_set_snd_settle_mode(messagingHandle->sender_link, sender_settle_mode_unsettled) != 0)
                    {
                        /*Codes_SRS_IOTHUBMESSAGING_12_030: [ If any of the uAMQP call fails IoTHubMessaging_LL_Open shall return IOTHUB_MESSAGING_ERROR ] */
                        LogError("Could not set the sender settle mode.");
                        free((char*)messagingHandle->sasl_plain_config.authcid);
                        free((char*)messagingHandle->sasl_plain_config.passwd);
                        result = IOTHUB_MESSAGING_ERROR;
                    }
                    /*Codes_SRS_IOTHUBMESSAGING_12_020: [ IoTHubMessaging_LL_Open shall set sender link AMQP maximum message size to the server maximum (255K) by calling link_set_max_message_size ] */
                    else if (link_set_max_message_size(messagingHandle->sender_link, 65536) != 0)
                    {
                        /*Codes_SRS_IOTHUBMESSAGING_12_030: [ If any of the uAMQP call fails IoTHubMessaging_LL_Open shall return IOTHUB_MESSAGING_ERROR ] */
                        LogError("Could not set the message size.");
                        free((char*)messagingHandle->sasl_plain_config.authcid);
                        free((char*)messagingHandle->sasl_plain_config.passwd);
                        result = IOTHUB_MESSAGING_ERROR;
                    }
                    /*Codes_SRS_IOTHUBMESSAGING_12_023: [ IoTHubMessaging_LL_Open shall create uAMQP message sender by calling the messagesender_create with the created sender link and the local IoTHubMessaging_LL_SenderStateChanged callback ] */
                    else if ((messagingHandle->message_sender = messagesender_create(messagingHandle->sender_link, IoTHubMessaging_LL_SenderStateChanged, messagingHandle)) == NULL)
                    {
                        /*Codes_SRS_IOTHUBMESSAGING_12_030: [ If any of the uAMQP call fails IoTHubMessaging_LL_Open shall return IOTHUB_MESSAGING_ERROR ] */
                        LogError("Could not create message sender.");
                        free((char*)messagingHandle->sasl_plain_config.authcid);
                        free((char*)messagingHandle->sasl_plain_config.passwd);
                        result = IOTHUB_MESSAGING_ERROR;
                    }
                    /*Codes_SRS_IOTHUBMESSAGING_12_023: [ IoTHubMessaging_LL_Open shall create uAMQP message sender by calling the messagesender_create with the created sender link and the local IoTHubMessaging_LL_SenderStateChanged callback ] */
                    else if (messagesender_open(messagingHandle->message_sender) != 0)
                    {
                        /*Codes_SRS_IOTHUBMESSAGING_12_030: [ If any of the uAMQP call fails IoTHubMessaging_LL_Open shall return IOTHUB_MESSAGING_ERROR ] */
                        LogError("Could not open the message sender.");
                        free((char*)messagingHandle->sasl_plain_config.authcid);
                        free((char*)messagingHandle->sasl_plain_config.passwd);
                        result = IOTHUB_MESSAGING_ERROR;
                    }
                    /*Codes_SRS_IOTHUBMESSAGING_12_027: [ IoTHubMessaging_LL_Open shall create uAMQP messaging source for receiver by calling the messaging_create_source ] */
                    else if ((receiveSource = messaging_create_source(receive_target_address)) == NULL)
                    {
                        /*Codes_SRS_IOTHUBMESSAGING_12_030: [ If any of the uAMQP call fails IoTHubMessaging_LL_Open shall return IOTHUB_MESSAGING_ERROR ] */
                        LogError("Could not create source for link.");
                        free((char*)messagingHandle->sasl_plain_config.authcid);
                        free((char*)messagingHandle->sasl_plain_config.passwd);
                        result = IOTHUB_MESSAGING_ERROR;
                    }
                    /*Codes_SRS_IOTHUBMESSAGING_12_028: [ IoTHubMessaging_LL_Open shall create uAMQP messaging target for receiver by calling the messaging_create_target ] */
                    else if ((receiveTarget = messaging_create_target("receiver_001")) == NULL)
                    {
                        /*Codes_SRS_IOTHUBMESSAGING_12_030: [ If any of the uAMQP call fails IoTHubMessaging_LL_Open shall return IOTHUB_MESSAGING_ERROR ] */
                        LogError("Could not create target for link.");
                        free((char*)messagingHandle->sasl_plain_config.authcid);
                        free((char*)messagingHandle->sasl_plain_config.passwd);
                        result = IOTHUB_MESSAGING_ERROR;
                    }
                    /*Codes_SRS_IOTHUBMESSAGING_12_024: [ IoTHubMessaging_LL_Open shall create uAMQP receiver link by calling the link_create ] */
                    else if ((messagingHandle->receiver_link = link_create(messagingHandle->session, "receiver-link", role_receiver, receiveSource, receiveTarget)) == NULL)
                    {
                        /*Codes_SRS_IOTHUBMESSAGING_12_030: [ If any of the uAMQP call fails IoTHubMessaging_LL_Open shall return IOTHUB_MESSAGING_ERROR ] */
                        LogError("Could not create link.");
                        free((char*)messagingHandle->sasl_plain_config.authcid);
                        free((char*)messagingHandle->sasl_plain_config.passwd);
                        result = IOTHUB_MESSAGING_ERROR;
                    }
                    /*Codes_SRS_IOTHUBMESSAGING_12_025: [ IoTHubMessaging_LL_Open shall set the AMQP receiver link settle mode to receiver_settle_mode_first by calling link_set_rcv_settle_mode ] */
                    else if (link_set_rcv_settle_mode(messagingHandle->receiver_link, receiver_settle_mode_first) != 0)
                    {
                        /*Codes_SRS_IOTHUBMESSAGING_12_030: [ If any of the uAMQP call fails IoTHubMessaging_LL_Open shall return IOTHUB_MESSAGING_ERROR ] */
                        LogError("Could not set the sender settle mode.");
                        free((char*)messagingHandle->sasl_plain_config.authcid);
                        free((char*)messagingHandle->sasl_plain_config.passwd);
                        result = IOTHUB_MESSAGING_ERROR;
                    }
                    /*Codes_SRS_IOTHUBMESSAGING_12_029: [ IoTHubMessaging_LL_Open shall create uAMQP message receiver by calling the messagereceiver_create with the created sender link and the local IoTHubMessaging_LL_ReceiverStateChanged callback ] */
                    else if ((messagingHandle->message_receiver = messagereceiver_create(messagingHandle->receiver_link, IoTHubMessaging_LL_ReceiverStateChanged, messagingHandle)) == NULL)
                    {
                        /*Codes_SRS_IOTHUBMESSAGING_12_030: [ If any of the uAMQP call fails IoTHubMessaging_LL_Open shall return IOTHUB_MESSAGING_ERROR ] */
                        LogError("Could not create message receiver.");
                        free((char*)messagingHandle->sasl_plain_config.authcid);
                        free((char*)messagingHandle->sasl_plain_config.passwd);
                        result = IOTHUB_MESSAGING_ERROR;
                    }
                    /*Codes_SRS_IOTHUBMESSAGING_12_029: [ IoTHubMessaging_LL_Open shall create uAMQP message receiver by calling the messagereceiver_create with the created sender link and the local IoTHubMessaging_LL_ReceiverStateChanged callback ] */
                    else if (messagereceiver_open(messagingHandle->message_receiver, IoTHubMessaging_LL_FeedbackMessageReceived, messagingHandle) != 0)
                    {
                        /*Codes_SRS_IOTHUBMESSAGING_12_030: [ If any of the uAMQP call fails IoTHubMessaging_LL_Open shall return IOTHUB_MESSAGING_ERROR ] */
                        LogError("Could not open the message receiver.");
                        free((char*)messagingHandle->sasl_plain_config.authcid);
                        free((char*)messagingHandle->sasl_plain_config.passwd);
                        result = IOTHUB_MESSAGING_ERROR;
                    }
                    else
                    {
                        /*Codes_SRS_IOTHUBMESSAGING_12_031: [ If all of the uAMQP call return 0 (success) IoTHubMessaging_LL_Open shall return IOTHUB_MESSAGING_OK ] */
                        messagingHandle->isOpened = true;
                        result = IOTHUB_MESSAGING_OK;
                    }
                }
            }
        }
    }
    amqpvalue_destroy(sendSource);
    amqpvalue_destroy(sendTarget);
    amqpvalue_destroy(receiveSource);
    amqpvalue_destroy(receiveTarget);

    if (send_target_address != NULL)
    {
        free(send_target_address);
    }
    if (receive_target_address != NULL)
    {
        free(receive_target_address);
    }
    return result;
}

void IoTHubMessaging_LL_Close(IOTHUB_MESSAGING_HANDLE messagingHandle)
{
    /*Codes_SRS_IOTHUBMESSAGING_12_032: [ If the messagingHandle input parameter is NULL IoTHubMessaging_LL_Close shall return IOTHUB_MESSAGING_INVALID_ARG ] */
    if (messagingHandle == NULL)
    {
        LogError("Input parameter cannot be NULL");
    }
    /*Codes_SRS_IOTHUBMESSAGING_12_033: [ IoTHubMessaging_LL_Close destroy the AMQP transportconnection by calling link_destroy, session_destroy, connection_destroy, xio_destroy, saslmechanism_destroy ] */
    else
    {
        messagesender_destroy(messagingHandle->message_sender);
        messagereceiver_destroy(messagingHandle->message_receiver);

        link_destroy(messagingHandle->sender_link);
        link_destroy(messagingHandle->receiver_link);

        session_destroy(messagingHandle->session);
        connection_destroy(messagingHandle->connection);
        xio_destroy(messagingHandle->sasl_io);
        xio_destroy(messagingHandle->tls_io);
        saslmechanism_destroy(messagingHandle->sasl_mechanism_handle);

        if (messagingHandle->sasl_plain_config.authcid != NULL)
        {
            free((char*)messagingHandle->sasl_plain_config.authcid);
        }
        if (messagingHandle->sasl_plain_config.passwd != NULL)
        {
            free((char*)messagingHandle->sasl_plain_config.passwd);
        }
        if (messagingHandle->sasl_plain_config.authzid != NULL)
        {
            free((char*)messagingHandle->sasl_plain_config.authzid);
        }
        messagingHandle->isOpened = false;
    }
}

IOTHUB_MESSAGING_RESULT IoTHubMessaging_LL_Send(IOTHUB_MESSAGING_HANDLE messagingHandle, const char* deviceId, IOTHUB_MESSAGE_HANDLE message, IOTHUB_SEND_COMPLETE_CALLBACK sendCompleteCallback, void* userContextCallback)
{
    IOTHUB_MESSAGING_RESULT result;

    const unsigned char* data;
    size_t len;

    char* deviceDestinationString = NULL;
    MESSAGE_HANDLE amqpMessage = NULL;
    AMQP_VALUE to_amqp_value = NULL;
    PROPERTIES_HANDLE properties = NULL;

    /*Codes_SRS_IOTHUBMESSAGING_12_034: [ IoTHubMessaging_LL_SendMessage shall verify the messagingHandle, deviceId, message input parameters and if any of them are NULL then return NULL ] */
    if (messagingHandle == NULL)
    {
        LogError("Input parameter messagingHandle cannot be NULL");
        result = IOTHUB_MESSAGING_INVALID_ARG;
    }
    /*Codes_SRS_IOTHUBMESSAGING_12_034: [ IoTHubMessaging_LL_SendMessage shall verify the messagingHandle, deviceId, message input parameters and if any of them are NULL then return NULL ] */
    else if (deviceId == NULL)
    {
        LogError("Input parameter deviceId cannot be NULL");
        result = IOTHUB_MESSAGING_INVALID_ARG;
    }
    /*Codes_SRS_IOTHUBMESSAGING_12_034: [ IoTHubMessaging_LL_SendMessage shall verify the messagingHandle, deviceId, message input parameters and if any of them are NULL then return NULL ] */
    else if (message == NULL)
    {
        LogError("Input parameter message cannot be NULL");
        result = IOTHUB_MESSAGING_INVALID_ARG;
    }
    /*Codes_SRS_IOTHUBMESSAGING_12_035: [ IoTHubMessaging_LL_SendMessage shall verify if the AMQP messaging has been established by a successfull call to _Open and if it is not then return IOTHUB_MESSAGING_ERROR ] */
    else if (messagingHandle->isOpened == 0)
    {
        LogError("Messaging is not opened - call IoTHubMessaging_LL_Open to open");
        result = IOTHUB_MESSAGING_ERROR;
    }
    /*Codes_SRS_IOTHUBMESSAGING_12_038: [ IoTHubMessaging_LL_SendMessage shall set the uAMQP message properties to the given message properties by calling message_set_properties ] */
    else if ((deviceDestinationString = createDeviceDestinationString(deviceId)) == NULL)
    {
        /*Codes_SRS_IOTHUBMESSAGING_12_040: [ If any of the uAMQP call fails IoTHubMessaging_LL_SendMessage shall return IOTHUB_MESSAGING_ERROR ] */
        LogError("Could not create a message.");
        result = IOTHUB_MESSAGING_ERROR;
    }
    /*Codes_SRS_IOTHUBMESSAGING_12_038: [ IoTHubMessaging_LL_SendMessage shall set the uAMQP message properties to the given message properties by calling message_set_properties ] */
    else if ((to_amqp_value = amqpvalue_create_string(deviceDestinationString)) == NULL)
    {
        /*Codes_SRS_IOTHUBMESSAGING_12_040: [ If any of the uAMQP call fails IoTHubMessaging_LL_SendMessage shall return IOTHUB_MESSAGING_ERROR ] */
        LogError("Could not create properties for message - amqpvalue_create_string");
        result = IOTHUB_MESSAGING_ERROR;
    }
    /*Codes_SRS_IOTHUBMESSAGING_12_038: [ IoTHubMessaging_LL_SendMessage shall set the uAMQP message properties to the given message properties by calling message_set_properties ] */
    else if ((properties = properties_create()) == NULL)
    {
        /*Codes_SRS_IOTHUBMESSAGING_12_040: [ If any of the uAMQP call fails IoTHubMessaging_LL_SendMessage shall return IOTHUB_MESSAGING_ERROR ] */
        LogError("Could not create properties for message - properties_create failed");
        result = IOTHUB_MESSAGING_ERROR;
    }
    /*Codes_SRS_IOTHUBMESSAGING_12_038: [ IoTHubMessaging_LL_SendMessage shall set the uAMQP message properties to the given message properties by calling message_set_properties ] */
    else if ((properties_set_to(properties, to_amqp_value)) != 0)
    {
        /*Codes_SRS_IOTHUBMESSAGING_12_040: [ If any of the uAMQP call fails IoTHubMessaging_LL_SendMessage shall return IOTHUB_MESSAGING_ERROR ] */
        LogError("Could not create properties for message - properties_set_to failed");
        result = IOTHUB_MESSAGING_ERROR;
    }
    else if (IoTHubMessage_GetByteArray(message, &data, &len) != IOTHUB_MESSAGE_OK)
    {
        /*Codes_SRS_IOTHUBMESSAGING_12_040: [ If any of the uAMQP call fails IoTHubMessaging_LL_SendMessage shall return IOTHUB_MESSAGING_ERROR ] */
        LogError("Could not create a message - IoTHubMessage_GetByteArray failed");
        result = IOTHUB_MESSAGING_ERROR;
    }
    else
    {
        BINARY_DATA binary_data;

        binary_data.bytes = data;
        binary_data.length = len;

        /*Codes_SRS_IOTHUBMESSAGING_12_036: [ IoTHubMessaging_LL_SendMessage shall create a uAMQP message by calling message_create ] */
        if ((amqpMessage = message_create()) == NULL)
        {
            /*Codes_SRS_IOTHUBMESSAGING_12_040: [ If any of the uAMQP call fails IoTHubMessaging_LL_SendMessage shall return IOTHUB_MESSAGING_ERROR ] */
            LogError("Could not create a message.");
            result = IOTHUB_MESSAGING_ERROR;
        }
        /*Codes_SRS_IOTHUBMESSAGING_12_037: [ IoTHubMessaging_LL_SendMessage shall set the uAMQP message body to the given message content by calling message_add_body_amqp_data ] */
        else if (message_add_body_amqp_data(amqpMessage, binary_data) != 0)
        {
            /*Codes_SRS_IOTHUBMESSAGING_12_040: [ If any of the uAMQP call fails IoTHubMessaging_LL_SendMessage shall return IOTHUB_MESSAGING_ERROR ] */
            LogError("Could not add the binary data to the message - message_add_body_amqp_data failed");
            message_destroy(amqpMessage);
            result = IOTHUB_MESSAGING_ERROR;
        }
        /*Codes_SRS_IOTHUBMESSAGING_12_038: [ IoTHubMessaging_LL_SendMessage shall set the uAMQP message properties to the given message properties by calling message_set_properties ] */
        else if ((message_set_properties(amqpMessage, properties)) != 0)
        {
            /*Codes_SRS_IOTHUBMESSAGING_12_040: [ If any of the uAMQP call fails IoTHubMessaging_LL_SendMessage shall return IOTHUB_MESSAGING_ERROR ] */
            LogError("Could not set the properties on the message - message_set_properties failed");
            message_destroy(amqpMessage);
            result = IOTHUB_MESSAGING_ERROR;
        }
        else
        {
            messagingHandle->callback_data->sendCompleteCallback = sendCompleteCallback;
            messagingHandle->callback_data->sendUserContext = userContextCallback;

            /*Codes_SRS_IOTHUBMESSAGING_12_039: [ IoTHubMessaging_LL_SendMessage shall call uAMQP messagesender_send with the created message with IoTHubMessaging_LL_SendMessageComplete callback by which IoTHubMessaging is notified of completition of send ] */
            if (messagesender_send(messagingHandle->message_sender, amqpMessage, (ON_MESSAGE_SEND_COMPLETE)IoTHubMessaging_LL_SendMessageComplete, messagingHandle) != 0)
            {
                /*Codes_SRS_IOTHUBMESSAGING_12_040: [ If any of the uAMQP call fails IoTHubMessaging_LL_SendMessage shall return IOTHUB_MESSAGING_ERROR ] */
                LogError("Could not set outgoing window.");
                message_destroy(amqpMessage);
                result = IOTHUB_MESSAGING_ERROR;
            }
            else
            {
                /*Codes_SRS_IOTHUBMESSAGING_12_041: [ If all uAMQP call return 0 then IoTHubMessaging_LL_SendMessage shall return IOTHUB_MESSAGING_OK  ] */
                result = IOTHUB_MESSAGING_OK;
            }
        }
    }
    message_destroy(amqpMessage);
    properties_destroy(properties);
    amqpvalue_destroy(to_amqp_value);
    if (deviceDestinationString != NULL)
    {
        free(deviceDestinationString);
    }
    return result;
}

IOTHUB_MESSAGING_RESULT IoTHubMessaging_LL_SetFeedbackMessageCallback(IOTHUB_MESSAGING_HANDLE messagingHandle, IOTHUB_FEEDBACK_MESSAGE_RECEIVED_CALLBACK feedbackMessageReceivedCallback, void* userContextCallback)
{
    IOTHUB_MESSAGING_RESULT result;

    /*Codes_SRS_IOTHUBMESSAGING_12_042: [ IoTHubMessaging_LL_SetCallbacks shall verify the messagingHandle input parameter and if it is NULL then return NULL ] */
    if (messagingHandle == NULL)
    {
        LogError("Input parameter cannot be NULL");
        result = IOTHUB_MESSAGING_INVALID_ARG;
    }
    else
    {
        /*Codes_SRS_IOTHUBMESSAGING_12_043: [ IoTHubMessaging_LL_SetCallbacks shall save the given feedbackMessageReceivedCallback to use them in local callbacks ] */
        /*Codes_SRS_IOTHUBMESSAGING_12_044: [ IoTHubMessaging_LL_Open shall return IOTHUB_MESSAGING_OK after the callbacks have been set ] */
        messagingHandle->callback_data->feedbackMessageCallback = feedbackMessageReceivedCallback;
        messagingHandle->callback_data->feedbackUserContext = userContextCallback;
        result = IOTHUB_MESSAGING_OK;
    }
    return result;
}

void IoTHubMessaging_LL_DoWork(IOTHUB_MESSAGING_HANDLE messagingHandle)
{
    /*Codes_SRS_IOTHUBMESSAGING_12_045: [ IoTHubMessaging_LL_DoWork shall verify if uAMQP transport has been initialized and if it is not then return immediately ] */
    if (messagingHandle != 0)
    {
        /*Codes_SRS_IOTHUBMESSAGING_12_046: [ IoTHubMessaging_LL_DoWork shall call uAMQP connection_dowork ] */
        /*Codes_SRS_IOTHUBMESSAGING_12_047: [ IoTHubMessaging_LL_SendMessageComplete callback given to messagesender_send will be called with MESSAGE_SEND_RESULT ] */
        /*Codes_SRS_IOTHUBMESSAGING_12_048: [ If message has been received the IoTHubMessaging_LL_FeedbackMessageReceived callback given to messagesender_receive will be called with the received MESSAGE_HANDLE ] */
        connection_dowork(messagingHandle->connection);
    }
}

