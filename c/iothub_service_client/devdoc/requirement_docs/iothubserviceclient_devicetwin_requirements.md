# IoTHubDeviceTwin Requirements

## Overview

IoTHubDeviceTwin allows retrieve tags and properties, update tags and desired state and receive twin notificatons.

## Exposed API

```c
#define IOTHUB_DEVICE_TWIN_RESULT_VALUES     \
    IOTHUB_DEVICE_TWIN_OK,                   \
    IOTHUB_DEVICE_TWIN_INVALID_ARG,          \
    IOTHUB_DEVICE_TWIN_ERROR,                \
    IOTHUB_DEVICE_TWIN_HTTPAPI_ERROR         \

DEFINE_ENUM(IOTHUB_DEVICE_TWIN_RESULT, IOTHUB_DEVICE_TWIN_RESULT_VALUES);

typedef struct IOTHUB_SERVICE_CLIENT_DEVICE_TWIN_TAG* IOTHUB_SERVICE_CLIENT_DEVICE_TWIN_HANDLE;

extern IOTHUB_SERVICE_CLIENT_DEVICE_TWIN_MANAGER_HANDLE IoTHubDeviceTwin_Create(IOTHUB_SERVICE_CLIENT_AUTH_HANDLE serviceClientHandle);
extern void IoTHubDeviceTwin_Destroy(IOTHUB_SERVICE_CLIENT_DEVICE_TWIN_MANAGER_HANDLE serviceClientDeviceTwinHandle);
extern char* IoTHubDeviceTwin_GetTwin(IOTHUB_SERVICE_CLIENT_DEVICE_TWIN_HANDLE serviceClientDeviceTwinHandle, const char* deviceId)
extern char* IoTHubDeviceTwin_UpdateTwin(IOTHUB_SERVICE_CLIENT_DEVICE_TWIN_HANDLE serviceClientDeviceTwinHandle, const char* deviceId, const char* deviceTwinJson)
```


## IoTHubDeviceTwin_Create
```c
extern IOTHUB_SERVICE_CLIENT_DEVICE_TWIN_MANAGER_HANDLE IoTHubDeviceTwin_Create(IOTHUB_SERVICE_CLIENT_AUTH_HANDLE serviceClientHandle);
```
**SRS_IOTHUBDEVICETWIN_12_001: [** If the `serviceClientHandle` input parameter is `NULL` `IoTHubDeviceTwin_Create` shall return `NULL` **]**

**SRS_IOTHUBDEVICETWIN_12_002: [** If any member of the `serviceClientHandle` input parameter is NULL `IoTHubDeviceTwin_Create` shall return `NULL` **]**

**SRS_IOTHUBDEVICETWIN_12_003: [** `IoTHubDeviceTwin_Create` shall allocate memory for a new `IOTHUB_SERVICE_CLIENT_DEVICE_TWIN_HANDLE` instance **]**

**SRS_IOTHUBDEVICETWIN_12_004: [** If the allocation failed, `IoTHubDeviceTwin_Create` shall return `NULL` **]**

**SRS_IOTHUBDEVICETWIN_12_005: [** If the allocation successful, `IoTHubDeviceTwin_Create` shall create a `IOTHUB_SERVICE_CLIENT_DEVICE_TWIN_HANDLE` from the given `IOTHUB_SERVICE_CLIENT_AUTH_HANDLE` and return with it **]**

**SRS_IOTHUBDEVICETWIN_12_006: [** `IoTHubDeviceTwin_Create` shall allocate memory and copy `hostName` to `result->hostName` by calling `mallocAndStrcpy_s`. **]**

**SRS_IOTHUBDEVICETWIN_12_007: [** If the `mallocAndStrcpy_s` fails, `IoTHubDeviceTwin_Create` shall do clean up and return `NULL`. **]**

**SRS_IOTHUBDEVICETWIN_12_012: [** `IoTHubDeviceTwin_Create` shall allocate memory and copy `sharedAccessKey` to `result->sharedAccessKey` by calling `mallocAndStrcpy_s`. **]**

**SRS_IOTHUBDEVICETWIN_12_013: [** If the `mallocAndStrcpy_s` fails, `IoTHubDeviceTwin_Create` shall do clean up and return `NULL`. **]**

**SRS_IOTHUBDEVICETWIN_12_014: [** `IoTHubDeviceTwin_Create` shall allocate memory and copy `keyName` to `result->keyName` by calling `mallocAndStrcpy_s`. **]**

**SRS_IOTHUBDEVICETWIN_12_015: [** If the mallocAndStrcpy_s fails, `IoTHubDeviceTwin_Create` shall do clean up and return `NULL`. **]**


## IoTHubDeviceTwin_Destroy
```c
void IoTHubDeviceTwin_Destroy(IOTHUB_SERVICE_CLIENT_DEVICE_TWIN_HANDLE serviceClientDeviceTwinHandle)
```
**SRS_IOTHUBDEVICETWIN_12_016: [** If the `serviceClientDeviceTwinHandle` input parameter is `NULL` `IoTHubDeviceTwin_Destroy` shall return **]**

**SRS_IOTHUBDEVICETWIN_12_017: [** If the `serviceClientDeviceTwinHandle` input parameter is not `NULL` `IoTHubDeviceTwin_Destroy` shall free the memory of it and return **]**


## IoTHubDeviceTwin_GetTwin
```c
extern char* IoTHubDeviceTwin_GetTwin(IOTHUB_SERVICE_CLIENT_DEVICE_TWIN_HANDLE serviceClientDeviceTwinHandle, const char* deviceId)
```
**SRS_IOTHUBDEVICETWIN_12_018: [** `IoTHubDeviceTwin_GetTwin` shall verify the input parameters and if any of them are `NULL` then return `NULL` **]**

**SRS_IOTHUBDEVICETWIN_12_019: [** `IoTHubDeviceTwin_GetTwin` shall create HTTP GET request URL using the given `deviceId` using the following format: url/twins/[deviceId] **]**

**SRS_IOTHUBDEVICETWIN_12_020: [** `IoTHubDeviceTwin_GetTwin` shall add the following headers to the created HTTP GET request: authorization=sasToken,Request-Id=1001,Accept=application/json,Content-Type=application/json,charset=utf-8 **]**

**SRS_IOTHUBDEVICETWIN_12_021: [** `IoTHubDeviceTwin_GetTwin` shall create an `HTTPAPIEX_SAS_HANDLE` handle by calling `HTTPAPIEX_SAS_Create` **]**

**SRS_IOTHUBDEVICETWIN_12_022: [** `IoTHubDeviceTwin_GetTwin` shall create an `HTTPAPIEX_HANDLE` handle by calling `HTTPAPIEX_Create` **]**

**SRS_IOTHUBDEVICETWIN_12_023: [** `IoTHubDeviceTwin_GetTwin` shall execute the HTTP GET request by calling `HTTPAPIEX_ExecuteRequest` **]**

**SRS_IOTHUBDEVICETWIN_12_024: [** If any of the call fails during the HTTP creation `IoTHubDeviceTwin_GetTwin` shall fail and return `NULL` **]**

**SRS_IOTHUBDEVICETWIN_12_025: [** If any of the HTTPAPI call fails `IoTHubDeviceTwin_GetTwin` shall fail and return `NULL` **]**

**SRS_IOTHUBDEVICETWIN_12_026: [** `IoTHubDeviceTwin_GetTwin` shall verify the received HTTP status code and if it is not equal to 200 then return `NULL` **]**

**SRS_IOTHUBDEVICETWIN_12_030: [** Otherwise `IoTHubDeviceTwin_GetTwin` shall save the received `deviceTwin` to the out parameter and return with it **]**


## IoTHubDeviceTwin_UpdateTwin
```c
extern char* IoTHubDeviceTwin_UpdateTwin(IOTHUB_SERVICE_CLIENT_DEVICE_TWIN_HANDLE serviceClientDeviceTwinHandle, const char* deviceId, const char* deviceTwinJson)
```
**SRS_IOTHUBDEVICETWIN_12_031: [** `IoTHubDeviceTwin_UpdateTwin` shall verify the input parameters and if any of them are `NULL` then return `NULL` **]**

**SRS_IOTHUBDEVICETWIN_12_032: [** `IoTHubDeviceTwin_UpdateTwin` shall create a BUFFER_HANDLE from `deviceTwinJson` by calling `BUFFER_create` **]**

**SRS_IOTHUBDEVICETWIN_12_033: [** If the creation fails, `IoTHubDeviceTwin_UpdateTwin` shall return `NULL` **]**

**SRS_IOTHUBDEVICETWIN_12_034: [** `IoTHubDeviceTwin_UpdateTwin` shall allocate memory for response buffer by calling `BUFFER_new` **]**

**SRS_IOTHUBDEVICETWIN_12_035: [** If the allocation failed, `IoTHubDeviceTwin_UpdateTwin` shall return `NULL` **]**

**SRS_IOTHUBDEVICETWIN_12_039: [** `IoTHubDeviceTwin_UpdateTwin` shall create an HTTP PATCH request using deviceTwinJson **]**

**SRS_IOTHUBDEVICETWIN_12_040: [** `IoTHubDeviceTwin_UpdateTwin` shall create an HTTP PATCH request using the createdfollowing HTTP headers: authorization=sasToken,Request-Id=1001,Accept=application/json,Content-Type=application/json,charset=utf-8 **]**

**SRS_IOTHUBDEVICETWIN_12_041: [** `IoTHubDeviceTwin_UpdateTwin` shall create an `HTTPAPIEX_SAS_HANDLE` handle by calling `HTTPAPIEX_SAS_Create` **]**

**SRS_IOTHUBDEVICETWIN_12_042: [** `IoTHubDeviceTwin_UpdateTwin` shall create an `HTTPAPIEX_HANDLE` handle by calling `HTTPAPIEX_Create` **]**

**SRS_IOTHUBDEVICETWIN_12_043: [** `IoTHubDeviceTwin_UpdateTwin` shall execute the HTTP PATCH request by calling `HTTPAPIEX_ExecuteRequest` **]**

**SRS_IOTHUBDEVICETWIN_12_044: [** If any of the call fails during the HTTP creation `IoTHubDeviceTwin_UpdateTwin` shall fail and return `NULL` **]**

**SRS_IOTHUBDEVICETWIN_12_045: [** If any of the HTTPAPI call fails `IoTHubDeviceTwin_UpdateTwin` shall fail and return `NULL` **]**

**SRS_IOTHUBDEVICETWIN_12_046: [** `IoTHubDeviceTwin_UpdateTwin` shall verify the received HTTP status code and if it is not equal to 200 then return `NULL` **]**

**SRS_IOTHUBDEVICETWIN_12_047: [** Otherwise `IoTHubDeviceTwin_UpdateTwin` shall save the received updated device twin to the out parameter and return with it **]**


