// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef DATA_PUBLISHER_H
#define DATA_PUBLISHER_H

#include "agenttypesystem.h"
#include "schema.h"
/* Normally we could include <stdbool> for cpp, but some toolchains are not well behaved and simply don't have it - ARM CC for example */
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DATA_PUBLISHER_RESULT_VALUES    \
DATA_PUBLISHER_OK,                      \
DATA_PUBLISHER_INVALID_ARG,             \
DATA_PUBLISHER_MARSHALLER_ERROR,        \
DATA_PUBLISHER_EMPTY_TRANSACTION,       \
DATA_PUBLISHER_AGENT_DATA_TYPES_ERROR,  \
DATA_PUBLISHER_SCHEMA_FAILED,           \
DATA_PUBLISHER_BUFFER_STORAGE_ERROR,    \
DATA_PUBLISHER_ERROR

DEFINE_ENUM(DATA_PUBLISHER_RESULT, DATA_PUBLISHER_RESULT_VALUES);

typedef void* TRANSACTION_HANDLE;
typedef void* DATA_PUBLISHER_HANDLE;

extern DATA_PUBLISHER_HANDLE DataPublisher_Create(SCHEMA_MODEL_TYPE_HANDLE modelHandle, bool includePropertyPath);
extern void DataPublisher_Destroy(DATA_PUBLISHER_HANDLE dataPublisherHandle);

extern TRANSACTION_HANDLE DataPublisher_StartTransaction(DATA_PUBLISHER_HANDLE dataPublisherHandle);
extern DATA_PUBLISHER_RESULT DataPublisher_PublishTransacted(TRANSACTION_HANDLE transactionHandle, const char* propertyPath, const AGENT_DATA_TYPE* data);
extern DATA_PUBLISHER_RESULT DataPublisher_EndTransaction(TRANSACTION_HANDLE transactionHandle, unsigned char** destination, size_t* destinationSize);
extern DATA_PUBLISHER_RESULT DataPublisher_CancelTransaction(TRANSACTION_HANDLE transactionHandle);
extern void DataPublisher_SetMaxBufferSize(size_t value);
extern size_t DataPublisher_GetMaxBufferSize(void);

#ifdef __cplusplus
}
#endif

#endif /* DATA_PUBLISHER_H */
