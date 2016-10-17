# Schema Requirements


 
Overview
The Schema module provides APIs for describing struct types and model types. A model type can be used to instantiate a specific device.
Below is a layout if a schema:
Schema
```code 
 |--- <Schema Namespace>
 |--- <0..n Struct Type> 
 |	 |-- <Struct Name>
 |   |-- <0..n Property> (Primitive, Structs, User Defined)
 |   |-- <0..n ReportedProperty> (Primitive, Structs, User Defined)
 |   |-- <0..n DesiredProperty> (Primitive, Structs, User Defined)
 |	 |--- <0..n Model Type> 
 |            |--- <Model Name>
 |            |--- <0..n Property> (Primitive, Structs, User Defined) 
 |            |--- <0..n ReportedProperty> (Primitive, Structs, User Defined)
 |            |--- <0..n DesiredProperty> (Primitive, Structs, User Defined)
 |            |--- <0..n Actions>
 |            |--- <0..n Models>
 |--- <0..n Model Type> 
     |--- <Model Name>
     |--- <0..n Property> (Primitive, Structs, User Defined) 
     |--- <0..n ReportedProperty> (Primitive, Structs, User Defined)
     |--- <0..n DesiredProperty> (Primitive, Structs, User Defined)
     |--- <0..n Actions>
     |--- <0..n Models>
```

Exposed API
**SRS_SCHEMA_99_095: [** Schema shall expose the following API: **]**
```c
typedef struct SCHEMA_HANDLE_DATA_TAG* SCHEMA_HANDLE;
typedef struct SCHEMA_MODEL_TYPE_HANDLE_DATA_TAG* SCHEMA_MODEL_TYPE_HANDLE;
typedef struct SCHEMA_STRUCT_TYPE_HANDLE_DATA_TAG* SCHEMA_STRUCT_TYPE_HANDLE;
typedef struct SCHEMA_PROPERTY_HANDLE_DATA_TAG* SCHEMA_PROPERTY_HANDLE;
typedef struct SCHEMA_REPORTED_PROPERTY_HANDLE_DATA_TAG* SCHEMA_REPORTED_PROPERTY_HANDLE;
typedef struct SCHEMA_DESIRED_PROPERTY_HANDLE_DATA_TAG* SCHEMA_DESIRED_PROPERTY_HANDLE;
typedef struct SCHEMA_ACTION_HANDLE_DATA_TAG* SCHEMA_ACTION_HANDLE;
typedef struct SCHEMA_ACTION_ARGUMENT_HANDLE_DATA_TAG* SCHEMA_ACTION_ARGUMENT_HANDLE;

typedef void(*pfOnDesiredProperty)(void* model);
typedef int(*pfDesiredPropertyFromAGENT_DATA_TYPE)(const AGENT_DATA_TYPE* source, void* dest);
typedef void(*pfDesiredPropertyInitialize)(void* destination);
typedef void(*pfDesiredPropertyDeinitialize)(void* destination);


#define SCHEMA_RESULT_VALUES    \
SCHEMA_OK,                      \
SCHEMA_INVALID_ARG,             \
SCHEMA_DUPLICATE_ELEMENT,       \
SCHEMA_ELEMENT_NOT_FOUND,       \
SCHEMA_MODEL_IN_USE,            \
SCHEMA_DEVICE_COUNT_ZERO,       \
SCHEMA_ERROR

DEFINE_ENUM(SCHEMA_RESULT, SCHEMA_RESULT_VALUES)

#define SCHEMA_ELEMENT_TYPE_VALUES \
SCHEMA_NOT_FOUND, \
SCHEMA_SEARCH_INVALID_ARG, \
SCHEMA_MODEL, \
SCHEMA_STRUCT, \
SCHEMA_PROPERTY, \
SCHEMA_REPORTED_PROPERTY, \
SCHEMA_DESIRED_PROPERTY, \
SCHEMA_MODEL_ACTION, \
SCHEMA_MODEL_IN_MODEL

DEFINE_ENUM(SCHEMA_ELEMENT_TYPE, SCHEMA_ELEMENT_TYPE_VALUES);

typedef struct SCHEMA_MODEL_ELEMENT_TAG
{
    SCHEMA_ELEMENT_TYPE elementType;
    union ELEMENT_HANDLE_UNION_TAG
    {
        SCHEMA_DESIRED_PROPERTY_HANDLE desiredPropertyHandle;
        SCHEMA_PROPERTY_HANDLE propertyHandle;
        SCHEMA_REPORTED_PROPERTY_HANDLE reportedPropertyHandle;
        SCHEMA_ACTION_HANDLE actionHandle;
        SCHEMA_MODEL_TYPE_HANDLE modelHandle;
    } elementHandle;
}SCHEMA_MODEL_ELEMENT;

#include "azure_c_shared_utility/umock_c_prod.h"
extern SCHEMA_HANDLE Schema_Create(const char* schemaNamespace, void* metadata);
extern void* Schema_GetMetadata(SCHEMA_HANDLE schemaHandle);
extern size_t Schema_GetSchemaCount(void);
extern SCHEMA_HANDLE Schema_GetSchemaByNamespace(const char* schemaNamespace);
extern SCHEMA_HANDLE Schema_GetSchemaForModel(const char* modelName);
extern const char* Schema_GetSchemaNamespace(SCHEMA_HANDLE schemaHandle);
extern SCHEMA_RESULT Schema_AddDeviceRef(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle);
extern SCHEMA_RESULT Schema_ReleaseDeviceRef(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle);

extern SCHEMA_MODEL_TYPE_HANDLE Schema_CreateModelType(SCHEMA_HANDLE schemaHandle, const char* modelName);
extern SCHEMA_HANDLE Schema_GetSchemaForModelType(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle);
extern const char* Schema_GetModelName(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle);

extern SCHEMA_STRUCT_TYPE_HANDLE Schema_CreateStructType(SCHEMA_HANDLE schemaHandle, const char* structTypeName);

extern const char* Schema_GetStructTypeName(SCHEMA_STRUCT_TYPE_HANDLE structTypeHandle);

extern SCHEMA_RESULT Schema_AddStructTypeProperty(SCHEMA_STRUCT_TYPE_HANDLE structTypeHandle, const char* propertyName, const char* propertyType);

extern SCHEMA_RESULT Schema_AddModelProperty(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, const char* propertyName, const char* propertyType);
extern SCHEMA_RESULT Schema_AddModelReportedProperty(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, const char* reportedPropertyName, const char* reportedPropertyType);
extern SCHEMA_RESULT Schema_AddModelDesiredProperty(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, const char* desiredPropertyName, const char* desiredPropertyType, pfDesiredPropertyFromAGENT_DATA_TYPE desiredPropertyFromAGENT_DATA_TYPE, pfDesiredPropertyInitialize desiredPropertyInitialize, pfDesiredPropertyDeinitialize desiredPropertyDeinitialize, size_t offset, pfOnDesiredProperty onDesiredProperty);
extern SCHEMA_RESULT Schema_AddModelModel(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, const char* propertyName, SCHEMA_MODEL_TYPE_HANDLE modelType, size_t offset, pfOnDesiredProperty onDesiredProperty);
extern SCHEMA_ACTION_HANDLE Schema_CreateModelAction(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, const char* actionName);
extern SCHEMA_RESULT Schema_AddModelActionArgument(SCHEMA_ACTION_HANDLE actionHandle, const char* argumentName, const char* argumentType);
extern pfDesiredPropertyFromAGENT_DATA_TYPE Schema_GetModelDesiredProperty_pfDesiredPropertyFromAGENT_DATA_TYPE(SCHEMA_DESIRED_PROPERTY_HANDLE desiredPropertyHandle);
extern pfOnDesiredProperty Schema_GetModelDesiredProperty_pfOnDesiredProperty(SCHEMA_DESIRED_PROPERTY_HANDLE desiredPropertyHandle);
extern pfOnDesiredProperty Schema_GetModelModelByName_OnDesiredProperty(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, const char* propertyName);

extern size_t Schema_GetModelModelByName_Offset(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, const char* propertyName);
extern size_t Schema_GetModelModelByIndex_Offset(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, size_t index);

extern size_t Schema_GetModelDesiredProperty_offset(SCHEMA_DESIRED_PROPERTY_HANDLE desiredPropertyHandle);
extern size_t Schema_GetModelDesiredProperty_(SCHEMA_DESIRED_PROPERTY_HANDLE desiredPropertyHandle);
extern const char* Schema_GetModelDesiredPropertyType(SCHEMA_DESIRED_PROPERTY_HANDLE desiredPropertyHandle);
extern pfDesiredPropertyDeinitialize Schema_GetModelDesiredProperty_pfDesiredPropertyDeinitialize(SCHEMA_DESIRED_PROPERTY_HANDLE desiredPropertyHandle);
extern pfDesiredPropertyInitialize Schema_GetModelDesiredProperty_pfDesiredPropertyInitialize(SCHEMA_DESIRED_PROPERTY_HANDLE desiredPropertyHandle);

extern SCHEMA_MODEL_ELEMENT Schema_GetModelElementByName(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, const char* elementName);

extern SCHEMA_RESULT Schema_GetModelCount(SCHEMA_HANDLE schemaHandle, size_t* modelCount);
extern SCHEMA_MODEL_TYPE_HANDLE Schema_GetModelByName(SCHEMA_HANDLE schemaHandle, const char* modelName);
extern SCHEMA_MODEL_TYPE_HANDLE Schema_GetModelByIndex(SCHEMA_HANDLE schemaHandle, size_t index);

extern SCHEMA_RESULT Schema_GetModelPropertyCount(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, size_t* propertyCount);
extern SCHEMA_PROPERTY_HANDLE Schema_GetModelPropertyByName(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, const char* propertyName);
extern SCHEMA_PROPERTY_HANDLE Schema_GetModelPropertyByIndex(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, size_t index);

extern SCHEMA_RESULT Schema_GetModelReportedPropertyCount(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, size_t* reportedPropertyCount);
extern SCHEMA_REPORTED_PROPERTY_HANDLE Schema_GetModelReportedPropertyByName(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, const char* reportedPropertyName);
extern SCHEMA_REPORTED_PROPERTY_HANDLE Schema_GetModelReportedPropertyByIndex(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, size_t index);

extern SCHEMA_RESULT Schema_GetModelDesiredPropertyCount(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, size_t* desiredPropertyCount);
extern SCHEMA_DESIRED_PROPERTY_HANDLE Schema_GetModelDesiredPropertyByName(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, const char* desiredPropertyName);
extern SCHEMA_DESIRED_PROPERTY_HANDLE Schema_GetModelDesiredPropertyByIndex(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, size_t index);

extern SCHEMA_RESULT Schema_GetModelModelCount(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, size_t* modelCount);
extern SCHEMA_MODEL_TYPE_HANDLE Schema_GetModelModelByName(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, const char* propertyName);
extern SCHEMA_MODEL_TYPE_HANDLE Schema_GetModelModelyByIndex(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, size_t index);
extern const char* Schema_GetModelModelPropertyNameByIndex(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, size_t index);

extern _Bool Schema_ModelPropertyByPathExists(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, const char* propertyPath);

extern _Bool Schema_ModelReportedPropertyByPathExists(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, const char* reportedPropertyPath);
extern _Bool Schema_ModelDesiredPropertyByPathExists(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, const char* desiredPropertyPath);

extern SCHEMA_RESULT Schema_GetModelActionCount(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, size_t* actionCount);
extern SCHEMA_ACTION_HANDLE Schema_GetModelActionByName(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, const char* actionName);
extern SCHEMA_ACTION_HANDLE Schema_GetModelActionByIndex(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, size_t index);

extern SCHEMA_RESULT Schema_GetModelActionArgumentCount(SCHEMA_ACTION_HANDLE actionHandle, size_t* argumentCount);
extern const char* Schema_GetModelActionName(SCHEMA_ACTION_HANDLE actionHandle);

extern SCHEMA_ACTION_ARGUMENT_HANDLE Schema_GetModelActionArgumentByName(SCHEMA_ACTION_HANDLE actionHandle, const char* actionArgumentName);
extern SCHEMA_ACTION_ARGUMENT_HANDLE Schema_GetModelActionArgumentByIndex(SCHEMA_ACTION_HANDLE actionHandle, size_t argumentIndex);
extern const char* Schema_GetActionArgumentName(SCHEMA_ACTION_ARGUMENT_HANDLE actionArgumentHandle);
extern const char* Schema_GetActionArgumentType(SCHEMA_ACTION_ARGUMENT_HANDLE actionArgumentHandle);

extern SCHEMA_RESULT Schema_GetStructTypeCount(SCHEMA_HANDLE schemaHandle, size_t* structTypeCount);
extern SCHEMA_STRUCT_TYPE_HANDLE Schema_GetStructTypeByName(SCHEMA_HANDLE schemaHandle, const char* structTypeName);
extern SCHEMA_STRUCT_TYPE_HANDLE Schema_GetStructTypeByIndex(SCHEMA_HANDLE schemaHandle, size_t index);

extern SCHEMA_RESULT Schema_GetStructTypePropertyCount(SCHEMA_STRUCT_TYPE_HANDLE structTypeHandle, size_t* propertyCount);
extern SCHEMA_PROPERTY_HANDLE Schema_GetStructTypePropertyByName(SCHEMA_STRUCT_TYPE_HANDLE structTypeHandle, const char* propertyName);
extern SCHEMA_PROPERTY_HANDLE Schema_GetStructTypePropertyByIndex(SCHEMA_STRUCT_TYPE_HANDLE structTypeHandle, size_t index);
extern const char* Schema_GetPropertyName(SCHEMA_PROPERTY_HANDLE propertyHandle);
extern const char* Schema_GetPropertyType(SCHEMA_PROPERTY_HANDLE propertyHandle);

extern void Schema_Destroy(SCHEMA_HANDLE schemaHandle);
extern SCHEMA_RESULT Schema_DestroyIfUnused(SCHEMA_MODEL_TYPE_HANDLE modelHandle);
```

### Schema_Create
```c
extern SCHEMA_HANDLE Schema_Create(const char* schemaNamespace, void* metadata);
```

`Schema_Create` construct a schema for a name and metadata.

**SRS_SCHEMA_99_001: [** Schema_Create shall create a schema with a given namespace. **]**

**SRS_SCHEMA_99_002: [** On success a non-NULL handle to the newly created schema shall be returned. **]**

**SRS_SCHEMA_99_003: [** On failure, NULL shall be returned. **]**

**SRS_SCHEMA_99_004: [** If schemaNamespace is NULL, Schema_Create shall fail. **]**

**SRS_SCHEMA_02_090: [** If `metadata` is `NULL` then Schema_Create shall fail and return NULL. **]**

### Schema_GetMetadata
```c
extern void* Schema_GetMetadata(SCHEMA_HANDLE schemaHandle);
```

`Schema_GetMetadata` returns the saved pointer to the metadata of the schema.

**SRS_SCHEMA_02_091: [** If `schemaHandle` is `NULL` then `Schema_GetMetadata` shall fail and return `NULL`. **]**

**SRS_SCHEMA_02_092: [** Otherwise, `Schema_GetMetadata` shall succeed and return the saved metadata pointer. **]**

### Schema_GetSchemaForModel
```c
SCHEMA_HANDLE Schema_GetSchemaForModel(const char* modelName)
```

**SRS_SCHEMA_02_093: [** If `modelName` is `NULL` then `Schema_GetSchemaForModel` shall fail and return `NULL`. **]**

**SRS_SCHEMA_02_094: [** `Schema_GetSchemaForModel` shall find the SCHEMA_HANDLE that contains a model by name `modelName` and if found, succeed and return that. **]**

**SRS_SCHEMA_02_095: [** If the model is not found in any schema, then `Schema_GetSchemaForModel` shall fail and return `NULL`. **]**

### size_t Schema_GetSchemaCount(void);

**SRS_SCHEMA_99_153: [** Schema_GetSchemaCount shall return the number of “active” schemas (all schemas created with Schema_Create in the current process, for which Schema_Destroy has not been called). **]**

### SCHEMA_HANDLE Schema_GetSchemaByNamespace(const char* schemaNamespace);

**SRS_SCHEMA_99_148: [** Schema_GetSchemaByNamespace shall search all active schemas and return the schema with the namespace given by the schemaNamespace argument. **]**

**SRS_SCHEMA_99_151: [** If no active schema matches the schemaNamespace argument, Schema_GetSchemaByNamespace shall return NULL. **]**

**SRS_SCHEMA_99_150: [** If the schemaNamespace argument is NULL, Schema_GetSchemaByNamespace shall return NULL. **]**

### const char* Schema_GetNamespace(SCHEMA_HANDLE schemaHandle);

**SRS_SCHEMA_99_129: [** Schema_GetNamespace shall return the namespace for the schema identified by schemaHandle. **]**

**SRS_SCHEMA_99_130: [** If the schemaHandle argument is NULL, Schema_GetNamespace shall return NULL. **]**

### void Schema_Destroy(SCHEMA_HANDLE schemaHandle);

**SRS_SCHEMA_99_005: [** Schema_Destroy shall free all resources associated with a schema. **]**

**SRS_SCHEMA_99_006: [** If the schemaHandle is NULL, Schema_Destroy shall do nothing. **]**

### void Schema_DestroyAll(void);
**SRS_SCHEMA_99_152: [** Schema_DestroyAll shall free resources associated with all schemas created in the current process for which Schema_Destroy has not been called. **]**

### SCHEMA_MODEL_TYPE_HANDLE Schema_CreateModelType(SCHEMA_HANDLE schemaHandle, const char* modelName);

**SRS_SCHEMA_99_007: [** Schema_CreateModelType shall create a new model type and return a handle to it. **]**

**SRS_SCHEMA_99_008: [** On success, a non-NULL handle shall be returned. **]**

**SRS_SCHEMA_99_009: [** On failure, Schema_CreateModelType shall return NULL. **]**

**SRS_SCHEMA_99_010: [** If any of the arguments is NULL, Schema_CreateModelType shall fail. **]**

**SRS_SCHEMA_99_100: [** Schema_CreateModelType shall return SCHEMA_DUPLICATE_ELEMENT if modelName already exists. **]**

**SRS_SCHEMA_99_101: [** Schema_CreateModelType shall return SCHEMA_DUPLICATE_ELEMENT if a Struct type with same modelName already exists. **]**

### SCHEMA_HANDLE Schema_GetSchemaForModelType(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle);

**SRS_SCHEMA_99_131: [** Schema_GetSchemaForModelType returns the schema handle for a given model type. **]**

**SRS_SCHEMA_99_132: [** If the modelTypeHandle argument is NULL, Schema_GetSchemaForModelType shall return NULL. **]**

### SCHEMA_STRUCT_TYPE_HANDLE Schema_CreateStructType(SCHEMA_HANDLE schemaHandle, const char* structTypeName);

**SRS_SCHEMA_99_057: [** Schema_CreateStructType shall create a new struct type and return a handle to it. **]**

**SRS_SCHEMA_99_058: [** On success, a non-NULL handle shall be returned. **]**

**SRS_SCHEMA_99_060: [** If any of the arguments is NULL, Schema_CreateStructType shall return NULL. **]**

**SRS_SCHEMA_99_061: [** If a struct type with the same name already exists, Schema_CreateStructType shall return NULL. **]**

**SRS_SCHEMA_99_066: [** On any other error, Schema_CreateStructType shall return NULL. **]**

### extern const char* Schema_GetStructTypeName(SCHEMA_STRUCT_TYPE_HANDLE structTypeHandle);

**SRS_SCHEMA_99_135: [** Schema_GetStructTypeName shall return the name of a struct  identified by the structTypeHandle argument. **]**

**SRS_SCHEMA_99_136: [** If structTypeHandle is NULL, Schema_GetStructTypeName shall return NULL. **]**

### SCHEMA_RESULT Schema_AddStructTypeProperty(SCHEMA_STRUCT_TYPE_HANDLE structTypeHandle, const char* propertyName, const char* propertyType);

**SRS_SCHEMA_99_070: [** Schema_AddStructTypeProperty shall add one property to the struct type identified by structTypeHandle. **]**

**SRS_SCHEMA_99_071: [** On success, Schema_AddStructTypeProperty shall return SCHEMA_OK. **]**

**SRS_SCHEMA_99_072: [** If any of the arguments is NULL, Schema_AddStructTypeProperty shall return SCHEMA_INVALID_ARG. **]**

**SRS_SCHEMA_99_073: [** On any other error, Schema_ AddStructTypeProperty shall return SCHEMA_ERROR. **]**

**SRS_SCHEMA_99_074: [** The property name shall be unique per struct type, if the same property name is added twice to a struct type, SCHEMA_DUPLICATE_ELEMENT shall be returned. **]**

### SCHEMA_RESULT Schema_AddModelProperty(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, const char* propertyName, const char* propertyType);

**SRS_SCHEMA_99_011: [** Schema_AddModelProperty shall add one property to the model type identified by modelTypeHandle. **]**

**SRS_SCHEMA_99_012: [** On success, Schema_AddModelProperty shall return SCHEMA_OK. **]**

**SRS_SCHEMA_99_013: [** If any of the arguments is NULL, Schema_AddModelProperty shall return SCHEMA_INVALID_ARG. **]**

**SRS_SCHEMA_99_014: [** On any other error, Schema_AddModelProperty shall return SCHEMA_ERROR. **]**

**SRS_SCHEMA_99_015: [** The property name shall be unique per model, if the same property name is added twice to a model, SCHEMA_PROPERTY_ELEMENT_EXISTS shall be returned. **]**

### Schema_AddModelReportedProperty
```c
SCHEMA_RESULT Schema_AddModelReportedProperty(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, const char* reportedPropertyName, const char* reportedPropertyType);
```

`Schema_AddModelReportedProperty` adds one reported property to `modelTypeHandle`.

**SRS_SCHEMA_02_001: [** If `modelTypeHandle` is `NULL` then `Schema_AddModelReportedProperty` shall fail and return `SCHEMA_INVALID_ARG`. **]**

**SRS_SCHEMA_02_002: [** If `reportedPropertyName` is `NULL` then `Schema_AddModelReportedProperty` shall fail and return `SCHEMA_INVALID_ARG`. **]**

**SRS_SCHEMA_02_003: [** If `reportedPropertyType` is `NULL` then `Schema_AddModelReportedProperty` shall fail and return `SCHEMA_INVALID_ARG`. **]**

**SRS_SCHEMA_02_004: [** If `reportedPropertyName` has already been added then `Schema_AddModelReportedProperty` shall fail and return `SCHEMA_DUPLICATE_ELEMENT`. **]**

**SRS_SCHEMA_02_005: [** `Schema_AddModelReportedProperty` shall record `reportedPropertyName` and `reportedPropertyType`. **]**

**SRS_SCHEMA_02_006: [** If any error occurs then `Schema_AddModelReportedProperty` shall fail and return `SCHEMA_ERROR`. **]**

**SRS_SCHEMA_02_007: [** Otherwise `Schema_AddModelReportedProperty` shall succeed and return `SCHEMA_OK`. **]**


### SCHEMA_ACTION_HANDLE Schema_CreateModelAction(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, const char* actionName);

**SRS_SCHEMA_99_102: [** Schema_CreateModelAction shall add one action to the model type identified by modelTypeHandle. **]**

**SRS_SCHEMA_99_103: [** On success, Schema_CreateModelAction shall return a none-NULL SCHEMA_ACTION_HANDLE to the newly created action. **]**

**SRS_SCHEMA_99_104: [** If any of the modelTypeHandle or actionName arguments is NULL, Schema_CreateModelAction shall return NULL. **]**

**SRS_SCHEMA_99_105: [** The action name shall be unique per model, if the same action name is added twice to a model, Schema_CreateModelAction shall return NULL. **]**

**SRS_SCHEMA_99_106: [** On any other error, Schema_CreateModelAction shall return NULL. **]**

### SCHEMA_RESULT Schema_AddModelActionArgument(SCHEMA_ACTION_HANDLE actionHandle, const char* argumentName, const char* argumentType);

**SRS_SCHEMA_99_107: [** Schema_AddModelActionArgument shall add one argument name & type to an action identified by actionHandle. **]**

**SRS_SCHEMA_99_119: [** Schema_AddModelActionArgument shall preserve the order of the action arguments according to the order in which they were added, starting with index 0 for the first added argument. **]**

**SRS_SCHEMA_99_108: [** On success, Schema_AddModelActionArgunent shall return SCHEMA_OK. **]**

**SRS_SCHEMA_99_109: [** If any of the arguments is NULL, Schema_AddModelActionArgument shall return SCHEMA_INVALID_ARG. **]**

**SRS_SCHEMA_99_110: [** The argument name shall be unique per action, if the same name is added twice to an action, SCHEMA_DUPLICATE_ELEMENT shall be returned. **]**

**SRS_SCHEMA_99_111: [** Schema_AddModelActionArgument shall accept arguments with different names of the same type. **]**

**SRS_SCHEMA_99_112: [** On any other error, Schema_ AddModelActionArgumet shall return SCHEMA_ERROR. **]**

### SCHEMA_RESULT Schema_GetModelCount(SCHEMA_HANDLE schemaHandle, size_t* modelCount);

**SRS_SCHEMA_99_120: [** Schema_GetModelCount shall provide the number of models defined in the schema identified by schemaHandle. **]**

**SRS_SCHEMA_99_121: [** The count shall be provided via the out argument modelCount. **]**

**SRS_SCHEMA_99_122: [** On success, Schema_GetModelCount shall return SCHEMA_OK. **]**

**SRS_SCHEMA_99_123: [** Schema_GetModelCount shall return SCHEMA_INVALID_ARG if any of the arguments is NULL. **]**

### SCHEMA_MODEL_TYPE_HANDLE Schema_GetModelByName(SCHEMA_HANDLE schemaHandle, const char* modelName);

**SRS_SCHEMA_99_124: [** Schema_GetModelByName shall return a non-NULL SCHEMA_MODEL_TYPE_HANDLE corresponding to the model identified by schemaHandle and matching the modelName argument value. **]**

**SRS_SCHEMA_99_125: [** Schema_GetModelByName shall return NULL if unable to find a matching model, or if any of the arguments are NULL. **]**

### SCHEMA_MODEL_TYPE_HANDLE Schema_GetModelByIndex(SCHEMA_HANDLE schemaHandle, size_t index);

**SRS_SCHEMA_99_126: [** Schema_GetModelByIndex shall return a non-NULL SCHEMA_MODEL_TYPE_HANDLE corresponding to the model identified by schemaHandle and matching the index number provided by the index argument. **]**

**SRS_SCHEMA_99_127: [** The index argument is zero based, and the order in which models were added shall be the index in which they will be retrieved. **]**

**SRS_SCHEMA_99_128: [** Schema_GetModelByIndex shall return NULL if the index specified is outside the valid range or if schemaHandle argument is NULL. **]**

### SCHEMA_RESULT Schema_GetModelPropertyCount(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, size_t* propertyCount);

**SRS_SCHEMA_99_089: [** Schema_GetModelPropertyCount shall provide the number of properties defined in the model type identified by modelTypeHandle. **]**

**SRS_SCHEMA_99_090: [** The count shall be provided via the out argument propertyCount. **]**

**SRS_SCHEMA_99_091: [** On success, Schema_GetModelPropertyCount shall return SCHEMA_OK. **]**

**SRS_SCHEMA_99_092: [** Schema_GetModelPropertyCount shall return SCHEMA_INVALID_ARG if any of the arguments is NULL. **]**

### Schema_GetModelReportedPropertyCount
```c
SCHEMA_RESULT Schema_GetModelReportedPropertyCount(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, size_t* reportedPropertyCount);
```

`Schema_GetModelReportedPropertyCount` returns the number of reported properties for a given model.

**SRS_SCHEMA_02_008: [** If parameter `modelTypeHandle` is `NULL` then `Schema_GetModelReportedPropertyCount` shall fail and return `SCHEMA_INVALID_ARG`. **]**

**SRS_SCHEMA_02_009: [** If parameter `reportedPropertyCount` is NULL then `Schema_GetModelReportedPropertyCount` shall fail and return `SCHEMA_INVALID_ARG`. **]**

**SRS_SCHEMA_02_010: [** `Schema_GetModelReportedPropertyCount` shall provide in `reportedPropertyCount` the number of reported properties and return `SCHEMA_OK`. **]**

### Schema_GetModelReportedPropertyByName
```c
SCHEMA_REPORTED_PROPERTY_HANDLE Schema_GetModelReportedPropertyByName(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, const char* reportedPropertyName);
```

`Schema_GetModelReportedPropertyByName` returns `SCHEMA_REPORTED_PROPERTY_HANDLE` for a reported property given by its name (`reportedPropertyName`).

**SRS_SCHEMA_02_011: [** If argument `modelTypeHandle` is `NULL` then `Schema_GetModelReportedPropertyByName` shall fail and return `NULL`. **]**

**SRS_SCHEMA_02_012: [** If argument `reportedPropertyName` is `NULL` then `Schema_GetModelReportedPropertyByName` shall fail and return `NULL`. **]**

**SRS_SCHEMA_02_013: [** If reported property by the name `reportedPropertyName` exists then `Schema_GetModelReportedPropertyByName` shall succeed and return a non-`NULL` value. **]**

**SRS_SCHEMA_02_014: [** Otherwise `Schema_GetModelReportedPropertyByName` shall fail and return `NULL`. **]**

### Schema_GetModelReportedPropertyByIndex
```c
SCHEMA_REPORTED_PROPERTY_HANDLE Schema_GetModelReportedPropertyByIndex(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, size_t index);
```

`Schema_GetModelReportedPropertyByIndex` returns the `index`th reported property for a model.

**SRS_SCHEMA_02_015: [** If argument `modelTypeHandle` is `NULL` then `Schema_GetModelReportedPropertyByIndex` shall fail and return `NULL`. **]**

**SRS_SCHEMA_02_016: [** If a reported property with index equal to `index` exists then `Schema_GetModelReportedPropertyByIndex` shall succeed and return the non-`NULL` handle of that REPORTED_PROPERTY. **]**

**SRS_SCHEMA_02_017: [** Otherwise `Schema_GetModelReportedPropertyByIndex` shall fail and return `NULL`. **]**


### SCHEMA_PROPERTY_HANDLE Schema_GetModelPropertyByName(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, const char* propertyName);

**SRS_SCHEMA_99_036: [** Schema_GetModelPropertyByName shall return a non-NULL SCHEMA_PROPERTY_HANDLE corresponding to the model type identified by modelTypeHandle and matching the propertyName argument value. **]**

**SRS_SCHEMA_99_038: [** Schema_GetModelPropertyByName shall return NULL if unable to find a matching property or if any of the arguments are NULL. **]**

### SCHEMA_PROPERTY_HANDLE Schema_GetModelPropertyByIndex(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, size_t index);

**SRS_SCHEMA_99_093: [** Schema_GetModelPropertyByIndex shall return a non-NULL SCHEMA_PROPERTY_HANDLE corresponding to the model type identified by modelTypeHandle and matching the index number provided by the index argument. **]**

**SRS_SCHEMA_99_097: [** index is zero based, and the order in which actions were added shall be the index in which they will be retrieved. **]**

**SRS_SCHEMA_99_094: [** Schema_GetModelPropertyByIndex shall return NULL if the index specified is outside the valid range or if modelTypeHandle argument is NULL. **]**

### SCHEMA_RESULT Schema_GetModelActionCount(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, size_t* actionCount);

**SRS_SCHEMA_99_042: [** Schema_GetModelActionCount shall provide the total number of actions defined in a model type identified by the modelTypeHandle. **]**

**SRS_SCHEMA_99_043: [** The count shall be provided via the out argument actionCount. **]**

**SRS_SCHEMA_99_044: [** On success, Schema_GetModelActionCount shall return SCHEMA_OK. **]**

**SRS_SCHEMA_99_045: [** If any of the modelTypeHandle or actionCount arguments is NULL, Schema_GetModelActionCount shall return SCHEMA_INVALID_ARG. **]**

### SCHEMA_ACTION_HANDLE Schema_GetModelActionByName(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, const char* actionName);

**SRS_SCHEMA_99_040: [** Schema_GetModelActionByName shall return a non-NULL SCHEMA_ACTION_HANDLE corresponding to the model type identified by modelTypeHandle and matching the actionName argument value. **]**

**SRS_SCHEMA_99_041: [** Schema_GetModelActionByName shall return NULL if unable to find a matching action, if any of the arguments are NULL. **]**

### SCHEMA_ACTION_HANDLE Schema_GetModelActionByIndex(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, size_t index);

**SRS_SCHEMA_99_047: [** Schema_GetModelActionByIndex shall return a non-NULL SCHEMA_ACTION_HANDLE corresponding to the model type identified by modelTypeHandle and matching the index number provided by the index argument. **]**

**SRS_SCHEMA_99_096: [** index is zero based and the order in which actions were added shall be the index in which they will be retrieved. **]**

**SRS_SCHEMA_99_048: [** Schema_GetModelActionByIndex shall return NULL if the index specified is outside the valid range or if modelTypeHandle argument is NULL. **]**

### const char* Schema_GetModelActionName(SCHEMA_ACTION_HANDLE actionHandle);

**SRS_SCHEMA_99_049: [** Schema_GetModelActionName shall return the action name for a given action handle. **]**

**SRS_SCHEMA_99_050: [** If the actionHandle is NULL, Schema_GetModelActionName shall return NULL. **]**

### SCHEMA_RESULT Schema_GetModelActionArgumentCount(SCHEMA_ACTION_HANDLE actionHandle, size_t* argumentCount);

**SRS_SCHEMA_99_051: [** Schema_GetModelActionArgumentCount shall provide the number of arguments for a specific schema action identified by actionHandle. **]**

**SRS_SCHEMA_99_052: [** The argument count shall be provided via the out argument argumentCount. **]**

**SRS_SCHEMA_99_053: [** On success, Schema_GetModelActionArgumentCount shall return SCHEMA_OK. **]**

**SRS_SCHEMA_99_054: [** If any argument is NULL, Schema_GetModelActionArgumentCount shall return SCHEMA_INVALID_ARG. **]**

### SCHEMA_ACTION_ARGUMENT_HANDLE Schema_GetModelActionArgumentByName(SCHEMA_ACTION_HANDLE actionHandle, const char* actionArgumentName);

**SRS_SCHEMA_99_117: [** Schema_GetModelActionArgumentByName shall return a non-NULL handle corresponding to an action argument identified by the actionHandle and actionArgumentName. **]**

**SRS_SCHEMA_99_118: [** Schema_GetModelActionArgumentByName shall return NULL if unable to find a matching argument or if any of the arguments are NULL. **]**

### SCHEMA_ACTION_ARGUMENT_HANDLE Schema_GetModelActionArgumentByIndex(SCHEMA_ACTION_HANDLE actionHandle, size_t argumentIndex);

**SRS_SCHEMA_99_055: [** Schema_GetModelActionArgumentByIndex shall return a non-NULL SCHEMA_ACTION_ARGUMENT_HANDLE corresponding to the action identified by actionHandle and matching the index number provided by the argumentIndex argument where 0 is the index of the first argument added to the action. **]**

**SRS_SCHEMA_99_056: [** Schema_GetModelActionArgumentByIndex shall return NULL if the index specified is outside the valid range or if the actionHandle argument is NULL. **]**

### SCHEMA_STRUCT_TYPE_HANDLE Schema_GetStructTypeByName(SCHEMA_HANDLE schemaHandle, const char* structTypeName);

**SRS_SCHEMA_99_068: [** Schema_GetStructTypeByName shall return a non-NULL handle corresponding to the struct type identified by the structTypeName in the schemaHandle schema. **]**
**SRS_SCHEMA_99_069: [** Schema_GetStructTypeByName shall return NULL if unable to find a matching struct or if any of the arguments are NULL. **]**

### SCHEMA_RESULT Schema_GetStructTypeCount(SCHEMA_HANDLE schemaHandle, size_t* structTypeCount);

**SRS_SCHEMA_99_137: [** Schema_GetStructTypeCount shall provide the number of structs defined in the schema identified by schemaHandle. **]**

**SRS_SCHEMA_99_138: [** The count shall be provided via the out argument structTypeCount. **]**

**SRS_SCHEMA_99_139: [** On success, Schema_GetStructTypeCount shall return SCHEMA_OK. **]**

**SRS_SCHEMA_99_140: [** Schema_GetStructTypeCount shall return SCHEMA_INVALID_ARG if any of the arguments is NULL. **]**

### SCHEMA_STRUCT_TYPE_HANDLE Schema_GetStructTypeByIndex(SCHEMA_HANDLE schemaHandle, size_t index);

**SRS_SCHEMA_99_141: [** Schema_GetStructTypeByIndex shall return a non-NULL SCHEMA_STRUCT_TYPE_HANDLE corresponding to the struct type identified by schemaHandle and matching the index number provided by the index argument. **]**

**SRS_SCHEMA_99_142: [** The index argument is zero based, and the order in which models were added shall be the index in which they will be retrieved. **]**

**SRS_SCHEMA_99_143: [** Schema_GetStructTypeByIndex shall return NULL if the index specified is outside the valid range or if schemaHandle argument is NULL. **]**

### SCHEMA_RESULT Schema_GetStructTypePropertyCount(SCHEMA_STRUCT_TYPE_HANDLE structTypeHandle, size_t* propertyCount);

**SRS_SCHEMA_99_077: [** Schema_GetStructTypePropertyCount shall provide the total number of properties defined in a struct type identified by structTypeHandle. **]**

**SRS_SCHEMA_99_081: [** The count shall be provided via the out argument propertyCount. **]**

**SRS_SCHEMA_99_078: [** On success, Schema_ GetStructTypePropertyCount shall return SCHEMA_OK. **]**

**SRS_SCHEMA_99_079: [** Schema_GetStructTypePropertyCount shall return SCHEMA_INVALID_ARG if any of the structlTypeHandle or propertyCount arguments is NULL. **]**

### SCHEMA_PROPERTY_HANDLE Schema_GetStructTypePropertyByName(SCHEMA_STRUCT_TYPE_HANDLE structTypeHandle, const char* propertyName);

**SRS_SCHEMA_99_075: [** Schema_GetStructTypePropertyByName shall return a non-NULL handle corresponding to a property identified by the structTypeHandle and propertyName. **]**

**SRS_SCHEMA_99_076: [** Schema_GetStructTypePropertyByName shall return NULL if unable to find a matching property or if any of the arguments are NULL. **]**

### SCHEMA_PROPERTY_HANDLE Schema_GetStructTypePropertyByIndex(SCHEMA_STRUCT_TYPE_HANDLE structTypeHandle, size_t index);

**SRS_SCHEMA_99_082: [** Schema_GetStructTypePropertyByIndex shall return a non-NULL SCHEMA_PROPERTY_HANDLE corresponding to the struct type identified by strutTypeHandle and matching the index number provided by the index argument. **]**

**SRS_SCHEMA_99_098: [** index is zero based and the order in which actions were added shall be the index in which they will be retrieved. **]**

**SRS_SCHEMA_99_083: [** Schema_ GetStructTypeProperty shall return NULL if the index specified is outside the valid range, if structTypeHandle argument is NULL **]**
.

### const char* Schema_GetPropertyName(SCHEMA_PROPERTY_HANDLE propertyHandle);

**SRS_SCHEMA_99_085: [** Schema_GetPropertyName shall return the property name identified by the propertyHandle. **]**

**SRS_SCHEMA_99_086: [** If propertyHandle is NULL, Schema_GetPropertyName shall return NULL. **]**

### const char* Schema_GetPropertyType(SCHEMA_PROPERTY_HANDLE propertyHandle);

**SRS_SCHEMA_99_087: [** Schema_GetPropertyType shall return the property type identified by the propertyHandle. **]**

**SRS_SCHEMA_99_088: [** If propertyHandle is NULL, Schema_GetPropertyType shall return NULL. **]**

### const char* Schema_GetActionArgumentName(SCHEMA_ACTION_ARGUMENT_HANDLE actionArgumentHandle);

**SRS_SCHEMA_99_113: [** Schema_GetActionArgumentName shall return the argument name identified by the actionArgumentHandle. **]**

**SRS_SCHEMA_99_114: [** Schema_GetActionArgumentName shall return NULL if actionArgumentHandle is NULL. **]**

### const char* Schema_GetActionArgumentType(SCHEMA_ACTION_ARGUMENT_HANDLE actionArgumentHandle);

**SRS_SCHEMA_99_115: [** Schema_GetActionArgumentType shall return the argument type identified by the actionArgumentHandle. **]**

**SRS_SCHEMA_99_116: [** Schema_GetActionArgumentType shall return NULL if actionArgumentHandle is NULL. **]**

### Schema_GetModelName
```c
const char* Schema_GetModelName(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle);
```

**SRS_SCHEMA_99_160: [** Schema_GetModelName shall return the name of the model identified by modelTypeHandle. If the name cannot be retrieved, then NULL shall be returned. **]**

### Schema_AddModelModel
```c
SCHEMA_RESULT Schema_AddModelModel(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, const char* propertyName, SCHEMA_MODEL_TYPE_HANDLE modelType, size_t offset, pfOnDesiredProperty onDesiredProperty);
```

**SRS_SCHEMA_99_163: [** Schema_AddModelModel shall insert an existing model, identified by the handle modelType, into the existing model identified by modelTypeHandle under a property having the name propertyName. **]**

**SRS_SCHEMA_99_164: [** If the function succeeds, then the return value shall be SCHEMA_OK. **]**

**SRS_SCHEMA_99_165: [** If any of the parameters is NULL then Schema_AddModelModel shall return SCHEMA_INVALID_ARG. **]**

**SRS_SCHEMA_99_166: [** If propertyName is used for any other already existing element in the model, the function shall fail and return SCHEMA_DUPLICATE_ELEMENT. **]**

**SRS_SCHEMA_99_174: [** The function shall return SCHEMA_ERROR if any other error occurs. **]**

### Schema_GetModelModelCount
```c
SCHEMA_RESULT Schema_GetModelModelCount(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, size_t* modelCount);
```

**SRS_SCHEMA_99_167: [** Schema_GetModelModelCount shall return in parameter modelCount the number of models inserted in the model identified by parameter modelTypeHandle. **]**

**SRS_SCHEMA_99_168: [** If the function succeeds, it shall return SCHEMA_OK. **]**

**SRS_SCHEMA_99_169: [** If any of the parameters is NULL, then the function shall return SCHEMA_INVALID_ARG. **]**

### Schema_GetModelModelByName
```c
SCHEMA_MODEL_TYPE_HANDLE Schema_GetModelModelByName(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, const char* propertyName);
```

**SRS_SCHEMA_99_170: [** Schema_GetModelModelByName shall return a handle to the model identified by the property with the name propertyName in the model identified by the handle modelTypeHandle. **]**

**SRS_SCHEMA_99_171: [** If Schema_GetModelModelByName is unable to provide the handle it shall return NULL. **]**

### Schema_GetModelModelyByIndex
```c
SCHEMA_MODEL_TYPE_HANDLE Schema_GetModelModelyByIndex(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, size_t index);
```

**SRS_SCHEMA_99_172: [**  Schema_GetModelModelyByIndex shall return a handle to the "index"th model inserted in the model identified by the parameter modelTypeHandle. **]**
The order of the models in the model is the order they were added by calls to Schema_AddModelModel.
**SRS_SCHEMA_99_173: [** Schema_GetModelModelyByIndex shall return NULL in the cases when it cannot provide the handle. **]**

### Schema_GetModelModelPropertyNameByIndex
```c
const char* Schema_GetModelModelPropertyNameByIndex(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, size_t index);
```

**SRS_SCHEMA_99_175: [** Schema_GetModelModelPropertyNameByIndex shall return the name of the property for the "index"th model in the model identified by modelTypeHandle parameter. **]**

**SRS_SCHEMA_99_176: [** If Schema_GetModelModelPropertyNameByIndex cannot produce the property name, it shall return NULL. **]**

### Schema_ModelPropertyByPathExists
```c
extern bool Schema_ModelPropertyByPathExists(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, const char* propertyPath);
```

**SRS_SCHEMA_99_177: [** Schema_ModelPropertyByPathExists shall return true if a leaf property exists in the model modelTypeHandle. **]**

**SRS_SCHEMA_99_178: [** The argument propertyPath shall be used to find the leaf property. **]**

**SRS_SCHEMA_99_179: [** The propertyPath shall be assumed to be in the format model1/model2/.../propertyName. **]**

**SRS_SCHEMA_99_180: [** If any of the arguments are NULL, Schema_ModelPropertyByPathExists shall return false. **]**

**SRS_SCHEMA_99_181: [** If the property cannot be found Schema_ModelPropertyByPathExists shall return false. **]**

**SRS_SCHEMA_99_182: [** A single slash ('/') at the beginning of the path shall be ignored and the path shall still be valid. **]**
Example: /model1/PropertyName. 
**SRS_SCHEMA_99_183: [** If the path propertyPath points to a sub-model, Schema_ModelPropertyByPathExists shall return true. **]**

### Schema_ModelReportedPropertyByPathExists
```c
bool Schema_ModelReportedPropertyByPathExists(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, const char* reportedPropertyPath);
```

**SRS_SCHEMA_02_018: [** If argument `modelTypeHandle` is `NULL` then `Schema_ModelReportedPropertyByPathExists` shall fail and return `false`. **]**

**SRS_SCHEMA_02_019: [** If argument `reportedPropertyPath` is `NULL` then `Schema_ModelReportedPropertyByPathExists` shall fail and return `false`. **]**

**SRS_SCHEMA_02_020: [** `reportedPropertyPath` shall be assumed to be in the format model1/model2/.../reportedPropertyName. **]**

**SRS_SCHEMA_02_021: [** If the reported property cannot be found `Schema_ModelReportedPropertyByPathExists` shall fail and return `false`. **]**

**SRS_SCHEMA_02_022: [** If the path reportedPropertyPath points to a sub-model, `Schema_ModelReportedPropertyByPathExists` shall succeed and `true`. **]**

**SRS_SCHEMA_02_023: [** If `reportedPropertyPath` exists then `Schema_ModelReportedPropertyByPathExists` shall succeed and return `true` **]**

### Schema_ReleaseDeviceRef
```c
void Schema_RemoveDeviceRef(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle);
```

**SRS_SCHEMA_07_184: [** Schema_DeviceRelease shall do nothing if the supplied modelTypeHandle is NULL. **]**

**SRS_SCHEMA_07_186: [** On a nonNULL SCHEMA_MODEL_TYPE_HANDLE if the DeviceCount variable is > 0 then the variable will be decremented. **]**

### Schema_AddDeviceRef
```c
SCHEMA_RESULT Schema_AddDeviceRef(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle);
```

**SRS_SCHEMA_07_187: [** Schema_AddDeviceRef shall return SCHEMA_INVALID_ARG if modelTypeHandle is NULL. **]**

**SRS_SCHEMA_07_188: [** If the modelTypeHandle is nonNULL, Schema_AddDeviceRef shall increment the MODEL_TYPE DeviceCount variable. **]**

### Schema_DestroyIfUnused
```c
void Schema_DestroyIfUnused(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle);
```

**SRS_SCHEMA_07_189: [** If modelTypeHandle variable is NULL Schema_DestroyIfUnused shall do nothing. **]**

**SRS_SCHEMA_07_190: [** Schema_DestroyIfUnused shall iterate through the ModuleTypeHandle objects and if all the DeviceCount variables == 0 then it will delete the schemaHandle. **]**

**SRS_SCHEMA_07_191: [** If 1 or more DeviceCount variables are > 0 then Schema_DestroyIfUnused shall do nothing. **]**

### Schema_AddModelDesiredProperty
```c
extern SCHEMA_RESULT Schema_AddModelDesiredProperty(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, const char* desiredPropertyName, const char* desiredPropertyType, pfDesiredPropertyFromAGENT_DATA_TYPE desiredPropertyFromAGENT_DATA_TYPE, pfDesiredPropertyInitialize desiredPropertyInitialize, pfDesiredPropertyDeinitialize desiredPropertyDeinitialize, size_t offset, pfOnDesiredProperty onDesiredProperty));
```

`Schema_AddModelDesiredProperty` adds a desired property to a model.

**SRS_SCHEMA_02_024: [** If `modelTypeHandle` is `NULL` then `Schema_AddModelDesiredProperty` shall fail and return `SCHEMA_INVALID_ARG`. **]**

**SRS_SCHEMA_02_025: [** If `desiredPropertyName` is `NULL` then `Schema_AddModelDesiredProperty` shall fail and return `SCHEMA_INVALID_ARG`. **]**

**SRS_SCHEMA_02_026: [** If `desiredPropertyType` is `NULL` then `Schema_AddModelDesiredProperty` shall fail and return `SCHEMA_INVALID_ARG`. **]**

**SRS_SCHEMA_02_048: [** If `desiredPropertyFromAGENT_DATA_TYPE` is `NULL` then `Schema_AddModelDesiredProperty` shall fail and return `SCHEMA_INVALID_ARG`. **]**

**SRS_SCHEMA_02_049: [** If `desiredPropertyInitialize` is `NULL` then `Schema_AddModelDesiredProperty` shall fail and return `SCHEMA_INVALID_ARG`. **]**

**SRS_SCHEMA_02_050: [** If `desiredPropertyDeinitialize` is `NULL` then `Schema_AddModelDesiredProperty` shall fail and return `SCHEMA_INVALID_ARG`. **]**

**SRS_SCHEMA_02_027: [** `Schema_AddModelDesiredProperty` shall add the desired property given by the name `desiredPropertyName` and the type `desiredPropertyType` to the collection of existing desired properties. **]**

**SRS_SCHEMA_02_047: [** If the desired property already exists, then `Schema_AddModelDesiredProperty` shall fail and return `SCHEMA_DUPLICATE_ELEMENT`. **]**

**SRS_SCHEMA_02_028: [** If any failure occurs then `Schema_AddModelDesiredProperty` shall fail and return `SCHEMA_ERROR`. **]**

**SRS_SCHEMA_02_029: [** Otherwise, `Schema_AddModelDesiredProperty` shall succeed and return `SCHEMA_OK`. **]**

### Schema_GetModelDesiredPropertyCount
```c
extern SCHEMA_RESULT Schema_GetModelDesiredPropertyCount(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, size_t* desiredPropertyCount)
```

`Schema_GetModelDesiredPropertyCount` returns the number of desired properties for a model.

**SRS_SCHEMA_02_030: [** If `modelTypeHandle` is `NULL` then `Schema_GetModelDesiredPropertyCount` shall fail and return `SCHEMA_INVALID_ARG`. **]**

**SRS_SCHEMA_02_031: [** If `desiredPropertyCount` is `NULL` then `Schema_GetModelDesiredPropertyCount` shall fail and return `SCHEMA_INVALID_ARG`. **]**

**SRS_SCHEMA_02_032: [** Otherwise, `Schema_GetModelDesiredPropertyCount` shall succeed and write in `desiredPropertyCount` the existing number of desired properties. **]**

### Schema_GetModelDesiredPropertyByName
```c 
extern SCHEMA_DESIRED_PROPERTY_HANDLE Schema_GetModelDesiredPropertyByName(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, const char* desiredPropertyName);
```

`Schema_GetModelDesiredPropertyByName` returns the SCHEMA_DESIRED_PROPERTY_HANDLE for a desired property indicated by name.

**SRS_SCHEMA_02_034: [** If `modelTypeHandle` is `NULL` then `Schema_GetModelDesiredPropertyByName` shall fail and return `NULL`. **]**

**SRS_SCHEMA_02_035: [** If `desiredPropertyName` is `NULL` then `Schema_GetModelDesiredPropertyByName` shall fail and return `NULL`. **]**

**SRS_SCHEMA_02_036: [** If a desired property having the name `desiredPropertyName` exists then `Schema_GetModelDesiredPropertyByName` shall succeed and  return a non-`NULL` value. **]**

**SRS_SCHEMA_02_037: [** Otherwise, `Schema_GetModelDesiredPropertyByName` shall fail and return `NULL`. **]**


### Schema_GetModelDesiredPropertyByIndex
```c
extern SCHEMA_DESIRED_PROPERTY_HANDLE Schema_GetModelDesiredPropertyByIndex(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, size_t index);
```

`Schema_GetModelDesiredPropertyByIndex` returns a `SCHEMA_DESIRED_PROPERTY_HANDLE` corresponding to an index.

**SRS_SCHEMA_02_038: [** If `modelTypeHandle` is `NULL` then `Schema_GetModelDesiredPropertyByIndex` shall fail and return `NULL`. **]**

**SRS_SCHEMA_02_039: [** If index is outside the range for existing indexes of desire properties, then `Schema_GetModelDesiredPropertyByIndex` shall fail and return `NULL`. **]**

**SRS_SCHEMA_02_040: [** Otherwise, `Schema_GetModelDesiredPropertyByIndex` shall succeed and return a non-`NULL` value. **]**


### Schema_ModelDesiredPropertyByPathExists
```c
extern bool Schema_ModelDesiredPropertyByPathExists(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, const char* desiredPropertyPath);
```

`Schema_ModelDesiredPropertyByPathExists` returns `true` if `desiredPropertyPath` can be located in the model.

**SRS_SCHEMA_02_041: [** If `modelTypeHandle` is `NULL` then `Schema_ModelDesiredPropertyByPathExists` shall fail and return `false`. **]**

**SRS_SCHEMA_02_042: [** If `desiredPropertyPath` is `NULL` then `Schema_ModelDesiredPropertyByPathExists` shall fail and return `false`. **]**

**SRS_SCHEMA_02_043: [** `desiredPropertyPath` shall be assumed to be in the format model1/model2/.../desiredPropertyName. **]**

**SRS_SCHEMA_02_044: [** If the desired property cannot be found `Schema_ModelDesiredPropertyByPathExists` shall fail and return `false`. **]**

**SRS_SCHEMA_02_045: [** If the path desiredPropertyPath points to a sub-model, `Schema_ModelDesiredPropertyByPathExists` shall succeed and `true`. **]**

**SRS_SCHEMA_02_046: [** If `desiredPropertyPath` exists then `Schema_ModelDesiredPropertyByPathExists` shall succeed and return `true`. **]**

###  Schema_GetModelDesiredProperty_pfDesiredPropertyFromAGENT_DATA_TYPE
```c
extern pfDesiredPropertyFromAGENT_DATA_TYPE Schema_GetModelDesiredProperty_pfDesiredPropertyFromAGENT_DATA_TYPE(SCHEMA_DESIRED_PROPERTY_HANDLE desiredPropertyHandle);
```

`Schema_GetModelDesiredProperty_pfDesiredPropertyFromAGENT_DATA_TYPE` returns the original `desiredPropertyFromAGENT_DATA_TYPE` that was passed to `Schema_AddModelDesiredProperty`.

**SRS_SCHEMA_02_051: [** If `desiredPropertyHandle` is `NULL` then `Schema_GetModelDesiredProperty_pfDesiredPropertyFromAGENT_DATA_TYPE` shall fail and return `NULL`. **]**

**SRS_SCHEMA_02_052: [** Otherwise `Schema_GetModelDesiredProperty_pfDesiredPropertyFromAGENT_DATA_TYPE` shall succeed and return a non-`NULL` value. **]**

### Schema_GetModelModelByName_Offset
```c
size_t Schema_GetModelModelByName_Offset(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, const char* propertyName);
```

`Schema_GetModelModelByName_Offset` returns the offset in memory for a model in model specified by name.

**SRS_SCHEMA_02_053: [** If `modelTypeHandle` is `NULL` then `Schema_GetModelModelByName_Offset` shall fail and return 0. **]**

**SRS_SCHEMA_02_054: [** If `propertyName` is `NULL` then `Schema_GetModelModelByName_Offset` shall fail and return 0. **]**

**SRS_SCHEMA_02_056: [** If `propertyName` is not a model then `Schema_GetModelModelByName_Offset` shall fail and return 0. **]**

**SRS_SCHEMA_02_055: [** Otherwise `Schema_GetModelModelByName_Offset` shall succeed and return the offset. **]**

### Schema_GetModelModelByIndex_Offset
```c
size_t Schema_GetModelModelByIndex_Offset(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, size_t index);
```

`Schema_GetModelModelByIndex_Offset` returns the offset in memory for a model in model specified by index.

**SRS_SCHEMA_02_057: [** If `modelTypeHandle` is `NULL` then `Schema_GetModelModelByIndex_Offset` shall fail and return 0. **]**

**SRS_SCHEMA_02_058: [** If `index` is not valid then `Schema_GetModelModelByIndex_Offset` shall fail and return 0. **]**

 **SRS_SCHEMA_02_059: [** Otherwise `Schema_GetModelModelByIndex_Offset` shall succeed and return the offset. **]**

 ### Schema_GetModelDesiredProperty_offset
 ```c
 size_t Schema_GetModelDesiredProperty_offset(SCHEMA_DESIRED_PROPERTY_HANDLE desiredPropertyHandle);
 ```

`Schema_GetModelDesiredProperty_offset` returns the offset of a desired property.

**SRS_SCHEMA_02_060: [** If `desiredPropertyHandle` is `NULL` then `Schema_GetModelDesiredProperty_offset` shall fail and return 0. **]**

**SRS_SCHEMA_02_061: [** Otherwise `Schema_GetModelDesiredProperty_offset` shall succeed and return the offset. **]**

### const char* Schema_GetModelDesiredPropertyType(SCHEMA_DESIRED_PROPERTY_HANDLE desiredPropertyHandle);
```c
const char* Schema_GetModelDesiredPropertyType(SCHEMA_DESIRED_PROPERTY_HANDLE desiredPropertyHandle);
```

`Schema_GetModelDesiredPropertyType` returns the type of a desired property.

**SRS_SCHEMA_02_062: [** If `desiredPropertyHandle` is `NULL` then `Schema_GetModelDesiredPropertyType` shall fail and return `NULL`. **]**

**SRS_SCHEMA_02_063: [** Otherwise, `Schema_GetModelDesiredPropertyType` shall return the type of the desired property. **]** 

### Schema_GetModelDesiredProperty_pfDesiredPropertyDeinitialize
```c
pfDesiredPropertyDeinitialize Schema_GetModelDesiredProperty_pfDesiredPropertyDeinitialize(SCHEMA_DESIRED_PROPERTY_HANDLE desiredPropertyHandle);
```

`Schema_GetModelDesiredProperty_pfDesiredPropertyDeinitialize` returns the desired property deinitialize function pointer.

**SRS_SCHEMA_02_064: [** If `desiredPropertyHandle` is `NULL` then `Schema_GetModelDesiredProperty_pfDesiredPropertyDeinitialize` shall fail and return `NULL`. **]**

**SRS_SCHEMA_02_065: [** Otherwise `Schema_GetModelDesiredProperty_pfDesiredPropertyDeinitialize` shall return a non-`NULL` function pointer. **]**

### Schema_GetModelDesiredProperty_pfDesiredPropertyInitialize
```c
pfDesiredPropertyInitialize Schema_GetModelDesiredProperty_pfDesiredPropertyInitialize(SCHEMA_DESIRED_PROPERTY_HANDLE desiredPropertyHandle);
```

`Schema_GetModelDesiredProperty_pfDesiredPropertyInitialize` returns the initialize function for a desired property.

**SRS_SCHEMA_02_066: [** If `desiredPropertyHandle` is `NULL` then `Schema_GetModelDesiredProperty_pfDesiredPropertyInitialize` shall fail and return `NULL`. **]**

**SRS_SCHEMA_02_067: [** Otherwise `Schema_GetModelDesiredProperty_pfDesiredPropertyInitialize` shall return a non-`NULL` function pointer. **]**

### Schema_GetModelElementByName
```c
extern SCHEMA_MODEL_ELEMENT Schema_GetModelElementByName(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, const char* elementName);
```

`Schema_GetModelElementByName` returns the handle of a model element by name.

**SRS_SCHEMA_02_076: [** If `modelTypeHandle` is `NULL` then `Schema_GetModelElementByName` shall fail and set `SCHEMA_MODEL_ELEMENT.elementType` to `SCHEMA_SEARCH_INVALID_ARG`. **]**

**SRS_SCHEMA_02_077: [** If `elementName` is `NULL` then `Schema_GetModelElementByName` shall fail and set `SCHEMA_MODEL_ELEMENT.elementType` to `SCHEMA_SEARCH_INVALID_ARG`. **]**

**SRS_SCHEMA_02_078: [** If `elementName` is a property then `Schema_GetModelElementByName` shall succeed and set `SCHEMA_MODEL_ELEMENT.elementType` to 
`SCHEMA_PROPERTY` and `SCHEMA_MODEL_ELEMENT.elementHandle.propertyHandle` to the handle of the property. **]**

**SRS_SCHEMA_02_079: [** If `elementName` is a reported property then `Schema_GetModelElementByName` shall succeed and set `SCHEMA_MODEL_ELEMENT.elementType` to 
`SCHEMA_REPORTED_PROPERTY` and `SCHEMA_MODEL_ELEMENT.elementHandle.reportedPropertyHandle` to the handle of the reported property. **]**

**SRS_SCHEMA_02_080: [** If `elementName` is a desired property then `Schema_GetModelElementByName` shall succeed and set `SCHEMA_MODEL_ELEMENT.elementType` to 
`SCHEMA_DESIRED_PROPERTY` and `SCHEMA_MODEL_ELEMENT.elementHandle.desiredPropertyHandle` to the handle of the desired property. **]**

**SRS_SCHEMA_02_081: [** If `elementName` is a model action then `Schema_GetModelElementByName` shall succeed and set `SCHEMA_MODEL_ELEMENT.elementType` to 
`SCHEMA_MODEL_ACTION` and `SCHEMA_MODEL_ELEMENT.elementHandle.actionHandle` to the handle of the action. **]**

**SRS_SCHEMA_02_082: [** If `elementName` is a model in model then `Schema_GetModelElementByName` shall succeed and set `SCHEMA_MODEL_ELEMENT.elementType` to 
`SCHEMA_MODEL_IN_MODEL` and `SCHEMA_MODEL_ELEMENT.elementHandle.modelHandle` to the handle of the model. **]**

**SRS_SCHEMA_02_083: [** Otherwise  `Schema_GetModelElementByName` shall fail and set `SCHEMA_MODEL_ELEMENT.elementType` to `SCHEMA_NOT_FOUND`. **]**

### Schema_GetModelDesiredProperty_pfOnDesiredProperty
```c
extern pfOnDesiredProperty Schema_GetModelDesiredProperty_pfOnDesiredProperty(SCHEMA_DESIRED_PROPERTY_HANDLE desiredPropertyHandle);
```

`Schema_GetModelDesiredProperty_pfOnDesiredProperty` retrieves the saved desired property callback from `desiredPropertyHandle`.

**SRS_SCHEMA_02_084: [** If `desiredPropertyHandle` is `NULL` then `Schema_GetModelDesiredProperty_pfOnDesiredProperty` shall return `NULL`. **]**

**SRS_SCHEMA_02_085: [** Otherwise `Schema_GetModelDesiredProperty_pfOnDesiredProperty` shall return the saved desired property callback. **]**


### Schema_GetModelModelByName_OnDesiredProperty
```c
extern pfOnDesiredProperty Schema_GetModelModelByName_OnDesiredProperty(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, const char*, propertyName);
```

`Schema_GetModelModelByName_OnDesiredProperty` returns the saved desired property callback for a model in model by name.

**SRS_SCHEMA_02_086: [** If `modelTypeHandle` is `NULL` then `Schema_GetModelModelByName_OnDesiredProperty` shall return `NULL`. **]**

**SRS_SCHEMA_02_087: [** If `propertyName` is `NULL` then `Schema_GetModelModelByName_OnDesiredProperty` shall return `NULL`. **]**

**SRS_SCHEMA_02_088: [** If `propertyName` is not a desired property then `Schema_GetModelModelByName_OnDesiredProperty` shall return `NULL`. **]**

**SRS_SCHEMA_02_089: [** Otherwise `Schema_GetModelModelByName_OnDesiredProperty` shall return the desired property callback. **]**


