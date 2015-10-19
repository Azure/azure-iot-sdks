// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#include "gballoc.h"

#include "mqttapi.h"
#include "iot_logging.h"
#include "strings.h"
#include "doublylinkedlist.h"
#include "lock.h"
#include "crt_abstractions.h"
#include "sastoken.h"

#include "MQTTClient.h"

#define SUBSCRIBEQOS 0
#define SENDMESSAGEQOS 1

#define EPOCH_TIME_T_VALUE (time_t)0
#define SAS_TOKEN_DEFAULT_LIFETIME (3600)
#define SAS_TOKEN_DEFAULT_REFRESH_LINE (600)

DEFINE_ENUM_STRINGS(MQTTAPI_RESULT, MQTTAPI_RESULT_VALUES);

typedef struct MQTTAPI_TOPIC_HANDLE_DATA_TAG
{
	STRING_HANDLE topicName;
	MQTTAPI_HANDLE mqttApiInstance;
} MQTTAPI_TOPIC_HANDLE_DATA, *PMQTTAPI_TOPIC_HANDLE_DATA;

typedef struct MQTTAPI_MESSAGE_RECEIVED_LIST_TAG
{
	MQTTAPI_Message* messageReceived;	
	DLIST_ENTRY entry;
}MQTTAPI_MESSAGE_RECEIVED_LIST;

typedef struct MQTTAPI_MESSAGE_SEND_LIST_TAG
{
	STRING_HANDLE topicName;
	void* context;
	MQTTClient_message* messageToSend;
	DLIST_ENTRY entry;
	MQTTClient_deliveryToken dt;
}MQTTAPI_MESSAGE_SEND_LIST;

typedef struct MQTTTAPI_HANDLE_DATA_TAG
{
	STRING_HANDLE device_id;

	STRING_HANDLE device_key;

	STRING_HANDLE sasTokenSr;
		
	// Holds the expiry (seconds since the epoch) that was last used for the SAS put to the CBS
	size_t lastExpiryUsed;

	// This will hold the amount of time (in seconds) that a SAS Token will have for it's expiration.
	size_t sasTokenLifetime;

	// This will hold how many seconds before the expiration of a SAS token we will try to start renewing
	// it.
	size_t sasRefreshLine;

	bool connected;

	bool subscribed;

	PMQTTAPI_TOPIC_HANDLE_DATA subscribedTopicHandleData;

	MQTTAPI_DeliveryComplete* dcCallback;
	
	MQTTAPI_MessageArrived* maCallBack;

	void* maCallbackContext;

	MQTTClient client; //Saved instance

	DLIST_ENTRY messagesReceived; 

	DLIST_ENTRY messagesToSend;

	DLIST_ENTRY messagesSent;

	LOCK_HANDLE LockHandle;
} MQTTTAPI_HANDLE_DATA, *PMQTTTAPI_HANDLE_DATA;



static bool connectToMQTTServer(MQTTClient client, const char* username, const char* password)
{
	bool result;
	MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
	MQTTClient_SSLOptions sslopts = MQTTClient_SSLOptions_initializer;
	int rc;
	conn_opts.keepAliveInterval = 20;
	conn_opts.cleansession = 1;
	conn_opts.MQTTVersion = 4;
	conn_opts.username = username;
	conn_opts.password = password;

	conn_opts.ssl = &sslopts;
	sslopts.enableServerCertAuth = 0;

	/* Codes_SRS_MQTTAPI_04_024: [MQTTAPI_Create shall call underlying library connection functions to establish connection with the server.]  */
	if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
	{
		LogError("Failed to connect, MQTTClient_connect returned: %d\r\n", rc);
		result = false;
	}
	else
	{
		result = true;
	}

	return result;
}


static bool checkAndTryToConnect(PMQTTTAPI_HANDLE_DATA mqttApiState)
{
	bool result;

	if (mqttApiState != NULL)
	{
		if (!mqttApiState->connected)
		{
			/* Codes_SRS_MQTTAPI_04_062: [MQTTAPI_create shall generate SAS Token with an expiring time of 1 hour.] */
			size_t secondsSinceEpoch = (size_t)(difftime(get_time(NULL), EPOCH_TIME_T_VALUE) + 0);
			size_t possibleNewExpiry = secondsSinceEpoch + mqttApiState->sasTokenLifetime;
			STRING_HANDLE zeroLengthString;

			if ((zeroLengthString = STRING_new()) == NULL)
			{
				LogError("Could not generate zeroLenghtString for skn. \r\n");
				result = false;
			}
			else
			{
				STRING_HANDLE newSASToken = SASToken_Create(mqttApiState->device_key, mqttApiState->sasTokenSr, zeroLengthString, possibleNewExpiry);

				if (newSASToken == NULL)
				{
					LogError("Could not generate a SAS Token\r\n");
					result = false;
				}
				else
				{
					/* Codes_SRS_MQTTAPI_04_024: [MQTTAPI_Create shall call underlying library connection functions to establish connection with the server.]  */
					if (!connectToMQTTServer(mqttApiState->client, STRING_c_str(mqttApiState->device_id), STRING_c_str(newSASToken)))
					{
						/* Tests_SRS_MQTTAPI_04_025: [If connection fails MQTTAPI_Create shall return NULL. */
						result = false;
					}
					else
					{
						mqttApiState->connected = true;
						mqttApiState->lastExpiryUsed = possibleNewExpiry;
						result = true;
					}
					STRING_delete(newSASToken);
				}
				STRING_delete(zeroLengthString);
			}
		}
		else
		{
			//Here is the point that we need to check if we need to disconnect, generate a new SAS Token and Connect Again. 
			size_t secondsSinceEpoch;
			int differenceWithLastExpiry;
			secondsSinceEpoch = (size_t)(difftime(get_time(NULL), EPOCH_TIME_T_VALUE) + 0);
			differenceWithLastExpiry = mqttApiState->lastExpiryUsed - secondsSinceEpoch;

			/* Codes_SRS_MQTTAPI_04_063: [DoWork shall check the SAS Token Expiration time for the current connection. If the token is near to expire and there is no pending item, it shall disconnect, generate a new SAS Token with 1 hour expiration and connect again.] */
			if ((differenceWithLastExpiry <= 0)	|| 
				(((size_t)differenceWithLastExpiry) < mqttApiState->sasRefreshLine && DList_IsListEmpty(&(mqttApiState->messagesSent))
				)
				) // Within refresh minutes (or past if negative) of the expiration
			{
				/* First Disconnect */
				/* Codes_SRS_MQTTAPI_04_064: [If the token is expired we shall disconnect, signal that all pending message are failed due to Token Expired.] */
				if (MQTTClient_disconnect(mqttApiState->client, 0) != MQTTCLIENT_SUCCESS)
				{
					LogError("Token is Expired or about to be expired and Disconnect Failed. \r\n");
				}
				else
				{
					size_t possibleNewExpiry = secondsSinceEpoch + mqttApiState->sasTokenLifetime;
					STRING_HANDLE zeroLengthString;

					if ((zeroLengthString = STRING_new()) == NULL)
					{
						LogError("Could not generate zeroLenghtString for skn. \r\n");
						result = false;
					}
					else
					{
						STRING_HANDLE newSASToken = SASToken_Create(mqttApiState->device_key, mqttApiState->sasTokenSr, zeroLengthString, possibleNewExpiry);
						mqttApiState->connected = false;

						if (newSASToken == NULL)
						{
							LogError("Could not generate a SAS Token\r\n");
							result = false;
						}
						else
						{
							/* Codes_SRS_MQTTAPI_04_024: [MQTTAPI_Create shall call underlying library connection functions to establish connection with the server.]  */
							if (!connectToMQTTServer(mqttApiState->client, STRING_c_str(mqttApiState->device_id), STRING_c_str(newSASToken)))
							{
								/* Tests_SRS_MQTTAPI_04_025: [If connection fails MQTTAPI_Create shall return NULL. */
								result = false;
							}
							else
							{
								mqttApiState->connected = true;
								mqttApiState->lastExpiryUsed = possibleNewExpiry;
								if (mqttApiState->subscribed)
								{
									int rc;
									LogInfo("Trying to Subscribe after reconnect.\r\n");

									if ((rc = MQTTClient_subscribe(mqttApiState->client, STRING_c_str(mqttApiState->subscribedTopicHandleData->topicName), SUBSCRIBEQOS)) != MQTTCLIENT_SUCCESS)
									{
										LogError("Could not subscribe again. Won't be able to receive messages. Error Code: %d.\r\n", rc);
									}
									else
									{
										LogInfo("Subscribed succesfully after reconnect..\r\n");
									}
								}
								result = true;
							}
							STRING_delete(newSASToken);
						}
						STRING_delete(zeroLengthString);
					}
				}
			}


			result = true;
		}
	}
	else
	{
		LogError("Invalid Arg. Handle is NULL. \r\n");
		result = false;
	}

	return result;
}


static void connectionLost(void *context, char *cause)
{
	PMQTTTAPI_HANDLE_DATA transportState = (PMQTTTAPI_HANDLE_DATA)context;
	LogError("Lost Connection, reseting Connection State. Cause: %s \r\n", cause);

	if (transportState != NULL)
	{
		transportState->connected = false;
	}

	LogInfo("Trying to connect again\r\n");

	if (!checkAndTryToConnect(transportState))
	{
		LogError("Could not connect again. \r\n");
	}
	else
	{
		LogInfo("Connected.\r\n");
		if (transportState->subscribed)
		{
			int rc;
			LogInfo("Trying to Subscribe after reconnect.\r\n");

			if ((rc = MQTTClient_subscribe(transportState->client, STRING_c_str(transportState->subscribedTopicHandleData->topicName), SUBSCRIBEQOS)) != MQTTCLIENT_SUCCESS)
			{
				LogError("Could not subscribe again. Won't be able to receive messages. Error Code: %d.\r\n", rc);
			}
			else
			{
				LogInfo("Subscribed succesfully after reconnect..\r\n");
			}
		}
	}

	if (cause != NULL)
	{
		MQTTClient_free(cause);
	}
}


static void delivered(void *context, MQTTClient_deliveryToken dt)
{
	PMQTTTAPI_HANDLE_DATA transportState = (PMQTTTAPI_HANDLE_DATA)context;

	if (context != NULL)
	{
		//First try to find the message on the list. 
		PDLIST_ENTRY currentListEntry;
		currentListEntry = transportState->messagesSent.Flink;

		if (currentListEntry != NULL)
		{
			while (currentListEntry != &transportState->messagesSent)
			{
				MQTTAPI_MESSAGE_SEND_LIST *mqttMessageDetailEntry = containingRecord(currentListEntry, MQTTAPI_MESSAGE_SEND_LIST, entry);

				//Identify here if the message is the one arrived. 
				if (mqttMessageDetailEntry->dt == dt)
				{
					(void)DList_RemoveEntryList(currentListEntry); //First remove the item from Waiting for Ack List.

					if (transportState->dcCallback != NULL)
					{
						/* Codes_SRS_MQTTAPI_04_009: [The context parameter is a point to the original value passed by MQTTAPI_PublishMessage, which contains message-specific context.] */
						/* Codes_SRS_MQTTAPI_04_052: [result contains the status of the message sent by publish.] */
						transportState->dcCallback(mqttMessageDetailEntry->context, MQTTAPI_CONFIRMATION_OK);
					}

					STRING_delete(mqttMessageDetailEntry->topicName);
					MQTTClient_freeMessage(&(mqttMessageDetailEntry->messageToSend));
					free(mqttMessageDetailEntry);
					break;
				}
				currentListEntry = currentListEntry->Flink;
			}
		}
		else
		{
			LogError("Error trying to access items on messagesSEnt list. Possible not initialized List.\r\n");
		}
	}
}

/* Codes_SRS_MQTTAPI_04_006: [The MQTTAPI_Message parameter contains the structure for the received message, with message payload.]  */
static int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
	MQTTAPI_MESSAGE_RECEIVED_LIST* mqttAPIMessgeEntryList;
	int result;
	PMQTTTAPI_HANDLE_DATA mqttApiHandleData = (PMQTTTAPI_HANDLE_DATA)context;
	(void)(topicLen, topicName);

	if (mqttApiHandleData == NULL)
	{
		LogError("Context parameter cannot be null. Ignoring message \r\n");
		result = 1;
	}
	else if (mqttApiHandleData->maCallBack == NULL)
	{
		/* Codes_SRS_MQTTAPI_04_061: [If maCallBack is not set, MQTTAPI_MessageArrived shall return 0 to ignore received messages.]  */
		result = 1;
	}
	else if (message == NULL)
	{
		LogError("message parameter cannot be null. Returning error, so it can be sent again.\r\n");
		result = 0;
	}
	else if ((mqttAPIMessgeEntryList = (MQTTAPI_MESSAGE_RECEIVED_LIST*)malloc(sizeof(MQTTAPI_MESSAGE_RECEIVED_LIST))) == NULL)
	{
		LogError("Memory Allocation Failure for MQTTAPI_Message entry list.\r\n");
		result = 0;
	}
	else if ((mqttAPIMessgeEntryList->messageReceived = (MQTTAPI_Message*)malloc(sizeof(MQTTAPI_Message))) == NULL)
	{
		LogError("Memory Allocation Failure for MQTTAPI_Message.\r\n");
		free(mqttAPIMessgeEntryList);
		result = 0;
	}
	else
	{
		mqttAPIMessgeEntryList->messageReceived->payloadlen = message->payloadlen;		
		
		if ((mqttAPIMessgeEntryList->messageReceived->payload = (unsigned char*)malloc(mqttAPIMessgeEntryList->messageReceived->payloadlen)) == NULL)
		{
			LogError("Memory allocation error.\r\n");
			free(mqttAPIMessgeEntryList->messageReceived);
			free(mqttAPIMessgeEntryList);
			result = 0;
		}
		else
		{
			(void)memcpy(mqttAPIMessgeEntryList->messageReceived->payload, message->payload, message->payloadlen);
			if (Lock(mqttApiHandleData->LockHandle) == LOCK_OK)
			{
				DList_InsertHeadList(&(mqttApiHandleData->messagesReceived), &(mqttAPIMessgeEntryList->entry));
				Unlock(mqttApiHandleData->LockHandle);
				result = 1;
			}
			else
			{
				LogError("Problem Aquiring Lock to add message received on the list.\r\n");
				free(mqttAPIMessgeEntryList->messageReceived->payload);
				free(mqttAPIMessgeEntryList->messageReceived);
				free(mqttAPIMessgeEntryList);
				result = 0;
			}
		}
	}

	//We just free the MQTTClient Message if it was succesfully received, otherwise MQTTClient will call this with a corrupted message.
	if (message != NULL && result == 1)
	{
		MQTTClient_freeMessage(&message);
	}

	return result;
}


MQTTAPI_HANDLE MQTTAPI_Create(const MQTTAPI_ConnectOptions* options)
{
	PMQTTTAPI_HANDLE_DATA result;

	/* Codes_SRS_MQTTAPI_04_011: [If any member of the parameter options is NULL or empty then MQTTAPI_Create shall return NULL.]  */
	if (options == NULL)
	{
		LogError("Invalid Argument. options cannot be NULL.\r\n")
		result = NULL;
	}
	else if (options->deviceId == NULL || strlen(options->deviceId) <= 0)
	{
		LogError("Invalid Argument. Device Id cannot be NULL or Empty.\r\n")
		result = NULL;
	}
	else if (options->deviceKey == NULL || strlen(options->deviceKey) <= 0)
	{
		LogError("Invalid Argument. Device Key cannot be NULL or Empty.\r\n")
		result = NULL;
	}
	else if (options->serverURI == NULL || strlen(options->serverURI) <= 0)
	{
		LogError("Invalid Argument. Device Key cannot be NULL or Empty.\r\n")
		result = NULL;
	}
	else if (options->sasTokenSr == NULL || strlen(options->sasTokenSr) <= 0)
	{
		LogError("Invalid Argument. sasTokenSr cannot be NULL or Empty.\r\n")
		result = NULL;
	}
	/* Tests_SRS_MQTTAPI_04_014: [Otherwise, MQTTAPI_Create shall return a MQTTAPI_HANDLE suitable for further calls to the module.] */
	else
	{
		result = (PMQTTTAPI_HANDLE_DATA)malloc(sizeof(MQTTTAPI_HANDLE_DATA));

		if (result != NULL)
		{
			result->connected = false;
			result->subscribed = false;
			result->dcCallback = NULL;
			result->maCallBack = NULL;
			result->maCallbackContext = NULL;
			result->subscribedTopicHandleData = NULL;
			result->sasTokenLifetime = SAS_TOKEN_DEFAULT_LIFETIME;
			result->sasRefreshLine = SAS_TOKEN_DEFAULT_REFRESH_LINE;

			DList_InitializeListHead(&(result->messagesReceived));
			DList_InitializeListHead(&(result->messagesToSend));
			DList_InitializeListHead(&(result->messagesSent));

			if ((result->LockHandle = Lock_Init()) == NULL)
			{
				LogError("Lock_Init failed\r\n");
				free(result);
				result = NULL;
			}
			/* Codes_SRS_MQTTAPI_04_012: [Parameters deviceId, deviceKey and sasTokenSr shall be saved.]  */
			else if ((result->device_id = STRING_construct(options->deviceId)) == NULL)
			{
				/* Codes_SRS_MQTTAPI_04_013: [If saving these parameters fail for any reason MQTTAPI_Create shall return NULL.] */
				LogError("Could not create deviceId String.\r\n");
				Lock_Deinit(result->LockHandle);
				free(result);
				result = NULL;
			}
			/* Codes_SRS_MQTTAPI_04_012: [Parameters deviceId, deviceKey and sasTokenSr shall be saved.]  */
			else if ((result->device_key = STRING_construct(options->deviceKey)) == NULL)
			{
				/* Codes_SRS_MQTTAPI_04_013: [If saving these parameters fail for any reason MQTTAPI_Create shall return NULL.] */
				LogError("Could not create deviceId String.\r\n");
				STRING_delete(result->device_id);
				Lock_Deinit(result->LockHandle);
				free(result);
				result = NULL;
			}
			/* Codes_SRS_MQTTAPI_04_012: [Parameters deviceId, deviceKey and sasTokenSr shall be saved.]  */
			else if ((result->sasTokenSr = STRING_construct(options->sasTokenSr)) == NULL)
			{
				LogError("Could not create sasTokenSr String.\r\n");
				STRING_delete(result->device_key);
				STRING_delete(result->device_id);
				Lock_Deinit(result->LockHandle);
				free(result);
				result = NULL;
			}
    		else if (MQTTClient_create(&result->client, options->serverURI, STRING_c_str(result->device_id),
				MQTTCLIENT_PERSISTENCE_NONE, NULL) != MQTTCLIENT_SUCCESS)
			{
				LogError("Could not create MQTT Client.\r\n");
				STRING_delete(result->sasTokenSr);
				STRING_delete(result->device_key);
				STRING_delete(result->device_id);
				Lock_Deinit(result->LockHandle);
				free(result);
				result = NULL;
			}
			/* Codes_SRS_MQTTAPI_04_004: [This function is registered when the user makes a call to MQTTAPI_SetCallbacks.] */
			/* Codes_SRS_MQTTAPI_04_005: [The context parameter is a pointer to the original value passed by MQTTAPI_SetCallbacks, which contains application-specific context.]  */
		    /* Codes_SRS_MQTTAPI_04_008: [This function is registered when the user makes a call to MQTTAPI_SetCallbacks.] */
			else if (MQTTClient_setCallbacks(result->client, result, connectionLost, msgarrvd, delivered) != MQTTCLIENT_SUCCESS)
			{
				LogError("Could not set callbacks.\r\n");
				STRING_delete(result->sasTokenSr);
				STRING_delete(result->device_key);
				STRING_delete(result->device_id);
				MQTTClient_destroy(&result->client);
				Lock_Deinit(result->LockHandle);
				free(result);
				result = NULL;
			}
			/* Codes_SRS_MQTTAPI_04_024: [MQTTAPI_Create shall call underlying library connection functions to establish connection with the server.]  */
			/* Codes_SRS_MQTTAPI_04_047: [Otherwise MQTTAPI_Create shall return a non-NULL MQTTAPI_HANDLE]  */
			else if (!checkAndTryToConnect(result))
			{
				/* Codes_SRS_MQTTAPI_04_025: [If connection fails MQTTAPI_Create shall return NULL. */
				LogError("Could not Connect to MQTT Server.\r\n");
				STRING_delete(result->sasTokenSr);
				STRING_delete(result->device_key);
				STRING_delete(result->device_id);
				MQTTClient_destroy(&result->client);
				Lock_Deinit(result->LockHandle);
				free(result);
				result = NULL;
			}

		}
		else
		{
			/* Codes_SRS_MQTTAPI_04_015: [If creating instance fails for any reason, then MQTTAPI_Create shall return NULL.]  */
			LogError("Could not create MQTT API handle data. Memory allocation failed.\r\n");
			result = NULL;
		}
	}

    return (MQTTAPI_HANDLE)result;
}

MQTTAPI_RESULT MQTTAPI_SetMessageCallback(MQTTAPI_HANDLE instance, void* context, MQTTAPI_MessageArrived* ma)
{
	MQTTAPI_RESULT result;
	/* Codes_SRS_MQTTAPI_04_016: [If instance parameter is NULL, setMessageCallback shall return MQTTAPI_INVALID_ARG.] */
	if (instance == NULL)
	{
		LogError("Invalid Argument. instance cannot be NULL.\r\n")
		result = MQTTAPI_INVALID_ARG;
	}
	else
	{
		PMQTTTAPI_HANDLE_DATA mqttapi_handle_data = (PMQTTTAPI_HANDLE_DATA)instance;
		/* Codes_SRS_MQTTAPI_04_020: [If while saving the callbacks fails setMessageCallback shall return MQTTAPI_ERROR, otherwise it shall return MQTTAPI_OK]  */
		/* Codes_SRS_MQTTAPI_04_019: [If any of the callback pointers are different than NULL it will be saved.]  */
		/* Codes_SRS_MQTTAPI_04_018: [If parameter context is not NULL, it will be saved and passed to the user when any of the callback is trigerred.]  */
		if (ma != NULL)
		{
			mqttapi_handle_data->maCallBack = ma;
			mqttapi_handle_data->maCallbackContext = context;
			result = MQTTAPI_OK;
		}
		else
		{
			LogError("Could not set because dc parameter cannot be NULL. \r\n");
			result = MQTTAPI_ERROR;
		}
	}

	return result;
}

MQTTAPI_RESULT MQTTAPI_SetDeliveryCompletedCallback(MQTTAPI_HANDLE instance, MQTTAPI_DeliveryComplete* dc)
{
	MQTTAPI_RESULT result;

	/* Codes_SRS_MQTTAPI_04_054: [If instance parameter is NULL, MQTTAPI_SetDeliveryCompletedCallback shall return MQTTAPI_INVALID_ARG.]  */
	if (instance == NULL)
	{
		LogError("Invalid Argument. instance cannot be NULL.\r\n")
		result = MQTTAPI_INVALID_ARG;
	}
	else
	{
		PMQTTTAPI_HANDLE_DATA mqttapi_handle_data = (PMQTTTAPI_HANDLE_DATA)instance;
		/* Codes_SRS_MQTTAPI_04_056: [If while saving the callbacks fails MQTTAPI_SetDeliveryCompletedCallback shall return MQTTAPI_ERROR]  */
		if (dc != NULL)
		{
			mqttapi_handle_data->dcCallback = dc;
			result = MQTTAPI_OK;
		}
		else
		{
			/* Codes_SRS_MQTTAPI_04_057: [otherwise it shall return MQTTAPI_OK]  */
			LogError("Could not set because dc parameter cannot be NULL. \r\n");
			result = MQTTAPI_ERROR;
		}
	}

	return result;
}


void MQTTAPI_Destroy(MQTTAPI_HANDLE instance)
{
	PMQTTTAPI_HANDLE_DATA mqttHandleData = (PMQTTTAPI_HANDLE_DATA)instance;

	/* Codes_SRS_MQTTAPI_04_021: [If parameter instance is NULL then MQTTAPI_Destroy shall take no action.] */
	if (mqttHandleData != NULL)
	{
		/* Codes_SRS_MQTTAPI_04_022: [MQTTAPI_Destroy shall free all resources used by MQTTAPI_HANDLE.]  */
		STRING_delete(mqttHandleData->device_id);
		STRING_delete(mqttHandleData->device_key);
		STRING_delete(mqttHandleData->sasTokenSr);
		/* Codes_SRS_MQTTAPI_04_049: [If the instance is connected, MQTTAPI_Destroy shall disconnect the instance] */
		if (mqttHandleData->subscribed)
		{
			MQTTAPI_Unsubscribe(mqttHandleData->subscribedTopicHandleData);
		}

		if (mqttHandleData->connected)
		{
			MQTTClient_disconnect(mqttHandleData->client, 0);
		}

		/* Codes_SRS_MQTTAPI_04_053: [If there are messages to be sent MQTTAPI_Destroy shall signal the user that the message couldn’t be sent by reason of MQTTAPI_CONFIRMATION_BECAUSE_DESTROY]  */
		{
			PDLIST_ENTRY received;
			while ((received = DList_RemoveHeadList(&(mqttHandleData->messagesReceived))) != &(mqttHandleData->messagesReceived))
			{
				MQTTAPI_MESSAGE_RECEIVED_LIST* temp = containingRecord(received, MQTTAPI_MESSAGE_RECEIVED_LIST, entry);
				if (mqttHandleData->maCallBack != NULL)
				{
					if (!mqttHandleData->maCallBack(mqttHandleData->maCallbackContext, temp->messageReceived))
					{
						LogError("Client could not handle message, dropping it.");
					}
				}

				free(temp->messageReceived->payload);
				free(temp->messageReceived);
				free(temp);
			}
		}

		{
			PDLIST_ENTRY messageToSend;
			while ((messageToSend = DList_RemoveHeadList(&(mqttHandleData->messagesToSend))) != &(mqttHandleData->messagesToSend))
			{
				MQTTAPI_MESSAGE_SEND_LIST* temp = containingRecord(messageToSend, MQTTAPI_MESSAGE_SEND_LIST, entry);

				if (mqttHandleData->dcCallback != NULL)
				{
					mqttHandleData->dcCallback(temp->context, MQTTAPI_CONFIRMATION_BECAUSE_DESTROY);
					MQTTClient_freeMessage(&(temp->messageToSend));
					free(temp->messageToSend);
					STRING_delete(temp->topicName);
					free(temp);
				}
			}
		}

		{
			PDLIST_ENTRY currentListEntry;
			while ((currentListEntry = DList_RemoveHeadList(&(mqttHandleData->messagesSent))) != &(mqttHandleData->messagesSent))
			{
				MQTTAPI_MESSAGE_SEND_LIST* temp = containingRecord(currentListEntry, MQTTAPI_MESSAGE_SEND_LIST, entry);

				if (mqttHandleData->dcCallback != NULL)
				{
					mqttHandleData->dcCallback(temp->context, MQTTAPI_CONFIRMATION_BECAUSE_DESTROY);
					MQTTClient_freeMessage(&(temp->messageToSend));
					free(temp->messageToSend);
					STRING_delete(temp->topicName);
					free(temp);
				}
			}
		}

		MQTTClient_destroy(&mqttHandleData->client);
		Lock_Deinit(mqttHandleData->LockHandle);
		free(mqttHandleData);
	}
}


MQTTAPI_RESULT MQTTAPI_PublishMessage(MQTTAPI_HANDLE instance, const char* topicName, const MQTTAPI_Message* msg, void* context)
{
	MQTTAPI_RESULT result;
	PMQTTTAPI_HANDLE_DATA mqttApiHandleData = (PMQTTTAPI_HANDLE_DATA)instance;

	/* Codes_SRS_MQTTAPI_04_038: [If any of the parameter is NULL, MQTTAPI_PublishMessage shall return MQTTAPI_INVALID_ARG.] */
	if (instance == NULL)
	{
		LogError("Invalid Argument. instance can't be null.\r\n");
		result = MQTTAPI_INVALID_ARG;
	}
	/* Codes_SRS_MQTTAPI_04_038: [If any of the parameter is NULL, MQTTAPI_PublishMessage shall return MQTTAPI_INVALID_ARG.] */
	else if (topicName == NULL || strlen(topicName) <= 0)
	{
		LogError("Invalid Argument. topicName can't be null or empty.\r\n");
		result = MQTTAPI_INVALID_ARG;
	}
	/* Codes_SRS_MQTTAPI_04_038: [If any of the parameter is NULL, MQTTAPI_PublishMessage shall return MQTTAPI_INVALID_ARG.] */
	else if (msg == NULL)
	{
		LogError("Invalid Argument. msg can't be null.\r\n");
		result = MQTTAPI_INVALID_ARG;
	}
	/* Codes_SRS_MQTTAPI_04_059: [If DeliveryCompleted callback is not set publishMessage shall return MQTTAPI_ERROR.] */
	else if (mqttApiHandleData->dcCallback == NULL)
	{
		LogError("Error Sending data, missind Delivery Completed callback to be set.\r\n");
		result = MQTTAPI_ERROR;
	}
	else
	{
		MQTTAPI_MESSAGE_SEND_LIST* mqttApiMessageEntryList; 

		/* Codes_SRS_MQTTAPI_04_039: [MQTTAPI_PublishMessage shall create structure needed to send a message to the topic described by the topicName parameter.]  */
		if ((mqttApiMessageEntryList = (MQTTAPI_MESSAGE_SEND_LIST*)malloc(sizeof(MQTTAPI_MESSAGE_SEND_LIST))) == NULL)
		{
			LogError("Memmory Allocation failure to create mqttApi Message.\r\n");
			result = MQTTAPI_ERROR;
		}
		else if ((mqttApiMessageEntryList->topicName = STRING_construct(topicName)) == NULL)
{
			LogError("Could not create topicName String.\r\n");
			free(mqttApiMessageEntryList);
			result = MQTTAPI_ERROR;
}
		else
		{
			MQTTClient_message *pubmsg = (MQTTClient_message *)malloc(sizeof(MQTTClient_message));

			if (pubmsg == NULL)
			{
				LogError("Memory Allocation Failure to create MQTTClient_message.\r\n");
				STRING_delete(mqttApiMessageEntryList->topicName);
				free(mqttApiMessageEntryList);
				result = MQTTAPI_ERROR;
			}
			else 
			{
				pubmsg->struct_id[0] = 'M';
				pubmsg->struct_id[1] = 'Q';
				pubmsg->struct_id[2] = 'T';
				pubmsg->struct_id[3] = 'M';
				pubmsg->struct_version = 0;
				pubmsg->qos = SENDMESSAGEQOS;
				pubmsg->retained = 0;
				pubmsg->dup = 0;
				pubmsg->msgid = 0;
				pubmsg->payloadlen = msg->payloadlen;

				/* Codes_SRS_MQTTAPI_04_043: [MQTTAPI_PublishMessage shall clone MQTTAPI_Message so the user can free it after calling publishMessage.]  */
				if ((pubmsg->payload = malloc(msg->payloadlen)) == NULL)
				{
					LogError("Memory Allocation Failure to create MQTTClient_message.\r\n");
					free(pubmsg);
					STRING_delete(mqttApiMessageEntryList->topicName);
					free(mqttApiMessageEntryList);
					result = MQTTAPI_ERROR;
				}
				else
{
					(void)memcpy(pubmsg->payload, msg->payload, msg->payloadlen);
					/* Codes_SRS_MQTTAPI_04_058: [If context is not NULL it shall be stored and passed to the user when received callback function is called.]  */
					mqttApiMessageEntryList->context = context;
					mqttApiMessageEntryList->messageToSend = pubmsg;
					DList_InsertHeadList(&(mqttApiHandleData->messagesToSend), &(mqttApiMessageEntryList->entry));

					/* Codes_SRS_MQTTAPI_04_048: [If the message was correctly queued (on it’s on a queue on underlying library) MQTTAPI_PublishMessage shall return MQTTAPI_OK.] */
					result = MQTTAPI_OK;
				}
			}
		}
	}

	return result;
}


MQTTAPI_TOPIC_HANDLE MQTTAPI_Subscribe(MQTTAPI_HANDLE instance, const char* topic)
{
	PMQTTTAPI_HANDLE_DATA handle = instance;
	PMQTTAPI_TOPIC_HANDLE_DATA result;

	/* Codes_SRS_MQTTAPI_04_029: [If any of the parameters instance or topic is NULL MQTTAPI_Subscribe shall return NULL.]  */
	if (instance == NULL || topic == NULL || strlen(topic) == 0)
	{
		result = NULL;
	}
	else
	{
		if ((result = (PMQTTAPI_TOPIC_HANDLE_DATA)malloc(sizeof(MQTTAPI_TOPIC_HANDLE_DATA))) == NULL)
		{
			LogError("Memory Allocation failure to store Topic Handle Data.\r\n");
			handle->subscribed = false;
		}
		else if (checkAndTryToConnect(handle))
		{
			int rc;
			/* Codes_SRS_MQTTAPI_04_031: [MQTTAPI_Subscribe shall call underlying library methods to subscribe to the topic on the topic parameter.] */
			if ((rc = MQTTClient_subscribe(handle->client, topic, SUBSCRIBEQOS)) != MQTTCLIENT_SUCCESS)
			{
				/* Codes_SRS_MQTTAPI_04_032: [If the underlying library fails MQTTAPI_Subscribe shall return NULL] */
				LogError("Error subscribing to topic. Code is:%d \r\n", rc);
				free(result);
				result = NULL;
				handle->subscribed = false;
			}
			else
			{
				/* Codes_SRS_MQTTAPI_04_033: [Otherwise MQTTAPI_Subscribe shall return a non-NULL MQTTAPI_TOPIC_HANDLE] */
				result->mqttApiInstance = handle;
				if ((result->topicName = STRING_construct(topic)) == NULL)
				{
					LogError("Problem to save topic name.\r\n");
					free(result);
					result = NULL;
					handle->subscribed = false;
				}
				else
				{
					handle->subscribedTopicHandleData = result;
					handle->subscribed = true;
				}
			}
		}
		else
		{
			LogError("MQTT Client Not Connected. Error trying to reconect.\r\n");
			free(result);
			result = NULL;
		}
	}

	return result;
}

void MQTTAPI_Unsubscribe(MQTTAPI_TOPIC_HANDLE topicInstance)
{
	/* Codes_SRS_MQTTAPI_04_034: [If topicInstance is NULL MQTTAPI_Unsubscribe shall do nothing.] */
	if (topicInstance != NULL)
	{
		PMQTTAPI_TOPIC_HANDLE_DATA topicHandleData = topicInstance;
		PMQTTTAPI_HANDLE_DATA mqttApiInstance = topicHandleData->mqttApiInstance;

		/* Codes_SRS_MQTTAPI_04_035: [MQTTAPI_Unsubscribe shall call underlying library methods to unsubscribe to the topic Instance.] */
		if (MQTTClient_unsubscribe(mqttApiInstance->client, STRING_c_str(topicHandleData->topicName)) != MQTTCLIENT_SUCCESS)
		{
			LogError("Problems on Unsubscribing to Commands.\r\n");
		}
		else
{
			mqttApiInstance->subscribed = false;
			/* Codes_SRS_MQTTAPI_04_060: [MQTTAPI_Unsubscribe shall release all previously allocated resources pointed by topicInstance.] */
			STRING_delete(mqttApiInstance->subscribedTopicHandleData->topicName);
			free(mqttApiInstance->subscribedTopicHandleData);
		}
	}
}

void MQTTAPI_DoWork(MQTTAPI_HANDLE instance)
{
	PMQTTTAPI_HANDLE_DATA handle = instance;

	/* Codes_SRS_MQTTAPI_04_050: [if parameter instance is NULL then MQTTAPI_DoWork shall not perform any action.] */
	if (handle != NULL)
	{

		if (!checkAndTryToConnect(handle))
		{
			LogError("Client Not Connected and could not connect.\r\n");
		}
		else
		{
			if (Lock(handle->LockHandle) == LOCK_OK)
			{

				//message
				{
					PDLIST_ENTRY received;
					while ((received = DList_RemoveHeadList(&(handle->messagesReceived))) != &(handle->messagesReceived))
					{
						MQTTAPI_MESSAGE_RECEIVED_LIST* temp = containingRecord(received, MQTTAPI_MESSAGE_RECEIVED_LIST, entry);
						if (handle->maCallBack != NULL)
						{
							if (!handle->maCallBack(handle->maCallbackContext, temp->messageReceived))
							{
								LogError("Client could not handle message, dropping it.");
							}
						}

						free(temp->messageReceived->payload);
						free(temp->messageReceived);
						free(temp);
					}
				}
				Unlock(handle->LockHandle);
			}
			else
			{
				LogError("Could not aquire lock for message.\r\n");
			}

			//Event
			{
				PDLIST_ENTRY messageToSend;
				while ((messageToSend = DList_RemoveHeadList(&(handle->messagesToSend))) != &(handle->messagesToSend))
				{
					MQTTAPI_MESSAGE_SEND_LIST* temp = containingRecord(messageToSend, MQTTAPI_MESSAGE_SEND_LIST, entry);

					if (MQTTClient_publishMessage(handle->client, STRING_c_str(temp->topicName), temp->messageToSend, &(temp->dt)) != MQTTCLIENT_SUCCESS)
					{
						handle->dcCallback(temp->context, MQTTAPI_CONFIRMATION_ERROR);
						MQTTClient_freeMessage(&(temp->messageToSend));
						free(temp->messageToSend);
						STRING_delete(temp->topicName);
						free(temp);
					}
					else
					{
						DList_InsertTailList(&(handle->messagesSent), &(temp->entry));
					}
				}
			}
		}
	}
	
}