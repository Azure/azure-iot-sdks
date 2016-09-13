// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// This file is under development and it is subject to change

#ifndef IOTHUB_DEVICETWIN_H
#define IOTHUB_DEVICETWIN_H

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

#include "azure_c_shared_utility/umock_c_prod.h"

#define IOTHUB_DEVICE_TWIN_RESULT_VALUES     \
    IOTHUB_DEVICE_TWIN_OK,                   \
    IOTHUB_DEVICE_TWIN_INVALID_ARG,          \
    IOTHUB_DEVICE_TWIN_ERROR,                \
    IOTHUB_DEVICE_TWIN_HTTPAPI_ERROR,        \
    IOTHUB_DEVICE_TWIN_JSON_EMPTY,           \
    IOTHUB_DEVICE_TWIN_JSON_ERROR            \

DEFINE_ENUM(IOTHUB_DEVICE_TWIN_RESULT, IOTHUB_DEVICE_TWIN_RESULT_VALUES);

typedef struct IOTHUB_DEVICE_TWIN_PROPERTY_TAG
{
    const char* key;
    const char* value;
} IOTHUB_DEVICE_TWIN_PROPERTY;

typedef struct IOTHUB_DEVICE_TWIN_PROPERTIES_TAG
{
    size_t size;
    IOTHUB_DEVICE_TWIN_PROPERTY* properties;
} IOTHUB_DEVICE_TWIN_PROPERTIES;

typedef struct IOTHUB_DEVICE_TWIN_TAG_TAG
{
    const char* name;
    const char* value;
} IOTHUB_DEVICE_TWIN_TAG;

typedef struct IOTHUB_DEVICE_TWIN_TAGS_TAG
{
    size_t size;
    IOTHUB_DEVICE_TWIN_TAG* tags;
} IOTHUB_DEVICE_TWIN_TAGS;

typedef struct IOTHUB_DEVICE_TWIN_TAG
{
    const char* deviceId;
    IOTHUB_DEVICE_TWIN_TAGS tags;
    IOTHUB_DEVICE_TWIN_PROPERTIES reportedProperties;
    IOTHUB_DEVICE_TWIN_PROPERTIES desiredProperties;
} IOTHUB_DEVICE_TWIN;

/** @brief Handle to hide struct and use it in consequent APIs
*/
typedef struct IOTHUB_SERVICE_CLIENT_DEVICE_TWIN_TAG* IOTHUB_SERVICE_CLIENT_DEVICE_TWIN_HANDLE;


/** @brief	Creates a IoT Hub Service Client DeviceTwin handle for use it in consequent APIs.
*
* @param	serviceClientHandle	Service client handle.
*
* @return	A non-NULL @c IOTHUB_SERVICE_CLIENT_DEVICE_TWIN_HANDLE value that is used when
* 			invoking other functions for IoT Hub DeviceTwin and @c NULL on failure.
*/
MOCKABLE_FUNCTION(, IOTHUB_SERVICE_CLIENT_DEVICE_TWIN_HANDLE, IoTHubDeviceTwin_Create, IOTHUB_SERVICE_CLIENT_AUTH_HANDLE, serviceClientHandle);

/** @brief	Disposes of resources allocated by the IoT Hub IoTHubDeviceTwin_Create.
*
* @param	serviceClientDeviceTwinHandle	The handle created by a call to the create function.
*/
MOCKABLE_FUNCTION(, void,  IoTHubDeviceTwin_Destroy, IOTHUB_SERVICE_CLIENT_DEVICE_TWIN_HANDLE, serviceClientDeviceTwinHandle);

/** @brief	Retrieves the given device's twin info.
*
* @param	serviceClientDeviceTwinHandle	The handle created by a call to the create function.
* @param    deviceId      The device name (id) to retrieve twin info for.
* @param    deviceTwin    Input parameter, if it is not NULL will contain the requested device twin info.
*
* @return	IOTHUB_DEVICE_TWIN_OK upon success or an error code upon failure.
*/
MOCKABLE_FUNCTION(, IOTHUB_DEVICE_TWIN_RESULT,  IoTHubDeviceTwin_GetTwin, IOTHUB_SERVICE_CLIENT_DEVICE_TWIN_HANDLE, serviceClientDeviceTwinHandle, const char*, deviceId, IOTHUB_DEVICE_TWIN*, deviceTwin);

/** @brief	Updates (partial update) the given device's twin info.
*
* @param	serviceClientDeviceTwinHandle	The handle created by a call to the create function.
* @param    deviceTwin    DeviceTwin data structure containing the info (tags, desired properties) to update.
*                         All well-known read-only members are ignored.
*                         Properties provided with value of null are removed from twin's document.
*
* @return	IOTHUB_DEVICE_TWIN_OK upon success or an error code upon failure.
*/
MOCKABLE_FUNCTION(, IOTHUB_DEVICE_TWIN_RESULT,  IoTHubDeviceTwin_UpdateTwin, IOTHUB_SERVICE_CLIENT_DEVICE_TWIN_HANDLE, serviceClientDeviceTwinHandle, const IOTHUB_DEVICE_TWIN*, deviceTwin);

/** @brief	Updates (partial update) the given device's desired properties info.
*
* @param	serviceClientDeviceTwinHandle	The handle created by a call to the create function.
* @param    deviceTwin    DeviceTwin data structure containing the info (desired properties) to update.
*                         All well-known read-only members are ignored.
*                         Properties provided with value of null are removed from desired properties document.
*
* @return	IOTHUB_DEVICE_TWIN_OK upon success or an error code upon failure.
*/
MOCKABLE_FUNCTION(, IOTHUB_DEVICE_TWIN_RESULT,  IoTHubDeviceTwin_UpdateDesiredProperties, IOTHUB_SERVICE_CLIENT_DEVICE_TWIN_HANDLE, serviceClientDeviceTwinHandle, const char*, deviceId, const IOTHUB_DEVICE_TWIN_PROPERTIES*, deviceTwinProperties);

/** @brief	Converts DeviceTwin info to formatted JSon string.
*
* @param    deviceTwin    DeviceTwin data structure containing the info.
*
* @return	Formatted JSon string containing all the device info upon success or an error code upon failure.
*/
MOCKABLE_FUNCTION(, const char*,  IoTHubDeviceTwin_ToJson, const IOTHUB_DEVICE_TWIN*, deviceTwin);

/** @brief	Converts DeviceTwin info to formatted JSon string.
*
* @param    deviceTwin    DeviceTwin data structure containing the info.
*
* @return	Formatted JSon string containing only the tag info upon success or an error code upon failure.
*/
MOCKABLE_FUNCTION(, const char*,  IoTHubDeviceTwin_TagsToJson, const IOTHUB_DEVICE_TWIN*, deviceTwin);

/** @brief	Converts DeviceTwin info to formatted JSon string.
*
* @param    deviceTwin    DeviceTwin data structure containing the info.
*
* @return	Formatted JSon string containing only the reported properties upon success or an error code upon failure.
*/
MOCKABLE_FUNCTION(, const char*,  IoTHubDeviceTwin_ReportedPropertiesToJson, const IOTHUB_DEVICE_TWIN*, deviceTwin);

/** @brief	Converts DeviceTwin info to formatted JSon string.
*
* @param    deviceTwin    DeviceTwin data structure containing the info.
*
* @return	Formatted JSon string containing only the desired properties upon success or an error code upon failure.
*/
MOCKABLE_FUNCTION(, const char*,  IoTHubDeviceTwin_DesiredPropertiesToJson, const IOTHUB_DEVICE_TWIN*, deviceTwin);

#ifdef __cplusplus
}
#endif

#endif // IOTHUB_DEVICETWIN_H
