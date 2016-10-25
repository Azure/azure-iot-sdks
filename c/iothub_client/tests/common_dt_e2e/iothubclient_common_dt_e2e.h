// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef IOTHUBCLIENT_COMMON_DT_E2E_H
#define IOTHUBCLIENT_COMMON_DT_E2E_H

#include "iothub_client_ll.h"

extern void dt_e2e_init(void);
extern void dt_e2e_deinit(void);
extern void dt_e2e_send_reported_test(IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol);
extern void dt_e2e_get_complete_desired_test(IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol);

#endif /* IOTHUBCLIENT_COMMON_DT_E2E_H */
