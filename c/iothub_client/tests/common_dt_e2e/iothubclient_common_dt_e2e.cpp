// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "testrunnerswitcher.h"

#include "serializer.h"
#include "iothub_client.h"
#include "iothub_client_options.h"
#include "iothub_devicetwin.h"

#include "iothub_account.h"
#include "iothubtest.h"

#include "azure_c_shared_utility/platform.h"
#include "azure_c_shared_utility/threadapi.h"
#include "azure_c_shared_utility/uniqueid.h"

#include "parson.h"
#include "../../../certs/certs.h"

static bool g_callbackRecv = false;

static IOTHUB_ACCOUNT_INFO_HANDLE g_iothubAcctInfo = NULL;

#define MAX_CLOUD_TRAVEL_TIME  60.0
#define BUFFER_SIZE            37

BEGIN_NAMESPACE(Contoso);

DECLARE_MODEL(thingie_t,
    WITH_REPORTED_PROPERTY(ascii_char_ptr, string_property),
    WITH_REPORTED_PROPERTY(int, integer_property)
);

END_NAMESPACE(Contoso);

TEST_DEFINE_ENUM_TYPE(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_RESULT_VALUES);
TEST_DEFINE_ENUM_TYPE(CODEFIRST_RESULT, CODEFIRST_RESULT_VALUES);
TEST_DEFINE_ENUM_TYPE(SERIALIZER_RESULT, SERIALIZER_RESULT_VALUES);


typedef struct DEVICE_DATA_TAG
{
    thingie_t *device;
    bool reportedAck;   // true when device callback has been called
    int  cb_status;     // status reported by the callback
    LOCK_HANDLE lock;
} DEVICE_DATA;

static void DeviceTwinCallback(int status_code, void* userContextCallback)
{
    DEVICE_DATA *device = (DEVICE_DATA *) userContextCallback;
    if (Lock(device->lock) == LOCK_ERROR)
    {
        LogError("unable to lock");
    }
    else
    {
        device->cb_status = status_code;
        device->reportedAck = true;
        (void)Unlock(device->lock);
    }
}

static int generate_new_int(void)
{
    int retValue;
    time_t nowTime = time(NULL);

    retValue = (int) nowTime;
    return retValue;
}

static DEVICE_DATA *device_data_new(void)
{
    DEVICE_DATA *retValue = (DEVICE_DATA *) malloc(sizeof(DEVICE_DATA));

    if (retValue == NULL)
    {
        LogError("failed to allocate device data structure.");
    }
    else
    {
        retValue->device = CREATE_MODEL_INSTANCE(Contoso, thingie_t);
        if (retValue->device == NULL)
        {
            LogError("failed to allocate the properties portion of the device data structure.");
            free(retValue);
            retValue = NULL;
        }
        else
        {
            if ( (retValue->device->string_property = (char *) malloc(BUFFER_SIZE)) == NULL ||
                 UniqueId_Generate(retValue->device->string_property, BUFFER_SIZE) != UNIQUEID_OK)
            {
                LogError("failed to set the string test property.");
                DESTROY_MODEL_INSTANCE(retValue->device);
                free(retValue);
                retValue = NULL;
            }
            else
            {
                retValue->device->integer_property = generate_new_int();
                retValue->lock = Lock_Init();
                if (retValue->lock == NULL)
                {
                    LogError("failed to construct a lock.");
                    free(retValue->device->string_property);
                    DESTROY_MODEL_INSTANCE(retValue->device);
                    free(retValue);
                    retValue = NULL;
                }
                else
                {
                    retValue->reportedAck = false;
                    retValue->cb_status = -1;
                }
            }
        }
    }

    return retValue;
}

static void device_data_delete(DEVICE_DATA *deviceData)
{
    if (deviceData != NULL)
    {
        Lock_Deinit(deviceData->lock);
        free(deviceData->device->string_property);
        DESTROY_MODEL_INSTANCE(deviceData->device);
        free(deviceData);
    }
}

void dt_e2e_init(void)
{
    int result = platform_init();
    ASSERT_ARE_EQUAL_WITH_MSG(int, 0, result, "First platform init failed.");

    g_iothubAcctInfo = IoTHubAccount_Init(true);
    ASSERT_IS_NOT_NULL_WITH_MSG(g_iothubAcctInfo, "Could not initialize IoTHubAccount.");

    /* the return value from the second init is deliberatly ignored. */
    platform_init();

    SERIALIZER_RESULT sResult = serializer_init(NULL);
    ASSERT_ARE_EQUAL_WITH_MSG(SERIALIZER_RESULT, SERIALIZER_OK, sResult, "serializer initialize failed.");
}

void dt_e2e_deinit(void)
{
    serializer_deinit();
    IoTHubAccount_deinit(g_iothubAcctInfo);

    // Need a double deinit
    platform_deinit();
    platform_deinit();
}

void dt_e2e_send_reported_test(IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol)
{
    // arrange
    IOTHUB_CLIENT_CONFIG iotHubConfig = { 0 };

    iotHubConfig.iotHubName = IoTHubAccount_GetIoTHubName(g_iothubAcctInfo);
    iotHubConfig.iotHubSuffix = IoTHubAccount_GetIoTHubSuffix(g_iothubAcctInfo);
    iotHubConfig.deviceId = IoTHubAccount_GetDeviceId(g_iothubAcctInfo);
    iotHubConfig.deviceKey = IoTHubAccount_GetDeviceKey(g_iothubAcctInfo);
    iotHubConfig.protocol = protocol;

    IOTHUB_CLIENT_HANDLE iotHubClientHandle;
    DEVICE_DATA *deviceData = device_data_new();
    ASSERT_IS_NOT_NULL_WITH_MSG(deviceData, "Could not allocate the client data.");

    // Send the Event
    {
        // Create the IoT Hub Data
        iotHubClientHandle = IoTHubClient_Create(&iotHubConfig);
        ASSERT_IS_NOT_NULL_WITH_MSG(iotHubClientHandle, "Could not create IoTHubClient.");

        // Turn on Log
        bool trace = true;
        (void)IoTHubClient_SetOption(iotHubClientHandle, OPTION_LOG_TRACE, &trace);
        (void)IoTHubClient_SetOption(iotHubClientHandle, "TrustedCerts", certificates);

        // generate the data
        unsigned char *buffer;
        size_t         bufferSize;
        CODEFIRST_RESULT result = SERIALIZE_REPORTED_PROPERTIES(&buffer, &bufferSize, *(deviceData->device));
        ASSERT_ARE_EQUAL_WITH_MSG(CODEFIRST_RESULT, CODEFIRST_OK, result, "Serializing reported data failed.");

        // act
        IOTHUB_CLIENT_RESULT iot_result = IoTHubClient_SendReportedState(iotHubClientHandle, buffer, bufferSize, DeviceTwinCallback, deviceData);
        ASSERT_ARE_EQUAL_WITH_MSG(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_OK, iot_result, "sending reported data failed.");

        // cleanup
        free(buffer);
    }

    time_t beginOperation, nowTime;
    beginOperation = time(NULL);
    while (
        (nowTime = time(NULL)),
        (difftime(nowTime, beginOperation) < MAX_CLOUD_TRAVEL_TIME) // time box
        )
    {
        if (Lock(deviceData->lock) != LOCK_OK)
        {
            ASSERT_FAIL("unable to lock.");
        }
        else
        {
            if (deviceData->reportedAck)
            {
                Unlock(deviceData->lock);
                break;
            }
            Unlock(deviceData->lock);
        }
        ThreadAPI_Sleep(1000);
    }

    if (Lock(deviceData->lock) != LOCK_OK)
    {
        ASSERT_FAIL("unable to lock.");
    }
    else
    {
        ASSERT_IS_TRUE_WITH_MSG(deviceData->reportedAck, "Failure sending data to IotHub"); // was received by the callback...
        ASSERT_IS_TRUE_WITH_MSG(deviceData->cb_status < 300, "Reported Callback Status is error");
        (void)Unlock(deviceData->lock);
    }

    const char *connectionString = IoTHubAccount_GetIoTHubConnString(g_iothubAcctInfo);
    IOTHUB_SERVICE_CLIENT_AUTH_HANDLE iotHubServiceClientHandle = IoTHubServiceClientAuth_CreateFromConnectionString(connectionString);
    ASSERT_IS_NOT_NULL_WITH_MSG(iotHubServiceClientHandle, "Failure in IoTHubServiceClientAuth_CreateFromConnectionString");

    IOTHUB_SERVICE_CLIENT_DEVICE_TWIN_HANDLE serviceClientDeviceTwinHandle = IoTHubDeviceTwin_Create(iotHubServiceClientHandle);
    ASSERT_IS_NOT_NULL_WITH_MSG(serviceClientDeviceTwinHandle, "Failure to initialize a device twin service session");

    char* deviceTwinData = IoTHubDeviceTwin_GetTwin(serviceClientDeviceTwinHandle, iotHubConfig.deviceId);
    ASSERT_IS_NOT_NULL_WITH_MSG(deviceTwinData, "Failure to retrieve device data");

    JSON_Value *root_value = json_parse_string(deviceTwinData);
    ASSERT_IS_NOT_NULL_WITH_MSG(root_value, "Failure to parse data from service");

    JSON_Object *root_object = json_value_get_object(root_value);
    const char *string_property = json_object_dotget_string(root_object, "properties.reported.string_property");
    ASSERT_ARE_EQUAL_WITH_MSG(char_ptr, deviceData->device->string_property, string_property, "data retrieved does not equal data reported");

    int integer_property = (int) json_object_dotget_number(root_object, "properties.reported.integer_property");
    ASSERT_ARE_EQUAL_WITH_MSG(int, deviceData->device->integer_property, integer_property, "data retrieved does not equal data reported");

    // cleanup
    IoTHubClient_Destroy(iotHubClientHandle);
    free(deviceTwinData);
    json_value_free(root_value);
    IoTHubDeviceTwin_Destroy(serviceClientDeviceTwinHandle);
    IoTHubServiceClientAuth_Destroy(iotHubServiceClientHandle);
    device_data_delete(deviceData);
}

void dt_e2e_get_desired_test(IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol)
{
    (void) protocol;
}