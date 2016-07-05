// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef IOTHUB_CLIENT_PRIVATE_H
#define IOTHUB_CLIENT_PRIVATE_H

#include <signal.h>

#include "azure_c_shared_utility/macro_utils.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_c_shared_utility/doublylinkedlist.h"

#include "iothub_message.h"
#include "iothub_client_ll.h"

#ifdef __cplusplus
extern "C"
{
#endif



#define EVENT_ENDPOINT "/messages/events"
#define MESSAGE_ENDPOINT "/messages/devicebound"
#define MESSAGE_ENDPOINT_HTTP "/messages/devicebound"
#define MESSAGE_ENDPOINT_HTTP_ETAG "/messages/devicebound/"
#define CLIENT_DEVICE_TYPE_PREFIX "iothubclient"
#define CLIENT_DEVICE_BACKSLASH "/"
#define CBS_REPLY_TO "cbs"
#define CBS_ENDPOINT "/$" CBS_REPLY_TO
#define API_VERSION "?api-version=2016-02-03"
#define REJECT_QUERY_PARAMETER "&reject"

extern void IoTHubClient_LL_SendComplete(IOTHUB_CLIENT_LL_HANDLE handle, PDLIST_ENTRY completed, IOTHUB_CLIENT_CONFIRMATION_RESULT result);
extern IOTHUBMESSAGE_DISPOSITION_RESULT IoTHubClient_LL_MessageCallback(IOTHUB_CLIENT_LL_HANDLE handle, IOTHUB_MESSAGE_HANDLE message);

typedef struct IOTHUB_MESSAGE_LIST_TAG
{
    IOTHUB_MESSAGE_HANDLE messageHandle;
    IOTHUB_CLIENT_EVENT_CONFIRMATION_CALLBACK callback;
    void* context; 
    DLIST_ENTRY entry;
    uint64_t ms_timesOutAfter; /* a value of "0" means "no timeout", if the IOTHUBCLIENT_LL's handle tickcounter > msTimesOutAfer then the message shall timeout*/
}IOTHUB_MESSAGE_LIST;


#ifdef __cplusplus
}
#endif

#endif /* IOTHUB_CLIENT_PRIVATE_H */
