// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef IOTDM_CLIENT_H
#define IOTDM_CLIENT_H

#include "iothub_client.h"
#include "iothub_client_ll.h"

#include "azure_c_shared_utility/threadapi.h"
#include "azure_c_shared_utility/iot_logging.h"


typedef void *IOTHUB_CHANNEL_HANDLE;
typedef enum IOTHUB_TRANSPORT_TAG
{
    COAP_TCPIP = 0
} IOTHUB_TRANSPORT;


typedef void(*ON_DM_CONNECT_COMPLETE)(IOTHUB_CLIENT_RESULT result, void *context);


/** @brief      Creates a IoT DM client for communication with an existing
*               IoT DM Service using the specified connection string parameter.
*
*   @param      connectionString    Pointer to a character string representing a device connection.
*               transport           Connection type. Reserved for future use.
*
*   @return     A non-NULL @c IOTHUB_CHANNEL_HANDLE value that is used when
*               invoking other functions for IoT DM client and @c NULL on failure.
*/
extern IOTHUB_CHANNEL_HANDLE IoTHubClient_DM_Open(const char *connectionString, IOTHUB_TRANSPORT transport);


/**
*   @brief      Closes the connection for the given client and disposes of resources allocated
*               by the IoT DM client. This is a blocking call.
*
*   @param      iotDMClientHandle   The handle created by a call to the create function.
*/
extern void IoTHubClient_DM_Close(IOTHUB_CHANNEL_HANDLE iotDMClientHandle);


/** @brief      Starts the client and remain running. A high level function which makes use
*               of IoTHubClient_DM_Connect and IoTHubClient_DM_DoWork.
*
*   @param      iotDMClientHandle   The handle created by a call to the create function.
*
*   @return     IOTHUB_CLIENT_OK upon success or an error code upon failure.
*/
extern IOTHUB_CLIENT_RESULT IoTHubClient_DM_Start(IOTHUB_CHANNEL_HANDLE iotDMClientHandle);


/** @brief      Connects the client to the the IoT DM service represented in the iotDMClientHandle.
*
*   @param      iotDMClientHandle   The handle created by a call to the create function.
*               onComplete          The pointer to a function to be called when the connection is completed.
*               callbackContext     The pointer to a chunck of memory to be interpreted by the call back function.
*
*   @return     IOTHUB_CLIENT_OK upon success or an error code upon failure.
*/
extern IOTHUB_CLIENT_RESULT IoTHubClient_DM_Connect(IOTHUB_CHANNEL_HANDLE iotDMClientHandle, ON_DM_CONNECT_COMPLETE onComplete, void *callbackContext);


/** @brief      Performs one unit of work. One unit of work consists of serveral steps as follows:
*               - Accept and process any pending requests from the DM service.
*               - Evaluate and process any observe / notify attributes.
*
*   @param      iotDMClientHandle   The handle created by a call to the create function.
*
*   @return     true if work is succesfully completed.
*/
extern bool IoTHubClient_DM_DoWork(IOTHUB_CHANNEL_HANDLE iotDMClientHandle);


/** @brief      Protects session specific data and should be used only for multi-threaded clients.
*               For example, if a client establishes a connection to an IotHub and has the
*               need to use worker threads to update session data from sensor feeds.
*
*   @param      iotDMClientHandle   The handle created by a call to the create function.
*
*   @return     true if work is succesfully completed.
*/
extern bool IotHubClient_DM_EnterCriticalSection(IOTHUB_CHANNEL_HANDLE iotDMClientHandle);


/** @brief		Releases the session lock.
*
*   @param      iotDMClientHandle   The handle created by a call to the create function.
*
*   @return     true if work is succesfully completed.
*/
extern bool IotHubClient_DM_LeaveCriticalSection(IOTHUB_CHANNEL_HANDLE iotDMClientHandle);

#endif /* IOTDM_CLIENT_H */
