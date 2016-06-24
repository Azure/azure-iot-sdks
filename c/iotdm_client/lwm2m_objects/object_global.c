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


static lwm2m_object_t * prv_findObject(lwm2m_context_t * contextP,
                                        uint16_t Id)
{
    return (lwm2m_object_t *)LWM2M_LIST_FIND(contextP->objectList, Id);
}

static OBJECT_instance_t * prv_findInstance(lwm2m_object_t * object, 
                                        uint16_t instanceID)
{
    return (OBJECT_instance_t *) LWM2M_LIST_FIND(object->instanceList, instanceID);
}


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
    uint8_t result;
	if (IotHubClient_DM_EnterCriticalSection((IOTHUB_CHANNEL_HANDLE)(objectP->userData)) == false)
	{
		LogError("enter critical section");
		result = COAP_500_INTERNAL_SERVER_ERROR;
	}

	else
	{
		uint16_t index;

		// is the server asking for the full instance ?
		if (*numDataP == 0)
		{
			uint16_t nbRes = get_property_count(objectP->objID, OP_R);
			if (nbRes == PROP_ERROR)
			{
				result = COAP_400_BAD_REQUEST;
			}

			else
			{
				*dataArrayP = lwm2m_data_new(nbRes);
				if (*dataArrayP == NULL)
				{
					result = COAP_500_INTERNAL_SERVER_ERROR;
				}

				else
				{
					uint16_t id = 0;

					*numDataP = nbRes;
					for (index = 0; (id != PROP_ERROR) && (index < nbRes); ++index)
					{
						id = get_property_for_index(objectP->objID, OP_R, index);
						if (id == PROP_ERROR)
						{
							lwm2m_free(*dataArrayP);
							*dataArrayP = NULL;
							result = COAP_500_INTERNAL_SERVER_ERROR;
						}

						else
						{
							(*dataArrayP)[index].id = id;
						}
					}
				}
			}
		}

		index = 0;
		do
		{
			result = prv_get_value((*dataArrayP) + index, objectP->objID, instanceId);
			index++;
		} while (index < *numDataP && result == COAP_205_CONTENT);

		if (IotHubClient_DM_LeaveCriticalSection((IOTHUB_CHANNEL_HANDLE)(objectP->userData)) == false)
		{
			LogError("leave critical section");
		}
	}

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
    uint8_t  result;
	if (IotHubClient_DM_EnterCriticalSection((IOTHUB_CHANNEL_HANDLE)(objectP->userData)) == false)
	{
		LogError("enter critical section");
		result = COAP_500_INTERNAL_SERVER_ERROR;
	}

	else
	{
		int index = 0;
		do
		{
			uint16_t  resourceID = dataArray[index].id;

			result = prv_set_value(&dataArray[index], objectP->objID, instanceID);
			if (result == COAP_204_CHANGED)
			{
				on_resource_value_changed((IOTHUB_CHANNEL_HANDLE)(objectP->userData), objectP->objID, instanceID, resourceID);
			}

			index++;

		} while (index < numData && result == COAP_204_CHANGED);

		if (IotHubClient_DM_LeaveCriticalSection((IOTHUB_CHANNEL_HANDLE)(objectP->userData)) == false)
		{
			LogError("leave critical section");
		}
	}

    fflush(stdout);
    fflush(stderr);

    return result;
}


uint8_t global_object_execute(uint16_t instanceID, uint16_t resourceID, uint8_t *buffer, int length, lwm2m_object_t *objectP)
{
	uint8_t rv;
	if (IotHubClient_DM_EnterCriticalSection((IOTHUB_CHANNEL_HANDLE)(objectP->userData)) == false)
	{
		LogError("enter critical section");
		rv = COAP_500_INTERNAL_SERVER_ERROR;
	}

	else
	{
		rv = dispatch_exec(objectP->objID, instanceID, resourceID);
		if (IotHubClient_DM_LeaveCriticalSection((IOTHUB_CHANNEL_HANDLE)(objectP->userData)) == false)
		{
			LogError("leave critical section");
		}
	}

    fflush(stdout);
    fflush(stderr);

    return rv;
}


static uint8_t prv_OBJECT_delete(uint16_t instanceID, lwm2m_object_t *objectP)
{
    return COAP_405_METHOD_NOT_ALLOWED;
}


static uint8_t prv_OBJECT_create(uint16_t instanceID, int numData, lwm2m_data_t *dataArray, lwm2m_object_t *objectP)
{
    return COAP_405_METHOD_NOT_ALLOWED;
}


static uint8_t prv_OBJECT_discover(uint16_t instanceID, int *numDataP, lwm2m_data_t **dataArrayP, lwm2m_object_t *objectP)
{
	LogError("DISCOVER OPERATION NOT ALLOWED!");
	return COAP_405_METHOD_NOT_ALLOWED;
}

// BKTODO: better name to describe new functionality
// BKTODO: this makes the object, maybe it should register it too.
lwm2m_object_t *make_global_object(IOTHUB_CHANNEL_HANDLE iotHubChannel, uint16_t objectID, uint16_t instanceID)
{
    lwm2m_object_t *obj;
    
    CLIENT_DATA *client = (CLIENT_DATA*)iotHubChannel;
    if (client == NULL)
    {
        LogError("NULL client");
        obj = NULL;
    }
    else
    {
        obj = prv_findObject(client->session, objectID);
        if (obj == NULL)
        {
            obj = (lwm2m_object_t *) lwm2m_malloc(sizeof(lwm2m_object_t));
            if (NULL == obj)
            {
                LogError("Malloc failed");
            }
            else
            {
                memset(obj, 0, sizeof(lwm2m_object_t));
                
                obj->objID = objectID;
                obj->readFunc = global_object_read;
                obj->writeFunc = global_object_write;
                obj->createFunc = prv_OBJECT_create;
                obj->deleteFunc = prv_OBJECT_delete;
                obj->executeFunc = global_object_execute;
                obj->discoverFunc = prv_OBJECT_discover;
                obj->userData = (void *) iotHubChannel;
            }

            if (obj !=NULL)
            {
                OBJECT_instance_t *inst = prv_findInstance(obj, instanceID);
                if (inst == NULL)
                {
                    inst = (OBJECT_instance_t *) lwm2m_malloc(sizeof(OBJECT_instance_t));
                    if (NULL == inst)
                    {
                        LogError("Malloc failed");
                    }
                    else
                    {
                        memset(inst, 0, sizeof(OBJECT_instance_t));
                        inst->instanceId = instanceID;
                        obj->instanceList = LWM2M_LIST_ADD(obj->instanceList, inst);
                    }
                }
            }
        }
    }

    return obj;
}
