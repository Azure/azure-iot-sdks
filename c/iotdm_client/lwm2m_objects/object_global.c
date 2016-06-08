// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

/*
 *  Resources:
 *
 *          Name         | ID | Operations | Instances | Mandatory |  Type   |  Range  | Units |
 *
 */
#include "iotdm_internal.h"
#include "iotdm_dispatchers.h"

typedef struct _OBJECT_instance_
{
    struct _OBJECT_instance_ *next;         // matches lwm2m_list_t::next
    uint16_t                  instanceId;   // matches lwm2m_list_t::id
} OBJECT_instance_t;


static uint8_t prv_get_value(lwm2m_data_t *tlvP, uint16_t objectID, uint16_t instanceID)
{
    return dispatch_read(objectID, instanceID, tlvP->id, tlvP);
}

static uint8_t prv_set_value(lwm2m_data_t *tlvP, uint16_t objectID, uint16_t instanceID)
{
    return dispatch_write(objectID, instanceID, tlvP->id, tlvP);
}

uint8_t global_object_read(uint16_t instanceId, int *numDataP, lwm2m_data_t **dataArrayP, lwm2m_object_t *objectP)
{
    uint8_t result = COAP_404_NOT_FOUND;
    uint16_t index;

    // is the server asking for the full instance ?
    if (*numDataP == 0)
    {
        uint16_t nbRes = get_property_count(objectP->objID, OP_R);
        if (nbRes == PROP_ERROR)
        {
            return COAP_400_BAD_REQUEST;
        }
            
        *dataArrayP = lwm2m_data_new(nbRes);
        if (*dataArrayP == NULL)
        {
            return COAP_500_INTERNAL_SERVER_ERROR;
        }

        *numDataP = nbRes;
        for (index = 0 ; index < nbRes ; ++index)
        {
            uint16_t id = get_property_for_index(objectP->objID, OP_R, index);
            if (id == PROP_ERROR)
            {
                lwm2m_free(*dataArrayP);
                *dataArrayP = NULL;
                return COAP_500_INTERNAL_SERVER_ERROR;
            }
            else
            {
                (*dataArrayP)[index].id = id;
            }
        }
    }

    index = 0;
    do
    {
        result = prv_get_value((*dataArrayP) + index, objectP->objID, instanceId);
        index++;
    } while (index < *numDataP && result == COAP_205_CONTENT);

    fflush(stdout);
    fflush(stderr);

    return result;
}


/*
 */
void on_resource_value_changed(IOTHUB_CHANNEL_HANDLE iotHubChannel, uint16_t objectID, uint16_t instanceID, uint16_t resourceID)
{
    if (NULL == iotHubChannel)
    {
        LogError("NULL Channel Handle.");
    }

    else
    {
        lwm2m_uri_t uri = {
            LWM2M_URI_FLAG_OBJECT_ID | LWM2M_URI_FLAG_INSTANCE_ID | LWM2M_URI_FLAG_RESOURCE_ID,
            objectID,
            instanceID,
            resourceID
        };

        CLIENT_DATA *cd = (CLIENT_DATA *) iotHubChannel;
        lwm2m_resource_value_changed(cd->session, &uri);
    }
}


uint8_t global_object_write(uint16_t instanceID, int numData, lwm2m_data_t *dataArray, lwm2m_object_t *objectP)
{
    uint8_t  result = COAP_404_NOT_FOUND;
    int index = 0;
    do
    {
        uint16_t  resourceID = dataArray[index].id;

        result = prv_set_value(&dataArray[index], objectP->objID, instanceID);
        if (result == COAP_204_CHANGED)
        {
            on_resource_value_changed((IOTHUB_CHANNEL_HANDLE) (objectP->userData), objectP->objID, instanceID, resourceID);
        }

        index++;

    } while (index < numData && result == COAP_204_CHANGED);

    fflush(stdout);
    fflush(stderr);

    return result;
}


uint8_t global_object_execute(uint16_t instanceID, uint16_t resourceID, uint8_t *buffer, int length, lwm2m_object_t *objectP)
{
    uint8_t rv = dispatch_exec(objectP->objID, instanceID, resourceID);

    fflush(stdout);
    fflush(stderr);

    return rv;
}


static uint8_t prv_OBJECT_delete(uint16_t id, lwm2m_object_t *objectP)
{
    return COAP_405_METHOD_NOT_ALLOWED;
}


static uint8_t prv_OBJECT_create(uint16_t instanceID, int numData, lwm2m_data_t *dataArray, lwm2m_object_t *objectP)
{
    return COAP_405_METHOD_NOT_ALLOWED;
}


static void prv_OBJECT_close(lwm2m_object_t *object)
{
    while (object->instanceList != NULL)
    {
        OBJECT_instance_t *oneInstance = (OBJECT_instance_t *)object->instanceList;
        object->instanceList = object->instanceList->next;
        lwm2m_free(oneInstance);
    }
}


lwm2m_object_t *make_global_object(IOTHUB_CHANNEL_HANDLE iotHubChannel)
{
    lwm2m_object_t *oneObj = (lwm2m_object_t *) lwm2m_malloc(sizeof(lwm2m_object_t));
    if (NULL == oneObj)
    {
        LogError("Malloc failed");
    }

    else
    {
        memset(oneObj, 0, sizeof(lwm2m_object_t));

        OBJECT_instance_t *oneInstance = (OBJECT_instance_t *) lwm2m_malloc(sizeof(OBJECT_instance_t));
        if (NULL == oneInstance)
        {
            LogError("Malloc failed");

            lwm2m_free(oneObj);
            oneObj = NULL;
        }

        else
        {
            memset(oneInstance, 0, sizeof(OBJECT_instance_t));
            oneObj->instanceList = LWM2M_LIST_ADD(oneObj->instanceList, oneInstance);

            oneObj->objID = LWM2M_DEVICE_OBJECT_ID; /* fake out wakaama :-) */
            oneObj->readFunc = global_object_read;
            oneObj->writeFunc = global_object_write;
            oneObj->createFunc = prv_OBJECT_create;
            oneObj->deleteFunc = prv_OBJECT_delete;
            oneObj->executeFunc = global_object_execute;

            oneObj->userData = (void *) iotHubChannel;
        }
    }

    return oneObj;
}
