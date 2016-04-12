#ifndef _IOTDM_DISPATCHERS_H_
#define _IOTDM_DISPATCHERS_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

///////////////////////////////////////////////////////////////////////////////////////////////////
// List of allowed operations
///////////////////////////////////////////////////////////////////////////////////////////////////

// bitfields with allowed operations
#define OP_R 0x01
#define OP_W 0x02
#define OP_E 0x04
#define OP_RW (OP_R | OP_W)
#define OP_RE (OP_R | OP_E)
#define OP_WE (OP_W | OP_E)
#define OP_RWE (OP_R | OP_W | OP_E)

// structure which defines the operations allowed on a property
typedef struct _TagDmOps {
    uint16_t propertyId;
    uint8_t allowedOp;
    lwm2m_data_type_t dataType;
} DmOps;

#define ALLOWED_OP_LIST(x) allowed ## x ## operations
#define BEGIN_OPERATION_LIST(x)  const DmOps ALLOWED_OP_LIST(x)[] = {
#define OPERATION(x,y,z) { x, y, z },
#define END_OPERATION_LIST() { 0, 0, LWM2M_TYPE_UNDEFINED} };

typedef struct _TagDmDispatchers DmDispatchers;
uint8_t operation_is_allowed(DmDispatchers *disp, uint16_t propertyId, uint8_t operation);

#define PROP_ERROR (0xffff) 
uint16_t get_property_count(uint16_t objectId, uint8_t operation);
uint16_t get_property_for_index(uint16_t objectId, uint8_t operation, uint16_t index);
lwm2m_data_type_t get_type_for_property(uint16_t objectId, uint16_t propertyId);
void *get_object_instance(uint16_t objectId, uint16_t instanceId);
void signal_all_resource_changes();


///////////////////////////////////////////////////////////////////////////////////////////////////
// A vtable that we use to call in to objects.
///////////////////////////////////////////////////////////////////////////////////////////////////

// function prototypes for the vtable 
typedef IOTHUB_CLIENT_RESULT(*FUNC_DISPATCH_READ_TO_OBJ_INSTANCE)(void *obj, uint16_t propertyId, lwm2m_data_t *tlvP);
typedef IOTHUB_CLIENT_RESULT(*FUNC_DISPATCH_EXEC_TO_OBJ_INSTANCE)(void *obj, uint16_t propertyId);
typedef IOTHUB_CLIENT_RESULT(*FUNC_DISPATCH_WRITE_TO_OBJ_INSTANCE)(void *obj, uint16_t propertyId, lwm2m_data_t *tlvP);
typedef void(*FUNC_SIGNAL_RESOURCE_CHANGE)(void *obj);

// Storage for that vtable
typedef struct _TagDmDispatchers {
    uint16_t objectId;
    const DmOps *allowedOperations;
    LIST_HANDLE *instanceList;
    FUNC_DISPATCH_READ_TO_OBJ_INSTANCE readDispatcher;
    FUNC_DISPATCH_EXEC_TO_OBJ_INSTANCE execDispatcher;
    FUNC_DISPATCH_WRITE_TO_OBJ_INSTANCE writeDispatcher;
    FUNC_SIGNAL_RESOURCE_CHANGE changeSignaller;
} DmDispatchers;

// A way to register that vtable
IOTHUB_CLIENT_RESULT register_dm_dispatchers(
    uint16_t objectId, 
    const DmOps *allowedOperations,
    LIST_HANDLE *instanceList,
    FUNC_DISPATCH_READ_TO_OBJ_INSTANCE readDispatcher,
    FUNC_DISPATCH_EXEC_TO_OBJ_INSTANCE execDispatcher,
    FUNC_DISPATCH_WRITE_TO_OBJ_INSTANCE writeDispatcher,
    FUNC_SIGNAL_RESOURCE_CHANGE changeSignaller);

// A set of ways we can call into various vtables
uint8_t dispatch_read(uint16_t objectId, uint16_t instanceId, uint16_t propertyId, lwm2m_data_t *tlvP);
uint8_t dispatch_exec(uint16_t objectId, uint16_t instanceId, uint16_t propertyId);
uint8_t dispatch_write(uint16_t objectId, uint16_t instanceId, uint16_t propertyId, lwm2m_data_t *tlvP);

///////////////////////////////////////////////////////////////////////////////////////////////////
// Helper functions that take an object instance and call the individual callbacks
///////////////////////////////////////////////////////////////////////////////////////////////////

// functions to call the type-specific read callback and pack the data into an lwm2m_data_t structure
#define FNAME_DISPATCH_READ_TO_CALLBACK(t) dispatch_ ## t ## _read_to_callback
#define PROTOTYPE_DISPATCH_READ_TO_CALLBACK(t) IOTHUB_CLIENT_RESULT FNAME_DISPATCH_READ_TO_CALLBACK(t) (void *obj, void *function, lwm2m_data_t *tlvP)
PROTOTYPE_DISPATCH_READ_TO_CALLBACK(string);
PROTOTYPE_DISPATCH_READ_TO_CALLBACK(integer);
PROTOTYPE_DISPATCH_READ_TO_CALLBACK(time);
PROTOTYPE_DISPATCH_READ_TO_CALLBACK(opaque);
PROTOTYPE_DISPATCH_READ_TO_CALLBACK(float);
PROTOTYPE_DISPATCH_READ_TO_CALLBACK(bool);

// function to call a specific exec callback
#define FNAME_DISPATCH_EXECUTE_TO_CALLBACK dispatch_exec_to_callback
IOTHUB_CLIENT_RESULT FNAME_DISPATCH_EXECUTE_TO_CALLBACK (void *obj, void *function);

// functions to unpack a lwm2m_data_t strucure and call a type-specific write callback
#define FNAME_DISPATCH_WRITE_TO_CALLBACK(t) dispatch_ ## t ## _write_to_callback
#define PROTOTYPE_DISPATCH_WRITE_TO_CALLBACK(t) IOTHUB_CLIENT_RESULT FNAME_DISPATCH_WRITE_TO_CALLBACK(t) (void *obj, void *function, lwm2m_data_t *tlvP)
PROTOTYPE_DISPATCH_WRITE_TO_CALLBACK(string);
PROTOTYPE_DISPATCH_WRITE_TO_CALLBACK(integer);
PROTOTYPE_DISPATCH_WRITE_TO_CALLBACK(time);
PROTOTYPE_DISPATCH_WRITE_TO_CALLBACK(opaque);
PROTOTYPE_DISPATCH_WRITE_TO_CALLBACK(float);
PROTOTYPE_DISPATCH_WRITE_TO_CALLBACK(bool);

///////////////////////////////////////////////////////////////////////////////////////////////////
// Object-specific dispatchers
///////////////////////////////////////////////////////////////////////////////////////////////////

// default function names for our dispatcher objects
#define INSTANCE_GETTER_NAME(x) get_ ## x ## _instance
#define READ_DISPATCHER_NAME(x) dispatch_ ## x ## _read
#define EXEC_DISPATCHER_NAME(x) dispatch_ ## x ## _exec
#define WRITE_DISPATCHER_NAME(x) dispatch_ ## x ## _write
#define SIGNAL_RESOURCE_CHANGE_NAME(x) signal_ ## x ## _resource_changed

// And an easy macro to wrap vtable registration
#define REGISTER_DISPATCHERS(id, x) \
    register_dm_dispatchers(id, \
        &ALLOWED_OP_LIST(x)[0], \
        &x ## _instance_list, \
        READ_DISPATCHER_NAME(x), \
        EXEC_DISPATCHER_NAME(x), \
        WRITE_DISPATCHER_NAME(x), \
        SIGNAL_RESOURCE_CHANGE_NAME(x))

// Dispatcher prototypes
#define PROTOTYPE_READ_DISPATCHER(x) IOTHUB_CLIENT_RESULT READ_DISPATCHER_NAME(x) (void *obj, uint16_t propertyId, lwm2m_data_t *tlvP)
#define PROTOTYPE_EXEC_DISPATCHER(x) IOTHUB_CLIENT_RESULT EXEC_DISPATCHER_NAME(x) (void *obj, uint16_t propertyId)
#define PROTOTYPE_WRITE_DISPATCHER(x) IOTHUB_CLIENT_RESULT WRITE_DISPATCHER_NAME(x) (void *obj, uint16_t propertyId, lwm2m_data_t *tlvP)
#define PROTOTYPE_CHANGE_SIGNALLER(x) void SIGNAL_RESOURCE_CHANGE_NAME(x) (void *obj);

// An easy way to pre-declare our dispatchers
#define DISPATCHER_FORWARD_DECLARATIONS(x) \
    PROTOTYPE_READ_DISPATCHER(x); \
    PROTOTYPE_EXEC_DISPATCHER(x); \
    PROTOTYPE_WRITE_DISPATCHER(x); \
    PROTOTYPE_CHANGE_SIGNALLER(x); \
    extern const DmOps ALLOWED_OP_LIST(x)[]; \
    static LIST_HANDLE x ## _instance_list = NULL;


// The beginning of every dispatcher
#define DISPATCHER_PREFIX(x) \
{ \
    IOTHUB_CLIENT_RESULT res = IOTHUB_CLIENT_NOT_IMPLEMENTED; \
    switch (propertyId) \
    {

// The end of every dispatcher
#define DISPATCHER_SUFFIX() \
        case PROP_ERROR: \
        default: \
            break; \
    } \
    return res; \
}


// Start of read dispatcher
#define BEGIN_READ_DISPATCHER(x) \
PROTOTYPE_READ_DISPATCHER(x) \
DISPATCHER_PREFIX(x)

// type specific read macros
#define TYPED_READ(x, y, type) \
        case x: \
            res = FNAME_DISPATCH_READ_TO_CALLBACK(type)(obj, (void*)y, tlvP); \
            break;

#define STRING_READ(x, y) TYPED_READ(x, y, string)

#define INTEGER_READ(x, y) TYPED_READ(x, y, integer)

#define TIME_READ(x, y) TYPED_READ(x, y, time)

#define OPAQUE_READ(x, y) TYPED_READ(x, y, opaque)

#define FLOAT_READ(x, y) TYPED_READ(x, y, float)

#define BOOL_READ(x, y) TYPED_READ(x, y, bool)

// end of the read dispatcher
#define END_READ_DISPATCHER() DISPATCHER_SUFFIX()

// Start of the exec dispatcher
#define BEGIN_EXEC_DISPATCHER(x)  \
PROTOTYPE_EXEC_DISPATCHER(x) \
DISPATCHER_PREFIX(x)

// macro for specific actions
#define EXEC(x, y) \
    case x: \
        res = dispatch_exec_to_callback(obj, (void*)y); \
        break;

// end of the exec dispatcher
#define END_EXEC_DISPATCHER() DISPATCHER_SUFFIX()

// start of the write dispatcher
#define BEGIN_WRITE_DISPATCHER(x)  \
PROTOTYPE_WRITE_DISPATCHER(x) \
DISPATCHER_PREFIX(x)

// macros for type-specific writes
#define TYPED_WRITE(x, y, type) \
        case x: \
            res = dispatch_ ## type ## _write_to_callback(obj, (void*)y, tlvP); \
            break;

#define TIME_WRITE(x, y) TYPED_WRITE(x, y, time)

#define STRING_WRITE(x, y) TYPED_WRITE(x, y, string)

#define INTEGER_WRITE(x, y) TYPED_WRITE(x, y, integer)

#define OPAQUE_WRITE(x, y)  TYPED_WRITE(x, y, opaque)

#define FLOAT_WRITE(x, y)  TYPED_WRITE(x, y, float)

#define BOOL_WRITE(x, y)  TYPED_WRITE(x, y, bool)

// end of the write dispatcher
#define END_WRITE_DISPATCHER() DISPATCHER_SUFFIX()

#ifdef __cplusplus
}
#endif // __cplusplus


#endif // _IOTDM_DISPATCHERS_H_
