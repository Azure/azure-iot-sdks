// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#include "gballoc.h"

#include <stdbool.h>
#include "iotdevice.h"
#include "datapublisher.h"
#include "commanddecoder.h"
#include "crt_abstractions.h"
#include "iot_logging.h"

#define LOG_DEVICE_ERROR \
    LogError("(result = %s)\r\n", ENUM_TO_STRING(DEVICE_RESULT, result))

typedef struct DEVICE_TAG
{
    SCHEMA_MODEL_TYPE_HANDLE model;
    DATA_PUBLISHER_HANDLE dataPublisherHandle;
    pPfDeviceActionCallback deviceActionCallback;
    void* callbackUserContext;
    COMMAND_DECODER_HANDLE commandDecoderHandle;
} DEVICE;

DEFINE_ENUM_STRINGS(DEVICE_RESULT, DEVICE_RESULT_VALUES);

static EXECUTE_COMMAND_RESULT DeviceInvokeAction(void* actionCallbackContext, const char* relativeActionPath, const char* actionName, size_t argCount, const AGENT_DATA_TYPE* args)
{
    EXECUTE_COMMAND_RESULT result;

    /*Codes_SRS_DEVICE_02_011: [If the parameter actionCallbackContent passed the callback is NULL then the callback shall return EXECUTE_COMMAND_ERROR.] */
    if (actionCallbackContext == NULL)
    {
        result = EXECUTE_COMMAND_ERROR;
        LogError("(Error code = %s)\r\n", ENUM_TO_STRING(DEVICE_RESULT, DEVICE_INVALID_ARG));
    }
    else
    {
        DEVICE* device = (DEVICE*)actionCallbackContext;

        /* Codes_SRS_DEVICE_01_052: [When the action callback passed to CommandDecoder is called, Device shall call the appropriate user callback associated with the device handle.] */
        /* Codes_SRS_DEVICE_01_055: [The value passed in callbackUserContext when creating the device shall be passed to the callback as the value for the callbackUserContext argument.] */
        result = device->deviceActionCallback((DEVICE_HANDLE)device, device->callbackUserContext, relativeActionPath, actionName, argCount, args);
    }

    return result;
}

DEVICE_RESULT Device_Create(SCHEMA_MODEL_TYPE_HANDLE modelHandle, pPfDeviceActionCallback deviceActionCallback, void* callbackUserContext, bool includePropertyPath, DEVICE_HANDLE* deviceHandle)
{
    DEVICE_RESULT result;

    /* Codes_SRS_DEVICE_05_014: [If any of the modelHandle, deviceHandle or deviceActionCallback arguments are NULL, Device_Create shall return DEVICE_INVALID_ARG.]*/
    if (modelHandle == NULL || deviceHandle == NULL || deviceActionCallback == NULL )
    {
        result = DEVICE_INVALID_ARG;
        LOG_DEVICE_ERROR;
    }
    else
    {
        DEVICE* device = (DEVICE*)malloc(sizeof(DEVICE));
        if (device == NULL)
        {
            /* Codes_SRS_DEVICE_05_015: [If an error occurs while trying to create the device, Device_Create shall return DEVICE_ERROR.] */
            result = DEVICE_ERROR;
            LOG_DEVICE_ERROR;
        }
        else
        {
            /* Codes_SRS_DEVICE_01_018: [Device_Create shall create a DataPublisher instance by calling DataPublisher_Create.] */
            /* Codes_SRS_DEVICE_01_020: [Device_Create shall pass to DataPublisher_Create the FrontDoor instance obtained earlier.] */
            /* Codes_SRS_DEVICE_01_004: [DeviceCreate shall pass to DataPublisher_create the includePropertyPath argument.] */
            if ((device->dataPublisherHandle = DataPublisher_Create(modelHandle, includePropertyPath)) == NULL)
            {
                free(device);

                /* Codes_SRS_DEVICE_01_019: [If creating the DataPublisher instance fails, Device_Create shall return DEVICE_DATA_PUBLISHER_FAILED.] */
                result = DEVICE_DATA_PUBLISHER_FAILED;
                LOG_DEVICE_ERROR;
            }
            else
            {
                device->model = modelHandle;
                device->deviceActionCallback = deviceActionCallback;
                device->callbackUserContext = callbackUserContext;

                /* Codes_SRS_DEVICE_01_001: [Device_Create shall create a CommandDecoder instance by calling CommandDecoder_Create and passing to it the model handle.] */
                /* Codes_SRS_DEVICE_01_002: [Device_Create shall also pass to CommandDecoder_Create a callback to be invoked when a command is received and a context that shall be the device handle.] */
                if ((device->commandDecoderHandle = CommandDecoder_Create(modelHandle, DeviceInvokeAction, device)) == NULL)
                {
                    free(device);

                    /* Codes_SRS_DEVICE_01_003: [If CommandDecoder_Create fails, Device_Create shall return DEVICE_COMMAND_DECODER_FAILED.] */
                    result = DEVICE_COMMAND_DECODER_FAILED;
                    LOG_DEVICE_ERROR;
                }
                else
                {
                    /* Codes_SRS_DEVICE_03_003: [The DEVICE_HANDLE shall be provided via the deviceHandle out argument.] */
                    *deviceHandle = (DEVICE_HANDLE)device;
                    /* Codes_SRS_DEVICE_03_004: [Device_Create shall return DEVICE_OK upon success.] */
                    result = DEVICE_OK;
                }
            }
        }
    }

    return result;
}

/* Codes_SRS_DEVICE_03_006: [Device_Destroy shall free all resources associated with a device.] */
void Device_Destroy(DEVICE_HANDLE deviceHandle)
{
    /* Codes_SRS_DEVICE_03_007: [Device_Destroy will not do anything if deviceHandle is NULL.] */
    if (deviceHandle != NULL)
    {
        DEVICE* device = (DEVICE*)deviceHandle;

        DataPublisher_Destroy(device->dataPublisherHandle);
        CommandDecoder_Destroy(device->commandDecoderHandle);

        free(device);
    }
}

TRANSACTION_HANDLE Device_StartTransaction(DEVICE_HANDLE deviceHandle)
{
    TRANSACTION_HANDLE result;

    /* Codes_SRS_DEVICE_01_035: [If any argument is NULL, Device_StartTransaction shall return NULL.] */
    if (deviceHandle == NULL)
    {
        result = NULL;
        LogError("(Error code = %s)\r\n", ENUM_TO_STRING(DEVICE_RESULT, DEVICE_INVALID_ARG));
    }
    else
    {
        DEVICE* deviceInstance = (DEVICE*)deviceHandle;

        /* Codes_SRS_DEVICE_01_034: [Device_StartTransaction shall invoke DataPublisher_StartTransaction for the DataPublisher handle associated with the deviceHandle argument.] */
        /* Codes_SRS_DEVICE_01_043: [On success, Device_StartTransaction shall return a non NULL handle.] */
        /* Codes_SRS_DEVICE_01_048: [When DataPublisher_StartTransaction fails, Device_StartTransaction shall return NULL.] */
        result = DataPublisher_StartTransaction(deviceInstance->dataPublisherHandle);
    }

    return result;
}

DEVICE_RESULT Device_PublishTransacted(TRANSACTION_HANDLE transactionHandle, const char* propertyPath, const AGENT_DATA_TYPE* data)
{
    DEVICE_RESULT result;

    /* Codes_SRS_DEVICE_01_037: [If any argument is NULL, Device_PublishTransacted shall return DEVICE_INVALID_ARG.] */
    if ((transactionHandle == NULL) ||
        (propertyPath == NULL) ||
        (data == NULL))
    {
        result = DEVICE_INVALID_ARG;
        LOG_DEVICE_ERROR;
    }
    /* Codes_SRS_DEVICE_01_036: [Device_PublishTransacted shall invoke DataPublisher_PublishTransacted.] */
    else if (DataPublisher_PublishTransacted(transactionHandle, propertyPath, data) != DATA_PUBLISHER_OK)
    {
        /* Codes_SRS_DEVICE_01_049: [When DataPublisher_PublishTransacted fails, Device_PublishTransacted shall return DEVICE_DATA_PUBLISHER_FAILED.] */
        result = DEVICE_DATA_PUBLISHER_FAILED;
        LOG_DEVICE_ERROR;
    }
    else
    {
        /* Codes_SRS_DEVICE_01_044: [On success, Device_PublishTransacted shall return DEVICE_OK.] */
        result = DEVICE_OK;
    }

    return result;
}

DEVICE_RESULT Device_EndTransaction(TRANSACTION_HANDLE transactionHandle, unsigned char** destination, size_t* destinationSize)
{
    DEVICE_RESULT result;

    /* Codes_SRS_DEVICE_01_039: [If any parameter is NULL, Device_EndTransaction shall return DEVICE_INVALID_ARG.]*/
    if (
        (transactionHandle == NULL) ||
        (destination == NULL) ||
        (destinationSize == NULL)
        )
    {
        result = DEVICE_INVALID_ARG;
        LOG_DEVICE_ERROR;
    }
    /* Codes_SRS_DEVICE_01_038: [Device_EndTransaction shall invoke DataPublisher_EndTransaction.] */
    else if (DataPublisher_EndTransaction(transactionHandle, destination, destinationSize) != DATA_PUBLISHER_OK)
    {
        /* Codes_SRS_DEVICE_01_050: [When DataPublisher_EndTransaction fails, Device_EndTransaction shall return DEVICE_DATA_PUBLISHER_FAILED.] */
        result = DEVICE_DATA_PUBLISHER_FAILED;
        LOG_DEVICE_ERROR;
    }
    else
    {
        /* Codes_SRS_DEVICE_01_045: [On success, Device_EndTransaction shall return DEVICE_OK.] */
        result = DEVICE_OK;
    }

    return result;
}

DEVICE_RESULT Device_CancelTransaction(TRANSACTION_HANDLE transactionHandle)
{
    DEVICE_RESULT result;

    /* Codes_SRS_DEVICE_01_041: [If any argument is NULL, Device_CancelTransaction shall return DEVICE_INVALID_ARG.] */
    if (transactionHandle == NULL)
    {
        result = DEVICE_INVALID_ARG;
        LOG_DEVICE_ERROR;
    }
    /* Codes_SRS_DEVICE_01_040: [Device_CancelTransaction shall invoke DataPublisher_CancelTransaction.] */
    else if (DataPublisher_CancelTransaction(transactionHandle) != DATA_PUBLISHER_OK)
    {
        /* Codes_SRS_DEVICE_01_051: [When DataPublisher_CancelTransaction fails, Device_CancelTransaction shall return DEVICE_DATA_PUBLISHER_FAILED.] */
        result = DEVICE_DATA_PUBLISHER_FAILED;
        LOG_DEVICE_ERROR;
    }
    else
    {
        /* Codes_SRS_DEVICE_01_046: [On success, Device_PublishTransacted shall return DEVICE_OK.] */
        result = DEVICE_OK;
    }

    return result;
}

EXECUTE_COMMAND_RESULT Device_ExecuteCommand(DEVICE_HANDLE deviceHandle, const char* command)
{
    EXECUTE_COMMAND_RESULT result;
    /*Codes_SRS_DEVICE_02_012: [If any parameters are NULL, then Device_ExecuteCommand shall return EXECUTE_COMMAND_ERROR.] */
    if (
        (deviceHandle == NULL) ||
        (command == NULL)
        )
    {
        result = EXECUTE_COMMAND_ERROR;
        LogError("invalid parameter (NULL passed to Device_ExecuteCommand DEVICE_HANDLE deviceHandle=%p, const char* command=%p", deviceHandle, command);
    }
    else
    {
        /*Codes_SRS_DEVICE_02_013: [Otherwise, Device_ExecuteCommand shall call CommandDecoder_ExecuteCommand and return what CommandDecoder_ExecuteCommand is returning.] */
        DEVICE* device = (DEVICE*)deviceHandle;
        result = CommandDecoder_ExecuteCommand(device->commandDecoderHandle, command);
    }
    return result;
}
