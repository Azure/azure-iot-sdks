// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <cstdlib>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "testrunnerswitcher.h"

extern "C" 
{
    #include "iotdm_internal.h"
    #include "iotdm_dispatchers.h"


    #define bool	_Bool
    #define false	0
    #define true	1

    bool lwm2m_session_is_equal(void *lhs, void *rhs, void *userData)
    {
        return (lhs == rhs);
    }
    
    // We don't use this, but we need to keep the linker happy and including observe.c 
    // opens up a world of pain.
    lwm2m_observed_t * observe_findByUri(lwm2m_context_t * contextP, lwm2m_uri_t * uriP)
    {
        return NULL;
    }

}

// BKTODO: unit test for two object instances
//////////////////////////////////////////////////////////////////////////////////////////////////
// Fake object that only has read properties
//////////////////////////////////////////////////////////////////////////////////////////////////

// object and property IDs
#define OID_FAKE_READONLY 1001
#define PID_READ_VALID_STRING 101
#define PID_READ_VALID_INTEGER 201
#define PID_READ_VALID_FLOAT 301
#define PID_READ_VALID_TIME 401
#define PID_READ_VALID_OPAQUE 501
#define PID_READ_VALID_BOOL 601
#define PID_READ_INT_NOTIMPL 701
#define PID_READ_INT_INVALIDARG 801

// Number of read properties in FAKE_READONLY object
#define COUNT_OF_FAKE_READ_PROPERTIES 8

// Index of PID_READ_VALID_TIME 
#define INDEX_PID_READ_VALID_TIME 3

// Unregistered object
#define OID_UNREGISTERED 2002

// property that isn't part of the FAKE_READONLY object
#define PID_NOT_IN_FAKE_READONLY_OBJECT 999

// instance ID to use for our fake readonly object
#define INSTANCE_ID_FAKE_READONLY_OBJECT 42

// Dispatcher declarations for our fake object
DISPATCHER_FORWARD_DECLARATIONS(object_fake_readonly);

// Values returned from property getters
#define VALID_INT 0x8675309
#define VALID_FLOAT 867.5307
#define VALID_TIME 1311379200
#define VALID_BOOL true
#define VALID_STRING "valid string"

// storage for our fake object
typedef struct TAG_object_fake_readonly{
    uint16_t instanceId;    // Must be first member of structure
    bool read_valid_int_called;
} object_fake_readonly;

void free_fake_readonly_object_submembers(void *obj)
{
    object_fake_readonly *p = (object_fake_readonly*)obj;
    // TODO: add free logic for any members of object_fake_readonly
}

void signal_object_fake_readonly_resource_changed(void *obj)
{
}

static IOTHUB_CLIENT_RESULT on_read_valid_string(object_fake_readonly *obj, char **value)
{
    *value = lwm2m_strdup(VALID_STRING);
    return IOTHUB_CLIENT_OK;
}

static IOTHUB_CLIENT_RESULT on_read_valid_int(object_fake_readonly *obj, int *value)
{
    *value = VALID_INT;
    obj->read_valid_int_called = true;
    return IOTHUB_CLIENT_OK;
}

static IOTHUB_CLIENT_RESULT on_read_valid_float(object_fake_readonly *obj, double *value)
{
    *value = VALID_FLOAT;
    return IOTHUB_CLIENT_OK;
}

static IOTHUB_CLIENT_RESULT on_read_valid_time(object_fake_readonly *obj, time_t *value)
{
    *value = VALID_TIME;
    return IOTHUB_CLIENT_OK;
}

static IOTHUB_CLIENT_RESULT on_read_valid_opaque(object_fake_readonly *obj, void **value, size_t *length)
{
    *value = lwm2m_strdup(VALID_STRING);
    *length = strlen((char*)*value) + 1;
    return IOTHUB_CLIENT_OK;
}

static IOTHUB_CLIENT_RESULT on_read_valid_bool(object_fake_readonly *obj, bool *value)
{
    *value = VALID_BOOL;
    return IOTHUB_CLIENT_OK;
}

static IOTHUB_CLIENT_RESULT on_read_int_notimpl(object_fake_readonly *obj, int *value)
{
    return IOTHUB_CLIENT_NOT_IMPLEMENTED;
}

static IOTHUB_CLIENT_RESULT on_read_int_invalidarg(object_fake_readonly *obj, int *value)
{
    return IOTHUB_CLIENT_INVALID_ARG;
}

BEGIN_READ_DISPATCHER(object_fake_readonly)
    STRING_READ(PID_READ_VALID_STRING, on_read_valid_string)
    INTEGER_READ(PID_READ_VALID_INTEGER, on_read_valid_int)
    FLOAT_READ(PID_READ_VALID_FLOAT, on_read_valid_float)
    TIME_READ(PID_READ_VALID_TIME, on_read_valid_time)
    OPAQUE_READ(PID_READ_VALID_OPAQUE, on_read_valid_opaque)
    BOOL_READ(PID_READ_VALID_BOOL, on_read_valid_bool)
    INTEGER_READ(PID_READ_INT_NOTIMPL, on_read_int_notimpl)
    INTEGER_READ(PID_READ_INT_INVALIDARG, on_read_int_invalidarg)
END_READ_DISPATCHER()

BEGIN_EXEC_DISPATCHER(object_fake_readonly)
END_EXEC_DISPATCHER()

BEGIN_WRITE_DISPATCHER(object_fake_readonly)
END_WRITE_DISPATCHER()

BEGIN_OPERATION_LIST(object_fake_readonly)
    OPERATION(PID_READ_VALID_STRING, OP_R, LWM2M_TYPE_STRING)
    OPERATION(PID_READ_VALID_INTEGER, OP_R, LWM2M_TYPE_INTEGER)
    OPERATION(PID_READ_VALID_FLOAT, OP_R, LWM2M_TYPE_FLOAT)
    OPERATION(PID_READ_VALID_TIME, OP_R, LWM2M_TYPE_INTEGER)
    OPERATION(PID_READ_VALID_OPAQUE, OP_R, LWM2M_TYPE_OPAQUE)
    OPERATION(PID_READ_VALID_BOOL, OP_R, LWM2M_TYPE_BOOLEAN)
    OPERATION(PID_READ_INT_NOTIMPL, OP_R, LWM2M_TYPE_INTEGER)
    OPERATION(PID_READ_INT_INVALIDARG, OP_R, LWM2M_TYPE_INTEGER)
END_OPERATION_LIST()

object_fake_readonly *create_and_register_fake_readonly_object()
{
    (void)REGISTER_DISPATCHERS(OID_FAKE_READONLY, object_fake_readonly);

    object_fake_readonly* obj = (object_fake_readonly*)lwm2m_malloc(sizeof(*obj));
    if (obj != NULL)
    {
        memset(obj, 0, sizeof(*obj));
        obj->instanceId = INSTANCE_ID_FAKE_READONLY_OBJECT;
    }

    IOTHUB_CLIENT_RESULT res = add_dm_object(&object_fake_readonly_instance_list, obj);
    if (res != IOTHUB_CLIENT_OK)
    {
        lwm2m_free(obj);
        obj = NULL;
    }

    return obj;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// Fake object that write and exec properties
//////////////////////////////////////////////////////////////////////////////////////////////////

// object and property IDs
#define OID_FAKE_WRITEEXEC 1002
#define PID_WRITE_STRING 102
#define PID_WRITE_INTEGER 202
#define PID_WRITE_FLOAT 302
#define PID_WRITE_TIME 402
#define PID_WRITE_OPAQUE 502
#define PID_WRITE_BOOL 602
#define PID_WRITE_NOTIMPL 1002
#define PID_WRITE_INVALIDARG 1102
#define PID_EXEC 702
#define PID_EXEC_INVALIDARG 802
#define PID_EXEC_NOTIMPL 902

// instance ID for our write/exec object
#define INSTANCE_ID_FAKE_WRITEEXEC_OBJECT 43

// Unparsable string -- not an int or bool or float or time
#define UNPARSABLE_STRING "unparsable"

// storage for our fake object
typedef struct TAG_object_fake_writeexec {
    uint16_t instanceId;    // Must be first member of structure
    bool exec_called;
    bool write_integer_called;
    int64_t integer_property;
    bool bool_property;
    double float_property;
    int64_t time_property;
    uint8_t *opaque_property;
    size_t opaque_length;
    char *string_property;
} object_fake_writeexec;

// Dispatcher declarations for our fake object
DISPATCHER_FORWARD_DECLARATIONS(object_fake_writeexec);

IOTHUB_CLIENT_RESULT on_write_string(object_fake_writeexec *obj, char *value)
{
    ASSERT_IS_NULL(obj->string_property);
    obj->string_property = (char*)lwm2m_malloc(strlen(value)+1);
    memcpy(obj->string_property, value, strlen(value) + 1);
    return IOTHUB_CLIENT_OK;
}

IOTHUB_CLIENT_RESULT on_write_integer(object_fake_writeexec *obj, int value)
{
    obj->write_integer_called = true;
    obj->integer_property = value;
    return IOTHUB_CLIENT_OK;
}

IOTHUB_CLIENT_RESULT on_write_float(object_fake_writeexec *obj, double value)
{
    obj->float_property = value;
    return IOTHUB_CLIENT_OK;
}

IOTHUB_CLIENT_RESULT on_write_time(object_fake_writeexec *obj, int value)
{
    obj->time_property = value;
    return IOTHUB_CLIENT_OK;
}

IOTHUB_CLIENT_RESULT on_write_opaque(object_fake_writeexec *obj, void *value, size_t length)
{
    obj->opaque_length = length;
    ASSERT_IS_NULL(obj->opaque_property);
    obj->opaque_property = (uint8_t*)lwm2m_malloc(length);
    memcpy(obj->opaque_property, value, length);
    return IOTHUB_CLIENT_OK;
}

IOTHUB_CLIENT_RESULT on_write_bool(object_fake_writeexec *obj, bool value)
{
    obj->bool_property = value;
    return IOTHUB_CLIENT_OK;
}

IOTHUB_CLIENT_RESULT on_write_invalidarg(object_fake_writeexec *obj, int value)
{
    return IOTHUB_CLIENT_INVALID_ARG;
}

IOTHUB_CLIENT_RESULT on_write_notimpl(object_fake_writeexec *obj, int value)
{
    return IOTHUB_CLIENT_NOT_IMPLEMENTED;
}

IOTHUB_CLIENT_RESULT on_exec(object_fake_writeexec *obj)
{
    obj->exec_called = true;
    return IOTHUB_CLIENT_OK;
}

IOTHUB_CLIENT_RESULT on_exec_invalidarg(object_fake_writeexec *obj)
{
    return IOTHUB_CLIENT_INVALID_ARG;
}

IOTHUB_CLIENT_RESULT on_exec_notimpl(object_fake_writeexec *obj)
{
    return IOTHUB_CLIENT_NOT_IMPLEMENTED;
}

BEGIN_READ_DISPATCHER(object_fake_writeexec)
END_READ_DISPATCHER()

BEGIN_EXEC_DISPATCHER(object_fake_writeexec)
    EXEC(PID_EXEC, on_exec)
    EXEC(PID_EXEC_INVALIDARG, on_exec_invalidarg)
    EXEC(PID_EXEC_NOTIMPL, on_exec_notimpl)
END_EXEC_DISPATCHER()

BEGIN_WRITE_DISPATCHER(object_fake_writeexec)
    STRING_WRITE(PID_WRITE_STRING, on_write_string)
    INTEGER_WRITE(PID_WRITE_INTEGER, on_write_integer)
    FLOAT_WRITE(PID_WRITE_FLOAT, on_write_float)
    TIME_WRITE(PID_WRITE_TIME, on_write_time)
    OPAQUE_WRITE(PID_WRITE_OPAQUE, on_write_opaque)
    BOOL_WRITE(PID_WRITE_BOOL, on_write_bool)
    INTEGER_WRITE(PID_WRITE_NOTIMPL, on_write_notimpl)
    INTEGER_WRITE(PID_WRITE_INVALIDARG, on_write_invalidarg)
END_WRITE_DISPATCHER()

BEGIN_OPERATION_LIST(object_fake_writeexec)
    OPERATION(PID_WRITE_STRING, OP_W, LWM2M_TYPE_STRING)
    OPERATION(PID_WRITE_INTEGER, OP_W, LWM2M_TYPE_INTEGER)
    OPERATION(PID_WRITE_FLOAT, OP_W, LWM2M_TYPE_FLOAT)
    OPERATION(PID_WRITE_TIME, OP_W, LWM2M_TYPE_INTEGER)
    OPERATION(PID_WRITE_OPAQUE, OP_W, LWM2M_TYPE_OPAQUE)
    OPERATION(PID_WRITE_BOOL, OP_W, LWM2M_TYPE_BOOLEAN )
    OPERATION(PID_WRITE_NOTIMPL, OP_W, LWM2M_TYPE_INTEGER)
    OPERATION(PID_WRITE_INVALIDARG, OP_W, LWM2M_TYPE_INTEGER)
    OPERATION(PID_EXEC, OP_E, LWM2M_TYPE_UNDEFINED)
    OPERATION(PID_EXEC_INVALIDARG, OP_E, LWM2M_TYPE_UNDEFINED)
    OPERATION(PID_EXEC_NOTIMPL, OP_E, LWM2M_TYPE_UNDEFINED)
END_OPERATION_LIST()

void free_fake_writeexec_object_submembers(void *obj)
{
    object_fake_writeexec *p = (object_fake_writeexec*)obj;
    if (p->opaque_property)
    {
        lwm2m_free(p->opaque_property);
        p->opaque_property = NULL;
    }
    if (p->string_property)
    {
        lwm2m_free(p->string_property);
        p->string_property = NULL;
    }
    // TODO: add free logic for any members of object_fake_writeexec
}

void signal_object_fake_writeexec_resource_changed(void *obj)
{
}

object_fake_writeexec *create_and_register_fake_writeexec_object()
{
    (void)REGISTER_DISPATCHERS(OID_FAKE_WRITEEXEC, object_fake_writeexec);

    object_fake_writeexec* obj = (object_fake_writeexec*)lwm2m_malloc(sizeof(*obj));
    if (obj != NULL)
    {
        memset(obj, 0, sizeof(*obj));
        obj->instanceId = INSTANCE_ID_FAKE_WRITEEXEC_OBJECT;
    }

    IOTHUB_CLIENT_RESULT res = add_dm_object(&object_fake_writeexec_instance_list, obj);
    if (res != IOTHUB_CLIENT_OK)
    {
        lwm2m_free(obj);
        obj = NULL;
    }

    return obj;
}

void free_lwm2m_data_t(lwm2m_data_t t)
{
    if (t.type == LWM2M_TYPE_STRING || t.type == LWM2M_TYPE_OPAQUE)
    {
        lwm2m_free(t.value.asBuffer.buffer);
    }
}


#define BYTE1(x) ((uint8_t)(((uint32_t)x) >> 24))
#define BYTE2(x) ((uint8_t)(((uint32_t)x) >> 16))
#define BYTE3(x) ((uint8_t)(((uint32_t)x) >> 8))
#define BYTE4(x) ((uint8_t)(((uint32_t)x) >> 0))

BEGIN_TEST_SUITE(iotdm_dispatchers_unittests)

    TEST_FUNCTION_INITIALIZE(BeforeEach)
    {
        test_clear_dm_object_list(&object_fake_readonly_instance_list, free_fake_readonly_object_submembers);
        test_clear_dm_object_list(&object_fake_writeexec_instance_list, free_fake_writeexec_object_submembers);
    }

    // Tests_SRS_DMDISPATCHERS_18_004: [ regiter_dm_dispatchers returns IOTHUB_CLIENT_OK on success ]
    TEST_FUNCTION(register_dm_dispatchers_returns_success)
    {
        IOTHUB_CLIENT_RESULT res = REGISTER_DISPATCHERS(OID_FAKE_READONLY, object_fake_readonly);
       
        ASSERT_ARE_EQUAL(int, res, IOTHUB_CLIENT_OK);
    }

    // Tests_SRS_DMDISPATCHERS_18_005: [ register_dm_dispatchers shall return IOTHUB_CLIENT_OK if registering a given objectId for a second time. ]	
    TEST_FUNCTION(register_dm_dispatchers_returns_success_if_called_a_second_time)
    {
        IOTHUB_CLIENT_RESULT res = REGISTER_DISPATCHERS(OID_FAKE_READONLY, object_fake_readonly);
        res = REGISTER_DISPATCHERS(OID_FAKE_READONLY, object_fake_readonly);

        ASSERT_ARE_EQUAL(int, res, IOTHUB_CLIENT_OK);
    }

    // Tests_SRS_DMDISPATCHERS_18_001: [get_property_count shall return the number of properties registered for the given objectId and the given operation]
    TEST_FUNCTION(get_property_count_returns_correct_number_of_registered_properties)
    {
        (void)REGISTER_DISPATCHERS(OID_FAKE_READONLY, object_fake_readonly);

        uint16_t count = get_property_count(OID_FAKE_READONLY, OP_R);
        ASSERT_ARE_EQUAL(int, count, COUNT_OF_FAKE_READ_PROPERTIES);
    }

    // Tests_SRS_DMDISPATCHERS_18_002: [ if the given objectId doesn't have any properties that support the given operation, get_property_count shall return 0 ]
    TEST_FUNCTION(get_property_count_returns_0_if_the_operation_isnt_supported)
    {
        (void)REGISTER_DISPATCHERS(OID_FAKE_READONLY, object_fake_readonly);

        uint16_t count = get_property_count(OID_FAKE_READONLY, OP_W);
        ASSERT_ARE_EQUAL(int, count, 0);
    }

    // Tests_SRS_DMDISPATCHERS_18_003: [ if the given objectId is not registered, get_property_count shall return PROP_ERROR ]
    TEST_FUNCTION(get_property_count_returns_error_for_unregistered_object)
    {
        uint16_t count = get_property_count(OID_UNREGISTERED, OP_W);
        ASSERT_ARE_EQUAL(int, count, PROP_ERROR);
    }

    // Tests_SRS_DMDISPATCHERS_18_006: [ get_property_for_index shall return the correct propertyId for the given index and operation ]
    TEST_FUNCTION(get_property_for_index_returns_correct_property_id)
    {
        (void)REGISTER_DISPATCHERS(OID_FAKE_READONLY, object_fake_readonly);

        uint16_t propertyId = get_property_for_index(OID_FAKE_READONLY, OP_R, INDEX_PID_READ_VALID_TIME);
        ASSERT_ARE_EQUAL(int, propertyId, PID_READ_VALID_TIME);
    }

    // Tests_SRS_DMDISPATCHERS_18_007: [ if index is out of range, get_property_for_index shall return PROP_ERROR ]
    TEST_FUNCTION(get_property_for_index_returns_error_when_out_of_range)
    {
        (void)REGISTER_DISPATCHERS(OID_FAKE_READONLY, object_fake_readonly);

        uint16_t propertyId = get_property_for_index(OID_FAKE_READONLY, OP_R, COUNT_OF_FAKE_READ_PROPERTIES);
        ASSERT_ARE_EQUAL(int, propertyId, PROP_ERROR);
    }

    // Tests_SRS_DMDISPATCHERS_18_008: [ if objectId is not registered, get_property_for_index shall return PROP_ERROR ]
    TEST_FUNCTION(get_property_for_index_returns_error_for_unregistered_objectId)
    {
        uint16_t propertyId = get_property_for_index(OID_UNREGISTERED, OP_R, INDEX_PID_READ_VALID_TIME);
        ASSERT_ARE_EQUAL(int, propertyId, PROP_ERROR);
    }

    //  Tests_SRS_DMDISPATCHERS_18_009: [ get_type_for_property shall return the correct type for the given objectId and propertyId ]
    TEST_FUNCTION(get_type_for_property_returns_correct_type)
    {
        (void)REGISTER_DISPATCHERS(OID_FAKE_READONLY, object_fake_readonly);

        lwm2m_data_type_t type = get_type_for_property(OID_FAKE_READONLY, PID_READ_VALID_FLOAT);
        ASSERT_ARE_EQUAL(int, type, LWM2M_TYPE_FLOAT);

    }

    // Tests_SRS_DMDISPATCHERS_18_010: [ if the given objectId is not registered, get_type_for_property shall return LWM2M_TYPE_UNDEFINED ]
    TEST_FUNCTION(get_type_for_property_returns_undefined_for_unregistereed_objectId)
    {
        lwm2m_data_type_t type = get_type_for_property(OID_UNREGISTERED, PID_READ_VALID_FLOAT);
        ASSERT_ARE_EQUAL(int, type, LWM2M_TYPE_UNDEFINED);
    }

    // Tests_SRS_DMDISPATCHERS_18_011: [ if the given objectId does not contain propertyId, get_type_for_property shall return LWM2M_TYPE_UNDERFINED ]
    TEST_FUNCTION(get_type_for_property_returns_undefined_for_nonexistant_property)
    {
        (void)REGISTER_DISPATCHERS(OID_FAKE_READONLY, object_fake_readonly);

        lwm2m_data_type_t type = get_type_for_property(OID_FAKE_READONLY, PID_NOT_IN_FAKE_READONLY_OBJECT);
        ASSERT_ARE_EQUAL(int, type, LWM2M_TYPE_UNDEFINED);
    }


    // Tests_SRS_DMDISPATCHERS_18_012: [ dispatch_read calls the correct callback for the given objectId and propertyId ]
    TEST_FUNCTION(dispatch_read_calls_correct_callback)
    {
        object_fake_readonly *obj = create_and_register_fake_readonly_object();
        lwm2m_data_t data;
        memset(&data, 0, sizeof(data));

        (void)dispatch_read(OID_FAKE_READONLY, INSTANCE_ID_FAKE_READONLY_OBJECT, PID_READ_VALID_INTEGER, &data);

        ASSERT_IS_TRUE(obj->read_valid_int_called);
        free_lwm2m_data_t(data);
    }

    // Tests_SRS_DMDISPATCHERS_18_015: [ dispatch_read returns COAP_205_CONTENT if the callback returns IOTHUB_CLIENT_OK ]
    TEST_FUNCTION(dispatch_read_returns_correct_result_on_callback_success)
    {
        object_fake_readonly *obj = create_and_register_fake_readonly_object();
        lwm2m_data_t data;
        memset(&data, 0, sizeof(data));

        uint8_t res = dispatch_read(OID_FAKE_READONLY, INSTANCE_ID_FAKE_READONLY_OBJECT, PID_READ_VALID_INTEGER, &data);

        ASSERT_ARE_EQUAL(int, res, COAP_205_CONTENT);
        free_lwm2m_data_t(data);
    }

    // Tests_SRS_DMDISPATCHERS_18_013: [ dispatch_read returns COAP_404_NOT_FOUND if the given objectId is not registered ]
    TEST_FUNCTION(dispatch_read_returns_correct_error_for_unregistered_object)
    {
        object_fake_readonly *obj = create_and_register_fake_readonly_object();
        lwm2m_data_t data;
        memset(&data, 0, sizeof(data));

        uint8_t res = dispatch_read(OID_UNREGISTERED, INSTANCE_ID_FAKE_READONLY_OBJECT, PID_READ_VALID_INTEGER, &data);

        ASSERT_ARE_EQUAL(int, res, COAP_404_NOT_FOUND);
        free_lwm2m_data_t(data);
    }

    // Tests_SRS_DMDISPATCHERS_18_014: [ dispatch_read returns COAP_404_NOT_FOUND if the given objectId does not contain the given propertyId ]
    TEST_FUNCTION(dispatch_read_returns_correct_error_for_nonexistant_propertyId)
    {
        object_fake_readonly *obj = create_and_register_fake_readonly_object();
        lwm2m_data_t data;
        memset(&data, 0, sizeof(data));

        uint8_t res = dispatch_read(OID_FAKE_READONLY, INSTANCE_ID_FAKE_READONLY_OBJECT, PID_NOT_IN_FAKE_READONLY_OBJECT, &data);

        ASSERT_ARE_EQUAL(int, res, COAP_404_NOT_FOUND);
        free_lwm2m_data_t(data);
    }

    // Tests_SRS_DMDISPATCHERS_18_016: [ dispatch_read returns COAP_501_NOT_IMPLEMENTED if the callback returns IOTHUB_CLIENT_NOT_IMPLEMENTED ]
    TEST_FUNCTION(dispatch_read_returns_correct_error_for_unimplemented_callback)
    {
        object_fake_readonly *obj = create_and_register_fake_readonly_object();
        lwm2m_data_t data;
        memset(&data, 0, sizeof(data));

        uint8_t res = dispatch_read(OID_FAKE_READONLY, INSTANCE_ID_FAKE_READONLY_OBJECT, PID_READ_INT_NOTIMPL, &data);

        ASSERT_ARE_EQUAL(int, res, COAP_501_NOT_IMPLEMENTED);
        free_lwm2m_data_t(data);
    }

    // Tests_SRS_DMDISPATCHERS_18_017: [ dispatch_read returns COAP_400_BAD_REQUEST if the callback returns IOTHUB_CLIENT_INVLAID_ARG ]
    TEST_FUNCTION(dispatch_read_returns_correct_error_for_invalid_argument)
    {
        object_fake_readonly *obj = create_and_register_fake_readonly_object();
        lwm2m_data_t data;
        memset(&data, 0, sizeof(data));

        uint8_t res = dispatch_read(OID_FAKE_READONLY, INSTANCE_ID_FAKE_READONLY_OBJECT, PID_READ_INT_INVALIDARG, &data);

        ASSERT_ARE_EQUAL(int, res, COAP_400_BAD_REQUEST);
        free_lwm2m_data_t(data);
    }

    // Tests_SRS_DMDISPATCHERS_18_018: [ dispatch_read returns COAP_405_METHOD_NOT_ALLOWED if the propertyId doesn't support the read operation ]
    TEST_FUNCTION(dispatch_read_returns_correct_error_for_invalid_operation)
    {
        object_fake_writeexec *obj = create_and_register_fake_writeexec_object();
        lwm2m_data_t data;
        memset(&data, 0, sizeof(data));

        uint8_t res = dispatch_read(OID_FAKE_WRITEEXEC, INSTANCE_ID_FAKE_WRITEEXEC_OBJECT, PID_WRITE_INTEGER, &data);

        ASSERT_ARE_EQUAL(int, res, COAP_405_METHOD_NOT_ALLOWED);
        free_lwm2m_data_t(data);
    }

    // Tests_SRS_DMDISPATCHERS_18_043: [ dispatch_read will correctly return a string property in the correct LWM2M format ]
    TEST_FUNCTION(dispatch_read_returns_string_property_correctly)
    {
        object_fake_readonly *obj = create_and_register_fake_readonly_object();
        lwm2m_data_t data;
        memset(&data, 0, sizeof(data));

        uint8_t res = dispatch_read(OID_FAKE_READONLY, INSTANCE_ID_FAKE_READONLY_OBJECT, PID_READ_VALID_STRING, &data);

        ASSERT_ARE_EQUAL(int, data.type, LWM2M_TYPE_STRING);
        ASSERT_ARE_EQUAL(int, strlen(VALID_STRING), data.value.asBuffer.length);
        ASSERT_IS_TRUE(strcmp((char*)data.value.asBuffer.buffer, VALID_STRING) == 0);

        free_lwm2m_data_t(data);
    }

    // Tests_SRS_DMDISPATCHERS_18_044: [ dispatch_read will correctly return an opaque property in the correct LWM2M format ]
    TEST_FUNCTION(dispatch_read_returns_opaque_property_correctly)
    {
        object_fake_readonly *obj = create_and_register_fake_readonly_object();
        lwm2m_data_t data;
        memset(&data, 0, sizeof(data));

        uint8_t res = dispatch_read(OID_FAKE_READONLY, INSTANCE_ID_FAKE_READONLY_OBJECT, PID_READ_VALID_OPAQUE, &data);

        ASSERT_ARE_EQUAL(int, data.type, LWM2M_TYPE_OPAQUE);
        ASSERT_ARE_EQUAL(int, strlen(VALID_STRING)+1, data.value.asBuffer.length);
        ASSERT_IS_TRUE(strcmp((char*)data.value.asBuffer.buffer, VALID_STRING) == 0);

        free_lwm2m_data_t(data);
    }

    // Tests_SRS_DMDISPATCHERS_18_045: [ dispatch_read will correcty convert an integer property into the correct LWM2M format ]
    TEST_FUNCTION(dispatch_read_returns_integer_property_correctly)
    {
        object_fake_readonly *obj = create_and_register_fake_readonly_object();
        lwm2m_data_t data;
        memset(&data, 0, sizeof(data));

        uint8_t res = dispatch_read(OID_FAKE_READONLY, INSTANCE_ID_FAKE_READONLY_OBJECT, PID_READ_VALID_INTEGER, &data);

        ASSERT_ARE_EQUAL(int, data.type, LWM2M_TYPE_INTEGER);
        ASSERT_ARE_EQUAL(int64_t, data.value.asInteger, VALID_INT);

        free_lwm2m_data_t(data);
    }

    // Tests_SRS_DMDISPATCHERS_18_046: [ dispatch_read will correctly convert a float property into the correct LWM2M format ]
    TEST_FUNCTION(dispatch_read_returns_float_property_correctly)
    {
        object_fake_readonly *obj = create_and_register_fake_readonly_object();
        lwm2m_data_t data;
        memset(&data, 0, sizeof(data));

        uint8_t res = dispatch_read(OID_FAKE_READONLY, INSTANCE_ID_FAKE_READONLY_OBJECT, PID_READ_VALID_FLOAT, &data);

        ASSERT_ARE_EQUAL(int, data.type, LWM2M_TYPE_FLOAT);
        ASSERT_ARE_EQUAL(double, data.value.asFloat, VALID_FLOAT);

        free_lwm2m_data_t(data);
    }

    // Tests_SRS_DMDISPATCHERS_18_047: [ dispatch_read will correctly convert a time property into the correct LWM2M format ]
    TEST_FUNCTION(dispatch_read_returns_time_property_correctly)
    {
        object_fake_readonly *obj = create_and_register_fake_readonly_object();
        lwm2m_data_t data;
        memset(&data, 0, sizeof(data));

        uint8_t res = dispatch_read(OID_FAKE_READONLY, INSTANCE_ID_FAKE_READONLY_OBJECT, PID_READ_VALID_TIME, &data);

        ASSERT_ARE_EQUAL(int, data.type, LWM2M_TYPE_INTEGER);
        ASSERT_ARE_EQUAL(int64_t, data.value.asInteger, VALID_TIME);

        free_lwm2m_data_t(data);
    }

    // Tests_SRS_DMDISPATCHERS_18_048: [ dispatch_read will correctly convert a bool property into the correct LWM2M format ]
    TEST_FUNCTION(dispatch_read_returns_bool_property_correctly)
    {
        object_fake_readonly *obj = create_and_register_fake_readonly_object();
        lwm2m_data_t data;
        memset(&data, 0, sizeof(data));

        uint8_t res = dispatch_read(OID_FAKE_READONLY, INSTANCE_ID_FAKE_READONLY_OBJECT, PID_READ_VALID_BOOL, &data);

        ASSERT_ARE_EQUAL(int, data.type, LWM2M_TYPE_BOOLEAN);
        ASSERT_ARE_EQUAL(int, data.value.asBoolean, 1);

        free_lwm2m_data_t(data);
    }


    // Tests_SRS_DMDISPATCHERS_18_019: [ dispatch_exec calls the correct callback for the given objectId and propertyId ]
    TEST_FUNCTION(dispatch_exec_calls_correct_callback)
    {
        object_fake_writeexec *obj = create_and_register_fake_writeexec_object();

        (void)dispatch_exec(OID_FAKE_WRITEEXEC, INSTANCE_ID_FAKE_WRITEEXEC_OBJECT, PID_EXEC);
        ASSERT_IS_TRUE(obj->exec_called);
    }

    // Tests_SRS_DMDISPATCHERS_18_022: [ dispatch_exec returns COAP_204_CHANGED if the callback returns IOTHUB_CLIENT_OK ]
    TEST_FUNCTION(dispatch_exec_returns_correct_result_on_callback_success)
    {
        object_fake_writeexec *obj = create_and_register_fake_writeexec_object();

        uint8_t res = dispatch_exec(OID_FAKE_WRITEEXEC, INSTANCE_ID_FAKE_WRITEEXEC_OBJECT, PID_EXEC);
        ASSERT_ARE_EQUAL(int, res, COAP_204_CHANGED);
    }

    // Tests_SRS_DMDISPATCHERS_18_020: [ dispatch_exec returns COAP_404_NOT_FOUND if the given objectId is not registered ]
    TEST_FUNCTION(dispatch_exec_returns_correct_error_for_unregistered_object)
    {
        object_fake_writeexec *obj = create_and_register_fake_writeexec_object();

        uint8_t res = dispatch_exec(OID_UNREGISTERED, INSTANCE_ID_FAKE_WRITEEXEC_OBJECT, PID_EXEC);
        ASSERT_ARE_EQUAL(int, res, COAP_404_NOT_FOUND);
    }

    // Tests_SRS_DMDISPATCHERS_18_021: [ dispatch_exec returns COAP_404_NOT_FOUND if the given objectId does not contain the given propertyId ]
    TEST_FUNCTION(dispatch_exec_returns_correct_error_for_nonexistant_propertyId)
    {
        object_fake_writeexec *obj = create_and_register_fake_writeexec_object();

        uint8_t res = dispatch_exec(OID_FAKE_WRITEEXEC, INSTANCE_ID_FAKE_WRITEEXEC_OBJECT, PID_READ_VALID_INTEGER);
        ASSERT_ARE_EQUAL(int, res, COAP_404_NOT_FOUND);
    }

    // Tests_SRS_DMDISPATCHERS_18_023: [ dispatch_exec returns COAP_501_NOT_IMPLEMENTED if the callback returns IOTHUB_CLIENT_NOT_IMPLEMENTED ]
    TEST_FUNCTION(dispatch_exec_returns_correct_error_for_unimplemented_callback)
    {
        object_fake_writeexec *obj = create_and_register_fake_writeexec_object();

        uint8_t res = dispatch_exec(OID_FAKE_WRITEEXEC, INSTANCE_ID_FAKE_WRITEEXEC_OBJECT, PID_EXEC_NOTIMPL);
        ASSERT_ARE_EQUAL(int, res, COAP_501_NOT_IMPLEMENTED);
    }

    // Tests_SRS_DMDISPATCHERS_18_024: [ dispatch_exec returns COAP_400_BAD_REQUEST if the callback returns IOTHUB_CLIENT_INVLAID_ARG ]
    TEST_FUNCTION(dispatch_exec_returns_correct_error_for_invalid_argument)
    {
        object_fake_writeexec *obj = create_and_register_fake_writeexec_object();

        uint8_t res = dispatch_exec(OID_FAKE_WRITEEXEC, INSTANCE_ID_FAKE_WRITEEXEC_OBJECT, PID_EXEC_INVALIDARG);
        ASSERT_ARE_EQUAL(int, res, COAP_400_BAD_REQUEST);
    }

    // Tests_SRS_DMDISPATCHERS_18_025: [ dispatch_exec returns COAP_405_METHOD_NOT_ALLOWED if the propertyId doesn't support the execute operation ]
    TEST_FUNCTION(dispatch_exec_returns_correct_error_for_invalid_operation)
    {
        object_fake_writeexec *obj = create_and_register_fake_writeexec_object();

        uint8_t res = dispatch_exec(OID_FAKE_WRITEEXEC, INSTANCE_ID_FAKE_WRITEEXEC_OBJECT, PID_WRITE_INTEGER);
        ASSERT_ARE_EQUAL(int, res, COAP_405_METHOD_NOT_ALLOWED);
    }

    void populate_lwm2m_integer(lwm2m_data_t *data, int64_t value)
    {
        data->type = LWM2M_TYPE_INTEGER;
        data->value.asInteger = value;
    }

    void populate_lwm2m_bool(lwm2m_data_t *data, bool value)
    {
        data->type = LWM2M_TYPE_BOOLEAN;
        data->value.asBoolean = value;
    }

    void populate_lwm2m_time(lwm2m_data_t *data, int64_t value)
    {
        data->type = LWM2M_TYPE_INTEGER;
        populate_lwm2m_integer(data, value);
    }


    void populate_lwm2m_float(lwm2m_data_t *data, float value)
    {
        data->type = LWM2M_TYPE_FLOAT;
        data->value.asFloat = value;
    }

    void populate_lwm2m_string(lwm2m_data_t *data, const char *value)
    {
        data->type = LWM2M_TYPE_STRING;
        data->value.asBuffer.length = strlen(value);
        data->value.asBuffer.buffer = (uint8_t*)lwm2m_malloc(data->value.asBuffer.length + 1);
        strcpy((char*)data->value.asBuffer.buffer, value);
    }

    // Tests_SRS_DMDISPATCHERS_18_026: [ dispatch_write calls the correct callback for the given objectId and propertyId ]
    TEST_FUNCTION(dispatch_write_calls_correct_callback)
    {
        lwm2m_data_t data;
        memset(&data, 0, sizeof(data));
        populate_lwm2m_integer(&data, VALID_INT);

        object_fake_writeexec *obj = create_and_register_fake_writeexec_object();
        (void)dispatch_write(OID_FAKE_WRITEEXEC, INSTANCE_ID_FAKE_WRITEEXEC_OBJECT, PID_WRITE_INTEGER, &data);

        ASSERT_IS_TRUE(obj->write_integer_called);

        free_lwm2m_data_t(data);
    }

    // Tests_SRS_DMDISPATCHERS_18_029: [ dispatch_write returns COAP_204_CHANGED if the callback returns IOTHUB_CLIENT_OK ]
    TEST_FUNCTION(dispatch_write_returns_correct_result_on_callback_success)
    {
        lwm2m_data_t data;
        memset(&data, 0, sizeof(data));
        populate_lwm2m_integer(&data, VALID_INT);

        object_fake_writeexec *obj = create_and_register_fake_writeexec_object();
        uint8_t res = dispatch_write(OID_FAKE_WRITEEXEC, INSTANCE_ID_FAKE_WRITEEXEC_OBJECT, PID_WRITE_INTEGER, &data);

        ASSERT_ARE_EQUAL(int, res, COAP_204_CHANGED);

        free_lwm2m_data_t(data);
    }

    // Tests_SRS_DMDISPATCHERS_18_027: [ dispatch_write returns COAP_404_NOT_FOUND if the given objectId is not registered ]
    TEST_FUNCTION(dispatch_write_returns_correct_error_for_unregistered_object)
    {
        lwm2m_data_t data;
        memset(&data, 0, sizeof(data));
        populate_lwm2m_integer(&data, VALID_INT);

        object_fake_writeexec *obj = create_and_register_fake_writeexec_object();
        uint8_t res = dispatch_write(OID_UNREGISTERED, INSTANCE_ID_FAKE_WRITEEXEC_OBJECT, PID_WRITE_INTEGER, &data);

        ASSERT_ARE_EQUAL(int, res, COAP_404_NOT_FOUND);

        free_lwm2m_data_t(data);
    }

    // Tests_SRS_DMDISPATCHERS_18_028: [ dispatch_write returns COAP_404_NOT_FOUND if the given objectId does not contain the given propertyId ]
    TEST_FUNCTION(dispatch_write_returns_correct_error_for_nonexistant_property)
    {
        lwm2m_data_t data;
        memset(&data, 0, sizeof(data));
        populate_lwm2m_integer(&data, VALID_INT);

        object_fake_writeexec *obj = create_and_register_fake_writeexec_object();
        uint8_t res = dispatch_write(OID_UNREGISTERED, INSTANCE_ID_FAKE_WRITEEXEC_OBJECT, PID_READ_VALID_INTEGER, &data);

        ASSERT_ARE_EQUAL(int, res, COAP_404_NOT_FOUND);

        free_lwm2m_data_t(data);
    }

    // Tests_SRS_DMDISPATCHERS_18_030: [ dispatch_write returns COAP_501_NOT_IMPLEMENTED if the callback returns IOTHUB_CLIENT_NOT_IMPLEMENTED ]
    TEST_FUNCTION(dispatch_write_returns_correct_error_for_unimplemented_callback)
    {
        lwm2m_data_t data;
        memset(&data, 0, sizeof(data));
        populate_lwm2m_integer(&data, VALID_INT);

        object_fake_writeexec *obj = create_and_register_fake_writeexec_object();
        uint8_t res = dispatch_write(OID_FAKE_WRITEEXEC, INSTANCE_ID_FAKE_WRITEEXEC_OBJECT, PID_WRITE_NOTIMPL, &data);

        ASSERT_ARE_EQUAL(int, res, COAP_501_NOT_IMPLEMENTED);

        free_lwm2m_data_t(data);
    }

    // Tests_SRS_DMDISPATCHERS_18_031: [ dispatch_write returns COAP_400_BAD_REQUEST if the callback returns IOTHUB_CLIENT_INVLAID_ARG ]
    TEST_FUNCTION(dispatch_write_returns_correct_error_for_invalid_argument)
    {
        lwm2m_data_t data;
        memset(&data, 0, sizeof(data));
        populate_lwm2m_integer(&data, VALID_INT);

        object_fake_writeexec *obj = create_and_register_fake_writeexec_object();
        uint8_t res = dispatch_write(OID_FAKE_WRITEEXEC, INSTANCE_ID_FAKE_WRITEEXEC_OBJECT, PID_WRITE_INVALIDARG, &data);

        ASSERT_ARE_EQUAL(int, res, COAP_400_BAD_REQUEST);

        free_lwm2m_data_t(data);
    }

    // Tests_SRS_DMDISPATCHERS_18_032: [ dispatch_write returns COAP_405_METHOD_NOT_ALLOWED if the propertyId doesn't support the write operation ]
    TEST_FUNCTION(dispatch_write_returns_correct_error_for_invalid_operation)
    {
        lwm2m_data_t data;
        memset(&data, 0, sizeof(data));
        populate_lwm2m_integer(&data, VALID_INT);

        object_fake_writeexec *obj = create_and_register_fake_writeexec_object();
        uint8_t res = dispatch_write(OID_FAKE_WRITEEXEC, INSTANCE_ID_FAKE_WRITEEXEC_OBJECT, PID_EXEC, &data);

        ASSERT_ARE_EQUAL(int, res, COAP_405_METHOD_NOT_ALLOWED);

        free_lwm2m_data_t(data);
    }

    // Tests_SRS_DMDISPATCHERS_18_033: [ dispatch_write returns COAP_400_BAD_REQUEST for an integer property that fails to parse ]
    TEST_FUNCTION(dispatch_write_returns_correct_error_for_unparsable_integer)
    {
        lwm2m_data_t data;
        memset(&data, 0, sizeof(data));
        populate_lwm2m_string(&data, UNPARSABLE_STRING);

        object_fake_writeexec *obj = create_and_register_fake_writeexec_object();
        uint8_t res = dispatch_write(OID_FAKE_WRITEEXEC, INSTANCE_ID_FAKE_WRITEEXEC_OBJECT, PID_WRITE_INTEGER, &data);

        ASSERT_ARE_EQUAL(int, res, COAP_400_BAD_REQUEST);

        free_lwm2m_data_t(data);
    }

    // Tests_SRS_DMDISPATCHERS_18_034: [ dispatch_write returns COAP_400_BAD_REQUEST for a time property that fails to parse ]
    TEST_FUNCTION(dispatch_write_returns_correct_error_for_unparsable_time)
    {
        lwm2m_data_t data;
        memset(&data, 0, sizeof(data));
        populate_lwm2m_string(&data, UNPARSABLE_STRING);

        object_fake_writeexec *obj = create_and_register_fake_writeexec_object();
        uint8_t res = dispatch_write(OID_FAKE_WRITEEXEC, INSTANCE_ID_FAKE_WRITEEXEC_OBJECT, PID_WRITE_TIME, &data);

        ASSERT_ARE_EQUAL(int, res, COAP_400_BAD_REQUEST);

        free_lwm2m_data_t(data);
    }

    // Tests_SRS_DMDISPATCHERS_18_035: [ dispatch_write returns COAP_400_BAD_REQUEST for a float property that fails to parse ]
    TEST_FUNCTION(dispatch_write_returns_correct_error_for_unparsable_float)
    {
        lwm2m_data_t data;
        memset(&data, 0, sizeof(data));
        populate_lwm2m_string(&data, UNPARSABLE_STRING);

        object_fake_writeexec *obj = create_and_register_fake_writeexec_object();
        uint8_t res = dispatch_write(OID_FAKE_WRITEEXEC, INSTANCE_ID_FAKE_WRITEEXEC_OBJECT, PID_WRITE_FLOAT, &data);

        ASSERT_ARE_EQUAL(int, res, COAP_400_BAD_REQUEST);

        free_lwm2m_data_t(data);
    }

    // Tests_SRS_DMDISPATCHERS_18_036: [ dispatch_write returns COAP_400_BAD_REQUEST for a bool property that fails to parse ]
    TEST_FUNCTION(dispatch_write_returns_correct_error_for_unparsable_bool)
    {
        lwm2m_data_t data;
        memset(&data, 0, sizeof(data));
        populate_lwm2m_string(&data, UNPARSABLE_STRING);

        object_fake_writeexec *obj = create_and_register_fake_writeexec_object();
        uint8_t res = dispatch_write(OID_FAKE_WRITEEXEC, INSTANCE_ID_FAKE_WRITEEXEC_OBJECT, PID_WRITE_BOOL, &data);

        ASSERT_ARE_EQUAL(int, res, COAP_400_BAD_REQUEST);

        free_lwm2m_data_t(data);
    }

    // Tests_SRS_DMDISPATCHERS_18_037: [ dispatch_write correctly passes a string argument into the callback ]
    TEST_FUNCTION(dispatch_write_passes_string_property_into_callback)
    {
        lwm2m_data_t data;
        memset(&data, 0, sizeof(data));
        populate_lwm2m_string(&data, UNPARSABLE_STRING);

        object_fake_writeexec *obj = create_and_register_fake_writeexec_object();
        uint8_t res = dispatch_write(OID_FAKE_WRITEEXEC, INSTANCE_ID_FAKE_WRITEEXEC_OBJECT, PID_WRITE_STRING, &data);

        ASSERT_IS_TRUE(strcmp(obj->string_property, UNPARSABLE_STRING) == 0);

        free_lwm2m_data_t(data);
    }

    // Tests_SRS_DMDISPATCHERS_18_038: [ dispatch_write correctly passes a oppaque argument into the callback ]
    TEST_FUNCTION(dispatch_write_passes_opaque_property_into_callback)
    {
        lwm2m_data_t data;
        memset(&data, 0, sizeof(data));
        populate_lwm2m_string(&data, "opaque");

        object_fake_writeexec *obj = create_and_register_fake_writeexec_object();
        (void)dispatch_write(OID_FAKE_WRITEEXEC, INSTANCE_ID_FAKE_WRITEEXEC_OBJECT, PID_WRITE_OPAQUE, &data);

        ASSERT_ARE_EQUAL(int, obj->opaque_length, data.value.asBuffer.length);
        for (size_t i = 0; i < data.value.asBuffer.length; i++)
        {
            ASSERT_ARE_EQUAL(int, obj->opaque_property[i], data.value.asBuffer.buffer[i]);
        }

        free_lwm2m_data_t(data);
    }

    // Tests_SRS_DMDISPATCHERS_18_039: [ dispatch_write correctly parses an well-formed integer argument and passes it into the callback ]
    TEST_FUNCTION(dispatch_write_passes_integer_property_into_callback)
    {
        lwm2m_data_t data;
        memset(&data, 0, sizeof(data));
        populate_lwm2m_integer(&data, VALID_INT);

        object_fake_writeexec *obj = create_and_register_fake_writeexec_object();
        (void)dispatch_write(OID_FAKE_WRITEEXEC, INSTANCE_ID_FAKE_WRITEEXEC_OBJECT, PID_WRITE_INTEGER, &data);

        ASSERT_ARE_EQUAL(int64_t, obj->integer_property, VALID_INT);

        free_lwm2m_data_t(data);
    }

    // Tests_SRS_DMDISPATCHERS_18_040: [ dispatch_write correctly parses an well-formed time argument and passes it into the callback ]
    TEST_FUNCTION(dispatch_write_passes_time_property_into_callback)
    {
        lwm2m_data_t data;
        memset(&data, 0, sizeof(data));
        populate_lwm2m_time(&data, VALID_TIME);

        object_fake_writeexec *obj = create_and_register_fake_writeexec_object();
        (void)dispatch_write(OID_FAKE_WRITEEXEC, INSTANCE_ID_FAKE_WRITEEXEC_OBJECT, PID_WRITE_TIME, &data);

        ASSERT_ARE_EQUAL(int64_t, obj->time_property, VALID_TIME);

        free_lwm2m_data_t(data);
    }

    // Tests_SRS_DMDISPATCHERS_18_041: [ dispatch_write correctly parses an well-formed float argument and passes it into the callback ]
    TEST_FUNCTION(dispatch_write_passes_float_property_into_callback)
    {
        lwm2m_data_t data;
        memset(&data, 0, sizeof(data));
        populate_lwm2m_float(&data, (float)VALID_FLOAT);

        object_fake_writeexec *obj = create_and_register_fake_writeexec_object();
        (void)dispatch_write(OID_FAKE_WRITEEXEC, INSTANCE_ID_FAKE_WRITEEXEC_OBJECT, PID_WRITE_FLOAT, &data);

        ASSERT_ARE_EQUAL(int, ((int)(obj->float_property * 1000)), ((int)(VALID_FLOAT*1000)));

        free_lwm2m_data_t(data);
    }

    // Tests_SRS_DMDISPATCHERS_18_042: [ dispatch_write correctly parses an well-formed bool argument and passes it into the callback ]
    TEST_FUNCTION(dispatch_write_passes_bool_property_into_callback)
    {
        lwm2m_data_t data;
        memset(&data, 0, sizeof(data));
        populate_lwm2m_bool(&data, true);

        object_fake_writeexec *obj = create_and_register_fake_writeexec_object();
        (void)dispatch_write(OID_FAKE_WRITEEXEC, INSTANCE_ID_FAKE_WRITEEXEC_OBJECT, PID_WRITE_BOOL, &data);

        ASSERT_IS_TRUE(obj->bool_property);

        free_lwm2m_data_t(data);
    }


END_TEST_SUITE(iotdm_dispatchers_unittests)

