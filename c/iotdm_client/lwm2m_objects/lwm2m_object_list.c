// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#include "iotdm_internal.h"

void test_clear_dm_object_list(LIST_HANDLE *list, FREE_SUBITEM_MEMORY freeSubItem)
{
    if (list != NULL)
    {
        LIST_ITEM_HANDLE item = list_get_head_item(*list);
        while (item != NULL)
        {
            void *pv = (void*)list_item_get_value(item);
            if (pv != NULL)
            {
                if (freeSubItem != NULL)
                {
                    freeSubItem(pv);
                }
                lwm2m_free(pv);
            }
            list_remove(*list, item);
            item = list_get_head_item(*list);
        }
    }
}

uint16_t get_total_dm_object_count(LIST_HANDLE list)
{
    uint16_t count = 0;
    if (list != NULL)
    {
        LIST_ITEM_HANDLE item = list_get_head_item(list);
        while (item != NULL)
        {
            count++;
            item = list_get_next_item(item);
        }
    }
    return count;
}

IOTHUB_CLIENT_RESULT add_dm_object(LIST_HANDLE *list, void *object)
{
    IOTHUB_CLIENT_RESULT res = IOTHUB_CLIENT_OK;
    if (list == NULL)
    {
        res = IOTHUB_CLIENT_INVALID_ARG;
    }
    else
    {
        if (*list == NULL)
        {
            *list = list_create();
            if (*list == NULL)
            {
                res = IOTHUB_CLIENT_ERROR;
            }
        }
        if (*list != NULL)
        {
            list_add(*list, object);
        }
    }
 
    return res;
}

IOTHUB_CLIENT_RESULT get_dm_object(LIST_HANDLE list, uint16_t id, void **obj)
{
    IOTHUB_CLIENT_RESULT res = IOTHUB_CLIENT_ERROR;
    
    if (obj == NULL)
    {
        res = IOTHUB_CLIENT_INVALID_ARG;
    }
    else
    {
        *obj = NULL;
        LIST_ITEM_HANDLE item = list_get_head_item(list);
        while (item != NULL)
        {
            void *pv = (void*)list_item_get_value(item);
            if (pv != NULL)
            {
                
                if (*((uint16_t*)pv) == id)
                {
                    *obj = pv;
                    res = IOTHUB_CLIENT_OK;
                    break;
                }
            }
            item = list_get_next_item(item);
         }
    }
   return res;
}
