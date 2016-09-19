# IoTDevice Requirements
 
## Overview
The IoTDevice module shall create a handle to a device associated with a specified model. It will provide a way for consumers to access properties of the device (e.g., its name), and to free device resources when they're done with it.

## Exposed API
```c
#define DEVICE_RESULT_VALUES			\
    DEVICE_OK,							\
    DEVICE_INVALID_ARG,					\
    DEVICE_FRONTDOOR_FAILED,			\
    DEVICE_DATA_PUBLISHER_FAILED,		\
    DEVICE_COMMAND_DECODER_FAILED,		\
    DEVICE_ERROR

DEFINE_ENUM(DEVICE_RESULT, DEVICE_RESULT_VALUES)

typedef void* DEVICE_HANDLE;
typedef EXECUTE_COMMAND_RESULT (*pPfDeviceActionCallback)(DEVICE_HANDLE deviceHandle, void* callbackUserContext, const char* relativeActionPath, const char* actionName, size_t argCount, const AGENT_DATA_TYPE* args);
 
extern DEVICE_RESULT Device_Create(SCHEMA_MODEL_TYPE_HANDLE modelHandle, pPfDeviceActionCallback deviceActionCallback, void* callbackUserContext, bool includePropertyPath, DEVICE_HANDLE* deviceHandle);

extern void Device_Destroy(DEVICE_HANDLE deviceHandle);

extern const char* Device_GetName(DEVICE_HANDLE deviceHandle);

extern TRANSACTION_HANDLE Device_StartTransaction(DEVICE_HANDLE deviceHandle);
extern DEVICE_RESULT Device_PublishTransacted(TRANSACTION_HANDLE transactionHandle, const char* propertyPath, const AGENT_DATA_TYPE* data);
extern DEVICE_RESULT Device_EndTransaction(TRANSACTION_HANDLE transactionHandle, unsigned char** destination, size_t* destinationSize);
extern DEVICE_RESULT Device_CancelTransaction(TRANSACTION_HANDLE transactionHandle);

extern REPORTED_PROPERTIES_TRANSACTION_HANDLE Device_CreateTransaction_ReportedProperties(DEVICE_HANDLE deviceHandle);
extern DEVICE_RESULT Device_PublishTransacted_ReportedProperty(REPORTED_PROPERTIES_TRANSACTION_HANDLE transactionHandle, const char* reportedPropertyPath, const AGENT_DATA_TYPE*, data);
extern DEVICE_RESULT Device_CommitTransaction_ReportedProperties(REPORTED_PROPERTIES_TRANSACTION_HANDLE transactionHandle, unsigned char** destination, size_t* destinationSize);
extern void Device_DestroyTransaction_ReportedProperties(REPORTED_PROPERTIES_TRANSACTION_HANDLE transactionHandle);
extern DEVICE_RESULT Device_IngestDesiredProperties(void* startAddress, DEVICE_HANDLE deviceHandle, const char* desiredProperties);

extern EXECUTE_COMMAND_RESULT Device_ExecuteCommand(DEVICE_HANDLE deviceHandle, const char* command);
```c

### Device_Create
```c
extern DEVICE_RESULT Device_Create(SCHEMA_MODEL_TYPE_HANDLE modelHandle, pPfDeviceActionCallback deviceActionCallback, void* callbackUserContext, bool includePropertyPath, DEVICE_HANDLE* deviceHandle);
```

**SRS_DEVICE_03_003: [** The DEVICE_HANDLE shall be provided via the deviceHandle out argument. **]**

**SRS_DEVICE_03_004: [** Device_Create shall return DEVICE_OK upon success. **]**

**SRS_DEVICE_05_014: [** If any of the modelHandle, deviceHandle or deviceActionCallback arguments are NULL, Device_Create shall return DEVICE_INVALID_ARG. **]**

**SRS_DEVICE_01_018: [** Device_Create shall create a DataPublisher instance by calling DataPublisher_Create. **]**

**SRS_DEVICE_01_004: [** DeviceCreate shall pass to DataPublisher_create the includePropertyPath argument. **]**

**SRS_DEVICE_01_019: [** If creating the DataPublisher instance fails, Device_Create shall return DEVICE_DATA_PUBLISHER_FAILED. **]**

**SRS_DEVICE_01_020: [** Device_Create shall pass to DataPublisher_Create the FrontDoor instance obtained earlier. **]**

**SRS_DEVICE_01_001: [** Device_Create shall create a CommandDecoder instance by calling CommandDecoder_Create and passing to it the model handle. **]**

**SRS_DEVICE_01_002: [** Device_Create shall also pass to CommandDecoder_Create a callback to be invoked when a command is received and a context that shall be the device handle. **]**

**SRS_DEVICE_01_003: [** If CommandDecoder_Create fails, Device_Create shall return DEVICE_COMMAND_DECODER_FAILED. **]**

**SRS_DEVICE_05_015: [** If an error occurs while trying to create the device, Device_Create shall return DEVICE_ERROR. **]**

### Device_Destroy
```c
extern void Device_Destroy(DEVICE_HANDLE deviceHandle);
```

**SRS_DEVICE_03_006: [** Device_Destroy shall free all resources associated with a device. **]**

**SRS_DEVICE_03_007: [** Device_Destroy will not do anything if deviceHandle is NULL. **]**

### Device_StartTransaction

**SRS_DEVICE_01_034: [** Device_StartTransaction shall invoke DataPublisher_StartTransaction for the DataPublisher handle associated with the deviceHandle argument. **]**

**SRS_DEVICE_01_043: [** On success, Device_StartTransaction shall return a non NULL handle. **]**

**SRS_DEVICE_01_048: [** When DataPublisher_StartTransaction fails, Device_StartTransaction shall return NULL. **]**

**SRS_DEVICE_01_035: [** If any argument is NULL, Device_StartTransaction shall return NULL. **]**


### Device_PublishTransacted

**SRS_DEVICE_01_036: [** Device_PublishTransacted shall invoke DataPublisher_PublishTransacted. **]**

**SRS_DEVICE_01_044: [** On success, Device_PublishTransacted shall return DEVICE_OK. **]**

**SRS_DEVICE_01_049: [** When DataPublisher_PublishTransacted fails, Device_PublishTransacted shall return DEVICE_DATA_PUBLISHER_FAILED. **]**

**SRS_DEVICE_01_037: [** If any argument is NULL, Device_PublishTransacted shall return DEVICE_INVALID_ARG. **]**


### Device_EndTransaction

**SRS_DEVICE_01_038: [** Device_EndTransaction shall invoke DataPublisher_EndTransaction. **]**

**SRS_DEVICE_01_045: [** On success, Device_EndTransaction shall return DEVICE_OK. **]**

**SRS_DEVICE_01_050: [** When DataPublisher_EndTransaction fails, Device_EndTransaction shall return DEVICE_DATA_PUBLISHER_FAILED. **]**

**SRS_DEVICE_01_039: [** If any parameter is NULL, Device_EndTransaction shall return DEVICE_INVALID_ARG. **]**


### Device_CancelTransaction

**SRS_DEVICE_01_040: [** Device_CancelTransaction shall invoke DataPublisher_CancelTransaction. **]**

**SRS_DEVICE_01_046: [** On success, Device_PublishTransacted shall return DEVICE_OK. **]**

**SRS_DEVICE_01_051: [** When DataPublisher_CancelTransaction fails, Device_CancelTransaction shall return DEVICE_DATA_PUBLISHER_FAILED. **]**

**SRS_DEVICE_01_041: [** If any argument is NULL, Device_CancelTransaction shall return DEVICE_INVALID_ARG. **]**


### Invoking actions
**SRS_DEVICE_01_052: [** When the action callback passed to CommandDecoder is called, Device shall call the appropriate user callback associated with the device handle. **]**

**SRS_DEVICE_01_053: [** The action name, argument count and action arguments shall be passed to the user callback. **]**

**SRS_DEVICE_01_055: [** The value passed in callbackUserContext when creating the device shall be passed to the callback as the value for the callbackUserContext argument. **]**


**SRS_DEVICE_02_011: [** If the parameter actionCallbackContent passed the callback is NULL then the callback shall return EXECUTION_COMMAND_ERROR. **]**


```c
Device_ExecuteCommand extern EXECUTE_COMMAND_RESULT Device_ExecuteCommand(DEVICE_HANDLE deviceHandle, const char* command);
```

**SRS_DEVICE_02_012: [** If any parameters are NULL, then Device_ExecuteCommand shall return EXECUTE_COMMAND_ERROR. **]**

**SRS_DEVICE_02_013: [** Otherwise, Device_ExecuteCommand shall call CommandDecoder_ExecuteCommand and return what CommandDecoder_ExecuteCommand is returning. **]**

### Device_CreateTransaction_ReportedProperties
```c
REPORTED_PROPERTIES_TRANSACTION_HANDLE Device_CreateTransaction_ReportedProperties(DEVICE_HANDLE deviceHandle);
```

`Device_CreateTransaction_ReportedProperties` creates a handle to be used to transact reported properties.

**SRS_DEVICE_02_014: [** If argument `deviceHandle` is `NULL` then `Device_CreateTransaction_ReportedProperties` shall fail and return `NULL`. **]**

**SRS_DEVICE_02_015: [** Otherwise, `Device_CreateTransaction_ReportedProperties` shall call `DataPublisher_CreateTransaction_ReportedProperties`. **]**

**SRS_DEVICE_02_016: [** If `DataPublisher_CreateTransaction_ReportedProperties` fails then `Device_CreateTransaction_ReportedProperties` shall fail and return `NULL`. **]**

**SRS_DEVICE_02_017: [** Otherwise `Device_CreateTransaction_ReportedProperties` shall succeed and return a non-`NULL` value. **]**

### Device_PublishTransacted_ReportedProperty
```c
DEVICE_RESULT Device_PublishTransacted_ReportedProperty(REPORTED_PROPERTIES_TRANSACTION_HANDLE transactionHandle, const char* reportedPropertyPath, const AGENT_DATA_TYPE* data)
```

`Device_PublishTransacted_ReportedProperty` adds an individual reported property to a transaction of reported properties.

**SRS_DEVICE_02_018: [** If argument `transactionHandle` is `NULL` then `Device_PublishTransacted_ReportedProperty` shall fail and return `DEVICE_INVALID_ARG`. **]**

**SRS_DEVICE_02_019: [** If argument `reportedPropertyPath` is `NULL` then `Device_PublishTransacted_ReportedProperty` shall fail and return `DEVICE_INVALID_ARG`. **]**

**SRS_DEVICE_02_020: [** If argument `data` is `NULL` then `Device_PublishTransacted_ReportedProperty` shall fail and return `DEVICE_INVALID_ARG`. **]**

**SRS_DEVICE_02_021: [** `Device_PublishTransacted_ReportedProperty` shall call `DataPublisher_PublishTransacted_ReportedProperty`. **]**

**SRS_DEVICE_02_022: [** If `DataPublisher_PublishTransacted_ReportedProperty` fails then `Device_PublishTransacted_ReportedProperty` shall fail and return `DEVICE_DATA_PUBLISHER_FAILED`. **]**

**SRS_DEVICE_02_023: [** Otherwise, `Device_PublishTransacted_ReportedProperty` shall succeed and return `DEVICE_OK`. **]**

### Device_CommitTransaction_ReportedProperties
```c
DEVICE_RESULT Device_CommitTransaction_ReportedProperties(REPORTED_PROPERTIES_TRANSACTION_HANDLE transactionHandle, unsigned char** destination, size_t* destinationSize)
```

Device_CommitTransaction_ReportedProperties commits a transaction thus filling the parameters `destination` and `destinationSize`.

**SRS_DEVICE_02_024: [** If argument `transactionHandle` is `NULL` then `Device_CommitTransaction_ReportedProperties` shall fail and return `DEVICE_INVALID_ARG`. **]**

**SRS_DEVICE_02_025: [** If argument `destination` is `NULL` then `Device_CommitTransaction_ReportedProperties` shall fail and return `DEVICE_INVALID_ARG`. **]**

**SRS_DEVICE_02_026: [** If argument `destinationSize` is `NULL`then `Device_CommitTransaction_ReportedProperties` shall fail and return `DEVICE_INVALID_ARG`. **]**

**SRS_DEVICE_02_027: [** `Device_CommitTransaction_ReportedProperties` shall call `DataPublisher_CommitTransaction_ReportedProperties`. **]**

**SRS_DEVICE_02_028: [** If `DataPublisher_CommitTransaction_ReportedProperties` fails then `Device_CommitTransaction_ReportedProperties` shall fail and return `DEVICE_DATA_PUBLISHER_FAILED`. **]**

**SRS_DEVICE_02_029: [** Otherwise `Device_CommitTransaction_ReportedProperties` shall succeed and return `DEVICE_OK`. **]**

### Device_DestroyTransaction_ReportedProperties
```c
void Device_DestroyTransaction_ReportedProperties(REPORTED_PROPERTIES_TRANSACTION_HANDLE transactionHandle)
```

`Device_DestroyTransaction_ReportedProperties` frees all resources used by `transactionHandle`.

**SRS_DEVICE_02_030: [** If argument `transactionHandle` is `NULL` then `Device_DestroyTransaction_ReportedProperties` shall return. **]**

**SRS_DEVICE_02_031: [** Otherwise `Device_DestroyTransaction_ReportedProperties` shall free all used resources. **]**

### Device_IngestDesiredProperties
```c
DEVICE_RESULT Device_IngestDesiredProperties(void* startAddress, DEVICE_HANDLE deviceHandle, const char* desiredProperties);
```

`Device_IngestDesiredProperties` acts as a passthrough for desired properties towards CommandDecoder module.

**SRS_DEVICE_02_032: [** If `deviceHandle` is `NULL` then `Device_IngestDesiredProperties` shall fail and return `DEVICE_INVALID_ARG`. **]**

**SRS_DEVICE_02_033: [** If `desiredProperties` is `NULL` then `Device_IngestDesiredProperties` shall fail and return `DEVICE_INVALID_ARG`. **]**

**SRS_DEVICE_02_037: [** If `startAddress` is `NULL` then `Device_IngestDesiredProperties` shall fail and return `DEVICE_INVALID_ARG`. **]**

**SRS_DEVICE_02_034: [** `Device_IngestDesiredProperties` shall call `CommandDecoder_IngestDesiredProperties`. **]**

**SRS_DEVICE_02_035: [** If any failure happens then `Device_IngestDesiredProperties` shall fail and return `DEVICE_ERROR`. **]**

**SRS_DEVICE_02_036: [** Otherwise, `Device_IngestDesiredProperties` shall succeed and return `DEVICE_OK`. **]**