// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#include "gballoc.h"
#include "string.h"
#include "string_tokenizer.h"

#include "iothub_client_ll.h"
#include "iothub_client_private.h"
#include "doublylinkedlist.h"
#include "version.h"

#include "iot_logging.h"

#define LOG_ERROR LogError("result = %s\r\n", ENUM_TO_STRING(IOTHUB_CLIENT_RESULT, result));
#define INDEFINITE_TIME ((time_t)(-1))

DEFINE_ENUM_STRINGS(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_RESULT_VALUES);

typedef struct IOTHUB_CLIENT_LL_HANDLE_DATA_TAG
{
    DLIST_ENTRY waitingToSend;
    TRANSPORT_HANDLE transportHandle;
    TRANSPORT_PROVIDER_FIELDS;
    IOTHUB_CLIENT_MESSAGE_CALLBACK_ASYNC messageCallback;
    void* messageUserContextCallback;
    time_t lastMessageReceiveTime;
}IOTHUB_CLIENT_LL_HANDLE_DATA;

static const char HOSTNAME_TOKEN[] = "HostName";
static const char DEVICEID_TOKEN[] = "DeviceId";
static const char DEVICEKEY_TOKEN[] = "SharedAccessKey";
static const char PROTOCOL_GATEWAY_HOST[] = "GatewayHostName";

IOTHUB_CLIENT_LL_HANDLE IoTHubClient_LL_CreateFromConnectionString(const char* connectionString, IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol)
{
    IOTHUB_CLIENT_LL_HANDLE* result = NULL;

    /*Codes_SRS_IOTHUBCLIENT_LL_05_001: [IoTHubClient_LL_CreateFromConnectionString shall obtain the version string by a call to IoTHubClient_GetVersionString.]*/
    /*Codes_SRS_IOTHUBCLIENT_LL_05_002: [IoTHubClient_LL_CreateFromConnectionString shall print the version string to standard output.]*/
    LogInfo("IoT Hub SDK for C, version %s\r\n", IoTHubClient_GetVersionString());

    /* SRS_IOTHUBCLIENT_LL_12_003: [IoTHubClient_LL_CreateFromConnectionString shall verify the input parameter and if it is NULL then return NULL] */
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
        /* SRS_IOTHUBCLIENT_LL_12_004: [IoTHubClient_LL_CreateFromConnectionString shall allocate IOTHUB_CLIENT_CONFIG structure] */
        IOTHUB_CLIENT_CONFIG* config = malloc(sizeof(IOTHUB_CLIENT_CONFIG));
        if (config == NULL)
        {
            /* SRS_IOTHUBCLIENT_LL_12_012: [If the allocation failed IoTHubClient_LL_CreateFromConnectionString  returns NULL]  */
            LogError("Malloc failed\r\n");
            return NULL;
        }
        else
        {
            STRING_TOKENIZER_HANDLE tokenizer1 = NULL;
            STRING_HANDLE connString = NULL;
            STRING_HANDLE tokenString = NULL;
            STRING_HANDLE valueString = NULL;
            STRING_HANDLE hostNameString = NULL;
            STRING_HANDLE hostSuffixString = NULL;
            STRING_HANDLE deviceIdString = NULL;
            STRING_HANDLE deviceKeyString = NULL;
            STRING_HANDLE protocolGateway = NULL;

            config->protocol = protocol;

            config->iotHubName = NULL;
            config->iotHubSuffix = NULL;
            config->deviceId = NULL;
            config->deviceKey = NULL;
			/* Codes_SRS_IOTHUBCLIENT_LL_04_002: [If it does not, it shall pass the protocolGatewayHostName NULL.] */
			config->protocolGatewayHostName = NULL;

            if ((connString = STRING_construct(connectionString)) == NULL)
            {
                LogError("Error constructing connectiong String\r\n");
            }
            else if ((tokenizer1 = STRING_TOKENIZER_create(connString)) == NULL)
            {
                LogError("Error creating Tokenizer\r\n");
            }
            else if ((tokenString = STRING_new()) == NULL)
            {
                LogError("Error creating Token String\r\n");
            }
            else if ((valueString = STRING_new()) == NULL)
            {
                LogError("Error creating Value String\r\n");
            }
            else if ((hostNameString = STRING_new()) == NULL)
            {
                LogError("Error creating HostName String\r\n");
            }
            else if ((hostSuffixString = STRING_new()) == NULL)
            {
                LogError("Error creating HostSuffix String\r\n");
            }
            /* SRS_IOTHUBCLIENT_LL_12_005: [IoTHubClient_LL_CreateFromConnectionString shall try to parse the connectionString input parameter for the following structure: "Key1=value1;key2=value2;key3=value3..."] */
            /* SRS_IOTHUBCLIENT_LL_12_006: [IoTHubClient_LL_CreateFromConnectionString shall verify the existence of the following Key/Value pairs in the connection string: HostName, DeviceId, SharedAccessKey.]  */
            else
            {
                while ((STRING_TOKENIZER_get_next_token(tokenizer1, tokenString, "=") == 0))
                {
                    if (STRING_TOKENIZER_get_next_token(tokenizer1, valueString, ";") != 0)
                    {
                        LogError("Tokenizer error\r\n");
                        break;
                    }
                    else
                    {
                        if (tokenString != NULL)
                        {
                            /* SRS_IOTHUBCLIENT_LL_12_010: [IoTHubClient_LL_CreateFromConnectionString shall fill up the IOTHUB_CLIENT_CONFIG structure using the following mapping: iotHubName = Name, iotHubSuffix = Suffix, deviceId = DeviceId, deviceKey = SharedAccessKey] */
                            const char* s_token = STRING_c_str(tokenString);
							if (strcmp(s_token, HOSTNAME_TOKEN) == 0)
                            {
                                /* SRS_IOTHUBCLIENT_LL_12_009: [IoTHubClient_LL_CreateFromConnectionString shall split the value of HostName to Name and Suffix using the first "." as a separator] */
                                STRING_TOKENIZER_HANDLE tokenizer2 = NULL;
                                if ((tokenizer2 = STRING_TOKENIZER_create(valueString)) == NULL)
                                {
                                    LogError("Error creating Tokenizer\r\n");
                                    break;
                                }
                                else
                                {
                                    /* SRS_IOTHUBCLIENT_LL_12_015: [If the string split failed, IoTHubClient_LL_CreateFromConnectionString returns NULL ] */
                                    if (STRING_TOKENIZER_get_next_token(tokenizer2, hostNameString, ".") != 0)
                                    {
                                        LogError("Tokenizer error\r\n");
                                        STRING_TOKENIZER_destroy(tokenizer2);
                                        break;
                                    }
                                    else
                                    {
                                        config->iotHubName = STRING_c_str(hostNameString);
                                        if (STRING_TOKENIZER_get_next_token(tokenizer2, hostSuffixString, ";") != 0)
                                        {
                                            LogError("Tokenizer error\r\n");
                                            STRING_TOKENIZER_destroy(tokenizer2);
                                            break;
                                        }
                                        else
                                        {
                                            config->iotHubSuffix = STRING_c_str(hostSuffixString);
                                        }
                                    }
                                    STRING_TOKENIZER_destroy(tokenizer2);
                                }
                            }
                            else if (strcmp(s_token, DEVICEID_TOKEN) == 0)
                            {
                                deviceIdString = STRING_clone(valueString);
                                if (deviceIdString != NULL)
                                {
                                    config->deviceId = STRING_c_str(deviceIdString);
                                }
                            }
                            else if (strcmp(s_token, DEVICEKEY_TOKEN) == 0)
                            {
                                deviceKeyString = STRING_clone(valueString);
                                if (deviceKeyString != NULL)
                                {
                                    config->deviceKey = STRING_c_str(deviceKeyString);
                                }
                            }
							/* Codes_SRS_IOTHUBCLIENT_LL_04_001: [IoTHubClient_LL_CreateFromConnectionString shall verify the existence of key/value pair GatewayHostName. If it does exist it shall pass the value to IoTHubClient_LL_Create API.] */
                            else if (strcmp(s_token, PROTOCOL_GATEWAY_HOST) == 0)
                            {
                                protocolGateway = STRING_clone(valueString);
                                if (protocolGateway != NULL)
                                {
                                    config->protocolGatewayHostName = STRING_c_str(protocolGateway);
                                }
                            }
                        }
                    }
                }
                /* parsing is done - check the result */
                if (config->iotHubName == NULL)
                {
                    LogError("iotHubName is not found\r\n");
                }
                else if (config->iotHubSuffix == NULL)
                {
                    LogError("iotHubSuffix is not found\r\n");
                }
                else if (config->deviceId == NULL)
                {
                    LogError("deviceId is not found\r\n");
                }
                else if (config->deviceKey == NULL)
                {
                    LogError("deviceId is not found\r\n");
                }
                else
                {
                    /* SRS_IOTHUBCLIENT_LL_12_011: [IoTHubClient_LL_CreateFromConnectionString shall call into the IoTHubClient_LL_Create API with the current structure and returns with the return value of it] */
                    result = IoTHubClient_LL_Create(config);
                    if (result == NULL)
                    {
                        LogError("IoTHubClient_LL_Create failed\r\n");
                    }
                }
            }
            if (deviceKeyString != NULL)
                STRING_delete(deviceKeyString);
            if (deviceIdString != NULL)
                STRING_delete(deviceIdString);
            if (hostSuffixString != NULL)
                STRING_delete(hostSuffixString);
            if (hostNameString != NULL)
                STRING_delete(hostNameString);
            if (valueString != NULL)
                STRING_delete(valueString);
            if (tokenString != NULL)
                STRING_delete(tokenString);
            if (connString != NULL)
                STRING_delete(connString);
            if (protocolGateway != NULL)
                STRING_delete(protocolGateway);

            if (tokenizer1 != NULL)
                STRING_TOKENIZER_destroy(tokenizer1);

            free(config);
        }
    }
    return result;
}


IOTHUB_CLIENT_LL_HANDLE IoTHubClient_LL_Create(const IOTHUB_CLIENT_CONFIG* config)
{
    IOTHUB_CLIENT_LL_HANDLE result;
    /*Codes_SRS_IOTHUBCLIENT_LL_02_001: [IoTHubClient_LL_Create shall return NULL if config parameter is NULL or protocol field is NULL.]*/
    if (
        (config == NULL) ||
        (config->protocol == NULL)
        )
    {
        result = NULL;
        LogError("invalid configuration (NULL detected)\r\n");
    }
    else
    {
        IOTHUB_CLIENT_LL_HANDLE_DATA* handleData = (IOTHUB_CLIENT_LL_HANDLE_DATA*)malloc(sizeof(IOTHUB_CLIENT_LL_HANDLE_DATA));
        if (handleData == NULL)
        {
            LogError("malloc failed\r\n");
            result = NULL;
        }
        else
        {
            /*Codes_SRS_IOTHUBCLIENT_LL_02_004: [Otherwise IoTHubClient_LL_Create shall initialize a new DLIST (further called "waitingToSend") containing records with fields of the following types: IOTHUB_MESSAGE_HANDLE, IOTHUB_CLIENT_EVENT_CONFIRMATION_CALLBACK, void*.]*/
            IOTHUBTRANSPORT_CONFIG lowerLayerConfig;
            DList_InitializeListHead(&(handleData->waitingToSend));
            handleData->IoTHubTransport_SetOption = ((TRANSPORT_PROVIDER*)config->protocol())->IoTHubTransport_SetOption;
            handleData->IoTHubTransport_Create = ((TRANSPORT_PROVIDER*)config->protocol())->IoTHubTransport_Create;
            handleData->IoTHubTransport_Destroy = ((TRANSPORT_PROVIDER*)config->protocol())->IoTHubTransport_Destroy;
            handleData->IoTHubTransport_Subscribe = ((TRANSPORT_PROVIDER*)config->protocol())->IoTHubTransport_Subscribe;
            handleData->IoTHubTransport_Unsubscribe = ((TRANSPORT_PROVIDER*)config->protocol())->IoTHubTransport_Unsubscribe;
            handleData->IoTHubTransport_DoWork = ((TRANSPORT_PROVIDER*)config->protocol())->IoTHubTransport_DoWork;
            handleData->IoTHubTransport_GetSendStatus = ((TRANSPORT_PROVIDER*)config->protocol())->IoTHubTransport_GetSendStatus;
            handleData->messageCallback = NULL;
            handleData->messageUserContextCallback = NULL;
            handleData->lastMessageReceiveTime = INDEFINITE_TIME;
            /*Codes_SRS_IOTHUBCLIENT_LL_02_006: [IoTHubClient_LL_Create shall populate a structure of type IOTHUBTRANSPORT_CONFIG with the information from config parameter and the previous DLIST and shall pass that to the underlying layer _Create function.]*/
            lowerLayerConfig.upperConfig = config;
            lowerLayerConfig.waitingToSend = &(handleData->waitingToSend);
            /*Codes_SRS_IOTHUBCLIENT_LL_02_007: [If the underlaying layer _Create function fails them IoTHubClient_LL_Create shall fail and return NULL.] */
            if ((handleData->transportHandle = handleData->IoTHubTransport_Create(&lowerLayerConfig)) == NULL)
            {
                LogError("underlying transport failed\r\n");
                free(handleData);
                result = NULL;
            }
            else
            {
                /*Codes_SRS_IOTHUBCLIENT_LL_02_008: [Otherwise, IoTHubClient_LL_Create shall succeed and return a non-NULL handle.] */
                result = handleData;
            }
        }
    }

    return result;
}

void IoTHubClient_LL_Destroy(IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle)
{
    /*Codes_SRS_IOTHUBCLIENT_LL_02_009: [IoTHubClient_LL_Destroy shall do nothing if parameter iotHubClientHandle is NULL.]*/
    if (iotHubClientHandle != NULL)
    {
        PDLIST_ENTRY unsend;
        /*Codes_SRS_IOTHUBCLIENT_LL_02_010: [IoTHubClient_LL_Destroy it shall call the underlaying layer's _Destroy function and shall free the resources allocated by IoTHubClient (if any).] */
        IOTHUB_CLIENT_LL_HANDLE_DATA* handleData = (IOTHUB_CLIENT_LL_HANDLE_DATA*)iotHubClientHandle;
        handleData->IoTHubTransport_Destroy(handleData->transportHandle);
        /*if any, remove the items currently not send*/
        while ((unsend = DList_RemoveHeadList(&(handleData->waitingToSend))) != &(handleData->waitingToSend))
        {
            IOTHUB_MESSAGE_LIST* temp = containingRecord(unsend, IOTHUB_MESSAGE_LIST, entry);
            /*Codes_SRS_IOTHUBCLIENT_LL_02_033: [Otherwise, IoTHubClient_LL_Destroy shall complete all the event message callbacks that are in the waitingToSend list with the result IOTHUB_CLIENT_CONFIRMATION_BECAUSE_DESTROY.] */
            if (temp->callback != NULL)
            {
                temp->callback(IOTHUB_CLIENT_CONFIRMATION_BECAUSE_DESTROY, temp->context);
            }
            IoTHubMessage_Destroy(temp->messageHandle);
            free(temp);
        }
        free(handleData);
    }
}

IOTHUB_CLIENT_RESULT IoTHubClient_LL_SendEventAsync(IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle, IOTHUB_MESSAGE_HANDLE eventMessageHandle, IOTHUB_CLIENT_EVENT_CONFIRMATION_CALLBACK eventConfirmationCallback, void* userContextCallback)
{
    IOTHUB_CLIENT_RESULT result;
    /*Codes_SRS_IOTHUBCLIENT_LL_02_011: [IoTHubClient_LL_SendEventAsync shall fail and return IOTHUB_CLIENT_INVALID_ARG if parameter iotHubClientHandle or eventMessageHandle is NULL.]*/
    if (
        (iotHubClientHandle == NULL) || 
        (eventMessageHandle == NULL) ||
        /*Codes_SRS_IOTHUBCLIENT_LL_02_012: [IoTHubClient_LL_SendEventAsync shall fail and return IOTHUB_CLIENT_INVALID_ARG if parameter eventConfirmationCallback is NULL and userContextCallback is not NULL.] */
        ((eventConfirmationCallback == NULL) && (userContextCallback != NULL))
        )
    {
        result = IOTHUB_CLIENT_INVALID_ARG;
        LOG_ERROR;
    }
    else
    {
        IOTHUB_MESSAGE_LIST *newEntry = (IOTHUB_MESSAGE_LIST*)malloc(sizeof(IOTHUB_MESSAGE_LIST));
        if (newEntry == NULL)
        {
            result = IOTHUB_CLIENT_ERROR;
            LOG_ERROR;
        }
        else
        {
            /*Codes_SRS_IOTHUBCLIENT_LL_02_013: [IoTHubClient_SendEventAsync shall add the DLIST waitingToSend a new record cloning the information from eventMessageHandle, eventConfirmationCallback, userContextCallback.]*/
            if ((newEntry->messageHandle = IoTHubMessage_Clone(eventMessageHandle)) == NULL)
            {
                /*Codes_SRS_IOTHUBCLIENT_LL_02_014: [If cloning and/or adding the information fails for any reason, IoTHubClient_LL_SendEventAsync shall fail and return IOTHUB_CLIENT_ERROR.] */
                result = IOTHUB_CLIENT_ERROR;
                free(newEntry);
                LOG_ERROR;
            }
            else
            {
                IOTHUB_CLIENT_LL_HANDLE_DATA* handleData = (IOTHUB_CLIENT_LL_HANDLE_DATA*)iotHubClientHandle;
                /*Codes_SRS_IOTHUBCLIENT_LL_02_013: [IoTHubClient_SendEventAsync shall add the DLIST waitingToSend a new record cloning the information from eventMessageHandle, eventConfirmationCallback, userContextCallback.]*/
                newEntry->callback = eventConfirmationCallback;
                newEntry->context = userContextCallback;
                DList_InsertTailList(&(handleData->waitingToSend), &(newEntry->entry));
                /*Codes_SRS_IOTHUBCLIENT_LL_02_015: [Otherwise IoTHubClient_LL_SendEventAsync shall succeed and return IOTHUB_CLIENT_OK.] */
                result = IOTHUB_CLIENT_OK;
            }
        }
    }
    return result; 
}

IOTHUB_CLIENT_RESULT IoTHubClient_LL_SetMessageCallback(IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle, IOTHUB_CLIENT_MESSAGE_CALLBACK_ASYNC messageCallback, void* userContextCallback)
{
    IOTHUB_CLIENT_RESULT result;
    /*Codes_SRS_IOTHUBCLIENT_LL_02_016: [IoTHubClient_LL_SetMessageCallback shall fail and return IOTHUB_CLIENT_INVALID_ARG if parameter iotHubClientHandle is NULL.] */
    if (iotHubClientHandle == NULL)
    {
        result = IOTHUB_CLIENT_INVALID_ARG;
        LOG_ERROR;
    }
    else
    {
        IOTHUB_CLIENT_LL_HANDLE_DATA* handleData = (IOTHUB_CLIENT_LL_HANDLE_DATA*)iotHubClientHandle;
        if (messageCallback == NULL)
        {
            /*Codes_SRS_IOTHUBCLIENT_LL_02_019: [If parameter messageCallback is NULL then IoTHubClient_LL_SetMessageCallback shall call the underlying layer's _Unsubscribe function and return IOTHUB_CLIENT_OK.] */
            handleData->IoTHubTransport_Unsubscribe(handleData->transportHandle);
            handleData->messageCallback = NULL;
            handleData->messageUserContextCallback = NULL;
            result = IOTHUB_CLIENT_OK;
        }
        else
        {
            /*Codes_SRS_IOTHUBCLIENT_LL_02_017: [If parameter messageCallback is non-NULL then IoTHubClient_LL_SetMessageCallback shall call the underlying layer's _Subscribe function.]*/
            if (handleData->IoTHubTransport_Subscribe(handleData->transportHandle) == 0)
            {
                handleData->messageCallback = messageCallback;
                handleData->messageUserContextCallback = userContextCallback;
                result = IOTHUB_CLIENT_OK;
            }
            else
            {
                handleData->messageCallback = NULL;
                handleData->messageUserContextCallback = NULL;
                /*Codes_SRS_IOTHUBCLIENT_LL_02_018: [If the underlying layer's _Subscribe function fails, then IoTHubClient_LL_SetMessageCallback shall fail and return IOTHUB_CLIENT_ERROR. Otherwise IoTHubClient_LL_SetMessageCallback shall succeed and return IOTHUB_CLIENT_OK.]*/
                result = IOTHUB_CLIENT_ERROR;
            }
        }
    }
    
    return result;
}

void IoTHubClient_LL_DoWork(IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle)
{
    /*Codes_SRS_IOTHUBCLIENT_LL_02_020: [If parameter iotHubClientHandle is NULL then IoTHubClient_LL_DoWork shall not perform any action.] */
    if (iotHubClientHandle != NULL)
    {
        IOTHUB_CLIENT_LL_HANDLE_DATA* handleData = (IOTHUB_CLIENT_LL_HANDLE_DATA*)iotHubClientHandle;
        handleData->IoTHubTransport_DoWork(handleData->transportHandle, iotHubClientHandle);
    }
}

IOTHUB_CLIENT_RESULT IoTHubClient_LL_GetSendStatus(IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle, IOTHUB_CLIENT_STATUS *iotHubClientStatus)
{
    IOTHUB_CLIENT_RESULT result;

    /* Codes_SRS_IOTHUBCLIENT_09_007: [IoTHubClient_GetSendStatus shall return IOTHUB_CLIENT_INVALID_ARG if called with NULL parameter] */
    if (iotHubClientHandle == NULL || iotHubClientStatus == NULL)
    {
        result = IOTHUB_CLIENT_INVALID_ARG;
        LOG_ERROR;
    }
    else
    {
        IOTHUB_CLIENT_LL_HANDLE_DATA* handleData = (IOTHUB_CLIENT_LL_HANDLE_DATA*)iotHubClientHandle;

        /* Codes_SRS_IOTHUBCLIENT_09_008: [IoTHubClient_GetSendStatus shall return IOTHUB_CLIENT_OK and status IOTHUB_CLIENT_SEND_STATUS_IDLE if there is currently no items to be sent] */
        /* Codes_SRS_IOTHUBCLIENT_09_009: [IoTHubClient_GetSendStatus shall return IOTHUB_CLIENT_OK and status IOTHUB_CLIENT_SEND_STATUS_BUSY if there are currently items to be sent] */
        result = handleData->IoTHubTransport_GetSendStatus(handleData->transportHandle, iotHubClientStatus);
    }

    return result;
}

void IoTHubClient_LL_SendComplete(IOTHUB_CLIENT_LL_HANDLE handle, PDLIST_ENTRY completed, IOTHUB_BATCHSTATE_RESULT result)
{
    /*Codes_SRS_IOTHUBCLIENT_LL_02_022: [If parameter completed is NULL, or parameter handle is NULL then IoTHubClient_LL_SendBatch shall return.]*/
    if (
        (handle == NULL) || 
        (completed == NULL)
        )
    {
        /*"shall return"*/
        LogError("invalid arg\r\n");
    }
    else
    {
        /*Codes_SRS_IOTHUBCLIENT_LL_02_027: [If parameter result is IOTHUB_BACTHSTATE_FAILED then IoTHubClient_LL_SendComplete shall call all the non-NULL callbacks with the result parameter set to IOTHUB_CLIENT_CONFIRMATION_ERROR and the context set to the context passed originally in the SendEventAsync call.] */
        /*Codes_SRS_IOTHUBCLIENT_LL_02_025: [If parameter result is IOTHUB_BATCHSTATE_SUCCESS then IoTHubClient_LL_SendComplete shall call all the non-NULL callbacks with the result parameter set to IOTHUB_CLIENT_CONFIRMATION_OK and the context set to the context passed originally in the SendEventAsync call.]*/
        IOTHUB_CLIENT_CONFIRMATION_RESULT resultToBeCalled = (result == IOTHUB_BATCHSTATE_SUCCESS) ? IOTHUB_CLIENT_CONFIRMATION_OK : IOTHUB_CLIENT_CONFIRMATION_ERROR;
        PDLIST_ENTRY oldest;
        while((oldest= DList_RemoveHeadList(completed))!=completed)
        {
            IOTHUB_MESSAGE_LIST* messageList = (IOTHUB_MESSAGE_LIST*)containingRecord(oldest, IOTHUB_MESSAGE_LIST, entry);
            if (messageList->callback != NULL)
            {
                messageList->callback(resultToBeCalled, messageList->context);
            }
            IoTHubMessage_Destroy(messageList->messageHandle);
            free(messageList);
        }
    }
}

IOTHUBMESSAGE_DISPOSITION_RESULT IoTHubClient_LL_MessageCallback(IOTHUB_CLIENT_LL_HANDLE handle, IOTHUB_MESSAGE_HANDLE message)
{
    int result;
    /*Codes_SRS_IOTHUBCLIENT_LL_02_029: [If parameter handle is NULL then IoTHubClient_LL_MessageCallback shall return IOTHUBMESSAGE_ABANDONED.] */
    if (handle == NULL)
    {
        LogError("invalid argument\r\n");
        result = IOTHUBMESSAGE_ABANDONED;
    }
    else
    {
        IOTHUB_CLIENT_LL_HANDLE_DATA* handleData = (IOTHUB_CLIENT_LL_HANDLE_DATA*)handle;

        /* Codes_SRS_IOTHUBCLIENT_LL_09_004: [IoTHubClient_LL_GetLastMessageReceiveTime shall return lastMessageReceiveTime in localtime] */
        handleData->lastMessageReceiveTime = get_time(NULL);

        /*Codes_SRS_IOTHUBCLIENT_LL_02_030: [IoTHubClient_LL_MessageCallback shall invoke the last callback function (the parameter messageCallback to IoTHubClient_LL_SetMessageCallback) passing the message and the passed userContextCallback.]*/
        if (handleData->messageCallback != NULL)
        {
            result = handleData->messageCallback(message, handleData->messageUserContextCallback);
        }
        else
        {
            /*Codes_SRS_IOTHUBCLIENT_LL_02_032: [If the last callback function was NULL, then IoTHubClient_LL_MessageCallback  shall return IOTHUBMESSAGE_ABANDONED.] */
            LogError("user callback was NULL\r\n");
            result = IOTHUBMESSAGE_ABANDONED;
        }
    }
    /*Codes_SRS_IOTHUBCLIENT_LL_02_031: [Then IoTHubClient_LL_MessageCallback shall return what the user function returns.]*/
    return result;
}

IOTHUB_CLIENT_RESULT IoTHubClient_LL_GetLastMessageReceiveTime(IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle, time_t* lastMessageReceiveTime)
{
    IOTHUB_CLIENT_RESULT result;
    IOTHUB_CLIENT_LL_HANDLE_DATA* handleData = (IOTHUB_CLIENT_LL_HANDLE_DATA*)iotHubClientHandle;

    /* Codes_SRS_IOTHUBCLIENT_LL_09_001: [IoTHubClient_LL_GetLastMessageReceiveTime shall return IOTHUB_CLIENT_INVALID_ARG if any of the arguments is NULL] */
    if (handleData == NULL || lastMessageReceiveTime == NULL)
    {
        result = IOTHUB_CLIENT_INVALID_ARG;
        LOG_ERROR;
    }
    else
    {
        /* Codes_SRS_IOTHUBCLIENT_LL_09_002: [IoTHubClient_LL_GetLastMessageReceiveTime shall return IOTHUB_CLIENT_INDEFINITE_TIME - and not set 'lastMessageReceiveTime' - if it is unable to provide the time for the last commands] */
        if (handleData->lastMessageReceiveTime == INDEFINITE_TIME)
        {
            result = IOTHUB_CLIENT_INDEFINITE_TIME;
            LOG_ERROR;
        }
        else
        {
            /* Codes_SRS_IOTHUBCLIENT_LL_09_003: [IoTHubClient_LL_GetLastMessageReceiveTime shall return IOTHUB_CLIENT_OK if it wrote in the lastMessageReceiveTime the time when the last command was received] */
            /* Codes_SRS_IOTHUBCLIENT_LL_09_004: [IoTHubClient_LL_GetLastMessageReceiveTime shall return lastMessageReceiveTime in localtime] */
            *lastMessageReceiveTime = handleData->lastMessageReceiveTime;
            result = IOTHUB_CLIENT_OK;
        }
    }

    return result;
}

IOTHUB_CLIENT_RESULT IoTHubClient_LL_SetOption(IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle, const char* optionName, const void* value)
{
    
    IOTHUB_CLIENT_RESULT result;
    /*Codes_SRS_IOTHUBCLIENT_LL_02_034: [If iotHubClientHandle is NULL then IoTHubClient_LL_SetOption shall return IOTHUB_CLIENT_INVALID_ARG.]*/
    /*Codes_SRS_IOTHUBCLIENT_LL_02_035: [If optionName is NULL then IoTHubClient_LL_SetOption shall return IOTHUB_CLIENT_INVALID_ARG.] */
    /*Codes_SRS_IOTHUBCLIENT_LL_02_036: [If value is NULL then IoTHubClient_LL_SetOption shall return IOTHUB_CLIENT_INVALID_ARG.] */
    if (
        (iotHubClientHandle == NULL) ||
        (optionName == NULL) ||
        (value == NULL)
        )
    {
        result = IOTHUB_CLIENT_INVALID_ARG;
        LogError("invalid argument (NULL)\r\n");
    }
    else
    {
        IOTHUB_CLIENT_LL_HANDLE_DATA* handleData = (IOTHUB_CLIENT_LL_HANDLE_DATA*)iotHubClientHandle;

        /*Codes_SRS_IOTHUBCLIENT_LL_02_038: [Otherwise, IoTHubClient_LL shall call the function _SetOption of the underlying transport and return what that function is returning.] */
        result = handleData->IoTHubTransport_SetOption(handleData->transportHandle, optionName, value);

        if (result != IOTHUB_CLIENT_OK)
        {
            LogError("underlying transport failed, returned = %s\r\n", ENUM_TO_STRING(IOTHUB_CLIENT_RESULT, result));
        }

    }
    return result;
}
