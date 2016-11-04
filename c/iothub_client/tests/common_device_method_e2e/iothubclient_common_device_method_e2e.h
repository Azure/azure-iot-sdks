// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef COMMON_DEVICE_METHOD_E2E
#define COMMON_DEVICE_METHOD_E2E

#include "iothub_client.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

extern void device_method_e2e_init(void);

extern void device_method_e2e_deinit(void);

extern void device_method_function_cleanup();

extern void device_method_e2e_method_call_with_string(IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol);

extern void device_method_e2e_method_call_with_double_quoted_json(IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol);

extern void device_method_e2e_method_call_with_empty_json_object(IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol);

extern void device_method_e2e_method_call_with_null(IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol);

extern void device_method_e2e_method_call_with_embedded_double_quote(IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol);

extern void device_method_e2e_method_call_with_embedded_single_quote(IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // COMMON_DEVICE_METHOD_E2E
