# SERIALIZER APIs v3
 
## Overview
The SERIALIZER APIs allows developers to quickly and easily define models for their devices directly as code, while supporting the required features for 
modeling devices (including multiple models and multiple devices within the same application).

## SERIALIZER description macro

 __Example__

In the header file:

```c
#include "serializer.h"
BEGIN_NAMESPACE(MyFunkyTV);
DECLARE_STRUCT(MenuType,
    int, source,
    double, brightness
    );

DECLARE_MODEL(FunkyTV,
    WITH_DATA(int, screenSize),
    WITH_DATA(bool, hasEthernet),
    WITH_DATA(MenuType, tvMenu),
    WITH_ACTION(LostSignal, int, source, int, resolution)
);

DECLARE_IOT_MODEL(AnotherDevice, 
    ...
);

END_NAMESPACE(MyFunkyTV);

```

### BEGIN_NAMESPACE(schemaNamespace)

This macro marks the start of a section that declares the model elements (like complex types, etc.). Declarations are typically placed in header files, so that they can be shared between translation units.

### END_NAMESPACE(schemaNamespace)

This macro marks the end of a section that declares the model elements.

### DECLARE_STRUCT(structTypeName, field1Type, field1Name, ...)

This macro allows declaring a struct (complex) type for an IOT model.

Arguments:
- structTypeName – specifies the struct type name
- (fieldXType, fieldXName) – The type and the name for the field X of the struct type. A struct type can have any number of fields from 1 to 61 (inclusive). At least one field must be defined. 

__Example__:

 ```c
 DECLARE_STRUCT(MenuType,
    int, source,
    double, brightness
    );
```

### DECLARE_MODEL(modelName, element1, ...)
This macro allows declaring a model that can be later used to instantiate a device.
Arguments:

-	`modelName` – specifies the model name
-	`element1`, `element2`, ... – a model element (can be a property and action).
-	A property is described in a model by using the `WITH_DATA`.
-	An action is described in a model by using the `WITH_ACTION` macro.

__Example:__
```c
DECLARE_IOT_MODEL(FunkyTV,
    ...
);
```

## WITH_DATA(propertyType, propertyName)

The `WITH_DATA` macro allows declaring a model property in a model. A property can be serialized by using the SERIALIZE macro.

Arguments:
-   propertyType – specifies the property type. Can be any of the following types:
    -	int
    -	double
    -	float
    -	long
    -	int8_t
    -	uint8_t
    -	int16_t
    -	int32_t
    -	int64_t
    -	bool
    -	ascii_char_ptr
    -	EDM_DATE_TIME_OFFSET
    -	EDM_GUID
    -	EDM_BINARY
    -	Any struct type previously introduced by another `DECLARE_STRUCT`.
-	propertyName – specifies the property name

```c
DECLARE_IOT_MODEL(FunkyTV,
    WITH_DATA(int, screenSize),
    WITH_DATA(bool, hasEthernet),
    ...
    );
```

## WITH_ACTION(actionName, arg1Type, arg1Name, ...)
The `WITH_ACTION` macro allows declaring a model action.

Arguments:
-	`actionName` – specifies the action name.
-	`argXtype`, `argXName` – defines the type and name for the Xth argument of the action. The type can be any of the primitive types or a struct type.

```c
DECLARE_IOT_MODEL(FunkyTV,
    ...
    WITH_ACTION(channelChange, ascii_char_ptr, Property1),
    ...
);
```

### GET_MODEL_HANDLE(schemaNamespace, modelName)

The GET_MODEL_HANDLE macro returns the model handle for the model with type `modelName` inside the namespace called `schemaNamespace`.

## Acting on models

### SERIALIZE(destination, destinationSize, property1, ...)

This macro produces the JSON serialized representation of the properties.

__Arguments:__

-	destination – pointer to an unsigned char* that will receive the serialized data. 
-	destinationSize – Pointer to a size_t that gets written with the size in bytes of the serialized data 
-	property1, property2, ... -  a list of property values to send. The order in which the properties appear in the list does not matter, all values will be sent together.

__Returns:__
-	CODEFIRST_OK on success
-	Any other value on failure

```c
...
DECLARE_IOT_MODEL(FunkyTV,
    WITH_DATA(int, screenSize),
    WITH_DATA(bool, hasEthernet),
    ...
);
...

int main(int argc, char** argv)
{
...
    FunkyTV* funkyTV = CREATE_MODEL_INSTANCE(MyFunkyTV, FunkyTV);
    unsigned char* destination; size_t destinationSize;
    funkyTV->hasEthernet = false;
    funkyTV->screenSize = 42;
    SERIALIZE(&destination, &destinationSize, funkyTV->hasEthernet, funkyTV->screenSize);
    printf("serialized data is %*.*s\r\n",(int)destinationSize, (int)destinationSize,  (char*)destination);
...
}
```

### EXECUTE_COMMAND

Any action that is declared in a model must also have an implementation as a C function.

The C function arguments must be:
-	First argument must be of the type pointer to device data (i.e. FunkyTV*).
-	Following arguments must match the arguments declared in the model action.

The macro `EXECUTE_COMMAND(device, commandBuffer)` shall execute the command indicated in the commandBuffer for the device. 

```c
DECLARE_IOT_MODEL(MyFunkyTV,
    ...
    WITH_ACTION(changeChannel, ascii_char_ptr, Property1)
    );
...

void changeChannel(FunkyTV* device, ascii_char_ptr Property1)
{
    printf("Changing Channel to channel %s\r\n", Property1);
}

#define COMMAND_TEXT "\"Name\":\"changeChannel\",\"Parameters\":\"FabrikamTV\""

int main(void)
{
    ...
    EXECUTE_COMMAND(funkyTv, COMMAND_TEXT);
    ...
} 
```

## SERIALIZER APIs

### serializer_init
```
SERIALIZER_RESULT serializer_init(const char* overrideSchemaNamespace)
```

Initializes the library.

__Arguments:__
-	`overrideSchemaNamespace` – An override schema namespace to use for all models. Optional, can be `NULL`. 
If `schemaNamespace` is not `NULL`, its value shall be used instead of the namespace defined for each model 
by using the DECLARE_XXX macro.

__Returns:__
-	`SERIALIZER_OK` on success
-	Any other error on failure

Example:
```c
int main(int argc, char** argv)
{
    ...
    if (serializer_init(NULL) != SERIALIZER_OK)
	{
		/* error */
    }
    else
    {   
	...
    }
}
```

### serializer_deinit
```c
void serializer_deinit()
```

Deinitializes the serializer library. The library will track all created devices and upon a call to serializer_deinit it will de-initialize all devices.
Example:

```c
int main(int argc, char** argv)
{
    ...
    (void)serializer_init(NULL);
    ...
    serializer_deinit();
}
```

### CREATE_MODEL_INSTANCE
```c
void* CREATE_MODEL_INSTANCE(schemaNamespace, modelName, serializerIncludePropertyPath)
```
Initializes a model instance that has the model identified by the schema Namespace and Model Name.

__Arguments:__
-	`schemaNamespace` - The schema namespace as specified in `BEGIN_NAMESPACE` macro.
-	`modelName` - The model name, as defined with the `DEFINE_MODEL` macro.
-	`serializerIncludePropertyPath` – an optional bool argument. Default value: `false`. If set to `true` it instructs the serializer to include the full property path (including the property name) in the resulting JSON. If set to `false`, the property path (and name) will not appear in the resulting JSON.

__Returns:__
-	A pointer to a structure of type modelName
-	`NULL` in case of an error.

Example:
```c
DECLARE_MODEL(MyFunkyTV,
    WITH_DATA(int, screenSize),
    WITH_DATA(bool, hasEthernet),
    ...
    );
int main(int argc, char** argv)
{

    (int)serializer_init(NULL);

	FunkyTV* funkyTV = CREATE_MODEL_INSTANCE(MyFunkyTV, FunkyTV);
    ...
    funkyTV->hasEthernet = false;
    funkyTV->screenSize = 42;
    unsigned char* destination; size_t destinationSize;
    SERIALIZE(&destination, &destinationSize, funkyTV->hasEthernet, funkyTV->screenSize);
    printf("serialization = %*.*s", (int)destinationSize, (int)destinationSize, destination);
}

```

### DESTROY_MODEL_INSTANCE
```
DESTROY_MODEL_INSTANCE(instance)
```
Frees any resources associated with the model instance.

__Arguments:__
-	instance – A previously created instance with `CREATE_MODEL_INSTANCE`.


