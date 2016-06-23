// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef IOTDM_INTERNAL_H
#define IOTDM_INTERNAL_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


/** from wakaama */
#undef NO_ERROR
#include "liblwm2m.h"
#include "internals.h"

/** from azure-shared-c-utility */
#undef LOG

#include "azure_c_shared_utility/iot_logging.h"
#include "azure_c_shared_utility/platform.h"
#include "azure_c_shared_utility/sastoken.h"
#include "azure_c_shared_utility/string_tokenizer.h"
#include "azure_c_shared_utility/tlsio.h"
#include "azure_c_shared_utility/urlencode.h"
#include "azure_c_shared_utility/xio.h"
#include "azure_c_shared_utility/list.h"
#include "azure_c_shared_utility/lock.h"
#include "azure_c_shared_utility/condition.h"

#include "iotdm_lwm2m_client_apis.h"

#ifdef NDEBUG
#define IOT_LogInfo LogInfo
#undef LogInfo
#define LogInfo(...) { /* NO_OP */}
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum IOTHUB_CLIENT_STATE_TAG
{
    BLOCKED,
    LENGTH,
    RECEIVING
} IOTHUB_CLIENT_STATE;


#define MAX_PACKET_SIZE 1024
typedef struct IO_BUFFER_TAG
{
    uint16_t  length;
    uint16_t  available;
    uint8_t   buffer[MAX_PACKET_SIZE];
} IO_BUFFER;

#define SIZEOF_MESSAGE_LENGTH_FIELD 2 // sizeof(IO_BUFFER::length)

typedef struct CLIENT_DATA_TAG
{
    lwm2m_context_t      *session;

    XIO_HANDLE            ioHandle;
    IOTHUB_CLIENT_STATE   state;
    IO_BUFFER             input;
    IOTHUB_CLIENT_CONFIG  config;
    STRING_HANDLE         SAS;

    LOCK_HANDLE           sessionLock;
	LOCK_HANDLE           channelLock;
} CLIENT_DATA;

typedef void(*ON_REGISTER_COMPLETE)(IOTHUB_CLIENT_RESULT result, void* context);

IOTHUB_CLIENT_RESULT iotdmc_register(CLIENT_DATA *cd, ON_REGISTER_COMPLETE onComplete, void* callbackContext);

// object_security.c
lwm2m_object_t *make_security_object(int serverId, const char* serverUri, bool isBootstrap);

// object_global.c
lwm2m_object_t *make_global_object(IOTHUB_CHANNEL_HANDLE iotHubChannel, uint16_t objectID, uint16_t instanceID);
void on_resource_value_changed(IOTHUB_CHANNEL_HANDLE iotHubChannel, uint16_t objectId, uint16_t instanceId, uint16_t resourceId);

// Lwm2m_object_list.c
typedef void(*FREE_SUBITEM_MEMORY)(void *object);
void test_clear_dm_object_list(LIST_HANDLE *list, FREE_SUBITEM_MEMORY freeSubItem);
uint16_t get_total_dm_object_count(LIST_HANDLE list);
IOTHUB_CLIENT_RESULT get_dm_object(LIST_HANDLE list, uint16_t id, void **object);
IOTHUB_CLIENT_RESULT add_dm_object(LIST_HANDLE *list, void *object);

// iotdm_client.c
char *iotdm_strndup(const char *buffer, size_t length);

// iotdm_dispatcher_list.c
void signal_all_resource_changes();
typedef bool (*for_each_oid_predicate)(uint16_t oid, void* context);
bool for_each_oid(for_each_oid_predicate pred, void* context);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !IOTDM_INTERNAL_H */
