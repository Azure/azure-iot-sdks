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
    (void)(update_state);
    /*by convention, userContextCallback is a pointer to a model instance created with CodeFirst_CreateDevice*/
    char* copyOfPayload = (char*)malloc(size + 1);
    if (copyOfPayload == NULL)
    {
        printf("unable to malloc\n");
    }
    else
    {
        memcpy(copyOfPayload, payLoad, size);
        copyOfPayload[size] = '\0';
        JSON_Value* allJSON = json_parse_string(copyOfPayload);
        if (allJSON == NULL)
        {
            printf(" failure in json_parse_string");
        }
        else
        {
            JSON_Object *allObject = json_value_get_object(allJSON);
            if (allObject == NULL)
            {
                printf("failure in json_value_get_object");
            }
            else
            {
                switch (update_state)
                {
                case DEVICE_TWIN_UPDATE_COMPLETE:
                {
                    JSON_Object* desired = json_object_get_object(allObject, "desired");
                    if (desired == NULL)
                    {
                        printf("failure in json_object_get_object");
                    }
                    else
                    {
                        (void)json_object_remove(desired, "$version"); //it might not exist
                        JSON_Value* desiredAfterRemove = json_object_get_value(allObject, "desired");
                        if (desiredAfterRemove != NULL)
                        {

                            char* pretty = json_serialize_to_string(desiredAfterRemove);
                            if (pretty == NULL)
                            {
                                printf("failure in json_serialize_to_string\n");
                            }
                            else
                            {
                                if (CodeFirst_IngestDesiredProperties(userContextCallback, pretty) != CODEFIRST_OK)
                                {
                                    printf("failure ingesting desired properties\n");
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
                    (void)json_object_remove(allObject, "$version");
                    char* pretty = json_serialize_to_string(allJSON);
                    if (pretty == NULL)
                    {
                        printf("failure in json_serialize_to_string\n");
                    }
                    else
                    {
                        if (CodeFirst_IngestDesiredProperties(userContextCallback, pretty) != CODEFIRST_OK)
                        {
                            printf("failure ingesting desired properties\n");
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
                    printf("INTERNAL ERROR: unexpected value for update_state=%d\n", (int)update_state);
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

int lazilyAddProtohandle(const SERIALIZER_DEVICETWIN_PROTOHANDLE* protoHandle)
{
    int result;
    if ((g_allProtoHandles == NULL) &&((g_allProtoHandles = VECTOR_create(sizeof(SERIALIZER_DEVICETWIN_PROTOHANDLE)))==NULL))
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
    SCHEMA_HANDLE h = Schema_GetSchemaForModel(name);
    if (h == NULL)
    {
        LogError("failure in Schema_GetSchemaForModel.");
        result = NULL;
    }
    else
    {
        void* metadata = Schema_GetMetadata(h);
        SCHEMA_MODEL_TYPE_HANDLE modelType;
        modelType = Schema_GetModelByName(h, name);
        if (modelType == NULL)
        {
            LogError("failure in Schema_GetModelByName");
            result = NULL;
        }
        else
        {
            result = CodeFirst_CreateDevice(modelType, metadata, sizeOfName, true);
            if (result == NULL)
            {
                LogError("failure in CodeFirst_CreateDevice");
                /*return as is*/
            }
            else
            {
                protoHandle->deviceAssigned = result;
                if (Generic_IoTHubClient_SetDeviceTwinCallback(protoHandle, serializer_ingest, result) != IOTHUB_CLIENT_OK)
                {
                    LogError("failure in Generic_IoTHubClient_SetDeviceTwinCallback");
                    CodeFirst_DestroyDevice(result);
                    result = NULL;
                }
                else
                {
                    /*lazily add the protohandle to the array of tracking handles*/
                    if (lazilyAddProtohandle(protoHandle) != 0)
                    {
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
    const SERIALIZER_DEVICETWIN_PROTOHANDLE* protoHandle = element;
    return protoHandle->deviceAssigned == value;
}

static void IoTHubDeviceTwin_Destroy_Impl(void* model)
{
    SERIALIZER_DEVICETWIN_PROTOHANDLE* protoHandle = VECTOR_find_if(g_allProtoHandles, protoHandleHasDeviceStartAddress, model);
    if (protoHandle == NULL)
    {
        LogError("failure in VECTOR_find_if [not found]");
    }
    else
    {
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
    CodeFirst_DestroyDevice(protoHandle->deviceAssigned);
    VECTOR_erase(g_allProtoHandles, protoHandle, 1);

    if (VECTOR_size(g_allProtoHandles) == 0) /*lazy init means more work @ destroy time*/
    {
        VECTOR_destroy(g_allProtoHandles);
        g_allProtoHandles = NULL;
    }
}

#define DECLARE_DEVICETWIN_MODEL(name, ...)    \
    DECLARE_MODEL(name, __VA_ARGS__)           \
    static name* C2(IoTHubDeviceTwin_Create, name)(IOTHUB_CLIENT_HANDLE iotHubClientHandle)                                                                                         \
    {                                                                                                                                                                               \
        SERIALIZER_DEVICETWIN_PROTOHANDLE protoHandle;                                                                                                                              \
        protoHandle.iothubClientHandleVariant.iothubClientHandleType = IOTHUB_CLIENT_CONVENIENCE_HANDLE_TYPE;                                                                       \
        protoHandle.iothubClientHandleVariant.iothubClientHandleValue.iothubClientHandle = iotHubClientHandle;                                                                      \
        return IoTHubDeviceTwinCreate_Impl(#name, sizeof(name), &protoHandle);                                                                                                      \
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
        return IoTHubDeviceTwinCreate_Impl(#name, sizeof(name), &protoHandle);                                                                                                      \
    }                                                                                                                                                                               \
                                                                                                                                                                                    \
    static void C2(IoTHubDeviceTwin_LL_Destroy, name) (name* model)                                                                                                                 \
    {                                                                                                                                                                               \
        IoTHubDeviceTwin_Destroy_Impl(model);                                                                                                                                       \
    }                                                                                                                                                                               \

#endif /*SERIALIZER_DEVICE_TWIN_H*/


