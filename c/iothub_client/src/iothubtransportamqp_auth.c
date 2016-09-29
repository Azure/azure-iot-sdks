// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "iothubtransportamqp_auth.h"

#define RESULT_OK 0
#define INDEFINITE_TIME ((time_t)(-1))
#define CBS_AUDIENCE "servicebus.windows.net:sastoken"

typedef struct AMQP_TRANSPORT_CBS_STATE_TAG
{
	// A component of the SAS token. Currently this must be an empty string.
	STRING_HANDLE sasTokenKeyName;
	// Time when the current SAS token was created, in seconds since epoch.
	size_t current_sas_token_create_time;
} AMQP_TRANSPORT_CBS_STATE;

typedef struct AUTHENTICATION_STATE_TAG
{
	STRING_HANDLE device_id;

	STRING_HANDLE devices_path;

	AMQP_TRANSPORT_CBS_CONNECTION* cbs_connection;

	AMQP_TRANSPORT_CREDENTIAL credential;

	AMQP_TRANSPORT_CBS_STATE cbs_state;

	AUTHENTICATION_STATUS status;
} AUTHENTICATION_STATE;

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

static void on_put_token_complete(void* context, CBS_OPERATION_RESULT operation_result, unsigned int status_code, const char* status_description)
{
#ifdef NO_LOGGING
	UNUSED(status_code);
	UNUSED(status_description);
#endif

	AUTHENTICATION_STATE* auth_state = (AUTHENTICATION_STATE*)context;

	if (operation_result == CBS_OPERATION_RESULT_OK)
	{
		auth_state->status = AUTHENTICATION_STATUS_OK;
	}
	else
	{
		auth_state->status = AUTHENTICATION_STATUS_FAILURE;
		LogError("CBS reported status code %u, error: %s for put token operation", status_code, status_description);
	}
}

static void on_delete_token_complete(void* context, CBS_OPERATION_RESULT operation_result, unsigned int status_code, const char* status_description)
{
#ifdef NO_LOGGING
	UNUSED(status_code);
	UNUSED(status_description);
#endif

	AUTHENTICATION_STATE* auth_state = (AUTHENTICATION_STATE*)context;

	if (operation_result == CBS_OPERATION_RESULT_OK)
	{
		auth_state->status = AUTHENTICATION_STATUS_IDLE;
	}
	else
	{
		auth_state->status = AUTHENTICATION_STATUS_FAILURE;
		LogError("CBS reported status code %u, error: %s for delete token operation", status_code, status_description);
	}
}

static int handSASTokenToCbs(AUTHENTICATION_STATE* auth_state, STRING_HANDLE sasToken, size_t sas_token_create_time)
{
	int result;
	
	if (cbs_put_token(auth_state->cbs_connection->cbs_handle, CBS_AUDIENCE, STRING_c_str(auth_state->devices_path), STRING_c_str(sasToken), on_put_token_complete, auth_state) != RESULT_OK)
	{
		LogError("Failed applying new SAS token to CBS.");
		result = __LINE__;
	}
	else
	{
		auth_state->status = AUTHENTICATION_STATUS_IN_PROGRESS;
		auth_state->cbs_state.current_sas_token_create_time = sas_token_create_time;
		result = RESULT_OK;
	}

	return result;
}

static int verifyAuthenticationTimeout(AUTHENTICATION_STATE* auth_state, bool* timeout_reached)
{
	int result;
	size_t currentTimeInSeconds;

	if (getSecondsSinceEpoch(&currentTimeInSeconds) != RESULT_OK)
	{
		LogError("Failed getting the current time to verify if the SAS token needs to be refreshed.");
		*timeout_reached = true;
		result = __LINE__;
	}
	else
	{
		*timeout_reached = ((currentTimeInSeconds - auth_state->cbs_state.current_sas_token_create_time) * 1000 >= auth_state->cbs_connection->cbs_request_timeout) ? true : false;
		result = RESULT_OK;
	}

	return result;
}

static bool isSasTokenRefreshRequired(AUTHENTICATION_STATE* auth_state)
{
	bool result;
	size_t currentTimeInSeconds;
	if (auth_state->credential.type == DEVICE_SAS_TOKEN)
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
		result = ((currentTimeInSeconds - auth_state->cbs_state.current_sas_token_create_time) >= (auth_state->cbs_connection->sas_token_refresh_time / 1000)) ? true : false;
	}

	return result;
} 

AUTHENTICATION_STATE_HANDLE authentication_create(const IOTHUB_DEVICE_CONFIG* device_config, STRING_HANDLE devices_path, AMQP_TRANSPORT_CBS_CONNECTION* cbs_connection)
{
	AUTHENTICATION_STATE* auth_state = NULL;
	bool cleanup_required = true;

	if (device_config == NULL)
	{
		LogError("Failed creating the authentication state (device_config is NULL)");
	}
	else if (devices_path == NULL)
	{
		LogError("Failed creating the authentication state (devices_path is NULL)");
	}
	else if (cbs_connection == NULL)
	{
		LogError("Failed creating the authentication state (cbs_connection handle is NULL)");
	}
	else if ((auth_state = (AUTHENTICATION_STATE*)malloc(sizeof(AUTHENTICATION_STATE))) == NULL)
	{
		LogError("Failed creating the authentication state (malloc failed)");
	}
	else
	{
		auth_state->device_id = NULL;
		auth_state->cbs_connection = cbs_connection;
		auth_state->credential.type = CREDENTIAL_NOT_BUILD;
		auth_state->credential.data.deviceKey = NULL;
		auth_state->credential.data.deviceSasToken = NULL;
		auth_state->credential.data.x509credential.x509certificate = NULL;
		auth_state->credential.data.x509credential.x509privatekey = NULL;
		auth_state->cbs_state.sasTokenKeyName = NULL;
		auth_state->status = AUTHENTICATION_STATUS_IDLE;

		if ((auth_state->device_id = STRING_construct(device_config->deviceId)) == NULL)
		{
			LogError("Failed creating the authentication state (could not copy the deviceId, STRING_construct failed)");
		}
		else if ((auth_state->devices_path = STRING_clone(devices_path)) == NULL)
		{
			LogError("Failed creating the authentication state (could not clone the devices_path)");
		}
		else
		{
			if (device_config->deviceSasToken != NULL)
			{
				if ((auth_state->cbs_state.sasTokenKeyName = STRING_new()) == NULL)
				{
					LogError("Failed to allocate device_state->sasTokenKeyName.");
				}
				else if ((auth_state->credential.data.deviceSasToken = STRING_construct(device_config->deviceSasToken)) == NULL)
				{
					LogError("unable to STRING_construct for deviceSasToken");
				}
				else
				{
					auth_state->credential.type = DEVICE_SAS_TOKEN;
					auth_state->cbs_state.current_sas_token_create_time = 0;
					cleanup_required = false;
				}
				
			}
			else if (device_config->deviceKey != NULL)
			{
				if ((auth_state->cbs_state.sasTokenKeyName = STRING_new()) == NULL)
				{
					LogError("Failed to allocate device_state->sasTokenKeyName.");
				}
				else if ((auth_state->credential.data.deviceKey = STRING_construct(device_config->deviceKey)) == NULL)
				{
					LogError("unable to STRING_construct for a deviceKey");
				}
				else
				{
					auth_state->credential.type = DEVICE_KEY;
					auth_state->cbs_state.current_sas_token_create_time = 0;
					cleanup_required = false;
				}
			}
			else
			{
				/*when both SAS token AND devicekey are NULL*/
				auth_state->credential.type = X509;
				auth_state->credential.data.x509credential.x509certificate = NULL;
				auth_state->credential.data.x509credential.x509privatekey = NULL;
				cleanup_required = false;
			}
		}
	}

	if (cleanup_required)
	{
		if (auth_state->credential.data.deviceKey != NULL)
			STRING_delete(auth_state->credential.data.deviceKey);
		if (auth_state->devices_path != NULL)
			STRING_delete(auth_state->devices_path);
		if (auth_state->credential.data.deviceSasToken != NULL)
			STRING_delete(auth_state->credential.data.deviceSasToken);
		if (auth_state->cbs_state.sasTokenKeyName != NULL)
			STRING_delete(auth_state->cbs_state.sasTokenKeyName);
		if (auth_state != NULL)
			free(auth_state);
	}

	return (AUTHENTICATION_STATE_HANDLE)auth_state;
}

int authentication_authenticate(AUTHENTICATION_STATE_HANDLE authentication_state_handle)
{
	int result;

	if (authentication_state_handle == NULL)
	{
		result = __LINE__;
		LogError("Failed to authenticate device (the authentication_state_handle is NULL)");
	}
	else
	{
		size_t currentTimeInSeconds;
		AUTHENTICATION_STATE* auth_state = (AUTHENTICATION_STATE*)authentication_state_handle;

		if ((result = getSecondsSinceEpoch(&currentTimeInSeconds)) != RESULT_OK)
		{
			LogError("Failed getting current time to compute the SAS token creation time (%d).", result);
			result = __LINE__;
		}
		else
		{
			// Codes_SRS_IOTHUBTRANSPORTAMQP_09_083: [SAS tokens expiration time shall be calculated using the number of seconds since Epoch UTC (Jan 1st 1970 00h00m00s000 GMT) to now (GMT), plus the 'sas_token_lifetime'.]
			size_t new_expiry_time = currentTimeInSeconds + (auth_state->cbs_connection->sas_token_lifetime / 1000);

			STRING_HANDLE newSASToken;

			switch (auth_state->credential.type)
			{
				default:
				{
					result = __LINE__;
					LogError("internal error, unexpected enum value transport_state->credential.credentialType=%d", auth_state->credential.type);
					break;
				}
				case DEVICE_KEY:
				{
					newSASToken = SASToken_Create(auth_state->credential.data.deviceKey, auth_state->devices_path, auth_state->cbs_state.sasTokenKeyName, new_expiry_time);
					
					if (newSASToken == NULL)
					{
						LogError("Could not generate a new SAS token for the CBS.");
						result = __LINE__;
					}
					else
					{
						if (handSASTokenToCbs(auth_state, newSASToken, currentTimeInSeconds) != 0)
						{
							LogError("unable to send the new SASToken to CBS");
							result = __LINE__;
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
					newSASToken = STRING_clone(auth_state->credential.data.deviceSasToken);

					if (newSASToken == NULL)
					{
						LogError("Could not generate a new SAS token for the CBS.");
						result = __LINE__;
					}
					else
					{
						if (handSASTokenToCbs(auth_state, newSASToken, currentTimeInSeconds) != 0)
						{
							LogError("unable to send the new SASToken to CBS");
							result = __LINE__;
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
	}

	return result;
}

AUTHENTICATION_STATUS authentication_get_status(AUTHENTICATION_STATE_HANDLE authentication_state_handle)
{
	AUTHENTICATION_STATUS auth_status = AUTHENTICATION_STATUS_IDLE;
	
	if (authentication_state_handle == NULL)
	{
		LogError("Failed retrieving the authentication status (the authentication_state_handle is NULL)");
	}
	else
	{
		AUTHENTICATION_STATE* auth_state = (AUTHENTICATION_STATE*)authentication_state_handle;

		if (auth_state->status == AUTHENTICATION_STATUS_OK)
		{
			bool timeout_reached;
			if (verifyAuthenticationTimeout(auth_state, &timeout_reached) != RESULT_OK)
			{
				LogError("Failed retrieving the status of the authentication (failed verifying if the authentication is expired)");
				auth_state->status = AUTHENTICATION_STATUS_FAILURE;
			}
			else if (timeout_reached)
			{
				auth_state->status = AUTHENTICATION_STATUS_EXPIRED;
			}
		}

		auth_status = auth_state->status;
	}

	return auth_status;
}

AMQP_TRANSPORT_CREDENTIAL* authentication_get_credential(AUTHENTICATION_STATE_HANDLE authentication_state_handle)
{
	AMQP_TRANSPORT_CREDENTIAL* credential = NULL;

	if (authentication_state_handle != NULL)
	{
		AUTHENTICATION_STATE* auth_state = (AUTHENTICATION_STATE*)authentication_state_handle;
		
		credential = &auth_state->credential;
	}

	return credential;
}

int authentication_refresh(AUTHENTICATION_STATE_HANDLE authentication_state_handle)
{
	int result;

	if ((result = authentication_authenticate(authentication_state_handle)) != RESULT_OK)
	{
		LogError("Failed refreshing authentication (%d).", result);
		result = __LINE__;
	}

	return result;
}

int authentication_reset(AUTHENTICATION_STATE_HANDLE authentication_state_handle)
{
	int result;

	if (authentication_state_handle == NULL)
	{
		LogError("Failed to reset the authentication state (authentication_state is NULL)");
		result = __LINE__;
	}
	else
	{
		AUTHENTICATION_STATE* auth_state = (AUTHENTICATION_STATE*)authentication_state_handle;

		if (auth_state->status == AUTHENTICATION_STATUS_FAILURE || auth_state->status == AUTHENTICATION_STATUS_EXPIRED)
		{
			auth_state->status = AUTHENTICATION_STATUS_IDLE;
			result = RESULT_OK;
		}
		else
		{
			if (auth_state->status != AUTHENTICATION_STATUS_OK && auth_state->status != AUTHENTICATION_STATUS_IN_PROGRESS)
			{
				LogError("Failed to reset the authentication state (authentication status is invalid: %i)", auth_state->status);
				result = __LINE__;
			}
			else if (cbs_delete_token(auth_state->cbs_connection->cbs_handle, STRING_c_str(auth_state->devices_path), CBS_AUDIENCE, on_delete_token_complete, auth_state) != RESULT_OK)
			{
				LogError("Failed to reset the authentication state (failed deleting the current SAS token from CBS)");
				result = __LINE__;
			}
			else
			{
				auth_state->status = AUTHENTICATION_STATUS_IDLE;
				auth_state->cbs_state.current_sas_token_create_time = 0;
				result = RESULT_OK;
			}
		}
	}

	return result;
}

int authentication_destroy(AUTHENTICATION_STATE_HANDLE authentication_state_handle)
{
	int result;

	if (authentication_state_handle == NULL)
	{
		LogError("Failed to destroy the authentication state (authentication_state is NULL)");
		result = __LINE__;
	}
	else
	{
		AUTHENTICATION_STATE* auth_state = (AUTHENTICATION_STATE*)authentication_state_handle;

		STRING_delete(auth_state->device_id);
		STRING_delete(auth_state->devices_path);

		switch (auth_state->credential.type)
		{
			default:
			{
				LogError("internal error: unexpected enum value transport_state->credential.credentialType=%d", auth_state->credential.type);
				result = __LINE__;
				break;
			}
			case (CREDENTIAL_NOT_BUILD):
			{
				/*nothing to do*/
				result = RESULT_OK;
				break;
			}
			case(X509):
			{
				/*nothing to do here, x509certificate and x509privatekey are both NULL*/
				result = RESULT_OK; 
				break;
			}
			case(DEVICE_KEY):
			{
				STRING_delete(auth_state->credential.data.deviceKey);
				STRING_delete(auth_state->cbs_state.sasTokenKeyName);
				result = RESULT_OK; 
				break;
			}
			case(DEVICE_SAS_TOKEN):
			{
				STRING_delete(auth_state->credential.data.deviceSasToken);
				STRING_delete(auth_state->cbs_state.sasTokenKeyName);
				result = RESULT_OK;
				break;
			}
		}
	}

	return result;
}