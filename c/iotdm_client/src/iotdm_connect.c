// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "iotdm_internal.h"
#include "azure_c_shared_utility/threadapi.h"

#include <ctype.h>

STRING_HANDLE iotdmc_SAS_create(const char* iotHubName, const char* deviceKey)
{
    STRING_HANDLE retValue = NULL;

    STRING_HANDLE uriResource = STRING_construct(iotHubName);
    if (NULL != uriResource)
    {
        STRING_HANDLE key = STRING_construct(deviceKey);
        if (NULL != key)
        {
            time_t now = get_time(NULL);
            size_t expiry = (size_t)(difftime(now, 0) + 3600);
            STRING_HANDLE zeroLengthString = STRING_new();
            if (NULL != zeroLengthString)
            {
                retValue = SASToken_Create(key, uriResource, zeroLengthString, expiry);

                STRING_delete(zeroLengthString);
            }

            STRING_delete(key);
        }

        STRING_delete(uriResource);
    }

    else
    {
        LogError("keyName is NULL\r\n");
    }

    return retValue;
}

STRING_HANDLE iotdmc_get_registration_query(const char* endpointName, const char* iotHubName, const char* deviceKey)
{
    STRING_HANDLE retValue = STRING_construct("?ep=");
    STRING_HANDLE sasToken = NULL;
    if (retValue != NULL)
    {
        if (0 != STRING_concat(retValue, endpointName) ||
            0 != STRING_concat(retValue, "&b=T&tk=") ||
            NULL == (sasToken = iotdmc_SAS_create(iotHubName, deviceKey)) ||
            0 != STRING_concat_with_STRING(retValue, sasToken))
        {
            STRING_delete(retValue);
            retValue = NULL;
        }
    }

    STRING_delete(sasToken);
    return retValue;
}


void iotdmc_handle_registration_reply(lwm2m_transaction_t *transacP, void *message)
{
    coap_packet_t  *packet = (coap_packet_t *)message;
    lwm2m_server_t *targetP = (lwm2m_server_t *)(transacP->peerP);

    switch (targetP->status)
    {
        case STATE_REG_PENDING:
        {
            time_t tv_sec = lwm2m_gettime();
            if (tv_sec >= 0)
            {
                targetP->registration = tv_sec;
            }

            if (packet != NULL && packet->code == CREATED_2_01)
            {
                targetP->status = STATE_REGISTERED;
                if (NULL != targetP->location)
                {
                    lwm2m_free(targetP->location);
                }

                targetP->location = coap_get_multi_option_as_string(packet->location_path);

                LogInfo("    => REGISTERED(%s)\r\n", targetP->location ? targetP->location : "");
            }

            else
            {
                targetP->status = STATE_REG_FAILED;
                LogError("    => Registration FAILED\r\n");
            }

            break;
        }

        default:
            break;
    }
}

void on_bytes_received(void *context, const unsigned char *buffer, size_t size);
IOTHUB_CLIENT_RESULT send_register_transaction(CLIENT_DATA *cd);

typedef struct
{
    CLIENT_DATA *client;
    ON_REGISTER_COMPLETE onComplete;
    void *context;
} IO_OPEN_COMPLETE_CONTEXT;


//void(*ON_IO_OPEN_COMPLETE)(void* context, IO_OPEN_RESULT open_result);
void on_io_open_complete(void *context, IO_OPEN_RESULT open_result)
{
    IO_OPEN_COMPLETE_CONTEXT *openContext = (IO_OPEN_COMPLETE_CONTEXT *) context;
    IOTHUB_CLIENT_RESULT result = (open_result == IO_OPEN_OK) ? IOTHUB_CLIENT_OK : IOTHUB_CLIENT_ERROR;

    if (result == IOTHUB_CLIENT_OK)
    {
        result = send_register_transaction(openContext->client);
    }

    openContext->onComplete(result, openContext->context);
}

//void(*ON_REGISTER_COMPLETE)(IOTHUB_CLIENT_RESULT result, void* context);
void on_reconnect_complete(IOTHUB_CLIENT_RESULT result, void* context)
{
    // ignore result, free the context
    free(context);
}

const int MAX_CONNECT_RETRY = 36000;
void on_io_error(void* context);
int dm_io_open(IO_OPEN_COMPLETE_CONTEXT *openContext)
{
    int retValue;
    int retry = 1;
    CLIENT_DATA *cd = openContext->client;
    while (retry <= MAX_CONNECT_RETRY)
    {
        retValue = xio_open(cd->ioHandle, on_io_open_complete, openContext, on_bytes_received, cd, on_io_error, cd);
        if (retValue == 0)
        {
            retValue = 0;
            retry = MAX_CONNECT_RETRY + 1;
        }

        else
        {
            if (0 != xio_close(cd->ioHandle, NULL, NULL))
            {
                LogError("xio_close failed");
                // What to do here?  absorb and continue for now.
            }

            LogInfo("Retry open in %d seconds\n", retry);
            ThreadAPI_Sleep(retry * 1000);
            retry *= 2;
        }
    }

    return retValue;
}


//void(*ON_IO_ERROR)(void* context);
void on_io_error(void* context)
{
    LogError("    on_io_error event\n");

    CLIENT_DATA *cd = (CLIENT_DATA *) context;
    (void)xio_close(cd->ioHandle, NULL, NULL);

    IO_OPEN_COMPLETE_CONTEXT *openContext = (IO_OPEN_COMPLETE_CONTEXT *) malloc(sizeof(IO_OPEN_COMPLETE_CONTEXT));
    if (NULL == openContext)
    {
        LogError("    failed to allocate context object\n");
    }

    else
    {
        openContext->client = cd;
        openContext->onComplete = on_reconnect_complete;
        openContext->context = openContext;

        (void)dm_io_open(openContext);
    }
}

static int set_keep_alive(CLIENT_DATA *cd)
{
    const int keepAlive = 1;
    const int keepAliveTime = 180; // send keep-alive after 3 minutes of socket inactivity
    const int keepAliveInterval = 10; // 10 seconds between keep-alive retries

    int result = xio_setoption(cd->ioHandle, "tcp_keepalive", &keepAlive);
    if (result != 0)
    {
        LogError("    xio_setoption(\"tcp_keepalive\") failed, returned = %d\r\n", result);
    }

    else
    {
        result = xio_setoption(cd->ioHandle, "tcp_keepalive_time", &keepAliveTime);
        if (result != 0)
        {
            LogError("    xio_setoption(\"tcp_keepalive_time\") failed, returned = %d\r\n", result);
        }

        else
        {
            result = xio_setoption(cd->ioHandle, "tcp_keepalive_interval", &keepAliveInterval);
            if (result != 0)
            {
                LogError("    xio_setoption(\"tcp_keepalive_interval\") failed, returned = %d\r\n", result);
            }
        }
    }

    return result;
}

XIO_HANDLE dm_io_create(const char* iotHubName);

IOTHUB_CLIENT_RESULT iotdmc_register(CLIENT_DATA *cd, ON_REGISTER_COMPLETE onComplete, void* callbackContext)
{
    IO_OPEN_COMPLETE_CONTEXT* openContext = malloc(sizeof(IO_OPEN_COMPLETE_CONTEXT));
    if (NULL == openContext)
    {
        LogError("    failed to allocate context object\n");
    }

    else
    {
        openContext->client = cd;
        openContext->onComplete = onComplete;
        openContext->context = callbackContext;

        cd->ioHandle = dm_io_create(cd->config.iotHubName);
        if (!cd->ioHandle)
        {
            LogError("    xio_create failed\n");
        }

        else
        {
            int result = dm_io_open(openContext);
            if (result == 0)
            {
                result = set_keep_alive(cd);
                if (result == 0)
                {
                    return IOTHUB_CLIENT_OK;
                }
            }

            LogError("    failed to open a connection to the service (error: %d)\n", result);
            xio_destroy(cd->ioHandle);
            cd->ioHandle = NULL;
        }

        free(openContext);
    }

    return IOTHUB_CLIENT_ERROR;
}

bool append_object_link(uint16_t oid, void *context)
{
    bool suceeded = false;

    // don't add LWM2M_SECURITY_OBJECT_ID to the payload
    if (oid == LWM2M_SECURITY_OBJECT_ID)
    {
        return true;
    }

    int cch = snprintf(NULL, 0, REG_OBJECT_INSTANCE_PATH, "", oid, 0);
    if (cch > 0)
    {
        char* link = malloc(cch + 1);
        if (link != NULL)
        {
            cch = snprintf(link, cch + 1, REG_OBJECT_INSTANCE_PATH, "", oid, 0);
            if (cch > 0)
            {
                STRING_HANDLE object_links = (STRING_HANDLE)context;
                int result = STRING_concat(object_links, link);
                suceeded = (result == 0);
            }

            free(link);
        }
    }

    return suceeded;
}

static STRING_HANDLE get_registration_payload()
{
    STRING_HANDLE payload = STRING_construct("</>;rt=\"oma.lwm2m\",");
    if (payload != NULL)
    {
        return for_each_oid(append_object_link, payload) ? payload : NULL;
    }

    return NULL;
}

IOTHUB_CLIENT_RESULT send_register_transaction(CLIENT_DATA *cd)
{
    IOTHUB_CLIENT_RESULT result;

    if (!cd->ioHandle)
    {
        result = IOTHUB_CLIENT_INVALID_ARG;
    }

    else
    {
        STRING_HANDLE hPayload = get_registration_payload();
        if (!hPayload)
        {
            result = IOTHUB_CLIENT_ERROR;
        }

        else
        {
            STRING_HANDLE hQuery = iotdmc_get_registration_query(cd->session->endpointName, cd->config.iotHubName, cd->config.deviceKey);
            if (!hQuery)
            {
                result = IOTHUB_CLIENT_ERROR;
            }

            else
            {
                lwm2m_context_t *contextP = cd->session;
                lwm2m_server_t  *server = contextP->serverList;

                LogInfo("    Connect to:'%s'\n", cd->config.iotHubName);

                server->sessionH = cd->ioHandle;
                lwm2m_transaction_t *transaction = transaction_new(COAP_TYPE_CON, COAP_POST, NULL, NULL, contextP->nextMID++, 4, NULL, ENDPOINT_SERVER, (void *)server);
                if (!transaction)
                {
                    result = IOTHUB_CLIENT_ERROR;
                }

                else
                {
                    coap_set_header_uri_query(transaction->message, STRING_c_str(hQuery));
                    coap_set_header_uri_path(transaction->message, "/"URI_REGISTRATION_SEGMENT);
                    coap_set_header_uri_host(transaction->message, cd->config.iotHubName);

                    coap_set_header_content_type(transaction->message, LWM2M_CONTENT_LINK);
                    coap_set_payload(transaction->message, STRING_c_str(hPayload), STRING_length(hPayload) - 1 /* remove the trailing '.'*/);

                    transaction->callback = iotdmc_handle_registration_reply;
                    transaction->userData = (void *)server;

                    contextP->transactionList = (lwm2m_transaction_t *)LWM2M_LIST_ADD(contextP->transactionList, transaction);
                    if (transaction_send(contextP, transaction) == 0)
                    {
                        server->status = STATE_REG_PENDING;
                        result = IOTHUB_CLIENT_OK;
                    }

                    else
                    {
                        result = IOTHUB_CLIENT_ERROR;
                    }
                }

                STRING_delete(hQuery);
            }

            STRING_delete(hPayload);
        }
    }

    return result;
}


uint32_t parse_int(uint8_t *bytes, size_t length)
{
    uint32_t retVal = 0;
    size_t   i = 0;

    while (i < length)
    {
        retVal <<= 8;
        retVal |= bytes[i++];
    }

    return retVal;
}


uint16_t prv_min(uint16_t x, uint16_t y, uint16_t z)
{
    uint16_t min = x;
    if (y < min) min = y;
    if (z < min) min = z;
    return min;
}


void reset_input_buffer(CLIENT_DATA *cd)
{
    cd->state = BLOCKED;
    cd->input.available = 0;
    memset(cd->input.buffer, 0, sizeof(cd->input.buffer));
}


//void(*ON_BYTES_RECEIVED)(void* context, const unsigned char* buffer, size_t size);
void on_bytes_received(void *context, const unsigned char *buffer, size_t size)
{
    CLIENT_DATA *client = (CLIENT_DATA *) context;
    while (size != 0)
    {
        switch (client->state)
        {
            case BLOCKED:
            {
                client->input.buffer[client->input.available] = buffer[0];
                ++client->input.available;
                client->state = LENGTH;

                ++buffer;
                --size;

                break;
            }

            case LENGTH:
            {
                client->input.buffer[client->input.available] = buffer[0];
                client->input.length = parse_int(client->input.buffer, SIZEOF_MESSAGE_LENGTH_FIELD);
                ++client->input.available;
                client->state = RECEIVING;

                ++buffer;
                --size;

                break;
            }

            case RECEIVING:
            {
                uint16_t toCopy = prv_min(
                    (uint16_t)size, // remaining source buffer size
                    sizeof(client->input.buffer) - client->input.available, // remaining destination buffer size
                    client->input.length + SIZEOF_MESSAGE_LENGTH_FIELD - client->input.available // remaining message size
                    );
                if (toCopy == 0)
                {
                    LogInfo(" on_bytes_received buffer argument is larger than destination");
                    size = 0;
                    reset_input_buffer(client);
                }

                else
                {
                    memcpy(&(client->input.buffer[client->input.available]), buffer, toCopy);
                    client->input.available += toCopy;

                    buffer += toCopy;
                    size -= toCopy;

                    if ((client->input.available - SIZEOF_MESSAGE_LENGTH_FIELD) == client->input.length)
                    {
                        LogInfo(" on_bytes_received - msgLength: %d\n", client->input.length);
                        lwm2m_handle_packet(client->session, &(client->input.buffer[SIZEOF_MESSAGE_LENGTH_FIELD]), client->input.length, client->ioHandle);
                        reset_input_buffer(client);
                    }
                }

                break;
            }

            default:
            {
                LogError("  Illegal client state.");
                size = 0;

                break;
            }
        }
    }
}


static void print_indent(FILE *stream, int num)
{
    int i;

    for (i = 0; i < num; i++)
        fprintf(stream, "    ");
}


static void output_buffer(FILE *stream, uint8_t *buffer, int length, int indent)
{
    int i;

    if (length == 0) fprintf(stream, "\n");

    i = 0;
    while (i < length)
    {
        uint8_t array[16];
        int j;

        print_indent(stream, indent);
        memcpy(array, buffer + i, 16);
        for (j = 0; j < 16 && i + j < length; j++)
        {
            fprintf(stream, "%02X ", array[j]);
            if (j % 4 == 3) fprintf(stream, " ");
        }

        if (length > 16)
        {
            while (j < 16)
            {
                fprintf(stream, "   ");
                if (j % 4 == 3) fprintf(stream, " ");
                j++;
            }
        }

        fprintf(stream, " ");
        for (j = 0; j < 16 && i + j < length; j++)
        {
            if (isprint(array[j])) fprintf(stream, "%c", array[j]);
            else                   fprintf(stream, ".");
        }

        fprintf(stream, "\n");
        i += 16;
    }
}


uint8_t lwm2m_buffer_send(void *context, uint8_t *buffer, size_t length, void *userData)
{
    XIO_HANDLE io_handle = (XIO_HANDLE) context;

    LogInfo("    Sending %zd bytes\n", length);
#ifdef WITH_LOGS
    output_buffer(stdout, buffer, length, 2);
#endif

    /** first, send the length of the message */
    uint8_t data[SIZEOF_MESSAGE_LENGTH_FIELD];

    data[0] = (uint8_t)(length >> 8);
    data[1] = (uint8_t)(length >> 0);

    int nbSent = xio_send(io_handle, data, sizeof(data), NULL, NULL);
    if (0 != nbSent) return COAP_500_INTERNAL_SERVER_ERROR;

    /** now the message */
    nbSent = xio_send(io_handle, buffer, length, NULL, NULL);
    if (0 != nbSent) return COAP_500_INTERNAL_SERVER_ERROR;

    return COAP_NO_ERROR;
}


bool lwm2m_session_is_equal(void *lhs,  void *rhs, void *userData)
{
    return (lhs == rhs);
}

void lwm2m_close_connection(void * sessionH, void * userData)
{
    // do nothing
}


