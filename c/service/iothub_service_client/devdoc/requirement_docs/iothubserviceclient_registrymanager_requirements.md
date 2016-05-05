# IotHubRegistryManager Requirements

## Overview

IoTHubRegistryManager allows creating/updating/deleting and listing devices as well as querying statistics about the iot hub.

## References

([IoT Hub SDK.doc](https://microsoft.sharepoint.com/teams/Azure_IoT/_layouts/15/WopiFrame.aspx?sourcedoc={9A552E4B-EC00-408F-AE9A-D8C2C37E904F}&file=IoT%20Hub%20SDK.docx&action=default))
[Parson Json parser](http://kgabis.github.io/parson/)

## Exposed API


```c
#define IOTHUB_DEVICE_STATUS_VALUES       \
    IOTHUB_DEVICE_STATUS_ENABLED,         \
    IOTHUB_DEVICE_STATUS_DISABLED         \

DEFINE_ENUM(IOTHUB_DEVICE_STATUS, IOTHUB_DEVICE_STATUS_VALUES);

#define IOTHUB_DEVICE_CONNECTION_STATE_VALUES         \
    IOTHUB_DEVICE_CONNECTION_STATE_CONNECTED,         \
    IOTHUB_DEVICE_CONNECTION_STATE_NOT_CONNECTED      \

DEFINE_ENUM(IOTHUB_DEVICE_CONNECTION_STATE, IOTHUB_DEVICE_CONNECTION_STATE_VALUES);

#define IOTHUB_REGISTRYMANAGER_RESULT_VALUES        \
    IOTHUB_REGISTRYMANAGER_OK,                      \
    IOTHUB_REGISTRYMANAGER_INVALID_ARG,             \
    IOTHUB_REGISTRYMANAGER_ERROR,                   \
    IOTHUB_REGISTRYMANAGER_INVALID_JSON,            \
    IOTHUB_REGISTRYMANAGER_DEVICE_EXIST,            \
    IOTHUB_REGISTRYMANAGER_DEVICE_NOT_EXIST,        \
    IOTHUB_REGISTRYMANAGER_CALLBACK_NOT_SET         \

DEFINE_ENUM(IOTHUB_REGISTRYMANAGER_RESULT, IOTHUB_REGISTRYMANAGER_RESULT_VALUES);

typedef struct IOTHUB_DEVICE_TAG
{
    const char* deviceId;
    const char* primaryKey;
    const char* secondaryKey;
    const char* generationId;
    const char* eTag;
    const char* configuration;
    MAP_HANDLE systemProperties;
    MAP_HANDLE customProperties;
    MAP_HANDLE serviceProperties;
    IOTHUB_DEVICE_CONNECTION_STATE connectionState;
    const char* connectionstateUpdatedTime;
    IOTHUB_DEVICE_STATUS status;
    const char* statusReason;
    time_t statusUpdatedTime;
    time_t lastActivityTime;
    size_t cloudToDeviceMessageCount;
} IOTHUB_DEVICE;

typedef struct IOTHUB_REGISTRY_DEVICE_CREATE_TAG
{
    const char* deviceId;
    const char* primaryKey;
    const char* secondaryKey;
} IOTHUB_REGISTRY_DEVICE_CREATE;

typedef struct IOTHUB_REGISTRY_DEVICE_UPDATE_TAG
{
    const char* primaryKey;
    const char* secondaryKey;
    IOTHUB_DEVICE_STATUS status;
} IOTHUB_REGISTRY_DEVICE_UPDATE;

typedef struct IOTHUB_REGISTRY_STATISTIC_TAG
{
    size_t totalDeviceCount;
    size_t enabledDeviceCount;
    size_t disabledDeviceCount;
} IOTHUB_REGISTRY_STATISTICS;

typedef struct IOTHUB_REGISTRYMANAGER_TAG* IOTHUB_REGISTRYMANAGER_HANDLE;

extern IOTHUB_REGISTRYMANAGER_HANDLE IoTHubRegistryManager_Create(IOTHUB_REGISTRYMANAGER_AUTH_HANDLE serviceClientHandle);
extern void IoTHubRegistryManager_Destroy(IOTHUB_REGISTRYMANAGER_HANDLE registryManagerHandle);
extern IOTHUB_REGISTRYMANAGER_RESULT IoTHubRegistryManager_CreateDevice(IOTHUB_REGISTRYMANAGER_HANDLE registryManagerHandle, const IOTHUB_REGISTRY_DEVICE_CREATE* deviceCreate, IOTHUB_DEVICE* device);
extern IOTHUB_REGISTRYMANAGER_RESULT IoTHubRegistryManager_GetDevice(IOTHUB_REGISTRYMANAGER_HANDLE registryManagerHandle, const char* deviceId, IOTHUB_DEVICE* device);
extern IOTHUB_REGISTRYMANAGER_RESULT IoTHubRegistryManager_UpdateDevice(IOTHUB_REGISTRYMANAGER_HANDLE registryManagerHandle, IOTHUB_REGISTRY_DEVICE_UPDATE* deviceUpdate);
extern IOTHUB_REGISTRYMANAGER_RESULT IoTHubRegistryManager_DeleteDevice(IOTHUB_REGISTRYMANAGER_HANDLE registryManagerHandle, const char* deviceId);
extern IOTHUB_REGISTRYMANAGER_RESULT IoTHubRegistryManager_GetDeviceList(IOTHUB_REGISTRYMANAGER_HANDLE registryManagerHandle, size_t numberOfDevices, LIST_HANDLE deviceList);
extern IOTHUB_REGISTRYMANAGER_RESULT IoTHubRegistryManager_GetStatistics(IOTHUB_REGISTRYMANAGER_HANDLE registryManagerHandle, IOTHUB_REGISTRY_STATISTICS* registryStatistics);
```


## IoTHubRegistryManager_Create
```c
extern IOTHUB_REGISTRYMANAGER_HANDLE IoTHubRegistryManager_Create(IOTHUB_REGISTRYMANAGER_AUTH_HANDLE serviceClientHandle);
```
**SRS_IOTHUBREGISTRYMANAGER_12_001: [** If the serviceClientHandle input parameter is NULL IoTHubRegistryManager_Create shall return NULL **]**

**SRS_IOTHUBREGISTRYMANAGER_12_002: [** IoTHubRegistryManager_Create shall allocate memory for a new registry manager instance **]**

**SRS_IOTHUBREGISTRYMANAGER_12_003: [** If the allocation failed, IoTHubRegistryManager_Create shall return NULL **]**

**SRS_IOTHUBREGISTRYMANAGER_12_004: [** If the allocation successful, IoTHubRegistryManager_Create shall create a IOTHUB_REGISTRYMANAGER_HANDLE from the given IOTHUB_REGISTRYMANAGER_AUTH_HANDLE and return with it **]**



## IoTHubRegistryManager_Destroy
```c
extern void IoTHubRegistryManager_Destroy(IOTHUB_REGISTRYMANAGER_HANDLE registryManagerHandle);
```
**SRS_IOTHUBREGISTRYMANAGER_12_005: [** If the registryManagerHandle input parameter is NULL IoTHubRegistryManager_Destroy shall return **]**

**SRS_IOTHUBREGISTRYMANAGER_12_006: [** If the registryManagerHandle input parameter is not NULL IoTHubRegistryManager_Destroy shall free the memory of it and return **]**


## IoTHubRegistryManager_CreateDevice
```c
extern IOTHUB_REGISTRYMANAGER_RESULT IoTHubRegistryManager_CreateDevice(IOTHUB_REGISTRYMANAGER_HANDLE registryManagerHandle, const IOTHUB_REGISTRY_DEVICE_CREATE* deviceCreate, IOTHUB_DEVICE* device);
```
**SRS_IOTHUBREGISTRYMANAGER_12_007: [** IoTHubRegistryManager_CreateDevice shall verify the input parameters and if any of them are NULL then return IOTHUB_REGISTRYMANAGER_INVALID_ARG **]**

**SRS_IOTHUBREGISTRYMANAGER_12_008: [** IoTHubRegistryManager_CreateDevice shall verify the deviceCreateInfo->deviceId input parameter and if it is NULL then return IOTHUB_REGISTRYMANAGER_INVALID_ARG **]**

**SRS_IOTHUBREGISTRYMANAGER_12_009: [** IoTHubRegistryManager_CreateDevice shall verify the deviceCreateInfo->deviceId input parameter and if it contains space(s) then return IOTHUB_REGISTRYMANAGER_INVALID_ARG **]**

**SRS_IOTHUBREGISTRYMANAGER_12_010: [** IoTHubRegistryManager_CreateDevice shall create a flat "key1:value2,key2:value2..." JSON representation from the given deviceCreateInfo parameter using the following parson APIs: json_value_init_object, json_value_get_object, json_object_set_string, json_object_dotset_string **]**

**SRS_IOTHUBREGISTRYMANAGER_12_011: [** IoTHubRegistryManager_CreateDevice shall set the "deviceId" value to the deviceCreateInfo->deviceId **]**

**SRS_IOTHUBREGISTRYMANAGER_12_012: [** IoTHubRegistryManager_CreateDevice shall set the following key/value pairs to default value: etag:null,connectionState:Disconnected,status:enabled,statusReason:null,connectionStateUpdatedTime:0001-01-01T00:00:00,statusUpdatedTime:0001-01-01T00:00:00,lastActivityTime:0001-01-01T00:00:00,authentication:{symmetricKey:{primaryKey:null,secondaryKey:null}} **]**

**SRS_IOTHUBREGISTRYMANAGER_12_013: [** IoTHubRegistryManager_CreateDevice shall return IOTHUB_REGISTRYMANAGER_ERROR_CREATING_JSON if the JSON creation failed  **]**

**SRS_IOTHUBREGISTRYMANAGER_12_014: [** IoTHubRegistryManager_CreateDevice shall create an HTTP PUT request using the created JSON **]**

**SRS_IOTHUBREGISTRYMANAGER_12_015: [** IoTHubRegistryManager_CreateDevice shall create an HTTP PUT request using the following HTTP headers: authorization=sasToken,Request-Id=1001,Accept=application/json,Content-Type=application/json,charset=utf-8 **]**

**SRS_IOTHUBREGISTRYMANAGER_12_016: [** IoTHubRegistryManager_CreateDevice shall create an HTTPAPIEX_SAS_HANDLE handle by calling HTTPAPIEX_SAS_Create **]**

**SRS_IOTHUBREGISTRYMANAGER_12_017: [** IoTHubRegistryManager_CreateDevice shall create an HTTPAPIEX_HANDLE handle by calling HTTPAPIEX_Create **]**

**SRS_IOTHUBREGISTRYMANAGER_12_018: [** IoTHubRegistryManager_CreateDevice shall execute the HTTP PUT request by calling HTTPAPIEX_ExecuteRequest **]**

**SRS_IOTHUBREGISTRYMANAGER_12_019: [** If any of the HTTPAPI call fails IoTHubRegistryManager_CreateDevice shall fail and return IOTHUB_REGISTRYMANAGER_ERROR **]**

**SRS_IOTHUBREGISTRYMANAGER_12_020: [** IoTHubRegistryManager_CreateDevice shall verify the received HTTP status code and if it is 409 then return IOTHUB_REGISTRYMANAGER_DEVICE_EXIST **]**

**SRS_IOTHUBREGISTRYMANAGER_12_021: [** IoTHubRegistryManager_CreateDevice shall verify the received HTTP status code and if it is greater than 300 then return IOTHUB_REGISTRYMANAGER_ERROR **]**

**SRS_IOTHUBREGISTRYMANAGER_12_022: [** IoTHubRegistryManager_CreateDevice shall verify the received HTTP status code and if it is less or equal than 300 then try to parse the response JSON to deviceInfo **]**

**SRS_IOTHUBREGISTRYMANAGER_12_023: [** If the JSON parsing failed, IoTHubRegistryManager_CreateDevice shall return IOTHUB_REGISTRYMANAGER_INVALID_JSON **]**

**SRS_IOTHUBREGISTRYMANAGER_12_024: [** If the deviceInfo out parameter is not NULL IoTHubRegistryManager_CreateDevice shall save the received deviceInfo to the out parameter and return IOTHUB_REGISTRYMANAGER_OK **]**


## IoTHubRegistryManager_GetDevice
```c
extern IOTHUB_REGISTRYMANAGER_RESULT IoTHubRegistryManager_GetDevice(IOTHUB_REGISTRYMANAGER_HANDLE registryManagerHandle, const char* deviceId, IOTHUB_DEVICE* device);
```
**SRS_IOTHUBREGISTRYMANAGER_12_025: [** IoTHubRegistryManager_GetDevice shall verify the registryManagerHandle and deviceId input parameters and if any of them are NULL then return IOTHUB_REGISTRYMANAGER_INVALID_ARG **]**

**SRS_IOTHUBREGISTRYMANAGER_12_026: [** IoTHubRegistryManager_GetDevice shall create HTTP GET request URL using the given deviceId using the following format: url/devices/[deviceId]?api-version=2016-02-03  **]**

**SRS_IOTHUBREGISTRYMANAGER_12_027: [** IoTHubRegistryManager_GetDevice shall add the following headers to the created HTTP GET request: authorization=sasToken,Request-Id=1001,Accept=application/json,Content-Type=application/json,charset=utf-8 **]**

**SRS_IOTHUBREGISTRYMANAGER_12_028: [** IoTHubRegistryManager_GetDevice shall create an HTTPAPIEX_SAS_HANDLE handle by calling HTTPAPIEX_SAS_Create **]**

**SRS_IOTHUBREGISTRYMANAGER_12_029: [** IoTHubRegistryManager_GetDevice shall create an HTTPAPIEX_HANDLE handle by calling HTTPAPIEX_Create **]**

**SRS_IOTHUBREGISTRYMANAGER_12_030: [** IoTHubRegistryManager_GetDevice shall execute the HTTP GET request by calling HTTPAPIEX_ExecuteRequest **]**

**SRS_IOTHUBREGISTRYMANAGER_12_031: [** If any of the HTTPAPI call fails IoTHubRegistryManager_GetDevice shall fail and return IOTHUB_REGISTRYMANAGER_ERROR **]**

**SRS_IOTHUBREGISTRYMANAGER_12_032: [** IoTHubRegistryManager_GetDevice shall verify the received HTTP status code and if it is greater than 300 then return IOTHUB_REGISTRYMANAGER_ERROR **]**

**SRS_IOTHUBREGISTRYMANAGER_12_033: [** IoTHubRegistryManager_GetDevice shall verify the received HTTP status code and if it is less or equal than 300 then try to parse the response JSON to deviceInfo for the following properties: deviceId, primaryKey, secondaryKey, generationId, eTag, connectionState, connectionstateUpdatedTime, status, statusReason, statusUpdatedTime, lastActivityTime, cloudToDeviceMessageCount **]**

**SRS_IOTHUBREGISTRYMANAGER_12_034: [** If any of the property field above missing from the JSON the property value will not be populated **]**

**SRS_IOTHUBREGISTRYMANAGER_12_035: [** If the JSON parsing failed, IoTHubRegistryManager_GetDevice shall return IOTHUB_REGISTRYMANAGER_INVALID_JSON **]**

**SRS_IOTHUBREGISTRYMANAGER_12_036: [** If the received JSON is empty, IoTHubRegistryManager_GetDevice shall return IOTHUB_REGISTRYMANAGER_DEVICE_NOT_EXIST **]**

**SRS_IOTHUBREGISTRYMANAGER_12_037: [** If the deviceInfo out parameter if not NULL IoTHubRegistryManager_GetDevice shall save the received deviceInfo to the out parameter and return IOTHUB_REGISTRYMANAGER_OK **]**



## IoTHubRegistryManager_UpdateDevice
```c
extern IOTHUB_REGISTRYMANAGER_RESULT IoTHubRegistryManager_UpdateDevice(IOTHUB_REGISTRYMANAGER_HANDLE registryManagerHandle, IOTHUB_REGISTRY_DEVICE_UPDATE* deviceUpdate);
```
**SRS_IOTHUBREGISTRYMANAGER_12_038: [** IoTHubRegistryManager_UpdateDevice shall verify the input parameters and if any of them are NULL then return IOTHUB_REGISTRYMANAGER_INVALID_ARG **]**

**SRS_IOTHUBREGISTRYMANAGER_12_039: [** IoTHubRegistryManager_UpdateDevice shall verify the deviceCreateInfo->deviceId input parameter and if it is NULL then return IOTHUB_REGISTRYMANAGER_INVALID_ARG **]**

**SRS_IOTHUBREGISTRYMANAGER_12_040: [** IoTHubRegistryManager_UpdateDevice shall verify the deviceCreateInfo->deviceId input parameter and if it contains space(s) then return IOTHUB_REGISTRYMANAGER_INVALID_ARG **]**

**SRS_IOTHUBREGISTRYMANAGER_12_041: [** IoTHubRegistryManager_UpdateDevice shall create a flat "key1:value2,key2:value2..." JSON representation from the given deviceCreateInfo parameter using the following parson APIs: json_value_init_object, json_value_get_object, json_object_set_string, json_object_dotset_string **]**

**SRS_IOTHUBREGISTRYMANAGER_12_042: [** IoTHubRegistryManager_UpdateDevice shall return IOTHUB_REGISTRYMANAGER_INVALID_JSON if the JSON creation failed  **]**

**SRS_IOTHUBREGISTRYMANAGER_12_043: [** IoTHubRegistryManager_UpdateDevice shall create an HTTP PUT request using the created JSON **]**

**SRS_IOTHUBREGISTRYMANAGER_12_044: [** IoTHubRegistryManager_UpdateDevice shall create an HTTP PUT request using the createdfollowing HTTP headers: authorization=sasToken,Request-Id=1001,Accept=application/json,Content-Type=application/json,charset=utf-8 **]**

**SRS_IOTHUBREGISTRYMANAGER_12_045: [** IoTHubRegistryManager_UpdateDevice shall create an HTTPAPIEX_SAS_HANDLE handle by calling HTTPAPIEX_SAS_Create **]**

**SRS_IOTHUBREGISTRYMANAGER_12_046: [** IoTHubRegistryManager_UpdateDevice shall create an HTTPAPIEX_HANDLE handle by calling HTTPAPIEX_Create **]**

**SRS_IOTHUBREGISTRYMANAGER_12_047: [** IoTHubRegistryManager_UpdateDevice shall execute the HTTP PUT request by calling HTTPAPIEX_ExecuteRequest **]**

**SRS_IOTHUBREGISTRYMANAGER_12_048: [** IoTHubRegistryManager_UpdateDevice shall verify the received HTTP status code and if it is greater than 300 then return IOTHUB_REGISTRYMANAGER_ERROR **]**

**SRS_IOTHUBREGISTRYMANAGER_12_049: [** IoTHubRegistryManager_UpdateDevice shall verify the received HTTP status code and if it is less or equal than 300 then try to parse the response JSON to deviceInfo **]**

**SRS_IOTHUBREGISTRYMANAGER_12_050: [** If the JSON parsing failed, IoTHubRegistryManager_UpdateDevice shall return IOTHUB_REGISTRYMANAGER_INVALID_JSON **]**

**SRS_IOTHUBREGISTRYMANAGER_12_051: [** If the JSON parsing succesful IoTHubRegistryManager_UpdateDevice shall return IOTHUB_REGISTRYMANAGER_OK **]**



## IoTHubRegistryManager_DeleteDevice
```c
extern IOTHUB_REGISTRYMANAGER_RESULT IoTHubRegistryManager_DeleteDevice(IOTHUB_REGISTRYMANAGER_HANDLE registryManagerHandle, const char* deviceId);
```
**SRS_IOTHUBREGISTRYMANAGER_12_052: [** IoTHubRegistryManager_DeleteDevice shall verify the input parameters and if any of them are NULL then return IOTHUB_REGISTRYMANAGER_INVALID_ARG **]**

**SRS_IOTHUBREGISTRYMANAGER_12_053: [** IoTHubRegistryManager_DeleteDevice shall create HTTP DELETE request URL using the given deviceId using the following format: url/devices/[deviceId]?api-version  **]**

**SRS_IOTHUBREGISTRYMANAGER_12_054: [** IoTHubRegistryManager_DeleteDevice shall add the following headers to the created HTTP GET request: authorization=sasToken,Request-Id=1001,Accept=application/json,Content-Type=application/json,charset=utf-8 **]**

**SRS_IOTHUBREGISTRYMANAGER_12_055: [** IoTHubRegistryManager_DeleteDevice shall create an HTTPAPIEX_SAS_HANDLE handle by calling HTTPAPIEX_SAS_Create **]**

**SRS_IOTHUBREGISTRYMANAGER_12_056: [** IoTHubRegistryManager_DeleteDevice shall create an HTTPAPIEX_HANDLE handle by calling HTTPAPIEX_Create **]**

**SRS_IOTHUBREGISTRYMANAGER_12_057: [** IoTHubRegistryManager_DeleteDevice shall execute the HTTP DELETE request by calling HTTPAPIEX_ExecuteRequest **]**

**SRS_IOTHUBREGISTRYMANAGER_12_058: [** IoTHubRegistryManager_DeleteDevice shall verify the received HTTP status code and if it is greater than 300 then return IOTHUB_REGISTRYMANAGER_ERROR **]**

**SRS_IOTHUBREGISTRYMANAGER_12_059: [** IoTHubRegistryManager_DeleteDevice shall verify the received HTTP status code and if it is less or equal than 300 then return IOTHUB_REGISTRYMANAGER_OK **]**



## IoTHubRegistryManager_GetDeviceList
```c
extern IOTHUB_REGISTRYMANAGER_RESULT IoTHubRegistryManager_GetDeviceList(IOTHUB_REGISTRYMANAGER_HANDLE registryManagerHandle, size_t numberOfDevices, LIST_HANDLE deviceList);
```
**SRS_IOTHUBREGISTRYMANAGER_12_060: [** IoTHubRegistryManager_GetDeviceList shall verify the input parameters and if any of them are NULL then return IOTHUB_REGISTRYMANAGER_INVALID_ARG **]**

**SRS_IOTHUBREGISTRYMANAGER_12_061: [** IoTHubRegistryManager_GetDeviceList shall verify if the numberOfDevices input parameter equal or less 1000 and if it is not then return IOTHUB_REGISTRYMANAGER_INVALID_ARG **]**

**SRS_IOTHUBREGISTRYMANAGER_12_062: [** IoTHubRegistryManager_GetDeviceList shall create HTTP GET request for numberOfDevices using the follwoing format: url/devices/?top=[numberOfDevices]&api-version **]**

**SRS_IOTHUBREGISTRYMANAGER_12_063: [** IoTHubRegistryManager_GetDeviceList shall add the following headers to the created HTTP GET request: authorization=sasToken,Request-Id=1001,Accept=application/json,Content-Type=application/json,charset=utf-8 **]**

**SRS_IOTHUBREGISTRYMANAGER_12_064: [** IoTHubRegistryManager_GetDeviceList shall create an HTTPAPIEX_SAS_HANDLE handle by calling HTTPAPIEX_SAS_Create **]**

**SRS_IOTHUBREGISTRYMANAGER_12_065: [** IoTHubRegistryManager_GetDeviceList shall create an HTTPAPIEX_HANDLE handle by calling HTTPAPIEX_Create **]**

**SRS_IOTHUBREGISTRYMANAGER_12_066: [** IoTHubRegistryManager_GetDeviceList shall execute the HTTP GET request by calling HTTPAPIEX_ExecuteRequest **]**

**SRS_IOTHUBREGISTRYMANAGER_12_067: [** IoTHubRegistryManager_GetDeviceList shall verify the received HTTP status code and if it is greater than 300 then return IOTHUB_REGISTRYMANAGER_ERROR **]**

**SRS_IOTHUBREGISTRYMANAGER_12_068: [** IoTHubRegistryManager_GetDeviceList shall verify the received HTTP status code and if it is less or equal than 300 then try to parse the response JSON to deviceList **]**

**SRS_IOTHUBREGISTRYMANAGER_12_069: [** IoTHubRegistryManager_GetDeviceList shall use the following parson APIs to parse the response JSON: json_parse_string, json_value_get_object, json_object_get_string, json_object_dotget_string  **]**

**SRS_IOTHUBREGISTRYMANAGER_12_070: [** If any of the parson API fails, IoTHubRegistryManager_GetDeviceList shall return IOTHUB_REGISTRYMANAGER_INVALID_JSON **]**

**SRS_IOTHUBREGISTRYMANAGER_12_071: [** IoTHubRegistryManager_GetDeviceList shall populate the deviceList parameter with structures of type "IOTHUB_DEVICE" **]**

**SRS_IOTHUBREGISTRYMANAGER_12_072: [** If populating the deviceList parameter fails IoTHubRegistryManager_GetDeviceList shall return IOTHUB_REGISTRYMANAGER_ERROR **]**

**SRS_IOTHUBREGISTRYMANAGER_12_073: [** If populating the deviceList parameter succesful IoTHubRegistryManager_GetDeviceList shall return IOTHUB_REGISTRYMANAGER_OK **]**
 

## IoTHubRegistryManager_GetStatistics
```c
extern IOTHUB_REGISTRYMANAGER_RESULT IoTHubRegistryManager_GetStatistics(IOTHUB_REGISTRYMANAGER_HANDLE registryManagerHandle, IOTHUB_REGISTRY_STATISTICS* registryStatistics);
```
**SRS_IOTHUBREGISTRYMANAGER_12_074: [** IoTHubRegistryManager_GetStatistics shall verify the input parameters and if any of them are NULL then return IOTHUB_REGISTRYMANAGER_INVALID_ARG **]**

**SRS_IOTHUBREGISTRYMANAGER_12_075: [** IoTHubRegistryManager_GetStatistics shall create HTTP GET request for statistics using the following format: url/statistics/devices?api-version **]**

**SRS_IOTHUBREGISTRYMANAGER_12_076: [** IoTHubRegistryManager_GetStatistics shall add the following headers to the created HTTP GET request: authorization=sasToken,Request-Id=1001,Accept=application/json,Content-Type=application/json,charset=utf-8 **]**

**SRS_IOTHUBREGISTRYMANAGER_12_077: [** IoTHubRegistryManager_GetStatistics shall create an HTTPAPIEX_SAS_HANDLE handle by calling HTTPAPIEX_SAS_Create **]**

**SRS_IOTHUBREGISTRYMANAGER_12_078: [** IoTHubRegistryManager_GetStatistics shall create an HTTPAPIEX_HANDLE handle by calling HTTPAPIEX_Create **]**

**SRS_IOTHUBREGISTRYMANAGER_12_079: [** IoTHubRegistryManager_GetStatistics shall execute the HTTP GET request by calling HTTPAPIEX_ExecuteRequest **]**

**SRS_IOTHUBREGISTRYMANAGER_12_080: [** IoTHubRegistryManager_GetStatistics shall verify the received HTTP status code and if it is greater than 300 then return IOTHUB_REGISTRYMANAGER_ERROR **]**

**SRS_IOTHUBREGISTRYMANAGER_12_081: [** IoTHubRegistryManager_GetStatistics shall verify the received HTTP status code and if it is less or equal than 300 then use the following parson APIs to parse the response JSON to registry statistics structure: json_parse_string, json_value_get_object, json_object_get_string, json_object_dotget_string **]**

**SRS_IOTHUBREGISTRYMANAGER_12_082: [** If the parsing failed, IoTHubRegistryManager_GetStatistics shall return IOTHUB_REGISTRYMANAGER_ERROR **]**

**SRS_IOTHUBREGISTRYMANAGER_12_083: [** IoTHubRegistryManager_GetStatistics shall save the registry statistics to the out value and return IOTHUB_REGISTRYMANAGER_OK **]**
