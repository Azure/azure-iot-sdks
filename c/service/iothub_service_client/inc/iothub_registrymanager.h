// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// This file is under development and it is subject to change

#ifndef IOTHUB_REGISTRYMANAGER_H
#define IOTHUB_REGISTRYMANAGER_H

#ifdef __cplusplus
extern "C"
{
#else
#endif

#include "crt_abstractions.h"
#include "list.h"
#include "map.h"
#include <time.h>
#include "iothub_service_client_auth.h"

#define IOTHUB_DEVICE_STATUS_VALUES       \
    IOTHUB_DEVICE_STATUS_ENABLED,         \
    IOTHUB_DEVICE_STATUS_DISABLED         \

DEFINE_ENUM(IOTHUB_DEVICE_STATUS, IOTHUB_DEVICE_STATUS_VALUES);

#define IOTHUB_DEVICE_CONNECTION_STATE_VALUES         \
    IOTHUB_DEVICE_CONNECTION_STATE_CONNECTED,         \
    IOTHUB_DEVICE_CONNECTION_STATE_DISCONNECTED       \

DEFINE_ENUM(IOTHUB_DEVICE_CONNECTION_STATE, IOTHUB_DEVICE_CONNECTION_STATE_VALUES);

#define IOTHUB_REGISTRYMANAGER_RESULT_VALUES        \
    IOTHUB_REGISTRYMANAGER_OK,                      \
    IOTHUB_REGISTRYMANAGER_INVALID_ARG,             \
    IOTHUB_REGISTRYMANAGER_ERROR,                   \
    IOTHUB_REGISTRYMANAGER_JSON_ERROR,              \
    IOTHUB_REGISTRYMANAGER_HTTPAPI_ERROR,           \
    IOTHUB_REGISTRYMANAGER_HTTP_STATUS_ERROR,       \
    IOTHUB_REGISTRYMANAGER_DEVICE_EXIST,            \
    IOTHUB_REGISTRYMANAGER_DEVICE_NOT_EXIST,        \
    IOTHUB_REGISTRYMANAGER_CALLBACK_NOT_SET         \

DEFINE_ENUM(IOTHUB_REGISTRYMANAGER_RESULT, IOTHUB_REGISTRYMANAGER_RESULT_VALUES);

typedef struct IOTHUB_DEVICE_TAG
{
    const char* deviceId;
    const char* primaryKey;
    const char* secondaryKey;
    const char* generationId;
    const char* eTag;
    IOTHUB_DEVICE_CONNECTION_STATE connectionState;
    const char* connectionStateUpdatedTime;
    IOTHUB_DEVICE_STATUS status;
    const char* statusReason;
    const char* statusUpdatedTime;
    const char* lastActivityTime;
    size_t cloudToDeviceMessageCount;

    bool isManaged;
    const char* configuration;
    const char* deviceProperties;
    const char* serviceProperties;
} IOTHUB_DEVICE;

typedef struct IOTHUB_REGISTRY_DEVICE_CREATE_TAG
{
    const char* deviceId;
    const char* primaryKey;
    const char* secondaryKey;
} IOTHUB_REGISTRY_DEVICE_CREATE;

typedef struct IOTHUB_REGISTRY_DEVICE_UPDATE_TAG
{
    const char* deviceId;
    const char* primaryKey;
    const char* secondaryKey;
    IOTHUB_DEVICE_STATUS status;
} IOTHUB_REGISTRY_DEVICE_UPDATE;

typedef struct IOTHUB_REGISTRY_STATISTIC_TAG
{
    size_t totalDeviceCount;
    size_t enabledDeviceCount;
    size_t disabledDeviceCount;
} IOTHUB_REGISTRY_STATISTICS;

/** @brief Structure to store IoTHub authentication information
*/
typedef struct IOTHUB_REGISTRYMANAGER_TAG
{
    char* hostname;
    char* iothubName;
    char* iothubSuffix;
    char* sharedAccessKey;
    char* keyName;
} IOTHUB_REGISTRYMANAGER;

/** @brief Handle to hide struct and use it in consequent APIs
*/
typedef struct IOTHUB_REGISTRYMANAGER_TAG* IOTHUB_REGISTRYMANAGER_HANDLE;


/**
* @brief	Creates a IoT Hub Registry Manager handle for use it
* 			in consequent APIs.
*
* @param	serviceClientHandle	Service client handle.
*
* @return	A non-NULL @c IOTHUB_REGISTRYMANAGER_HANDLE value that is used when
* 			invoking other functions for IoT Hub REgistry Manager and @c NULL on failure.
*/
extern IOTHUB_REGISTRYMANAGER_HANDLE IoTHubRegistryManager_Create(IOTHUB_SERVICE_CLIENT_AUTH_HANDLE serviceClientHandle);

/**
* @brief	Disposes of resources allocated by the IoT Hub Registry Manager.
*
* @param	registryManagerHandle	The handle created by a call to the create function.
*/
extern void IoTHubRegistryManager_Destroy(IOTHUB_REGISTRYMANAGER_HANDLE registryManagerHandle);

/**
* @brief	Creates a device on IoT Hub.
* 			in consequent APIs.
*
* @param	registryManagerHandle   The handle created by a call to the create function.
* @param    deviceCreate            IOTHUB_REGISTRY_DEVICE_CREATE structure containing 
*                                   the new device Id, primaryKey (optional) and secondaryKey (optional)
* @param    device                  Input parameter, if it is not NULL will contain the created device info structure
*
* @return	IOTHUB_REGISTRYMANAGER_RESULT_OK upon success or an error code upon failure.
*/
extern IOTHUB_REGISTRYMANAGER_RESULT IoTHubRegistryManager_CreateDevice(IOTHUB_REGISTRYMANAGER_HANDLE registryManagerHandle, const IOTHUB_REGISTRY_DEVICE_CREATE* deviceCreate, IOTHUB_DEVICE* device);

/**
* @brief	Gets device info for a given device.
*
* @param	registryManagerHandle   The handle created by a call to the create function.
* @param	deviceId    The Id of the requested device.
* @param    device      Input parameter, if it is not NULL will contain the requested device info structure
*
* @return	IOTHUB_REGISTRYMANAGER_RESULT_OK upon success or an error code upon failure.
*/
extern IOTHUB_REGISTRYMANAGER_RESULT IoTHubRegistryManager_GetDevice(IOTHUB_REGISTRYMANAGER_HANDLE registryManagerHandle, const char* deviceId, IOTHUB_DEVICE* device);

/**
* @brief	Creates a device on IoT Hub.
* 			in consequent APIs.
*
* @param	registryManagerHandle   The handle created by a call to the create function.
* @param    deviceUpdate            IOTHUB_REGISTRY_DEVICE_UPDATE structure containing
*                                   the new device Id, primaryKey (optional), secondaryKey (optional) and status
*
* @return	IOTHUB_REGISTRYMANAGER_RESULT_OK upon success or an error code upon failure.
*/
extern IOTHUB_REGISTRYMANAGER_RESULT IoTHubRegistryManager_UpdateDevice(IOTHUB_REGISTRYMANAGER_HANDLE registryManagerHandle, IOTHUB_REGISTRY_DEVICE_UPDATE* deviceUpdate);

/**
* @brief	Deletes a given device.
*
* @param	registryManagerHandle   The handle created by a call to the create function.
* @param	deviceId    The Id of the device to delete.
*
* @return	IOTHUB_REGISTRYMANAGER_RESULT_OK upon success or an error code upon failure.
*/
extern IOTHUB_REGISTRYMANAGER_RESULT IoTHubRegistryManager_DeleteDevice(IOTHUB_REGISTRYMANAGER_HANDLE registryManagerHandle, const char* deviceId);

/**
* @brief	Gets device a list of devices registered on the IoTHUb.
*
* @param	registryManagerHandle   The handle created by a call to the create function.
* @param	numberOfDevices     Number of devices requested.
* @param    deviceList          Input parameter, if it is not NULL will contain the requested list of devices.
*
* @return	IOTHUB_REGISTRYMANAGER_RESULT_OK upon success or an error code upon failure.
*/
extern IOTHUB_REGISTRYMANAGER_RESULT IoTHubRegistryManager_GetDeviceList(IOTHUB_REGISTRYMANAGER_HANDLE registryManagerHandle, size_t numberOfDevices, LIST_HANDLE deviceList);

/**
* @brief	Gets the registry statistic info.
*
* @param	registryManagerHandle   The handle created by a call to the create function.
* @param    registryStatistics      Input parameter, if it is not NULL will contain the requested registry info.
*
* @return	IOTHUB_REGISTRYMANAGER_RESULT_OK upon success or an error code upon failure.
*/
extern IOTHUB_REGISTRYMANAGER_RESULT IoTHubRegistryManager_GetStatistics(IOTHUB_REGISTRYMANAGER_HANDLE registryManagerHandle, IOTHUB_REGISTRY_STATISTICS* registryStatistics);

#ifdef __cplusplus
}
#endif

#endif // IOTHUB_REGISTRYMANAGER_H
