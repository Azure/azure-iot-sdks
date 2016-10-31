// Copyright (c) Microsoft. All rights reserved. 
// Licensed under the MIT license. See LICENSE file in the project root for full license information. 

#ifndef IOTHUBTRANSPORTAMQP_AUTH_H 
#define IOTHUBTRANSPORTAMQP_AUTH_H 

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#include <stdint.h>
#include "azure_c_shared_utility/strings.h" 
#include "azure_c_shared_utility/sastoken.h"
#include "azure_c_shared_utility/xio.h"
#include "azure_uamqp_c/cbs.h"
#include "azure_uamqp_c/sasl_mechanism.h"
#include "iothub_transport_ll.h" 
#include "azure_c_shared_utility/umock_c_prod.h"

#ifdef __cplusplus 
extern "C" 
{ 
#endif 

typedef struct AMQP_TRANSPORT_CBS_CONNECTION_TAG
{
	// How long a SAS token created by the transport is valid, in milliseconds.
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
	CBS_HANDLE cbs_handle;
} AMQP_TRANSPORT_CBS_CONNECTION;

typedef struct AUTHENTICATION_CONFIG_TAG
{
	const char* device_id;
	const char* device_key;
	const char* device_sas_token;
	const char* iot_hub_host_fqdn;
	const AMQP_TRANSPORT_CBS_CONNECTION* cbs_connection;

} AUTHENTICATION_CONFIG;

typedef enum AUTHENTICATION_STATUS_TAG 
{
	AUTHENTICATION_STATUS_IDLE, 
	AUTHENTICATION_STATUS_IN_PROGRESS, 
	AUTHENTICATION_STATUS_TIMEOUT,
	AUTHENTICATION_STATUS_REFRESH_REQUIRED,
	AUTHENTICATION_STATUS_FAILURE, 
	AUTHENTICATION_STATUS_OK,
	AUTHENTICATION_STATUS_NONE
} AUTHENTICATION_STATUS; 
 
typedef enum AMQP_TRANSPORT_CREDENTIAL_TYPE_TAG 
{ 
	CREDENTIAL_NOT_BUILD, 
	X509, 
	DEVICE_KEY, 
	DEVICE_SAS_TOKEN, 
} AMQP_TRANSPORT_CREDENTIAL_TYPE; 
 
typedef struct X509_CREDENTIAL_TAG 
{ 
	const char* x509certificate; 
	const char* x509privatekey; 
} X509_CREDENTIAL; 
 
typedef union AMQP_TRANSPORT_CREDENTIAL_DATA_TAG 
{ 
	// Key associated to the device to be used. 
	STRING_HANDLE deviceKey; 
 
	// SAS associated to the device to be used. 
	STRING_HANDLE deviceSasToken; 
 
	// X509  
	X509_CREDENTIAL x509credential; 
} AMQP_TRANSPORT_CREDENTIAL_DATA; 
 
typedef struct AMQP_TRANSPORT_CREDENTIAL_TAG 
{ 
	AMQP_TRANSPORT_CREDENTIAL_TYPE type; 
	AMQP_TRANSPORT_CREDENTIAL_DATA data; 
} AMQP_TRANSPORT_CREDENTIAL; 

struct AUTHENTICATION_STATE;
typedef struct AUTHENTICATION_STATE* AUTHENTICATION_STATE_HANDLE;
 
/** @brief Creates a state holder for all authentication-related information and connections. 
* 
*   @returns an instance of the AUTHENTICATION_STATE_HANDLE if succeeds, NULL if any failure occurs. 
*/ 
MOCKABLE_FUNCTION(, AUTHENTICATION_STATE_HANDLE, authentication_create, const AUTHENTICATION_CONFIG*, config);
 
/** @brief Establishes the first authentication for the device in the transport it is registered to. 
* 
* @details If SAS token or key are used, creates a cbs instance for the transport if it does not have one,  
*            and puts a SAS token in (creates one if key is used, or applies the SAS token if provided by user). 
*            If certificates are used, they are set on the tls_io instance of the transport. 
* 
*   @returns 0 if it succeeds, non-zero if it fails. 
*/ 
MOCKABLE_FUNCTION(, int, authentication_authenticate, AUTHENTICATION_STATE_HANDLE, authentication_state);
 
/** @brief Indicates if the device is authenticated successfuly, if authentication is in progress or completed with failure. 
* 
*   @returns A flag indicating the current authentication status of the device. 
*/ 
MOCKABLE_FUNCTION(, AUTHENTICATION_STATUS, authentication_get_status, AUTHENTICATION_STATE_HANDLE, authentication_state);

/** @brief Gets the credential stored by the handle for authenticating the device.
*
*   @returns A AMQP_TRANSPORT_CREDENTIAL with the credentials type and data.
*/
MOCKABLE_FUNCTION(, AMQP_TRANSPORT_CREDENTIAL*, authentication_get_credential, AUTHENTICATION_STATE_HANDLE, authentication_state);

/** @brief Refreshes the authentication if needed. 
* 
* @details If SAS key is used, a new token is generated and put to cbs if the previous generated token is expired. 
* 
*   @returns 0 if it succeeds, non-zero if it fails. 
*/ 
MOCKABLE_FUNCTION(, int, authentication_refresh, AUTHENTICATION_STATE_HANDLE, authentication_state);

/** @brief Resets the state of the authentication.
*
* @details Causes the status of the authentication state to be reset to IDLE (similar to when the state is created).
*
*   @returns 0 if it succeeds, non-zero if it fails.
*/
MOCKABLE_FUNCTION(, int, authentication_reset, AUTHENTICATION_STATE_HANDLE, authentication_state);

/** @brief De-authenticates the device and destroy the state instance. 
* 
* @details Closes the subscription to cbs if in use, destroys the cbs instance if it is the last device registered. 
*            No action is taken if certificate-based authentication if used. 
* 
*   @returns Nothing. 
*/ 
MOCKABLE_FUNCTION(, void, authentication_destroy, AUTHENTICATION_STATE_HANDLE, authentication_state);
 
#ifdef __cplusplus 
} 
#endif 
 
#endif /*IOTHUBTRANSPORTAMQP_AUTH_H*/ 