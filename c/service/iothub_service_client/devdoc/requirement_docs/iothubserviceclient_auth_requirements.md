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

**SRS_IOTHUBSERVICECLIENT_12_004: [** IoTHubServiceClientAuth_CreateFromConnectionString shall populate hostName, iotHubName, iotHubSuffix, sharedAccessKeyName, sharedAccessKeyValue from the given connection string by calling connectionstringparser_parse **]**

**SRS_IOTHUBSERVICECLIENT_12_005: [** If populating the IOTHUB_SERVICE_CLIENT_AUTH fails, IoTHubServiceClientAuth_CreateFromConnectionString shall do clean up and return NULL **]**

**SRS_IOTHUBSERVICECLIENT_12_006: [** If the IOTHUB_SERVICE_CLIENT_AUTH has been populated IoTHubServiceClientAuth_CreateFromConnectionString shall return with a IOTHUB_SERVICE_CLIENT_AUTH_HANDLE to it **]**



## IoTHubServiceClient_Destroy
```c
extern void IoTHubServiceClientAuth_Destroy(IOTHUB_SERVICE_CLIENT_AUTH_HANDLE serviceClientHandle);
```
**SRS_IOTHUBSERVICECLIENT_12_007: [** If the serviceClientHandle input parameter is NULL IoTHubServiceClient_Destroy shall return **]**

**SRS_IOTHUBSERVICECLIENT_12_008: [** If the serviceClientHandle input parameter is not NULL IoTHubServiceClient_Destroy shall free the memory of it and return **]**
