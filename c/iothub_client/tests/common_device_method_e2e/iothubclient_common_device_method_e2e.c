// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include <stddef.h>
#include <string.h>

#include "iothub_messaging_ll.h"

#include "umock_c.h"
#include "testrunnerswitcher.h"

#include "iothub_account.h"
#include "iothubtest.h"



#include "azure_c_shared_utility/platform.h"
#include "azure_c_shared_utility/threadapi.h"
#include "iothub_service_client_auth.h"
#include "iothub_devicemethod.h"
#include "iothub_client.h"


#include "iothubclient_common_device_method_e2e.h"


TEST_DEFINE_ENUM_TYPE(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_RESULT_VALUES);
TEST_DEFINE_ENUM_TYPE(IOTHUB_DEVICE_METHOD_RESULT, IOTHUB_DEVICE_METHOD_RESULT_VALUES);


static IOTHUB_ACCOUNT_INFO_HANDLE g_iothubAcctInfo = NULL;

static size_t g_uniqueTestId = 0;

void device_method_e2e_init(void)
{
    ASSERT_ARE_EQUAL(int, 0, platform_init() );

    g_iothubAcctInfo = IoTHubAccount_Init(true);
    ASSERT_IS_NOT_NULL(g_iothubAcctInfo);

    g_uniqueTestId = 0;
}

void device_method_e2e_deinit(void)
{
    IoTHubAccount_deinit(g_iothubAcctInfo);

    platform_deinit();
}

char *METHOD_NAME = "I'm a method";
const int METHOD_RESPONSE_STATUS = 201;
const unsigned int TIMEOUT = 60;

char *expectedMethodPayload = NULL;

static int DeviceMethodCallback(const char* method_name, const unsigned char* payload, size_t size, unsigned char** response, size_t* resp_size, void* userContextCallback)
{
    int responseCode = METHOD_RESPONSE_STATUS;
    (void)userContextCallback;

    if (strcmp(METHOD_NAME, method_name))
    {
        LogError("Method name incorrect - expected %s but got %s", METHOD_NAME, method_name);
        responseCode = __LINE__;
    }
    if (size != strlen(expectedMethodPayload))
    {
        LogError("payload size incorect - expected %d but got %d", strlen(expectedMethodPayload), size);
        responseCode = __LINE__;
    }
    if (strncmp((char*)payload, expectedMethodPayload, size))
    {
        LogError("Payload strings do not match");
        responseCode = __LINE__;
    }

    *resp_size = size;
    if (size == 0)
    {
        *response = NULL;
    }
    else
    {
        if ((*response = (unsigned char*)malloc(*resp_size)) == NULL)
        {
            LogError("allocation failure");
            responseCode = __LINE__;
        }
        else
        {
            memcpy(*response, payload, *resp_size);
        }
    }
    return responseCode;
}

void test_device_method_with_string(IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol, char *payload)
{
    IOTHUB_CLIENT_RESULT result;
    IOTHUB_CLIENT_CONFIG iotHubConfig = { 0 };
    IOTHUB_CLIENT_HANDLE iotHubClientHandle;
    
    iotHubConfig.iotHubName = IoTHubAccount_GetIoTHubName(g_iothubAcctInfo);
    iotHubConfig.iotHubSuffix = IoTHubAccount_GetIoTHubSuffix(g_iothubAcctInfo);
    iotHubConfig.deviceId = IoTHubAccount_GetDeviceId(g_iothubAcctInfo);
    iotHubConfig.deviceKey = IoTHubAccount_GetDeviceKey(g_iothubAcctInfo);
    iotHubConfig.protocol = protocol;
    
    iotHubClientHandle = IoTHubClient_Create(&iotHubConfig);
    ASSERT_IS_NOT_NULL_WITH_MSG(iotHubClientHandle, "Could not create IoTHubClient");

    result = IoTHubClient_SetDeviceMethodCallback(iotHubClientHandle, DeviceMethodCallback, NULL);
    ASSERT_ARE_EQUAL_WITH_MSG(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_OK, result, "Could not set the device method callback");

    // Wait for the method subscription to go through
    ThreadAPI_Sleep(3 * 1000);
    
    IOTHUB_SERVICE_CLIENT_AUTH_HANDLE iotHubServiceClientHandle = IoTHubServiceClientAuth_CreateFromConnectionString(IoTHubAccount_GetIoTHubConnString(g_iothubAcctInfo));
    ASSERT_IS_NOT_NULL_WITH_MSG(iotHubServiceClientHandle, "Could not create service client handle");
    
    IOTHUB_SERVICE_CLIENT_DEVICE_METHOD_HANDLE serviceClientDeviceMethodHandle = IoTHubDeviceMethod_Create(iotHubServiceClientHandle);
    ASSERT_IS_NOT_NULL_WITH_MSG(serviceClientDeviceMethodHandle, "Could not create device method handle");

    expectedMethodPayload = payload;
    int responseStatus;
    unsigned char* responsePayload;
    size_t responsePayloadSize;
    IOTHUB_DEVICE_METHOD_RESULT invokeResult = IoTHubDeviceMethod_Invoke(serviceClientDeviceMethodHandle, iotHubConfig.deviceId, METHOD_NAME, payload, TIMEOUT, &responseStatus, &responsePayload, &responsePayloadSize);

    ASSERT_ARE_EQUAL_WITH_MSG(IOTHUB_DEVICE_METHOD_RESULT, IOTHUB_DEVICE_METHOD_OK, invokeResult, "IoTHubDeviceMethod_Invoke failed");
    ASSERT_ARE_EQUAL_WITH_MSG(int, METHOD_RESPONSE_STATUS, responseStatus, "response status is incorrect");

    ASSERT_ARE_EQUAL_WITH_MSG(size_t, strlen(payload), responsePayloadSize, "payload size is incorrect");
    if (strncmp((char*)payload, (char*)responsePayload, responsePayloadSize))
    {
        ASSERT_FAIL("response string does not match");
    }

    free(responsePayload);
    
    IoTHubDeviceMethod_Destroy(serviceClientDeviceMethodHandle);
    
    IoTHubServiceClientAuth_Destroy(iotHubServiceClientHandle);
    
    IoTHubClient_Destroy(iotHubClientHandle);
    
}

void device_method_e2e_method_call_with_string(IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol)
{
    test_device_method_with_string(protocol, "I'm a happy little string");
}

void device_method_e2e_method_call_with_single_quoted_json(IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol)
{
    test_device_method_with_string(protocol, "{'foo':41,'bar':42,'baz':'boo'}");
}

void device_method_e2e_method_call_with_double_quoted_json(IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol)
{
    test_device_method_with_string(protocol, "{\"foo\":41,\"bar\":42,\"baz\":\"boo\"}");
}

void device_method_e2e_method_call_with_empty_json_object(IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol)
{
    test_device_method_with_string(protocol, "{}");
}

void device_method_e2e_method_call_with_empty_string(IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol)
{
    test_device_method_with_string(protocol, "");
}

extern void device_method_e2e_method_call_with_embedded_double_quote(IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol)
{
    test_device_method_with_string(protocol, "\"");
}

extern void device_method_e2e_method_call_with_embedded_double_quote_quote(IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol)
{
    test_device_method_with_string(protocol, "\"\"");
}

extern void device_method_e2e_method_call_with_embedded_double_quote_quote_quote(IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol)
{
    test_device_method_with_string(protocol, "\"\"\"");
}

extern void device_method_e2e_method_call_with_embedded_quote(IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol)
{
    test_device_method_with_string(protocol, "'");
}

extern void device_method_e2e_method_call_with_embedded_quote_quote(IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol)
{
    test_device_method_with_string(protocol, "''");
}

extern void device_method_e2e_method_call_with_embedded_quote_quote_quote(IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol)
{
    test_device_method_with_string(protocol, "'''");
}

