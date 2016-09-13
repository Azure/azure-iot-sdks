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
#include "azure_c_shared_utility/list.h"
#include "azure_c_shared_utility/buffer_.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/httpapiex.h"
#include "azure_c_shared_utility/httpapiexsas.h"

#include "parson.h"
#include "connection_string_parser.h"
#include "iothub_devicetwin.h"

#define IOTHUB_TWIN_REQUEST_MODE_VALUES    \
    IOTHUB_TWIN_REQUEST_GET,               \
    IOTHUB_TWIN_REQUEST_UPDATE,            \
    IOTHUB_TWIN_REQUEST_REPLACE_TAGS,      \
    IOTHUB_TWIN_REQUEST_REPLACE_DESIRED,   \
    IOTHUB_TWIN_REQUEST_UPDATE_DESIRED

DEFINE_ENUM(IOTHUB_TWIN_REQUEST_MODE, IOTHUB_TWIN_REQUEST_MODE_VALUES);

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

static const char* RELATIVE_PATH_FMT_TWIN = "/twin/%s";
static const char* RELATIVE_PATH_FMT_TWIN_TAGS = "/twin/%s/tags";
static const char* RELATIVE_PATH_FMT_TWIN_DESIRED = "/twin/%s/properties/desired";

/** @brief Structure to store IoTHub authentication information
*/
typedef struct IOTHUB_SERVICE_CLIENT_DEVICE_TWIN_TAG
{
    char* hostname;
    char* iothubName;
    char* iothubSuffix;
    char* sharedAccessKey;
    char* keyName;
} IOTHUB_SERVICE_CLIENT_DEVICE_TWIN;

static IOTHUB_DEVICE_TWIN_RESULT parseDeviceJson(BUFFER_HANDLE jsonBuffer, IOTHUB_DEVICE_TWIN* deviceInfo)
{
    (void*)jsonBuffer;
    (void*)deviceInfo;
    return IOTHUB_DEVICE_TWIN_ERROR;
}

static IOTHUB_DEVICE_TWIN_RESULT createRelativePath(IOTHUB_TWIN_REQUEST_MODE iotHubTwinRequestMode, const char* deviceId, char* relativePath)
{
    //IOTHUB_TWIN_REQUEST_GET               GET      {iot hub}/twins/{device id}                     // Get device twin  
    //IOTHUB_TWIN_REQUEST_UPDATE            PATCH    {iot hub}/twins/{device id}                     // Partally update device twin
    //IOTHUB_TWIN_REQUEST_REPLACE_TAGS      PUT      {iot hub}/twins/{device id}/tags                // Replace update tags
    //IOTHUB_TWIN_REQUEST_REPLACE_DESIRED   PUT      {iot hub}/twins/{device id}/properties/desired  // Replace update desired properties
    //IOTHUB_TWIN_REQUEST_UPDATE_DESIRED    PATCH    {iot hub}/twins/{device id}/properties/desired  // Partially update desired properties

    IOTHUB_DEVICE_TWIN_RESULT result;

    if ((iotHubTwinRequestMode == IOTHUB_TWIN_REQUEST_GET) || (iotHubTwinRequestMode == IOTHUB_TWIN_REQUEST_UPDATE))
    {
        if (snprintf(relativePath, 256, RELATIVE_PATH_FMT_TWIN, deviceId) > 0)
        {
            result = IOTHUB_DEVICE_TWIN_OK;
        }
        else
        {
            result = IOTHUB_DEVICE_TWIN_ERROR;
        }
    }
    else if (iotHubTwinRequestMode == IOTHUB_TWIN_REQUEST_REPLACE_TAGS)
    {
        if (snprintf(relativePath, 256, RELATIVE_PATH_FMT_TWIN_TAGS, deviceId) > 0)
        {
            result = IOTHUB_DEVICE_TWIN_OK;
        }
        else
        {
            result = IOTHUB_DEVICE_TWIN_ERROR;
        }
    }
    else if ((iotHubTwinRequestMode == IOTHUB_TWIN_REQUEST_REPLACE_DESIRED) || (iotHubTwinRequestMode == IOTHUB_TWIN_REQUEST_UPDATE_DESIRED))
    {
        if (snprintf(relativePath, 256, RELATIVE_PATH_FMT_TWIN_DESIRED, deviceId) > 0)
        {
            result = IOTHUB_DEVICE_TWIN_OK;
        }
        else
        {
            result = IOTHUB_DEVICE_TWIN_ERROR;
        }
    }
    else
    {
        result = IOTHUB_DEVICE_TWIN_ERROR;
    }

    return result;
}

static HTTP_HEADERS_HANDLE createHttpHeader(IOTHUB_TWIN_REQUEST_MODE iotHubTwinRequestMode)
{
    /*Codes_SRS_IOTHUBDEVICETWIN_12_020: [ IoTHubDeviceTwin_GetTwin shall add the following headers to the created HTTP GET request: authorization=sasToken,Request-Id=1001,Accept=application/json,Content-Type=application/json,charset=utf-8 ]*/
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

    if (iotHubTwinRequestMode != IOTHUB_TWIN_REQUEST_GET)
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

static IOTHUB_DEVICE_TWIN_RESULT sendHttpRequestTwin(IOTHUB_SERVICE_CLIENT_DEVICE_TWIN_HANDLE serviceClientDeviceTwinHandle, IOTHUB_TWIN_REQUEST_MODE iotHubTwinRequestMode, const char* deviceName, BUFFER_HANDLE deviceJsonBuffer, BUFFER_HANDLE responseBuffer)
{
    IOTHUB_DEVICE_TWIN_RESULT result;

    STRING_HANDLE uriResouce = NULL;
    STRING_HANDLE accessKey = NULL;
    STRING_HANDLE keyName = NULL;
    HTTPAPIEX_SAS_HANDLE httpExApiSasHandle = NULL;
    HTTPAPIEX_HANDLE httpExApiHandle = NULL;
    HTTP_HEADERS_HANDLE httpHeader = NULL;

    if ((uriResouce = STRING_construct(serviceClientDeviceTwinHandle->hostname)) == NULL)
    {
        /*Codes_SRS_IOTHUBDEVICETWIN_12_024: [ If any of the HTTPAPI call fails IoTHubDeviceTwin_GetTwin shall fail and return IOTHUB_DEVICE_TWIN_ERROR ]*/
        LogError("STRING_construct failed for uriResource");
        result = IOTHUB_DEVICE_TWIN_ERROR;
    }
    else if ((accessKey = STRING_construct(serviceClientDeviceTwinHandle->sharedAccessKey)) == NULL)
    {
        /*Codes_SRS_IOTHUBDEVICETWIN_12_024: [ If any of the call fails during the HTTP creation IoTHubDeviceTwin_GetTwin shall fail and return IOTHUB_DEVICE_TWIN_ERROR ]*/
        LogError("STRING_construct failed for accessKey");
        result = IOTHUB_DEVICE_TWIN_ERROR;
    }
    else if ((keyName = STRING_construct(serviceClientDeviceTwinHandle->keyName)) == NULL)
    {
        /*Codes_SRS_IOTHUBDEVICETWIN_12_024: [ If any of the call fails during the HTTP creation IoTHubDeviceTwin_GetTwin shall fail and return IOTHUB_DEVICE_TWIN_ERROR ]*/
        LogError("STRING_construct failed for keyName");
        result = IOTHUB_DEVICE_TWIN_ERROR;
    }
    /*Codes_SRS_IOTHUBDEVICETWIN_12_020: [ IoTHubDeviceTwin_GetTwin shall add the following headers to the created HTTP GET request: authorization=sasToken,Request-Id=1001,Accept=application/json,Content-Type=application/json,charset=utf-8 ]*/
    else if ((httpHeader = createHttpHeader(iotHubTwinRequestMode)) == NULL)
    {
        /*Codes_SRS_IOTHUBDEVICETWIN_12_024: [ If any of the call fails during the HTTP creation IoTHubDeviceTwin_GetTwin shall fail and return IOTHUB_DEVICE_TWIN_ERROR ]*/
        LogError("HttpHeader creation failed");
        result = IOTHUB_DEVICE_TWIN_ERROR;
    }
    /*Codes_SRS_IOTHUBDEVICETWIN_12_021: [ IoTHubDeviceTwin_GetTwin shall create an HTTPAPIEX_SAS_HANDLE handle by calling HTTPAPIEX_SAS_Create ]*/
    else if ((httpExApiSasHandle = HTTPAPIEX_SAS_Create(accessKey, uriResouce, keyName)) == NULL)
    {
        /*Codes_SRS_IOTHUBDEVICETWIN_12_025: [ If any of the HTTPAPI call fails IoTHubDeviceTwin_GetTwin shall fail and return IOTHUB_DEVICE_TWIN_HTTPAPI_ERROR ]*/
        LogError("HTTPAPIEX_SAS_Create failed");
        result = IOTHUB_DEVICE_TWIN_HTTPAPI_ERROR;
    }
    /*Codes_SRS_IOTHUBDEVICETWIN_12_022: [ IoTHubDeviceTwin_GetTwin shall create an HTTPAPIEX_HANDLE handle by calling HTTPAPIEX_Create ]*/
    else if ((httpExApiHandle = HTTPAPIEX_Create(serviceClientDeviceTwinHandle->hostname)) == NULL)
    {
        /*Codes_SRS_IOTHUBDEVICETWIN_12_025: [ If any of the HTTPAPI call fails IoTHubDeviceTwin_GetTwin shall fail and return IOTHUB_DEVICE_TWIN_HTTPAPI_ERROR ]*/
        LogError("HTTPAPIEX_Create failed");
        result = IOTHUB_DEVICE_TWIN_HTTPAPI_ERROR;
    }
    else 
    {
        HTTPAPI_REQUEST_TYPE httpApiRequestType = HTTPAPI_REQUEST_GET;
        char relativePath[256];
        unsigned int statusCode;
        unsigned char is_error = 0;

        //IOTHUB_TWIN_REQUEST_GET               GET      {iot hub}/twins/{device id}                     // Get device twin  
        //IOTHUB_TWIN_REQUEST_UPDATE            PATCH    {iot hub}/twins/{device id}                     // Partally update device twin
        //IOTHUB_TWIN_REQUEST_REPLACE_TAGS      PUT      {iot hub}/twins/{device id}/tags                // Replace update tags
        //IOTHUB_TWIN_REQUEST_REPLACE_DESIRED   PUT      {iot hub}/twins/{device id}/properties/desired  // Replace update desired properties
        //IOTHUB_TWIN_REQUEST_UPDATE_DESIRED    PATCH    {iot hub}/twins/{device id}/properties/desired  // Partially update desired properties

        if ((iotHubTwinRequestMode == IOTHUB_TWIN_REQUEST_REPLACE_TAGS) || (iotHubTwinRequestMode == IOTHUB_TWIN_REQUEST_REPLACE_DESIRED))
        {
            httpApiRequestType = HTTPAPI_REQUEST_PUT;
        }
        else if ((iotHubTwinRequestMode == IOTHUB_TWIN_REQUEST_UPDATE) || (iotHubTwinRequestMode == IOTHUB_TWIN_REQUEST_UPDATE_DESIRED))
        {
            httpApiRequestType = HTTPAPI_REQUEST_PATCH;
        }
        else if ((iotHubTwinRequestMode == IOTHUB_TWIN_REQUEST_GET))
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
            result = IOTHUB_DEVICE_TWIN_HTTPAPI_ERROR;
        }
        else
        {
            /*Codes_SRS_IOTHUBDEVICETWIN_12_019: [ IoTHubDeviceTwin_GetTwin shall create HTTP GET request URL using the given deviceId using the following format: url/twins/[deviceId] ]*/
            if (createRelativePath(iotHubTwinRequestMode, deviceName, relativePath) != IOTHUB_DEVICE_TWIN_OK)
            {
                /*Codes_SRS_IOTHUBDEVICETWIN_12_024: [ If any of the call fails during the HTTP creation IoTHubDeviceTwin_GetTwin shall fail and return IOTHUB_DEVICE_TWIN_ERROR ]*/
                LogError("Failure creating relative path");
                result = IOTHUB_DEVICE_TWIN_ERROR;
            }
            /*Codes_SRS_IOTHUBDEVICETWIN_12_023: [ IoTHubDeviceTwin_GetTwin shall execute the HTTP GET request by calling HTTPAPIEX_ExecuteRequest ]*/
            else if (HTTPAPIEX_SAS_ExecuteRequest(httpExApiSasHandle, httpExApiHandle, httpApiRequestType, relativePath, httpHeader, deviceJsonBuffer, &statusCode, NULL, responseBuffer) != HTTPAPIEX_OK)
            {
                /*Codes_SRS_IOTHUBDEVICETWIN_12_025: [ If any of the HTTPAPI call fails IoTHubDeviceTwin_GetTwin shall fail and return IOTHUB_DEVICE_TWIN_HTTPAPI_ERROR ]*/
                LogError("HTTPAPIEX_SAS_ExecuteRequest failed");
                result = IOTHUB_DEVICE_TWIN_HTTPAPI_ERROR;
            }
            else
            {
                if (statusCode == 200)
                {
                    /*Codes_SRS_IOTHUBDEVICETWIN_12_027: [ IoTHubDeviceTwin_GetTwin shall verify the received HTTP status code and if it is equal to 200 then try to parse the response JSON to deviceTwin ]*/
                    result = IOTHUB_DEVICE_TWIN_OK;
                }
                else
                {
                    /*Codes_SRS_IOTHUBDEVICETWIN_12_026: [ IoTHubDeviceTwin_GetTwin shall verify the received HTTP status code and if it is not equal to 200 then return IOTHUB_DEVICE_TWIN_ERROR ]*/
                    LogError("Http Failure status code %d.", statusCode);
                    result = IOTHUB_DEVICE_TWIN_ERROR;
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

static IOTHUB_DEVICE_TWIN* createTemporaryDeviceTwin(const IOTHUB_DEVICE_TWIN* deviceTwin)
{
    (void*)deviceTwin;
    return NULL;
}

static IOTHUB_DEVICE_TWIN_TAGS* createTemporaryDeviceTwinTags(const IOTHUB_DEVICE_TWIN_TAGS* deviceTwinTags)
{
    (void*)deviceTwinTags;
    return NULL;
}

static IOTHUB_DEVICE_TWIN_PROPERTIES* createTemporaryDeviceTwinProperties(const IOTHUB_DEVICE_TWIN_PROPERTIES* deviceTwinProperties)
{
    (void*)deviceTwinProperties;
    return NULL;
}

static BUFFER_HANDLE constructDeviceTwinJson(IOTHUB_DEVICE_TWIN* deviceTwin)
{
    (void*)deviceTwin;
    return NULL;
}

static BUFFER_HANDLE constructDeviceTwinTagsJson(IOTHUB_DEVICE_TWIN_TAGS* deviceTwinTags)
{
    (void*)deviceTwinTags;
    return NULL;
}

static BUFFER_HANDLE constructDeviceTwinPropertiesJson(IOTHUB_DEVICE_TWIN_PROPERTIES* deviceTwinProperties)
{
    (void*)deviceTwinProperties;
    return NULL;
}

IOTHUB_SERVICE_CLIENT_DEVICE_TWIN_HANDLE IoTHubDeviceTwin_Create(IOTHUB_SERVICE_CLIENT_AUTH_HANDLE serviceClientHandle)
{
    IOTHUB_SERVICE_CLIENT_DEVICE_TWIN_HANDLE result;

    /*Codes_SRS_IOTHUBDEVICETWIN_12_001: [ If the serviceClientHandle input parameter is NULL IoTHubDeviceTwin_Create shall return NULL ]*/
    if (serviceClientHandle == NULL)
    {
        LogError("serviceClientHandle input parameter cannot be NULL");
        result = NULL;
    }
    else
    {
        /*Codes_SRS_IOTHUBDEVICETWIN_12_002: [ If any member of the serviceClientHandle input parameter is NULL IoTHubDeviceTwin_Create shall return NULL ]*/
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
            /*Codes_SRS_IOTHUBDEVICETWIN_12_003: [ IoTHubDeviceTwin_Create shall allocate memory for a new IOTHUB_SERVICE_CLIENT_DEVICE_TWIN_MANAGER_HANDLE instance ]*/
            result = malloc(sizeof(IOTHUB_SERVICE_CLIENT_DEVICE_TWIN));
            if (result == NULL)
            {
                /*Codes_SRS_IOTHUBDEVICETWIN_12_004: [ If the allocation failed, IoTHubDeviceTwin_Create shall return NULL ]*/
                LogError("Malloc failed for IOTHUB_SERVICE_CLIENT_DEVICE_TWIN");
            }
            else
            {
                /*Codes_SRS_IOTHUBDEVICETWIN_12_005: [ If the allocation successful, IoTHubDeviceTwin_Create shall create a IOTHUB_SERVICE_CLIENT_DEVICE_TWIN_MANAGER_HANDLE from the given IOTHUB_SERVICE_CLIENT_AUTH_HANDLE and return with it ]*/
                /*Codes_SRS_IOTHUBDEVICETWIN_12_006: [ IoTHubDeviceTwin_Create shall allocate memory and copy hostName to result->hostName by calling mallocAndStrcpy_s. ]*/
                if (mallocAndStrcpy_s(&result->hostname, serviceClientAuth->hostname) != 0)
                {
                    /*Codes_SRS_IOTHUBDEVICETWIN_12_007: [ If the mallocAndStrcpy_s fails, IoTHubDeviceTwin_Create shall do clean up and return NULL. ]*/
                    LogError("mallocAndStrcpy_s failed for hostName");
                    free(result);
                    result = NULL;
                }
                /*Codes_SRS_IOTHUBDEVICETWIN_12_008: [ IoTHubDeviceTwin_Create shall allocate memory and copy iothubName to result->iothubName by calling mallocAndStrcpy_s. ]*/
                else if (mallocAndStrcpy_s(&result->iothubName, serviceClientAuth->iothubName) != 0)
                {
                    /*Codes_SRS_IOTHUBDEVICETWIN_12_009: [ If the mallocAndStrcpy_s fails, IoTHubDeviceTwin_Create shall do clean up and return NULL. ]*/
                    LogError("mallocAndStrcpy_s failed for iothubName");
                    free(result->hostname);
                    free(result);
                    result = NULL;
                }
                /*Codes_SRS_IOTHUBDEVICETWIN_12_010: [ IoTHubDeviceTwin_Create shall allocate memory and copy iothubSuffix to result->iothubSuffix by calling mallocAndStrcpy_s. ]*/
                else if (mallocAndStrcpy_s(&result->iothubSuffix, serviceClientAuth->iothubSuffix) != 0)
                {
                    /*Codes_SRS_IOTHUBDEVICETWIN_12_011: [ If the mallocAndStrcpy_s fails, IoTHubDeviceTwin_Create shall do clean up and return `NULL`. ]*/
                    LogError("mallocAndStrcpy_s failed for iothubSuffix");
                    free(result->hostname);
                    free(result->iothubName);
                    free(result);
                    result = NULL;
                }
                /*Codes_SRS_IOTHUBDEVICETWIN_12_012: [ IoTHubDeviceTwin_Create shall allocate memory and copy sharedAccessKey to result->sharedAccessKey by calling mallocAndStrcpy_s. ]*/
                else if (mallocAndStrcpy_s(&result->sharedAccessKey, serviceClientAuth->sharedAccessKey) != 0)
                {
                    /*Codes_SRS_IOTHUBDEVICETWIN_12_013: [ If the mallocAndStrcpy_s fails, IoTHubDeviceTwin_Create shall do clean up and return NULL. ]*/
                    LogError("mallocAndStrcpy_s failed for sharedAccessKey");
                    free(result->hostname);
                    free(result->iothubName);
                    free(result->iothubSuffix);
                    free(result);
                    result = NULL;
                }
                /*Codes_SRS_IOTHUBDEVICETWIN_12_014: [ IoTHubDeviceTwin_Create shall allocate memory and copy keyName to result->keyName by calling mallocAndStrcpy_s. ]*/
                else if (mallocAndStrcpy_s(&result->keyName, serviceClientAuth->keyName) != 0)
                {
                    /*Codes_SRS_IOTHUBDEVICETWIN_12_015: [ If the mallocAndStrcpy_s fails, IoTHubDeviceTwin_Create shall do clean up and return NULL. ]*/
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

void IoTHubDeviceTwin_Destroy(IOTHUB_SERVICE_CLIENT_DEVICE_TWIN_HANDLE serviceClientDeviceTwinHandle)
{
    /*Codes_SRS_IOTHUBDEVICETWIN_12_016: [ If the serviceClientDeviceTwinHandle input parameter is NULL IoTHubDeviceTwin_Destroy shall return ]*/
    if (serviceClientDeviceTwinHandle != NULL)
    {
        /*Codes_SRS_IOTHUBDEVICETWIN_12_017: [ If the serviceClientDeviceTwinHandle input parameter is not NULL IoTHubDeviceTwin_Destroy shall free the memory of it and return ]*/
        IOTHUB_SERVICE_CLIENT_DEVICE_TWIN* serviceClientDeviceTwin = (IOTHUB_SERVICE_CLIENT_DEVICE_TWIN*)serviceClientDeviceTwinHandle;

        free(serviceClientDeviceTwin->hostname);
        free(serviceClientDeviceTwin->iothubName);
        free(serviceClientDeviceTwin->iothubSuffix);
        free(serviceClientDeviceTwin->sharedAccessKey);
        free(serviceClientDeviceTwin->keyName);
        free(serviceClientDeviceTwin);
    }
}

IOTHUB_DEVICE_TWIN_RESULT IoTHubDeviceTwin_GetTwin(IOTHUB_SERVICE_CLIENT_DEVICE_TWIN_HANDLE serviceClientDeviceTwinHandle, const char* deviceId, IOTHUB_DEVICE_TWIN* deviceTwin)
{
    IOTHUB_DEVICE_TWIN_RESULT result;

    /*Codes_SRS_IOTHUBDEVICETWIN_12_018: [ IoTHubDeviceTwin_GetTwin shall verify the input parameters and if any of them are NULL then return IOTHUB_DEVICE_TWIN_INVALID_ARG ]*/
    if ((serviceClientDeviceTwinHandle == NULL) || (deviceId == NULL) || (deviceTwin == NULL))
    {
        LogError("Input parameter cannot be NULL");
        result = IOTHUB_DEVICE_TWIN_INVALID_ARG;
    }
    else
    {
        BUFFER_HANDLE responseBuffer;

        if ((responseBuffer = BUFFER_new()) == NULL)
        {
            LogError("BUFFER_new failed for responseBuffer");
            result = IOTHUB_DEVICE_TWIN_ERROR;
        }
        /*Codes_SRS_IOTHUBDEVICETWIN_12_019: [ IoTHubDeviceTwin_GetTwin shall create HTTP GET request URL using the given deviceId using the following format: url/twins/[deviceId] ]*/
        /*Codes_SRS_IOTHUBDEVICETWIN_12_020: [ IoTHubDeviceTwin_GetTwin shall add the following headers to the created HTTP GET request: authorization=sasToken,Request-Id=1001,Accept=application/json,Content-Type=application/json,charset=utf-8 ]*/
        /*Codes_SRS_IOTHUBDEVICETWIN_12_021: [ IoTHubDeviceTwin_GetTwin shall create an HTTPAPIEX_SAS_HANDLE handle by calling HTTPAPIEX_SAS_Create ]*/
        /*Codes_SRS_IOTHUBDEVICETWIN_12_022: [ IoTHubDeviceTwin_GetTwin shall create an HTTPAPIEX_HANDLE handle by calling HTTPAPIEX_Create ]*/
        /*Codes_SRS_IOTHUBDEVICETWIN_12_023: [ IoTHubDeviceTwin_GetTwin shall execute the HTTP GET request by calling HTTPAPIEX_ExecuteRequest ]*/
        else if ((result = sendHttpRequestTwin(serviceClientDeviceTwinHandle, IOTHUB_TWIN_REQUEST_GET, deviceId, NULL, responseBuffer)) == IOTHUB_DEVICE_TWIN_ERROR)
        {
            /*Codes_SRS_IOTHUBDEVICETWIN_12_024: [ If any of the call fails during the HTTP creation IoTHubDeviceTwin_GetTwin shall fail and return IOTHUB_DEVICE_TWIN_ERROR ]*/
            /*Codes_SRS_IOTHUBDEVICETWIN_12_025: [ If any of the HTTPAPI call fails IoTHubDeviceTwin_GetTwin shall fail and return IOTHUB_DEVICE_TWIN_HTTPAPI_ERROR ]*/
            LogError("Failure sending HTTP request for create device");
        }
        else if (result == IOTHUB_DEVICE_TWIN_OK)
        {
            /*Codes_SRS_IOTHUBDEVICETWIN_12_026: [ IoTHubDeviceTwin_GetTwin shall verify the received HTTP status code and if it is not equal to 200 then return IOTHUB_DEVICE_TWIN_ERROR ]*/
            /*Codes_SRS_IOTHUBDEVICETWIN_12_027: [ IoTHubDeviceTwin_GetTwin shall verify the received HTTP status code and if it is equal to 200 then try to parse the response JSON to deviceTwin ]*/
            /*Codes_SRS_IOTHUBDEVICETWIN_12_028: [ If the JSON parsing failed, IoTHubDeviceTwin_GetTwin shall return IOTHUB_DEVICE_TWIN_JSON_INVALID ]*/
            if ((result = parseDeviceJson(responseBuffer, deviceTwin)) == IOTHUB_DEVICE_TWIN_OK)
            {
                /*Codes_SRS_IOTHUBDEVICETWIN_12_029: [ If the received JSON is empty, IoTHubDeviceTwin_GetTwin shall return IOTHUB_DEVICE_TWIN_JSON_EMPTY ]*/
                if ((deviceTwin == NULL) || (deviceTwin->deviceId == NULL))
                {
                    result = IOTHUB_DEVICE_TWIN_JSON_EMPTY;
                }
                else
                {
                    /*Codes_SRS_IOTHUBDEVICETWIN_12_030: [ Otherwise IoTHubDeviceTwin_GetTwin shall save the received `deviceTwin` to the out parameter and return IOTHUB_DEVICE_TWIN_OK ]*/
                }
            }
        }

        BUFFER_delete(responseBuffer);
    }
    return result;
}

IOTHUB_DEVICE_TWIN_RESULT IoTHubDeviceTwin_UpdateTwin(IOTHUB_SERVICE_CLIENT_DEVICE_TWIN_HANDLE serviceClientDeviceTwinHandle, const IOTHUB_DEVICE_TWIN* deviceTwin)
{
    IOTHUB_DEVICE_TWIN_RESULT result;

    /*Codes_SRS_IOTHUBDEVICETWIN_12_031: [ `IoTHubDeviceTwin_UpdateTwin` shall verify the input parameters and if any of them are `NULL` then return `IOTHUB_DEVICE_TWIN_INVALID_ARG` ]*/
    if ((serviceClientDeviceTwinHandle == NULL) || (deviceTwin == NULL))
    {
        LogError("Input parameter cannot be NULL");
        result = IOTHUB_DEVICE_TWIN_INVALID_ARG;
    }
    else
    {
        /*Codes_SRS_IOTHUBDEVICETWIN_12_032: [ IoTHubDeviceTwin_UpdateTwin shall verify the deviceTwin->deviceId input parameter and if it is NULL then return IOTHUB_DEVICE_TWIN_INVALID_ARG ]*/
        if (deviceTwin->deviceId == NULL)
        {
            LogError("deviceId cannot be NULL");
            result = IOTHUB_DEVICE_TWIN_INVALID_ARG;
        }
        else
        {
            /*Codes_SRS_IOTHUBDEVICETWIN_12_033: [ IoTHubDeviceTwin_UpdateTwin shall create a copy of the deviceTwin structure ]*/
            IOTHUB_DEVICE_TWIN* tempDeviceTwin;
            if ((tempDeviceTwin = createTemporaryDeviceTwin(deviceTwin)) == NULL)
            {
                /*Codes_SRS_IOTHUBDEVICETWIN_12_034: [ If the copy fails, IoTHubDeviceTwin_UpdateTwin shall do clean up and return NULL ]*/
                LogError("Failed to create tempDeviceTwin");
                result = IOTHUB_DEVICE_TWIN_ERROR;
            }
            else
            {
                BUFFER_HANDLE deviceJsonBuffer = NULL;
                BUFFER_HANDLE responseBuffer = NULL;

                /*Codes_SRS_IOTHUBDEVICETWIN_12_035: [ IoTHubDeviceTwin_UpdateTwin shall construct a Json representation of the deviceTwin structure ]*/
                if ((deviceJsonBuffer = constructDeviceTwinJson(tempDeviceTwin)) == NULL)
                {
                    /*Codes_SRS_IOTHUBDEVICETWIN_12_036: [ If Json creation fails, IoTHubDeviceTwin_UpdateTwin shall return IOTHUB_DEVICE_TWIN_JSON_ERROR  ]*/
                    LogError("Json creation failed");
                    result = IOTHUB_DEVICE_TWIN_JSON_ERROR;
                }
                /*Codes_SRS_IOTHUBDEVICETWIN_12_037: [ IoTHubDeviceTwin_UpdateTwin shall allocate memory for response buffer by calling BUFFER_new ]*/
                else if ((responseBuffer = BUFFER_new()) == NULL)
                {
                    /*Codes_SRS_IOTHUBDEVICETWIN_12_038: [ If the BUFFER_new fails, IoTHubDeviceTwin_UpdateTwin shall do clean up and return IOTHUB_DEVICE_TWIN_ERROR. ]*/
                    LogError("BUFFER_new failed for responseBuffer");
                    result = IOTHUB_DEVICE_TWIN_ERROR;
                }
                /*Codes_SRS_IOTHUBDEVICETWIN_12_039: [ IoTHubDeviceTwin_UpdateTwin shall create an HTTP PATCH request using the created JSON ]*/
                /*Codes_SRS_IOTHUBDEVICETWIN_12_040: [ IoTHubDeviceTwin_UpdateTwin shall create an HTTP PATCH request using the createdfollowing HTTP headers: authorization=sasToken,Request-Id=1001,Accept=application/json,Content-Type=application/json,charset=utf-8 ]*/
                /*Codes_SRS_IOTHUBDEVICETWIN_12_041: [ IoTHubDeviceTwin_UpdateTwin shall create an HTTPAPIEX_SAS_HANDLE handle by calling HTTPAPIEX_SAS_Create ]*/
                /*Codes_SRS_IOTHUBDEVICETWIN_12_042: [ IoTHubDeviceTwin_UpdateTwin shall create an HTTPAPIEX_HANDLE handle by calling HTTPAPIEX_Create ]*/
                /*Codes_SRS_IOTHUBDEVICETWIN_12_043: [ IoTHubDeviceTwin_UpdateTwin shall execute the HTTP PATCH request by calling HTTPAPIEX_ExecuteRequest ]*/
                else if ((result = sendHttpRequestTwin(serviceClientDeviceTwinHandle, IOTHUB_TWIN_REQUEST_UPDATE, deviceTwin->deviceId, deviceJsonBuffer, responseBuffer)) == IOTHUB_DEVICE_TWIN_OK)
                {
                    /*Codes_SRS_IOTHUBDEVICETWIN_12_044: [ If any of the call fails during the HTTP creation IoTHubDeviceTwin_UpdateTwin shall fail and return IOTHUB_DEVICE_TWIN_ERROR ]*/
                    /*Codes_SRS_IOTHUBDEVICETWIN_12_045: [ If any of the HTTPAPI call fails IoTHubDeviceTwin_UpdateTwin shall fail and return IOTHUB_DEVICE_TWIN_HTTPAPI_ERROR ]*/
                    LogError("Failure sending HTTP request for update device twin");
                }

                /*Codes_SRS_IOTHUBDEVICETWIN_12_048: [ IoTHubDeviceTwin_UpdateTwin shall do clean up before return ]*/
                if (deviceJsonBuffer != NULL)
                {
                    BUFFER_delete(deviceJsonBuffer);
                }
                if (responseBuffer != NULL)
                {
                    BUFFER_delete(responseBuffer);
                }
                free(tempDeviceTwin);
            }
        }
    }
    return result;
}

IOTHUB_DEVICE_TWIN_RESULT IoTHubDeviceTwin_UpdateDesiredProperties(IOTHUB_SERVICE_CLIENT_DEVICE_TWIN_HANDLE serviceClientDeviceTwinHandle, const char* deviceId, const IOTHUB_DEVICE_TWIN_PROPERTIES* deviceTwinProperties)
{
    IOTHUB_DEVICE_TWIN_RESULT result;

    /*Codes_SRS_IOTHUBDEVICETWIN_12_085: [ IoTHubDeviceTwin_UpdateDesiredProperties shall verify the input parameters and if any of them are NULL then return IOTHUB_DEVICE_TWIN_INVALID_ARG ]*/
    if ((serviceClientDeviceTwinHandle == NULL) || (deviceTwinProperties == NULL))
    {
        LogError("Input parameter cannot be NULL");
        result = IOTHUB_DEVICE_TWIN_INVALID_ARG;
    }
    else
    {
        /*Codes_SRS_IOTHUBDEVICETWIN_12_086: [ IoTHubDeviceTwin_UpdateDesiredProperties shall verify the deviceId input parameter and if it is NULL then return IOTHUB_DEVICE_TWIN_INVALID_ARG ]*/
        if (deviceId == NULL)
        {
            LogError("deviceId cannot be NULL");
            result = IOTHUB_DEVICE_TWIN_INVALID_ARG;
        }
        else
        {
            /*Codes_SRS_IOTHUBDEVICETWIN_12_087: [ IoTHubDeviceTwin_UpdateDesiredProperties shall create a copy of the deviceTwinProperties structure ]*/
            IOTHUB_DEVICE_TWIN_PROPERTIES* tempDeviceTwinProperties;
            if ((tempDeviceTwinProperties = createTemporaryDeviceTwinProperties(deviceTwinProperties)) == NULL)
            {
                /*Codes_SRS_IOTHUBDEVICETWIN_12_088: [ If the copy fails, IoTHubDeviceTwin_UpdateDesiredProperties shall do clean up and return NULL ]*/
                LogError("Failed to create tempDeviceTwin");
                result = IOTHUB_DEVICE_TWIN_ERROR;
            }
            else
            {
                BUFFER_HANDLE deviceJsonBuffer = NULL;
                BUFFER_HANDLE responseBuffer = NULL;

                /*Codes_SRS_IOTHUBDEVICETWIN_12_089: [ IoTHubDeviceTwin_UpdateDesiredProperties shall construct a Json representation of the deviceTwinProperties structure ]*/
                if ((deviceJsonBuffer = constructDeviceTwinPropertiesJson(tempDeviceTwinProperties)) == NULL)
                {
                    /*Codes_SRS_IOTHUBDEVICETWIN_12_090: [ If Json creation fails, IoTHubDeviceTwin_UpdateDesiredProperties shall return IOTHUB_DEVICE_TWIN_JSON_ERROR  ]*/
                    LogError("Json creation failed");
                    result = IOTHUB_DEVICE_TWIN_JSON_ERROR;
                }
                /*Codes_SRS_IOTHUBDEVICETWIN_12_091: [ IoTHubDeviceTwin_UpdateDesiredProperties shall allocate memory for response buffer by calling BUFFER_new ]*/
                else if ((responseBuffer = BUFFER_new()) == NULL)
                {
                    /*Codes_SRS_IOTHUBDEVICETWIN_12_092: [ If the BUFFER_new fails, IoTHubDeviceTwin_UpdateDesiredProperties shall do clean up and return IOTHUB_DEVICE_TWIN_ERROR. ]*/
                    LogError("BUFFER_new failed for responseBuffer");
                    result = IOTHUB_DEVICE_TWIN_ERROR;
                }
                /*Codes_SRS_IOTHUBDEVICETWIN_12_093: [ IoTHubDeviceTwin_UpdateDesiredProperties shall create an HTTP PATCH request using the created JSON ]*/
                /*Codes_SRS_IOTHUBDEVICETWIN_12_094: [ IoTHubDeviceTwin_UpdateDesiredProperties shall create an HTTP PATCH request using the createdfollowing HTTP headers: authorization=sasToken,Request-Id=1001,Accept=application/json,Content-Type=application/json,charset=utf-8 ]*/
                /*Codes_SRS_IOTHUBDEVICETWIN_12_095: [ IoTHubDeviceTwin_UpdateDesiredProperties shall create an HTTPAPIEX_SAS_HANDLE handle by calling `HTTPAPIEX_SAS_Create` ]*/
                /*Codes_SRS_IOTHUBDEVICETWIN_12_096: [ IoTHubDeviceTwin_UpdateDesiredProperties shall create an HTTPAPIEX_HANDLE handle by calling HTTPAPIEX_Create ]*/
                /*Codes_SRS_IOTHUBDEVICETWIN_12_097: [ IoTHubDeviceTwin_UpdateDesiredProperties shall execute the HTTP PATCH request by calling HTTPAPIEX_ExecuteRequest ]*/
                else if ((result = sendHttpRequestTwin(serviceClientDeviceTwinHandle, IOTHUB_TWIN_REQUEST_UPDATE_DESIRED, deviceId, deviceJsonBuffer, responseBuffer)) == IOTHUB_DEVICE_TWIN_OK) 
                {
                    /*Codes_SRS_IOTHUBDEVICETWIN_12_098: [ If any of the call fails during the HTTP creation IoTHubDeviceTwin_UpdateDesiredProperties shall fail and return IOTHUB_DEVICE_TWIN_ERROR ]*/
                    /*Codes_SRS_IOTHUBDEVICETWIN_12_099: [ If any of the HTTPAPI call fails IoTHubDeviceTwin_UpdateDesiredProperties shall fail and return IOTHUB_DEVICE_TWIN_HTTPAPI_ERROR ]*/
                    LogError("Failure sending HTTP request for update device twin properties");
                }

                /*Codes_SRS_IOTHUBDEVICETWIN_12_102: [ IoTHubDeviceTwin_UpdateDesiredProperties shall do clean up before return ]*/
                if (deviceJsonBuffer != NULL)
                {
                    BUFFER_delete(deviceJsonBuffer);
                }
                if (responseBuffer != NULL)
                {
                    BUFFER_delete(responseBuffer);
                }
                free(tempDeviceTwinProperties);
            }
        }
    }
    return result;
}

const char* IoTHubDeviceTwin_ToJson(const IOTHUB_DEVICE_TWIN* deviceTwin)
{
    (void*)deviceTwin;
    return NULL;
}

const char* IoTHubDeviceTwin_TagsToJson(const IOTHUB_DEVICE_TWIN* deviceTwin)
{
    (void*)deviceTwin;
    return NULL;
}

const char* IoTHubDeviceTwin_ReportedPropertiesToJson(const IOTHUB_DEVICE_TWIN* deviceTwin)
{
    (void*)deviceTwin;
    return NULL;
}

const char* IoTHubDeviceTwin_DesiredPropertiesToJson(const IOTHUB_DEVICE_TWIN* deviceTwin)
{
    (void*)deviceTwin;
    return NULL;
}

