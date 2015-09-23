// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef IOTHUB_CLIENT_PRIVATE_H
#define IOTHUB_CLIENT_PRIVATE_H

#include <signal.h>

#include "macro_utils.h"
#include "threadapi.h"
#include "lock.h"
#include "crt_abstractions.h"

#include "doublylinkedlist.h"
#include "iothub_message.h"
#include "iothub_client_ll.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define IOTHUB_BATCHSTATE_RESULT_VALUES IOTHUB_BATCHSTATE_SUCCESS,  \
    IOTHUB_BATCHSTATE_FAILED 

DEFINE_ENUM(IOTHUB_BATCHSTATE_RESULT, IOTHUB_BATCHSTATE_RESULT_VALUES);

#define EVENT_ENDPOINT "/messages/events"
#define MESSAGE_ENDPOINT "/messages/devicebound"
#define MESSAGE_ENDPOINT_HTTP "/messages/devicebound"
#define MESSAGE_ENDPOINT_HTTP_ETAG "/messages/devicebound/"
#define CBS_REPLY_TO "cbs"
#define CBS_ENDPOINT "/$" CBS_REPLY_TO
#define API_VERSION "?api-version=2015-08-15-preview"
#define REJECT_QUERY_PARAMETER "&reject"
#define PROTON_EVENT_OUTGOING_WINDOW_SIZE (10)
#define PROTON_CBS_OUTGOING_WINDOW_SIZE (1)
#define PROTON_MESSAGE_INCOMING_WINDOW_SIZE (1)
#define PROTON_CBS_INCOMING_WINDOW_SIZE (1)
#define PROTON_INCOMING_WINDOW_SIZE (PROTON_MESSAGE_INCOMING_WINDOW_SIZE + PROTON_CBS_INCOMING_WINDOW_SIZE)
#define PROTON_OUTGOING_WINDOW_SIZE (PROTON_EVENT_OUTGOING_WINDOW_SIZE + PROTON_CBS_OUTGOING_WINDOW_SIZE)
#define PROTON_PROCESSING_YIELD_IN_MILLISECONDS (100)
#define PROTON_MESSENGER_STOP_TRIES (10)
#define PROTON_MAXIMUM_EVENT_LENGTH (256*1024)



extern void IoTHubClient_LL_SendComplete(IOTHUB_CLIENT_LL_HANDLE handle, PDLIST_ENTRY completed, IOTHUB_BATCHSTATE_RESULT result);
extern IOTHUBMESSAGE_DISPOSITION_RESULT IoTHubClient_LL_MessageCallback(IOTHUB_CLIENT_LL_HANDLE handle, IOTHUB_MESSAGE_HANDLE message);

typedef struct IOTHUBTRANSPORT_CONFIG_TAG
{
    const IOTHUB_CLIENT_CONFIG* upperConfig;
    PDLIST_ENTRY waitingToSend;
}IOTHUBTRANSPORT_CONFIG;

typedef struct IOTHUB_MESSAGE_LIST_TAG
{
    IOTHUB_MESSAGE_HANDLE messageHandle;
    IOTHUB_CLIENT_EVENT_CONFIRMATION_CALLBACK callback;
    void* context; 
    DLIST_ENTRY entry;
}IOTHUB_MESSAGE_LIST;

typedef void* TRANSPORT_HANDLE;

typedef IOTHUB_CLIENT_RESULT(*pfIoTHubTransport_SetOption)(TRANSPORT_HANDLE handle, const char *optionName, const void* value);
typedef TRANSPORT_HANDLE(*pfIoTHubTransport_Create)(const IOTHUBTRANSPORT_CONFIG* config);
typedef void (*pfIoTHubTransport_Destroy)(TRANSPORT_HANDLE handle);
typedef int (*pfIoTHubTransport_Subscribe)(TRANSPORT_HANDLE handle);
typedef void (*pfIoTHubTransport_Unsubscribe)(TRANSPORT_HANDLE handle);
typedef void (*pfIoTHubTransport_DoWork)(TRANSPORT_HANDLE handle, IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle);
typedef IOTHUB_CLIENT_RESULT(*pfIoTHubTransport_GetSendStatus)(TRANSPORT_HANDLE handle, IOTHUB_CLIENT_STATUS *iotHubClientStatus);

#define TRANSPORT_PROVIDER_FIELDS                            \
pfIoTHubTransport_SetOption IoTHubTransport_SetOption;       \
pfIoTHubTransport_Create IoTHubTransport_Create;             \
pfIoTHubTransport_Destroy IoTHubTransport_Destroy;           \
pfIoTHubTransport_Subscribe IoTHubTransport_Subscribe;       \
pfIoTHubTransport_Unsubscribe IoTHubTransport_Unsubscribe;   \
pfIoTHubTransport_DoWork IoTHubTransport_DoWork;             \
pfIoTHubTransport_GetSendStatus IoTHubTransport_GetSendStatus  /*there's an intentional missing ; on this line*/ \

typedef struct TRANSPORT_PROVIDER_TAG
{
    TRANSPORT_PROVIDER_FIELDS;
}TRANSPORT_PROVIDER;

#ifdef __cplusplus
}
#endif

#endif /* IOTHUB_CLIENT_PRIVATE_H */
