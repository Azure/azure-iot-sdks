// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#include "gballoc.h"

#include <time.h>
#include "iothub_client_private.h"
#include "iothubtransporthttp.h"

#include "httpapiexsas.h"
#include "urlencode.h"
#include "iot_logging.h"
#include "httpapiex.h"
#include "httpapiexsas.h"
#include "strings.h"
#include "base64.h"
#include "doublylinkedlist.h"
#include "httpheaders.h"
#include "agenttime.h"

#define IOTHUB_APP_PREFIX "iothub-app-"
const char* IOTHUB_MESSAGE_ID = "iothub-messageid";
const char* IOTHUB_CORRELATION_ID = "iothub-correlationid";

#define CONTENT_TYPE "Content-Type"
#define APPLICATION_OCTET_STREAM "application/octet-stream"
#define APPLICATION_VND_MICROSOFT_IOTHUB_JSON "application/vnd.microsoft.iothub.json"

/*DEFAULT_GETMINIMUMPOLLINGTIME is the minimum time in seconds allowed between 2 consecutive GET issues to the service (GET=fetch messages)*/
/*the default is 25 minutes*/
#define DEFAULT_GETMINIMUMPOLLINGTIME ((unsigned int)25*60) 

#define MAXIMUM_MESSAGE_SIZE (255*1024-1)
#define MAXIMUM_PAYLOAD_OVERHEAD 384
#define MAXIMUM_PROPERTY_OVERHEAD 16

/*forward declaration*/
static int appendMapToJSON(STRING_HANDLE existing, const char* const* keys, const char* const* values, size_t count);

/*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_069: [This function shall return a pointer to a structure of type TRANSPORT_PROVIDER having the following values for its fields:] */
static TRANSPORT_PROVIDER thisTransportProvider =
{
    IoTHubTransportHttp_SetOption, /*pfIoTHubTransport_SetOption IoTHubTransport_SetOption;       */
    IoTHubTransportHttp_Create, /*pfIoTHubTransport_Create IoTHubTransport_Create;                                                  */
    IoTHubTransportHttp_Destroy, /*pfIoTHubTransport_Destroy IoTHubTransport_Destroy;                                                */
    IoTHubTransportHttp_Subscribe, /*pfIoTHubTransport_Subscribe IoTHubTransport_Subscribe;                                            */
    IoTHubTransportHttp_Unsubscribe, /*pfIoTHubTransport_Unsubscribe IoTHubTransport_Unsubscribe;                                        */
    IoTHubTransportHttp_DoWork, /*pfIoTHubTransport_DoWork IoTHubTransport_DoWork; */
    IoTHubTransportHttp_GetSendStatus /* pfIoTHubTransport_GetSendStatus IoTHubTransport_GetSendStatus */
};

const void* HTTP_Protocol(void)
{
    return &thisTransportProvider;
}

typedef struct HTTPTRANSPORT_HANDLE_DATA_TAG
{
    STRING_HANDLE eventHTTPrelativePath;
    STRING_HANDLE messageHTTPrelativePath;
    HTTP_HEADERS_HANDLE eventHTTPrequestHeaders;
    STRING_HANDLE hostName;
    HTTPAPIEX_HANDLE httpApiExHandle;
    HTTP_HEADERS_HANDLE messageHTTPrequestHeaders;
    STRING_HANDLE abandonHTTPrelativePathBegin;
    HTTPAPIEX_SAS_HANDLE sasObject;
    bool DoWork_PullMessage;
    bool doBatchedTransfers;
    bool isFirstPoll;
    unsigned int getMinimumPollingTime;
    time_t lastPollTime;
    PDLIST_ENTRY waitingToSend;
    DLIST_ENTRY eventConfirmations; /*holds items for event confirmations*/
}HTTPTRANSPORT_HANDLE_DATA;


/*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_003: [Otherwise IoTHubTransportHttp_Create shall create an immutable string (further called "event HTTP relative path") from the following pieces: "/devices/" + URL_ENCODED(config->upperConfig->deviceId) + "/messages/events?api-version=2015-08-15-preview".]*/
static void destroy_eventHTTPrelativePath(HTTPTRANSPORT_HANDLE_DATA* handleData)
{
    STRING_delete(handleData->eventHTTPrelativePath);
    handleData->eventHTTPrelativePath = NULL;
}

/*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_003: [Otherwise IoTHubTransportHttp_Create shall create an immutable string (further called "event HTTP relative path") from the following pieces: "/devices/" + URL_ENCODED(config->upperConfig->deviceId) + "/messages/events?api-version=2015-08-15-preview".]*/
static bool create_eventHTTPrelativePath(HTTPTRANSPORT_HANDLE_DATA* handleData, const IOTHUBTRANSPORT_CONFIG* config)
{
    bool result;
    STRING_HANDLE urlEncodedDeviceId = NULL;
    handleData->eventHTTPrelativePath = STRING_construct("/devices/");
    if (handleData->eventHTTPrelativePath == NULL)
    {
        result = false;
    }
    else if (!(
        ((urlEncodedDeviceId = URL_EncodeString(config->upperConfig->deviceId)) != NULL) &&
        (STRING_concat_with_STRING(handleData->eventHTTPrelativePath, urlEncodedDeviceId) == 0) &&
        (STRING_concat(handleData->eventHTTPrelativePath, EVENT_ENDPOINT API_VERSION) == 0)
        ))
    {
        destroy_eventHTTPrelativePath(handleData);
        result = false;
    }
    else
    {
        result = true;
    }
    STRING_delete(urlEncodedDeviceId);
    return result;
}

/*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_034: [Otherwise, IoTHubTransportHttp_Create shall create an immutable string (further called "message HTTP relative path") from the following pieces: "/devices/" + URL_ENCODED(config->upperConfig->deviceId) + "/messages/devicebound?api-version=2015-08-15-preview".]*/
static void destroy_messageHTTPrelativePath(HTTPTRANSPORT_HANDLE_DATA* handleData)
{
    STRING_delete(handleData->messageHTTPrelativePath);
    handleData->messageHTTPrelativePath = NULL;
}

/*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_034: [Otherwise, IoTHubTransportHttp_Create shall create an immutable string (further called "message HTTP relative path") from the following pieces: "/devices/" + URL_ENCODED(config->upperConfig->deviceId) + "/messages/devicebound?api-version=2015-08-15-preview".]*/
static bool create_messageHTTPrelativePath(HTTPTRANSPORT_HANDLE_DATA* handleData, const IOTHUBTRANSPORT_CONFIG* config)
{
    bool result;
    handleData->messageHTTPrelativePath = STRING_construct("/devices/");
    if (handleData->messageHTTPrelativePath == NULL)
    {
        result = false;
    }
    else
    {
        STRING_HANDLE urlEncodedDeviceId = NULL;
        if (!(
            ((urlEncodedDeviceId = URL_EncodeString(config->upperConfig->deviceId)) != NULL) &&
            (STRING_concat_with_STRING(handleData->messageHTTPrelativePath, urlEncodedDeviceId) == 0) &&
            (STRING_concat(handleData->messageHTTPrelativePath, MESSAGE_ENDPOINT_HTTP API_VERSION) == 0)
            ))
        {
            result = false;
            destroy_messageHTTPrelativePath(handleData);
        }
        else
        {
            result = true;
        }
        STRING_delete(urlEncodedDeviceId);
    }
    
    return result;
}

/*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_005: [Otherwise, IoTHubTransportHttp_Create shall create a set of HTTP headers (further on called "event HTTP request headers") consisting of the following fixed field names and values:
"iothub-to":"/devices/" + URL_ENCODED(config->upperConfig->deviceId) + "/messages/events";
"Authorization":" "
"Content-Type":"application/vnd.microsoft.iothub.json"
"Accept":"application/json"
"Connection":"Keep-Alive"]*/
static void destroy_eventHTTPrequestHeaders(HTTPTRANSPORT_HANDLE_DATA* handleData)
{
    HTTPHeaders_Free(handleData->eventHTTPrequestHeaders);
    handleData->eventHTTPrequestHeaders = NULL;
}

/*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_005: [Otherwise, IoTHubTransportHttp_Create shall create a set of HTTP headers (further on called "event HTTP request headers") consisting of the following fixed field names and values:
"iothub-to":"/devices/" + URL_ENCODED(config->upperConfig->deviceId) + "/messages/events";
"Authorization":" "
"Content-Type":"application/vnd.microsoft.iothub.json"
"Accept":"application/json"
"Connection":"Keep-Alive"]*/
static bool create_eventHTTPrequestHeaders(HTTPTRANSPORT_HANDLE_DATA* handleData, const IOTHUBTRANSPORT_CONFIG* config)
{
    bool result;
    handleData->eventHTTPrequestHeaders = HTTPHeaders_Alloc();
    if (handleData->eventHTTPrequestHeaders == NULL)
    {
        result = false;
    }
    else
    {
        STRING_HANDLE temp = STRING_construct("/devices/");
        if (temp == NULL)
        {
            result = false;
            destroy_eventHTTPrequestHeaders(handleData);
        }
        else
        {
            STRING_HANDLE urlEncodedDeviceId = NULL;
            if (!(
                ((urlEncodedDeviceId = URL_EncodeString(config->upperConfig->deviceId)) != NULL) &&
                (STRING_concat_with_STRING(temp, urlEncodedDeviceId) == 0) &&
                (STRING_concat(temp, EVENT_ENDPOINT) == 0)
                ))
            {
                result = false;
                destroy_eventHTTPrequestHeaders(handleData);
            }
            else
            {
                if (!(
                    (HTTPHeaders_AddHeaderNameValuePair(handleData->eventHTTPrequestHeaders, "iothub-to", STRING_c_str(temp)) == HTTP_HEADERS_OK) &&
                    (HTTPHeaders_AddHeaderNameValuePair(handleData->eventHTTPrequestHeaders, "Authorization", " ") == HTTP_HEADERS_OK) &&
                    (HTTPHeaders_AddHeaderNameValuePair(handleData->eventHTTPrequestHeaders, "Accept", "application/json") == HTTP_HEADERS_OK) &&
                    (HTTPHeaders_AddHeaderNameValuePair(handleData->eventHTTPrequestHeaders, "Connection", "Keep-Alive") == HTTP_HEADERS_OK)
                    ))
                {
                    result = false;
                    destroy_eventHTTPrequestHeaders(handleData);
                }
                else
                {
                    result = true;
                }
            }
            STRING_delete(temp);
            STRING_delete(urlEncodedDeviceId);
        }
    }
    return result;
}

/*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_007: [Otherwise, IoTHubTransportHttp_Create shall create an immutable string (further called hostname) containing config->upperConfig->iotHubName + config->upperConfig->iotHubSuffix.]*/
static void destroy_hostName(HTTPTRANSPORT_HANDLE_DATA* handleData)
{
    STRING_delete(handleData->hostName);
    handleData->hostName = NULL;
}

/*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_007: [Otherwise, IoTHubTransportHttp_Create shall create an immutable string (further called hostname) containing config->upperConfig->iotHubName + config->upperConfig->iotHubSuffix.]*/
static bool create_hostName(HTTPTRANSPORT_HANDLE_DATA* handleData, const IOTHUBTRANSPORT_CONFIG* config)
{
    bool result;
    handleData->hostName = STRING_construct(config->upperConfig->iotHubName);
    if (handleData->hostName == NULL)
    {
        result = false;
    }
    else
    {
        if ((STRING_concat(handleData->hostName, ".") != 0) ||
            (STRING_concat(handleData->hostName, config->upperConfig->iotHubSuffix) != 0))
        {
            destroy_hostName(handleData);
            result = false;
        }
        else
        {
            result = true;
        }
    }
    return result;
}

/*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_009: [Otherwise, IoTHubTransportHttp_Create shall create a HTTPAPIEX_HANDLE by a call to HTTPAPIEX_Create passing for hostName the hostname so far constructed by IoTHubTransportHttp_Create.]*/
static void destroy_httpApiExHandle(HTTPTRANSPORT_HANDLE_DATA* handleData)
{
    HTTPAPIEX_Destroy(handleData->httpApiExHandle);
    handleData->httpApiExHandle = NULL;
}

/*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_007: [Otherwise, IoTHubTransportHttp_Create shall create an immutable string (further called hostname) containing config->upperConfig->iotHubName + config->upperConfig->iotHubSuffix.]*/
static bool create_httpApiExHandle(HTTPTRANSPORT_HANDLE_DATA* handleData, const IOTHUBTRANSPORT_CONFIG* config)
{
    bool result;
    (void)config;
    handleData->httpApiExHandle = HTTPAPIEX_Create(STRING_c_str(handleData->hostName));
    if (handleData->httpApiExHandle == NULL)
    {
        result = false;
    }
    else
    {
        result = true;
    }
    return result;
}

/*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_059: [Otherwise, IoTHubTransportHttp_Create shall create a set of HTTP headers (further on called "message HTTP request headers") consisting of the following fixed field names and values:
"Authorization": " "]*/
static void destroy_messageHTTPrequestHeaders(HTTPTRANSPORT_HANDLE_DATA* handleData)
{
    HTTPHeaders_Free(handleData->messageHTTPrequestHeaders);
    handleData->messageHTTPrequestHeaders = NULL;
}

/*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_059: [Otherwise, IoTHubTransportHttp_Create shall create a set of HTTP headers (further on called "message HTTP request headers") consisting of the following fixed field names and values:
"Authorization": " "]*/
static bool create_messageHTTPrequestHeaders(HTTPTRANSPORT_HANDLE_DATA* handleData, const IOTHUBTRANSPORT_CONFIG* config)
{
    bool result;
    (void)config;
    handleData->messageHTTPrequestHeaders = HTTPHeaders_Alloc();
    if (handleData->messageHTTPrequestHeaders == NULL)
    {
        result = false;
    }
    else
    {
        if (HTTPHeaders_AddHeaderNameValuePair(handleData->messageHTTPrequestHeaders, "Authorization", " ") != HTTP_HEADERS_OK)
        {
            destroy_messageHTTPrequestHeaders(handleData);
            result = false;
        }
        else
        {
            result = true;
        }
    }
    return result;
}

/*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_061: [Otherwise, IoTHubTransportHttp_Create shall create a STRING containing: "/devices/" + URL_ENCODED(device id) +"/messages/deviceBound/" called abandonHTTPrelativePathBegin.] */
static void destroy_abandonHTTPrelativePathBegin(HTTPTRANSPORT_HANDLE_DATA* handleData)
{
    STRING_delete(handleData->abandonHTTPrelativePathBegin);
    handleData->abandonHTTPrelativePathBegin = NULL;
}

/*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_061: [Otherwise, IoTHubTransportHttp_Create shall create a STRING containing: "/devices/" + URL_ENCODED(device id) +"/messages/deviceBound/" called abandonHTTPrelativePathBegin.] */
static bool create_abandonHTTPrelativePathBegin(HTTPTRANSPORT_HANDLE_DATA* handleData, const IOTHUBTRANSPORT_CONFIG* config)
{
    bool result;
    handleData->abandonHTTPrelativePathBegin = STRING_construct("/devices/");
    if (handleData->abandonHTTPrelativePathBegin == NULL)
    {
        result = false;
    }
    else
    {
        STRING_HANDLE urlEncodedDeviceId = NULL;
        if (!(
            ((urlEncodedDeviceId = URL_EncodeString(config->upperConfig->deviceId)) != NULL) &&
            (STRING_concat_with_STRING(handleData->abandonHTTPrelativePathBegin, urlEncodedDeviceId) == 0) &&
            (STRING_concat(handleData->abandonHTTPrelativePathBegin, MESSAGE_ENDPOINT_HTTP_ETAG) == 0)
            ))
        {
            LogError("unable to STRING_concat\r\n");
            STRING_delete(handleData->abandonHTTPrelativePathBegin);
            result = false;
        }
        else
        {
            result = true;
        }
        STRING_delete(urlEncodedDeviceId);
    }
    return result;
}

static void destroy_SASObject(HTTPTRANSPORT_HANDLE_DATA* handleData)
{
    HTTPAPIEX_SAS_Destroy(handleData->sasObject);
    handleData->sasObject = NULL;
}

static bool create_deviceSASObject(HTTPTRANSPORT_HANDLE_DATA* handleData, const IOTHUBTRANSPORT_CONFIG* config)
{
    STRING_HANDLE keyName;
    bool result;
    /*Codes_SRS_IOTHUBTRANSPORTTHTTP_06_001: [IoTHubTransportHttp_Create shall invoke URL_EncodeString with an argument of device id.]*/
    keyName = URL_EncodeString(config->upperConfig->deviceId);
    if (keyName == NULL)
    {
        /*Codes_SRS_IOTHUBTRANSPORTTHTTP_06_002: [If the encode fails then IoTHubTransportHttp_Create shall fail and return NULL.]*/
        result = false;
    }
    else
    {
        STRING_HANDLE uriResource;
        /*Codes_SRS_IOTHUBTRANSPORTTHTTP_06_003: [IoTHubTransportHttp_Create shall invoke STRING_clone using the previously created hostname.]*/
        uriResource = STRING_clone(handleData->hostName);
        /*Codes_SRS_IOTHUBTRANSPORTTHTTP_06_004: [If the clone fails then IoTHubTransportHttp_Create shall fail and return NULL.]*/
        if (uriResource != NULL)
        {
            /*Codes_SRS_IOTHUBTRANSPORTTHTTP_06_005: [IoTHubTransportHttp_Create shall invoke STRING_concat with arguments uriResource and the string "/devices/".]*/
            /*Codes_SRS_IOTHUBTRANSPORTTHTTP_06_006: [If the concat fails then IoTHubTransportHttp_Create shall fail and return NULL.]*/
            /*Codes_SRS_IOTHUBTRANSPORTTHTTP_06_007: [IoTHubTransportHttp_Create shall invoke STRING_concat_with_STRING with arguments uriResource and keyName.]*/
            /*Codes_SRS_IOTHUBTRANSPORTTHTTP_06_008: [If the STRING_concat_with_STRING fails then IoTHubTransportHttp_Create shall fail and return NULL.]*/
            if ((STRING_concat(uriResource, "/devices/") == 0) &&
                (STRING_concat_with_STRING(uriResource, keyName) == 0))
            {
                /*Codes_SRS_IOTHUBTRANSPORTTHTTP_06_009: [IoTHubTransportHttp_Create shall invoke STRING_construct with an argument of config->upperConfig->deviceKey.]*/
                /*Codes_SRS_IOTHUBTRANSPORTTHTTP_06_010: [If the STRING_construct fails then IoTHubTransportHttp_Create shall fail and return NULL.]*/
                STRING_HANDLE key = STRING_construct(config->upperConfig->deviceKey);
                if (key != NULL)
                {
                    /*Codes_SRS_IOTHUBTRANSPORTTHTTP_06_013: [The keyName is shortened to zero length, if that fails then IoTHubTransportHttp_Create shall fail and return NULL.]*/
                    if (STRING_empty(keyName) != 0)
                    {
                        LogError("Unable to form the device key name for the SAS\r\n");
                        result = false;
                    }
                    else
                    {
                        /*Codes_SRS_IOTHUBTRANSPORTTHTTP_06_011: [IoTHubTransportHttp_Create shall invoke HTTPAPIEX_SAS_Create with arguments key, uriResource, and zero length keyName.]*/
                        /*Codes_SRS_IOTHUBTRANSPORTTHTTP_06_012: [If the HTTPAPIEX_SAS_Create fails then IoTHubTransportHttp_Create shall fail and return NULL.]*/
                        handleData->sasObject = HTTPAPIEX_SAS_Create(key, uriResource, keyName);
                        result = (handleData->sasObject != NULL) ? (true) : (false);
                    }
                    STRING_delete(key);
                }
                else
                {
                    result = false;
                }
            }
            else
            {
                result = false;
            }
            STRING_delete(uriResource);
        }
        else
        {
            result = false;
        }
        STRING_delete(keyName);
    }
    return result;
}

TRANSPORT_HANDLE IoTHubTransportHttp_Create(const IOTHUBTRANSPORT_CONFIG* config)
{
    HTTPTRANSPORT_HANDLE_DATA* result;
    /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_001: [If parameter config is NULL then IoTHubTransportHttp_Create shall fail and return NULL.] */
    /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_002: [IoTHubTransportHttp_Create shall fail and return NULL if any fields of the config structure are NULL.] */
    if (config == NULL)
    {
        LogError("invalid arg (configuration is missing)\r\n");
        result = NULL;
    }
    else if (config->upperConfig == NULL)
    {
        LogError("invalid arg (upperConfig is NULL)\r\n");
        result = NULL;
    }
    else if (config->waitingToSend == NULL)
    {
        LogError("invalid arg (waitingToSend is NULL)\r\n");
        result = NULL;
    }
    else if (config->upperConfig->protocol == NULL)
    {
        LogError("invalid arg (protocol is NULL)\r\n");
        result = NULL;
    }
    else if (config->upperConfig->deviceId == NULL)
    {
        LogError("invalid arg (deviceId is NULL)\r\n");
        result = NULL;
    }
    else if (config->upperConfig->iotHubName == NULL)
    {
        LogError("invalid arg (iotHubName is NULL)\r\n");
        result = NULL;
    }
    else if (config->upperConfig->iotHubSuffix == NULL)
    {
        LogError("invalid arg (iotHubSuffix is NULL)\r\n");
        result = NULL;
    }
    else if (config->upperConfig->deviceKey == NULL)
    {
        LogError("invalid arg (deviceKey is NULL)\r\n");
        result = NULL;
    }
    else
    {
        /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_003: [Otherwise IoTHubTransportHttp_Create shall create an immutable string (further called "event HTTP relative path") from the following pieces: "/devices/" + URL_ENCODED(config->upperConfig->deviceId) + "/messages/events?api-version=2015-08-15-preview".]*/
        result = (HTTPTRANSPORT_HANDLE_DATA*)malloc(sizeof(HTTPTRANSPORT_HANDLE_DATA));
        if (result == NULL)
        {
            LogError("unable to malloc\r\n");
        }
        else
        {
            bool was_eventHTTPrelativePath_ok = create_eventHTTPrelativePath(result, config);
            bool was_messageHTTPrelativePath_ok = was_eventHTTPrelativePath_ok && create_messageHTTPrelativePath(result, config);
            bool was_eventHTTPrequestHeaders_ok = was_messageHTTPrelativePath_ok && create_eventHTTPrequestHeaders(result, config);
            bool was_hostName_ok = was_eventHTTPrequestHeaders_ok && create_hostName(result, config);
            bool was_httpApiExHandle_ok = was_hostName_ok && create_httpApiExHandle(result, config);
            bool was_messageHTTPrequestHeaders_ok = was_httpApiExHandle_ok && create_messageHTTPrequestHeaders(result, config);
            bool was_abandonHTTPrelativePathBegin_ok = was_messageHTTPrequestHeaders_ok && create_abandonHTTPrelativePathBegin(result, config);
            bool was_sasObject_ok = was_abandonHTTPrelativePathBegin_ok && create_deviceSASObject(result, config);

            /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_011: [Otherwise, IoTHubTransportHttp_Create shall set a flag called "DoWork_PullMessage" to false, succeed and return a non-NULL value.]*/
            if (was_sasObject_ok)
            {
                result->DoWork_PullMessage = false;
                result->doBatchedTransfers = false;
                result->isFirstPoll = true;
                result->getMinimumPollingTime = DEFAULT_GETMINIMUMPOLLINGTIME;
                result->waitingToSend = config->waitingToSend;
                DList_InitializeListHead(&(result->eventConfirmations));
            }
            else
            {
                /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_004: [If creating the string fail for any reason then IoTHubTransportHttp_Create shall fail and return NULL.] */
                if (was_eventHTTPrelativePath_ok) destroy_eventHTTPrelativePath(result);
                /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_035: [If creating the message HTTP relative path fails, then IoTHubTransportHttp_Create shall fail and return NULL.] */
                if (was_messageHTTPrelativePath_ok) destroy_messageHTTPrelativePath(result);
                /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_006: [If creating the event HTTP request headers fails, then IoTHubTransportHttp_Create shall fail and return NULL.] */
                if (was_eventHTTPrequestHeaders_ok) destroy_eventHTTPrequestHeaders(result);
                /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_008: [If creating the hostname fails then IoTHubTransportHttp_Create shall fail and return NULL.] */
                if (was_hostName_ok) destroy_hostName(result);
                /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_010: [If creating the HTTPAPIEX_HANDLE fails then IoTHubTransportHttp_Create shall fail and return NULL.] */
                if (was_httpApiExHandle_ok) destroy_httpApiExHandle(result);
                /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_060: [If creating message HTTP request headers then IoTHubTransportHttp_Create shall fail and return NULL.]*/
                if (was_messageHTTPrequestHeaders_ok) destroy_messageHTTPrequestHeaders(result);
                /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_062: [If creating the abandonHTTPrelativePathBegin fails then IoTHubTransportHttp_Create shall fail and return NULL] */
                if (was_abandonHTTPrelativePathBegin_ok) destroy_abandonHTTPrelativePathBegin(result);

                free(result);
                result = NULL;
            }
        }
    }
    return result;
}

void IoTHubTransportHttp_Destroy(TRANSPORT_HANDLE handle)
{
    /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_012: [IoTHubTransportHttp_Destroy shall do nothing if parameter handle is NULL.]*/
    if (handle != NULL)
    {
        /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_013: [Otherwise IoTHubTransportHttp_Destroy shall free all the resources currently in use.] */
        destroy_eventHTTPrelativePath(handle);
        destroy_messageHTTPrelativePath(handle);
        destroy_eventHTTPrequestHeaders(handle);
        destroy_hostName(handle);
        destroy_httpApiExHandle(handle);
        destroy_messageHTTPrequestHeaders(handle);
        destroy_abandonHTTPrelativePathBegin(handle);
        destroy_SASObject(handle);
        free(handle);
    }
}

int IoTHubTransportHttp_Subscribe(TRANSPORT_HANDLE handle)
{
    int result;
    /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_014: [If parameter handle is NULL then IoTHubTransportHttp_Subscribe shall fail and return a non-zero value.] */
    if (handle == NULL)
    {
        LogError("invalid arg passed to IoTHubTransportHttp_Subscribe\r\n");
        result = __LINE__;
    }
    else
    {
        /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_056: [Otherwise, IoTHubTransportHttp_Subscribe shall set the flag called DoWork_PullMessages to true and succeed.] */
        HTTPTRANSPORT_HANDLE_DATA* handleData = (HTTPTRANSPORT_HANDLE_DATA*)handle;
        handleData->DoWork_PullMessage = true;
        result = 0;
    }
    return result;
}

void IoTHubTransportHttp_Unsubscribe(TRANSPORT_HANDLE handle)
{
    /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_016: [If parameter handle is NULL then IoTHubTransportHttp_Unsubscribe shall do nothing.] */
    if (handle != NULL)
    {
        HTTPTRANSPORT_HANDLE_DATA* handleData = (HTTPTRANSPORT_HANDLE_DATA*)handle;
        /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_058: [Otherwise it shall set the flag DoWork_PullMessage to false.] */
        handleData->DoWork_PullMessage = false;
    }
}

/*produces a representation of the properties, if they exist*/
/*if they do not exist, produces ""*/
static int concat_Properties(STRING_HANDLE existing, MAP_HANDLE map, size_t* propertiesMessageSizeContribution)
{
    int result;
    const char*const* keys;
    const char*const* values;
    size_t count;
    if (Map_GetInternals(map, &keys, &values, &count) != MAP_OK)
    {
        result = __LINE__;
        LogError("error while Map_GetInternals\r\n");
    }
    else
    {

        if (count == 0)
        {
            /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_076: [If IoTHubMessage does not have properties, then "properties":{...} shall be missing from the payload*/
            /*no properties - do nothing with existing*/
            result = 0;
            *propertiesMessageSizeContribution = 0;
        }
        else
        {
            /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_075: [If IoTHubMessage has properties, then they shall be serialized at the same level as "body" using the following pattern: "properties":{"iothub-app-name1":"value1","iothub-app-name2":"value2*/
            if (STRING_concat(existing, ",\"properties\":") != 0)
            {
                /*go ahead and return it*/
                result = __LINE__;
                LogError("failed STRING_concat\r\n");
            }
            else if (appendMapToJSON(existing, keys, values, count) != 0)
            {
                result = __LINE__;
                LogError("unable to append the properties\r\n");
            }
            else
            {
                /*all is fine*/
                size_t i;
                *propertiesMessageSizeContribution = 0;
                for (i = 0;i < count;i++)
                {
                    /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_120: [Every property name shall add to the message size the length of the property name + the length of the property value + 16 bytes.] */
                    *propertiesMessageSizeContribution += (strlen(keys[i]) + strlen(values[i]) + MAXIMUM_PROPERTY_OVERHEAD);
                }
                result = 0;
            }
        }
    }
    return result;
}

/*produces a JSON representation of the map : {"a": "value_of_a","b":"value_of_b"}*/
static int appendMapToJSON(STRING_HANDLE existing, const char* const* keys, const char* const* values, size_t count) /*under consideration: move to MAP module when it has more than 1 user*/
{
    int result;
    if (STRING_concat(existing, "{") != 0)
    {
        /*go on and return it*/
        result = __LINE__;
        LogError("STRING_construct failed\r\n");
    }
    else
    {
        size_t i;
        for (i = 0; i < count; i++)
        {
            if (!(
                (STRING_concat(existing, (i == 0) ? "\"" IOTHUB_APP_PREFIX : ",\"" IOTHUB_APP_PREFIX) == 0) &&
                (STRING_concat(existing, keys[i]) == 0) &&
                (STRING_concat(existing, "\":\"") == 0) &&
                (STRING_concat(existing, values[i]) == 0) &&
                (STRING_concat(existing, "\"") == 0)
                ))
            {
                LogError("unable to STRING_concat\r\n");
                break;
            }
        }

        if (i < count)
        {
            result = __LINE__;
            /*error, let it go through*/
        }
        else if (STRING_concat(existing, "}") != 0)
        {
            result = __LINE__;
            LogError("unable to STRING_concat\r\n");
        }
        else
        {
            /*all is fine*/
            result = 0;
        }
    }
    return result;
}

/*makes the following string:{"body":"base64 encoding of the message content"[,"properties":{"a":"valueOfA"}]}*/
/*return NULL if there was a failure, or a non-NULL STRING_HANDLE that contains the intended data*/
static STRING_HANDLE make1EventJSONitem(PDLIST_ENTRY item, size_t *messageSizeContribution)
{
    STRING_HANDLE result; /*temp wants to contain :{"body":"base64 encoding of the message content"[,"properties":{"a":"valueOfA"}]}*/
    IOTHUB_MESSAGE_LIST* message = containingRecord(item, IOTHUB_MESSAGE_LIST, entry);
    IOTHUBMESSAGE_CONTENT_TYPE contentType = IoTHubMessage_GetContentType(message->messageHandle);
    
    switch (contentType)
    {
    case IOTHUBMESSAGE_BYTEARRAY:
    {
        result = STRING_construct("{\"body\":\"");
        if (result == NULL)
        {
            LogError("unable to STRING_construct\r\n");
        }
        else
        {
            const unsigned char* source;
            size_t size;

            if (IoTHubMessage_GetByteArray(message->messageHandle, &source, &size) != IOTHUB_MESSAGE_OK)
            {
                LogError("unable to get the data for the message.\r\n");
                STRING_delete(result);
                result = NULL;
            }
            else
            {
                STRING_HANDLE encoded = Base64_Encode_Bytes(source, size);
                if (encoded == NULL)
                {
                    LogError("unable to Base64_Encode_Bytes.\r\n");
                    STRING_delete(result);
                    result = NULL;
                }
                else
                {
                    size_t propertiesSize;
                    if (!(
                        (STRING_concat_with_STRING(result, encoded) == 0) &&
                        (STRING_concat(result, "\"") == 0) && /*\" because closing value*/
                        (concat_Properties(result, IoTHubMessage_Properties(message->messageHandle), &propertiesSize) == 0) &&
                        (STRING_concat(result, "},") == 0) /*the last comma shall be replaced by a ']' by DaCr's suggestion (which is awesome enough to receive credits in the source code)*/
                        ))
                    {
                        STRING_delete(result);
                        result = NULL;
                        LogError("unable to STRING_concat_with_STRING.\r\n");
                    }
                    else
                    {
                        /*all is fine... */
                        /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_119: [The message size is computed from the length of the payload + 384.] */
                        *messageSizeContribution = size + MAXIMUM_PAYLOAD_OVERHEAD + propertiesSize;
                    }
                    STRING_delete(encoded);
                }
            }
        }
        break;
    }
    /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_113: [If a messages to be send has type IOTHUBMESSAGE_STRING, then its serialization shall be {"body":"JSON encoding of the string", "base64Encoded":false}] */
    case IOTHUBMESSAGE_STRING:
    {
        result = STRING_construct("{\"body\":");
        if (result == NULL)
        {
            LogError("unable to STRING_construct\r\n");
        }
        else
        {
            const char* source = IoTHubMessage_GetString(message->messageHandle);
            if (source == NULL)
            {
                LogError("unable to IoTHubMessage_GetString\r\n");
                STRING_delete(result);
                result = NULL;
            }
            else
            {
                STRING_HANDLE asJson = STRING_new_JSON(source);
                if (asJson == NULL)
                {
                    LogError("unable to STRING_new_JSON\r\n");
                    STRING_delete(result);
                    result = NULL;
                }
                else
                {
                    size_t propertiesSize;
                    if (!(
                        (STRING_concat_with_STRING(result, asJson) == 0) &&
                        (STRING_concat(result, ",\"base64Encoded\":false") == 0) &&
                        (concat_Properties(result, IoTHubMessage_Properties(message->messageHandle), &propertiesSize) == 0) &&
                        (STRING_concat(result, "},") == 0) /*the last comma shall be replaced by a ']' by DaCr's suggestion (which is awesome enough to receive credits in the source code)*/
                        ))
                    {
                        LogError("unable to STRING_concat_with_STRING");
                        STRING_delete(result);
                        result = NULL;
                    }
                    else
                    {
                        /*result has the intended content*/
                        /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_119: [The message size is computed from the length of the payload + 384.] */
                        *messageSizeContribution = strlen(source) + MAXIMUM_PAYLOAD_OVERHEAD + propertiesSize;
                    }
                    STRING_delete(asJson);
                }
            }
        }
        break;
    }
    default:
    {
        LogError("an unknown message type was encountered (%d)\r\n", contentType);
        result = NULL; /*unknown message type*/
        break;
    }
    }
    return result;
}

#define MAKE_PAYLOAD_RESULT_VALUES \
    MAKE_PAYLOAD_OK, /*returned when there is a payload to be later send by HTTP*/ \
    MAKE_PAYLOAD_NO_ITEMS, /*returned when there are no items to be send*/ \
    MAKE_PAYLOAD_ERROR, /*returned when there were errors*/ \
    MAKE_PAYLOAD_FIRST_ITEM_DOES_NOT_FIT /*returned when the first item doesn't fit*/

DEFINE_ENUM(MAKE_PAYLOAD_RESULT, MAKE_PAYLOAD_RESULT_VALUES);

/*this function assembles several {"body":"base64 encoding of the message content"," base64Encoded": true} into 1 payload*/
/*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_070: [IoTHubTransportHttp_DoWork shall build the following string:[{"body":"base64 encoding of the message1 content"},{"body":"base64 encoding of the message2 content"}...]]*/
static MAKE_PAYLOAD_RESULT makePayload(HTTPTRANSPORT_HANDLE_DATA* handleData, STRING_HANDLE* payload)
{
    MAKE_PAYLOAD_RESULT result;
    size_t allMessagesSize = 0;
    *payload = STRING_construct("[");
    if (*payload == NULL)
    {
        LogError("unable to STRING_construct\r\n");
        result = MAKE_PAYLOAD_ERROR;
    }
    else
    {
        bool isFirst = true;
        PDLIST_ENTRY actual;
        /*either all the items enter the list or only some*/
        result = MAKE_PAYLOAD_OK; /*optimistically initializing it*/
        bool keepGoing = true; /*keepGoing gets sometimes to false from within the loop*/
        while (keepGoing && ((actual = handleData->waitingToSend->Flink) != handleData->waitingToSend))
        {
            size_t messageSize;
            STRING_HANDLE temp = make1EventJSONitem(actual, &messageSize);
            if (isFirst)
            {
                isFirst = false;
                /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_073: [If there is no valid payload, IoTHubTransportHttp_DoWork shall advance to the next activity.]*/
                if (temp == NULL) /*first item failed to create, nothing to send*/
                {
                    result = MAKE_PAYLOAD_ERROR;
                    STRING_delete(*payload);
                    *payload = NULL;
                    keepGoing = false;
                }
                else
                {
                    /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_071: [If the oldest message in waitingToSend causes the message size to exceed the message size limit then it shall be removed from waitingToSend, and IoTHubClient_LL_SendComplete shall be called. Parameter PDLIST_ENTRY completed shall point to a list containing only the oldest item, and parameter IOTHUB_BATCHSTATE result shall be set to IOTHUB_BATCHSTATE_FAILED.]*/
                    /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_118: [The message size shall be limited to 255KB - 1 byte.]*/
                    if (messageSize > MAXIMUM_MESSAGE_SIZE)
                    {
                        PDLIST_ENTRY head = DList_RemoveHeadList(handleData->waitingToSend); /*actually this is the same as "actual", but now it is removed*/
                        DList_InsertTailList(&(handleData->eventConfirmations), head);
                        result = MAKE_PAYLOAD_FIRST_ITEM_DOES_NOT_FIT;
                        STRING_delete(*payload);
                        *payload = NULL;
                        keepGoing = false;
                    }
                    else
                    {
                        if (STRING_concat_with_STRING(*payload, temp) != 0)
                        {
                            /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_073: [If there is no valid payload, IoTHubTransportHttp_DoWork shall advance to the next activity.]*/
                            result = MAKE_PAYLOAD_ERROR;
                            STRING_delete(*payload);
                            *payload = NULL;
                            keepGoing = false;
                        }
                        else
                        {
                            /*first item was put nicely in the payload*/
                            PDLIST_ENTRY head = DList_RemoveHeadList(handleData->waitingToSend); /*actually this is the same as "actual", but now it is removed*/
                            DList_InsertTailList(&(handleData->eventConfirmations), head);
                            allMessagesSize += messageSize;
                        }
                    }
                    STRING_delete(temp);
                }
            }
            else
            {
                /*there is at least 1 item already in the payload*/
                if (temp == NULL)
                {
                    /*there are multiple payloads encoded, the last one had an internal error, just go with those - closing the payload happens "after the loop"*/
                    /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_072: [If at any point during construction of the string there are errors, IoTHubTransportHttp_DoWork shall use the so far constructed string as payload.]*/
                    result = MAKE_PAYLOAD_OK;
                    keepGoing = false;
                }
                else
                {
                    if (allMessagesSize + messageSize > MAXIMUM_MESSAGE_SIZE)
                    {
                        /*this item doesn't make it to the payload, but the payload is valid so far*/
                        /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_072: [If at any point during construction of the string there are errors, IoTHubTransportHttp_DoWork shall use the so far constructed string as payload.]*/
                        result = MAKE_PAYLOAD_OK;
                        keepGoing = false;
                    }
                    else if (STRING_concat_with_STRING(*payload, temp) != 0)
                    {
                        /*should still send what there is so far...*/
                        /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_072: [If at any point during construction of the string there are errors, IoTHubTransportHttp_DoWork shall use the so far constructed string as payload.]*/
                        result = MAKE_PAYLOAD_OK;
                        keepGoing = false;
                    }
                    else
                    {
                        /*cool, the payload made it there, let's continue... */
                        PDLIST_ENTRY head = DList_RemoveHeadList(handleData->waitingToSend); /*actually this is the same as "actual", but now it is removed*/
                        DList_InsertTailList(&(handleData->eventConfirmations), head);
                        allMessagesSize += messageSize;
                    }
                    STRING_delete(temp);
                }
            }
        }

        /*closing the payload*/
        if (result == MAKE_PAYLOAD_OK)
        {
            ((char*)STRING_c_str(*payload))[STRING_length(*payload) - 1] = ']'; /*TODO - do this in STRING_HANDLE*/
        }
        else
        {
            /*no need to close anything*/
        }
    }
    return result;
}

static void reversePutListBackIn(PDLIST_ENTRY source, PDLIST_ENTRY destination)
{
    /*this function takes a list, and inserts it in another list. When done in the context of this file, it reverses the effects of a not-able-to-send situation*/
    DList_AppendTailList(destination->Flink, source);
    DList_RemoveEntryList(source);
    DList_InitializeListHead(source);
}

static void DoEvent(TRANSPORT_HANDLE handle, IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle)
{
    HTTPTRANSPORT_HANDLE_DATA* handleData = (HTTPTRANSPORT_HANDLE_DATA*)handle;
    if (DList_IsListEmpty(handleData->waitingToSend))
    {
        /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_019: [If the list is empty then IoTHubTransportHttp_DoWork shall proceed to the following action.] */
    }
    else
    {
        /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_101: [If option SetBatching is true then _Dowork shall send batched event message as specced below.] */
        if (handleData->doBatchedTransfers)
        {
            /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_102: [Request HTTP headers shall have the value of "Content-Type" created or updated to "application/vnd.microsoft.iothub.json" by a call to HTTPHeaders_ReplaceHeaderNameValuePair.] */
            if (HTTPHeaders_ReplaceHeaderNameValuePair(handleData->eventHTTPrequestHeaders, CONTENT_TYPE, APPLICATION_VND_MICROSOFT_IOTHUB_JSON) != HTTP_HEADERS_OK)
            {
                /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_103: [If updating Content-Type fails for any reason, then _DoWork shall advance to the next action.] */
                LogError("unable to HTTPHeaders_ReplaceHeaderNameValuePair\r\n");
            }
            else
            {
                /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_018: [It shall inspect the "waitingToSend" DLIST passed in config structure.] */
                STRING_HANDLE payload;
                switch (makePayload(handleData, &payload))
                {
                case MAKE_PAYLOAD_OK:
                {
                    /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_031: [Once a final payload has been obtained, IoTHubTransportHttp_DoWork shall call HTTPAPIEX_SAS_ExecuteRequest passing the following parameters:] */
                    BUFFER_HANDLE temp = BUFFER_new();
                    if (temp == NULL)
                    {
                        LogError("unable to BUFFER_new\r\n");
                        /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_073: [If there is no valid payload, IoTHubTransportHttp_DoWork shall advance to the next activity.]*/
                        reversePutListBackIn(&(handleData->eventConfirmations), handleData->waitingToSend);
                    }
                    else
                    {
                        if (BUFFER_build(temp, (const unsigned char*)STRING_c_str(payload), STRING_length(payload)) != 0)
                        {
                            LogError("unable to BUFFER_build\r\n");
                            //items go back to waitingToSend
                            /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_073: [If there is no valid payload, IoTHubTransportHttp_DoWork shall advance to the next activity.]*/
                            reversePutListBackIn(&(handleData->eventConfirmations), handleData->waitingToSend);
                        }
                        else
                        {
                            unsigned int statusCode;
                            HTTPAPIEX_RESULT r;
                            if ((r = HTTPAPIEX_SAS_ExecuteRequest(
                                handleData->sasObject,
                                handleData->httpApiExHandle,
                                HTTPAPI_REQUEST_POST,
                                STRING_c_str(handleData->eventHTTPrelativePath),
                                handleData->eventHTTPrequestHeaders,
                                temp,
                                &statusCode,
                                NULL,
                                NULL
                                )) != HTTPAPIEX_OK)
                            {
                                LogError("unable to HTTPAPIEX_ExecuteRequest\r\n");
                                //items go back to waitingToSend
                                /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_065: [if HTTPAPIEX_SAS_ExecuteRequest fails or the http status code >=300 then IoTHubTransportHttp_DoWork shall not do any other action (it is assumed at the next _DoWork it shall be retried).] */
                                reversePutListBackIn(&(handleData->eventConfirmations), handleData->waitingToSend);
                            }
                            else
                            {
                                if (statusCode < 300)
                                {
                                    /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_067: [If HTTPAPIEX_SAS_ExecuteRequest does not fail and http status code <300 then IoTHubTransportHttp_DoWork shall call IoTHubClient_LL_SendComplete. Parameter PDLIST_ENTRY completed shall point to a list containing all the items batched, and parameter IOTHUB_BATCHSTATE result shall be set to IOTHUB_BATCHSTATE_SUCESS. The batched items shall be removed from waitingToSend.] */
                                    IoTHubClient_LL_SendComplete(iotHubClientHandle, &(handleData->eventConfirmations), IOTHUB_BATCHSTATE_SUCCESS);
                                }
                                else
                                {
                                    //items go back to waitingToSend
                                    /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_065: [if HTTPAPIEX_SAS_ExecuteRequest fails or the http status code >=300 then IoTHubTransportHttp_DoWork shall not do any other action (it is assumed at the next _DoWork it shall be retried).] */
                                    LogError("unexpected HTTP status code (%u)\r\n", statusCode);
                                    reversePutListBackIn(&(handleData->eventConfirmations), handleData->waitingToSend);
                                }
                            }
                        }
                        BUFFER_delete(temp);
                    }
                    STRING_delete(payload);
                    break;
                }
                case MAKE_PAYLOAD_FIRST_ITEM_DOES_NOT_FIT:
                {
                    IoTHubClient_LL_SendComplete(iotHubClientHandle, &(handleData->eventConfirmations), IOTHUB_BATCHSTATE_FAILED); /*takes care of emptying the list too*/
                    break;
                }
                case MAKE_PAYLOAD_ERROR:
                {
                    LogError("unrecoverable errors while building a batch message\r\n");
                    break;
                }
                case MAKE_PAYLOAD_NO_ITEMS:
                {
                    /*do nothing*/
                    break;
                }
                default:
                {
                    LogError("internal error: switch's default branch reached when never intended\r\n");
                    break;
                }
                }
            }
        }
        else
        {
            const unsigned char* messageContent=NULL;
            size_t messageSize=0;
            size_t originalMessageSize=0;
            IOTHUB_MESSAGE_LIST* message = containingRecord(handleData->waitingToSend->Flink, IOTHUB_MESSAGE_LIST, entry);
            IOTHUBMESSAGE_CONTENT_TYPE contentType = IoTHubMessage_GetContentType(message->messageHandle);

            /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_122: [The message size is computed from the length of the payload + 384.]*/
            if (!(
                (((contentType == IOTHUBMESSAGE_BYTEARRAY) && 
                    (IoTHubMessage_GetByteArray(message->messageHandle, &messageContent, &originalMessageSize)==IOTHUB_MESSAGE_OK)) ? (messageSize= originalMessageSize + MAXIMUM_PAYLOAD_OVERHEAD, 1): 0)
                
                ||

                ((contentType == IOTHUBMESSAGE_STRING) && (
                    messageContent = (const unsigned char*)IoTHubMessage_GetString(message->messageHandle), 
                    (messageSize = MAXIMUM_PAYLOAD_OVERHEAD + (originalMessageSize = ((messageContent == NULL)?0:strlen((const char*)messageContent)))), 
                    messageContent!=NULL)
                    )
                ))
            {
                LogError("unable to get the message content\r\n");
                /*go on...*/
            }
            else
            {
                /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_109: [If the oldest message in waitingToSend causes the message to exceed the message size limit then it shall be removed from waitingToSend, and IoTHubClient_LL_SendComplete shall be called. Parameter PDLIST_ENTRY completed shall point to a list containing only the oldest item, and parameter IOTHUB_BATCHSTATE result shall be set to IOTHUB_BATCHSTATE_FAILED.]*/
                /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_121: [The message size shall be limited to 255KB -1 bytes.] */
                if (messageSize > MAXIMUM_MESSAGE_SIZE)
                {
                    PDLIST_ENTRY head = DList_RemoveHeadList(handleData->waitingToSend); /*actually this is the same as "actual", but now it is removed*/
                    DList_InsertTailList(&(handleData->eventConfirmations), head);
                    IoTHubClient_LL_SendComplete(iotHubClientHandle, &(handleData->eventConfirmations), IOTHUB_BATCHSTATE_FAILED); /*takes care of emptying the list too*/
                }
                else
                {
                    /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_104: [If option SetBatching is false then _Dowork shall send individual event message as specced below.] */
                    /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_105: [A clone of the event HTTP request headers shall be created.]*/
                    HTTP_HEADERS_HANDLE clonedEventHTTPrequestHeaders = HTTPHeaders_Clone(handleData->eventHTTPrequestHeaders);
                    if (clonedEventHTTPrequestHeaders == NULL)
                    {
                        /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_108: [If any HTTP header operation fails, _DoWork shall advance to the next action.] */
                        LogError("HTTPHeaders_Clone failed\r\n");
                    }
                    else
                    {
                        /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_106: [The cloned HTTP headers shall have the HTTP header "Content-Type" set to "application/octet-stream".] */
                        if (HTTPHeaders_ReplaceHeaderNameValuePair(clonedEventHTTPrequestHeaders, CONTENT_TYPE, APPLICATION_OCTET_STREAM) != HTTP_HEADERS_OK)
                        {
                            /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_108: [If any HTTP header operation fails, _DoWork shall advance to the next action.] */
                            LogError("HTTPHeaders_ReplaceHeaderNameValuePair failed\r\n");
                        }
                        else
                        {
                            /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_107: [Every message property "property":"value" shall be added to the HTTP headers as an individual header "iothub-app-property":"value".] */
                            MAP_HANDLE map = IoTHubMessage_Properties(message->messageHandle);
                            const char*const* keys;
                            const char*const* values;
                            size_t count;
                            if (Map_GetInternals(map, &keys, &values, &count) != MAP_OK)
                            {
                                /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_108: [If any HTTP header operation fails, _DoWork shall advance to the next action.] */
                                LogError("unable to Map_GetInternals\r\n");
                            }
                            else
                            {
                                size_t i;
                                bool goOn = true;
                                for (i = 0; (i < count) && goOn; i++)
                                {
                                    /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_123: [Every property name shall add  to the message size the length of the property name + the length of the property value + 16 bytes.] */
                                    messageSize += (strlen(values[i]) + strlen(keys[i]) + MAXIMUM_PROPERTY_OVERHEAD);
                                    if (messageSize > MAXIMUM_MESSAGE_SIZE)
                                    {
                                        /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_121: [The message size shall be limited to 255KB -1 bytes.] */
                                        PDLIST_ENTRY head = DList_RemoveHeadList(handleData->waitingToSend); /*actually this is the same as "actual", but now it is removed*/
                                        DList_InsertTailList(&(handleData->eventConfirmations), head);
                                        IoTHubClient_LL_SendComplete(iotHubClientHandle, &(handleData->eventConfirmations), IOTHUB_BATCHSTATE_FAILED); /*takes care of emptying the list too*/
                                        goOn = false;
                                    }
                                    else
                                    {
                                        STRING_HANDLE temp = STRING_construct(IOTHUB_APP_PREFIX);
                                        if (temp == NULL)
                                        {
                                            /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_108: [If any HTTP header operation fails, _DoWork shall advance to the next action.] */
                                            LogError("unable to STRING_construct\r\n");
                                            goOn = false;
                                        }
                                        else
                                        {
                                            if (STRING_concat(temp, keys[i]) != 0)
                                            {
                                                /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_108: [If any HTTP header operation fails, _DoWork shall advance to the next action.] */
                                                LogError("unable to STRING_concat\r\n");
                                                goOn = false;
                                            }
                                            else
                                            {
                                                if (HTTPHeaders_ReplaceHeaderNameValuePair(clonedEventHTTPrequestHeaders, STRING_c_str(temp), values[i]) != HTTP_HEADERS_OK)
                                                {
                                                    /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_108: [If any HTTP header operation fails, _DoWork shall advance to the next action.] */
                                                    LogError("unable to HTTPHeaders_ReplaceHeaderNameValuePair\r\n");
                                                    goOn = false;
                                                }
                                            }
                                            STRING_delete(temp);
                                        }
                                    }
                                }

                                // Add the Message Id and the Correlation Id
                                const char* msgId = IoTHubMessage_GetMessageId(message->messageHandle);
                                if (goOn && msgId != NULL)
                                {
                                    if (HTTPHeaders_ReplaceHeaderNameValuePair(clonedEventHTTPrequestHeaders, IOTHUB_MESSAGE_ID, msgId) != HTTP_HEADERS_OK)
                                    {
                                        LogError("unable to HTTPHeaders_ReplaceHeaderNameValuePair\r\n");
                                        goOn = false;
                                    }
                                }

                                const char* corrId = IoTHubMessage_GetCorrelationId(message->messageHandle);
                                if (goOn && corrId != NULL)
                                {
                                    if (HTTPHeaders_ReplaceHeaderNameValuePair(clonedEventHTTPrequestHeaders, IOTHUB_CORRELATION_ID, corrId) != HTTP_HEADERS_OK)
                                    {
                                        LogError("unable to HTTPHeaders_ReplaceHeaderNameValuePair\r\n");
                                        goOn = false;
                                    }
                                }

                                if (!goOn)
                                {
                                    /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_108: [If any HTTP header operation fails, _DoWork shall advance to the next action.] */
                                }
                                else
                                {
                                    /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_110: [IoTHubTransportHttp_DoWork shall call HTTPAPIEX_SAS_ExecuteRequest passing the following parameters] */
                                    BUFFER_HANDLE toBeSend = BUFFER_new();
                                    if (toBeSend == NULL)
                                    {
                                        LogError("unable to BUFFER_new\r\n");
                                    }
                                    else
                                    {
                                        if (BUFFER_build(toBeSend, messageContent, originalMessageSize) != 0)
                                        {
                                            LogError("unable to BUFFER_build\r\n");
                                        }
                                        else
                                        {
                                            unsigned int statusCode;
                                            HTTPAPIEX_RESULT r;
                                            if ((r = HTTPAPIEX_SAS_ExecuteRequest(
                                                handleData->sasObject,
                                                handleData->httpApiExHandle,
                                                HTTPAPI_REQUEST_POST,
                                                STRING_c_str(handleData->eventHTTPrelativePath),
                                                clonedEventHTTPrequestHeaders,
                                                toBeSend,
                                                &statusCode,
                                                NULL,
                                                NULL
                                                )) != HTTPAPIEX_OK)
                                            {
                                                LogError("unable to HTTPAPIEX_ExecuteRequest\r\n");
                                            }
                                            else
                                            {
                                                if (statusCode < 300)
                                                {
                                                    /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_112: [If HTTPAPIEX_SAS_ExecuteRequest does not fail and http status code <300 then IoTHubTransportHttp_DoWork shall call IoTHubClient_LL_SendComplete. Parameter PDLIST_ENTRY completed shall point to a list the item send, and parameter IOTHUB_BATCHSTATE result shall be set to IOTHUB_BATCHSTATE_SUCCESS. The item shall be removed from waitingToSend.] */
                                                    PDLIST_ENTRY justSent = DList_RemoveHeadList(handleData->waitingToSend); /*actually this is the same as "actual", but now it is removed*/
                                                    DList_InsertTailList(&(handleData->eventConfirmations), justSent);
                                                    IoTHubClient_LL_SendComplete(iotHubClientHandle, &(handleData->eventConfirmations), IOTHUB_BATCHSTATE_SUCCESS); /*takes care of emptying the list too*/
                                                }
                                                else
                                                {
                                                    /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_111: [If HTTPAPIEX_SAS_ExecuteRequest fails or the http status code >=300 then IoTHubTransportHttp_DoWork shall not do any other action (it is assumed at the next _DoWork it shall be retried).] */
                                                    LogError("unexpected HTTP status code (%u)\r\n", statusCode);
                                                }
                                            }
                                        }
                                        BUFFER_delete(toBeSend);
                                    }
                                }
                            }
                        }
                        HTTPHeaders_Free(clonedEventHTTPrequestHeaders);
                    }
                }
            }
        }
    }
}

#define ACTION_VALUES \
    ABANDON, \
    REJECT, \
    ACCEPT
DEFINE_ENUM(ACTION, ACTION_VALUES);

static void abandonOrAcceptMessage(HTTPTRANSPORT_HANDLE_DATA* handleData, const char* ETag, ACTION action)
{
    /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_050: [_DoWork shall call HTTPAPIEX_SAS_ExecuteRequest with the following parameters:
-requestType: POST
-relativePath: abandon relative path begin (as created by _Create) + value of ETag + "/abandon?api-version=2015-08-15-preview" 
- requestHttpHeadersHandle: an HTTP headers instance containing the following
    Authorization: " "
    If-Match: value of ETag
- requestContent: NULL
- statusCode: a pointer to unsigned int which might be examined for logging
- responseHeadearsHandle: NULL
- responseContent: NULL]*/
    /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_051: [_DoWork shall call HTTPAPIEX_SAS_ExecuteRequest with the following parameters:
-requestType: DELETE
-relativePath: abandon relative path begin + value of ETag + "?api-version=2015-08-15-preview" 
- requestHttpHeadersHandle: an HTTP headers instance containing the following
    Authorization: " "
    If-Match: value of ETag
- requestContent: NULL
- statusCode: a pointer to unsigned int which might be used by logging
- responseHeadearsHandle: NULL
- responseContent: NULL]*/
    /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_077: [_DoWork shall call HTTPAPIEX_SAS_ExecuteRequest with the following parameters:
-requestType: DELETE
-relativePath: abandon relative path begin + value of ETag +"?api-version=2015-08-15-preview" + "&reject"
- requestHttpHeadersHandle: an HTTP headers instance containing the following
    Authorization: " "
    If-Match: value of ETag
- requestContent: NULL
- statusCode: a pointer to unsigned int which might be used by logging
- responseHeadearsHandle: NULL
- responseContent: NULL]*/
    
    STRING_HANDLE fullAbandonRelativePath = STRING_clone(handleData->abandonHTTPrelativePathBegin);
    if (fullAbandonRelativePath == NULL)
    {
        /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_052: [Abandoning the message is considered successful if the HTTPAPIEX_SAS_ExecuteRequest doesn't fail and the statusCode is 204.]*/
        /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_054: [Accepting a message is successful when HTTPAPIEX_SAS_ExecuteRequest completes successfully and the status code is 204.] */
        /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_078: [Rejecting a message is successful when HTTPAPIEX_SAS_ExecuteRequest completes successfully and the status code is 204.] */
        LogError("unable to STRING_clone\r\n");
    }
    else
    {
        STRING_HANDLE ETagUnquoted = STRING_construct_n(ETag + 1, strlen(ETag) - 2); /*skip first character which is '"' and the last one (which is also '"')*/
        if (ETagUnquoted == NULL)
        {
            /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_052: [Abandoning the message is considered successful if the HTTPAPIEX_SAS_ExecuteRequest doesn't fail and the statusCode is 204.]*/
            /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_054: [Accepting a message is successful when HTTPAPIEX_SAS_ExecuteRequest completes successfully and the status code is 204.] */
            /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_078: [Rejecting a message is successful when HTTPAPIEX_SAS_ExecuteRequest completes successfully and the status code is 204.] */
            LogError("unable to STRING_construct_n\r\n");
        }
        else
        {
            if (!(
                (STRING_concat_with_STRING(fullAbandonRelativePath, ETagUnquoted) == 0) &&
                (STRING_concat(fullAbandonRelativePath, (action == ABANDON) ? "/abandon" API_VERSION : ((action == REJECT) ? API_VERSION "&reject" : API_VERSION)) == 0)
                ))
            {
                /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_052: [Abandoning the message is considered successful if the HTTPAPIEX_SAS_ExecuteRequest doesn't fail and the statusCode is 204.]*/
                /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_054: [Accepting a message is successful when HTTPAPIEX_SAS_ExecuteRequest completes successfully and the status code is 204.] */
                /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_078: [Rejecting a message is successful when HTTPAPIEX_SAS_ExecuteRequest completes successfully and the status code is 204.] */
                LogError("unable to STRING_concat\r\n");
            }
            else
            {
                HTTP_HEADERS_HANDLE abandonRequestHttpHeaders = HTTPHeaders_Alloc();
                if (abandonRequestHttpHeaders == NULL)
                {
                    /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_052: [Abandoning the message is considered successful if the HTTPAPIEX_SAS_ExecuteRequest doesn't fail and the statusCode is 204.]*/
                    /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_054: [Accepting a message is successful when HTTPAPIEX_SAS_ExecuteRequest completes successfully and the status code is 204.] */
                    /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_078: [Rejecting a message is successful when HTTPAPIEX_SAS_ExecuteRequest completes successfully and the status code is 204.] */
                    LogError("unable to HTTPHeaders_Alloc\r\n");
                }
                else
                {
                    if (!(
                        (HTTPHeaders_AddHeaderNameValuePair(abandonRequestHttpHeaders, "Authorization", " ") == HTTP_HEADERS_OK) &&
                        (HTTPHeaders_AddHeaderNameValuePair(abandonRequestHttpHeaders, "If-Match", ETag) == HTTP_HEADERS_OK)
                        ))
                    {
                        /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_052: [Abandoning the message is considered successful if the HTTPAPIEX_SAS_ExecuteRequest doesn't fail and the statusCode is 204.]*/
                        /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_054: [Accepting a message is successful when HTTPAPIEX_SAS_ExecuteRequest completes successfully and the status code is 204.] */
                        /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_078: [Rejecting a message is successful when HTTPAPIEX_SAS_ExecuteRequest completes successfully and the status code is 204.] */
                        LogError("unable to HTTPHeaders_AddHeaderNameValuePair\r\n");
                    }
                    else
                    {
                        unsigned int statusCode;
                        if (HTTPAPIEX_SAS_ExecuteRequest(
                            handleData->sasObject,
                            handleData->httpApiExHandle,
                            (action == ABANDON) ? HTTPAPI_REQUEST_POST : HTTPAPI_REQUEST_DELETE,                               /*-requestType: POST                                                                                                       */
                            STRING_c_str(fullAbandonRelativePath),              /*-relativePath: abandon relative path begin (as created by _Create) + value of ETag + "/abandon?api-version=2015-08-15-preview"   */
                            abandonRequestHttpHeaders,                          /*- requestHttpHeadersHandle: an HTTP headers instance containing the following                                            */
                            NULL,                                               /*- requestContent: NULL                                                                                                   */
                            &statusCode,                                         /*- statusCode: a pointer to unsigned int which might be examined for logging                                              */
                            NULL,                                               /*- responseHeadearsHandle: NULL                                                                                           */
                            NULL                                                /*- responseContent: NULL]                                                                                                 */
                            ) != HTTPAPIEX_OK)
                        {
                            /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_052: [Abandoning the message is considered successful if the HTTPAPIEX_SAS_ExecuteRequest doesn't fail and the statusCode is 204.]*/
                            /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_054: [Accepting a message is successful when HTTPAPIEX_SAS_ExecuteRequest completes successfully and the status code is 204.] */
                            /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_078: [Rejecting a message is successful when HTTPAPIEX_SAS_ExecuteRequest completes successfully and the status code is 204.] */
                            LogError("unable to HTTPAPIEX_ExecuteRequest\r\n");
                        }
                        else
                        {
                            if (statusCode != 204)
                            {
                                /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_052: [Abandoning the message is considered successful if the HTTPAPIEX_SAS_ExecuteRequest doesn't fail and the statusCode is 204.]*/
                                /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_054: [Accepting a message is successful when HTTPAPIEX_SAS_ExecuteRequest completes successfully and the status code is 204.] */
                                /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_078: [Rejecting a message is successful when HTTPAPIEX_SAS_ExecuteRequest completes successfully and the status code is 204.] */
                                LogError("unexpected status code returned %u (was expecting 204)\r\n", statusCode);
                            }
                            else
                            {
                                /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_052: [Abandoning the message is considered successful if the HTTPAPIEX_SAS_ExecuteRequest doesn't fail and the statusCode is 204.]*/
                                /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_054: [Accepting a message is successful when HTTPAPIEX_SAS_ExecuteRequest completes successfully and the status code is 204.] */
                                /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_078: [Rejecting a message is successful when HTTPAPIEX_SAS_ExecuteRequest completes successfully and the status code is 204.] */
                                /*all is fine*/
                            }
                        }
                    }
                    HTTPHeaders_Free(abandonRequestHttpHeaders);
                }
            }
            STRING_delete(ETagUnquoted);
        }
        STRING_delete(fullAbandonRelativePath);
    }
}

static void DoMessages(TRANSPORT_HANDLE handle, IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle)
{
    HTTPTRANSPORT_HANDLE_DATA* handleData = (HTTPTRANSPORT_HANDLE_DATA*)handle;
    /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_057: [If flag DoWork_PullMessage is set to false then _DoWork shall advance to the next action.] */
    if (handleData->DoWork_PullMessage)
    {
        /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_116: [After client creation, the first GET shall be allowed no matter what the value of GetMinimumPollingTime.] */
        /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_117: [If time is not available then all calls shall be treated as if they are the first one.] */
        /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_115: [A GET request that happens earlier than GetMinimumPollingTime shall be ignored.] */
        time_t timeNow = get_time(NULL);
        bool isPollingAllowed = handleData->isFirstPoll || (timeNow == (time_t)(-1)) || (get_difftime(timeNow, handleData->lastPollTime) > handleData->getMinimumPollingTime);
        if (isPollingAllowed)
        {
        HTTP_HEADERS_HANDLE responseHTTPHeaders = HTTPHeaders_Alloc();
        if (responseHTTPHeaders == NULL)
        {
            /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_037: [If the call to HTTPAPIEX_SAS_ExecuteRequest did not executed successfully or building any part of the prerequisites of the call fails, then _DoWork shall advance to the next action in this description.] */
            LogError("unable to HTTPHeaders_Alloc\r\n");
        }
        else
        {
            BUFFER_HANDLE responseContent = BUFFER_new();
            if (responseContent == NULL)
            {
                /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_037: [If the call to HTTPAPIEX_SAS_ExecuteRequest did not executed successfully or building any part of the prerequisites of the call fails, then _DoWork shall advance to the next action in this description.] */
                LogError("unable to BUFFER_new\r\n");
            }
            else
            {
                unsigned int statusCode;
                /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_036: [Otherwise, IoTHubTransportHttp_DoWork shall call HTTPAPIEX_SAS_ExecuteRequest passing the following parameters
requestType: GET
relativePath: the message HTTP relative path
requestHttpHeadersHandle: message HTTP request headers created by _Create
requestContent: NULL
statusCode: a pointer to unsigned int which shall be later examined
responseHeadearsHandle: a new instance of HTTP headers
responseContent: a new instance of buffer] 
*/
                if (HTTPAPIEX_SAS_ExecuteRequest(
                    handleData->sasObject,
                    handleData->httpApiExHandle,     
                    HTTPAPI_REQUEST_GET,                                            /*requestType: GET*/
                    STRING_c_str(handleData->messageHTTPrelativePath),         /*relativePath: the message HTTP relative path*/
                    handleData->messageHTTPrequestHeaders,                     /*requestHttpHeadersHandle: message HTTP request headers created by _Create*/
                    NULL,                                                           /*requestContent: NULL*/
                    &statusCode,                                                    /*statusCode: a pointer to unsigned int which shall be later examined*/
                    responseHTTPHeaders,                                            /*responseHeadearsHandle: a new instance of HTTP headers*/
                    responseContent                                                 /*responseContent: a new instance of buffer*/
                    ) 
                    != HTTPAPIEX_OK)
                {
                    /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_037: [If the call to HTTPAPIEX_SAS_ExecuteRequest did not executed successfully or building any part of the prerequisites of the call fails, then _DoWork shall advance to the next action in this description.] */
                    LogError("unable to HTTPAPIEX_ExecuteRequest\r\n");
                }
                else
                { 
                        /*HTTP dialogue was succesfull*/
                        if (timeNow == (time_t)(-1))
                        {
                            handleData->isFirstPoll = true;
                        }
                        else
                        {
                            handleData->isFirstPoll = false;
                            handleData->lastPollTime = timeNow;
                        }
                    if (statusCode == 204)
                    {
                        /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_038: [If the HTTPAPIEX_SAS_ExecuteRequest executed successfully then status code shall be examined. Any status code different than 200 causes _DoWork to advance to the next action.] */
                        /*this is an expected status code, means "no commands", but logging that creates panic*/

                        /*do nothing, advance to next action*/
                    }
                    else if (statusCode != 200)
                    {
                        /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_038: [If the HTTPAPIEX_SAS_ExecuteRequest executed successfully then status code shall be examined. Any status code different than 200 causes _DoWork to advance to the next action.] */
                        LogError("expected status code was 200, but actually was received %u... moving on\r\n", statusCode);
                    }
                    else
                    {
                        /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_039: [If status code is 200, then _DoWork shall make a copy of the value of the "ETag" http header.]*/
                        const char* etagValue = HTTPHeaders_FindHeaderValue(responseHTTPHeaders, "ETag");
                        if (etagValue == NULL)
                        {
                            LogError("unable to find a received header called \"E-Tag\"\r\n");
                        }
                        else
                        {
                            /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_040: [If no such header is found or is invalid, then _DoWork shall advance to the next action.]*/
                            size_t etagsize = strlen(etagValue);
                            if (
                                (etagsize < 2) ||
                                (etagValue[0] != '"') ||
                                (etagValue[etagsize - 1] != '"')
                                )
                            {
                                LogError("ETag is not a valid quoted string\r\n");
                            }
                            else
                            {
                                /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_041: [_DoWork shall assemble an IOTHUBMESSAGE_HANDLE from the received HTTP content (using the responseContent buffer).] */
                                IOTHUB_MESSAGE_HANDLE receivedMessage = IoTHubMessage_CreateFromByteArray(BUFFER_u_char(responseContent), BUFFER_length(responseContent));
                                if (receivedMessage == NULL)
                                {
                                    /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_042: [If assembling the message fails in any way, then _DoWork shall "abandon" the message.]*/
                                    LogError("unable to IoTHubMessage_CreateFromByteArray, trying to abandon the message... \r\n");
                                    abandonOrAcceptMessage(handle, etagValue, ABANDON);
                                }
                                else
                                {
                                    /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_087: [All the HTTP headers of the form iothub-app-name:somecontent shall be transformed in message properties {name, somecontent}.]*/
                                    /*Codes_SRS_IOTHUBTRANSPORTTHTTP_07_008: [The HTTP header of iothub-messageid shall be set in the MessageId.]*/
                                    size_t nHeaders;
                                    if (HTTPHeaders_GetHeaderCount(responseHTTPHeaders, &nHeaders) != HTTP_HEADERS_OK)
                                    {
                                        LogError("unable to get the count of HTTP headers\r\n");
                                        abandonOrAcceptMessage(handle, etagValue, ABANDON);
                                    }
                                    else
                                    {
                                        size_t i;
                                        MAP_HANDLE properties = (nHeaders > 0) ? IoTHubMessage_Properties(receivedMessage) : NULL;
                                        for (i = 0; i < nHeaders; i++)
                                        {
                                            char* completeHeader;
                                            if (HTTPHeaders_GetHeader(responseHTTPHeaders, i, &completeHeader) != HTTP_HEADERS_OK)
                                            {
                                                break;
                                            }
                                            else
                                            {
                                                if (strncmp(IOTHUB_APP_PREFIX, completeHeader, strlen(IOTHUB_APP_PREFIX)) == 0)
                                                {
                                                    /*looks like a property headers*/
                                                    /*there's a guaranteed ':' in the completeHeader, by HTTP_HEADERS module*/
                                                    char* whereIsColon = strchr(completeHeader, ':');
                                                    if (whereIsColon != NULL)
                                                    {
                                                        *whereIsColon = '\0'; /*cut it down*/
                                                        if (Map_AddOrUpdate(properties, completeHeader + strlen(IOTHUB_APP_PREFIX), whereIsColon + 2) != MAP_OK) /*whereIsColon+1 is a space because HTTPEHADERS outputs a ": " between name and value*/
                                                        {
                                                            free(completeHeader);
                                                            break;
                                                        }
                                                    }
                                                }
                                                else if (strncmp(IOTHUB_MESSAGE_ID, completeHeader, strlen(IOTHUB_MESSAGE_ID)) == 0)
                                                {
                                                    char* whereIsColon = strchr(completeHeader, ':');
                                                    if (whereIsColon != NULL)
                                                    {
                                                        *whereIsColon = '\0'; /*cut it down*/
                                                        if (IoTHubMessage_SetMessageId(receivedMessage, whereIsColon + 2) != IOTHUB_MESSAGE_OK)
                                                        {
                                                            free(completeHeader);
                                                            break;
                                                        }
                                                    }
                                                }
                                                else if (strncmp(IOTHUB_CORRELATION_ID, completeHeader, strlen(IOTHUB_CORRELATION_ID)) == 0)
                                                {
                                                    char* whereIsColon = strchr(completeHeader, ':');
                                                    if (whereIsColon != NULL)
                                                    {
                                                        *whereIsColon = '\0'; /*cut it down*/
                                                        if (IoTHubMessage_SetCorrelationId(receivedMessage, whereIsColon + 2) != IOTHUB_MESSAGE_OK)
                                                        {
                                                            free(completeHeader);
                                                            break;
                                                        }
                                                    }
                                                }
                                                free(completeHeader);
                                            }
                                        }

                                        if (i < nHeaders)
                                        {
                                            abandonOrAcceptMessage(handle, etagValue, ABANDON);
                                        }
                                        else
                                        {
                                            /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_043: [Otherwise, _DoWork shall call IoTHubClient_LL_MessageCallback with parameters handle = iotHubClientHandle and message = newly created message.]*/
                                            IOTHUBMESSAGE_DISPOSITION_RESULT messageResult = IoTHubClient_LL_MessageCallback(iotHubClientHandle, receivedMessage);
                                            if (messageResult == IOTHUBMESSAGE_ACCEPTED)
                                            {
                                                /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_044: [If IoTHubClient_LL_MessageCallback returns IOTHUBMESSAGE_ACCEPTED then _DoWork shall "accept" the message.]*/
                                                abandonOrAcceptMessage(handle, etagValue, ACCEPT);
                                            }
                                            else if (messageResult == IOTHUBMESSAGE_REJECTED)
                                            {
                                                /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_074: [If IoTHubClient_LL_MessageCallback returns IOTHUBMESSAGE_REJECTED then _DoWork shall "reject" the message.]*/
                                                abandonOrAcceptMessage(handle, etagValue, REJECT);
                                            }
                                            else
                                            {
                                                /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_079: [If IoTHubClient_LL_MessageCallback returns IOTHUBMESSAGE_ABANDONED then _DoWork shall "abandon" the message.] */
                                                abandonOrAcceptMessage(handle, etagValue, ABANDON);
                                            }
                                        }
                                    }
                                    IoTHubMessage_Destroy(receivedMessage);
                                }
                            }
                            
                        }
                    }
                }
                BUFFER_delete(responseContent);
            }
            HTTPHeaders_Free(responseHTTPHeaders);
        }
    }
        else
        {
            /*isPollingAllowed is false... */
            /*do nothing "shall be ignored*/
}
    }
}

void IoTHubTransportHttp_DoWork(TRANSPORT_HANDLE handle, IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle)
{
    /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_017: [If parameter handle is NULL or parameter iotHubClientHandle then IoTHubTransportHttp_DoWork shall immeditely return.]*/
    if ((handle != NULL) && (iotHubClientHandle != NULL))
    {
        DoEvent(handle, iotHubClientHandle);
        DoMessages(handle, iotHubClientHandle);
    }
}

IOTHUB_CLIENT_RESULT IoTHubTransportHttp_GetSendStatus(TRANSPORT_HANDLE handle, IOTHUB_CLIENT_STATUS *iotHubClientStatus)
{
    IOTHUB_CLIENT_RESULT result;

    /* Codes_SRS_IOTHUBTRANSPORTTHTTP_09_001: [IoTHubTransportHttp_GetSendStatus shall return IOTHUB_CLIENT_INVALID_ARG if called with NULL parameter] */
    if (handle == NULL)
    {
        result = IOTHUB_CLIENT_INVALID_ARG;
        LogError("Invalid handle to IoTHubClient HTTP transport instance.\r\n");
    }
    else if (iotHubClientStatus == NULL)
    {
        result = IOTHUB_CLIENT_INVALID_ARG;
        LogError("Invalid pointer to output parameter IOTHUB_CLIENT_STATUS.\r\n");
    }
    else
    {
        HTTPTRANSPORT_HANDLE_DATA* handleData = (HTTPTRANSPORT_HANDLE_DATA*)handle;

        /* Codes_SRS_IOTHUBTRANSPORTTHTTP_09_002: [IoTHubTransportHttp_GetSendStatus shall return IOTHUB_CLIENT_OK and status IOTHUB_CLIENT_SEND_STATUS_IDLE if there are currently no event items to be sent or being sent] */
        if (!DList_IsListEmpty(handleData->waitingToSend))
        {
            *iotHubClientStatus = IOTHUB_CLIENT_SEND_STATUS_BUSY;
        }
        /* Codes_SRS_IOTHUBTRANSPORTTHTTP_09_003: [IoTHubTransportHttp_GetSendStatus shall return IOTHUB_CLIENT_OK and status IOTHUB_CLIENT_SEND_STATUS_BUSY if there are currently event items to be sent or being sent] */
        else
        {
            *iotHubClientStatus = IOTHUB_CLIENT_SEND_STATUS_IDLE;
        }

        result = IOTHUB_CLIENT_OK;
    }

    return result;
}

IOTHUB_CLIENT_RESULT IoTHubTransportHttp_SetOption(TRANSPORT_HANDLE handle, const char* option, const void* value)
{
    IOTHUB_CLIENT_RESULT result;
    /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_080: [If handle parameter is NULL then IoTHubTransportHttp_SetOption shall return IOTHUB_CLIENT_INVALID_ARG.] */
    /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_081: [If option parameter is NULL then IoTHubTransportHttp_SetOption shall return IOTHUB_CLIENT_INVALID_ARG.] */
    /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_082: [If value parameter is NULL then IoTHubTransportHttp_SetOption shall return IOTHUB_CLIENT_INVALID_ARG.] */
    if (
        (handle == NULL) ||
        (option == NULL) ||
        (value == NULL)
        )
    { 
        result = IOTHUB_CLIENT_INVALID_ARG;
        LogError("invalid parameter (NULL) passed to IoTHubTransportHttp_SetOption\r\n");
    }
    else
    {
        HTTPTRANSPORT_HANDLE_DATA* handleData = (HTTPTRANSPORT_HANDLE_DATA*)handle;
        /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_097: ["Batching"] */
        if (strcmp("Batching", option) == 0)
        {
            /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_083: [If optionName is an option handled by IoTHubTransportHttp then it shall be set.] */
            handleData->doBatchedTransfers = *(bool*)value;
            result = IOTHUB_CLIENT_OK;
        }
        /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_114: ["MinimumPollingTime"] */
        else if (strcmp("MinimumPollingTime", option) == 0)
        {
            handleData->getMinimumPollingTime = *(unsigned int*)value;
            result = IOTHUB_CLIENT_OK;
        }
        else
        {
            /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_084: [Otherwise, IoTHubTransport_Http shall call HTTPAPIEX_SetOption with the same parameters and return the translated code.] */
            HTTPAPIEX_RESULT HTTPAPIEX_result = HTTPAPIEX_SetOption(handleData->httpApiExHandle, option, value);
            /*Codes_SRS_IOTHUBTRANSPORTTHTTP_02_086: [The following table translates HTTPAPIEX return codes to IOTHUB_CLIENT_RESULT return codes:] */
            if (HTTPAPIEX_result == HTTPAPIEX_OK)
            {
                result = IOTHUB_CLIENT_OK;
            }
            else if (HTTPAPIEX_result == HTTPAPIEX_INVALID_ARG)
            {
                result = IOTHUB_CLIENT_INVALID_ARG;
                LogError("HTTPAPIEX_SetOption failed\r\n");
            }
            else
            {
                result = IOTHUB_CLIENT_ERROR;
                LogError("HTTPAPIEX_SetOption failed\r\n");
            }
        }
    }
    return result;
}
