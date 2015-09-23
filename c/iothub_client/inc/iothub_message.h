// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef IOTHUB_MESSAGE_H
#define IOTHUB_MESSAGE_H

#include "macro_utils.h"
#include "map.h" 

#ifdef __cplusplus
#include <cstddef>
extern "C" 
{
#else
#include <stddef.h>
#endif

#define IOTHUB_MESSAGE_RESULT_VALUES         \
    IOTHUB_MESSAGE_OK,                       \
    IOTHUB_MESSAGE_INVALID_ARG,              \
    IOTHUB_MESSAGE_INVALID_TYPE,             \
    IOTHUB_MESSAGE_ERROR                     \

DEFINE_ENUM(IOTHUB_MESSAGE_RESULT, IOTHUB_MESSAGE_RESULT_VALUES);

#define IOTHUBMESSAGE_CONTENT_TYPE_VALUES \
IOTHUBMESSAGE_BYTEARRAY, \
IOTHUBMESSAGE_STRING, \
IOTHUBMESSAGE_UNKNOWN \

DEFINE_ENUM(IOTHUBMESSAGE_CONTENT_TYPE, IOTHUBMESSAGE_CONTENT_TYPE_VALUES);

typedef void* IOTHUB_MESSAGE_HANDLE;

extern IOTHUB_MESSAGE_HANDLE IoTHubMessage_CreateFromByteArray(const unsigned char* byteArray, size_t size);
extern IOTHUB_MESSAGE_HANDLE IoTHubMessage_CreateFromString(const char* source);

extern IOTHUB_MESSAGE_HANDLE IoTHubMessage_Clone(IOTHUB_MESSAGE_HANDLE iotHubMessageHandle);

extern IOTHUB_MESSAGE_RESULT IoTHubMessage_GetByteArray(IOTHUB_MESSAGE_HANDLE iotHubMessageHandle, const unsigned char** buffer, size_t* size);
extern const char* IoTHubMessage_GetString(IOTHUB_MESSAGE_HANDLE iotHubMessageHandle);
extern IOTHUBMESSAGE_CONTENT_TYPE IoTHubMessage_GetContentType(IOTHUB_MESSAGE_HANDLE iotHubMessageHandle);
extern MAP_HANDLE IoTHubMessage_Properties(IOTHUB_MESSAGE_HANDLE iotHubMessageHandle);

extern void IoTHubMessage_Destroy(IOTHUB_MESSAGE_HANDLE iotHubMessageHandle);

#ifdef __cplusplus
}
#endif

#endif /* IOTHUB_MESSAGE_H */
