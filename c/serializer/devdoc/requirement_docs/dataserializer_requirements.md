# Data Serializer 
References: 
Data Serializer is a module that facilitates the Serializing of data to/from a given format.  This module will rely on a function pointer to a given module to encode or decode a MultiTree to Buffer value.  The work of serializing the data will be put on the serialization modules that exposes the function pointer to be passed.

 
**SRS_DATA_SERIALIZER_07_001: [** DataSerializer will have the following interface **]**
```c
#define DATA_SERIALIZER_RESULT_VALUES           \
DATA_SERIALIZER_INVALID_ARG,                    \
DATA_SERIALIZER_ERROR							\
 
DEFINE_ENUM(DATA_SERIALIZER_RESULT, DATA_SERIALIZER_RESULT_VALUES);
 
#define DATA_SERIALIZER_MULTITREE_TYPE_VALUES   \
    DATA_SERIALIZER_TYPE_CHAR_PTR,              \
    DATA_SERIALIZER_TYPE_AGENT_DATA             \
 
DEFINE_ENUM(DATA_SERIALIZER_MULTITREE_TYPE, DATA_SERIALIZER_MULTITREE_TYPE_VALUES);
 
typedef BUFFER_HANDLE (*DATA_SERIALIZER_ENCODE_FUNC)(MULTITREE_HANDLE multiTreeHandle, DATA_SERIALIZER_MULTITREE_TYPE dataType);
typedef MULTITREE_HANDLE (*DATA_SERIALIZER_DECODE_FUNC)(BUFFER_HANDLE decodeData);
 
extern BUFFER_HANDLE DataSerializer_Encode(MULTITREE_HANDLE multiTreeHandle, DATA_SERIALIZER_MULTITREE_TYPE dataType, DATA_SERIALIZER_ENCODE_FUNC encodeFunc);
extern MULTITREE_HANDLE DataSerializer_Decode(BUFFER_HANDLE data, DATA_SERIALIZER_DECODE_FUNC decodeFunc);
```

### DataSerializer_Encode
```c
BUFFER* DataSerializer_Encode(MULTITREE_HANDLE multiTreeHandle, DATA_SERIALIZER_MULTITREE_TYPE dataType, DATA_SERIALIZER_ENCODE_FUNC encodeFunc);
```

**SRS_DATA_SERIALIZER_07_002: [** DataSerializer_Encode shall return a valid Buffer* when the function executes successfully. **]**

**SRS_DATA_SERIALIZER_07_003: [** NULL shall be returned when an invalid parameter is supplied. **]**

**SRS_DATA_SERIALIZER_07_009: [** DataSerializer_Encode function shall call into the given DATA_SERIALIZER_ENCODE_FUNC callback with a valid MULTITREE_HANDLE object. **]**

**SRS_DATA_SERIALIZER_07_010: [** Upon a DATA_SERIALIZER_ENCODE_FUNC failure the DataSerializer_Encode function shall return NULL. **]**

### DataSerializer_Decode
```c
MULTITREE_HANDLE DataSerializer_Decode(BUFFER* data, DATA_SERIALIZER_DECODE_FUNC decodeFunc);
```

**SRS_DATA_SERIALIZER_07_006: [** DataSerializer_Decode shall return a valid MULTITREE_HANDLE when the function executes successfully. **]**

**SRS_DATA_SERIALIZER_07_007: [** NULL shall be returned when an invalid parameter is supplied. **]**

**SRS_DATA_SERIALIZER_07_012: [** DataSerializer_Decode function shall call into the given DATA_SERIALIZER_DECODE_FUNC callback with a valid BUFFER object. **]**

**SRS_DATA_SERIALIZER_07_013: [** Upon a DATA_SERIALIZER_DECODE_FUNC callback failure the DataSerializer_Encode function shall return NULL. **]**



