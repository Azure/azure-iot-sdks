# IoTHubDeviceMethod Requirements

## Overview

IoTHubDeviceMethod allows to call method and receive result on a particular device.

## Exposed API

```c
#define IOTHUB_DEVICE_METHOD_RESULT_VALUES     \
    IOTHUB_DEVICE_METHOD_OK,                   \
    IOTHUB_DEVICE_METHOD_INVALID_ARG,          \
    IOTHUB_DEVICE_METHOD_ERROR,                \
    IOTHUB_DEVICE_METHOD_HTTPAPI_ERROR         \

DEFINE_ENUM(IOTHUB_DEVICE_METHOD_RESULT, IOTHUB_DEVICE_METHOD_RESULT_VALUES);

typedef struct IOTHUB_SERVICE_CLIENT_DEVICE_METHOD_TAG* IOTHUB_SERVICE_CLIENT_DEVICE_METHOD_HANDLE;

extern IOTHUB_SERVICE_CLIENT_DEVICE_METHOD_MANAGER_HANDLE IoTHubDeviceMethod_Create(IOTHUB_SERVICE_CLIENT_AUTH_HANDLE serviceClientHandle);
extern void IoTHubDeviceMethod_Destroy(IOTHUB_SERVICE_CLIENT_DEVICE_METHOD_MANAGER_HANDLE serviceClientDeviceMethodHandle);
char* IoTHubDeviceMethod_Invoke(IOTHUB_SERVICE_CLIENT_DEVICE_METHOD_HANDLE serviceClientDeviceMethodHandle, const char* deviceId, const char* methodName, const char* methodPayload, unsigned int timeout, unsigned char** response)
```


## IoTHubDeviceMethod_Create
```c
extern IOTHUB_SERVICE_CLIENT_DEVICE_METHOD_MANAGER_HANDLE IoTHubDeviceMethod_Create(IOTHUB_SERVICE_CLIENT_AUTH_HANDLE serviceClientHandle);
```
**SRS_IOTHUBDEVICEMETHOD_12_001: [** If the `serviceClientHandle` input parameter is `NULL` `IoTHubDeviceMethod_Create` shall return `NULL` **]**

**SRS_IOTHUBDEVICEMETHOD_12_002: [** If any member of the `serviceClientHandle` input parameter is NULL `IoTHubDeviceMethod_Create` shall return `NULL` **]**

**SRS_IOTHUBDEVICEMETHOD_12_003: [** `IoTHubDeviceMethod_Create` shall allocate memory for a new `IOTHUB_SERVICE_CLIENT_DEVICE_METHOD_HANDLE` instance **]**

**SRS_IOTHUBDEVICEMETHOD_12_004: [** If the allocation failed, `IoTHubDeviceMethod_Create` shall return `NULL` **]**

**SRS_IOTHUBDEVICEMETHOD_12_005: [** If the allocation is successful, `IoTHubDeviceMethod_Create` shall create a `IOTHUB_SERVICE_CLIENT_DEVICE_METHOD_HANDLE` from the given `IOTHUB_SERVICE_CLIENT_AUTH_HANDLE` and return with it **]**

**SRS_IOTHUBDEVICEMETHOD_12_006: [** `IoTHubDeviceMethod_Create` shall allocate memory and copy `hostName` to `result->hostName` by calling `mallocAndStrcpy_s`. **]**

**SRS_IOTHUBDEVICEMETHOD_12_007: [** If the `mallocAndStrcpy_s` fails, `IoTHubDeviceMethod_Create` shall do clean up and return `NULL`. **]**

**SRS_IOTHUBDEVICEMETHOD_12_012: [** `IoTHubDeviceMethod_Create` shall allocate memory and copy `sharedAccessKey` to `result->sharedAccessKey` by calling `mallocAndStrcpy_s`. **]**

**SRS_IOTHUBDEVICEMETHOD_12_013: [** If the `mallocAndStrcpy_s` fails, `IoTHubDeviceMethod_Create` shall do clean up and return `NULL`. **]**

**SRS_IOTHUBDEVICEMETHOD_12_014: [** `IoTHubDeviceMethod_Create` shall allocate memory and copy `keyName` to `result->keyName` by calling `mallocAndStrcpy_s`. **]**

**SRS_IOTHUBDEVICEMETHOD_12_015: [** If the mallocAndStrcpy_s fails, `IoTHubDeviceMethod_Create` shall do clean up and return `NULL`. **]**


## IoTHubDeviceMethod_Destroy
```c
void IoTHubDeviceMethod_Destroy(IOTHUB_SERVICE_CLIENT_DEVICE_METHOD_HANDLE serviceClientDeviceMethodHandle)
```
**SRS_IOTHUBDEVICEMETHOD_12_016: [** If the `serviceClientDeviceMethodHandle` input parameter is `NULL` `IoTHubDeviceMethod_Destroy` shall return **]**

**SRS_IOTHUBDEVICEMETHOD_12_017: [** If the `serviceClientDeviceMethodHandle` input parameter is not `NULL` `IoTHubDeviceMethod_Destroy` shall free the memory of it and return **]**


## IoTHubDeviceMethod_Invoke
```c
extern IOTHUB_DEVICE_METHOD_RESULT IoTHubDeviceMethod_Invoke(IOTHUB_SERVICE_CLIENT_DEVICE_METHOD_HANDLE serviceClientDeviceMethodHandle, const char* deviceId, const char* methodName, const char* methodPayload, unsigned int timeout, int* responseStatus, unsigned char** responsePayload, size_t* responsePayloadSize)
```
**SRS_IOTHUBDEVICEMETHOD_12_031: [** `IoTHubDeviceMethod_Invoke` shall verify the input parameters and if any of them (except the timeout) are `NULL` then return `IOTHUB_DEVICE_METHOD_INVALID_ARG` **]**

**SRS_IOTHUBDEVICEMETHOD_12_032: [** `IoTHubDeviceMethod_Invoke` shall create a BUFFER_HANDLE from `methodName`, `timeout` and `methodPayload` by calling `BUFFER_create` **]**

**SRS_IOTHUBDEVICEMETHOD_12_033: [** If the creation fails, `IoTHubDeviceMethod_Invoke` shall return `IOTHUB_DEVICE_METHOD_ERROR` **]**

**SRS_IOTHUBDEVICEMETHOD_12_034: [** `IoTHubDeviceMethod_Invoke` shall allocate memory for response buffer by calling `BUFFER_new` **]**

**SRS_IOTHUBDEVICEMETHOD_12_035: [** If the allocation failed, `IoTHubDeviceMethod_Invoke` shall return `IOTHUB_DEVICE_METHOD_ERROR` **]**

**SRS_IOTHUBDEVICEMETHOD_12_039: [** `IoTHubDeviceMethod_Invoke` shall create an HTTP POST request using methodPayloadBuffer **]**

**SRS_IOTHUBDEVICEMETHOD_12_040: [** `IoTHubDeviceMethod_Invoke` shall create an HTTP POST request using the following HTTP headers: authorization=sasToken,Request-Id=1001,Accept=application/json,Content-Type=application/json,charset=utf-8 **]**

**SRS_IOTHUBDEVICEMETHOD_12_041: [** `IoTHubDeviceMethod_Invoke` shall create an `HTTPAPIEX_SAS_HANDLE` handle by calling `HTTPAPIEX_SAS_Create` **]**

**SRS_IOTHUBDEVICEMETHOD_12_042: [** `IoTHubDeviceMethod_Invoke` shall create an `HTTPAPIEX_HANDLE` handle by calling `HTTPAPIEX_Create` **]**

**SRS_IOTHUBDEVICEMETHOD_12_043: [** `IoTHubDeviceMethod_Invoke` shall execute the HTTP POST request by calling `HTTPAPIEX_ExecuteRequest` **]**

**SRS_IOTHUBDEVICEMETHOD_12_044: [** If any of the call fails during the HTTP creation `IoTHubDeviceMethod_Invoke` shall fail and return `IOTHUB_DEVICE_METHOD_ERROR` **]**

**SRS_IOTHUBDEVICEMETHOD_12_045: [** If any of the HTTPAPI call fails `IoTHubDeviceMethod_Invoke` shall fail and return `IOTHUB_DEVICE_METHOD_ERROR` **]**

**SRS_IOTHUBDEVICEMETHOD_12_046: [** `IoTHubDeviceMethod_Invoke` shall verify the received HTTP status code and if it is not equal to 200 then return `IOTHUB_DEVICE_METHOD_ERROR` **]**

**SRS_IOTHUBDEVICEMETHOD_12_047: [** If parsing the response fails `IoTHubDeviceMethod_Invoke` shall return `IOTHUB_DEVICE_METHOD_ERROR` **]**

**SRS_IOTHUBDEVICEMETHOD_12_048: [** If memory allocation for output paramater fails `IoTHubDeviceMethod_Invoke` shall return `IOTHUB_DEVICE_METHOD_ERROR` **]**

**SRS_IOTHUBDEVICEMETHOD_12_049: [** Otherwise `IoTHubDeviceMethod_Invoke` shall save the received status and payload to the corresponding out parameter and return with `IOTHUB_DEVICE_METHOD_OK` **]**


