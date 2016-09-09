// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <cstdlib>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "testrunnerswitcher.h"
#include "iothub_client_version.h"

BEGIN_TEST_SUITE(version_ut)

    TEST_FUNCTION(the_version_constant_has_the_expected_value)
    {
        ASSERT_ARE_EQUAL(char_ptr, "1.0.15", IOTHUB_SDK_VERSION);
    }

    /*Tests_SRS_IOTHUBCLIENT_05_001: [IoTHubClient_GetVersionString shall return a pointer to a constant string which indicates the version of IoTHubClient API.]*/
    TEST_FUNCTION(IoTHubClient_GetVersionString_returns_the_version)
    {
        ASSERT_ARE_EQUAL(char_ptr, IOTHUB_SDK_VERSION, IoTHubClient_GetVersionString());
    }

END_TEST_SUITE(version_ut)
