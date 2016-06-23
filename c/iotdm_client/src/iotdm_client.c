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
static int initialize_from_connection_string(IOTHUB_CHANNEL_HANDLE iotDMClientHandle, const char *connectionString)
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
        CLIENT_DATA *cd = (CLIENT_DATA *) iotDMClientHandle;
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

    STRING_delete(deviceKeyString);
    STRING_delete(deviceIdString);
    STRING_delete(hostNameString);
    STRING_delete(hostSuffix);
    STRING_delete(valueString);
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
    CLIENT_DATA *returnValue = (CLIENT_DATA *)lwm2m_malloc(sizeof(CLIENT_DATA));
    if (NULL == returnValue)
    {
        LogError("malloc failed");
    }

    else
    {
        memset(returnValue, 0, sizeof(CLIENT_DATA));

		returnValue->channelLock = Lock_Init();
        if (NULL == returnValue->channelLock)
        {
            lwm2m_free(returnValue);
            returnValue = NULL;
        }
	
		returnValue->sessionLock = Lock_Init();
		if (NULL == returnValue->sessionLock)
		{
			Lock_Deinit(returnValue->channelLock);
			lwm2m_free(returnValue);
			returnValue = NULL;
		}
	}

    return (IOTHUB_CHANNEL_HANDLE) returnValue;
}


static void destroy_iothub_handle(IOTHUB_CHANNEL_HANDLE iotDMClientHandle)
{
    if (iotDMClientHandle != NULL)
    {
        CLIENT_DATA *client = (CLIENT_DATA *) iotDMClientHandle;
		if (LOCK_ERROR == Lock_Deinit(client->sessionLock))
		{
			LogError("Lock_Deinit returned LOCK_ERROR");
		}

		if (LOCK_ERROR == Lock_Deinit(client->channelLock))
		{
			LogError("Lock_Deinit returned LOCK_ERROR");
		}

        lwm2m_free(iotDMClientHandle);
    }
}

//typedef void * (*lwm2m_connect_server_callback_t)(uint16_t secObjInstID, void * userData);
void* lwm2m_connect_server(uint16_t secObjInstID, void* userData)
{
    LogError("lwm2m_connect_server_callback called, but not implemented");
    abort();
}


static IOTHUB_CLIENT_RESULT create_default_lwm2m_objects(IOTHUB_CHANNEL_HANDLE iotDMClientHandle)
{
    IOTHUB_CLIENT_RESULT result = IOTHUB_CLIENT_OK;

    LogInfo("prepare the LWM2M Server Object");
    create_lwm2mserver_object(iotDMClientHandle, NULL);
    if (result != IOTHUB_CLIENT_OK != result)
    {
        LogError("failure to add the LWM2M Server object for client: %p", iotDMClientHandle);
    }

    else
    {
        set_lwm2mserver_shortserverid(0, msft_server_ID);
        set_lwm2mserver_lifetime(0, 300);
        set_lwm2mserver_binding(0, "T");

        LogInfo("prepare the Device Object");
        result = create_device_object(iotDMClientHandle, NULL);
        if (IOTHUB_CLIENT_OK != result)
        {
            LogError("failure to add the Device object for client: %p", iotDMClientHandle);

            /** TODO: */
            /** delete_lwm2mserver_object() */
        }

        else
        {
            LogInfo("prepare the Firmware Update Object");
            result = create_firmwareupdate_object(iotDMClientHandle, NULL);
            if (IOTHUB_CLIENT_OK != result)
            {
                LogError("failure to add the Firmware Update object for client: %p", iotDMClientHandle);

                /** TODO: */
                /** delete_lwm2mserver_object() */
                /** delete_firmwareupdate_object() */
            }

            else
            {
                LogInfo("prepare the Config Object");
                result = create_config_object(iotDMClientHandle, NULL);
                if (IOTHUB_CLIENT_OK != result)
                {
                    LogError("failure to add the Config object for client: %p", iotDMClientHandle);

                    /** TODO: */
                    /** delete_lwm2mserver_object() */
                    /** delete_firmwareupdate_object() */
                    /** delete_config_object() */
                }
            }
        }
    }

    return result;
}


IOTHUB_CHANNEL_HANDLE IoTHubClient_DM_Open(const char *connectionString, IOTHUB_TRANSPORT transport)
{
    CLIENT_DATA *returnValue = (CLIENT_DATA *) create_iothub_handle();
    if (NULL == returnValue)
    {
        LogError("failed to create iothub handle");
    }

    else
    {
        if (LOCK_ERROR == Lock(returnValue->channelLock))
        {
            LogError("lock returned LOCK_ERROR");
        }

        else
        {
            bool bHappy;
            if (initialize_from_connection_string(returnValue, connectionString) == 0)
            {
                LogInfo("HostName: %s", returnValue->config.iotHubName);

                LogInfo("prepare LWM2M objects");
                if (IOTHUB_CLIENT_OK == create_default_lwm2m_objects(returnValue))
                {
                    bHappy = true;
                }

                else
                {
                    LogError("failure to create LWM2M objects for client");

                    bHappy = false;
                }
            }

            else
            {
                LogError("failed to initialize from connection string");

                bHappy = false;
            }

            if (LOCK_ERROR == Unlock(returnValue->channelLock))
            {
                LogError("Unlock returned LOCK_ERROR");
            }

            if (false == bHappy)
            {
                destroy_iothub_handle(returnValue);
                returnValue = NULL;
            }
        }
    }

    return returnValue;
}


void IoTHubClient_DM_Close(IOTHUB_CHANNEL_HANDLE iotDMClientHandle)
{
    if (NULL != iotDMClientHandle)
    {
        CLIENT_DATA *client = (CLIENT_DATA *) iotDMClientHandle;

        if (LOCK_ERROR == Lock(client->channelLock))
        {
            LogError("Lock returned LOCK_ERROR");
        }

        else
        {
            if (NULL != client->session)
            {
                lwm2m_close(client->session);
                client->session = NULL;
            }

            if (NULL != client->ioHandle)
            {
                xio_destroy(client->ioHandle);
                client->ioHandle = NULL;
            }

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

            if (LOCK_ERROR == Unlock(client->channelLock))
            {
                LogError("Unlock return LOCK_ERROR");
            }
        }
    }
}


typedef struct
{
    CLIENT_DATA *client;
    ON_DM_CONNECT_COMPLETE onComplete;
    void *context;
} ON_REGISTER_COMPLETE_CONTEXT;

void on_register_complete(IOTHUB_CLIENT_RESULT result, void *context)
{
    ON_REGISTER_COMPLETE_CONTEXT *registerContext = (ON_REGISTER_COMPLETE_CONTEXT *) context;
    if (registerContext->onComplete)
    {
        registerContext->onComplete(result, registerContext->context);
    }
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

    IOTHUB_CLIENT_RESULT  retValue;
    CLIENT_DATA          *client = (CLIENT_DATA *)iotDMClientHandle;
    if (LOCK_ERROR == Lock(client->channelLock))
    {
        LogError("Lock returned LOCK_ERROR");
    }

    else
    {
        client->session = lwm2m_init(client);
        if (NULL == client->session)
        {
            LogError("    failed to initiate a client lwm2m session.\n");

            retValue = IOTHUB_CLIENT_ERROR;
        }

        else
        {
            // BKTODO: detangle object registration even more
            lwm2m_object_t *objs[5] = {
                make_security_object(msft_server_ID, client->config.iotHubName, false),
                make_global_object(client, OID_LWM2MSERVER, 0),
                make_global_object(client, OID_DEVICE, 0),
                make_global_object(client, OID_CONFIG, 0),
                make_global_object(client, OID_FIRMWAREUPDATE, 0)
            };

            int result = lwm2m_configure(client->session, client->config.deviceId, NULL, NULL, 5, &objs[0]);
            if (0 != result)
            {
                LogError("    failed to configure lwm2m session");

                retValue = IOTHUB_CLIENT_ERROR;
            }

            else
            {
                result = lwm2m_start(client->session);
                if (0 != result)
                {
                    LogError("    failed to start lwm2m session");

                    retValue = IOTHUB_CLIENT_ERROR;
                }

                else
                {
                    ON_REGISTER_COMPLETE_CONTEXT *context = (ON_REGISTER_COMPLETE_CONTEXT *)malloc(sizeof(ON_REGISTER_COMPLETE_CONTEXT));
                    if (NULL == context)
                    {
                        LogError("    failed to allocate context object");

                        retValue = IOTHUB_CLIENT_ERROR;
                    }

                    else
                    {
                        context->client = client;
                        context->onComplete = onComplete;
                        context->context = callbackContext;

                        retValue = iotdmc_register(client, on_register_complete, context);
                    }
                }
            }
        }

        if (LOCK_ERROR == Unlock(client->channelLock))
        {
            LogError("Unlock returned LOKC_ERROR");
        }
    }

    return retValue;
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
        CLIENT_DATA *client = (CLIENT_DATA *) iotDMClientHandle;
		if (LOCK_ERROR == Lock(client->channelLock))
		{
			LogError("lock returned LOCK_ERROR");
			retValue = false;
		}

		else if (NULL == client->session)
        {
            LogError("Uninitialized lwm2m session");
			if (LOCK_ERROR == Unlock(client->channelLock))
			{
				LogError("unlock returned LOCK_ERROR");
			}

            retValue = false;
        }

        else
        {
            /* check for pending requests. */
            xio_dowork(client->ioHandle);

            /**
             * LOOK: This code will only work for simple clients. The cases for bootstrap must be considered carefully.
             */
            switch (client->session->serverList->status)
            {
                case STATE_REGISTERED:
                    /* process any updates that are due. */
					Lock(client->sessionLock);
                    signal_all_resource_changes();
					Unlock(client->sessionLock);

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

			if (LOCK_ERROR == Unlock(client->channelLock))
			{
				LogError("unlock returned LOCK_ERROR");
			}
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
        if (LOCK_ERROR == Lock(cnResultPtr->my_lock))
        {
            LogError("Lock returned LOCK_ERROR");
        }

        else
        {
            cnResultPtr->connectionResult = result;
            if (LOCK_ERROR == Unlock(cnResultPtr->my_lock))
            {
                LogError("Unlock returned LOCK_ERROR");
            }
        }
    }
}


/***------------------------------------------------------------------- */
IOTHUB_CLIENT_RESULT IoTHubClient_DM_Start(IOTHUB_CHANNEL_HANDLE iotDMClientHandle)
{
    CN_RESULT    cnResult = { IOTHUB_CLIENT_OK, Lock_Init() };

    IOTHUB_CLIENT_RESULT result = IoTHubClient_DM_Connect(iotDMClientHandle, on_dm_connect_complete, &cnResult);
    while (result == IOTHUB_CLIENT_OK)
    {
        // If the asynchronous call to IoTHubClient_DM_Connect above eventually fails,
        // then exit this blocking function. If the connection is still pending, or if the
        // connection was successful, continue calling IoTHubClient_DM_DoWork in a loop.
        if (LOCK_ERROR == Lock(cnResult.my_lock))
        {
            LogError("Lock returned LOCK_ERROR");

            result = IOTHUB_CLIENT_ERROR;
        }

        else
        {
            result = cnResult.connectionResult;
            if (LOCK_ERROR == Unlock(cnResult.my_lock))
            {
                LogError("Unlock returned LOCK_ERROR");
            }

            if (result == IOTHUB_CLIENT_OK)
            {
                if (IoTHubClient_DM_DoWork(iotDMClientHandle))
                {
                    ThreadAPI_Sleep(1000);
                }

                else
                {
                    result = IOTHUB_CLIENT_ERROR;
                }
            }
        }
    }

    Lock_Deinit(cnResult.my_lock);
    return result;
}

bool IotHubClient_DM_EnterCriticalSection(IOTHUB_CHANNEL_HANDLE iotDMClientHandle)
{
	bool retValue;
	if (NULL == iotDMClientHandle)
	{
		retValue = false;
	}

	else
	{
		CLIENT_DATA *client = (CLIENT_DATA *)iotDMClientHandle;
		retValue = LOCK_OK == Lock(client->sessionLock);
	}

	return retValue;
}

bool IotHubClient_DM_LeaveCriticalSection(IOTHUB_CHANNEL_HANDLE iotDMClientHandle)
{
	bool retValue;
	if (NULL == iotDMClientHandle)
	{
		retValue = false;
	}

	else
	{
		CLIENT_DATA *client = (CLIENT_DATA *)iotDMClientHandle;
		retValue = LOCK_OK == Unlock(client->sessionLock);
	}

	return retValue;
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

