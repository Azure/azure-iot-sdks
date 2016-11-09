// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include <ctype.h>
#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_c_shared_utility/string_tokenizer.h"
#include "azure_c_shared_utility/singlylinkedlist.h"
#include "azure_c_shared_utility/buffer_.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/httpapiex.h"
#include "azure_c_shared_utility/httpapiexsas.h"

#include "parson.h"
#include "connection_string_parser.h"
#include "iothub_registrymanager.h"

#define IOTHUB_REQUEST_MODE_VALUES    \
    IOTHUB_REQUEST_CREATE,            \
    IOTHUB_REQUEST_GET,               \
    IOTHUB_REQUEST_UPDATE,            \
    IOTHUB_REQUEST_DELETE,            \
    IOTHUB_REQUEST_GET_DEVICE_LIST,   \
    IOTHUB_REQUEST_GET_STATISTICS     \

DEFINE_ENUM(IOTHUB_REQUEST_MODE, IOTHUB_REQUEST_MODE_VALUES);

#define  HTTP_HEADER_KEY_AUTHORIZATION  "Authorization"
#define  HTTP_HEADER_VAL_AUTHORIZATION  " "
#define  HTTP_HEADER_KEY_REQUEST_ID  "Request-Id"
#define  HTTP_HEADER_VAL_REQUEST_ID  "1001"
#define  HTTP_HEADER_KEY_USER_AGENT  "User-Agent"
#define  HTTP_HEADER_VAL_USER_AGENT  "Microsoft.Azure.Devices/1.0.0"
#define  HTTP_HEADER_KEY_ACCEPT  "Accept"
#define  HTTP_HEADER_VAL_ACCEPT  "application/json"
#define  HTTP_HEADER_KEY_CONTENT_TYPE  "Content-Type"
#define  HTTP_HEADER_VAL_CONTENT_TYPE  "application/json; charset=utf-8"
#define  HTTP_HEADER_KEY_IFMATCH  "If-Match"
#define  HTTP_HEADER_VAL_IFMATCH  "*"

static size_t IOTHUB_DEVICES_MAX_REQUEST = 1000;

static void* DEVICE_JSON_DEFAULT_VALUE_NULL = NULL;
static const char* DEVICE_JSON_KEY_DEVICE_NAME = "deviceId";
static const char* DEVICE_JSON_KEY_DEVICE_PRIMARY_KEY = "authentication.symmetricKey.primaryKey";
static const char* DEVICE_JSON_KEY_DEVICE_SECONDARY_KEY = "authentication.symmetricKey.secondaryKey";

static const char* DEVICE_JSON_KEY_DEVICE_GENERATION_ID = "generationId";
static const char* DEVICE_JSON_KEY_DEVICE_ETAG = "etag";

static const char* DEVICE_JSON_KEY_DEVICE_CONNECTIONSTATE = "connectionState";
static const char* DEVICE_JSON_KEY_DEVICE_CONNECTIONSTATEUPDATEDTIME = "connectionStateUpdatedTime";

static const char* DEVICE_JSON_KEY_DEVICE_STATUS = "status";
static const char* DEVICE_JSON_KEY_DEVICE_STATUSREASON = "statusReason";
static const char* DEVICE_JSON_KEY_DEVICE_STATUSUPDATEDTIME = "statusUpdatedTime";

static const char* DEVICE_JSON_KEY_DEVICE_LASTACTIVITYTIME = "lastActivityTime";
static const char* DEVICE_JSON_KEY_DEVICE_CLOUDTODEVICEMESSAGECOUNT = "cloudToDeviceMessageCount";

static const char* DEVICE_JSON_KEY_DEVICE_ISMANAGED = "isManaged";
static const char* DEVICE_JSON_KEY_DEVICE_CONFIGURATION = "configuration";
static const char* DEVICE_JSON_KEY_DEVICE_DEVICEROPERTIES = "deviceProperties";
static const char* DEVICE_JSON_KEY_DEVICE_SERVICEPROPERTIES = "serviceProperties";

static const char* DEVICE_JSON_KEY_TOTAL_DEVICECOUNT = "totalDeviceCount";
static const char* DEVICE_JSON_KEY_ENABLED_DEVICECCOUNT = "enabledDeviceCount";
static const char* DEVICE_JSON_KEY_DISABLED_DEVICECOUNT = "disabledDeviceCount";

static const char* DEVICE_JSON_DEFAULT_VALUE_ENABLED = "Enabled";
static const char* DEVICE_JSON_DEFAULT_VALUE_DISABLED = "Disabled";
static const char* DEVICE_JSON_DEFAULT_VALUE_CONNECTED = "Connected";
static const char* DEVICE_JSON_DEFAULT_VALUE_DISCONNECTED = "Disconnected";
static const char* DEVICE_JSON_DEFAULT_VALUE_TIME = "0001-01-01T00:00:00";
static const char* DEVICE_JSON_DEFAULT_VALUE_TRUE = "true";
static const char* DEVICE_JSON_DEFAULT_VALUE_FALSE = "false";

static const char* URL_API_VERSION = "api-version=2016-11-14";

static const char* RELATIVE_PATH_FMT_CRUD = "/devices/%s?%s";
static const char* RELATIVE_PATH_FMT_LIST = "/devices/?top=%s&%s";
static const char* RELATIVE_PATH_FMT_STAT = "/statistics/devices?%s";

static int strHasNoWhitespace(const char* s)
{
    while (*s)
    {
        if (isspace(*s) != 0)
            return 1;
        s++;
    }
    return 0;
}

static BUFFER_HANDLE constructDeviceJson(const IOTHUB_DEVICE* deviceInfo)
{
    BUFFER_HANDLE result;

    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_010: [ IoTHubRegistryManager_CreateDevice shall create a flat "key1:value2,key2:value2..." JSON representation from the given deviceCreateInfo parameter using the following parson APIs: json_value_init_object, json_value_get_object, json_object_set_string, json_object_dotset_string ] */
    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_012: [ IoTHubRegistryManager_CreateDevice shall set the "symmetricKey" value to deviceCreateInfo->primaryKey and deviceCreateInfo->secondaryKey ] */
    JSON_Value* root_value = NULL;
    JSON_Object* root_object = NULL;
    JSON_Status jsonStatus;

    if (deviceInfo == NULL)
    {
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_013: [ IoTHubRegistryManager_CreateDevice shall return IOTHUB_REGISTRYMANAGER_ERROR_CREATING_JSON if the JSON creation failed  ] */
        LogError("deviceInfo cannot be null");
        result = NULL;
    }
    else if (deviceInfo->deviceId == NULL)
    {
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_013: [ IoTHubRegistryManager_CreateDevice shall return IOTHUB_REGISTRYMANAGER_ERROR_CREATING_JSON if the JSON creation failed  ] */
        LogError("Device id cannot be NULL");
        result = NULL;
    }
    if ((root_value = json_value_init_object()) == NULL)
    {
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_013: [ IoTHubRegistryManager_CreateDevice shall return IOTHUB_REGISTRYMANAGER_ERROR_CREATING_JSON if the JSON creation failed  ] */
        LogError("json_value_init_object failed");
        result = NULL;
    }
    else if ((root_object = json_value_get_object(root_value)) == NULL)
    {
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_013: [ IoTHubRegistryManager_CreateDevice shall return IOTHUB_REGISTRYMANAGER_ERROR_CREATING_JSON if the JSON creation failed  ] */
        LogError("json_value_get_object failed");
        result = NULL;
    }
    else if ((json_object_set_string(root_object, DEVICE_JSON_KEY_DEVICE_NAME, deviceInfo->deviceId)) != JSONSuccess)
    {
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_013: [ IoTHubRegistryManager_CreateDevice shall return IOTHUB_REGISTRYMANAGER_ERROR_CREATING_JSON if the JSON creation failed  ] */
        LogError("json_object_set_string failed for deviceId");
        result = NULL;
    }
    else if ((json_object_dotset_string(root_object, DEVICE_JSON_KEY_DEVICE_PRIMARY_KEY, deviceInfo->primaryKey)) != JSONSuccess)
    {
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_013: [ IoTHubRegistryManager_CreateDevice shall return IOTHUB_REGISTRYMANAGER_ERROR_CREATING_JSON if the JSON creation failed  ] */
        LogError("json_object_dotset_string failed for primaryKey");
        result = NULL;
    }
    else if ((json_object_dotset_string(root_object, DEVICE_JSON_KEY_DEVICE_SECONDARY_KEY, deviceInfo->secondaryKey)) != JSONSuccess)
    {
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_013: [ IoTHubRegistryManager_CreateDevice shall return IOTHUB_REGISTRYMANAGER_ERROR_CREATING_JSON if the JSON creation failed  ] */
        LogError("json_object_dotset_string failed for secondaryKey");
        result = NULL;
    }
    else
    {
        char* serialized_string;
        if ((serialized_string = json_serialize_to_string(root_value)) == NULL)
        {
            /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_013: [ IoTHubRegistryManager_CreateDevice shall return IOTHUB_REGISTRYMANAGER_ERROR_CREATING_JSON if the JSON creation failed  ] */
            LogError("json_serialize_to_string failed");
            result = NULL;
        }
        else
        {
            if ((result = BUFFER_create((const unsigned char*)serialized_string, strlen(serialized_string))) == NULL)
            {
                /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_013: [ IoTHubRegistryManager_CreateDevice shall return IOTHUB_REGISTRYMANAGER_ERROR_CREATING_JSON if the JSON creation failed  ] */
                LogError("Buffer_Create failed");
                result = NULL;
            }
            json_free_serialized_string(serialized_string);
        }
    }

    if ((jsonStatus = json_object_clear(root_object)) != JSONSuccess)
    {
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_013: [ IoTHubRegistryManager_CreateDevice shall return IOTHUB_REGISTRYMANAGER_ERROR_CREATING_JSON if the JSON creation failed  ] */
        LogError("json_object_clear failed");
        BUFFER_delete(result);
        result = NULL;
    }
    if(root_value != NULL)
        json_value_free(root_value);

    return result;
}

static IOTHUB_REGISTRYMANAGER_RESULT parseDeviceJson(BUFFER_HANDLE jsonBuffer, IOTHUB_DEVICE* deviceInfo)
{
    IOTHUB_REGISTRYMANAGER_RESULT result;

    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_024: [ If the deviceInfo out parameter is not NULL IoTHubRegistryManager_CreateDevice shall save the received deviceInfo to the out parameter and return IOTHUB_REGISTRYMANAGER_OK ] */
    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_033: [ IoTHubRegistryManager_GetDevice shall verify the received HTTP status code and if it is less or equal than 300 then try to parse the response JSON to deviceInfo for the following properties: deviceId, primaryKey, secondaryKey, generationId, eTag, connectionState, connectionstateUpdatedTime, status, statusReason, statusUpdatedTime, lastActivityTime, cloudToDeviceMessageCount ] */
    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_034: [ If any of the property field above missing from the JSON the property value will not be populated ] */
    if (jsonBuffer == NULL)
    {
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_023: [ If the JSON parsing failed, IoTHubRegistryManager_CreateDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR ] */
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_035: [ If the JSON parsing failed, IoTHubRegistryManager_GetDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR ] */
        LogError("jsonBuffer cannot be NULL");
        result = IOTHUB_REGISTRYMANAGER_JSON_ERROR;
    }
    else if (deviceInfo == NULL)
    {
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_023: [ If the JSON parsing failed, IoTHubRegistryManager_CreateDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR ] */
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_035: [ If the JSON parsing failed, IoTHubRegistryManager_GetDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR ] */
        LogError("deviceInfo cannot be NULL");
        result = IOTHUB_REGISTRYMANAGER_JSON_ERROR;
    }
    else
    {
        const char* bufferStr = NULL;
        JSON_Value* root_value = NULL;
        JSON_Object* root_object = NULL;
        JSON_Status jsonStatus;

        deviceInfo->deviceId = NULL;
        deviceInfo->primaryKey = NULL;
        deviceInfo->secondaryKey = NULL;
        deviceInfo->generationId = NULL;
        deviceInfo->eTag = NULL;
        deviceInfo->connectionState = IOTHUB_DEVICE_CONNECTION_STATE_DISCONNECTED;
        deviceInfo->connectionStateUpdatedTime = NULL;
        deviceInfo->status = IOTHUB_DEVICE_STATUS_DISABLED;
        deviceInfo->statusReason = NULL;
        deviceInfo->statusUpdatedTime = NULL;
        deviceInfo->lastActivityTime = NULL;
        deviceInfo->cloudToDeviceMessageCount = 0;
        deviceInfo->isManaged = false;
        deviceInfo->configuration = NULL;
        deviceInfo->deviceProperties = NULL;
        deviceInfo->serviceProperties = NULL;

        if ((bufferStr = (const char*)BUFFER_u_char(jsonBuffer)) == NULL)
        {
            /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_023: [ If the JSON parsing failed, IoTHubRegistryManager_CreateDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR ] */
            /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_035: [ If the JSON parsing failed, IoTHubRegistryManager_GetDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR ] */
            LogError("BUFFER_u_char failed");
            result = IOTHUB_REGISTRYMANAGER_JSON_ERROR;
        }
        else if ((root_value = json_parse_string(bufferStr)) == NULL)
        {
            /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_023: [ If the JSON parsing failed, IoTHubRegistryManager_CreateDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR ] */
            /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_035: [ If the JSON parsing failed, IoTHubRegistryManager_GetDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR ] */
            LogError("json_parse_string failed");
            result = IOTHUB_REGISTRYMANAGER_JSON_ERROR;
        }
        else if ((root_object = json_value_get_object(root_value)) == NULL)
        {
            /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_023: [ If the JSON parsing failed, IoTHubRegistryManager_CreateDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR ] */
            /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_035: [ If the JSON parsing failed, IoTHubRegistryManager_GetDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR ] */
            LogError("json_value_get_object failed");
            result = IOTHUB_REGISTRYMANAGER_JSON_ERROR;
        }
        else
        {
            const char* deviceId = (char*)json_object_get_string(root_object, DEVICE_JSON_KEY_DEVICE_NAME);
            const char* primaryKey = (char*)json_object_dotget_string(root_object, DEVICE_JSON_KEY_DEVICE_PRIMARY_KEY);
            const char* secondaryKey = (char*)json_object_dotget_string(root_object, DEVICE_JSON_KEY_DEVICE_SECONDARY_KEY);
            const char* generationId = (char*)json_object_get_string(root_object, DEVICE_JSON_KEY_DEVICE_GENERATION_ID);
            const char* eTag = (char*)json_object_get_string(root_object, DEVICE_JSON_KEY_DEVICE_ETAG);
            const char* connectionState = (char*)json_object_get_string(root_object, DEVICE_JSON_KEY_DEVICE_CONNECTIONSTATE);
            const char* connectionStateUpdatedTime = (char*)json_object_get_string(root_object, DEVICE_JSON_KEY_DEVICE_CONNECTIONSTATEUPDATEDTIME);
            const char* status = (char*)json_object_get_string(root_object, DEVICE_JSON_KEY_DEVICE_STATUS);
            const char* statusReason = (char*)json_object_get_string(root_object, DEVICE_JSON_KEY_DEVICE_STATUSREASON);
            const char* statusUpdatedTime = (char*)json_object_get_string(root_object, DEVICE_JSON_KEY_DEVICE_STATUSUPDATEDTIME);
            const char* lastActivityTime = (char*)json_object_get_string(root_object, DEVICE_JSON_KEY_DEVICE_LASTACTIVITYTIME);
            const char* cloudToDeviceMessageCount = (char*)json_object_get_string(root_object, DEVICE_JSON_KEY_DEVICE_CLOUDTODEVICEMESSAGECOUNT);
            const char* isManaged = (char*)json_object_get_string(root_object, DEVICE_JSON_KEY_DEVICE_ISMANAGED);
            const char* configuration = (char*)json_object_get_string(root_object, DEVICE_JSON_KEY_DEVICE_CONFIGURATION);
            const char* deviceProperties = (char*)json_object_get_string(root_object, DEVICE_JSON_KEY_DEVICE_DEVICEROPERTIES);
            const char* serviceProperties = (char*)json_object_get_string(root_object, DEVICE_JSON_KEY_DEVICE_SERVICEPROPERTIES);

            if ((deviceId != NULL) && (mallocAndStrcpy_s((char**)&(deviceInfo->deviceId), deviceId) != 0))
            {
                /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_023: [ If the JSON parsing failed, IoTHubRegistryManager_CreateDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR ] */
                /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_035: [ If the JSON parsing failed, IoTHubRegistryManager_GetDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR ] */
                LogError("mallocAndStrcpy_s failed for deviceId");
                result = IOTHUB_REGISTRYMANAGER_JSON_ERROR;
            }
            else if ((primaryKey != NULL) && (mallocAndStrcpy_s((char**)&deviceInfo->primaryKey, primaryKey) != 0))
            {
                /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_023: [ If the JSON parsing failed, IoTHubRegistryManager_CreateDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR ] */
                /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_035: [ If the JSON parsing failed, IoTHubRegistryManager_GetDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR ] */
                LogError("mallocAndStrcpy_s failed for primaryKey");
                result = IOTHUB_REGISTRYMANAGER_JSON_ERROR;
            }
            else if ((secondaryKey != NULL) && (mallocAndStrcpy_s((char**)&deviceInfo->secondaryKey, secondaryKey) != 0))
            {
                /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_023: [ If the JSON parsing failed, IoTHubRegistryManager_CreateDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR ] */
                /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_035: [ If the JSON parsing failed, IoTHubRegistryManager_GetDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR ] */
                LogError("mallocAndStrcpy_s failed for secondaryKey");
                result = IOTHUB_REGISTRYMANAGER_JSON_ERROR;
            }
            else if ((generationId != NULL) && (mallocAndStrcpy_s((char**)&deviceInfo->generationId, generationId) != 0))
            {
                /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_023: [ If the JSON parsing failed, IoTHubRegistryManager_CreateDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR ] */
                /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_035: [ If the JSON parsing failed, IoTHubRegistryManager_GetDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR ] */
                LogError("mallocAndStrcpy_s failed for generationId");
                result = IOTHUB_REGISTRYMANAGER_JSON_ERROR;
            }
            else if ((eTag != NULL) && (mallocAndStrcpy_s((char**)&deviceInfo->eTag, eTag) != 0))
            {
                /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_023: [ If the JSON parsing failed, IoTHubRegistryManager_CreateDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR ] */
                /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_035: [ If the JSON parsing failed, IoTHubRegistryManager_GetDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR ] */
                LogError("mallocAndStrcpy_s failed for eTag");
                result = IOTHUB_REGISTRYMANAGER_JSON_ERROR;
            }
            else if ((connectionStateUpdatedTime != NULL) && (mallocAndStrcpy_s((char**)&deviceInfo->connectionStateUpdatedTime, connectionStateUpdatedTime) != 0))
            {
                /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_023: [ If the JSON parsing failed, IoTHubRegistryManager_CreateDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR ] */
                /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_035: [ If the JSON parsing failed, IoTHubRegistryManager_GetDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR ] */
                LogError("mallocAndStrcpy_s failed for connectionStateUpdatedTime");
                result = IOTHUB_REGISTRYMANAGER_JSON_ERROR;
            }
            else if ((statusReason != NULL) && (mallocAndStrcpy_s((char**)&deviceInfo->statusReason, statusReason) != 0))
            {
                /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_023: [ If the JSON parsing failed, IoTHubRegistryManager_CreateDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR ] */
                /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_035: [ If the JSON parsing failed, IoTHubRegistryManager_GetDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR ] */
                LogError("mallocAndStrcpy_s failed for statusReason");
                result = IOTHUB_REGISTRYMANAGER_JSON_ERROR;
            }
            else if ((statusUpdatedTime != NULL) && (mallocAndStrcpy_s((char**)&deviceInfo->statusUpdatedTime, statusUpdatedTime) != 0))
            {
                /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_023: [ If the JSON parsing failed, IoTHubRegistryManager_CreateDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR ] */
                /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_035: [ If the JSON parsing failed, IoTHubRegistryManager_GetDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR ] */
                LogError("mallocAndStrcpy_s failed for statusUpdatedTime");
                result = IOTHUB_REGISTRYMANAGER_JSON_ERROR;
            }
            else if ((lastActivityTime != NULL) && (mallocAndStrcpy_s((char**)&deviceInfo->lastActivityTime, lastActivityTime) != 0))
            {
                /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_023: [ If the JSON parsing failed, IoTHubRegistryManager_CreateDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR ] */
                /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_035: [ If the JSON parsing failed, IoTHubRegistryManager_GetDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR ] */
                LogError("mallocAndStrcpy_s failed for lastActivityTime");
                result = IOTHUB_REGISTRYMANAGER_JSON_ERROR;
            }
            else if ((configuration != NULL) && (mallocAndStrcpy_s((char**)&deviceInfo->configuration, configuration) != 0))
            {
                /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_023: [ If the JSON parsing failed, IoTHubRegistryManager_CreateDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR ] */
                /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_035: [ If the JSON parsing failed, IoTHubRegistryManager_GetDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR ] */
                LogError("mallocAndStrcpy_s failed for configuration");
                result = IOTHUB_REGISTRYMANAGER_JSON_ERROR;
            }
            else if ((deviceProperties != NULL) && (mallocAndStrcpy_s((char**)&deviceInfo->deviceProperties, deviceProperties) != 0))
            {
                /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_023: [ If the JSON parsing failed, IoTHubRegistryManager_CreateDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR ] */
                /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_035: [ If the JSON parsing failed, IoTHubRegistryManager_GetDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR ] */
                LogError("mallocAndStrcpy_s failed for deviceProperties");
                result = IOTHUB_REGISTRYMANAGER_JSON_ERROR;
            }
            else if ((serviceProperties != NULL) && (mallocAndStrcpy_s((char**)&deviceInfo->serviceProperties, serviceProperties) != 0))
            {
                /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_023: [ If the JSON parsing failed, IoTHubRegistryManager_CreateDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR ] */
                /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_035: [ If the JSON parsing failed, IoTHubRegistryManager_GetDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR ] */
                LogError("mallocAndStrcpy_s failed for serviceProperties");
                result = IOTHUB_REGISTRYMANAGER_JSON_ERROR;
            }
            else
            {
                if ((connectionState != NULL) && (strcmp(connectionState, DEVICE_JSON_DEFAULT_VALUE_CONNECTED) == 0))
                {
                    deviceInfo->connectionState = IOTHUB_DEVICE_CONNECTION_STATE_CONNECTED;
                }
                if ((status != NULL) && (strcmp(status, DEVICE_JSON_DEFAULT_VALUE_ENABLED) == 0))
                {
                    deviceInfo->status = IOTHUB_DEVICE_STATUS_ENABLED;
                }
                if (cloudToDeviceMessageCount != NULL)
                {
                    deviceInfo->cloudToDeviceMessageCount = atoi(cloudToDeviceMessageCount);
                }
                if ((isManaged != NULL) && (strcmp(isManaged, DEVICE_JSON_DEFAULT_VALUE_TRUE) == 0))
                {
                    deviceInfo->isManaged = true;
                }
                result = IOTHUB_REGISTRYMANAGER_OK;
            }
        }

        if ((jsonStatus = json_object_clear(root_object)) != JSONSuccess)
        {
            /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_023: [ If the JSON parsing failed, IoTHubRegistryManager_CreateDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR ] */
            /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_035: [ If the JSON parsing failed, IoTHubRegistryManager_GetDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR ] */
            LogError("json_object_clear failed");
            result = IOTHUB_REGISTRYMANAGER_JSON_ERROR;
        }

        json_value_free(root_value);

        if (result != IOTHUB_REGISTRYMANAGER_OK)
        {
            if (deviceInfo->deviceId != NULL)
            {
                free((void*)deviceInfo->deviceId);
                deviceInfo->deviceId = NULL;
            }
            if (deviceInfo->primaryKey != NULL)
            {
                free((void*)deviceInfo->primaryKey);
                deviceInfo->primaryKey = NULL;
            }
            if (deviceInfo->secondaryKey != NULL)
            {
                free((void*)deviceInfo->secondaryKey);
                deviceInfo->secondaryKey = NULL;
            }
            if (deviceInfo->generationId != NULL)
            {
                free((void*)deviceInfo->generationId);
                deviceInfo->generationId = NULL;
            }
            if (deviceInfo->eTag != NULL)
            {
                free((void*)deviceInfo->eTag);
                deviceInfo->eTag = NULL;
            }
            if (deviceInfo->connectionStateUpdatedTime != NULL)
            {
                free((void*)deviceInfo->connectionStateUpdatedTime);
                deviceInfo->connectionStateUpdatedTime = NULL;
            }
            if (deviceInfo->statusReason != NULL)
            {
                free((void*)deviceInfo->statusReason);
                deviceInfo->statusReason = NULL;
            }
            if (deviceInfo->statusUpdatedTime != NULL)
            {
                free((void*)deviceInfo->statusUpdatedTime);
                deviceInfo->statusUpdatedTime = NULL;
            }
            if (deviceInfo->lastActivityTime != NULL)
            {
                free((void*)deviceInfo->lastActivityTime);
                deviceInfo->lastActivityTime = NULL;
            }
            if (deviceInfo->configuration != NULL)
            {
                free((void*)deviceInfo->configuration);
                deviceInfo->configuration = NULL;
            }
            if (deviceInfo->deviceProperties != NULL)
            {
                free((void*)deviceInfo->deviceProperties);
                deviceInfo->deviceProperties = NULL;
            }
            if (deviceInfo->serviceProperties != NULL)
            {
                free((void*)deviceInfo->serviceProperties);
                deviceInfo->serviceProperties = NULL;
            }
        }
    }
    return result;
}

static IOTHUB_REGISTRYMANAGER_RESULT parseDeviceListJson(BUFFER_HANDLE jsonBuffer, SINGLYLINKEDLIST_HANDLE deviceList)
{
    IOTHUB_REGISTRYMANAGER_RESULT result;

    const char* bufferStr = NULL;
    JSON_Value* root_value = NULL;
    JSON_Array* device_array = NULL;
    JSON_Status jsonStatus = JSONFailure;

    if (jsonBuffer == NULL)
    {
        LogError("jsonBuffer cannot be NULL");
        result = IOTHUB_REGISTRYMANAGER_JSON_ERROR;
    }
    else if (deviceList == NULL)
    {
        LogError("deviceList cannot be NULL");
        result = IOTHUB_REGISTRYMANAGER_JSON_ERROR;
    }
    else
    {
        if ((bufferStr = (const char*)BUFFER_u_char(jsonBuffer)) == NULL)
        {
            /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_072: [** If populating the deviceList parameter fails IoTHubRegistryManager_GetDeviceList shall return IOTHUB_REGISTRYMANAGER_ERROR **] */
            LogError("BUFFER_u_char failed");
            result = IOTHUB_REGISTRYMANAGER_ERROR;
        }
        else if ((root_value = json_parse_string(bufferStr)) == NULL)
        {
            /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_070: [** If any of the parson API fails, IoTHubRegistryManager_GetDeviceList shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR **] */
            LogError("json_parse_string failed");
            result = IOTHUB_REGISTRYMANAGER_JSON_ERROR;
        }
        else if ((device_array = json_value_get_array(root_value)) == NULL)
        {
            /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_070: [** If any of the parson API fails, IoTHubRegistryManager_GetDeviceList shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR **] */
            LogError("json_value_get_object failed");
            result = IOTHUB_REGISTRYMANAGER_JSON_ERROR;
        }
        else
        {
            result = IOTHUB_REGISTRYMANAGER_OK;

            size_t array_count = json_array_get_count(device_array);
            for (size_t i = 0; i < array_count; i++)
            {
                IOTHUB_DEVICE* iothubDevice = NULL;
                JSON_Object* device_object = NULL;

                // Create temp device struct
                if ((iothubDevice = (IOTHUB_DEVICE*)malloc(sizeof(IOTHUB_DEVICE))) == NULL)
                {
                    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_072: [** If populating the deviceList parameter fails IoTHubRegistryManager_GetDeviceList shall return IOTHUB_REGISTRYMANAGER_ERROR **] */
                    LogError("Malloc failed for iothubDevice");
                    result = IOTHUB_REGISTRYMANAGER_ERROR;
                }
                else if ((device_object = json_array_get_object(device_array, i)) == NULL)
                {
                    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_070: [** If any of the parson API fails, IoTHubRegistryManager_GetDeviceList shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR **] */
                    LogError("json_array_get_object failed");
                    free(iothubDevice);
                    result = IOTHUB_REGISTRYMANAGER_JSON_ERROR;
                }
                else
                {
                    iothubDevice->deviceId = NULL;
                    iothubDevice->primaryKey = NULL;
                    iothubDevice->secondaryKey = NULL;
                    iothubDevice->generationId = NULL;
                    iothubDevice->eTag = NULL;
                    iothubDevice->connectionStateUpdatedTime = NULL;
                    iothubDevice->connectionState = IOTHUB_DEVICE_CONNECTION_STATE_DISCONNECTED;
                    iothubDevice->status = IOTHUB_DEVICE_STATUS_DISABLED;
                    iothubDevice->statusReason = NULL;
                    iothubDevice->statusUpdatedTime = NULL;
                    iothubDevice->lastActivityTime = NULL;
                    iothubDevice->cloudToDeviceMessageCount = 0;
                    iothubDevice->isManaged = false;
                    iothubDevice->configuration = NULL;
                    iothubDevice->deviceProperties = NULL;
                    iothubDevice->serviceProperties = NULL;
                    const char* deviceId = (char*)json_object_get_string(device_object, DEVICE_JSON_KEY_DEVICE_NAME);
                    const char* primaryKey = (char*)json_object_dotget_string(device_object, DEVICE_JSON_KEY_DEVICE_PRIMARY_KEY);
                    const char* secondaryKey = (char*)json_object_dotget_string(device_object, DEVICE_JSON_KEY_DEVICE_SECONDARY_KEY);
                    const char* generationId = (char*)json_object_get_string(device_object, DEVICE_JSON_KEY_DEVICE_GENERATION_ID);
                    const char* eTag = (char*)json_object_get_string(device_object, DEVICE_JSON_KEY_DEVICE_ETAG);
                    const char* connectionState = (char*)json_object_get_string(device_object, DEVICE_JSON_KEY_DEVICE_CONNECTIONSTATE);
                    const char* connectionStateUpdatedTime = (char*)json_object_get_string(device_object, DEVICE_JSON_KEY_DEVICE_CONNECTIONSTATEUPDATEDTIME);
                    const char* status = (char*)json_object_get_string(device_object, DEVICE_JSON_KEY_DEVICE_STATUS);
                    const char* statusReason = (char*)json_object_get_string(device_object, DEVICE_JSON_KEY_DEVICE_STATUSREASON);
                    const char* statusUpdatedTime = (char*)json_object_get_string(device_object, DEVICE_JSON_KEY_DEVICE_STATUSUPDATEDTIME);
                    const char* lastActivityTime = (char*)json_object_get_string(device_object, DEVICE_JSON_KEY_DEVICE_LASTACTIVITYTIME);
                    const char* cloudToDeviceMessageCount = (char*)json_object_get_string(device_object, DEVICE_JSON_KEY_DEVICE_CLOUDTODEVICEMESSAGECOUNT);
                    const char* isManaged = (char*)json_object_get_string(device_object, DEVICE_JSON_KEY_DEVICE_ISMANAGED);
                    const char* configuration = (char*)json_object_get_string(device_object, DEVICE_JSON_KEY_DEVICE_CONFIGURATION);
                    const char* deviceProperties = (char*)json_object_get_string(device_object, DEVICE_JSON_KEY_DEVICE_DEVICEROPERTIES);
                    const char* serviceProperties = (char*)json_object_get_string(device_object, DEVICE_JSON_KEY_DEVICE_SERVICEPROPERTIES);

                    if ((deviceId != NULL) && (mallocAndStrcpy_s((char**)&(iothubDevice->deviceId), deviceId) != 0))
                    {
                        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_072: [** If populating the deviceList parameter fails IoTHubRegistryManager_GetDeviceList shall return IOTHUB_REGISTRYMANAGER_ERROR **] */
                        LogError("mallocAndStrcpy_s failed for deviceId");
                        free(iothubDevice);
                        result = IOTHUB_REGISTRYMANAGER_ERROR;
                    }
                    else if ((primaryKey != NULL) && (mallocAndStrcpy_s((char**)&(iothubDevice->primaryKey), primaryKey) != 0))
                    {
                        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_072: [** If populating the deviceList parameter fails IoTHubRegistryManager_GetDeviceList shall return IOTHUB_REGISTRYMANAGER_ERROR **] */
                        LogError("mallocAndStrcpy_s failed for primaryKey");
                        free((char*)iothubDevice->deviceId);
                        free(iothubDevice);
                        result = IOTHUB_REGISTRYMANAGER_ERROR;
                    }
                    else if ((secondaryKey != NULL) && (mallocAndStrcpy_s((char**)&(iothubDevice->secondaryKey), secondaryKey) != 0))
                    {
                        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_072: [** If populating the deviceList parameter fails IoTHubRegistryManager_GetDeviceList shall return IOTHUB_REGISTRYMANAGER_ERROR **] */
                        LogError("mallocAndStrcpy_s failed for secondaryKey");
                        free((char*)iothubDevice->deviceId);
                        free((char*)iothubDevice->primaryKey);
                        free(iothubDevice);
                        result = IOTHUB_REGISTRYMANAGER_ERROR;
                    }
                    else if ((generationId != NULL) && (mallocAndStrcpy_s((char**)&(iothubDevice->generationId), generationId) != 0))
                    {
                        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_072: [** If populating the deviceList parameter fails IoTHubRegistryManager_GetDeviceList shall return IOTHUB_REGISTRYMANAGER_ERROR **] */
                        LogError("mallocAndStrcpy_s failed for generationId");
                        free((char*)iothubDevice->deviceId);
                        free((char*)iothubDevice->primaryKey);
                        free((char*)iothubDevice->secondaryKey);
                        free(iothubDevice);
                        result = IOTHUB_REGISTRYMANAGER_ERROR;
                    }
                    else if ((eTag != NULL) && (mallocAndStrcpy_s((char**)&(iothubDevice->eTag), eTag) != 0))
                    {
                        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_072: [** If populating the deviceList parameter fails IoTHubRegistryManager_GetDeviceList shall return IOTHUB_REGISTRYMANAGER_ERROR **] */
                        LogError("mallocAndStrcpy_s failed for eTag");
                        free((char*)iothubDevice->deviceId);
                        free((char*)iothubDevice->primaryKey);
                        free((char*)iothubDevice->secondaryKey);
                        free((char*)iothubDevice->generationId);
                        free(iothubDevice);
                        result = IOTHUB_REGISTRYMANAGER_ERROR;
                    }
                    else if ((connectionStateUpdatedTime != NULL) && (mallocAndStrcpy_s((char**)&(iothubDevice->connectionStateUpdatedTime), connectionStateUpdatedTime) != 0))
                    {
                        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_072: [** If populating the deviceList parameter fails IoTHubRegistryManager_GetDeviceList shall return IOTHUB_REGISTRYMANAGER_ERROR **] */
                        LogError("mallocAndStrcpy_s failed for connectionStateUpdatedTime");
                        free((char*)iothubDevice->deviceId);
                        free((char*)iothubDevice->primaryKey);
                        free((char*)iothubDevice->secondaryKey);
                        free((char*)iothubDevice->generationId);
                        free((char*)iothubDevice->eTag);
                        free(iothubDevice);
                        result = IOTHUB_REGISTRYMANAGER_ERROR;
                    }
                    else if ((statusReason != NULL) && (mallocAndStrcpy_s((char**)&(iothubDevice->statusReason), statusReason) != 0))
                    {
                        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_072: [** If populating the deviceList parameter fails IoTHubRegistryManager_GetDeviceList shall return IOTHUB_REGISTRYMANAGER_ERROR **] */
                        LogError("mallocAndStrcpy_s failed for statusReason");
                        free((char*)iothubDevice->deviceId);
                        free((char*)iothubDevice->primaryKey);
                        free((char*)iothubDevice->secondaryKey);
                        free((char*)iothubDevice->generationId);
                        free((char*)iothubDevice->eTag);
                        free((char*)iothubDevice->connectionStateUpdatedTime);
                        free(iothubDevice);
                        result = IOTHUB_REGISTRYMANAGER_ERROR;
                    }
                    else if ((statusUpdatedTime != NULL) && (mallocAndStrcpy_s((char**)&(iothubDevice->statusUpdatedTime), statusUpdatedTime) != 0))
                    {
                        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_072: [** If populating the deviceList parameter fails IoTHubRegistryManager_GetDeviceList shall return IOTHUB_REGISTRYMANAGER_ERROR **] */
                        LogError("mallocAndStrcpy_s failed for statusUpdatedTime");
                        free((char*)iothubDevice->deviceId);
                        free((char*)iothubDevice->primaryKey);
                        free((char*)iothubDevice->secondaryKey);
                        free((char*)iothubDevice->generationId);
                        free((char*)iothubDevice->eTag);
                        free((char*)iothubDevice->connectionStateUpdatedTime);
                        free((char*)iothubDevice->statusReason);
                        free((char*)iothubDevice);
                        result = IOTHUB_REGISTRYMANAGER_ERROR;
                    }
                    else if ((lastActivityTime != NULL) && (mallocAndStrcpy_s((char**)&(iothubDevice->lastActivityTime), lastActivityTime) != 0))
                    {
                        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_072: [** If populating the deviceList parameter fails IoTHubRegistryManager_GetDeviceList shall return IOTHUB_REGISTRYMANAGER_ERROR **] */
                        LogError("mallocAndStrcpy_s failed for lastActivityTime");
                        free((char*)iothubDevice->deviceId);
                        free((char*)iothubDevice->primaryKey);
                        free((char*)iothubDevice->secondaryKey);
                        free((char*)iothubDevice->generationId);
                        free((char*)iothubDevice->eTag);
                        free((char*)iothubDevice->connectionStateUpdatedTime);
                        free((char*)iothubDevice->statusReason);
                        free((char*)iothubDevice->statusUpdatedTime);
                        free(iothubDevice);
                        result = IOTHUB_REGISTRYMANAGER_ERROR;
                    }
                    else if ((configuration != NULL) && (mallocAndStrcpy_s((char**)&(iothubDevice->configuration), configuration) != 0))
                    {
                        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_072: [** If populating the deviceList parameter fails IoTHubRegistryManager_GetDeviceList shall return IOTHUB_REGISTRYMANAGER_ERROR **] */
                        LogError("mallocAndStrcpy_s failed for configuration");
                        free((char*)iothubDevice->deviceId);
                        free((char*)iothubDevice->primaryKey);
                        free((char*)iothubDevice->secondaryKey);
                        free((char*)iothubDevice->generationId);
                        free((char*)iothubDevice->eTag);
                        free((char*)iothubDevice->connectionStateUpdatedTime);
                        free((char*)iothubDevice->statusReason);
                        free((char*)iothubDevice->statusUpdatedTime);
                        free((char*)iothubDevice->lastActivityTime);
                        free(iothubDevice);
                        result = IOTHUB_REGISTRYMANAGER_ERROR;
                    }
                    else if ((deviceProperties != NULL) && (mallocAndStrcpy_s((char**)&(iothubDevice->deviceProperties), deviceProperties) != 0))
                    {
                        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_072: [** If populating the deviceList parameter fails IoTHubRegistryManager_GetDeviceList shall return IOTHUB_REGISTRYMANAGER_ERROR **] */
                        LogError("mallocAndStrcpy_s failed for deviceProperties");
                        free((char*)iothubDevice->deviceId);
                        free((char*)iothubDevice->primaryKey);
                        free((char*)iothubDevice->secondaryKey);
                        free((char*)iothubDevice->generationId);
                        free((char*)iothubDevice->eTag);
                        free((char*)iothubDevice->connectionStateUpdatedTime);
                        free((char*)iothubDevice->statusReason);
                        free((char*)iothubDevice->statusUpdatedTime);
                        free((char*)iothubDevice->lastActivityTime);
                        free((char*)iothubDevice->configuration);
                        free(iothubDevice);
                        result = IOTHUB_REGISTRYMANAGER_ERROR;
                    }
                    else if ((serviceProperties != NULL) && (mallocAndStrcpy_s((char**)&(iothubDevice->serviceProperties), serviceProperties) != 0))
                    {
                        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_072: [** If populating the deviceList parameter fails IoTHubRegistryManager_GetDeviceList shall return IOTHUB_REGISTRYMANAGER_ERROR **] */
                        LogError("mallocAndStrcpy_s failed for serviceProperties");
                        free((char*)iothubDevice->deviceId);
                        free((char*)iothubDevice->primaryKey);
                        free((char*)iothubDevice->secondaryKey);
                        free((char*)iothubDevice->generationId);
                        free((char*)iothubDevice->eTag);
                        free((char*)iothubDevice->connectionStateUpdatedTime);
                        free((char*)iothubDevice->statusReason);
                        free((char*)iothubDevice->statusUpdatedTime);
                        free((char*)iothubDevice->lastActivityTime);
                        free((char*)iothubDevice->configuration);
                        free((char*)iothubDevice->deviceProperties);
                        free(iothubDevice);
                        result = IOTHUB_REGISTRYMANAGER_ERROR;
                    }
                    else
                    {
                        if ((connectionState != NULL) && (strcmp(connectionState, DEVICE_JSON_DEFAULT_VALUE_CONNECTED) == 0))
                        {
	                        iothubDevice->connectionState = IOTHUB_DEVICE_CONNECTION_STATE_CONNECTED;
                        }
                        if ((status != NULL) && (strcmp(status, DEVICE_JSON_DEFAULT_VALUE_ENABLED) == 0))
                        {
                            iothubDevice->status = IOTHUB_DEVICE_STATUS_ENABLED;
                        }
                        if (cloudToDeviceMessageCount != NULL)
                        {
                            iothubDevice->cloudToDeviceMessageCount = atoi(cloudToDeviceMessageCount);
                        }
                        if ((isManaged != NULL) && (strcmp(isManaged, DEVICE_JSON_DEFAULT_VALUE_TRUE) == 0))
                        {
                            iothubDevice->isManaged = true;
                        }

                        if ((singlylinkedlist_add(deviceList, iothubDevice)) == NULL)
                        {
                            /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_072: [** If populating the deviceList parameter fails IoTHubRegistryManager_GetDeviceList shall return IOTHUB_REGISTRYMANAGER_ERROR **] */
                            LogError("singlylinkedlist_add failed");
                            free((char*)iothubDevice->deviceId);
                            free((char*)iothubDevice->primaryKey);
                            free((char*)iothubDevice->secondaryKey);
                            free((char*)iothubDevice->generationId);
                            free((char*)iothubDevice->eTag);
                            free((char*)iothubDevice->connectionStateUpdatedTime);
                            free((char*)iothubDevice->statusReason);
                            free((char*)iothubDevice->statusUpdatedTime);
                            free((char*)iothubDevice->lastActivityTime);
                            free((char*)iothubDevice->configuration);
                            free((char*)iothubDevice->deviceProperties);
                            free((char*)iothubDevice->serviceProperties);
                            free(iothubDevice);
                            result = IOTHUB_REGISTRYMANAGER_JSON_ERROR;
                        }
                    }
                }

                if ((device_object != NULL) && ((jsonStatus = json_object_clear(device_object)) != JSONSuccess))
                {
                    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_070: [** If any of the parson API fails, IoTHubRegistryManager_GetDeviceList shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR **] */
                    LogError("json_object_clear failed");
                    result = IOTHUB_REGISTRYMANAGER_JSON_ERROR;
                }
                if (result != IOTHUB_REGISTRYMANAGER_OK)
                {
                    break;
                }
            }
        }
    }
    if (device_array != NULL)
    {
        if ((jsonStatus = json_array_clear(device_array)) != JSONSuccess)
        {
            /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_070: [** If any of the parson API fails, IoTHubRegistryManager_GetDeviceList shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR **] */
            LogError("json_array_clear failed");
            result = IOTHUB_REGISTRYMANAGER_JSON_ERROR;
        }
    }

    if (root_value != NULL)
    {
        json_value_free(root_value);
    }

    if (result != IOTHUB_REGISTRYMANAGER_OK)
    {
        if (deviceList != NULL)
        {
            LIST_ITEM_HANDLE itemHandle = singlylinkedlist_get_head_item(deviceList);
            while (itemHandle != NULL)
            {
                IOTHUB_DEVICE* deviceInfo = (IOTHUB_DEVICE*)singlylinkedlist_item_get_value(itemHandle);
                LIST_ITEM_HANDLE lastHandle = itemHandle;
                itemHandle = singlylinkedlist_get_next_item(itemHandle);

                if (deviceInfo->deviceId != NULL)
                    free((char*)deviceInfo->deviceId);
                if (deviceInfo->primaryKey != NULL)
                    free((char*)deviceInfo->primaryKey);
                if (deviceInfo->secondaryKey != NULL)
                    free((char*)deviceInfo->secondaryKey);
                if (deviceInfo->generationId != NULL)
                    free((char*)deviceInfo->generationId);
                if (deviceInfo->eTag != NULL)
                    free((char*)deviceInfo->eTag);
                if (deviceInfo->connectionStateUpdatedTime != NULL)
                    free((char*)deviceInfo->connectionStateUpdatedTime);
                if (deviceInfo->statusReason != NULL)
                    free((char*)deviceInfo->statusReason);
                if (deviceInfo->statusUpdatedTime != NULL)
                    free((char*)deviceInfo->statusUpdatedTime);
                if (deviceInfo->lastActivityTime != NULL)
                    free((char*)deviceInfo->lastActivityTime);
                if (deviceInfo->configuration != NULL)
                    free((char*)deviceInfo->configuration);
                if (deviceInfo->deviceProperties != NULL)
                    free((char*)deviceInfo->deviceProperties);
                if (deviceInfo->serviceProperties != NULL)
                    free((char*)deviceInfo->serviceProperties);
                free(deviceInfo);

                singlylinkedlist_remove(deviceList, lastHandle);
            }
        }
    }
    return result;
}

static IOTHUB_REGISTRYMANAGER_RESULT parseStatisticsJson(BUFFER_HANDLE jsonBuffer, IOTHUB_REGISTRY_STATISTICS* registryStatistics)
{
    IOTHUB_REGISTRYMANAGER_RESULT result;

    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_083: [ IoTHubRegistryManager_GetStatistics shall save the registry statistics to the out value and return IOTHUB_REGISTRYMANAGER_OK ] */
    if (jsonBuffer == NULL)
    {
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_082: [ If the parsing failed, IoTHubRegistryManager_GetStatistics shall return IOTHUB_REGISTRYMANAGER_ERROR ] */
        LogError("jsonBuffer cannot be NULL");
        result = IOTHUB_REGISTRYMANAGER_JSON_ERROR;
    }
    else if (registryStatistics == NULL)
    {
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_082: [ If the parsing failed, IoTHubRegistryManager_GetStatistics shall return IOTHUB_REGISTRYMANAGER_ERROR ] */
        LogError("registryStatistics cannot be NULL");
        result = IOTHUB_REGISTRYMANAGER_JSON_ERROR;
    }
    else
    {
        const char* bufferStr = NULL;
        JSON_Value* root_value = NULL;
        JSON_Object* root_object = NULL;
        JSON_Status jsonStatus;

        if ((bufferStr = (const char*)BUFFER_u_char(jsonBuffer)) == NULL)
        {
            /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_082: [ If the parsing failed, IoTHubRegistryManager_GetStatistics shall return IOTHUB_REGISTRYMANAGER_ERROR ] */
            LogError("BUFFER_u_char failed");
            result = IOTHUB_REGISTRYMANAGER_JSON_ERROR;
        }
        else if ((root_value = json_parse_string(bufferStr)) == NULL)
        {
            /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_082: [ If the parsing failed, IoTHubRegistryManager_GetStatistics shall return IOTHUB_REGISTRYMANAGER_ERROR ] */
            LogError("json_parse_string failed");
            result = IOTHUB_REGISTRYMANAGER_JSON_ERROR;
        }
        else if ((root_object = json_value_get_object(root_value)) == NULL)
        {
            /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_082: [ If the parsing failed, IoTHubRegistryManager_GetStatistics shall return IOTHUB_REGISTRYMANAGER_ERROR ] */
            LogError("json_value_get_object failed");
            result = IOTHUB_REGISTRYMANAGER_JSON_ERROR;
        }
        else
        {
            /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_083: [ IoTHubRegistryManager_GetStatistics shall save the registry statistics to the out value and return IOTHUB_REGISTRYMANAGER_OK ] */
            registryStatistics->totalDeviceCount = (size_t)json_object_get_number(root_object, DEVICE_JSON_KEY_TOTAL_DEVICECOUNT);
            registryStatistics->enabledDeviceCount = (size_t)json_object_get_number(root_object, DEVICE_JSON_KEY_ENABLED_DEVICECCOUNT);
            registryStatistics->disabledDeviceCount = (size_t)json_object_get_number(root_object, DEVICE_JSON_KEY_DISABLED_DEVICECOUNT);

            result = IOTHUB_REGISTRYMANAGER_OK;
        }

        if ((jsonStatus = json_object_clear(root_object)) != JSONSuccess)
        {
            /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_082: [ If the parsing failed, IoTHubRegistryManager_GetStatistics shall return IOTHUB_REGISTRYMANAGER_ERROR ] */
            LogError("json_object_clear failed");
            result = IOTHUB_REGISTRYMANAGER_JSON_ERROR;
        }
        json_value_free(root_value);
    }
    return result;
}

static IOTHUB_REGISTRYMANAGER_RESULT createRelativePath(IOTHUB_REQUEST_MODE iotHubRequestMode, const char* deviceName, size_t numberOfDevices, char* relativePath)
{
    IOTHUB_REGISTRYMANAGER_RESULT result;

    if (iotHubRequestMode == IOTHUB_REQUEST_GET_DEVICE_LIST)
    {
        if ((numberOfDevices <= 0) || (numberOfDevices > IOTHUB_DEVICES_MAX_REQUEST))
        {
            numberOfDevices = IOTHUB_DEVICES_MAX_REQUEST;
        }

        char numberStr[256];
        result = IOTHUB_REGISTRYMANAGER_ERROR;
        if (snprintf(numberStr, 256, "%zu", numberOfDevices) > 0)
        {
            if (snprintf(relativePath, 256, RELATIVE_PATH_FMT_LIST, numberStr, URL_API_VERSION) > 0)
            {
                result = IOTHUB_REGISTRYMANAGER_OK;
            }
            else
            {
                result = IOTHUB_REGISTRYMANAGER_ERROR;
            }
        }
        else
        {
            result = IOTHUB_REGISTRYMANAGER_ERROR;
        }
    }
    else if (iotHubRequestMode == IOTHUB_REQUEST_GET_STATISTICS)
    {
        if (snprintf(relativePath, 256, RELATIVE_PATH_FMT_STAT, URL_API_VERSION) > 0)
        {
            result = IOTHUB_REGISTRYMANAGER_OK;
        }
        else
        {
            result = IOTHUB_REGISTRYMANAGER_ERROR;
        }
    }
    else
    {
        if (snprintf(relativePath, 256, RELATIVE_PATH_FMT_CRUD, deviceName, URL_API_VERSION) > 0)
        {
            result = IOTHUB_REGISTRYMANAGER_OK;
        }
        else
        {
            result = IOTHUB_REGISTRYMANAGER_ERROR;
        }
    }

    return result;
}

static HTTP_HEADERS_HANDLE createHttpHeader(IOTHUB_REQUEST_MODE iotHubRequestMode)
{
    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_015: [ IoTHubRegistryManager_CreateDevice shall create an HTTP PUT request using the following HTTP headers: authorization=sasToken,Request-Id=1001,Accept=application/json,Content-Type=application/json,charset=utf-8 ] */
    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_027: [ IoTHubRegistryManager_GetDevice shall add the following headers to the created HTTP GET request: authorization=sasToken,Request-Id=1001,Accept=application/json,Content-Type=application/json,charset=utf-8 ] */
    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_054: [ IoTHubRegistryManager_DeleteDevice shall add the following headers to the created HTTP GET request : authorization = sasToken, Request - Id = 1001, Accept = application / json, Content - Type = application / json, charset = utf - 8 ] */
    HTTP_HEADERS_HANDLE httpHeader;

    if ((httpHeader = HTTPHeaders_Alloc()) == NULL)
    {
        LogError("HTTPHeaders_Alloc failed");
    }
    else if (HTTPHeaders_AddHeaderNameValuePair(httpHeader, HTTP_HEADER_KEY_AUTHORIZATION, HTTP_HEADER_VAL_AUTHORIZATION) != HTTP_HEADERS_OK)
    {
        LogError("HTTPHeaders_AddHeaderNameValuePair failed for Authorization header");
        HTTPHeaders_Free(httpHeader);
        httpHeader = NULL;
    }
    else if (HTTPHeaders_AddHeaderNameValuePair(httpHeader, HTTP_HEADER_KEY_REQUEST_ID, HTTP_HEADER_VAL_REQUEST_ID) != HTTP_HEADERS_OK)
    {
        LogError("HTTPHeaders_AddHeaderNameValuePair failed for RequestId header");
        HTTPHeaders_Free(httpHeader);
        httpHeader = NULL;
    }
    else if (HTTPHeaders_AddHeaderNameValuePair(httpHeader, HTTP_HEADER_KEY_USER_AGENT, HTTP_HEADER_VAL_USER_AGENT) != HTTP_HEADERS_OK)
    {
        LogError("HTTPHeaders_AddHeaderNameValuePair failed for User-Agent header");
        HTTPHeaders_Free(httpHeader);
        httpHeader = NULL;
    }
    else if (HTTPHeaders_AddHeaderNameValuePair(httpHeader, HTTP_HEADER_KEY_ACCEPT, HTTP_HEADER_VAL_ACCEPT) != HTTP_HEADERS_OK)
    {
        LogError("HTTPHeaders_AddHeaderNameValuePair failed for Accept header");
        HTTPHeaders_Free(httpHeader);
        httpHeader = NULL;
    }
    else if (HTTPHeaders_AddHeaderNameValuePair(httpHeader, HTTP_HEADER_KEY_CONTENT_TYPE, HTTP_HEADER_VAL_CONTENT_TYPE) != HTTP_HEADERS_OK)
    {
        LogError("HTTPHeaders_AddHeaderNameValuePair failed for Content-Type header");
        HTTPHeaders_Free(httpHeader);
        httpHeader = NULL;
    }

    if ((iotHubRequestMode == IOTHUB_REQUEST_DELETE) || (iotHubRequestMode == IOTHUB_REQUEST_UPDATE))
    {
        if (HTTPHeaders_AddHeaderNameValuePair(httpHeader, HTTP_HEADER_KEY_IFMATCH, HTTP_HEADER_VAL_IFMATCH) != HTTP_HEADERS_OK)
        {
            LogError("HTTPHeaders_AddHeaderNameValuePair failed for If-Match header");
            HTTPHeaders_Free(httpHeader);
            httpHeader = NULL;
        }

    }
    return httpHeader;
}

static IOTHUB_REGISTRYMANAGER_RESULT sendHttpRequestCRUD(IOTHUB_REGISTRYMANAGER_HANDLE registryManagerHandle, IOTHUB_REQUEST_MODE iotHubRequestMode, const char* deviceName, BUFFER_HANDLE deviceJsonBuffer, size_t numberOfDevices, BUFFER_HANDLE responseBuffer)
{
    IOTHUB_REGISTRYMANAGER_RESULT result;

    STRING_HANDLE uriResouce = NULL;
    STRING_HANDLE accessKey = NULL;
    STRING_HANDLE keyName = NULL;
    HTTPAPIEX_SAS_HANDLE httpExApiSasHandle = NULL;
    HTTPAPIEX_HANDLE httpExApiHandle = NULL;
    HTTP_HEADERS_HANDLE httpHeader = NULL;

    if ((uriResouce = STRING_construct(registryManagerHandle->hostname)) == NULL)
    {
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_099: [ If any of the call fails during the HTTP creation IoTHubRegistryManager_CreateDevice shall fail and return IOTHUB_REGISTRYMANAGER_ERROR ] */
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_103: [ If any of the call fails during the HTTP creation IoTHubRegistryManager_UpdateDevice shall fail and return IOTHUB_REGISTRYMANAGER_ERROR ] */
        LogError("STRING_construct failed for uriResource");
        result = IOTHUB_REGISTRYMANAGER_ERROR;
    }
    else if ((accessKey = STRING_construct(registryManagerHandle->sharedAccessKey)) == NULL)
    {
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_099: [ If any of the call fails during the HTTP creation IoTHubRegistryManager_CreateDevice shall fail and return IOTHUB_REGISTRYMANAGER_ERROR ] */
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_103: [ If any of the call fails during the HTTP creation IoTHubRegistryManager_UpdateDevice shall fail and return IOTHUB_REGISTRYMANAGER_ERROR ] */
        LogError("STRING_construct failed for accessKey");
        result = IOTHUB_REGISTRYMANAGER_ERROR;
    }
    else if ((keyName = STRING_construct(registryManagerHandle->keyName)) == NULL)
    {
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_099: [ If any of the call fails during the HTTP creation IoTHubRegistryManager_CreateDevice shall fail and return IOTHUB_REGISTRYMANAGER_ERROR ] */
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_103: [ If any of the call fails during the HTTP creation IoTHubRegistryManager_UpdateDevice shall fail and return IOTHUB_REGISTRYMANAGER_ERROR ] */
        LogError("STRING_construct failed for keyName");
        result = IOTHUB_REGISTRYMANAGER_ERROR;
    }
    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_015: [ IoTHubRegistryManager_CreateDevice shall create an HTTP PUT request using the following HTTP headers: authorization=sasToken,Request-Id=1001,Accept=application/json,Content-Type=application/json,charset=utf-8 ] */
    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_027: [ IoTHubRegistryManager_GetDevice shall add the following headers to the created HTTP GET request: authorization=sasToken,Request-Id=1001,Accept=application/json,Content-Type=application/json,charset=utf-8 ] */
    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_043: [ IoTHubRegistryManager_UpdateDevice shall create an HTTP PUT request using the created JSON ] */
    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_044: [ IoTHubRegistryManager_UpdateDevice shall create an HTTP PUT request using the createdfollowing HTTP headers : authorization = sasToken, Request - Id = 1001, Accept = application / json, Content - Type = application / json, charset = utf - 8 ] */
    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_054: [ IoTHubRegistryManager_DeleteDevice shall add the following headers to the created HTTP GET request : authorization=sasToken, Request-Id=1001, Accept=application/json, Content-Type=application/json, charset=utf-8 ] */
    else if ((httpHeader = createHttpHeader(iotHubRequestMode)) == NULL)
    {
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_019: [ If any of the HTTPAPI call fails IoTHubRegistryManager_CreateDevice shall fail and return IOTHUB_REGISTRYMANAGER_HTTPAPI_ERROR ] */
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_104: [ If any of the HTTPAPI call fails IoTHubRegistryManager_UpdateDevice shall fail and return IOTHUB_REGISTRYMANAGER_HTTPAPI_ERROR ] */
        LogError("HttpHeader creation failed");
        result = IOTHUB_REGISTRYMANAGER_HTTPAPI_ERROR;
    }
    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_016: [ IoTHubRegistryManager_CreateDevice shall create an HTTPAPIEX_SAS_HANDLE handle by calling HTTPAPIEX_SAS_Create ] */
    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_028: [ IoTHubRegistryManager_GetDevice shall create an HTTPAPIEX_SAS_HANDLE handle by calling HTTPAPIEX_SAS_Create ] */
    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_045: [ IoTHubRegistryManager_UpdateDevice shall create an HTTPAPIEX_SAS_HANDLE handle by calling HTTPAPIEX_SAS_Create ] */
    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_055: [ IoTHubRegistryManager_DeleteDevice shall create an HTTPAPIEX_SAS_HANDLE handle by calling HTTPAPIEX_SAS_Create ] */
    else if ((httpExApiSasHandle = HTTPAPIEX_SAS_Create(accessKey, uriResouce, keyName)) == NULL)
    {
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_019: [ If any of the HTTPAPI call fails IoTHubRegistryManager_CreateDevice shall fail and return IOTHUB_REGISTRYMANAGER_HTTPAPI_ERROR ] */
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_104: [ If any of the HTTPAPI call fails IoTHubRegistryManager_UpdateDevice shall fail and return IOTHUB_REGISTRYMANAGER_HTTPAPI_ERROR ] */
        LogError("HTTPAPIEX_SAS_Create failed");
        result = IOTHUB_REGISTRYMANAGER_HTTPAPI_ERROR;
    }
    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_017: [ IoTHubRegistryManager_CreateDevice shall create an HTTPAPIEX_HANDLE handle by calling HTTPAPIEX_Create ] */
    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_029: [ IoTHubRegistryManager_GetDevice shall create an HTTPAPIEX_HANDLE handle by calling HTTPAPIEX_Create ] */
    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_046: [ IoTHubRegistryManager_UpdateDevice shall create an HTTPAPIEX_HANDLE handle by calling HTTPAPIEX_Create ] */
    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_056: [ IoTHubRegistryManager_DeleteDevice shall create an HTTPAPIEX_HANDLE handle by calling HTTPAPIEX_Create ] */
    else if ((httpExApiHandle = HTTPAPIEX_Create(registryManagerHandle->hostname)) == NULL)
    {
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_019: [ If any of the HTTPAPI call fails IoTHubRegistryManager_CreateDevice shall fail and return IOTHUB_REGISTRYMANAGER_HTTPAPI_ERROR ] */
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_104: [ If any of the HTTPAPI call fails IoTHubRegistryManager_UpdateDevice shall fail and return IOTHUB_REGISTRYMANAGER_HTTPAPI_ERROR ] */
        LogError("HTTPAPIEX_Create failed");
        result = IOTHUB_REGISTRYMANAGER_HTTPAPI_ERROR;
    }
    else 
    {
        HTTPAPI_REQUEST_TYPE httpApiRequestType = HTTPAPI_REQUEST_GET;
        char relativePath[256];
        unsigned int statusCode;
        unsigned char is_error = 0;

        if ((iotHubRequestMode == IOTHUB_REQUEST_CREATE) || (iotHubRequestMode == IOTHUB_REQUEST_UPDATE))
        {
            httpApiRequestType = HTTPAPI_REQUEST_PUT;
        }
        else if (iotHubRequestMode == IOTHUB_REQUEST_DELETE)
        {
            httpApiRequestType = HTTPAPI_REQUEST_DELETE;
        }
        else if ((iotHubRequestMode == IOTHUB_REQUEST_GET) || (iotHubRequestMode == IOTHUB_REQUEST_GET_DEVICE_LIST) || (iotHubRequestMode == IOTHUB_REQUEST_GET_STATISTICS))
        {
            httpApiRequestType = HTTPAPI_REQUEST_GET;
        }
        else
        {
            is_error = 1;
        }

        if (is_error)
        {
            LogError("Invalid request type");
            result = IOTHUB_REGISTRYMANAGER_HTTPAPI_ERROR;
        }
        else
        {
            /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_026: [ IoTHubRegistryManager_GetDevice shall create HTTP GET request URL using the given deviceId using the following format: url/devices/[deviceId]?api-version=2016-11-14  ] */
            /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_053: [ IoTHubRegistryManager_DeleteDevice shall create HTTP DELETE request URL using the given deviceId using the following format : url/devices/[deviceId]?api-version ] */
            if (createRelativePath(iotHubRequestMode, deviceName, numberOfDevices, relativePath) != IOTHUB_REGISTRYMANAGER_OK)
            {
                LogError("Failure creating relative path");
                result = IOTHUB_REGISTRYMANAGER_ERROR;
            }
            /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_014: [ IoTHubRegistryManager_CreateDevice shall create an HTTP PUT request using the created JSON ] */
            /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_018: [ IoTHubRegistryManager_CreateDevice shall execute the HTTP PUT request by calling HTTPAPIEX_ExecuteRequest ] */
            /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_030: [ IoTHubRegistryManager_GetDevice shall execute the HTTP GET request by calling HTTPAPIEX_ExecuteRequest ] */
            /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_047: [ IoTHubRegistryManager_UpdateDevice shall execute the HTTP PUT request by calling HTTPAPIEX_ExecuteRequest ] */
            /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_057: [ IoTHubRegistryManager_DeleteDevice shall execute the HTTP DELETE request by calling HTTPAPIEX_ExecuteRequest ] */
            else if (HTTPAPIEX_SAS_ExecuteRequest(httpExApiSasHandle, httpExApiHandle, httpApiRequestType, relativePath, httpHeader, deviceJsonBuffer, &statusCode, NULL, responseBuffer) != HTTPAPIEX_OK)
            {
                /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_019: [ If any of the HTTPAPI call fails IoTHubRegistryManager_CreateDevice shall fail and return IOTHUB_REGISTRYMANAGER_HTTPAPI_ERROR ] */
                LogError("HTTPAPIEX_SAS_ExecuteRequest failed");
                result = IOTHUB_REGISTRYMANAGER_HTTPAPI_ERROR;
            }
            else
            {
                if (statusCode > 300)
                {
                    if ((iotHubRequestMode == IOTHUB_REQUEST_CREATE) && (statusCode == 409))
                    {
                        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_020: [ IoTHubRegistryManager_CreateDevice shall verify the received HTTP status code and if it is 409 then return IOTHUB_REGISTRYMANAGER_DEVICE_EXIST ] */
                        result = IOTHUB_REGISTRYMANAGER_DEVICE_EXIST;
                    }
                    else
                    {
                        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_021: [ IoTHubRegistryManager_CreateDevice shall verify the received HTTP status code and if it is greater than 300 then return IOTHUB_REGISTRYMANAGER_HTTP_STATUS_ERROR ] */
                        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_032: [ IoTHubRegistryManager_GetDevice shall verify the received HTTP status code and if it is greater than 300 then return IOTHUB_REGISTRYMANAGER_ERROR ] */
                        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_058: [ IoTHubRegistryManager_DeleteDevice shall verify the received HTTP status code and if it is greater than 300 then return IOTHUB_REGISTRYMANAGER_HTTP_STATUS_ERROR ] */
                        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_048: [ IoTHubRegistryManager_UpdateDevice shall verify the received HTTP status code and if it is greater than 300 then return IOTHUB_REGISTRYMANAGER_ERROR ] */
                        LogError("Http Failure status code %d.", statusCode);
                        result = IOTHUB_REGISTRYMANAGER_HTTP_STATUS_ERROR;
                    }
                }
                else
                {
                    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_022: [ IoTHubRegistryManager_CreateDevice shall verify the received HTTP status code and if it is less or equal than 300 then try to parse the response JSON to deviceInfo ] */
                    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_059: [ IoTHubRegistryManager_DeleteDevice shall verify the received HTTP status code and if it is less or equal than 300 then return IOTHUB_REGISTRYMANAGER_OK ] */
                    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_049: [ IoTHubRegistryManager_UpdateDevice shall verify the received HTTP status code and if it is less or equal than 300 then return IOTHUB_REGISTRYMANAGER_OK ] */
                    result = IOTHUB_REGISTRYMANAGER_OK;
                }
            }
        }
    }

    HTTPHeaders_Free(httpHeader);
    HTTPAPIEX_Destroy(httpExApiHandle);
    HTTPAPIEX_SAS_Destroy(httpExApiSasHandle);
    STRING_delete(keyName);
    STRING_delete(accessKey);
    STRING_delete(uriResouce);
    return result;
}

IOTHUB_REGISTRYMANAGER_HANDLE IoTHubRegistryManager_Create(IOTHUB_SERVICE_CLIENT_AUTH_HANDLE serviceClientHandle)
{
    IOTHUB_REGISTRYMANAGER_HANDLE result;

    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_001: [ If the serviceClientHandle input parameter is NULL IoTHubRegistryManager_Create shall return NULL ] */
    if (serviceClientHandle == NULL)
    {
        LogError("serviceClientHandle input parameter cannot be NULL");
        result = NULL;
    }
    else
    {
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_084: [ If any member of the serviceClientHandle input parameter is NULL IoTHubRegistryManager_Create shall return NULL ] */
        IOTHUB_SERVICE_CLIENT_AUTH* serviceClientAuth = (IOTHUB_SERVICE_CLIENT_AUTH*)serviceClientHandle;

        if (serviceClientAuth->hostname == NULL)
        {
            LogError("authInfo->hostName input parameter cannot be NULL");
            result = NULL;
        }
        else if (serviceClientAuth->iothubName == NULL)
        {
            LogError("authInfo->iothubName input parameter cannot be NULL");
            result = NULL;
        }
        else if (serviceClientAuth->iothubSuffix == NULL)
        {
            LogError("authInfo->iothubSuffix input parameter cannot be NULL");
            result = NULL;
        }
        else if (serviceClientAuth->keyName == NULL)
        {
            LogError("authInfo->keyName input parameter cannot be NULL");
            result = NULL;
        }
        else if (serviceClientAuth->sharedAccessKey == NULL)
        {
            LogError("authInfo->sharedAccessKey input parameter cannot be NULL");
            result = NULL;
        }
        else
        {
            /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_002: [ IoTHubRegistryManager_Create shall allocate memory for a new registry manager instance ] */
            result = malloc(sizeof(IOTHUB_REGISTRYMANAGER));
            if (result == NULL)
            {
                /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_003: [ If the allocation failed, IoTHubRegistryManager_Create shall return NULL ] */
                LogError("Malloc failed for IOTHUB_REGISTRYMANAGER");
            }
            else
            {
                /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_004: [ If the allocation successful, IoTHubRegistryManager_Create shall create a IOTHUB_REGISTRYMANAGER_HANDLE from the given IOTHUB_REGISTRYMANAGER_AUTH_HANDLE and return with it ] */
                /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_085: [ IoTHubRegistryManager_Create shall allocate memory and copy hostName to result->hostName by calling mallocAndStrcpy_s. ] */
                if (mallocAndStrcpy_s(&result->hostname, serviceClientAuth->hostname) != 0)
                {
                    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_086: [ If the mallocAndStrcpy_s fails, IoTHubRegistryManager_Create shall do clean up and return NULL. ] */
                    LogError("mallocAndStrcpy_s failed for hostName");
                    free(result);
                    result = NULL;
                }
                /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_087: [ IoTHubRegistryManager_Create shall allocate memory and copy iothubName to result->iothubName by calling mallocAndStrcpy_s. ] */
                else if (mallocAndStrcpy_s(&result->iothubName, serviceClientAuth->iothubName) != 0)
                {
                    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_088: [ If the mallocAndStrcpy_s fails, IoTHubRegistryManager_Create shall do clean up and return NULL. ] */
                    LogError("mallocAndStrcpy_s failed for iothubName");
                    free(result->hostname);
                    free(result);
                    result = NULL;
                }
                /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_089: [ IoTHubRegistryManager_Create shall allocate memory and copy iothubSuffix to result->iothubSuffix by calling mallocAndStrcpy_s. ] */
                else if (mallocAndStrcpy_s(&result->iothubSuffix, serviceClientAuth->iothubSuffix) != 0)
                {
                    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_090: [ If the mallocAndStrcpy_s fails, IoTHubRegistryManager_Create shall do clean up and return NULL. ] */
                    LogError("mallocAndStrcpy_s failed for iothubSuffix");
                    free(result->hostname);
                    free(result->iothubName);
                    free(result);
                    result = NULL;
                }
                /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_091: [ IoTHubRegistryManager_Create shall allocate memory and copy sharedAccessKey to result->sharedAccessKey by calling mallocAndStrcpy_s. ] */
                else if (mallocAndStrcpy_s(&result->sharedAccessKey, serviceClientAuth->sharedAccessKey) != 0)
                {
                    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_092: [ If the mallocAndStrcpy_s fails, IoTHubRegistryManager_Create shall do clean up and return NULL. ] */
                    LogError("mallocAndStrcpy_s failed for sharedAccessKey");
                    free(result->hostname);
                    free(result->iothubName);
                    free(result->iothubSuffix);
                    free(result);
                    result = NULL;
                }
                /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_093: [ IoTHubRegistryManager_Create shall allocate memory and copy keyName to result->keyName by calling mallocAndStrcpy_s. ] */
                else if (mallocAndStrcpy_s(&result->keyName, serviceClientAuth->keyName) != 0)
                {
                    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_094: [ If the mallocAndStrcpy_s fails, IoTHubRegistryManager_Create shall do clean up and return NULL. ] */
                    LogError("mallocAndStrcpy_s failed for keyName");
                    free(result->hostname);
                    free(result->iothubName);
                    free(result->iothubSuffix);
                    free(result->sharedAccessKey);
                    free(result);
                    result = NULL;
                }
            }
        }
    }
    return result;
}

void IoTHubRegistryManager_Destroy(IOTHUB_REGISTRYMANAGER_HANDLE registryManagerHandle)
{
    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_005: [ If the registryManagerHandle input parameter is NULL IoTHubRegistryManager_Destroy shall return ] */
    if (registryManagerHandle != NULL)
    {
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_006 : [ If the registryManagerHandle input parameter is not NULL IoTHubRegistryManager_Destroy shall free the memory of it and return ] */
        IOTHUB_REGISTRYMANAGER* regManHandle = (IOTHUB_REGISTRYMANAGER*)registryManagerHandle;

        free(regManHandle->hostname);
        free(regManHandle->iothubName);
        free(regManHandle->iothubSuffix);
        free(regManHandle->sharedAccessKey);
        free(regManHandle->keyName);
        free(regManHandle);
    }
}

IOTHUB_REGISTRYMANAGER_RESULT IoTHubRegistryManager_CreateDevice(IOTHUB_REGISTRYMANAGER_HANDLE registryManagerHandle, const IOTHUB_REGISTRY_DEVICE_CREATE* deviceCreateInfo, IOTHUB_DEVICE* deviceInfo)
{
    IOTHUB_REGISTRYMANAGER_RESULT result;

    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_007: [ IoTHubRegistryManager_CreateDevice shall verify the input parameters and if any of them are NULL then return IOTHUB_REGISTRYMANAGER_INVALID_ARG ] */
    if ((registryManagerHandle == NULL) || (deviceCreateInfo == NULL) || (deviceInfo == NULL))
    {
        LogError("Input parameter cannot be NULL");
        result = IOTHUB_REGISTRYMANAGER_INVALID_ARG;
    }
    else
    {
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_008: [ IoTHubRegistryManager_CreateDevice shall verify the deviceCreateInfo->deviceId input parameter and if it is NULL then return IOTHUB_REGISTRYMANAGER_INVALID_ARG ] */
        if (deviceCreateInfo->deviceId == NULL)
        {
            LogError("deviceId cannot be NULL");
            result = IOTHUB_REGISTRYMANAGER_INVALID_ARG;
        }
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_009: [ IoTHubRegistryManager_CreateDevice shall verify the deviceCreateInfo->deviceId input parameter and if it contains space(s) then return IOTHUB_REGISTRYMANAGER_INVALID_ARG ] */
        else if ((strHasNoWhitespace(deviceCreateInfo->deviceId)) != 0)
        {
            LogError("deviceId cannot contain spaces");
            result = IOTHUB_REGISTRYMANAGER_INVALID_ARG;
        }
        else
        {
            /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_095: [ IoTHubRegistryManager_CreateDevice shall allocate memory for device info structure by calling malloc ] */
            IOTHUB_DEVICE* tempDeviceInfo;
            if ((tempDeviceInfo = malloc(sizeof(IOTHUB_DEVICE))) == NULL)
            {
                /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_096 : [ If the malloc fails, IoTHubRegistryManager_Create shall do clean up and return NULL. ] */
                LogError("Malloc failed for tempDeviceInfo");
                result = IOTHUB_REGISTRYMANAGER_ERROR;
            }
            else
            {
                /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_012: [ IoTHubRegistryManager_CreateDevice shall set the "symmetricKey" value to deviceCreateInfo->primaryKey and deviceCreateInfo->secondaryKey ] */
                tempDeviceInfo->deviceId = deviceCreateInfo->deviceId;
                tempDeviceInfo->primaryKey = deviceCreateInfo->primaryKey;
                tempDeviceInfo->secondaryKey = deviceCreateInfo->secondaryKey;

                BUFFER_HANDLE deviceJsonBuffer = NULL;
                BUFFER_HANDLE responseBuffer = NULL;

                /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_010: [ IoTHubRegistryManager_CreateDevice shall create a flat "key1:value2,key2:value2..." JSON representation from the given deviceCreateInfo parameter using the following parson APIs: json_value_init_object, json_value_get_object, json_object_set_string, json_object_dotset_string ] */
                if ((deviceJsonBuffer = constructDeviceJson(tempDeviceInfo)) == NULL)
                {
                    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_013: [ IoTHubRegistryManager_CreateDevice shall return IOTHUB_REGISTRYMANAGER_ERROR_CREATING_JSON if the JSON creation failed  ] */
                    LogError("Json creation failed");
                    result = IOTHUB_REGISTRYMANAGER_JSON_ERROR;
                }
                /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_097: [ IoTHubRegistryManager_CreateDevice shall allocate memory for response buffer by calling BUFFER_new ] */
                else if ((responseBuffer = BUFFER_new()) == NULL)
                {
                    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_098 : [ If the BUFFER_new fails, IoTHubRegistryManager_CreateDevice shall do clean up and return NULL. ] */
                    LogError("BUFFER_new failed for responseBuffer");
                    result = IOTHUB_REGISTRYMANAGER_ERROR;
                }
                /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_014: [ IoTHubRegistryManager_CreateDevice shall create an HTTP PUT request using the created JSON ] */
                /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_015: [ IoTHubRegistryManager_CreateDevice shall create an HTTP PUT request using the following HTTP headers: authorization=sasToken,Request-Id=1001,Accept=application/json,Content-Type=application/json,charset=utf-8 ] */
                /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_016: [ IoTHubRegistryManager_CreateDevice shall create an HTTPAPIEX_SAS_HANDLE handle by calling HTTPAPIEX_SAS_Create ] */
                /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_017: [ IoTHubRegistryManager_CreateDevice shall create an HTTPAPIEX_HANDLE handle by calling HTTPAPIEX_Create ] */
                /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_018: [ IoTHubRegistryManager_CreateDevice shall execute the HTTP PUT request by calling HTTPAPIEX_ExecuteRequest ] */
                else if ((result = sendHttpRequestCRUD(registryManagerHandle, IOTHUB_REQUEST_CREATE, deviceCreateInfo->deviceId, deviceJsonBuffer, 0, responseBuffer)) == IOTHUB_REGISTRYMANAGER_ERROR)
                {
                    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_019: [ If any of the HTTPAPI call fails IoTHubRegistryManager_CreateDevice shall fail and return IOTHUB_REGISTRYMANAGER_HTTPAPI_ERROR ] */
                    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_099: [ If any of the call fails during the HTTP creation IoTHubRegistryManager_CreateDevice shall fail and return IOTHUB_REGISTRYMANAGER_ERROR ] */
                    LogError("Failure sending HTTP request for create device");
                }
                else if (result == IOTHUB_REGISTRYMANAGER_OK)
                {
                    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_022: [ IoTHubRegistryManager_CreateDevice shall verify the received HTTP status code and if it is less or equal than 300 then try to parse the response JSON to deviceInfo ] */
                    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_023: [ If the JSON parsing failed, IoTHubRegistryManager_CreateDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR ] */
                    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_024: [ If the deviceInfo out parameter is not NULL IoTHubRegistryManager_CreateDevice shall save the received deviceInfo to the out parameter and return IOTHUB_REGISTRYMANAGER_OK ] */
                    result = parseDeviceJson(responseBuffer, deviceInfo);
                }
                else
                {
                    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_020: [ IoTHubRegistryManager_CreateDevice shall verify the received HTTP status code and if it is 409 then return IOTHUB_REGISTRYMANAGER_DEVICE_EXIST ] */
                }

                /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_100: [ IoTHubRegistryManager_CreateDevice shall do clean up before return ] */
                if (responseBuffer != NULL)
                {
                    BUFFER_delete(responseBuffer);
                }
                if (deviceJsonBuffer != NULL)
                {
                    BUFFER_delete(deviceJsonBuffer);
                }
                free(tempDeviceInfo);
            }
        }
    }
    return result;
}

IOTHUB_REGISTRYMANAGER_RESULT IoTHubRegistryManager_GetDevice(IOTHUB_REGISTRYMANAGER_HANDLE registryManagerHandle, const char* deviceId, IOTHUB_DEVICE* deviceInfo)
{
    IOTHUB_REGISTRYMANAGER_RESULT result;

    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_025: [ IoTHubRegistryManager_GetDevice shall verify the registryManagerHandle and deviceId input parameters and if any of them are NULL then return IOTHUB_REGISTRYMANAGER_INVALID_ARG ] */
    if ((registryManagerHandle == NULL) || (deviceId == NULL))
    {
        LogError("Input parameter cannot be NULL");
        result = IOTHUB_REGISTRYMANAGER_INVALID_ARG;
    }
    else
    {
        BUFFER_HANDLE responseBuffer;

        if ((responseBuffer = BUFFER_new()) == NULL)
        {
            LogError("BUFFER_new failed for responseBuffer");
            result = IOTHUB_REGISTRYMANAGER_ERROR;
        }
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_026: [ IoTHubRegistryManager_GetDevice shall create HTTP GET request URL using the given deviceId using the following format: url/devices/[deviceId]?api-version=2016-11-14  ] */
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_027: [ IoTHubRegistryManager_GetDevice shall add the following headers to the created HTTP GET request: authorization=sasToken,Request-Id=1001,Accept=application/json,Content-Type=application/json,charset=utf-8 ] */
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_028: [ IoTHubRegistryManager_GetDevice shall create an HTTPAPIEX_SAS_HANDLE handle by calling HTTPAPIEX_SAS_Create ] */
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_029: [ IoTHubRegistryManager_GetDevice shall create an HTTPAPIEX_HANDLE handle by calling HTTPAPIEX_Create ] */
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_030: [ IoTHubRegistryManager_GetDevice shall execute the HTTP GET request by calling HTTPAPIEX_ExecuteRequest ] */
        else if ((result = sendHttpRequestCRUD(registryManagerHandle, IOTHUB_REQUEST_GET, deviceId, NULL, 0, responseBuffer)) == IOTHUB_REGISTRYMANAGER_ERROR)
        {
            /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_031: [ If any of the HTTPAPI call fails IoTHubRegistryManager_GetDevice shall fail and return IOTHUB_REGISTRYMANAGER_ERROR ] */
            LogError("Failure sending HTTP request for create device");
        }
        else if (result == IOTHUB_REGISTRYMANAGER_OK)
        {
            /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_033: [ IoTHubRegistryManager_GetDevice shall verify the received HTTP status code and if it is less or equal than 300 then try to parse the response JSON to deviceInfo for the following properties: deviceId, primaryKey, secondaryKey, generationId, eTag, connectionState, connectionstateUpdatedTime, status, statusReason, statusUpdatedTime, lastActivityTime, cloudToDeviceMessageCount ] */
            /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_034: [ If any of the property field above missing from the JSON the property value will not be populated ] */
            /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_035: [ If the JSON parsing failed, IoTHubRegistryManager_GetDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR ] */
            /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_036: [ If the received JSON is empty, IoTHubRegistryManager_GetDevice shall return IOTHUB_REGISTRYMANAGER_DEVICE_NOT_EXIST ] */
            /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_037: [ If the deviceInfo out parameter if not NULL IoTHubRegistryManager_GetDevice shall save the received deviceInfo to the out parameter and return IOTHUB_REGISTRYMANAGER_OK ] */
            if ((result = parseDeviceJson(responseBuffer, deviceInfo)) == IOTHUB_REGISTRYMANAGER_OK)
            {

                /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_036: [ If the received JSON is empty, IoTHubRegistryManager_GetDevice shall return IOTHUB_REGISTRYMANAGER_DEVICE_NOT_EXIST ] */
                if ((deviceInfo == NULL) || (deviceInfo->deviceId == NULL))
                {
                    result = IOTHUB_REGISTRYMANAGER_DEVICE_NOT_EXIST;
                }
                else
                {
                    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_037: [ If the deviceInfo out parameter if not NULL IoTHubRegistryManager_GetDevice shall save the received deviceInfo to the out parameter and return IOTHUB_REGISTRYMANAGER_OK ] */
                }
            }
        }

        BUFFER_delete(responseBuffer);
    }
    return result;
}

IOTHUB_REGISTRYMANAGER_RESULT IoTHubRegistryManager_UpdateDevice(IOTHUB_REGISTRYMANAGER_HANDLE registryManagerHandle, IOTHUB_REGISTRY_DEVICE_UPDATE* deviceUpdate)
{
    IOTHUB_REGISTRYMANAGER_RESULT result;

    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_038: [ IoTHubRegistryManager_UpdateDevice shall verify the input parameters and if any of them are NULL then return IOTHUB_REGISTRYMANAGER_INVALID_ARG ] */
    if ((registryManagerHandle == NULL) || (deviceUpdate == NULL))
    {
        LogError("Input parameter cannot be NULL");
        result = IOTHUB_REGISTRYMANAGER_INVALID_ARG;
    }
    else
    {
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_039: [ IoTHubRegistryManager_UpdateDevice shall verify the deviceCreateInfo->deviceId input parameter and if it is NULL then return IOTHUB_REGISTRYMANAGER_INVALID_ARG ] */
        if (deviceUpdate->deviceId == NULL)
        {
            LogError("deviceId cannot be NULL");
            result = IOTHUB_REGISTRYMANAGER_INVALID_ARG;
        }
        else
        {
            /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_106: [ IoTHubRegistryManager_UpdateDevice shall allocate memory for device info structure by calling malloc ] */
            IOTHUB_DEVICE* tempDeviceInfo;
            if ((tempDeviceInfo = malloc(sizeof(IOTHUB_DEVICE))) == NULL)
            {
                /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_108: [ If the malloc fails, IoTHubRegistryManager_UpdateDevice shall do clean up and return NULL ] */
                LogError("Malloc failed for tempDeviceInfo");
                result = IOTHUB_REGISTRYMANAGER_ERROR;
            }
            else
            {
                /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_107: [ IoTHubRegistryManager_UpdateDevice shall set the "symmetricKey" value to deviceCreateInfo->primaryKey and deviceCreateInfo->secondaryKey ] */
                (void)memset(tempDeviceInfo, 0, sizeof(IOTHUB_DEVICE));
                tempDeviceInfo->deviceId = deviceUpdate->deviceId;
                tempDeviceInfo->primaryKey = deviceUpdate->primaryKey;
                tempDeviceInfo->secondaryKey = deviceUpdate->secondaryKey;
                tempDeviceInfo->status = deviceUpdate->status;

                BUFFER_HANDLE deviceJsonBuffer = NULL;
                BUFFER_HANDLE responseBuffer = NULL;

                /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_041: [ IoTHubRegistryManager_UpdateDevice shall create a flat "key1:value2,key2:value2..." JSON representation from the given deviceCreateInfo parameter using the following parson APIs : json_value_init_object, json_value_get_object, json_object_set_string, json_object_dotset_string ] */
                if ((deviceJsonBuffer = constructDeviceJson(tempDeviceInfo)) == NULL)
                {
                    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_042: [ IoTHubRegistryManager_UpdateDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR if the JSON creation failed  ] */
                    LogError("Json creation failed");
                    result = IOTHUB_REGISTRYMANAGER_JSON_ERROR;
                }
                /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_101: [ IoTHubRegistryManager_UpdateDevice shall allocate memory for response buffer by calling BUFFER_new ] */
                else if ((responseBuffer = BUFFER_new()) == NULL)
                {
                    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_102: [ If the BUFFER_new fails, IoTHubRegistryManager_UpdateDevice shall do clean up and return NULL. ] */
                    LogError("BUFFER_new failed for responseBuffer");
                    result = IOTHUB_REGISTRYMANAGER_ERROR;
                }
                /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_043: [ IoTHubRegistryManager_UpdateDevice shall create an HTTP PUT request using the created JSON ] */
                /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_044: [ IoTHubRegistryManager_UpdateDevice shall create an HTTP PUT request using the createdfollowing HTTP headers : authorization = sasToken, Request - Id = 1001, Accept = application / json, Content - Type = application / json, charset = utf - 8 ] */
                /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_045: [ IoTHubRegistryManager_UpdateDevice shall create an HTTPAPIEX_SAS_HANDLE handle by calling HTTPAPIEX_SAS_Create ] */
                /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_046: [ IoTHubRegistryManager_UpdateDevice shall create an HTTPAPIEX_HANDLE handle by calling HTTPAPIEX_Create ] */
                /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_047: [ IoTHubRegistryManager_UpdateDevice shall execute the HTTP PUT request by calling HTTPAPIEX_ExecuteRequest ] */
                else if ((result = sendHttpRequestCRUD(registryManagerHandle, IOTHUB_REQUEST_UPDATE, deviceUpdate->deviceId, deviceJsonBuffer, 0, responseBuffer)) == IOTHUB_REGISTRYMANAGER_ERROR)
                {
                    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_103: [ If any of the call fails during the HTTP creation IoTHubRegistryManager_UpdateDevice shall fail and return IOTHUB_REGISTRYMANAGER_ERROR ] */
                    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_104: [ If any of the HTTPAPI call fails IoTHubRegistryManager_UpdateDevice shall fail and return IOTHUB_REGISTRYMANAGER_HTTPAPI_ERROR ] */
                    LogError("Failure sending HTTP request for update device");
                }

                /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_105: [ IoTHubRegistryManager_UpdateDevice shall do clean up before return ] */
                if (deviceJsonBuffer != NULL)
                {
                    BUFFER_delete(deviceJsonBuffer);
                }
                if (responseBuffer != NULL)
                {
                    BUFFER_delete(responseBuffer);
                }
                free(tempDeviceInfo);
            }
        }
    }
    return result;
}

IOTHUB_REGISTRYMANAGER_RESULT IoTHubRegistryManager_DeleteDevice(IOTHUB_REGISTRYMANAGER_HANDLE registryManagerHandle, const char* deviceId)
{
    IOTHUB_REGISTRYMANAGER_RESULT result;

    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_052: [ IoTHubRegistryManager_DeleteDevice shall verify the input parameters and if any of them are NULL then return IOTHUB_REGISTRYMANAGER_INVALID_ARG ] */
    if ((registryManagerHandle == NULL) || (deviceId == NULL))
    {
        LogError("Input parameter cannot be NULL");
        result = IOTHUB_REGISTRYMANAGER_INVALID_ARG;
    }
    else
    {
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_053: [ IoTHubRegistryManager_DeleteDevice shall create HTTP DELETE request URL using the given deviceId using the following format : url / devices / [deviceId] ? api - version  ] */
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_054: [ IoTHubRegistryManager_DeleteDevice shall add the following headers to the created HTTP GET request : authorization = sasToken, Request - Id = 1001, Accept = application / json, Content - Type = application / json, charset = utf - 8 ] */
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_055: [ IoTHubRegistryManager_DeleteDevice shall create an HTTPAPIEX_SAS_HANDLE handle by calling HTTPAPIEX_SAS_Create ] */
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_056: [ IoTHubRegistryManager_DeleteDevice shall create an HTTPAPIEX_HANDLE handle by calling HTTPAPIEX_Create ] */
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_057: [ IoTHubRegistryManager_DeleteDevice shall execute the HTTP DELETE request by calling HTTPAPIEX_ExecuteRequest ] */
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_058: [ IoTHubRegistryManager_DeleteDevice shall verify the received HTTP status code and if it is greater than 300 then return IOTHUB_REGISTRYMANAGER_HTTP_STATUS_ERROR ] */
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_059: [ IoTHubRegistryManager_DeleteDevice shall verify the received HTTP status code and if it is less or equal than 300 then return IOTHUB_REGISTRYMANAGER_OK ] */
        result = sendHttpRequestCRUD(registryManagerHandle, IOTHUB_REQUEST_DELETE, deviceId, NULL, 0, NULL);
    }
    return result;
}

IOTHUB_REGISTRYMANAGER_RESULT IoTHubRegistryManager_GetDeviceList(IOTHUB_REGISTRYMANAGER_HANDLE registryManagerHandle, size_t numberOfDevices, SINGLYLINKEDLIST_HANDLE deviceList)
{
    IOTHUB_REGISTRYMANAGER_RESULT result;

    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_060: [ IoTHubRegistryManager_GetDeviceList shall verify the input parameters and if any of them are NULL then return IOTHUB_REGISTRYMANAGER_INVALID_ARG ] */
    if ((registryManagerHandle == NULL) || (deviceList == NULL))
    {
        LogError("Input parameter cannot be NULL");
        result = IOTHUB_REGISTRYMANAGER_INVALID_ARG;
    }
    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_061: [ IoTHubRegistryManager_GetDeviceList shall verify if the numberOfDevices input parameter is between 1 and 1000 and if it is not then return IOTHUB_REGISTRYMANAGER_INVALID_ARG ] */
    else if ((numberOfDevices == 0) || (numberOfDevices > IOTHUB_DEVICES_MAX_REQUEST))
    {
        LogError("numberOfDevices has to be between 1 and 1000");
        result = IOTHUB_REGISTRYMANAGER_INVALID_ARG;
    }
    else
    {
        BUFFER_HANDLE responseBuffer = NULL;

        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_109: [ IoTHubRegistryManager_GetDeviceList shall allocate memory for response buffer by calling BUFFER_new ] */
        if ((responseBuffer = BUFFER_new()) == NULL)
        {
            /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_110: [ If the BUFFER_new fails, IoTHubRegistryManager_GetDeviceList shall do clean up and return NULL ] */
            LogError("BUFFER_new failed for responseBuffer");
            result = IOTHUB_REGISTRYMANAGER_ERROR;
        }
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_062: [ IoTHubRegistryManager_GetDeviceList shall create HTTP GET request for numberOfDevices using the follwoing format: url/devices/?top=[numberOfDevices]&api-version ] */
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_063: [ IoTHubRegistryManager_GetDeviceList shall add the following headers to the created HTTP GET request: authorization=sasToken,Request-Id=1001,Accept=application/json,Content-Type=application/json,charset=utf-8 ] */
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_064: [ IoTHubRegistryManager_GetDeviceList shall create an HTTPAPIEX_SAS_HANDLE handle by calling HTTPAPIEX_SAS_Create ] */
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_065: [ IoTHubRegistryManager_GetDeviceList shall create an HTTPAPIEX_HANDLE handle by calling HTTPAPIEX_Create ] */
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_066: [ IoTHubRegistryManager_GetDeviceList shall execute the HTTP GET request by calling HTTPAPIEX_ExecuteRequest ] */
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_067: [ IoTHubRegistryManager_GetDeviceList shall verify the received HTTP status code and if it is greater than 300 then return IOTHUB_REGISTRYMANAGER_ERROR ] */
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_068: [ IoTHubRegistryManager_GetDeviceList shall verify the received HTTP status code and if it is less or equal than 300 then try to parse the response JSON to deviceList ] */
        else if ((result = sendHttpRequestCRUD(registryManagerHandle, IOTHUB_REQUEST_GET_DEVICE_LIST, NULL, NULL, numberOfDevices, responseBuffer)) == IOTHUB_REGISTRYMANAGER_ERROR)
        {
            /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_115: [ If any of the HTTPAPI call fails IoTHubRegistryManager_GetDeviceList shall fail and return IOTHUB_REGISTRYMANAGER_ERROR ] */
            LogError("Failure sending HTTP request for get device list");
        }
        else if (result == IOTHUB_REGISTRYMANAGER_OK)
        {
            /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_069: [ IoTHubRegistryManager_GetDeviceList shall use the following parson APIs to parse the response JSON: json_parse_string, json_value_get_object, json_object_get_string, json_object_dotget_string  ] */
            /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_070: [ If any of the parson API fails, IoTHubRegistryManager_GetDeviceList shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR ] */
            /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_071: [ IoTHubRegistryManager_GetDeviceList shall populate the deviceList parameter with structures of type "IOTHUB_DEVICE" ] */
            /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_072: [ If populating the deviceList parameter fails IoTHubRegistryManager_GetDeviceList shall return IOTHUB_REGISTRYMANAGER_ERROR ] */
            /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_073: [ If populating the deviceList parameter successful IoTHubRegistryManager_GetDeviceList shall return IOTHUB_REGISTRYMANAGER_OK ] */
            result = parseDeviceListJson(responseBuffer, deviceList);
        }

        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_111: [ IoTHubRegistryManager_GetDeviceList shall do clean up before return ] */
        if (responseBuffer != NULL)
        {
            BUFFER_delete(responseBuffer);
        }
    }
    return result;
}

IOTHUB_REGISTRYMANAGER_RESULT IoTHubRegistryManager_GetStatistics(IOTHUB_REGISTRYMANAGER_HANDLE registryManagerHandle, IOTHUB_REGISTRY_STATISTICS* registryStatistics)
{
    IOTHUB_REGISTRYMANAGER_RESULT result;

    /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_074: [ IoTHubRegistryManager_GetStatistics shall verify the input parameters and if any of them are NULL then return IOTHUB_REGISTRYMANAGER_INVALID_ARG ] */
    if ((registryManagerHandle == NULL) || (registryStatistics == NULL))
    {
        LogError("Input parameter cannot be NULL");
        result = IOTHUB_REGISTRYMANAGER_INVALID_ARG;
    }
    else
    {
        BUFFER_HANDLE responseBuffer;

        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_112: [ IoTHubRegistryManager_GetStatistics shall allocate memory for response buffer by calling BUFFER_new ] */
        if ((responseBuffer = BUFFER_new()) == NULL)
        {
            /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_113: [ If the BUFFER_new fails, IoTHubRegistryManager_GetStatistics shall do clean up and return NULL ] */
            LogError("BUFFER_new failed for responseBuffer");
            result = IOTHUB_REGISTRYMANAGER_ERROR;
        }
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_075: [ IoTHubRegistryManager_GetStatistics shall create HTTP GET request for statistics using the following format: url/statistics/devices?api-version ] */
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_076: [ IoTHubRegistryManager_GetStatistics shall add the following headers to the created HTTP GET request: authorization=sasToken,Request-Id=1001,Accept=application/json,Content-Type=application/json,charset=utf-8 ] */
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_077: [ IoTHubRegistryManager_GetStatistics shall create an HTTPAPIEX_SAS_HANDLE handle by calling HTTPAPIEX_SAS_Create ] */
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_078: [ IoTHubRegistryManager_GetStatistics shall create an HTTPAPIEX_HANDLE handle by calling HTTPAPIEX_Create ] */
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_079: [ IoTHubRegistryManager_GetStatistics shall execute the HTTP GET request by calling HTTPAPIEX_ExecuteRequest ] */
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_080: [ IoTHubRegistryManager_GetStatistics shall verify the received HTTP status code and if it is greater than 300 then return IOTHUB_REGISTRYMANAGER_ERROR ] */
        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_081: [ IoTHubRegistryManager_GetStatistics shall verify the received HTTP status code and if it is less or equal than 300 then use the following parson APIs to parse the response JSON to registry statistics structure: json_parse_string, json_value_get_object, json_object_get_string, json_object_dotget_string ] */
        else if ((result = sendHttpRequestCRUD(registryManagerHandle, IOTHUB_REQUEST_GET_STATISTICS, NULL, NULL, 0, responseBuffer)) == IOTHUB_REGISTRYMANAGER_ERROR)
        {
            /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_116: [ If any of the HTTPAPI call fails IoTHubRegistryManager_GetStatistics shall fail and return IOTHUB_REGISTRYMANAGER_ERROR ] */
            LogError("Failure sending HTTP request for get registry statistics");
        }
        else if (result == IOTHUB_REGISTRYMANAGER_OK)
        {
            /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_082: [ If the parsing failed, IoTHubRegistryManager_GetStatistics shall return IOTHUB_REGISTRYMANAGER_ERROR ] */
            /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_083: [ IoTHubRegistryManager_GetStatistics shall save the registry statistics to the out value and return IOTHUB_REGISTRYMANAGER_OK ] */
            result = parseStatisticsJson(responseBuffer, registryStatistics);
        }

        /*Codes_SRS_IOTHUBREGISTRYMANAGER_12_114: [ IoTHubRegistryManager_GetStatistics shall do clean up before return ] */
        if (responseBuffer != NULL)
        {
            BUFFER_delete(responseBuffer);
        }
    }
    return result;
}
