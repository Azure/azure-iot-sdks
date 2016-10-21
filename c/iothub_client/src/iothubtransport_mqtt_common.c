// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#include "azure_c_shared_utility/gballoc.h"

#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/strings.h"
#include "azure_c_shared_utility/doublylinkedlist.h"
#include "azure_c_shared_utility/crt_abstractions.h"

#include "iothub_client_ll.h"
#include "iothub_client_options.h"
#include "iothub_client_private.h"
#include "azure_umqtt_c/mqtt_client.h"
#include "azure_c_shared_utility/sastoken.h"
#include "azure_c_shared_utility/tickcounter.h"

#include "azure_c_shared_utility/tlsio.h"
#include "azure_c_shared_utility/platform.h"

#include "azure_c_shared_utility/string_tokenizer.h"
#include "iothub_client_version.h"

#include "iothubtransport_mqtt_common.h"

#include <stdarg.h>
#include <stdio.h>

#include <limits.h>

#define SAS_TOKEN_DEFAULT_LIFETIME  3600
#define SAS_REFRESH_MULTIPLIER      .8
#define EPOCH_TIME_T_VALUE          0
#define DEFAULT_MQTT_KEEPALIVE      4*60 // 4 min
#define BUILD_CONFIG_USERNAME       24
#define SAS_TOKEN_DEFAULT_LEN       10
#define RESEND_TIMEOUT_VALUE_MIN    1*60
#define MAX_SEND_RECOUNT_LIMIT      2
#define DEFAULT_CONNECTION_INTERVAL 30
#define FAILED_CONN_BACKOFF_VALUE   5

static const char* TOPIC_DEVICE_MSG = "devices/%s/messages/devicebound/#";
static const char* TOPIC_DEVICE_DEVICE = "devices/%s/messages/events/";
static const char* PROPERTY_SEPARATOR = "&";

#define UNSUBSCRIBE_FROM_TOPIC                  0x0000
#define SUBSCRIBE_TELEMETRY_TOPIC               0x0004
#define SUBSCRIBE_TOPIC_COUNT                   1

typedef struct SYSTEM_PROPERTY_INFO_TAG
{
    const char* propName;
    size_t propLength;
} SYSTEM_PROPERTY_INFO;

static SYSTEM_PROPERTY_INFO sysPropList[] = {
    { "%24.exp", 7 },
    { "%24.mid", 7 },
    { "%24.uid", 7 },
    { "%24.to", 6 },
    { "%24.cid", 7 },
    { "devices/", 8 },
    { "iothub-operation", 16 },
    { "iothub-ack", 10 }
};

static TICK_COUNTER_HANDLE g_msgTickCounter;

typedef enum MQTT_TRANSPORT_CREDENTIAL_TYPE_TAG
{
    CREDENTIAL_NOT_BUILD,
    X509,
    SAS_TOKEN_FROM_USER,
    DEVICE_KEY,
} MQTT_TRANSPORT_CREDENTIAL_TYPE;

typedef struct MQTT_TRANSPORT_CREDENTIALS_TAG
{
    MQTT_TRANSPORT_CREDENTIAL_TYPE credential_type;
    union
    {
        // Key associated to the device to be used.
        STRING_HANDLE deviceKey;

        // SAS associated to the device to be used.
        STRING_HANDLE deviceSasToken;

    } CREDENTIAL_VALUE;
} MQTT_TRANSPORT_CREDENTIALS;

typedef struct MQTTTRANSPORT_HANDLE_DATA_TAG
{
    // Topic control
    STRING_HANDLE topic_MqttEvent;
    STRING_HANDLE topic_MqttMessage;

    uint32_t topics_ToSubscribe;
    // Connection related constants
    STRING_HANDLE hostAddress;
    STRING_HANDLE device_id;
    STRING_HANDLE devicesPath;
    int portNum;

    // Authentication
    MQTT_TRANSPORT_CREDENTIALS transport_creds;

    MQTT_GET_IO_TRANSPORT get_io_transport;

    // The current mqtt iothub implementation requires that the hub name and the domain suffix be passed as the first of a series of segments
    // passed through the username portion of the connection frame.
    // The second segment will contain the device id.  The two segments are delemited by a "/".
    // The first segment can be a maximum 256 characters.
    // The second segment can be a maximum 128 characters.
    // With the / delimeter you have 384 chars (Plus a terminator of 0).
    STRING_HANDLE configPassedThroughUsername;

    // Upper layer
    IOTHUB_CLIENT_LL_HANDLE llClientHandle;

    // Protocol 
    MQTT_CLIENT_HANDLE mqttClient;
    XIO_HANDLE xioTransport;

    // Session - connection
    uint16_t packetId;

    // Connection state control
    bool isRegistered;
    bool isConnected;
    bool isDestroyCalled;
    uint16_t keepAliveValue;
    uint64_t mqtt_connect_time;
    size_t connectFailCount;
    uint64_t connectTick;
    bool log_trace;
    bool raw_trace;

    // Internal lists for message tracking
    PDLIST_ENTRY waitingToSend;
    // Message tracking
    CONTROL_PACKET_TYPE currPacketState;

    // Telemetry specific
    DLIST_ENTRY telemetry_waitingForAck;
} MQTTTRANSPORT_HANDLE_DATA, *PMQTTTRANSPORT_HANDLE_DATA;

typedef struct MQTT_MESSAGE_DETAILS_LIST_TAG
{
    uint64_t msgPublishTime;
    size_t retryCount;
    IOTHUB_MESSAGE_LIST* iotHubMessageEntry;
    void* context;
    uint16_t packet_id;
    DLIST_ENTRY entry;
} MQTT_MESSAGE_DETAILS_LIST, *PMQTT_MESSAGE_DETAILS_LIST;

static uint16_t get_next_packet_id(PMQTTTRANSPORT_HANDLE_DATA transport_data)
{
    if (transport_data->packetId+1 >= USHRT_MAX)
    {
        transport_data->packetId = 1;
    }
    else
    {
        transport_data->packetId++;
    }
    return transport_data->packetId;
}

static const char* retrieve_mqtt_return_codes(CONNECT_RETURN_CODE rtn_code)
{
    switch (rtn_code)
    {
        case CONNECTION_ACCEPTED:
            return "Accepted";
        case CONN_REFUSED_UNACCEPTABLE_VERSION:
            return "Unacceptable Version";
        case CONN_REFUSED_ID_REJECTED:
            return "Id Rejected";
        case CONN_REFUSED_SERVER_UNAVAIL:
            return "Server Unavailable";
        case CONN_REFUSED_BAD_USERNAME_PASSWORD:
            return "Bad Username/Password";
        case CONN_REFUSED_NOT_AUTHORIZED:
            return "Not Authorized";
        case CONN_REFUSED_UNKNOWN:
        default:
            return "Unknown";
    }
}

static void sendMsgComplete(IOTHUB_MESSAGE_LIST* iothubMsgList, PMQTTTRANSPORT_HANDLE_DATA transport_data, IOTHUB_CLIENT_CONFIRMATION_RESULT confirmResult)
{
    DLIST_ENTRY messageCompleted;
    DList_InitializeListHead(&messageCompleted);
    DList_InsertTailList(&messageCompleted, &(iothubMsgList->entry));
    IoTHubClient_LL_SendComplete(transport_data->llClientHandle, &messageCompleted, confirmResult);
}

static STRING_HANDLE addPropertiesTouMqttMessage(IOTHUB_MESSAGE_HANDLE iothub_message_handle, const char* eventTopic)
{
    STRING_HANDLE result = STRING_construct(eventTopic);
    const char* const* propertyKeys;
    const char* const* propertyValues;
    size_t propertyCount;

    // Construct Properties
    MAP_HANDLE properties_map = IoTHubMessage_Properties(iothub_message_handle);
    if (properties_map != NULL)
    {
        if (Map_GetInternals(properties_map, &propertyKeys, &propertyValues, &propertyCount) != MAP_OK)
        {
            LogError("Failed to get the internals of the property map.");
            STRING_delete(result);
            result = NULL;
        }
        else
        {
            if (propertyCount != 0)
            {
                size_t index = 0;
                for (index = 0; index < propertyCount && result != NULL; index++)
                {
                    if (STRING_sprintf(result, "%s=%s%s", propertyKeys[index], propertyValues[index], propertyCount - 1 == index ? "" : PROPERTY_SEPARATOR) != 0)
                    {
                        STRING_delete(result);
                        result = NULL;
                    }
                }
            }
        }
    }
    return result;
}

static int publish_mqtt_telemetry_msg(PMQTTTRANSPORT_HANDLE_DATA transport_data, MQTT_MESSAGE_DETAILS_LIST* mqttMsgEntry, const unsigned char* payload, size_t len)
{
    int result;
    STRING_HANDLE msgTopic = addPropertiesTouMqttMessage(mqttMsgEntry->iotHubMessageEntry->messageHandle, STRING_c_str(transport_data->topic_MqttEvent));
    if (msgTopic == NULL)
    {
        result = __LINE__;
    }
    else
    {
        MQTT_MESSAGE_HANDLE mqttMsg = mqttmessage_create(mqttMsgEntry->packet_id, STRING_c_str(msgTopic), DELIVER_AT_LEAST_ONCE, payload, len);
        if (mqttMsg == NULL)
        {
            result = __LINE__;
        }
        else
        {
            if (tickcounter_get_current_ms(g_msgTickCounter, &mqttMsgEntry->msgPublishTime) != 0)
            {
                LogError("Failed retrieving tickcounter info");
                result = __LINE__;
            }
            else
            {
                if (mqtt_client_publish(transport_data->mqttClient, mqttMsg) != 0)
                {
                    result = __LINE__;
                }
                else
                {
                    mqttMsgEntry->retryCount++;
                    result = 0;
                }
            }
            mqttmessage_destroy(mqttMsg);
        }
        STRING_delete(msgTopic);
    }
    return result;
}

static bool isSystemProperty(const char* tokenData)
{
    bool result = false;
    size_t propCount = sizeof(sysPropList)/sizeof(sysPropList[0]);
    size_t index = 0;
    for (index = 0; index < propCount; index++)
    {
        if (memcmp(tokenData, sysPropList[index].propName, sysPropList[index].propLength) == 0)
        {
            result = true;
            break;
        }
    }
    return result;
}

static int extractMqttProperties(IOTHUB_MESSAGE_HANDLE IoTHubMessage, const char* topic_name)
{
    int result;
    STRING_HANDLE mqttTopic = STRING_construct(topic_name);
    if (mqttTopic == NULL)
    {
        LogError("Failure constructing string topic name.");
        result = __LINE__;
    }
    else
    {
        STRING_TOKENIZER_HANDLE token = STRING_TOKENIZER_create(mqttTopic);
        if (token != NULL)
        {
            MAP_HANDLE propertyMap = IoTHubMessage_Properties(IoTHubMessage);
            if (propertyMap == NULL)
            {
                LogError("Failure to retrieve IoTHubMessage_properties.");
                result = __LINE__;
            }
            else
            {
                STRING_HANDLE output = STRING_new();
                if (output == NULL)
                {
                    LogError("Failure to allocate STRING_new.");
                    result = __LINE__;
                }
                else
                {
                    result = 0;
                    while (STRING_TOKENIZER_get_next_token(token, output, PROPERTY_SEPARATOR) == 0 && result == 0)
                    {
                        const char* tokenData = STRING_c_str(output);
                        size_t tokenLen = strlen(tokenData);
                        if (tokenData == NULL || tokenLen == 0)
                        {
                            break;
                        }
                        else
                        {
                            if (!isSystemProperty(tokenData) )
                            {
                                const char* iterator = tokenData;
                                while (iterator != NULL && *iterator != '\0' && result == 0)
                                {
                                    if (*iterator == '=')
                                    {
                                        size_t nameLen = iterator - tokenData;
                                        char* propName = malloc(nameLen + 1);

                                        size_t valLen = tokenLen - (nameLen + 1) + 1;
                                        char* propValue = malloc(valLen + 1);

                                        if (propName == NULL || propValue == NULL)
                                        {
                                            result = __LINE__;
                                        }
                                        else
                                        {
                                            strncpy(propName, tokenData, nameLen);
                                            propName[nameLen] = '\0';

                                            strncpy(propValue, iterator + 1, valLen);
                                            propValue[valLen] = '\0';

                                            if (Map_AddOrUpdate(propertyMap, propName, propValue) != MAP_OK)
                                            {
                                                LogError("Map_AddOrUpdate failed.");
                                                result = __LINE__;
                                            }
                                        }
                                        free(propName);
                                        free(propValue);

                                        break;
                                    }
                                    iterator++;
                                }
                            }
                        }
                    }
                    STRING_delete(output);
                }
            }
            STRING_TOKENIZER_destroy(token);
        }
        else
        {
            LogError("Unable to create Tokenizer object.");
            result = __LINE__;
        }
        STRING_delete(mqttTopic);
    }
    return result;
}

static void mqtt_notification_callback(MQTT_MESSAGE_HANDLE msgHandle, void* callbackCtx)
{
    if (msgHandle != NULL && callbackCtx != NULL)
    {
        const char* topic_resp = mqttmessage_getTopicName(msgHandle);
        if (topic_resp == NULL)
        {
            LogError("Failure: NULL topic name encountered");
        }
        else
        {
            PMQTTTRANSPORT_HANDLE_DATA transportData = (PMQTTTRANSPORT_HANDLE_DATA)callbackCtx;

            const APP_PAYLOAD* appPayload = mqttmessage_getApplicationMsg(msgHandle);
            IOTHUB_MESSAGE_HANDLE IoTHubMessage = IoTHubMessage_CreateFromByteArray(appPayload->message, appPayload->length);
            if (IoTHubMessage == NULL)
            {
                LogError("Failure: IotHub Message creation has failed.");
            }
            else
            {
                // Will need to update this when the service has messages that can be rejected
                if (extractMqttProperties(IoTHubMessage, topic_resp) != 0)
                {
                    LogError("failure extracting mqtt properties.");
                }
                else
                {
                    if (IoTHubClient_LL_MessageCallback(transportData->llClientHandle, IoTHubMessage) != IOTHUBMESSAGE_ACCEPTED)
                    {
                        LogError("Event not accepted by our client.");
                    }
                }
                IoTHubMessage_Destroy(IoTHubMessage);
            }
        }
    }
}

static void mqtt_operation_complete_callback(MQTT_CLIENT_HANDLE handle, MQTT_CLIENT_EVENT_RESULT actionResult, const void* msgInfo, void* callbackCtx)
{
    (void)handle;
    if (callbackCtx != NULL)
    {
        PMQTTTRANSPORT_HANDLE_DATA transport_data = (PMQTTTRANSPORT_HANDLE_DATA)callbackCtx;

        switch (actionResult)
        {
            case MQTT_CLIENT_ON_PUBLISH_ACK:
            case MQTT_CLIENT_ON_PUBLISH_COMP:
            {
                const PUBLISH_ACK* puback = (const PUBLISH_ACK*)msgInfo;
                if (puback != NULL)
                {
                    PDLIST_ENTRY currentListEntry = transport_data->telemetry_waitingForAck.Flink;
                    while (currentListEntry != &transport_data->telemetry_waitingForAck)
                    {
                        MQTT_MESSAGE_DETAILS_LIST* mqttMsgEntry = containingRecord(currentListEntry, MQTT_MESSAGE_DETAILS_LIST, entry);
                        DLIST_ENTRY saveListEntry;
                        saveListEntry.Flink = currentListEntry->Flink;

                        if (puback->packetId == mqttMsgEntry->packet_id)
                        {
                            (void)DList_RemoveEntryList(currentListEntry); //First remove the item from Waiting for Ack List.
                            sendMsgComplete(mqttMsgEntry->iotHubMessageEntry, transport_data, IOTHUB_CLIENT_CONFIRMATION_OK);
                            free(mqttMsgEntry);
                        }
                        currentListEntry = saveListEntry.Flink;
                    }
                }
                else
                {
                    LogError("Failure: MQTT_CLIENT_ON_PUBLISH_ACK publish_ack structure NULL.");
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
                        transport_data->currPacketState = CONNACK_TYPE;
                    }
                    else
                    {
                        LogError("Connection Not Accepted: 0x%x: %s", connack->returnCode, retrieve_mqtt_return_codes(connack->returnCode) );
                        (void)mqtt_client_disconnect(transport_data->mqttClient);
                        transport_data->isConnected = false;
                        transport_data->currPacketState = PACKET_TYPE_ERROR;
                    }
                }
                else
                {
                    LogError("MQTT_CLIENT_ON_CONNACK CONNACK parameter is NULL.");
                }
                break;
            }
            case MQTT_CLIENT_ON_SUBSCRIBE_ACK:
            {
                const SUBSCRIBE_ACK* suback = (const SUBSCRIBE_ACK*)msgInfo;
                if (suback != NULL)
                {
                    size_t index = 0;
                    for (index = 0; index < suback->qosCount; index++)
                    {
                        if (suback->qosReturn[index] == DELIVER_FAILURE)
                        {
                            LogError("Subscribe delivery failure of subscribe %zu", index);
                        }
                    }
                    // The connect packet has been acked
                    transport_data->currPacketState = SUBACK_TYPE;
                }
                else
                {
                    LogError("Failure: MQTT_CLIENT_ON_SUBSCRIBE_ACK SUBSCRIBE_ACK parameter is NULL.");
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
                transport_data->isConnected = false;
                transport_data->currPacketState = DISCONNECT_TYPE;
                break;
            }
            case MQTT_CLIENT_NO_PING_RESPONSE:
                LogError("Mqtt Ping Response was not encountered.  Reconnecting device...");
            case MQTT_CLIENT_ON_ERROR:
            {
                transport_data->isConnected = false;
                transport_data->currPacketState = PACKET_TYPE_ERROR;
                if (transport_data->topic_MqttMessage != NULL)
                {
                    transport_data->topics_ToSubscribe |= SUBSCRIBE_TELEMETRY_TOPIC;
                }
            }
        }
    }
}

static void SubscribeToMqttProtocol(PMQTTTRANSPORT_HANDLE_DATA transport_data)
{
    if (transport_data->topics_ToSubscribe != UNSUBSCRIBE_FROM_TOPIC)
    {
        uint32_t topic_subscription = 0;
        size_t subscribe_count = 0;
        SUBSCRIBE_PAYLOAD subscribe[SUBSCRIBE_TOPIC_COUNT];
        if ((transport_data->topic_MqttMessage != NULL) && (SUBSCRIBE_TELEMETRY_TOPIC & transport_data->topics_ToSubscribe))
        {
            subscribe[subscribe_count].subscribeTopic = STRING_c_str(transport_data->topic_MqttMessage);
            subscribe[subscribe_count].qosReturn = DELIVER_AT_LEAST_ONCE;
            topic_subscription |= SUBSCRIBE_TELEMETRY_TOPIC;
            subscribe_count++;
        }

        if (subscribe_count != 0)
        {
            /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_016: [IoTHubTransportMqtt_Subscribe shall call mqtt_client_subscribe to subscribe to the Message Topic.] */
            if (mqtt_client_subscribe(transport_data->mqttClient, get_next_packet_id(transport_data), subscribe, subscribe_count) != 0)
            {
                /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_017: [Upon failure IoTHubTransportMqtt_Subscribe shall return a non-zero value.] */
                LogError("Failure: mqtt_client_subscribe returned error.");
            }
            else
            {
                /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_018: [On success IoTHubTransportMqtt_Subscribe shall return 0.] */
                transport_data->topics_ToSubscribe &= ~topic_subscription;
                transport_data->currPacketState = SUBSCRIBE_TYPE;
            }
        }
        else
        {
            /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_017: [Upon failure IoTHubTransportMqtt_Subscribe shall return a non-zero value.] */
            LogError("Failure: subscribe_topic is empty.");
        }
        transport_data->currPacketState = SUBSCRIBE_TYPE;
    }
    else
    {
        transport_data->currPacketState = PUBLISH_TYPE;
    }
}

static const unsigned char* RetrieveMessagePayload(IOTHUB_MESSAGE_HANDLE messageHandle, size_t* length)
{
    const unsigned char* result;

    IOTHUBMESSAGE_CONTENT_TYPE contentType = IoTHubMessage_GetContentType(messageHandle);
    if (contentType == IOTHUBMESSAGE_BYTEARRAY)
    {
        if (IoTHubMessage_GetByteArray(messageHandle, &result, length) != IOTHUB_MESSAGE_OK)
        {
            LogError("Failure result from IoTHubMessage_GetByteArray");
            result = NULL;
            *length = 0;
        }
    }
    else if (contentType == IOTHUBMESSAGE_STRING)
    {
        result = (const unsigned char*)IoTHubMessage_GetString(messageHandle);
        if (result == NULL)
        {
            LogError("Failure result from IoTHubMessage_GetString");
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

static int GetTransportProviderIfNecessary(PMQTTTRANSPORT_HANDLE_DATA transport_data)
{
    int result;

    if (transport_data->xioTransport == NULL)
    {
        // construct address
        const char* hostAddress = STRING_c_str(transport_data->hostAddress);
        transport_data->xioTransport = transport_data->get_io_transport(hostAddress);
        if (transport_data->xioTransport == NULL)
        {
            LogError("Unable to create the lower level TLS layer.");
            result = __LINE__;
        }
        else
        {
            result = 0;
        }
    }
    else
    {
        result = 0;
    }
    return result;
}

static int SendMqttConnectMsg(PMQTTTRANSPORT_HANDLE_DATA transport_data)
{
    int result;

    // Not checking the success of this variable, if fail it will fail in the SASToken creation and return false;
    STRING_HANDLE emptyKeyName = STRING_new();
    if (emptyKeyName == NULL)
    {
        result = __LINE__;
    }
    else
    {
        STRING_HANDLE sasToken = NULL;

        switch (transport_data->transport_creds.credential_type)
        {
            case SAS_TOKEN_FROM_USER:
                sasToken = STRING_clone(transport_data->transport_creds.CREDENTIAL_VALUE.deviceSasToken);
                break;
            case DEVICE_KEY:
            {
                // Construct SAS token
                size_t secSinceEpoch = (size_t)(difftime(get_time(NULL), EPOCH_TIME_T_VALUE) + 0);
                size_t expiryTime = secSinceEpoch + SAS_TOKEN_DEFAULT_LIFETIME;

                sasToken = SASToken_Create(transport_data->transport_creds.CREDENTIAL_VALUE.deviceKey, transport_data->devicesPath, emptyKeyName, expiryTime);
                break;
            }
            case X509:
            default:
                // The assumption here is that x509 is in place, if not setup
                // correctly the connection will be rejected.
                sasToken = NULL;
                break;
        }

        MQTT_CLIENT_OPTIONS options = { 0 };
        options.clientId = (char*)STRING_c_str(transport_data->device_id);
        options.willMessage = NULL;
        options.username = (char*)STRING_c_str(transport_data->configPassedThroughUsername);
        if (sasToken != NULL)
        {
            options.password = (char*)STRING_c_str(sasToken);
        }
        options.keepAliveInterval = transport_data->keepAliveValue;
        options.useCleanSession = false;
        options.qualityOfServiceValue = DELIVER_AT_LEAST_ONCE;

        if (GetTransportProviderIfNecessary(transport_data) == 0)
        {
            if (mqtt_client_connect(transport_data->mqttClient, transport_data->xioTransport, &options) != 0)
            {
                LogError("failure connecting to address %s:%d.", STRING_c_str(transport_data->hostAddress), transport_data->portNum);
                result = __LINE__;
            }
            else
            {
                (void)tickcounter_get_current_ms(g_msgTickCounter, &transport_data->mqtt_connect_time);
                result = 0;
            }
        }
        else
        {
            result = __LINE__;
        }
        STRING_delete(emptyKeyName);
        STRING_delete(sasToken);
    }
    return result;
}

static int InitializeConnection(PMQTTTRANSPORT_HANDLE_DATA transport_data)
{
    int result = 0;

    // Make sure we're not destroying the object
    if (!transport_data->isDestroyCalled)
    {
        // If we are not isConnected then check to see if we need 
        // to back off the connecting to the server
        if (!transport_data->isConnected)
        {
            // Default makeConnection as true if something goes wrong we'll make the connection
            bool makeConnection = true;
            // If we've failed for FAILED_CONN_BACKOFF_VALUE straight times them let's slow down connection
            // to the service
            if (transport_data->connectFailCount > FAILED_CONN_BACKOFF_VALUE)
            {
                uint64_t currentTick;
                if (tickcounter_get_current_ms(g_msgTickCounter, &currentTick) == 0)
                {
                    if ( ((currentTick - transport_data->connectTick)/1000) <= DEFAULT_CONNECTION_INTERVAL)
                    {
                        result = __LINE__;
                        makeConnection = false;
                    }
                }
            }

            if (makeConnection)
            {
                if (tickcounter_get_current_ms(g_msgTickCounter, &transport_data->connectTick) != 0)
                {
                    transport_data->connectFailCount++;
                    result = __LINE__;
                }
                else
                {
                    if (SendMqttConnectMsg(transport_data) != 0)
                    {
                        transport_data->connectFailCount++;
                        result = __LINE__;
                    }
                    else
                    {
                        transport_data->connectFailCount = 0;
                        transport_data->isConnected = true;
                        result = 0;
                    }
                }
            }
        }

        if (transport_data->isConnected)
        {
            // We are isConnected and not being closed, so does SAS need to reconnect?
            uint64_t current_time;
            if (tickcounter_get_current_ms(g_msgTickCounter, &current_time) != 0)
            {
                transport_data->connectFailCount++;
                result = __LINE__;
            }
            else
            {
                if ((current_time - transport_data->mqtt_connect_time) / 1000 > (SAS_TOKEN_DEFAULT_LIFETIME*SAS_REFRESH_MULTIPLIER))
                {
                    (void)mqtt_client_disconnect(transport_data->mqttClient);
                    transport_data->isConnected = false;
                    transport_data->currPacketState = UNKNOWN_TYPE;
                    if (transport_data->topic_MqttMessage != NULL)
                    {
                        transport_data->topics_ToSubscribe |= SUBSCRIBE_TELEMETRY_TOPIC;
                    }
                }
            }
        }
    }
    return result;
}

static STRING_HANDLE buildConfigForUsername(const IOTHUB_CLIENT_CONFIG* upperConfig)
{
    STRING_HANDLE result = STRING_construct_sprintf("%s.%s/%s/DeviceClientType=%s%%2F%s", upperConfig->iotHubName, upperConfig->iotHubSuffix, upperConfig->deviceId, CLIENT_DEVICE_TYPE_PREFIX, IOTHUB_SDK_VERSION);
    return result;
}

static int construct_credential_information(const IOTHUB_CLIENT_CONFIG* upperConfig, PMQTTTRANSPORT_HANDLE_DATA transport_data)
{
    int result;
    if (upperConfig->deviceKey != NULL)
    {
        transport_data->transport_creds.CREDENTIAL_VALUE.deviceKey = STRING_construct(upperConfig->deviceKey);
        if (transport_data->transport_creds.CREDENTIAL_VALUE.deviceKey == NULL)
        {
            LogError("Could not create device key for MQTT");
            result = __LINE__;
        }
        else if ((transport_data->devicesPath = STRING_construct_sprintf("%s.%s/devices/%s", upperConfig->iotHubName, upperConfig->iotHubSuffix, upperConfig->deviceId)) == NULL)
        {
            STRING_delete(transport_data->transport_creds.CREDENTIAL_VALUE.deviceKey);
            result = __LINE__;
        }
        else
        {
            transport_data->transport_creds.credential_type = DEVICE_KEY;
            result = 0;
        }
    }
    else if (upperConfig->deviceSasToken != NULL)
    {
        transport_data->transport_creds.CREDENTIAL_VALUE.deviceSasToken = STRING_construct(upperConfig->deviceSasToken);
        if (transport_data->transport_creds.CREDENTIAL_VALUE.deviceSasToken == NULL)
        {
            result = __LINE__;
        }
        else
        {
            transport_data->transport_creds.credential_type = SAS_TOKEN_FROM_USER;
            transport_data->devicesPath = NULL;
            result = 0;
        }
    }
    else
    {
        /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_041: [If both deviceKey and deviceSasToken fields are NULL then IoTHubTransportMqtt_Create shall assume a x509 authentication.] */
        transport_data->transport_creds.credential_type = X509;
        transport_data->devicesPath = NULL;
        result = 0;
    }
    return result;
}

static PMQTTTRANSPORT_HANDLE_DATA InitializeTransportHandleData(const IOTHUB_CLIENT_CONFIG* upperConfig, PDLIST_ENTRY waitingToSend)
{
    PMQTTTRANSPORT_HANDLE_DATA state = (PMQTTTRANSPORT_HANDLE_DATA)malloc(sizeof(MQTTTRANSPORT_HANDLE_DATA));
    if (state == NULL)
    {
        LogError("Could not create MQTT transport state. Memory allocation failed.");
    }
    else if ((state->device_id = STRING_construct(upperConfig->deviceId)) == NULL)
    {
        LogError("failure constructing device_id.");
        free(state);
        state = NULL;
    }
    else
    {
        if (construct_credential_information(upperConfig, state) != 0)
        {
            STRING_delete(state->device_id);
            free(state);
            state = NULL;
        }
        else if ( (state->topic_MqttEvent = STRING_construct_sprintf(TOPIC_DEVICE_DEVICE, upperConfig->deviceId) ) == NULL)
        {
            LogError("Could not create topic_MqttEvent for MQTT");
            STRING_delete(state->devicesPath);
            if (state->transport_creds.credential_type == DEVICE_KEY)
            {
                STRING_delete(state->transport_creds.CREDENTIAL_VALUE.deviceKey);
            }
            else if (state->transport_creds.credential_type == SAS_TOKEN_FROM_USER)
            {
                STRING_delete(state->transport_creds.CREDENTIAL_VALUE.deviceSasToken);
            }
            STRING_delete(state->device_id);
            free(state);
            state = NULL;
        }
        else
        {
            state->mqttClient = mqtt_client_init(mqtt_notification_callback, mqtt_operation_complete_callback, state);
            if (state->mqttClient == NULL)
            {
                LogError("failure initializing mqtt client.");
                STRING_delete(state->devicesPath);
                if (state->transport_creds.credential_type == DEVICE_KEY)
                {
                    STRING_delete(state->transport_creds.CREDENTIAL_VALUE.deviceKey);
                }
                else if (state->transport_creds.credential_type == SAS_TOKEN_FROM_USER)
                {
                    STRING_delete(state->transport_creds.CREDENTIAL_VALUE.deviceSasToken);
                }
                STRING_delete(state->topic_MqttEvent);
                STRING_delete(state->device_id);
                free(state);
                state = NULL;
            }
            else
            {
                /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_008: [If the upperConfig contains a valid protocolGatewayHostName value the this shall be used for the hostname, otherwise the hostname shall be constructed using the iothubname and iothubSuffix.] */
                if (upperConfig->protocolGatewayHostName == NULL)
                {
                     state->hostAddress = STRING_construct_sprintf("%s.%s", upperConfig->iotHubName, upperConfig->iotHubSuffix);
                }
                else
                {
                    state->hostAddress = STRING_construct(upperConfig->protocolGatewayHostName);
                }
                
                if (state->hostAddress == NULL)
                {
                    LogError("failure constructing host address.");
                    STRING_delete(state->devicesPath);
                    if (state->transport_creds.credential_type == DEVICE_KEY)
                    {
                        STRING_delete(state->transport_creds.CREDENTIAL_VALUE.deviceKey);
                    }
                    else if (state->transport_creds.credential_type == SAS_TOKEN_FROM_USER)
                    {
                        STRING_delete(state->transport_creds.CREDENTIAL_VALUE.deviceSasToken);
                    }
                    mqtt_client_deinit(state->mqttClient);
                    STRING_delete(state->topic_MqttEvent);
                    STRING_delete(state->device_id);
                    free(state);
                    state = NULL;
                }
                else if ((state->configPassedThroughUsername = buildConfigForUsername(upperConfig)) == NULL)
                {
                    STRING_delete(state->devicesPath);
                    if (state->transport_creds.credential_type == DEVICE_KEY)
                    {
                        STRING_delete(state->transport_creds.CREDENTIAL_VALUE.deviceKey);
                    }
                    else if (state->transport_creds.credential_type == SAS_TOKEN_FROM_USER)
                    {
                        STRING_delete(state->transport_creds.CREDENTIAL_VALUE.deviceSasToken);
                    }
                    mqtt_client_deinit(state->mqttClient);
                    STRING_delete(state->hostAddress);
                    STRING_delete(state->topic_MqttEvent);
                    STRING_delete(state->device_id);
                    free(state);
                    state = NULL;
                }
                else
                {
                    /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_010: [IoTHubTransportMqtt_Create shall allocate memory to save its internal state where all topics, hostname, device_id, device_key, sasTokenSr and client handle shall be saved.] */
                    DList_InitializeListHead(&(state->telemetry_waitingForAck));
                    state->isDestroyCalled = false;
                    state->isRegistered = false;
                    state->isConnected = false;
                    state->packetId = 1;
                    state->llClientHandle = NULL;
                    state->xioTransport = NULL;
                    state->portNum = 0;
                    state->waitingToSend = waitingToSend;
                    state->currPacketState = CONNECT_TYPE;
                    state->keepAliveValue = DEFAULT_MQTT_KEEPALIVE;
                    state->connectFailCount = 0;
                    state->connectTick = 0;
                    state->topic_MqttMessage = NULL;
                    state->topics_ToSubscribe = UNSUBSCRIBE_FROM_TOPIC;
                    state->log_trace = state->raw_trace = false;
                }
            }
        }
    }
    return state;
}

TRANSPORT_LL_HANDLE IoTHubTransport_MQTT_Common_Create(const IOTHUBTRANSPORT_CONFIG* config, MQTT_GET_IO_TRANSPORT get_io_transport)
{
    PMQTTTRANSPORT_HANDLE_DATA result;
    size_t deviceIdSize;

    /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_001: [If parameter config is NULL then IoTHubTransportMqtt_Create shall return NULL.] */
    /* Codes_SRS_IOTHUB_TRANSPORT_MQTT_COMMON_07_041: [ if get_io_transport is NULL then IoTHubTransport_MQTT_Common_Create shall return NULL. ] */
    if (config == NULL || get_io_transport == NULL)
    {
        LogError("Invalid Argument: Config Parameter is NULL.");
        result = NULL;
    }
    /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_002: [If the parameter config's variables upperConfig or waitingToSend are NULL then IoTHubTransportMqtt_Create shall return NULL.] */
    /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_003: [If the upperConfig's variables deviceId, both deviceKey and deviceSasToken, iotHubName, protocol, or iotHubSuffix are NULL then IoTHubTransportMqtt_Create shall return NULL.] */
    /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_03_003: [If both deviceKey & deviceSasToken fields are NOT NULL then IoTHubTransportMqtt_Create shall return NULL.] */
    else if (config->upperConfig == NULL ||
             config->upperConfig->protocol == NULL || 
             config->upperConfig->deviceId == NULL || 
            ((config->upperConfig->deviceKey != NULL) && (config->upperConfig->deviceSasToken != NULL)) ||
            config->upperConfig->iotHubName == NULL || 
            config->upperConfig->iotHubSuffix == NULL)
    {
        LogError("Invalid Argument: upperConfig structure contains an invalid parameter");
        result = NULL;
    }
    /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_002: [If the parameter config's variables upperConfig or waitingToSend are NULL then IoTHubTransportMqtt_Create shall return NULL.] */
    else if (config->waitingToSend == NULL)
    {
        LogError("Invalid Argument: waitingToSend is NULL)");
        result = NULL;
    }
    /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_006: [If the upperConfig's variables deviceId is an empty strings or length is greater then 128 then IoTHubTransportMqtt_Create shall return NULL.] */
    else if ( ( (deviceIdSize = strlen(config->upperConfig->deviceId)) > 128U) || (deviceIdSize == 0) )
    {
        LogError("Invalid Argument: DeviceId is of an invalid size");
        result = NULL;
    }
    /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_003: [If the upperConfig's variables deviceId, both deviceKey and deviceSasToken, iotHubName, protocol, or iotHubSuffix are NULL then IoTHubTransportMqtt_Create shall return NULL.] */
    else if ((config->upperConfig->deviceKey != NULL) && (strlen(config->upperConfig->deviceKey) == 0))
    {
        LogError("Invalid Argument: deviceKey is empty");
        result = NULL;
    }
    /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_003: [If the upperConfig's variables deviceId, both deviceKey and deviceSasToken, iotHubName, protocol, or iotHubSuffix are NULL then IoTHubTransportMqtt_Create shall return NULL.] */
    else if ((config->upperConfig->deviceSasToken != NULL) && (strlen(config->upperConfig->deviceSasToken) == 0))
    {
        LogError("Invalid Argument: deviceSasToken is empty");
        result = NULL;
    }
    /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_003: [If the upperConfig's variables deviceId, deviceKey, iotHubName, protocol, or iotHubSuffix are NULL then IoTHubTransportMqtt_Create shall return NULL.] */
    else if (strlen(config->upperConfig->iotHubName) == 0)
    {
        LogError("Invalid Argument: iotHubName is empty");
        result = NULL;
    }
    else if ((g_msgTickCounter = tickcounter_create()) == NULL)
    {
        LogError("Invalid Argument: iotHubName is empty");
        result = NULL;
    }
    else
    {
        result = InitializeTransportHandleData(config->upperConfig, config->waitingToSend);
        if (result == NULL)
        {
            tickcounter_destroy(g_msgTickCounter);
        }
        else
        {
            result->get_io_transport = get_io_transport;
        }
    }
    /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_009: [If any error is encountered then IoTHubTransportMqtt_Create shall return NULL.] */
    /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_011: [On Success IoTHubTransportMqtt_Create shall return a non-NULL value.] */
    return result;
}

static void DisconnectFromClient(PMQTTTRANSPORT_HANDLE_DATA transport_data)
{
    (void)mqtt_client_disconnect(transport_data->mqttClient);
    xio_destroy(transport_data->xioTransport);
    transport_data->xioTransport = NULL;

    transport_data->isConnected = false;
    transport_data->currPacketState = DISCONNECT_TYPE;
}

void IoTHubTransport_MQTT_Common_Destroy(TRANSPORT_LL_HANDLE handle)
{
    /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_012: [IoTHubTransportMqtt_Destroy shall do nothing if parameter handle is NULL.] */
    PMQTTTRANSPORT_HANDLE_DATA transport_data = (PMQTTTRANSPORT_HANDLE_DATA)handle;
    if (transport_data != NULL)
    {
        transport_data->isDestroyCalled = true;

        DisconnectFromClient(transport_data);

        //Empty the Waiting for Ack Messages.
        while (!DList_IsListEmpty(&transport_data->telemetry_waitingForAck))
        {
            PDLIST_ENTRY currentEntry = DList_RemoveHeadList(&transport_data->telemetry_waitingForAck);
            MQTT_MESSAGE_DETAILS_LIST* mqttMsgEntry = containingRecord(currentEntry, MQTT_MESSAGE_DETAILS_LIST, entry);
            sendMsgComplete(mqttMsgEntry->iotHubMessageEntry, transport_data, IOTHUB_CLIENT_CONFIRMATION_BECAUSE_DESTROY);
            free(mqttMsgEntry);
        }

        switch (transport_data->transport_creds.credential_type)
        {
            case SAS_TOKEN_FROM_USER:
                STRING_delete(transport_data->transport_creds.CREDENTIAL_VALUE.deviceSasToken);
                break;
            case DEVICE_KEY:
                STRING_delete(transport_data->transport_creds.CREDENTIAL_VALUE.deviceKey);
                STRING_delete(transport_data->devicesPath);
                break;
            case X509:
            default:
                break;
        }

        /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_014: [IoTHubTransportMqtt_Destroy shall free all the resources currently in use.] */
        mqtt_client_deinit(transport_data->mqttClient);
        STRING_delete(transport_data->topic_MqttEvent);
        STRING_delete(transport_data->topic_MqttMessage);
        STRING_delete(transport_data->device_id);
        STRING_delete(transport_data->hostAddress);
        STRING_delete(transport_data->configPassedThroughUsername);
        tickcounter_destroy(g_msgTickCounter);
        free(transport_data);
    }
}

int IoTHubTransport_MQTT_Common_Subscribe(IOTHUB_DEVICE_HANDLE handle)
{
    int result;
    PMQTTTRANSPORT_HANDLE_DATA transport_data = (PMQTTTRANSPORT_HANDLE_DATA)handle;
    if (transport_data == NULL)
    {
        /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_015: [If parameter handle is NULL than IoTHubTransportMqtt_Subscribe shall return a non-zero value.] */
        LogError("Invalid handle parameter. NULL.");
        result = __LINE__;
    }
    else
    {
        /* Code_SRS_IOTHUB_MQTT_TRANSPORT_07_016: [IoTHubTransportMqtt_Subscribe shall set a flag to enable mqtt_client_subscribe to be called to subscribe to the Message Topic.] */
        transport_data->topic_MqttMessage = STRING_construct_sprintf(TOPIC_DEVICE_MSG, STRING_c_str(transport_data->device_id) );
        if (transport_data->topic_MqttMessage == NULL)
        {
            LogError("Failure constructing Message Topic");
            result = __LINE__;
        }
        else
        {
            transport_data->topics_ToSubscribe |= SUBSCRIBE_TELEMETRY_TOPIC;
            /* Code_SRS_IOTHUB_MQTT_TRANSPORT_07_035: [If current packet state is not CONNACT, DISCONNECT_TYPE, or PACKET_TYPE_ERROR then IoTHubTransportMqtt_Subscribe shall set the packet state to SUBSCRIBE_TYPE.]*/
            if (transport_data->currPacketState != CONNACK_TYPE &&
                transport_data->currPacketState != CONNECT_TYPE &&
                transport_data->currPacketState != DISCONNECT_TYPE &&
                transport_data->currPacketState != PACKET_TYPE_ERROR)
            {
                transport_data->currPacketState = SUBSCRIBE_TYPE;
            }
            result = 0;
        }
    }
    return result;
}

void IoTHubTransport_MQTT_Common_Unsubscribe(IOTHUB_DEVICE_HANDLE handle)
{
    PMQTTTRANSPORT_HANDLE_DATA transport_data = (PMQTTTRANSPORT_HANDLE_DATA)handle;
    /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_019: [If parameter handle is NULL then IoTHubTransportMqtt_Unsubscribe shall do nothing.] */
    if (transport_data != NULL)
    {
        /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_020: [IoTHubTransportMqtt_Unsubscribe shall call mqtt_client_unsubscribe to unsubscribe the mqtt message topic.] */
        const char* unsubscribe[1];
        unsubscribe[0] = STRING_c_str(transport_data->topic_MqttMessage);
        if (mqtt_client_unsubscribe(transport_data->mqttClient, get_next_packet_id(transport_data), unsubscribe, 1) != 0)
        {
            LogError("Failure calling mqtt_client_unsubscribe");
        }
        STRING_delete(transport_data->topic_MqttMessage);
        transport_data->topic_MqttMessage = NULL;
        transport_data->topics_ToSubscribe &= ~SUBSCRIBE_TELEMETRY_TOPIC;
    }
    else
    {
        LogError("Invalid argument to unsubscribe (NULL).");
    }
}

void IoTHubTransport_MQTT_Common_DoWork(TRANSPORT_LL_HANDLE handle, IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle)
{
    /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_026: [IoTHubTransportMqtt_DoWork shall do nothing if parameter handle and/or iotHubClientHandle is NULL.] */
    PMQTTTRANSPORT_HANDLE_DATA transport_data = (PMQTTTRANSPORT_HANDLE_DATA)handle;
    if (transport_data != NULL && iotHubClientHandle != NULL)
    {
        transport_data->llClientHandle = iotHubClientHandle;

        if (InitializeConnection(transport_data) != 0)
        {
            // Don't want to flood the logs with failures here
        }
        else
        {
            if (transport_data->currPacketState == CONNACK_TYPE || transport_data->currPacketState == SUBSCRIBE_TYPE)
            {
                SubscribeToMqttProtocol(transport_data);
            }
            else if (transport_data->currPacketState == SUBACK_TYPE)
            {
                // Publish can be called now
                transport_data->currPacketState = PUBLISH_TYPE;
            }
            else if (transport_data->currPacketState == PUBLISH_TYPE)
            {
                PDLIST_ENTRY currentListEntry = transport_data->telemetry_waitingForAck.Flink;
                while (currentListEntry != &transport_data->telemetry_waitingForAck)
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
                            sendMsgComplete(mqttMsgEntry->iotHubMessageEntry, transport_data, IOTHUB_CLIENT_CONFIRMATION_MESSAGE_TIMEOUT);
                            free(mqttMsgEntry);
                        }
                        else
                        {
                            size_t messageLength;
                            const unsigned char* messagePayload = RetrieveMessagePayload(mqttMsgEntry->iotHubMessageEntry->messageHandle, &messageLength);
                            if (messageLength == 0 || messagePayload == NULL)
                            {
                                LogError("Failure from creating Message IoTHubMessage_GetData");
                            }
                            else
                            {
                                if (publish_mqtt_telemetry_msg(transport_data, mqttMsgEntry, messagePayload, messageLength) != 0)
                                {
                                    (void)DList_RemoveEntryList(currentListEntry);
                                    sendMsgComplete(mqttMsgEntry->iotHubMessageEntry, transport_data, IOTHUB_CLIENT_CONFIRMATION_ERROR);
                                    free(mqttMsgEntry);
                                }
                            }
                        }
                    }
                    currentListEntry = nextListEntry.Flink;
                }

                currentListEntry = transport_data->waitingToSend->Flink;
                /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_027: [IoTHubTransportMqtt_DoWork shall inspect the "waitingToSend" DLIST passed in config structure.] */
                while (currentListEntry != transport_data->waitingToSend)
                {
                    IOTHUB_MESSAGE_LIST* iothubMsgList = containingRecord(currentListEntry, IOTHUB_MESSAGE_LIST, entry);
                    DLIST_ENTRY savedFromCurrentListEntry;
                    savedFromCurrentListEntry.Flink = currentListEntry->Flink;

                    /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_027: [IoTHubTransportMqtt_DoWork shall inspect the "waitingToSend" DLIST passed in config structure.] */
                    size_t messageLength;
                    const unsigned char* messagePayload = RetrieveMessagePayload(iothubMsgList->messageHandle, &messageLength);
                    if (messageLength == 0 || messagePayload == NULL)
                    {
                        LogError("Failure result from IoTHubMessage_GetData");
                    }
                    else
                    {
                        /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_029: [IoTHubTransportMqtt_DoWork shall create a MQTT_MESSAGE_HANDLE and pass this to a call to mqtt_client_publish.] */
                        MQTT_MESSAGE_DETAILS_LIST* mqttMsgEntry = (MQTT_MESSAGE_DETAILS_LIST*)malloc(sizeof(MQTT_MESSAGE_DETAILS_LIST));
                        if (mqttMsgEntry == NULL)
                        {
                            LogError("Allocation Error: Failure allocating MQTT Message Detail List.");
                        }
                        else
                        {
                            mqttMsgEntry->retryCount = 0;
                            mqttMsgEntry->iotHubMessageEntry = iothubMsgList;
                            mqttMsgEntry->packet_id = get_next_packet_id(transport_data);
                            if (publish_mqtt_telemetry_msg(transport_data, mqttMsgEntry, messagePayload, messageLength) != 0)
                            {
                                (void)(DList_RemoveEntryList(currentListEntry));
                                sendMsgComplete(iothubMsgList, transport_data, IOTHUB_CLIENT_CONFIRMATION_ERROR);
                                free(mqttMsgEntry);
                            }
                            else
                            {
                                (void)(DList_RemoveEntryList(currentListEntry));
                                DList_InsertTailList(&(transport_data->telemetry_waitingForAck), &(mqttMsgEntry->entry));
                            }
                        }
                    }
                    currentListEntry = savedFromCurrentListEntry.Flink;
                }
            }
            /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_030: [IoTHubTransportMqtt_DoWork shall call mqtt_client_dowork everytime it is called if it is isConnected.] */
            mqtt_client_dowork(transport_data->mqttClient);
        }
    }
}

IOTHUB_CLIENT_RESULT IoTHubTransport_MQTT_Common_GetSendStatus(IOTHUB_DEVICE_HANDLE handle, IOTHUB_CLIENT_STATUS *iotHubClientStatus)
{
    IOTHUB_CLIENT_RESULT result;

    if (handle == NULL || iotHubClientStatus == NULL)
    {
        /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_023: [IoTHubTransportMqtt_GetSendStatus shall return IOTHUB_CLIENT_INVALID_ARG if called with NULL parameter.] */
        LogError("invalid arument.");
        result = IOTHUB_CLIENT_INVALID_ARG;
    }
    else
    {
        MQTTTRANSPORT_HANDLE_DATA* handleData = (MQTTTRANSPORT_HANDLE_DATA*)handle;
        if (!DList_IsListEmpty(handleData->waitingToSend) || !DList_IsListEmpty(&(handleData->telemetry_waitingForAck)))
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

IOTHUB_CLIENT_RESULT IoTHubTransport_MQTT_Common_SetOption(TRANSPORT_LL_HANDLE handle, const char* option, const void* value)
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
        LogError("invalid parameter (NULL) passed to clientTransportAMQP_SetOption.");
    }
    else
    {
        MQTTTRANSPORT_HANDLE_DATA* transport_data = (MQTTTRANSPORT_HANDLE_DATA*)handle;
        /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_031: [If the option parameter is set to "logtrace" then the value shall be a bool_ptr and the value will determine if the mqtt client log is on or off.] */
        if (strcmp(OPTION_LOG_TRACE, option) == 0)
        {
            transport_data->log_trace = *((bool*)value);
            mqtt_client_set_trace(transport_data->mqttClient, transport_data->log_trace, transport_data->raw_trace);
            result = IOTHUB_CLIENT_OK;
        }
        else if (strcmp("rawlogtrace", option) == 0)
        {
            transport_data->raw_trace = *((bool*)value);
            mqtt_client_set_trace(transport_data->mqttClient, transport_data->log_trace, transport_data->raw_trace);
            result = IOTHUB_CLIENT_OK;
        }
        else if (strcmp(OPTION_KEEP_ALIVE, option) == 0)
        {
            /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_036: [If the option parameter is set to "keepalive" then the value shall be a int_ptr and the value will determine the mqtt keepalive time that is set for pings.] */
            int* keepAliveOption = (int*)value;
            /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_037 : [If the option parameter is set to supplied int_ptr keepalive is the same value as the existing keepalive then IoTHubTransportMqtt_SetOption shall do nothing.] */
            if (*keepAliveOption != transport_data->keepAliveValue)
            {
                transport_data->keepAliveValue = (uint16_t)(*keepAliveOption);
                if (transport_data->isConnected)
                {
                    /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_038: [If the client is isConnected when the keepalive is set then IoTHubTransportMqtt_SetOption shall disconnect and reconnect with the specified keepalive value.] */
                    DisconnectFromClient(transport_data);
                }
            }
            result = IOTHUB_CLIENT_OK;
        }
        /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_039: [If the option parameter is set to "x509certificate" then the value shall be a const char of the certificate to be used for x509.] */
        else if ((strcmp(OPTION_X509_CERT, option) == 0) && (transport_data->transport_creds.credential_type != X509))
        {
            LogError("x509certificate specified, but authentication method is not x509");
            result = IOTHUB_CLIENT_INVALID_ARG;
        }
        /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_040: [If the option parameter is set to "x509privatekey" then the value shall be a const char of the RSA Private Key to be used for x509.] */
        else if ((strcmp(OPTION_X509_PRIVATE_KEY, option) == 0) && (transport_data->transport_creds.credential_type != X509))
        {
            LogError("x509privatekey specified, but authentication method is not x509");
            result = IOTHUB_CLIENT_INVALID_ARG;
        }
        else
        {
            /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_032: [IoTHubTransportMqtt_SetOption shall pass down the option to xio_setoption if the option parameter is not a known option string for the MQTT transport.] */
            if (GetTransportProviderIfNecessary(transport_data) == 0)
            {
                if (xio_setoption(transport_data->xioTransport, option, value) == 0)
                {
                    result = IOTHUB_CLIENT_OK;
                }
                else
                {
                    /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_132: [IoTHubTransportMqtt_SetOption shall return IOTHUB_CLIENT_INVALID_ARG xio_setoption fails] */
                    result = IOTHUB_CLIENT_INVALID_ARG;
                }
            }
            else
            {
                result = IOTHUB_CLIENT_ERROR;
            }
        }
    }
    return result;
}

IOTHUB_DEVICE_HANDLE IoTHubTransport_MQTT_Common_Register(TRANSPORT_LL_HANDLE handle, const IOTHUB_DEVICE_CONFIG* device, IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle, PDLIST_ENTRY waitingToSend)
{
    IOTHUB_DEVICE_HANDLE result = NULL;
    (void)iotHubClientHandle;

    // Codes_SRS_IOTHUB_MQTT_TRANSPORT_17_001: [ IoTHubTransportMqtt_Register shall return NULL if the TRANSPORT_LL_HANDLE is NULL.]
    // Codes_SRS_IOTHUB_MQTT_TRANSPORT_17_002: [ IoTHubTransportMqtt_Register shall return NULL if device or waitingToSend are NULL.]
    if ((handle == NULL) || (device == NULL) || (waitingToSend == NULL))
    {
        LogError("IoTHubTransportMqtt_Register: handle, device or waitingToSend is NULL.");
        result = NULL;
    }
    else
    {
        MQTTTRANSPORT_HANDLE_DATA* transport_data = (MQTTTRANSPORT_HANDLE_DATA*)handle;

        // Codes_SRS_IOTHUB_MQTT_TRANSPORT_03_001: [ IoTHubTransportMqtt_Register shall return NULL if deviceId, or both deviceKey and deviceSasToken are NULL.]
        if (device->deviceId == NULL)
        {
            LogError("IoTHubTransportMqtt_Register: deviceId is NULL.");
            result = NULL;
        }
        // Codes_SRS_IOTHUB_MQTT_TRANSPORT_03_002: [ IoTHubTransportMqtt_Register shall return NULL if both deviceKey and deviceSasToken are provided.]
        else if ((device->deviceKey != NULL) && (device->deviceSasToken != NULL))
        {
            LogError("IoTHubTransportMqtt_Register: Both deviceKey and deviceSasToken are defined. Only one can be used.");
            result = NULL;
        }
        else
        {
            // Codes_SRS_IOTHUB_MQTT_TRANSPORT_17_003: [ IoTHubTransportMqtt_Register shall return NULL if deviceId or deviceKey do not match the deviceId and deviceKey passed in during IoTHubTransportMqtt_Create.]
            if (strcmp(STRING_c_str(transport_data->device_id), device->deviceId) != 0)
            {
                LogError("IoTHubTransportMqtt_Register: deviceId does not match.");
                result = NULL;
            }
            else if ( (transport_data->transport_creds.credential_type == DEVICE_KEY) && (strcmp(STRING_c_str(transport_data->transport_creds.CREDENTIAL_VALUE.deviceKey), device->deviceKey) != 0))
            {
                LogError("IoTHubTransportMqtt_Register: deviceKey does not match.");
                result = NULL;
            }
            else
            {
                if (transport_data->isRegistered == true)
                {
                    LogError("Transport already has device registered by id: [%s]", device->deviceId);
                    result = NULL;
                }
                else
                {
                    transport_data->isRegistered = true;
                    // Codes_SRS_IOTHUB_MQTT_TRANSPORT_17_004: [ IoTHubTransportMqtt_Register shall return the TRANSPORT_LL_HANDLE as the IOTHUB_DEVICE_HANDLE. ]
                    result = (IOTHUB_DEVICE_HANDLE)handle;
                }
            }
        }
    }

    return result;
}

// Codes_SRS_IOTHUB_MQTT_TRANSPORT_17_005: [ IoTHubTransportMqtt_Unregister shall return. ]
void IoTHubTransport_MQTT_Common_Unregister(IOTHUB_DEVICE_HANDLE deviceHandle)
{
    if (deviceHandle != NULL)
    {
        MQTTTRANSPORT_HANDLE_DATA* transport_data = (MQTTTRANSPORT_HANDLE_DATA*)deviceHandle;

        transport_data->isRegistered = false;
    }
}

STRING_HANDLE IoTHubTransport_MQTT_Common_GetHostname(TRANSPORT_LL_HANDLE handle)
{
    STRING_HANDLE result;
    /*Codes_SRS_IOTHUB_MQTT_TRANSPORT_02_001: [ If handle is NULL then IoTHubTransportMqtt_GetHostname shall fail and return NULL. ]*/
    if (handle == NULL)
    {
        result = NULL;
    }
    else
    {
        /*Codes_SRS_IOTHUB_MQTT_TRANSPORT_02_002: [ Otherwise IoTHubTransportMqtt_GetHostname shall return a non-NULL STRING_HANDLE containg the hostname. ]*/
        result = ((MQTTTRANSPORT_HANDLE_DATA*)handle)->hostAddress;
    }
    return result;
}
