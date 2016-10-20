// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "iothubtransportamqp_auth.h"
#include "azure_c_shared_utility/agenttime.h" 

#define RESULT_OK 0
#define INDEFINITE_TIME ((time_t)(-1))
#define SAS_TOKEN_TYPE "servicebus.windows.net:sastoken"

typedef struct AMQP_TRANSPORT_CBS_STATE_TAG
{
	// A component of the SAS token. Currently this must be an empty string.
	STRING_HANDLE sasTokenKeyName;
	// Time when the current SAS token was created, in seconds since epoch.
	size_t current_sas_token_create_time;
	// Time when the current SAS token was put to CBS, in seconds since epoch.
	size_t current_sas_token_put_time;
} AMQP_TRANSPORT_CBS_STATE;

typedef struct AUTHENTICATION_STATE_TAG
{
	STRING_HANDLE device_id;

	STRING_HANDLE iot_hub_host_fqdn;

	const AMQP_TRANSPORT_CBS_CONNECTION* cbs_connection;

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
		// Codes_IOTHUBTRANSPORTAMQP_AUTH_09_041: [When cbs_put_token() calls back, if the result is CBS_OPERATION_RESULT_OK the state status shall be set to AUTHENTICATION_STATUS_OK]
		auth_state->status = AUTHENTICATION_STATUS_OK;
	}
	else
	{
		// Codes_IOTHUBTRANSPORTAMQP_AUTH_09_042: [When cbs_put_token() calls back, if the result is not CBS_OPERATION_RESULT_OK the state status shall be set to AUTHENTICATION_STATUS_FAILURE]
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
		// Codes_IOTHUBTRANSPORTAMQP_AUTH_09_066: [If cbs_delete_token calls back with result different than CBS_OPERATION_RESULT_OK, authentication_reset() shall set the state status to AUTHENTICATION_STATUS_FAILURE]
		auth_state->status = AUTHENTICATION_STATUS_FAILURE;
		LogError("CBS reported status code %u, error: %s for delete token operation", status_code, status_description);
	}
}

static int handSASTokenToCbs(AUTHENTICATION_STATE* auth_state, STRING_HANDLE cbs_audience, STRING_HANDLE sasToken, size_t current_time_in_sec_since_epoch)
{
	int result;

	// Codes_IOTHUBTRANSPORTAMQP_AUTH_09_035: [The SAS token provided shall be sent to CBS using cbs_put_token(), using `servicebus.windows.net:sastoken` as token type and `devices_path` as audience]
	// Codes_IOTHUBTRANSPORTAMQP_AUTH_09_028: [The SAS token shall be sent to CBS using cbs_put_token(), using `servicebus.windows.net:sastoken` as token type and `devices_path` as audience]
	if (cbs_put_token(auth_state->cbs_connection->cbs_handle, SAS_TOKEN_TYPE, STRING_c_str(cbs_audience), STRING_c_str(sasToken), on_put_token_complete, auth_state) != RESULT_OK)
	{
		LogError("Failed applying new SAS token to CBS.");
		result = __LINE__;
	}
	else
	{
		// Codes_IOTHUBTRANSPORTAMQP_AUTH_09_029: [If cbs_put_token() succeeds, authentication_authenticate() shall set the state status to AUTHENTICATION_STATUS_IN_PROGRESS]
		// Codes_IOTHUBTRANSPORTAMQP_AUTH_09_037: [If cbs_put_token() succeeds, authentication_authenticate() shall set the state status to AUTHENTICATION_STATUS_IN_PROGRESS]
		auth_state->status = AUTHENTICATION_STATUS_IN_PROGRESS;
		// Codes_IOTHUBTRANSPORTAMQP_AUTH_09_030: [If cbs_put_token() succeeds, authentication_authenticate() shall set `current_sas_token_put_time` with the current time]
		// Codes_IOTHUBTRANSPORTAMQP_AUTH_09_038: [If cbs_put_token() succeeds, authentication_authenticate() shall set `current_sas_token_put_time` with the current time]
		auth_state->cbs_state.current_sas_token_put_time = current_time_in_sec_since_epoch;
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
		// Codes_IOTHUBTRANSPORTAMQP_AUTH_09_046: [The authentication timeout shall be computed comparing the last time a SAS token was put (`current_sas_token_put_time`) to `cbs_request_timeout`]
		// Codes_IOTHUBTRANSPORTAMQP_AUTH_09_052: [The authentication timeout shall be computed comparing the last time the SAS token was put (`current_sas_token_put_time`) to `cbs_request_timeout`]
		*timeout_reached = ((currentTimeInSeconds - auth_state->cbs_state.current_sas_token_put_time) * 1000 >= auth_state->cbs_connection->cbs_request_timeout) ? true : false;
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
		// Codes_IOTHUBTRANSPORTAMQP_AUTH_09_049: [The SAS token expiration shall be computed comparing its create time to `sas_token_refresh_time`]
		result = ((currentTimeInSeconds - auth_state->cbs_state.current_sas_token_create_time) >= (auth_state->cbs_connection->sas_token_refresh_time / 1000)) ? true : false;
	}

	return result;
} 

AUTHENTICATION_STATE_HANDLE authentication_create(const AUTHENTICATION_CONFIG* config)
{
	AUTHENTICATION_STATE* auth_state = NULL;
	bool cleanup_required = true;

	// Codes_SRS_IOTHUBTRANSPORTAMQP_AUTH_09_001: [If parameter config, config->device_id, config->iot_hub_host_fqdn or config->cbs_connection are NULL, authentication_create() shall fail and return NULL.]
	if (config == NULL)
	{
		LogError("Failed creating the authentication state (config is NULL)");
	}
	else if (config->device_id == NULL)
	{
		LogError("Failed creating the authentication state (device_id is NULL)");
	}
	else if (config->iot_hub_host_fqdn == NULL)
	{
		LogError("Failed creating the authentication state (iot_hub_host_fqdn is NULL)");
	}
	else if (config->cbs_connection == NULL)
	{
		LogError("Failed creating the authentication state (cbs_connection handle is NULL)");
	}
	// Codes_SRS_IOTHUBTRANSPORTAMQP_AUTH_09_002: [authentication_create() shall allocate memory for a new authenticate state structure AUTHENTICATION_STATE.]
	else if ((auth_state = (AUTHENTICATION_STATE*)malloc(sizeof(AUTHENTICATION_STATE))) == NULL)
	{
		// Codes_SRS_IOTHUBTRANSPORTAMQP_AUTH_09_003: [If malloc() fails, authentication_create() shall fail and return NULL.]
		LogError("Failed creating the authentication state (malloc failed)");
	}
	else
	{
		auth_state->device_id = NULL;
		// Codes_IOTHUBTRANSPORTAMQP_AUTH_09_005: [authentication_create() shall save a reference to the `cbs_connection` into the AUTHENTICATION_STATE instance.]
		auth_state->cbs_connection = config->cbs_connection;
		auth_state->credential.type = CREDENTIAL_NOT_BUILD;
		auth_state->credential.data.deviceKey = NULL;
		auth_state->credential.data.deviceSasToken = NULL;
		auth_state->credential.data.x509credential.x509certificate = NULL;
		auth_state->credential.data.x509credential.x509privatekey = NULL;
		auth_state->cbs_state.sasTokenKeyName = NULL;
		// Codes_IOTHUBTRANSPORTAMQP_AUTH_09_004: [authentication_create() shall set the initial status of AUTHENTICATION_STATE as AUTHENTICATION_STATUS_IDLE.]
		auth_state->status = AUTHENTICATION_STATUS_IDLE;

		// Codes_IOTHUBTRANSPORTAMQP_AUTH_09_006: [authentication_create() shall save a copy of `device_config->deviceId` into the AUTHENTICATION_STATE instance.]
		if ((auth_state->device_id = STRING_construct(config->device_id)) == NULL)
		{
			// Codes_IOTHUBTRANSPORTAMQP_AUTH_09_007: [If STRING_construct() fails to copy `device_config->deviceId`, authentication_create() shall fail and return NULL]
			LogError("Failed creating the authentication state (could not copy the deviceId, STRING_construct failed)");
		}
		// Codes_IOTHUBTRANSPORTAMQP_AUTH_09_008: [authentication_create() shall save a copy of `iot_hub_host_fqdn` into the AUTHENTICATION_STATE instance.]
		else if ((auth_state->iot_hub_host_fqdn = STRING_construct(config->iot_hub_host_fqdn)) == NULL)
		{
			// Codes_IOTHUBTRANSPORTAMQP_AUTH_09_009: [If STRING_clone() fails to copy `iot_hub_host_fqdn`, authentication_create() shall fail and return NULL]
			LogError("Failed creating the authentication state (could not clone the devices_path)");
		}
		else
		{
			if (config->device_sas_token != NULL)
			{
				// Codes_IOTHUBTRANSPORTAMQP_AUTH_09_013: [If the credential type is DEVICE_SAS_TOKEN or DEVICE_KEY and parameter cbs_connection is NULL, authentication_create() shall fail and return NULL]
				if (auth_state->cbs_connection == NULL)
				{
					LogError("authentication_create() failed (credential type is DEVICE_SAS_TOKEN but cbs_connection handle is NULL).");
				}
				// Codes_IOTHUBTRANSPORTAMQP_AUTH_09_014: [If the credential type is DEVICE_SAS_TOKEN or DEVICE_KEY, authentication_create() shall set sasTokenKeyName in the AUTHENTICATION_STATE as a non-NULL empty string.]
				else if ((auth_state->cbs_state.sasTokenKeyName = STRING_new()) == NULL)
				{
					// Codes_IOTHUBTRANSPORTAMQP_AUTH_09_015: [If STRING_new() fails and cannot create sasTokenKeyName, authentication_create() shall fail and return NULL]
					LogError("Failed to allocate device_state->sasTokenKeyName.");
				}
				// Codes_IOTHUBTRANSPORTAMQP_AUTH_09_010: [If `device_config->deviceSasToken` is not NULL, authentication_create() shall save a copy into the AUTHENTICATION_STATE instance.]
				else if ((auth_state->credential.data.deviceSasToken = STRING_construct(config->device_sas_token)) == NULL)
				{
					// Codes_IOTHUBTRANSPORTAMQP_AUTH_09_011: [If STRING_construct() fails to copy `device_config->deviceSasToken`, authentication_create() shall fail and return NULL]
					LogError("unable to STRING_construct for deviceSasToken");
				}
				else
				{
					// Codes_IOTHUBTRANSPORTAMQP_AUTH_09_012: [If `device_config->deviceSasToken` is not NULL, authentication_create() shall set the credential type in the AUTHENTICATION_STATE as DEVICE_SAS_TOKEN.]
					auth_state->credential.type = DEVICE_SAS_TOKEN;
					auth_state->cbs_state.current_sas_token_create_time = 0;
					cleanup_required = false;
				}
				
			}
			else if (config->device_key != NULL)
			{
				// Codes_IOTHUBTRANSPORTAMQP_AUTH_09_013: [If the credential type is DEVICE_SAS_TOKEN or DEVICE_KEY and parameter cbs_connection is NULL, authentication_create() shall fail and return NULL]
				if (auth_state->cbs_connection == NULL)
				{
					LogError("authentication_create() failed (credential type is DEVICE_KEY but cbs_connection handle is NULL).");
				}
				// Codes_IOTHUBTRANSPORTAMQP_AUTH_09_014: [If the credential type is DEVICE_SAS_TOKEN or DEVICE_KEY, authentication_create() shall set sasTokenKeyName in the AUTHENTICATION_STATE as a non-NULL empty string.]
				else if ((auth_state->cbs_state.sasTokenKeyName = STRING_new()) == NULL)
				{
					// Codes_IOTHUBTRANSPORTAMQP_AUTH_09_015: [If STRING_new() fails and cannot create sasTokenKeyName, authentication_create() shall fail and return NULL]
					LogError("Failed to allocate device_state->sasTokenKeyName.");
				}
				// Codes_IOTHUBTRANSPORTAMQP_AUTH_09_016: [If `device_config->deviceKey` is not NULL, authentication_create() shall save a copy into the AUTHENTICATION_STATE instance.]
				else if ((auth_state->credential.data.deviceKey = STRING_construct(config->device_key)) == NULL)
				{
					// Codes_IOTHUBTRANSPORTAMQP_AUTH_09_017: [If STRING_construct() fails to copy `device_config->deviceKey`, authentication_create() shall fail and return NULL]
					LogError("unable to STRING_construct for a deviceKey");
				}
				else
				{
					// Codes_IOTHUBTRANSPORTAMQP_AUTH_09_018: [If `device_config->deviceKey` is not NULL, authentication_create() shall set the credential type in the AUTHENTICATION_STATE as DEVICE_KEY.]
					auth_state->credential.type = DEVICE_KEY;
					auth_state->cbs_state.current_sas_token_create_time = 0;
					cleanup_required = false;
				}
			}
			else
			{
				// Codes_IOTHUBTRANSPORTAMQP_AUTH_09_020: [If the credential type is X509 and parameter cbs_connection is not NULL, authentication_create() shall fail and return NULL]
				if (auth_state->cbs_connection == NULL)
				{
					LogError("authentication_create() failed (credential type is X509 but cbs_connection handle is not NULL).");
				}
				else
				{
					// Codes_IOTHUBTRANSPORTAMQP_AUTH_09_019: [If `device_config->deviceKey` and `device_config->deviceSasToken` are NULL, authentication_create() shall set the credential type in the AUTHENTICATION_STATE as X509]
					auth_state->credential.type = X509;
					auth_state->credential.data.x509credential.x509certificate = NULL;
					auth_state->credential.data.x509credential.x509privatekey = NULL;
					cleanup_required = false;
				}
			}
		}
	}

	// Codes_IOTHUBTRANSPORTAMQP_AUTH_09_021: [If any failure occurs, authentication_create() shall free any memory it allocated previously, including the AUTHENTICATION_STATE]
	if (cleanup_required)
	{
		if (auth_state->credential.data.deviceKey != NULL)
			STRING_delete(auth_state->credential.data.deviceKey);
		if (auth_state->iot_hub_host_fqdn != NULL)
			STRING_delete(auth_state->iot_hub_host_fqdn);
		if (auth_state->credential.data.deviceSasToken != NULL)
			STRING_delete(auth_state->credential.data.deviceSasToken);
		if (auth_state->cbs_state.sasTokenKeyName != NULL)
			STRING_delete(auth_state->cbs_state.sasTokenKeyName);
		if (auth_state != NULL)
			free(auth_state);
	}

	// Codes_IOTHUBTRANSPORTAMQP_AUTH_09_022: [If no failure occurs, authentication_create() shall return a reference to the AUTHENTICATION_STATE handle]
	return (AUTHENTICATION_STATE_HANDLE)auth_state;
}

static STRING_HANDLE concatenate_3_strings(const char* prefix, const char* infix, const char* suffix)
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

static STRING_HANDLE create_devices_path(STRING_HANDLE device_id, STRING_HANDLE iothub_host_fqdn)
{
	STRING_HANDLE devices_path;

	if ((devices_path = concatenate_3_strings(STRING_c_str(device_id), "/devices/", STRING_c_str(iothub_host_fqdn))) == NULL)
	{
		LogError("Could not create the devices_path parameter (concatenate_3_strings failed)");
	}

	return devices_path;
}

int authentication_authenticate(AUTHENTICATION_STATE_HANDLE authentication_state_handle)
{
	int result;

	// Codes_SRS_IOTHUBTRANSPORTAMQP_AUTH_09_023: [If authentication_state is NULL, authentication_authenticate() shall fail and return an error code]
	if (authentication_state_handle == NULL)
	{
		result = __LINE__;
		LogError("Failed to authenticate device (the authentication_state_handle is NULL)");
	}
	else
	{
		AUTHENTICATION_STATE* auth_state = (AUTHENTICATION_STATE*)authentication_state_handle;

		switch (auth_state->credential.type)
		{
			// Codes_IOTHUBTRANSPORTAMQP_AUTH_09_024: [If the credential type is DEVICE_KEY, authentication_authenticate() shall create a SAS token and put it to CBS]
			case DEVICE_KEY:
			{
				size_t currentTimeInSeconds;

				if ((result = getSecondsSinceEpoch(&currentTimeInSeconds)) != RESULT_OK)
				{
					LogError("Failed getting current time to compute the SAS token creation time (%d).", result);
					result = __LINE__;
				}
				else
				{
					STRING_HANDLE devices_path = NULL;
					STRING_HANDLE newSASToken = NULL;

					// Codes_IOTHUBTRANSPORTAMQP_AUTH_09_026: [The SAS token expiration time shall be calculated adding `sas_token_lifetime` to the current number of seconds since epoch time UTC]
					size_t new_expiry_time = currentTimeInSeconds + (auth_state->cbs_connection->sas_token_lifetime / 1000);

					// Codes_SRS_IOTHUBTRANSPORTAMQP_AUTH_09_076: [A STRING_HANDLE, referred to as `devices_path`, shall be created from the following parts: iot_hub_host_fqdn + "/devices/" + device_id]
					if ((devices_path = create_devices_path(auth_state->iot_hub_host_fqdn, auth_state->device_id)) == NULL)
					{
						// Codes_SRS_IOTHUBTRANSPORTAMQP_AUTH_09_077: [If `devices_path` failed to be created, authentication_authenticate() shall fail and return an error code]
						LogError("Failed to authenticate device (could not create the devices_path parameter)");
						result = __LINE__;
					}
					// Codes_IOTHUBTRANSPORTAMQP_AUTH_09_025: [The SAS token shall be created using SASToken_Create(), passing the deviceKey, device_path, sasTokenKeyName and expiration time as arguments]
					else if ((newSASToken = SASToken_Create(auth_state->credential.data.deviceKey, devices_path, auth_state->cbs_state.sasTokenKeyName, new_expiry_time)) == NULL)
					{
						// Codes_IOTHUBTRANSPORTAMQP_AUTH_09_027: [If SASToken_Create() fails, authentication_authenticate() shall fail and return an error code]
						LogError("Could not generate a new SAS token for the CBS.");
						result = __LINE__;
					}
					else
					{
						auth_state->cbs_state.current_sas_token_create_time = currentTimeInSeconds;

						if (handSASTokenToCbs(auth_state, devices_path, newSASToken, currentTimeInSeconds) != 0)
						{
							// Codes_IOTHUBTRANSPORTAMQP_AUTH_09_032: [If cbs_put_token() fails, authentication_authenticate() shall fail and return an error code]
							LogError("unable to send the new SASToken to CBS");
							result = __LINE__;
						}
						else
						{
							// Codes_IOTHUBTRANSPORTAMQP_AUTH_09_033: [If cbs_put_token() succeeds, authentication_authenticate() shall return success code 0]
							result = RESULT_OK;
						}

						// Codes_IOTHUBTRANSPORTAMQP_AUTH_09_031: [authentication_authenticate() shall free the memory allocated for the new SAS token using STRING_delete()]
						STRING_delete(newSASToken);
					}

					if (devices_path != NULL)
						STRING_delete(devices_path);
				}

				break;
			}
			// Codes_IOTHUBTRANSPORTAMQP_AUTH_09_034: [If the credential type is DEVICE_SAS_TOKEN, authentication_authenticate() shall put the SAS token provided to CBS]
			case DEVICE_SAS_TOKEN:
			{
				size_t currentTimeInSeconds;

				if ((result = getSecondsSinceEpoch(&currentTimeInSeconds)) != RESULT_OK)
				{
					LogError("Failed getting current time to compute the SAS token creation time (%d).", result);
					result = __LINE__;
				}
				else
				{
					STRING_HANDLE devices_path = NULL;

					// Codes_SRS_IOTHUBTRANSPORTAMQP_AUTH_09_078: [A STRING_HANDLE, referred to as `devices_path`, shall be created from the following parts: iot_hub_host_fqdn + "/devices/" + device_id]
					if ((devices_path = create_devices_path(auth_state->iot_hub_host_fqdn, auth_state->device_id)) == NULL)
					{
						// Codes_SRS_IOTHUBTRANSPORTAMQP_AUTH_09_079: [If `devices_path` failed to be created, authentication_authenticate() shall fail and return an error code]
						LogError("Failed to authenticate device (could not create the devices_path parameter)");
						result = __LINE__;
					}
					else if (handSASTokenToCbs(auth_state, devices_path, auth_state->credential.data.deviceSasToken, currentTimeInSeconds) != 0)
					{
						// Codes_IOTHUBTRANSPORTAMQP_AUTH_09_036: [If cbs_put_token() fails, authentication_authenticate() shall fail and return an error code]
						LogError("unable to send the new SASToken to CBS");
						result = __LINE__;
					}
					else
					{
						// Codes_IOTHUBTRANSPORTAMQP_AUTH_09_040: [If cbs_put_token() succeeds, authentication_authenticate() shall return success code 0]
						result = RESULT_OK;
					}

					if (devices_path != NULL)
						STRING_delete(devices_path);
				}
				break;
			}
			// Codes_IOTHUBTRANSPORTAMQP_AUTH_09_043: [If the credential type is X509, authentication_authenticate() shall not perform any authentication and return a success code]
			case X509:
				if (auth_state->status != AUTHENTICATION_STATUS_IDLE)
				{
					LogError("Failed to authenticate device [X509] (authentication status %d is invalid)", auth_state->status);
					result = __LINE__;
				}
				else
				{
					auth_state->status = AUTHENTICATION_STATUS_OK;
					result = RESULT_OK;
				}
				break;
			default:
				result = __LINE__;
				LogError("Failed to authenticate the device (unexpected credential type %d)", auth_state->credential.type);
				break;
		}
	}

	return result;
}

AUTHENTICATION_STATUS authentication_get_status(AUTHENTICATION_STATE_HANDLE authentication_state_handle)
{
	AUTHENTICATION_STATUS auth_status = AUTHENTICATION_STATUS_IDLE;
	
	// Codes_IOTHUBTRANSPORTAMQP_AUTH_09_044: [If authentication_state is NULL, authentication_get_status() shall fail and return AUTHENTICATION_STATUS_NONE]
	if (authentication_state_handle == NULL)
	{
		LogError("Failed retrieving the authentication status (the authentication_state_handle is NULL)");
		auth_status = AUTHENTICATION_STATUS_NONE;
	}
	else
	{
		AUTHENTICATION_STATE* auth_state = (AUTHENTICATION_STATE*)authentication_state_handle;

		switch (auth_state->credential.type)
		{
			case DEVICE_KEY:
				// Codes_IOTHUBTRANSPORTAMQP_AUTH_09_045: [If the credential type is DEVICE_KEY and current status is AUTHENTICATION_STATUS_IN_PROGRESS, authentication_get_status() shall check for authentication timeout]
				if (auth_state->status == AUTHENTICATION_STATUS_IN_PROGRESS)
				{
					bool timeout_reached;

					if (verifyAuthenticationTimeout(auth_state, &timeout_reached) != RESULT_OK)
					{
						LogError("Failed retrieving the status of the authentication (failed verifying if the authentication is expired)");
						auth_state->status = AUTHENTICATION_STATUS_FAILURE;
					}
					else if (timeout_reached)
					{
						// Codes_IOTHUBTRANSPORTAMQP_AUTH_09_047: [If authentication has timed out, authentication_get_status() shall set the status of the state to AUTHENTICATION_STATUS_TIMEOUT]
						auth_state->status = AUTHENTICATION_STATUS_TIMEOUT;
					}
				}
				// Codes_IOTHUBTRANSPORTAMQP_AUTH_09_048: [If the credential type is DEVICE_KEY and current status is AUTHENTICATION_STATUS_OK, authentication_get_status() shall check if SAS token must be refreshed]
				else if (auth_state->status == AUTHENTICATION_STATUS_OK && isSasTokenRefreshRequired(auth_state))
				{
					// Codes_IOTHUBTRANSPORTAMQP_AUTH_09_050: [If the SAS token must be refreshed, authentication_get_status() shall set the status of the state to AUTHENTICATION_STATUS_REFRESH_REQUIRED]
					auth_state->status = AUTHENTICATION_STATUS_REFRESH_REQUIRED;
				}
				break;
			case DEVICE_SAS_TOKEN:
				if (auth_state->status == AUTHENTICATION_STATUS_IN_PROGRESS)
				{
					// Codes_IOTHUBTRANSPORTAMQP_AUTH_09_051: [If the credential type is DEVICE_SAS_TOKEN and current status is AUTHENTICATION_STATUS_IN_PROGRESS, authentication_get_status() shall check for authentication timeout]
					bool timeout_reached;
					if (verifyAuthenticationTimeout(auth_state, &timeout_reached) != RESULT_OK)
					{
						LogError("Failed retrieving the status of the authentication (failed verifying if the authentication is expired)");
						auth_state->status = AUTHENTICATION_STATUS_FAILURE;
					}
					else if (timeout_reached)
					{
						// Codes_IOTHUBTRANSPORTAMQP_AUTH_09_053: [If authentication has timed out, authentication_get_status() shall set the status of the state to AUTHENTICATION_STATUS_TIMEOUT]
						auth_state->status = AUTHENTICATION_STATUS_TIMEOUT;
					}
				}
				break;
			case X509:
				break;
			default:
				LogError("Failed to retrieve the authentication status (unexpected credential type %d)", auth_state->credential.type);
				break;
		}

		// Codes_IOTHUBTRANSPORTAMQP_AUTH_09_054: [After checks and updates, authentication_get_status() shall return the status saved on the AUTHENTICATION_STATE]
		auth_status = auth_state->status;
	}

	return auth_status;
}

AMQP_TRANSPORT_CREDENTIAL* authentication_get_credential(AUTHENTICATION_STATE_HANDLE authentication_state_handle)
{
	AMQP_TRANSPORT_CREDENTIAL* credential;

	// Codes_SRS_IOTHUBTRANSPORTAMQP_AUTH_09_056: [If authentication_state is NULL, authentication_get_credential() shall fail and return NULL]
	if (authentication_state_handle == NULL)
	{
		LogError("authentication_get_credential() failed (authentication_state_handle is NULL)");
		credential = NULL;
	}
	// Codes_SRS_IOTHUBTRANSPORTAMQP_AUTH_09_055: [If authentication_state is not NULL, authentication_get_credential() shall return a reference to credentials saved in the AUTHENTICATION_STATE]
	else
	{
		AUTHENTICATION_STATE* auth_state = (AUTHENTICATION_STATE*)authentication_state_handle;
		credential = &auth_state->credential;
	}

	return credential;
}

int authentication_refresh(AUTHENTICATION_STATE_HANDLE authentication_state_handle)
{
	int result;

	// Codes_IOTHUBTRANSPORTAMQP_AUTH_09_057: [If authentication_state is NULL, authentication_refresh() shall fail and return error code]
	if (authentication_state_handle == NULL)
	{
		LogError("authentication_refresh() failed (authentication_state_handle is NULL)");
		result = __LINE__;
	}
	else
	{
		AUTHENTICATION_STATE* auth_state = (AUTHENTICATION_STATE*)authentication_state_handle;

		// Codes_IOTHUBTRANSPORTAMQP_AUTH_09_058: [If the credential type is X509, authentication_refresh() shall return with success code 0]
		if (auth_state->credential.type == X509)
		{
			result = RESULT_OK;
		}
		// Codes_IOTHUBTRANSPORTAMQP_AUTH_09_059: [authentication_refresh() shall invoke authentication_authenticate(), passing the authentication_state handle]
		else if ((result = authentication_authenticate(authentication_state_handle)) != RESULT_OK)
		{
			// Codes_IOTHUBTRANSPORTAMQP_AUTH_09_060: [if authentication_authenticate() fails, authentication_refresh() shall fail and return an error code]
			LogError("Failed refreshing authentication (%d).", result);
			result = __LINE__;
		}
	}

	return result;
}

int authentication_reset(AUTHENTICATION_STATE_HANDLE authentication_state_handle)
{
	int result;

	// Codes_IOTHUBTRANSPORTAMQP_AUTH_09_061: [If authentication_state is NULL, authentication_reset() shall fail and return an error code]
	if (authentication_state_handle == NULL)
	{
		LogError("Failed to reset the authentication state (authentication_state is NULL)");
		result = __LINE__;
	}
	else
	{
		AUTHENTICATION_STATE* auth_state = (AUTHENTICATION_STATE*)authentication_state_handle;

		switch (auth_state->credential.type)
		{
			case DEVICE_KEY:
			case DEVICE_SAS_TOKEN:
			{
				STRING_HANDLE devices_path = NULL;

				if (auth_state->status == AUTHENTICATION_STATUS_FAILURE || auth_state->status == AUTHENTICATION_STATUS_REFRESH_REQUIRED)
				{
					// Codes_IOTHUBTRANSPORTAMQP_AUTH_09_063: [If the authentication_state status is AUTHENTICATION_STATUS_FAILURE or AUTHENTICATION_STATUS_REFRESH_REQUIRED, authentication_reset() shall set the status to AUTHENTICATION_STATUS_IDLE]
					auth_state->status = AUTHENTICATION_STATUS_IDLE;
					result = RESULT_OK;
				}
				else if (auth_state->status != AUTHENTICATION_STATUS_OK && auth_state->status != AUTHENTICATION_STATUS_IN_PROGRESS)
				{
					LogError("Failed to reset the authentication state (authentication status is invalid: %i)", auth_state->status);
					result = __LINE__;
				}
				// Codes_IOTHUBTRANSPORTAMQP_AUTH_09_064: [If the authentication_state status is AUTHENTICATION_STATUS_OK or AUTHENTICATION_STATUS_IN_PROGRESS, authentication_reset() delete the previous token using cbs_delete_token()]
				else if ((devices_path = create_devices_path(auth_state->iot_hub_host_fqdn, auth_state->device_id)) == NULL)
				{
					LogError("Failed to reset the authenticate state (could not create the devices_path parameter for cbs_delete_token)");
					result = __LINE__;
				}
				else if (cbs_delete_token(auth_state->cbs_connection->cbs_handle, STRING_c_str(devices_path), SAS_TOKEN_TYPE, on_delete_token_complete, auth_state) != RESULT_OK)
				{
					// Codes_IOTHUBTRANSPORTAMQP_AUTH_09_065: [If cbs_delete_token fails, authentication_reset() shall fail and return an error code]
					LogError("Failed to reset the authentication state (failed deleting the current SAS token from CBS)");
					result = __LINE__;
				}
				else
				{
					// Codes_IOTHUBTRANSPORTAMQP_AUTH_09_067: [If no error occurs, authentication_reset() shall return 0]
					auth_state->status = AUTHENTICATION_STATUS_IDLE;
					auth_state->cbs_state.current_sas_token_create_time = 0;
					result = RESULT_OK;
				}
				break;
			}
			case X509:
				// Codes_IOTHUBTRANSPORTAMQP_AUTH_09_062: [If the credential type is X509, authentication_reset() shall set the status to AUTHENTICATION_STATUS_IDLE and return with success code 0]
				auth_state->status = AUTHENTICATION_STATUS_IDLE;
				result = RESULT_OK;
				break;
			default:
				result = __LINE__;
				LogError("Failed to reset the authentication state (unexpected credential type %d)", auth_state->credential.type);
				break;
		}
	}

	return result;
}

void authentication_destroy(AUTHENTICATION_STATE_HANDLE authentication_state_handle)
{
	// Codes_IOTHUBTRANSPORTAMQP_AUTH_09_068: [If authentication_state is NULL, authentication_destroy() shall fail and return]
	if (authentication_state_handle == NULL)
	{
		LogError("Failed to destroy the authentication state (authentication_state is NULL)");
	}
	else
	{
		AUTHENTICATION_STATE* auth_state = (AUTHENTICATION_STATE*)authentication_state_handle;

		// Codes_IOTHUBTRANSPORTAMQP_AUTH_09_069: [authentication_destroy() shall destroy the AUTHENTICATION_STATE->device_id using STRING_delete()]
		STRING_delete(auth_state->device_id);
		// Codes_IOTHUBTRANSPORTAMQP_AUTH_09_070: [authentication_destroy() shall destroy the AUTHENTICATION_STATE->iot_hub_host_fqdn using STRING_delete()]
		STRING_delete(auth_state->iot_hub_host_fqdn);

		switch (auth_state->credential.type)
		{
			case (CREDENTIAL_NOT_BUILD):
			case(X509):
				// Codes_IOTHUBTRANSPORTAMQP_AUTH_09_075: [authentication_destroy() shall free the AUTHENTICATION_STATE]
				free(auth_state);
				break;
			case(DEVICE_KEY):
				// Codes_IOTHUBTRANSPORTAMQP_AUTH_09_071: [If the credential type is DEVICE_KEY, authentication_destroy() shall destroy `deviceKey` in AUTHENTICATION_STATE using STRING_delete()]
				STRING_delete(auth_state->credential.data.deviceKey);
				// Codes_IOTHUBTRANSPORTAMQP_AUTH_09_072: [If the credential type is DEVICE_KEY, authentication_destroy() shall destroy `sasTokenKeyName` in AUTHENTICATION_STATE using STRING_delete()]
				STRING_delete(auth_state->cbs_state.sasTokenKeyName);
				// Codes_IOTHUBTRANSPORTAMQP_AUTH_09_075: [authentication_destroy() shall free the AUTHENTICATION_STATE]
				free(auth_state);
				break;
			case(DEVICE_SAS_TOKEN):
				// Codes_IOTHUBTRANSPORTAMQP_AUTH_09_073: [If the credential type is DEVICE_SAS_TOKEN, authentication_destroy() shall destroy `deviceSasToken` in AUTHENTICATION_STATE using STRING_delete()]
				STRING_delete(auth_state->credential.data.deviceSasToken);
				// Codes_IOTHUBTRANSPORTAMQP_AUTH_09_074: [If the credential type is DEVICE_SAS_TOKEN, authentication_destroy() shall destroy `sasTokenKeyName` in AUTHENTICATION_STATE using STRING_delete()]
				STRING_delete(auth_state->cbs_state.sasTokenKeyName);
				// Codes_IOTHUBTRANSPORTAMQP_AUTH_09_075: [authentication_destroy() shall free the AUTHENTICATION_STATE]
				free(auth_state);
				break;
			default:
				LogError("Failed to destroy the authentication state (unexpected credential type %d)", auth_state->credential.type);
				break;
		}
	}
}