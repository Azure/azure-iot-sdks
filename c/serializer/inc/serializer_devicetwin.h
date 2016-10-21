// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.


#ifndef SERIALIZER_DEVICE_TWIN_H
#define SERIALIZER_DEVICE_TWIN_H

#include "serializer.h"

#include "iothub_client.h"
#include "iothub_client_ll.h"
#include "parson.h"
#include "vector.h"

static void serializer_ingest(DEVICE_TWIN_UPDATE_STATE update_state, const unsigned char* payLoad, size_t size, void* userContextCallback)
{
    /*by convention, userContextCallback is a pointer to a model instance created with CodeFirst_CreateDevice*/

    /*Codes_SRS_SERIALIZERDEVICETWIN_02_001: [ serializer_ingest shall clone the payload into a null terminated string. ]*/
    char* copyOfPayload = (char*)malloc(size + 1);
    if (copyOfPayload == NULL)
    {
        /*Codes_SRS_SERIALIZERDEVICETWIN_02_008: [ If any of the above operations fail, then serializer_ingest shall return. ]*/
        LogError("unable to malloc\n");
    }
    else
    {
        memcpy(copyOfPayload, payLoad, size);
        copyOfPayload[size] = '\0';

        /*Codes_SRS_SERIALIZERDEVICETWIN_02_002: [ serializer_ingest shall parse the null terminated string into parson data types. ]*/
        JSON_Value* allJSON = json_parse_string(copyOfPayload);
        if (allJSON == NULL)
        {
            /*Codes_SRS_SERIALIZERDEVICETWIN_02_008: [ If any of the above operations fail, then serializer_ingest shall return. ]*/
            LogError("failure in json_parse_string");
        }
        else
        {
            JSON_Object *allObject = json_value_get_object(allJSON);
            if (allObject == NULL)
            {
                /*Codes_SRS_SERIALIZERDEVICETWIN_02_008: [ If any of the above operations fail, then serializer_ingest shall return. ]*/
                LogError("failure in json_value_get_object");
            }
            else
            {
                switch (update_state)
                {
                    /*Codes_SRS_SERIALIZERDEVICETWIN_02_003: [ If update_state is DEVICE_TWIN_UPDATE_COMPLETE then serializer_ingest shall locate "desired" json name. ]*/
                    case DEVICE_TWIN_UPDATE_COMPLETE:
                    {
                        JSON_Object* desired = json_object_get_object(allObject, "desired");
                        if (desired == NULL)
                        {
                            /*Codes_SRS_SERIALIZERDEVICETWIN_02_008: [ If any of the above operations fail, then serializer_ingest shall return. ]*/
                            LogError("failure in json_object_get_object");
                        }
                        else
                        {
                            /*Codes_SRS_SERIALIZERDEVICETWIN_02_004: [ If "desired" contains "$version" then serializer_ingest shall remove it. ]*/
                            (void)json_object_remove(desired, "$version"); //it might not exist
                            JSON_Value* desiredAfterRemove = json_object_get_value(allObject, "desired");
                            if (desiredAfterRemove != NULL)
                            {
                                /*Codes_SRS_SERIALIZERDEVICETWIN_02_005: [ The "desired" value shall be outputed to a null terminated string and serializer_ingest shall call CodeFirst_IngestDesiredProperties. ]*/
                                char* pretty = json_serialize_to_string(desiredAfterRemove);
                                if (pretty == NULL)
                                {
                                    /*Codes_SRS_SERIALIZERDEVICETWIN_02_008: [ If any of the above operations fail, then serializer_ingest shall return. ]*/
                                    LogError("failure in json_serialize_to_string\n");
                                }
                                else
                                {
                                    if (CodeFirst_IngestDesiredProperties(userContextCallback, pretty) != CODEFIRST_OK)
                                    {
                                        /*Codes_SRS_SERIALIZERDEVICETWIN_02_008: [ If any of the above operations fail, then serializer_ingest shall return. ]*/
                                        LogError("failure ingesting desired properties\n");
                                    }
                                    else
                                    {
                                        /*all is fine*/
                                    }
                                    free(pretty);
                                }
                            }
                        }
                        break;
                    }
                    case DEVICE_TWIN_UPDATE_PARTIAL:
                    {
                        /*Codes_SRS_SERIALIZERDEVICETWIN_02_006: [ If update_state is DEVICE_TWIN_UPDATE_PARTIAL then serializer_ingest shall remove "$version" (if it exists). ]*/
                        (void)json_object_remove(allObject, "$version");

                        /*Codes_SRS_SERIALIZERDEVICETWIN_02_007: [ The JSON shall be outputed to a null terminated string and serializer_ingest shall call CodeFirst_IngestDesiredProperties. ]*/
                        char* pretty = json_serialize_to_string(allJSON);
                        if (pretty == NULL)
                        {
                            /*Codes_SRS_SERIALIZERDEVICETWIN_02_008: [ If any of the above operations fail, then serializer_ingest shall return. ]*/
                            LogError("failure in json_serialize_to_string\n");
                        }
                        else
                        {
                            if (CodeFirst_IngestDesiredProperties(userContextCallback, pretty) != CODEFIRST_OK)
                            {
                                /*Codes_SRS_SERIALIZERDEVICETWIN_02_008: [ If any of the above operations fail, then serializer_ingest shall return. ]*/
                                LogError("failure ingesting desired properties\n");
                            }
                            else
                            {
                                /*all is fine*/
                            }
                            free(pretty);
                        }
                        break;
                    }
                    default:
                    {
                        LogError("INTERNAL ERROR: unexpected value for update_state=%d\n", (int)update_state);
                    }
                }
            }
            json_value_free(allJSON);
        }
        free(copyOfPayload);
    }
}

/*an enum that sets the type of the handle used to record IoTHubDeviceTwin_Create was called*/
#define IOTHUB_CLIENT_HANDLE_TYPE_VALUES \
    IOTHUB_CLIENT_CONVENIENCE_HANDLE_TYPE, \
    IOTHUB_CLIENT_LL_HANDLE_TYPE

DEFINE_ENUM(IOTHUB_CLIENT_HANDLE_TYPE, IOTHUB_CLIENT_HANDLE_TYPE_VALUES)

typedef union IOTHUB_CLIENT_HANDLE_VALUE_TAG
{
    IOTHUB_CLIENT_HANDLE iothubClientHandle;
    IOTHUB_CLIENT_LL_HANDLE iothubClientLLHandle;
} IOTHUB_CLIENT_HANDLE_VALUE;

typedef struct IOTHUB_CLIENT_HANDLE_VARIANT_TAG
{
    IOTHUB_CLIENT_HANDLE_TYPE iothubClientHandleType;
    IOTHUB_CLIENT_HANDLE_VALUE iothubClientHandleValue;
} IOTHUB_CLIENT_HANDLE_VARIANT;

typedef struct SERIALIZER_DEVICETWIN_PROTOHANDLE_TAG /*it is called "PROTOHANDLE" because it is a primitive type of handle*/
{
    IOTHUB_CLIENT_HANDLE_VARIANT iothubClientHandleVariant;
    void* deviceAssigned;
} SERIALIZER_DEVICETWIN_PROTOHANDLE;
 
static VECTOR_HANDLE g_allProtoHandles=NULL; /*contains SERIALIZER_DEVICETWIN_PROTOHANDLE*/

static int lazilyAddProtohandle(const SERIALIZER_DEVICETWIN_PROTOHANDLE* protoHandle)
{
    int result;
    if ((g_allProtoHandles == NULL) && ((g_allProtoHandles = VECTOR_create(sizeof(SERIALIZER_DEVICETWIN_PROTOHANDLE))) == NULL))
    {
        LogError("failure in VECTOR_create");
        result = __LINE__;
    }
    else
    {
        if (VECTOR_push_back(g_allProtoHandles, protoHandle, 1) != 0)
        {
            LogError("failure in VECTOR_push_back");
            result = __LINE__;

            /*leave it as it was*/

            if (VECTOR_size(g_allProtoHandles) == 0)
            {
                VECTOR_destroy(g_allProtoHandles);
                g_allProtoHandles = NULL;
            }
        }
        else
        {
            result = 0;
        }
    }
    return result;
}

static IOTHUB_CLIENT_RESULT Generic_IoTHubClient_SetDeviceTwinCallback(const SERIALIZER_DEVICETWIN_PROTOHANDLE* protoHandle, IOTHUB_CLIENT_DEVICE_TWIN_CALLBACK deviceTwinCallback, void* userContextCallback)
{
    IOTHUB_CLIENT_RESULT result;
    /*Codes_SRS_SERIALIZERDEVICETWIN_02_011: [ IoTHubDeviceTwinCreate_Impl shall set the device twin callback. ]*/
    switch (protoHandle->iothubClientHandleVariant.iothubClientHandleType)
    {
    case IOTHUB_CLIENT_CONVENIENCE_HANDLE_TYPE:
    {
        if ((result = IoTHubClient_SetDeviceTwinCallback(protoHandle->iothubClientHandleVariant.iothubClientHandleValue.iothubClientHandle, deviceTwinCallback, userContextCallback)) != IOTHUB_CLIENT_OK)
        {
            LogError("failure in IoTHubClient_SetDeviceTwinCallback");
        }
        break;
    }
    case IOTHUB_CLIENT_LL_HANDLE_TYPE:
    {
        if ((result =IoTHubClient_LL_SetDeviceTwinCallback(protoHandle->iothubClientHandleVariant.iothubClientHandleValue.iothubClientLLHandle, deviceTwinCallback, userContextCallback)) != IOTHUB_CLIENT_OK)
        {
            LogError("failure in IoTHubClient_LL_SetDeviceTwinCallback");
        }
        break;
    }
    default:
    {
        result = IOTHUB_CLIENT_ERROR;
        LogError("INTERNAL ERROR");
    }
    }/*switch*/
    return result;
}

static void* IoTHubDeviceTwinCreate_Impl(const char* name, size_t sizeOfName, SERIALIZER_DEVICETWIN_PROTOHANDLE* protoHandle)
{
    void* result;
    /*Codes_SRS_SERIALIZERDEVICETWIN_02_009: [ IoTHubDeviceTwinCreate_Impl shall locate the model and the metadata for name by calling Schema_GetSchemaForModel/Schema_GetMetadata/Schema_GetModelByName. ]*/
    SCHEMA_HANDLE h = Schema_GetSchemaForModel(name);
    if (h == NULL)
    {
        /*Codes_SRS_SERIALIZERDEVICETWIN_02_014: [ Otherwise, IoTHubDeviceTwinCreate_Impl shall fail and return NULL. ]*/
        LogError("failure in Schema_GetSchemaForModel.");
        result = NULL;
    }
    else
    {
        void* metadata = Schema_GetMetadata(h);
        SCHEMA_MODEL_TYPE_HANDLE modelType = Schema_GetModelByName(h, name);
        if (modelType == NULL)
        {
            /*Codes_SRS_SERIALIZERDEVICETWIN_02_014: [ Otherwise, IoTHubDeviceTwinCreate_Impl shall fail and return NULL. ]*/
            LogError("failure in Schema_GetModelByName");
            result = NULL;
        }
        else
        {
            /*Codes_SRS_SERIALIZERDEVICETWIN_02_010: [ IoTHubDeviceTwinCreate_Impl shall call CodeFirst_CreateDevice. ]*/
            result = CodeFirst_CreateDevice(modelType, (REFLECTED_DATA_FROM_DATAPROVIDER *)metadata, sizeOfName, true);
            if (result == NULL)
            {
                /*Codes_SRS_SERIALIZERDEVICETWIN_02_014: [ Otherwise, IoTHubDeviceTwinCreate_Impl shall fail and return NULL. ]*/
                LogError("failure in CodeFirst_CreateDevice");
                /*return as is*/
            }
            else
            {
                protoHandle->deviceAssigned = result;
                if (Generic_IoTHubClient_SetDeviceTwinCallback(protoHandle, serializer_ingest, result) != IOTHUB_CLIENT_OK)
                {
                    /*Codes_SRS_SERIALIZERDEVICETWIN_02_014: [ Otherwise, IoTHubDeviceTwinCreate_Impl shall fail and return NULL. ]*/
                    LogError("failure in Generic_IoTHubClient_SetDeviceTwinCallback");
                    CodeFirst_DestroyDevice(result);
                    result = NULL;
                }
                else
                {
                    /*lazily add the protohandle to the array of tracking handles*/

                    /*Codes_SRS_SERIALIZERDEVICETWIN_02_012: [ IoTHubDeviceTwinCreate_Impl shall record the pair of (device, IoTHubClient(_LL)). ]*/
                    if (lazilyAddProtohandle(protoHandle) != 0)
                    {
                        /*Codes_SRS_SERIALIZERDEVICETWIN_02_014: [ Otherwise, IoTHubDeviceTwinCreate_Impl shall fail and return NULL. ]*/
                        LogError("unable to add the protohandle to the collection of handles");
                        /*unsubscribe*/
                        if (Generic_IoTHubClient_SetDeviceTwinCallback(protoHandle, NULL, NULL) != IOTHUB_CLIENT_OK)
                        {
                            /*just log the error*/
                            LogError("failure in Generic_IoTHubClient_SetDeviceTwinCallback");
                        }
                        CodeFirst_DestroyDevice(result);
                        result = NULL;
                    }
                    else
                    {
                        /*Codes_SRS_SERIALIZERDEVICETWIN_02_013: [ If all operations complete successfully then IoTHubDeviceTwinCreate_Impl shall succeeds and return a non-NULL value. ]*/
                        /*return as is*/
                    }
                }
            }
        }
    }
    return result;
}

static bool protoHandleHasDeviceStartAddress(const void* element, const void* value)
{
    const SERIALIZER_DEVICETWIN_PROTOHANDLE* protoHandle = (const SERIALIZER_DEVICETWIN_PROTOHANDLE*)element;
    return protoHandle->deviceAssigned == value;
}

static void IoTHubDeviceTwin_Destroy_Impl(void* model)
{
    /*Codes_SRS_SERIALIZERDEVICETWIN_02_020: [ If model is NULL then IoTHubDeviceTwin_Destroy_Impl shall return. ]*/
    if (model == NULL)
    {
        LogError("invalid argument void* model=%p", model);
    }
    else
    {
        /*Codes_SRS_SERIALIZERDEVICETWIN_02_015: [ IoTHubDeviceTwin_Destroy_Impl shall locate the saved handle belonging to model. ]*/
        SERIALIZER_DEVICETWIN_PROTOHANDLE* protoHandle = (SERIALIZER_DEVICETWIN_PROTOHANDLE*)VECTOR_find_if(g_allProtoHandles, protoHandleHasDeviceStartAddress, model);
        if (protoHandle == NULL)
        {
            LogError("failure in VECTOR_find_if [not found]");
        }
        else
        {
            /*Codes_SRS_SERIALIZERDEVICETWIN_02_016: [ IoTHubDeviceTwin_Destroy_Impl shall set the devicetwin callback to NULL. ]*/
            switch (protoHandle->iothubClientHandleVariant.iothubClientHandleType)
            {
            case IOTHUB_CLIENT_CONVENIENCE_HANDLE_TYPE:
            {
                if (IoTHubClient_SetDeviceTwinCallback(protoHandle->iothubClientHandleVariant.iothubClientHandleValue.iothubClientHandle, NULL, NULL) != IOTHUB_CLIENT_OK)
                {
                    LogError("failure in IoTHubClient_SetDeviceTwinCallback");
                }
                break;
            }
            case IOTHUB_CLIENT_LL_HANDLE_TYPE:
            {
                if (IoTHubClient_LL_SetDeviceTwinCallback(protoHandle->iothubClientHandleVariant.iothubClientHandleValue.iothubClientLLHandle, NULL, NULL) != IOTHUB_CLIENT_OK)
                {
                    LogError("failure in IoTHubClient_LL_SetDeviceTwinCallback");
                }
                break;
            }
            default:
            {
                LogError("INTERNAL ERROR");
            }
            }/*switch*/
        }

        /*Codes_SRS_SERIALIZERDEVICETWIN_02_017: [ IoTHubDeviceTwin_Destroy_Impl shall call CodeFirst_DestroyDevice. ]*/
        CodeFirst_DestroyDevice(protoHandle->deviceAssigned);

        /*Codes_SRS_SERIALIZERDEVICETWIN_02_018: [ IoTHubDeviceTwin_Destroy_Impl shall remove the IoTHubClient_Handle and the device handle from the recorded set. ]*/
        VECTOR_erase(g_allProtoHandles, protoHandle, 1);

        /*Codes_SRS_SERIALIZERDEVICETWIN_02_019: [ If the recorded set of IoTHubClient handles is zero size, then the set shall be destroyed. ]*/
        if (VECTOR_size(g_allProtoHandles) == 0) /*lazy init means more work @ destroy time*/
        {
            VECTOR_destroy(g_allProtoHandles);
            g_allProtoHandles = NULL;
        }
    }
}

#define DECLARE_DEVICETWIN_MODEL(name, ...)    \
    DECLARE_MODEL(name, __VA_ARGS__)           \
    static name* C2(IoTHubDeviceTwin_Create, name)(IOTHUB_CLIENT_HANDLE iotHubClientHandle)                                                                                         \
    {                                                                                                                                                                               \
        SERIALIZER_DEVICETWIN_PROTOHANDLE protoHandle;                                                                                                                              \
        protoHandle.iothubClientHandleVariant.iothubClientHandleType = IOTHUB_CLIENT_CONVENIENCE_HANDLE_TYPE;                                                                       \
        protoHandle.iothubClientHandleVariant.iothubClientHandleValue.iothubClientHandle = iotHubClientHandle;                                                                      \
        return (name*)IoTHubDeviceTwinCreate_Impl(#name, sizeof(name), &protoHandle);                                                                                               \
    }                                                                                                                                                                               \
                                                                                                                                                                                    \
    static void C2(IoTHubDeviceTwin_Destroy, name) (name* model)                                                                                                                    \
    {                                                                                                                                                                               \
        IoTHubDeviceTwin_Destroy_Impl(model);                                                                                                                                       \
    }                                                                                                                                                                               \
                                                                                                                                                                                    \
    static name* C2(IoTHubDeviceTwin_LL_Create, name)(IOTHUB_CLIENT_LL_HANDLE iotHubClientLLHandle)                                                                                 \
    {                                                                                                                                                                               \
        SERIALIZER_DEVICETWIN_PROTOHANDLE protoHandle;                                                                                                                              \
        protoHandle.iothubClientHandleVariant.iothubClientHandleType = IOTHUB_CLIENT_LL_HANDLE_TYPE;                                                                                \
        protoHandle.iothubClientHandleVariant.iothubClientHandleValue.iothubClientLLHandle = iotHubClientLLHandle;                                                                  \
        return (name*)IoTHubDeviceTwinCreate_Impl(#name, sizeof(name), &protoHandle);                                                                                               \
    }                                                                                                                                                                               \
                                                                                                                                                                                    \
    static void C2(IoTHubDeviceTwin_LL_Destroy, name) (name* model)                                                                                                                 \
    {                                                                                                                                                                               \
        IoTHubDeviceTwin_Destroy_Impl(model);                                                                                                                                       \
    }                                                                                                                                                                               \

#endif /*SERIALIZER_DEVICE_TWIN_H*/


