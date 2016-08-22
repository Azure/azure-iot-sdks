// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

/** @file iothub_client_version.h
*	@brief Functions for managing the client SDK version.
*/

#ifndef IOTHUB_QUEUE_H
#define IOTHUB_QUEUE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "iothub_client_private.h"

typedef struct IOTHUB_QUEUE_DATA_TAG* IOTHUB_QUEUE_HANDLE;
typedef struct IOTHUB_QUEUE_ENUM_TAG* IOTHUB_QUEUE_ENUM_HANDLE;

#define IOTHUB_QUEUE_RESULT_VALUES     \
    IOTHUB_QUEUE_OK,                   \
    IOTHUB_QUEUE_INVALID_ARG,          \
    IOTHUB_QUEUE_QUEUE_EMPTY,          \
    IOTHUB_QUEUE_ERROR                 \

DEFINE_ENUM(IOTHUB_QUEUE_RESULT, IOTHUB_QUEUE_RESULT_VALUES);

typedef union CLIENT_QUEUE_ITEM_TAG
{
    IOTHUB_MESSAGE_LIST iothub_client_msg;
    IOTHUB_DEVICE_TWIN iothub_device_twin;
} CLIENT_QUEUE_ITEM;

#define QUEUE_ITEM_TYPE_VALUES      \
    QUEUE_ITEM_TYPE_MESSAGE_LIST,   \
    QUEUE_ITEM_TYPE_DEVICE_TWIN     \

DEFINE_ENUM(QUEUE_ITEM_TYPE, QUEUE_ITEM_TYPE_VALUES);

typedef void(*IOTHUB_QUEUE_DESTROY_CALLBACK)(QUEUE_ITEM_TYPE msg_type, CLIENT_QUEUE_ITEM* queue_item);

MOCKABLE_FUNCTION(, IOTHUB_QUEUE_HANDLE, IoTHubQueue_Create_Queue);
MOCKABLE_FUNCTION(, void, IoTHubQueue_Destroy_Queue, IOTHUB_QUEUE_HANDLE, handle, IOTHUB_QUEUE_DESTROY_CALLBACK, destroy_callback);
MOCKABLE_FUNCTION(, IOTHUB_QUEUE_RESULT, IoTHubQueue_Add_Item, IOTHUB_QUEUE_HANDLE, handle, QUEUE_ITEM_TYPE, msg_type, CLIENT_QUEUE_ITEM*, client_queue_item);
MOCKABLE_FUNCTION(, const CLIENT_QUEUE_ITEM*, IoTHubQueue_Get_Queue_Item, IOTHUB_QUEUE_HANDLE, handle);
MOCKABLE_FUNCTION(, IOTHUB_QUEUE_RESULT, IoTHubQueue_Remove_Next_Item, IOTHUB_QUEUE_HANDLE, handle, IOTHUB_QUEUE_DESTROY_CALLBACK, destroy_callback);
MOCKABLE_FUNCTION(, IOTHUB_QUEUE_ENUM_HANDLE, IoTHubQueue_Enum_Queue, IOTHUB_QUEUE_HANDLE, handle);
MOCKABLE_FUNCTION(, const CLIENT_QUEUE_ITEM*, IoTHubQueue_Enum_Next_Item, IOTHUB_QUEUE_ENUM_HANDLE, enum_handle);
MOCKABLE_FUNCTION(, void, IoTHubQueue_Enum_Close, IOTHUB_QUEUE_ENUM_HANDLE, enum_handle);

#ifdef __cplusplus
}
#endif

#endif // IOTHUB_QUEUE_H
