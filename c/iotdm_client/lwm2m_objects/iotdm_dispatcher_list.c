#include "iothub_client.h"
#include "iotdm_internal.h"
#include <iotdm_dispatchers.h>

static LIST_HANDLE dispatcherList = NULL;

IOTHUB_CLIENT_RESULT register_dm_dispatchers(
    uint16_t objectId,
    const DmOps *allowedOperations,
    LIST_HANDLE *instanceList,
    FUNC_DISPATCH_READ_TO_OBJ_INSTANCE readDispatcher,
    FUNC_DISPATCH_EXEC_TO_OBJ_INSTANCE execDispatcher,
    FUNC_DISPATCH_WRITE_TO_OBJ_INSTANCE writeDispatcher,
    FUNC_SIGNAL_RESOURCE_CHANGE changeSignaller)
 {
    IOTHUB_CLIENT_RESULT res = IOTHUB_CLIENT_OK;
    DmDispatchers *disp = NULL;

    res = get_dm_object(dispatcherList, objectId, (void**)&disp);
    if (res != IOTHUB_CLIENT_ERROR)
    {
        // Codes_SRS_DMDISPATCHERS_18_005: [ register_dm_dispatchers shall return IOTHUB_CLIENT_OK if registering a given objectId for a second time. ]
        res = IOTHUB_CLIENT_OK;
    }
    else
    {
        if (disp == NULL)
        {
            disp = (DmDispatchers*)lwm2m_malloc(sizeof(*disp));
            if (disp == NULL)
            {
                LogError("Alloc error");
                res = IOTHUB_CLIENT_ERROR;
            }
            else
            {
                disp->objectId = objectId;
                disp->allowedOperations = allowedOperations;
                disp->instanceList = instanceList;
                disp->readDispatcher = readDispatcher;
                disp->execDispatcher = execDispatcher;
                disp->writeDispatcher = writeDispatcher;
                disp->changeSignaller = changeSignaller;

                // Codes_SRS_DMDISPATCHERS_18_004: [ regiter_dm_dispatchers returns IOTHUB_CLIENT_OK on success ]
                res = add_dm_object(&dispatcherList, disp);
                if (res != IOTHUB_CLIENT_OK)
                {
                    LogError("add_dm_object error");
                    lwm2m_free(disp);
                }
            }

        }
    }

    return res;
}

uint8_t get_dispatcher_and_object_instance(uint16_t objectId, uint16_t instanceId, DmDispatchers **disp, void **obj)
{
    uint8_t res = COAP_500_INTERNAL_SERVER_ERROR;

    if ((get_dm_object(dispatcherList, objectId, (void**)disp) != IOTHUB_CLIENT_OK) ||
        (*disp == NULL))
    {
        //Codes_SRS_DMDISPATCHERS_18_013: [ dispatch_read returns COAP_404_NOT_FOUND if the given objectId is not registered ]
        //Codes_SRS_DMDISPATCHERS_18_020: [ dispatch_exec returns COAP_404_NOT_FOUND if the given objectId is not registered ]
        //Codes_SRS_DMDISPATCHERS_18_027: [ dispatch_write returns COAP_404_NOT_FOUND if the given objectId is not registered ]
        res = COAP_404_NOT_FOUND;
    }
    else
    {
        if ((get_dm_object(*(*disp)->instanceList, instanceId, obj) != IOTHUB_CLIENT_OK) ||
            (*obj == NULL))
        {
            //Codes_SRS_DMDISPATCHERS_18_013: [ dispatch_read returns COAP_404_NOT_FOUND if the given objectId is not registered ]
            //Codes_SRS_DMDISPATCHERS_18_020: [ dispatch_exec returns COAP_404_NOT_FOUND if the given objectId is not registered ]
            //Codes_SRS_DMDISPATCHERS_18_027: [ dispatch_write returns COAP_404_NOT_FOUND if the given objectId is not registered ]
            disp = NULL;
            res = COAP_404_NOT_FOUND;
        }
        else
        {
            res = COAP_NO_ERROR;
        }
    }

    return res;
}

uint8_t ClientResultToCoapResult(IOTHUB_CLIENT_RESULT res, uint8_t operation)
{
    switch (res)
    {
    //Codes_SRS_DMDISPATCHERS_18_015: [ dispatch_read returns COAP_205_CONTENT if the callback returns IOTHUB_CLIENT_OK ]
    //Codes_SRS_DMDISPATCHERS_18_022: [ dispatch_exec returns COAP_204_CHANGED if the callback returns IOTHUB_CLIENT_OK ]
    //Codes_SRS_DMDISPATCHERS_18_029: [ dispatch_write returns COAP_204_CHANGED if the callback returns IOTHUB_CLIENT_OK ]
    case IOTHUB_CLIENT_OK:
        return operation == OP_R ? COAP_205_CONTENT : COAP_204_CHANGED;
    //Codes_SRS_DMDISPATCHERS_18_016: [ dispatch_read returns COAP_501_NOT_IMPLEMENTED if the callback returns IOTHUB_CLIENT_NOT_IMPLEMENTED ]
    //Codes_SRS_DMDISPATCHERS_18_023: [ dispatch_exec returns COAP_501_NOT_IMPLEMENTED if the callback returns IOTHUB_CLIENT_NOT_IMPLEMENTED ]
    //Codes_SRS_DMDISPATCHERS_18_030: [ dispatch_write returns COAP_501_NOT_IMPLEMENTED if the callback returns IOTHUB_CLIENT_NOT_IMPLEMENTED ]
    case IOTHUB_CLIENT_NOT_IMPLEMENTED:
        return COAP_501_NOT_IMPLEMENTED;
    //Codes_SRS_DMDISPATCHERS_18_017: [ dispatch_read returns COAP_400_BAD_REQUEST if the callback returns IOTHUB_CLIENT_INVLAID_ARG ]
    //Codes_SRS_DMDISPATCHERS_18_024: [ dispatch_exec returns COAP_400_BAD_REQUEST if the callback returns IOTHUB_CLIENT_INVLAID_ARG ]
    //Codes_SRS_DMDISPATCHERS_18_031: [ dispatch_write returns COAP_400_BAD_REQUEST if the callback returns IOTHUB_CLIENT_INVLAID_ARG ]
    case IOTHUB_CLIENT_INVALID_ARG:
        return COAP_400_BAD_REQUEST;
    default:
        return COAP_500_INTERNAL_SERVER_ERROR;
    }
}

uint8_t dispatch_read(uint16_t objectId, uint16_t instanceId, uint16_t propertyId, lwm2m_data_t *tlvP)
{
    uint8_t res = COAP_500_INTERNAL_SERVER_ERROR;
    DmDispatchers *disp = NULL;
    void *obj = NULL;

    res = get_dispatcher_and_object_instance(objectId, instanceId, &disp, &obj);
    if (res == COAP_NO_ERROR)
    {
        res = operation_is_allowed(disp, propertyId, OP_R);
        if (res == COAP_NO_ERROR)
        {
            //Codes_SRS_DMDISPATCHERS_18_012: [ dispatch_read calls the correct callback for the given objectId and propertyId ]
            res = ClientResultToCoapResult(disp->readDispatcher(obj, propertyId, tlvP), OP_R);
        }
    }
    return res;
}

uint8_t dispatch_exec(uint16_t objectId, uint16_t instanceId, uint16_t propertyId)
{
    uint8_t res = COAP_500_INTERNAL_SERVER_ERROR;
    DmDispatchers *disp = NULL;
    void *obj = NULL;

    res = get_dispatcher_and_object_instance(objectId, instanceId, &disp, &obj);
    if (res == COAP_NO_ERROR)
    {
        res = operation_is_allowed(disp, propertyId, OP_E);
        if (res == COAP_NO_ERROR)
        {
            //Codes_SRS_DMDISPATCHERS_18_019: [ dispatch_exec calls the correct callback for the given objectId and propertyId ]
            res = ClientResultToCoapResult(disp->execDispatcher(obj, propertyId), OP_E);
        }
    }
    return res;
}

uint8_t dispatch_write(uint16_t objectId, uint16_t instanceId, uint16_t propertyId, lwm2m_data_t *tlvP)
{
    uint8_t res = COAP_500_INTERNAL_SERVER_ERROR;
    DmDispatchers *disp = NULL;
    void *obj = NULL;

    res = get_dispatcher_and_object_instance(objectId, instanceId, &disp, &obj);
    if (res == COAP_NO_ERROR)
    {
        res = operation_is_allowed(disp, propertyId, OP_W);
        if (res == COAP_NO_ERROR)
        {
            //Codes_SRS_DMDISPATCHERS_18_026: [ dispatch_write calls the correct callback for the given objectId and propertyId ]
            res = ClientResultToCoapResult(disp->writeDispatcher(obj, propertyId, tlvP), OP_W);
        }
    }
    return res;
}

uint8_t operation_is_allowed(DmDispatchers *disp, uint16_t propertyId, uint8_t operation)
{
    //Codes_SRS_DMDISPATCHERS_18_014: [ dispatch_read returns COAP_404_NOT_FOUND if the given objectId does not contain the given propertyId ]
    //Codes_SRS_DMDISPATCHERS_18_021: [ dispatch_exec returns COAP_404_NOT_FOUND if the given objectId does not contain the given propertyId ]
    //Codes_SRS_DMDISPATCHERS_18_028: [ dispatch_write returns COAP_404_NOT_FOUND if the given objectId does not contain the given propertyId ]
    uint8_t result = COAP_404_NOT_FOUND;

    const DmOps *ops = disp->allowedOperations;
    while (ops->allowedOp != 0)
    {
        if (ops->propertyId == propertyId)
        {
            if ((ops->allowedOp & operation) == operation)
            {
                result = COAP_NO_ERROR;
            }
            else
            {
                //Codes_SRS_DMDISPATCHERS_18_018: [ dispatch_read returns COAP_405_METHOD_NOT_ALLOWED if the propertyId doesn't support the read operation ]
                //Codes_SRS_DMDISPATCHERS_18_025: [ dispatch_exec returns COAP_405_METHOD_NOT_ALLOWED if the propertyId doesn't support the execute operation ]
                //Codes_SRS_DMDISPATCHERS_18_032: [ dispatch_write returns COAP_405_METHOD_NOT_ALLOWED if the propertyId doesn't support the write operation ]
                result = COAP_405_METHOD_NOT_ALLOWED;
            }
            break;
        }
        ops++;
    }

    return result;
}

uint16_t get_property_count(uint16_t objectId, uint8_t operation)
{
    DmDispatchers *disp = NULL;
    // Codes_SRS_DMDISPATCHERS_18_002 (if the given objectId doesn't have any properties that support the given operation, get_property_count shall return 0)
    uint16_t count = 0;
    
    // Codes_SRS_DMDISPATCHERS_18_003: (if the given objectId is not registered, get_property_count shall return PROP_ERROR)
    if ((get_dm_object(dispatcherList, objectId, (void**)&disp) != IOTHUB_CLIENT_OK) ||
        (disp == NULL))
    {

        count = PROP_ERROR;
    }
    else
    {
        const DmOps *ops = disp->allowedOperations;
        while (ops->allowedOp != 0)
        {
            // Codes_SRS_DMDISPATCHERS_18_001 (get_property_count shall return the number of properties registered for the given objectId and the given operation) 
            if ((ops->allowedOp & operation) == operation)
            {
                count++;
            }
            ops++;
        }
        
    }
    return count;
}


uint16_t get_property_for_index(uint16_t objectId, uint8_t operation, uint16_t index)
{
    DmDispatchers *disp = NULL;
// Codes_SRS_DMDISPATCHERS_18_007: [ if index is out of range, get_property_for_index shall return PROP_ERROR ]
    uint16_t res = PROP_ERROR;
    uint16_t count = 0;
    
    if ((get_dm_object(dispatcherList, objectId, (void**)&disp) != IOTHUB_CLIENT_OK) ||
        (disp == NULL))
    {
        // Codes_SRS_DMDISPATCHERS_18_008: [ if objectId is not registered, get_property_for_index shall return PROP_ERROR ]
        res = PROP_ERROR;
    }
    else
    {
        const DmOps *ops = disp->allowedOperations;
        while (ops->allowedOp != 0)
        {
            // Codes_SRS_DMDISPATCHERS_18_006: [ get_property_for_index shall return the correct propertyId for the given index and operation ]
            if ((ops->allowedOp & operation) == operation)
            {
                if (count == index)
                {
                    res = ops->propertyId;
                    break;
                }
                count++;
            }
            ops++;
        }
    }
    
    return res;
}


lwm2m_data_type_t get_type_for_property(uint16_t objectId, uint16_t propertyId)
{
    // Codes_SRS_DMDISPATCHERS_18_011: [ if the given objectId does not contain propertyId, get_type_for_property shall return LWM2M_TYPE_UNDERFINED ]
    lwm2m_data_type_t res = LWM2M_TYPE_UNDEFINED;    
    DmDispatchers *disp = NULL;
    
    if ((get_dm_object(dispatcherList, objectId, (void**)&disp) != IOTHUB_CLIENT_OK) ||
        (disp == NULL))
    {
        // Codes_SRS_DMDISPATCHERS_18_010: [ if the given objectId is not registered, get_type_for_property shall return LWM2M_TYPE_UNDEFINED ]
        res = LWM2M_TYPE_UNDEFINED;    
    }
    else
    {
        const DmOps *ops = disp->allowedOperations;
        while (ops->allowedOp != 0)
        {
            // Codes_SRS_DMDISPATCHERS_18_009: [ get_type_for_property shall return the correct type for the given objectId and propertyId ]
            if (ops->propertyId == propertyId)
            {
                res = ops->dataType;
                break;
            }
            ops++;
        }
    }
    
    return res;
}

// BKTODO: unit tests
void *get_object_instance(uint16_t objectId, uint16_t instanceId)
{
    DmDispatchers *disp = NULL;
    void* obj = NULL;

    uint8_t res = get_dispatcher_and_object_instance(objectId, instanceId, &disp, &obj);
    if (res == COAP_NO_ERROR)
    {
        // Codes_SRS_DMDISPATCHERS_18_051: [ get_object_instance returns the correct object for the given objectId and instanceId ]
        return obj;
    }
    else
    {
        // Codes_SRS_DMDISPATCHERS_18_049: [ get_object_instance returns NULL if the given objectId isn't registered ]
        // Codes_SRS_DMDISPATCHERS_18_050: [ get_object_instance returns NULL if the given instanceId doesn't exist ]
        return NULL;
    }
}

typedef bool(*for_each_dispatcher_predicate)(DmDispatchers* disp, void* context);

static bool for_each_dispatcher(for_each_dispatcher_predicate pred, void* context)
{
    if (!dispatcherList)
    {
        return false;
    }

    LIST_ITEM_HANDLE dispHandle = list_get_head_item(dispatcherList);
    while (dispHandle != NULL)
    {
        DmDispatchers* disp = (DmDispatchers*)list_item_get_value(dispHandle);

        if (!pred(disp, context))
        {
            break;
        }

        dispHandle = list_get_next_item(dispHandle);
    }

    return (dispHandle == NULL);
}

static bool signal_object_resource_changes(DmDispatchers* disp, void* context)
{
    if (disp->instanceList != NULL)
    {
        LIST_ITEM_HANDLE objHandle = list_get_head_item(*disp->instanceList);
        while (objHandle != NULL)
        {
            void* obj = (void*)list_item_get_value(objHandle);
            disp->changeSignaller(obj);
            objHandle = list_get_next_item(objHandle);
        }
    }

    return true;
}

//
// calls on_resource_value_changed for every resource changed on every instance of every object 
//
// setter functions (e.g. set_config_name) udpate obj->resouceUpdated as a dirty bit.  setters can
// be called from any thread.
//
// The main thread calls signal_all_resource_changes.  This looks at obj->resourceUpdated and
// uses those bits to call on_resource_value_changed, which could trigger a NOTIFY message 
// during the next observe_step() cycle.
//
void signal_all_resource_changes()
{
    (void)for_each_dispatcher(signal_object_resource_changes, NULL);
}

typedef struct oid_context
{
    for_each_oid_predicate pred;
    void* context;
} oid_context;

static bool call_outer_predicate_with_oid(DmDispatchers* disp, void* context)
{
    oid_context* outer = (oid_context*)context;
    return outer->pred(disp->objectId, outer->context);
}

bool for_each_oid(for_each_oid_predicate pred, void* context)
{
    oid_context outer = { pred, context };
    return for_each_dispatcher(call_outer_predicate_with_oid, &outer);
}
