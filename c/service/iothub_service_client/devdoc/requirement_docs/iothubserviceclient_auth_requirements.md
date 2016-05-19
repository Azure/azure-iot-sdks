# IotHubServiceClient Requirements

## Overview

IotHubServiceClient provides functionality for creating authentication info from IoTHub connection string.

## References

[IoT Hub SDK.doc](https://microsoft.sharepoint.com/teams/Azure_IoT/_layouts/15/WopiFrame.aspx?sourcedoc={9A552E4B-EC00-408F-AE9A-D8C2C37E904F}&file=IoT%20Hub%20SDK.docx&action=default)
[Connection string parser requirement]()

## Exposed API

```c
typedef struct IOTHUB_SERVICE_CLIENT_AUTH_TAG
{
    const char* hostname;
    const char* iothubName;
    const char* iothubSuffix;
    const char* sharedAccessKey;
    const char* keyName;
} IOTHUB_SERVICE_CLIENT_AUTH;

typedef struct IOTHUB_SERVICE_CLIENT_AUTH_TAG* IOTHUB_SERVICE_CLIENT_AUTH_HANDLE;

extern IOTHUB_SERVICE_CLIENT_AUTH_HANDLE IoTHubServiceClientAuth_CreateFromConnectionString(const char* connectionString);
extern void IoTHubServiceClientAuth_Destroy(IOTHUB_SERVICE_CLIENT_AUTH_HANDLE serviceClientHandle);
```


## IoTHubServiceClient_CreateFromConnectionString
```c
extern IOTHUB_SERVICE_CLIENT_AUTH_HANDLE IoTHubServiceClientAuth_CreateFromConnectionString(const char* connectionString);
```
**SRS_IOTHUBSERVICECLIENT_12_001: [** IoTHubServiceClientAuth_CreateFromConnectionString shall verify the input parameter and if it is NULL then return NULL **]**

**SRS_IOTHUBSERVICECLIENT_12_002: [** IoTHubServiceClientAuth_CreateFromConnectionString shall allocate memory for a new service client instance. **]**

**SRS_IOTHUBSERVICECLIENT_12_003: [** If the allocation failed, IoTHubServiceClientAuth_CreateFromConnectionString shall return NULL **]**

**SRS_IOTHUBSERVICECLIENT_12_009: [** IoTHubServiceClientAuth_CreateFromConnectionString shall create a STRING_HANDLE from the given connection string by calling STRING_construct. **]**

**SRS_IOTHUBSERVICECLIENT_12_010: [** If the STRING_construct fails, IoTHubServiceClientAuth_CreateFromConnectionString shall do clean up and return NULL. **]**

**SRS_IOTHUBSERVICECLIENT_12_004: [** IoTHubServiceClientAuth_CreateFromConnectionString shall populate hostName, iotHubName, iotHubSuffix, sharedAccessKeyName, sharedAccessKeyValue from the given connection string by calling connectionstringparser_parse **]**

**SRS_IOTHUBSERVICECLIENT_12_005: [** If populating the IOTHUB_SERVICE_CLIENT_AUTH fails, IoTHubServiceClientAuth_CreateFromConnectionString shall do clean up and return NULL **]**

**SRS_IOTHUBSERVICECLIENT_12_011: [** If the populating HostName fails, IoTHubServiceClientAuth_CreateFromConnectionString shall do clean up and return NULL. **]**

**SRS_IOTHUBSERVICECLIENT_12_012: [** If the populating SharedAccessKeyName fails, IoTHubServiceClientAuth_CreateFromConnectionString shall do clean up and return NULL. **]**

**SRS_IOTHUBSERVICECLIENT_12_013: [** If the populating SharedAccessKey fails, IoTHubServiceClientAuth_CreateFromConnectionString shall do clean up and return NULL. **]**

**SRS_IOTHUBSERVICECLIENT_12_038: [** IoTHubServiceClientAuth_CreateFromConnectionString shall create a STRING_handle from hostName by calling STRING_construct. **]**

**SRS_IOTHUBSERVICECLIENT_12_039: [** If the STRING_construct fails, IoTHubServiceClientAuth_CreateFromConnectionString shall do clean up and return NULL. **]**

**SRS_IOTHUBSERVICECLIENT_12_014: [** IoTHubServiceClientAuth_CreateFromConnectionString shall create a STRING_TOKENIZER to parse HostName by calling STRING_TOKENIZER_create. **]**

**SRS_IOTHUBSERVICECLIENT_12_015: [** If the STRING_TOKENIZER_create fails, IoTHubServiceClientAuth_CreateFromConnectionString shall do clean up and return NULL. **]**

**SRS_IOTHUBSERVICECLIENT_12_016: [** IoTHubServiceClientAuth_CreateFromConnectionString shall create a new STRING_HANDLE for token key string by calling STRING_new. **]**

**SRS_IOTHUBSERVICECLIENT_12_017: [** If the STRING_new fails, IoTHubServiceClientAuth_CreateFromConnectionString shall do clean up and return NULL. **]**

**SRS_IOTHUBSERVICECLIENT_12_018: [** IoTHubServiceClientAuth_CreateFromConnectionString shall create a new STRING_HANDLE for token value string by calling STRING_new. **]**

**SRS_IOTHUBSERVICECLIENT_12_019: [** If the STRING_new fails, IoTHubServiceClientAuth_CreateFromConnectionString shall do clean up and return NULL. **]**

**SRS_IOTHUBSERVICECLIENT_12_020: [** IoTHubServiceClientAuth_CreateFromConnectionString shall call STRING_TOKENIZER_get_next_token to get token key string. **]**

**SRS_IOTHUBSERVICECLIENT_12_021: [** If the STRING_TOKENIZER_get_next_token fails, IoTHubServiceClientAuth_CreateFromConnectionString shall do clean up and return NULL. **]**

**SRS_IOTHUBSERVICECLIENT_12_022: [** IoTHubServiceClientAuth_CreateFromConnectionString shall call STRING_TOKENIZER_get_next_token to get token value string. **]**

**SRS_IOTHUBSERVICECLIENT_12_023: [** If the STRING_TOKENIZER_get_next_token fails, IoTHubServiceClientAuth_CreateFromConnectionString shall do clean up and return NULL. **]**

**SRS_IOTHUBSERVICECLIENT_12_034: [** IoTHubServiceClientAuth_CreateFromConnectionString shall create C string from token key string handle by calling STRING_c_str. **]**

**SRS_IOTHUBSERVICECLIENT_12_035: [** If the STRING_c_str fails, IoTHubServiceClientAuth_CreateFromConnectionString shall do clean up and return NULL. **]**

**SRS_IOTHUBSERVICECLIENT_12_036: [** IoTHubServiceClientAuth_CreateFromConnectionString shall create C string from token value string handle by calling STRING_c_str. **]**

**SRS_IOTHUBSERVICECLIENT_12_037: [** If the mallocAndStrcpy_s fails, IoTHubServiceClientAuth_CreateFromConnectionString shall do clean up and return NULL. **]**

**SRS_IOTHUBSERVICECLIENT_12_024: [** IoTHubServiceClientAuth_CreateFromConnectionString shall allocate memory and copy hostName to result->hostName by calling mallocAndStrcpy_s. **]**

**SRS_IOTHUBSERVICECLIENT_12_025: [** If the mallocAndStrcpy_s fails, IoTHubServiceClientAuth_CreateFromConnectionString shall do clean up and return NULL. **]**

**SRS_IOTHUBSERVICECLIENT_12_026: [** IoTHubServiceClientAuth_CreateFromConnectionString shall allocate memory and copy keyName to result->keyName by calling mallocAndStrcpy_s. **]**

**SRS_IOTHUBSERVICECLIENT_12_027: [** If the mallocAndStrcpy_s fails, IoTHubServiceClientAuth_CreateFromConnectionString shall do clean up and return NULL. **]**

**SRS_IOTHUBSERVICECLIENT_12_028: [** IoTHubServiceClientAuth_CreateFromConnectionString shall allocate memory and copy sharedAccessKey to result->sharedAccessKey by calling mallocAndStrcpy_s. **]**

**SRS_IOTHUBSERVICECLIENT_12_029: [** If the mallocAndStrcpy_s fails, IoTHubServiceClientAuth_CreateFromConnectionString shall do clean up and return NULL. **]**

**SRS_IOTHUBSERVICECLIENT_12_030: [** IoTHubServiceClientAuth_CreateFromConnectionString shall allocate memory and copy iothubName to result->iothubName by calling mallocAndStrcpy_s. **]**

**SRS_IOTHUBSERVICECLIENT_12_031: [** If the mallocAndStrcpy_s fails, IoTHubServiceClientAuth_CreateFromConnectionString shall do clean up and return NULL. **]**

**SRS_IOTHUBSERVICECLIENT_12_032: [** IoTHubServiceClientAuth_CreateFromConnectionString shall allocate memory and copy iothubSuffix to result->iothubSuffix by calling mallocAndStrcpy_s. **]**

**SRS_IOTHUBSERVICECLIENT_12_033: [** If the mallocAndStrcpy_s fails, IoTHubServiceClientAuth_CreateFromConnectionString shall do clean up and return NULL. **]**

**SRS_IOTHUBSERVICECLIENT_12_006: [** If the IOTHUB_SERVICE_CLIENT_AUTH has been populated IoTHubServiceClientAuth_CreateFromConnectionString shall do clean up and return with a IOTHUB_SERVICE_CLIENT_AUTH_HANDLE to it **]**


## IoTHubServiceClient_Destroy
```c
extern void IoTHubServiceClientAuth_Destroy(IOTHUB_SERVICE_CLIENT_AUTH_HANDLE serviceClientHandle);
```
**SRS_IOTHUBSERVICECLIENT_12_007: [** If the serviceClientHandle input parameter is NULL IoTHubServiceClient_Destroy shall return **]**

**SRS_IOTHUBSERVICECLIENT_12_008: [** If the serviceClientHandle input parameter is not NULL IoTHubServiceClient_Destroy shall free the memory of it and return **]**
