// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#include "gballoc.h"

#include "iot_logging.h"
#include "strings.h"
#include "doublylinkedlist.h"
#include "crt_abstractions.h"

#include "iothub_client_ll.h"
#include "iothub_client_private.h"
#include "iothubtransportmqtt.h"
#include "mqttapi.h"


#define QOS         1

typedef struct MQTTTRANSPORT_HANDLE_DATA_TAG
{
    STRING_HANDLE hostAddressHandle;

    STRING_HANDLE device_id;

    STRING_HANDLE device_key;

	STRING_HANDLE sasTokenSr;
    
    DLIST_ENTRY waitingForAck;

    STRING_HANDLE mqttEventTopic;

    STRING_HANDLE mqttMessageTopic;

    MQTTAPI_HANDLE mqttApiInstance;
    
    MQTTAPI_TOPIC_HANDLE messageTopicHandle;

    bool connected;

    bool subscribed;

    PDLIST_ENTRY waitingToSend;

    IOTHUB_CLIENT_LL_HANDLE savedClientHandle;
} MQTTTRANSPORT_HANDLE_DATA, *PMQTTTRANSPORT_HANDLE_DATA;

typedef struct MQTT_MESSAGE_DETAILS_LIST_TAG
{
    IOTHUB_MESSAGE_LIST* iotHubMessageEntry;
    void* context;
    DLIST_ENTRY entry;
}MQTT_MESSAGE_DETAILS_LIST, *PMQTT_MESSAGE_DETAILS_LIST;

static void delivered(void* context, MQTTAPI_CONFIRMATION_RESULT result)
{
    /* Tests_SRS_IOTHUBTRANSPORTMQTT_04_045: [If context is NULL, IoTHubTransportMqtt delivery completed callback shall do nothing.]  */
    if (context != NULL)
    {
        PMQTT_MESSAGE_DETAILS_LIST sentMQTTMessage = context;
        PMQTTTRANSPORT_HANDLE_DATA transportState = sentMQTTMessage->context;

        if (transportState != NULL)
        {
            //First try to find the message on the list. 
            PDLIST_ENTRY currentListEntry;
            currentListEntry = transportState->waitingForAck.Flink;
            /* Codes_SRS_IOTHUBTRANSPORTMQTT_04_041: [IoTHubTransportMqtt shall search for the message received on context parameter to identify the message.]  */
            while (currentListEntry != &transportState->waitingForAck)
            {
                MQTT_MESSAGE_DETAILS_LIST *mqttMessageDetailEntry = containingRecord(currentListEntry, MQTT_MESSAGE_DETAILS_LIST, entry);
                DLIST_ENTRY savedFromCurrentListEntry;
                savedFromCurrentListEntry.Flink = currentListEntry->Flink;

                //Identify here if the message is the one arrived. 
                if (mqttMessageDetailEntry == context)
                {
                    /* Codes_SRS_IOTHUBTRANSPORTMQTT_04_042: [Once identified, IoTHubTransportMqtt shall remove the message from the WaitingforAck List.] */
                    (void)DList_RemoveEntryList(currentListEntry); //First remove the item from Waiting for Ack List.
                    DLIST_ENTRY messageCompleted;
                    DList_InitializeListHead(&messageCompleted);
                    DList_InsertHeadList(&messageCompleted, &(mqttMessageDetailEntry->iotHubMessageEntry->entry));

                    if (result == MQTTAPI_CONFIRMATION_OK)
                    {
                        /* Codes_SRS_IOTHUBTRANSPORTMQTT_04_043: [If result IoTHubTransportMqtt is MQTTAPI_CONFIRMATION_OK IoTHubTransportMqtt shall call IoTHubClient_LL_SendComplete passing the message and status IOTHUB_BATCHSTATE_SUCCESS.] */
                        IoTHubClient_LL_SendComplete(transportState->savedClientHandle, &messageCompleted, IOTHUB_BATCHSTATE_SUCCESS); //SendBatchComplete Frees the Message. 
                    }
                    else
                    {
                        /* Codes_SRS_IOTHUBTRANSPORTMQTT_04_044: [Otherwise, IoTHubTransportMqtt shall call IoTHubClient_LL_SendComplete passing the message and status IOTHUB_BATCHSTATE_FAIL.]  */
                        IoTHubClient_LL_SendComplete(transportState->savedClientHandle, &messageCompleted, IOTHUB_BATCHSTATE_FAILED); //SendBatchComplete Frees the Message. 
                    }
                    
                    break;
                }
                currentListEntry = savedFromCurrentListEntry.Flink;
            }			
        }

        free(sentMQTTMessage);
    }
}


static bool msgarrvd(void* context, const MQTTAPI_Message* message)
{
    int result;

    /* Codes_SRS_IOTHUBTRANSPORTMQTT_04_046: [If IoTHubTransportMqtt receives a call to MQTTAPI_messageArrived with message parameter or context NULL, it shall return false]  */
    if (context == NULL || message == NULL)
    {
        LogError("Received message, but without any content.\r\n");
        result = false;
    }
    else
    {
        /* Codes_SRS_IOTHUBTRANSPORTMQTT_04_047: [IotHubTransportMqtt shall Create an IoTHubMessage by calling IoTHubMessage_CreateFromByteArray when MQTTAPI_messageArrived is called.] */
        IOTHUB_MESSAGE_HANDLE IoTHubMessage;

        if ((IoTHubMessage = IoTHubMessage_CreateFromByteArray(message->payload, message->payloadlen)) == NULL)
        {
            /* Tests_SRS_IOTHUBTRANSPORTMQTT_04_048: [If the call to IoTHubMessage_CreateFromByteArray fails, IoTHubTransportMqtt – message arrived shall return false.]  */
            LogError("Failed to Create IoTHubMessage. \r\n");
            result = false;
        }
        else
        {
            PMQTTTRANSPORT_HANDLE_DATA state = context;
            /* Codes_SRS_IOTHUBTRANSPORTMQTT_04_051: [IoTHubTransportMqtt shall call IoTHubClient_LL_MessageCallback.]  */
            if (IoTHubClient_LL_MessageCallback(state->savedClientHandle, IoTHubMessage) != IOTHUBMESSAGE_ACCEPTED)
            {
                /* Codes_SRS_IOTHUBTRANSPORTMQTT_04_052: [If this calls fails, it shall return false] */
                LogError("Noficiation not accepted by our client. \r\n");
                result = false;
            }
            else
            {
                /* Codes_SRS_IOTHUBTRANSPORTMQTT_04_053: [Otherwise it shall return true.] */
                result = true;
            }
        }

        if (IoTHubMessage != NULL)
        {
            IoTHubMessage_Destroy(IoTHubMessage);
        }
    }


    return result;
}

static PMQTTTRANSPORT_HANDLE_DATA mqttClientTransportState_Create(STRING_HANDLE hostAddress, const char* iothubName, const char* iotHubSuffix, const char* deviceId, const char* deviceKey, PDLIST_ENTRY waitingToSend)
{
    PMQTTTRANSPORT_HANDLE_DATA state;
    state = (PMQTTTRANSPORT_HANDLE_DATA)malloc(sizeof(MQTTTRANSPORT_HANDLE_DATA));
    if (state == NULL)
    {
        /* Codes_SRS_IOTHUBTRANSPORTMQTT_04_010: [If memory allocation fails IoTHubTransportMqtt_Create shall fail and return NULL.] */
        LogError("Could not create MQTT transport state. Memory allocation failed.\r\n");
    }
    /* Codes_SRS_IOTHUBTRANSPORTMQTT_04_009: [IoTHubTransportMqtt_Create shall allocate memory to save its internal state where all topics, hostname, device_id, device_key, sasTokenSr and client handle shall be saved.]  */
    else if ((state->device_id = STRING_construct(deviceId)) == NULL)
    {
        LogError("Could not create device id for MQTT\r\n");
        free(state);
        state = NULL;
    }
    /* Codes_SRS_IOTHUBTRANSPORTMQTT_04_009: [IoTHubTransportMqtt_Create shall allocate memory to save its internal state where all topics, hostname, device_id, device_key, sasTokenSr and client handle shall be saved.]  */
    else if ((state->device_key = STRING_construct(deviceKey)) == NULL)
    {
        LogError("Could not create device key for MQTT\r\n");
        STRING_delete(state->device_id);
        free(state);
        state = NULL;
    }
	/* Codes_SRS_IOTHUBTRANSPORTMQTT_04_009: [IoTHubTransportMqtt_Create shall allocate memory to save its internal state where all topics, hostname, device_id, device_key, sasTokenSr and client handle shall be saved.]  */
	else if ((state->sasTokenSr = STRING_construct(iothubName)) == NULL ||
		     (STRING_concat(state->sasTokenSr, ".")) != 0 ||
		     (STRING_concat(state->sasTokenSr, iotHubSuffix)) != 0 || 
		     (STRING_concat(state->sasTokenSr, "/devices/")) != 0 || 
		     (STRING_concat(state->sasTokenSr, deviceId)) != 0)
	{
		LogError("Could not create Sas Token Sr String.\r\n");
		STRING_delete(state->sasTokenSr);
		STRING_delete(state->device_key);
		STRING_delete(state->device_id);
		free(state);
		state = NULL;
	}
	/* Codes_SRS_IOTHUBTRANSPORTMQTT_04_005: [Otherwise, IoTHubTransportMqtt_Create shall create an immutable string (further called “Event mqttEventTopic”) that will be formed by “devices/<deviceId>/messages/events”.] */
	else if ((state->mqttEventTopic = STRING_construct("devices/")) == NULL ||
        (STRING_concat(state->mqttEventTopic, deviceId)) != 0 ||
        (STRING_concat(state->mqttEventTopic, "/messages/events")) != 0)
    {
        /* Codes_SRS_IOTHUBTRANSPORTMQTT_04_006: [If creating the string fail for any reason then IoTHubTransportMqtt_Create shall fail and return NULL.] */
        LogError("Could not create mqttEventTopic for MQTT\r\n");
		STRING_delete(state->sasTokenSr);
        STRING_delete(state->device_key);
        STRING_delete(state->device_id);
        STRING_delete(state->mqttEventTopic);
        free(state);
        state = NULL;
    }
	/* Codes_SRS_IOTHUBTRANSPORTMQTT_04_007: [Otherwise, IoTHubTransportMqtt_Create shall create an immutable string(further called “Event mqttMessageTopic”) that will be formed by “devices/<deviceId>/messages/devicebound”.] */
	else if ((state->mqttMessageTopic = STRING_construct("devices/")) == NULL||
		(STRING_concat(state->mqttMessageTopic, deviceId)) != 0 ||
		(STRING_concat(state->mqttMessageTopic, "/messages/devicebound")) != 0)
    {
        /* Codes_SRS_IOTHUBTRANSPORTMQTT_04_008: [If creating the string fail for any reason then IoTHubTransportMqtt_Create shall fail and return NULL.] */
        LogError("Could not create mqttMessageTopic for MQTT\r\n");
        STRING_delete(state->mqttEventTopic);
		STRING_delete(state->sasTokenSr);
        STRING_delete(state->device_key);
        STRING_delete(state->device_id);
        STRING_delete(state->mqttMessageTopic);
        free(state);
        state = NULL;
    }
    else
    {
        /* CodesSRS_IOTHUBTRANSPORTMQTT_04_011: [IoTHubTransportMqtt_Create shall call MQTTAPI_Create passing MQTTAPI_connectOptions (deviceId, deviceKey and HostAddress).] */
		MQTTAPI_ConnectOptions connOpt;
		connOpt.deviceId = STRING_c_str(state->device_id);
		connOpt.deviceKey = STRING_c_str(state->device_key);
		connOpt.serverURI = STRING_c_str(hostAddress);
		connOpt.sasTokenSr = STRING_c_str(state->sasTokenSr);

		if ((state->mqttApiInstance = MQTTAPI_Create(&connOpt)) == NULL)
        {
            /* Codes_SRS_IOTHUBTRANSPORTMQTT_04_012: [If MQTTAPI_Create fail, than IoTHubTransportMqtt_Create shall fail and return NULL.]  */
            LogError("Could not create MQTTAPI instance.\r\n");
            STRING_delete(state->mqttEventTopic);
            STRING_delete(state->mqttMessageTopic);
			STRING_delete(state->sasTokenSr);
            STRING_delete(state->device_key);
            STRING_delete(state->device_id);
            free(state);
            state = NULL;
        }
        /* Codes_SRS_IOTHUBTRANSPORTMQTT_04_025: [IoTHubTransportMqtt_Create shall call MQTTAPI_SetMessageCallback and MQTTAPI_SetDeliveryCompletedCallback for message arrived and message delivered (Answer to Event).] */
		else if (MQTTAPI_SetMessageCallback(state->mqttApiInstance, state, msgarrvd) != MQTTAPI_OK)
        {
            /* Codes_SRS_IOTHUBTRANSPORTMQTT_04_026: [If MQTTAPI_SetCallbaks fails, IoTHubTransportMqtt_Create shall fail and return NULL.]  */
            LogError("Could not set message callback.\r\n");
            STRING_delete(state->mqttEventTopic);
            STRING_delete(state->mqttMessageTopic);
			STRING_delete(state->sasTokenSr);
            STRING_delete(state->device_key);
            STRING_delete(state->device_id);
            free(state);
            state = NULL;
        }
        /* Codes_SRS_IOTHUBTRANSPORTMQTT_04_025: [IoTHubTransportMqtt_Create shall call MQTTAPI_SetMessageCallback and MQTTAPI_SetDeliveryCompletedCallback for message arrived and message delivered (Answer to Event).] */
		else if (MQTTAPI_SetDeliveryCompletedCallback(state->mqttApiInstance, delivered) != MQTTAPI_OK)
        {
            /* Codes_SRS_IOTHUBTRANSPORTMQTT_04_026: [If MQTTAPI_SetCallbaks fails, IoTHubTransportMqtt_Create shall fail and return NULL.]  */
            LogError("Could not set event confirmation callback.\r\n");
            STRING_delete(state->mqttEventTopic);
            STRING_delete(state->mqttMessageTopic);
			STRING_delete(state->sasTokenSr);
            STRING_delete(state->device_key);
            STRING_delete(state->device_id);
            free(state);
            state = NULL;
        }
        else
        {
            state->hostAddressHandle = hostAddress;
            state->connected = false;
            state->subscribed = false;
            state->waitingToSend = waitingToSend;
            state->savedClientHandle = NULL;
			state->messageTopicHandle = NULL;
            DList_InitializeListHead(&(state->waitingForAck));
        }
    }

    return state;
}



extern TRANSPORT_HANDLE IoTHubTransportMqtt_Create(const IOTHUBTRANSPORT_CONFIG* config)
{
    
    PMQTTTRANSPORT_HANDLE_DATA result;
    size_t deviceIdSize;
    STRING_HANDLE hostAddress;
    
    /* Codes_SRS_IOTHUBTRANSPORTMQTT_04_001: [If parameter config is NULL then IoTHubTransportMqtt_Create shall fail and return NULL.]  */
    if (config == NULL)
    {
        LogError("Config Parameter is NULL.\r\n");
        result = NULL;
    }
    /* Codes_SRS_IOTHUBTRANSPORTMQTT_04_002: [IoTHubTransportMqtt_Create shall fail and return NULL if any fields of the config structure are NULL.]  */
    else if (config->upperConfig == NULL)
    {
        LogError("invalid arg (upperConfig is NULL)\r\n");
        result = NULL;
    }
    /* Codes_SRS_IOTHUBTRANSPORTMQTT_04_002: [IoTHubTransportMqtt_Create shall fail and return NULL if any fields of the config structure are NULL.]  */
    else if (config->waitingToSend == NULL)
    {
        LogError("invalid arg (waitingToSend is NULL)\r\n");
        result = NULL;
    }
    /* Codes_SRS_IOTHUBTRANSPORTMQTT_04_002: [IoTHubTransportMqtt_Create shall fail and return NULL if any fields of the config structure are NULL.]  */
    else if (config->upperConfig->protocol == NULL)
    {
        LogError("invalid arg (protocol is NULL)\r\n");
        result = NULL;
    }
    /* Codes_SRS_IOTHUBTRANSPORTMQTT_04_002: [IoTHubTransportMqtt_Create shall fail and return NULL if any fields of the config structure are NULL.]  */
    else if (config->upperConfig->deviceId == NULL)
    {
        LogError("invalid arg (deviceId is NULL)\r\n");
        result = NULL;
    }
    /* Codes_SRS_IOTHUBTRANSPORTMQTT_04_002: [IoTHubTransportMqtt_Create shall fail and return NULL if any fields of the config structure are NULL.]  */
    else if (config->upperConfig->deviceKey == NULL)
    {
        LogError("invalid arg (deviceKey is NULL)\r\n");
        result = NULL;
    }
    /* Codes_SRS_IOTHUBTRANSPORTMQTT_04_002: [IoTHubTransportMqtt_Create shall fail and return NULL if any fields of the config structure are NULL.]  */
    else if (config->upperConfig->iotHubName == NULL)
    {
        LogError("invalid arg (iotHubName is NULL)\r\n");
        result = NULL;
    }
    /* Codes_SRS_IOTHUBTRANSPORTMQTT_04_002: [IoTHubTransportMqtt_Create shall fail and return NULL if any fields of the config structure are NULL.]  */
    else if (config->upperConfig->iotHubSuffix == NULL)
    {
        LogError("invalid arg (iotHubSuffix is NULL)\r\n");
        result = NULL;
    }
    /* Codes_SRS_IOTHUBTRANSPORTMQTT_04_002: [IoTHubTransportMqtt_Create shall fail and return NULL if any fields of the config structure are NULL.]  */
    else if (config->upperConfig->protocolGatewayHostName == NULL)
    {
        LogError("invalid arg (protocolGatewayHostName is NULL)\r\n");
        result = NULL;
    }
    /* Codes_SRS_IOTHUBTRANSPORTMQTT_04_003: [IoTHubTransportMqtt_Create shall fail and return NULL if the configuration deviceId is greater than 128 ascii characters.]  */
    else if ((deviceIdSize = strlen(config->upperConfig->deviceId)) > 128U)
    {
        LogError("invalid arg (device Id is longer than 128 ASCII Characters)\r\n");
        result = NULL;
    }
    /* Codes_SRS_IOTHUBTRANSPORTMQTT_04_004: [IoTHubTransportMqtt_Create shall fail and return NULL if the configuration parameters deviceId, deviceKey, IoTHubName and protocolGatewayHostName are an empty string in size.]  */
    else if ((deviceIdSize == 0))
    {
        LogError("invalid arg (device Id is empty)\r\n");
        result = NULL;
    }
    /* Codes_SRS_IOTHUBTRANSPORTMQTT_04_004: [IoTHubTransportMqtt_Create shall fail and return NULL if the configuration parameters deviceId, deviceKey, IoTHubName and protocolGatewayHostName are an empty string in size.]  */
    else if (strlen(config->upperConfig->deviceKey) == 0)
    {
        LogError("invalid arg (device key is empty)\r\n");
        result = NULL;
    }
    /* Codes_SRS_IOTHUBTRANSPORTMQTT_04_004: [IoTHubTransportMqtt_Create shall fail and return NULL if the configuration parameters deviceId, deviceKey, IoTHubName and protocolGatewayHostName are an empty string in size.]  */
    else if (strlen(config->upperConfig->iotHubName) == 0)
    {
        LogError("invalid arg (iotHubName is empty)\r\n");
        result = NULL;
    }
    /* Codes_SRS_IOTHUBTRANSPORTMQTT_04_004: [IoTHubTransportMqtt_Create shall fail and return NULL if the configuration parameters deviceId, deviceKey, IoTHubName and protocolGatewayHostName are an empty string in size.]  */
    else if (strlen(config->upperConfig->protocolGatewayHostName) == 0)
    {
        LogError("invalid arg (protocolGatewayHostName is empty)\r\n");
        result = NULL;
    }
    /* Codes_SRS_IOTHUBTRANSPORTMQTT_04_009: [IoTHubTransportMqtt_Create shall allocate memory to save its internal state where all topics, hostname, device_id, device_key and client handle shall be saved.] */
    else if ((hostAddress = STRING_construct(config->upperConfig->protocolGatewayHostName)) == NULL)
    {
        LogError("Failed to construct MQTT address for Event.\r\n");
        result = NULL;
    }
    /* Codes_SRS_IOTHUBTRANSPORTMQTT_04_013: [Otherwise, IoTHubTransportMqtt_Create shall succeed and return a non-NULL value]  */
    else if ((result = mqttClientTransportState_Create(hostAddress, config->upperConfig->iotHubName, config->upperConfig->iotHubSuffix, config->upperConfig->deviceId, config->upperConfig->deviceKey, config->waitingToSend)) == NULL)
    {
        STRING_delete(hostAddress);
        LogError("Failed to allocate the state for the mqtt transport\r\n");
    }

    return result;
}

extern void IoTHubTransportMqtt_Destroy(TRANSPORT_HANDLE handle)
{
    PMQTTTRANSPORT_HANDLE_DATA transportState = (PMQTTTRANSPORT_HANDLE_DATA)handle;

    /* Codes_SRS_IOTHUBTRANSPORTMQTT_04_014: [IoTHubTransportMqtt_Destroy shall do nothing if parameter handle is NULL.] */
    if (transportState != NULL)
    {
        /* Codes_SRS_IOTHUBTRANSPORTMQTT_04_015: [Otherwise IoTHubTransportMqtt_Destroy shall free all the resources currently in use.]  */
        if (transportState->subscribed)
        {
            IoTHubTransportMqtt_Unsubscribe(handle);
        }
        //Empty the Waiting for Ack Messages.
        while (!DList_IsListEmpty(&transportState->waitingForAck))
        {
            DLIST_ENTRY messageCompleted;
            PDLIST_ENTRY currentEntry = DList_RemoveHeadList(&transportState->waitingForAck);

            MQTT_MESSAGE_DETAILS_LIST* mqttMessageEntry = containingRecord(currentEntry, MQTT_MESSAGE_DETAILS_LIST, entry);

            DList_InitializeListHead(&messageCompleted);
            DList_InsertTailList(&messageCompleted, &(mqttMessageEntry->iotHubMessageEntry->entry));
            IoTHubClient_LL_SendComplete(transportState->savedClientHandle, &messageCompleted, IOTHUB_BATCHSTATE_FAILED);
            free(mqttMessageEntry);
        }

        STRING_delete(transportState->mqttEventTopic);
        STRING_delete(transportState->mqttMessageTopic);
        STRING_delete(transportState->device_id);
        STRING_delete(transportState->device_key);
        STRING_delete(transportState->hostAddressHandle);
		STRING_delete(transportState->sasTokenSr);

		MQTTAPI_Destroy(transportState->mqttApiInstance);
        free(transportState);
    }
}

extern int IoTHubTransportMqtt_Subscribe(TRANSPORT_HANDLE handle)
{
    int result;
    PMQTTTRANSPORT_HANDLE_DATA transportState = (PMQTTTRANSPORT_HANDLE_DATA)handle;
        
    if (transportState == NULL)
    {
        /* Codes_SRS_IOTHUBTRANSPORTMQTT_04_016: [If parameter handle is NULL than IoTHubTransportMqtt_Subscribe shall fail and return a non-zero value.] */
        LogError("Invalid handle parameter. NULL.\r\n");
        result = __LINE__;
    }
    /* Codes_SRS_IOTHUBTRANSPORTMQTT_04_017: [Otherwise, IoTHubTransportMqtt_Subscribe shall call MQTTAPI_Subscribe.] */
	else if ((transportState->messageTopicHandle = MQTTAPI_Subscribe(transportState->mqttApiInstance, STRING_c_str(transportState->mqttMessageTopic))) == NULL)
    {
        /* Codes_SRS_IOTHUBTRANSPORTMQTT_04_019: [Otherwise, IoTHubTransportMqtt_Subscribe shall fail and return a non-zero value.]  */
        LogError("MATTAPI_subscribe failed.\r\n");
        result = __LINE__;
    }
    else
    {
        /* Codes_SRS_IOTHUBTRANSPORTMQTT_04_018: [If MQTTAPI_Subscribe succeeed, IoTHubTransportMqtt_Subscribe shall succeed.] */
        transportState->subscribed = true;
        result = 0;
    }


    return result;
}

extern void IoTHubTransportMqtt_Unsubscribe(TRANSPORT_HANDLE handle)
{
    PMQTTTRANSPORT_HANDLE_DATA transportState = (PMQTTTRANSPORT_HANDLE_DATA)handle;

    /* Codes_SRS_IOTHUBTRANSPORTMQTT_04_020: [If parameter handle is NULL then IoTHubTransportMqtt_Unsubscribe shall do nothing.]  */
    if (transportState != NULL)
    {
        /* Codes_SRS_IOTHUBTRANSPORTMQTT_04_021: [Otherwise it shall call MQTTAPI_Unsubscribe.]  */
		MQTTAPI_Unsubscribe(transportState->messageTopicHandle);
        transportState->subscribed = false;
    }
}

extern void IoTHubTransportMqtt_DoWork(TRANSPORT_HANDLE handle, IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle)
{
    PMQTTTRANSPORT_HANDLE_DATA transportState = (PMQTTTRANSPORT_HANDLE_DATA)handle;

    /* Codes_SRS_IOTHUBTRANSPORTMQTT_04_022: [If parameter handle is NULL or parameter iotHubClientHandle then IoTHubTransportMqtt_DoWork shall immediately return.]  */
    if (transportState != NULL && iotHubClientHandle != NULL)
    {
        transportState->savedClientHandle = iotHubClientHandle;
        /* Codes_SRS_IOTHUBTRANSPORTMQTT_04_027: [IoTHubTransportMqtt_DoWork shall inspect the “waitingToSend” DLIST passed in config structure.]  */
        PDLIST_ENTRY currentListEntry;
        currentListEntry = transportState->waitingToSend->Flink;

        /* Codes_SRS_IOTHUBTRANSPORTMQTT_04_028: [If the list is empty then IoTHubTransportMqtt_DoWork shall proceed to the following action.] */
        while (currentListEntry != transportState->waitingToSend)
        {
            IOTHUB_MESSAGE_LIST *iotHubMessageEntry = containingRecord(currentListEntry, IOTHUB_MESSAGE_LIST, entry);
            DLIST_ENTRY savedFromCurrentListEntry;
            const unsigned char* messagePayload;
            size_t messageLength;
            IOTHUBMESSAGE_CONTENT_TYPE contentType = IoTHubMessage_GetContentType(iotHubMessageEntry->messageHandle);

            savedFromCurrentListEntry.Flink = currentListEntry->Flink;

            /* Codes_SRS_IOTHUBTRANSPORTMQTT_04_031: [If the list has at least one entry IotHubTransportMQtt_DoWork shall call IotHubMessage_GetData to receive message content and size.]*/
            /*Codes_SRS_IOTHUBTRANSPORTMQTT_02_001: [If the message type is IOTHUBMESSAGE_STRING then IotHubTransportMQtt_DoWork shall call IotHubMessage_GetString to receive message content and shall compute the size as strlen.] */
            if (!(
                ((contentType == IOTHUBMESSAGE_BYTEARRAY) && (IoTHubMessage_GetByteArray(iotHubMessageEntry->messageHandle, &messagePayload, &messageLength) == IOTHUB_MESSAGE_OK)) ||
                ((contentType == IOTHUBMESSAGE_STRING) && (
                    messagePayload = (const unsigned char*)IoTHubMessage_GetString(iotHubMessageEntry->messageHandle),
                    (messageLength = (messagePayload == NULL) ? 0 : strlen((const char*)messagePayload)),
                    messagePayload != NULL)
                    )
                ))
            {
                /* Codes_SRS_IOTHUBTRANSPORTMQTT_04_032: [If IoTHubMessage_GetDatafail, IoTHubTransportMqtt_DoWork shall go to the next item on the list or move to the next action.]  */
                LogError("Failure result from IoTHubMessage_GetData\r\n");
            }
            else
            {
                MQTT_MESSAGE_DETAILS_LIST *mqttMessageEntry = (MQTT_MESSAGE_DETAILS_LIST *)malloc(sizeof(MQTT_MESSAGE_DETAILS_LIST));

                if (mqttMessageEntry == NULL)
                {
                    LogError("Failed to create a MQTT Detail List Entry. \r\n.");
                }
                else
                {
					/* Codes_SRS_IOTHUBTRANSPORTMQTT_04_033: [Otherwise IoTHubTransportMqtt_DoWork shall call MQTTAPI_PublishMessage, passing the address of the message as a context parameter.]  */
					MQTTAPI_Message pubmsg; 
                    pubmsg.payload = (void*)messagePayload;
                    pubmsg.payloadlen = messageLength;
                    mqttMessageEntry->iotHubMessageEntry = iotHubMessageEntry;
                    mqttMessageEntry->context = transportState;

                    (void)(DList_RemoveEntryList(currentListEntry));

					if (MQTTAPI_PublishMessage(transportState->mqttApiInstance, STRING_c_str(transportState->mqttEventTopic), &pubmsg, mqttMessageEntry) != MQTTAPI_OK)
                    {
						/* Codes_SRS_IOTHUBTRANSPORTMQTT_04_034: [If MQTTAPI_PublishMessage fails, IoTHubTransportMqtt_DoWork shall call IoTHubClient_LL_SendComplete with status of IOTHUB_BATCHSTATE_FAILED.] */
                        DLIST_ENTRY messageCompleted;
                        DList_InitializeListHead(&messageCompleted);
                        DList_InsertTailList(&messageCompleted, &(mqttMessageEntry->iotHubMessageEntry->entry));
                        IoTHubClient_LL_SendComplete(transportState->savedClientHandle, &messageCompleted, IOTHUB_BATCHSTATE_FAILED); //SendBatchComplete Frees the Message. 
                        free(mqttMessageEntry);
                    }
                    else
                    {
                        /* Codes_SRS_IOTHUBTRANSPORTMQTT_04_030: [Otherwise the message shall be removed from waitingToSend and moved to a list where it will be waiting for receiving an ACK from Mqtt server(IoTHub or ProtocolGateway)] */
                        DList_InsertTailList(&(transportState->waitingForAck), &(mqttMessageEntry->entry));
                    }
                }
            }

            currentListEntry = savedFromCurrentListEntry.Flink;
        }

        /* Codes_SRS_IOTHUBTRANSPORTMQTT_04_040: [IoTHubTransportMqtt_DoWork shall call MQTTAPI_DoWork everytime it is called, once.] */
		MQTTAPI_DoWork(transportState->mqttApiInstance);
    }
}

IOTHUB_CLIENT_RESULT IoTHubTransportMqtt_GetSendStatus(TRANSPORT_HANDLE handle, IOTHUB_CLIENT_STATUS *iotHubClientStatus)
{
    IOTHUB_CLIENT_RESULT result;

    /* Codes_SRS_IOTHUBTRANSPORTMQTT_04_035: [IoTHubTransportMqtt_GetSendStatus shall return IOTHUB_CLIENT_INVALID_ARG if called with NULL parameter.] */
    if (handle == NULL)
    {
        LogError("invalid arument. handle is NULL. \r\n");
        result = IOTHUB_CLIENT_INVALID_ARG;
    }
    /* Codes_SRS_IOTHUBTRANSPORTMQTT_04_035: [IoTHubTransportMqtt_GetSendStatus shall return IOTHUB_CLIENT_INVALID_ARG if called with NULL parameter.] */
    else if (iotHubClientStatus == NULL)
    {
        LogError("invalid arument. iotHubClientStatus is NULL. \r\n");
        result = IOTHUB_CLIENT_INVALID_ARG;
    }
    else
    {
        MQTTTRANSPORT_HANDLE_DATA* handleData = (MQTTTRANSPORT_HANDLE_DATA*)handle;
        if (!DList_IsListEmpty(handleData->waitingToSend) || !DList_IsListEmpty(&(handleData->waitingForAck)))
        {
            /* Codes_SRS_IOTHUBTRANSPORTMQTT_04_037: [IoTHubTransportMqtt_GetSendStatus shall return IOTHUB_CLIENT_OK and status IOTHUB_CLIENT_SEND_STATUS_BUSY if there are currently event items to be sent or being sent.]  */
            *iotHubClientStatus = IOTHUB_CLIENT_SEND_STATUS_BUSY;
        }
        else
        {
            /* Codes_SRS_IOTHUBTRANSPORTMQTT_04_036: [IoTHubTransportMqtt_GetSendStatus shall return IOTHUB_CLIENT_OK and status IOTHUB_CLIENT_SEND_STATUS_IDLE if there are currently no event items to be sent or being sent.] */
            *iotHubClientStatus = IOTHUB_CLIENT_SEND_STATUS_IDLE;
        }

        result = IOTHUB_CLIENT_OK;
    }

    return result;
}

IOTHUB_CLIENT_RESULT IoTHubTransportMqtt_SetOption(TRANSPORT_HANDLE handle, const char* option, const void* value)
{
    (void)(handle, option, value);
    /* Codes_SRS_IOTHUBTRANSPORTMQTT_04_038: [IoTHubTransportMqtt_SetOption shall return IOTHUB_CLIENT_ERROR , since it does not support any extra parameter today.]  */
    return IOTHUB_CLIENT_ERROR;
}

/* Codes_SRS_IOTHUBTRANSPORTMQTT_04_039: [This function shall return a pointer to a structure of type TRANSPORT_PROVIDER having the following values for it’s fields:] */
TRANSPORT_PROVIDER myfunc = {
    IoTHubTransportMqtt_SetOption,
    IoTHubTransportMqtt_Create, IoTHubTransportMqtt_Destroy, IoTHubTransportMqtt_Subscribe, IoTHubTransportMqtt_Unsubscribe, IoTHubTransportMqtt_DoWork, IoTHubTransportMqtt_GetSendStatus
};

extern const void* MQTT_Protocol(void)
{
    return &myfunc;
}
