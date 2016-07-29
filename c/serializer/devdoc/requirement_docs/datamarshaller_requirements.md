# Data Marshaller

## References: 
JSON objects 

Data Marshaller is a module that produces for a set of pairs (property full path, property value) the data (that eventually contains a JSON object).
**SRS_DATA_MARSHALLER_99_002: [**  DataMarshaller shall have the following interface **]**

```c
#define DATA_MARSHALLER_RESULT_VALUES           \
DATA_MARSHALLER_OK,                             \
DATA_MARSHALLER_INVALID_ARG,                    \
DATA_MARSHALLER_NO_DEVICE_IDENTITY,             \
DATA_MARSHALLER_SCHEMA_FAILED,                  \
DATA_MARSHALLER_INVALID_MODEL_PROPERTY,         \
DATA_MARSHALLER_JSON_ENCODER_ERROR,             \
DATA_MARSHALLER_ERROR,                          \
DATA_MARSHALLER_AGENT_DATA_TYPES_ERROR,         \
DATA_MARSHALLER_MULTITREE_ERROR,                \

DEFINE_ENUM(DATA_MARSHALLER_RESULT, DATA_MARSHALLER_RESULT_VALUES);

typedef struct DATA_MARSHALLER_VALUE_TAG
{
    const char* PropertyPath;
    const AGENT_DATA_TYPE* Value;
} DATA_MARSHALLER_VALUE;

typedef void* DATA_MARSHALLER_HANDLE;

DATA_MARSHALLER_HANDLE DataMarshaller_Create(SCHEMA_MODEL_TYPE_HANDLE modelHandle, bool includePropertyPath);
extern void DataMarshaller_Destroy(DATA_MARSHALLER_HANDLE dataMarshallerHandle);
DATA_MARSHALLER_RESULT DataMarshaller_SendData(DATA_MARSHALLER_HANDLE dataMarshallerHandle, size_t valueCount, const DATA_MARSHALLER_VALUE* values, unsigned char** destination, size_t* destinationSize);
```

### DataMarshaller_Create
```c
DATA_MARSHALLER_HANDLE DataMarshaller_Create(SCHEMA_MODEL_TYPE_HANDLE modelHandle, bool includePropertyPath)
```

**SRS_DATA_MARSHALLER_99_018: [**  DataMarshaller_Create shall create a new DataMarshaller instance and on success it shall return a non NULL handle. **]**

**SRS_DATA_MARSHALLER_99_019: [**  DataMarshaller_Create shall return NULL if any argument is NULL. **]**

**SRS_DATA_MARSHALLER_99_048: [** On any other errors not explicitly specified, DataMarshaller_Create shall return NULL. **]**

### DataMarshaller_Destroy
```c
extern void DataMarshaller_Destroy(DATA_MARSHALLER_HANDLE dataMarshallerHandle);
```

**SRS_DATA_MARSHALLER_99_022: [**  DataMarshaller_Destroy shall free all resources associated with the dataMarshallerHandle argument. **]**

**SRS_DATA_MARSHALLER_99_024: [**  When called with a NULL handle, DataMarshaller_Destroy shall do nothing. **]**

### DataMarshaller_SendData
```c
DATA_MARSHALLER_RESULT DataMarshaller_SendData(DATA_MARSHALLER_HANDLE dataMarshallerHandle, size_t valueCount, const DATA_MARSHALLER_VALUE* values, unsigned char** destination, size_t* destinationSize)
```

DataMarshaller_SendData shall use JSON encoder and MultiTree to produce a JSON object from all the pairs of (model property full path, property value) and it shall provide the object in (*destination, destinationSize) pair of output parameters.

**SRS_DATA_MARSHALLER_99_003: [**  DATA_MARSHALLER_OK shall be returned when the function execution finishes successfully. **]**

**SRS_DATA_MARSHALLER_99_004: [**  DATA_MARSHALLER_INVALID_ARG shall be returned when the function has detected an invalid parameter (NULL) being passed to the function. **]**

**SRS_DATA_MARSHALLER_99_033: [**  DATA_MARSHALLER_INVALID_ARG shall be returned if the valueCount is zero. **]**

**SRS_DATA_MARSHALLER_99_042: [** The entity set name shall be obtained by calling Schema_GetModelRootName. **]**

**SRS_DATA_MARSHALLER_99_043: [** The namespace shall be obtained by using Schema_GetSchemaNamespace. **]**

**SRS_DATA_MARSHALLER_99_044: [** If any Schema APIs fail, DATA_MARSHALLER_SCHEMA_FAILED shall be returned. **]**

**SRS_DATA_MARSHALLER_99_007: [**  DATA_MARSHALLER_INVALID_MODEL_PROPERTY shall be returned when any of the items in values contain invalid data **]**

**SRS_DATA_MARSHALLER_99_034: [** All argument checks shall be performed before calling any other modules. **]**

**SRS_DATA_MARSHALLER_99_027: [**  DATA_MARSHALLER_JSON_ENCODER_ERROR shall be returned when JSONEncoder returns an error code. **]**

**SRS_DATA_MARSHALLER_99_037: [** DataMarshaller shall store as MultiTree the data to be encoded by the JSONEncoder module. **]**

**SRS_DATA_MARSHALLER_99_035: [** DATA_MARSHALLER_MULTITREE_ERROR shall be returned in case any MultiTree API call fails. **]**

**SRS_DATA_MARSHALLER_99_036: [** DATA_MARSHALLER_AGENT_DATA_TYPES_ERROR shall be returned in case any AgentTypeSystem APIs fails. **]**

**SRS_DATAMARSHALLER_02_007: [** DataMarshaller_SendData shall copy in the output parameters *destination, *destinationSize the content and the content length of the encoded JSON tree. **]**

**SRS_DATA_MARSHALLER_99_015: [**  DATA_MARSHALLER_ERROR shall be returned in all the other error cases not explicitly defined here. **]**

Remarks:
	**SRS_DATA_MARSHALLER_99_038: [** For each pair in the values argument, a string:value pair shall exist in the JSON object in the form of propertyName:value. **]**

**SRS_DATA_MARSHALLER_99_039: [**  If the includePropertyPath argument passed to DataMarshaller_Create was true each property shall be placed in the appropriate position in the JSON according to its path in the model. **]**


**SRS_DATAMARSHALLER_01_001: [** If the includePropertyPath argument passed to DataMarshaller_Create was false and only one struct is being sent, the relative path of the value passed to DataMarshaller_SendData – including property name - shall be ignored and the value shall be placed at JSON root. **]**

**SRS_DATAMARSHALLER_01_004: [** In this case the members of the struct shall be added as leafs into the MultiTree, each leaf having the name of the struct member. **]**

**SRS_DATAMARSHALLER_01_002: [** If the includePropertyPath argument passed to DataMarshaller_Create was false and the number of values passed to SendData is greater than 1 and at least one of them is a struct, DataMarshaller_SendData shall fallback to  including the complete property path in the output JSON. **]**



