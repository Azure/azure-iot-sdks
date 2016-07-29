// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

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

#include "iothub_client_ll.h"
#include "iothub_client_private.h"
#include "iothub_client_version.h"
#include "iothub_transport_ll.h"

#ifndef DONT_USE_UPLOADTOBLOB
#include "iothub_client_ll_uploadtoblob.h"
#endif

#define LOG_ERROR_RESULT LogError("result = %s", ENUM_TO_STRING(IOTHUB_CLIENT_RESULT, result));
#define INDEFINITE_TIME ((time_t)(-1))

DEFINE_ENUM_STRINGS(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_RESULT_VALUES);
DEFINE_ENUM_STRINGS(IOTHUB_CLIENT_CONFIRMATION_RESULT, IOTHUB_CLIENT_CONFIRMATION_RESULT_VALUES);

typedef struct IOTHUB_CLIENT_LL_HANDLE_DATA_TAG
{
    DLIST_ENTRY waitingToSend;
    TRANSPORT_LL_HANDLE transportHandle;
    bool isSharedTransport;
    IOTHUB_DEVICE_HANDLE deviceHandle;
    TRANSPORT_PROVIDER_FIELDS;
    IOTHUB_CLIENT_MESSAGE_CALLBACK_ASYNC messageCallback;
    void* messageUserContextCallback;
    time_t lastMessageReceiveTime;
    TICK_COUNTER_HANDLE tickCounter; /*shared tickcounter used to track message timeouts in waitingToSend list*/
    uint64_t currentMessageTimeout;
#ifndef DONT_USE_UPLOADTOBLOB
    IOTHUB_CLIENT_LL_UPLOADTOBLOB_HANDLE uploadToBlobHandle;
#endif

}IOTHUB_CLIENT_LL_HANDLE_DATA;

static const char HOSTNAME_TOKEN[] = "HostName";
static const char DEVICEID_TOKEN[] = "DeviceId";
static const char X509_TOKEN[] = "x509";
static const char X509_TOKEN_ONLY_ACCEPTABLE_VALUE[] = "true";
static const char DEVICEKEY_TOKEN[] = "SharedAccessKey";
static const char DEVICESAS_TOKEN[] = "SharedAccessSignature";
static const char PROTOCOL_GATEWAY_HOST[] = "GatewayHostName";

IOTHUB_CLIENT_LL_HANDLE IoTHubClient_LL_CreateFromConnectionString(const char* connectionString, IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol)
{
    IOTHUB_CLIENT_LL_HANDLE result = NULL;

    /*Codes_SRS_IOTHUBCLIENT_LL_05_001: [IoTHubClient_LL_CreateFromConnectionString shall obtain the version string by a call to IoTHubClient_GetVersionString.]*/
    /*Codes_SRS_IOTHUBCLIENT_LL_05_002: [IoTHubClient_LL_CreateFromConnectionString shall print the version string to standard output.]*/
    LogInfo("IoT Hub SDK for C, version %s", IoTHubClient_GetVersionString());

    /* Codes_SRS_IOTHUBCLIENT_LL_12_003: [IoTHubClient_LL_CreateFromConnectionString shall verify the input parameter and if it is NULL then return NULL] */
    if (connectionString == NULL)
    {
        LogError("Input parameter is NULL: connectionString");
    }
    else if (protocol == NULL)
    {
        LogError("Input parameter is NULL: protocol");
    }
    else
    {
        /* Codes_SRS_IOTHUBCLIENT_LL_12_004: [IoTHubClient_LL_CreateFromConnectionString shall allocate IOTHUB_CLIENT_CONFIG structure] */
		IOTHUB_CLIENT_CONFIG* config = (IOTHUB_CLIENT_CONFIG*) malloc(sizeof(IOTHUB_CLIENT_CONFIG));
        if (config == NULL)
        {
            /* Codes_SRS_IOTHUBCLIENT_LL_12_012: [If the allocation failed IoTHubClient_LL_CreateFromConnectionString  returns NULL]  */
            LogError("Malloc failed");
            return NULL;
        }
        else
        {
            STRING_TOKENIZER_HANDLE tokenizer1 = NULL;
            STRING_HANDLE connString = NULL;
            STRING_HANDLE tokenString = NULL;
            STRING_HANDLE valueString = NULL;
            STRING_HANDLE hostNameString = NULL;
            STRING_HANDLE hostSuffixString = NULL;
            STRING_HANDLE deviceIdString = NULL;
            STRING_HANDLE deviceKeyString = NULL;
            STRING_HANDLE deviceSasTokenString = NULL;
            STRING_HANDLE protocolGateway = NULL;

            config->protocol = protocol;

            config->iotHubName = NULL;
            config->iotHubSuffix = NULL;
            config->deviceId = NULL;
            config->deviceKey = NULL;
            config->deviceSasToken = NULL;

            /* Codes_SRS_IOTHUBCLIENT_LL_04_002: [If it does not, it shall pass the protocolGatewayHostName NULL.] */
            config->protocolGatewayHostName = NULL;

            if ((connString = STRING_construct(connectionString)) == NULL)
            {
                LogError("Error constructing connectiong String");
            }
            else if ((tokenizer1 = STRING_TOKENIZER_create(connString)) == NULL)
            {
                LogError("Error creating Tokenizer");
            }
            else if ((tokenString = STRING_new()) == NULL)
            {
                LogError("Error creating Token String");
            }
            else if ((valueString = STRING_new()) == NULL)
            {
                LogError("Error creating Value String");
            }
            else if ((hostNameString = STRING_new()) == NULL)
            {
                LogError("Error creating HostName String");
            }
            else if ((hostSuffixString = STRING_new()) == NULL)
            {
                LogError("Error creating HostSuffix String");
            }
            /* Codes_SRS_IOTHUBCLIENT_LL_12_005: [IoTHubClient_LL_CreateFromConnectionString shall try to parse the connectionString input parameter for the following structure: "Key1=value1;key2=value2;key3=value3..."] */
            /* Codes_SRS_IOTHUBCLIENT_LL_12_006: [IoTHubClient_LL_CreateFromConnectionString shall verify the existence of the following Key/Value pairs in the connection string: HostName, DeviceId, SharedAccessKey, SharedAccessSignature or x509]  */
            else
            {
                int isx509found = 0;
                while ((STRING_TOKENIZER_get_next_token(tokenizer1, tokenString, "=") == 0))
                {
                    if (STRING_TOKENIZER_get_next_token(tokenizer1, valueString, ";") != 0)
                    {
                        LogError("Tokenizer error");
                        break;
                    }
                    else
                    {
                        if (tokenString != NULL)
                        {
                            /* Codes_SRS_IOTHUBCLIENT_LL_12_010: [IoTHubClient_LL_CreateFromConnectionString shall fill up the IOTHUB_CLIENT_CONFIG structure using the following mapping: iotHubName = Name, iotHubSuffix = Suffix, deviceId = DeviceId, deviceKey = SharedAccessKey or deviceSasToken = SharedAccessSignature] */
                            const char* s_token = STRING_c_str(tokenString);
                            if (strcmp(s_token, HOSTNAME_TOKEN) == 0)
                            {
                                /* Codes_SRS_IOTHUBCLIENT_LL_12_009: [IoTHubClient_LL_CreateFromConnectionString shall split the value of HostName to Name and Suffix using the first "." as a separator] */
                                STRING_TOKENIZER_HANDLE tokenizer2 = NULL;
                                if ((tokenizer2 = STRING_TOKENIZER_create(valueString)) == NULL)
                                {
                                    LogError("Error creating Tokenizer");
                                    break;
                                }
                                else
                                {
                                    /* Codes_SRS_IOTHUBCLIENT_LL_12_015: [If the string split failed, IoTHubClient_LL_CreateFromConnectionString returns NULL ] */
                                    if (STRING_TOKENIZER_get_next_token(tokenizer2, hostNameString, ".") != 0)
                                    {
                                        LogError("Tokenizer error");
                                        STRING_TOKENIZER_destroy(tokenizer2);
                                        break;
                                    }
                                    else
                                    {
                                        config->iotHubName = STRING_c_str(hostNameString);
                                        if (STRING_TOKENIZER_get_next_token(tokenizer2, hostSuffixString, ";") != 0)
                                        {
                                            LogError("Tokenizer error");
                                            STRING_TOKENIZER_destroy(tokenizer2);
                                            break;
                                        }
                                        else
                                        {
                                            config->iotHubSuffix = STRING_c_str(hostSuffixString);
                                        }
                                    }
                                    STRING_TOKENIZER_destroy(tokenizer2);
                                }
                            }
                            else if (strcmp(s_token, DEVICEID_TOKEN) == 0)
                            {
                                deviceIdString = STRING_clone(valueString);
                                if (deviceIdString != NULL)
                                {
                                    config->deviceId = STRING_c_str(deviceIdString);
                                }
                            }
                            else if (strcmp(s_token, DEVICEKEY_TOKEN) == 0)
                            {
                                deviceKeyString = STRING_clone(valueString);
                                if (deviceKeyString != NULL)
                                {
                                    config->deviceKey = STRING_c_str(deviceKeyString);
                                }
                            }
                            else if (strcmp(s_token, DEVICESAS_TOKEN) == 0)
                            {
                                deviceSasTokenString = STRING_clone(valueString);
                                if (deviceSasTokenString != NULL)
                                {
                                    config->deviceSasToken = STRING_c_str(deviceSasTokenString);
                                }
                            }
                            else if (strcmp(s_token, X509_TOKEN) == 0)
                            {
                                if (strcmp(STRING_c_str(valueString), X509_TOKEN_ONLY_ACCEPTABLE_VALUE) != 0)
                                {
                                    LogError("x509 option has wrong value, the only acceptable one is \"true\"");
                                }
                                else
                                {
                                    isx509found = 1;
                                }
                            }
                            /* Codes_SRS_IOTHUBCLIENT_LL_04_001: [IoTHubClient_LL_CreateFromConnectionString shall verify the existence of key/value pair GatewayHostName. If it does exist it shall pass the value to IoTHubClient_LL_Create API.] */
                            else if (strcmp(s_token, PROTOCOL_GATEWAY_HOST) == 0)
                            {
                                protocolGateway = STRING_clone(valueString);
                                if (protocolGateway != NULL)
                                {
                                    config->protocolGatewayHostName = STRING_c_str(protocolGateway);
                                }
                            }
                        }
                    }
                }
                /* parsing is done - check the result */
                if (config->iotHubName == NULL)
                {
                    LogError("iotHubName is not found");
                }
                else if (config->iotHubSuffix == NULL)
                {
                    LogError("iotHubSuffix is not found");
                }
                else if (config->deviceId == NULL)
                {
                    LogError("deviceId is not found");
                }
                else if (!(
                    ((!isx509found) && (config->deviceSasToken == NULL) ^ (config->deviceKey == NULL)) ||
                    ((isx509found) && (config->deviceSasToken == NULL) && (config->deviceKey == NULL))
                    ))
                {
                    LogError("invalid combination of x509, deviceSasToken and deviceKey");
                }
                else
                {
                    /* Codes_SRS_IOTHUBCLIENT_LL_12_011: [IoTHubClient_LL_CreateFromConnectionString shall call into the IoTHubClient_LL_Create API with the current structure and returns with the return value of it] */
                    
                    result = IoTHubClient_LL_Create(config);
                    if (result == NULL)
                    {
                        LogError("IoTHubClient_LL_Create failed");
                    }
                    else
                    {
                        /*return as is*/
                    }
                }
            }
            if (deviceSasTokenString != NULL)
                STRING_delete(deviceSasTokenString);
            if (deviceKeyString != NULL)
                STRING_delete(deviceKeyString);
            if (deviceIdString != NULL)
                STRING_delete(deviceIdString);
            if (hostSuffixString != NULL)
                STRING_delete(hostSuffixString);
            if (hostNameString != NULL)
                STRING_delete(hostNameString);
            if (valueString != NULL)
                STRING_delete(valueString);
            if (tokenString != NULL)
                STRING_delete(tokenString);
            if (connString != NULL)
                STRING_delete(connString);
            if (protocolGateway != NULL)
                STRING_delete(protocolGateway);

            if (tokenizer1 != NULL)
                STRING_TOKENIZER_destroy(tokenizer1);

            free(config);
        }
    }
    return result;
}

static void setTransportProtocol(IOTHUB_CLIENT_LL_HANDLE_DATA* handleData, TRANSPORT_PROVIDER* protocol)
{
    handleData->IoTHubTransport_GetHostname = protocol->IoTHubTransport_GetHostname;
    handleData->IoTHubTransport_SetOption = protocol->IoTHubTransport_SetOption;
    handleData->IoTHubTransport_Create = protocol->IoTHubTransport_Create;
    handleData->IoTHubTransport_Destroy = protocol->IoTHubTransport_Destroy;
    handleData->IoTHubTransport_Register = protocol->IoTHubTransport_Register;
    handleData->IoTHubTransport_Unregister = protocol->IoTHubTransport_Unregister;
    handleData->IoTHubTransport_Subscribe = protocol->IoTHubTransport_Subscribe;
    handleData->IoTHubTransport_Unsubscribe = protocol->IoTHubTransport_Unsubscribe;
    handleData->IoTHubTransport_DoWork = protocol->IoTHubTransport_DoWork;
    handleData->IoTHubTransport_GetSendStatus = protocol->IoTHubTransport_GetSendStatus;

}

IOTHUB_CLIENT_LL_HANDLE IoTHubClient_LL_Create(const IOTHUB_CLIENT_CONFIG* config)
{
    IOTHUB_CLIENT_LL_HANDLE result;
    /*Codes_SRS_IOTHUBCLIENT_LL_02_001: [IoTHubClient_LL_Create shall return NULL if config parameter is NULL or protocol field is NULL.]*/
    if( 
        (config == NULL) ||
        (config->protocol == NULL)
        )
    {
        result = NULL;
        LogError("invalid configuration (NULL detected)");
    }
    else
    {
        IOTHUB_CLIENT_LL_HANDLE_DATA* handleData = (IOTHUB_CLIENT_LL_HANDLE_DATA*)malloc(sizeof(IOTHUB_CLIENT_LL_HANDLE_DATA));
        if (handleData == NULL)
        {
            LogError("malloc failed");
            result = NULL;
        }
        else
        {
#ifndef DONT_USE_UPLOADTOBLOB
            /*Codes_SRS_IOTHUBCLIENT_LL_02_094: [ IoTHubClient_LL_Create shall create a IOTHUB_CLIENT_LL_UPLOADTOBLOB_HANDLE from IOTHUB_CLIENT_CONFIG. ]*/
            /*Codes_SRS_IOTHUBCLIENT_LL_02_092: [ IoTHubClient_LL_CreateFromConnectionString shall create a IOTHUB_CLIENT_LL_UPLOADTOBLOB_HANDLE from IOTHUB_CLIENT_CONFIG. ]*/
            handleData->uploadToBlobHandle = IoTHubClient_LL_UploadToBlob_Create(config);
            if (handleData->uploadToBlobHandle == NULL)
            {
                /*Codes_SRS_IOTHUBCLIENT_LL_02_093: [ If creating the IOTHUB_CLIENT_LL_UPLOADTOBLOB_HANDLE fails then IoTHubClient_LL_CreateFromConnectionString shall fail and return NULL. ]*/
                /*Codes_SRS_IOTHUBCLIENT_LL_02_095: [ If creating the IOTHUB_CLIENT_LL_UPLOADTOBLOB_HANDLE fails then IoTHubClient_LL_Create shall fail and return NULL. ]*/
                LogError("unable to IoTHubClient_LL_UploadToBlob_Create");
                free(handleData);
                result = NULL;
            }
            else
#endif
            {
                /*Codes_SRS_IOTHUBCLIENT_LL_02_045: [ Otherwise IoTHubClient_LL_Create shall create a new TICK_COUNTER_HANDLE ]*/
                if ((handleData->tickCounter = tickcounter_create()) == NULL)
                {
#ifndef DONT_USE_UPLOADTOBLOB
                    /*Codes_SRS_IOTHUBCLIENT_LL_02_046: [ If creating the TICK_COUNTER_HANDLE fails then IoTHubClient_LL_Create shall fail and return NULL. ]*/
                    IoTHubClient_LL_UploadToBlob_Destroy(handleData->uploadToBlobHandle);
#endif
                    LogError("unable to get a tickcounter");
                    free(handleData);
                    result = NULL;
                }
                else
                {
                    /*Codes_SRS_IOTHUBCLIENT_LL_02_004: [Otherwise IoTHubClient_LL_Create shall initialize a new DLIST (further called "waitingToSend") containing records with fields of the following types: IOTHUB_MESSAGE_HANDLE, IOTHUB_CLIENT_EVENT_CONFIRMATION_CALLBACK, void*.]*/
                    IOTHUBTRANSPORT_CONFIG lowerLayerConfig;
                    DList_InitializeListHead(&(handleData->waitingToSend));
                    setTransportProtocol(handleData, (TRANSPORT_PROVIDER*)config->protocol());
                    handleData->messageCallback = NULL;
                    handleData->messageUserContextCallback = NULL;
                    handleData->lastMessageReceiveTime = INDEFINITE_TIME;
                    /*Codes_SRS_IOTHUBCLIENT_LL_02_006: [IoTHubClient_LL_Create shall populate a structure of type IOTHUBTRANSPORT_CONFIG with the information from config parameter and the previous DLIST and shall pass that to the underlying layer _Create function.]*/
                    lowerLayerConfig.upperConfig = config;
                    lowerLayerConfig.waitingToSend = &(handleData->waitingToSend);
                    /*Codes_SRS_IOTHUBCLIENT_LL_02_007: [If the underlaying layer _Create function fails them IoTHubClient_LL_Create shall fail and return NULL.] */
                    if ((handleData->transportHandle = handleData->IoTHubTransport_Create(&lowerLayerConfig)) == NULL)
                    {
                        LogError("underlying transport failed");
#ifndef DONT_USE_UPLOADTOBLOB
                        IoTHubClient_LL_UploadToBlob_Destroy(handleData->uploadToBlobHandle);
#endif
                        tickcounter_destroy(handleData->tickCounter);
                        free(handleData);
                        result = NULL;
                    }
                    else
                    {
                        IOTHUB_DEVICE_CONFIG deviceConfig;

                        deviceConfig.deviceId = config->deviceId;
                        deviceConfig.deviceKey = config->deviceKey;
                        deviceConfig.deviceSasToken = config->deviceSasToken;

                        /*Codes_SRS_IOTHUBCLIENT_LL_17_008: [IoTHubClient_LL_Create shall call the transport _Register function with a populated structure of type IOTHUB_DEVICE_CONFIG and waitingToSend list.] */
                        if ((handleData->deviceHandle = handleData->IoTHubTransport_Register(handleData->transportHandle, &deviceConfig, handleData, &(handleData->waitingToSend))) == NULL)
                        {
                            /*Codes_SRS_IOTHUBCLIENT_LL_17_009: [If the _Register function fails, this function shall fail and return NULL.]*/
                            LogError("Registering device in transport failed");
                            handleData->IoTHubTransport_Destroy(handleData->transportHandle);
#ifndef DONT_USE_UPLOADTOBLOB
                            IoTHubClient_LL_UploadToBlob_Destroy(handleData->uploadToBlobHandle);
#endif
                            tickcounter_destroy(handleData->tickCounter);
                            free(handleData);
                            result = NULL;
                        }
                        else
                        {
                            /*Codes_SRS_IOTHUBCLIENT_LL_02_008: [Otherwise, IoTHubClient_LL_Create shall succeed and return a non-NULL handle.] */
                            handleData->isSharedTransport = false;
                            /*Codes_SRS_IOTHUBCLIENT_LL_02_042: [ By default, messages shall not timeout. ]*/
                            handleData->currentMessageTimeout = 0;
                            result = handleData;
                        }
                    }
                }
            }
        }
    }

    return result;
}

IOTHUB_CLIENT_LL_HANDLE IoTHubClient_LL_CreateWithTransport(const IOTHUB_CLIENT_DEVICE_CONFIG * config)
{
    IOTHUB_CLIENT_LL_HANDLE result;
    /*Codes_SRS_IOTHUBCLIENT_LL_17_001: [IoTHubClient_LL_CreateWithTransport shall return NULL if config parameter is NULL, or protocol field is NULL or transportHandle is NULL.]*/
    if (
        (config == NULL) ||
        (config->protocol == NULL) ||
        (config->transportHandle == NULL) ||
        /*Codes_SRS_IOTHUBCLIENT_LL_02_098: [ IoTHubClient_LL_CreateWithTransport shall fail and return NULL if both config->deviceKey AND config->deviceSasToken are NULL. ]*/
        ((config->deviceKey == NULL) && (config->deviceSasToken == NULL))
        )
    {
        result = NULL;
        LogError("invalid configuration (NULL detected)");
    }
    else
    {
        /*Codes_SRS_IOTHUBCLIENT_LL_17_002: [IoTHubClient_LL_CreateWithTransport shall allocate data for the IOTHUB_CLIENT_LL_HANDLE.]*/
        IOTHUB_CLIENT_LL_HANDLE_DATA* handleData = (IOTHUB_CLIENT_LL_HANDLE_DATA*)malloc(sizeof(IOTHUB_CLIENT_LL_HANDLE_DATA));
        if (handleData == NULL)
        {
            /*Codes_SRS_IOTHUBCLIENT_LL_17_003: [If allocation fails, the function shall fail and return NULL.] */
            LogError("malloc failed");
            result = NULL;
        }
        else
        {
            handleData->transportHandle = config->transportHandle;
            setTransportProtocol(handleData, (TRANSPORT_PROVIDER*)config->protocol());

#ifndef DONT_USE_UPLOADTOBLOB
            const char* hostname = STRING_c_str(handleData->IoTHubTransport_GetHostname(handleData->transportHandle));
            /*Codes_SRS_IOTHUBCLIENT_LL_02_096: [ IoTHubClient_LL_CreateWithTransport shall create the data structures needed to instantiate a IOTHUB_CLIENT_LL_UPLOADTOBLOB_HANDLE. ]*/
            /*the first '.' says where the iothubname finishes*/
            const char* whereIsDot = strchr(hostname, '.');
            if (whereIsDot == NULL)
            {
                /*Codes_SRS_IOTHUBCLIENT_LL_02_097: [ If creating the data structures fails or instantiating the IOTHUB_CLIENT_LL_UPLOADTOBLOB_HANDLE fails then IoTHubClient_LL_CreateWithTransport shall fail and return NULL. ]*/
                LogError("unable to determine the IoTHub name");
                free(handleData);
                result = NULL;
            }
            else
            {
                /*Codes_SRS_IOTHUBCLIENT_LL_02_096: [ IoTHubClient_LL_CreateWithTransport shall create the data structures needed to instantiate a IOTHUB_CLIENT_LL_UPLOADTOBLOB_HANDLE. ]*/
				char* IoTHubName = (char*) malloc(whereIsDot - hostname + 1);
                if (IoTHubName == NULL)
                {
                    /*Codes_SRS_IOTHUBCLIENT_LL_02_097: [ If creating the data structures fails or instantiating the IOTHUB_CLIENT_LL_UPLOADTOBLOB_HANDLE fails then IoTHubClient_LL_CreateWithTransport shall fail and return NULL. ]*/
                    LogError("unable to malloc");
                    free(handleData);
                    result = NULL;
                }
                else
                {
                    const char* IotHubSuffix = whereIsDot + 1;
                    memcpy(IoTHubName, hostname, whereIsDot - hostname);
                    IoTHubName[whereIsDot - hostname ] = '\0';
                    
                    IOTHUB_CLIENT_CONFIG temp;
                    temp.deviceId = config->deviceId;
                    temp.deviceKey = config->deviceKey;
                    temp.deviceSasToken = config->deviceSasToken;
                    temp.iotHubName = IoTHubName;
                    temp.iotHubSuffix = IotHubSuffix;
                    temp.protocol = NULL; /*irrelevant to IoTHubClient_LL_UploadToBlob*/
                    temp.protocolGatewayHostName = NULL; /*irrelevant to IoTHubClient_LL_UploadToBlob*/

                    /*Codes_SRS_IOTHUBCLIENT_LL_02_097: [ If creating the data structures fails or instantiating the IOTHUB_CLIENT_LL_UPLOADTOBLOB_HANDLE fails then IoTHubClient_LL_CreateWithTransport shall fail and return NULL. ]*/
                    handleData->uploadToBlobHandle = IoTHubClient_LL_UploadToBlob_Create(&temp);
                    if (handleData->uploadToBlobHandle == NULL)
                    {
                        /*Codes_SRS_IOTHUBCLIENT_LL_02_096: [ IoTHubClient_LL_CreateWithTransport shall create the data structures needed to instantiate a IOTHUB_CLIENT_LL_UPLOADTOBLOB_HANDLE. ]*/
                        LogError("unable to IoTHubClient_LL_UploadToBlob_Create");
                        free(handleData);
                        result = NULL;
                    }
                    else
#endif
                    {
                        /*Codes_SRS_IOTHUBCLIENT_LL_02_047: [ IoTHubClient_LL_CreateWithTransport shall create a TICK_COUNTER_HANDLE. ]*/
                        if ((handleData->tickCounter = tickcounter_create()) == NULL)
                        {
                            /*Codes_SRS_IOTHUBCLIENT_LL_02_048: [ If creating the handle fails, then IoTHubClient_LL_CreateWithTransport shall fail and return NULL ]*/
                            LogError("unable to get a tickcounter");
#ifndef DONT_USE_UPLOADTOBLOB
                            IoTHubClient_LL_UploadToBlob_Destroy(handleData->uploadToBlobHandle);
#endif
                            free(handleData);
                            result = NULL;
                        }
                        else
                        {
                            /*Codes_SRS_IOTHUBCLIENT_LL_17_004: [IoTHubClient_LL_CreateWithTransport shall initialize a new DLIST (further called "waitingToSend") containing records with fields of the following types: IOTHUB_MESSAGE_HANDLE, IOTHUB_CLIENT_EVENT_CONFIRMATION_CALLBACK, void*.]*/
                            DList_InitializeListHead(&(handleData->waitingToSend));
                            
                            handleData->messageCallback = NULL;
                            handleData->messageUserContextCallback = NULL;
                            handleData->lastMessageReceiveTime = INDEFINITE_TIME;
                            

                            IOTHUB_DEVICE_CONFIG deviceConfig;

                            deviceConfig.deviceId = config->deviceId;
                            deviceConfig.deviceKey = config->deviceKey;
                            deviceConfig.deviceSasToken = config->deviceSasToken;

                            /*Codes_SRS_IOTHUBCLIENT_LL_17_006: [IoTHubClient_LL_CreateWithTransport shall call the transport _Register function with the IOTHUB_DEVICE_CONFIG populated structure and waitingToSend list.]*/
                            if ((handleData->deviceHandle = handleData->IoTHubTransport_Register(config->transportHandle, &deviceConfig, handleData, &(handleData->waitingToSend))) == NULL)
                            {
                                /*Codes_SRS_IOTHUBCLIENT_LL_17_007: [If the _Register function fails, this function shall fail and return NULL.]*/
                                LogError("Registering device in transport failed");
#ifndef DONT_USE_UPLOADTOBLOB
                                IoTHubClient_LL_UploadToBlob_Destroy(handleData->uploadToBlobHandle);
#endif
                                tickcounter_destroy(handleData->tickCounter);
                                free(handleData);
                                result = NULL;
                            }
                            else
                            {
                                /*Codes_SRS_IOTHUBCLIENT_LL_17_005: [IoTHubClient_LL_CreateWithTransport shall save the transport handle and mark this transport as shared.]*/
                                handleData->isSharedTransport = true;
                                /*Codes_SRS_IOTHUBCLIENT_LL_02_042: [ By default, messages shall not timeout. ]*/
                                handleData->currentMessageTimeout = 0;
                                result = handleData;
                            }
                        }
                    }
#ifndef DONT_USE_UPLOADTOBLOB
                    free(IoTHubName);
                }
            }
#endif
        }
    }

    return result;
}

void IoTHubClient_LL_Destroy(IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle)
{
    /*Codes_SRS_IOTHUBCLIENT_LL_02_009: [IoTHubClient_LL_Destroy shall do nothing if parameter iotHubClientHandle is NULL.]*/
    if (iotHubClientHandle != NULL)
    {
        PDLIST_ENTRY unsend;
        /*Codes_SRS_IOTHUBCLIENT_LL_17_010: [IoTHubClient_LL_Destroy  shall call the underlaying layer's _Unregister function] */
        IOTHUB_CLIENT_LL_HANDLE_DATA* handleData = (IOTHUB_CLIENT_LL_HANDLE_DATA*)iotHubClientHandle;
        handleData->IoTHubTransport_Unregister(handleData->deviceHandle);
        if (handleData->isSharedTransport == false)
        {
            /*Codes_SRS_IOTHUBCLIENT_LL_02_010: [If iotHubClientHandle was not created by IoTHubClient_LL_CreateWithTransport, IoTHubClient_LL_Destroy  shall call the underlaying layer's _Destroy function.] */
            handleData->IoTHubTransport_Destroy(handleData->transportHandle);
        }
        /*if any, remove the items currently not send*/
        while ((unsend = DList_RemoveHeadList(&(handleData->waitingToSend))) != &(handleData->waitingToSend))
        {
            IOTHUB_MESSAGE_LIST* temp = containingRecord(unsend, IOTHUB_MESSAGE_LIST, entry);
            /*Codes_SRS_IOTHUBCLIENT_LL_02_033: [Otherwise, IoTHubClient_LL_Destroy shall complete all the event message callbacks that are in the waitingToSend list with the result IOTHUB_CLIENT_CONFIRMATION_BECAUSE_DESTROY.] */
            if (temp->callback != NULL)
            {
                temp->callback(IOTHUB_CLIENT_CONFIRMATION_BECAUSE_DESTROY, temp->context);
            }
            IoTHubMessage_Destroy(temp->messageHandle);
            free(temp);
        }
        /*Codes_SRS_IOTHUBCLIENT_LL_17_011: [IoTHubClient_LL_Destroy  shall free the resources allocated by IoTHubClient (if any).] */
        tickcounter_destroy(handleData->tickCounter);
#ifndef DONT_USE_UPLOADTOBLOB
        IoTHubClient_LL_UploadToBlob_Destroy(handleData->uploadToBlobHandle);
#endif
        free(handleData);
    }
}

/*Codes_SRS_IOTHUBCLIENT_LL_02_044: [ Messages already delivered to IoTHubClient_LL shall not have their timeouts modified by a new call to IoTHubClient_LL_SetOption. ]*/
/*returns 0 on success, any other value is error*/
static int attach_ms_timesOutAfter(IOTHUB_CLIENT_LL_HANDLE_DATA* handleData, IOTHUB_MESSAGE_LIST *newEntry)
{
    int result;
    /*Codes_SRS_IOTHUBCLIENT_LL_02_043: [ Calling IoTHubClient_LL_SetOption with value set to "0" shall disable the timeout mechanism for all new messages. ]*/
    if (handleData->currentMessageTimeout == 0)
    {
        newEntry->ms_timesOutAfter = 0; /*do not timeout*/
        result = 0;
    }
    else
    {
        /*Codes_SRS_IOTHUBCLIENT_LL_02_039: [ "messageTimeout" - once IoTHubClient_LL_SendEventAsync is called the message shall timeout after value miliseconds. Value is a pointer to a uint64. ]*/
        if (tickcounter_get_current_ms(handleData->tickCounter, &newEntry->ms_timesOutAfter) != 0)
        {
            result = __LINE__;
            LogError("unable to get the current relative tickcount");
        }
        else
        {
            newEntry->ms_timesOutAfter += handleData->currentMessageTimeout;
            result = 0;
        }
    }
    return result;
}

IOTHUB_CLIENT_RESULT IoTHubClient_LL_SendEventAsync(IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle, IOTHUB_MESSAGE_HANDLE eventMessageHandle, IOTHUB_CLIENT_EVENT_CONFIRMATION_CALLBACK eventConfirmationCallback, void* userContextCallback)
{
    IOTHUB_CLIENT_RESULT result;
    /*Codes_SRS_IOTHUBCLIENT_LL_02_011: [IoTHubClient_LL_SendEventAsync shall fail and return IOTHUB_CLIENT_INVALID_ARG if parameter iotHubClientHandle or eventMessageHandle is NULL.]*/
    if (
        (iotHubClientHandle == NULL) ||
        (eventMessageHandle == NULL) ||
        /*Codes_SRS_IOTHUBCLIENT_LL_02_012: [IoTHubClient_LL_SendEventAsync shall fail and return IOTHUB_CLIENT_INVALID_ARG if parameter eventConfirmationCallback is NULL and userContextCallback is not NULL.] */
        ((eventConfirmationCallback == NULL) && (userContextCallback != NULL))
        )
    {
        result = IOTHUB_CLIENT_INVALID_ARG;
        LOG_ERROR_RESULT;
    }
    else
    {
        IOTHUB_MESSAGE_LIST *newEntry = (IOTHUB_MESSAGE_LIST*)malloc(sizeof(IOTHUB_MESSAGE_LIST));
        if (newEntry == NULL)
        {
            result = IOTHUB_CLIENT_ERROR;
            LOG_ERROR_RESULT;
        }
        else
        {
            IOTHUB_CLIENT_LL_HANDLE_DATA* handleData = (IOTHUB_CLIENT_LL_HANDLE_DATA*)iotHubClientHandle;

            if (attach_ms_timesOutAfter(handleData, newEntry) != 0)
            {
                result = IOTHUB_CLIENT_ERROR;
                LOG_ERROR_RESULT;
                free(newEntry);
            }
            else
            {
                /*Codes_SRS_IOTHUBCLIENT_LL_02_013: [IoTHubClient_SendEventAsync shall add the DLIST waitingToSend a new record cloning the information from eventMessageHandle, eventConfirmationCallback, userContextCallback.]*/
                if ((newEntry->messageHandle = IoTHubMessage_Clone(eventMessageHandle)) == NULL)
                {
                    /*Codes_SRS_IOTHUBCLIENT_LL_02_014: [If cloning and/or adding the information fails for any reason, IoTHubClient_LL_SendEventAsync shall fail and return IOTHUB_CLIENT_ERROR.] */
                    result = IOTHUB_CLIENT_ERROR;
                    free(newEntry);
                    LOG_ERROR_RESULT;
                }
                else
                {
                    /*Codes_SRS_IOTHUBCLIENT_LL_02_013: [IoTHubClient_SendEventAsync shall add the DLIST waitingToSend a new record cloning the information from eventMessageHandle, eventConfirmationCallback, userContextCallback.]*/
                    newEntry->callback = eventConfirmationCallback;
                    newEntry->context = userContextCallback;
                    DList_InsertTailList(&(iotHubClientHandle->waitingToSend), &(newEntry->entry));
                    /*Codes_SRS_IOTHUBCLIENT_LL_02_015: [Otherwise IoTHubClient_LL_SendEventAsync shall succeed and return IOTHUB_CLIENT_OK.] */
                    result = IOTHUB_CLIENT_OK;
                }
            }
        }
    }
    return result;
}

IOTHUB_CLIENT_RESULT IoTHubClient_LL_SetMessageCallback(IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle, IOTHUB_CLIENT_MESSAGE_CALLBACK_ASYNC messageCallback, void* userContextCallback)
{
    IOTHUB_CLIENT_RESULT result;
    /*Codes_SRS_IOTHUBCLIENT_LL_02_016: [IoTHubClient_LL_SetMessageCallback shall fail and return IOTHUB_CLIENT_INVALID_ARG if parameter iotHubClientHandle is NULL.] */
    if (iotHubClientHandle == NULL)
    {
        result = IOTHUB_CLIENT_INVALID_ARG;
        LOG_ERROR_RESULT;
    }
    else
    {
        IOTHUB_CLIENT_LL_HANDLE_DATA* handleData = (IOTHUB_CLIENT_LL_HANDLE_DATA*)iotHubClientHandle;
        if (messageCallback == NULL)
        {
            /*Codes_SRS_IOTHUBCLIENT_LL_02_019: [If parameter messageCallback is NULL then IoTHubClient_LL_SetMessageCallback shall call the underlying layer's _Unsubscribe function and return IOTHUB_CLIENT_OK.] */
            handleData->IoTHubTransport_Unsubscribe(handleData->deviceHandle);
            handleData->messageCallback = NULL;
            handleData->messageUserContextCallback = NULL;
            result = IOTHUB_CLIENT_OK;
        }
        else
        {
            /*Codes_SRS_IOTHUBCLIENT_LL_02_017: [If parameter messageCallback is non-NULL then IoTHubClient_LL_SetMessageCallback shall call the underlying layer's _Subscribe function.]*/
            if (handleData->IoTHubTransport_Subscribe(handleData->deviceHandle) == 0)
            {
                handleData->messageCallback = messageCallback;
                handleData->messageUserContextCallback = userContextCallback;
                result = IOTHUB_CLIENT_OK;
            }
            else
            {
                handleData->messageCallback = NULL;
                handleData->messageUserContextCallback = NULL;
                /*Codes_SRS_IOTHUBCLIENT_LL_02_018: [If the underlying layer's _Subscribe function fails, then IoTHubClient_LL_SetMessageCallback shall fail and return IOTHUB_CLIENT_ERROR. Otherwise IoTHubClient_LL_SetMessageCallback shall succeed and return IOTHUB_CLIENT_OK.]*/
                result = IOTHUB_CLIENT_ERROR;
            }
        }
    }

    return result;
}

static void DoTimeouts(IOTHUB_CLIENT_LL_HANDLE_DATA* handleData)
{
    uint64_t nowTick;
    if (tickcounter_get_current_ms(handleData->tickCounter, &nowTick) != 0)
    {
        LogError("unable to get the current ms, timeouts will not be processed");
    }
    else
    {
        DLIST_ENTRY* currentItemInWaitingToSend = handleData->waitingToSend.Flink;
        while (currentItemInWaitingToSend != &(handleData->waitingToSend)) /*while we are not at the end of the list*/
        {
            IOTHUB_MESSAGE_LIST* fullEntry = containingRecord(currentItemInWaitingToSend, IOTHUB_MESSAGE_LIST, entry);
            /*Codes_SRS_IOTHUBCLIENT_LL_02_041: [ If more than value miliseconds have passed since the call to IoTHubClient_LL_SendEventAsync then the message callback shall be called with a status code of IOTHUB_CLIENT_CONFIRMATION_TIMEOUT. ]*/
            if ((fullEntry->ms_timesOutAfter != 0) && (fullEntry->ms_timesOutAfter < nowTick))
            {
                PDLIST_ENTRY theNext = currentItemInWaitingToSend->Flink; /*need to save the next item, because the below operations are destructive*/
                DList_RemoveEntryList(currentItemInWaitingToSend);
                if (fullEntry->callback != NULL)
                {
                    fullEntry->callback(IOTHUB_CLIENT_CONFIRMATION_MESSAGE_TIMEOUT, fullEntry->context);
                }
                IoTHubMessage_Destroy(fullEntry->messageHandle); /*because it has been cloned*/
                free(fullEntry);
                currentItemInWaitingToSend = theNext;
            }
            else
            {
                currentItemInWaitingToSend = currentItemInWaitingToSend->Flink;
            }
        }
    }
}

void IoTHubClient_LL_DoWork(IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle)
{
    /*Codes_SRS_IOTHUBCLIENT_LL_02_020: [If parameter iotHubClientHandle is NULL then IoTHubClient_LL_DoWork shall not perform any action.] */
    if (iotHubClientHandle != NULL)
    {
        IOTHUB_CLIENT_LL_HANDLE_DATA* handleData = (IOTHUB_CLIENT_LL_HANDLE_DATA*)iotHubClientHandle;
        DoTimeouts(handleData);

        /*Codes_SRS_IOTHUBCLIENT_LL_02_021: [Otherwise, IoTHubClient_LL_DoWork shall invoke the underlaying layer's _DoWork function.]*/
        handleData->IoTHubTransport_DoWork(handleData->transportHandle, iotHubClientHandle);
    }
}

IOTHUB_CLIENT_RESULT IoTHubClient_LL_GetSendStatus(IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle, IOTHUB_CLIENT_STATUS *iotHubClientStatus)
{
    IOTHUB_CLIENT_RESULT result;

    /* Codes_SRS_IOTHUBCLIENT_09_007: [IoTHubClient_GetSendStatus shall return IOTHUB_CLIENT_INVALID_ARG if called with NULL parameter] */
    if (iotHubClientHandle == NULL || iotHubClientStatus == NULL)
    {
        result = IOTHUB_CLIENT_INVALID_ARG;
        LOG_ERROR_RESULT;
    }
    else
    {
        IOTHUB_CLIENT_LL_HANDLE_DATA* handleData = (IOTHUB_CLIENT_LL_HANDLE_DATA*)iotHubClientHandle;

        /* Codes_SRS_IOTHUBCLIENT_09_008: [IoTHubClient_GetSendStatus shall return IOTHUB_CLIENT_OK and status IOTHUB_CLIENT_SEND_STATUS_IDLE if there is currently no items to be sent] */
        /* Codes_SRS_IOTHUBCLIENT_09_009: [IoTHubClient_GetSendStatus shall return IOTHUB_CLIENT_OK and status IOTHUB_CLIENT_SEND_STATUS_BUSY if there are currently items to be sent] */
        result = handleData->IoTHubTransport_GetSendStatus(handleData->deviceHandle, iotHubClientStatus);
    }

    return result;
}

void IoTHubClient_LL_SendComplete(IOTHUB_CLIENT_LL_HANDLE handle, PDLIST_ENTRY completed, IOTHUB_CLIENT_CONFIRMATION_RESULT result)
{
    /*Codes_SRS_IOTHUBCLIENT_LL_02_022: [If parameter completed is NULL, or parameter handle is NULL then IoTHubClient_LL_SendBatch shall return.]*/
    if (
        (handle == NULL) ||
        (completed == NULL)
        )
    {
        /*"shall return"*/
        LogError("invalid arg");
    }
    else
    {
        /*Codes_SRS_IOTHUBCLIENT_LL_02_027: [If parameter result is IOTHUB_CLIENT_CONFIRMATION_ERROR then IoTHubClient_LL_SendComplete shall call all the non-NULL callbacks with the result parameter set to IOTHUB_CLIENT_CONFIRMATION_ERROR and the context set to the context passed originally in the SendEventAsync call.] */
        /*Codes_SRS_IOTHUBCLIENT_LL_02_025: [If parameter result is IOTHUB_CLIENT_CONFIRMATION_OK then IoTHubClient_LL_SendComplete shall call all the non-NULL callbacks with the result parameter set to IOTHUB_CLIENT_CONFIRMATION_OK and the context set to the context passed originally in the SendEventAsync call.]*/
        PDLIST_ENTRY oldest;
        while ((oldest = DList_RemoveHeadList(completed)) != completed)
        {
            IOTHUB_MESSAGE_LIST* messageList = (IOTHUB_MESSAGE_LIST*)containingRecord(oldest, IOTHUB_MESSAGE_LIST, entry);
            /*Codes_SRS_IOTHUBCLIENT_LL_02_026: [If any callback is NULL then there shall not be a callback call.]*/
            if (messageList->callback != NULL)
            {
                messageList->callback(result, messageList->context);
            }
            IoTHubMessage_Destroy(messageList->messageHandle);
            free(messageList);
        }
    }
}

IOTHUBMESSAGE_DISPOSITION_RESULT IoTHubClient_LL_MessageCallback(IOTHUB_CLIENT_LL_HANDLE handle, IOTHUB_MESSAGE_HANDLE message)
{
    int result;
    /*Codes_SRS_IOTHUBCLIENT_LL_02_029: [If parameter handle is NULL then IoTHubClient_LL_MessageCallback shall return IOTHUBMESSAGE_ABANDONED.] */
    if (handle == NULL)
    {
        LogError("invalid argument");
        result = IOTHUBMESSAGE_ABANDONED;
    }
    else
    {
        IOTHUB_CLIENT_LL_HANDLE_DATA* handleData = (IOTHUB_CLIENT_LL_HANDLE_DATA*)handle;

        /* Codes_SRS_IOTHUBCLIENT_LL_09_004: [IoTHubClient_LL_GetLastMessageReceiveTime shall return lastMessageReceiveTime in localtime] */
        handleData->lastMessageReceiveTime = get_time(NULL);

        /*Codes_SRS_IOTHUBCLIENT_LL_02_030: [IoTHubClient_LL_MessageCallback shall invoke the last callback function (the parameter messageCallback to IoTHubClient_LL_SetMessageCallback) passing the message and the passed userContextCallback.]*/
        if (handleData->messageCallback != NULL)
        {
            result = handleData->messageCallback(message, handleData->messageUserContextCallback);
        }
        else
        {
            /*Codes_SRS_IOTHUBCLIENT_LL_02_032: [If the last callback function was NULL, then IoTHubClient_LL_MessageCallback  shall return IOTHUBMESSAGE_ABANDONED.] */
            LogError("user callback was NULL");
            result = IOTHUBMESSAGE_ABANDONED;
        }
    }
    /*Codes_SRS_IOTHUBCLIENT_LL_02_031: [Then IoTHubClient_LL_MessageCallback shall return what the user function returns.]*/
	return (IOTHUBMESSAGE_DISPOSITION_RESULT) result;
}

IOTHUB_CLIENT_RESULT IoTHubClient_LL_GetLastMessageReceiveTime(IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle, time_t* lastMessageReceiveTime)
{
    IOTHUB_CLIENT_RESULT result;
    IOTHUB_CLIENT_LL_HANDLE_DATA* handleData = (IOTHUB_CLIENT_LL_HANDLE_DATA*)iotHubClientHandle;

    /* Codes_SRS_IOTHUBCLIENT_LL_09_001: [IoTHubClient_LL_GetLastMessageReceiveTime shall return IOTHUB_CLIENT_INVALID_ARG if any of the arguments is NULL] */
    if (handleData == NULL || lastMessageReceiveTime == NULL)
    {
        result = IOTHUB_CLIENT_INVALID_ARG;
        LOG_ERROR_RESULT;
    }
    else
    {
        /* Codes_SRS_IOTHUBCLIENT_LL_09_002: [IoTHubClient_LL_GetLastMessageReceiveTime shall return IOTHUB_CLIENT_INDEFINITE_TIME - and not set 'lastMessageReceiveTime' - if it is unable to provide the time for the last commands] */
        if (handleData->lastMessageReceiveTime == INDEFINITE_TIME)
        {
            result = IOTHUB_CLIENT_INDEFINITE_TIME;
            LOG_ERROR_RESULT;
        }
        else
        {
            /* Codes_SRS_IOTHUBCLIENT_LL_09_003: [IoTHubClient_LL_GetLastMessageReceiveTime shall return IOTHUB_CLIENT_OK if it wrote in the lastMessageReceiveTime the time when the last command was received] */
            /* Codes_SRS_IOTHUBCLIENT_LL_09_004: [IoTHubClient_LL_GetLastMessageReceiveTime shall return lastMessageReceiveTime in localtime] */
            *lastMessageReceiveTime = handleData->lastMessageReceiveTime;
            result = IOTHUB_CLIENT_OK;
        }
    }

    return result;
}

IOTHUB_CLIENT_RESULT IoTHubClient_LL_SetOption(IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle, const char* optionName, const void* value)
{

    IOTHUB_CLIENT_RESULT result;
    /*Codes_SRS_IOTHUBCLIENT_LL_02_034: [If iotHubClientHandle is NULL then IoTHubClient_LL_SetOption shall return IOTHUB_CLIENT_INVALID_ARG.]*/
    /*Codes_SRS_IOTHUBCLIENT_LL_02_035: [If optionName is NULL then IoTHubClient_LL_SetOption shall return IOTHUB_CLIENT_INVALID_ARG.] */
    /*Codes_SRS_IOTHUBCLIENT_LL_02_036: [If value is NULL then IoTHubClient_LL_SetOption shall return IOTHUB_CLIENT_INVALID_ARG.] */
    if (
        (iotHubClientHandle == NULL) ||
        (optionName == NULL) ||
        (value == NULL)
        )
    {
        result = IOTHUB_CLIENT_INVALID_ARG;
        LogError("invalid argument (NULL)");
    }
    else
    {
        IOTHUB_CLIENT_LL_HANDLE_DATA* handleData = (IOTHUB_CLIENT_LL_HANDLE_DATA*)iotHubClientHandle;

        /*Codes_SRS_IOTHUBCLIENT_LL_02_039: [ "messageTimeout" - once IoTHubClient_LL_SendEventAsync is called the message shall timeout after value miliseconds. Value is a pointer to a uint64. ]*/
        if (strcmp(optionName, "messageTimeout") == 0)
        {
            /*this is an option handled by IoTHubClient_LL*/
            /*Codes_SRS_IOTHUBCLIENT_LL_02_043: [ Calling IoTHubClient_LL_SetOption with value set to "0" shall disable the timeout mechanism for all new messages. ]*/
            handleData->currentMessageTimeout = *(const uint64_t*)value;
            result = IOTHUB_CLIENT_OK;
        }
        else
        {

            /*Codes_SRS_IOTHUBCLIENT_LL_02_099: [ IoTHubClient_LL_SetOption shall return according to the table below ]*/
            IOTHUB_CLIENT_RESULT uploadToBlob_result; 
#ifndef DONT_USE_UPLOADTOBLOB
            uploadToBlob_result = IoTHubClient_LL_UploadToBlob_SetOption(handleData->uploadToBlobHandle, optionName, value);
            if(uploadToBlob_result == IOTHUB_CLIENT_ERROR)
            {
                LogError("unable to IoTHubClient_LL_UploadToBlob_SetOption");
                result = IOTHUB_CLIENT_ERROR;
            }
#else
            uploadToBlob_result = IOTHUB_CLIENT_INVALID_ARG; /*harmless value (IOTHUB_CLIENT_INVALID_ARG)in the case when uploadtoblob is not compiled in, otherwise whatever IoTHubClient_LL_UploadToBlob_SetOption returned*/
#endif /*DONT_USE_UPLOADTOBLOB*/

                
            result =
                /*based on uploadToBlob_result value this is what happens:*/
                /*IOTHUB_CLIENT_INVALID_ARG always returns what IoTHubTransport_SetOption returns*/
                /*IOTHUB_CLIENT_ERROR always returns IOTHUB_CLIENT_ERROR */
                /*IOTHUB_CLIENT_OK returns OK
                    IOTHUB_CLIENT_OK if IoTHubTransport_SetOption returns OK or INVALID_ARG
                    IOTHUB_CLIENT_ERROR if IoTHubTransport_SetOption returns ERROR*/

                (uploadToBlob_result == IOTHUB_CLIENT_INVALID_ARG) ? handleData->IoTHubTransport_SetOption(handleData->transportHandle, optionName, value) :
                (uploadToBlob_result == IOTHUB_CLIENT_ERROR) ? IOTHUB_CLIENT_ERROR :
                (handleData->IoTHubTransport_SetOption(handleData->transportHandle, optionName, value) == IOTHUB_CLIENT_ERROR) ? IOTHUB_CLIENT_ERROR : IOTHUB_CLIENT_OK;

            if (result != IOTHUB_CLIENT_OK)
            {
                LogError("underlying transport failed, returned = %s", ENUM_TO_STRING(IOTHUB_CLIENT_RESULT, result));
            }
        }
    }
    return result;
}

#ifndef DONT_USE_UPLOADTOBLOB
IOTHUB_CLIENT_RESULT IoTHubClient_LL_UploadToBlob(IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle, const char* destinationFileName, const unsigned char* source, size_t size)
{
    IOTHUB_CLIENT_RESULT result;
    /*Codes_SRS_IOTHUBCLIENT_LL_02_061: [ If iotHubClientHandle is NULL then IoTHubClient_LL_UploadToBlob shall fail and return IOTHUB_CLIENT_INVALID_ARG. ]*/
    /*Codes_SRS_IOTHUBCLIENT_LL_02_062: [ If destinationFileName is NULL then IoTHubClient_LL_UploadToBlob shall fail and return IOTHUB_CLIENT_INVALID_ARG. ]*/
    if (
        (iotHubClientHandle == NULL) ||
        (destinationFileName == NULL) ||
        ((source == NULL) && (size >0))
        )
    {
        LogError("invalid parameters IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle=%p, const char* destinationFileName=%s, const unsigned char* source=%p, size_t size=%zu", iotHubClientHandle, destinationFileName, source, size);
        result = IOTHUB_CLIENT_INVALID_ARG;
    }
    else
    {
        result = IoTHubClient_LL_UploadToBlob_Impl(iotHubClientHandle->uploadToBlobHandle, destinationFileName, source, size);
    }
    return result;
}
#endif
