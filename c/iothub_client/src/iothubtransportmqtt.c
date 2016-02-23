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
#include "mqtt_client.h"
#include "sastoken.h"
#include "tickcounter.h"

#include "tlsio.h"
#include "platform.h"

#include "iothub_client_version.h"

#include <stdarg.h>
#include <stdio.h>

#define SAS_TOKEN_DEFAULT_LIFETIME  3600
#define EPOCH_TIME_T_VALUE          0
#define DEFAULT_MQTT_KEEPALIVE      20
#define DEFAULT_PORT_NUMBER         8883
#define DEFAULT_TEMP_STRING_LEN     256
#define BUILD_CONFIG_USERNAME       24
#define EVENT_TOPIC_DEFAULT_LEN     27
#define SAS_TOKEN_DEFAULT_LEN       10
#define RESEND_TIMEOUT_VALUE_MIN    1*60
#define MAX_SEND_RECOUNT_LIMIT      2

TICK_COUNTER_HANDLE g_msgTickCounter;

typedef struct MQTTTRANSPORT_HANDLE_DATA_TAG
{
    STRING_HANDLE device_id;
    STRING_HANDLE device_key;
    STRING_HANDLE sasTokenSr;
    STRING_HANDLE mqttEventTopic;
    STRING_HANDLE mqttMessageTopic;
    STRING_HANDLE hostAddress;
    // The current mqtt iothub implementation requires that the hub name and the domain suffix be passed as the first of a series of segments
    // passed through the username portion of the connection frame.
    // The second segment will contain the device id.  The two segments are delemited by a "/".
    // The first segment can be a maximum 256 characters.
    // The second segment can be a maximum 128 characters.
    // With the / delimeter you have 384 chars (Plus a terminator of 0).
    STRING_HANDLE configPassedThroughUsername;
    int portNum;
    MQTT_CLIENT_HANDLE mqttClient;
    uint16_t packetId;
    bool connected;
    bool subscribed;
    bool destroyCalled;
    DLIST_ENTRY waitingForAck;
    PDLIST_ENTRY waitingToSend;
    IOTHUB_CLIENT_LL_HANDLE llClientHandle;
    CONTROL_PACKET_TYPE currPacketState;
    XIO_HANDLE xioTransport;
} MQTTTRANSPORT_HANDLE_DATA, *PMQTTTRANSPORT_HANDLE_DATA;

typedef struct MQTT_MESSAGE_DETAILS_LIST_TAG
{
    uint64_t msgPublishTime;
    size_t retryCount;
    IOTHUB_MESSAGE_LIST* iotHubMessageEntry;
    void* context;
    uint16_t msgPacketId;
    DLIST_ENTRY entry;
} MQTT_MESSAGE_DETAILS_LIST, *PMQTT_MESSAGE_DETAILS_LIST;

static void defaultPrintLogFunction(unsigned int options, char* format, ...)
{
    va_list args;
    va_start(args, format);
    (void)vprintf(format, args);
    va_end(args);

    if (options & LOG_LINE)
    {
        (void)printf("\r\n");
    }
}

static void sendMsgComplete(IOTHUB_MESSAGE_LIST* iothubMsgList, PMQTTTRANSPORT_HANDLE_DATA transportState, IOTHUB_BATCHSTATE_RESULT batchResult)
{
    DLIST_ENTRY messageCompleted;
    DList_InitializeListHead(&messageCompleted);
    DList_InsertTailList(&messageCompleted, &(iothubMsgList->entry));
    IoTHubClient_LL_SendComplete(transportState->llClientHandle, &messageCompleted, batchResult);
}

static int publishMqttMessage(PMQTTTRANSPORT_HANDLE_DATA transportState, MQTT_MESSAGE_DETAILS_LIST* mqttMsgEntry, const unsigned char* payload, size_t len)
{
    int result;
    MQTT_MESSAGE_HANDLE mqttMsg = mqttmessage_create(transportState->packetId++, STRING_c_str(transportState->mqttEventTopic), DELIVER_AT_LEAST_ONCE, payload, len);
    if (mqttMsg == NULL)
    {
        result = __LINE__;
    }
    else
    {
        if (mqtt_client_publish(transportState->mqttClient, mqttMsg) != 0)
        {
            result = __LINE__;
        }
        else
        {
            mqttMsgEntry->retryCount++;
            (void)tickcounter_get_current_ms(g_msgTickCounter, &mqttMsgEntry->msgPublishTime);
            result = 0;
        }
        mqttmessage_destroy(mqttMsg);
    }
    return result;
}

static void MqttRecvCallback(MQTT_MESSAGE_HANDLE msgHandle, void* callbackCtx)
{
    if (msgHandle != NULL && callbackCtx != NULL)
    {
        const APP_PAYLOAD* appPayload = mqttmessage_getApplicationMsg(msgHandle);
        IOTHUB_MESSAGE_HANDLE IoTHubMessage = IoTHubMessage_CreateFromByteArray(appPayload->message, appPayload->length);
        if (IoTHubMessage == NULL)
        {
            LogError("IotHub Message creation has failed.\r\n");
        }
        else
        {
            PMQTTTRANSPORT_HANDLE_DATA transportData = (PMQTTTRANSPORT_HANDLE_DATA)callbackCtx;
            if (IoTHubClient_LL_MessageCallback(transportData->llClientHandle, IoTHubMessage) != IOTHUBMESSAGE_ACCEPTED)
            {
                LogError("Event not accepted by our client.\r\n");
            }
            IoTHubMessage_Destroy(IoTHubMessage);
        }
    }
}

static void MqttOpCompleteCallback(MQTT_CLIENT_HANDLE handle, MQTT_CLIENT_EVENT_RESULT actionResult, const void* msgInfo, void* callbackCtx)
{
    (void)handle;
    if (callbackCtx != NULL)
    {
        PMQTTTRANSPORT_HANDLE_DATA transportData = (PMQTTTRANSPORT_HANDLE_DATA)callbackCtx;

        switch (actionResult)
        {
            case MQTT_CLIENT_ON_PUBLISH_ACK:
            case MQTT_CLIENT_ON_PUBLISH_COMP:
            {
                const PUBLISH_ACK* puback = (const PUBLISH_ACK*)msgInfo;
                if (puback != NULL)
                {
                    PDLIST_ENTRY currentListEntry = transportData->waitingForAck.Flink;
                    while (currentListEntry != &transportData->waitingForAck)
                    {
                        MQTT_MESSAGE_DETAILS_LIST* mqttMsgEntry = containingRecord(currentListEntry, MQTT_MESSAGE_DETAILS_LIST, entry);
                        DLIST_ENTRY saveListEntry;
                        saveListEntry.Flink = currentListEntry->Flink;

                        if (puback->packetId == mqttMsgEntry->msgPacketId)
                        {
                            (void)DList_RemoveEntryList(currentListEntry); //First remove the item from Waiting for Ack List.
                            sendMsgComplete(mqttMsgEntry->iotHubMessageEntry, transportData, IOTHUB_BATCHSTATE_SUCCESS);
                            free(mqttMsgEntry);
                        }
                        currentListEntry = saveListEntry.Flink;
                    }
                }
                break;
            }
            case MQTT_CLIENT_ON_CONNACK:
            {
                const CONNECT_ACK* connack = (const CONNECT_ACK*)msgInfo;
                if (connack != NULL)
                {
                    if (connack->returnCode == CONNECTION_ACCEPTED)
                    {
                        // The connect packet has been acked
                        transportData->currPacketState = CONNACK_TYPE;
                    }
                    else
                    {
                        LogError("Connection not accepted, return code: %d.\r\n", connack->returnCode);
                        (void)mqtt_client_disconnect(transportData->mqttClient);
                        transportData->connected = false;
                        transportData->currPacketState = PACKET_TYPE_ERROR;
                    }
                }
                else
                {
                    LogError("MQTT_CLIENT_ON_CONNACK CONNACK parameter is NULL.\r\n");
                }
                break;
            }
            case MQTT_CLIENT_ON_SUBSCRIBE_ACK:
            {
                const SUBSCRIBE_ACK* suback = (const SUBSCRIBE_ACK*)msgInfo;
                if (suback != NULL)
                {
                    if (suback->qosCount == 1)
                    {
                        // The connect packet has been acked
                        transportData->currPacketState = SUBACK_TYPE;
                    }
                    else
                    {
                        LogError("QOS count was not expected: %d.\r\n", (int)suback->qosCount);
                    }
                }
                break;
            }
            case MQTT_CLIENT_ON_PUBLISH_RECV:
            case MQTT_CLIENT_ON_PUBLISH_REL:
            {
                // Currently not used
                break;
            }
            case MQTT_CLIENT_ON_DISCONNECT:
            {
                // Close the client so we can reconnect again
                transportData->connected = false;
                transportData->currPacketState = DISCONNECT_TYPE;
                break;
            }
            case MQTT_CLIENT_ON_ERROR:
            {
                xio_close(transportData->xioTransport, NULL, NULL);
                transportData->connected = false;
                transportData->currPacketState = PACKET_TYPE_ERROR;
            }
        }
    }
}

const XIO_HANDLE getIoTransportProvider(const char* fqdn, int port)
{
    TLSIO_CONFIG tls_io_config = { fqdn, port };
    const IO_INTERFACE_DESCRIPTION* io_interface_description = platform_get_default_tlsio();
    return (void*)xio_create(io_interface_description, &tls_io_config, NULL/*defaultPrintLogFunction*/);
}

static int SubscribeToMqttProtocol(PMQTTTRANSPORT_HANDLE_DATA transportState)
{
    int result;

    SUBSCRIBE_PAYLOAD subscribe[] = {
        { STRING_c_str(transportState->mqttMessageTopic), DELIVER_AT_LEAST_ONCE }
    };
    /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_016: [IoTHubTransportMqtt_Subscribe shall call mqtt_client_subscribe to subscribe to the Message Topic.] */
    if (mqtt_client_subscribe(transportState->mqttClient, transportState->packetId++, subscribe, 1) != 0)
    {
        /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_017: [Upon failure IoTHubTransportMqtt_Subscribe shall return a non-zero value.] */
        result = __LINE__;
    }
    else
    {
        /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_018: [On success IoTHubTransportMqtt_Subscribe shall return 0.] */
        transportState->subscribed = true;
        transportState->currPacketState = SUBSCRIBE_TYPE;
        result = 0;
    }
    return result;
}

static const unsigned char* RetrieveMessagePayload(IOTHUB_MESSAGE_HANDLE messageHandle, size_t* length)
{
    const unsigned char* result;

    IOTHUBMESSAGE_CONTENT_TYPE contentType = IoTHubMessage_GetContentType(messageHandle);
    if (contentType == IOTHUBMESSAGE_BYTEARRAY)
    {
        if (IoTHubMessage_GetByteArray(messageHandle, &result, length) != IOTHUB_MESSAGE_OK)
        {
            LogError("Failure result from IoTHubMessage_GetByteArray\r\n");
            result = NULL;
            *length = 0;
        }
    }
    else if (contentType == IOTHUBMESSAGE_STRING)
    {
        result = (const unsigned char*)IoTHubMessage_GetString(messageHandle);
        if (result == NULL)
        {
            LogError("Failure result from IoTHubMessage_GetString\r\n");
            result = NULL;
            *length = 0;
        }
        else
        {
            *length = strlen((const char*)result);
        }
    }
    else
    {
        result = NULL;
        *length = 0;
    }
    return result;
}

static STRING_HANDLE ConstructSasToken(const char* iothubName, const char* iotHubSuffix, const char* deviceId)
{
    STRING_HANDLE result;
    size_t len = strlen(iothubName);
    len += strlen(iotHubSuffix);
    len += strlen(deviceId);

    char* sasToken = malloc(len + SAS_TOKEN_DEFAULT_LEN + 1);
    if (sasToken == NULL)
    {
        result = NULL;
    }
    else
    {
        (void)sprintf(sasToken, "%s.%s/devices/%s", iothubName, iotHubSuffix, deviceId);
        result = STRING_construct(sasToken);
        free(sasToken);
    }
    return result;
}

static STRING_HANDLE ConstructEventTopic(const char* deviceId)
{
    STRING_HANDLE result;
    size_t len = strlen(deviceId);

    char* eventTopic = malloc(len + EVENT_TOPIC_DEFAULT_LEN + 1);
    if (eventTopic == NULL)
    {
        result = NULL;
    }
    else
    {
        (void)sprintf(eventTopic, "devices/%s/messages/events/", deviceId);
        result = STRING_construct(eventTopic);
        free(eventTopic);
    }
    return result;
}

static STRING_HANDLE ConstructMessageTopic(const char* deviceId)
{
    STRING_HANDLE result;
    size_t len = strlen(deviceId);

    char* messageTopic = malloc(len + 32 + 1);
    if (messageTopic == NULL)
    {
        result = NULL;
    }
    else
    {
        (void)sprintf(messageTopic, "devices/%s/messages/devicebound/#", deviceId);
        result = STRING_construct(messageTopic);
        free(messageTopic);
    }
    return result;
}

static int InitializeConnection(PMQTTTRANSPORT_HANDLE_DATA transportState, bool initialConnection)
{
    int result = 0;
    if (!transportState->connected && !transportState->destroyCalled)
    {
        // Construct SAS token
        size_t secSinceEpoch = (size_t)(difftime(get_time(NULL), EPOCH_TIME_T_VALUE) + 0);
        size_t expiryTime = secSinceEpoch + SAS_TOKEN_DEFAULT_LIFETIME;

        // Not checking the success of this variable, if fail it will fail in the SASToken creation and return false;
        STRING_HANDLE emptyKeyName = STRING_new();
        STRING_HANDLE sasToken = SASToken_Create(transportState->device_key, transportState->sasTokenSr, emptyKeyName, expiryTime);
        if (sasToken == NULL)
        {
            result = __LINE__;
        }
        else
        {
            MQTT_CLIENT_OPTIONS options = { 0 };
            options.clientId = (char*)STRING_c_str(transportState->device_id);
            options.willMessage = NULL;
            options.username = (char*)STRING_c_str(transportState->configPassedThroughUsername);
            options.password = (char*)STRING_c_str(sasToken);
            options.keepAliveInterval = DEFAULT_MQTT_KEEPALIVE;
            options.useCleanSession = false;
            options.qualityOfServiceValue = DELIVER_AT_LEAST_ONCE;

            // construct address
            const char* hostAddress = STRING_c_str(transportState->hostAddress);
            const char* hostName = strstr(hostAddress, "//");
            if (hostName == NULL)
            {
                hostName = hostAddress;
            }
            else
            {
                // Increment beyond the double backslash
                hostName += 2;
            }

            transportState->xioTransport = getIoTransportProvider(hostName, transportState->portNum);
            if (mqtt_client_connect(transportState->mqttClient, transportState->xioTransport, &options) != 0)
            {
                LogError("failure connecting to address %s:%d.\r\n", STRING_c_str(transportState->hostAddress), transportState->portNum);
                result = __LINE__;
            }
            else
            {
                transportState->connected = true;
                result = 0;
            }
            STRING_delete(emptyKeyName);
            STRING_delete(sasToken);
        }
    }
    return result;
}

static STRING_HANDLE buildConfigForUsername(const IOTHUB_CLIENT_CONFIG* upperConfig)
{
    STRING_HANDLE result;

    size_t len = strlen(upperConfig->iotHubName)+strlen(upperConfig->iotHubSuffix)+strlen(upperConfig->deviceId)+strlen(CLIENT_DEVICE_TYPE_PREFIX)+strlen(IOTHUB_SDK_VERSION);
    char* eventTopic = malloc(len + BUILD_CONFIG_USERNAME + 1);
    if (eventTopic == NULL)
    {
        result = NULL;
    }
    else
    {
        (void)sprintf(eventTopic, "%s.%s/%s/DeviceClientType=%s%%2F%s", upperConfig->iotHubName, upperConfig->iotHubSuffix, upperConfig->deviceId, CLIENT_DEVICE_TYPE_PREFIX, IOTHUB_SDK_VERSION);
        result = STRING_construct(eventTopic);
        free(eventTopic);
    }
    return result;
}

static PMQTTTRANSPORT_HANDLE_DATA InitializeTransportHandleData(const IOTHUB_CLIENT_CONFIG* upperConfig, PDLIST_ENTRY waitingToSend)
{
    PMQTTTRANSPORT_HANDLE_DATA state = (PMQTTTRANSPORT_HANDLE_DATA)malloc(sizeof(MQTTTRANSPORT_HANDLE_DATA));
    if (state == NULL)
    {
        LogError("Could not create MQTT transport state. Memory allocation failed.\r\n");
    }
    else if ((state->device_id = STRING_construct(upperConfig->deviceId)) == NULL)
    {
        free(state);
        state = NULL;
    }
    else if ((state->device_key = STRING_construct(upperConfig->deviceKey)) == NULL)
    {
        LogError("Could not create device key for MQTT\r\n");
        STRING_delete(state->device_id);
        free(state);
        state = NULL;
    }
    else if ( (state->sasTokenSr = ConstructSasToken(upperConfig->iotHubName, upperConfig->iotHubSuffix, upperConfig->deviceId) ) == NULL)
    {
        LogError("Could not create Sas Token Sr String.\r\n");
        STRING_delete(state->device_key);
        STRING_delete(state->device_id);
        free(state);
        state = NULL;
    }
    else if ( (state->mqttEventTopic = ConstructEventTopic(upperConfig->deviceId) ) == NULL)
    {
        LogError("Could not create mqttEventTopic for MQTT\r\n");
        STRING_delete(state->sasTokenSr);
        STRING_delete(state->device_key);
        STRING_delete(state->device_id);
        free(state);
        state = NULL;
    }
    else if ((state->mqttMessageTopic = ConstructMessageTopic(upperConfig->deviceId) ) == NULL)
    {
        LogError("Could not create mqttMessageTopic for MQTT\r\n");
        STRING_delete(state->mqttEventTopic);
        STRING_delete(state->sasTokenSr);
        STRING_delete(state->device_key);
        STRING_delete(state->device_id);
        free(state);
        state = NULL;
    }
    else
    {
        state->mqttClient = mqtt_client_init(MqttRecvCallback, MqttOpCompleteCallback, state, defaultPrintLogFunction);
        if (state->mqttClient == NULL)
        {
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
            /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_008: [The hostname shall be constructed using the iothubname and iothubSuffix.] */
            // TODO: need to strip the ssl or http or tls
            char tempAddress[DEFAULT_TEMP_STRING_LEN];
            (void)snprintf(tempAddress, DEFAULT_TEMP_STRING_LEN, "%s.%s", upperConfig->iotHubName, upperConfig->iotHubSuffix);
            if ((state->hostAddress = STRING_construct(tempAddress)) == NULL)
            {
                STRING_delete(state->mqttEventTopic);
                STRING_delete(state->mqttMessageTopic);
                STRING_delete(state->sasTokenSr);
                STRING_delete(state->device_key);
                STRING_delete(state->device_id);
                free(state);
                state = NULL;
            }
            else if ((state->configPassedThroughUsername = buildConfigForUsername(upperConfig)) == NULL)
            {
                STRING_delete(state->hostAddress);
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
                /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_010: [IoTHubTransportMqtt_Create shall allocate memory to save its internal state where all topics, hostname, device_id, device_key, sasTokenSr and client handle shall be saved.] */
                DList_InitializeListHead(&(state->waitingForAck));
                state->destroyCalled = false;
                state->subscribed = false;
                state->connected = false;
                state->packetId = 1;
                state->llClientHandle = NULL;
                state->xioTransport = NULL;
                state->portNum = DEFAULT_PORT_NUMBER;
                state->waitingToSend = waitingToSend;
                state->currPacketState = CONNECT_TYPE;
            }
        }
    }
    return state;
}

extern TRANSPORT_HANDLE IoTHubTransportMqtt_Create(const IOTHUBTRANSPORT_CONFIG* config)
{
    PMQTTTRANSPORT_HANDLE_DATA result;
    size_t deviceIdSize;
    
    /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_001: [If parameter config is NULL then IoTHubTransportMqtt_Create shall return NULL.] */
    if (config == NULL)
    {
        LogError("Invalid Argument: Config Parameter is NULL.\r\n");
        result = NULL;
    }
    /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_002: [If the parameter config's variables upperConfig or waitingToSend are NULL then IoTHubTransportMqtt_Create shall return NULL.] */
    /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_003: [If the upperConfig's variables deviceId, deviceKey, iotHubName, protocol, or iotHubSuffix are NULL then IoTHubTransportMqtt_Create shall return NULL.] */
    else if (config->upperConfig == NULL || config->upperConfig->protocol == NULL || config->upperConfig->deviceId == NULL || config->upperConfig->deviceKey == NULL ||
        config->upperConfig->iotHubName == NULL || config->upperConfig->iotHubSuffix == NULL)
    {
        LogError("Invalid Argument: upperConfig structure contains an invalid parameter\r\n");
        result = NULL;
    }
    /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_002: [If the parameter config's variables upperConfig or waitingToSend are NULL then IoTHubTransportMqtt_Create shall return NULL.] */
    else if (config->waitingToSend == NULL)
    {
        LogError("Invalid Argument: waitingToSend is NULL)\r\n");
        result = NULL;
    }
    /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_006: [If the upperConfig's variables deviceId is an empty strings or length is greater then 128 then IoTHubTransportMqtt_Create shall return NULL.] */
    else if ( ( (deviceIdSize = strlen(config->upperConfig->deviceId)) > 128U) || (deviceIdSize == 0) )
    {
        LogError("Invalid Argument: DeviceId is of an invalid size\r\n");
        result = NULL;
    }
    /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_003: [If the upperConfig's variables deviceId, deviceKey, iotHubName, protocol, or iotHubSuffix are NULL then IoTHubTransportMqtt_Create shall return NULL.] */
    else if (strlen(config->upperConfig->deviceKey) == 0)
    {
        LogError("Invalid Argument: deviceKey is empty\r\n");
        result = NULL;
    }
    /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_003: [If the upperConfig's variables deviceId, deviceKey, iotHubName, protocol, or iotHubSuffix are NULL then IoTHubTransportMqtt_Create shall return NULL.] */
    else if (strlen(config->upperConfig->iotHubName) == 0)
    {
        LogError("Invalid Argument: iotHubName is empty\r\n");
        result = NULL;
    }
    else
    {
        result = InitializeTransportHandleData(config->upperConfig, config->waitingToSend);
        if (result != NULL)
        {
            g_msgTickCounter = tickcounter_create();
        }
    }
    /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_009: [If any error is encountered then IoTHubTransportMqtt_Create shall return NULL.] */
    /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_011: [On Success IoTHubTransportMqtt_Create shall return a non-NULL value.] */
    return result;
}

void IoTHubTransportMqtt_Destroy(TRANSPORT_HANDLE handle)
{
    /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_012: [IoTHubTransportMqtt_Destroy shall do nothing if parameter handle is NULL.] */
    PMQTTTRANSPORT_HANDLE_DATA transportState = (PMQTTTRANSPORT_HANDLE_DATA)handle;
    if (transportState != NULL)
    {
        transportState->destroyCalled = true;

        /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_013: [If the parameter subscribe is true then IoTHubTransportMqtt_Destroy shall call IoTHubTransportMqtt_Unsubscribe.] */
        if (transportState->subscribed)
        {
            IoTHubTransportMqtt_Unsubscribe(handle);
        }

        //Empty the Waiting for Ack Messages.
        while (!DList_IsListEmpty(&transportState->waitingForAck))
        {
            PDLIST_ENTRY currentEntry = DList_RemoveHeadList(&transportState->waitingForAck);
            MQTT_MESSAGE_DETAILS_LIST* mqttMsgEntry = containingRecord(currentEntry, MQTT_MESSAGE_DETAILS_LIST, entry);
            sendMsgComplete(mqttMsgEntry->iotHubMessageEntry, transportState, IOTHUB_BATCHSTATE_FAILED);
            free(mqttMsgEntry);
        }

        (void)mqtt_client_disconnect(transportState->mqttClient);

        xio_destroy(transportState->xioTransport);

        /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_014: [IoTHubTransportMqtt_Destroy shall free all the resources currently in use.] */
        transportState->connected = false;
        transportState->currPacketState = DISCONNECT_TYPE;
        mqtt_client_deinit(transportState->mqttClient);
        STRING_delete(transportState->mqttEventTopic);
        STRING_delete(transportState->mqttMessageTopic);
        STRING_delete(transportState->device_id);
        STRING_delete(transportState->device_key);
        STRING_delete(transportState->sasTokenSr);
        STRING_delete(transportState->hostAddress);
        STRING_delete(transportState->configPassedThroughUsername);
        tickcounter_destroy(g_msgTickCounter);
        free(transportState);
    }
}

int IoTHubTransportMqtt_Subscribe(IOTHUB_DEVICE_HANDLE handle)
{
    int result;
    PMQTTTRANSPORT_HANDLE_DATA transportState = (PMQTTTRANSPORT_HANDLE_DATA)handle;
    if (transportState == NULL)
    {
        /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_015: [If parameter handle is NULL than IoTHubTransportMqtt_Subscribe shall return a non-zero value.] */
        LogError("Invalid handle parameter. NULL.\r\n");
        result = __LINE__;
    }
    else
    {
        result = 0;
    }
    return result;
}

void IoTHubTransportMqtt_Unsubscribe(IOTHUB_DEVICE_HANDLE handle)
{
    PMQTTTRANSPORT_HANDLE_DATA transportState = (PMQTTTRANSPORT_HANDLE_DATA)handle;
    /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_019: [If parameter handle is NULL then IoTHubTransportMqtt_Unsubscribe shall do nothing.] */
    if (transportState != NULL && transportState->subscribed)
    {
        /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_020: [IoTHubTransportMqtt_Unsubscribe shall call mqtt_client_unsubscribe to unsubscribe the mqtt message topic.] */
        const char* unsubscribe[] = { STRING_c_str(transportState->mqttMessageTopic) };
        (void)mqtt_client_unsubscribe(transportState->mqttClient, transportState->packetId++, unsubscribe, 1);
        transportState->subscribed = false;
    }
}

extern void IoTHubTransportMqtt_DoWork(TRANSPORT_HANDLE handle, IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle)
{
    /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_026: [IoTHubTransportMqtt_DoWork shall do nothing if parameter handle and/or iotHubClientHandle is NULL.] */
    PMQTTTRANSPORT_HANDLE_DATA transportState = (PMQTTTRANSPORT_HANDLE_DATA)handle;
    if (transportState != NULL && iotHubClientHandle != NULL)
    {
        transportState->llClientHandle = iotHubClientHandle;

        if (InitializeConnection(transportState, true) != 0)
        {
            // Don't want to flood the logs with failures here
        }
        else
        {
            if (transportState->currPacketState == CONNACK_TYPE)
            {
                (void)SubscribeToMqttProtocol(transportState);
            }
            else if (transportState->currPacketState == SUBACK_TYPE)
            {
                // Publish can be called now
                transportState->currPacketState = PUBLISH_TYPE;
            }
            else if (transportState->currPacketState == PUBLISH_TYPE)
            {
                PDLIST_ENTRY currentListEntry = transportState->waitingForAck.Flink;
                while (currentListEntry != &transportState->waitingForAck)
                {
                    MQTT_MESSAGE_DETAILS_LIST* mqttMsgEntry = containingRecord(currentListEntry, MQTT_MESSAGE_DETAILS_LIST, entry);
                    DLIST_ENTRY nextListEntry;
                    nextListEntry.Flink = currentListEntry->Flink;

                    uint64_t current_ms;
                    (void)tickcounter_get_current_ms(g_msgTickCounter, &current_ms);
                    /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_033: [IoTHubTransportMqtt_DoWork shall iterate through the Waiting Acknowledge messages looking for any message that has been waiting longer than 2 min.]*/
                    if (((current_ms - mqttMsgEntry->msgPublishTime) / 1000) > RESEND_TIMEOUT_VALUE_MIN)
                    {
                        /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_034: [If IoTHubTransportMqtt_DoWork has resent the message two times then it shall fail the message] */
                        if (mqttMsgEntry->retryCount >= MAX_SEND_RECOUNT_LIMIT)
                        {
                            (void)DList_RemoveEntryList(currentListEntry);
                            sendMsgComplete(mqttMsgEntry->iotHubMessageEntry, transportState, IOTHUB_BATCHSTATE_FAILED);
                            free(mqttMsgEntry);
                        }
                        else
                        {
                            size_t messageLength;
                            const unsigned char* messagePayload = RetrieveMessagePayload(mqttMsgEntry->iotHubMessageEntry->messageHandle, &messageLength);
                            if (messageLength == 0 || messagePayload == NULL)
                            {
                                LogError("Failure from creating Message IoTHubMessage_GetData\r\n");
                            }
                            else
                            {
                                if (publishMqttMessage(transportState, mqttMsgEntry, messagePayload, messageLength) != 0)
                                {
                                    (void)DList_RemoveEntryList(currentListEntry);
                                    sendMsgComplete(mqttMsgEntry->iotHubMessageEntry, transportState, IOTHUB_BATCHSTATE_FAILED);
                                    free(mqttMsgEntry);
                                }
                            }
                        }
                    }
                    currentListEntry = nextListEntry.Flink;
                }

                currentListEntry = transportState->waitingToSend->Flink;
                /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_027: [IoTHubTransportMqtt_DoWork shall inspect the “waitingToSend” DLIST passed in config structure.] */
                while (currentListEntry != transportState->waitingToSend)
                {
                    IOTHUB_MESSAGE_LIST* iothubMsgList = containingRecord(currentListEntry, IOTHUB_MESSAGE_LIST, entry);
                    DLIST_ENTRY savedFromCurrentListEntry;
                    savedFromCurrentListEntry.Flink = currentListEntry->Flink;

                    /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_027: [IoTHubTransportMqtt_DoWork shall inspect the “waitingToSend” DLIST passed in config structure.] */
                    size_t messageLength;
                    const unsigned char* messagePayload = RetrieveMessagePayload(iothubMsgList->messageHandle, &messageLength);
                    if (messageLength == 0 || messagePayload == NULL)
                    {
                        LogError("Failure result from IoTHubMessage_GetData\r\n");
                    }
                    else
                    {
                        /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_029: [IoTHubTransportMqtt_DoWork shall create a MQTT_MESSAGE_HANDLE and pass this to a call to mqtt_client_publish.] */
                        MQTT_MESSAGE_DETAILS_LIST* mqttMsgEntry = (MQTT_MESSAGE_DETAILS_LIST*)malloc(sizeof(MQTT_MESSAGE_DETAILS_LIST));
                        if (mqttMsgEntry == NULL)
                        {
                            LogError("Allocation Error: Failure allocating MQTT Message Detail List.\r\n");
                        }
                        else
                        {
                            mqttMsgEntry->retryCount = 0;
                            mqttMsgEntry->msgPacketId = transportState->packetId;
                            mqttMsgEntry->iotHubMessageEntry = iothubMsgList;

                            if (publishMqttMessage(transportState, mqttMsgEntry, messagePayload, messageLength) != 0)
                            {
                                (void)(DList_RemoveEntryList(currentListEntry));
                                sendMsgComplete(iothubMsgList, transportState, IOTHUB_BATCHSTATE_FAILED);
                                free(mqttMsgEntry);
                            }
                            else
                            {
                                (void)(DList_RemoveEntryList(currentListEntry));
                                DList_InsertTailList(&(transportState->waitingForAck), &(mqttMsgEntry->entry));
                            }
                        }
                    }
                    currentListEntry = savedFromCurrentListEntry.Flink;
                }
            }
            /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_030: [IoTHubTransportMqtt_DoWork shall call mqtt_client_dowork everytime it is called if it is connected.] */
            mqtt_client_dowork(transportState->mqttClient);
        }
    }
}

IOTHUB_CLIENT_RESULT IoTHubTransportMqtt_GetSendStatus(IOTHUB_DEVICE_HANDLE handle, IOTHUB_CLIENT_STATUS *iotHubClientStatus)
{
    IOTHUB_CLIENT_RESULT result;

    if (handle == NULL || iotHubClientStatus == NULL)
    {
        /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_023: [IoTHubTransportMqtt_GetSendStatus shall return IOTHUB_CLIENT_INVALID_ARG if called with NULL parameter.] */
        LogError("invalid arument. \r\n");
        result = IOTHUB_CLIENT_INVALID_ARG;
    }
    else
    {
        MQTTTRANSPORT_HANDLE_DATA* handleData = (MQTTTRANSPORT_HANDLE_DATA*)handle;
        if (!DList_IsListEmpty(handleData->waitingToSend) || !DList_IsListEmpty(&(handleData->waitingForAck)))
        {
            /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_025: [IoTHubTransportMqtt_GetSendStatus shall return IOTHUB_CLIENT_OK and status IOTHUB_CLIENT_SEND_STATUS_BUSY if there are currently event items to be sent or being sent.] */
            *iotHubClientStatus = IOTHUB_CLIENT_SEND_STATUS_BUSY;
        }
        else
        {
            /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_024: [IoTHubTransportMqtt_GetSendStatus shall return IOTHUB_CLIENT_OK and status IOTHUB_CLIENT_SEND_STATUS_IDLE if there are currently no event items to be sent or being sent.] */
            *iotHubClientStatus = IOTHUB_CLIENT_SEND_STATUS_IDLE;
        }
        result = IOTHUB_CLIENT_OK;
    }
    return result;
}

IOTHUB_CLIENT_RESULT IoTHubTransportMqtt_SetOption(TRANSPORT_HANDLE handle, const char* option, const void* value)
{
    /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_021: [If any parameter is NULL then IoTHubTransportMqtt_SetOption shall return IOTHUB_CLIENT_INVALID_ARG.] */
    IOTHUB_CLIENT_RESULT result;
    if (
        (handle == NULL) ||
        (option == NULL) ||
        (value == NULL)
        )
    {
        result = IOTHUB_CLIENT_INVALID_ARG;
        LogError("invalid parameter (NULL) passed to clientTransportAMQP_SetOption\r\n");
    }
    else
    {
        /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_031: [If the option parameter is set to "logtrace" then the value shall be a bool_ptr and the value will determine if the mqtt client log is on or off.] */
        if (strcmp("logtrace", option) == 0)
        {
            MQTTTRANSPORT_HANDLE_DATA* transportState = (MQTTTRANSPORT_HANDLE_DATA*)handle;
            bool* traceVal = (bool*)value;
            mqtt_client_set_trace(transportState->mqttClient, *traceVal, *traceVal);
            result = IOTHUB_CLIENT_OK;
        }
        else
        {
            /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_032: [IoTHubTransportMqtt_SetOption shall return IOTHUB_CLIENT_INVALID_ARG if the option parameter is not a known option string*] */
            result = IOTHUB_CLIENT_INVALID_ARG;
        }
    }
    return result;
}

IOTHUB_DEVICE_HANDLE IoTHubTransportMqtt_Register(TRANSPORT_HANDLE handle, const char* deviceId, const char* deviceKey, PDLIST_ENTRY waitingToSend)
{
	IOTHUB_DEVICE_HANDLE result;
	// Codes_SRS_IOTHUB_MQTT_TRANSPORT_17_001: [ IoTHubTransportMqtt_Register shall return NULL if the TRANSPORT_HANDLE is NULL.]
	// Codes_SRS_IOTHUB_MQTT_TRANSPORT_17_002: [ IoTHubTransportMqtt_Register shall return NULL if deviceId, deviceKey or waitingToSend are NULL.]
	if ((handle == NULL) || (deviceId == NULL) || (deviceKey == NULL) || (waitingToSend == NULL))
	{
		result = NULL;
	}
	else
	{
		MQTTTRANSPORT_HANDLE_DATA* transportState = (MQTTTRANSPORT_HANDLE_DATA*)handle;

		// Codes_SRS_IOTHUB_MQTT_TRANSPORT_17_003: [ IoTHubTransportMqtt_Register shall return NULL if deviceId or deviceKey do not match the deviceId and deviceKey passed in during IoTHubTransportMqtt_Create.]
		if (strcmp(STRING_c_str(transportState->device_id), deviceId) != 0)
		{
			result = NULL;
		}
		else if (strcmp(STRING_c_str(transportState->device_key), deviceKey) != 0)
		{
			result = NULL;
		}
		else
		{
			// Codes_SRS_IOTHUB_MQTT_TRANSPORT_17_004: [ IoTHubTransportMqtt_Register shall return the TRANSPORT_HANDLE as the IOTHUB_DEVICE_HANDLE. ]
			result = (IOTHUB_DEVICE_HANDLE)handle;
		}
	}

	return result;
}

// Codes_SRS_IOTHUB_MQTT_TRANSPORT_17_005: [ IoTHubTransportMqtt_Unregister shall return. ]
void IoTHubTransportMqtt_Unregister(IOTHUB_DEVICE_HANDLE deviceHandle)
{
	return;
}

TRANSPORT_PROVIDER myfunc = {
    IoTHubTransportMqtt_SetOption,
    IoTHubTransportMqtt_Create, 
    IoTHubTransportMqtt_Destroy, 
	IoTHubTransportMqtt_Register,
	IoTHubTransportMqtt_Unregister,
    IoTHubTransportMqtt_Subscribe, 
    IoTHubTransportMqtt_Unsubscribe, 
    IoTHubTransportMqtt_DoWork, 
    IoTHubTransportMqtt_GetSendStatus
};

/* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_022: [This function shall return a pointer to a structure of type TRANSPORT_PROVIDER having the following values for it’s fields: IoTHubTransport_Create = IoTHubTransportMqtt_Create
IoTHubTransport_Destroy = IoTHubTransportMqtt_Destroy
IoTHubTransport_Subscribe = IoTHubTransportMqtt_Subscribe
IoTHubTransport_Unsubscribe = IoTHubTransportMqtt_Unsubscribe
IoTHubTransport_DoWork = IoTHubTransportMqtt_DoWork
IoTHubTransport_SetOption = IoTHubTransportMqtt_SetOption] */
extern const void* MQTT_Protocol(void)
{
    return &myfunc;
}
