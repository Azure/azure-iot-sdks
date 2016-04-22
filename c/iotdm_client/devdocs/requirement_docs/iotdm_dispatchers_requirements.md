# IOTDM Dispatchers Requirements

## Overview

The dispatchers are part of the IOT DM library which are resposible for dispatching object/property operations into the relevant LWM2M object.  

It interfaces with the LWM2M transport library at the top and individual LWM2M objects at the bottom

## Lower level exposed API

```C
// bitfields with allowed operations
#define OP_R 0x01
#define OP_W 0x02
#define OP_E 0x04

// structure which defines the operations allowed on a property
typedef struct _TagDmOps {
    uint8_t propertyId;
    uint8_t allowedOp;
    lwm2m_data_type_t dataType;
} DmOps;

// function prototypes for the vtable 
typedef IOTHUB_CLIENT_RESULT(*FUNC_DISPATCH_READ_TO_OBJ_INSTANCE)(void *obj, uint16_t propertyId, lwm2m_data_t *tlvP);
typedef IOTHUB_CLIENT_RESULT(*FUNC_DISPATCH_EXEC_TO_OBJ_INSTANCE)(void *obj, uint16_t propertyId);
typedef IOTHUB_CLIENT_RESULT(*FUNC_DISPATCH_WRITE_TO_OBJ_INSTANCE)(void *obj, uint16_t propertyId, lwm2m_data_t *tlvP);

// Storage for that vtable
typedef struct _TagDmDispatchers {
    uint16_t objectId;
    const DmOps *allowedOperations;
    LIST_HANDLE instanceList;
    FUNC_DISPATCH_READ_TO_OBJ_INSTANCE readDispatcher;
    FUNC_DISPATCH_EXEC_TO_OBJ_INSTANCE execDispatcher;
    FUNC_DISPATCH_WRITE_TO_OBJ_INSTANCE writeDispatcher;
} DmDispatchers;

// A way to register that vtable
IOTHUB_CLIENT_RESULT register_dm_dispatchers(
    uint16_t objectId, 
    const DmOps *allowedOperations,
    LIST_HANDLE instanceList,
    FUNC_DISPATCH_READ_TO_OBJ_INSTANCE readDispatcher,
    FUNC_DISPATCH_EXEC_TO_OBJ_INSTANCE execDispatcher,
    FUNC_DISPATCH_WRITE_TO_OBJ_INSTANCE writeDispatcher);
```

 
## Upper level exposed API

```C
uint16_t get_property_count(uint16_t objectId, uint8_t operation);
uint16_t get_property_for_index(uint16_t objectId, uint8_t operation, uint16_t index);
lwm2m_data_type_t get_type_for_property(uint16_t objectId, uint16_t propertyId);

uint8_t dispatch_read(uint16_t objectId, uint16_t instanceId, uint16_t propertyId, lwm2m_data_t *tlvP);
uint8_t dispatch_exec(uint16_t objectId, uint16_t instanceId, uint16_t propertyId);
uint8_t dispatch_write(uint16_t objectId, uint16_t instanceId, uint16_t propertyId, lwm2m_data_t *tlvP);
```

## register_dm_dispatchers
```C
IOTHUB_CLIENT_RESULT register_dm_dispatchers(
    uint16_t objectId, 
    const DmOps *allowedOperations,
    LIST_HANDLE instanceList,
    FUNC_DISPATCH_READ_TO_OBJ_INSTANCE readDispatcher,
    FUNC_DISPATCH_EXEC_TO_OBJ_INSTANCE execDispatcher,
    FUNC_DISPATCH_WRITE_TO_OBJ_INSTANCE writeDispatcher);
```

`register_dm_dispatchers` is responsible for setting callbacks, types, and allowed operations for the properties on a given `objectId`

**SRS_DMDISPATCHERS_18_004: [** `regiter_dm_dispatchers` returns `IOTHUB_CLIENT_OK` on success **]**   
**SRS_DMDISPATCHERS_18_005: [** `register_dm_dispatchers` shall return `IOTHUB_CLIENT_OK` if registering a given objectId for a second time. **]** 

## get_property_count
```C
uint16_t get_property_count(uint16_t objectId, uint8_t operation);
```
**SRS_DMDISPATCHERS_18_001: [**`get_property_count` shall return the number of properties registered for the given `objectId` and the given `operation`**]**  
**SRS_DMDISPATCHERS_18_002: [** if the given `objectId` doesn't have any properties that support the given `operation`, `get_property_count` shall return 0 **]**  
**SRS_DMDISPATCHERS_18_003: [** if the given `objectId ` is not registered, `get_property_count` shall return `PROP_ERROR` **]**   

## get_property_for_index
```C
uint16_t get_property_for_index(uint16_t objectId, uint8_t operation, uint16_t index);
```
**SRS_DMDISPATCHERS_18_006: [** `get_property_for_index` shall return the correct `propertyId` for the given `index` and `operation` **]**   
**SRS_DMDISPATCHERS_18_007: [** if `index` is out of range, `get_property_for_index` shall return `PROP_ERROR` **]**  
**SRS_DMDISPATCHERS_18_008: [** if `objectId` is not registered, `get_property_for_index` shall return `PROP_ERROR` **]**  

## get_type_for_property
```C
lwm2m_data_type_t get_type_for_property(uint16_t objectId, uint16_t propertyId);
```

**SRS_DMDISPATCHERS_18_009: [** `get_type_for_property` shall return the correct type for the given `objectId` and `propertyId` **]**  
**SRS_DMDISPATCHERS_18_010: [** if the given `objectId` is not registered, `get_type_for_property` shall return `LWM2M_TYPE_UNDEFINED` **]**  
**SRS_DMDISPATCHERS_18_011: [** if the given `objectId` does not contain `propertyId`, `get_type_for_property` shall return `LWM2M_TYPE_UNDERFINED` **]**  

## dispatch_read
```C
uint8_t dispatch_read(uint16_t objectId, uint16_t instanceId, uint16_t propertyId, lwm2m_data_t *tlvP);
```
**SRS_DMDISPATCHERS_18_012: [** `dispatch_read` calls the correct callback for the given `objectId` and `propertyId` **]**  
**SRS_DMDISPATCHERS_18_013: [** `dispatch_read` returns `COAP_404_NOT_FOUND` if the given `objectId` is not registered **]**  
**SRS_DMDISPATCHERS_18_014: [** `dispatch_read` returns `COAP_404_NOT_FOUND` if the given `objectId` does not contain the given `propertyId` **]**  
**SRS_DMDISPATCHERS_18_015: [** `dispatch_read` returns `COAP_205_CONTENT` if the callback returns `IOTHUB_CLIENT_OK` **]**  
**SRS_DMDISPATCHERS_18_016: [** `dispatch_read` returns `COAP_501_NOT_IMPLEMENTED` if the callback returns `IOTHUB_CLIENT_NOT_IMPLEMENTED` **]**  
**SRS_DMDISPATCHERS_18_017: [** `dispatch_read` returns `COAP_400_BAD_REQUEST` if the callback returns `IOTHUB_CLIENT_INVLAID_ARG` **]**  
**SRS_DMDISPATCHERS_18_018: [** `dispatch_read` returns `COAP_405_METHOD_NOT_ALLOWED` if the `propertyId` doesn't support the read operation **]**   
**SRS_DMDISPATCHERS_18_043: [** `dispatch_read` will correctly return a string property in the correct LWM2M format **]**  
**SRS_DMDISPATCHERS_18_044: [** `dispatch_read` will correctly return an opaque property in the correct LWM2M format **]**  
**SRS_DMDISPATCHERS_18_045: [** `dispatch_read` will correcty convert an integer property into the correct LWM2M format **]**  
**SRS_DMDISPATCHERS_18_046: [** `dispatch_read` will correctly convert a float property into the correct LWM2M format **]**  
**SRS_DMDISPATCHERS_18_047: [** `dispatch_read` will correctly convert a time property into the correct LWM2M format **]**  
**SRS_DMDISPATCHERS_18_048: [** `dispatch_read` will correctly convert a bool property into the correct LWM2M format **]**  

## dispatch_exec
```C
uint8_t dispatch_exec(uint16_t objectId, uint16_t instanceId, uint16_t propertyId);
```
**SRS_DMDISPATCHERS_18_019: [** `dispatch_exec` calls the correct callback for the given `objectId` and `propertyId` **]**  
**SRS_DMDISPATCHERS_18_020: [** `dispatch_exec` returns `COAP_404_NOT_FOUND` if the given `objectId` is not registered **]**    
**SRS_DMDISPATCHERS_18_021: [** `dispatch_exec` returns `COAP_404_NOT_FOUND` if the given `objectId` does not contain the given `propertyId` **]**    
**SRS_DMDISPATCHERS_18_022: [** `dispatch_exec` returns `COAP_204_CHANGED` if the callback returns `IOTHUB_CLIENT_OK` **]**    
**SRS_DMDISPATCHERS_18_023: [** `dispatch_exec` returns `COAP_501_NOT_IMPLEMENTED` if the callback returns `IOTHUB_CLIENT_NOT_IMPLEMENTED` **]**    
**SRS_DMDISPATCHERS_18_024: [** `dispatch_exec` returns `COAP_400_BAD_REQUEST` if the callback returns `IOTHUB_CLIENT_INVLAID_ARG` **]**    
**SRS_DMDISPATCHERS_18_025: [** `dispatch_exec` returns `COAP_405_METHOD_NOT_ALLOWED` if the `propertyId` doesn't support the execute operation **]**    

## dispatch_write
```C
uint8_t dispatch_write(uint16_t objectId, uint16_t instanceId, uint16_t propertyId, lwm2m_data_t *tlvP);
```

**SRS_DMDISPATCHERS_18_026: [** `dispatch_write` calls the correct callback for the given `objectId` and `propertyId` **]**  
**SRS_DMDISPATCHERS_18_027: [** `dispatch_write` returns `COAP_404_NOT_FOUND` if the given `objectId` is not registered **]**  
**SRS_DMDISPATCHERS_18_028: [** `dispatch_write` returns `COAP_404_NOT_FOUND` if the given `objectId` does not contain the given `propertyId` **]**  
**SRS_DMDISPATCHERS_18_029: [** `dispatch_write` returns `COAP_204_CHANGED` if the callback returns `IOTHUB_CLIENT_OK` **]**  
**SRS_DMDISPATCHERS_18_030: [** `dispatch_write` returns `COAP_501_NOT_IMPLEMENTED` if the callback returns `IOTHUB_CLIENT_NOT_IMPLEMENTED` **]**  
**SRS_DMDISPATCHERS_18_031: [** `dispatch_write` returns `COAP_400_BAD_REQUEST` if the callback returns `IOTHUB_CLIENT_INVLAID_ARG` **]**  
**SRS_DMDISPATCHERS_18_032: [** `dispatch_write` returns `COAP_405_METHOD_NOT_ALLOWED` if the `propertyId` doesn't support the write operation **]**  
**SRS_DMDISPATCHERS_18_033: [** `dispatch_write` returns `COAP_400_BAD_REQUEST` for an integer property that fails to parse **]**  
**SRS_DMDISPATCHERS_18_034: [** `dispatch_write` returns `COAP_400_BAD_REQUEST` for a time property that fails to parse **]**  
**SRS_DMDISPATCHERS_18_035: [** `dispatch_write` returns `COAP_400_BAD_REQUEST` for a float property that fails to parse **]**  
**SRS_DMDISPATCHERS_18_036: [** `dispatch_write` returns `COAP_400_BAD_REQUEST` for a bool property that fails to parse **]**  
**SRS_DMDISPATCHERS_18_037: [** `dispatch_write` correctly passes a string argument into the callback **]**  
**SRS_DMDISPATCHERS_18_038: [** `dispatch_write` correctly passes a oppaque argument into the callback **]**  
**SRS_DMDISPATCHERS_18_039: [** `dispatch_write` correctly parses an well-formed integer argument and passes it into the callback **]**  
**SRS_DMDISPATCHERS_18_040: [** `dispatch_write` correctly parses an well-formed time argument and passes it into the callback **]**  
**SRS_DMDISPATCHERS_18_041: [** `dispatch_write` correctly parses an well-formed float argument and passes it into the callback **]**  
**SRS_DMDISPATCHERS_18_042: [** `dispatch_write` correctly parses an well-formed bool argument and passes it into the callback **]**  
