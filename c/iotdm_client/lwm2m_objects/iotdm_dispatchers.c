#include "iothub_client.h"
#include "iotdm_internal.h"
#include <iotdm_dispatchers.h>




// BKTODO: General comment for all log messages. I think we should use something like 'failure to perform [operation description]'. Avoid using 'ERROR: ' since the logger will always prefix the message appropriately.

IOTHUB_CLIENT_RESULT dispatch_string_read_to_callback(void *obj, void *function, lwm2m_data_t *tlvP)
{
    IOTHUB_CLIENT_RESULT res = IOTHUB_CLIENT_OK;
    char *data = NULL;

    res = ((IOTHUB_CLIENT_GET_STRINGPROP)function)(obj, &data);
    if (res == IOTHUB_CLIENT_OK)
    {
        // Codes_SRS_DMDISPATCHERS_18_043: [ dispatch_read will correctly return a string property in the correct LWM2M format ]
        tlvP->value.asBuffer.length = data ? strlen(data) : 0;
        tlvP->value.asBuffer.buffer = data;
        tlvP->type = LWM2M_TYPE_STRING;
    }
    
    return res;
}

IOTHUB_CLIENT_RESULT dispatch_integer_read_to_callback(void *obj, void *function, lwm2m_data_t *tlvP)
{
    IOTHUB_CLIENT_RESULT res = IOTHUB_CLIENT_OK;
    int64_t data = 0;
    res = ((IOTHUB_CLIENT_GET_INTEGERPROP)function)(obj, &data);
    if (res == IOTHUB_CLIENT_OK)
    {
        // Codes_SRS_DMDISPATCHERS_18_045: [ dispatch_read will correcty convert an integer property into the correct LWM2M format ]
        lwm2m_data_encode_int(data, tlvP);
    }
    return res;
}

IOTHUB_CLIENT_RESULT dispatch_time_read_to_callback(void *obj, void *function, lwm2m_data_t *tlvP)
{
    IOTHUB_CLIENT_RESULT res = IOTHUB_CLIENT_OK;
    int64_t data;
    res = ((IOTHUB_CLIENT_GET_TIMEPROP)function)(obj, &data);
    if (res == IOTHUB_CLIENT_OK)
    {
        // Codes_SRS_DMDISPATCHERS_18_047: [ dispatch_read will correctly convert a time property into the correct LWM2M format ]
        lwm2m_data_encode_int(data, tlvP);
        tlvP->type = LWM2M_TYPE_INTEGER;
    }
    return res;
}

IOTHUB_CLIENT_RESULT dispatch_opaque_read_to_callback(void *obj, void *function, lwm2m_data_t *tlvP)
{
    IOTHUB_CLIENT_RESULT res = IOTHUB_CLIENT_OK;
    void *data = NULL;
    size_t length = 0;
    res = ((IOTHUB_CLIENT_GET_OPAQUEPROP)function)(obj, &data, &length);
    if (res == IOTHUB_CLIENT_OK)
    {
        tlvP->value.asBuffer.length = length;
        tlvP->value.asBuffer.buffer = data;
        tlvP->type = LWM2M_TYPE_OPAQUE;
    }
    return res;
}

IOTHUB_CLIENT_RESULT dispatch_float_read_to_callback(void *obj, void *function, lwm2m_data_t *tlvP)
{
    IOTHUB_CLIENT_RESULT res = IOTHUB_CLIENT_OK;
    double data;
    res = ((IOTHUB_CLIENT_GET_FLOATPROP)function)(obj, &data);
    if (res == IOTHUB_CLIENT_OK)
    {
        // Codes_SRS_DMDISPATCHERS_18_046: [ dispatch_read will correctly convert a float property into the correct LWM2M format ]
        lwm2m_data_encode_float(data, tlvP);
    }
    return res;
}

IOTHUB_CLIENT_RESULT dispatch_bool_read_to_callback(void *obj, void *function, lwm2m_data_t *tlvP)
{
    IOTHUB_CLIENT_RESULT res = IOTHUB_CLIENT_OK;
    bool data;
    res = ((IOTHUB_CLIENT_GET_BOOLPROP)function)(obj, &data);
    if (res == IOTHUB_CLIENT_OK)
    {
        // Codes_SRS_DMDISPATCHERS_18_048: [ dispatch_read will correctly convert a bool property into the correct LWM2M format ]
        lwm2m_data_encode_bool(data, tlvP);
    }

    return res;
}

IOTHUB_CLIENT_RESULT dispatch_exec_to_callback(void *obj, void *function)
{
    return ((IOTHUB_CLIENT_HANDLE_EXECUTE)function)(obj);
}

IOTHUB_CLIENT_RESULT dispatch_string_write_to_callback(void *obj, void *function, lwm2m_data_t *tlvP)
{
    IOTHUB_CLIENT_RESULT res = IOTHUB_CLIENT_OK;

    //Codes_SRS_DMDISPATCHERS_18_037: [ dispatch_write correctly passes a string argument into the callback ]
    return ((IOTHUB_CLIENT_SET_STRINGPROP)function)(obj, (char*)tlvP->value.asBuffer.buffer, tlvP->value.asBuffer.length);
}

IOTHUB_CLIENT_RESULT dispatch_integer_write_to_callback(void *obj, void *function, lwm2m_data_t *tlvP)
{
    IOTHUB_CLIENT_RESULT res = IOTHUB_CLIENT_OK;
    int64_t data;
    
    //Codes_SRS_DMDISPATCHERS_18_033: [ dispatch_write returns COAP_400_BAD_REQUEST for an integer property that fails to parse ]
    if (lwm2m_data_decode_int(tlvP, &data) == 0)
    {
        res = IOTHUB_CLIENT_INVALID_ARG;
    }
    else
    {
        //Codes_SRS_DMDISPATCHERS_18_039: [ dispatch_write correctly parses an well-formed integer argument and passes it into the callback ]
        // BKTODO: make callback accept an int64_t?
        // BKTODO: address overflow
        res = ((IOTHUB_CLIENT_SET_INTEGERPROP)function)(obj, (int32_t)data);
    }
    return res;
}

IOTHUB_CLIENT_RESULT dispatch_time_write_to_callback(void *obj, void *function, lwm2m_data_t *tlvP)
{
    IOTHUB_CLIENT_RESULT res = IOTHUB_CLIENT_OK;
    int64_t data;
    
    //Codes_SRS_DMDISPATCHERS_18_034: [ dispatch_write returns COAP_400_BAD_REQUEST for a time property that fails to parse ]
    if (lwm2m_data_decode_int(tlvP, &data) == 0)
    {
        res = IOTHUB_CLIENT_INVALID_ARG;
    }
    else
    {
        //Codes_SRS_DMDISPATCHERS_18_040: [ dispatch_write correctly parses an well-formed time argument and passes it into the callback ]
        res = ((IOTHUB_CLIENT_SET_TIMEPROP)function)(obj, (int32_t)data);
    }
    return res;
}

//Codes_SRS_DMDISPATCHERS_18_038: [ dispatch_write correctly passes a oppaque argument into the callback ]
IOTHUB_CLIENT_RESULT dispatch_opaque_write_to_callback(void *obj, void *function, lwm2m_data_t *tlvP)
{
    return ((IOTHUB_CLIENT_SET_OPAQUEPROP)function)(obj, tlvP->value.asBuffer.buffer, tlvP->value.asBuffer.length);
}

IOTHUB_CLIENT_RESULT dispatch_float_write_to_callback(void *obj, void *function, lwm2m_data_t *tlvP)
{
    IOTHUB_CLIENT_RESULT res = IOTHUB_CLIENT_OK;
    double data;
    
    //Codes_SRS_DMDISPATCHERS_18_035: [ dispatch_write returns COAP_400_BAD_REQUEST for a float property that fails to parse ]
    if (lwm2m_data_decode_float(tlvP, &data) == 0)
    {
        res = IOTHUB_CLIENT_INVALID_ARG;
    }
    else
    {
        //Codes_SRS_DMDISPATCHERS_18_041: [ dispatch_write correctly parses an well-formed float argument and passes it into the callback ]
        res = ((IOTHUB_CLIENT_SET_FLOATPROP)function)(obj, data);
    }
    return res;
}

IOTHUB_CLIENT_RESULT dispatch_bool_write_to_callback(void *obj, void *function, lwm2m_data_t *tlvP)
{
    IOTHUB_CLIENT_RESULT res = IOTHUB_CLIENT_OK;
    bool data;
    
    //Codes_SRS_DMDISPATCHERS_18_036: [ dispatch_write returns COAP_400_BAD_REQUEST for a bool property that fails to parse ]
    if (lwm2m_data_decode_bool(tlvP, &data) == 0)
    {
        res = IOTHUB_CLIENT_INVALID_ARG;
    }
    else
    {
        //Codes_SRS_DMDISPATCHERS_18_042: [ dispatch_write correctly parses an well-formed bool argument and passes it into the callback ]
        res = ((IOTHUB_CLIENT_SET_BOOLPROP)function)(obj, data);
    }
    return res;
}



