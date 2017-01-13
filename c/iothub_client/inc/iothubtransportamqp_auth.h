// Copyright (c) Microsoft. All rights reserved. 
// Licensed under the MIT license. See LICENSE file in the project root for full license information. 
 
#ifndef IOTHUBTRANSPORTAMQP_AUTH_H 
#define IOTHUBTRANSPORTAMQP_AUTH_H 
 
#include "azure-uamqp-c/cbs.h" 
#include "azure_c_shared_utility/strings.h" 
#include "iothub_transport_ll.h" 
 
#ifdef __cplusplus 
extern "C" 
{ 
#endif 
 
 typedef enum AUTHENTICATION_STATUS_TAG 
 { 
  AUTHENTICATION_STATUS_IDLE, 
  AUTHENTICATION_STATUS_IN_PROGRESS, 
  AUTHENTICATION_STATUS_TIMEOUT, 
  AUTHENTICATION_STATUS_FAILURE, 
  AUTHENTICATION_STATUS_OK 
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
 
 typedef union AMQP_TRANSPORT_CREDENTIAL_UNION_TAG 
 { 
  // Key associated to the device to be used. 
  STRING_HANDLE deviceKey; 
 
  // SAS associated to the device to be used. 
  STRING_HANDLE deviceSasToken; 
 
  // X509  
  X509_CREDENTIAL x509credential; 
 } AMQP_TRANSPORT_CREDENTIAL_UNION; 
 
 typedef struct AMQP_TRANSPORT_CREDENTIAL_TAG 
 { 
  AMQP_TRANSPORT_CREDENTIAL_TYPE credentialType; 
  AMQP_TRANSPORT_CREDENTIAL_UNION credential; 
 } AMQP_TRANSPORT_CREDENTIAL; 
 
 typedef AUTHENTICATION_STATE_HANDLE void*; 
 
 /** @brief Creates a state holder for all authentication-related information and connections. 
 * 
 *   @returns an instance of the AUTHENTICATION_STATE_HANDLE if succeeds, NULL if any failure occurs. 
 */ 
 extern AUTHENTICATION_STATE_HANDLE authentication_create(const char* deviceId, AMQP_TRANSPORT_CREDENTIAL* credentials, TRANSPORT_LL_HANDLE transport); 
 
 /** @brief Establishes the first authentication for the device in the transport it is registered to. 
 * 
 * @details If SAS token or key are used, creates a cbs instance for the transport if it does not have one,  
 *            and puts a SAS token in (creates one if key is used, or applies the SAS token if provided by user). 
 *            If certificates are used, they are set on the tls_io instance of the transport. 
 * 
 *   @returns 0 if it succeeds, non-zero if it fails. 
 */ 
 extern int authentication_authenticate(AUTHENTICATION_STATE_HANDLE authentication_state); 
 
 /** @brief Indicates if the device is authenticated successfuly, if authentication is in progress or completed with failure. 
 * 
 *   @returns A flag indicating the current authentication status of the device. 
 */ 
 extern AUTHENTICATION_STATUS authentication_get_status(AUTHENTICATION_STATE_HANDLE authentication_state); 
 
 /** @brief Refreshes the authentication if needed. 
 * 
 * @details If SAS key is used, a new token is generated and put to cbs if the previous generated token is expired. 
 * 
 *   @returns 0 if it succeeds, non-zero if it fails. 
 */ 
 extern int authentication_refresh(AUTHENTICATION_STATE_HANDLE authentication_state); 
 
 /** @brief Sets the common options related to certificates and cbs. 
 * 
 * @details The new options are only applied effectively to the transport on the next call to authenticate() or refresh(). 
 * 
 *   @returns 0 if it succeeds, non-zero if it fails. 
 */ 
 extern int authentication_set_option(AUTHENTICATION_STATE_HANDLE authentication_state, const char* name, const void* value); 
 
 /** @brief De-authenticates the device and destroy the state instance. 
 * 
 * @details Closes the subscription to cbs if in use, destroys the cbs instance if it is the last device registered. 
 *            No action is taken if certificate-based authentication if used. 
 * 
 *   @returns 0 if it succeeds, non-zero if it fails. 
 */ 
 extern int authentication_destroy(AUTHENTICATION_STATE_HANDLE authentication_state); 
 
#ifdef __cplusplus 
} 
#endif 
 
#endif /*IOTHUBTRANSPORTAMQP_AUTH_H*/ 