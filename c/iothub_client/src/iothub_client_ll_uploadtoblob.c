// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifdef DONT_USE_UPLOADTOBLOB
#error "trying to compile iothub_client_ll_uploadtoblob.c while the symbol DONT_USE_UPLOADTOBLOB is #define'd"
#else

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#include <string.h>
#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/string_tokenizer.h"
#include "azure_c_shared_utility/doublylinkedlist.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/tickcounter.h"
#include "azure_c_shared_utility/httpapiexsas.h"

#include "iothub_client_ll.h"
#include "iothub_client_options.h"
#include "iothub_client_private.h"
#include "iothub_client_version.h"
#include "iothub_transport_ll.h"
#include "parson.h"
#include "iothub_client_ll_uploadtoblob.h"
#include "blob.h"


#ifdef WINCE
#include <stdarg.h>
// Returns number of characters copied.
int snprintf(char * s, size_t n, const char * format, ...)
{
    int result;
    va_list args;
    va_start(args, format);
    result = vsnprintf(s, n, format, args);
    va_end(args);
    return result;
}
#endif


/*Codes_SRS_IOTHUBCLIENT_LL_02_085: [ IoTHubClient_LL_UploadToBlob shall use the same authorization as step 1. to prepare and perform a HTTP request with the following parameters: ]*/
#define FILE_UPLOAD_FAILED_BODY "{ \"isSuccess\":false, \"statusCode\":-1,\"statusDescription\" : \"client not able to connect with the server\" }"

#define AUTHORIZATION_SCHEME_VALUES \
    DEVICE_KEY, \
    X509,       \
    SAS_TOKEN
DEFINE_ENUM(AUTHORIZATION_SCHEME, AUTHORIZATION_SCHEME_VALUES);

typedef struct UPLOADTOBLOB_X509_CREDENTIALS_TAG
{
    const char* x509certificate;
    const char* x509privatekey;
}UPLOADTOBLOB_X509_CREDENTIALS;

typedef struct IOTHUB_CLIENT_LL_UPLOADTOBLOB_HANDLE_DATA_TAG
{
    STRING_HANDLE deviceId;                     /*needed for file upload*/
    const char* hostname;                       /*needed for file upload*/
    AUTHORIZATION_SCHEME authorizationScheme;   /*needed for file upload*/
    union {
        STRING_HANDLE deviceKey;    /*used when authorizationScheme is DEVICE_KEY*/
        STRING_HANDLE sas;          /*used when authorizationScheme is SAS_TOKEN*/
        UPLOADTOBLOB_X509_CREDENTIALS x509credentials; /*assumed to be used when both deviceKey and deviceSasToken are NULL*/
    } credentials;                              /*needed for file upload*/
}IOTHUB_CLIENT_LL_UPLOADTOBLOB_HANDLE_DATA;

IOTHUB_CLIENT_LL_UPLOADTOBLOB_HANDLE IoTHubClient_LL_UploadToBlob_Create(const IOTHUB_CLIENT_CONFIG* config)
{
    IOTHUB_CLIENT_LL_UPLOADTOBLOB_HANDLE_DATA* handleData = malloc(sizeof(IOTHUB_CLIENT_LL_UPLOADTOBLOB_HANDLE_DATA));
    if (handleData == NULL)
    {
        LogError("oom - malloc");
        /*return as is*/
    }
    else
    {
        size_t iotHubNameLength = strlen(config->iotHubName);
        size_t iotHubSuffixLength = strlen(config->iotHubSuffix);
        handleData->deviceId = STRING_construct(config->deviceId);
        if (handleData->deviceId == NULL)
        {
            LogError("unable to STRING_construct");
            free(handleData);
            handleData = NULL;
        }
        else
        {
            handleData->hostname = malloc(iotHubNameLength + 1 + iotHubSuffixLength + 1); /*first +1 is because "." the second +1 is because \0*/
            if (handleData->hostname == NULL)
            {
                LogError("malloc failed");
                STRING_delete(handleData->deviceId);
                free(handleData);
                handleData = NULL;
            }
            else
            {
                memcpy((char*)handleData->hostname, config->iotHubName, iotHubNameLength);
                ((char*)handleData->hostname)[iotHubNameLength] = '.';
                memcpy((char*)handleData->hostname + iotHubNameLength + 1, config->iotHubSuffix, iotHubSuffixLength + 1); /*+1 will copy the \0 too*/
                if ((config->deviceSasToken != NULL) && (config->deviceKey == NULL))
                {
                    handleData->authorizationScheme = SAS_TOKEN;
                    handleData->credentials.sas = STRING_construct(config->deviceSasToken);
                    if (handleData->credentials.sas == NULL)
                    {
                        LogError("unable to STRING_construct");
                        free((void*)handleData->hostname);
                        STRING_delete(handleData->deviceId);
                        free(handleData);
                        handleData = NULL;
                    }
                    else
                    {
                        /*return as is*/
                    }
                }
                else if ((config->deviceSasToken == NULL) && (config->deviceKey != NULL))
                {
                    handleData->authorizationScheme = DEVICE_KEY;
                    handleData->credentials.deviceKey = STRING_construct(config->deviceKey);
                    if (handleData->credentials.deviceKey == NULL)
                    {
                        LogError("unable to STRING_construct");
                        free((void*)handleData->hostname);
                        STRING_delete(handleData->deviceId);
                        free(handleData);
                        handleData = NULL;
                    }
                    else
                    {
                        /*return as is*/
                    }
                }
                else if ((config->deviceSasToken == NULL) && (config->deviceKey == NULL))
                {
                    handleData->authorizationScheme = X509;
                    handleData->credentials.x509credentials.x509certificate = NULL;
                    handleData->credentials.x509credentials.x509privatekey = NULL;
                    /*return as is*/
                }
            }
        }
    }
    return (IOTHUB_CLIENT_LL_UPLOADTOBLOB_HANDLE)handleData;
    
}

/*returns 0 when correlationId, sasUri contain data*/
static int IoTHubClient_LL_UploadToBlob_step1and2(IOTHUB_CLIENT_LL_UPLOADTOBLOB_HANDLE_DATA* handleData, HTTPAPIEX_HANDLE iotHubHttpApiExHandle, HTTP_HEADERS_HANDLE requestHttpHeaders, const char* destinationFileName,
    STRING_HANDLE correlationId, STRING_HANDLE sasUri)
{
    int result;

    /*Codes_SRS_IOTHUBCLIENT_LL_02_066: [ IoTHubClient_LL_UploadToBlob shall create an HTTP relative path formed from "/devices/" + deviceId + "/files/" + destinationFileName + "?api-version=API_VERSION". ]*/
    STRING_HANDLE relativePath = STRING_construct("/devices/");
    if (relativePath == NULL)
    {
        /*Codes_SRS_IOTHUBCLIENT_LL_02_067: [ If creating the relativePath fails then IoTHubClient_LL_UploadToBlob shall fail and return IOTHUB_CLIENT_ERROR. ]*/
        LogError("unable to STRING_construct");
        result = __LINE__;
    }
    else
    {
        if (!(
            (STRING_concat_with_STRING(relativePath, handleData->deviceId) == 0) &&
            (STRING_concat(relativePath, "/files/") == 0) &&
            (STRING_concat(relativePath, destinationFileName) == 0) &&
            (STRING_concat(relativePath, API_VERSION) == 0)
            ))
        {
            /*Codes_SRS_IOTHUBCLIENT_LL_02_067: [ If creating the relativePath fails then IoTHubClient_LL_UploadToBlob shall fail and return IOTHUB_CLIENT_ERROR. ]*/
            LogError("unable to concatenate STRING");
            result = __LINE__;
        }
        else
        {
            /*Codes_SRS_IOTHUBCLIENT_LL_02_068: [ IoTHubClient_LL_UploadToBlob shall create an HTTP responseContent BUFFER_HANDLE. ]*/
            BUFFER_HANDLE responseContent = BUFFER_new();
            if (responseContent == NULL)
            {
                /*Codes_SRS_IOTHUBCLIENT_LL_02_069: [ If creating the HTTP response buffer handle fails then IoTHubClient_LL_UploadToBlob shall fail and return IOTHUB_CLIENT_ERROR. ]*/
                result = __LINE__;
                LogError("unable to BUFFER_new");
            }
            else
            {
                /*Codes_SRS_IOTHUBCLIENT_LL_02_072: [ IoTHubClient_LL_UploadToBlob shall add the following name:value to request HTTP headers: ] "Content-Type": "application/json" "Accept": "application/json" "User-Agent": "iothubclient/" IOTHUB_SDK_VERSION*/
                /*Codes_SRS_IOTHUBCLIENT_LL_02_107: [ - "Authorization" header shall not be build. ]*/
                if (!(
                    (HTTPHeaders_AddHeaderNameValuePair(requestHttpHeaders, "Content-Type", "application/json") == HTTP_HEADERS_OK) &&
                    (HTTPHeaders_AddHeaderNameValuePair(requestHttpHeaders, "Accept", "application/json") == HTTP_HEADERS_OK) &&
                    (HTTPHeaders_AddHeaderNameValuePair(requestHttpHeaders, "User-Agent", "iothubclient/" IOTHUB_SDK_VERSION) == HTTP_HEADERS_OK) &&
                    (handleData->authorizationScheme==X509 || (HTTPHeaders_AddHeaderNameValuePair(requestHttpHeaders, "Authorization", "") == HTTP_HEADERS_OK))
                    ))
                {
                    /*Codes_SRS_IOTHUBCLIENT_LL_02_071: [ If creating the HTTP headers fails then IoTHubClient_LL_UploadToBlob shall fail and return IOTHUB_CLIENT_ERROR. ]*/
                    LogError("unable to HTTPHeaders_AddHeaderNameValuePair");
                    result = __LINE__;
                }
                else
                {
                    int wasIoTHubRequestSuccess = 0; /*!=0 means responseContent has a buffer that should be parsed by parson after executing the below switch*/
                    /* set the result to error by default */
                    result = __LINE__;
                    switch (handleData->authorizationScheme)
                    {
                    default:
                    {
                        /*wasIoTHubRequestSuccess takes care of the return value*/
                        LogError("Internal Error: unexpected value in handleData->authorizationScheme = %d", handleData->authorizationScheme);
                        result = __LINE__;
                        break;
                    }
                    case(X509):
                    {
                        unsigned int statusCode;
                        /*Codes_SRS_IOTHUBCLIENT_LL_02_108: [ IoTHubClient_LL_UploadToBlob shall execute HTTPAPIEX_ExecuteRequest passing the following information for arguments: ]*/
                        if (HTTPAPIEX_ExecuteRequest(
                            iotHubHttpApiExHandle,          /*HTTPAPIEX_HANDLE handle - the handle created at the beginning of `IoTHubClient_LL_UploadToBlob`*/
                            HTTPAPI_REQUEST_GET,            /*HTTPAPI_REQUEST_TYPE requestType - HTTPAPI_REQUEST_GET*/
                            STRING_c_str(relativePath),     /*const char* relativePath - the HTTP relative path*/
                            requestHttpHeaders,             /*HTTP_HEADERS_HANDLE requestHttpHeadersHandle - request HTTP headers*/
                            NULL,                           /*BUFFER_HANDLE requestContent - NULL*/
                            &statusCode,                    /*unsigned int* statusCode - the address of an unsigned int that will contain the HTTP status code*/
                            NULL,                           /*HTTP_HEADERS_HANDLE responseHttpHeadersHandl - NULL*/
                            responseContent                 /*BUFFER_HANDLE responseContent - the HTTP response BUFFER_HANDLE - responseContent*/
                        ) != HTTPAPIEX_OK)
                        {
                            /*Codes_SRS_IOTHUBCLIENT_LL_02_076: [ If HTTPAPIEX_ExecuteRequest call fails then IoTHubClient_LL_UploadToBlob shall fail and return IOTHUB_CLIENT_ERROR. ]*/
                            result = __LINE__;
                            LogError("unable to HTTPAPIEX_ExecuteRequest");
                        }
                        else
                        {
                            /*Codes_SRS_IOTHUBCLIENT_LL_02_077: [ If HTTP statusCode is greater than or equal to 300 then IoTHubClient_LL_UploadToBlob shall fail and return IOTHUB_CLIENT_ERROR. ]*/
                            if (statusCode >= 300)
                            {
                                result = __LINE__;
                                LogError("HTTP code was %u", statusCode);
                            }
                            else
                            {
                                wasIoTHubRequestSuccess = 1;
                            }
                        }
                        break;
                    }
                    case (SAS_TOKEN):
                    {
                        const char* sasToken = STRING_c_str(handleData->credentials.sas);
                        /*Codes_SRS_IOTHUBCLIENT_LL_02_073: [ If the credentials used to create handle have "sasToken" then IoTHubClient_LL_UploadToBlob shall add the following HTTP request headers: ]*/
                        if (HTTPHeaders_ReplaceHeaderNameValuePair(requestHttpHeaders, "Authorization", sasToken) != HTTP_HEADERS_OK)
                        {
                            /*Codes_SRS_IOTHUBCLIENT_LL_02_074: [ If adding "Authorization" fails then IoTHubClient_LL_UploadToBlob shall fail and return IOTHUB_CLIENT_ERROR ]*/
                            result = __LINE__;
                            LogError("unable to HTTPHeaders_AddHeaderNameValuePair");
                        }
                        else
                        {
                            unsigned int statusCode;
                            /*Codes_SRS_IOTHUBCLIENT_LL_02_075: [ IoTHubClient_LL_UploadToBlob shall execute HTTPAPIEX_ExecuteRequest passing the following information for arguments: ]*/
                            if (HTTPAPIEX_ExecuteRequest(
                                iotHubHttpApiExHandle,          /*HTTPAPIEX_HANDLE handle - the handle created at the beginning of `IoTHubClient_LL_UploadToBlob`*/
                                HTTPAPI_REQUEST_GET,            /*HTTPAPI_REQUEST_TYPE requestType - HTTPAPI_REQUEST_GET*/
                                STRING_c_str(relativePath),     /*const char* relativePath - the HTTP relative path*/
                                requestHttpHeaders,             /*HTTP_HEADERS_HANDLE requestHttpHeadersHandle - request HTTP headers*/
                                NULL,                           /*BUFFER_HANDLE requestContent - NULL*/
                                &statusCode,                    /*unsigned int* statusCode - the address of an unsigned int that will contain the HTTP status code*/
                                NULL,                           /*HTTP_HEADERS_HANDLE responseHttpHeadersHandl - NULL*/
                                responseContent                 /*BUFFER_HANDLE responseContent - the HTTP response BUFFER_HANDLE - responseContent*/
                            ) != HTTPAPIEX_OK)
                            {
                                /*Codes_SRS_IOTHUBCLIENT_LL_02_076: [ If HTTPAPIEX_ExecuteRequest call fails then IoTHubClient_LL_UploadToBlob shall fail and return IOTHUB_CLIENT_ERROR. ]*/
                                result = __LINE__;
                                LogError("unable to HTTPAPIEX_ExecuteRequest");
                            }
                            else
                            {
                                /*Codes_SRS_IOTHUBCLIENT_LL_02_077: [ If HTTP statusCode is greater than or equal to 300 then IoTHubClient_LL_UploadToBlob shall fail and return IOTHUB_CLIENT_ERROR. ]*/
                                if (statusCode >= 300)
                                {
                                    result = __LINE__;
                                    LogError("HTTP code was %u", statusCode);
                                }
                                else
                                {
                                    wasIoTHubRequestSuccess = 1;
                                }
                            }
                        }
                        break;
                    }
                    case(DEVICE_KEY):
                    {
                        /*Codes_SRS_IOTHUBCLIENT_LL_02_078: [ If the credentials used to create handle have "deviceKey" then IoTHubClient_LL_UploadToBlob shall create an HTTPAPIEX_SAS_HANDLE passing as arguments: ]*/
                        STRING_HANDLE uriResource = STRING_construct(handleData->hostname);
                        if (uriResource == NULL)
                        {
                            /*Codes_SRS_IOTHUBCLIENT_LL_02_089: [ If creating the HTTPAPIEX_SAS_HANDLE fails then IoTHubClient_LL_UploadToBlob shall fail and return IOTHUB_CLIENT_ERROR. ]*/
                            result = __LINE__;
                            LogError("unable to STRING_construct");
                        }
                        else
                        {
                            if (!(
                                (STRING_concat(uriResource, "/devices/") == 0) &&
                                (STRING_concat_with_STRING(uriResource, handleData->deviceId) == 0)
                                ))
                            {
                                /*Codes_SRS_IOTHUBCLIENT_LL_02_089: [ If creating the HTTPAPIEX_SAS_HANDLE fails then IoTHubClient_LL_UploadToBlob shall fail and return IOTHUB_CLIENT_ERROR. ]*/
                                LogError("unable to STRING_concat_with_STRING");
                                result = __LINE__;
                            }
                            else
                            {
                                STRING_HANDLE empty = STRING_new();
                                if (empty == NULL)
                                {
                                    LogError("unable to STRING_new");
                                    result = __LINE__;
                                }
                                else
                                {
                                    /*Codes_SRS_IOTHUBCLIENT_LL_02_089: [ If creating the HTTPAPIEX_SAS_HANDLE fails then IoTHubClient_LL_UploadToBlob shall fail and return IOTHUB_CLIENT_ERROR. ]*/
                                    HTTPAPIEX_SAS_HANDLE sasHandle = HTTPAPIEX_SAS_Create(handleData->credentials.deviceKey, uriResource, empty);
                                    if (sasHandle == NULL)
                                    {
                                        LogError("unable to HTTPAPIEX_SAS_Create");
                                        result = __LINE__;
                                    }
                                    else
                                    {
                                        unsigned int statusCode;
                                        /*Codes_SRS_IOTHUBCLIENT_LL_02_090: [ IoTHubClient_LL_UploadToBlob shall call HTTPAPIEX_SAS_ExecuteRequest passing as arguments: ]*/
                                        if (HTTPAPIEX_SAS_ExecuteRequest(
                                            sasHandle,                      /*HTTPAPIEX_SAS_HANDLE sasHandle - the created HTTPAPIEX_SAS_HANDLE*/
                                            iotHubHttpApiExHandle,          /*HTTPAPIEX_HANDLE handle - the created HTTPAPIEX_HANDLE*/
                                            HTTPAPI_REQUEST_GET,            /*HTTPAPI_REQUEST_TYPE requestType - HTTPAPI_REQUEST_GET*/
                                            STRING_c_str(relativePath),     /*const char* relativePath - the HTTP relative path*/
                                            requestHttpHeaders,             /*HTTP_HEADERS_HANDLE requestHttpHeadersHandle - request HTTP headers*/
                                            NULL,                           /*BUFFER_HANDLE requestContent - NULL*/
                                            &statusCode,                    /*unsigned int* statusCode - the address of an unsigned int that will contain the HTTP status code*/
                                            NULL,                           /*HTTP_HEADERS_HANDLE responseHeadersHandle - NULL*/
                                            responseContent
                                        ) != HTTPAPIEX_OK)
                                        {
                                            /*Codes_SRS_IOTHUBCLIENT_LL_02_079: [ If HTTPAPIEX_SAS_ExecuteRequest fails then IoTHubClient_LL_UploadToBlob shall fail and return IOTHUB_CLIENT_ERROR. ]*/
                                            LogError("unable to HTTPAPIEX_SAS_ExecuteRequest");
                                            result = __LINE__;
                                        }
                                        else
                                        {
                                            if (statusCode >= 300)
                                            {
                                                /*Codes_SRS_IOTHUBCLIENT_LL_02_080: [ If status code is greater than or equal to 300 then IoTHubClient_LL_UploadToBlob shall fail and return IOTHUB_CLIENT_ERROR. ]*/
                                                result = __LINE__;
                                                LogError("HTTP code was %u", statusCode);
                                            }
                                            else
                                            {
                                                wasIoTHubRequestSuccess = 1;
                                            }
                                        }
                                        HTTPAPIEX_SAS_Destroy(sasHandle);
                                    }
                                    STRING_delete(empty);
                                }
                            }
                            STRING_delete(uriResource);
                        }
                    }
                    } /*switch*/

                    if (wasIoTHubRequestSuccess == 0)
                    {
                        /*do nothing, shall be reported as an error*/
                    }
                    else
                    {
                        const unsigned char*responseContent_u_char = BUFFER_u_char(responseContent);
                        size_t responseContent_length = BUFFER_length(responseContent);
                        STRING_HANDLE responseAsString = STRING_from_byte_array(responseContent_u_char, responseContent_length);
                        if (responseAsString == NULL)
                        {
                            result = __LINE__;
                            LogError("unable to get the response as string");
                        }
                        else
                        {
                            /*Codes_SRS_IOTHUBCLIENT_LL_02_081: [ Otherwise, IoTHubClient_LL_UploadToBlob shall use parson to extract and save the following information from the response buffer: correlationID and SasUri. ]*/
                            JSON_Value* allJson = json_parse_string(STRING_c_str(responseAsString));
                            if (allJson == NULL)
                            {
                                /*Codes_SRS_IOTHUBCLIENT_LL_02_082: [ If extracting and saving the correlationId or SasUri fails then IoTHubClient_LL_UploadToBlob shall fail and return IOTHUB_CLIENT_ERROR. ]*/
                                LogError("unable to json_parse_string");
                                result = __LINE__;
                            }
                            else
                            {
                                JSON_Object* jsonObject = json_value_get_object(allJson);
                                if (jsonObject == NULL)
                                {
                                    /*Codes_SRS_IOTHUBCLIENT_LL_02_082: [ If extracting and saving the correlationId or SasUri fails then IoTHubClient_LL_UploadToBlob shall fail and return IOTHUB_CLIENT_ERROR. ]*/
                                    LogError("unable to json_value_get_object");
                                    result = __LINE__;
                                }
                                else
                                {
                                    const char* json_correlationId;
                                    json_correlationId = json_object_get_string(jsonObject, "correlationId");
                                    if (json_correlationId == NULL)
                                    {
                                        /*Codes_SRS_IOTHUBCLIENT_LL_02_082: [ If extracting and saving the correlationId or SasUri fails then IoTHubClient_LL_UploadToBlob shall fail and return IOTHUB_CLIENT_ERROR. ]*/
                                        LogError("unable to json_object_get_string(jsonObject, \"correlationId\")");
                                        result = __LINE__;
                                    }
                                    else
                                    {
                                        if (STRING_copy(correlationId, json_correlationId) != 0)
                                        {
                                            /*Codes_SRS_IOTHUBCLIENT_LL_02_082: [ If extracting and saving the correlationId or SasUri fails then IoTHubClient_LL_UploadToBlob shall fail and return IOTHUB_CLIENT_ERROR. ]*/
                                            LogError("unable to copy json_correlationId");
                                            result = __LINE__;
                                        }
                                        else
                                        {
                                            const char* json_hostName = json_object_get_string(jsonObject, "hostName");
                                            if (json_hostName == NULL)
                                            {
                                                /*Codes_SRS_IOTHUBCLIENT_LL_02_082: [ If extracting and saving the correlationId or SasUri fails then IoTHubClient_LL_UploadToBlob shall fail and return IOTHUB_CLIENT_ERROR. ]*/
                                                LogError("unable to json_object_get_string(jsonObject, \"hostName\")");
                                                result = __LINE__;
                                            }
                                            else
                                            {
                                                const char* json_containerName = json_object_get_string(jsonObject, "containerName");
                                                if (json_containerName == NULL)
                                                {
                                                    /*Codes_SRS_IOTHUBCLIENT_LL_02_082: [ If extracting and saving the correlationId or SasUri fails then IoTHubClient_LL_UploadToBlob shall fail and return IOTHUB_CLIENT_ERROR. ]*/
                                                    LogError("unable to json_object_get_string(jsonObject, \"containerName\")");
                                                    result = __LINE__;
                                                }
                                                else
                                                {
                                                    const char* json_blobName = json_object_get_string(jsonObject, "blobName");
                                                    if (json_blobName == NULL)
                                                    {
                                                        /*Codes_SRS_IOTHUBCLIENT_LL_02_082: [ If extracting and saving the correlationId or SasUri fails then IoTHubClient_LL_UploadToBlob shall fail and return IOTHUB_CLIENT_ERROR. ]*/
                                                        LogError("unable to json_object_get_string(jsonObject, \"blobName\")");
                                                        result = __LINE__;
                                                    }
                                                    else
                                                    {
                                                        const char* json_sasToken = json_object_get_string(jsonObject, "sasToken");
                                                        if (json_sasToken == NULL)
                                                        {
                                                            /*Codes_SRS_IOTHUBCLIENT_LL_02_082: [ If extracting and saving the correlationId or SasUri fails then IoTHubClient_LL_UploadToBlob shall fail and return IOTHUB_CLIENT_ERROR. ]*/
                                                            LogError("unable to json_object_get_string(jsonObject, \"sasToken\")");
                                                            result = __LINE__;
                                                        }
                                                        else
                                                        {
                                                            /*good JSON received from the service*/

                                                            if (STRING_copy(sasUri, "https://") != 0)
                                                            {
                                                                /*Codes_SRS_IOTHUBCLIENT_LL_02_082: [ If extracting and saving the correlationId or SasUri fails then IoTHubClient_LL_UploadToBlob shall fail and return IOTHUB_CLIENT_ERROR. ]*/
                                                                LogError("unable to STRING_copy");
                                                                result = __LINE__;
                                                            }
                                                            else
                                                            {
                                                                if (!(
                                                                    (STRING_concat(sasUri, json_hostName) == 0) &&
                                                                    (STRING_concat(sasUri, "/") == 0) &&
                                                                    (STRING_concat(sasUri, json_containerName) == 0) &&
                                                                    (STRING_concat(sasUri, "/") == 0) &&
                                                                    (STRING_concat(sasUri, json_blobName) == 0) &&
                                                                    (STRING_concat(sasUri, json_sasToken) == 0)
                                                                    ))
                                                                {
                                                                    /*Codes_SRS_IOTHUBCLIENT_LL_02_082: [ If extracting and saving the correlationId or SasUri fails then IoTHubClient_LL_UploadToBlob shall fail and return IOTHUB_CLIENT_ERROR. ]*/
                                                                    LogError("unable to STRING_concat");
                                                                    result = __LINE__;
                                                                }
                                                                else
                                                                {
                                                                    result = 0; /*success in step 1*/
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                json_value_free(allJson);
                            }
                            STRING_delete(responseAsString);
                        }
                    }
                }
                BUFFER_delete(responseContent);
            }
        }
        STRING_delete(relativePath);
    }

    return result;
}

/*returns 0 when the IoTHub has been informed about the file upload status*/
static int IoTHubClient_LL_UploadToBlob_step3(IOTHUB_CLIENT_LL_UPLOADTOBLOB_HANDLE_DATA* handleData, STRING_HANDLE correlationId, HTTPAPIEX_HANDLE iotHubHttpApiExHandle, HTTP_HEADERS_HANDLE requestHttpHeaders, BUFFER_HANDLE messageBody)
{
    int result;
    /*here is step 3. depending on the outcome of step 2 it needs to inform IoTHub about the file upload status*/
    /*if step 1 failed, there's nothing that step 3 needs to report.*/
    /*this POST "tries" to happen*/

    /*Codes_SRS_IOTHUBCLIENT_LL_02_085: [ IoTHubClient_LL_UploadToBlob shall use the same authorization as step 1. to prepare and perform a HTTP request with the following parameters: ]*/
    STRING_HANDLE uriResource = STRING_construct(handleData->hostname);
    if (uriResource == NULL)
    {
        LogError("unable to construct URI");
        result = __LINE__;
    }
    else
    {
        if (!(
            (STRING_concat(uriResource, "/devices/") == 0) &&
            (STRING_concat_with_STRING(uriResource, handleData->deviceId) == 0) &&
            (STRING_concat(uriResource, "/files/notifications") == 0)
            ))
        {
            LogError("unable to STRING_concat");
            result = __LINE__;
        }
        else
        {
            STRING_HANDLE relativePathNotification = STRING_construct("/devices/");
            if (relativePathNotification == NULL)
            {
                result = __LINE__;
                LogError("unable to STRING_construct");
            }
            else
            {
                if (!(
                    (STRING_concat_with_STRING(relativePathNotification, handleData->deviceId) == 0) &&
                    (STRING_concat(relativePathNotification, "/files/notifications/") == 0) &&
                    (STRING_concat(relativePathNotification, STRING_c_str(correlationId)) == 0) &&
                    (STRING_concat(relativePathNotification, API_VERSION) == 0)
                    ))
                {
                    LogError("unable to STRING_concat_with_STRING");
                    result = __LINE__;
                }
                else
                {
                    /*Codes_SRS_IOTHUBCLIENT_LL_02_086: [ If performing the HTTP request fails then IoTHubClient_LL_UploadToBlob shall fail and return IOTHUB_CLIENT_ERROR. ]*/
                    switch (handleData->authorizationScheme)
                    {
                    default:
                    {
                        LogError("internal error: unknown authorization Scheme");
                        result = __LINE__;
                        break;
                    }
                    case (X509):
                    {
                        unsigned int notificationStatusCode;
                        if (HTTPAPIEX_ExecuteRequest(
                            iotHubHttpApiExHandle,
                            HTTPAPI_REQUEST_POST,
                            STRING_c_str(relativePathNotification),
                            requestHttpHeaders,
                            messageBody,
                            &notificationStatusCode,
                            NULL,
                            NULL) != HTTPAPIEX_OK)
                        {
                            LogError("unable to do HTTPAPIEX_ExecuteRequest");
                            result = __LINE__;
                        }
                        else
                        {
                            if (notificationStatusCode >= 300)
                            {
                                /*Codes_SRS_IOTHUBCLIENT_LL_02_087: [If the statusCode of the HTTP request is greater than or equal to 300 then IoTHubClient_LL_UploadToBlob shall fail and return IOTHUB_CLIENT_ERROR]*/
                                LogError("server didn't like the notification request");
                                result = __LINE__;
                            }
                            else
                            {
                                result = 0;
                            }
                        }
                        break;
                    }
                    case (DEVICE_KEY):
                    {
                        STRING_HANDLE empty = STRING_new();
                        if (empty == NULL)
                        {
                            LogError("unable to STRING_new");
                            result = __LINE__;
                        }
                        else
                        {
                            HTTPAPIEX_SAS_HANDLE sasHandle = HTTPAPIEX_SAS_Create(handleData->credentials.deviceKey, uriResource, empty);
                            if (sasHandle == NULL)
                            {
                                LogError("unable to HTTPAPIEX_SAS_Create");
                                result = __LINE__;
                            }
                            else
                            {
                                unsigned int statusCode;
                                if (HTTPAPIEX_SAS_ExecuteRequest(
                                    sasHandle,                      /*HTTPAPIEX_SAS_HANDLE sasHandle - the created HTTPAPIEX_SAS_HANDLE*/
                                    iotHubHttpApiExHandle,          /*HTTPAPIEX_HANDLE handle - the created HTTPAPIEX_HANDLE*/
                                    HTTPAPI_REQUEST_POST,            /*HTTPAPI_REQUEST_TYPE requestType - HTTPAPI_REQUEST_GET*/
                                    STRING_c_str(relativePathNotification),     /*const char* relativePath - the HTTP relative path*/
                                    requestHttpHeaders,             /*HTTP_HEADERS_HANDLE requestHttpHeadersHandle - request HTTP headers*/
                                    messageBody,                    /*BUFFER_HANDLE requestContent*/
                                    &statusCode,                    /*unsigned int* statusCode - the address of an unsigned int that will contain the HTTP status code*/
                                    NULL,                           /*HTTP_HEADERS_HANDLE responseHeadersHandle - NULL*/
                                    NULL
                                ) != HTTPAPIEX_OK)
                                {
                                    /*Codes_SRS_IOTHUBCLIENT_LL_02_079: [ If HTTPAPIEX_SAS_ExecuteRequest fails then IoTHubClient_LL_UploadToBlob shall fail and return IOTHUB_CLIENT_ERROR. ]*/
                                    LogError("unable to HTTPAPIEX_SAS_ExecuteRequest");
                                    result = __LINE__;
                                    ;
                                }
                                else
                                {
                                    if (statusCode >= 300)
                                    {
                                        /*Codes_SRS_IOTHUBCLIENT_LL_02_087: [If the statusCode of the HTTP request is greater than or equal to 300 then IoTHubClient_LL_UploadToBlob shall fail and return IOTHUB_CLIENT_ERROR]*/
                                        result = __LINE__;
                                        LogError("HTTP code was %u", statusCode);
                                    }
                                    else
                                    {
                                        result = 0;
                                    }
                                }
                                HTTPAPIEX_SAS_Destroy(sasHandle);
                            }
                            STRING_delete(empty);
                        }
                        break;
                    }
                    case(SAS_TOKEN):
                    {
                        unsigned int notificationStatusCode;
                        if (HTTPAPIEX_ExecuteRequest(
                            iotHubHttpApiExHandle,
                            HTTPAPI_REQUEST_POST,
                            STRING_c_str(relativePathNotification),
                            requestHttpHeaders,
                            messageBody,
                            &notificationStatusCode,
                            NULL,
                            NULL) != HTTPAPIEX_OK)
                        {
                            LogError("unable to do HTTPAPIEX_ExecuteRequest");
                            result = __LINE__;
                        }
                        else
                        {
                            if (notificationStatusCode >= 300)
                            {
                                /*Codes_SRS_IOTHUBCLIENT_LL_02_087: [If the statusCode of the HTTP request is greater than or equal to 300 then IoTHubClient_LL_UploadToBlob shall fail and return IOTHUB_CLIENT_ERROR]*/
                                LogError("server didn't like the notification request");
                                result = __LINE__;
                            }
                            else
                            {
                                result = 0;
                            }
                        }
                        break;
                    }
                    } /*switch authorizationScheme*/
                }
                STRING_delete(relativePathNotification);
            }
        }
        STRING_delete(uriResource);
    }
    return result;
}

IOTHUB_CLIENT_RESULT IoTHubClient_LL_UploadToBlob_Impl(IOTHUB_CLIENT_LL_UPLOADTOBLOB_HANDLE handle, const char* destinationFileName, const unsigned char* source, size_t size)
{
    IOTHUB_CLIENT_RESULT result;
    BUFFER_HANDLE toBeTransmitted;
    int requiredStringLength;
    char* requiredString;

    /*Codes_SRS_IOTHUBCLIENT_LL_02_061: [ If handle is NULL then IoTHubClient_LL_UploadToBlob shall fail and return IOTHUB_CLIENT_INVALID_ARG. ]*/
    /*Codes_SRS_IOTHUBCLIENT_LL_02_062: [ If destinationFileName is NULL then IoTHubClient_LL_UploadToBlob shall fail and return IOTHUB_CLIENT_INVALID_ARG. ]*/
    /*Codes_SRS_IOTHUBCLIENT_LL_02_063: [ If source is NULL and size is greater than 0 then IoTHubClient_LL_UploadToBlob shall fail and return IOTHUB_CLIENT_INVALID_ARG. ]*/
    if (
        (handle == NULL) ||
        (destinationFileName == NULL) ||
        ((source == NULL) && (size > 0))
        )
    {
        LogError("invalid argument detected handle=%p destinationFileName=%p source=%p size=%zu", handle, destinationFileName, source, size);
        result = IOTHUB_CLIENT_INVALID_ARG;
    }
    else
    {
        IOTHUB_CLIENT_LL_UPLOADTOBLOB_HANDLE_DATA* handleData = (IOTHUB_CLIENT_LL_UPLOADTOBLOB_HANDLE_DATA*)handle;

        /*Codes_SRS_IOTHUBCLIENT_LL_02_064: [ IoTHubClient_LL_UploadToBlob shall create an HTTPAPIEX_HANDLE to the IoTHub hostname. ]*/
        HTTPAPIEX_HANDLE iotHubHttpApiExHandle = HTTPAPIEX_Create(handleData->hostname);

        /*Codes_SRS_IOTHUBCLIENT_LL_02_065: [ If creating the HTTPAPIEX_HANDLE fails then IoTHubClient_LL_UploadToBlob shall fail and return IOTHUB_CLIENT_ERROR. ]*/
        if (iotHubHttpApiExHandle == NULL)
        {
            LogError("unable to HTTPAPIEX_Create");
            result = IOTHUB_CLIENT_ERROR;
        }
        else
        {
            if (
                (handleData->authorizationScheme == X509) &&

                /*transmit the x509certificate and x509privatekey*/
                /*Codes_SRS_IOTHUBCLIENT_LL_02_106: [ - x509certificate and x509privatekey saved options shall be passed on the HTTPAPIEX_SetOption ]*/
                (!(
                    (HTTPAPIEX_SetOption(iotHubHttpApiExHandle, OPTION_X509_CERT, handleData->credentials.x509credentials.x509certificate) == HTTPAPIEX_OK) &&
                    (HTTPAPIEX_SetOption(iotHubHttpApiExHandle, OPTION_X509_PRIVATE_KEY, handleData->credentials.x509credentials.x509privatekey) == HTTPAPIEX_OK)
                ))
                )
            {
                LogError("unable to HTTPAPIEX_SetOption for x509");
                result = IOTHUB_CLIENT_ERROR;
            }
            else
            {

                STRING_HANDLE correlationId = STRING_new();
                if (correlationId == NULL)
                {
                    LogError("unable to STRING_new");
                    result = IOTHUB_CLIENT_ERROR;
                }
                else
                {
                    STRING_HANDLE sasUri = STRING_new();
                    if (sasUri == NULL)
                    {
                        LogError("unable to STRING_new");
                        result = IOTHUB_CLIENT_ERROR;
                    }
                    else
                    {
                        /*Codes_SRS_IOTHUBCLIENT_LL_02_070: [ IoTHubClient_LL_UploadToBlob shall create request HTTP headers. ]*/
                        HTTP_HEADERS_HANDLE requestHttpHeaders = HTTPHeaders_Alloc(); /*these are build by step 1 and used by step 3 too*/
                        if (requestHttpHeaders == NULL)
                        {
                            LogError("unable to HTTPHeaders_Alloc");
                            result = IOTHUB_CLIENT_ERROR;
                        }
                        else
                        {
                            /*do step 1*/
                            if (IoTHubClient_LL_UploadToBlob_step1and2(handleData, iotHubHttpApiExHandle, requestHttpHeaders, destinationFileName, correlationId, sasUri) != 0)
                            {
                                LogError("error in IoTHubClient_LL_UploadToBlob_step1");
                                result = IOTHUB_CLIENT_ERROR;
                            }
                            else
                            {
                                /*do step 2.*/

                                unsigned int httpResponse;
                                BUFFER_HANDLE responseToIoTHub = BUFFER_new();
                                if (responseToIoTHub == NULL)
                                {
                                    result = IOTHUB_CLIENT_ERROR;
                                    LogError("unable to BUFFER_new");
                                }
                                else
                                {
                                    int step2success;
                                    /*Codes_SRS_IOTHUBCLIENT_LL_02_083: [ IoTHubClient_LL_UploadToBlob shall call Blob_UploadFromSasUri and capture the HTTP return code and HTTP body. ]*/
                                    step2success = (Blob_UploadFromSasUri(STRING_c_str(sasUri), source, size, &httpResponse, responseToIoTHub) == BLOB_OK);
                                    if (!step2success)
                                    {
                                        /*Codes_SRS_IOTHUBCLIENT_LL_02_084: [ If Blob_UploadFromSasUri fails then IoTHubClient_LL_UploadToBlob shall fail and return IOTHUB_CLIENT_ERROR. ]*/
                                        LogError("unable to Blob_UploadFromSasUri");

                                        /*do step 3*/ /*try*/
                                        /*Codes_SRS_IOTHUBCLIENT_LL_02_091: [ If step 2 fails without establishing an HTTP dialogue, then the HTTP message body shall look like: ]*/
                                        if (BUFFER_build(responseToIoTHub, (const unsigned char*)FILE_UPLOAD_FAILED_BODY, sizeof(FILE_UPLOAD_FAILED_BODY) / sizeof(FILE_UPLOAD_FAILED_BODY[0])) == 0)
                                        {
                                            if (IoTHubClient_LL_UploadToBlob_step3(handleData, correlationId, iotHubHttpApiExHandle, requestHttpHeaders, responseToIoTHub) != 0)
                                            {
                                                LogError("IoTHubClient_LL_UploadToBlob_step3 failed");
                                            }
                                        }
                                        result = IOTHUB_CLIENT_ERROR;
                                    }
                                    else
                                    {
                                        /*must make a json*/

                                        requiredStringLength = snprintf(NULL, 0, "{\"isSuccess\":%s, \"statusCode\":%d, \"statusDescription\":\"%s\"}", ((httpResponse < 300) ? "true" : "false"), httpResponse, BUFFER_u_char(responseToIoTHub));

                                        requiredString = malloc(requiredStringLength + 1);
                                        if (requiredString == 0)
                                        {
                                            LogError("unable to malloc");
                                            result = IOTHUB_CLIENT_ERROR;
                                        }
                                        else
                                        {
                                            /*do again snprintf*/
                                            (void)snprintf(requiredString, requiredStringLength + 1, "{\"isSuccess\":%s, \"statusCode\":%d, \"statusDescription\":\"%s\"}", ((httpResponse < 300) ? "true" : "false"), httpResponse, BUFFER_u_char(responseToIoTHub));
                                            toBeTransmitted = BUFFER_create((const unsigned char*)requiredString, requiredStringLength);
                                            if (toBeTransmitted == NULL)
                                            {
                                                LogError("unable to BUFFER_create");
                                                result = IOTHUB_CLIENT_ERROR;
                                            }
                                            else
                                            {
                                                if (IoTHubClient_LL_UploadToBlob_step3(handleData, correlationId, iotHubHttpApiExHandle, requestHttpHeaders, toBeTransmitted) != 0)
                                                {
                                                    LogError("IoTHubClient_LL_UploadToBlob_step3 failed");
                                                    result = IOTHUB_CLIENT_ERROR;
                                                }
                                                else
                                                {
                                                    result = (httpResponse < 300) ? IOTHUB_CLIENT_OK : IOTHUB_CLIENT_ERROR;
                                                }
                                                BUFFER_delete(toBeTransmitted);
                                            }
                                            free(requiredString);
                                        }
                                    }
                                    BUFFER_delete(responseToIoTHub);
                                }
                            }
                            HTTPHeaders_Free(requestHttpHeaders);
                        }
                        STRING_delete(sasUri);
                    }
                    STRING_delete(correlationId);
                }
            }
            HTTPAPIEX_Destroy(iotHubHttpApiExHandle);
        }
    }
    return result;
}

void IoTHubClient_LL_UploadToBlob_Destroy(IOTHUB_CLIENT_LL_UPLOADTOBLOB_HANDLE handle)
{
    if (handle == NULL)
    {
        LogError("unexepected NULL argument");
    }
    else
    {
        IOTHUB_CLIENT_LL_UPLOADTOBLOB_HANDLE_DATA* handleData = (IOTHUB_CLIENT_LL_UPLOADTOBLOB_HANDLE_DATA*)handle;
        switch (handleData->authorizationScheme)
        {
            case(SAS_TOKEN):
            {
                STRING_delete(handleData->credentials.sas);
                break;
            }
            case(DEVICE_KEY):
            {
                STRING_delete(handleData->credentials.deviceKey);
                break;
            }
            case(X509):
            {
                if (handleData->credentials.x509credentials.x509certificate != NULL)
                {
                    free((void*)handleData->credentials.x509credentials.x509certificate);
                }
                if (handleData->credentials.x509credentials.x509privatekey != NULL)
                {
                    free((void*)handleData->credentials.x509credentials.x509privatekey);
                }
                break;
            }
            default:
            {
                LogError("INTERNAL ERROR");
                break;
            }
        }
        free((void*)handleData->hostname);
        STRING_delete(handleData->deviceId);
        free(handleData);
    }
}

IOTHUB_CLIENT_RESULT IoTHubClient_LL_UploadToBlob_SetOption(IOTHUB_CLIENT_LL_UPLOADTOBLOB_HANDLE handle, const char* optionName, const void* value)
{
    IOTHUB_CLIENT_RESULT result;
    /*Codes_SRS_IOTHUBCLIENT_LL_02_110: [ If parameter handle is NULL then IoTHubClient_LL_UploadToBlob_SetOption shall fail and return IOTHUB_CLIENT_ERROR. ]*/
    if (handle == NULL)
    {
        LogError("invalid argument detected: IOTHUB_CLIENT_LL_UPLOADTOBLOB_HANDLE handle=%p, const char* optionName=%s, const void* value=%p", handle, optionName, value);
        result = IOTHUB_CLIENT_ERROR;
    }
    else
    {
        IOTHUB_CLIENT_LL_UPLOADTOBLOB_HANDLE_DATA* handleData = (IOTHUB_CLIENT_LL_UPLOADTOBLOB_HANDLE_DATA*)handle;

        /*Codes_SRS_IOTHUBCLIENT_LL_02_100: [ x509certificate - then value then is a null terminated string that contains the x509 certificate. ]*/
        if (strcmp(optionName, OPTION_X509_CERT) == 0)
        {
            /*Codes_SRS_IOTHUBCLIENT_LL_02_109: [ If the authentication scheme is NOT x509 then IoTHubClient_LL_UploadToBlob_SetOption shall return IOTHUB_CLIENT_INVALID_ARG. ]*/
            if (handleData->authorizationScheme != X509)
            {
                LogError("trying to set a x509 certificate while the authentication scheme is not x509");
                result = IOTHUB_CLIENT_INVALID_ARG;
            }
            else
            {
                /*Codes_SRS_IOTHUBCLIENT_LL_02_103: [ The options shall be saved. ]*/
                /*try to make a copy of the certificate*/
                char* temp;
                if (mallocAndStrcpy_s(&temp, value) != 0)
                {
                    /*Codes_SRS_IOTHUBCLIENT_LL_02_104: [ If saving fails, then IoTHubClient_LL_UploadToBlob_SetOption shall fail and return IOTHUB_CLIENT_ERROR. ]*/
                    LogError("unable to mallocAndStrcpy_s");
                    result = IOTHUB_CLIENT_ERROR;
                }
                else
                {
                    /*Codes_SRS_IOTHUBCLIENT_LL_02_105: [ Otherwise IoTHubClient_LL_UploadToBlob_SetOption shall succeed and return IOTHUB_CLIENT_OK. ]*/
                    if (handleData->credentials.x509credentials.x509certificate != NULL) /*free any previous values, if any*/
                    {
                        free((void*)handleData->credentials.x509credentials.x509certificate);
                    }
                    handleData->credentials.x509credentials.x509certificate = temp;
                    result = IOTHUB_CLIENT_OK;
                }
            }
        }
        /*Codes_SRS_IOTHUBCLIENT_LL_02_101: [ x509privatekey - then value is a null terminated string that contains the x509 privatekey. ]*/
        else if (strcmp(optionName, OPTION_X509_PRIVATE_KEY) == 0)
        {
            /*Codes_SRS_IOTHUBCLIENT_LL_02_109: [ If the authentication scheme is NOT x509 then IoTHubClient_LL_UploadToBlob_SetOption shall return IOTHUB_CLIENT_INVALID_ARG. ]*/
            if (handleData->authorizationScheme != X509)
            {
                LogError("trying to set a x509 privatekey while the authentication scheme is not x509");
                result = IOTHUB_CLIENT_INVALID_ARG;
            }
            else
            {
                /*Codes_SRS_IOTHUBCLIENT_LL_02_103: [ The options shall be saved. ]*/
                /*try to make a copy of the privatekey*/
                char* temp;
                if (mallocAndStrcpy_s(&temp, value) != 0)
                {
                    /*Codes_SRS_IOTHUBCLIENT_LL_02_104: [ If saving fails, then IoTHubClient_LL_UploadToBlob_SetOption shall fail and return IOTHUB_CLIENT_ERROR. ]*/
                    LogError("unable to mallocAndStrcpy_s");
                    result = IOTHUB_CLIENT_ERROR;
                }
                else
                {
                    /*Codes_SRS_IOTHUBCLIENT_LL_02_105: [ Otherwise IoTHubClient_LL_UploadToBlob_SetOption shall succeed and return IOTHUB_CLIENT_OK. ]*/
                    if (handleData->credentials.x509credentials.x509privatekey != NULL) /*free any previous values, if any*/
                    {
                        free((void*)handleData->credentials.x509credentials.x509privatekey);
                    }
                    handleData->credentials.x509credentials.x509privatekey = temp;
                    result = IOTHUB_CLIENT_OK;
                }
            }
        }
        else
        {
            /*Codes_SRS_IOTHUBCLIENT_LL_02_102: [ If an unknown option is presented then IoTHubClient_LL_UploadToBlob_SetOption shall return IOTHUB_CLIENT_INVALID_ARG. ]*/
            result = IOTHUB_CLIENT_INVALID_ARG;
        }
    }
    return result;
}


#endif /*DONT_USE_UPLOADTOBLOB*/


