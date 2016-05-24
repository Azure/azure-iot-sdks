/*******************************************************************************
 *
 * Copyright (c) 2015 Bosch Software Innovations GmbH Germany.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * The Eclipse Distribution License is available at
 *    http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Bosch Software Innovations GmbH - Please refer to git log
 *    Pascal Rieux - please refer to git log
 *    
 ******************************************************************************/

// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "iotdm_internal.h"

typedef struct _server_instance_
{
    struct _server_instance_ *next;   // matches lwm2m_list_t::next
    uint16_t    instanceId;           // matches lwm2m_list_t::id
    uint16_t    shortServerId;
    uint32_t    lifetime;
    bool        storing;
    bool        disabled;              // do not send updates or re-register!
    char        binding[4];
} server_instance_t;

/* this code is here to facilitate unit testing / debugging - it should be removed before release*/
static uint8_t prv_read(uint16_t instanceId, int *numDataP, lwm2m_data_t **dataArrayP, lwm2m_object_t *objectP)
{
    uint8_t result;
    int     index = 0;
    do
    {
        lwm2m_data_t      *tlvP = (*dataArrayP) + index;
        server_instance_t *targetP = (server_instance_t *)lwm2m_list_find(objectP->instanceList, instanceId);
        switch (tlvP->id)
        {
            case LWM2M_SERVER_SHORT_ID_ID:
                lwm2m_data_encode_int(targetP->shortServerId, tlvP);
                result = COAP_205_CONTENT;

                break;

            case LWM2M_SERVER_LIFETIME_ID:
                lwm2m_data_encode_int(targetP->lifetime, tlvP);
                result = COAP_205_CONTENT;

                break;

            case LWM2M_SERVER_BINDING_ID:
                tlvP->value.asBuffer.buffer = (uint8_t *)targetP->binding;
                tlvP->value.asBuffer.length = strlen(targetP->binding);
                // BKTODO tlvP->flags = LWM2M_TLV_FLAG_STATIC_DATA;
                tlvP->type = LWM2M_TYPE_STRING;
                result = COAP_205_CONTENT;

                break;

            default:
                result = global_object_read(instanceId, numDataP, dataArrayP, objectP);
                break;
        }

        index++;
    } while ((index < *numDataP) && (result = COAP_205_CONTENT));

    return result;
}

lwm2m_object_t *make_server_object(int serverId, int lifetime, bool storing)
{
    lwm2m_object_t *serverObj = (lwm2m_object_t *)lwm2m_malloc(sizeof(lwm2m_object_t));
    if (NULL != serverObj)
    {
        memset(serverObj, 0, sizeof(lwm2m_object_t));
        serverObj->objID = LWM2M_SERVER_OBJECT_ID;
        serverObj->readFunc = prv_read;
        serverObj->writeFunc = global_object_write;
        serverObj->executeFunc = global_object_execute;

        // Manually create an hardcoded server
        server_instance_t *serverInstance = (server_instance_t *) lwm2m_malloc(sizeof(server_instance_t));
        if (NULL == serverInstance)
        {
            lwm2m_free(serverObj);
            return NULL;
        }

        memset(serverInstance, 0, sizeof(server_instance_t));

        serverInstance->instanceId = 0;
        serverInstance->shortServerId = serverId;
        serverInstance->lifetime = lifetime;
        serverInstance->storing = storing;
        serverInstance->disabled = false;
        sprintf(serverInstance->binding, "T");

        serverObj->instanceList = LWM2M_LIST_ADD(serverObj->instanceList, serverInstance);
    }

    return serverObj;
}
