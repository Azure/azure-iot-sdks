# serializer_dt

serializer_dt.h is a header file that defines the macros an application can use to serialize and execute model commands in device twin context.
It inherits all the features of serializer.h. The glue code between serializer.h and iothub client is realized by the following functions: 

The new additions are:
```c

DECLARE_DEVICETWIN_MODEL(modelName, ... );

and several static functions:

static void serializer_ingest(DEVICE_TWIN_UPDATE_STATE update_state, const unsigned char* payLoad, size_t size, void* userContextCallback)
static void* IoTHubDeviceTwinCreate_Impl(const char* name, size_t sizeOfName, SERIALIZER_DEVICETWIN_PROTOHANDLE* protoHandle)
static void IoTHubDeviceTwin_Destroy_Impl(void* model)
```

### serializer_ingest
```c
void serializer_ingest(DEVICE_TWIN_UPDATE_STATE update_state, const unsigned char* payLoad, size_t size, void* userContextCallback)
```

`serializer_ingest` takes a payload consisting of `payLoad` and `size` (at the moment assumed to be containing a JSON value), removes clutter elements from it
and then updates the desired properties based on the payload.

**SRS_SERIALIZERDEVICETWIN_02_001: [** `serializer_ingest` shall clone the payload into a null terminated string. **]**

**SRS_SERIALIZERDEVICETWIN_02_002: [** `serializer_ingest` shall parse the null terminated string into parson data types. **]**

**SRS_SERIALIZERDEVICETWIN_02_003: [** If `update_state` is `DEVICE_TWIN_UPDATE_COMPLETE` then `serializer_ingest` shall locate "desired" json name. **]**

**SRS_SERIALIZERDEVICETWIN_02_004: [** If "desired" contains "$version" then `serializer_ingest` shall remove it. **]**

**SRS_SERIALIZERDEVICETWIN_02_005: [** The "desired" value shall be outputed to a null terminated string and `serializer_ingest` shall call `CodeFirst_IngestDesiredProperties`. **]**

**SRS_SERIALIZERDEVICETWIN_02_006: [** If `update_state` is `DEVICE_TWIN_UPDATE_PARTIAL` then `serializer_ingest` shall remove "$version" (if it exists). **]**

**SRS_SERIALIZERDEVICETWIN_02_007: [** The JSON shall be outputed to a null terminated string and `serializer_ingest` shall call `CodeFirst_IngestDesiredProperties`. **]**

**SRS_SERIALIZERDEVICETWIN_02_008: [** If any of the above operations fail, then `serializer_ingest` shall return. **]**

### IoTHubDeviceTwinCreate_Impl
```c
static void* IoTHubDeviceTwinCreate_Impl(const char* name, size_t sizeOfName, SERIALIZER_DEVICETWIN_PROTOHANDLE* protoHandle)
```

`IoTHubDeviceTwinCreate_Impl` creates a device of type `name`, links it to IoTHubClient or IoTHubClient_LL and returns it.

**SRS_SERIALIZERDEVICETWIN_02_009: [** `IoTHubDeviceTwinCreate_Impl` shall locate the model and the metadata for `name` by calling Schema_GetSchemaForModel/Schema_GetMetadata/Schema_GetModelByName. **]**

**SRS_SERIALIZERDEVICETWIN_02_010: [** `IoTHubDeviceTwinCreate_Impl` shall call CodeFirst_CreateDevice. **]**

**SRS_SERIALIZERDEVICETWIN_02_011: [** `IoTHubDeviceTwinCreate_Impl` shall set the device twin callback. **]**

**SRS_SERIALIZERDEVICETWIN_02_012: [** `IoTHubDeviceTwinCreate_Impl` shall record the pair of (device, IoTHubClient(_LL)). **]**

**SRS_SERIALIZERDEVICETWIN_02_013: [** If all operations complete successfully then `IoTHubDeviceTwinCreate_Impl` shall succeeds and return a non-`NULL` value. **]**

**SRS_SERIALIZERDEVICETWIN_02_014: [** Otherwise, `IoTHubDeviceTwinCreate_Impl` shall fail and return `NULL`. **]**

### IoTHubDeviceTwin_Destroy_Impl
```c
static void IoTHubDeviceTwin_Destroy_Impl(void* model)
```

`IoTHubDeviceTwin_Destroy_Impl` frees all used resources created by `IoTHubDeviceTwinCreate_Impl`.

**SRS_SERIALIZERDEVICETWIN_02_020: [** If `model` is `NULL` then `IoTHubDeviceTwin_Destroy_Impl` shall return. **]**

**SRS_SERIALIZERDEVICETWIN_02_015: [** `IoTHubDeviceTwin_Destroy_Impl` shall locate the saved handle belonging to `model`. **]**

**SRS_SERIALIZERDEVICETWIN_02_016: [** `IoTHubDeviceTwin_Destroy_Impl` shall set the devicetwin callback to `NULL`. **]**

**SRS_SERIALIZERDEVICETWIN_02_017: [** `IoTHubDeviceTwin_Destroy_Impl` shall call `CodeFirst_DestroyDevice`. **]**

**SRS_SERIALIZERDEVICETWIN_02_018: [** `IoTHubDeviceTwin_Destroy_Impl` shall remove the IoTHubClient_Handle and the device handle from the recorded set. **]**

**SRS_SERIALIZERDEVICETWIN_02_019: [** If the recorded set of IoTHubClient handles is zero size, then the set shall be destroyed. **]**









