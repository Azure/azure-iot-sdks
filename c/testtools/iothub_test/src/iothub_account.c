// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include <stdio.h>
#include "string.h"
#include "string_tokenizer.h"
#include "iothub_account.h"

#include "httpapiexsas.h"
#include "base64.h"
#include "hmacsha256.h"
#include "iot_logging.h"
#include "sastoken.h"

#ifdef MBED_BUILD_TIMESTAMP
#define MBED_PARAM_MAX_LENGTH 256
#endif

const char* URL_API_VERSION = "api-version=2016-02-03";
const char* DEVICE_JSON_FMT = "{ deviceId: \"%s\", Etag: null, State: null, StateReason: null, ConnectionState: null, ConnectionStateUpdatedTime: null, LastActivityTime: null }";
const char* DEVICE_PREFIX_FMT = "e2eDevice_%d%d%d";
const char* RELATIVE_PATH_FMT = "/devices/%s?%s";
const char* SHARED_ACCESS_KEY = "SharedAccessSignature sr=%s&sig=%s&se=%s&skn=%s";

const char *DEFAULT_CONSUMER_GROUP = "$Default";
const int DEFAULT_PARTITION_COUNT = 16;
#define MAX_LENGTH_OF_UNSIGNED_INT  12
#define MAX_RAND_VALUE              10000

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

// TODO: This function will query the server using http
static int create_Device(IOTHUB_ACCOUNT_INFO* accountInfo)
{
    int result = 0;
    if (mallocAndStrcpy_s(&accountInfo->deviceId, getenv("IOTHUB_DEVICE_ID")) != 0 ||
        mallocAndStrcpy_s(&accountInfo->deviceKey, getenv("IOTHUB_DEVICE_KEY")) != 0)
    {
        free(accountInfo->deviceId);
        free(accountInfo->deviceKey);
        accountInfo->deviceId = NULL;
        accountInfo->deviceKey = NULL;
        result = __LINE__;
    }
    return result;
}

static int delete_Device(IOTHUB_ACCOUNT_INFO* accountInfo)
{
    int result = 0;
    free(accountInfo->deviceId);
    free(accountInfo->deviceKey);
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

IOTHUB_ACCOUNT_INFO_HANDLE IoTHubAccount_Init(bool createDevice)
{
    IOTHUB_ACCOUNT_INFO* result = malloc(sizeof(IOTHUB_ACCOUNT_INFO));
    if (result != NULL)
    {
        memset(result, 0, sizeof(IOTHUB_ACCOUNT_INFO));
        result->connString = getenv("IOTHUB_CONNECTION_STRING");
        result->eventhubConnString = getenv("IOTHUB_EVENTHUB_CONNECTION_STRING");

        if (result->connString == NULL || result->eventhubConnString == NULL)
        {
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
                if (create_Device(result) != 0)
                {
                    free(result);
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
#ifdef MBED_BUILD_TIMESTAMP
    static char* value[MBED_PARAM_MAX_LENGTH];
    (void)mbed_log("EventHubConnectionString?\r\n");
    (void)scanf("%s", &value);
    (void)mbed_log("Received '%s'\r\n", value);
    return value;
#else
    const char* result = NULL;
    IOTHUB_ACCOUNT_INFO* acctInfo = (IOTHUB_ACCOUNT_INFO*)acctHandle;
    if (acctInfo != NULL)
    {
        result = acctInfo->eventhubConnString;
    }
    return result;
#endif
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
#ifdef MBED_BUILD_TIMESTAMP
    static char* value[MBED_PARAM_MAX_LENGTH];
    (void)mbed_log("EventhubListenName?\r\n");
    (void)scanf("%s", &value);
    (void)mbed_log("Received '%s'\r\n", value);
    return value;
#else
    return IoTHubAccount_GetIoTHubName(acctHandle);
#endif
}

const char* IoTHubAccount_GetDeviceId(IOTHUB_ACCOUNT_INFO_HANDLE acctHandle)
{
#ifdef MBED_BUILD_TIMESTAMP
    static char* value[MBED_PARAM_MAX_LENGTH];
    (void)mbed_log("DeviceId?\r\n");
    (void)scanf("%s", &value);
    (void)mbed_log("Received '%s'\r\n", value);
    return value;
#else
    if (acctHandle != NULL)
    {
        return ((IOTHUB_ACCOUNT_INFO*)acctHandle)->deviceId;
    }
    else
    {
        return NULL;
    }
#endif
}

const char* IoTHubAccount_GetDeviceKey(IOTHUB_ACCOUNT_INFO_HANDLE acctHandle)
{
#ifdef MBED_BUILD_TIMESTAMP
    static char* value[MBED_PARAM_MAX_LENGTH];
    (void)mbed_log("DeviceKey?\r\n");
    (void)scanf("%s", &value);
    (void)mbed_log("Received '%s'\r\n", value);
    return value;
#else
    if (acctHandle != NULL)
    {
        return ((IOTHUB_ACCOUNT_INFO*)acctHandle)->deviceKey;
    }
    else
    {
        return NULL;
    }
#endif
}

const char* IoTHubAccount_GetIoTHubConnString(IOTHUB_ACCOUNT_INFO_HANDLE acctHandle)
{
#ifdef MBED_BUILD_TIMESTAMP
    static char* value[MBED_PARAM_MAX_LENGTH];
    (void)mbed_log("IoTHubConnString?\r\n");
    (void)scanf("%s", &value);
    (void)mbed_log("Received '%s'\r\n", value);
    return value;
#else
    if (acctHandle != NULL)
    {
        return ((IOTHUB_ACCOUNT_INFO*)acctHandle)->connString;
    }
    else
    {
        return NULL;
    }
#endif
}

const char* IoTHubAccount_GetSharedAccessSignature(IOTHUB_ACCOUNT_INFO_HANDLE acctHandle)
{
#ifdef MBED_BUILD_TIMESTAMP
    char tempValue[MBED_PARAM_MAX_LENGTH];
    static char value[MBED_PARAM_MAX_LENGTH];

    (void)mbed_log("SharedAccessSignature?\r\n");
    (void)scanf("%s", &tempValue);
    (void)sprintf(value, "SharedAccessSignature %s", tempValue);
    (void)mbed_log("Received '%s'\r\n", value);
    return &value;
#else
    const char* result = NULL;
    IOTHUB_ACCOUNT_INFO* acctInfo = (IOTHUB_ACCOUNT_INFO*)acctHandle;
    if (acctInfo != NULL)
    {
        if (acctInfo->sharedAccessToken != NULL)
        {
            free(acctInfo->sharedAccessToken);
        }
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
    else
    {
        result = NULL;
    }
    return result; 
#endif
}

const char* IoTHubAccount_GetEventhubAccessKey(IOTHUB_ACCOUNT_INFO_HANDLE acctHandle)
{
    const char *iothub_connection_string;
    static char access_key[128];

    if ((iothub_connection_string = IoTHubAccount_GetIoTHubConnString(acctHandle)) != NULL)
    {
        STRING_HANDLE iothub_connection_string_str;

        if((iothub_connection_string_str = STRING_construct(iothub_connection_string)) != NULL)
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
    static char* value[MBED_PARAM_MAX_LENGTH];
    (void)mbed_log("EventhubConsumerGroup?\r\n");
    (void)scanf("%s", &value);
    (void)mbed_log("Received '%s'\r\n", value);
    return value;
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
    (void)mbed_log("EventHubPartitionCount?\r\n");
    (void)scanf("%i", &value);
    (void)mbed_log("Received '%i'\r\n", value);
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