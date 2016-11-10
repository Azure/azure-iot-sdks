// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "iothubclient_common_device_method_e2e.h"
#include "testrunnerswitcher.h"
#include "iothubtransportamqp.h"
#include "iothub_devicemethod.h"


static TEST_MUTEX_HANDLE g_dllByDll;

BEGIN_TEST_SUITE(iothubclient_amqp_device_method_e2e)

    TEST_SUITE_INITIALIZE(TestClassInitialize)
    {
        TEST_INITIALIZE_MEMORY_DEBUG(g_dllByDll);
        device_method_e2e_init();
    }

    TEST_SUITE_CLEANUP(TestClassCleanup)
    {
        device_method_e2e_deinit();
        TEST_DEINITIALIZE_MEMORY_DEBUG(g_dllByDll);
    }

    TEST_FUNCTION_CLEANUP(TestFunctionCleanup)
    {
        device_method_function_cleanup();
    }

    TEST_FUNCTION(IotHub_AMQP_Method_Call_With_String)
    {
        device_method_e2e_method_call_with_string(AMQP_Protocol);
    }

    TEST_FUNCTION(IotHub_AMQP_Method_Call_With_Double_Quoted_Json)
    {
        device_method_e2e_method_call_with_double_quoted_json(AMQP_Protocol);
    }

    TEST_FUNCTION(IotHub_AMQP_Method_Call_With_Empty_Json_Object)
    {
        device_method_e2e_method_call_with_empty_json_object(AMQP_Protocol);
    }

    TEST_FUNCTION(IotHub_AMQP_Method_Call_With_Null)
    {
        device_method_e2e_method_call_with_null(AMQP_Protocol);
    }

    TEST_FUNCTION(IotHub_AMQP_Method_Call_With_Embedded_Double_Quote)
    {
        device_method_e2e_method_call_with_embedded_double_quote(AMQP_Protocol);
    }

    TEST_FUNCTION(IotHub_AMQP_Method_Call_With_Embedded_Single_Quote)
    {
        device_method_e2e_method_call_with_embedded_single_quote(AMQP_Protocol);
    }

END_TEST_SUITE(iothubclient_amqp_device_method_e2e)

