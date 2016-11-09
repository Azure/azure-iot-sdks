# SERIALIZER APIs v5
 
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

DECLARE_MODEL(AnotherDevice, 
    ...
);

END_NAMESPACE(MyFunkyTV);

```

### BEGIN_NAMESPACE(schemaNamespace)

This macro marks the start of a section that declares the model elements (like complex types, etc.). Declarations are typically placed in header files, so that they can be shared between translation units.

### END_NAMESPACE(schemaNamespace)

This macro marks the end of a section that declares the model elements.

### DECLARE_STRUCT(structTypeName, field1Type, field1Name, ...)

This macro allows declaring a struct (complex) type for a model.

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
DECLARE_MODEL(FunkyTV,
    ...
);
```

## WITH_DATA(propertyType, propertyName)

The `WITH_DATA` macro allows declaring a model property in a model. A property can be serialized by using the SERIALIZE macro.

__Arguments:__
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
DECLARE_MODEL(FunkyTV,
    WITH_DATA(int, screenSize),
    WITH_DATA(bool, hasEthernet),
    ...
    );
```

## WITH_REPORTED_PROPERTY
```c
WITH_REPORTED_PROPERTY(propertyType, propertyName)
```

`WITH_REPORTED_PROPERTY` macro allows declaring a reported property (in the context of DeviceTwin).
Reported properties have the same type as properties declared by `WITH_DATA` macro. 

## WITH_DESIRED_PROPERTY
```c
WITH_DESIRED_PROPERTY(propertyType, propertyname [,onDesiredProperty])
```

`WITH_DESIRED_PROPERTY` macro declares a desired property. This is a DeviceTwin notion. 
Desired properties have the same types as regular properties declared using `WITH_DATA` macro.
Additionally, desired properties can have an optional 3rd argument, which is a function name. 
The function will be called when the desired property is received. The function receives a pointer to
the encompassing model where the desired property is declared.

__Example__
```c
DECLARE_MODEL(Car,
    WITH_DESIRED_PROPERTY(int, softwareVersion),
    WITH_DESIRED_PROPERTY(ascii_char_ptr, firmwareVersionAsString),
    WITH_DESIRED_PROPERTY(int, maxSpeed, OnMaxSpeed)
    ...
    );

void OnMaxSpeed(void* v)
{
    Car* car = v;
    printf("maxSpeed has been received and it is %d\n", car->maxSpeed);
}
```

## WITH_ACTION(actionName, arg1Type, arg1Name, ...)

The `WITH_ACTION` macro allows declaring a model action. Once the action is declared, it will have to be complemented by 
a C function that defines the action. The C function prototype is the following:

Arguments:
-	`actionName` – specifies the action name.
-	`argXtype`, `argXName` – defines the type and name for the Xth argument of the action. The type can be any of the primitive types or a struct type.

```c
DECLARE_MODEL(FunkyTV,
    ...
    WITH_ACTION(channelChange, ascii_char_ptr, Property1),
    ...
);

The following is the C function definition:

```c
EXECUTE_COMMAND_RESULT actionName(modelName* model, arg1Type arg1Name, arg2Type arg2Name) /*more arguments can follow if more are declared in `WITH_ACTION` */
{
    
}
```


## WITH_METHOD(methodName, arg1Type, arg1, arg2Type, arg2, ...)

`WITH_METHOD` introduces a Device Method in the model. `WITH_METHOD` is similar to `WITH_ACTION`: it will result in a user-supplied C function being called.
The main difference is in return value (Methods return a number and a JSON value, Actions return values of an enum).

Arguments:
-	`methodName` – specifies the method name.
-	`argXtype`, `argXName` – defines the type and name for the Xth argument of the method. The type can be any of the primitive types or a struct type.

```c
DECLARE_MODEL(FunkyTV,
    ...
    WITH_METHOD(channelChange, ascii_char_ptr, Property1),
    ...
);
```

The following is the C function definition of a method:

```c
METHOD_RETURN_HANDLE methodName(modelName* model, arg1Type arg1, arg2Type arg2)
```

`METHOD_RETURN_HANDLE` is an abstract type around the following data:
-   int result; /\*the result of the method call\*/
-   char* jsonValue; /\*the JSON value to be returned, can be `NULL`. \*/



## GET_MODEL_HANDLE(schemaNamespace, modelName)

The GET_MODEL_HANDLE macro returns the model handle for the model with type `modelName` inside the namespace called `schemaNamespace`.

## Acting on models

### SERIALIZE(destination, destinationSize, property1, ...)

This macro produces the JSON serialized representation of the properties.

__Arguments:__

-	destination – pointer to an unsigned char* that will receive the serialized data. 
-	destinationSize – pointer to a size_t that gets written with the size in bytes of the serialized data 
-	property1, property2, ... -  a list of property values to send. The order in which the properties appear in the list does not matter, all values will be sent together.

__Returns:__
-	CODEFIRST_OK on success
-	Any other value on failure

```c
...
DECLARE_MODEL(FunkyTV,
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

### SERIALIZE_REPORTED_PROPERTIES
```c
SERIALIZE_REPORTED_PROPERTIES(destination, destinationSize, reportedProperty1, ...)
```

This macro produces the JSON serialized representation of the reported properties. The arguments can be
individual properties or a complete model. 

__Arguments:__

-	destination – pointer to an unsigned char* that will receive the serialized reported properties. 
-	destinationSize – pointer to a size_t that gets written with the size in bytes of the serialized reported properties. 
-	property1, property2, ... -  a list of reported properties to send. The order in which the reported properties appear in 
the list does not matter, all values will be sent together. If the reported property argument is a complete model, then only
the reported properties in that model will be serialized.

__Returns:__
-	CODEFIRST_OK on success
-	Any other value on failure

__Example__
```c
...
DECLARE_MODEL(FunkyTV,
    WITH_REPORTED_PROPERTY(int, screenSize),
    WITH_REPORTED_PROPERTY(bool, hasEthernet),
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
    SERIALIZE_REPORTED_PROPERTIES(&destination, &destinationSize, funkyTV->hasEthernet, funkyTV->screenSize);
    printf("serialized reported properties \n%*.*s\r\n",(int)destinationSize, (int)destinationSize,  (char*)destination);
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
DECLARE_MODEL(MyFunkyTV,
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

### INGEST_DESIRED_PROPERTIES
```c
INGEST_DESIRED_PROPERTIES(modelInstance, DESIRED_PROPERTIES_AS_JSON)
```

`INGEST_DESIRED_PROPERTIES` shall populate desired properties with values from the JSON.

__Arguments:__ 
- `modelInstance` is a previous model instance created by `CREATE_MODEL_INSTANCE` macro;
- `DESIRED_PROPERTIES_AS_JSON` is a `const char*` pointer pointing to a null terminated string. 

__Returns__:
`INGEST_DESIRED_PROPERTIES` returns `CODEFIRST_OK` if success, any other error indicates failure.

__Example__:
```c
DECLARE_MODEL(Car,
    WITH_DESIRED_PROPERTY(int, softwareVersion),
    WITH_DESIRED_PROPERTY(ascii_char_ptr, firmwareVersionAsString)
    ...
    );

int main(void)
{

	Car* car = CREATE_MODEL_INSTANCE(MyFunkyCarNamespace, Car);
    ...
    INGEST_DESIRED_PROPERTIES(car, "{\"softwareVersion\":3, \"firmwareVersionAsString\":\"the firmware 0.1\"}");
    ...
}




## SERIALIZER APIs

### serializer_init
```
SERIALIZER_RESULT serializer_init(const char* overrideSchemaNamespace)
```

An optional API used to pass `overrideSchemaNamespace`. If `serializer_init` is not called then `overrideSchemaNamespace`
shall be assumed to be `NULL`.

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
void serializer_deinit(void)
```

Deinitializes the serializer library. An empty function preserved for compatibility purposes.
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


## SERIALIZER_DEVICETWIN

In order to better support Device Twin features by providing a stronger cohesion, serializer has been enhanced 
with a layer specially crafted for Device Twin. The new functionality is accesibile by #include "serializer_devicetwin.h".

`DECLARE_MODEL` becomes `DECLARE_DEVICETWIN_MODEL`. All the semantics are preserved. Model instances
are created by a C function, and not a by `CREATE_MODEL_INSTANCE` macro.

__Example:__
```c
#include "serializer_devicetwin.h"

BEGIN_NAMESPACE(Contoso);
DECLARE_DEVICETWIN_MODEL(Car,
    WITH_DESIRED_PROPERTY(CarSettings, settings)
);
END_NAMESPACE(Contoso);
...

REGISTER_SCHEMA_NAMESPACE(Contoso);
Car* car = IoTHubDeviceTwin_CreateCar(iotHubClientHandle);
car->settings = ...;
```

Before using the model name, register the namespace by `REGISTER_SCHEMA_NAMESPACE`. `REGISTER_SCHEMA_NAMESPACE` takes as argument
the namespace name. 

The above code declares a DEVICETWIN model. The model declaration brings in `IoTHubDeviceTwin_CreateCar` - a C function that 
creates a model instance (in this example, `Car`).

At the time of calling `IoTHubDeviceTwin_CreateCar(iotHubClientHandle)` the device twin callback is routed to an internal function
that will automatically update the desired properties.

The model instances created by `IoTHubDeviceTwin_Create`_`ModelName`_ have to be disposed of by calling `IoTHubDeviceTwin_Destroy`_`ModelName`_.

### DECLARE_DEVICETWIN_MODEL
```c
DECLARE_DEVICETWIN_MODEL(modelName,... )
```

`DECLARE_DEVICETWIN_MODEL` defines a DEVICETWIN model. DEVICETWIN models are automatically linked with an `IOTHUB_CLIENT_HANDLE` or `IOTHUB_CLIENT_LL_HANDLE` for the purpose
of receiving desired properties.

### SERIALIZER_REGISTER_NAMESPACE
```c
SERIALIZER_REGISTER_NAMESPACE(name)
```

`SERIALIZER_REGISTER_NAMESPACE` registers the constituents of a namespace.

### IoTHubDeviceTwin_Create*ModelName*
```c
IoTHubDeviceTwin_CreateModelName(IOTHUB_CLIENT_HANDLE handle);
```

`IoTHubDeviceTwin_Create`_`ModelName`_ creates a `ModelName` model instance. It also links this model instance with
a IoTHubClient handle for the purpose of receiving desired properties.

### IoTHubDeviceTwin_Destroy*ModelName*
```c
IoTHubDeviceTwin_DestroyModelName(ModelName* model);
```
`IoTHubDeviceTwin_DestroyModelName` frees all used resources by a model instance of type `ModelName`. It also unregisters the DeviceTwin callback.


### IoTHubDeviceTwin_LL_Create*ModelName*
```c
IoTHubDeviceTwin_LL_CreateModelName(IOTHUB_CLIENT_LL_HANDLE handle);
```

`IoTHubDeviceTwin_LL_Create`_`ModelName`_ creates a `ModelName` model instance. It also links this model instance with
a IoTHubClient_LL handle for the purpose of receiving desired properties.

### IoTHubDeviceTwin_LL_Destroy*ModelName*
```c
IoTHubDeviceTwin_LL_DestroyModelName(ModelName* model);
```
`IoTHubDeviceTwin_LL_DestroyModelName` frees all used resources by a model instance of type `ModelName`. It also unregisters the DeviceTwin callback.



    
    




