// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#include <stdint.h>
#include <time.h>
#include <limits.h>
#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_c_shared_utility/doublylinkedlist.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/platform.h"
#include "azure_c_shared_utility/sastoken.h"
#include "azure_c_shared_utility/strings.h"
#include "azure_c_shared_utility/urlencode.h"
#include "azure_c_shared_utility/tlsio.h"

#include "azure_uamqp_c/cbs.h"
#include "azure_uamqp_c/link.h"
#include "azure_uamqp_c/message.h"
#include "azure_uamqp_c/amqpvalue.h"
#include "azure_uamqp_c/message_receiver.h"
#include "azure_uamqp_c/message_sender.h"
#include "azure_uamqp_c/messaging.h"
#include "azure_uamqp_c/sasl_mssbcbs.h"
#include "azure_uamqp_c/saslclientio.h"

#include "iothub_client_ll.h"
#include "iothub_client_private.h"
#include "iothubtransportamqp.h"
#include "iothub_client_version.h"

#define RESULT_OK 0
#define RESULT_FAILURE 1
#define RESULT_TIMEOUT 2

#define RFC1035_MAX_FQDN_LENGTH 255
#define DEFAULT_IOTHUB_AMQP_PORT 5671
#define DEFAULT_SAS_TOKEN_LIFETIME_MS 3600000
#define DEFAULT_CBS_REQUEST_TIMEOUT_MS 30000
#define CBS_AUDIENCE "servicebus.windows.net:sastoken"
#define DEFAULT_CONTAINER_ID "default_container_id"
#define DEFAULT_INCOMING_WINDOW_SIZE UINT_MAX
#define DEFAULT_OUTGOING_WINDOW_SIZE 100
#define MESSAGE_RECEIVER_LINK_NAME "receiver-link"
#define MESSAGE_RECEIVER_TARGET_ADDRESS "ingress-rx"
#define MESSAGE_RECEIVER_MAX_LINK_SIZE 65536
#define MESSAGE_SENDER_LINK_NAME "sender-link"
#define MESSAGE_SENDER_SOURCE_ADDRESS "ingress"
#define MESSAGE_SENDER_MAX_LINK_SIZE UINT64_MAX

typedef XIO_HANDLE(*TLS_IO_TRANSPORT_PROVIDER)(const char* fqdn, int port);

typedef enum CBS_STATE_TAG
{
    CBS_STATE_IDLE,
    CBS_STATE_AUTH_IN_PROGRESS,
    CBS_STATE_AUTHENTICATED
} CBS_STATE;

typedef enum AMQP_TRANSPORT_CREDENTIAL_TYPE_TAG
{
    CREDENTIAL_NOT_BUILD,
    X509,
    DEVICE_KEY,
    DEVICE_SAS_TOKEN,
}AMQP_TRANSPORT_CREDENTIAL_TYPE;

typedef struct X509_CREDENTIAL_TAG
{
    const char* x509certificate;
    const char* x509privatekey;
}X509_CREDENTIAL;

typedef union AMQP_TRANSPORT_CREDENTIAL_UNION_TAG
{
    // Key associated to the device to be used.
    STRING_HANDLE deviceKey;
    
    // SAS associated to the device to be used.
    STRING_HANDLE deviceSasToken;

    // X509 
    X509_CREDENTIAL x509credential;
}AMQP_TRANSPORT_CREDENTIAL_UNION;

typedef struct AMQP_TRANSPORT_CREDENTIAL_TAG
{
    AMQP_TRANSPORT_CREDENTIAL_TYPE credentialType;
    AMQP_TRANSPORT_CREDENTIAL_UNION credential;
}AMQP_TRANSPORT_CREDENTIAL;

typedef struct AMQP_TRANSPORT_STATE_TAG
{
    // FQDN of the IoT Hub.
    STRING_HANDLE iotHubHostFqdn;
    // AMQP port of the IoT Hub.
    int iotHubPort;
    // contains the credentials to be used
    AMQP_TRANSPORT_CREDENTIAL credential;
    // Address to which the transport will connect to and send events.
    STRING_HANDLE targetAddress;
    // Address to which the transport will connect to and receive messages from.
    STRING_HANDLE messageReceiveAddress;
    // A component of the SAS token. Currently this must be an empty string.
    STRING_HANDLE sasTokenKeyName;
    // Internal parameter that identifies the current logical device within the service.
    STRING_HANDLE devicesPath;
    // How long a SAS token created by the transport is valid, in milliseconds.
    size_t sas_token_lifetime;
    // Maximum period of time for the transport to wait before refreshing the SAS token it created previously, in milliseconds.
    size_t sas_token_refresh_time;
    // Maximum time the transport waits for  uAMQP cbs_put_token() to complete before marking it a failure, in milliseconds.
    size_t cbs_request_timeout;
    // Maximum time for the connection establishment/retry logic should wait for a connection to succeed, in milliseconds.
    size_t connection_timeout;
    // Saved reference to the IoTHub LL Client.
    IOTHUB_CLIENT_LL_HANDLE iothub_client_handle;

    // TSL I/O transport.
    XIO_HANDLE tls_io;
    // Pointer to the function that creates the TLS I/O (internal use only).
    TLS_IO_TRANSPORT_PROVIDER tls_io_transport_provider;
    // AMQP SASL I/O transport created on top of the TLS I/O layer.
    XIO_HANDLE sasl_io;
    // AMQP SASL I/O mechanism to be used.
    SASL_MECHANISM_HANDLE sasl_mechanism;
    // AMQP connection.
    CONNECTION_HANDLE connection;
    // Current AMQP connection state;
    AMQP_MANAGEMENT_STATE connection_state;
    // Last time the AMQP connection establishment was initiated.
    size_t connection_establish_time;
    // AMQP session.
    SESSION_HANDLE session;
    // AMQP link used by the event sender.
    LINK_HANDLE sender_link;
    // uAMQP event sender.
    MESSAGE_SENDER_HANDLE message_sender;
    // Internal flag that controls if messages should be received or not.
    bool receive_messages;
    // AMQP link used by the message receiver.
    LINK_HANDLE receiver_link;
    // uAMQP message receiver.
    MESSAGE_RECEIVER_HANDLE message_receiver;
    // List with events still pending to be sent. It is provided by the upper layer.
    PDLIST_ENTRY waitingToSend;
    // Internal list with the items currently being processed/sent through uAMQP.
    DLIST_ENTRY inProgress;
    // Connection instance with the Azure IoT CBS.
    CBS_HANDLE cbs;
    // Current state of the CBS connection.
    CBS_STATE cbs_state;
    // Time when the current SAS token was created, in seconds since epoch.
    size_t current_sas_token_create_time;
    // Mark if device is registered in transport (only one device per transport).
    bool isRegistered;
    // Turns logging on and off
    bool is_trace_on;
} AMQP_TRANSPORT_INSTANCE;



// Auxiliary functions

static STRING_HANDLE concat3Params(const char* prefix, const char* infix, const char* suffix)
{
    STRING_HANDLE result = NULL;
    char* concat;
    size_t totalLength = strlen(prefix) + strlen(infix) + strlen(suffix) + 1; // One extra for \0.

    if ((concat = (char*)malloc(totalLength)) != NULL)
    {
        (void)strcpy(concat, prefix);
        (void)strcat(concat, infix);
        (void)strcat(concat, suffix);
        result = STRING_construct(concat);
        free(concat);
    }
    else
    {
        result = NULL;
    }

    return result;
}

static size_t getSecondsSinceEpoch(void)
{
    return (size_t)(difftime(get_time(NULL), (time_t)0));
}

static void trackEventInProgress(IOTHUB_MESSAGE_LIST* message, AMQP_TRANSPORT_INSTANCE* transport_state)
{
    DList_RemoveEntryList(&message->entry);
    DList_InsertTailList(&transport_state->inProgress, &message->entry);
}

static IOTHUB_MESSAGE_LIST* getNextEventToSend(AMQP_TRANSPORT_INSTANCE* transport_state)
{
    IOTHUB_MESSAGE_LIST* message;

    if (!DList_IsListEmpty(transport_state->waitingToSend))
    {
        PDLIST_ENTRY list_entry = transport_state->waitingToSend->Flink;
        message = containingRecord(list_entry, IOTHUB_MESSAGE_LIST, entry);
    }
    else
    {
        message = NULL;
    }

    return message;
}

static int isEventInInProgressList(IOTHUB_MESSAGE_LIST* message)
{
    return !DList_IsListEmpty(&message->entry);
}

static void removeEventFromInProgressList(IOTHUB_MESSAGE_LIST* message)
{
    DList_RemoveEntryList(&message->entry);
    DList_InitializeListHead(&message->entry);
}

static void rollEventBackToWaitList(IOTHUB_MESSAGE_LIST* message, AMQP_TRANSPORT_INSTANCE* transport_state)
{
    removeEventFromInProgressList(message);
    DList_InsertTailList(transport_state->waitingToSend, &message->entry);
}

static void rollEventsBackToWaitList(AMQP_TRANSPORT_INSTANCE* transport_state)
{
    PDLIST_ENTRY entry = transport_state->inProgress.Blink;

    while (entry != &transport_state->inProgress)
    {
        IOTHUB_MESSAGE_LIST* message = containingRecord(entry, IOTHUB_MESSAGE_LIST, entry);
        entry = entry->Blink;
        rollEventBackToWaitList(message, transport_state);
    }
}


static int addPropertiesTouAMQPMessage(IOTHUB_MESSAGE_HANDLE iothub_message_handle, MESSAGE_HANDLE uamqp_message)
{
    int result;
    MAP_HANDLE properties_map;
    const char* const* propertyKeys;
    const char* const* propertyValues;
    size_t propertyCount;

    /* Codes_SRS_IOTHUBTRANSPORTAMQP_01_007: [The IoTHub message properties shall be obtained by calling IoTHubMessage_Properties.] */
    properties_map = IoTHubMessage_Properties(iothub_message_handle);
    if (properties_map == NULL)
    {
        /* Codes_SRS_IOTHUBTRANSPORTAMQP_01_014: [If any of the APIs fails while building the property map and setting it on the uAMQP message, IoTHubTransportAMQP_DoWork shall notify the failure by invoking the upper layer message send callback with IOTHUB_CLIENT_CONFIRMATION_ERROR.] */
        LogError("Failed to get property map from IoTHub message.");
        result = __LINE__;
    }
    /* Codes_SRS_IOTHUBTRANSPORTAMQP_01_015: [The actual keys and values, as well as the number of properties shall be obtained by calling Map_GetInternals on the handle obtained from IoTHubMessage_Properties.] */
    else if (Map_GetInternals(properties_map, &propertyKeys, &propertyValues, &propertyCount) != MAP_OK)
    {
        /* Codes_SRS_IOTHUBTRANSPORTAMQP_01_014: [If any of the APIs fails while building the property map and setting it on the uAMQP message, IoTHubTransportAMQP_DoWork shall notify the failure by invoking the upper layer message send callback with IOTHUB_CLIENT_CONFIRMATION_ERROR.] */
        LogError("Failed to get the internals of the property map.");
        result = __LINE__;
    }
    else
    {
        /* Codes_SRS_IOTHUBTRANSPORTAMQP_01_016: [If the number of properties is 0, no uAMQP map shall be created and no application properties shall be set on the uAMQP message.] */
        if (propertyCount != 0)
        {
            size_t i;
            /* Codes_SRS_IOTHUBTRANSPORTAMQP_01_009: [The uAMQP map shall be created by calling amqpvalue_create_map.] */
            AMQP_VALUE uamqp_map = amqpvalue_create_map();
            if (uamqp_map == NULL)
            {
                /* Codes_SRS_IOTHUBTRANSPORTAMQP_01_014: [If any of the APIs fails while building the property map and setting it on the uAMQP message, IoTHubTransportAMQP_DoWork shall notify the failure by invoking the upper layer message send callback with IOTHUB_CLIENT_CONFIRMATION_ERROR.] */
                LogError("Failed to create uAMQP map for the properties.");
                result = __LINE__;
            }
            else
            {
                for (i = 0; i < propertyCount; i++)
                {
                    /* Codes_SRS_IOTHUBTRANSPORTAMQP_01_010: [A key uAMQP value shall be created by using amqpvalue_create_string.] */
                    AMQP_VALUE map_key_value = amqpvalue_create_string(propertyKeys[i]);
                    if (map_key_value == NULL)
                    {
                        /* Codes_SRS_IOTHUBTRANSPORTAMQP_01_014: [If any of the APIs fails while building the property map and setting it on the uAMQP message, IoTHubTransportAMQP_DoWork shall notify the failure by invoking the upper layer message send callback with IOTHUB_CLIENT_CONFIRMATION_ERROR.] */
                        LogError("Failed to create uAMQP property key value.");
                        break;
                    }

                    /* Codes_SRS_IOTHUBTRANSPORTAMQP_01_011: [A value uAMQP value shall be created by using amqpvalue_create_string.] */
                    AMQP_VALUE map_value_value = amqpvalue_create_string(propertyValues[i]);
                    if (map_value_value == NULL)
                    {
                        amqpvalue_destroy(map_key_value);
                        /* Codes_SRS_IOTHUBTRANSPORTAMQP_01_014: [If any of the APIs fails while building the property map and setting it on the uAMQP message, IoTHubTransportAMQP_DoWork shall notify the failure by invoking the upper layer message send callback with IOTHUB_CLIENT_CONFIRMATION_ERROR.] */
                        LogError("Failed to create uAMQP property key value.");
                        break;
                    }

                    /* Codes_SRS_IOTHUBTRANSPORTAMQP_01_008: [All properties shall be transferred to a uAMQP map.] */
                    /* Codes_SRS_IOTHUBTRANSPORTAMQP_01_012: [The key/value pair for the property shall be set into the uAMQP property map by calling amqpvalue_map_set_value.] */
                    if (amqpvalue_set_map_value(uamqp_map, map_key_value, map_value_value) != 0)
                    {
                        amqpvalue_destroy(map_key_value);
                        amqpvalue_destroy(map_value_value);
                        /* Codes_SRS_IOTHUBTRANSPORTAMQP_01_014: [If any of the APIs fails while building the property map and setting it on the uAMQP message, IoTHubTransportAMQP_DoWork shall notify the failure by invoking the upper layer message send callback with IOTHUB_CLIENT_CONFIRMATION_ERROR.] */
                        LogError("Failed to create uAMQP property key value.");
                        break;
                    }

                    amqpvalue_destroy(map_key_value);
                    amqpvalue_destroy(map_value_value);
                }

                if (i < propertyCount)
                {
                    result = __LINE__;
                }
                else
                {
                    /* Codes_SRS_IOTHUBTRANSPORTAMQP_01_013: [After all properties have been filled in the uAMQP map, the uAMQP properties map shall be set on the uAMQP message by calling message_set_application_properties.] */
                    if (message_set_application_properties(uamqp_message, uamqp_map) != 0)
                    {
                        /* Codes_SRS_IOTHUBTRANSPORTAMQP_01_014: [If any of the APIs fails while building the property map and setting it on the uAMQP message, IoTHubTransportAMQP_DoWork shall notify the failure by invoking the upper layer message send callback with IOTHUB_CLIENT_CONFIRMATION_ERROR.] */
                        LogError("Failed to transfer the message properties to the uAMQP message.");
                        result = __LINE__;
                    }
                    else
                    {
                        result = 0;
                    }
                }

                amqpvalue_destroy(uamqp_map);
            }
        }
        else
        {
            result = 0;
        }
    }

    return result;
}

static int readPropertiesFromuAMQPMessage(IOTHUB_MESSAGE_HANDLE iothub_message_handle, MESSAGE_HANDLE uamqp_message)
{
    int return_value;
    PROPERTIES_HANDLE uamqp_message_properties;
    AMQP_VALUE uamqp_message_property;
    const char* uamqp_message_property_value;
    int api_call_result;

    /* Codes_SRS_IOTHUBTRANSPORTAMQP_09_155: [uAMQP message properties shall be retrieved using message_get_properties.] */
    if ((api_call_result = message_get_properties(uamqp_message, &uamqp_message_properties)) != 0)
    {
        /* Codes_SRS_IOTHUBTRANSPORTAMQP_09_156: [If message_get_properties fails, the error shall be notified and ‘on_message_received' shall continue.] */
        LogError("Failed to get property properties map from uAMQP message (error code %d).", api_call_result);
        return_value = __LINE__;
    }
    else
    {
        return_value = 0; // Properties 'message-id' and 'correlation-id' are optional according to the AMQP 1.0 spec.
    
        /* Codes_SRS_IOTHUBTRANSPORTAMQP_09_157: [The message-id property shall be read from the uAMQP message by calling properties_get_message_id.] */
        if ((api_call_result = properties_get_message_id(uamqp_message_properties, &uamqp_message_property)) != 0)
        {
            /* Codes_SRS_IOTHUBTRANSPORTAMQP_09_158: [If properties_get_message_id fails, the error shall be notified and ‘on_message_received' shall continue.] */
            LogInfo("Failed to get value of uAMQP message 'message-id' property (%d).", api_call_result);
            return_value = __LINE__;
        }
        else if (amqpvalue_get_type(uamqp_message_property) != AMQP_TYPE_NULL)
        {
            /* Codes_SRS_IOTHUBTRANSPORTAMQP_09_159: [The message-id value shall be retrieved from the AMQP_VALUE as char* by calling amqpvalue_get_string.] */
            if ((api_call_result = amqpvalue_get_string(uamqp_message_property, &uamqp_message_property_value)) != 0)
            {
                /* Codes_SRS_IOTHUBTRANSPORTAMQP_09_160: [If amqpvalue_get_string fails, the error shall be notified and ‘on_message_received' shall continue.] */
                LogError("Failed to get value of uAMQP message 'message-id' property (%d).", api_call_result);
                return_value = __LINE__;
            }
            /* Codes_SRS_IOTHUBTRANSPORTAMQP_09_161: [The message-id property shall be set on the IOTHUB_MESSAGE_HANDLE by calling IoTHubMessage_SetMessageId, passing the value read from the uAMQP message.] */
            else if (IoTHubMessage_SetMessageId(iothub_message_handle, uamqp_message_property_value) != IOTHUB_MESSAGE_OK)
            {
                /* Codes_SRS_IOTHUBTRANSPORTAMQP_09_162: [If IoTHubMessage_SetMessageId fails, the error shall be notified and ‘on_message_received' shall continue.] */
                LogError("Failed to set IOTHUB_MESSAGE_HANDLE 'message-id' property.");
                return_value = __LINE__;
            }
        }

        /* Codes_SRS_IOTHUBTRANSPORTAMQP_09_163: [The correlation-id property shall be read from the uAMQP message by calling properties_get_correlation_id.] */
        if ((api_call_result = properties_get_correlation_id(uamqp_message_properties, &uamqp_message_property)) != 0)
        {
            /* Codes_SRS_IOTHUBTRANSPORTAMQP_09_164: [If properties_get_correlation_id fails, the error shall be notified and ‘on_message_received' shall continue.] */
            LogError("Failed to get value of uAMQP message 'correlation-id' property (%d).", api_call_result);
            return_value = __LINE__;
        }
        else if (amqpvalue_get_type(uamqp_message_property) != AMQP_TYPE_NULL)
        {
            /* Codes_SRS_IOTHUBTRANSPORTAMQP_09_165: [The correlation-id value shall be retrieved from the AMQP_VALUE as char* by calling amqpvalue_get_string.] */
            if ((api_call_result = amqpvalue_get_string(uamqp_message_property, &uamqp_message_property_value)) != 0)
            {
                /* Codes_SRS_IOTHUBTRANSPORTAMQP_09_166: [If amqpvalue_get_string fails, the error shall be notified and ‘on_message_received' shall continue.] */
                LogError("Failed to get value of uAMQP message 'correlation-id' property (%d).", api_call_result);
                return_value = __LINE__;
            }
            /* Codes_SRS_IOTHUBTRANSPORTAMQP_09_167: [The correlation-id property shall be set on the IOTHUB_MESSAGE_HANDLE by calling IoTHubMessage_SetCorrelationId, passing the value read from the uAMQP message.] */
            else if (IoTHubMessage_SetCorrelationId(iothub_message_handle, uamqp_message_property_value) != IOTHUB_MESSAGE_OK)
            {
                /* Codes_SRS_IOTHUBTRANSPORTAMQP_09_168: [If IoTHubMessage_SetCorrelationId fails, the error shall be notified and ‘on_message_received' shall continue.] */
                LogError("Failed to set IOTHUB_MESSAGE_HANDLE 'correlation-id' property.");
                return_value = __LINE__;
            }
        }
        properties_destroy(uamqp_message_properties);
    }

    return return_value;
}

static int readApplicationPropertiesFromuAMQPMessage(IOTHUB_MESSAGE_HANDLE iothub_message_handle, MESSAGE_HANDLE uamqp_message)
{
    int result;
    AMQP_VALUE uamqp_app_properties;
    uint32_t property_count;
    MAP_HANDLE iothub_message_properties_map;

    // Codes_SRS_IOTHUBTRANSPORTAMQP_09_170: [The IOTHUB_MESSAGE_HANDLE properties shall be retrieved using IoTHubMessage_Properties.]
    if ((iothub_message_properties_map = IoTHubMessage_Properties(iothub_message_handle)) == NULL)
    {
        // Codes_SRS_IOTHUBTRANSPORTAMQP_09_186: [If IoTHubMessage_Properties fails, the error shall be notified and ‘on_message_received' shall continue.]
        LogError("Failed to get property map from IoTHub message.");
        result = __LINE__;
    }
    // Codes_SRS_IOTHUBTRANSPORTAMQP_09_171: [uAMQP message application properties shall be retrieved using message_get_application_properties.]
    else if ((result = message_get_application_properties(uamqp_message, &uamqp_app_properties)) != 0)
    {
        // Codes_SRS_IOTHUBTRANSPORTAMQP_09_172: [If message_get_application_properties fails, the error shall be notified and ‘on_message_received' shall continue.]
        LogError("Failed reading the incoming uAMQP message properties (return code %d).", result);
        result = __LINE__;
    }
    // Codes_SRS_IOTHUBTRANSPORTAMQP_09_187: [If message_get_application_properties succeeds but returns a NULL application properties map (there are no properties), ‘on_message_received' shall continue normally.]
    else if (uamqp_app_properties == NULL)
    {
        result = 0;
    }
    // Codes_SRS_IOTHUBTRANSPORTAMQP_09_173: [The actual uAMQP message application properties should be extracted from the result of message_get_application_properties using amqpvalue_get_inplace_described_value.]
    else if ((uamqp_app_properties = amqpvalue_get_inplace_described_value(uamqp_app_properties)) == NULL)
    {
        // Codes_SRS_IOTHUBTRANSPORTAMQP_09_174: [If amqpvalue_get_inplace_described_value fails, the error shall be notified and ‘on_message_received' shall continue.]
        LogError("Failed getting the map of uAMQP message application properties (return code %d).", result);
        result = __LINE__;
    }
    // Codes_SRS_IOTHUBTRANSPORTAMQP_09_175: [The number of items in the uAMQP message application properties shall be obtained using amqpvalue_get_map_pair_count.]
    else if ((result = amqpvalue_get_map_pair_count(uamqp_app_properties, &property_count)) != 0)
    {
        // Codes_SRS_IOTHUBTRANSPORTAMQP_09_176: [If amqpvalue_get_map_pair_count fails, the error shall be notified and ‘on_message_received' shall continue.]
        LogError("Failed reading the number of values in the uAMQP property map (return code %d).", result);
        result = __LINE__;
    }
    else
    {
        // Codes_SRS_IOTHUBTRANSPORTAMQP_09_177: [‘on_message_received' shall iterate through each uAMQP application property and add it on IOTHUB_MESSAGE_HANDLE properties.]
        uint32_t i;
        for (i = 0; i < property_count; i++)
        {
            AMQP_VALUE map_key_name;
            AMQP_VALUE map_key_value;
            const char *key_name;
            const char* key_value;

            // Codes_SRS_IOTHUBTRANSPORTAMQP_09_178: [The uAMQP application property name and value shall be obtained using amqpvalue_get_map_key_value_pair.]
            if ((result = amqpvalue_get_map_key_value_pair(uamqp_app_properties, i, &map_key_name, &map_key_value)) != 0)
            {
                // Codes_SRS_IOTHUBTRANSPORTAMQP_09_179: [If amqpvalue_get_map_key_value_pair fails, the error shall be notified and ‘on_message_received' shall continue.]
                LogError("Failed reading the key/value pair from the uAMQP property map (return code %d).", result);
                result = __LINE__;
                break;
            }
            // Codes_SRS_IOTHUBTRANSPORTAMQP_09_180: [The uAMQP application property name shall be extracted as string using amqpvalue_get_string.]
            else if ((result = amqpvalue_get_string(map_key_name, &key_name)) != 0)
            {
                // Codes_SRS_IOTHUBTRANSPORTAMQP_09_181: [If amqpvalue_get_string fails, the error shall be notified and ‘on_message_received' shall continue.]
                LogError("Failed parsing the uAMQP property name (return code %d).", result);
                result = __LINE__;
                break;
            }
            // Codes_SRS_IOTHUBTRANSPORTAMQP_09_182: [The uAMQP application property value shall be extracted as string using amqpvalue_get_string.]
            else if ((result = amqpvalue_get_string(map_key_value, &key_value)) != 0)
            {
                // Codes_SRS_IOTHUBTRANSPORTAMQP_09_183: [If amqpvalue_get_string fails, the error shall be notified and ‘on_message_received' shall continue.]
                LogError("Failed parsing the uAMQP property value (return code %d).", result);
                result = __LINE__;
                break;
            }
            // Codes_SRS_IOTHUBTRANSPORTAMQP_09_184: [The application property name and value shall be added to IOTHUB_MESSAGE_HANDLE properties using Map_AddOrUpdate.]
            else if (Map_AddOrUpdate(iothub_message_properties_map, key_name, key_value) != MAP_OK)
            {
                // Codes_SRS_IOTHUBTRANSPORTAMQP_09_185: [If Map_AddOrUpdate fails, the error shall be notified and ‘on_message_received' shall continue.]
                LogError("Failed to add/update IoTHub message property map.");
                result = __LINE__;
                break;
            }
        }
    }

    return result;
}

static void on_message_send_complete(void* context, MESSAGE_SEND_RESULT send_result)
{
    IOTHUB_MESSAGE_LIST* message = (IOTHUB_MESSAGE_LIST*)context;

    IOTHUB_CLIENT_RESULT iot_hub_send_result;

    // Codes_SRS_IOTHUBTRANSPORTAMQP_09_142: [The callback 'on_message_send_complete' shall pass to the upper layer callback an IOTHUB_CLIENT_CONFIRMATION_OK if the result received is MESSAGE_SEND_OK] 
    if (send_result == MESSAGE_SEND_OK)
    {
        iot_hub_send_result = IOTHUB_CLIENT_CONFIRMATION_OK;
    }
    // Codes_SRS_IOTHUBTRANSPORTAMQP_09_143: [The callback 'on_message_send_complete' shall pass to the upper layer callback an IOTHUB_CLIENT_CONFIRMATION_ERROR if the result received is MESSAGE_SEND_ERROR]
    else
    {
        iot_hub_send_result = IOTHUB_CLIENT_CONFIRMATION_ERROR;
    }

    // Codes_SRS_IOTHUBTRANSPORTAMQP_09_102: [The callback 'on_message_send_complete' shall invoke the upper layer callback for message received if provided] 
    if (message->callback != NULL)
    {
        message->callback(iot_hub_send_result, message->context);
    }

    // Codes_SRS_IOTHUBTRANSPORTAMQP_09_100: [The callback 'on_message_send_complete' shall remove the target message from the in-progress list after the upper layer callback] 
    if (isEventInInProgressList(message))
    {
        removeEventFromInProgressList(message);
    }

    // Codes_SRS_IOTHUBTRANSPORTAMQP_09_151: [The callback 'on_message_send_complete' shall destroy the message handle (IOTHUB_MESSAGE_HANDLE) using IoTHubMessage_Destroy()]
    IoTHubMessage_Destroy(message->messageHandle);

    // Codes_SRS_IOTHUBTRANSPORTAMQP_09_152: [The callback 'on_message_send_complete' shall destroy the IOTHUB_MESSAGE_LIST instance]
    free(message);
}

static void on_put_token_complete(void* context, CBS_OPERATION_RESULT operation_result, unsigned int status_code, const char* status_description)
{
    AMQP_TRANSPORT_INSTANCE* transportState = (AMQP_TRANSPORT_INSTANCE*)context;

    if (operation_result == CBS_OPERATION_RESULT_OK)
    {
        transportState->cbs_state = CBS_STATE_AUTHENTICATED;
    }
    else
    {
        LogError("CBS reported status %u error: %s", status_code, status_description);
    }
}

static AMQP_VALUE on_message_received(const void* context, MESSAGE_HANDLE message)
{
    AMQP_VALUE result = NULL;

    // Codes_SRS_IOTHUBTRANSPORTAMQP_09_104: [The callback 'on_message_received' shall invoke IoTHubClient_LL_MessageCallback() passing the client and the incoming message handles as parameters] 
    IOTHUB_MESSAGE_HANDLE iothub_message = NULL;
    MESSAGE_BODY_TYPE body_type;

    if (message_get_body_type(message, &body_type) != 0)
    {
        LogError("Failed to get the type of the message received by the transport.");
    }
    else
    {
        if (body_type == MESSAGE_BODY_TYPE_DATA)
        {
            BINARY_DATA binary_data;
            if (message_get_body_amqp_data(message, 0, &binary_data) != 0)
            {
                LogError("Failed to get the body of the message received by the transport.");
            }
            else
            {
                iothub_message = IoTHubMessage_CreateFromByteArray(binary_data.bytes, binary_data.length);
            }
        }
    }

    if (iothub_message == NULL)
    {
        LogError("Transport failed processing the message received.");

        result = messaging_delivery_rejected("Rejected due to failure reading AMQP message", "Failed reading message body");
    }
    else
    {
        /* Codes_SRS_IOTHUBTRANSPORTAMQP_09_153: [The callback ‘on_message_received' shall read the message-id property from the uAMQP message and set it on the IoT Hub Message if the property is defined.] */
        /* Codes_SRS_IOTHUBTRANSPORTAMQP_09_154: [The callback ‘on_message_received' shall read the correlation-id property from the uAMQP message and set it on the IoT Hub Message if the property is defined.] */
        if (readPropertiesFromuAMQPMessage(iothub_message, message) != 0)
        {
            LogError("Transport failed reading properties of the message received.");
        }

        // Codes_SRS_IOTHUBTRANSPORTAMQP_09_169: [The callback ‘on_message_received' shall read the application properties from the uAMQP message and set it on the IoT Hub Message if any are provided.]
        if (readApplicationPropertiesFromuAMQPMessage(iothub_message, message) != 0)
        {
            // Codes_SRS_IOTHUBTRANSPORTAMQP_09_188: [If ‘on_message_received' fails reading the application properties from the uAMQP message, it shall NOT call IoTHubClient_LL_MessageCallback and shall reject the message.]
            LogError("Transport failed reading application properties of the message received.");
            
            result = messaging_delivery_rejected("Rejected due to failure reading AMQP message", "Failed reading application properties");
        }
        else
        {
            IOTHUBMESSAGE_DISPOSITION_RESULT disposition_result;

        disposition_result = IoTHubClient_LL_MessageCallback((IOTHUB_CLIENT_LL_HANDLE)context, iothub_message);

        // Codes_SRS_IOTHUBTRANSPORTAMQP_09_105: [The callback 'on_message_received' shall return the result of messaging_delivery_accepted() if the IoTHubClient_LL_MessageCallback() returns IOTHUBMESSAGE_ACCEPTED] 
        if (disposition_result == IOTHUBMESSAGE_ACCEPTED)
        {
            result = messaging_delivery_accepted();
        }
        // Codes_SRS_IOTHUBTRANSPORTAMQP_09_106: [The callback 'on_message_received' shall return the result of messaging_delivery_released() if the IoTHubClient_LL_MessageCallback() returns IOTHUBMESSAGE_ABANDONED] 
        else if (disposition_result == IOTHUBMESSAGE_ABANDONED)
        {
            result = messaging_delivery_released();
        }
        // Codes_SRS_IOTHUBTRANSPORTAMQP_09_107: [The callback 'on_message_received' shall return the result of messaging_delivery_rejected("Rejected by application", "Rejected by application") if the IoTHubClient_LL_MessageCallback() returns IOTHUBMESSAGE_REJECTED] 
        else if (disposition_result == IOTHUBMESSAGE_REJECTED)
        {
            result = messaging_delivery_rejected("Rejected by application", "Rejected by application");
        }
        }

        IoTHubMessage_Destroy(iothub_message);
    }

    return result;
}

static XIO_HANDLE getTLSIOTransport(const char* fqdn, int port)
{
    TLSIO_CONFIG tls_io_config;
    tls_io_config.hostname = fqdn;
    tls_io_config.port = port;
    const IO_INTERFACE_DESCRIPTION* io_interface_description = platform_get_default_tlsio();
    return xio_create(io_interface_description, &tls_io_config);
}

static void destroyConnection(AMQP_TRANSPORT_INSTANCE* transport_state)
{
    if (transport_state->cbs != NULL)
    {
        cbs_destroy(transport_state->cbs);
        transport_state->cbs = NULL;
    }

    if (transport_state->session != NULL)
    {
        session_destroy(transport_state->session);
        transport_state->session = NULL;
    }

    if (transport_state->connection != NULL)
    {
        connection_destroy(transport_state->connection);
        transport_state->connection = NULL;
    }

    if (transport_state->sasl_io != NULL)
    {
        xio_destroy(transport_state->sasl_io);
        transport_state->sasl_io = NULL;
    }

    if (transport_state->sasl_mechanism != NULL)
    {
        saslmechanism_destroy(transport_state->sasl_mechanism);
        transport_state->sasl_mechanism = NULL;
    }

    if (transport_state->tls_io != NULL)
    {
        // Codes_SRS_IOTHUBTRANSPORTAMQP_09_034: [IoTHubTransportAMQP_Destroy shall destroy the AMQP TLS I/O transport.]
        xio_destroy(transport_state->tls_io);
        transport_state->tls_io = NULL;
    }
}

static void on_amqp_management_state_changed(void* context, AMQP_MANAGEMENT_STATE new_amqp_management_state, AMQP_MANAGEMENT_STATE previous_amqp_management_state)
{
    (void)previous_amqp_management_state;
    AMQP_TRANSPORT_INSTANCE* transport_state = (AMQP_TRANSPORT_INSTANCE*)context;

    if (transport_state != NULL)
    {
        transport_state->connection_state = new_amqp_management_state;
    }
}

static void on_connection_io_error(void* context)
{
    AMQP_TRANSPORT_INSTANCE* transport_state = (AMQP_TRANSPORT_INSTANCE*)context;

    if (transport_state != NULL)
    {
        transport_state->connection_state = AMQP_MANAGEMENT_STATE_ERROR;
    }
}

static int establishConnection(AMQP_TRANSPORT_INSTANCE* transport_state)
{
    int result;

    // Codes_SRS_IOTHUBTRANSPORTAMQP_09_110: [IoTHubTransportAMQP_DoWork shall create the TLS IO using transport_state->io_transport_provider callback function] 
    if (transport_state->tls_io == NULL &&
        (transport_state->tls_io = transport_state->tls_io_transport_provider(STRING_c_str(transport_state->iotHubHostFqdn), transport_state->iotHubPort)) == NULL)
    {
        // Codes_SRS_IOTHUBTRANSPORTAMQP_09_136: [If transport_state->io_transport_provider_callback fails, IoTHubTransportAMQP_DoWork shall fail and return immediately]
        result = RESULT_FAILURE;
        LogError("Failed to obtain a TLS I/O transport layer.");
    }
    // Codes_SRS_IOTHUBTRANSPORTAMQP_09_056: [IoTHubTransportAMQP_DoWork shall create the SASL mechanism using AMQP's saslmechanism_create() API] 
    else if ((transport_state->sasl_mechanism = saslmechanism_create(saslmssbcbs_get_interface(), NULL)) == NULL)
    {
        // Codes_SRS_IOTHUBTRANSPORTAMQP_09_057: [If saslmechanism_create() fails, IoTHubTransportAMQP_DoWork shall fail and return immediately]
        result = RESULT_FAILURE;
        LogError("Failed to create a SASL mechanism.");
    }
    else
    {
        // Codes_SRS_IOTHUBTRANSPORTAMQP_09_060: [IoTHubTransportAMQP_DoWork shall create the SASL I / O layer using the xio_create() C Shared Utility API]
        SASLCLIENTIO_CONFIG sasl_client_config;
        sasl_client_config.sasl_mechanism = transport_state->sasl_mechanism;
        sasl_client_config.underlying_io = transport_state->tls_io;
        if ((transport_state->sasl_io = xio_create(saslclientio_get_interface_description(), &sasl_client_config)) == NULL)
        {
            // Codes_SRS_IOTHUBTRANSPORTAMQP_09_061: [If xio_create() fails creating the SASL I/O layer, IoTHubTransportAMQP_DoWork shall fail and return immediately] 
            result = RESULT_FAILURE;
            LogError("Failed to create a SASL I/O layer.");
        }
        // Codes_SRS_IOTHUBTRANSPORTAMQP_09_062: [IoTHubTransportAMQP_DoWork shall create the connection with the IoT service using connection_create2() AMQP API, passing the SASL I/O layer, IoT Hub FQDN and container ID as parameters (pass NULL for callbacks)] 
        else if ((transport_state->connection = connection_create2(transport_state->sasl_io, STRING_c_str(transport_state->iotHubHostFqdn), DEFAULT_CONTAINER_ID, NULL, NULL, NULL, NULL, on_connection_io_error, (void*)transport_state)) == NULL)
        {
            // Codes_SRS_IOTHUBTRANSPORTAMQP_09_063: [If connection_create2() fails, IoTHubTransportAMQP_DoWork shall fail and return immediately.] 
            result = RESULT_FAILURE;
            LogError("Failed to create the AMQP connection.");
        }
        // Codes_SRS_IOTHUBTRANSPORTAMQP_09_137: [IoTHubTransportAMQP_DoWork shall create the AMQP session session_create() AMQP API, passing the connection instance as parameter]
        else if ((transport_state->session = session_create(transport_state->connection, NULL, NULL)) == NULL)
        {
            // Codes_SRS_IOTHUBTRANSPORTAMQP_09_138 : [If session_create() fails, IoTHubTransportAMQP_DoWork shall fail and return immediately]
            result = RESULT_FAILURE;
            LogError("Failed to create the AMQP session.");
        }
        else
        {
            // Codes_SRS_IOTHUBTRANSPORTAMQP_09_065: [IoTHubTransportAMQP_DoWork shall apply a default value of UINT_MAX for the parameter 'AMQP incoming window'] 
            if (session_set_incoming_window(transport_state->session, (uint32_t)DEFAULT_INCOMING_WINDOW_SIZE) != 0)
            {
                LogError("Failed to set the AMQP incoming window size.");
            }

            // Codes_SRS_IOTHUBTRANSPORTAMQP_09_115: [IoTHubTransportAMQP_DoWork shall apply a default value of 100 for the parameter 'AMQP outgoing window'] 
            if (session_set_outgoing_window(transport_state->session, DEFAULT_OUTGOING_WINDOW_SIZE) != 0)
            {
                LogError("Failed to set the AMQP outgoing window size.");
            }

            // Codes_SRS_IOTHUBTRANSPORTAMQP_09_066: [IoTHubTransportAMQP_DoWork shall establish the CBS connection using the cbs_create() AMQP API] 
            if ((transport_state->cbs = cbs_create(transport_state->session, on_amqp_management_state_changed, NULL)) == NULL)
            {
                // Codes_SRS_IOTHUBTRANSPORTAMQP_09_067: [If cbs_create() fails, IoTHubTransportAMQP_DoWork shall fail and return immediately] 
                result = RESULT_FAILURE;
                LogError("Failed to create the CBS connection.");
            }
            // Codes_SRS_IOTHUBTRANSPORTAMQP_09_139: [IoTHubTransportAMQP_DoWork shall open the CBS connection using the cbs_open() AMQP API] 
            else if (cbs_open(transport_state->cbs) != 0)
            {
                // Codes_SRS_IOTHUBTRANSPORTAMQP_09_140: [If cbs_open() fails, IoTHubTransportAMQP_DoWork shall fail and return immediately]
                result = RESULT_FAILURE;
                LogError("Failed to open the connection with CBS.");
            }
            else
            {
                transport_state->connection_establish_time = getSecondsSinceEpoch();
                transport_state->cbs_state = CBS_STATE_IDLE;
                connection_set_trace(transport_state->connection, transport_state->is_trace_on);
                (void)xio_setoption(transport_state->sasl_io, "logtrace", &transport_state->is_trace_on);
                result = RESULT_OK;
            }
        }
    }

    if (result == RESULT_FAILURE)
    {
        destroyConnection(transport_state);
    }

    return result;
}

static int handSASTokenToCbs(AMQP_TRANSPORT_INSTANCE* transport_state, STRING_HANDLE sasToken, size_t sas_token_create_time)
{
    int result;
    if (cbs_put_token(transport_state->cbs, CBS_AUDIENCE, STRING_c_str(transport_state->devicesPath), STRING_c_str(sasToken), on_put_token_complete, transport_state) != RESULT_OK)
    {
        LogError("Failed applying new SAS token to CBS.");
        result = __LINE__;
    }
    else
    {
        transport_state->cbs_state = CBS_STATE_AUTH_IN_PROGRESS;
        transport_state->current_sas_token_create_time = sas_token_create_time;
        result = RESULT_OK;
    }
    return result;
}

static int startAuthentication(AMQP_TRANSPORT_INSTANCE* transport_state)
{
    int result;

    size_t sas_token_create_time = getSecondsSinceEpoch(); // I.e.: NOW, in seconds since epoch.

                                                           // Codes_SRS_IOTHUBTRANSPORTAMQP_09_083: [Each new SAS token created by the transport shall be valid for up to 'sas_token_lifetime' milliseconds from the time of creation]
    size_t new_expiry_time = sas_token_create_time + (transport_state->sas_token_lifetime / 1000);

    STRING_HANDLE newSASToken;

    switch (transport_state->credential.credentialType)
    {
        default:
        {
            result = __LINE__;
            LogError("internal error, unexpected enum value transport_state->credential.credentialType=%d", transport_state->credential.credentialType);
            break;
        }
        case DEVICE_KEY:
        {
            newSASToken = SASToken_Create(transport_state->credential.credential.deviceKey, transport_state->devicesPath, transport_state->sasTokenKeyName, new_expiry_time);
            if (newSASToken == NULL)
            {
                LogError("Could not generate a new SAS token for the CBS.");
                result = RESULT_FAILURE;
            }
            else
            {
                if (handSASTokenToCbs(transport_state, newSASToken, sas_token_create_time) != 0)
                {
                    LogError("unable to handSASTokenToCbs");
                    result = RESULT_FAILURE;
                }
                else
                {
                    result = RESULT_OK;
                }

                // Codes_SRS_IOTHUBTRANSPORTAMQP_09_145: [Each new SAS token created shall be deleted from memory immediately after sending it to CBS]
                STRING_delete(newSASToken);
            }
            break;
        }
        case DEVICE_SAS_TOKEN:
        {
            newSASToken = STRING_clone(transport_state->credential.credential.deviceSasToken);
            if (newSASToken == NULL)
            {
                LogError("Could not generate a new SAS token for the CBS.");
                result = RESULT_FAILURE;
            }
            else
            {
                if (handSASTokenToCbs(transport_state, newSASToken, sas_token_create_time) != 0)
                {
                    LogError("unable to handSASTokenToCbs");
                    result = RESULT_FAILURE;
                }
                else
                {
                    result = RESULT_OK;
                }

                // Codes_SRS_IOTHUBTRANSPORTAMQP_09_145: [Each new SAS token created shall be deleted from memory immediately after sending it to CBS]
                STRING_delete(newSASToken);
            }
            break;
        }
    }
    return result;
}

static int verifyAuthenticationTimeout(AMQP_TRANSPORT_INSTANCE* transport_state)
{
    return ((getSecondsSinceEpoch() - transport_state->current_sas_token_create_time) * 1000 >= transport_state->cbs_request_timeout) ? RESULT_TIMEOUT : RESULT_OK;
}

static void attachDeviceClientTypeToLink(LINK_HANDLE link)
{
    fields attach_properties;
    AMQP_VALUE deviceClientTypeKeyName;
    AMQP_VALUE deviceClientTypeValue;
    int result;

    //
    // Attempt to add the device client type string to the attach properties.
    // If this doesn't happen, well, this isn't that important.  We can operate
    // without this property.  It's worth noting that even though we are going
    // on, the reasons any of these operations fail don't bode well for the
    // actual upcoming attach.
    //

    // Codes_SRS_IOTHUBTRANSPORTAMQP_06_187: [If IotHubTransportAMQP_DoWork fails to create an attach properties map and assign that map to the link the function will STILL proceed with the attempt to create the message sender.]

    if ((attach_properties = amqpvalue_create_map()) == NULL)
    {
        LogError("Failed to create the map for device client type.");
    }
    else
    {
        if ((deviceClientTypeKeyName = amqpvalue_create_symbol("com.microsoft:client-version")) == NULL)
        {
            LogError("Failed to create the key name for the device client type.");
        }
        else
        {
            if ((deviceClientTypeValue = amqpvalue_create_string(CLIENT_DEVICE_TYPE_PREFIX CLIENT_DEVICE_BACKSLASH IOTHUB_SDK_VERSION)) == NULL)
            {
                LogError("Failed to create the key value for the device client type.");
            }
            else
            {
                if ((result = amqpvalue_set_map_value(attach_properties, deviceClientTypeKeyName, deviceClientTypeValue)) != 0)
                {
                    LogError("Failed to set the property map for the device client type.  Error code is: %d", result);
                }
                else if ((result = link_set_attach_properties(link, attach_properties)) != 0)
                {
                    LogError("Unable to attach the device client type to the link properties. Error code is: %d", result);
                }

                amqpvalue_destroy(deviceClientTypeValue);
            }

            amqpvalue_destroy(deviceClientTypeKeyName);
        }

        amqpvalue_destroy(attach_properties);
    }
}

static void destroyEventSender(AMQP_TRANSPORT_INSTANCE* transport_state)
{
    if (transport_state->message_sender != NULL)
    {
        messagesender_destroy(transport_state->message_sender);
        transport_state->message_sender = NULL;

        link_destroy(transport_state->sender_link);
        transport_state->sender_link = NULL;
    }
}

void on_event_sender_state_changed(void* context, MESSAGE_SENDER_STATE new_state, MESSAGE_SENDER_STATE previous_state)
{
    (void)context;
    (void)new_state;
    (void)previous_state;
}

static int createEventSender(AMQP_TRANSPORT_INSTANCE* transport_state)
{
    int result = RESULT_FAILURE;

    if (transport_state->message_sender == NULL)
    {
        AMQP_VALUE source = NULL;
        AMQP_VALUE target = NULL;

        // Codes_SRS_IOTHUBTRANSPORTAMQP_09_068: [IoTHubTransportAMQP_DoWork shall create the AMQP link for sending messages using 'source' as "ingress", target as the IoT hub FQDN, link name as "sender-link" and role as 'role_sender'] 
        if ((source = messaging_create_source(MESSAGE_SENDER_SOURCE_ADDRESS)) == NULL)
        {
            LogError("Failed creating AMQP messaging source attribute.");
        }
        else if ((target = messaging_create_target(STRING_c_str(transport_state->targetAddress))) == NULL)
        {
            LogError("Failed creating AMQP messaging target attribute.");
        }
        else if ((transport_state->sender_link = link_create(transport_state->session, MESSAGE_SENDER_LINK_NAME, role_sender, source, target)) == NULL)
        {
            // Codes_SRS_IOTHUBTRANSPORTAMQP_09_069: [If IoTHubTransportAMQP_DoWork fails to create the AMQP link for sending messages, the function shall fail and return immediately, flagging the connection to be re-stablished] 
            LogError("Failed creating AMQP link for message sender.");
        }
        else
        {
            // Codes_SRS_IOTHUBTRANSPORTAMQP_09_119: [IoTHubTransportAMQP_DoWork shall apply a default value of 65536 for the parameter 'Link MAX message size']
            if (link_set_max_message_size(transport_state->sender_link, MESSAGE_SENDER_MAX_LINK_SIZE) != RESULT_OK)
            {
                LogError("Failed setting AMQP link max message size.");
            }

            attachDeviceClientTypeToLink(transport_state->sender_link);

            // Codes_SRS_IOTHUBTRANSPORTAMQP_09_070: [IoTHubTransportAMQP_DoWork shall create the AMQP message sender using messagesender_create() AMQP API] 
            if ((transport_state->message_sender = messagesender_create(transport_state->sender_link, on_event_sender_state_changed, (void*)transport_state)) == NULL)
            {
                // Codes_SRS_IOTHUBTRANSPORTAMQP_09_071: [IoTHubTransportAMQP_DoWork shall fail and return immediately if the AMQP message sender instance fails to be created, flagging the connection to be re-established] 
                LogError("Could not allocate AMQP message sender");
            }
            else
            {
                // Codes_SRS_IOTHUBTRANSPORTAMQP_09_072: [IoTHubTransportAMQP_DoWork shall open the AMQP message sender using messagesender_open() AMQP API] 
                if (messagesender_open(transport_state->message_sender) != RESULT_OK)
                {
                    // Codes_SRS_IOTHUBTRANSPORTAMQP_09_073: [IoTHubTransportAMQP_DoWork shall fail and return immediately if the AMQP message sender instance fails to be opened, flagging the connection to be re-established] 
                    LogError("Failed opening the AMQP message sender.");
                }
                else
                {
                    result = RESULT_OK;
                }
            }
        }

        if (source != NULL)
            amqpvalue_destroy(source);
        if (target != NULL)
            amqpvalue_destroy(target);
    }

    return result;
}

static int destroyMessageReceiver(AMQP_TRANSPORT_INSTANCE* transport_state)
{
    int result = RESULT_FAILURE;

    if (transport_state->message_receiver != NULL)
    {
        if (messagereceiver_close(transport_state->message_receiver) != RESULT_OK)
        {
            LogError("Failed closing the AMQP message receiver.");
        }

        messagereceiver_destroy(transport_state->message_receiver);

        transport_state->message_receiver = NULL;

        link_destroy(transport_state->receiver_link);

        transport_state->receiver_link = NULL;

        result = RESULT_OK;
    }

    return result;
}

static int createMessageReceiver(AMQP_TRANSPORT_INSTANCE* transport_state, IOTHUB_CLIENT_LL_HANDLE iothub_client_handle)
{
    int result = RESULT_FAILURE;

    if (transport_state->message_receiver == NULL)
    {
        AMQP_VALUE source = NULL;
        AMQP_VALUE target = NULL;

        // Codes_SRS_IOTHUBTRANSPORTAMQP_09_074: [IoTHubTransportAMQP_DoWork shall create the AMQP link for receiving messages using 'source' as messageReceiveAddress, target as the "ingress-rx", link name as "receiver-link" and role as 'role_receiver'] 
        if ((source = messaging_create_source(STRING_c_str(transport_state->messageReceiveAddress))) == NULL)
        {
            LogError("Failed creating AMQP message receiver source attribute.");
        }
        else if ((target = messaging_create_target(MESSAGE_RECEIVER_TARGET_ADDRESS)) == NULL)
        {
            LogError("Failed creating AMQP message receiver target attribute.");
        }
        else if ((transport_state->receiver_link = link_create(transport_state->session, MESSAGE_RECEIVER_LINK_NAME, role_receiver, source, target)) == NULL)
        {
            // Codes_SRS_IOTHUBTRANSPORTAMQP_09_075: [If IoTHubTransportAMQP_DoWork fails to create the AMQP link for receiving messages, the function shall fail and return immediately, flagging the connection to be re-stablished] 
            LogError("Failed creating AMQP link for message receiver.");
        }
        // Codes_SRS_IOTHUBTRANSPORTAMQP_09_076: [IoTHubTransportAMQP_DoWork shall set the receiver link settle mode as receiver_settle_mode_first] 
        else if (link_set_rcv_settle_mode(transport_state->receiver_link, receiver_settle_mode_first) != RESULT_OK)
        {
            // Codes_SRS_IOTHUBTRANSPORTAMQP_09_141: [If IoTHubTransportAMQP_DoWork fails to set the settle mode on the AMQP link for receiving messages, the function shall fail and return immediately, flagging the connection to be re-stablished]
            LogError("Failed setting AMQP link settle mode for message receiver.");
        }
        else
        {
            // Codes_SRS_IOTHUBTRANSPORTAMQP_09_119: [IoTHubTransportAMQP_DoWork shall apply a default value of 65536 for the parameter 'Link MAX message size']
            if (link_set_max_message_size(transport_state->receiver_link, MESSAGE_RECEIVER_MAX_LINK_SIZE) != RESULT_OK)
            {
                LogError("Failed setting AMQP link max message size for message receiver.");
            }

            attachDeviceClientTypeToLink(transport_state->receiver_link);

            // Codes_SRS_IOTHUBTRANSPORTAMQP_09_077: [IoTHubTransportAMQP_DoWork shall create the AMQP message receiver using messagereceiver_create() AMQP API] 
            if ((transport_state->message_receiver = messagereceiver_create(transport_state->receiver_link, NULL, NULL)) == NULL)
            {
                // Codes_SRS_IOTHUBTRANSPORTAMQP_09_078: [IoTHubTransportAMQP_DoWork shall fail and return immediately if the AMQP message receiver instance fails to be created, flagging the connection to be re-established] 
                LogError("Could not allocate AMQP message receiver.");
            }
            else
            {
                // Codes_SRS_IOTHUBTRANSPORTAMQP_09_079: [IoTHubTransportAMQP_DoWork shall open the AMQP message receiver using messagereceiver_open() AMQP API, passing a callback function for handling C2D incoming messages] 
                // Codes_SRS_IOTHUBTRANSPORTAMQP_09_123: [IoTHubTransportAMQP_DoWork shall create each AMQP message_receiver passing the 'on_message_received' as the callback function] 
                if (messagereceiver_open(transport_state->message_receiver, on_message_received, (const void*)iothub_client_handle) != RESULT_OK)
                {
                    // Codes_SRS_IOTHUBTRANSPORTAMQP_09_080: [IoTHubTransportAMQP_DoWork shall fail and return immediately if the AMQP message receiver instance fails to be opened, flagging the connection to be re-established] 
                    LogError("Failed opening the AMQP message receiver.");
                }
                else
                {
                    result = RESULT_OK;
                }
            }
        }

        if (source != NULL)
            amqpvalue_destroy(source);
        if (target != NULL)
            amqpvalue_destroy(target);
    }

    return result;
}

static int sendPendingEvents(AMQP_TRANSPORT_INSTANCE* transport_state)
{
    int result = RESULT_OK;
    IOTHUB_MESSAGE_LIST* message;

    while ((message = getNextEventToSend(transport_state)) != NULL)
    {
        result = RESULT_FAILURE;

        IOTHUBMESSAGE_CONTENT_TYPE contentType = IoTHubMessage_GetContentType(message->messageHandle);
        const unsigned char* messageContent = NULL;
        size_t messageContentSize = 0;
        MESSAGE_HANDLE amqp_message = NULL;
        bool is_message_error = false;

        // Codes_SRS_IOTHUBTRANSPORTAMQP_09_086: [IoTHubTransportAMQP_DoWork shall move queued events to an "in-progress" list right before processing them for sending]
        trackEventInProgress(message, transport_state);

        // Codes_SRS_IOTHUBTRANSPORTAMQP_09_087: [If the event contains a message of type IOTHUBMESSAGE_BYTEARRAY, IoTHubTransportAMQP_DoWork shall obtain its char* representation and size using IoTHubMessage_GetByteArray()] 
        if (contentType == IOTHUBMESSAGE_BYTEARRAY &&
            IoTHubMessage_GetByteArray(message->messageHandle, &messageContent, &messageContentSize) != IOTHUB_MESSAGE_OK)
        {
            LogError("Failed getting the BYTE array representation of the event content to be sent.");
            is_message_error = true;
        }
        // Codes_SRS_IOTHUBTRANSPORTAMQP_09_089: [If the event contains a message of type IOTHUBMESSAGE_STRING, IoTHubTransportAMQP_DoWork shall obtain its char* representation using IoTHubMessage_GetString()] 
        // Codes_SRS_IOTHUBTRANSPORTAMQP_09_090: [If the event contains a message of type IOTHUBMESSAGE_STRING, IoTHubTransportAMQP_DoWork shall obtain the size of its char* representation using strlen()] 
        else if (contentType == IOTHUBMESSAGE_STRING &&
            ((messageContent = (const unsigned char*)IoTHubMessage_GetString(message->messageHandle)) == NULL))
        {
            LogError("Failed getting the STRING representation of the event content to be sent.");
            is_message_error = true;
        }
        // Codes_SRS_IOTHUBTRANSPORTAMQP_09_092: [If the event contains a message of type IOTHUBMESSAGE_UNKNOWN, IoTHubTransportAMQP_DoWork shall remove the event from the in-progress list and invoke the upper layer callback reporting the error] 
        else if (contentType == IOTHUBMESSAGE_UNKNOWN)
        {
            LogError("Cannot send events with content type IOTHUBMESSAGE_UNKNOWN.");
            is_message_error = true;
        }
        // Codes_SRS_IOTHUBTRANSPORTAMQP_09_093: [IoTHubTransportAMQP_DoWork shall create an amqp message using message_create() uAMQP API] 
        else if ((amqp_message = message_create()) == NULL)
        {
            LogError("Failed allocating the AMQP message for sending the event.");
        }
        else
        {
            BINARY_DATA binary_data;

            if (contentType == IOTHUBMESSAGE_STRING)
            {
                messageContentSize = strlen((const char*)messageContent);
            }

            binary_data.bytes = messageContent;
            binary_data.length = messageContentSize;

            // Codes_SRS_IOTHUBTRANSPORTAMQP_09_095: [IoTHubTransportAMQP_DoWork shall set the AMQP message body using message_add_body_amqp_data() uAMQP API] 
            if (message_add_body_amqp_data(amqp_message, binary_data) != RESULT_OK)
            {
                LogError("Failed setting the body of the AMQP message.");
            }
            else
            {
                if (addPropertiesTouAMQPMessage(message->messageHandle, amqp_message) != 0)
                {
                    /* Codes_SRS_IOTHUBTRANSPORTAMQP_01_014: [If any of the APIs fails while building the property map and setting it on the uAMQP message, IoTHubTransportAMQP_DoWork shall notify the failure by invoking the upper layer message send callback with IOTHUB_CLIENT_CONFIRMATION_ERROR.] */
                    is_message_error = true;
                }
                else
                {
                    // Codes_SRS_IOTHUBTRANSPORTAMQP_09_097: [IoTHubTransportAMQP_DoWork shall pass the encoded AMQP message to AMQP for sending (along with on_message_send_complete callback) using messagesender_send()] 
                    if (messagesender_send(transport_state->message_sender, amqp_message, on_message_send_complete, message) != RESULT_OK)
                    {
                        LogError("Failed sending the AMQP message.");
                    }
                    else
                    {
                        result = RESULT_OK;
                    }
                }
            }
        }

        if (amqp_message != NULL)
        {
            // It can be destroyed because AMQP keeps a clone of the message.
            message_destroy(amqp_message);
        }

        if (result != RESULT_OK)
        {
            // Codes_SRS_IOTHUBTRANSPORTAMQP_09_088: [If IoTHubMessage_GetByteArray() fails, IoTHubTransportAMQP_DoWork shall remove the event from the in-progress list and invoke the upper layer callback reporting the error] 
            // Codes_SRS_IOTHUBTRANSPORTAMQP_09_091: [If IoTHubMessage_GetString() fails, IoTHubTransportAMQP_DoWork shall remove the event from the in-progress list and invoke the upper layer callback reporting the error] 
            if (is_message_error)
            {
                on_message_send_complete(message, MESSAGE_SEND_ERROR);
            }
            else
            {
                // Codes_SRS_IOTHUBTRANSPORTAMQP_09_111: [If message_create() fails, IoTHubTransportAMQP_DoWork notify the failure, roll back the event to waitToSent list and return]
                // Codes_SRS_IOTHUBTRANSPORTAMQP_09_112: [If message_add_body_amqp_data() fails, IoTHubTransportAMQP_DoWork notify the failure, roll back the event to waitToSent list and return]
                // Codes_SRS_IOTHUBTRANSPORTAMQP_09_113: [If messagesender_send() fails, IoTHubTransportAMQP_DoWork notify the failure, roll back the event to waitToSent list and return]
                rollEventBackToWaitList(message, transport_state);
                break;
            }
        }
    }

    return result;
}

static bool isSasTokenRefreshRequired(AMQP_TRANSPORT_INSTANCE* transport_state)
{
    if (transport_state->credential.credentialType == DEVICE_SAS_TOKEN)
    {
        return false;
    }
    else
    {
    return ((getSecondsSinceEpoch() - transport_state->current_sas_token_create_time) >= (transport_state->sas_token_refresh_time / 1000)) ? true : false;
}
}

static void prepareForConnectionRetry(AMQP_TRANSPORT_INSTANCE* transport_state)
{
    destroyMessageReceiver(transport_state);
    destroyEventSender(transport_state);
    destroyConnection(transport_state);
    transport_state->connection_state = AMQP_MANAGEMENT_STATE_IDLE;
    rollEventsBackToWaitList(transport_state);
}


static void credential_destroy(AMQP_TRANSPORT_INSTANCE* transport_state)
{
    switch (transport_state->credential.credentialType)
    {
        default:
        {
            LogError("internal error: unexpected enum value transport_state->credential.credentialType=%d", transport_state->credential.credentialType);
            break;
        }
        case (CREDENTIAL_NOT_BUILD):
        {
            /*nothing to do*/
            break;
        }
        case(X509):
        {
            /*nothing to do here, x509certificate and x509privatekey are both NULL*/
            break;
        }
        case(DEVICE_KEY):
        {
            STRING_delete(transport_state->credential.credential.deviceKey);
            break;
        }
        case(DEVICE_SAS_TOKEN):
        {
            STRING_delete(transport_state->credential.credential.deviceSasToken);
            break;
        }
    }
}

// API functions

static TRANSPORT_LL_HANDLE IoTHubTransportAMQP_Create(const IOTHUBTRANSPORT_CONFIG* config)
{
    AMQP_TRANSPORT_INSTANCE* transport_state = NULL;
    size_t deviceIdLength;

    // Codes_SRS_IOTHUBTRANSPORTAMQP_09_005: [If parameter config (or its fields) is NULL then IoTHubTransportAMQP_Create shall fail and return NULL.] 
    if (config == NULL || config->upperConfig == NULL || config->waitingToSend == NULL)
    {
        LogError("IoTHub AMQP client transport null configuration parameter.");
    }
    // Codes_SRS_IOTHUBTRANSPORTAMQP_09_006: [IoTHubTransportAMQP_Create shall fail and return NULL if any fields of the config structure are NULL.]
    // Codes_SRS_IOTHUBTRANSPORTAMQP_03_001: [IoTHubTransportAMQP_Create shall fail and return NULL if both deviceKey & deviceSasToken fields are NOT NULL.]
    else if (config->upperConfig->protocol == NULL)
    {
        LogError("Invalid configuration (NULL protocol detected)");
    }
    else if (config->upperConfig->deviceId == NULL)
    {
        LogError("Invalid configuration (NULL deviceId detected)");
    }
    else if (config->upperConfig->deviceKey != NULL && config->upperConfig->deviceSasToken != NULL)
    {
        LogError("Invalid configuration (Both deviceKey and deviceSasToken are defined)");
    }
    else if (config->upperConfig->iotHubName == NULL)
    {
        LogError("Invalid configuration (NULL iotHubName detected)");
    }
    else if (config->upperConfig->iotHubSuffix == NULL)
    {
        LogError("Invalid configuration (NULL iotHubSuffix detected)");
    }
    else if (!config->waitingToSend)
    {
        LogError("Invalid configuration (NULL waitingToSend list detected)");
    }
    // Codes_SRS_IOTHUBTRANSPORTAMQP_09_008: [IoTHubTransportAMQP_Create shall fail and return NULL if any config field of type string is zero length.] 
    else if ((deviceIdLength = strlen(config->upperConfig->deviceId)) == 0 ||
        (strlen(config->upperConfig->iotHubName) == 0) ||
        (strlen(config->upperConfig->iotHubSuffix) == 0))
    {
        LogError("Zero-length config parameter (deviceId, iotHubName or iotHubSuffix)");
    }
    else if ((config->upperConfig->deviceKey != NULL) && (strlen(config->upperConfig->deviceKey) == 0))
    {
        LogError("Zero-length config parameter (deviceKey)");
    }
    else if ((config->upperConfig->deviceSasToken != NULL) && (strlen(config->upperConfig->deviceSasToken) == 0))
    {
        LogError("Zero-length config parameter (deviceSasToken)");
    }
    // Codes_SRS_IOTHUBTRANSPORTAMQP_09_007: [IoTHubTransportAMQP_Create shall fail and return NULL if the deviceId length is greater than 128.]
    else if (deviceIdLength > 128U)
    {
        LogError("deviceId is too long");
    }
    // Codes_SRS_IOTHUBTRANSPORTAMQP_09_134: [IoTHubTransportAMQP_Create shall fail and return NULL if the combined length of config->iotHubName and config->iotHubSuffix exceeds 254 bytes (RFC1035)]
    else if ((strlen(config->upperConfig->iotHubName) + strlen(config->upperConfig->iotHubSuffix)) > (RFC1035_MAX_FQDN_LENGTH - 1))
    {
        LogError("The lengths of iotHubName and iotHubSuffix together exceed the maximum FQDN length allowed (RFC 1035)");
    }
    else
    {
        // Codes_SRS_IOTHUBTRANSPORTAMQP_09_009: [IoTHubTransportAMQP_Create shall fail and return NULL if memory allocation of the transport's internal state structure fails.]
        transport_state = (AMQP_TRANSPORT_INSTANCE*)malloc(sizeof(AMQP_TRANSPORT_INSTANCE));

        if (transport_state == NULL)
        {
            LogError("Could not allocate AMQP transport state");
        }
        else
        {
            bool cleanup_required = false;

            transport_state->iotHubHostFqdn = NULL;
            transport_state->iotHubPort = DEFAULT_IOTHUB_AMQP_PORT;
            transport_state->devicesPath = NULL;
            transport_state->messageReceiveAddress = NULL;
            transport_state->sasTokenKeyName = NULL;
            transport_state->targetAddress = NULL;
            transport_state->waitingToSend = config->waitingToSend;

            transport_state->cbs = NULL;
            transport_state->cbs_state = CBS_STATE_IDLE;
            transport_state->current_sas_token_create_time = 0;
            transport_state->connection = NULL;
            transport_state->connection_state = AMQP_MANAGEMENT_STATE_IDLE;
            transport_state->connection_establish_time = 0;
            transport_state->iothub_client_handle = NULL;
            transport_state->receive_messages = false;
            transport_state->message_receiver = NULL;
            transport_state->message_sender = NULL;
            transport_state->receiver_link = NULL;
            transport_state->sasl_io = NULL;
            transport_state->sasl_mechanism = NULL;
            transport_state->sender_link = NULL;
            transport_state->session = NULL;
            transport_state->tls_io = NULL;
            transport_state->tls_io_transport_provider = getTLSIOTransport;
            transport_state->isRegistered = false;
            transport_state->is_trace_on = false;

            transport_state->waitingToSend = config->waitingToSend;
            DList_InitializeListHead(&transport_state->inProgress);

            transport_state->credential.credentialType = CREDENTIAL_NOT_BUILD;

            // Codes_SRS_IOTHUBTRANSPORTAMQP_09_010: [IoTHubTransportAMQP_Create shall create an immutable string, referred to as iotHubHostFqdn, from the following pieces: config->iotHubName + "." + config->iotHubSuffix.] 
            if ((transport_state->iotHubHostFqdn = concat3Params(config->upperConfig->iotHubName, ".", config->upperConfig->iotHubSuffix)) == NULL)
            {
                LogError("Failed to set transport_state->iotHubHostFqdn.");
                cleanup_required = true;
            }
            // Codes_SRS_IOTHUBTRANSPORTAMQP_09_012: [IoTHubTransportAMQP_Create shall create an immutable string, referred to as devicesPath, from the following parts: host_fqdn + "/devices/" + deviceId.] 
            else if ((transport_state->devicesPath = concat3Params(STRING_c_str(transport_state->iotHubHostFqdn), "/devices/", config->upperConfig->deviceId)) == NULL)
            {
                // Codes_SRS_IOTHUBTRANSPORTAMQP_09_013: [If creating devicesPath fails for any reason then IoTHubTransportAMQP_Create shall fail and return NULL.] 
                LogError("Failed to allocate transport_state->devicesPath.");
                cleanup_required = true;
            }
            // Codes_SRS_IOTHUBTRANSPORTAMQP_09_014: [IoTHubTransportAMQP_Create shall create an immutable string, referred to as targetAddress, from the following parts: "amqps://" + devicesPath + "/messages/events".]
            else if ((transport_state->targetAddress = concat3Params("amqps://", STRING_c_str(transport_state->devicesPath), "/messages/events")) == NULL)
            {
                // Codes_SRS_IOTHUBTRANSPORTAMQP_09_015: [If creating the targetAddress fails for any reason then IoTHubTransportAMQP_Create shall fail and return NULL.] 
                LogError("Failed to allocate transport_state->targetAddress.");
                cleanup_required = true;
            }
            // Codes_SRS_IOTHUBTRANSPORTAMQP_09_053: [IoTHubTransportAMQP_Create shall define the source address for receiving messages as "amqps://" + devicesPath + "/messages/devicebound", stored in the transport handle as messageReceiveAddress]
            else if ((transport_state->messageReceiveAddress = concat3Params("amqps://", STRING_c_str(transport_state->devicesPath), "/messages/devicebound")) == NULL)
            {
                // Codes_SRS_IOTHUBTRANSPORTAMQP_09_054: [If creating the messageReceiveAddress fails for any reason then IoTHubTransportAMQP_Create shall fail and return NULL.]
                LogError("Failed to allocate transport_state->messageReceiveAddress.");
                cleanup_required = true;
            }
            // Codes_SRS_IOTHUBTRANSPORTAMQP_09_016: [IoTHubTransportAMQP_Create shall initialize handle->sasTokenKeyName with a zero-length STRING_HANDLE instance.] 
            else if ((transport_state->sasTokenKeyName = STRING_new()) == NULL)
            {
                // Codes_SRS_IOTHUBTRANSPORTAMQP_09_017: [If IoTHubTransportAMQP_Create fails to initialize handle->sasTokenKeyName with a zero-length STRING the function shall fail and return NULL.] 
                LogError("Failed to allocate transport_state->sasTokenKeyName.");
                cleanup_required = true;
            }
            else 
            {

                if (config->upperConfig->deviceSasToken != NULL)
                {
                    if (config->upperConfig->deviceKey != NULL)
                    {
                        LogError("invalid configuration, both deviceSasToken AND deviceKey specified");
                        cleanup_required = true;
                    }
                    else
                    {
                        /*only SAS token specified*/
                        transport_state->credential.credential.deviceSasToken = STRING_construct(config->upperConfig->deviceSasToken);
                        if (transport_state->credential.credential.deviceSasToken == NULL)
                        {
                            LogError("unable to STRING_construct for deviceSasToken");
                            cleanup_required = true;
                        }
                        else
                        {
                            transport_state->credential.credentialType = DEVICE_SAS_TOKEN;
                        }
                    }
                }
                else
                {
                    /*when deviceSasToken == NULL*/
                    if (config->upperConfig->deviceKey != NULL)
                    {
                        /*it is device key*/
                        
                        transport_state->credential.credential.deviceKey = STRING_construct(config->upperConfig->deviceKey);
                        if (transport_state->credential.credential.deviceKey == NULL)
                        {
                            LogError("unable to STRING_construct for a deviceKey");
                            cleanup_required = true;
                        }
                        else
                        {
                            transport_state->credential.credentialType = DEVICE_KEY;
                        }
                    }
                    else
                    {
                        /*Codes_SRS_IOTHUBTRANSPORTAMQP_02_003: [ IoTHubTransportAMQP_Register shall assume a x509 authentication mechanism when both deviceKey and deviceSasToken are NULL. ]*/
                        /*when both SAS token AND devicekey are NULL*/
                        transport_state->credential.credentialType = X509;
                        transport_state->credential.credential.x509credential.x509certificate = NULL;
                        transport_state->credential.credential.x509credential.x509privatekey = NULL;
                    }
                }

                if(transport_state->credential.credentialType != CREDENTIAL_NOT_BUILD )
                {
                    // Codes_SRS_IOTHUBTRANSPORTAMQP_09_020: [IoTHubTransportAMQP_Create shall set parameter transport_state->sas_token_lifetime with the default value of 3600000 (milliseconds).]
                    transport_state->sas_token_lifetime = DEFAULT_SAS_TOKEN_LIFETIME_MS;

                    // Codes_SRS_IOTHUBTRANSPORTAMQP_09_128: [IoTHubTransportAMQP_Create shall set parameter transport_state->sas_token_refresh_time with the default value of sas_token_lifetime/2 (milliseconds).] 
                    transport_state->sas_token_refresh_time = transport_state->sas_token_lifetime / 2;

                    // Codes_SRS_IOTHUBTRANSPORTAMQP_09_129 : [IoTHubTransportAMQP_Create shall set parameter transport_state->cbs_request_timeout with the default value of 30000 (milliseconds).]
                    transport_state->cbs_request_timeout = DEFAULT_CBS_REQUEST_TIMEOUT_MS;
                }
            }

            if (cleanup_required)
            {
                credential_destroy(transport_state);
                if (transport_state->sasTokenKeyName != NULL)
                    STRING_delete(transport_state->sasTokenKeyName);
                if (transport_state->targetAddress != NULL)
                    STRING_delete(transport_state->targetAddress);
                if (transport_state->messageReceiveAddress != NULL)
                    STRING_delete(transport_state->messageReceiveAddress);
                if (transport_state->devicesPath != NULL)
                    STRING_delete(transport_state->devicesPath);
                if (transport_state->iotHubHostFqdn != NULL)
                    STRING_delete(transport_state->iotHubHostFqdn);

                free(transport_state);
                transport_state = NULL;
            }
        }
    }

    // Codes_SRS_IOTHUBTRANSPORTAMQP_09_023: [If IoTHubTransportAMQP_Create succeeds it shall return a non-NULL pointer to the structure that represents the transport.] 
    return transport_state;
}

static void IoTHubTransportAMQP_Destroy(TRANSPORT_LL_HANDLE handle)
{
    if (handle != NULL)
    {
        AMQP_TRANSPORT_INSTANCE* transport_state = (AMQP_TRANSPORT_INSTANCE*)handle;

        // Codes_SRS_IOTHUBTRANSPORTAMQP_09_024: [IoTHubTransportAMQP_Destroy shall destroy the AMQP message_sender.]
        // Codes_SRS_IOTHUBTRANSPORTAMQP_09_029 : [IoTHubTransportAMQP_Destroy shall destroy the AMQP link.]
        destroyEventSender(transport_state);

        // Codes_SRS_IOTHUBTRANSPORTAMQP_09_025: [IoTHubTransportAMQP_Destroy shall destroy the AMQP message_receiver.] 
        // Codes_SRS_IOTHUBTRANSPORTAMQP_09_029 : [IoTHubTransportAMQP_Destroy shall destroy the AMQP link.]
        destroyMessageReceiver(transport_state);

        // Codes_SRS_IOTHUBTRANSPORTAMQP_09_027 : [IoTHubTransportAMQP_Destroy shall destroy the AMQP cbs instance]
        // Codes_SRS_IOTHUBTRANSPORTAMQP_09_030 : [IoTHubTransportAMQP_Destroy shall destroy the AMQP session.]
        // Codes_SRS_IOTHUBTRANSPORTAMQP_09_031 : [IoTHubTransportAMQP_Destroy shall destroy the AMQP connection.]
        // Codes_SRS_IOTHUBTRANSPORTAMQP_09_032 : [IoTHubTransportAMQP_Destroy shall destroy the AMQP SASL I / O transport.]
        // Codes_SRS_IOTHUBTRANSPORTAMQP_09_033 : [IoTHubTransportAMQP_Destroy shall destroy the AMQP SASL mechanism.]
        destroyConnection(transport_state);

        // Codes_SRS_IOTHUBTRANSPORTAMQP_09_035 : [IoTHubTransportAMQP_Destroy shall delete its internally - set parameters(deviceKey, targetAddress, devicesPath, sasTokenKeyName).]
        STRING_delete(transport_state->targetAddress);
        STRING_delete(transport_state->messageReceiveAddress);
        STRING_delete(transport_state->sasTokenKeyName);
        credential_destroy(transport_state);
        STRING_delete(transport_state->devicesPath);
        STRING_delete(transport_state->iotHubHostFqdn);

        // Codes_SRS_IOTHUBTRANSPORTAMQP_09_036 : [IoTHubTransportAMQP_Destroy shall return the remaining items in inProgress to waitingToSend list.]
        rollEventsBackToWaitList(transport_state);

        // Codes_SRS_IOTHUBTRANSPORTAMQP_09_150: [IoTHubTransportAMQP_Destroy shall destroy the transport instance]
        free(transport_state);
    }
}

static void IoTHubTransportAMQP_DoWork(TRANSPORT_LL_HANDLE handle, IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle)
{
    // Codes_SRS_IOTHUBTRANSPORTAMQP_09_051: [IoTHubTransportAMQP_DoWork shall fail and return immediately if the transport handle parameter is NULL] 
    if (handle == NULL)
    {
        LogError("IoTHubClient DoWork failed: transport handle parameter is NULL.");
    }
    // Codes_[IoTHubTransportAMQP_DoWork shall fail and return immediately if the client handle parameter is NULL] 
    else if (iotHubClientHandle == NULL)
    {
        LogError("IoTHubClient DoWork failed: client handle parameter is NULL.");
    }
    else
    {
        bool trigger_connection_retry = false;
        AMQP_TRANSPORT_INSTANCE* transport_state = (AMQP_TRANSPORT_INSTANCE*)handle;

        // Codes_SRS_IOTHUBTRANSPORTAMQP_09_147: [IoTHubTransportAMQP_DoWork shall save a reference to the client handle in transport_state->iothub_client_handle]
        transport_state->iothub_client_handle = iotHubClientHandle;

        if (transport_state->connection != NULL &&
            transport_state->connection_state == AMQP_MANAGEMENT_STATE_ERROR)
        {
            LogError("An error occured on AMQP connection. The connection will be restablished.");
            trigger_connection_retry = true;
        }
        // Codes_SRS_IOTHUBTRANSPORTAMQP_09_055: [If the transport handle has a NULL connection, IoTHubTransportAMQP_DoWork shall instantiate and initialize the AMQP components and establish the connection] 
        else if (transport_state->connection == NULL &&
            establishConnection(transport_state) != RESULT_OK)
        {
            LogError("AMQP transport failed to establish connection with service.");
            trigger_connection_retry = true;
        }
        // Codes_SRS_IOTHUBTRANSPORTAMQP_09_081: [IoTHubTransportAMQP_DoWork shall put a new SAS token if the one has not been out already, or if the previous one failed to be put due to timeout of cbs_put_token().]
        // Codes_SRS_IOTHUBTRANSPORTAMQP_09_082: [IoTHubTransportAMQP_DoWork shall refresh the SAS token if the current token has been used for more than 'sas_token_refresh_time' milliseconds]
        else if ((transport_state->cbs_state == CBS_STATE_IDLE || isSasTokenRefreshRequired(transport_state)) &&
            startAuthentication(transport_state) != RESULT_OK)
        {
            // Codes_SRS_IOTHUBTRANSPORTAMQP_09_146: [If the SAS token fails to be sent to CBS (cbs_put_token), IoTHubTransportAMQP_DoWork shall fail and exit immediately]
            LogError("Failed authenticating AMQP connection within CBS.");
            trigger_connection_retry = true;
        }
        // Codes_SRS_IOTHUBTRANSPORTAMQP_09_084: [IoTHubTransportAMQP_DoWork shall wait for 'cbs_request_timeout' milliseconds for the cbs_put_token() to complete before failing due to timeout]
        else if (transport_state->cbs_state == CBS_STATE_AUTH_IN_PROGRESS &&
            verifyAuthenticationTimeout(transport_state) == RESULT_TIMEOUT)
        {
            LogError("AMQP transport authentication timed out.");
            trigger_connection_retry = true;
        }
        else if (transport_state->cbs_state == CBS_STATE_AUTHENTICATED)
        {
            // Codes_SRS_IOTHUBTRANSPORTAMQP_09_121: [IoTHubTransportAMQP_DoWork shall create an AMQP message_receiver if transport_state->message_receive is NULL and transport_state->receive_messages is true] 
            if (transport_state->receive_messages == true &&
                transport_state->message_receiver == NULL &&
                createMessageReceiver(transport_state, iotHubClientHandle) != RESULT_OK)
            {
                LogError("Failed creating AMQP transport message receiver.");
                trigger_connection_retry = true;
            }
            // Codes_SRS_IOTHUBTRANSPORTAMQP_09_122: [IoTHubTransportAMQP_DoWork shall destroy the transport_state->message_receiver (and set it to NULL) if it exists and transport_state->receive_messages is false] 
            else if (transport_state->receive_messages == false &&
                transport_state->message_receiver != NULL &&
                destroyMessageReceiver(transport_state) != RESULT_OK)
            {
                LogError("Failed destroying AMQP transport message receiver.");
            }

            if (transport_state->message_sender == NULL &&
                createEventSender(transport_state) != RESULT_OK)
            {
                LogError("Failed creating AMQP transport event sender.");
                trigger_connection_retry = true;
            }
            else if (sendPendingEvents(transport_state) != RESULT_OK)
            {
                LogError("AMQP transport failed sending events.");
            }
        }

        if (trigger_connection_retry)
        {
            prepareForConnectionRetry(transport_state);
        }
        else
        {
            // Codes_SRS_IOTHUBTRANSPORTAMQP_09_103: [IoTHubTransportAMQP_DoWork shall invoke connection_dowork() on AMQP for triggering sending and receiving messages] 
            connection_dowork(transport_state->connection);
        }
    }
}

static int IoTHubTransportAMQP_Subscribe(TRANSPORT_LL_HANDLE handle)
{
    int result;

    // Codes_SRS_IOTHUBTRANSPORTAMQP_09_037: [IoTHubTransportAMQP_Subscribe shall fail if the transport handle parameter received is NULL.] 
    if (handle == NULL)
    {
        LogError("Invalid handle to IoTHubClient AMQP transport.");
        result = __LINE__;
    }
    else
    {
        // Codes_SRS_IOTHUBTRANSPORTAMQP_09_038: [IoTHubTransportAMQP_Subscribe shall set transport_handle->receive_messages to true and return success code.]
        AMQP_TRANSPORT_INSTANCE* transport_state = (AMQP_TRANSPORT_INSTANCE*)handle;
        transport_state->receive_messages = true;
        result = 0;
    }

    return result;
}

static void IoTHubTransportAMQP_Unsubscribe(TRANSPORT_LL_HANDLE handle)
{
    // Codes_SRS_IOTHUBTRANSPORTAMQP_09_039: [IoTHubTransportAMQP_Unsubscribe shall fail if the transport handle parameter received is NULL.] 
    if (handle == NULL)
    {
        LogError("Invalid handle to IoTHubClient AMQP transport.");
    }
    else
    {
        // Codes_SRS_IOTHUBTRANSPORTAMQP_09_040: [IoTHubTransportAMQP_Unsubscribe shall set transport_handle->receive_messages to false and return success code.]
        AMQP_TRANSPORT_INSTANCE* transport_state = (AMQP_TRANSPORT_INSTANCE*)handle;
        transport_state->receive_messages = false;
    }
}

static IOTHUB_CLIENT_RESULT IoTHubTransportAMQP_GetSendStatus(IOTHUB_DEVICE_HANDLE handle, IOTHUB_CLIENT_STATUS *iotHubClientStatus)
{
    IOTHUB_CLIENT_RESULT result;

    // Codes_SRS_IOTHUBTRANSPORTAMQP_09_041: [IoTHubTransportAMQP_GetSendStatus shall return IOTHUB_CLIENT_INVALID_ARG if called with NULL parameter.]
    if (handle == NULL)
    {
        result = IOTHUB_CLIENT_INVALID_ARG;
        LogError("Invalid handle to IoTHubClient AMQP transport instance.");
    }
    else if (iotHubClientStatus == NULL)
    {
        result = IOTHUB_CLIENT_INVALID_ARG;
        LogError("Invalid pointer to output parameter IOTHUB_CLIENT_STATUS.");
    }
    else
    {
        AMQP_TRANSPORT_INSTANCE* transport_state = (AMQP_TRANSPORT_INSTANCE*)handle;

        // Codes_SRS_IOTHUBTRANSPORTAMQP_09_043: [IoTHubTransportAMQP_GetSendStatus shall return IOTHUB_CLIENT_OK and status IOTHUB_CLIENT_SEND_STATUS_BUSY if there are currently event items to be sent or being sent.]
        if (!DList_IsListEmpty(transport_state->waitingToSend) || !DList_IsListEmpty(&(transport_state->inProgress)))
        {
            *iotHubClientStatus = IOTHUB_CLIENT_SEND_STATUS_BUSY;
        }
        // Codes_SRS_IOTHUBTRANSPORTAMQP_09_042: [IoTHubTransportAMQP_GetSendStatus shall return IOTHUB_CLIENT_OK and status IOTHUB_CLIENT_SEND_STATUS_IDLE if there are currently no event items to be sent or being sent.] 
        else
        {
            *iotHubClientStatus = IOTHUB_CLIENT_SEND_STATUS_IDLE;
        }

        result = IOTHUB_CLIENT_OK;
    }

    return result;
}

static IOTHUB_CLIENT_RESULT IoTHubTransportAMQP_SetOption(TRANSPORT_LL_HANDLE handle, const char* option, const void* value)
{
    IOTHUB_CLIENT_RESULT result;

    // Codes_SRS_IOTHUBTRANSPORTAMQP_09_044: [If handle parameter is NULL then IoTHubTransportAMQP_SetOption shall return IOTHUB_CLIENT_INVALID_ARG.]
    // Codes_SRS_IOTHUBTRANSPORTAMQP_09_045: [If parameter optionName is NULL then IoTHubTransportAMQP_SetOption shall return IOTHUB_CLIENT_INVALID_ARG.] 
    // Codes_SRS_IOTHUBTRANSPORTAMQP_09_046: [If parameter value is NULL then IoTHubTransportAMQP_SetOption shall return IOTHUB_CLIENT_INVALID_ARG.]
    if (
        (handle == NULL) ||
        (option == NULL) ||
        (value == NULL)
        )
    {
        result = IOTHUB_CLIENT_INVALID_ARG;
        LogError("Invalid parameter (NULL) passed to AMQP transport SetOption()");
    }
    else
    {
        AMQP_TRANSPORT_INSTANCE* transport_state = (AMQP_TRANSPORT_INSTANCE*)handle;

        // Codes_SRS_IOTHUBTRANSPORTAMQP_09_048: [IotHubTransportAMQP_SetOption shall save and apply the value if the option name is "sas_token_lifetime", returning IOTHUB_CLIENT_OK] 
        if (strcmp("sas_token_lifetime", option) == 0)
        {
            transport_state->sas_token_lifetime = *((size_t*)value);
            result = IOTHUB_CLIENT_OK;
        }
        // Codes_SRS_IOTHUBTRANSPORTAMQP_09_049: [IotHubTransportAMQP_SetOption shall save and apply the value if the option name is "sas_token_refresh_time", returning IOTHUB_CLIENT_OK] 
        else if (strcmp("sas_token_refresh_time", option) == 0)
        {
            transport_state->sas_token_refresh_time = *((size_t*)value);
            result = IOTHUB_CLIENT_OK;
        }
        // Codes_SRS_IOTHUBTRANSPORTAMQP_09_148: [IotHubTransportAMQP_SetOption shall save and apply the value if the option name is "cbs_request_timeout", returning IOTHUB_CLIENT_OK] 
        else if (strcmp("cbs_request_timeout", option) == 0)
        {
            transport_state->cbs_request_timeout = *((size_t*)value);
            result = IOTHUB_CLIENT_OK;
        }
        else if (strcmp("logtrace", option) == 0)
        {
            transport_state->is_trace_on = *((bool*)value);
            if (transport_state->connection != NULL)
            {
                connection_set_trace(transport_state->connection, transport_state->is_trace_on);
            }
            result = IOTHUB_CLIENT_OK;
        }
        // Codes_SRS_IOTHUBTRANSPORTAMQP_09_047: [If the option name does not match one of the options handled by this module, then IoTHubTransportAMQP_SetOption shall get  the handle to the XIO and invoke the xio_setoption passing down the option name and value parameters.] 
        else
        {
            if (transport_state->tls_io == NULL &&
                (transport_state->tls_io = transport_state->tls_io_transport_provider(STRING_c_str(transport_state->iotHubHostFqdn), transport_state->iotHubPort)) == NULL)
            {
                result = IOTHUB_CLIENT_ERROR;
                LogError("Failed to obtain a TLS I/O transport layer.");
            }
            else
            {
                /* Codes_SRS_IOTHUBTRANSPORTAMQP_03_001: [If xio_setoption fails, IoTHubTransportAMQP_SetOption shall return IOTHUB_CLIENT_ERROR.] */
                if (xio_setoption(transport_state->tls_io, option, value) == 0)
                {
                    result = IOTHUB_CLIENT_OK;
                }
                else
                {
                    result = IOTHUB_CLIENT_ERROR;
                    LogError("Invalid option (%s) passed to uAMQP transport SetOption()", option);
                }
            }
        }
    }

    return result;
}

static IOTHUB_DEVICE_HANDLE IoTHubTransportAMQP_Register(TRANSPORT_LL_HANDLE handle, const IOTHUB_DEVICE_CONFIG* device, IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle, PDLIST_ENTRY waitingToSend)
{
    IOTHUB_DEVICE_HANDLE result;
    // Codes_SRS_IOTHUBTRANSPORTAMQP_17_001: [IoTHubTransportAMQP_Register shall return NULL if device, or waitingToSend are NULL.] 
    // Codes_SRS_IOTHUBTRANSPORTAMQP_17_005: [IoTHubTransportAMQP_Register shall return NULL if the TRANSPORT_LL_HANDLE is NULL.]
    if ((handle == NULL) || (device == NULL) || (waitingToSend == NULL))
    {
        LogError("invalid parameter TRANSPORT_LL_HANDLE handle=%p, const IOTHUB_DEVICE_CONFIG* device=%p, IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle=%p, PDLIST_ENTRY waitingToSend=%p",
            handle, device, iotHubClientHandle, waitingToSend);
        result = NULL;
    }
    else
    {
        AMQP_TRANSPORT_INSTANCE* transport_state = (AMQP_TRANSPORT_INSTANCE*)handle;

        // Codes_SRS_IOTHUBTRANSPORTAMQP_03_002: [IoTHubTransportAMQP_Register shall return NULL if deviceId is NULL.**]
        if (device->deviceId == NULL)
        {
            LogError("invalid parameter device->deviceId was NULL");
            result = NULL;
        }
        // Codes_SRS_IOTHUBTRANSPORTAMQP_03_003: [IoTHubTransportAMQP_Register shall return NULL if both deviceKey and deviceSasToken are not NULL.]
        else if ( (device->deviceSasToken != NULL) && (device->deviceKey != NULL) )
        {
            LogError("invalid parameter both device->deviceSasToken and device->deviceKey were NULL");
            result = NULL;
        }
        else
        {
            STRING_HANDLE devicesPath = concat3Params(STRING_c_str(transport_state->iotHubHostFqdn), "/devices/", device->deviceId);
            if (devicesPath == NULL)
            {
                LogError("Could not create devicesPath");
                result = NULL;
            }
            else
            {
                // Codes_SRS_IOTHUBTRANSPORTAMQP_17_002: [IoTHubTransportAMQP_Register shall return NULL if deviceId or deviceKey do not match the deviceId and deviceKey passed in during IoTHubTransportAMQP_Create.] 
                if (strcmp(STRING_c_str(transport_state->devicesPath), STRING_c_str(devicesPath)) != 0)
                {
                    LogError("Attemping to add new device to AMQP transport, not allowed.");
                    result = NULL;
                }
                else if ((transport_state->credential.credentialType == DEVICE_KEY) && strcmp(STRING_c_str(transport_state->credential.credential.deviceKey), device->deviceKey) != 0)
                {
                    LogError("Attemping to add new device to AMQP transport, not allowed.");
                    result = NULL;
                }
                else
                {
                    if (transport_state->isRegistered == true)
                    {
                        LogError("Transport already has device registered by id: [%s]", device->deviceId);
                        result = NULL;
                    }
                    else
                    {
                        transport_state->isRegistered = true;
                        // Codes_SRS_IOTHUBTRANSPORTAMQP_17_003: [IoTHubTransportAMQP_Register shall return the TRANSPORT_LL_HANDLE as the IOTHUB_DEVICE_HANDLE.] 
                        result = (IOTHUB_DEVICE_HANDLE)handle;
                    }
                }
                STRING_delete(devicesPath);
            }
        }
    }

    return result;
}

// Codes_SRS_IOTHUBTRANSPORTAMQP_17_004: [IoTHubTransportAMQP_Unregister shall return.] 
static void IoTHubTransportAMQP_Unregister(IOTHUB_DEVICE_HANDLE deviceHandle)
{
    if (deviceHandle != NULL)
    {
        AMQP_TRANSPORT_INSTANCE* transport_state = (AMQP_TRANSPORT_INSTANCE*)deviceHandle;

        transport_state->isRegistered = false;
    }
}

static STRING_HANDLE IoTHubTransportAMQP_GetHostname(TRANSPORT_LL_HANDLE handle)
{
    STRING_HANDLE result;
    /*Codes_SRS_IOTHUBTRANSPORTAMQP_02_001: [ If parameter handle is NULL then IoTHubTransportAMQP_GetHostname shall return NULL. ]*/
    if (handle == NULL)
    {
        result = NULL;
    }
    else
    {
        /*Codes_SRS_IOTHUBTRANSPORTAMQP_02_002: [ Otherwise IoTHubTransportAMQP_GetHostname shall return a STRING_HANDLE for the hostname. ]*/
        result = ((AMQP_TRANSPORT_INSTANCE*)(handle))->iotHubHostFqdn;
    }
    return result;
}

static TRANSPORT_PROVIDER thisTransportProvider = {
    IoTHubTransportAMQP_GetHostname,
    IoTHubTransportAMQP_SetOption,
    IoTHubTransportAMQP_Create,
    IoTHubTransportAMQP_Destroy,
    IoTHubTransportAMQP_Register,
    IoTHubTransportAMQP_Unregister,
    IoTHubTransportAMQP_Subscribe,
    IoTHubTransportAMQP_Unsubscribe,
    IoTHubTransportAMQP_DoWork,
    IoTHubTransportAMQP_GetSendStatus
};

extern const TRANSPORT_PROVIDER* AMQP_Protocol(void)
{
    return &thisTransportProvider;
}
