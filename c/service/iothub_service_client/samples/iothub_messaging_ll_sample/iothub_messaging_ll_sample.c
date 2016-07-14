// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_c_shared_utility/threadapi.h"
#include "azure_c_shared_utility/map.h"
#include "azure_c_shared_utility/lock.h"
#include "azure_c_shared_utility/strings.h"
#include "azure_c_shared_utility/string_tokenizer.h"
#include "azure_c_shared_utility/platform.h"
#include "azure_c_shared_utility/consolelogger.h"
#include "azure_c_shared_utility/xlogging.h"

#include "iothub_messaging_ll_sample.h"

#include "connection_string_parser.h"
#include "iothub_service_client_auth.h"
#include "iothub_messaging_ll.h"
#include "iothub_message.h"


/* String containing Hostname, SharedAccessKeyName and SharedAccessKey in the format:                       */
/* "HostName=<host_name>;SharedAccessKeyName=<shared_access_key_name>;SharedAccessKey=<shared_access_key>" */
static const char* connectionString = "[IoTHub Connection String]";
static const char* deviceId = "[Device Id]";

#define MESSAGE_COUNT 10

void openCompleteCallback(void* context)
{
    (void)printf("Open completed, context: %s\n", (char*)context);
}

void sendCompleteCallback(void* context, IOTHUB_MESSAGING_RESULT messagingResult)
{
    if (messagingResult == IOTHUB_MESSAGING_OK)
    {
        (void)printf("Message %d has been sent successfully\n", *(int*)(context));
    }
    else
    {
        (void)printf("Send failed\n");
    }
}

void feedbackReceivedCallback(void* context, IOTHUB_SERVICE_FEEDBACK_BATCH* feedbackBatch)
{
    printf("Feedback received, context: %s\n", (char*)context);

    if (feedbackBatch != NULL)
    {
        (void)printf("Batch userId       : %s\r\n", feedbackBatch->userId);
        (void)printf("Batch lockToken    : %s\r\n", feedbackBatch->lockToken);
        if (feedbackBatch->feedbackRecordList != NULL)
        {
            LIST_ITEM_HANDLE feedbackRecord = list_get_head_item(feedbackBatch->feedbackRecordList);
            while (feedbackRecord != NULL)
            {
                IOTHUB_SERVICE_FEEDBACK_RECORD* feedback = (IOTHUB_SERVICE_FEEDBACK_RECORD*)list_item_get_value(feedbackRecord);
                if (feedback != NULL)
                {
                    (void)printf("Feedback\r\n");
                    (void)printf("    deviceId        : %s\r\n", feedback->deviceId);
                    (void)printf("    generationId    : %s\r\n", feedback->generationId);
                    (void)printf("    correlationId   : %s\r\n", feedback->correlationId);
                    (void)printf("    description     : %s\r\n", feedback->description);
                    (void)printf("    enqueuedTimeUtc : %s\r\n", feedback->enqueuedTimeUtc);

                    feedbackRecord = list_get_next_item(feedbackRecord);
                }
            }
        }
    }
}

IOTHUB_SERVICE_CLIENT_AUTH_HANDLE iotHubServiceClientHandle;
IOTHUB_MESSAGING_HANDLE iotHubMessagingHandle;
IOTHUB_MESSAGING_RESULT iotHubMessagingResult;

void iothub_messaging_ll_sample_run(void)
{
    xlogging_set_log_function(consolelogger_log);

    if (platform_init() != 0)
    {
        (void)printf("Failed to initialize the platform.\r\n");
    }
    else
    {
        (void)printf("Calling IoTHubServiceClientAuth_CreateFromConnectionString with the connection string\n");
        iotHubServiceClientHandle = IoTHubServiceClientAuth_CreateFromConnectionString(connectionString);
        if (iotHubServiceClientHandle == NULL)
        {
            (void)printf("IoTHubServiceClientAuth_CreateFromConnectionString failed\n");
        }
        else
        {
            (void)printf("iotHubServiceClientHandle has been created successfully\n");

            (void)printf("Creating Messaging...\n");
            iotHubMessagingHandle = IoTHubMessaging_LL_Create(iotHubServiceClientHandle);

            if (iotHubMessagingHandle != NULL)
            {
                (void)printf("Messaging has been created successfully\n");
                (void)printf("Opening Messaging...\n");

                iotHubMessagingResult = IoTHubMessaging_LL_SetFeedbackMessageCallback(iotHubMessagingHandle, feedbackReceivedCallback, "Context string for feedback");

                iotHubMessagingResult = IoTHubMessaging_LL_Open(iotHubMessagingHandle, openCompleteCallback, "Context string for open");
                if (iotHubMessagingResult == IOTHUB_MESSAGING_OK)
                {
                    for (int i = 0; i < MESSAGE_COUNT; i++)
                    {
                        double avgWindSpeed = 10.0;
                        static char msgText[1024];
                        sprintf_s(msgText, sizeof(msgText), "{\"deviceId\":%s,\"windSpeed\":%.2f}", deviceId, avgWindSpeed + (rand() % 4 + 2));
                        IOTHUB_MESSAGE_HANDLE messageHandle = IoTHubMessage_CreateFromByteArray((const unsigned char*)msgText, strlen(msgText));

                        iotHubMessagingResult = IoTHubMessaging_LL_Send(iotHubMessagingHandle, deviceId, messageHandle, sendCompleteCallback, (void*)&i);
                        if (iotHubMessagingResult == IOTHUB_MESSAGING_OK)
                        {
                            int j;
                            for (j = 0; j < 100; j++)
                            {
                                IoTHubMessaging_LL_DoWork(iotHubMessagingHandle);
                                ThreadAPI_Sleep(50);
                            }
                        }
                    }
                }
                IoTHubMessaging_LL_Close(iotHubMessagingHandle);
            }
            (void)printf("Calling IoTHubMessaging_LL_Destroy...\n");
            IoTHubMessaging_LL_Destroy(iotHubMessagingHandle);

            (void)printf("Calling IoTHubServiceClientAuth_Destroy...\n");
            IoTHubServiceClientAuth_Destroy(iotHubServiceClientHandle);
        }
        platform_deinit();
    }

    /* Wait for Commands. */
    (void)printf("Press any key to exit the application. \r\n");
    (void)getchar();
}