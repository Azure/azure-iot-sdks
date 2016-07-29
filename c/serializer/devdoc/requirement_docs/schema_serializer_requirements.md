# SchemaSerializer Requirements


## Overview

SchemaSerializer is a module that serializes the schema for one device to a JSON text.
## Exposed API
```c
#define SCHEMA_SERIALIZER_VALUES	\
    SCHEMA_SERIALIZER_OK,			\
    SCHEMA_SERIALIZER_INVALID_ARG,  \
    SCHEMA_SERIALIZER_ERROR

DEFINE_ENUM(SCHEMA_SERIALIZER_RESULT, SCHEMA_SERIALIZER_VALUES)

extern SCHEMA_SERIALIZER_RESULT SchemaSerializer_SerializeCommandMetadata(SCHEMA_MODEL_TYPE_HANDLE modelHandle, STRING_HANDLE schemaText);
SchemaSerializer_SerializeCommandMetadata
extern SCHEMA_SERIALIZER_RESULT SchemaSerializer_SerializeCommandMetadata(SCHEMA_MODEL_TYPE_HANDLE modelHandle, STRING_HANDLE schemaText);
```

**SRS_SCHEMA_SERIALIZER_01_001: [** SchemaSerializer_SerializeCommandMetadata shall serialize a specific model to a string using JSON as format. **]**

A sample JSON result looks like:
```json
[
      {
         "Name":"ChangeKey",
         "Parameters":[
            {
               "Name":"Key",
               "Type":"string"
            },
            {
               "Name":"Key2",
               "Type":"string"
            }
         ]
      },
      {
         "Name":"ChangeConfig",
         "Parameters":[
            {
               "Name":"AppConfig",
               "Type":"string"
            }
         ]
      }
]
```

**SRS_SCHEMA_SERIALIZER_01_002: [** Only commands shall be serialized, the properties of a model shall be ignored. **]**
**SRS_SCHEMA_SERIALIZER_01_003: [** The output JSON shall have an array, where each array element shall represent a command. **]**
**SRS_SCHEMA_SERIALIZER_01_005: [** Each array element shall be a JSON object. **]**
**SRS_SCHEMA_SERIALIZER_01_006: [** The object for a command shall have a member named "Name", whose value shall be the command name as obtained by using Schema APIs. **]**
**SRS_SCHEMA_SERIALIZER_01_007: [** The object for a command shall also have a "Parameters" member. **]**
**SRS_SCHEMA_SERIALIZER_01_008: [** The parameters member shall be an array, where each entry is a command parameter. **]**
**SRS_SCHEMA_SERIALIZER_01_009: [** Each command parameter shall have a member named "Name", that should have as value the command argument name as obtained by using Schema APIs. **]**
**SRS_SCHEMA_SERIALIZER_01_010: [** Each command parameter shall have a member named "Type", that should have as value the command argument type as obtained by using Schema APIs. **]**

**SRS_SCHEMA_SERIALIZER_01_011: [** The JSON text shall be built into the string indicated by the schemaText argument by using String APIs. **]**

**SRS_SCHEMA_SERIALIZER_01_012: [** On success SchemaSerializer_SerializeCommandMetadata shall return SCHEMA_SERIALIZER_OK. **]**

**SRS_SCHEMA_SERIALIZER_01_013: [** If the modelHandle argument is NULL, SchemaSerializer_SerializeCommandMetadata shall return SCHEMA_SERIALIZER_INVALID_ARG. **]**

**SRS_SCHEMA_SERIALIZER_01_014: [** If the schemaText argument is NULL, SchemaSerializer_SerializeCommandMetadata shall return SCHEMA_SERIALIZER_INVALID_ARG. **]**

**SRS_SCHEMA_SERIALIZER_01_015: [** If any of the Schema or String APIs fail then SchemaSerializer_SerializeCommandMetadata shall return SCHEMA_SERIALIZER_ERROR. **]**

The following mappings should be applied between when serializing the argument type:
**SRS_SCHEMA_SERIALIZER_01_016: [** "ascii_char_ptr" shall be translated to "string". **]**

**SRS_SCHEMA_SERIALIZER_01_017: [** All other types shall be kept as they are. **]**

