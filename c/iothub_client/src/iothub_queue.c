// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h> 
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#include "azure_c_shared_utility/gballoc.h"

#include "iothub_queue.h"

#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/doublylinkedlist.h"
#include "azure_c_shared_utility/xlogging.h"
#include "iothub_client_private.h"

typedef struct IOTHUB_QUEUE_ITEM_TAG
{
    QUEUE_ITEM_TYPE msg_type;
    CLIENT_QUEUE_ITEM* client_queue_item;
    DLIST_ENTRY entry;
} IOTHUB_QUEUE_ITEM;

typedef struct IOTHUB_QUEUE_DATA_TAG
{
    DLIST_ENTRY list_entry;
} IOTHUB_QUEUE_DATA;

typedef struct IOTHUB_QUEUE_ENUM_TAG
{
    PDLIST_ENTRY queue_enum_entry;
    PDLIST_ENTRY start_of_queue;
} IOTHUB_QUEUE_ENUM;

IOTHUB_QUEUE_HANDLE IoTHubQueue_Create_Queue()
{
    IOTHUB_QUEUE_DATA* result;

    result = (IOTHUB_QUEUE_DATA*)malloc(sizeof(IOTHUB_QUEUE_DATA));
    if (result == NULL)
    {
        /* Codes_SRS_IOTHUB_QUEUE_07_001: [If an error is encountered IoTHubQueue_Create_Queue shall return NULL.] */
        LogError("Failure allocating IOTHUB_QUEUE_DATA.");
    }
    else
    {
        /* Codes_SRS_IOTHUB_QUEUE_07_002: [On Success IoTHubQueue_Create_Queue shall return a non-NULL handle that refers to a Queue.] */
        DList_InitializeListHead(&result->list_entry);
    }
    return result;
}

void IoTHubQueue_Destroy_Queue(IOTHUB_QUEUE_HANDLE handle, IOTHUB_QUEUE_DESTROY_CALLBACK destroy_callback)
{
    /* Codes_SRS_IOTHUB_QUEUE_07_003: [If handle is NULL then IoTHubQueue_Destroy_Queue shall do nothing.] */
    if (handle != NULL)
    {
        IOTHUB_QUEUE_DATA* queue_data = (IOTHUB_QUEUE_DATA*)handle;
        while (!DList_IsListEmpty(&queue_data->list_entry) )
        {
            PDLIST_ENTRY currentEntry = DList_RemoveHeadList(&queue_data->list_entry);
            IOTHUB_QUEUE_ITEM* queue_item = containingRecord(currentEntry, IOTHUB_QUEUE_ITEM, entry);
            
            /* Codes_SRS_IOTHUB_QUEUE_07_005: [If destroy_callback is not NULL, IoTHubQueue_Destroy_Queue shall call destroy_callback with the item to be deleted.] */
            if (destroy_callback != NULL)
            {
                destroy_callback(queue_item->msg_type, queue_item->client_queue_item);
            }
            /* Codes_SRS_IOTHUB_QUEUE_07_004: [IoTHubQueue_Destroy_Queue shall only free memory allocated within this compilation unit.] */
            free(queue_item);
        }
        free(queue_data);
    }
}

IOTHUB_QUEUE_RESULT IoTHubQueue_Add_Item(IOTHUB_QUEUE_HANDLE handle, QUEUE_ITEM_TYPE msg_type, CLIENT_QUEUE_ITEM* client_queue_item)
{
    IOTHUB_QUEUE_RESULT result;
    /* Codes_SRS_IOTHUB_QUEUE_07_006: [If handle or queue_item are NULL IoTHubQueue_Add_Item shall return IOTHUB_QUEUE_INVALID_ARG.] */
    if (handle == NULL || client_queue_item == NULL)
    {
        LogError("Invalid argument: Handle or client_queue_item are NULL.");
        result = IOTHUB_QUEUE_INVALID_ARG;
    }
    else
    {
        IOTHUB_QUEUE_ITEM* queue_item = (IOTHUB_QUEUE_ITEM*)malloc(sizeof(IOTHUB_QUEUE_ITEM));
        if (queue_item == NULL)
        {
            /* Codes_SRS_IOTHUB_QUEUE_07_009: [If any error is encountered IoTHubQueue_Add_Item shall return IOTHUB_QUEUE_ERROR.] */
            LogError("Failure allocating IOTHUB_QUEUE_ITEM.");
            result = IOTHUB_QUEUE_ERROR;
        }
        else
        {
            IOTHUB_QUEUE_DATA* queue_data = (IOTHUB_QUEUE_DATA*)handle;

            /* Codes_SRS_IOTHUB_QUEUE_07_007: [IoTHubQueue_Add_Item shall allocate an queue item and store the item into the queue.] */
            queue_item->msg_type = msg_type;
            queue_item->client_queue_item = client_queue_item;
            DList_InsertTailList(&queue_data->list_entry, &queue_item->entry);

            /* Codes_SRS_IOTHUB_QUEUE_07_008: [On success IoTHubQueue_Add_Item shall return IOTHUB_QUEUE_OK.] */
            result = IOTHUB_QUEUE_OK;
        }
    }
    return result;
}

const CLIENT_QUEUE_ITEM* IoTHubQueue_Get_Queue_Item(IOTHUB_QUEUE_HANDLE handle)
{
    CLIENT_QUEUE_ITEM* result;
    /* Codes_SRS_IOTHUB_QUEUE_07_010: [If handle is NULL IoTHubQueue_Get_Queue_Item shall return NULL.] */
    if (handle == NULL)
    {
        LogError("Invalid argument: handle is NULL");
        result = NULL;
    }
    else
    {
        IOTHUB_QUEUE_DATA* queue_data = (IOTHUB_QUEUE_DATA*)handle;
        PDLIST_ENTRY currentListEntry = queue_data->list_entry.Flink;
        if (currentListEntry == &queue_data->list_entry)
        {
            /* Codes_SRS_IOTHUB_QUEUE_07_011: [If the queue is empty, IoTHubQueue_Get_Queue_Item shall return NULL.] */
            // Nothing in the list
            result = NULL;
        }
        else
        {
            /* Codes_SRS_IOTHUB_QUEUE_07_012: [If the IOTHUB_QUEUE_ITEM is retrieved from the queue IoTHubQueue_Get_Queue_Item shall return a const IOTHUB_QUEUE_ITEM.] */
            IOTHUB_QUEUE_ITEM* queue_item = containingRecord(currentListEntry, IOTHUB_QUEUE_ITEM, entry);
            result = queue_item->client_queue_item;
        }
    }
    return result;
}

IOTHUB_QUEUE_RESULT IoTHubQueue_Remove_Next_Item(IOTHUB_QUEUE_HANDLE handle, IOTHUB_QUEUE_DESTROY_CALLBACK destroy_callback)
{
    IOTHUB_QUEUE_RESULT result = IOTHUB_QUEUE_ERROR;
    if (handle == NULL)
    {
        /* Codes_SRS_IOTHUB_QUEUE_07_013: [If handle is NULL IoTHubQueue_Remove_Next_Item shall return IOTHUB_QUEUE_INVALID_ARG.] */
        LogError("Invalid argument: handle is NULL");
        result = IOTHUB_QUEUE_INVALID_ARG;
    }
    else
    {
        IOTHUB_QUEUE_DATA* queue_data = (IOTHUB_QUEUE_DATA*)handle;
        PDLIST_ENTRY currentListEntry = queue_data->list_entry.Flink;
        if (currentListEntry == &queue_data->list_entry)
        {
            /* Codes_SRS_IOTHUB_QUEUE_07_014: [If the queue is empty, IoTHubQueue_Remove_Next_Item shall return IOTHUB_QUEUE_QUEUE_EMPTY.] */
            // Nothing in the list
            result = IOTHUB_QUEUE_QUEUE_EMPTY;
        }
        else
        {
            DLIST_ENTRY saveListEntry;
            saveListEntry.Flink = currentListEntry->Flink;

            /* Codes_SRS_IOTHUB_QUEUE_07_015: [If the Item is successfully Removed from the queue and destroy_callback is not NULL, IoTHubQueue_Remove_Next_Item will call destroy_callback and deallocate the item.] */
            IOTHUB_QUEUE_ITEM* queue_item = containingRecord(currentListEntry, IOTHUB_QUEUE_ITEM, entry);
            if (destroy_callback != NULL)
            {
                destroy_callback(queue_item->msg_type, queue_item->client_queue_item);
            }
            (void)DList_RemoveEntryList(currentListEntry);
            free(queue_item);
            currentListEntry->Flink = saveListEntry.Flink;
            /* Codes_SRS_IOTHUB_QUEUE_07_016: [On success IoTHubQueue_Remove_Next_Item shall return IOTHUB_QUEUE_OK.] */
            result = IOTHUB_QUEUE_OK;
        }
    }
    return result;
}

IOTHUB_QUEUE_ENUM_HANDLE IoTHubQueue_Enum_Queue(IOTHUB_QUEUE_HANDLE handle)
{
    IOTHUB_QUEUE_ENUM* result;
    /* Codes_SRS_IOTHUB_QUEUE_07_017: [If handle is NULL IoTHubQueue_Enum_Queue shall return NULL.] */
    if (handle == NULL)
    {
        LogError("Invalid argument: handle is NULL");
        result = NULL;
    }
    else
    {
        IOTHUB_QUEUE_DATA* queue_data = (IOTHUB_QUEUE_DATA*)handle;
        PDLIST_ENTRY currentListEntry = queue_data->list_entry.Flink;
        /* Codes_SRS_IOTHUB_QUEUE_07_025: [If the queue referenced by handle is empty IoTHubQueue_Enum_Queue shall return NULL.] */
        if (currentListEntry == &queue_data->list_entry)
        {
            LogError("List entry is Empty");
            result = NULL;
        }
        else
        {
            /* Codes_SRS_IOTHUB_QUEUE_07_018: [IoTHubQueue_Enum_Queue shall allocate and initialize the data neccessary for enumeration of the queue.] */
            result = (IOTHUB_QUEUE_ENUM*)malloc(sizeof(IOTHUB_QUEUE_ENUM) );
            if (result == NULL)
            {
                /* Codes_SRS_IOTHUB_QUEUE_07_026: [If any error is encountered IoTHubQueue_Enum_Queue shall return NULL.] */
                LogError("unable to allocate Queue Enum");
            }
            else
            {
                /* Codes_SRS_IOTHUB_QUEUE_07_019: [On Success IoTHubQueue_Enum_Queue shall return a IOTHUB_QUEUE_ENUM_HANDLE.] */
                result->start_of_queue = result->queue_enum_entry = &queue_data->list_entry;
            }
        }
    }
    return result;
}

const CLIENT_QUEUE_ITEM* IoTHubQueue_Enum_Next_Item(IOTHUB_QUEUE_ENUM_HANDLE enum_handle)
{
    const CLIENT_QUEUE_ITEM* result;
    /* Codes_SRS_IOTHUB_QUEUE_07_020: [If enum_handle is NULL IoTHubQueue_Enum_Next_Item shall return NULL.] */
    if (enum_handle == NULL)
    {
        LogError("Invalid argument: handle is NULL");
        result = NULL;
    }
    else
    {
        IOTHUB_QUEUE_ENUM* queue_enum = (IOTHUB_QUEUE_ENUM*)enum_handle;
        PDLIST_ENTRY currentListEntry = queue_enum->queue_enum_entry->Flink;
        /* Codes_SRS_IOTHUB_QUEUE_07_021: [If the end of the queue is reached IoTHubQueue_Enum_Next_Item shall return NULL.] */
        if (currentListEntry == queue_enum->start_of_queue)
        {
            // Back to the start of the queue
            result = NULL;
        }
        else
        {
            /* Codes_SRS_IOTHUB_QUEUE_07_022: [On success IoTHubQueue_Enum_Next_Item shall return the next queue item from the previous call to IoTHubQueue_Enum_Next_Item.] */
            IOTHUB_QUEUE_ITEM* queue_item = containingRecord(currentListEntry, IOTHUB_QUEUE_ITEM, entry);
            result = queue_item->client_queue_item;

            DLIST_ENTRY saveListEntry;
            saveListEntry.Flink = queue_enum->queue_enum_entry->Flink;
            queue_enum->queue_enum_entry = saveListEntry.Flink;
        }
    }
    return result;
}

void IoTHubQueue_Enum_Close(IOTHUB_QUEUE_ENUM_HANDLE enum_handle)
{
    /* Codes_SRS_IOTHUB_QUEUE_07_023: [If enum_handle is NULL IoTHubQueue_Enum_Close shall do nothing.] */
    if (enum_handle != NULL)
    {
        /* Codes_SRS_IOTHUB_QUEUE_07_024: [IoTHubQueue_Enum_Close shall deallocate any information that has been allocated in IoTHubQueue_Enum_Queue.] */
        IOTHUB_QUEUE_ENUM* queue_enum = (IOTHUB_QUEUE_ENUM*)enum_handle;
        free(queue_enum);
    }
}
