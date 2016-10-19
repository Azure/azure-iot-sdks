// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <cstdlib>
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

#include "parson.h"
#include "../../../certs/certs.h"

static bool g_callbackRecv = false;

static size_t g_iotHubTestId = 0;
static IOTHUB_ACCOUNT_INFO_HANDLE g_iothubAcctInfo = NULL;

#define MAX_CLOUD_TRAVEL_TIME        60.0
#define TEST_STRING_VALUE             "test_string_value"

BEGIN_NAMESPACE(Contoso);

DECLARE_MODEL(thingie_t,
    WITH_REPORTED_PROPERTY(ascii_char_ptr, string_prop)
);

END_NAMESPACE(Contoso);


//TEST_DEFINE_ENUM_TYPE(IOTHUB_TEST_CLIENT_RESULT, IOTHUB_TEST_CLIENT_RESULT_VALUES);
//TEST_DEFINE_ENUM_TYPE(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_RESULT_VALUES);
TEST_DEFINE_ENUM_TYPE(SERIALIZER_RESULT, SERIALIZER_RESULT_VALUES);

typedef struct DEVICE_DATA_TAG
{
    thingie_t *device;
    bool reportedAck;   // true when device callback has been called
    int  cb_status;     // status reported by the callback
    LOCK_HANDLE lock;
} DEVICE_DATA;

/*this function wait for 30 seconds only before 31.10.2016*/
static void wait30seconds(void)
{
    struct tm ten_31_2016;
    ten_31_2016.tm_year = 2016 - 1900;
    ten_31_2016.tm_mon = 9;
    ten_31_2016.tm_mday = 31;
    ten_31_2016.tm_hour = 0;
    ten_31_2016.tm_min = 0;
    ten_31_2016.tm_sec = 0;
    ten_31_2016.tm_isdst = -1;
    time_t deadline = mktime(&ten_31_2016);
    if (deadline == (time_t)-1)
    {
        LogError("mktime failed");
    }
    else
    {
        time_t now = time(NULL);
        if (difftime(deadline, now) > 0)
        {
            /*sleep 30 seconds*/
            LogInfo("sleeping for 30 seconds");
            ThreadAPI_Sleep(30 * 1000);
            LogInfo("done sleeping!!!");
        }
    }
}

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

static DEVICE_DATA *device_data_new(void)
{
    DEVICE_DATA *retValue = (DEVICE_DATA *) malloc(sizeof(DEVICE_DATA));

    if (retValue != NULL)
    {
        retValue->device = CREATE_MODEL_INSTANCE(Contoso, thingie_t);
        if (retValue->device == NULL)
        {
            free(retValue);
            retValue = NULL;
        }
        else
        {
            if (mallocAndStrcpy_s(&(retValue->device->string_prop), TEST_STRING_VALUE) != 0)
            {
                DESTROY_MODEL_INSTANCE(retValue->device);
                free(retValue);
                retValue = NULL;
            }
            else
            {
                retValue->lock = Lock_Init();
                if (retValue->lock == NULL)
                {
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
        DESTROY_MODEL_INSTANCE(deviceData->device);
        free(deviceData);
    }
}

extern "C" void dt_e2e_init(void)
{
    int result = platform_init();
    ASSERT_ARE_EQUAL_WITH_MSG(int, 0, result, "Platform init failed");
    g_iothubAcctInfo = IoTHubAccount_Init(true);
    ASSERT_IS_NOT_NULL_WITH_MSG(g_iothubAcctInfo, "Could not initialize IoTHubAccount");
    platform_init();
    SERIALIZER_RESULT sResult = serializer_init(NULL);
    ASSERT_ARE_EQUAL_WITH_MSG(SERIALIZER_RESULT, SERIALIZER_OK, sResult, "serrializer initialize failed");
}

extern "C" void dt_e2e_deinit(void)
{
    IoTHubAccount_deinit(g_iothubAcctInfo);
    // Need a double deinit
    platform_deinit();
    platform_deinit();
}

extern "C" void dt_e2e_send_reported_test(IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol)
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
    ASSERT_IS_NOT_NULL_WITH_MSG(deviceData, "Could not create the device data");

    // Send the Event
    {
        // Create the IoT Hub Data
        iotHubClientHandle = IoTHubClient_Create(&iotHubConfig);
        ASSERT_IS_NOT_NULL_WITH_MSG(iotHubClientHandle, "Could not create IoTHubClient");

    // Turn on Log 
        bool trace = true;
        (void)IoTHubClient_SetOption(iotHubClientHandle, OPTION_LOG_TRACE, &trace);
        (void)IoTHubClient_SetOption(iotHubClientHandle, "TrustedCerts", certificates);

        // generate the data
        unsigned char *buffer;
        size_t         bufferSize;
        int result = SERIALIZE_REPORTED_PROPERTIES(&buffer, &bufferSize, *(deviceData->device));
        ASSERT_ARE_EQUAL_WITH_MSG(int, CODEFIRST_OK, result, "Serializing reported data failed");

        // act
        result = IoTHubClient_SendReportedState(iotHubClientHandle, buffer, bufferSize, DeviceTwinCallback, deviceData);
        ASSERT_ARE_EQUAL_WITH_MSG(int, IOTHUB_CLIENT_OK, result, "Setting reported data failed");
    }

    wait30seconds();

    time_t beginOperation, nowTime;
    beginOperation = time(NULL);
    while (
        (nowTime = time(NULL)),
        (difftime(nowTime, beginOperation) < MAX_CLOUD_TRAVEL_TIME) // time box
        )
    {
        if (Lock(deviceData->lock) != LOCK_OK)
        {
            ASSERT_FAIL("unable to lock");
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
        ThreadAPI_Sleep(100);
    }

    if (Lock(deviceData->lock) != LOCK_OK)
    {
        ASSERT_FAIL("unable to lock");
    }
    else
    {
        ASSERT_IS_TRUE_WITH_MSG(deviceData->reportedAck, "Failure sending data to IotHub"); // was received by the callback...
        ASSERT_ARE_EQUAL(int, 204, deviceData->cb_status, "Reported Callback Status is error");
        (void)Unlock(deviceData->lock);
    }

    // cleanup
    IoTHubClient_Destroy(iotHubClientHandle);
    device_data_delete(deviceData);

    const char *connectionString = IoTHubAccount_GetEventHubConnectionString(g_iothubAcctInfo);
    IOTHUB_SERVICE_CLIENT_AUTH_HANDLE iotHubServiceClientHandle = IoTHubServiceClientAuth_CreateFromConnectionString(connectionString);
    ASSERT_IS_NOT_NULL_WITH_MSG(iotHubServiceClientHandle, "Failure to connect to service");

    IOTHUB_SERVICE_CLIENT_DEVICE_TWIN_HANDLE serviceClientDeviceTwinHandle = IoTHubDeviceTwin_Create(iotHubServiceClientHandle);
    ASSERT_IS_NOT_NULL_WITH_MSG(serviceClientDeviceTwinHandle, "Failure to initialize a device twin service session");

    char* deviceTwinData = IoTHubDeviceTwin_GetTwin(serviceClientDeviceTwinHandle, iotHubConfig.deviceId);
    ASSERT_IS_NOT_NULL_WITH_MSG(deviceTwinData, "Failure to retrieve device data");

    JSON_Value *root_value = json_parse_string(deviceTwinData);
    ASSERT_IS_NOT_NULL_WITH_MSG(root_value, "Failure to parse data from service");
    free(deviceTwinData);

    JSON_Object *root_object = json_value_get_object(root_value);
    const char *data = json_object_dotget_string(root_object, "properties.reported.string_prop");
    ASSERT_IS_TRUE_WITH_MSG(strcmp(data, TEST_STRING_VALUE) == 0, "data retrieved does not equal data reported");

    json_value_free(root_value);
}

