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

#include "uamqp_messaging.h"
#include "iothub_client_ll.h"
#include "iothub_client_options.h"
#include "iothub_client_private.h"
#include "iothubtransportamqp.h"
#include "iothub_client_version.h"

#define INDEFINITE_TIME ((time_t)(-1))

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

/*the below structure contains fields that are only used with CBS (when authentication mechanisn is based on deviceKey or deviceSasToken)*/
/*these fields are mutually exclusive with the fields of the AMQP_TRANSPORT_STATE_X509 authentication*/
typedef struct AMQP_TRANSPORT_STATE_CBS_TAG
{
    // A component of the SAS token. Currently this must be an empty string.
    STRING_HANDLE sasTokenKeyName;
    size_t sas_token_lifetime;
    // Maximum period of time for the transport to wait before refreshing the SAS token it created previously, in milliseconds.
    size_t sas_token_refresh_time;
    // Maximum time the transport waits for  uAMQP cbs_put_token() to complete before marking it a failure, in milliseconds.
    size_t cbs_request_timeout;

    // AMQP SASL I/O transport created on top of the TLS I/O layer.
    XIO_HANDLE sasl_io;
    // AMQP SASL I/O mechanism to be used.
    SASL_MECHANISM_HANDLE sasl_mechanism;

    // Connection instance with the Azure IoT CBS.
    CBS_HANDLE cbs;
    // Current state of the CBS connection.
    CBS_STATE cbs_state;
    // Time when the current SAS token was created, in seconds since epoch.
    size_t current_sas_token_create_time;
}AMQP_TRANSPORT_STATE_CBS;

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

    // Internal parameter that identifies the current logical device within the service.
    STRING_HANDLE devicesPath;
    // How long a SAS token created by the transport is valid, in milliseconds.

    // Maximum time for the connection establishment/retry logic should wait for a connection to succeed, in milliseconds.
    size_t connection_timeout;
    // Saved reference to the IoTHub LL Client.
    IOTHUB_CLIENT_LL_HANDLE iothub_client_handle;

    // TSL I/O transport.
    XIO_HANDLE tls_io;
    // Pointer to the function that creates the TLS I/O (internal use only).
    TLS_IO_TRANSPORT_PROVIDER tls_io_transport_provider;

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

    // all things CBS (and only CBS)
    AMQP_TRANSPORT_STATE_CBS cbs;

    // Mark if device is registered in transport (only one device per transport).
    bool isRegistered;
    // Turns logging on and off
    bool is_trace_on;

    /*here are the options from the xio layer if any is saved*/
    OPTIONHANDLER_HANDLE xioOptions;
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

static int getSecondsSinceEpoch(size_t* seconds)
{
	int result;
	time_t current_time;
	
	if ((current_time = get_time(NULL)) == INDEFINITE_TIME)
	{
		LogError("Failed getting the current local time (get_time() failed)");
		result = __LINE__;
	}
	else
	{
		*seconds = (size_t)get_difftime(current_time, (time_t)0);
		
		result = RESULT_OK;
	}
	
	return result;
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
#ifdef NO_LOGGING
    UNUSED(status_code);
    UNUSED(status_description);
#endif

    AMQP_TRANSPORT_INSTANCE* transportState = (AMQP_TRANSPORT_INSTANCE*)context;

    if (operation_result == CBS_OPERATION_RESULT_OK)
    {
        transportState->cbs.cbs_state = CBS_STATE_AUTHENTICATED;
    }
    else
    {
        LogError("CBS reported status %u error: %s", status_code, status_description);
    }
}

static AMQP_VALUE on_message_received(const void* context, MESSAGE_HANDLE message)
{
    AMQP_VALUE result = NULL;
    int api_call_result;
    IOTHUB_MESSAGE_HANDLE iothub_message = NULL;
	// Codes_SRS_IOTHUBTRANSPORTAMQP_09_195: [The callback 'on_message_received' shall shall get a IOTHUB_MESSAGE_HANDLE instance out of the uamqp's MESSAGE_HANDLE instance by using IoTHubMessage_CreateFromUamqpMessage()]
	if ((api_call_result = IoTHubMessage_CreateFromUamqpMessage(message, &iothub_message)) != RESULT_OK)
	{
		LogError("Transport failed processing the message received (error = %d).", api_call_result);

		// Codes_SRS_IOTHUBTRANSPORTAMQP_09_196: [If IoTHubMessage_CreateFromUamqpMessage fails, the callback 'on_message_received' shall reject the incoming message by calling messaging_delivery_rejected() and return.]
		result = messaging_delivery_rejected("Rejected due to failure reading AMQP message", "Failed reading AMQP message");
	}
	else
	{
		IOTHUBMESSAGE_DISPOSITION_RESULT disposition_result;

		// Codes_SRS_IOTHUBTRANSPORTAMQP_09_104: [The callback 'on_message_received' shall invoke IoTHubClient_LL_MessageCallback() passing the client and the incoming message handles as parameters] 
		disposition_result = IoTHubClient_LL_MessageCallback((IOTHUB_CLIENT_LL_HANDLE)context, iothub_message);

		// Codes_SRS_IOTHUBTRANSPORTAMQP_09_197: [The callback 'on_message_received' shall destroy the IOTHUB_MESSAGE_HANDLE instance after invoking IoTHubClient_LL_MessageCallback().]
		IoTHubMessage_Destroy(iothub_message);

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

	return result;
}

static XIO_HANDLE getTLSIOTransport(const char* fqdn, int port)
{
    XIO_HANDLE result;
    TLSIO_CONFIG tls_io_config;
    tls_io_config.hostname = fqdn;
    tls_io_config.port = port;
    const IO_INTERFACE_DESCRIPTION* io_interface_description = platform_get_default_tlsio();
    result = xio_create(io_interface_description, &tls_io_config);
    if (result == NULL)
    {
        LogError("unable to xio_create");
        /*return as is*/
    }
    else
    {
        /*also return as is*/
    }
    return result;
}

static void destroyConnection(AMQP_TRANSPORT_INSTANCE* transport_state)
{
    if (transport_state->cbs.cbs != NULL)
    {
        cbs_destroy(transport_state->cbs.cbs);
        transport_state->cbs.cbs = NULL;
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

    if (transport_state->cbs.sasl_io != NULL)
    {
        xio_destroy(transport_state->cbs.sasl_io);
        transport_state->cbs.sasl_io = NULL;
    }

    if (transport_state->cbs.sasl_mechanism != NULL)
    {
        saslmechanism_destroy(transport_state->cbs.sasl_mechanism);
        transport_state->cbs.sasl_mechanism = NULL;
    }

    if (transport_state->tls_io != NULL)
    {
        /*before destroying, we shall save its options for later use*/
        transport_state->xioOptions = xio_retrieveoptions(transport_state->tls_io);
        if (transport_state->xioOptions == NULL)
        {
            LogError("unable to retrieve xio_retrieveoptions");
        }
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
    else
    {
        /*in the case when a tls_io_transport has been created, replay its options*/
        if (transport_state->xioOptions != NULL)
        {
            if (OptionHandler_FeedOptions(transport_state->xioOptions, transport_state->tls_io) != 0)
            {
                LogError("unable to replay options to TLS"); /*pessimistically hope TLS will fail, be recreated and options re-given*/
            }
            else
            {
                /*everything is fine, forget the saved options...*/
                OptionHandler_Destroy(transport_state->xioOptions);
                transport_state->xioOptions = NULL;
            }
        }

        switch (transport_state->credential.credentialType)
        {
            case (DEVICE_KEY):
            case (DEVICE_SAS_TOKEN):
            {
                // Codes_SRS_IOTHUBTRANSPORTAMQP_09_056: [IoTHubTransportAMQP_DoWork shall create the SASL mechanism using AMQP's saslmechanism_create() API] 
                if ((transport_state->cbs.sasl_mechanism = saslmechanism_create(saslmssbcbs_get_interface(), NULL)) == NULL)
                {
                    // Codes_SRS_IOTHUBTRANSPORTAMQP_09_057: [If saslmechanism_create() fails, IoTHubTransportAMQP_DoWork shall fail and return immediately]
                    result = RESULT_FAILURE;
                    LogError("Failed to create a SASL mechanism.");
                }
                else
                {
                    // Codes_SRS_IOTHUBTRANSPORTAMQP_09_060: [IoTHubTransportAMQP_DoWork shall create the SASL I / O layer using the xio_create() C Shared Utility API]
                    SASLCLIENTIO_CONFIG sasl_client_config;
                    sasl_client_config.sasl_mechanism = transport_state->cbs.sasl_mechanism;
                    sasl_client_config.underlying_io = transport_state->tls_io;
                    if ((transport_state->cbs.sasl_io = xio_create(saslclientio_get_interface_description(), &sasl_client_config)) == NULL)
                    {
                        // Codes_SRS_IOTHUBTRANSPORTAMQP_09_061: [If xio_create() fails creating the SASL I/O layer, IoTHubTransportAMQP_DoWork shall fail and return immediately] 
                        result = RESULT_FAILURE;
                        LogError("Failed to create a SASL I/O layer.");
                    }
                    // Codes_SRS_IOTHUBTRANSPORTAMQP_09_062: [IoTHubTransportAMQP_DoWork shall create the connection with the IoT service using connection_create2() AMQP API, passing the SASL I/O layer, IoT Hub FQDN and container ID as parameters (pass NULL for callbacks)] 
                    else if ((transport_state->connection = connection_create2(transport_state->cbs.sasl_io, STRING_c_str(transport_state->iotHubHostFqdn), DEFAULT_CONTAINER_ID, NULL, NULL, NULL, NULL, on_connection_io_error, (void*)transport_state)) == NULL)
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
                        if ((transport_state->cbs.cbs = cbs_create(transport_state->session, on_amqp_management_state_changed, NULL)) == NULL)
                        {
                            // Codes_SRS_IOTHUBTRANSPORTAMQP_09_067: [If cbs_create() fails, IoTHubTransportAMQP_DoWork shall fail and return immediately] 
                            result = RESULT_FAILURE;
                            LogError("Failed to create the CBS connection.");
                        }
                        // Codes_SRS_IOTHUBTRANSPORTAMQP_09_139: [IoTHubTransportAMQP_DoWork shall open the CBS connection using the cbs_open() AMQP API] 
                        else if (cbs_open(transport_state->cbs.cbs) != 0)
                        {
                            // Codes_SRS_IOTHUBTRANSPORTAMQP_09_140: [If cbs_open() fails, IoTHubTransportAMQP_DoWork shall fail and return immediately]
                            result = RESULT_FAILURE;
                            LogError("Failed to open the connection with CBS.");
                        }
						else if (getSecondsSinceEpoch(&transport_state->connection_establish_time) != RESULT_OK)
						{
							LogError("Failed setting the connection establish time.");
							result = RESULT_FAILURE;
						}
                        else
                        {
                            transport_state->cbs.cbs_state = CBS_STATE_IDLE;
                            connection_set_trace(transport_state->connection, transport_state->is_trace_on);
                            (void)xio_setoption(transport_state->cbs.sasl_io, OPTION_LOG_TRACE, &transport_state->is_trace_on);
                            result = RESULT_OK;
                        }
                    }
                }
                break;
            }
            case(X509):
            {
                /*Codes_SRS_IOTHUBTRANSPORTAMQP_02_006: [ IoTHubTransportAMQP_DoWork shall not establish a CBS connection. ]*/
                /*Codes_SRS_IOTHUBTRANSPORTAMQP_02_005: [ IoTHubTransportAMQP_DoWork shall create the connection with the IoT service using connection_create2() AMQP API, passing the TLS I/O layer, IoT Hub FQDN and container ID as parameters (pass NULL for callbacks) ]*/
                if ((transport_state->connection = connection_create2(transport_state->tls_io, STRING_c_str(transport_state->iotHubHostFqdn), DEFAULT_CONTAINER_ID, NULL, NULL, NULL, NULL, on_connection_io_error, (void*)transport_state)) == NULL)
                {
                    // Codes_SRS_IOTHUBTRANSPORTAMQP_09_063: [If connection_create2() fails, IoTHubTransportAMQP_DoWork shall fail and return immediately.] 
                    result = RESULT_FAILURE;
                    LogError("Failed to create the AMQP connection.");
                }
                else
                {
                    // Codes_SRS_IOTHUBTRANSPORTAMQP_09_137: [IoTHubTransportAMQP_DoWork shall create the AMQP session session_create() AMQP API, passing the connection instance as parameter]
                    if ((transport_state->session = session_create(transport_state->connection, NULL, NULL)) == NULL)
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

						if (getSecondsSinceEpoch(&transport_state->connection_establish_time) != RESULT_OK)
						{
							LogError("Failed setting the connection establish time.");
							result = RESULT_FAILURE;
						}
						else
						{
                        connection_set_trace(transport_state->connection, transport_state->is_trace_on);
                        (void)xio_setoption(transport_state->tls_io, OPTION_LOG_TRACE, &transport_state->is_trace_on);
                        result = RESULT_OK;
						}
                    }
                }
                break;
            }
            default:
            {
                LogError("internal error: unexpected enum value for transport_state->credential.credentialType = %d", transport_state->credential.credentialType);
                result = RESULT_FAILURE;
                break;
            }
        }/*switch*/
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
    if (cbs_put_token(transport_state->cbs.cbs, CBS_AUDIENCE, STRING_c_str(transport_state->devicesPath), STRING_c_str(sasToken), on_put_token_complete, transport_state) != RESULT_OK)
    {
        LogError("Failed applying new SAS token to CBS.");
        result = __LINE__;
    }
    else
    {
        transport_state->cbs.cbs_state = CBS_STATE_AUTH_IN_PROGRESS;
        transport_state->cbs.current_sas_token_create_time = sas_token_create_time;
        result = RESULT_OK;
    }
    return result;
}

static int startAuthentication(AMQP_TRANSPORT_INSTANCE* transport_state)
{
    int result;
    size_t currentTimeInSeconds;

	if (getSecondsSinceEpoch(&currentTimeInSeconds) != RESULT_OK)
	{
		LogError("Failed getting current time to compute the SAS token creation time.");
		result = __LINE__;
	}
	else
	{
		// Codes_SRS_IOTHUBTRANSPORTAMQP_09_083: [SAS tokens expiration time shall be calculated using the number of seconds since Epoch UTC (Jan 1st 1970 00h00m00s000 GMT) to now (GMT), plus the 'sas_token_lifetime'.]
		size_t new_expiry_time = currentTimeInSeconds + (transport_state->cbs.sas_token_lifetime / 1000);

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
				newSASToken = SASToken_Create(transport_state->credential.credential.deviceKey, transport_state->devicesPath, transport_state->cbs.sasTokenKeyName, new_expiry_time);
				if (newSASToken == NULL)
				{
					LogError("Could not generate a new SAS token for the CBS.");
					result = RESULT_FAILURE;
				}
				else
				{
					if (handSASTokenToCbs(transport_state, newSASToken, currentTimeInSeconds) != 0)
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
					if (handSASTokenToCbs(transport_state, newSASToken, currentTimeInSeconds) != 0)
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
	}
    return result;
}

static int verifyAuthenticationTimeout(AMQP_TRANSPORT_INSTANCE* transport_state)
{
	int result;
	size_t currentTimeInSeconds;
	if (getSecondsSinceEpoch(&currentTimeInSeconds) != RESULT_OK)
	{
		LogError("Failed getting the current time to verify if the SAS token needs to be refreshed.");
		result = RESULT_TIMEOUT; // Fail safe.
	}
	else
	{
		result = ((currentTimeInSeconds - transport_state->cbs.current_sas_token_create_time) * 1000 >= transport_state->cbs.cbs_request_timeout) ? RESULT_TIMEOUT : RESULT_OK;
	}
	return result;
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
    if (context != NULL)
    {
        AMQP_TRANSPORT_INSTANCE* transport_state = (AMQP_TRANSPORT_INSTANCE*)context;

        if (transport_state->is_trace_on)
        {
            LogInfo("Event sender state changed [%d->%d]", previous_state, new_state);
        }

        // Codes_SRS_IOTHUBTRANSPORTAMQP_09_192: [If a message sender instance changes its state to MESSAGE_SENDER_STATE_ERROR (first transition only) the connection retry logic shall be triggered]
        if (new_state != previous_state && new_state == MESSAGE_SENDER_STATE_ERROR)
        {
            transport_state->connection_state = AMQP_MANAGEMENT_STATE_ERROR;
        }
    }
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
            // Codes_SRS_IOTHUBTRANSPORTAMQP_09_191: [IoTHubTransportAMQP_DoWork shall create each AMQP message sender tracking its state changes with a callback function]
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

void on_message_receiver_state_changed(const void* context, MESSAGE_RECEIVER_STATE new_state, MESSAGE_RECEIVER_STATE previous_state)
{
    if (context != NULL)
    {
        AMQP_TRANSPORT_INSTANCE* transport_state = (AMQP_TRANSPORT_INSTANCE*)context;

        if (transport_state->is_trace_on)
        {
            LogInfo("Message receiver state changed [%d->%d]", previous_state, new_state);
        }

        // Codes_SRS_IOTHUBTRANSPORTAMQP_09_190: [If a message_receiver instance changes its state to MESSAGE_RECEIVER_STATE_ERROR (first transition only) the connection retry logic shall be triggered]
        if (new_state != previous_state && new_state == MESSAGE_RECEIVER_STATE_ERROR)
        {
            transport_state->connection_state = AMQP_MANAGEMENT_STATE_ERROR;
        }
    }
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
            // Codes_SRS_IOTHUBTRANSPORTAMQP_09_189: [IoTHubTransportAMQP_DoWork shall create each AMQP message_receiver tracking its state changes with a callback function]
            if ((transport_state->message_receiver = messagereceiver_create(transport_state->receiver_link, on_message_receiver_state_changed, (void*)transport_state)) == NULL)
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

        MESSAGE_HANDLE amqp_message = NULL;
        bool is_message_error = false;

        // Codes_SRS_IOTHUBTRANSPORTAMQP_09_086: [IoTHubTransportAMQP_DoWork shall move queued events to an "in-progress" list right before processing them for sending]
        trackEventInProgress(message, transport_state);

		// Codes_SRS_IOTHUBTRANSPORTAMQP_09_193: [IoTHubTransportAMQP_DoWork shall get a MESSAGE_HANDLE instance out of the event's IOTHUB_MESSAGE_HANDLE instance by using message_create_from_iothub_message().]
		if ((result = message_create_from_iothub_message(message->messageHandle, &amqp_message)) != RESULT_OK)
		{
			LogError("Failed creating AMQP message (error=%d).", result);
			result = __LINE__;
			is_message_error = true;
		}
		// Codes_SRS_IOTHUBTRANSPORTAMQP_09_097: [IoTHubTransportAMQP_DoWork shall pass the MESSAGE_HANDLE intance to uAMQP for sending (along with on_message_send_complete callback) using messagesender_send()] 
		else if (messagesender_send(transport_state->message_sender, amqp_message, on_message_send_complete, message) != RESULT_OK)
        {
            LogError("Failed sending the AMQP message.");
			result = __LINE__;
        }
        else
        {
            result = RESULT_OK;
        }

		// Codes_SRS_IOTHUBTRANSPORTAMQP_09_194: [IoTHubTransportAMQP_DoWork shall destroy the MESSAGE_HANDLE instance after messagesender_send() is invoked.]
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
                // Codes_SRS_IOTHUBTRANSPORTAMQP_09_111: [If message_create_from_iothub_message() fails, IoTHubTransportAMQP_DoWork notify the failure, roll back the event to waitToSend list and return]
                // Codes_SRS_IOTHUBTRANSPORTAMQP_09_112: [If message_add_body_amqp_data() fails, IoTHubTransportAMQP_DoWork notify the failure, roll back the event to waitToSent list and return]
                // Codes_SRS_IOTHUBTRANSPORTAMQP_09_113: [If messagesender_send() fails, IoTHubTransportAMQP_DoWork notify the failure, roll back the event to waitToSend list and return]
                rollEventBackToWaitList(message, transport_state);
                break;
            }
        }
    }

    return result;
}

static bool isSasTokenRefreshRequired(AMQP_TRANSPORT_INSTANCE* transport_state)
{
	bool result;
	size_t currentTimeInSeconds;
    if (transport_state->credential.credentialType == DEVICE_SAS_TOKEN)
    {
        result = false;
    }
	else if (getSecondsSinceEpoch(&currentTimeInSeconds) != RESULT_OK)
	{
		LogError("Failed getting the current time to verify if the SAS token needs to be refreshed.");
		result = true; // Fail safe.
	}
    else
    {
        result = ((currentTimeInSeconds - transport_state->cbs.current_sas_token_create_time) >= (transport_state->cbs.sas_token_refresh_time / 1000)) ? true : false;
    }
	
	return result;
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
            transport_state->targetAddress = NULL;
            transport_state->waitingToSend = config->waitingToSend;
            transport_state->connection = NULL;
            transport_state->connection_state = AMQP_MANAGEMENT_STATE_IDLE;
            transport_state->connection_establish_time = 0;
            transport_state->iothub_client_handle = NULL;
            transport_state->receive_messages = false;
            transport_state->message_receiver = NULL;
            transport_state->message_sender = NULL;
            transport_state->receiver_link = NULL;
            transport_state->sender_link = NULL;
            transport_state->session = NULL;
            transport_state->tls_io = NULL;
            transport_state->tls_io_transport_provider = getTLSIOTransport;
            transport_state->isRegistered = false;
            transport_state->is_trace_on = false;

            transport_state->cbs.cbs = NULL;
            transport_state->cbs.sasTokenKeyName = NULL;
            transport_state->cbs.cbs_state = CBS_STATE_IDLE;
            transport_state->cbs.current_sas_token_create_time = 0;
            transport_state->cbs.sasl_io = NULL;
            transport_state->cbs.sasl_mechanism = NULL;

            transport_state->xioOptions = NULL; 

            transport_state->waitingToSend = config->waitingToSend;
            DList_InitializeListHead(&transport_state->inProgress);

            transport_state->credential.credentialType = CREDENTIAL_NOT_BUILD;

            // Codes_SRS_IOTHUBTRANSPORTAMQP_09_010: [If config->upperConfig->protocolGatewayHostName is NULL, IoTHubTransportAMQP_Create shall create an immutable string, referred to as iotHubHostFqdn, from the following pieces: config->iotHubName + "." + config->iotHubSuffix.] 
            // Codes_SRS_IOTHUBTRANSPORTAMQP_20_001: [If config->upperConfig->protocolGatewayHostName is not NULL, IoTHubTransportAMQP_Create shall use it as iotHubHostFqdn]
            if ((transport_state->iotHubHostFqdn = (config->upperConfig->protocolGatewayHostName != NULL ? STRING_construct(config->upperConfig->protocolGatewayHostName) : concat3Params(config->upperConfig->iotHubName, ".", config->upperConfig->iotHubSuffix))) == NULL)
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
            else
            {
                if (config->upperConfig->deviceSasToken != NULL)
                {
                    /*only SAS token specified*/
                    // Codes_SRS_IOTHUBTRANSPORTAMQP_09_016: [IoTHubTransportAMQP_Create shall initialize handle->sasTokenKeyName with a zero-length STRING_HANDLE instance.] 
                    if ((transport_state->cbs.sasTokenKeyName = STRING_new()) == NULL)
                    {
                        // Codes_SRS_IOTHUBTRANSPORTAMQP_09_017: [If IoTHubTransportAMQP_Create fails to initialize handle->sasTokenKeyName with a zero-length STRING the function shall fail and return NULL.] 
                        LogError("Failed to allocate transport_state->sasTokenKeyName.");
                        cleanup_required = true;
                    }
                    else
                    {
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
                        // Codes_SRS_IOTHUBTRANSPORTAMQP_09_016: [IoTHubTransportAMQP_Create shall initialize handle->sasTokenKeyName with a zero-length STRING_HANDLE instance.] 
                        if ((transport_state->cbs.sasTokenKeyName = STRING_new()) == NULL)
                        {
                            // Codes_SRS_IOTHUBTRANSPORTAMQP_09_017: [If IoTHubTransportAMQP_Create fails to initialize handle->sasTokenKeyName with a zero-length STRING the function shall fail and return NULL.] 
                            LogError("Failed to allocate transport_state->sasTokenKeyName.");
                            cleanup_required = true;
                        }
                        else
                        {
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
                    }
                    else
                    {
                        /*Codes_SRS_IOTHUBTRANSPORTAMQP_02_004: [ If both deviceKey and deviceSasToken fields are NULL then IoTHubTransportAMQP_Create shall assume a x509 authentication. ]*/
                        /*Codes_SRS_IOTHUBTRANSPORTAMQP_02_003: [ IoTHubTransportAMQP_Register shall assume a x509 authentication mechanism when both deviceKey and deviceSasToken are NULL. ]*/
                        /*when both SAS token AND devicekey are NULL*/
                        transport_state->credential.credentialType = X509;
                        transport_state->credential.credential.x509credential.x509certificate = NULL;
                        transport_state->credential.credential.x509credential.x509privatekey = NULL;
                    }
                }

                if (transport_state->credential.credentialType != CREDENTIAL_NOT_BUILD)
                {
                    // Codes_SRS_IOTHUBTRANSPORTAMQP_09_020: [IoTHubTransportAMQP_Create shall set parameter transport_state->sas_token_lifetime with the default value of 3600000 (milliseconds).]
                    transport_state->cbs.sas_token_lifetime = DEFAULT_SAS_TOKEN_LIFETIME_MS;

                    // Codes_SRS_IOTHUBTRANSPORTAMQP_09_128: [IoTHubTransportAMQP_Create shall set parameter transport_state->sas_token_refresh_time with the default value of sas_token_lifetime/2 (milliseconds).] 
                    transport_state->cbs.sas_token_refresh_time = transport_state->cbs.sas_token_lifetime / 2;

                    // Codes_SRS_IOTHUBTRANSPORTAMQP_09_129 : [IoTHubTransportAMQP_Create shall set parameter transport_state->cbs_request_timeout with the default value of 30000 (milliseconds).]
                    transport_state->cbs.cbs_request_timeout = DEFAULT_CBS_REQUEST_TIMEOUT_MS;
                }
                
            }

            if (cleanup_required)
            {
                credential_destroy(transport_state);
                if (transport_state->cbs.sasTokenKeyName != NULL)
                    STRING_delete(transport_state->cbs.sasTokenKeyName);
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
        STRING_delete(transport_state->cbs.sasTokenKeyName);
        credential_destroy(transport_state);
        STRING_delete(transport_state->devicesPath);
        STRING_delete(transport_state->iotHubHostFqdn);

        // Codes_SRS_IOTHUBTRANSPORTAMQP_09_036 : [IoTHubTransportAMQP_Destroy shall return the remaining items in inProgress to waitingToSend list.]
        rollEventsBackToWaitList(transport_state);

        if (transport_state->xioOptions != NULL)
        {
            OptionHandler_Destroy(transport_state->xioOptions);
        }

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
        else 
        {
            switch(transport_state->credential.credentialType)
            {
                case(DEVICE_KEY):
                case(DEVICE_SAS_TOKEN):
                {
                    // Codes_SRS_IOTHUBTRANSPORTAMQP_09_081: [IoTHubTransportAMQP_DoWork shall put a new SAS token if the one has not been out already, or if the previous one failed to be put due to timeout of cbs_put_token().]
                    // Codes_SRS_IOTHUBTRANSPORTAMQP_09_082: [IoTHubTransportAMQP_DoWork shall refresh the SAS token if the current token has been used for more than 'sas_token_refresh_time' milliseconds]
                    if ((transport_state->cbs.cbs_state == CBS_STATE_IDLE || isSasTokenRefreshRequired(transport_state)) &&
                        startAuthentication(transport_state) != RESULT_OK)
                    {
                        // Codes_SRS_IOTHUBTRANSPORTAMQP_09_146: [If the SAS token fails to be sent to CBS (cbs_put_token), IoTHubTransportAMQP_DoWork shall fail and exit immediately]
                        LogError("Failed authenticating AMQP connection within CBS.");
                        trigger_connection_retry = true;
                    }
                    // Codes_SRS_IOTHUBTRANSPORTAMQP_09_084: [IoTHubTransportAMQP_DoWork shall wait for 'cbs_request_timeout' milliseconds for the cbs_put_token() to complete before failing due to timeout]
                    else if (transport_state->cbs.cbs_state == CBS_STATE_AUTH_IN_PROGRESS &&
                        verifyAuthenticationTimeout(transport_state) == RESULT_TIMEOUT)
                    {
                        LogError("AMQP transport authentication timed out.");
                        trigger_connection_retry = true;
                    }
                    else if (transport_state->cbs.cbs_state == CBS_STATE_AUTHENTICATED)
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
                    break;
                }
                case (X509):
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
                    break;
                }
                default:
                {
                    LogError("internal error: unexpected enum value : transport_state->credential.credentialType = %d", transport_state->credential.credentialType);
                    trigger_connection_retry = true;
                }
            }/*switch*/
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
        if (strcmp(OPTION_SAS_TOKEN_LIFETIME, option) == 0)
        {
            transport_state->cbs.sas_token_lifetime = *((size_t*)value);
            result = IOTHUB_CLIENT_OK;
        }
        // Codes_SRS_IOTHUBTRANSPORTAMQP_09_049: [IotHubTransportAMQP_SetOption shall save and apply the value if the option name is "sas_token_refresh_time", returning IOTHUB_CLIENT_OK] 
        else if (strcmp(OPTION_SAS_TOKEN_REFRESH_TIME, option) == 0)
        {
            transport_state->cbs.sas_token_refresh_time = *((size_t*)value);
            result = IOTHUB_CLIENT_OK;
        }
        // Codes_SRS_IOTHUBTRANSPORTAMQP_09_148: [IotHubTransportAMQP_SetOption shall save and apply the value if the option name is "cbs_request_timeout", returning IOTHUB_CLIENT_OK] 
        else if (strcmp(OPTION_CBS_REQUEST_TIMEOUT, option) == 0)
        {
            transport_state->cbs.cbs_request_timeout = *((size_t*)value);
            result = IOTHUB_CLIENT_OK;
        }
        else if (strcmp(OPTION_LOG_TRACE, option) == 0)
        {
            transport_state->is_trace_on = *((bool*)value);
            if (transport_state->connection != NULL)
            {
                connection_set_trace(transport_state->connection, transport_state->is_trace_on);
            }
            result = IOTHUB_CLIENT_OK;
        }
        /*Codes_SRS_IOTHUBTRANSPORTAMQP_02_007: [ If optionName is x509certificate and the authentication method is not x509 then IoTHubTransportAMQP_SetOption shall return IOTHUB_CLIENT_INVALID_ARG. ]*/
        else if ((strcmp(OPTION_X509_CERT, option) == 0) && (transport_state->credential.credentialType != X509))
        {
            LogError("x509certificate specified, but authentication method is not x509");
            result = IOTHUB_CLIENT_INVALID_ARG;
        }
        /*Codes_SRS_IOTHUBTRANSPORTAMQP_02_008: [ If optionName is x509privatekey and the authentication method is not x509 then IoTHubTransportAMQP_SetOption shall return IOTHUB_CLIENT_INVALID_ARG. ]*/
        else if ((strcmp(OPTION_X509_PRIVATE_KEY, option) == 0) && (transport_state->credential.credentialType != X509))
        {
            LogError("x509privatekey specified, but authentication method is not x509");
            result = IOTHUB_CLIENT_INVALID_ARG;
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
                /*in the case when a tls_io_transport has been created, replay its options*/
                if (transport_state->xioOptions != NULL)
                {
                    if (OptionHandler_FeedOptions(transport_state->xioOptions, transport_state->tls_io) != 0)
                    {
                        LogError("unable to replay options to TLS"); /*pessimistically hope TLS will fail, be recreated and options re-given*/
                    }
                    else
                    {
                        /*everything is fine, forget the saved options...*/
                        OptionHandler_Destroy(transport_state->xioOptions);
                        transport_state->xioOptions = NULL;
                    }
                }

                /* Codes_SRS_IOTHUBTRANSPORTAMQP_03_001: [If xio_setoption fails, IoTHubTransportAMQP_SetOption shall return IOTHUB_CLIENT_ERROR.] */
                if (xio_setoption(transport_state->tls_io, option, value) == 0)
                {
                    result = IOTHUB_CLIENT_OK;
                }
                else
                {
                    result = IOTHUB_CLIENT_ERROR;
                    LogError("Invalid option (%s) passed to IoTHubTransportAMQP_SetOption", option);
                }
            }
        }
    }

    return result;
}

static IOTHUB_DEVICE_HANDLE IoTHubTransportAMQP_Register(TRANSPORT_LL_HANDLE handle, const IOTHUB_DEVICE_CONFIG* device, IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle, PDLIST_ENTRY waitingToSend)
{
#ifdef NO_LOGGING
    UNUSED(iotHubClientHandle);
#endif

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
        else if ((device->deviceSasToken != NULL) && (device->deviceKey != NULL))
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
