// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#include "gballoc.h"

#include "cbs.h"
#include "link.h"
#include "message.h"
#include "message_receiver.h"
#include "message_sender.h"
#include "messaging.h"
#include "sasl_mssbcbs.h"
#include "saslclientio.h"

#include "crt_abstractions.h"
#include "doublylinkedlist.h"
#include "iot_logging.h"
#include "platform.h"
#include "sastoken.h"
#include "strings.h"
#include "tlsio_openssl.h"
#include "tlsio_schannel.h"
#include "tlsio_wolfssl.h"
#include "urlencode.h"

#include "iothub_client_ll.h"
#include "iothub_client_private.h"
#include "iothubtransportuamqp.h"

#define RESULT_OK 0
#define RESULT_FAILURE 1
#define RESULT_TIMEOUT 2

#define TIME_MAX SIZE_MAX
#define RFC1035_MAX_FQDN_LENGTH 255
#define DEFAULT_IOTHUB_AMQP_PORT 5671
#define DEFAULT_SAS_TOKEN_LIFETIME_MS 3600000
#define DEFAULT_CBS_REQUEST_TIMEOUT_MS 30000
#define DEFAULT_MESSAGE_SEND_TIMEOUT_MS 300000
#define CBS_AUDIENCE "servicebus.windows.net:sastoken"
#define DEFAULT_CONTAINER_ID "default_container_id"
#define DEFAULT_INCOMING_WINDOW_SIZE SIZE_MAX
#define DEFAULT_OUTGOING_WINDOW_SIZE 100
#define MESSAGE_RECEIVER_LINK_NAME "receiver-link"
#define MESSAGE_RECEIVER_TARGET_ADDRESS "ingress-rx"
#define MESSAGE_RECEIVER_MAX_LINK_SIZE 65536
#define MESSAGE_SENDER_LINK_NAME "sender-link"
#define MESSAGE_SENDER_SOURCE_ADDRESS "ingress"
#define MESSAGE_SENDER_MAX_LINK_SIZE 65536

typedef enum CBS_STATE_TAG
{
	CBS_STATE_IDLE,
	CBS_STATE_AUTH_IN_PROGRESS,
	CBS_STATE_AUTHENTICATED
} CBS_STATE;

typedef struct UAMQP_TRANSPORT_STATE_TAG
{
	// FQDN of the IoT Hub.
	STRING_HANDLE iotHubHostFqdn;
	// AMQP port of the IoT Hub.
	int iotHubPort;
	// Key associated to the device to be used.
	STRING_HANDLE deviceKey;
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
	// Maximum time the transport waits for an event to be sent before marking it a failure, in milliseconds.
	size_t message_send_timeout;
	// Maximum time for the connection establishment/retry logic should wait for a connection to succeed, in milliseconds.
	size_t connection_timeout;
	// A callback passed by the app layer to provide the I/O transport instance (e.g., TLS, WebSockets, etc).
	IO_TRANSPORT_PROVIDER_CALLBACK io_transport_provider_callback;
	// Saved reference to the IoTHub LL Client.
	IOTHUB_CLIENT_LL_HANDLE iothub_client_handle;
	
	// Instance obtained using io_transport_provider_callback.
	XIO_HANDLE tls_io;
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
} UAMQP_TRANSPORT_INSTANCE;

// This structure is used to track an event being sent and the time it was sent.
typedef struct EVENT_TRACKER_TAG
{
	IOTHUB_CLIENT_LL_HANDLE iothub_client_handle;
	IOTHUB_MESSAGE_LIST* message;
	time_t time_sent;
	DLIST_ENTRY entry;
} EVENT_TRACKER;


// Auxiliary functions

static STRING_HANDLE concat3Params(const char* prefix, const char* infix, const char* suffix)
{
	STRING_HANDLE result = NULL;
	char* concat = NULL;
	size_t totalLength = strlen(prefix) + strlen(infix) + strlen(suffix) + 1; // One extra for \0.

	if ((concat = (char*)malloc(sizeof(char) * totalLength)) != NULL)
	{
		if (snprintf(concat, totalLength, "%s%s%s", prefix, infix, suffix) > 0)
		{
			result = STRING_construct(concat);
		}
		free(concat);
	}

	return result;
}

static size_t getSecondsSinceEpoch()
{
	return (size_t)(difftime(get_time(NULL), (time_t)0) + 0);
}

static EVENT_TRACKER* trackEvent(IOTHUB_MESSAGE_LIST* message, UAMQP_TRANSPORT_INSTANCE* transport_state)
{
	EVENT_TRACKER* event_tracker = NULL;

	if ((event_tracker = (EVENT_TRACKER*)malloc(sizeof(EVENT_TRACKER))) != NULL)
	{
		event_tracker->iothub_client_handle = transport_state->iothub_client_handle;
		event_tracker->message = message;
		event_tracker->time_sent = get_time(NULL);
		DList_InitializeListHead(&event_tracker->entry);

		DList_RemoveEntryList(&message->entry);
		DList_InsertTailList(&transport_state->inProgress, &event_tracker->entry);
	}

	return event_tracker;
}

static void destroyEventTracker(EVENT_TRACKER* event_tracker)
{
	free(event_tracker);
}

static IOTHUB_MESSAGE_LIST* getNextEventToSend(UAMQP_TRANSPORT_INSTANCE* transport_state)
{
	IOTHUB_MESSAGE_LIST* message = NULL;

	if (!DList_IsListEmpty(transport_state->waitingToSend))
	{
		PDLIST_ENTRY list_entry = transport_state->waitingToSend->Flink;
		message = containingRecord(list_entry, IOTHUB_MESSAGE_LIST, entry);
	}

	return message;
}

static int isEventInInProgressList(EVENT_TRACKER* event_tracker)
{
	return !DList_IsListEmpty(&event_tracker->entry);
}

static void removeEventFromInProgressList(EVENT_TRACKER* event_tracker)
{
	DList_RemoveEntryList(&event_tracker->entry);
	DList_InitializeListHead(&event_tracker->entry);
}

static void rollEventBackToWaitList(EVENT_TRACKER* event_tracker, UAMQP_TRANSPORT_INSTANCE* transport_state)
{
	removeEventFromInProgressList(event_tracker);
	DList_InsertTailList(transport_state->waitingToSend, &event_tracker->message->entry);
	destroyEventTracker(event_tracker);
}

static void rollEventsBackToWaitList(UAMQP_TRANSPORT_INSTANCE* transport_state)
{
	PDLIST_ENTRY entry = transport_state->inProgress.Blink;

	while (entry != &transport_state->inProgress)
	{
		EVENT_TRACKER* event_tracker = containingRecord(entry, EVENT_TRACKER, entry);
		entry = entry->Blink;
		rollEventBackToWaitList(event_tracker, transport_state);
	}
}

void on_message_send_complete(const void* context, MESSAGE_SEND_RESULT send_result)
{
	if (context != NULL)
	{
		EVENT_TRACKER* event_tracker = (EVENT_TRACKER*)context;

		IOTHUB_BATCHSTATE_RESULT iot_hub_send_result;

		// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_142: [The callback ‘on_message_send_complete’ shall pass to the upper layer callback an IOTHUB_BATCHSTATE_SUCCESS if the result received is MESSAGE_SEND_OK] 
		if (send_result == MESSAGE_SEND_OK)
		{
			iot_hub_send_result = IOTHUB_BATCHSTATE_SUCCESS;
		}
		// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_143: [The callback ‘on_message_send_complete’ shall pass to the upper layer callback an IOTHUB_BATCHSTATE_FAILED if the result received is MESSAGE_SEND_ERROR]
		else if (send_result == MESSAGE_SEND_ERROR)
		{
			iot_hub_send_result = IOTHUB_BATCHSTATE_FAILED;
		}

		// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_102: [The callback ‘on_message_send_complete’ shall invoke the upper layer IoTHubClient_LL_SendComplete() passing the client handle, message received and batch state result] 
		IoTHubClient_LL_SendComplete(event_tracker->iothub_client_handle, &event_tracker->message->entry, iot_hub_send_result);

		// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_100: [The callback ‘on_message_send_complete’ shall remove the target message from the in-progress list after the upper layer callback] 
		if (isEventInInProgressList(event_tracker))
		{
			removeEventFromInProgressList(event_tracker);
			destroyEventTracker(event_tracker);
		}
	}

}

void on_put_token_complete(const void* context, OPERATION_RESULT operation_result, unsigned int status_code, const char* status_description)
{
	UAMQP_TRANSPORT_INSTANCE* transportState = (UAMQP_TRANSPORT_INSTANCE*)context;

	if (operation_result == OPERATION_RESULT_OK)
	{
		transportState->cbs_state = CBS_STATE_AUTHENTICATED;
	}
}

AMQP_VALUE on_message_received(const void* context, MESSAGE_HANDLE message)
{
	AMQP_VALUE result = NULL;
	IOTHUBMESSAGE_DISPOSITION_RESULT disposition_result = IOTHUBMESSAGE_REJECTED;

	// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_104: [The callback ‘on_message_received’ shall invoke IoTHubClient_LL_MessageCallback() passing the client and the incoming message handles as parameters] 
	IOTHUB_CLIENT_LL_HANDLE iothub_client_handle = (IOTHUB_CLIENT_LL_HANDLE)context;
	IOTHUB_MESSAGE_HANDLE iothub_message = NULL;
	MESSAGE_BODY_TYPE body_type;

	
	if (message_get_body_type(message, &body_type) != 0)
	{
		LogError("Failed to get the type of the message received by the transport.\r\n");
	}
	else 
	{
		if (body_type == MESSAGE_BODY_TYPE_DATA)
		{
			BINARY_DATA binary_data;
			if (message_get_body_amqp_data(message, 0, &binary_data) != 0)
			{
				LogError("Failed to get the body of the message received by the transport.\r\n");
			}
			else
			{
				iothub_message = IoTHubMessage_CreateFromByteArray(binary_data.bytes, binary_data.length);
			}
		}
	}
	
	if (iothub_message == NULL)
	{
		LogError("Transport failed processing the message received.\r\n");
	}
	else
	{
		disposition_result = IoTHubClient_LL_MessageCallback((IOTHUB_CLIENT_LL_HANDLE)context, iothub_message);

		// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_105: [The callback ‘on_message_received’ shall return the result of messaging_delivery_accepted() if the IoTHubClient_LL_MessageCallback() returns IOTHUBMESSAGE_ACCEPTED] 
		if (disposition_result == IOTHUBMESSAGE_ACCEPTED)
		{
			result = messaging_delivery_accepted();
		}
		// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_106: [The callback ‘on_message_received’ shall return the result of messaging_delivery_released() if the IoTHubClient_LL_MessageCallback() returns IOTHUBMESSAGE_ABANDONED] 
		else if (disposition_result == IOTHUBMESSAGE_ABANDONED)
		{
			result = messaging_delivery_released();
		}
		// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_107: [The callback ‘on_message_received’ shall return the result of messaging_delivery_rejected(“Rejected by application”, “Rejected by application”) if the IoTHubClient_LL_MessageCallback() returns IOTHUBMESSAGE_REJECTED] 
		else if (disposition_result == IOTHUBMESSAGE_REJECTED)
		{
			result = messaging_delivery_rejected("Rejected by application", "Rejected by application");
		}
	}

	return result;
}

XIO_HANDLE default_io_transport_provider(const char* fqdn, int port)
{
	const IO_INTERFACE_DESCRIPTION* io_interface_description;

#ifdef _WIN32
    // Codes_SRS_IOTHUBTRANSPORTUAMQP_09_126: [default_io_transport_provider shall create and return a XIO_HANDLE instance for TLS transport using SChannel if the current platform is WINDOWS] 
    TLSIO_SCHANNEL_CONFIG tls_io_config = { fqdn, port };
    io_interface_description = tlsio_schannel_get_interface_description();
#else
#ifdef MBED_BUILD_TIMESTAMP
    TLSIO_WOLFSSL_CONFIG tls_io_config = { fqdn, port };
    io_interface_description = tlsio_wolfssl_get_interface_description();
#else
    // Codes_SRS_IOTHUBTRANSPORTUAMQP_09_127: [default_io_transport_provider shall create and return a XIO_HANDLE instance for TLS transport using OpenSSL if the current platform is not WINDOWS]
    TLSIO_OPENSSL_CONFIG tls_io_config = { fqdn, port };
    io_interface_description = tlsio_openssl_get_interface_description();
#endif
#endif

	return xio_create(io_interface_description, &tls_io_config, NULL);
}

static void destroyConnection(UAMQP_TRANSPORT_INSTANCE* transport_state)
{
	if (transport_state != NULL)
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
			// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_034: [IoTHubTransportuAMQP_Destroy shall destroy the uAMQP TLS I/O transport if created internally.]
			if (transport_state->io_transport_provider_callback == default_io_transport_provider)
			{
				xio_destroy(transport_state->tls_io);
			}

			transport_state->tls_io = NULL;
		}
	}
}

static void on_amqp_management_state_changed(void* context, AMQP_MANAGEMENT_STATE new_amqp_management_state, AMQP_MANAGEMENT_STATE previous_amqp_management_state)
{
	if (context != NULL)
	{
		UAMQP_TRANSPORT_INSTANCE* transport_state = (UAMQP_TRANSPORT_INSTANCE*)context;
		transport_state->connection_state = new_amqp_management_state;
	}
}

static int establishConnection(UAMQP_TRANSPORT_INSTANCE* transport_state)
{
	int result;

	// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_110: [IoTHubTransportuAMQP_DoWork shall create the TLS IO using transport_state->io_transport_provider callback function] 
	if (transport_state->tls_io == NULL &&
		(transport_state->tls_io = transport_state->io_transport_provider_callback(STRING_c_str(transport_state->iotHubHostFqdn), transport_state->iotHubPort)) == NULL)
	{
		// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_136: [If transport_state->io_transport_provider_callback fails, IoTHubTransportuAMQP_DoWork shall fail and return immediately]
		result = RESULT_FAILURE;
		LogError("Failed to obtain a I/O transport layer from io_transport_provider_callback.\r\n");
	}
	// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_056: [IoTHubTransportuAMQP_DoWork shall create the SASL mechanism using uAMQP’s saslmechanism_create() API] 
	else if ((transport_state->sasl_mechanism = saslmechanism_create(saslmssbcbs_get_interface(), NULL)) == NULL)
	{
		// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_057: [If saslmechanism_create() fails, IoTHubTransportuAMQP_DoWork shall fail and return immediately]
		result = RESULT_FAILURE;
		LogError("Failed to create a SASL mechanism.\r\n");
	}
	else
	{
		// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_060: [IoTHubTransportuAMQP_DoWork shall create the SASL I / O layer using the xio_create() C Shared Utility API]
		SASLCLIENTIO_CONFIG sasl_client_config = { transport_state->tls_io, transport_state->sasl_mechanism };
		if ((transport_state->sasl_io = xio_create(saslclientio_get_interface_description(), &sasl_client_config, NULL)) == NULL)
		{
			// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_061: [If xio_create() fails creating the SASL I/O layer, IoTHubTransportuAMQP_DoWork shall fail and return immediately] 
			result = RESULT_FAILURE;
			LogError("Failed to create a SASL I/O layer.\r\n");
		}
		// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_062: [IoTHubTransportuAMQP_DoWork shall create the connection with the IoT service using connection_create() uAMQP API, passing the SASL I/O layer, IoT Hub FQDN and container ID as parameters (pass NULL for callbacks)] 
		else if ((transport_state->connection = connection_create(transport_state->sasl_io, STRING_c_str(transport_state->iotHubHostFqdn), DEFAULT_CONTAINER_ID, NULL, NULL)) == NULL)
		{
			// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_063: [If connection_create() fails, IoTHubTransportuAMQP_DoWork shall fail and return immediately.] 
			result = RESULT_FAILURE;
			LogError("Failed to create the uAMQP connection.\r\n");
		}
		// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_137: [IoTHubTransportuAMQP_DoWork shall create the AMQP session session_create() uAMQP API, passing the connection instance as parameter]
		else if ((transport_state->session = session_create(transport_state->connection, NULL, NULL)) == NULL)
		{
			// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_138 : [If session_create() fails, IoTHubTransportuAMQP_DoWork shall fail and return immediately]
			result = RESULT_FAILURE;
			LogError("Failed to create the uAMQP session.\r\n");
		}
		else
		{
			// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_065: [IoTHubTransportuAMQP_DoWork shall apply a default value of UINT_MAX for the parameter ‘AMQP incoming window’] 
			if (session_set_incoming_window(transport_state->session, DEFAULT_INCOMING_WINDOW_SIZE) != 0)
			{
				LogError("Failed to set the uAMQP incoming window size.\r\n");
			}

			// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_115: [IoTHubTransportuAMQP_DoWork shall apply a default value of 100 for the parameter ‘AMQP outgoing window’] 
			if (session_set_outgoing_window(transport_state->session, DEFAULT_OUTGOING_WINDOW_SIZE) != 0)
			{
				LogError("Failed to set the uAMQP outgoing window size.\r\n");
			}

			// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_066: [IoTHubTransportuAMQP_DoWork shall establish the CBS connection using the cbs_create() uAMQP API] 
			if ((transport_state->cbs = cbs_create(transport_state->session, on_amqp_management_state_changed, NULL)) == NULL)
			{
				// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_067: [If cbs_create() fails, IoTHubTransportuAMQP_DoWork shall fail and return immediately] 
				result = RESULT_FAILURE;
				LogError("Failed to create the CBS connection.\r\n");
			}
			// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_139: [IoTHubTransportuAMQP_DoWork shall open the CBS connection using the cbs_open() uAMQP API] 
			else if ((cbs_open(transport_state->cbs)) != 0)
			{
				// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_140: [If cbs_open() fails, IoTHubTransportuAMQP_DoWork shall fail and return immediately]
				result = RESULT_FAILURE;
				LogError("Failed to open the connection with CBS.\r\n");
			}
			else
			{
				transport_state->connection_establish_time = getSecondsSinceEpoch();
				transport_state->cbs_state = CBS_STATE_IDLE;
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

static int startAuthentication(UAMQP_TRANSPORT_INSTANCE* transport_state)
{
	int result = RESULT_FAILURE;

	size_t sas_token_create_time = getSecondsSinceEpoch(); // I.e.: NOW, in seconds since epoch.
	
	// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_083: [Each new SAS token created by the transport shall be valid for up to ‘sas_token_lifetime’ milliseconds from the time of creation]
	size_t new_expiry_time = sas_token_create_time + (transport_state->sas_token_lifetime / 1000);

	STRING_HANDLE newSASToken = SASToken_Create(transport_state->deviceKey, transport_state->devicesPath, transport_state->sasTokenKeyName, new_expiry_time);

	if (newSASToken == NULL)
	{
		LogError("Could not generate a new SAS token for the CBS\r\n");
	}
	else if (cbs_put_token(transport_state->cbs, CBS_AUDIENCE, STRING_c_str(transport_state->devicesPath), STRING_c_str(newSASToken), on_put_token_complete, transport_state) != RESULT_OK)
	{
		LogError("Failed applying new SAS token to CBS\r\n");
	}
	else
	{
		transport_state->cbs_state = CBS_STATE_AUTH_IN_PROGRESS;
		transport_state->current_sas_token_create_time = sas_token_create_time;
		result = RESULT_OK;
	}

	// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_145: [Each new SAS token created shall be deleted from memory immediately after sending it to CBS]
	if (newSASToken != NULL)
		STRING_delete(newSASToken);

	return result;
}

static int verifyAuthenticationTimeout(UAMQP_TRANSPORT_INSTANCE* transport_state)
{
	return ((getSecondsSinceEpoch() - transport_state->current_sas_token_create_time) * 1000 >= transport_state->cbs_request_timeout) ? RESULT_TIMEOUT : RESULT_OK;
}

static void handleEventSendTimeouts(UAMQP_TRANSPORT_INSTANCE* transport_state)
{
	time_t current_time = get_time(NULL);
	PDLIST_ENTRY entry = transport_state->inProgress.Flink;

	while (entry != &transport_state->inProgress)
	{
		EVENT_TRACKER* event_tracker = containingRecord(entry, EVENT_TRACKER, entry);

		// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_085: [IoTHubTransportuAMQP_DoWork shall attempt to send all the queued messages for up to ‘message_send_timeout’ milliseconds] 
		if (difftime(current_time, event_tracker->time_sent) * 1000 >= transport_state->message_send_timeout)
		{
			// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_120: [If a ‘message_send_timeout’ occurs the timed out events removed from the inProgress and the upper layer notified of the send error] 
			on_message_send_complete(event_tracker, MESSAGE_SEND_ERROR);
		}

		entry = entry->Flink;
	}
}

static int destroyEventSender(UAMQP_TRANSPORT_INSTANCE* transport_state)
{
	int result = RESULT_FAILURE;

	if (transport_state->message_sender != NULL)
	{
		messagesender_destroy(transport_state->message_sender);

		transport_state->message_sender = NULL;

		link_destroy(transport_state->sender_link);

		transport_state->sender_link = NULL;

		result = RESULT_OK;
	}

	return result;
}

static int createEventSender(UAMQP_TRANSPORT_INSTANCE* transport_state)
{
	int result = RESULT_FAILURE;

	if (transport_state->message_sender == NULL)
	{
		AMQP_VALUE source = NULL;
		AMQP_VALUE target = NULL;

		// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_068: [IoTHubTransportuAMQP_DoWork shall create the AMQP link for sending messages using ‘source’ as “ingress”, target as the IoT hub FQDN, link name as “sender-link” and role as ‘role_sender’] 
		if ((source = messaging_create_source(MESSAGE_SENDER_SOURCE_ADDRESS)) == NULL)
		{
			LogError("Failed creating uAMQP messaging source attribute.\r\n");
		}
		else if ((target = messaging_create_target(STRING_c_str(transport_state->targetAddress))) == NULL)
		{
			LogError("Failed creating uAMQP messaging target attribute.\r\n");
		}
		else if ((transport_state->sender_link = link_create(transport_state->session, MESSAGE_SENDER_LINK_NAME, role_sender, source, target)) == NULL)
		{
			// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_069: [If IoTHubTransportuAMQP_DoWork fails to create the AMQP link for sending messages, the function shall fail and return immediately, flagging the connection to be re-stablished] 
			LogError("Failed creating uAMQP link for message sender.\r\n");
		}
		else
		{
			// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_119: [IoTHubTransportuAMQP_DoWork shall apply a default value of 65536 for the parameter ‘Link MAX message size’]
			if (link_set_max_message_size(transport_state->sender_link, MESSAGE_SENDER_MAX_LINK_SIZE) != RESULT_OK)
			{
				LogError("Failed setting uAMQP link max message size.\r\n");
			}

			// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_070: [IoTHubTransportuAMQP_DoWork shall create the AMQP message sender using messagesender_create() uAMQP API] 
			if ((transport_state->message_sender = messagesender_create(transport_state->sender_link, NULL, NULL, NULL)) == NULL)
			{
				// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_071: [IoTHubTransportuAMQP_DoWork shall fail and return immediately if the uAMQP message sender instance fails to be created, flagging the connection to be re-established] 
				LogError("Could not allocate uAMQP message sender\r\n");
			}
			else
			{
				// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_072: [IoTHubTransportuAMQP_DoWork shall open the AMQP message sender using messagesender_open() uAMQP API] 
				if (messagesender_open(transport_state->message_sender) != RESULT_OK)
				{
					// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_073: [IoTHubTransportuAMQP_DoWork shall fail and return immediately if the uAMQP message sender instance fails to be opened, flagging the connection to be re-established] 
					LogError("Failed opening the uAMQP message sender.\r\n");
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

static int destroyMessageReceiver(UAMQP_TRANSPORT_INSTANCE* transport_state)
{
	int result = RESULT_FAILURE;

	if (transport_state->message_receiver != NULL)
	{
		if (messagereceiver_close(transport_state->message_receiver) != RESULT_OK)
		{
			LogError("Failed closing the uAMQP message receiver.\r\n");
		}

		messagereceiver_destroy(transport_state->message_receiver);

		transport_state->message_receiver = NULL;

		link_destroy(transport_state->receiver_link);

		transport_state->receiver_link = NULL;

		result = RESULT_OK;
	}

	return result;
}

static int createMessageReceiver(UAMQP_TRANSPORT_INSTANCE* transport_state, IOTHUB_CLIENT_LL_HANDLE iothub_client_handle)
{
	int result = RESULT_FAILURE;

	if (transport_state->message_sender == NULL)
	{
		AMQP_VALUE source = NULL;
		AMQP_VALUE target = NULL;

		// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_074: [IoTHubTransportuAMQP_DoWork shall create the AMQP link for receiving messages using ‘source’ as messageReceiveAddress, target as the “ingress-rx”, link name as “receiver-link” and role as ‘role_receiver’] 
		if ((source = messaging_create_source(STRING_c_str(transport_state->messageReceiveAddress))) == NULL)
		{
			LogError("Failed creating uAMQP message receiver source attribute.\r\n");
		}
		else if ((target = messaging_create_target(MESSAGE_RECEIVER_TARGET_ADDRESS)) == NULL)
		{
			LogError("Failed creating uAMQP message receiver target attribute.\r\n");
		}
		else if ((transport_state->receiver_link = link_create(transport_state->session, MESSAGE_RECEIVER_LINK_NAME, role_receiver, source, target)) == NULL)
		{
			// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_075: [If IoTHubTransportuAMQP_DoWork fails to create the AMQP link for receiving messages, the function shall fail and return immediately, flagging the connection to be re-stablished] 
			LogError("Failed creating uAMQP link for message receiver.\r\n");
		}
		// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_076: [IoTHubTransportuAMQP_DoWork shall set the receiver link settle mode as receiver_settle_mode_first] 
		else if (link_set_rcv_settle_mode(transport_state->receiver_link, receiver_settle_mode_first) != RESULT_OK)
		{
			// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_141: [If IoTHubTransportuAMQP_DoWork fails to set the settle mode on the AMQP link for receiving messages, the function shall fail and return immediately, flagging the connection to be re-stablished]
			LogError("Failed setting uAMQP link settle mode for message receiver.\r\n");
		}
		else
		{
			// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_119: [IoTHubTransportuAMQP_DoWork shall apply a default value of 65536 for the parameter ‘Link MAX message size’]
			if (link_set_max_message_size(transport_state->receiver_link, MESSAGE_RECEIVER_MAX_LINK_SIZE) != RESULT_OK)
			{
				LogError("Failed setting uAMQP link max message size for message receiver.\r\n");
			}

			// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_077: [IoTHubTransportuAMQP_DoWork shall create the AMQP message receiver using messagereceiver_create() uAMQP API] 
			if ((transport_state->message_receiver = messagereceiver_create(transport_state->receiver_link, NULL, NULL)) == NULL)
			{
				// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_078: [IoTHubTransportuAMQP_DoWork shall fail and return immediately if the uAMQP message receiver instance fails to be created, flagging the connection to be re-established] 
				LogError("Could not allocate uAMQP message receiver.\r\n");
			}
			else
			{
				// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_079: [IoTHubTransportuAMQP_DoWork shall open the AMQP message receiver using messagereceiver_open() uAMQP API, passing a callback function for handling C2D incoming messages] 
				// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_123: [IoTHubTransportuAMQP_DoWork shall create each uAMQP message_receiver passing the ‘on_message_received’ as the callback function] 
				if (messagereceiver_open(transport_state->message_receiver, on_message_received, (const void*)iothub_client_handle) != RESULT_OK)
				{
					// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_080: [IoTHubTransportuAMQP_DoWork shall fail and return immediately if the uAMQP message receiver instance fails to be opened, flagging the connection to be re-established] 
					LogError("Failed opening the uAMQP message receiver.\r\n");
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

static int sendPendingEvents(UAMQP_TRANSPORT_INSTANCE* transport_state)
{
	int result = RESULT_OK;
	IOTHUB_MESSAGE_LIST* message = NULL;

	while ((message = getNextEventToSend(transport_state)) != NULL)
	{
		result = RESULT_FAILURE;
		
		IOTHUBMESSAGE_CONTENT_TYPE contentType = IoTHubMessage_GetContentType(message->messageHandle);
		const unsigned char* messageContent = NULL;
		size_t messageContentSize = 0;
		MESSAGE_HANDLE amqp_message;
		bool is_message_error = false;
		EVENT_TRACKER* event_tracker;

		// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_086: [IoTHubTransportuAMQP_DoWork shall move queued events to an “in-progress” list right before processing them for sending]
		if ((event_tracker = trackEvent(message, transport_state)) == NULL)
		{
			LogError("Failed tracking the event to be sent.\r\n");
		}
		// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_087: [If the event contains a message of type IOTHUBMESSAGE_BYTEARRAY, IoTHubTransportuAMQP_DoWork shall obtain its char* representation and size using IoTHubMessage_GetByteArray()] 
		else if (contentType == IOTHUBMESSAGE_BYTEARRAY && 
			IoTHubMessage_GetByteArray(message->messageHandle, &messageContent, &messageContentSize) != IOTHUB_MESSAGE_OK)
		{
			LogError("Failed getting the BYTE array representation of the event content to be sent.\r\n");
			is_message_error = true;
		}
		// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_089: [If the event contains a message of type IOTHUBMESSAGE_STRING, IoTHubTransportuAMQP_DoWork shall obtain its char* representation using IoTHubMessage_GetString()] 
		// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_090: [If the event contains a message of type IOTHUBMESSAGE_STRING, IoTHubTransportuAMQP_DoWork shall obtain the size of its char* representation using strlen()] 
		else if (contentType == IOTHUBMESSAGE_STRING &&
			((messageContent = IoTHubMessage_GetString(message->messageHandle)) == NULL || (messageContentSize = strlen(messageContent)) <= 0))
		{
			LogError("Failed getting the STRING representation of the event content to be sent.\r\n");
			is_message_error = true;
		}
		// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_092: [If the event contains a message of type IOTHUBMESSAGE_UNKNOWN, IoTHubTransportuAMQP_DoWork shall remove the event from the in-progress list and invoke the upper layer callback reporting the error] 
		else if (contentType == IOTHUBMESSAGE_UNKNOWN)
		{
			LogError("Cannot send events with content type IOTHUBMESSAGE_UNKNOWN.\r\n");
			is_message_error = true;
		}
		// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_093: [IoTHubTransportuAMQP_DoWork shall create an amqp message using message_create() uAMQP API] 
		else if ((amqp_message = message_create()) == NULL)
		{
			LogError("Failed allocating the AMQP message for sending the event.\r\n");
		}
		else
		{
			BINARY_DATA binary_data = { messageContent, messageContentSize };
			
			// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_095: [IoTHubTransportuAMQP_DoWork shall set the AMQP message body using message_add_body_amqp_data() uAMQP API] 
			if (message_add_body_amqp_data(amqp_message, binary_data) != RESULT_OK)
			{
				LogError("Failed setting the body of the AMQP message.\r\n");
			}
			else
			{
				// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_097: [IoTHubTransportuAMQP_DoWork shall pass the encoded AMQP message to uAMQP for sending (along with on_message_send_complete callback) using messagesender_send()] 
				if (messagesender_send(transport_state->message_sender, amqp_message, on_message_send_complete, (const void*)event_tracker) != RESULT_OK)
				{
					LogError("Failed sending the AMQP message.\r\n");
				}
				else
				{
					result = RESULT_OK;
				}
			}
		}

		if (amqp_message != NULL)
		{
			// It can be destroyed because uAMQP keeps a clone of the message.
			message_destroy(amqp_message);
		}

		if (result != RESULT_OK)
		{
			if (event_tracker == NULL)
			{
				break;
			}
			else
			{
				// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_088: [If IoTHubMessage_GetByteArray() fails, IoTHubTransportuAMQP_DoWork shall remove the event from the in-progress list and invoke the upper layer callback reporting the error] 
				// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_091: [If IoTHubMessage_GetString() fails, IoTHubTransportuAMQP_DoWork shall remove the event from the in-progress list and invoke the upper layer callback reporting the error] 
				if (is_message_error)
				{
					on_message_send_complete(event_tracker, MESSAGE_SEND_ERROR);
				}
				else
				{
					// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_111: [If message_create() fails, IoTHubTransportuAMQP_DoWork notify the failure, roll back the event to waitToSent list and return]
					// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_112: [If message_add_body_amqp_data() fails, IoTHubTransportuAMQP_DoWork notify the failure, roll back the event to waitToSent list and return]
					// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_113: [If messagesender_send() fails, IoTHubTransportuAMQP_DoWork notify the failure, roll back the event to waitToSent list and return]
					rollEventBackToWaitList(event_tracker, transport_state);
					break;
				}
			}
		}
	}

	return result;
}

static bool isSasTokenRefreshRequired(UAMQP_TRANSPORT_INSTANCE* transport_state)
{
	return ((getSecondsSinceEpoch() - transport_state->current_sas_token_create_time) >= (transport_state->sas_token_refresh_time / 1000)) ? true : false;
}

static void prepareForConnectionRetry(UAMQP_TRANSPORT_INSTANCE* transport_state)
{
	destroyConnection(transport_state);
	rollEventsBackToWaitList(transport_state);
}


// API functions

static TRANSPORT_HANDLE IoTHubTransportuAMQP_Create(const IOTHUBTRANSPORT_CONFIG* config)
{
	UAMQP_TRANSPORT_INSTANCE* transport_state = NULL;
	bool cleanup_required = false;
	int deviceIdLength = -1;

	// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_005: [If parameter config (or its fields) is NULL then IoTHubTransportuAMQP_Create shall fail and return NULL.] 
	if (config == NULL || config->upperConfig == NULL || config->waitingToSend == NULL)
	{
		LogError("IoTHub uAMQP client transport null configuration parameter.\r\n");
	}
	// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_006: [IoTHubTransportuAMQP_Create shall fail and return NULL if any fields of the config structure are NULL.]
	else if (config->upperConfig->protocol == NULL)
	{
		LogError("Invalid configuration (NULL protocol detected)\r\n");
	}
	else if (config->upperConfig->deviceId == NULL)
	{
		LogError("Invalid configuration (NULL deviceId detected)\r\n");
	}
	else if (config->upperConfig->deviceKey == NULL)
	{
		LogError("Invalid configuration (NULL deviceKey detected)\r\n");
	}
	else if (config->upperConfig->iotHubName == NULL)
	{
		LogError("Invalid configuration (NULL iotHubName detected)\r\n");
	}
	else if (config->upperConfig->iotHubSuffix == NULL)
	{
		LogError("Invalid configuration (NULL iotHubSuffix detected)\r\n");
	}
	else if (!config->waitingToSend)
	{
		LogError("Invalid configuration (NULL waitingToSend list detected)\r\n");
	}
	// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_008: [IoTHubTransportuAMQP_Create shall fail and return NULL if any config field of type string is zero length.] 
	else if ((deviceIdLength = strlen(config->upperConfig->deviceId)) == 0 ||
		(strlen(config->upperConfig->deviceKey) == 0) ||
		(strlen(config->upperConfig->iotHubName) == 0) ||
		(strlen(config->upperConfig->iotHubSuffix) == 0))
	{
		LogError("Zero-length config parameter (deviceId, deviceKey, iotHubName or iotHubSuffix)\r\n");
	}
	// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_007: [IoTHubTransportuAMQP_Create shall fail and return NULL if the deviceId length is greater than 128.]
	else if (deviceIdLength > 128U)
	{
		LogError("deviceId is too long\r\n");
	}
	// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_134: [IoTHubTransportuAMQP_Create shall fail and return NULL if the combined length of config->iotHubName and config->iotHubSuffix exceeds 254 bytes (RFC1035)]
	else if ((strlen(config->upperConfig->iotHubName) + strlen(config->upperConfig->iotHubSuffix)) > (RFC1035_MAX_FQDN_LENGTH - 1))
	{
		LogError("The lengths of iotHubName and iotHubSuffix together exceed the maximum FQDN length allowed (RFC 1035)\r\n");
	}
	else
	{
		// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_009: [IoTHubTransportuAMQP_Create shall fail and return NULL if memory allocation of the transport’s internal state structure fails.]
		transport_state = (UAMQP_TRANSPORT_INSTANCE*)malloc(sizeof(UAMQP_TRANSPORT_INSTANCE));

		if (transport_state == NULL)
		{
			LogError("Could not allocate AMQP transport state\r\n");
		}
		else
		{
			transport_state->iotHubHostFqdn = NULL;
			transport_state->iotHubPort = DEFAULT_IOTHUB_AMQP_PORT;
			transport_state->deviceKey = NULL;
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

			transport_state->waitingToSend = config->waitingToSend;
			DList_InitializeListHead(&transport_state->inProgress);

			// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_010: [IoTHubTransportuAMQP_Create shall create an immutable string, referred to as iotHubHostFqdn, from the following pieces: config->iotHubName + "." + config->iotHubSuffix.] 
			if ((transport_state->iotHubHostFqdn = concat3Params(config->upperConfig->iotHubName, ".", config->upperConfig->iotHubSuffix)) == NULL)
			{
				LogError("Failed to set transport_state->iotHubHostFqdn.\r\n");
				cleanup_required = true;
			}
			// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_012: [IoTHubTransportuAMQP_Create shall create an immutable string, referred to as devicesPath, from the following parts: host_fqdn + “/devices/” + deviceId.] 
			else if ((transport_state->devicesPath = concat3Params(STRING_c_str(transport_state->iotHubHostFqdn), "/devices/", config->upperConfig->deviceId)) == NULL)
			{
				// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_013: [If creating devicesPath fails for any reason then IoTHubTransportuAMQP_Create shall fail and return NULL.] 
				LogError("Failed to allocate transport_state->devicesPath.\r\n");
				cleanup_required = true;
			}
			// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_014: [IoTHubTransportuAMQP_Create shall create an immutable string, referred to as targetAddress, from the following parts: "amqps://" + devicesPath + "/messages/events".]
			else if ((transport_state->targetAddress = concat3Params("amqps://", STRING_c_str(transport_state->devicesPath), "/messages/events")) == NULL)
			{
				// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_015: [If creating the targetAddress fails for any reason then IoTHubTransportuAMQP_Create shall fail and return NULL.] 
				LogError("Failed to allocate transport_state->targetAddress.\r\n");
				cleanup_required = true;
			}
			// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_053: [IoTHubTransportuAMQP_Create shall define the source address for receiving messages as "amqps://" + devicesPath + "/messages/devicebound", stored in the transport handle as messageReceiveAddress]
			else if ((transport_state->messageReceiveAddress = concat3Params("amqps://", STRING_c_str(transport_state->devicesPath), "/messages/devicebound")) == NULL)
			{
				// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_054: [If creating the messageReceiveAddress fails for any reason then IoTHubTransportuAMQP_Create shall fail and return NULL.]
				LogError("Failed to allocate transport_state->messageReceiveAddress.\r\n");
				cleanup_required = true;
			}
			// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_016: [IoTHubTransportuAMQP_Create shall initialize handle->sasTokenKeyName with a zero-length STRING_HANDLE instance.] 
			else if ((transport_state->sasTokenKeyName = STRING_new()) == NULL)
			{
				// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_017: [If IoTHubTransportuAMQP_Create fails to initialize handle->sasTokenKeyName with a zero-length STRING the function shall fail and return NULL.] 
				LogError("Failed to allocate transport_state->sasTokenKeyName.\r\n");
				cleanup_required = true;
			}
			// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_018: [IoTHubTransportuAMQP_Create shall store a copy of config->deviceKey (passed by upper layer) into the transport’s own deviceKey field] 
			else if ((transport_state->deviceKey = STRING_new()) == NULL ||
					STRING_copy(transport_state->deviceKey, config->upperConfig->deviceKey) != 0)
			{
				// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_019: [If IoTHubTransportuAMQP_Create fails to copy config->deviceKey, the function shall fail and return NULL.]
				LogError("Failed to allocate transport_state->deviceKey.\r\n");
				cleanup_required = true;
			}
			else 
			{
				// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_020: [IoTHubTransportuAMQP_Create shall set parameter transport_state->sas_token_lifetime with the default value of 3600000 (milliseconds).]
				transport_state->sas_token_lifetime = DEFAULT_SAS_TOKEN_LIFETIME_MS;

				// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_128: [IoTHubTransportuAMQP_Create shall set parameter transport_state->sas_token_refresh_time with the default value of sas_token_lifetime/2 (milliseconds).] 
				transport_state->sas_token_refresh_time = transport_state->sas_token_lifetime / 2;

				// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_129 : [IoTHubTransportuAMQP_Create shall set parameter transport_state->cbs_request_timeout with the default value of 30000 (milliseconds).]
				transport_state->cbs_request_timeout = DEFAULT_CBS_REQUEST_TIMEOUT_MS;

				// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_130 : [IoTHubTransportuAMQP_Create shall set parameter transport_state->message_send_timeout with the default value of 300000 (milliseconds).]
				transport_state->message_send_timeout = DEFAULT_MESSAGE_SEND_TIMEOUT_MS;

				// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_132: [If config->upperConfig->io_transport_provider_callback is not NULL, IoTHubTransportuAMQP_Create shall set transport_state->io_transport_provider_callback to config->upperConfig->io_transport_provider_callback]
				if (config->upperConfig->io_transport_provider_callback != NULL)
				{
					transport_state->io_transport_provider_callback = config->upperConfig->io_transport_provider_callback;
				}
				// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_125: [If config->upperConfig->io_transport_provider_callback is NULL, IoTHubTransportuAMQP_Create shall set it to default_io_transport_provider] 
				else
				{
					transport_state->io_transport_provider_callback = default_io_transport_provider;
				}
			}
		}
	}

	if (cleanup_required)
	{
		if (transport_state->deviceKey != NULL)
			STRING_delete(transport_state->deviceKey);
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

	// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_023: [If IoTHubTransportuAMQP_Create succeeds it shall return a non-NULL pointer to the structure that represents the transport.] 
	return transport_state;
}

static void IoTHubTransportuAMQP_Destroy(TRANSPORT_HANDLE handle)
{
	if (handle != NULL)
	{
		UAMQP_TRANSPORT_INSTANCE* transport_state = (UAMQP_TRANSPORT_INSTANCE*)handle;
		
		// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_024: [IoTHubTransportuAMQP_Destroy shall destroy the uAMQP message_sender.]
		// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_029 : [IoTHubTransportuAMQP_Destroy shall destroy the uAMQP link.]
		destroyEventSender(transport_state);

		// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_025: [IoTHubTransportuAMQP_Destroy shall destroy the uAMQP message_receiver.] 
		// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_029 : [IoTHubTransportuAMQP_Destroy shall destroy the uAMQP link.]
		destroyMessageReceiver(transport_state);

		// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_027 : [IoTHubTransportuAMQP_Destroy shall destroy the uAMQP cbs instance]
		// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_030 : [IoTHubTransportuAMQP_Destroy shall destroy the uAMQP session.]
		// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_031 : [IoTHubTransportuAMQP_Destroy shall destroy the uAMQP connection.]
		// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_032 : [IoTHubTransportuAMQP_Destroy shall destroy the uAMQP SASL I / O transport.]
		// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_033 : [IoTHubTransportuAMQP_Destroy shall destroy the uAMQP SASL mechanism.]
		destroyConnection(transport_state);

		// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_035 : [IoTHubTransportuAMQP_Destroy shall delete its internally - set parameters(deviceKey, targetAddress, devicesPath, sasTokenKeyName).]
		STRING_delete(transport_state->targetAddress);
		STRING_delete(transport_state->messageReceiveAddress);
		STRING_delete(transport_state->sasTokenKeyName);
		STRING_delete(transport_state->deviceKey);
		STRING_delete(transport_state->devicesPath);

		// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_036 : [IoTHubTransportuAMQP_Destroy shall return the remaining items in inProgress to waitingToSend list.]
		rollEventsBackToWaitList(transport_state);

		// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_150: [IoTHubTransportuAMQP_Destroy shall destroy the transport instance]
		free(transport_state);
	}
}

static void IoTHubTransportuAMQP_DoWork(TRANSPORT_HANDLE handle, IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle)
{
	// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_051: [IoTHubTransportuAMQP_DoWork shall fail and return immediately if the transport handle parameter is NULL] 
	if (handle == NULL)
	{
		LogError("IoTHubClient DoWork failed: transport handle parameter is NULL.\r\n");
	}
	// Codes_[IoTHubTransportuAMQP_DoWork shall fail and return immediately if the client handle parameter is NULL] 
	else if (iotHubClientHandle == NULL)
	{
		LogError("IoTHubClient DoWork failed: client handle parameter is NULL.\r\n");
	}
	else
	{
		bool trigger_connection_retry = false;
		UAMQP_TRANSPORT_INSTANCE* transport_state = (UAMQP_TRANSPORT_INSTANCE*)handle;

		// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_147: [IoTHubTransportuAMQP_DoWork shall save a reference to the client handle in transport_state->iothub_client_handle]
		transport_state->iothub_client_handle = iotHubClientHandle;

		// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_055: [If the transport handle has a NULL connection, IoTHubTransportuAMQP_DoWork shall instantiate and initialize the uAMQP components and establish the connection] 
		if (transport_state->connection == NULL &&
			establishConnection(transport_state) != RESULT_OK)
		{
			LogError("uAMQP transport failed to establish connection with service.\r\n");
			trigger_connection_retry = true;
		}
		// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_081: [IoTHubTransportuAMQP_DoWork shall put a new SAS token if the one has not been out already, or if the previous one failed to be put due to timeout of cbs_put_token().]
		// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_082: [IoTHubTransportuAMQP_DoWork shall refresh the SAS token if the current token has been used for more than ‘sas_token_refresh_time’ milliseconds]
		else if ((transport_state->cbs_state == CBS_STATE_IDLE || isSasTokenRefreshRequired(transport_state)) &&
				startAuthentication(transport_state) != RESULT_OK)
		{
			// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_146: [If the SAS token fails to be sent to CBS (cbs_put_token), IoTHubTransportuAMQP_DoWork shall fail and exit immediately]
			LogError("Failed authenticating AMQP connection within CBS.\r\n");
			trigger_connection_retry = true;
		}
		// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_084: [IoTHubTransportuAMQP_DoWork shall wait for ‘cbs_request_timeout’ milliseconds for the cbs_put_token() to complete before failing due to timeout]
		else if (transport_state->cbs_state == CBS_STATE_AUTH_IN_PROGRESS &&  
				verifyAuthenticationTimeout(transport_state) == RESULT_TIMEOUT)
		{
			LogError("uAMQP transport authentication timed out.\r\n");
			trigger_connection_retry = true;
		}
		else if (transport_state->cbs_state == CBS_STATE_AUTHENTICATED)
		{
			// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_121: [IoTHubTransportuAMQP_DoWork shall create an uAMQP message_receiver if transport_state->message_receive is NULL and transport_state->receive_messages is true] 
			if (transport_state->receive_messages == true &&
				transport_state->message_receiver == NULL &&
				createMessageReceiver(transport_state, iotHubClientHandle) != RESULT_OK)
			{
				LogError("Failed creating uAMQP transport message receiver.\r\n");
				trigger_connection_retry = true;
			}
			// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_122: [IoTHubTransportuAMQP_DoWork shall destroy the transport_state->message_receiver (and set it to NULL) if it exists and transport_state->receive_messages is false] 
			else if (transport_state->receive_messages == false &&
				transport_state->message_receiver != NULL &&
				destroyMessageReceiver(transport_state) != RESULT_OK)
			{
				LogError("Failed destroying uAMQP transport message receiver.\r\n");
			}

			if (transport_state->message_sender == NULL &&
				createEventSender(transport_state) != RESULT_OK)
			{
				LogError("Failed creating uAMQP transport event sender.\r\n");
				trigger_connection_retry = true;
			}
			else if (sendPendingEvents(transport_state) != RESULT_OK)
			{
				LogError("uAMQP transport failed sending events.\r\n");
				trigger_connection_retry = true;
			}
		}

		if (trigger_connection_retry)
		{
			prepareForConnectionRetry(transport_state);
		}
		else
		{
			// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_103: [IoTHubTransportuAMQP_DoWork shall invoke connection_dowork() on uAMQP for triggering sending and receiving messages] 
			connection_dowork(transport_state->connection);
		}

		handleEventSendTimeouts(transport_state);
	}
}

static int IoTHubTransportuAMQP_Subscribe(TRANSPORT_HANDLE handle)
{
	int result;
	
	// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_037: [IoTHubTransportuAMQP_Subscribe shall fail if the transport handle parameter received is NULL.] 
    if (handle == NULL)
    {
        LogError("Invalid handle to IoTHubClient uAMQP transport.\r\n");
        result = __LINE__;
    }
    else
    {
		// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_038: [IoTHubTransportuAMQP_Subscribe shall set transport_handle->receive_messages to true and return success code.]
		UAMQP_TRANSPORT_INSTANCE* transport_state = (UAMQP_TRANSPORT_INSTANCE*)handle;
		transport_state->receive_messages = true;
        result = 0;
    }

    return result;
}

static void IoTHubTransportuAMQP_Unsubscribe(TRANSPORT_HANDLE handle)
{
	// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_039: [IoTHubTransportuAMQP_Unsubscribe shall fail if the transport handle parameter received is NULL.] 
	if (handle == NULL)
	{
		LogError("Invalid handle to IoTHubClient uAMQP transport.\r\n");
	}
	else
	{
		// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_040: [IoTHubTransportuAMQP_Unsubscribe shall set transport_handle->receive_messages to false and return success code.]
		UAMQP_TRANSPORT_INSTANCE* transport_state = (UAMQP_TRANSPORT_INSTANCE*)handle;
		transport_state->receive_messages = false;
	}
}

static IOTHUB_CLIENT_RESULT IoTHubTransportuAMQP_GetSendStatus(TRANSPORT_HANDLE handle, IOTHUB_CLIENT_STATUS *iotHubClientStatus)
{
    IOTHUB_CLIENT_RESULT result;

	// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_041: [IoTHubTransportuAMQP_GetSendStatus shall return IOTHUB_CLIENT_INVALID_ARG if called with NULL parameter.]
    if (handle == NULL)
    {
        result = IOTHUB_CLIENT_INVALID_ARG;
        LogError("Invalid handle to IoTHubClient uAMQP transport instance.\r\n");
    }
    else if (iotHubClientStatus == NULL)
    {
        result = IOTHUB_CLIENT_INVALID_ARG;
        LogError("Invalid pointer to output parameter IOTHUB_CLIENT_STATUS.\r\n");
    }
    else
    {
		UAMQP_TRANSPORT_INSTANCE* transport_state = (UAMQP_TRANSPORT_INSTANCE*)handle;

		// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_043: [IoTHubTransportuAMQP_GetSendStatus shall return IOTHUB_CLIENT_OK and status IOTHUB_CLIENT_SEND_STATUS_BUSY if there are currently event items to be sent or being sent.]
		if (!DList_IsListEmpty(transport_state->waitingToSend) || !DList_IsListEmpty(&(transport_state->inProgress)))
		{
			*iotHubClientStatus = IOTHUB_CLIENT_SEND_STATUS_BUSY;
		}
		// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_042: [IoTHubTransportuAMQP_GetSendStatus shall return IOTHUB_CLIENT_OK and status IOTHUB_CLIENT_SEND_STATUS_IDLE if there are currently no event items to be sent or being sent.] 
		else
		{
			*iotHubClientStatus = IOTHUB_CLIENT_SEND_STATUS_IDLE;
		}

		result = IOTHUB_CLIENT_OK;
    }

    return result;
}

static IOTHUB_CLIENT_RESULT IoTHubTransportuAMQP_SetOption(TRANSPORT_HANDLE handle, const char* option, const void* value)
{
	IOTHUB_CLIENT_RESULT result;

	// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_044: [If handle parameter is NULL then IoTHubTransportuAMQP_SetOption shall return IOTHUB_CLIENT_INVALID_ARG.]
	// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_045: [If parameter optionName is NULL then IoTHubTransportuAMQP_SetOption shall return IOTHUB_CLIENT_INVALID_ARG.] 
	// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_046: [If parameter value is NULL then IoTHubTransportuAMQP_SetOption shall return IOTHUB_CLIENT_INVALID_ARG.]
	if (
		(handle == NULL) ||
		(option == NULL) ||
		(value == NULL)
		)
	{
		result = IOTHUB_CLIENT_INVALID_ARG;
		LogError("Invalid parameter (NULL) passed to uAMQP transport SetOption()\r\n");
	}
	else
	{
		UAMQP_TRANSPORT_INSTANCE* transport_state = (UAMQP_TRANSPORT_INSTANCE*)handle;

		if (strcmp("trusted_certs", option) == 0)
		{
			result = IOTHUB_CLIENT_ERROR;
			LogError("Invalid option (trusted_certs) passed to uAMQP transport SetOption() (not implemented)\r\n");
		}
		// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_048: [IoTHubTransportuAMQP_SetOption shall save and apply the value if the option name is "sas_token_lifetime", returning IOTHUB_CLIENT_OK] 
		else if (strcmp("sas_token_lifetime", option) == 0)
		{
			transport_state->sas_token_lifetime = *((size_t*)value);
			result = IOTHUB_CLIENT_OK;
		}
		// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_049: [IoTHubTransportuAMQP_SetOption shall save and apply the value if the option name is "sas_token_refresh_time", returning IOTHUB_CLIENT_OK] 
		else if (strcmp("sas_token_refresh_time", option) == 0)
		{
			transport_state->sas_token_refresh_time = *((size_t*)value);
			result = IOTHUB_CLIENT_OK;
		}
		// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_148: [IoTHubTransportuAMQP_SetOption shall save and apply the value if the option name is "cbs_request_timeout", returning IOTHUB_CLIENT_OK] 
		else if (strcmp("cbs_request_timeout", option) == 0)
		{
			transport_state->cbs_request_timeout = *((size_t*)value);
			result = IOTHUB_CLIENT_OK;
		}
		// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_149: [IoTHubTransportuAMQP_SetOption shall save and apply the value if the option name is "message_send_timeout", returning IOTHUB_CLIENT_OK]
		else if (strcmp("message_send_timeout", option) == 0)
		{
			transport_state->message_send_timeout = *((size_t*)value);
			result = IOTHUB_CLIENT_OK;
		}
		// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_047: [If optionName is not an option supported then IoTHubTransportuAMQP_SetOption shall return IOTHUB_CLIENT_INVALID_ARG.] 
		else
		{
			result = IOTHUB_CLIENT_INVALID_ARG;
			LogError("Invalid option (%s) passed to uAMQP transport SetOption()\r\n", option);
		}
	}

    return result;
}

static TRANSPORT_PROVIDER thisTransportProvider = {
    IoTHubTransportuAMQP_SetOption, 
    IoTHubTransportuAMQP_Create, 
	IoTHubTransportuAMQP_Destroy, 
	IoTHubTransportuAMQP_Subscribe, 
	IoTHubTransportuAMQP_Unsubscribe, 
	IoTHubTransportuAMQP_DoWork, 
	IoTHubTransportuAMQP_GetSendStatus
};

extern const void* uAMQP_Protocol(void)
{
    return &thisTransportProvider;
}
