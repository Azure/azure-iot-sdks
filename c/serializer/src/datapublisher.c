// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#include "gballoc.h"

#include <stdbool.h>
#include "datapublisher.h"
#include "jsonencoder.h"
#include "datamarshaller.h"
#include "agenttypesystem.h"
#include "schema.h"
#include "crt_abstractions.h"
#include "iot_logging.h"

DEFINE_ENUM_STRINGS(DATA_PUBLISHER_RESULT, DATA_PUBLISHER_RESULT_VALUES)

#define LOG_DATA_PUBLISHER_ERROR \
    LogError("(result = %s)\r\n", ENUM_TO_STRING(DATA_PUBLISHER_RESULT, result))

#define DEFAULT_MAX_BUFFER_SIZE 10240
/* Codes_SRS_DATA_PUBLISHER_99_066:[ A single value shall be used by all instances of DataPublisher.] */
/* Codes_SRS_DATA_PUBLISHER_99_067:[ Before any call to DataPublisher_SetMaxBufferSize, the default max buffer size shall be equal to 10KB.] */
static size_t maxBufferSize_ = DEFAULT_MAX_BUFFER_SIZE;

typedef struct DATA_PUBLISHER_INSTANCE_TAG
{
    DATA_MARSHALLER_HANDLE DataMarshallerHandle;
    SCHEMA_MODEL_TYPE_HANDLE ModelHandle;
} DATA_PUBLISHER_INSTANCE;

typedef struct TRANSACTION_TAG
{
    DATA_PUBLISHER_INSTANCE* DataPublisherInstance;
    size_t ValueCount;
    DATA_MARSHALLER_VALUE* Values;
} TRANSACTION;

DATA_PUBLISHER_HANDLE DataPublisher_Create(SCHEMA_MODEL_TYPE_HANDLE modelHandle, bool includePropertyPath)
{
    DATA_PUBLISHER_HANDLE result;
    DATA_PUBLISHER_INSTANCE* dataPublisherInstance;

    /* Codes_SRS_DATA_PUBLISHER_99_042:[ If a NULL argument is passed to it, DataPublisher_Create shall return NULL.] */
    if (
        (modelHandle == NULL)
        )
    {
        result = NULL;
        LogError("(result = %s)\r\n", ENUM_TO_STRING(DATA_PUBLISHER_RESULT, DATA_PUBLISHER_INVALID_ARG));
    }
    else if ((dataPublisherInstance = (DATA_PUBLISHER_INSTANCE*)malloc(sizeof(DATA_PUBLISHER_INSTANCE))) == NULL)
    {
        /* Codes_SRS_DATA_PUBLISHER_99_047:[ For any other error not specified here, DataPublisher_Create shall return NULL.] */
        result = NULL;
        LogError("(result = %s)\r\n", ENUM_TO_STRING(DATA_PUBLISHER_RESULT, DATA_PUBLISHER_ERROR));
    }
    else
    {
        /* Codes_SRS_DATA_PUBLISHER_99_043:[ DataPublisher_Create shall initialize and hold a handle to a DataMarshaller instance.] */
        /* Codes_SRS_DATA_PUBLISHER_01_001: [DataPublisher_Create shall pass the includePropertyPath argument to DataMarshaller_Create.] */
        if ((dataPublisherInstance->DataMarshallerHandle = DataMarshaller_Create(modelHandle, includePropertyPath)) == NULL)
        {
            free(dataPublisherInstance);

            /* Codes_SRS_DATA_PUBLISHER_99_044:[ If the creation of the DataMarshaller instance fails, DataPublisher_Create shall return NULL.] */
            result = NULL;
            LogError("(result = %s)\r\n", ENUM_TO_STRING(DATA_PUBLISHER_RESULT, DATA_PUBLISHER_MARSHALLER_ERROR));
        }
        else
        {
            dataPublisherInstance->ModelHandle = modelHandle;

            /* Codes_SRS_DATA_PUBLISHER_99_041:[ DataPublisher_Create shall create a new DataPublisher instance and return a non-NULL handle in case of success.] */
            result = dataPublisherInstance;
        }
    }

    return result;
}

void DataPublisher_Destroy(DATA_PUBLISHER_HANDLE dataPublisherHandle)
{
    if (dataPublisherHandle != NULL)
    {
        DATA_PUBLISHER_INSTANCE* dataPublisherInstance = (DATA_PUBLISHER_INSTANCE*)dataPublisherHandle;
        DataMarshaller_Destroy(dataPublisherInstance->DataMarshallerHandle);
        
        free(dataPublisherHandle);
    }
}

TRANSACTION_HANDLE DataPublisher_StartTransaction(DATA_PUBLISHER_HANDLE dataPublisherHandle)
{
    TRANSACTION* transaction;

    /* Codes_SRS_DATA_PUBLISHER_99_038:[ If DataPublisher_StartTransaction is called with a NULL argument it shall return NULL.] */
    if (dataPublisherHandle == NULL)
    {
        transaction = NULL;
        LogError("(Error code: %s)\r\n", ENUM_TO_STRING(DATA_PUBLISHER_RESULT, DATA_PUBLISHER_INVALID_ARG));
    }
    else
    {
        /* Codes_SRS_DATA_PUBLISHER_99_007:[ A call to DataPublisher_StartTransaction shall start a new transaction.] */
        transaction = (TRANSACTION*)malloc(sizeof(TRANSACTION));
        if (transaction == NULL)
        {
            LogError("Allocating transaction failed (Error code: %s)\r\n", ENUM_TO_STRING(DATA_PUBLISHER_RESULT, DATA_PUBLISHER_ERROR));
        }
        else
        {
            transaction->ValueCount = 0;
            transaction->Values = NULL;
            transaction->DataPublisherInstance = (DATA_PUBLISHER_INSTANCE*)dataPublisherHandle;
        }
    }

    /* Codes_SRS_DATA_PUBLISHER_99_008:[ DataPublisher_StartTransaction shall return a non-NULL handle upon success.] */
    /* Codes_SRS_DATA_PUBLISHER_99_009:[ DataPublisher_StartTransaction shall return NULL upon failure.] */
    return transaction;
}

DATA_PUBLISHER_RESULT DataPublisher_PublishTransacted(TRANSACTION_HANDLE transactionHandle, const char* propertyPath, const AGENT_DATA_TYPE* data)
{
    DATA_PUBLISHER_RESULT result;
    char* propertyPathCopy;

    /* Codes_SRS_DATA_PUBLISHER_99_017:[ When one or more NULL parameter(s) are specified, DataPublisher_PublishTransacted is called with a NULL transactionHandle, it shall return DATA_PUBLISHER_INVALID_ARG.] */
    if ((transactionHandle == NULL) ||
        (propertyPath == NULL) ||
        (data == NULL))
    {
        result = DATA_PUBLISHER_INVALID_ARG;
        LOG_DATA_PUBLISHER_ERROR;
    }
    else if (mallocAndStrcpy_s(&propertyPathCopy, propertyPath) != 0)
    {
        /* Codes_SRS_DATA_PUBLISHER_99_020:[ For any errors not explicitly mentioned here the DataPublisher APIs shall return DATA_PUBLISHER_ERROR.] */
        result = DATA_PUBLISHER_ERROR;
        LOG_DATA_PUBLISHER_ERROR;
    }
    else
    {
        TRANSACTION* transaction = (TRANSACTION*)transactionHandle;
        AGENT_DATA_TYPE* propertyValue;

        if (!Schema_ModelPropertyByPathExists(transaction->DataPublisherInstance->ModelHandle, propertyPath))
        {
            free(propertyPathCopy);

            /* Codes_SRS_DATA_PUBLISHER_99_040:[ When propertyPath does not exist in the supplied model, DataPublisher_Publish shall return DATA_PUBLISHER_SCHEMA_FAILED without dispatching data.] */
            result = DATA_PUBLISHER_SCHEMA_FAILED;
            LOG_DATA_PUBLISHER_ERROR;
        }
        else if ((propertyValue = (AGENT_DATA_TYPE*)malloc(sizeof(AGENT_DATA_TYPE))) == NULL)
        {
            free(propertyPathCopy);

            /* Codes_SRS_DATA_PUBLISHER_99_020:[ For any errors not explicitly mentioned here the DataPublisher APIs shall return DATA_PUBLISHER_ERROR.] */
            result = DATA_PUBLISHER_ERROR;
            LOG_DATA_PUBLISHER_ERROR;
        }
        else if (Create_AGENT_DATA_TYPE_from_AGENT_DATA_TYPE(propertyValue, data) != AGENT_DATA_TYPES_OK)
        {
            free(propertyPathCopy);
            free(propertyValue);

            /* Codes_SRS_DATA_PUBLISHER_99_028:[ If creating the copy fails then DATA_PUBLISHER_AGENT_DATA_TYPES_ERROR shall be returned.] */
            result = DATA_PUBLISHER_AGENT_DATA_TYPES_ERROR;
            LOG_DATA_PUBLISHER_ERROR;
        }
        else
        {
            size_t i;
            DATA_MARSHALLER_VALUE* propertySlot = NULL;

            /* Codes_SRS_DATA_PUBLISHER_99_019:[ If the same property is associated twice with a transaction, then the last value shall be kept associated with the transaction.] */
            for (i = 0; i < transaction->ValueCount; i++)
            {
                if (strcmp(transaction->Values[i].PropertyPath, propertyPath) == 0)
                {
                    propertySlot = &transaction->Values[i];
                    break;
                }
            }

            if (propertySlot == NULL)
            {
                DATA_MARSHALLER_VALUE* newValues = (DATA_MARSHALLER_VALUE*)realloc(transaction->Values, sizeof(DATA_MARSHALLER_VALUE)* (transaction->ValueCount + 1));
                if (newValues != NULL)
                {
                    transaction->Values = newValues;
                    propertySlot = &transaction->Values[transaction->ValueCount];
                    propertySlot->Value = NULL;
                    propertySlot->PropertyPath = NULL;
                    transaction->ValueCount++;
                }
            }

            if (propertySlot == NULL)
            {
                Destroy_AGENT_DATA_TYPE((AGENT_DATA_TYPE*)propertyValue);
                free(propertyValue);
                free(propertyPathCopy);

                /* Codes_SRS_DATA_PUBLISHER_99_020:[ For any errors not explicitly mentioned here the DataPublisher APIs shall return DATA_PUBLISHER_ERROR.] */
                result = DATA_PUBLISHER_ERROR;
                LOG_DATA_PUBLISHER_ERROR;
            }
            else
            {
                if (propertySlot->Value != NULL)
                {
                    Destroy_AGENT_DATA_TYPE((AGENT_DATA_TYPE*)propertySlot->Value);
                    free((AGENT_DATA_TYPE*)propertySlot->Value);
                }
                if (propertySlot->PropertyPath != NULL)
                {
                    char* existingValue = (char*)propertySlot->PropertyPath;
                    free(existingValue);
                }

                /* Codes_SRS_DATA_PUBLISHER_99_016:[ When DataPublisher_PublishTransacted is invoked, DataPublisher shall associate the data with the transaction identified by the transactionHandle argument and return DATA_PUBLISHER_OK. No data shall be dispatched at the time of the call.] */
                propertySlot->PropertyPath = propertyPathCopy;
                propertySlot->Value = propertyValue;

                result = DATA_PUBLISHER_OK;
            }
        }
    }

    return result;
}

DATA_PUBLISHER_RESULT DataPublisher_EndTransaction(TRANSACTION_HANDLE transactionHandle, unsigned char** destination, size_t* destinationSize)
{
    DATA_PUBLISHER_RESULT result;

    /*Codes_SRS_DATA_PUBLISHER_02_006: [If the destination argument is NULL, DataPublisher_EndTransaction shall return DATA_PUBLISHER_INVALID_ARG.] */
    /*Codes_SRS_DATA_PUBLISHER_02_007: [If the destinationSize argument is NULL, DataPublisher_EndTransaction shall return DATA_PUBLISHER_INVALID_ARG.] */
    if (
        (transactionHandle == NULL) || 
        (destination == NULL) ||
        (destinationSize == NULL)
        )
    {
        /* Codes_SRS_DATA_PUBLISHER_99_011:[ If the transactionHandle argument is NULL, DataPublisher_EndTransaction shall return DATA_PUBLISHER_INVALID_ARG.] */
        result = DATA_PUBLISHER_INVALID_ARG;
        LOG_DATA_PUBLISHER_ERROR;
    }
    else
    {
        TRANSACTION* transaction = (TRANSACTION*)transactionHandle;

        if (transaction->ValueCount == 0)
        {
            /* Codes_SRS_DATA_PUBLISHER_99_024:[ If no values have been associated with the transaction, no data shall be dispatched
                                            to DataMarshaller, the transaction shall be discarded and DataPublisher_EndTransaction shall return DATA_PUBLISHER_EMPTY_TRANSACTION.] */
            result = DATA_PUBLISHER_EMPTY_TRANSACTION;
            LOG_DATA_PUBLISHER_ERROR;
        }
        /* Codes_SRS_DATA_PUBLISHER_99_010:[ A call to DataPublisher_EndTransaction shall mark the end of a transaction and, trigger a dispatch of all the data grouped by that transaction.] */
        else if (DataMarshaller_SendData(transaction->DataPublisherInstance->DataMarshallerHandle, transaction->ValueCount, transaction->Values, destination, destinationSize) != DATA_MARSHALLER_OK)
        {
            /* Codes_SRS_DATA_PUBLISHER_99_025:[ When the DataMarshaller_SendData call fails, DataPublisher_EndTransaction shall return DATA_PUBLISHER_MARSHALLER_ERROR.] */
            result = DATA_PUBLISHER_MARSHALLER_ERROR;
            LOG_DATA_PUBLISHER_ERROR;
        }
        else
        {
            /* Codes_SRS_DATA_PUBLISHER_99_026:[ On success, DataPublisher_EndTransaction shall return DATA_PUBLISHER_OK.] */
            result = DATA_PUBLISHER_OK;
        }

        /* Codes_SRS_DATA_PUBLISHER_99_012:[ DataPublisher_EndTransaction shall dispose of any resources associated with the transaction.] */
        (void)DataPublisher_CancelTransaction(transactionHandle);
    }

    return result;
}

DATA_PUBLISHER_RESULT DataPublisher_CancelTransaction(TRANSACTION_HANDLE transactionHandle)
{
    DATA_PUBLISHER_RESULT result;

    if (transactionHandle == NULL)
    {
        /* Codes_SRS_DATA_PUBLISHER_99_014:[ If the transactionHandle argument is NULL DataPublisher_CancelTransaction shall return DATA_PUBLISHER_INVALID_ARG.] */
        result = DATA_PUBLISHER_INVALID_ARG;
        LOG_DATA_PUBLISHER_ERROR;
    }
    else
    {
        TRANSACTION* transaction = (TRANSACTION*)transactionHandle;
        size_t i;

        /* Codes_SRS_DATA_PUBLISHER_99_015:[ DataPublisher_CancelTransaction shall dispose of any resources associated with the transaction.] */
        for (i = 0; i < transaction->ValueCount; i++)
        {
            Destroy_AGENT_DATA_TYPE((AGENT_DATA_TYPE*)transaction->Values[i].Value);
            free((char*)transaction->Values[i].PropertyPath);
            free((AGENT_DATA_TYPE*)transaction->Values[i].Value);
        }

        /* Codes_SRS_DATA_PUBLISHER_99_015:[ DataPublisher_CancelTransaction shall dispose of any resources associated with the transaction.] */
        free(transaction->Values);
        free(transaction);

        /* Codes_SRS_DATA_PUBLISHER_99_013:[ A call to DataPublisher_CancelTransaction shall dispose of the transaction without dispatching 
                                        the data to the DataMarshaller module and it shall return DATA_PUBLISHER_OK.] */
        result = DATA_PUBLISHER_OK;
    }

    return result;
}

/* Codes_SRS_DATA_PUBLISHER_99_065:[ DataPublisher_SetMaxBufferSize shall directly update the value used to limit how much data (in bytes) can be buffered in the BufferStorage instance.] */
void DataPublisher_SetMaxBufferSize(size_t value)
{
    maxBufferSize_ = value;
}

/* Codes_SRS_DATA_PUBLISHER_99_069:[ DataMarshaller_GetMaxBufferSize shall return the current max buffer size value used by any new instance of DataMarshaller.] */
size_t DataPublisher_GetMaxBufferSize(void)
{
    return maxBufferSize_;
}
