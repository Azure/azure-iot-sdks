# DataPublisher

 
## Overview
The DataPublisher module dispatches data to DataMarshaller and provides the capability of grouping several different properties/events into one transaction.

## Consumed APIs

**SRS_DATA_PUBLISHER_99_002: [**  The DataPublisher module shall make use of the Schema module APIs to query schema information. **]**

**SRS_DATA_PUBLISHER_99_039: [**  The DataPublisher module shall make use of the DataPublisher module APIs to dispatch data to be published. **]**


## Exposed API
**SRS_DATA_PUBLISHER_99_001: **[** DataPublisher shall expose the following API: **]**
```c
#define DATA_PUBLISHER_RESULT_VALUES    \
DATA_PUBLISHER_OK,                      \
DATA_PUBLISHER_INVALID_ARG,             \
DATA_PUBLISHER_MARSHALLER_ERROR,        \
DATA_PUBLISHER_EMPTY_TRANSACTION,       \
DATA_PUBLISHER_AGENT_DATA_TYPES_ERROR,  \
DATA_PUBLISHER_SCHEMA_FAILED,           \
DATA_PUBLISHER_BUFFER_STORAGE_ERROR,    \
DATA_PUBLISHER_ERROR

DEFINE_ENUM(DATA_PUBLISHER_RESULT, DATA_PUBLISHER_RESULT_VALUES);

typedef void* TRANSACTION_HANDLE;
typedef void* DATA_PUBLISHER_HANDLE;

extern DATA_PUBLISHER_HANDLE DataPublisher_Create(SCHEMA_MODEL_TYPE_HANDLE modelHandle, bool includePropertyPath);
extern void DataPublisher_Destroy(DATA_PUBLISHER_HANDLE dataPublisherHandle);

extern TRANSACTION_HANDLE DataPublisher_StartTransaction(DATA_PUBLISHER_HANDLE dataPublisherHandle);
extern DATA_PUBLISHER_RESULT DataPublisher_PublishTransacted(TRANSACTION_HANDLE transactionHandle, const char* propertyPath, const AGENT_DATA_TYPE* data);
extern DATA_PUBLISHER_RESULT DataPublisher_EndTransaction(TRANSACTION_HANDLE transactionHandle, unsigned char** destination, size_t* destinationSize)
;
extern DATA_PUBLISHER_RESULT DataPublisher_CancelTransaction(TRANSACTION_HANDLE transactionHandle);

extern void DataPublisher_SetMaxBufferSize(size_t value);
extern size_t DataPublisher_GetMaxBufferSize(void);
```c

### DataPublisher_Create
```c
extern DATA_PUBLISHER_HANDLE DataPublisher_Create(SCHEMA_MODEL_TYPE_HANDLE modelHandle, bool includePropertyPath);
```

**SRS_DATA_PUBLISHER_99_041: [**  DataPublisher_Create shall create a new DataPublisher instance and return a non-NULL handle in case of success. **]**

**SRS_DATA_PUBLISHER_99_042: [**  If a NULL argument is passed to it, DataPublisher_Create shall return NULL. **]**

**SRS_DATA_PUBLISHER_99_043: [**  DataPublisher_Create shall initialize and hold a handle to a DataMarshaller instance. **]**

**SRS_DATA_PUBLISHER_01_001: [** DataPublisher_Create shall pass the includePropertyPath argument to DataMarshaller_Create. **]**

**SRS_DATA_PUBLISHER_99_044: [**  If the creation of the DataMarshaller instance fails, DataPublisher_Create shall return NULL. **]**

**SRS_DATA_PUBLISHER_99_047: [**  For any other error not specified here, DataPublisher_Create shall return NULL. **]**


### DataPublisher_Destroy
```c
void DataPublisher_Destroy(DATA_PUBLISHER_HANDLE dataPublisherHandle);
```

**SRS_DATA_PUBLISHER_99_045: [**  DataPublisher_Destroy shall free all resources associated with a DataPublisher instance. **]**

**SRS_DATA_PUBLISHER_99_046: [**  If a NULL argument is passed to it, DataPublisher_Destroy shall do nothing. **]**

### DataPublisher_StartTransaction
```c
TRANSACTION_HANDLE DataPublisher_StartTransaction(DATA_PUBLISHER_HANDLE dataPublisherHandle);
```

**SRS_DATA_PUBLISHER_99_007: [**  A call to DataPublisher_StartTransaction shall start a new transaction. **]**

**SRS_DATA_PUBLISHER_99_008: [**  DataPublisher_StartTransaction shall return a non-NULL handle upon success. **]**

**SRS_DATA_PUBLISHER_99_038: [**  If DataPublisher_StartTransaction is called with a NULL argument it shall return NULL. **]**

**SRS_DATA_PUBLISHER_99_009: [**  DataPublisher_StartTransaction shall return NULL upon failure. **]**


### DataPublisher_EndTransaction
```c
DATA_PUBLISHER_RESULT DataPublisher_EndTransaction(TRANSACTION_HANDLE transactionHandle, bool isDelayed, unsigned char** destination, size_t* destinationSize)
```

**SRS_DATA_PUBLISHER_99_010: [**  A call to DataPublisher_EndTransaction shall mark the end of a transaction, trigger a dispatch of all the data grouped by that transaction. **]**

**SRS_DATA_PUBLISHER_99_026: [**  On success, DataPublisher_EndTransaction shall return DATA_PUBLISHER_OK. **]**

**SRS_DATA_PUBLISHER_99_011: [**  If the transactionHandle argument is NULL, DataPublisher_EndTransaction shall return DATA_PUBLISHER_INVALID_ARG. **]**

**SRS_DATA_PUBLISHER_02_006: [** If the destination argument is NULL, DataPublisher_EndTransaction shall return DATA_PUBLISHER_INVALID_ARG. **]**

**SRS_DATA_PUBLISHER_02_007: [** If the destinationSize argument is NULL, DataPublisher_EndTransaction shall return DATA_PUBLISHER_INVALID_ARG. **]**

**SRS_DATA_PUBLISHER_99_012: [**  DataPublisher_EndTransaction shall dispose of any resources associated with the transaction. **]**

**SRS_DATA_PUBLISHER_99_024: [**  If no values have been associated with the transaction, no data shall be dispatched to DataMarshaller, the transaction shall be discarded and DataPublisher_EndTransaction shall return DATA_PUBLISHER_EMPTY_TRANSACTION. **]**

**SRS_DATA_PUBLISHER_99_025: [**  When the DataMarshaller_SendData call fails, DataPublisher_EndTransaction shall return DATA_PUBLISHER_MARSHALLER_ERROR. **]**


### DataPublisher_CancelTransaction
```c
DATA_PUBLISHER_RESULT DataPublisher_CancelTransaction(TRANSACTION_HANDLE transactionHandle);
```

**SRS_DATA_PUBLISHER_99_013: [**  A call to DataPublisher_CancelTransaction shall dispose of the transaction without dispatching the data to the DataMarshaller module and it shall return DATA_PUBLISHER_OK. **]**

**SRS_DATA_PUBLISHER_99_014: [**  If the transactionHandle argument is NULL DataPublisher_CancelTransaction shall return DATA_PUBLISHER_INVALID_ARG. **]**

**SRS_DATA_PUBLISHER_99_015: [**  DataPublisher_CancelTransaction shall dispose of any resources associated with the transaction. **]**

### DataPublisher_PublishTransacted
```c
DATA_PUBLISHER_RESULT DataPublisher_PublishTransacted(TRANSACTION_HANDLE transactionHandle, const char* propertyPath, const AGENT_DATA_TYPE* data);
```

**SRS_DATA_PUBLISHER_99_016: [**  When DataPublisher_PublishTransacted is invoked, DataPublisher shall associate the data with the transaction identified by the transactionHandle argument and return DATA_PUBLISHER_OK. No data shall be dispatched at the time of the call. **]**

**SRS_DATA_PUBLISHER_99_017: [**  When one or more NULL parameter(s) are specified, DataPublisher_PublishTransacted shall return DATA_PUBLISHER_INVALID_ARG. **]**

**SRS_DATA_PUBLISHER_99_040: [**  When propertyPath does not exist in the supplied model, DataPublisher_Publish shall return DATA_PUBLISHER_SCHEMA_FAILED without dispatching data. **]**

**SRS_DATA_PUBLISHER_99_019: [**  If the same property is associated twice with a transaction, then the last value shall be kept associated with the transaction. **]**

**SRS_DATA_PUBLISHER_99_027: [**  DataPublisher shall make a copy of the data when associating it with the transaction by using AgentTypeSystem APIs. **]**

**SRS_DATA_PUBLISHER_99_028: [**  If creating the copy fails then DATA_PUBLISHER_AGENT_DATA_TYPES_ERROR shall be returned. **]**

### DataPublisher_SetMaxBufferSize
```c
void DataPublisher_SetMaxBufferSize(size_t value);
```

**SRS_DATA_PUBLISHER_99_065: [**  DataPublisher_SetMaxBufferSize shall directly update the value used to limit how much data (in bytes) can be buffered in the BufferStorage instance. **]**

**SRS_DATA_PUBLISHER_99_066: [**  A single value shall be used by all instances of DataPublisher. **]**

**SRS_DATA_PUBLISHER_99_067: [**  Before any call to DataPublisher_SetMaxBufferSize, the default max buffer size shall be equal to 10KB. **]**

**SRS_DATA_PUBLISHER_99_068: [**  The max buffer size value is passed as the parameter to BufferStorage_Create. **]**

### DataPublisher_GetMaxBufferSize
```c
size_t DataPublisher_GetMaxBufferSize(void);
```

**SRS_DATA_PUBLISHER_99_069: [**  DataMarshaller_GetMaxBufferSize shall return the current max buffer size value used by any new instance of DataMarshaller. **]**

Miscellaneous
**SRS_DATA_PUBLISHER_99_020: [**  For any errors not explicitly mentioned here the DataPublisher APIs shall return DATA_PUBLISHER_ERROR. **]**



