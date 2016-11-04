// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "testrunnerswitcher.h"
#include "iothubclient_common_dt_e2e.h"
#include "iothubtransportmqtt.h"

static TEST_MUTEX_HANDLE g_dllByDll;

BEGIN_TEST_SUITE(iothubclient_mqtt_dt_e2e)

TEST_SUITE_INITIALIZE(TestClassInitialize)
{
    TEST_INITIALIZE_MEMORY_DEBUG(g_dllByDll);
    dt_e2e_init();
}

TEST_SUITE_CLEANUP(TestClassCleanup)
{
    dt_e2e_deinit();
    TEST_DEINITIALIZE_MEMORY_DEBUG(g_dllByDll);
}

TEST_FUNCTION(IoTHub_MQTT_SendReported_e2e)
{
    dt_e2e_send_reported_test(MQTT_Protocol);
}

TEST_FUNCTION(IoTHub_MQTT_GetFullDesired_e2e)
{
    dt_e2e_get_complete_desired_test(MQTT_Protocol);
}

END_TEST_SUITE(iothubclient_mqtt_dt_e2e)
