// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "iotdm_client.h"
#undef LOG
#include "iotdm_internal.h"
#include "device_object.h"
#include "config_object.h"
#include "firmwareupdate_object.h"
#include "lwm2mserver_object.h"

// missing from string.h because of convoluted preprocessor stuff
#ifndef WIN32
char *strdup(const char *s);
#endif // WIN32

static const char HOSTNAME_TOKEN[] = "HostName";
static const char DEVICEID_TOKEN[] = "DeviceId";
static const char DEVICEKEY_TOKEN[] = "SharedAccessKey";
int IoTHubClient_InitializeFromConnectionString(IOTHUB_CHANNEL_HANDLE iotDMClientHandle, const char *connectionString)
{
    int retValue = 0;

    if (NULL == iotDMClientHandle)
    {
        LogError("Input parameter 'clientHandle' is NULL\r\n");
        return -1;
    }

    if (NULL == connectionString)
    {
        LogError("Input parameter 'connectionString' is NULL\r\n");
        return -1;
    }

    STRING_HANDLE connString = STRING_construct(connectionString);
    if (NULL == connString)
    {
        LogError("Error constructing String.\r\n");
        return -2;
    }

    STRING_TOKENIZER_HANDLE tokenizer = STRING_TOKENIZER_create(connString);
    if (NULL == tokenizer)
    {
        LogError("Error constructing String Tokenizer.\r\n");
        STRING_delete(connString);

        return -3;
    }

    STRING_HANDLE tokenString = STRING_new();
    STRING_HANDLE valueString = STRING_new();
    STRING_HANDLE hostNameString = STRING_new();
    STRING_HANDLE hostSuffix = STRING_new();
    STRING_HANDLE deviceIdString = STRING_new();
    STRING_HANDLE deviceKeyString = STRING_new();
    if ((NULL == tokenString) || (NULL == valueString) || (NULL == hostNameString) || (NULL == deviceIdString) || (NULL == deviceKeyString))
    {
        LogError("Error constructing String\r\n");
        retValue = -4;
    }

    else
    {
        CLIENT_DATA *cd = (CLIENT_DATA *)iotDMClientHandle;
        while ((STRING_TOKENIZER_get_next_token(tokenizer, tokenString, "=") == 0))
        {
            if (STRING_TOKENIZER_get_next_token(tokenizer, valueString, ";") != 0)
            {
                LogError("Tokenizer error\r\n");
                retValue = -5;

                break;
            }
            const char* s_token = STRING_c_str(tokenString);
            if (0 == strcmp(s_token, HOSTNAME_TOKEN))
            {
                STRING_TOKENIZER_HANDLE hTokenizer = STRING_TOKENIZER_create(valueString);
                if (NULL == hTokenizer)
                {
                    LogError("Tokenizer Error\r\n");
                    break;
                }

                cd->config.iotHubName = strdup(STRING_c_str(valueString));
                while (0 == STRING_TOKENIZER_get_next_token(hTokenizer, hostSuffix, "."));

                cd->config.iotHubSuffix = strdup(STRING_c_str(hostSuffix));
                STRING_TOKENIZER_destroy(hTokenizer);
            }

            else if (0 == strcmp(s_token, DEVICEID_TOKEN))
            {
                cd->config.deviceId = strdup(STRING_c_str(valueString));
            }

            else if (0 == strcmp(s_token, DEVICEKEY_TOKEN))
            {
                cd->config.deviceKey = strdup(STRING_c_str(valueString));
            }

            else
            {
                retValue = -8;
            }
        }
    }

    if (deviceKeyString != NULL)
        STRING_delete(deviceKeyString);
    if (deviceIdString != NULL)
        STRING_delete(deviceIdString);
    if (hostNameString != NULL)
        STRING_delete(hostNameString);
    if (hostSuffix != NULL)
        STRING_delete(hostSuffix);
    if (valueString != NULL)
        STRING_delete(valueString);
    if (tokenString != NULL)
        STRING_delete(tokenString);

    STRING_delete(connString);
    STRING_TOKENIZER_destroy(tokenizer);

    return retValue;
}


static const uint16_t msft_server_ID = 124;

/**
 *
 */
static IOTHUB_CHANNEL_HANDLE create_iothub_handle()
{
    IOTHUB_CLIENT_RESULT result = IOTHUB_CLIENT_OK;

    CLIENT_DATA *returnValue = (CLIENT_DATA *)lwm2m_malloc(sizeof(CLIENT_DATA));
    if (NULL != returnValue)
    {
        memset(returnValue, 0, sizeof(CLIENT_DATA));

        returnValue->push_event_lock = Lock_Init();
        if (returnValue->push_event_lock == NULL)
        {
            lwm2m_free(returnValue);
            returnValue = NULL;
        }
        else
        {
            returnValue->push_event_condition = Condition_Init();
            if (returnValue->push_event_condition == NULL)
            {
                Lock_Deinit(returnValue->push_event_lock);
                lwm2m_free(returnValue);
                returnValue = NULL;
            }
        }
    }

    return (IOTHUB_CHANNEL_HANDLE)returnValue;
}

static void destroy_iothub_handle(IOTHUB_CHANNEL_HANDLE iotDMClientHandle)
{
    if (iotDMClientHandle != NULL)
    {
        CLIENT_DATA *cd = (CLIENT_DATA *) iotDMClientHandle;
        Condition_Deinit(cd->push_event_condition);
        Lock_Deinit(cd->push_event_lock);
        lwm2m_free(iotDMClientHandle);
    }
}

//typedef void * (*lwm2m_connect_server_callback_t)(uint16_t secObjInstID, void * userData);
void* lwm2m_connect_server(uint16_t secObjInstID, void* userData)
{
    LogError("lwm2m_connect_server_callback called, but not implemented");
    abort();
}

IOTHUB_CHANNEL_HANDLE IoTHubClient_DM_Open(const char *connectionString, IOTHUB_TRANSPORT transport)
{
    IOTHUB_CHANNEL_HANDLE *returnValue = create_iothub_handle();
    if (IoTHubClient_InitializeFromConnectionString(returnValue, connectionString) != 0)
    {
        destroy_iothub_handle(returnValue);
        returnValue = NULL;
    }
    else
    {
        CLIENT_DATA *cd = (CLIENT_DATA *)returnValue;
        LogInfo("HostName: %s\r\n", cd->config.iotHubName);
    }

    return returnValue;
}


IOTHUB_CLIENT_RESULT IoTHubClient_DM_CreateDefaultObjects(IOTHUB_CHANNEL_HANDLE iotDMClientHandle)
{
    IOTHUB_CLIENT_RESULT result = IOTHUB_CLIENT_OK;
    
    LogInfo("prepare the LWM2M Server Object\r\n");
    result = create_lwm2mserver_object(iotDMClientHandle, NULL);
    if (result != IOTHUB_CLIENT_OK)
    {
        LogError("failure to add the LWM2M Server object for client: %p\r\n", iotDMClientHandle);

        return result;
    }

    LogInfo("prepare the Device Object\r\n");
    result = create_device_object(iotDMClientHandle, NULL);
    if (result != IOTHUB_CLIENT_OK)
    {
        LogError("failure to add the Device object for client: %p\r\n", iotDMClientHandle);

        return result;
    }

    LogInfo("prepare the Firmware Update Object\r\n");
    result = create_firmwareupdate_object(iotDMClientHandle, NULL);
    if (result != IOTHUB_CLIENT_OK)
    {
        LogError("failure to add the Firmware Update object for client: %p\r\n", iotDMClientHandle);

        return result;
    }

    LogInfo("prepare the Config Object\r\n");
    result = create_config_object(iotDMClientHandle, NULL);
    if (result != IOTHUB_CLIENT_OK)
    {
        LogError("failure to add the Config object for client: %p\r\n", iotDMClientHandle);

        return result;
    }

    return result;
}


static COND_RESULT tickle_event_condition(CLIENT_DATA *client)
{
    Lock(client->push_event_lock);
    COND_RESULT retValue = Condition_Post(client->push_event_condition);
    Unlock(client->push_event_lock);

    return retValue;
}


void IoTHubClient_DM_Close(IOTHUB_CHANNEL_HANDLE iotDMClientHandle)
{
    if (NULL != iotDMClientHandle)
    {
        CLIENT_DATA *client = (CLIENT_DATA *)iotDMClientHandle;

        tickle_event_condition(client);
        client->state = SHUTTING_DOWN;

        lwm2m_close(client->session);
        client->session = NULL;

        xio_destroy(client->ioHandle);
        client->ioHandle = NULL;

        if (NULL != client->config.deviceId)
        {
            free((void *)(client->config.deviceId));
            client->config.deviceId = NULL;
        }

        if (NULL != client->config.deviceKey)
        {
            free((void *)(client->config.deviceKey));
            client->config.deviceKey = NULL;
        }

        if (NULL != client->config.iotHubName)
        {
            free((void *)(client->config.iotHubName));
            client->config.iotHubName = NULL;
        }

        if (NULL != client->config.iotHubSuffix)
        {
            free((void *)(client->config.iotHubSuffix));
            client->config.iotHubSuffix = NULL;
        }

        lwm2m_free(client);
    }

    platform_deinit();
}

typedef struct
{
    CLIENT_DATA* client;
    ON_DM_CONNECT_COMPLETE onComplete;
    void* context;
} ON_REGISTER_COMPLETE_CONTEXT;

void on_register_complete(IOTHUB_CLIENT_RESULT result, void* context)
{
    ON_REGISTER_COMPLETE_CONTEXT* registerContext = (ON_REGISTER_COMPLETE_CONTEXT*)context;

    if (result == IOTHUB_CLIENT_OK)
    {
        registerContext->client->lastnotify_time = get_time(NULL);
    }

    registerContext->onComplete(result, registerContext->context);
    free(context);
}

typedef struct oids_found
{
    bool server;
    bool device;
} oids_found;

static bool find_mandatory_objects(uint16_t oid, void *ctx)
{
    oids_found *found = (oids_found*)ctx;

    if (oid == LWM2M_SERVER_OBJECT_ID) found->server = true;
    if (oid == LWM2M_DEVICE_OBJECT_ID) found->device = true;

    return true;
}

static bool mandatory_objects_exist()
{
    oids_found found = { false, false };

    if (for_each_oid(find_mandatory_objects, &found))
    {
        return (found.server && found.device);
    }

    return false;
}

IOTHUB_CLIENT_RESULT IoTHubClient_DM_Connect(IOTHUB_CHANNEL_HANDLE iotDMClientHandle, ON_DM_CONNECT_COMPLETE onComplete, void* callbackContext)
{
    if (NULL == iotDMClientHandle)
    {
        LogError("Null client handle\r\n");
        return IOTHUB_CLIENT_INVALID_ARG;
    }

    if (!mandatory_objects_exist())
    {
        LogError("    one or more of the minimum required lwm2m objects is missing.\n");
        return IOTHUB_CLIENT_ERROR;
    }

    platform_init();

#if defined(WIN32)
#pragma warning(push)
#pragma warning(disable:4113)
#endif

    CLIENT_DATA *client = (CLIENT_DATA *)iotDMClientHandle;
    client->session = lwm2m_init(client);

#if defined(WIN32)
#pragma warning(pop)
#endif

    if (NULL == client->session)
    {
        LogError("    failed to initiate a client lwm2m session.\n");
        return IOTHUB_CLIENT_ERROR;
    }

    lwm2m_object_t *three[3] = {
        make_security_object(msft_server_ID, client->config.iotHubName, false),
        make_server_object(msft_server_ID, 300, false),
        make_global_object(client->session)
    };
    int result = lwm2m_configure(client->session, client->config.deviceId, NULL, NULL, 3, &three[0]);
    if (0 != result)
    {
        LogError("    failed to configure lwm2m session\n");
        return IOTHUB_CLIENT_ERROR;
    }

    result = lwm2m_start(client->session);
    if (0 != result)
    {
        LogError("    failed to start lwm2m session\n");
        return IOTHUB_CLIENT_ERROR;
    }

    ON_REGISTER_COMPLETE_CONTEXT* context = malloc(sizeof(ON_REGISTER_COMPLETE_CONTEXT));
    if (!context)
    {
        LogError("    failed to allocate context object\n");
        return IOTHUB_CLIENT_ERROR;
    }

    context->client = client;
    context->onComplete = onComplete;
    context->context = callbackContext;

    return iotdmc_register(client, on_register_complete, context);
}


IOTHUB_CLIENT_RESULT wake_main_dm_thread(IOTHUB_CHANNEL_HANDLE iotDMClientHandle)
{
    IOTHUB_CLIENT_RESULT result = IOTHUB_CLIENT_ERROR;

    if (NULL != iotDMClientHandle)
    {
        if (tickle_event_condition((CLIENT_DATA *)iotDMClientHandle) == COND_OK)
        {
            result = IOTHUB_CLIENT_OK;
        }
    }

    return result;
}


/**
 * This function will return FALSE if the server refuses to accept the client's request to connect,
 * or when the client is shutting down. All other states indicate happy.
 */
bool IoTHubClient_DM_DoWork(IOTHUB_CHANNEL_HANDLE iotDMClientHandle)
{
    bool retValue;
    if (NULL == iotDMClientHandle)
    {
        LogError("Null client handle\r\n");
        retValue = false;
    }

    else
    {
        CLIENT_DATA *client = (CLIENT_DATA *)iotDMClientHandle;

        if (client->state == SHUTTING_DOWN)
        {
            LogInfo("Client is shutting down.");

            return false;
        }

        /* check for pending requests. */
        xio_dowork(client->ioHandle);

        /**
         * LOOK: This code will only work for simple clients. The cases for bootstrap must be considered carefully.
         */
        switch (client->session->serverList->status)
        {
            case STATE_REGISTERED:
                /* process any updates that are due. */
                signal_all_resource_changes();

                time_t now = lwm2m_gettime();
                time_t timeout = 60;
                observe_step(client->session, now, &timeout);
                transaction_step(client->session, now, &timeout);

                retValue = true;
                break;

            case STATE_REG_FAILED:
                retValue = false;
                LogError("Registration request failed.");

                break;

            case STATE_REG_PENDING:
                retValue = true;
                LogInfo("Registration request is pending...");

                break;

            default:
                retValue = true;
                LogInfo("LWM2M Session Connection State is 'other'.");

                break;
        }
    }

    return retValue;
}


typedef struct CN_RESULT_TAG
{
    int          connectionResult;
    LOCK_HANDLE  my_lock;
} CN_RESULT;


void on_dm_connect_complete(IOTHUB_CLIENT_RESULT result, void* context)
{
    if (result != IOTHUB_CLIENT_OK)
    {
        LogError("failed to connect to the service, error=%d", result);
    }

    if (NULL == context)
    {
        LogInfo("on_dm_connect_complete is given a NULL argument");
    }

    else
    {
        CN_RESULT *cnResultPtr = (CN_RESULT *) context;
        Lock(cnResultPtr->my_lock);
        cnResultPtr->connectionResult = result;
        Unlock(cnResultPtr->my_lock);
    }
}


/***------------------------------------------------------------------- */
IOTHUB_CLIENT_RESULT IoTHubClient_DM_Start(IOTHUB_CHANNEL_HANDLE iotDMClientHandle)
{
    CLIENT_DATA *cd = (CLIENT_DATA *) iotDMClientHandle;
    CN_RESULT    cnResult;

    cnResult.connectionResult = IOTHUB_CLIENT_OK;
    cnResult.my_lock = Lock_Init();

    IOTHUB_CLIENT_RESULT result = IoTHubClient_DM_Connect(iotDMClientHandle, on_dm_connect_complete, &cnResult);
    while (result == IOTHUB_CLIENT_OK)
    {
        // If the asynchronous call to IoTHubClient_DM_Connect above eventually fails,
        // then exit this blocking function. If the connection is still pending, or if the
        // connection was successful, continue calling IoTHubClient_DM_DoWork in a loop.
        Lock(cnResult.my_lock);
        result = cnResult.connectionResult;
        Unlock(cnResult.my_lock);

        if (result == IOTHUB_CLIENT_OK)
        {
            if (IoTHubClient_DM_DoWork(iotDMClientHandle))
            {
                Lock(cd->push_event_lock);
                if (Condition_Wait(cd->push_event_condition, cd->push_event_lock, 1000) == COND_ERROR)
                {
                    return IOTHUB_CLIENT_ERROR;
                }
                Unlock(cd->push_event_lock);
                ThreadAPI_Sleep(1000);
            }

            else
            {
                result = IOTHUB_CLIENT_ERROR;
            }
        }
    }

    Lock_Deinit(cnResult.my_lock);
    return result;
}

char * iotdm_strndup(const char *buffer, size_t length)
{
    char *out = lwm2m_malloc(length + 1);
    if (out)
    {
        memcpy(out, buffer, length);
        out[length] = 0;
    }
    return out;
}

