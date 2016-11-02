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

#include "iothub_messaging_sample.h"

#include "iothub_service_client_auth.h"
#include "iothub_messaging.h"


/* String containing Hostname, SharedAccessKeyName and SharedAccessKey in the format:                       */
/* "HostName=<host_name>;SharedAccessKeyName=<shared_access_key_name>;SharedAccessKey=<shared_access_key>" */
static const char* connectionString = "[IoTHub Connection String]";
static const char* deviceId = "[Device Id]";

#define MESSAGE_COUNT 10

static void openCompleteCallback(void* context)
{
    (void)printf("Open completed, context: %s\n", (char*)context);
}

static void sendCompleteCallback(void* context, IOTHUB_MESSAGING_RESULT messagingResult)
{
    (void)context;
    if (messagingResult == IOTHUB_MESSAGING_OK)
    {
        (void)printf("Message has been sent successfully\n");
    }
    else
    {
        (void)printf("Send failed\n");
    }
}

static void feedbackReceivedCallback(void* context, IOTHUB_SERVICE_FEEDBACK_BATCH* feedbackBatch)
{
    printf("Feedback received, context: %s\n", (char*)context);

    if (feedbackBatch != NULL)
    {
        (void)printf("Batch userId       : %s\r\n", feedbackBatch->userId);
        (void)printf("Batch lockToken    : %s\r\n", feedbackBatch->lockToken);
        if (feedbackBatch->feedbackRecordList != NULL)
        {
            LIST_ITEM_HANDLE feedbackRecord = singlylinkedlist_get_head_item(feedbackBatch->feedbackRecordList);
            while (feedbackRecord != NULL)
            {
                IOTHUB_SERVICE_FEEDBACK_RECORD* feedback = (IOTHUB_SERVICE_FEEDBACK_RECORD*)singlylinkedlist_item_get_value(feedbackRecord);
                if (feedback != NULL)
                {
                    (void)printf("Feedback\r\n");
                    (void)printf("    deviceId        : %s\r\n", feedback->deviceId);
                    (void)printf("    generationId    : %s\r\n", feedback->generationId);
                    (void)printf("    correlationId   : %s\r\n", feedback->correlationId);
                    (void)printf("    description     : %s\r\n", feedback->description);
                    (void)printf("    enqueuedTimeUtc : %s\r\n", feedback->enqueuedTimeUtc);

                    feedbackRecord = singlylinkedlist_get_next_item(feedbackRecord);
                }
            }
        }
    }
}

static IOTHUB_SERVICE_CLIENT_AUTH_HANDLE iotHubServiceClientHandle;
static IOTHUB_MESSAGING_CLIENT_HANDLE iotHubMessagingHandle;
static IOTHUB_MESSAGING_RESULT iotHubMessagingResult;

void iothub_messaging_sample_run(void)
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
            iotHubMessagingHandle = IoTHubMessaging_Create(iotHubServiceClientHandle);
            if (iotHubMessagingHandle == NULL)
            {
                (void)printf("IoTHubMessaging_Create failed\n");
            }
            else
            {
                (void)printf("Messaging has been created successfully\n");
                (void)printf("Opening Messaging...\n");

                iotHubMessagingResult = IoTHubMessaging_SetFeedbackMessageCallback(iotHubMessagingHandle, feedbackReceivedCallback, "Context string for feedback");
                if (iotHubMessagingResult != IOTHUB_MESSAGING_OK)
                {
                    (void)printf("IoTHubMessaging_SetFeedbackMessageCallback failed\n");
                }
                else
                {
                    iotHubMessagingResult = IoTHubMessaging_Open(iotHubMessagingHandle, openCompleteCallback, "Context string for open");
                    if (iotHubMessagingResult != IOTHUB_MESSAGING_OK)
                    {
                        (void)printf("IoTHubMessaging_Open failed\n");
                    }
                    else
                    {
                        double avgWindSpeed = 10.0;
                        static char msgText[64];

                        for (int i = 0; i < MESSAGE_COUNT; i++)
                        {
                            sprintf_s(msgText, sizeof(msgText), "{\"deviceId\":%s,\"windSpeed\":%.2f, \"i\":%d}", deviceId, avgWindSpeed, i);
                            IOTHUB_MESSAGE_HANDLE messageHandle = IoTHubMessage_CreateFromByteArray((const unsigned char*)msgText, strlen(msgText));
                            if (messageHandle == NULL)
                            {
                                (void)printf("IoTHubMessage_CreateFromByteArray failed\n");
                            }
                            else
                            {
                                iotHubMessagingResult = IoTHubMessaging_SendAsync(iotHubMessagingHandle, deviceId, messageHandle, sendCompleteCallback, NULL);
                                if (iotHubMessagingResult != IOTHUB_MESSAGING_OK)
                                {
                                    (void)printf("IoTHubMessaging_SendAsync failed. Exiting...\n");
                                    IoTHubMessage_Destroy(messageHandle);
                                    break;
                                }
                                else
                                {
                                    ThreadAPI_Sleep(1000);
                                }
                                IoTHubMessage_Destroy(messageHandle);
                            }
                        }
                    }
                    /* Wait for Commands. */
                    (void)printf("Press any key to exit the application. \r\n");
                    (void)getchar();

                    IoTHubMessaging_Close(iotHubMessagingHandle);
                }
                (void)printf("Calling IoTHubmessaging_Destroy...\n");
                IoTHubMessaging_Destroy(iotHubMessagingHandle);
            }
            (void)printf("Calling IoTHubServiceClientAuth_Destroy...\n");
            IoTHubServiceClientAuth_Destroy(iotHubServiceClientHandle);
        }
        platform_deinit();
    }
}