// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <cstdlib>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "testrunnerswitcher.h"
#include "micromock.h"
#include "micromockcharstararenullterminatedstrings.h"

#include "iothub_account.h"
#include "iothubtest.h"

#include "serializer.h"
#include "azure_c_shared_utility/buffer_.h"
#include "azure_c_shared_utility/strings.h"
#include "azure_c_shared_utility/lock.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_c_shared_utility/threadapi.h"
#include "iothubtransportamqp.h"
#include "iothubtransporthttp.h"
#include "MacroE2EModelAction.h"
#include "iothub_client.h"

#include "azure_c_shared_utility/platform.h"

static MICROMOCK_GLOBAL_SEMAPHORE_HANDLE g_dllByDll;

/*the following time expressed in seconds denotes the maximum time to read all the events available in an event hub*/
#define MAX_DRAIN_TIME 100.0

/*the following time expressed in seconds denotes the maximum "cloud" travel time - the time from a moment some data reaches the cloud until that data is available for a consumer*/
#define MAX_CLOUD_TRAVEL_TIME 60.0
#define TIME_DATA_LENGTH    32

DEFINE_MICROMOCK_ENUM_TO_STRING(IOTHUB_TEST_CLIENT_RESULT, IOTHUB_TEST_CLIENT_RESULT_VALUES);
DEFINE_MICROMOCK_ENUM_TO_STRING(IOTHUB_MESSAGE_RESULT, IOTHUB_MESSAGE_RESULT_VALUES);
DEFINE_MICROMOCK_ENUM_TO_STRING(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_RESULT_VALUES);

static const char* TEST_SEND_DATA_FMT = "{\"ExampleData\": { \"SendDate\": \"%.24s\", \"UniqueId\":%d} }";
static const char* TEST_RECV_DATA_FMT = "{\\\"Name\\\": \\\"testaction\\\", \\\"Parameters\\\": { \\\"property1\\\": \\\"%.24s\\\", \\\"UniqueId\\\":%d}}";
static const char* TEST_CMP_DATA_FMT = "{\"Name\": \"testaction\", \"Parameters\": { \"property1\": \"%.24s\", \"UniqueId\":%d} }";
static const char* TEST_MACRO_CMP_DATA_FMT = "{\"UniqueId\":%d, \"property1\":\"%.24s\"}";
static const char* TEST_MACRO_RECV_DATA_FMT = "{\"Name\":\"dataMacroCallback\", \"Parameters\":{\"property1\":\"%.24s\", \"UniqueId\": %d}}";

static MICROMOCK_MUTEX_HANDLE g_testByTest = NULL;

static size_t g_uniqueTestId = 0;

typedef struct _tagEXPECTED_SEND_DATA
{
    const char* expectedString;
    bool wasFound;
    bool dataWasSent;
} EXPECTED_SEND_DATA;

typedef struct _tagEXPECTED_RECEIVE_DATA
{
    const char* toBeSend;
    size_t toBeSendSize;
    const char* compareData;
    size_t compareDataSize;
    bool wasFound;
} EXPECTED_RECEIVE_DATA;

static EXPECTED_RECEIVE_DATA* g_recvMacroData;

// This is a Static function called from the macro
EXECUTE_COMMAND_RESULT dataMacroCallback(deviceModel* device, ascii_char_ptr property1, int UniqueId)
{
    device;
    if (g_recvMacroData != NULL)
    {
        if ( (g_uniqueTestId == (size_t)UniqueId) && (strcmp(g_recvMacroData->compareData, property1) == 0) )
        {
            g_recvMacroData->wasFound = true;
        }
    }
    return EXECUTE_COMMAND_SUCCESS;
}

/*this function "links" IoTHub to the serialization library*/
static IOTHUBMESSAGE_DISPOSITION_RESULT IoTHubMessage(IOTHUB_MESSAGE_HANDLE message, void* userContextCallback)
{
    const unsigned char* buffer;
    size_t size;
    if (IoTHubMessage_GetByteArray(message, &buffer, &size) != IOTHUB_MESSAGE_OK)
    {
        ASSERT_FAIL("unable to IoTHubMessage_GetByteArray");
    }
    else
    {
        /*buffer is not zero terminated*/
        STRING_HANDLE temp = STRING_construct_n((char*)buffer, size);
        ASSERT_IS_NOT_NULL(temp);
        EXECUTE_COMMAND(userContextCallback, STRING_c_str(temp));
        STRING_delete(temp);
    }
    return IOTHUBMESSAGE_ACCEPTED;
}

/*AT NO TIME, CAN THERE BE 2 TRANSPORTS OVER OPENSSL IN THE SAME TIME.*/
/*If there are, nasty Linux bugs will happen*/
BEGIN_TEST_SUITE(serializer_e2e)

    static void iotHubMacroCallBack(IOTHUB_CLIENT_CONFIRMATION_RESULT result, void* userContextCallback)
    {
        (void)result;
        EXPECTED_SEND_DATA* expectedData = (EXPECTED_SEND_DATA*)userContextCallback;
        if (expectedData != NULL)
        {
            expectedData->dataWasSent = true;
        }
    }

    static int IoTHubCallback(void* context, const char* data, size_t size)
    {
        size;
        int result = 0; // 0 means "keep processing"
        EXPECTED_SEND_DATA* expectedData = (EXPECTED_SEND_DATA*)context;
        if (expectedData != NULL)
        {
            if (strcmp(expectedData->expectedString, data) == 0)
            {
                expectedData->wasFound = true;
                result = 1;
            }
        }
        return result;
    }

    /*by code convention, context for this function is dereferenceable to EXPECTED_DATA*/
    static void RecvCallback(const void* buffer, size_t size, void* receiveCallbackContext)
    {
        EXPECTED_RECEIVE_DATA* expectedData = (EXPECTED_RECEIVE_DATA*)receiveCallbackContext;
        if (size == expectedData->compareDataSize)
        {
            if (memcmp(buffer, expectedData->compareData, size) == 0)
            {
                expectedData->wasFound = true;
            }
        }
    }

    static EXPECTED_RECEIVE_DATA* RecvTestData_Create(void)
    {
        EXPECTED_RECEIVE_DATA* result;
        result = (EXPECTED_RECEIVE_DATA*)malloc(sizeof(EXPECTED_RECEIVE_DATA));
        if (result != NULL)
        {
            char temp[1000];
            char* tempString;
            result->wasFound = false;
            time_t t = time(NULL);
            (void)sprintf_s(temp, sizeof(temp), TEST_CMP_DATA_FMT, ctime(&t), g_uniqueTestId);
            result->compareDataSize = strlen(temp);
            tempString = (char*)malloc(result->compareDataSize+1);
            if (tempString == NULL)
            {
                free(result);
                result = NULL;
            }
            else
            {
                strcpy(tempString, temp);
                result->compareData = tempString;
                (void)sprintf_s(temp, sizeof(temp), TEST_RECV_DATA_FMT, ctime(&t), g_uniqueTestId);
                tempString = (char*)malloc(strlen(temp) + 1);
                if (tempString == NULL)
                {
                    free((void*)result->compareData);
                    free(result);
                    result = NULL;
                }
                else
                {
                    strcpy(tempString, temp);
                    result->toBeSendSize = strlen(tempString);
                    result->toBeSend = tempString;
                }
            }
        }
        return result;
    }

    static EXPECTED_RECEIVE_DATA* RecvMacroTestData_Create(void)
    {
        EXPECTED_RECEIVE_DATA* result;
        result = (EXPECTED_RECEIVE_DATA*)malloc(sizeof(EXPECTED_RECEIVE_DATA));
        if (result != NULL)
        {
            char* tempString;

            result->wasFound = false;

            tempString = (char*)malloc(TIME_DATA_LENGTH);
            if (tempString == NULL)
            {
                free(result);
                result = NULL;
            }
            else
            {
                time_t t = time(NULL);
                (void)sprintf_s(tempString, TIME_DATA_LENGTH, "%.24s", ctime(&t) );
                result->compareData = tempString;
                result->compareDataSize = strlen(result->compareData);
                size_t nLen = result->compareDataSize+strlen(TEST_MACRO_RECV_DATA_FMT)+4;
                tempString = (char*)malloc(nLen);
                if (tempString == NULL)
                {
                    free((void*)result->compareData);
                    free(result);
                    result = NULL;
                }
                else
                {
                    (void)sprintf_s(tempString, nLen, TEST_MACRO_RECV_DATA_FMT, result->compareData, g_uniqueTestId);
                    result->toBeSendSize = strlen(tempString);
                    result->toBeSend = tempString;
                }
            }
        }
        return result;
    }

    static void RecvTestData_Destroy(EXPECTED_RECEIVE_DATA* data)
    {
        if (data != NULL)
        {
            if (data->compareData != NULL)
            {
                free((void*)(data->compareData));
            }
            if (data->toBeSend != NULL)
            {
                free((void*)data->toBeSend);
            }
            free(data);
        }
    }

    static EXPECTED_SEND_DATA* SendTestData_Create(void)
    {
        EXPECTED_SEND_DATA* result = (EXPECTED_SEND_DATA*)malloc(sizeof(EXPECTED_SEND_DATA));
        if (result != NULL)
        {
            char temp[1000];
            char* tempString;
            time_t t = time(NULL);
            sprintf(temp, TEST_SEND_DATA_FMT, ctime(&t), g_uniqueTestId);
            if ((tempString = (char*)malloc(strlen(temp) + 1)) == NULL)
            {
                free(result);
                result = NULL;
            }
            else
            {
                strcpy(tempString, temp);
                result->expectedString = tempString;
                result->wasFound = false;
                result->dataWasSent = false;
            }
        }
        return result;
    }

    static EXPECTED_SEND_DATA* SendMacroTestData_Create(const char* pszTime)
    {
        EXPECTED_SEND_DATA* result = (EXPECTED_SEND_DATA*)malloc(sizeof(EXPECTED_SEND_DATA));
        if (result != NULL)
        {
            char temp[1000];
            char* tempString;
            sprintf(temp, TEST_MACRO_CMP_DATA_FMT, g_uniqueTestId, pszTime);
            if ((tempString = (char*)malloc(strlen(temp) + 1)) == NULL)
            {
                free(result);
                result = NULL;
            }
            else
            {
                strcpy(tempString, temp);
                result->expectedString = tempString;
                result->wasFound = false;
                result->dataWasSent = false;
            }
        }
        return result;
    }

    static void SendTestData_Destroy(EXPECTED_SEND_DATA* data)
    {
        if (data != NULL)
        {
            if (data->expectedString != NULL)
            {
                free((void*)data->expectedString);
            }
            free(data);
        }
    }

    TEST_SUITE_INITIALIZE(TestClassInitialize)
    {
        TEST_INITIALIZE_MEMORY_DEBUG(g_dllByDll);

        g_testByTest = MicroMockCreateMutex();
        ASSERT_IS_NOT_NULL(g_testByTest);

        ASSERT_ARE_EQUAL(int, 0, platform_init() );
        ASSERT_ARE_EQUAL(int, 0, serializer_init(NULL));

        g_uniqueTestId = 0;
    }

    TEST_SUITE_CLEANUP(TestClassCleanup)
    {
        platform_deinit();
        serializer_deinit();

        MicroMockDestroyMutex(g_testByTest);
        TEST_DEINITIALIZE_MEMORY_DEBUG(g_dllByDll);
    }

    TEST_FUNCTION_INITIALIZE(TestMethodInitialize)
    {
        if (!MicroMockAcquireMutex(g_testByTest))
        {
            ASSERT_FAIL("our mutex is ABANDONED. Failure in test framework");
        }

        g_uniqueTestId++;
    }

    TEST_FUNCTION_CLEANUP(TestMethodCleanup)
    {
        if (!MicroMockReleaseMutex(g_testByTest))
        {
            ASSERT_FAIL("failure in test framework at ReleaseMutex");
        }
    }

#if 0
    TEST_FUNCTION(IoTClient_AMQP_AmqpMacroRecv_e2e)
    {
        // arrange
        IOTHUB_CLIENT_CONFIG iotHubConfig = { 0 };
        IOTHUB_CLIENT_HANDLE iotHubClientHandle;

        // act
        iotHubConfig.iotHubName = IoTHubAccount_GetIoTHubName();
        iotHubConfig.iotHubSuffix = IoTHubAccount_GetIoTHubSuffix();
        iotHubConfig.deviceId = IoTHubAccount_GetDeviceId();
        iotHubConfig.deviceKey = IoTHubAccount_GetDeviceKey();
        iotHubConfig.protocol = AMQP_Protocol;

        //step 1: data is retrieved by device using AMQP
        time_t beginOperation, nowTime;

        // step 2: data is created
        g_recvMacroData = RecvMacroTestData_Create();
        ASSERT_IS_NOT_NULL(g_recvMacroData);

        // step 3: data is pushed to the topic/subscription
        IOTHUB_TEST_HANDLE devhubTestHandle = IoTHubTest_Initialize(IoTHubAccount_GetEventHubConnectionString(), IoTHubAccount_GetIoTHubConnString(), IoTHubAccount_GetDeviceId(), IoTHubAccount_GetDeviceKey(), IoTHubAccount_GetEventhubListenName(), IoTHubAccount_GetEventhubAccessKey(), IoTHubAccount_GetSharedAccessSignature(), IoTHubAccount_GetEventhubConsumerGroup() );
        ASSERT_IS_NOT_NULL(devhubTestHandle);

        IOTHUB_TEST_CLIENT_RESULT dhTestResult = IoTHubTest_SendMessage(devhubTestHandle, (const unsigned char*)g_recvMacroData->toBeSend, g_recvMacroData->toBeSendSize);
        ASSERT_ARE_EQUAL(IOTHUB_TEST_CLIENT_RESULT, IOTHUB_TEST_CLIENT_OK, dhTestResult);

        IoTHubTest_Deinit(devhubTestHandle);

        iotHubClientHandle = IoTHubClient_Create(&iotHubConfig);
        ASSERT_IS_NOT_NULL(iotHubClientHandle);

        deviceModel* devModel = CREATE_MODEL_INSTANCE(MacroE2EModelAction, deviceModel);
        ASSERT_IS_NOT_NULL(devModel);

        auto setMessageResult = IoTHubClient_SetMessageCallback(iotHubClientHandle, IoTHubMessage, devModel);
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_OK, setMessageResult);

        beginOperation = time(NULL);
        while (
              (
                  (nowTime = time(NULL)),
                  (difftime(nowTime, beginOperation) < MAX_CLOUD_TRAVEL_TIME) //time box
              ) &&
                  (!g_recvMacroData->wasFound) //condition box
              )
        {
            ThreadAPI_Sleep(100);
        }

        // assert
        ASSERT_IS_TRUE(g_recvMacroData->wasFound);

        ///cleanup
        DESTROY_MODEL_INSTANCE(devModel);
        RecvTestData_Destroy(g_recvMacroData);
        IoTHubClient_Destroy(iotHubClientHandle);
    }

    TEST_FUNCTION(IoTClient_AMQP_MacroSend_e2e)
    {
        // arrange
        /*IOTHUB_CLIENT_CONFIG iotHubConfig = { 0 };
        IOTHUB_CLIENT_HANDLE iotHubClientHandle;
        deviceModel* devModel;
        time_t beginOperation, nowTime;

        iotHubConfig.iotHubName = IoTHubAccount_GetIoTHubName();
        iotHubConfig.iotHubSuffix = IoTHubAccount_GetIoTHubSuffix();
        iotHubConfig.deviceId = IoTHubAccount_GetDeviceId();
        iotHubConfig.deviceKey = IoTHubAccount_GetDeviceKey();
        iotHubConfig.protocol = AMQP_Protocol;

        // step 1: prepare data
        time_t t = time(NULL);
        char sztimeText[64];
        (void)sprintf_s(sztimeText, sizeof(sztimeText), "%.24s", ctime(&t));

        EXPECTED_SEND_DATA* expectedData = SendMacroTestData_Create(sztimeText);
        ASSERT_IS_NOT_NULL(expectedData);

        /// act
        // step 2: send data with AMQP
        {
            iotHubClientHandle = IoTHubClient_Create(&iotHubConfig);
            ASSERT_IS_NOT_NULL(iotHubClientHandle);

            devModel = CREATE_MODEL_INSTANCE(MacroE2EModelAction, deviceModel);
            ASSERT_IS_NOT_NULL(devModel);

            devModel->property1 = sztimeText;
            devModel->UniqueId = g_uniqueTestId;
            unsigned char* destination;
            size_t destinationSize;
            IOT_AGENT_RESULT nResult = SERIALIZE(&destination, &destinationSize, *devModel);
            ASSERT_ARE_EQUAL(int, IOT_AGENT_OK, nResult);
            auto iothubMessageHandle = IoTHubMessage_CreateFromByteArray(destination, destinationSize);
            ASSERT_IS_NOT_NULL(iothubMessageHandle);
            free(destination);

            auto iothubClientResult = IoTHubClient_SendEventAsync(iotHubClientHandle, iothubMessageHandle, iotHubMacroCallBack, expectedData);
            
            IoTHubMessage_Destroy(iothubMessageHandle);
            // Wait til the data gets sent to the callback
            beginOperation = time(NULL);
            while (
                  (
                      (nowTime = time(NULL)),
                      (difftime(nowTime, beginOperation) < MAX_CLOUD_TRAVEL_TIME) //time box
                  ) &&
                      (!expectedData->dataWasSent)
                  ) //condition box
            {
                ThreadAPI_Sleep(100);
            }
        }
        ASSERT_IS_TRUE(expectedData->dataWasSent); 

        ///assert
        //step3: get the data from the other side
        {
            IOTHUB_TEST_HANDLE devhubTestHandle = IoTHubTest_Initialize(IoTHubAccount_GetEventHubConnectionString(), IoTHubAccount_GetIoTHubConnString(), IoTHubAccount_GetDeviceId(), IoTHubAccount_GetDeviceKey(), IoTHubAccount_GetEventhubListenName(), IoTHubAccount_GetEventhubAccessKey(), IoTHubAccount_GetSharedAccessSignature(), IoTHubAccount_GetEventhubConsumerGroup() );
            ASSERT_IS_NOT_NULL(devhubTestHandle);

            IOTHUB_TEST_CLIENT_RESULT result = IoTHubTest_ListenForEventForMaxDrainTime(devhubTestHandle, IoTHubCallback, IoTHubAccount_GetIoTHubPartitionCount(), expectedData);
            ASSERT_ARE_EQUAL(IOTHUB_TEST_CLIENT_RESULT, IOTHUB_TEST_CLIENT_OK, result);

            IoTHubTest_Deinit(devhubTestHandle);
        }

        // Sent Send is Async we need to make sure all the Data has been sent
        beginOperation = time(NULL);
        while (
                (
                    (nowTime = time(NULL)),
                    (difftime(nowTime, beginOperation) < MAX_CLOUD_TRAVEL_TIME) //time box
                ) &&
                    (!expectedData->wasFound)
                ) //condition box
        {
            ThreadAPI_Sleep(100);
        }

        ASSERT_IS_TRUE(expectedData->wasFound); // was found is written by the callback...

        ///cleanup 
        DESTROY_MODEL_INSTANCE(devModel);
        IoTHubClient_Destroy(iotHubClientHandle);
        SendTestData_Destroy(expectedData); //cleanup*/
    }

    TEST_FUNCTION(IoTClient_Http_MacroRecv_e2e)
    {
        IOTHUB_CLIENT_CONFIG iotHubConfig = { 0 };
        IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle;
        deviceModel* devModel;

        iotHubConfig.iotHubName = IoTHubAccount_GetIoTHubName();
        iotHubConfig.iotHubSuffix = IoTHubAccount_GetIoTHubSuffix();
        iotHubConfig.deviceId = IoTHubAccount_GetDeviceId();
        iotHubConfig.deviceKey = IoTHubAccount_GetDeviceKey();
        iotHubConfig.protocol = HTTP_Protocol;

        //step 1: data is created
        g_recvMacroData = RecvMacroTestData_Create();
        ASSERT_IS_NOT_NULL(g_recvMacroData);

        //step 2: data is pushed to the topic/subscription
        {
            IOTHUB_TEST_HANDLE devhubTestHandle = IoTHubTest_Initialize(IoTHubAccount_GetEventHubConnectionString(), IoTHubAccount_GetIoTHubConnString(), IoTHubAccount_GetDeviceId(), IoTHubAccount_GetDeviceKey(), IoTHubAccount_GetEventhubListenName(), IoTHubAccount_GetEventhubAccessKey(), IoTHubAccount_GetSharedAccessSignature(), IoTHubAccount_GetEventhubConsumerGroup() );
            ASSERT_IS_NOT_NULL(devhubTestHandle);

            IOTHUB_TEST_CLIENT_RESULT dhTestResult = IoTHubTest_SendMessage(devhubTestHandle, (const unsigned char*)g_recvMacroData->toBeSend, g_recvMacroData->toBeSendSize);
            ASSERT_ARE_EQUAL(IOTHUB_TEST_CLIENT_RESULT, IOTHUB_TEST_CLIENT_OK, dhTestResult);

            IoTHubTest_Deinit(devhubTestHandle);
        }

        // act

        //step 3: data is retrieved by HTTP
        {
            time_t beginOperation, nowTime;

            iotHubClientHandle = IoTHubClient_LL_Create(&iotHubConfig);
            ASSERT_IS_NOT_NULL(iotHubClientHandle);

            unsigned int minimumPollingTime = 0; /*because it should not wait*/
            if (IoTHubClient_LL_SetOption(iotHubClientHandle, "MinimumPollingTime", &minimumPollingTime) != IOTHUB_CLIENT_OK)
            {
                printf("failure to set option \"MinimumPollingTime\"\r\n");
            }

            devModel = CREATE_MODEL_INSTANCE(MacroE2EModelAction, deviceModel);
            ASSERT_IS_NOT_NULL(devModel);

            auto setMessageResult = IoTHubClient_LL_SetMessageCallback(iotHubClientHandle, IoTHubMessage, devModel);
            ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_OK, setMessageResult);

            beginOperation = time(NULL);
            while (
                  (
                    (nowTime = time(NULL)),
                    (difftime(nowTime, beginOperation) < MAX_CLOUD_TRAVEL_TIME) //time box
                  ) &&
                    (!g_recvMacroData->wasFound) //condition box
                  )
            {
                //just go on;
                IoTHubClient_LL_DoWork(iotHubClientHandle);
                ThreadAPI_Sleep(100);
            }
        }

        // assert
        ASSERT_IS_TRUE(g_recvMacroData->wasFound);

        ///cleanup
        DESTROY_MODEL_INSTANCE(devModel);
        IoTHubClient_LL_Destroy(iotHubClientHandle);
        RecvTestData_Destroy(g_recvMacroData);
    }

    TEST_FUNCTION(IoTClient_Http_MacroSend_e2e)
    {
        // arrange
        /*IOTHUB_CLIENT_CONFIG iotHubConfig = { 0 };
        IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle;
        deviceModel* devModel;
        time_t beginOperation, nowTime;

        iotHubConfig.iotHubName = IoTHubAccount_GetIoTHubName();
        iotHubConfig.iotHubSuffix = IoTHubAccount_GetIoTHubSuffix();
        iotHubConfig.deviceId = IoTHubAccount_GetDeviceId();
        iotHubConfig.deviceKey = IoTHubAccount_GetDeviceKey();
        iotHubConfig.protocol = HTTP_Protocol;

        // step 1: prepare data
        time_t t = time(NULL);
        char sztimeText[64];
        (void)sprintf_s(sztimeText, sizeof(sztimeText), "%.24s", ctime(&t));

        EXPECTED_SEND_DATA* expectedData = SendMacroTestData_Create(sztimeText);
        ASSERT_IS_NOT_NULL(expectedData);

        ///act
        // step 2: send data with HTTP
        {
            iotHubClientHandle = IoTHubClient_LL_Create(&iotHubConfig);
            ASSERT_IS_NOT_NULL(iotHubClientHandle);

            devModel = CREATE_MODEL_INSTANCE(MacroE2EModelAction, deviceModel);
            ASSERT_IS_NOT_NULL(devModel);

            devModel->property1 = sztimeText;
            devModel->UniqueId = g_uniqueTestId;
            unsigned char* destination;
            size_t destinationSize;
            IOT_AGENT_RESULT nResult = SERIALIZE(&destination, &destinationSize, *devModel);
            ASSERT_ARE_EQUAL(int, IOT_AGENT_OK, nResult);
            auto iothubMessageHandle = IoTHubMessage_CreateFromByteArray(destination, destinationSize);
            ASSERT_IS_NOT_NULL(iothubMessageHandle);
            free(destination);

            auto iothubClientResult = IoTHubClient_LL_SendEventAsync(iotHubClientHandle, iothubMessageHandle, iotHubMacroCallBack, expectedData);

            IoTHubMessage_Destroy(iothubMessageHandle);

            // Make sure all the Data has been sent
            beginOperation = time(NULL);
            while (
                  (
                      (nowTime = time(NULL)),
                      (difftime(nowTime, beginOperation) < MAX_CLOUD_TRAVEL_TIME) //time box
                  ) &&
                      (!expectedData->dataWasSent)
                  ) //condition box
            {
                //just go on;
                IoTHubClient_LL_DoWork(iotHubClientHandle);
                ThreadAPI_Sleep(100);
            }
        }
        ASSERT_IS_TRUE(expectedData->dataWasSent); 

        ///assert
        //step3: get the data from the other side
        {
            IOTHUB_TEST_HANDLE devhubTestHandle = IoTHubTest_Initialize(IoTHubAccount_GetEventHubConnectionString(), IoTHubAccount_GetIoTHubConnString(), IoTHubAccount_GetDeviceId(), IoTHubAccount_GetDeviceKey(), IoTHubAccount_GetEventhubListenName(), IoTHubAccount_GetEventhubAccessKey(), IoTHubAccount_GetSharedAccessSignature(), IoTHubAccount_GetEventhubConsumerGroup() );
            ASSERT_IS_NOT_NULL(devhubTestHandle);

            IOTHUB_TEST_CLIENT_RESULT result = IoTHubTest_ListenForEventForMaxDrainTime(devhubTestHandle, IoTHubCallback, IoTHubAccount_GetIoTHubPartitionCount(), expectedData);
            ASSERT_ARE_EQUAL(IOTHUB_TEST_CLIENT_RESULT, IOTHUB_TEST_CLIENT_OK, result);

            IoTHubTest_Deinit(devhubTestHandle);
        }

        ASSERT_IS_TRUE(expectedData->wasFound); // was found is written by the callback...

        ///cleanup 
        DESTROY_MODEL_INSTANCE(devModel);
        IoTHubClient_LL_Destroy(iotHubClientHandle);
        SendTestData_Destroy(expectedData); //cleanup*/
    }
#endif
END_TEST_SUITE(serializer_e2e)
