// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#include <stdint.h>
#include <time.h>
#include <limits.h>
#include "azure_c_shared_utility/agenttime.h"
#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_c_shared_utility/doublylinkedlist.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/platform.h"
#include "azure_c_shared_utility/strings.h"
#include "azure_c_shared_utility/urlencode.h"
#include "azure_c_shared_utility/tlsio.h"
#include "azure_c_shared_utility/vector.h"

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
#include "iothubtransportamqp_auth.h"
#include "iothubtransportamqp_methods.h"
#include "iothubtransport_amqp_common.h"
#include "iothub_client_version.h"

#define RESULT_OK 0

#define INDEFINITE_TIME ((time_t)(-1))
#define RFC1035_MAX_FQDN_LENGTH 255
#define DEFAULT_SAS_TOKEN_LIFETIME_MS 3600000
#define DEFAULT_CBS_REQUEST_TIMEOUT_MS 30000
#define DEFAULT_CONTAINER_ID "default_container_id"
#define DEFAULT_INCOMING_WINDOW_SIZE UINT_MAX
#define DEFAULT_OUTGOING_WINDOW_SIZE 100
#define MESSAGE_RECEIVER_LINK_NAME_TAG "receiver"
#define MESSAGE_RECEIVER_TARGET_ADDRESS "target"
#define MESSAGE_RECEIVER_MAX_LINK_SIZE 65536
#define MESSAGE_SENDER_LINK_NAME_TAG "sender"
#define MESSAGE_SENDER_SOURCE_NAME_TAG "source"
#define MESSAGE_SENDER_MAX_LINK_SIZE UINT64_MAX

typedef enum RESULT_TAG
{
	RESULT_SUCCESS,
	RESULT_INVALID_ARGUMENT,
	RESULT_TIME_OUT,
	RESULT_RETRYABLE_ERROR,
	RESULT_CRITICAL_ERROR
} RESULT;

typedef struct AMQP_TRANSPORT_STATE_TAG
{
    // FQDN of the IoT Hub.
    STRING_HANDLE iotHubHostFqdn;

    // TSL I/O transport.
    XIO_HANDLE tls_io;
    // Pointer to the function that creates the TLS I/O (internal use only).
	AMQP_GET_IO_TRANSPORT underlying_io_transport_provider;
    // AMQP connection.
    CONNECTION_HANDLE connection;
    // AMQP session.
    SESSION_HANDLE session;
    // All things CBS (and only CBS)
    AMQP_TRANSPORT_CBS_CONNECTION cbs_connection;

	// Current AMQP connection state;
	AMQP_MANAGEMENT_STATE connection_state;

	AMQP_TRANSPORT_CREDENTIAL_TYPE preferred_credential_type;
	// List of registered devices.
	VECTOR_HANDLE registered_devices;
    // Turns logging on and off
    bool is_trace_on;
	// Used to generate unique AMQP link names
	int link_count;

    /*here are the options from the xio layer if any is saved*/
    OPTIONHANDLER_HANDLE xioOptions;
} AMQP_TRANSPORT_INSTANCE;

typedef struct AMQP_TRANSPORT_DEVICE_STATE_TAG
{
	// Identity of the device.
	STRING_HANDLE deviceId;
	// contains the credentials to be used
	AUTHENTICATION_STATE_HANDLE authentication;

	// Address to which the transport will connect to and send events.
	STRING_HANDLE targetAddress;
	// Address to which the transport will connect to and receive messages from.
	STRING_HANDLE messageReceiveAddress;
	// Internal parameter that identifies the current logical device within the service.
	STRING_HANDLE devicesPath;
	// Saved reference to the IoTHub LL Client.
	IOTHUB_CLIENT_LL_HANDLE iothub_client_handle;
	// Saved reference to the transport the device is registered on.
	AMQP_TRANSPORT_INSTANCE* transport_state;
	// AMQP link used by the event sender.
	LINK_HANDLE sender_link;
	// uAMQP event sender.
	MESSAGE_SENDER_HANDLE message_sender;
	// State of the message sender.
	MESSAGE_SENDER_STATE message_sender_state;
	// Internal flag that controls if messages should be received or not.
	bool receive_messages;
	// AMQP link used by the message receiver.
	LINK_HANDLE receiver_link;
	// uAMQP message receiver.
	MESSAGE_RECEIVER_HANDLE message_receiver;
	// Message receiver state.
	MESSAGE_RECEIVER_STATE message_receiver_state;
	// List with events still pending to be sent. It is provided by the upper layer.
	PDLIST_ENTRY waitingToSend;
	// Internal list with the items currently being processed/sent through uAMQP.
	DLIST_ENTRY inProgress;
    // the methods portion
    IOTHUBTRANSPORT_AMQP_METHODS_HANDLE methods_handle;
    // is subscription for methods needed?
    int subscribe_methods_needed : 1;
    // is the transport subscribed for methods?
    int subscribed_for_methods : 1;
} AMQP_TRANSPORT_DEVICE_STATE;


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

static STRING_HANDLE create_link_name(const char* deviceId, const char* tag, int index)
{
	STRING_HANDLE name = NULL;
	char name_str[1024];

	if (sprintf(name_str, "link-%s-%s-%i", deviceId, tag, index) <= 0)
	{
		LogError("create_link_name failed (sprintf failed)");
	}
	else if ((name = STRING_construct(name_str)) == NULL)
	{
		LogError("create_link_name failed (STRING_construct failed)");
	}

	return name;
}

static STRING_HANDLE create_link_source_name(STRING_HANDLE link_name)
{
	STRING_HANDLE name = NULL;
	char name_str[1024];

	if (sprintf(name_str, "%s-source", STRING_c_str(link_name)) <= 0)
	{
		LogError("create_link_source_name failed (sprintf failed)");
	}
	else if ((name = STRING_construct(name_str)) == NULL)
	{
		LogError("create_link_source_name failed (STRING_construct failed)");
	}

	return name;
}

static STRING_HANDLE create_link_target_name(STRING_HANDLE link_name)
{
	STRING_HANDLE name = NULL;
	char name_str[1024];

	if (sprintf(name_str, "%s-target", STRING_c_str(link_name)) <= 0)
	{
		LogError("create_link_target_name failed (sprintf failed)");
	}
	else if ((name = STRING_construct(name_str)) == NULL)
	{
		LogError("create_link_target_name failed (STRING_construct failed)");
	}

	return name;
}

// Auxiliary function to be used on VECTOR_find_if()
static bool findDeviceById(const void* element, const void* value)
{
	const AMQP_TRANSPORT_DEVICE_STATE* device_state = *(const AMQP_TRANSPORT_DEVICE_STATE **)element;
	const char* deviceId = (const char *)value;

	return (strcmp(STRING_c_str(device_state->deviceId), deviceId) == 0);
}

static void trackEventInProgress(IOTHUB_MESSAGE_LIST* message, AMQP_TRANSPORT_DEVICE_STATE* device_state)
{
    DList_RemoveEntryList(&message->entry);
    DList_InsertTailList(&device_state->inProgress, &message->entry);
}

static IOTHUB_MESSAGE_LIST* getNextEventToSend(AMQP_TRANSPORT_DEVICE_STATE* device_state)
{
    IOTHUB_MESSAGE_LIST* message;

    if (!DList_IsListEmpty(device_state->waitingToSend))
    {
        PDLIST_ENTRY list_entry = device_state->waitingToSend->Flink;
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

static void rollEventBackToWaitList(IOTHUB_MESSAGE_LIST* message, AMQP_TRANSPORT_DEVICE_STATE* device_state)
{
    removeEventFromInProgressList(message);
    DList_InsertTailList(device_state->waitingToSend, &message->entry);
}

static void rollEventsBackToWaitList(AMQP_TRANSPORT_DEVICE_STATE* device_state)
{
    PDLIST_ENTRY entry = device_state->inProgress.Blink;

    while (entry != &device_state->inProgress)
    {
        IOTHUB_MESSAGE_LIST* message = containingRecord(entry, IOTHUB_MESSAGE_LIST, entry);
        entry = entry->Blink;
        rollEventBackToWaitList(message, device_state);
    }
}

static void on_message_send_complete(void* context, MESSAGE_SEND_RESULT send_result)
{
    IOTHUB_MESSAGE_LIST* message = (IOTHUB_MESSAGE_LIST*)context;

    IOTHUB_CLIENT_RESULT iot_hub_send_result;

    // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_142: [The callback 'on_message_send_complete' shall pass to the upper layer callback an IOTHUB_CLIENT_CONFIRMATION_OK if the result received is MESSAGE_SEND_OK] 
    if (send_result == MESSAGE_SEND_OK)
    {
        iot_hub_send_result = IOTHUB_CLIENT_CONFIRMATION_OK;
    }
    // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_143: [The callback 'on_message_send_complete' shall pass to the upper layer callback an IOTHUB_CLIENT_CONFIRMATION_ERROR if the result received is MESSAGE_SEND_ERROR]
    else
    {
        iot_hub_send_result = IOTHUB_CLIENT_CONFIRMATION_ERROR;
    }

    // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_102: [The callback 'on_message_send_complete' shall invoke the upper layer callback for message received if provided] 
    if (message->callback != NULL)
    {
        message->callback(iot_hub_send_result, message->context);
    }

    // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_100: [The callback 'on_message_send_complete' shall remove the target message from the in-progress list after the upper layer callback] 
    if (isEventInInProgressList(message))
    {
        removeEventFromInProgressList(message);
    }

    // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_151: [The callback 'on_message_send_complete' shall destroy the message handle (IOTHUB_MESSAGE_HANDLE) using IoTHubMessage_Destroy()]
    IoTHubMessage_Destroy(message->messageHandle);

    // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_152: [The callback 'on_message_send_complete' shall destroy the IOTHUB_MESSAGE_LIST instance]
    free(message); 
}

static AMQP_VALUE on_message_received(const void* context, MESSAGE_HANDLE message)
{
    AMQP_VALUE result = NULL;
    int api_call_result;
    IOTHUB_MESSAGE_HANDLE iothub_message = NULL;
    // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_195: [The callback 'on_message_received' shall shall get a IOTHUB_MESSAGE_HANDLE instance out of the uamqp's MESSAGE_HANDLE instance by using IoTHubMessage_CreateFromUamqpMessage()]
    if ((api_call_result = IoTHubMessage_CreateFromUamqpMessage(message, &iothub_message)) != RESULT_OK)
    {
        LogError("Transport failed processing the message received (error = %d).", api_call_result);

        // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_196: [If IoTHubMessage_CreateFromUamqpMessage fails, the callback 'on_message_received' shall reject the incoming message by calling messaging_delivery_rejected() and return.]
        result = messaging_delivery_rejected("Rejected due to failure reading AMQP message", "Failed reading AMQP message");
    }
    else
    {
        IOTHUBMESSAGE_DISPOSITION_RESULT disposition_result;

        // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_104: [The callback 'on_message_received' shall invoke IoTHubClient_LL_MessageCallback() passing the client and the incoming message handles as parameters] 
        disposition_result = IoTHubClient_LL_MessageCallback((IOTHUB_CLIENT_LL_HANDLE)context, iothub_message);

        // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_197: [The callback 'on_message_received' shall destroy the IOTHUB_MESSAGE_HANDLE instance after invoking IoTHubClient_LL_MessageCallback().]
        IoTHubMessage_Destroy(iothub_message);

        // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_105: [The callback 'on_message_received' shall return the result of messaging_delivery_accepted() if the IoTHubClient_LL_MessageCallback() returns IOTHUBMESSAGE_ACCEPTED] 
        if (disposition_result == IOTHUBMESSAGE_ACCEPTED)
        {
            result = messaging_delivery_accepted();
        }
        // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_106: [The callback 'on_message_received' shall return the result of messaging_delivery_released() if the IoTHubClient_LL_MessageCallback() returns IOTHUBMESSAGE_ABANDONED] 
        else if (disposition_result == IOTHUBMESSAGE_ABANDONED)
        {
            result = messaging_delivery_released();
        }
        // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_107: [The callback 'on_message_received' shall return the result of messaging_delivery_rejected("Rejected by application", "Rejected by application") if the IoTHubClient_LL_MessageCallback() returns IOTHUBMESSAGE_REJECTED] 
        else if (disposition_result == IOTHUBMESSAGE_REJECTED)
        {
            result = messaging_delivery_rejected("Rejected by application", "Rejected by application");
        }
    }

    return result;
}

static void destroyConnection(AMQP_TRANSPORT_INSTANCE* transport_state)
{
    if (transport_state->cbs_connection.cbs_handle != NULL)
    {
        cbs_destroy(transport_state->cbs_connection.cbs_handle);
        transport_state->cbs_connection.cbs_handle = NULL;
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

    if (transport_state->cbs_connection.sasl_io != NULL)
    {
        xio_destroy(transport_state->cbs_connection.sasl_io);
        transport_state->cbs_connection.sasl_io = NULL;
    }

    if (transport_state->cbs_connection.sasl_mechanism != NULL)
    {
        saslmechanism_destroy(transport_state->cbs_connection.sasl_mechanism);
        transport_state->cbs_connection.sasl_mechanism = NULL;
    }

    if (transport_state->tls_io != NULL)
    {
        /*before destroying, we shall save its options for later use*/
        transport_state->xioOptions = xio_retrieveoptions(transport_state->tls_io);
        if (transport_state->xioOptions == NULL)
        {
            LogError("unable to retrieve xio_retrieveoptions");
        }
        
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

static void on_methods_error(void* context)
{
    /* Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_030: [ `on_methods_error` shall do nothing. ]*/
    (void)context;
}

static int on_method_request_received(void* context, const char* method_name, const unsigned char* request, size_t request_size, IOTHUBTRANSPORT_AMQP_METHOD_HANDLE method_handle)
{
    int result;

    /* Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_016: [ `on_methods_request_received` shall create a BUFFER_HANDLE by calling `BUFFER_new`. ]*/
    BUFFER_HANDLE response_buffer = BUFFER_new();

    if (response_buffer == NULL)
    {
        /* Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_025: [ If creating the buffer fails, `on_methods_request_received` shall fail and return a non-zero value. ]*/
        LogError("Could not allocate buffer");
        result = __LINE__;
    }
    else
    {
        AMQP_TRANSPORT_DEVICE_STATE* device_state = (AMQP_TRANSPORT_DEVICE_STATE*)context;

        /* Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_017: [ `on_methods_request_received` shall call the `IoTHubClient_LL_DeviceMethodComplete` passing the method name, request buffer and size and the newly created BUFFER handle. ]*/
        /* Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_022: [ The status code shall be the return value of the call to `IoTHubClient_LL_DeviceMethodComplete`. ]*/
        int status = IoTHubClient_LL_DeviceMethodComplete(device_state->iothub_client_handle, method_name, request, request_size, response_buffer);

        /* Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_020: [ The response bytes shall be obtained by calling `BUFFER_u_char`. ]*/
        const unsigned char* response_payload = BUFFER_u_char(response_buffer);

        /* Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_021: [ The response size shall be obtained by calling `BUFFER_length`. ]*/
        size_t response_size = BUFFER_length(response_buffer);

        /* Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_019: [ `on_methods_request_received` shall call `iothubtransportamqp_methods_respond` passing to it the `method_handle` argument, the response bytes, response size and the status code. ]*/
        if (iothubtransportamqp_methods_respond(method_handle, response_payload, response_size, status) != 0)
        {
            /* Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_029: [ If `iothubtransportamqp_methods_respond` fails, `on_methods_request_received` shall return a non-zero value. ]*/
            LogError("iothubtransportamqp_methods_respond failed");
            result = __LINE__;
        }
        else
        {
            /* Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_028: [ On success, `on_methods_request_received` shall return 0. ]*/
            result = 0;
        }

        /* Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_023: [ After calling `iothubtransportamqp_methods_respond`, the allocated buffer shall be freed by using BUFFER_delete. ]*/
        BUFFER_delete(response_buffer);
    }

    return result;
}

static int subscribe_methods(AMQP_TRANSPORT_DEVICE_STATE* deviceState)
{
    int result;

    if (deviceState->subscribe_methods_needed == 0)
    {
        result = 0;
    }
    else
    {
        /* Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_024: [ If the device authentication status is AUTHENTICATION_STATUS_OK and `IoTHubTransport_AMQP_Common_Subscribe_DeviceMethod` was called to register for methods, `IoTHubTransport_AMQP_Common_DoWork` shall call `iothubtransportamqp_methods_subscribe`. ]*/
        /* Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_027: [ The current session handle shall be passed to `iothubtransportamqp_methods_subscribe`. ]*/
        if (iothubtransportamqp_methods_subscribe(deviceState->methods_handle, deviceState->transport_state->session, on_methods_error, deviceState, on_method_request_received, deviceState) != 0)
        {
            LogError("Cannot subscribe for methods");
            result = __LINE__;
        }
        else
        {
            deviceState->subscribed_for_methods = 1;
            result = 0;
        }
    }

    return result;
}

static void set_session_options(SESSION_HANDLE session)
{
	// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_065: [IoTHubTransport_AMQP_Common_DoWork shall apply a default value of UINT_MAX for the parameter 'AMQP incoming window'] 
	if (session_set_incoming_window(session, (uint32_t)DEFAULT_INCOMING_WINDOW_SIZE) != 0)
	{
		LogError("Failed to set the AMQP incoming window size.");
	}

	// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_115: [IoTHubTransport_AMQP_Common_DoWork shall apply a default value of 100 for the parameter 'AMQP outgoing window'] 
	if (session_set_outgoing_window(session, DEFAULT_OUTGOING_WINDOW_SIZE) != 0)
	{
		LogError("Failed to set the AMQP outgoing window size.");
	}
}

static int establishConnection(AMQP_TRANSPORT_INSTANCE* transport_state)
{
    int result;

    // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_110: [IoTHubTransport_AMQP_Common_DoWork shall create the TLS IO using transport_state->io_transport_provider callback function] 
    if (transport_state->tls_io == NULL &&
        (transport_state->tls_io = transport_state->underlying_io_transport_provider(STRING_c_str(transport_state->iotHubHostFqdn))) == NULL)
    {
        // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_136: [If transport_state->io_transport_provider_callback fails, IoTHubTransport_AMQP_Common_DoWork shall fail and return immediately]
        result = __LINE__;
        LogError("Failed to obtain a TLS I/O transport layer.");
    }
    else
    {
		// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_239: [IoTHubTransport_AMQP_Common_DoWork shall apply any TLS I/O saved options to the new TLS instance using OptionHandler_FeedOptions]
        if (transport_state->xioOptions != NULL)
        {
            if (OptionHandler_FeedOptions(transport_state->xioOptions, transport_state->tls_io) != 0)
            {
                LogError("unable to replay options to TLS"); /*pessimistically hope TLS will fail, be recreated and options re-given*/
            }
            else
            {
				// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_240: [If OptionHandler_FeedOptions succeeds, IoTHubTransport_AMQP_Common_DoWork shall destroy any TLS options saved on the transport state]
                OptionHandler_Destroy(transport_state->xioOptions);
                transport_state->xioOptions = NULL;
            }
        }

        switch (transport_state->preferred_credential_type)
        {
            case (DEVICE_KEY):
            case (DEVICE_SAS_TOKEN):
            {
                // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_056: [IoTHubTransport_AMQP_Common_DoWork shall create the SASL mechanism using AMQP's saslmechanism_create() API] 
                if ((transport_state->cbs_connection.sasl_mechanism = saslmechanism_create(saslmssbcbs_get_interface(), NULL)) == NULL)
                {
                    // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_057: [If saslmechanism_create() fails, IoTHubTransport_AMQP_Common_DoWork shall fail and return immediately]
                    result = __LINE__;
                    LogError("Failed to create a SASL mechanism.");
                }
                else
                {
                    // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_060: [IoTHubTransport_AMQP_Common_DoWork shall create the SASL I / O layer using the xio_create() C Shared Utility API]
                    SASLCLIENTIO_CONFIG sasl_client_config;
                    sasl_client_config.sasl_mechanism = transport_state->cbs_connection.sasl_mechanism;
                    sasl_client_config.underlying_io = transport_state->tls_io;
                    if ((transport_state->cbs_connection.sasl_io = xio_create(saslclientio_get_interface_description(), &sasl_client_config)) == NULL)
                    {
                        // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_061: [If xio_create() fails creating the SASL I/O layer, IoTHubTransport_AMQP_Common_DoWork shall fail and return immediately] 
                        result = __LINE__;
                        LogError("Failed to create a SASL I/O layer.");
                    }
                    // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_062: [IoTHubTransport_AMQP_Common_DoWork shall create the connection with the IoT service using connection_create2() AMQP API, passing the SASL I/O layer, IoT Hub FQDN and container ID as parameters (pass NULL for callbacks)] 
                    else if ((transport_state->connection = connection_create2(transport_state->cbs_connection.sasl_io, STRING_c_str(transport_state->iotHubHostFqdn), DEFAULT_CONTAINER_ID, NULL, NULL, NULL, NULL, on_connection_io_error, (void*)transport_state)) == NULL)
                    {
                        // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_063: [If connection_create2() fails, IoTHubTransport_AMQP_Common_DoWork shall fail and return immediately.] 
                        result = __LINE__;
                        LogError("Failed to create the AMQP connection.");
                    }
                    // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_137: [IoTHubTransport_AMQP_Common_DoWork shall create the AMQP session session_create() AMQP API, passing the connection instance as parameter]
                    else if ((transport_state->session = session_create(transport_state->connection, NULL, NULL)) == NULL)
                    {
                        // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_138 : [If session_create() fails, IoTHubTransport_AMQP_Common_DoWork shall fail and return immediately]
                        result = __LINE__;
                        LogError("Failed to create the AMQP session.");
                    }
                    else
                    {
						set_session_options(transport_state->session);

                        // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_066: [IoTHubTransport_AMQP_Common_DoWork shall establish the CBS connection using the cbs_create() AMQP API] 
                        if ((transport_state->cbs_connection.cbs_handle = cbs_create(transport_state->session, on_amqp_management_state_changed, NULL)) == NULL)
                        {
                            // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_067: [If cbs_create() fails, IoTHubTransport_AMQP_Common_DoWork shall fail and return immediately] 
                            result = __LINE__;
                            LogError("Failed to create the CBS connection.");
                        }
                        // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_139: [IoTHubTransport_AMQP_Common_DoWork shall open the CBS connection using the cbs_open() AMQP API] 
                        else if (cbs_open(transport_state->cbs_connection.cbs_handle) != 0)
                        {
                            // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_140: [If cbs_open() fails, IoTHubTransport_AMQP_Common_DoWork shall fail and return immediately]
                            result = __LINE__;
                            LogError("Failed to open the connection with CBS.");
                        }
                        else
                        {
							// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_199: [The value of the option `logtrace` saved by the transport instance shall be applied to each new connection instance using connection_set_trace().]
							connection_set_trace(transport_state->connection, transport_state->is_trace_on);
							// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_200: [The value of the option `logtrace` saved by the transport instance shall be applied to each new SASL_IO instance using xio_setoption().]
                            (void)xio_setoption(transport_state->cbs_connection.sasl_io, OPTION_LOG_TRACE, &transport_state->is_trace_on);
                            result = RESULT_OK;
                        }
                    }
                }
                break;
            }
            case(X509):
            {
                /*Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_02_006: [ IoTHubTransport_AMQP_Common_DoWork shall not establish a CBS connection. ]*/
                /*Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_02_005: [ IoTHubTransport_AMQP_Common_DoWork shall create the connection with the IoT service using connection_create2() AMQP API, passing the TLS I/O layer, IoT Hub FQDN and container ID as parameters (pass NULL for callbacks) ]*/
                if ((transport_state->connection = connection_create2(transport_state->tls_io, STRING_c_str(transport_state->iotHubHostFqdn), DEFAULT_CONTAINER_ID, NULL, NULL, NULL, NULL, on_connection_io_error, (void*)transport_state)) == NULL)
                {
                    // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_063: [If connection_create2() fails, IoTHubTransport_AMQP_Common_DoWork shall fail and return immediately.] 
                    result = __LINE__;
                    LogError("Failed to create the AMQP connection.");
                }
                else
                {
                    // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_137: [IoTHubTransport_AMQP_Common_DoWork shall create the AMQP session session_create() AMQP API, passing the connection instance as parameter]
                    if ((transport_state->session = session_create(transport_state->connection, NULL, NULL)) == NULL)
                    {
                        // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_138 : [If session_create() fails, IoTHubTransport_AMQP_Common_DoWork shall fail and return immediately]
                        result = __LINE__;
                        LogError("Failed to create the AMQP session.");
                    }
                    else
                    {
						set_session_options(transport_state->session);
					
						// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_199: [The value of the option `logtrace` saved by the transport instance shall be applied to each new connection instance using connection_set_trace().]
						connection_set_trace(transport_state->connection, transport_state->is_trace_on);
						// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_201: [The value of the option `logtrace` saved by the transport instance shall be applied to each new TLS_IO instance using xio_setoption().]
						(void)xio_setoption(transport_state->tls_io, OPTION_LOG_TRACE, &transport_state->is_trace_on);
						result = RESULT_OK;
                    }
                }
                break;
            }
            default:
            {
                LogError("internal error: unexpected enum value for transport_state->credential.credentialType = %d", transport_state->preferred_credential_type);
                result = __LINE__;
                break;
            }
        }/*switch*/
    }

    if (result != RESULT_OK)
    {
        destroyConnection(transport_state);
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

static void destroyEventSender(AMQP_TRANSPORT_DEVICE_STATE* device_state)
{
    if (device_state->message_sender != NULL)
    {
        messagesender_destroy(device_state->message_sender);
        device_state->message_sender = NULL;

        link_destroy(device_state->sender_link);
        device_state->sender_link = NULL;
    }
}

static void on_event_sender_state_changed(void* context, MESSAGE_SENDER_STATE new_state, MESSAGE_SENDER_STATE previous_state)
{
    if (context != NULL)
    {
        AMQP_TRANSPORT_DEVICE_STATE* device_state = (AMQP_TRANSPORT_DEVICE_STATE*)context;

        if (device_state->transport_state->is_trace_on)
        {
            LogInfo("Event sender state changed [%s, %d->%d]", STRING_c_str(device_state->deviceId), previous_state, new_state);
        }

		device_state->message_sender_state = new_state;

        // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_192: [If a message sender instance changes its state to MESSAGE_SENDER_STATE_ERROR (first transition only) the connection retry logic shall be triggered]
        if (new_state != previous_state && new_state == MESSAGE_SENDER_STATE_ERROR)
        {
			device_state->transport_state->connection_state = AMQP_MANAGEMENT_STATE_ERROR;
        }
    }
}

static int createEventSender(AMQP_TRANSPORT_DEVICE_STATE* device_state)
{
    int result;

	STRING_HANDLE link_name = NULL;
	STRING_HANDLE source_name = NULL;
    AMQP_VALUE source = NULL;
    AMQP_VALUE target = NULL;

	// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_251: [Every new message_sender AMQP link shall be created using unique link and source names per device, per connection]
	if ((link_name = create_link_name(STRING_c_str(device_state->deviceId), MESSAGE_SENDER_LINK_NAME_TAG, device_state->transport_state->link_count++)) == NULL)
	{
		LogError("Failed creating a name for the AMQP message sender link.");
		result = __LINE__;
	}
	else if ((source_name = create_link_source_name(link_name)) == NULL)
	{
		LogError("Failed creating a name for the AMQP message sender source.");
		result = __LINE__;
	}
    else if ((source = messaging_create_source(STRING_c_str(source_name))) == NULL)
    {
        LogError("Failed creating AMQP messaging source attribute.");
		result = __LINE__;
    }
	// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_252: [The message_sender AMQP link shall be created using the `target` address created according to SRS_IOTHUBTRANSPORTAMQP_09_014]
    else if ((target = messaging_create_target(STRING_c_str(device_state->targetAddress))) == NULL)
    {
        LogError("Failed creating AMQP messaging target attribute.");
		result = __LINE__;
    }
	// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_068: [IoTHubTransport_AMQP_Common_DoWork shall create the AMQP link using link_create(), with role as 'role_sender'] 
	else if ((device_state->sender_link = link_create(device_state->transport_state->session, STRING_c_str(link_name), role_sender, source, target)) == NULL)
    {
        // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_069: [If IoTHubTransport_AMQP_Common_DoWork fails to create the AMQP link for sending messages, the function shall fail and return immediately, flagging the connection to be re-stablished] 
        LogError("Failed creating AMQP link for message sender.");
		result = __LINE__;
    }
    else
    {
        // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_119: [IoTHubTransport_AMQP_Common_DoWork shall apply a default value of 65536 for the parameter 'Link MAX message size']
        if (link_set_max_message_size(device_state->sender_link, MESSAGE_SENDER_MAX_LINK_SIZE) != RESULT_OK)
        {
            LogError("Failed setting AMQP link max message size.");
        }

		// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_249: [The message sender link should have a property set with the type and version of the IoT Hub client application, set as `CLIENT_DEVICE_TYPE_PREFIX/IOTHUB_SDK_VERSION`]
		// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_250: [If the message sender link fails to have the client type and version set on its properties, the failure shall be ignored]
        attachDeviceClientTypeToLink(device_state->sender_link);

        // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_070: [IoTHubTransport_AMQP_Common_DoWork shall create the AMQP message sender using messagesender_create() AMQP API] 
        // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_191: [IoTHubTransport_AMQP_Common_DoWork shall create each AMQP message sender tracking its state changes with a callback function]
        if ((device_state->message_sender = messagesender_create(device_state->sender_link, on_event_sender_state_changed, (void*)device_state)) == NULL)
        {
            // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_071: [IoTHubTransport_AMQP_Common_DoWork shall fail and return immediately if the AMQP message sender instance fails to be created, flagging the connection to be re-established] 
            LogError("Could not allocate AMQP message sender");
			result = __LINE__;
        }
        else
        {
            // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_072: [IoTHubTransport_AMQP_Common_DoWork shall open the AMQP message sender using messagesender_open() AMQP API] 
            if (messagesender_open(device_state->message_sender) != RESULT_OK)
            {
                // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_073: [IoTHubTransport_AMQP_Common_DoWork shall fail and return immediately if the AMQP message sender instance fails to be opened, flagging the connection to be re-established] 
                LogError("Failed opening the AMQP message sender.");
				result = __LINE__;
            }
            else
            {
                result = RESULT_OK;
            }
        }
    }

	if (link_name != NULL)
		STRING_delete(link_name);
	if (source_name != NULL)
		STRING_delete(source_name);
    if (source != NULL)
        amqpvalue_destroy(source);
    if (target != NULL)
        amqpvalue_destroy(target);

    return result;
}

static int destroyMessageReceiver(AMQP_TRANSPORT_DEVICE_STATE* device_state)
{
    int result;

	if (device_state->message_receiver == NULL)
	{
		result = RESULT_OK;
	}
	else
	{
		if (messagereceiver_close(device_state->message_receiver) != RESULT_OK)
		{
			LogError("Failed closing the AMQP message receiver.");
			result = __LINE__;
		}
		else
		{
			messagereceiver_destroy(device_state->message_receiver);

			device_state->message_receiver = NULL;

			link_destroy(device_state->receiver_link);

			device_state->receiver_link = NULL;

			result = RESULT_OK;
		}
	}

    return result;
}

static void on_message_receiver_state_changed(const void* context, MESSAGE_RECEIVER_STATE new_state, MESSAGE_RECEIVER_STATE previous_state)
{
    if (context != NULL)
    {
        AMQP_TRANSPORT_DEVICE_STATE* device_state = (AMQP_TRANSPORT_DEVICE_STATE*)context;

        if (device_state->transport_state->is_trace_on)
        {
			LogInfo("Message receiver state changed [%s; %d->%d]", STRING_c_str(device_state->deviceId), previous_state, new_state);
        }

		device_state->message_receiver_state = new_state;

        // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_190: [If a message_receiver instance changes its state to MESSAGE_RECEIVER_STATE_ERROR (first transition only) the connection retry logic shall be triggered]
        if (new_state != previous_state && new_state == MESSAGE_RECEIVER_STATE_ERROR)
        {
			device_state->transport_state->connection_state = AMQP_MANAGEMENT_STATE_ERROR;
        }
    }
}

static int createMessageReceiver(AMQP_TRANSPORT_DEVICE_STATE* device_state)
{
    int result;

	STRING_HANDLE link_name = NULL;
	STRING_HANDLE target_name = NULL;
    AMQP_VALUE source = NULL;
    AMQP_VALUE target = NULL;

	// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_246: [Every new message_receiver AMQP link shall be created using unique link and target names per device, per connection]
	if ((link_name = create_link_name(STRING_c_str(device_state->deviceId), MESSAGE_RECEIVER_LINK_NAME_TAG, device_state->transport_state->link_count++)) == NULL)
	{
		LogError("Failed creating a name for the AMQP message receiver link.");
                    result = __LINE__;
	}
	else if ((target_name = create_link_target_name(link_name)) == NULL)
	{
		LogError("Failed creating a name for the AMQP message receiver target.");
                    result = __LINE__;
	}
	// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_253: [The message_receiver AMQP link shall be created using the `source` address created according to SRS_IOTHUBTRANSPORTAMQP_09_053]
    else if ((source = messaging_create_source(STRING_c_str(device_state->messageReceiveAddress))) == NULL)
    {
        LogError("Failed creating AMQP message receiver source attribute.");
        result = __LINE__;
    }
    else if ((target = messaging_create_target(STRING_c_str(target_name))) == NULL)
    {
        LogError("Failed creating AMQP message receiver target attribute.");
        result = __LINE__;
    }
	// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_074: [IoTHubTransport_AMQP_Common_DoWork shall create the AMQP link using link_create(), with role as 'role_receiver'] 
	else if ((device_state->receiver_link = link_create(device_state->transport_state->session, STRING_c_str(link_name), role_receiver, source, target)) == NULL)
    {
        // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_075: [If IoTHubTransport_AMQP_Common_DoWork fails to create the AMQP link for receiving messages, the function shall fail and return immediately, flagging the connection to be re-stablished] 
        LogError("Failed creating AMQP link for message receiver.");
        result = __LINE__;
    }
    // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_076: [IoTHubTransport_AMQP_Common_DoWork shall set the receiver link settle mode as receiver_settle_mode_first] 
    else if (link_set_rcv_settle_mode(device_state->receiver_link, receiver_settle_mode_first) != RESULT_OK)
    {
        // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_141: [If IoTHubTransport_AMQP_Common_DoWork fails to set the settle mode on the AMQP link for receiving messages, the function shall fail and return immediately, flagging the connection to be re-stablished]
        LogError("Failed setting AMQP link settle mode for message receiver.");
        result = __LINE__;
    }
    else
    {
        // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_119: [IoTHubTransport_AMQP_Common_DoWork shall apply a default value of 65536 for the parameter 'Link MAX message size']
        if (link_set_max_message_size(device_state->receiver_link, MESSAGE_RECEIVER_MAX_LINK_SIZE) != RESULT_OK)
        {
            LogError("Failed setting AMQP link max message size for message receiver.");
        }

        // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_247: [The message receiver link should have a property set with the type and version of the IoT Hub client application, set as `CLIENT_DEVICE_TYPE_PREFIX/IOTHUB_SDK_VERSION`]
        // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_248: [If the message receiver link fails to have the client type and version set on its properties, the failure shall be ignored]
        attachDeviceClientTypeToLink(device_state->receiver_link);

        // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_077: [IoTHubTransport_AMQP_Common_DoWork shall create the AMQP message receiver using messagereceiver_create() AMQP API] 
        // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_189: [IoTHubTransport_AMQP_Common_DoWork shall create each AMQP message_receiver tracking its state changes with a callback function]
        if ((device_state->message_receiver = messagereceiver_create(device_state->receiver_link, on_message_receiver_state_changed, (void*)device_state)) == NULL)
        {
            // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_078: [IoTHubTransport_AMQP_Common_DoWork shall fail and return immediately if the AMQP message receiver instance fails to be created, flagging the connection to be re-established] 
            LogError("Could not allocate AMQP message receiver.");
            result = __LINE__;
        }
        else
        {
            // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_079: [IoTHubTransport_AMQP_Common_DoWork shall open the AMQP message receiver using messagereceiver_open() AMQP API, passing a callback function for handling C2D incoming messages] 
            // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_123: [IoTHubTransport_AMQP_Common_DoWork shall create each AMQP message_receiver passing the 'on_message_received' as the callback function] 
            if (messagereceiver_open(device_state->message_receiver, on_message_received, (const void*)device_state->iothub_client_handle) != RESULT_OK)
            {
                // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_080: [IoTHubTransport_AMQP_Common_DoWork shall fail and return immediately if the AMQP message receiver instance fails to be opened, flagging the connection to be re-established] 
                LogError("Failed opening the AMQP message receiver.");
                result = __LINE__;
            }
            else
            {
                result = RESULT_OK;
            }
        }
    }

    if (link_name != NULL)
        STRING_delete(link_name);
    if (target_name != NULL)
        STRING_delete(target_name);
    if (source != NULL)
        amqpvalue_destroy(source);
    if (target != NULL)
        amqpvalue_destroy(target);

    return result;
}

static int sendPendingEvents(AMQP_TRANSPORT_DEVICE_STATE* device_state)
{
    int result = RESULT_OK;
    IOTHUB_MESSAGE_LIST* message;

    while ((message = getNextEventToSend(device_state)) != NULL)
    {
        result = __LINE__;

        MESSAGE_HANDLE amqp_message = NULL;
        bool is_message_error = false;

        // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_086: [IoTHubTransport_AMQP_Common_DoWork shall move queued events to an "in-progress" list right before processing them for sending]
        trackEventInProgress(message, device_state);

		// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_193: [IoTHubTransport_AMQP_Common_DoWork shall get a MESSAGE_HANDLE instance out of the event's IOTHUB_MESSAGE_HANDLE instance by using message_create_from_iothub_message().]
		if ((result = message_create_from_iothub_message(message->messageHandle, &amqp_message)) != RESULT_OK)
		{
			LogError("Failed creating AMQP message (error=%d).", result);
			result = __LINE__;
			is_message_error = true;
		}
		// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_097: [IoTHubTransport_AMQP_Common_DoWork shall pass the MESSAGE_HANDLE intance to uAMQP for sending (along with on_message_send_complete callback) using messagesender_send()] 
		else if (messagesender_send(device_state->message_sender, amqp_message, on_message_send_complete, message) != RESULT_OK)
        {
            LogError("Failed sending the AMQP message.");
            result = __LINE__;
        }
        else
        {
            result = RESULT_OK;
        }

        // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_194: [IoTHubTransport_AMQP_Common_DoWork shall destroy the MESSAGE_HANDLE instance after messagesender_send() is invoked.]
        if (amqp_message != NULL)
        {
            // It can be destroyed because AMQP keeps a clone of the message.
            message_destroy(amqp_message);
        }

        if (result != RESULT_OK)
        {
            if (is_message_error)
            {
                on_message_send_complete(message, MESSAGE_SEND_ERROR);
            }
            else
            {
                // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_111: [If message_create_from_iothub_message() fails, IoTHubTransport_AMQP_Common_DoWork notify the failure, roll back the event to waitToSend list and return]
                // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_113: [If messagesender_send() fails, IoTHubTransport_AMQP_Common_DoWork notify the failure, roll back the event to waitToSend list and return]
                rollEventBackToWaitList(message, device_state);
                break;
            }
        }
    }

    return result;
}

static void prepareDeviceForConnectionRetry(AMQP_TRANSPORT_DEVICE_STATE* device_state)
{
	if (authentication_reset(device_state->authentication) != RESULT_OK)
	{
		LogError("Failed resetting the authenticatication state of device %s", device_state->deviceId);
	}

    iothubtransportamqp_methods_unsubscribe(device_state->methods_handle);
    device_state->subscribed_for_methods = 0;

	destroyMessageReceiver(device_state);
	destroyEventSender(device_state);
	rollEventsBackToWaitList(device_state);
}

static void prepareForConnectionRetry(AMQP_TRANSPORT_INSTANCE* transport_state)
{
	size_t number_of_registered_devices = VECTOR_size(transport_state->registered_devices);

	for (size_t i = 0; i < number_of_registered_devices; i++)
	{
		AMQP_TRANSPORT_DEVICE_STATE* device_state = *(AMQP_TRANSPORT_DEVICE_STATE**)VECTOR_element(transport_state->registered_devices, i);

		prepareDeviceForConnectionRetry(device_state);
	}

    destroyConnection(transport_state);
    transport_state->connection_state = AMQP_MANAGEMENT_STATE_IDLE;
}

static int is_credential_compatible(const IOTHUB_DEVICE_CONFIG* device_config, AMQP_TRANSPORT_CREDENTIAL_TYPE preferred_authentication_type)
{
	int result;

	if (preferred_authentication_type == CREDENTIAL_NOT_BUILD)
	{
		result = RESULT_OK;
	}
	else if (preferred_authentication_type == X509 && (device_config->deviceKey != NULL || device_config->deviceSasToken != NULL))
	{
		LogError("Incompatible credentials: transport is using X509 certificate authentication, but device config contains deviceKey and/or sasToken");
		result = __LINE__;
	}
	else if (preferred_authentication_type != X509 && (device_config->deviceKey == NULL && device_config->deviceSasToken == NULL))
	{
		LogError("Incompatible credentials: transport is using CBS authentication, but device config does not contain deviceKey nor sasToken");
		result = __LINE__;
	}
	else
	{
		result = RESULT_OK;
	}

	return result;
}


// API functions

TRANSPORT_LL_HANDLE IoTHubTransport_AMQP_Common_Create(const IOTHUBTRANSPORT_CONFIG* config, AMQP_GET_IO_TRANSPORT get_io_transport)
{
    AMQP_TRANSPORT_INSTANCE* transport_state = NULL;

    // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_005: [If parameter config or config->upperConfig are NULL then IoTHubTransport_AMQP_Common_Create shall fail and return NULL.] 
    if (config == NULL || config->upperConfig == NULL)
    {
        LogError("IoTHub AMQP client transport null configuration parameter.");
    }
    // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_006: [IoTHubTransport_AMQP_Common_Create shall fail and return NULL if any fields of the config->upperConfig structure are NULL.]
    else if (config->upperConfig->protocol == NULL)
    {
        LogError("Invalid configuration (NULL protocol detected)");
    }
    else if (config->upperConfig->iotHubName == NULL)
    {
        LogError("Invalid configuration (NULL iotHubName detected)");
    }
    else if (config->upperConfig->iotHubSuffix == NULL)
    {
        LogError("Invalid configuration (NULL iotHubSuffix detected)");
    }
    // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_134: [IoTHubTransport_AMQP_Common_Create shall fail and return NULL if the combined length of config->iotHubName and config->iotHubSuffix exceeds 254 bytes (RFC1035)]
    else if ((strlen(config->upperConfig->iotHubName) + strlen(config->upperConfig->iotHubSuffix)) > (RFC1035_MAX_FQDN_LENGTH - 1))
    {
        LogError("The lengths of iotHubName and iotHubSuffix together exceed the maximum FQDN length allowed (RFC 1035)");
    }
	// Codes_SRS_IOTHUBTRANSPORTAMQP_09_254: [If parameter `get_io_transport` is NULL then IoTHubTransport_AMQP_Common_Create shall fail and return NULL.]
	else if (get_io_transport == NULL)
	{
		LogError("Invalid configuration (get_io_transport is NULL)");
	}
    else
    {
        // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_009: [IoTHubTransport_AMQP_Common_Create shall fail and return NULL if memory allocation of the transport's internal state structure fails.]
        if ((transport_state = (AMQP_TRANSPORT_INSTANCE*)malloc(sizeof(AMQP_TRANSPORT_INSTANCE))) == NULL)
        {
            LogError("Could not allocate AMQP transport state");
        }
        else
        {
            bool cleanup_required = false;

            transport_state->iotHubHostFqdn = NULL;
            transport_state->connection = NULL;
            transport_state->connection_state = AMQP_MANAGEMENT_STATE_IDLE;
            transport_state->session = NULL;
            transport_state->tls_io = NULL;
            transport_state->underlying_io_transport_provider = get_io_transport;
            transport_state->is_trace_on = false;

            transport_state->cbs_connection.cbs_handle = NULL;
            transport_state->cbs_connection.sasl_io = NULL;
            transport_state->cbs_connection.sasl_mechanism = NULL;

			// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_020: [IoTHubTransport_AMQP_Common_Create shall set parameter device_state->sas_token_lifetime with the default value of 3600000 (milliseconds).]
			transport_state->cbs_connection.sas_token_lifetime = DEFAULT_SAS_TOKEN_LIFETIME_MS;
			// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_128: [IoTHubTransport_AMQP_Common_Create shall set parameter device_state->sas_token_refresh_time with the default value of sas_token_lifetime/2 (milliseconds).] 
			transport_state->cbs_connection.sas_token_refresh_time = transport_state->cbs_connection.sas_token_lifetime / 2;
			// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_129 : [IoTHubTransport_AMQP_Common_Create shall set parameter device_state->cbs_request_timeout with the default value of 30000 (milliseconds).]
			transport_state->cbs_connection.cbs_request_timeout = DEFAULT_CBS_REQUEST_TIMEOUT_MS;

			transport_state->preferred_credential_type = CREDENTIAL_NOT_BUILD;

            transport_state->xioOptions = NULL; 
			transport_state->link_count = 0;

            // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_010: [If config->upperConfig->protocolGatewayHostName is NULL, IoTHubTransport_AMQP_Common_Create shall create an immutable string, referred to as iotHubHostFqdn, from the following pieces: config->iotHubName + "." + config->iotHubSuffix.] 
            // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_20_001: [If config->upperConfig->protocolGatewayHostName is not NULL, IoTHubTransport_AMQP_Common_Create shall use it as iotHubHostFqdn]
            if ((transport_state->iotHubHostFqdn = (config->upperConfig->protocolGatewayHostName != NULL ? STRING_construct(config->upperConfig->protocolGatewayHostName) : concat3Params(config->upperConfig->iotHubName, ".", config->upperConfig->iotHubSuffix))) == NULL)
            {
                LogError("Failed to set transport_state->iotHubHostFqdn.");
                cleanup_required = true;
            }
			// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_218: [IoTHubTransport_AMQP_Common_Create shall initialize the transport state registered device list with a VECTOR instance.]
			else if ((transport_state->registered_devices = VECTOR_create(sizeof(IOTHUB_DEVICE_HANDLE))) == NULL)
			{
				// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_219: [If VECTOR_create fails, IoTHubTransport_AMQP_Common_Create shall fail and return.]
				LogError("Failed to initialize the internal list of registered devices");
				cleanup_required = true;
			}

			// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_236: [If IoTHubTransport_AMQP_Common_Create fails it shall free any memory it allocated (iotHubHostFqdn, transport state).]
            if (cleanup_required)
            {
                if (transport_state->iotHubHostFqdn != NULL)
                    STRING_delete(transport_state->iotHubHostFqdn);

                free(transport_state);
                transport_state = NULL;
            }
        }
    }

    // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_023: [If IoTHubTransport_AMQP_Common_Create succeeds it shall return a non-NULL pointer to the structure that represents the transport.] 
    return transport_state;
}

static RESULT device_DoWork(AMQP_TRANSPORT_DEVICE_STATE* device_state)
{
	RESULT result = RESULT_SUCCESS;
	// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_243: [IoTHubTransport_AMQP_Common_DoWork shall retrieve the authenticatication status of the device using deviceauthentication_get_status()]
	AUTHENTICATION_STATUS auth_status = authentication_get_status(device_state->authentication);

	switch (auth_status)
	{
		case AUTHENTICATION_STATUS_IDLE:
			// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_243: [If the device authentication status is AUTHENTICATION_STATUS_IDLE, IoTHubTransport_AMQP_Common_DoWork shall authenticate it using authentication_authenticate()]
			if (authentication_authenticate(device_state->authentication) != RESULT_OK)
			{
				// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_146: [If authentication_authenticate() fails, IoTHubTransport_AMQP_Common_DoWork shall fail and process the next device]
				LogError("Failed authenticating AMQP connection [%s]", STRING_c_str(device_state->deviceId));
				result = RESULT_RETRYABLE_ERROR;
			}
			break;
		case AUTHENTICATION_STATUS_REFRESH_REQUIRED:
			// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_081: [If the device authentication status is AUTHENTICATION_STATUS_REFRESH_REQUIRED, IoTHubTransport_AMQP_Common_DoWork shall refresh it using authentication_refresh()]
			if (authentication_refresh(device_state->authentication) != RESULT_OK)
			{
				// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_082: [**If authentication_refresh() fails, IoTHubTransport_AMQP_Common_DoWork shall fail and process the next device]
				LogError("AMQP transport failed to refresh authentication [%s]", STRING_c_str(device_state->deviceId));
				result = RESULT_RETRYABLE_ERROR;
			}
			break;
		case AUTHENTICATION_STATUS_OK:
            /* Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_031: [ `iothubtransportamqp_methods_subscribe` shall only be called once (subsequent DoWork calls shall not call it if already subscribed). ]*/
            if ((device_state->subscribed_for_methods == 0) &&
                (subscribe_methods(device_state) != 0))
            {
                LogError("Failed subscribing for methods");
            }

			// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_145: [If the device authentication status is AUTHENTICATION_STATUS_OK, IoTHubTransport_AMQP_Common_DoWork shall proceed to sending events, registering for messages]

			// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_121: [IoTHubTransport_AMQP_Common_DoWork shall create an AMQP message_receiver if transport_state->message_receive is NULL and transport_state->receive_messages is true] 
			if (device_state->receive_messages == true &&
				device_state->message_receiver == NULL &&
				createMessageReceiver(device_state) != RESULT_OK)
			{
				LogError("Failed creating AMQP transport message receiver [%s]", STRING_c_str(device_state->deviceId));
				result = RESULT_CRITICAL_ERROR;
			}
			// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_122: [IoTHubTransport_AMQP_Common_DoWork shall destroy the transport_state->message_receiver (and set it to NULL) if it exists and transport_state->receive_messages is false] 
			else if (device_state->receive_messages == false &&
				device_state->message_receiver != NULL &&
				destroyMessageReceiver(device_state) != RESULT_OK)
			{
				LogError("Failed destroying AMQP transport message receiver [%s]", STRING_c_str(device_state->deviceId));
			}

			if (device_state->message_sender == NULL &&
				createEventSender(device_state) != RESULT_OK)
			{
				LogError("Failed creating AMQP transport event sender [%s]", STRING_c_str(device_state->deviceId));
				result = RESULT_CRITICAL_ERROR;
			}
			// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_245: [IoTHubTransport_AMQP_Common_DoWork shall skip sending events if the state of the message_sender is not MESSAGE_SENDER_STATE_OPEN]
			else if (device_state->message_sender_state == MESSAGE_SENDER_STATE_OPEN &&
				sendPendingEvents(device_state) != RESULT_OK)
			{
				LogError("AMQP transport failed sending events [%s]", STRING_c_str(device_state->deviceId));
				result = RESULT_CRITICAL_ERROR;
			}
			break;
		case AUTHENTICATION_STATUS_FAILURE:
			// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_083: [If the device authentication status is AUTHENTICATION_STATUS_FAILURE, IoTHubTransport_AMQP_Common_DoWork shall fail and process the next device]
			LogError("Authentication failed [%s]", STRING_c_str(device_state->deviceId));
			result = RESULT_CRITICAL_ERROR;
			break;
		case AUTHENTICATION_STATUS_TIMEOUT:
			// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_084: [If the device authentication status is AUTHENTICATION_STATUS_TIMEOUT, IoTHubTransport_AMQP_Common_DoWork shall fail and process the next device]
			LogError("Authentication timed-out [%s]", STRING_c_str(device_state->deviceId));
			result = RESULT_CRITICAL_ERROR;
			break;
		default:
			// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_244: [If the device authentication status is AUTHENTICATION_STATUS_IN_PROGRESS, IoTHubTransport_AMQP_Common_DoWork shall skip and process the next device]
			break;
	}

	return result;
}

IOTHUB_PROCESS_ITEM_RESULT IoTHubTransport_AMQP_Common_ProcessItem(TRANSPORT_LL_HANDLE handle, IOTHUB_IDENTITY_TYPE item_type, IOTHUB_IDENTITY_INFO* iothub_item)
{
    (void)handle;
    (void)item_type;
    (void)iothub_item;
    LogError("Currently Not Supported.");
    return IOTHUB_CLIENT_ERROR;
}

void IoTHubTransport_AMQP_Common_DoWork(TRANSPORT_LL_HANDLE handle, IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle)
{
	(void)iotHubClientHandle;
    // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_051: [IoTHubTransport_AMQP_Common_DoWork shall fail and return immediately if the transport handle parameter is NULL] 
    if (handle == NULL)
    {
        LogError("IoTHubClient DoWork failed: transport handle parameter is NULL.");
    } 
    else
    {
        bool trigger_connection_retry = false;
        AMQP_TRANSPORT_INSTANCE* transport_state = (AMQP_TRANSPORT_INSTANCE*)handle;
		size_t number_of_registered_devices = VECTOR_size(transport_state->registered_devices);

		// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_237: [IoTHubTransport_AMQP_Common_DoWork shall return immediately if there are no devices registered on the transport]
		if (number_of_registered_devices > 0)
		{
			// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_238: [If the transport state has a faulty connection state, IoTHubTransport_AMQP_Common_DoWork shall trigger the connection-retry logic]
			if (transport_state->connection != NULL &&
				transport_state->connection_state == AMQP_MANAGEMENT_STATE_ERROR)
			{
				LogError("An error occured on AMQP connection. The connection will be restablished.");
				trigger_connection_retry = true;
			}
			// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_055: [If the transport handle has a NULL connection, IoTHubTransport_AMQP_Common_DoWork shall instantiate and initialize the AMQP components and establish the connection] 
			else if (transport_state->connection == NULL &&
				establishConnection(transport_state) != RESULT_OK)
			{
				LogError("AMQP transport failed to establish connection with service.");
				trigger_connection_retry = true;
			}
			else
			{
				// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_241: [IoTHubTransport_AMQP_Common_DoWork shall iterate through all its registered devices to process authentication, events to be sent, messages to be received]
				for (size_t i = 0; i < number_of_registered_devices; i++)
				{
					AMQP_TRANSPORT_DEVICE_STATE* device_state = *(AMQP_TRANSPORT_DEVICE_STATE**)VECTOR_element(transport_state->registered_devices, i);

					RESULT actionable_result = device_DoWork(device_state);

					if (actionable_result == RESULT_CRITICAL_ERROR)
					{
						trigger_connection_retry = true;
					}
				}
			}

			if (trigger_connection_retry)
			{
				prepareForConnectionRetry(transport_state);
			}
			else
			{
				// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_103: [IoTHubTransport_AMQP_Common_DoWork shall invoke connection_dowork() on AMQP for triggering sending and receiving messages] 
				connection_dowork(transport_state->connection);
			}
		}
    }
}

int IoTHubTransport_AMQP_Common_Subscribe(IOTHUB_DEVICE_HANDLE handle)
{
    int result;

    // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_037: [IoTHubTransport_AMQP_Common_Subscribe shall fail if the transport handle parameter received is NULL.] 
    if (handle == NULL)
    {
        LogError("Invalid handle to IoTHubClient AMQP transport device handle.");
        result = __LINE__;
    }
    else
    {
        // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_038: [IoTHubTransport_AMQP_Common_Subscribe shall set transport_handle->receive_messages to true and return success code.]
        AMQP_TRANSPORT_DEVICE_STATE* device_state = (AMQP_TRANSPORT_DEVICE_STATE*)handle;
		device_state->receive_messages = true;
        result = 0;
    }

    return result;
}

void IoTHubTransport_AMQP_Common_Unsubscribe(IOTHUB_DEVICE_HANDLE handle)
{
    // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_039: [IoTHubTransport_AMQP_Common_Unsubscribe shall fail if the transport handle parameter received is NULL.] 
    if (handle == NULL)
    {
        LogError("Invalid handle to IoTHubClient AMQP transport device handle.");
    }
    else
    {
        // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_040: [IoTHubTransport_AMQP_Common_Unsubscribe shall set transport_handle->receive_messages to false.]
		AMQP_TRANSPORT_DEVICE_STATE* device_state = (AMQP_TRANSPORT_DEVICE_STATE*)handle;
		device_state->receive_messages = false;
    }
}

int IoTHubTransport_AMQP_Common_Subscribe_DeviceTwin(IOTHUB_DEVICE_HANDLE handle)
{
    (void)handle;
    /*Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_02_009: [ IoTHubTransport_AMQP_Common_Subscribe_DeviceTwin shall return a non-zero value. ]*/
    int result = __LINE__;
    LogError("IoTHubTransport_AMQP_Common_Subscribe_DeviceTwin Not supported");
    return result;
}

void IoTHubTransport_AMQP_Common_Unsubscribe_DeviceTwin(IOTHUB_DEVICE_HANDLE handle)
{
    (void)handle;
    /*Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_02_010: [ IoTHubTransport_AMQP_Common_Unsubscribe_DeviceTwin shall return. ]*/
    LogError("IoTHubTransport_AMQP_Common_Unsubscribe_DeviceTwin Not supported");
}

int IoTHubTransport_AMQP_Common_Subscribe_DeviceMethod(IOTHUB_DEVICE_HANDLE handle)
{
    int result;

    if (handle == NULL)
    {
        /* Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_004: [ If `handle` is NULL, `IoTHubTransport_AMQP_Common_Subscribe_DeviceMethod` shall fail and return a non-zero value. ] */
        LogError("NULL handle");
        result = __LINE__;
    }
    else
    {
        AMQP_TRANSPORT_DEVICE_STATE* device_state = (AMQP_TRANSPORT_DEVICE_STATE*)handle;
        /* Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_026: [ `IoTHubTransport_AMQP_Common_Subscribe_DeviceMethod` shall remember that a subscribe is to be performed in the next call to DoWork and on success it shall return 0. ]*/
        /* Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_005: [ If the transport is already subscribed to receive C2D method requests, `IoTHubTransport_AMQP_Common_Subscribe_DeviceMethod` shall perform no additional action and return 0. ]*/
        device_state->subscribe_methods_needed = 1;
        result = 0;
    }

    return result;
}

void IoTHubTransport_AMQP_Common_Unsubscribe_DeviceMethod(IOTHUB_DEVICE_HANDLE handle)
{
    if (handle == NULL)
    {
        /* Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_006: [ If `handle` is NULL, `IoTHubTransport_AMQP_Common_Unsubscribe_DeviceMethod` shall do nothing. ]*/
        LogError("NULL handle");
    }
    else
    {
        AMQP_TRANSPORT_DEVICE_STATE* device_state = (AMQP_TRANSPORT_DEVICE_STATE*)handle;

        /* Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_008: [ If the transport is not subscribed to receive C2D method requests then `IoTHubTransport_AMQP_Common_Unsubscribe_DeviceMethod` shall do nothing. ]*/
        if (device_state->subscribe_methods_needed != 0)
        {
            /* Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_007: [ `IoTHubTransport_AMQP_Common_Unsubscribe_DeviceMethod` shall unsubscribe from receiving C2D method requests by calling `iothubtransportamqp_methods_unsubscribe`. ]*/
            device_state->subscribe_methods_needed = 0;
            iothubtransportamqp_methods_unsubscribe(device_state->methods_handle);
        }
    }
}

IOTHUB_CLIENT_RESULT IoTHubTransport_AMQP_Common_GetSendStatus(IOTHUB_DEVICE_HANDLE handle, IOTHUB_CLIENT_STATUS *iotHubClientStatus)
{
    IOTHUB_CLIENT_RESULT result;

    // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_041: [IoTHubTransport_AMQP_Common_GetSendStatus shall return IOTHUB_CLIENT_INVALID_ARG if called with NULL parameter.]
    if (handle == NULL)
    {
        result = IOTHUB_CLIENT_INVALID_ARG;
        LogError("Invalid handle to IoTHubClient AMQP transport instance.");
    }
	// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_041: [IoTHubTransport_AMQP_Common_GetSendStatus shall return IOTHUB_CLIENT_INVALID_ARG if called with NULL parameter.]
    else if (iotHubClientStatus == NULL)
    {
        result = IOTHUB_CLIENT_INVALID_ARG;
        LogError("Invalid pointer to output parameter IOTHUB_CLIENT_STATUS.");
    }
    else
    {
        AMQP_TRANSPORT_DEVICE_STATE* device_state = (AMQP_TRANSPORT_DEVICE_STATE*)handle;

        // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_043: [IoTHubTransport_AMQP_Common_GetSendStatus shall return IOTHUB_CLIENT_OK and status IOTHUB_CLIENT_SEND_STATUS_BUSY if there are currently event items to be sent or being sent.]
        if (!DList_IsListEmpty(device_state->waitingToSend) || !DList_IsListEmpty(&(device_state->inProgress)))
        {
            *iotHubClientStatus = IOTHUB_CLIENT_SEND_STATUS_BUSY;
        }
        // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_042: [IoTHubTransport_AMQP_Common_GetSendStatus shall return IOTHUB_CLIENT_OK and status IOTHUB_CLIENT_SEND_STATUS_IDLE if there are currently no event items to be sent or being sent.] 
        else
        {
            *iotHubClientStatus = IOTHUB_CLIENT_SEND_STATUS_IDLE;
        }

        result = IOTHUB_CLIENT_OK;
    }

    return result;
}

IOTHUB_CLIENT_RESULT IoTHubTransport_AMQP_Common_SetOption(TRANSPORT_LL_HANDLE handle, const char* option, const void* value)
{
    IOTHUB_CLIENT_RESULT result;

    // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_044: [If handle parameter is NULL then IoTHubTransport_AMQP_Common_SetOption shall return IOTHUB_CLIENT_INVALID_ARG.]
    // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_045: [If parameter optionName is NULL then IoTHubTransport_AMQP_Common_SetOption shall return IOTHUB_CLIENT_INVALID_ARG.] 
    // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_046: [If parameter value is NULL then IoTHubTransport_AMQP_Common_SetOption shall return IOTHUB_CLIENT_INVALID_ARG.]
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

        // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_048: [IotHubTransportAMQP_SetOption shall save and apply the value if the option name is "sas_token_lifetime", returning IOTHUB_CLIENT_OK] 
        if (strcmp(OPTION_SAS_TOKEN_LIFETIME, option) == 0)
        {
            transport_state->cbs_connection.sas_token_lifetime = *((size_t*)value);
            result = IOTHUB_CLIENT_OK;
        }
        // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_049: [IotHubTransportAMQP_SetOption shall save and apply the value if the option name is "sas_token_refresh_time", returning IOTHUB_CLIENT_OK] 
        else if (strcmp(OPTION_SAS_TOKEN_REFRESH_TIME, option) == 0)
        {
            transport_state->cbs_connection.sas_token_refresh_time = *((size_t*)value);
            result = IOTHUB_CLIENT_OK;
        }
        // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_148: [IotHubTransportAMQP_SetOption shall save and apply the value if the option name is "cbs_request_timeout", returning IOTHUB_CLIENT_OK] 
        else if (strcmp(OPTION_CBS_REQUEST_TIMEOUT, option) == 0)
        {
            transport_state->cbs_connection.cbs_request_timeout = *((size_t*)value);
            result = IOTHUB_CLIENT_OK;
        }
        else if (strcmp(OPTION_LOG_TRACE, option) == 0)
        {
			// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_198: [If `optionName` is `logtrace`, IoTHubTransport_AMQP_Common_SetOption shall save the value on the transport instance.]
			transport_state->is_trace_on = *((bool*)value);

            if (transport_state->connection != NULL)
            {
				// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_202: [If `optionName` is `logtrace`, IoTHubTransport_AMQP_Common_SetOption shall apply it using connection_set_trace() to current connection instance if it exists and return IOTHUB_CLIENT_OK.]
                connection_set_trace(transport_state->connection, transport_state->is_trace_on);
            }

			// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_203: [If `optionName` is `logtrace`, IoTHubTransport_AMQP_Common_SetOption shall apply it using xio_setoption() to current SASL IO instance if it exists.]
			if (transport_state->cbs_connection.sasl_io != NULL &&
				xio_setoption(transport_state->cbs_connection.sasl_io, OPTION_LOG_TRACE, &transport_state->is_trace_on) != RESULT_OK)
			{
				// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_204: [If xio_setoption() fails, IoTHubTransport_AMQP_Common_SetOption shall fail and return IOTHUB_CLIENT_ERROR.]
				LogError("IoTHubTransport_AMQP_Common_SetOption failed (xio_setoption failed to set logging on SASL IO)");
				result = IOTHUB_CLIENT_ERROR;
			}
			else
			{
				// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_205: [If xio_setoption() succeeds, IoTHubTransport_AMQP_Common_SetOption shall return IOTHUB_CLIENT_OK.]
				result = IOTHUB_CLIENT_OK;
			}
        }
        // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_047: [If the option name does not match one of the options handled by this module, IoTHubTransport_AMQP_Common_SetOption shall pass the value and name to the XIO using xio_setoption().] 
        else
        {
			result = IOTHUB_CLIENT_OK;

			// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_02_007: [ If optionName is x509certificate and the authentication method is not x509 then IoTHubTransport_AMQP_Common_SetOption shall return IOTHUB_CLIENT_INVALID_ARG. ]
			if (strcmp(OPTION_X509_CERT, option) == 0)
			{
				if (transport_state->preferred_credential_type == CREDENTIAL_NOT_BUILD)
				{
					transport_state->preferred_credential_type = X509;
				}
				else if (transport_state->preferred_credential_type != X509)
				{
					LogError("x509certificate specified, but authentication method is not x509");
					result = IOTHUB_CLIENT_INVALID_ARG;
				}
			}
			/*Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_02_008: [ If optionName is x509privatekey and the authentication method is not x509 then IoTHubTransport_AMQP_Common_SetOption shall return IOTHUB_CLIENT_INVALID_ARG. ]*/
			else if (strcmp(OPTION_X509_PRIVATE_KEY, option) == 0)
			{
				if (transport_state->preferred_credential_type == CREDENTIAL_NOT_BUILD)
				{
					transport_state->preferred_credential_type = X509;
				}
				else if (transport_state->preferred_credential_type != X509)
				{
					LogError("x509privatekey specified, but authentication method is not x509");
					result = IOTHUB_CLIENT_INVALID_ARG;
				}
			}

			if (result != IOTHUB_CLIENT_INVALID_ARG)
			{
				// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_206: [If the TLS IO does not exist, IoTHubTransport_AMQP_Common_SetOption shall create it and save it on the transport instance.]
				if (transport_state->tls_io == NULL &&
					(transport_state->tls_io = transport_state->underlying_io_transport_provider(STRING_c_str(transport_state->iotHubHostFqdn))) == NULL)
				{
					// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_207: [If IoTHubTransport_AMQP_Common_SetOption fails creating the TLS IO instance, it shall fail and return IOTHUB_CLIENT_ERROR.]
					result = IOTHUB_CLIENT_ERROR;
					LogError("IoTHubTransport_AMQP_Common_SetOption failed (failed to obtain a TLS I/O transport layer).");
				}
				else
				{
					// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_208: [When a new TLS IO instance is created, IoTHubTransport_AMQP_Common_SetOption shall apply the TLS I/O Options with OptionHandler_FeedOptions() if it is has any saved.]
					if (transport_state->xioOptions != NULL)
					{
						if (OptionHandler_FeedOptions(transport_state->xioOptions, transport_state->tls_io) != 0)
						{
							LogError("IoTHubTransport_AMQP_Common_SetOption failed (unable to replay options to TLS)");
						}
						else
						{
							OptionHandler_Destroy(transport_state->xioOptions);
							transport_state->xioOptions = NULL;
						}
					}

					if (xio_setoption(transport_state->tls_io, option, value) != RESULT_OK)
					{
						/* Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_03_001: [If xio_setoption fails, IoTHubTransport_AMQP_Common_SetOption shall return IOTHUB_CLIENT_ERROR.] */
						result = IOTHUB_CLIENT_ERROR;
						LogError("Invalid option (%s) passed to IoTHubTransport_AMQP_Common_SetOption", option);
					}
					else
					{
						result = IOTHUB_CLIENT_OK;
					}
				}
			}
        }
    }

    return result;
}

IOTHUB_DEVICE_HANDLE IoTHubTransport_AMQP_Common_Register(TRANSPORT_LL_HANDLE handle, const IOTHUB_DEVICE_CONFIG* device, IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle, PDLIST_ENTRY waitingToSend)
{
#ifdef NO_LOGGING
    UNUSED(iotHubClientHandle);
#endif

    IOTHUB_DEVICE_HANDLE result = NULL;
    // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_17_001: [IoTHubTransport_AMQP_Common_Register shall return NULL if device, or waitingToSend are NULL.] 
    // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_17_005: [IoTHubTransport_AMQP_Common_Register shall return NULL if the TRANSPORT_LL_HANDLE is NULL.]
    if ((handle == NULL) || (device == NULL) || (waitingToSend == NULL))
    {
        LogError("invalid parameter TRANSPORT_LL_HANDLE handle=%p, const IOTHUB_DEVICE_CONFIG* device=%p, IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle=%p, PDLIST_ENTRY waitingToSend=%p",
            handle, device, iotHubClientHandle, waitingToSend);
    }
	// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_220: [IoTHubTransport_AMQP_Common_Register shall fail and return NULL if the IOTHUB_CLIENT_LL_HANDLE is NULL.]
	else if (iotHubClientHandle == NULL)
	{
		LogError("IoTHubTransport_AMQP_Common_Register failed (invalid parameter; iotHubClientHandle list is NULL)");
	}
	else
	{
		AMQP_TRANSPORT_INSTANCE* transport_state = (AMQP_TRANSPORT_INSTANCE*)handle;

		// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_03_002: [IoTHubTransport_AMQP_Common_Register shall return NULL if deviceId is NULL.]
		if (device->deviceId == NULL)
		{
			LogError("IoTHubTransport_AMQP_Common_Register failed (deviceId provided is NULL)");
		}
		// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_03_003: [IoTHubTransport_AMQP_Common_Register shall return NULL if both deviceKey and deviceSasToken are not NULL.]
		else if ((device->deviceSasToken != NULL) && (device->deviceKey != NULL))
		{
			LogError("IoTHubTransport_AMQP_Common_Register failed (invalid IOTHUB_DEVICE_CONFIG; must provide EITHER 'deviceSasToken' OR 'deviceKey')");
		}
		// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_221: [IoTHubTransport_AMQP_Common_Register shall fail and return NULL if the device is not using an authentication mode compatible with the currently used by the transport.]
		else if (is_credential_compatible(device, transport_state->preferred_credential_type) != RESULT_OK)
		{
			LogError("IoTHubTransport_AMQP_Common_Register failed (transport does not support mixed authentication methods)");
		}
		else
		{
			AMQP_TRANSPORT_DEVICE_STATE* device_state;

			// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_222: [If a device matching the deviceId provided is already registered, IoTHubTransport_AMQP_Common_Register shall fail and return NULL.]
			if (VECTOR_find_if(transport_state->registered_devices, findDeviceById, device->deviceId) != NULL)
			{
				LogError("IoTHubTransport_AMQP_Common_Register failed (device '%s' already registered on this transport instance)", device->deviceId);
			}
			// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_223: [IoTHubTransport_AMQP_Common_Register shall allocate an instance of AMQP_TRANSPORT_DEVICE_STATE to store the state of the new registered device.]
			else if ((device_state = (AMQP_TRANSPORT_DEVICE_STATE*)malloc(sizeof(AMQP_TRANSPORT_DEVICE_STATE))) == NULL)
			{
				// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_224: [If malloc fails to allocate memory for AMQP_TRANSPORT_DEVICE_STATE, IoTHubTransport_AMQP_Common_Register shall fail and return NULL.]
				LogError("IoTHubTransport_AMQP_Common_Register failed (malloc failed)");
			}
			else
			{
				bool cleanup_required;
                const char* deviceId = device->deviceId;

				// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_225: [IoTHubTransport_AMQP_Common_Register shall save the handle references to the IoTHubClient, transport, waitingToSend list on the device state.]
				device_state->iothub_client_handle = iotHubClientHandle;
				device_state->transport_state = transport_state;

				device_state->waitingToSend = waitingToSend;
				// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_226: [IoTHubTransport_AMQP_Common_Register shall initialize the device state inProgress list using DList_InitializeListHead().]
				DList_InitializeListHead(&device_state->inProgress);

				device_state->deviceId = NULL;
				device_state->authentication = NULL;
				device_state->devicesPath = NULL;
				device_state->messageReceiveAddress = NULL;
				device_state->targetAddress = NULL;

				device_state->receive_messages = false;
				device_state->message_receiver = NULL;
				device_state->message_sender = NULL;
                device_state->message_sender_state = MESSAGE_SENDER_STATE_IDLE;
				device_state->receiver_link = NULL;
				device_state->sender_link = NULL;
                device_state->subscribe_methods_needed = 0;
                device_state->subscribed_for_methods = 0;

				// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_227: [IoTHubTransport_AMQP_Common_Register shall store a copy of config->deviceId into device_state->deviceId.]
				if ((device_state->deviceId = STRING_construct(deviceId)) == NULL)
				{
					// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_228: [If STRING_construct fails to copy config->deviceId, IoTHubTransport_AMQP_Common_Register shall fail and return NULL.]
					LogError("IoTHubTransport_AMQP_Common_Register failed to copy the deviceId.");
					cleanup_required = true;
				}
				// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_012: [IoTHubTransport_AMQP_Common_Register shall create an immutable string, referred to as devicesPath, from the following parts: host_fqdn + "/devices/" + deviceId.] 
				else if ((device_state->devicesPath = concat3Params(STRING_c_str(transport_state->iotHubHostFqdn), "/devices/", STRING_c_str(device_state->deviceId))) == NULL)
				{
					// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_013: [If creating devicesPath fails for any reason then IoTHubTransport_AMQP_Common_Register shall fail and return NULL.] 
					LogError("IoTHubTransport_AMQP_Common_Register failed to construct the devicesPath.");
					cleanup_required = true;
				}
				// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_014: [IoTHubTransport_AMQP_Common_Register shall create an immutable string, referred to as targetAddress, from the following parts: "amqps://" + devicesPath + "/messages/events".]
				else if ((device_state->targetAddress = concat3Params("amqps://", STRING_c_str(device_state->devicesPath), "/messages/events")) == NULL)
				{
					// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_015: [If creating the targetAddress fails for any reason then IoTHubTransport_AMQP_Common_Register shall fail and return NULL.] 
					LogError("IoTHubTransport_AMQP_Common_Register failed to construct the targetAddress.");
					cleanup_required = true;
				}
				// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_053: [IoTHubTransport_AMQP_Common_Register shall define the source address for receiving messages as "amqps://" + devicesPath + "/messages/devicebound", stored in the transport handle as messageReceiveAddress]
				else if ((device_state->messageReceiveAddress = concat3Params("amqps://", STRING_c_str(device_state->devicesPath), "/messages/devicebound")) == NULL)
				{
					// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_054: [If creating the messageReceiveAddress fails for any reason then IoTHubTransport_AMQP_Common_Register shall fail and return NULL.]
					LogError("IoTHubTransport_AMQP_Common_Register failed to construct the messageReceiveAddress.");
					cleanup_required = true;
				}
				else
				{
					/* Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_010: [ `IoTHubTransport_AMQP_Common_Create` shall create a new iothubtransportamqp_methods instance by calling `iothubtransportamqp_methods_create` while passing to it the the fully qualified domain name and the device Id. ]*/
					device_state->methods_handle = iothubtransportamqp_methods_create(STRING_c_str(transport_state->iotHubHostFqdn), deviceId);
					if (device_state->methods_handle == NULL)
					{
						/* Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_011: [ If `iothubtransportamqp_methods_create` fails, `IoTHubTransport_AMQP_Common_Create` shall fail and return NULL. ]*/
						LogError("Cannot create the methods module");
						cleanup_required = true;
					}
					// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_231: [IoTHubTransport_AMQP_Common_Register shall add the device to transport_state->registered_devices using VECTOR_push_back().]
					else if (VECTOR_push_back(transport_state->registered_devices, &device_state, 1) != 0)
					{
						// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_232: [If VECTOR_push_back() fails to add the new registered device, IoTHubTransport_AMQP_Common_Register shall clean the memory it allocated, fail and return NULL.]
						LogError("IoTHubTransport_AMQP_Common_Register failed to add the new device to its list of registered devices (VECTOR_push_back failed).");
						cleanup_required = true;
					}
					else
					{
						AUTHENTICATION_CONFIG auth_config;
						auth_config.device_id = deviceId;
						auth_config.device_key = device->deviceKey;
						auth_config.device_sas_token = device->deviceSasToken;
						auth_config.cbs_connection = &device_state->transport_state->cbs_connection;
						auth_config.iot_hub_host_fqdn = STRING_c_str(device_state->transport_state->iotHubHostFqdn);

						// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_229: [IoTHubTransport_AMQP_Common_Register shall create an authentication state for the device using authentication_create() and store it on the device state.]
						if ((device_state->authentication = authentication_create(&auth_config)) == NULL)
						{
							// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_230: [If authentication_create() fails, IoTHubTransport_AMQP_Common_Register shall fail and return NULL.]
							LogError("IoTHubTransport_AMQP_Common_Register failed to create an authentication state for the device.");
							cleanup_required = true;
						}
						else
						{
							// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_234: [If the device is the first being registered on the transport, IoTHubTransport_AMQP_Common_Register shall save its authentication mode as the transport preferred authentication mode.]
							if (VECTOR_size(transport_state->registered_devices) == 1)
							{
								transport_state->preferred_credential_type = authentication_get_credential(device_state->authentication)->type;
							}

							// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_233: [IoTHubTransport_AMQP_Common_Register shall return its internal device representation as a IOTHUB_DEVICE_HANDLE.]
							result = (IOTHUB_DEVICE_HANDLE)device_state;
							cleanup_required = false;
						}
					}
				}

				// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_233: [If IoTHubTransport_AMQP_Common_Register fails, it shall free all memory it alloacated (destroy deviceId, authentication state, targetAddress, messageReceiveAddress, devicesPath, device state).]
				if (cleanup_required)
				{
					if (device_state->deviceId != NULL)
						STRING_delete(device_state->deviceId);
					if (device_state->authentication != NULL)
						authentication_destroy(device_state->authentication);
					if (device_state->targetAddress != NULL)
						STRING_delete(device_state->targetAddress);
					if (device_state->messageReceiveAddress != NULL)
						STRING_delete(device_state->messageReceiveAddress);
					if (device_state->devicesPath != NULL)
						STRING_delete(device_state->devicesPath);

					free(device_state);
				}
			}
		}
	}

	return result;
}

void IoTHubTransport_AMQP_Common_Unregister(IOTHUB_DEVICE_HANDLE deviceHandle)
{
	// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_214: [IoTHubTransport_AMQP_Common_Unregister should fail and return if the IOTHUB_DEVICE_HANDLE parameter provided is NULL.]
	if (deviceHandle == NULL)
	{
		LogError("IoTHubTransport_AMQP_Common_Unregister failed (deviceHandle is NULL).");
	}
	else
	{
		AMQP_TRANSPORT_DEVICE_STATE* device_state = (AMQP_TRANSPORT_DEVICE_STATE*)deviceHandle;

		// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_215: [IoTHubTransport_AMQP_Common_Unregister should fail and return if the IOTHUB_DEVICE_HANDLE parameter provided has a NULL reference to its transport instance.]
		if (device_state->transport_state == NULL)
		{
			LogError("IoTHubTransport_AMQP_Common_Unregister failed (deviceHandle does not have a transport state associated to).");
		}
		else
		{
			IOTHUB_DEVICE_HANDLE* registered_device_state;

			// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_216: [IoTHubTransport_AMQP_Common_Unregister should fail and return if the device is not registered with this transport.]
			if ((registered_device_state = VECTOR_find_if(device_state->transport_state->registered_devices, findDeviceById, STRING_c_str(device_state->deviceId))) == NULL)
			{
				LogError("IoTHubTransport_AMQP_Common_Unregister failed (device '%s' is not registered on this transport instance)", STRING_c_str(device_state->deviceId));
			}
			else
			{
				// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_024: [IoTHubTransport_AMQP_Common_Unregister shall destroy the AMQP message_sender.]
				// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_029: [IoTHubTransport_AMQP_Common_Unregister shall destroy the AMQP message_sender link.]
				destroyEventSender(device_state);

				// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_025: [IoTHubTransport_AMQP_Common_Unregister shall destroy the AMQP message_receiver.] 
				// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_211: [IoTHubTransport_AMQP_Common_Unregister shall destroy the AMQP message_receiver link.]
				destroyMessageReceiver(device_state);

				// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_036: [IoTHubTransport_AMQP_Common_Unregister shall return the remaining items in inProgress to waitingToSend list.]
				rollEventsBackToWaitList(device_state);

				// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_035: [IoTHubTransport_AMQP_Common_Unregister shall delete its internally-set parameters (targetAddress, messageReceiveAddress, devicesPath, deviceId).]
				STRING_delete(device_state->targetAddress);
				STRING_delete(device_state->messageReceiveAddress);
				STRING_delete(device_state->devicesPath);
				STRING_delete(device_state->deviceId);
				// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_217: [IoTHubTransport_AMQP_Common_Unregister shall destroy the authentication state of the device using authentication_destroy.]
				authentication_destroy(device_state->authentication);

                /* Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_012: [IoTHubTransport_AMQP_Common_Unregister shall destroy the C2D methods handler by calling iothubtransportamqp_methods_destroy.]*/
                iothubtransportamqp_methods_destroy(device_state->methods_handle);

				// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_218: [IoTHubTransport_AMQP_Common_Unregister shall remove the device from its list of registered devices using VECTOR_erase().]
				VECTOR_erase(device_state->transport_state->registered_devices, registered_device_state, 1);

				// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_219: [IoTHubTransport_AMQP_Common_Unregister shall destroy the IOTHUB_DEVICE_HANDLE instance provided.]
				free(device_state);
			}
		}
	}
}

void IoTHubTransport_AMQP_Common_Destroy(TRANSPORT_LL_HANDLE handle)
{
	if (handle != NULL)
	{
		AMQP_TRANSPORT_INSTANCE* transport_state = (AMQP_TRANSPORT_INSTANCE*)handle;

		size_t numberOfRegisteredDevices = VECTOR_size(transport_state->registered_devices);

		for (size_t i = 0; i < numberOfRegisteredDevices; i++)
		{
			// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_209: [IoTHubTransport_AMQP_Common_Destroy shall invoke IoTHubTransport_AMQP_Common_Unregister on each of its registered devices.]
			IoTHubTransport_AMQP_Common_Unregister(*(AMQP_TRANSPORT_DEVICE_STATE**)VECTOR_element(transport_state->registered_devices, i));
		}

		// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_210: [IoTHubTransport_AMQP_Common_Destroy shall its list of registered devices using VECTOR_destroy().]
		VECTOR_destroy(transport_state->registered_devices);

		// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_027: [IoTHubTransport_AMQP_Common_Destroy shall destroy the AMQP cbs instance]
		// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_030: [IoTHubTransport_AMQP_Common_Destroy shall destroy the AMQP session.]
		// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_031: [IoTHubTransport_AMQP_Common_Destroy shall destroy the AMQP connection.]
		// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_032: [IoTHubTransport_AMQP_Common_Destroy shall destroy the AMQP SASL I / O transport.]
		// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_033: [IoTHubTransport_AMQP_Common_Destroy shall destroy the AMQP SASL mechanism.]
		// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_034: [IoTHubTransport_AMQP_Common_Destroy shall destroy the AMQP TLS I/O transport.]
		destroyConnection(transport_state);

		// CodeS_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_212: [IoTHubTransport_AMQP_Common_Destroy shall destroy the IoTHub FQDN value saved on the transport instance]
		STRING_delete(transport_state->iotHubHostFqdn);

		// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_213: [IoTHubTransport_AMQP_Common_Destroy shall destroy any TLS I/O options saved on the transport instance using OptionHandler_Destroy()]
		if (transport_state->xioOptions != NULL)
		{
			OptionHandler_Destroy(transport_state->xioOptions);
		}

		// Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_150: [IoTHubTransport_AMQP_Common_Destroy shall destroy the transport instance]
		free(transport_state);
	}
}

int IoTHubTransport_AMQP_Common_SetRetryPolicy(TRANSPORT_LL_HANDLE handle, IOTHUB_CLIENT_RETRY_POLICY retryPolicy, size_t retryTimeoutLimitInSeconds)
{
    int result;
    (void)handle;
    (void)retryPolicy;
    (void)retryTimeoutLimitInSeconds;

    LogError("Not implemented for AMQP");
    result = __LINE__;
    return result;
}

STRING_HANDLE IoTHubTransport_AMQP_Common_GetHostname(TRANSPORT_LL_HANDLE handle)
{
    STRING_HANDLE result;
    // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_02_001: [If parameter handle is NULL then IoTHubTransport_AMQP_Common_GetHostname shall return NULL.]
    if (handle == NULL)
    {
        result = NULL;
    }
    else
    {
        // Codes_SRS_IOTHUBTRANSPORT_AMQP_COMMON_02_002: [Otherwise IoTHubTransport_AMQP_Common_GetHostname shall return the target IoT Hub FQDN as a STRING_HANDLE.]
        result = ((AMQP_TRANSPORT_INSTANCE*)(handle))->iotHubHostFqdn;
    }
    return result;
}

