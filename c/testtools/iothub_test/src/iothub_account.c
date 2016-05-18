// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include <stdio.h>
#include <string.h>
#include "azure_c_shared_utility/string_tokenizer.h"
#include "iothub_account.h"

#include "azure_c_shared_utility/httpapiexsas.h"
#include "azure_c_shared_utility/base64.h"
#include "azure_c_shared_utility/hmacsha256.h"
#include "azure_c_shared_utility/iot_logging.h"
#include "azure_c_shared_utility/sastoken.h"

#include "azure_c_shared_utility/httpapiexsas.h"
#include "azure_c_shared_utility/base64.h"
#include "azure_c_shared_utility/iot_logging.h"
#include "azure_c_shared_utility/uniqueid.h"

#ifdef MBED_BUILD_TIMESTAMP
#define MBED_PARAM_MAX_LENGTH 256
#endif

static const char* URL_API_VERSION = "api-version=2016-02-03";
static const char* DEVICE_JSON_FMT = "{\"deviceId\":\"%s\",\"etag\":null,\"connectionState\":\"Disconnected\",\"status\":\"enabled\",\"statusReason\":null,\"connectionStateUpdatedTime\":\"0001-01-01T00:00:00\",\"statusUpdatedTime\":\"0001-01-01T00:00:00\",\"lastActivityTime\":\"0001-01-01T00:00:00\",\"authentication\":{\"symmetricKey\":{\"primaryKey\":null,\"secondaryKey\":null}}}";
static const char* DEVICE_PREFIX_FMT = "e2eDevice_%s";
static const char* RELATIVE_PATH_FMT = "/devices/%s?%s";
static const char* SHARED_ACCESS_KEY = "SharedAccessSignature sr=%s&sig=%s&se=%s&skn=%s";

static const char *DEFAULT_CONSUMER_GROUP = "$Default";
static const int DEFAULT_PARTITION_COUNT = 16;

static const char* PRIMARY_KEY_FIELD = "\"primaryKey\":\"";
static const size_t PRIMARY_KEY_FIELD_LEN = 14;

#define MAX_LENGTH_OF_UNSIGNED_INT  6
#define MAX_LENGTH_OF_TIME_VALUE    12
#define MAX_RAND_VALUE              10000
#define DEVICE_GUID_SIZE            37

typedef struct IOTHUB_ACCOUNT_INFO_TAG
{
    const char* connString;
    const char* eventhubConnString;
    char* hostname;
    char* iothubName;
    char* iothubSuffix;
    char* sharedAccessKey;
    char* sharedAccessToken;
    char* keyName;
    char* eventhubAccessKey;
    char* deviceId;
    char* deviceKey;
} IOTHUB_ACCOUNT_INFO;

static int parseDeviceJson(IOTHUB_ACCOUNT_INFO* accountInfo, BUFFER_HANDLE jsonBuffer)
{
    int result = __LINE__;
    const char* deviceJson = (const char*)BUFFER_u_char(jsonBuffer);
    size_t jsonLen = BUFFER_length(jsonBuffer);
    if (deviceJson != NULL && jsonLen > PRIMARY_KEY_FIELD_LEN)
    {
        // Start to the end of the json because the primary key is at the end
        const char* iterator = deviceJson + jsonLen - PRIMARY_KEY_FIELD_LEN;
        while (iterator >= deviceJson)
        {
            // Once we find the primary key field then copy it into the buffer
            if (memcmp(iterator, PRIMARY_KEY_FIELD, PRIMARY_KEY_FIELD_LEN) == 0)
            {
                // Move to the beginning of the primary key value
                iterator += PRIMARY_KEY_FIELD_LEN;
                // Store where we are
                const char* devKeyMarker = iterator;

                // Now go till we find the quote
                while (*devKeyMarker != '\"' && devKeyMarker != (deviceJson + jsonLen - PRIMARY_KEY_FIELD_LEN)) { devKeyMarker++; }
                size_t keyLen = devKeyMarker - iterator;

                // Allocate key
                accountInfo->deviceKey = (char*)malloc(keyLen + 1);
                if (accountInfo->deviceKey == NULL)
                {
                    LogError("Failure allocating device key.\r\n");
                    result = __LINE__;
                }
                else
                {
                    // Copy the data to the key
                    memcpy(accountInfo->deviceKey, iterator, keyLen);
                    accountInfo->deviceKey[keyLen] = '\0';
                    result = 0;
                }
                break;
            }
            iterator--;
        }
    }
    return result;
}

static HTTP_HEADERS_HANDLE getContentHeaders(bool appendIfMatch)
{
    HTTP_HEADERS_HANDLE httpHeader = HTTPHeaders_Alloc();
    if (httpHeader != NULL)
    {
        if (HTTPHeaders_AddHeaderNameValuePair(httpHeader, "Authorization", " ") != HTTP_HEADERS_OK ||
            HTTPHeaders_AddHeaderNameValuePair(httpHeader, "User-Agent", "Microsoft.Azure.Devices/1.0.0") != HTTP_HEADERS_OK ||
            HTTPHeaders_AddHeaderNameValuePair(httpHeader, "Accept", "application/json") != HTTP_HEADERS_OK ||
            HTTPHeaders_AddHeaderNameValuePair(httpHeader, "Content-Type", "application/json; charset=utf-8") != HTTP_HEADERS_OK)
        {
            LogError("Failure adding http headers.\r\n");
            HTTPHeaders_Free(httpHeader);
            httpHeader = NULL;
        }
        else
        {
            if (appendIfMatch)
            {
                if (HTTPHeaders_AddHeaderNameValuePair(httpHeader, "If-Match", "*") != HTTP_HEADERS_OK)
                {
                    LogError("Failure adding if-Match http headers.\r\n");
                    HTTPHeaders_Free(httpHeader);
                    httpHeader = NULL;
                }
            }
        }
    }
    return httpHeader;
}

static int generateDeviceName(IOTHUB_ACCOUNT_INFO* accountInfo, const char* callerName)
{
    int result;
    char deviceGuid[DEVICE_GUID_SIZE];
    if (UniqueId_Generate(deviceGuid, DEVICE_GUID_SIZE) != UNIQUEID_OK)
    {
        LogError("Unable to generate unique Id.\r\n");
        result = __LINE__;
    }
    else
    {
        size_t len = strlen(DEVICE_PREFIX_FMT) + DEVICE_GUID_SIZE;
        accountInfo->deviceId = (char*)malloc(len + 1);
        if (accountInfo->deviceId == NULL)
        {
            LogError("Failure allocating device ID.\r\n");
            result = __LINE__;
        }
        else
        {
            if (sprintf_s(accountInfo->deviceId, len + 1, DEVICE_PREFIX_FMT, deviceGuid) <= 0)
            {
                LogError("Failure constructing device ID.\r\n");
                result = __LINE__;
            }
            else
            {
                LogInfo("Created Device %s.", accountInfo->deviceId);
                result = 0;
            }
        }
    }
    return result;
}

static BUFFER_HANDLE constructDeviceJson(IOTHUB_ACCOUNT_INFO* accountInfo)
{
    BUFFER_HANDLE result;

    size_t len = strlen(DEVICE_JSON_FMT) + strlen(accountInfo->deviceId);
    char* deviceJson = (char*)malloc(len + 1);
    if (deviceJson == NULL)
    {
        LogError("Failure allocating device Json.\r\n");
        free(accountInfo->deviceId);
    }
    else
    {
        int dataLen = sprintf_s(deviceJson, len + 1, DEVICE_JSON_FMT, accountInfo->deviceId);
        if (dataLen <= 0)
        {
            LogError("Failure constructing device Json.\r\n");
            free(accountInfo->deviceId);
        }
        else
        {
            result = BUFFER_create(deviceJson, dataLen);
            if (result == NULL)
            {
                LogError("Failure creating Json buffer.\r\n");
                accountInfo->deviceId = NULL;
            }
        }
        free(deviceJson);
    }
    return result;
}

static BUFFER_HANDLE sendDeviceRegistryInfo(IOTHUB_ACCOUNT_INFO* accountInfo, BUFFER_HANDLE deviceBuffer, HTTPAPI_REQUEST_TYPE requestType)
{
    BUFFER_HANDLE result;

    STRING_HANDLE accessKey = STRING_construct(accountInfo->sharedAccessKey);
    STRING_HANDLE uriResouce = STRING_construct(accountInfo->hostname);
    STRING_HANDLE keyName = STRING_construct(accountInfo->keyName);
    if (accessKey != NULL && uriResouce != NULL && keyName != NULL)
    {
        HTTPAPIEX_SAS_HANDLE httpHandle = HTTPAPIEX_SAS_Create(accessKey, uriResouce, keyName);
        if (httpHandle != NULL)
        {
            HTTPAPIEX_HANDLE httpExApi = HTTPAPIEX_Create(accountInfo->hostname);
            if (httpExApi == NULL)
            {
                LogError("Failure creating httpApiEx with hostname: %s.\r\n", accountInfo->hostname);
                result = NULL;
            }
            else
            {
                char relativePath[256];
                if (sprintf_s(relativePath, 256, RELATIVE_PATH_FMT, accountInfo->deviceId, URL_API_VERSION) <= 0)
                {
                    LogError("Failure creating relative path.\r\n");
                    result = NULL;
                }
                else
                {

                    unsigned int statusCode = 0;

                    // Send PUT method to url
                    HTTP_HEADERS_HANDLE httpHeader = getContentHeaders((deviceBuffer == NULL) ? true : false);
                    if (httpHeader == NULL)
                    {
                        result = NULL;
                    }
                    else
                    {
                        BUFFER_HANDLE responseContent = BUFFER_new();
                        if (HTTPAPIEX_SAS_ExecuteRequest(httpHandle, httpExApi, requestType, relativePath, httpHeader, deviceBuffer, &statusCode, NULL, responseContent) != HTTPAPIEX_OK)
                        {
                            LogError("Failure calling HTTPAPIEX_SAS_ExecuteRequest.\r\n");
                            result = NULL;
                        }
                        else
                        {
                            // 409 means the device is already created so we don't need
                            // to create another one.
                            if (statusCode != 409 && statusCode > 300)
                            {
                                LogError("Http Failure status code %d.\r\n", statusCode);
                                BUFFER_delete(responseContent);
                                result = NULL;
                            }
                            else
                            {
                                result = responseContent;
                            }
                        }
                    }
                    HTTPHeaders_Free(httpHeader);
                }
                HTTPAPIEX_Destroy(httpExApi);
            }
            HTTPAPIEX_SAS_Destroy(httpHandle);
        }
        else
        {
            LogError("Http Failure with HTTPAPIEX_SAS_Create.\r\n");
            result = NULL;
        }
    }
    STRING_delete(accessKey);
    STRING_delete(uriResouce);
    STRING_delete(keyName);
    return result;
}

static int create_Device(IOTHUB_ACCOUNT_INFO* accountInfo, const char* callerName)
{
    int result = 0;
    if (generateDeviceName(accountInfo, callerName) != 0)
    {
        result = __LINE__;
    }
    else
    {
        BUFFER_HANDLE deviceJson = constructDeviceJson(accountInfo);
        if (deviceJson == NULL)
        {
            result = __LINE__;
            free(accountInfo->deviceId);
            accountInfo->deviceId = NULL;
        }
        else
        {
            BUFFER_HANDLE deviceResp = sendDeviceRegistryInfo(accountInfo, deviceJson, HTTPAPI_REQUEST_PUT);
            if (deviceResp != NULL)
            {
                if (parseDeviceJson(accountInfo, deviceResp) != 0)
                {
                    result = __LINE__;
                    free(accountInfo->deviceId);
                    accountInfo->deviceId = NULL;
                }
                else
                {
                    result = 0;
                }
                BUFFER_delete(deviceResp);
            }
            else
            {
                result = __LINE__;
                free(accountInfo->deviceId);
                accountInfo->deviceId = NULL;
            }
            BUFFER_delete(deviceJson);
        }
    }
    return result;
}

static int delete_Device(IOTHUB_ACCOUNT_INFO* accountInfo)
{
    int result = 0;
    if (accountInfo->deviceId != NULL)
    {
        BUFFER_HANDLE deleteDevice = sendDeviceRegistryInfo(accountInfo, NULL, HTTPAPI_REQUEST_DELETE);
        if (deleteDevice == NULL)
        {
            LogError("Unable to delete created device %s.\r\n", accountInfo->deviceId);
            result = __LINE__;
        }
        else
        {
            BUFFER_delete(deleteDevice);
            result = 0;
        }

        free(accountInfo->deviceId);
        free(accountInfo->deviceKey);
    }
    return result;
}

static int retrieveConnStringInfo(IOTHUB_ACCOUNT_INFO* accountInfo)
{
    int result;
    int beginName, endName, beginIothub, endIothub, beginHost, endHost, beginKey;
    int totalLen = strlen(accountInfo->connString);

    if (sscanf(accountInfo->connString, "HostName=%n%*[^.]%n.%n%*[^;];%nSharedAccessKeyName=%n%*[^;];%nSharedAccessKey=%n", &beginHost, &endHost, &beginIothub, &endIothub, &beginName, &endName, &beginKey) != 0)
    {
        LogError("Failure determining the string length parameters.\r\n");
        result = __LINE__;
    }
    else
    {
        if ((accountInfo->iothubName = (char*)malloc(endHost - beginHost + 1)) == NULL)
        {
            LogError("Failure allocating iothubName.\r\n");
            result = __LINE__;
        }
        else if ((accountInfo->hostname = (char*)malloc(endIothub - beginHost + 1)) == NULL)
        {
            LogError("Failure allocating hostname.\r\n");
            free(accountInfo->iothubName);
            result = __LINE__;
        }
        else if ((accountInfo->keyName = (char*)malloc(endName - beginName + 1)) == NULL)
        {
            LogError("Failure allocating hostName.\r\n");
            free(accountInfo->iothubName);
            free(accountInfo->hostname);
            result = __LINE__;
        }
        else if ((accountInfo->sharedAccessKey = (char*)malloc(totalLen + 1 - beginKey + 1)) == NULL)
        {
            LogError("Failure allocating hostName.\r\n");
            free(accountInfo->iothubName);
            free(accountInfo->keyName);
            free(accountInfo->hostname);
            result = __LINE__;
        }
        else if (sscanf(accountInfo->connString, "HostName=%[^.].%[^;];SharedAccessKeyName=%[^;];SharedAccessKey=%s", accountInfo->iothubName,
            accountInfo->hostname + endHost - beginHost + 1,
            accountInfo->keyName,
            accountInfo->sharedAccessKey) != 4)
        {
            LogError("Failure determining the string values.\r\n");
            free(accountInfo->iothubName);
            free(accountInfo->hostname);
            free(accountInfo->keyName);
            free(accountInfo->sharedAccessKey);
            result = __LINE__;
        }
        else
        {
            (void)strcpy(accountInfo->hostname, accountInfo->iothubName);
            accountInfo->hostname[endHost - beginHost] = '.';
            if (mallocAndStrcpy_s(&accountInfo->iothubSuffix, accountInfo->hostname + endHost - beginHost + 1) != 0)
            {
				LogError("[IoTHubAccount] Failure constructing the iothubSuffix.");
                free(accountInfo->iothubName);
                free(accountInfo->hostname);
                free(accountInfo->keyName);
                free(accountInfo->sharedAccessKey);
                result = __LINE__;
            }
            else
            {
                result = 0;
            }
        }
    }
    return result;
}

#ifdef MBED_BUILD_TIMESTAMP
static const char* getMbedParameter(const char* name)
{
	static char value[MBED_PARAM_MAX_LENGTH];
	(void)printf("%s?\r\n", name);
	(void)scanf("%s", &value);
	(void)printf("Received '%s'\r\n", value);				

    return value;	
}
#endif

IOTHUB_ACCOUNT_INFO_HANDLE IoTHubAccount_Init(bool createDevice, const char* callerName)
{
    IOTHUB_ACCOUNT_INFO* result = malloc(sizeof(IOTHUB_ACCOUNT_INFO));
	if (result == NULL)
	{
		LogError("[IoTHubAccount] Failed allocating IOTHUB_ACCOUNT_INFO.");
	}
	else
	{
        memset(result, 0, sizeof(IOTHUB_ACCOUNT_INFO));
		
#ifdef MBED_BUILD_TIMESTAMP
		result->connString = getMbedParameter("IOTHUB_CONNECTION_STRING");
		result->eventhubConnString = getMbedParameter("IOTHUB_EVENTHUB_CONNECTION_STRING");
#else
        result->connString = getenv("IOTHUB_CONNECTION_STRING");
        result->eventhubConnString = getenv("IOTHUB_EVENTHUB_CONNECTION_STRING");
#endif
		
        if (result->connString == NULL || result->eventhubConnString == NULL)
        {
            LogError("Failure retrieving Connection Strings values.\r\n");
            free(result);
            result = NULL;
        }
        else
        {
            if (retrieveConnStringInfo(result) != 0)
            {
                free(result);
                result = NULL;
            }
            else if (createDevice)
            {
				int create_device_result;
                if ((create_device_result = create_Device(result, callerName)) != 0)
                {
					LogError("Failed creating IoT device (%d)", create_device_result);
                    IoTHubAccount_deinit((IOTHUB_ACCOUNT_INFO_HANDLE)result);
                    result = NULL;
                }
            }
        }
    }
    return (IOTHUB_ACCOUNT_INFO_HANDLE)result;
}

void IoTHubAccount_deinit(IOTHUB_ACCOUNT_INFO_HANDLE acctHandle)
{
    if (acctHandle != NULL)
    {
        IOTHUB_ACCOUNT_INFO* acctInfo = (IOTHUB_ACCOUNT_INFO*)acctHandle;
        (void)delete_Device(acctInfo);

        free(acctInfo->hostname);
        free(acctInfo->iothubName);
        free(acctInfo->iothubSuffix);
        free(acctInfo->sharedAccessKey);
        free(acctInfo->sharedAccessToken);
        free(acctInfo->keyName);
        free(acctInfo->eventhubAccessKey);
        free(acctInfo);
    }
}

const char* IoTHubAccount_GetEventHubConnectionString(IOTHUB_ACCOUNT_INFO_HANDLE acctHandle)
{
    const char* result = NULL;
    IOTHUB_ACCOUNT_INFO* acctInfo = (IOTHUB_ACCOUNT_INFO*)acctHandle;
    if (acctInfo != NULL)
    {
        result = acctInfo->eventhubConnString;
    }
    return result;
}

const char* IoTHubAccount_GetIoTHubName(IOTHUB_ACCOUNT_INFO_HANDLE acctHandle)
{
    const char* result = NULL;
    IOTHUB_ACCOUNT_INFO* acctInfo = (IOTHUB_ACCOUNT_INFO*)acctHandle;
    if (acctInfo != NULL)
    {
        result = acctInfo->iothubName;
    }
    return result;
}

const char* IoTHubAccount_GetIoTHubSuffix(IOTHUB_ACCOUNT_INFO_HANDLE acctHandle)
{
    const char* result = NULL;
    IOTHUB_ACCOUNT_INFO* acctInfo = (IOTHUB_ACCOUNT_INFO*)acctHandle;
    if (acctInfo != NULL)
    {
        result = acctInfo->iothubSuffix;
    }
    return result;
}

const char* IoTHubAccount_GetEventhubListenName(IOTHUB_ACCOUNT_INFO_HANDLE acctHandle)
{
	static char listenName[64];  
	const char* value;
	
#ifndef MBED_BUILD_TIMESTAMP
	if ((value = getenv("IOTHUB_EVENTHUB_LISTEN_NAME")) == NULL)
#else
	if ((value = getMbedParameter("IOTHUB_EVENTHUB_LISTEN_NAME")) == NULL)
#endif
	{
		value = IoTHubAccount_GetIoTHubName(acctHandle); 
	}

	if (value != NULL &&
		sprintf_s(listenName, 64, "%s", value) <= 0)
	{
		LogError("Failed reading IoT Hub Event Hub listen namespace (sprintf_s failed).");
	}
    
	return listenName;
}

const char* IoTHubAccount_GetDeviceId(IOTHUB_ACCOUNT_INFO_HANDLE acctHandle)
{
    if (acctHandle != NULL)
    {
        return ((IOTHUB_ACCOUNT_INFO*)acctHandle)->deviceId;
    }
    else
    {
        return NULL;
    }
}

const char* IoTHubAccount_GetDeviceKey(IOTHUB_ACCOUNT_INFO_HANDLE acctHandle)
{
    if (acctHandle != NULL)
    {
        return ((IOTHUB_ACCOUNT_INFO*)acctHandle)->deviceKey;
    }
    else
    {
        return NULL;
    }
}

const char* IoTHubAccount_GetIoTHubConnString(IOTHUB_ACCOUNT_INFO_HANDLE acctHandle)
{
    if (acctHandle != NULL)
    {
        return ((IOTHUB_ACCOUNT_INFO*)acctHandle)->connString;
    }
    else
    {
        return NULL;
    }
}

const char* IoTHubAccount_GetSharedAccessSignature(IOTHUB_ACCOUNT_INFO_HANDLE acctHandle)
{
    const char* result = NULL;
    IOTHUB_ACCOUNT_INFO* acctInfo = (IOTHUB_ACCOUNT_INFO*)acctHandle;
    if (acctInfo != NULL)
    {
        if (acctInfo->sharedAccessToken != NULL)
        {
            // Reuse the sharedAccessToken if it's been created already
            result = acctInfo->sharedAccessToken;
        }
        else
        {
        time_t currentTime = time(NULL);
        size_t expiry_time = (size_t)(currentTime + 3600);

        STRING_HANDLE accessKey = STRING_construct(acctInfo->sharedAccessKey);
        STRING_HANDLE iotName = STRING_construct(acctInfo->hostname);
        STRING_HANDLE keyName = STRING_construct(acctInfo->keyName);
        if (accessKey != NULL && iotName != NULL && keyName != NULL)
        {
            STRING_HANDLE sasHandle = SASToken_Create(accessKey, iotName, keyName, expiry_time);
            if (sasHandle == NULL)
            {

                result = NULL;
            }
            else
            {
                if (mallocAndStrcpy_s(&acctInfo->sharedAccessToken, STRING_c_str(sasHandle)) != 0)
                {
                    result = NULL;
                }
                else
                {
                    result = acctInfo->sharedAccessToken;
                }
                STRING_delete(sasHandle);
            }
        }
        STRING_delete(accessKey);
        STRING_delete(iotName);
        STRING_delete(keyName);
    }
    }
    else
    {
        result = NULL;
    }
    return result; 
}

const char* IoTHubAccount_GetEventhubAccessKey(IOTHUB_ACCOUNT_INFO_HANDLE acctHandle)
{
    const char *iothub_connection_string;
    static char access_key[128];

    if ((iothub_connection_string = IoTHubAccount_GetIoTHubConnString(acctHandle)) != NULL)
    {
        STRING_HANDLE iothub_connection_string_str;

        if ((iothub_connection_string_str = STRING_construct(iothub_connection_string)) != NULL)
        {
            STRING_TOKENIZER_HANDLE tokenizer;
        
            if ((tokenizer = STRING_TOKENIZER_create(iothub_connection_string_str)) != NULL)
            {
                STRING_HANDLE tokenString;

                if ((tokenString = STRING_new()) != NULL)
                {
                    STRING_HANDLE valueString;

                    if ((valueString = STRING_new()) != NULL)
                    {
                        while ((STRING_TOKENIZER_get_next_token(tokenizer, tokenString, "=") == 0))
                        {
                            char tokenValue[128];
                            strcpy(tokenValue, STRING_c_str(tokenString));

                            if (STRING_TOKENIZER_get_next_token(tokenizer, tokenString, ";") != 0)
                            {
                                break;
                            }

                            if (strcmp(tokenValue, "SharedAccessKey") == 0)
                            {
                                strcpy(access_key, STRING_c_str(tokenString));
                                break;
                            }
                        }
                    
                        STRING_delete(valueString);
                    }

                    STRING_delete(tokenString);
                }

                STRING_TOKENIZER_destroy(tokenizer);
            }
            
            STRING_delete(iothub_connection_string_str);
        }
    } 

    return access_key;
}

const char* IoTHubAccount_GetEventhubConsumerGroup(IOTHUB_ACCOUNT_INFO_HANDLE acctHandle)
{
    (void)acctHandle;
#ifdef MBED_BUILD_TIMESTAMP
    return getMbedParameter("IOTHUB_EVENTHUB_CONSUMER_GROUP");
#else
    static char consumerGroup[64];
    char *envVarValue = getenv("IOTHUB_EVENTHUB_CONSUMER_GROUP");
    if (envVarValue != NULL)
    {
        strcpy(consumerGroup, envVarValue);
    }
    else
    { 
        strcpy(consumerGroup, DEFAULT_CONSUMER_GROUP);
    }
    return consumerGroup;
#endif
}

const size_t IoTHubAccount_GetIoTHubPartitionCount(IOTHUB_ACCOUNT_INFO_HANDLE acctHandle)
{
    int value;
    (void)acctHandle;
#ifdef MBED_BUILD_TIMESTAMP
	char* str_value;
	if ((str_value = getMbedParameter("IOTHUB_PARTITION_COUNT")) != NULL)
	{
		sscanf(str_value, "%i", &value);
	}
#else
    char *envVarValue = getenv("IOTHUB_PARTITION_COUNT");
    if (envVarValue != NULL)
    {
        value = atoi(envVarValue);
    }
    else
    {
        value = DEFAULT_PARTITION_COUNT;
    }
#endif
    return (size_t)value;
}

