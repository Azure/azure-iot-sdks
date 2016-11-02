// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef IOTHUBCLIENT_COMMON_E2E_H
#define IOTHUBCLIENT_COMMON_E2E_H

#include "iothub_client_ll.h"

#ifdef __cplusplus
extern "C" {
#endif

extern void e2e_init(void);
extern void e2e_deinit(void);
extern void e2e_send_event_test(IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol);
extern void e2e_recv_message_test(IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol);

#ifdef __cplusplus
}
#endif

#endif /* IOTHUBCLIENT_COMMON_E2E_H */
