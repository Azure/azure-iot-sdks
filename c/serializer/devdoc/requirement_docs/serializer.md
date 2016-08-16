# serializer

serializer.h is a header file that defines the macros an application can use to serialize and execute model commands.
These macros are:
```c
#define BEGIN_NAMESPACE(schemaNamespace) /*...*/

#define DECLARE_STRUCT(name, field1Type, field1Name, ...) /*...*/
#define DECLARE_MODEL(name, element1, element2, ...) /*...*/

#define WITH_DATA (type, name) /*...*/
#define WITH_ACTION(name, param1Type, param1Name, ...) /*...*/

#define WITH_REPORTED_PROPERTY(type,name)

#define END_NAMESPACE(schemaNamespace) /*...*/

#define CREATE_MODEL_INSTANCE(schemaNamespace, modelName, serializerIncludePropertyPath) /*...*/
#define (deviceData) /*...*/

#define GET_MODEL_HANDLE(modelName) /*...*/

#define SERIALIZE(destination, destinationSize, property2, ...) /*...*/
#define SERIALIZE_REPORTED_DATA(destination, reported_property1, reported_property2, ...)

#define EXECUTE_COMMAND(device, commandBuffer, commandBufferSize)
```

### BEGIN_NAMESPACE(schemaNamespace)
### END_NAMESPACE(schemaNamespace)

The BEGIN_NAMESPACE macro marks the beginning of a schema declaration block. END_NAMESPACE marks the end. Declaration statements (DECLARE_* macros) are only to be placed within a declaration block.

**SRS_SERIALIZER_H_99_001: [** For each completed schema declaration block, a unique storage container for schema metadata shall be available in the translation unit at runtime. **]**

**SRS_SERIALIZER_H_99_076: [**  The following declaration macros shall be valid between BEGIN_NAMESPACE and END_NAMESPACE: DECLARE_MODEL, DECLARE_STRUCT **]**

### DECLARE_STRUCT(name, field1Type, field1Name, ...)

A struct in the IOT Agent is a complex type composed of primitive types and other structs. After a struct is declared in the schema, it can be used as a property in a model.

**SRS_SERIALIZER_H_99_080: [**  The DECLARE_STRUCT declaration shall insert metadata describing a complex data type. **]**

**SRS_SERIALIZER_H_99_081: [**  DECLARE_STRUCT's name argument shall uniquely identify the struct within the schema. **]**

**SRS_SERIALIZER_H_99_082: [**  DECLARE_STRUCT's field<n>Name argument shall uniquely name a field within the struct. **]**

**SRS_SERIALIZER_H_99_083: [**  DECLARE_STRUCT's field<n>Type argument, which describes the type of the corresponding field, shall be one of the data types permitted for model properties. **]**

**SRS_SERIALIZER_H_99_096: [**  DECLARE_STRUCT shall declare a matching C struct data type named name, which can be referenced from any code that can access the declaration. **]**

### DECLARE_MODEL(name, element1, element2, ...)

A model in the IOT Agent describes the type and structure of data captured for a device.

**SRS_SERIALIZER_H_99_077: [**  The DECLARE_MODEL declaration shall insert metadata describing the data to be captured for a device. **]**

**SRS_SERIALIZER_H_99_078: [**  DECLARE_MODEL's name argument shall uniquely identify the model within the schema namespace. **]**

**SRS_SERIALIZER_H_99_097: [**  DECLARE_MODEL shall declare a matching C struct data type named name, which can be referenced from any code that can access the declaration. **]**

**SRS_SERIALIZER_H_99_103: [**  The following statements shall be valid as elements within a model: WITH_DATA, WITH_ACTION. **]**

### WITH_DATA (type, name)

**SRS_SERIALIZER_H_99_087: [**  The WITH_DATA declaration shall insert metadata describing a property in the model. **]**

**SRS_SERIALIZER_H_99_088: [**  WITH_DATA's name argument shall uniquely identify the property within the model. **]**

WITH_DATA's name argument shall be one of the following data types: 
**SRS_SERIALIZER_H_99_004: [** double **]**
 
**SRS_SERIALIZER_H_99_005: [**  int **]**
 
**SRS_SERIALIZER_H_99_006: [**  float **]**
 
**SRS_SERIALIZER_H_99_007: [**  long **]**
 
**SRS_SERIALIZER_H_99_008: [**  int8_t **]**
 
**SRS_SERIALIZER_H_99_009: [**  uint8_t **]**
 
**SRS_SERIALIZER_H_99_010: [**  int16_t **]**
 
**SRS_SERIALIZER_H_99_011: [**  int32_t **]**
 
**SRS_SERIALIZER_H_99_012: [**  int64_t **]**
 
**SRS_SERIALIZER_H_99_013: [**  bool **]**
 
**SRS_SERIALIZER_H_99_014: [**  ascii_char_ptr **]**
  
**SRS_SERIALIZER_H_01_001: [** ascii_char_ptr_no_quotes, **]**

**SRS_SERIALIZER_H_99_015: [**  a struct type introduced previously by DECLARE_STRUCT **]**
 
**SRS_SERIALIZER_H_99_051: [**  EDM_DATE_TIME_OFFSET **]**

**SRS_SERIALIZER_H_99_072: [**  EDM_GUID **]**

**SRS_SERIALIZER_H_99_074: [**  EDM_BINARY **]**

**SRS_SERIALIZER_H_99_133: [** a model type introduced previously by DECLARE_MODEL **]**

### WITH_ACTION(name, param1Type, param1Name, ...)

An action defines a command which the IOT service can invoke on any device that is associated (via device registration) with the model.

**SRS_SERIALIZER_H_99_090: [**  The WITH_ACTION declaration shall insert metadata describing an action in the model. **]**

**SRS_SERIALIZER_H_99_091: [**  WITH_ACTION's name argument shall uniquely identify the action within the set of all actions in the model. **]**

**SRS_SERIALIZER_H_99_092: [**  WITH_ACTION's param<n>Name argument shall uniquely name the nth parameter in the action's function prototype. **]**

**SRS_SERIALIZER_H_99_042: [**  WITH_ACTIONS's field<n>Type argument, which describes the type of the nth parameter in the action's function prototype, shall be one of the data types permitted for model properties. **]**

**SRS_SERIALIZER_H_99_039: [**  WITH_ACTION shall declare a function with the signature 'void name(param1Type param1Name, ...)', which the developer can define to receive corresponding commands from the IOT service. **]**

**SRS_SERIALIZER_H_99_043: [**  It is valid for an action function not to have any parameters. **]**

**SRS_SERIALIZER_H_99_040: [**  WITH_ACTION shall result in the declaration of a conversion function with the prototype DATAPROVIDER_RESULT nameWRAPPER(size_t ParameterCount, const AGENT_DATA_TYPE* values)'. **]**

**SRS_SERIALIZER_H_99_041: [**  The function shall convert the input arguments to the types declared in the action parameter list and then call the developer-defined action function. **]**

**SRS_SERIALIZER_H_99_045: [** If the number of arguments passed to the conversion function does not match the expected count, the function shall return DATAPROVIDER_INVALID_ARG. **]**

**SRS_SERIALIZER_H_99_046: [**  If an AGENT_DATA_TYPE argument passed to the conversion function does not match the expected type, the function shall return DATAPROVIDER_INVALID_ARG. **]**

### CREATE_MODEL_INSTANCE(schemaNamespace, modelName) 

**SRS_SERIALIZER_H_99_104: [**  CREATE_MODEL_INSTANCE shall call GET_MODEL_HANDLE, passing schemaNamespace and modelName, to get a model handle representing the model defined in the corresponding schema declaration block. **]**

**SRS_SERIALIZER_H_99_106: [**  CREATE_MODEL_INSTANCE shall call CodeFirst_CreateDevice, passing the model handle (SCHEMA_MODEL_TYPE_HANDLE **]**

**SRS_SERIALIZER_H_01_002: [** If the argument serializerIncludePropertyPath is specified, its value shall be passed to CodeFirst_Create. **]**

**SRS_SERIALIZER_H_01_003: [** If the argument serializerIncludePropertyPath is not specified, CREATE_MODEL_INSTANCE shall pass false to CodeFirst_Create. **]**

**SRS_SERIALIZER_H_99_107: [**  If CodeFirst_CreateDevice fails, CREATE_MODEL_INSTANCE shall return NULL. **]**

**SRS_SERIALIZER_H_99_108: [**  If CodeFirst_CreateDevice succeeds, CREATE_MODEL_INSTANCE shall return a pointer to an instance of the C struct representing the model for the device. **]**

### DESTROY_MODEL_INSTANCE(deviceData)
**SRS_SERIALIZER_H_99_109: [**  DESTROY_MODEL_INSTANCE shall call CodeFirst_DestroyDevice, passing the pointer returned from CREATE_MODEL_INSTANCE, to release all resources associated with the device. **]**

### GET_MODEL_HANDLE(schemaNamespace, modelName)

**SRS_SERIALIZER_H_99_110: [**  The GET_MODEL_HANDLE function macro shall first register the schema by calling CodeFirst_RegisterSchema, passing schemaNamespace and a pointer to the metadata generated in the schema declaration block. **]**

**SRS_SERIALIZER_H_99_094: [**  GET_MODEL_HANDLE shall then call Schema_GetModelByName, passing the schemaHandle obtained from CodeFirst_RegisterSchema and modelName arguments, to retrieve the SCHEMA_MODEL_TYPE_HANDLE corresponding to the modelName argument. **]**

**SRS_SERIALIZER_H_99_112: [**  GET_MODEL_HANDLE will return the handle for the named model. **]**

### SERIALIZE(destination, destinationSize, property1, property2, ...)

The SERIALIZE function macro is used to publish device data to the IOT service.

Parameter destinations need to be the address of an unsigned char*, parameter destinationSize needs to be a pointer to size_t.

**SRS_SERIALIZER_H_99_113: [**  SERIALIZE shall call CodeFirst_SendAsync, passing a destination, destinationSize, the number of properties to publish, and pointers to the values for each property. **]**

**SRS_SERIALIZER_H_99_114: [**  If CodeFirst_SendAsync fails, SERIALIZE shall return SERIALIZER_SERIALIZE_FAILED. **]**

**SRS_SERIALIZER_H_99_117: [**  If CodeFirst_SendAsync succeeds, SERIALIZE will return SERIALIZER_OK. **]**

**SRS_SERIALIZER_H_99_118: [** If SERIALIZE is invoked with no arguments then it shall not compile. **]**

### EXECUTE_COMMAND
```c
EXECUTE_COMMAND(device, command)
```

device is a previously created device by a call to CREATE_MODEL_INSTANCE.
command is a null terminated string containing the command to execute in JSON format.

**SRS_SERIALIZER_H_02_018: [** EXECUTE_COMMAND macro shall call CodeFirst_ExecuteCommand passing device, command. **]**

### WITH_REPORTED_PROPERTY
```c
WITH_REPORTED_PROPERTY(type, name)
```

WITH_REPORTED_PROPERTY introduces a property having a type and a name in a model. This property takes part in the reported property section of DeviceTwin JSON.
It supports the same types as WITH_DATA.

**SRS_SERIALIZER_H_02_019: [** The WITH_REPORTED_PROPERTY declaration shall insert metadata describing a reported property in the model. **]**

 **SRS_SERIALIZER_H_02_020: [** WITH_REPORTED_PROPERTY's name argument shall uniquely identify the reported property within the model. **]**

### SERIALIZE_REPORTED_PROPERTIES
```c
SERIALIZE_REPORTED_PROPERTIES(destination, destinationSize, reported_property1, reported_property2, ...)
```

SERIALIZE_REPORTED_PROPERTIES produced a serialized form (JSON) of the reported properties indicated as arguments. 
A complete model instance can be passed as parameter in which case all the reported properties shall be serialized.

**SRS_SERIALIZER_H_02_021: [** SERIALIZE_REPORTED_PROPERTIES shall call CodeFirst_SendAsyncReportedProperties, passing a destination, destinationSize, the number of reported properties to publish, and pointers to the values for each reported property. **]**
**SRS_SERIALIZER_H_02_022: [** If CodeFirst_SendAsyncReportedProperties fails, SERIALIZE_REPORTED_PROPERTIES shall return SERIALIZER_SERIALIZE_FAILED. **]**
**SRS_SERIALIZER_H_02_023: [** If CodeFirst_SendAsyncReportedProperties succeeds, SERIALIZE_REPORTED_PROPERTIES will return SERIALIZER_OK. **]**
**SRS_SERIALIZER_H_02_024: [** If SERIALIZE_REPORTED_PROPERTIES is invoked with no arguments then it shall not compile. **]** 