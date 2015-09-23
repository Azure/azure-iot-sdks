// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h> /*for free*/
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#include "gballoc.h"

#include <stdbool.h>
#include "datamarshaller.h"
#include "crt_abstractions.h"
#include "schema.h"
#include "jsonencoder.h"
#include "agenttypesystem.h"
#include "iot_logging.h"

DEFINE_ENUM_STRINGS(DATA_MARSHALLER_RESULT, DATA_MARSHALLER_RESULT_VALUES);

#define LOG_DATA_MARSHALLER_ERROR \
    LogError("(result = %s)\r\n", ENUM_TO_STRING(DATA_MARSHALLER_RESULT, result));

typedef struct DATA_MARSHALLER_INSTANCE_TAG
{
    SCHEMA_MODEL_TYPE_HANDLE ModelHandle;
    bool IncludePropertyPath;
} DATA_MARSHALLER_INSTANCE;

static int NoCloneFunction(void** destination, const void* source)
{
    *destination = (void*)source;
    return 0;
}

static void NoFreeFunction(void* value)
{
    (void)value;
}

DATA_MARSHALLER_HANDLE DataMarshaller_Create(SCHEMA_MODEL_TYPE_HANDLE modelHandle, bool includePropertyPath)
{
    DATA_MARSHALLER_HANDLE result;
    DATA_MARSHALLER_INSTANCE* dataMarshallerInstance;

    /*Codes_SRS_DATA_MARSHALLER_99_019:[ DataMarshaller_Create shall return NULL if any argument is NULL.]*/
    if (
        (modelHandle == NULL) 
        )
    {
        result = NULL;
        LogError("(result = %s)\r\n", ENUM_TO_STRING(DATA_MARSHALLER_RESULT, DATA_MARSHALLER_INVALID_ARG));
    }
    else if ((dataMarshallerInstance = (DATA_MARSHALLER_INSTANCE*)malloc(sizeof(DATA_MARSHALLER_INSTANCE))) == NULL)
    {
        /* Codes_SRS_DATA_MARSHALLER_99_048:[On any other errors not explicitly specified, DataMarshaller_Create shall return NULL.] */
        result = NULL;
        LogError("(result = %s)\r\n", ENUM_TO_STRING(DATA_MARSHALLER_RESULT, DATA_MARSHALLER_ERROR));
    }
    else
    {
        /*everything ok*/
        dataMarshallerInstance->ModelHandle = modelHandle;
        dataMarshallerInstance->IncludePropertyPath = includePropertyPath;

        /*Codes_SRS_DATA_MARSHALLER_99_018:[ DataMarshaller_Create shall create a new DataMarshaller instance and on success it shall return a non NULL handle.]*/
        result = dataMarshallerInstance;
    }
    return result;
}

void DataMarshaller_Destroy(DATA_MARSHALLER_HANDLE dataMarshallerHandle)
{
    /* Codes_SRS_DATA_MARSHALLER_99_024:[ When called with a NULL handle, DataMarshaller_Destroy shall do nothing.] */
    if (dataMarshallerHandle != NULL)
    {
        /* Codes_SRS_DATA_MARSHALLER_99_022:[ DataMarshaller_Destroy shall free all resources associated with the dataMarshallerHandle argument.] */
        DATA_MARSHALLER_INSTANCE* dataMarshallerInstance = (DATA_MARSHALLER_INSTANCE*)dataMarshallerHandle;
        free(dataMarshallerInstance);
    }
}

DATA_MARSHALLER_RESULT DataMarshaller_SendData(DATA_MARSHALLER_HANDLE dataMarshallerHandle, size_t valueCount, const DATA_MARSHALLER_VALUE* values, unsigned char** destination, size_t* destinationSize)
{
    DATA_MARSHALLER_INSTANCE* dataMarshallerInstance = (DATA_MARSHALLER_INSTANCE*)dataMarshallerHandle;
    DATA_MARSHALLER_RESULT result;
    MULTITREE_HANDLE treeHandle;

    /* Codes_SRS_DATA_MARSHALLER_99_034:[All argument checks shall be performed before calling any other modules.] */
    /* Codes_SRS_DATA_MARSHALLER_99_004:[ DATA_MARSHALLER_INVALID_ARG shall be returned when the function has detected an invalid parameter (NULL) being passed to the function.] */
    if ((values == NULL) ||
        (dataMarshallerHandle == NULL) ||
        (destination == NULL) ||
        (destinationSize == NULL) ||
        /* Codes_SRS_DATA_MARSHALLER_99_033:[ DATA_MARSHALLER_INVALID_ARG shall be returned if the valueCount is zero.] */
        (valueCount == 0))
    {
        result = DATA_MARSHALLER_INVALID_ARG;
        LOG_DATA_MARSHALLER_ERROR
    }
    else
    {
        size_t i;
        /* VS complains wrongly that result is not initialized */
        result = DATA_MARSHALLER_ERROR;
        bool includePropertyPath = dataMarshallerInstance->IncludePropertyPath;

        for (i = 0; i < valueCount; i++)
        {
            if ((values[i].PropertyPath == NULL) ||
                (values[i].Value == NULL))
            {
                /*Codes_SRS_DATA_MARSHALLER_99_007:[ DATA_MARSHALLER_INVALID_MODEL_PROPERTY shall be returned when any of the items in values contain invalid data]*/
                result = DATA_MARSHALLER_INVALID_MODEL_PROPERTY;
                LOG_DATA_MARSHALLER_ERROR
                break;
            }

            if ((!dataMarshallerInstance->IncludePropertyPath) &&
                (values[i].Value->type == EDM_COMPLEX_TYPE_TYPE) &&
                (valueCount > 1))
            {
                /* Codes_SRS_DATAMARSHALLER_01_002: [If the includePropertyPath argument passed to DataMarshaller_Create was false and the number of values passed to SendData is greater than 1 and at least one of them is a struct, DataMarshaller_SendData shall fallback to  including the complete property path in the output JSON.] */
                includePropertyPath = true;
            }
        }

        if (i == valueCount)
        {
            /* Codes_SRS_DATA_MARSHALLER_99_037:[DataMarshaller shall store as MultiTree the data to be encoded by the JSONEncoder module.] */
            if ((treeHandle = MultiTree_Create(NoCloneFunction, NoFreeFunction)) == NULL)
            {
                /* Codes_SRS_DATA_MARSHALLER_99_035:[DATA_MARSHALLER_MULTITREE_ERROR shall be returned in case any MultiTree API call fails.] */
                result = DATA_MARSHALLER_MULTITREE_ERROR;
                LOG_DATA_MARSHALLER_ERROR
            }
            else
            {
                size_t j;
                result = DATA_MARSHALLER_OK; /* addressing warning in VS compiler */
                /* Codes_SRS_DATA_MARSHALLER_99_038:[For each pair in the values argument, a string : value pair shall exist in the JSON object in the form of propertyName : value.] */
                for (j = 0; j < valueCount; j++)
                {
                    if ((includePropertyPath == false) && (values[j].Value->type == EDM_COMPLEX_TYPE_TYPE))
                    {
                        size_t k;

                        /* Codes_SRS_DATAMARSHALLER_01_001: [If the includePropertyPath argument passed to DataMarshaller_Create was false and only one struct is being sent, the relative path of the value passed to DataMarshaller_SendData - including property name - shall be ignored and the value shall be placed at JSON root.] */
                        for (k = 0; k < values[j].Value->value.edmComplexType.nMembers; k++)
                        {
                            /* Codes_SRS_DATAMARSHALLER_01_004: [In this case the members of the struct shall be added as leafs into the MultiTree, each leaf having the name of the struct member.] */
                            if (MultiTree_AddLeaf(treeHandle, values[j].Value->value.edmComplexType.fields[k].fieldName, (void*)values[j].Value->value.edmComplexType.fields[k].value) != MULTITREE_OK)
                            {
                                break;
                            }
                        }

                        if (k < values[j].Value->value.edmComplexType.nMembers)
                        {
                            /* Codes_SRS_DATA_MARSHALLER_99_035:[DATA_MARSHALLER_MULTITREE_ERROR shall be returned in case any MultiTree API call fails.] */
                            result = DATA_MARSHALLER_MULTITREE_ERROR;
                            LOG_DATA_MARSHALLER_ERROR
                            break;
                        }
                    }
                    else
                    {
                        /* Codes_SRS_DATA_MARSHALLER_99_039:[ If the includePropertyPath argument passed to DataMarshaller_Create was true each property shall be placed in the appropriate position in the JSON according to its path in the model.] */
                        if (MultiTree_AddLeaf(treeHandle, values[j].PropertyPath, (void*)values[j].Value) != MULTITREE_OK)
                        {
                            /* Codes_SRS_DATA_MARSHALLER_99_035:[DATA_MARSHALLER_MULTITREE_ERROR shall be returned in case any MultiTree API call fails.] */
                            result = DATA_MARSHALLER_MULTITREE_ERROR;
                            LOG_DATA_MARSHALLER_ERROR
                            break;
                        }
                    }

                }

                if (j == valueCount)
                {
                    STRING_HANDLE payload = STRING_new();
                    if (payload == NULL)
                    {
                        result = DATA_MARSHALLER_ERROR;
                        LOG_DATA_MARSHALLER_ERROR
                    }
                    else
                    {
                        if (JSONEncoder_EncodeTree(treeHandle, payload, (JSON_ENCODER_TOSTRING_FUNC)AgentDataTypes_ToString) != JSON_ENCODER_OK)
                        {
                            /* Codes_SRS_DATA_MARSHALLER_99_027:[ DATA_MARSHALLER_JSON_ENCODER_ERROR shall be returned when JSONEncoder returns an error code.] */
                            result = DATA_MARSHALLER_JSON_ENCODER_ERROR;
                            LOG_DATA_MARSHALLER_ERROR
                        }
                        else
                        {
                            /*Codes_SRS_DATAMARSHALLER_02_007: [DataMarshaller_SendData shall copy in the output parameters *destination, *destinationSize the content and the content length of the encoded JSON tree.] */
                            size_t resultSize = STRING_length(payload);
                            unsigned char* temp = malloc(resultSize);
                            if (temp == NULL)
                            {
                                /*Codes_SRS_DATA_MARSHALLER_99_015:[ DATA_MARSHALLER_ERROR shall be returned in all the other error cases not explicitly defined here.]*/
                                result = DATA_MARSHALLER_ERROR;
                                LOG_DATA_MARSHALLER_ERROR;
                            }
                            else
                            {
                                memcpy(temp, STRING_c_str(payload), resultSize);
                                *destination = temp;
                                *destinationSize = resultSize;
                                result = DATA_MARSHALLER_OK;
                            }
                        }
                        STRING_delete(payload);
                    }
                } /* if (j==valueCount)*/
                MultiTree_Destroy(treeHandle);
            } /* MultiTree_Create */
        }
    }

    return result;
}
