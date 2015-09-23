// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef IOTDEVICE_H
#define IOTDEVICE_H

#include <stdbool.h>
#include "macro_utils.h"
#include "schema.h"
#include "datapublisher.h"
#include "commanddecoder.h"

#ifdef __cplusplus
#include <cstddef>
extern "C" {
#else
#include <stddef.h>
#endif

#define DEVICE_RESULT_VALUES			\
    DEVICE_OK,							\
    DEVICE_INVALID_ARG,					\
    DEVICE_DATA_PUBLISHER_FAILED,		\
    DEVICE_COMMAND_DECODER_FAILED,		\
    DEVICE_ERROR

DEFINE_ENUM(DEVICE_RESULT, DEVICE_RESULT_VALUES)

typedef void* DEVICE_HANDLE;
typedef EXECUTE_COMMAND_RESULT (*pPfDeviceActionCallback)(DEVICE_HANDLE deviceHandle, void* callbackUserContext, const char* relativeActionPath, const char* actionName, size_t argCount, const AGENT_DATA_TYPE* args);

extern DEVICE_RESULT Device_Create(SCHEMA_MODEL_TYPE_HANDLE modelHandle, pPfDeviceActionCallback deviceActionCallback, void* callbackUserContext, bool includePropertyPath, DEVICE_HANDLE* deviceHandle);
extern void Device_Destroy(DEVICE_HANDLE deviceHandle);

extern TRANSACTION_HANDLE Device_StartTransaction(DEVICE_HANDLE deviceHandle);
extern DEVICE_RESULT Device_PublishTransacted(TRANSACTION_HANDLE transactionHandle, const char* propertyPath, const AGENT_DATA_TYPE* data);
extern DEVICE_RESULT Device_EndTransaction(TRANSACTION_HANDLE transactionHandle, unsigned char** destination, size_t* destinationSize);
extern DEVICE_RESULT Device_CancelTransaction(TRANSACTION_HANDLE transactionHandle);

extern EXECUTE_COMMAND_RESULT Device_ExecuteCommand(DEVICE_HANDLE deviceHandle, const char* command);
#ifdef __cplusplus
}
#endif

#endif /* IOTDEVICE_H */

