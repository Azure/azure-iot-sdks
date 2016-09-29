// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// This file is under development and it is subject to change

#ifndef IOTHUB_MESSAGING_LL_H
#define IOTHUB_MESSAGING_LL_H

#ifdef __cplusplus
extern "C"
{
#else
#endif

#include "crt_abstractions.h"
#include "list.h"
#include "map.h"
#include "iothub_message.h"
#include "iothub_service_client_auth.h"

#define IOTHUB_FEEDBACK_STATUS_CODE_VALUES               \
    IOTHUB_FEEDBACK_STATUS_CODE_SUCCESS,                 \
    IOTHUB_FEEDBACK_STATUS_CODE_EXPIRED,                 \
    IOTHUB_FEEDBACK_STATUS_CODE_DELIVER_COUNT_EXCEEDED,  \
    IOTHUB_FEEDBACK_STATUS_CODE_REJECTED,                \
    IOTHUB_FEEDBACK_STATUS_CODE_UNKNOWN                  \

DEFINE_ENUM(IOTHUB_FEEDBACK_STATUS_CODE, IOTHUB_FEEDBACK_STATUS_CODE_VALUES);

#define IOTHUB_MESSAGE_SEND_STATE_VALUES                 \
    IOTHUB_MESSAGE_SEND_STATE_NOT_SENT,                  \
    IOTHUB_MESSAGE_SEND_STATE_SEND_IN_PROGRESS,          \
    IOTHUB_MESSAGE_SEND_STATE_SENT_OK,                   \
    IOTHUB_MESSAGE_SEND_STATE_SEND_FAILED                \

DEFINE_ENUM(IOTHUB_MESSAGE_SEND_STATE, IOTHUB_MESSAGE_SEND_STATE_VALUES);

#define IOTHUB_MESSAGING_RESULT_VALUES       \
    IOTHUB_MESSAGING_OK,                     \
    IOTHUB_MESSAGING_INVALID_ARG,            \
    IOTHUB_MESSAGING_ERROR,                  \
    IOTHUB_MESSAGING_INVALID_JSON,           \
    IOTHUB_MESSAGING_DEVICE_EXIST,           \
    IOTHUB_MESSAGING_CALLBACK_NOT_SET        \

DEFINE_ENUM(IOTHUB_MESSAGING_RESULT, IOTHUB_MESSAGING_RESULT_VALUES);

typedef struct IOTHUB_SERVICE_FEEDBACK_RECORD_TAG
{
    char* description;
    const char* deviceId;
    const char* correlationId;
    const char* generationId;
    const char* enqueuedTimeUtc;
    IOTHUB_FEEDBACK_STATUS_CODE statusCode;
    const char* originalMessageId;
} IOTHUB_SERVICE_FEEDBACK_RECORD;

typedef struct IOTHUB_SERVICE_FEEDBACK_BATCH_TAG
{
    const char* userId;
    const char* lockToken;
    LIST_HANDLE feedbackRecordList;
} IOTHUB_SERVICE_FEEDBACK_BATCH;

typedef struct IOTHUB_MESSAGING_TAG* IOTHUB_MESSAGING_HANDLE;

typedef void(*IOTHUB_OPEN_COMPLETE_CALLBACK)(void* context);
typedef void(*IOTHUB_SEND_COMPLETE_CALLBACK)(void* context, IOTHUB_MESSAGING_RESULT messagingResult);
typedef void(*IOTHUB_FEEDBACK_MESSAGE_RECEIVED_CALLBACK)(void* context, IOTHUB_SERVICE_FEEDBACK_BATCH* feedbackBatch);

extern IOTHUB_MESSAGING_HANDLE IoTHubMessaging_LL_Create(IOTHUB_SERVICE_CLIENT_AUTH_HANDLE serviceClientHandle);
extern void IoTHubMessaging_LL_Destroy(IOTHUB_MESSAGING_HANDLE messagingHandle);

extern IOTHUB_MESSAGING_RESULT IoTHubMessaging_LL_Open(IOTHUB_MESSAGING_HANDLE messagingHandle, IOTHUB_OPEN_COMPLETE_CALLBACK openCompleteCallback, void* userContextCallback);
extern void IoTHubMessaging_LL_Close(IOTHUB_MESSAGING_HANDLE messagingHandle);

extern IOTHUB_MESSAGING_RESULT IoTHubMessaging_LL_Send(IOTHUB_MESSAGING_HANDLE messagingHandle, const char* deviceId, IOTHUB_MESSAGE_HANDLE message, IOTHUB_SEND_COMPLETE_CALLBACK sendCompleteCallback, void* userContextCallback);

extern IOTHUB_MESSAGING_RESULT IoTHubMessaging_LL_SetFeedbackMessageCallback(IOTHUB_MESSAGING_HANDLE messagingHandle, IOTHUB_FEEDBACK_MESSAGE_RECEIVED_CALLBACK feedbackMessageReceivedCallback, void* userContextCallback);

extern void IoTHubMessaging_LL_DoWork(IOTHUB_MESSAGING_HANDLE messagingHandle);

#ifdef __cplusplus
}
#endif

#endif // IOTHUB_MESSAGING_LL_H
