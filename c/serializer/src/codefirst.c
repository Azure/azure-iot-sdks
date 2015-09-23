// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#include "gballoc.h"

#include "codefirst.h"
#include "macro_utils.h"
#include "crt_abstractions.h"
#include "iot_logging.h"
#include <stddef.h>
#include "crt_abstractions.h"
#include "iotdevice.h"

DEFINE_ENUM_STRINGS(CODEFIRST_RESULT, CODEFIRST_ENUM_VALUES)
DEFINE_ENUM_STRINGS(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_RESULT_VALUES)

#define LOG_CODEFIRST_ERROR \
    LogError("(result = %s)\r\n", ENUM_TO_STRING(CODEFIRST_RESULT, result))

typedef struct DEVICE_HEADER_DATA_TAG
{
    DEVICE_HANDLE DeviceHandle;
    const REFLECTED_DATA_FROM_DATAPROVIDER* ReflectedData;
    SCHEMA_MODEL_TYPE_HANDLE ModelHandle;
    size_t DataSize;
    unsigned char* data;
} DEVICE_HEADER_DATA;

#define COUNT_OF(A) (sizeof(A) / sizeof((A)[0]))

typedef enum CODEFIRST_STATE_TAG
{
    CODEFIRST_STATE_NOT_INIT,
    CODEFIRST_STATE_INIT
}CODEFIRST_STATE;

static CODEFIRST_STATE g_state = CODEFIRST_STATE_NOT_INIT;

static const char* g_OverrideSchemaNamespace;
static size_t g_DeviceCount = 0;
static DEVICE_HEADER_DATA** g_Devices = NULL;

static void DestroyDevice(DEVICE_HEADER_DATA* deviceHeader)
{
    /* Codes_SRS_CODEFIRST_99_085:[CodeFirst_DestroyDevice shall free all resources associated with a device.] */
    /* Codes_SRS_CODEFIRST_99_087:[In order to release the device handle, CodeFirst_DestroyDevice shall call Device_Destroy.] */
    Device_Destroy(deviceHeader->DeviceHandle);
    free(deviceHeader->data);
    free(deviceHeader);
}

static CODEFIRST_RESULT buildStructTypes(SCHEMA_HANDLE schemaHandle, const REFLECTED_DATA_FROM_DATAPROVIDER* reflectedData)
{
    CODEFIRST_RESULT result = CODEFIRST_OK;

    const REFLECTED_SOMETHING* something;
    for (something = reflectedData->reflectedData; something != NULL; something = something->next)
    {
        if (something->type == REFLECTION_STRUCT_TYPE)
        {
            SCHEMA_STRUCT_TYPE_HANDLE structTypeHandle;
            structTypeHandle = Schema_CreateStructType(schemaHandle, something->what.structure.name);

            if (structTypeHandle == NULL)
            {
                /*Codes_SRS_CODEFIRST_99_076:[If any Schema APIs fail, CODEFIRST_SCHEMA_ERROR shall be returned.]*/
                result = CODEFIRST_SCHEMA_ERROR;
                LogError("create struct failed %s\r\n", ENUM_TO_STRING(CODEFIRST_RESULT, result));
                break;
            }
            else
            {
                const REFLECTED_SOMETHING* maybeField;
                /*look for the field... */
                for (maybeField = reflectedData->reflectedData; maybeField != NULL; maybeField = maybeField->next)
                {
                    if (maybeField->type == REFLECTION_FIELD_TYPE)
                    {
                        if (strcmp(maybeField->what.field.structName, something->what.structure.name) == 0)
                        {
                            if (Schema_AddStructTypeProperty(structTypeHandle, maybeField->what.field.fieldName, maybeField->what.field.fieldType) != SCHEMA_OK)
                            {
                                /*Codes_SRS_CODEFIRST_99_076:[If any Schema APIs fail, CODEFIRST_SCHEMA_ERROR shall be returned.]*/
                                result = CODEFIRST_SCHEMA_ERROR;
                                LogError("add struct property failed %s\r\n", ENUM_TO_STRING(CODEFIRST_RESULT, result));
                                break;
                            }
                        }
                    }
                }
            }
        }
    }

    return result;
}

static CODEFIRST_RESULT buildModel(SCHEMA_HANDLE schemaHandle, const REFLECTED_DATA_FROM_DATAPROVIDER* reflectedData, const REFLECTED_SOMETHING* modelReflectedData)
{
    CODEFIRST_RESULT result = CODEFIRST_OK;
    const REFLECTED_SOMETHING* something;
    SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle;

    modelTypeHandle = Schema_GetModelByName(schemaHandle, modelReflectedData->what.model.name);
    if (modelTypeHandle == NULL)
    {
        /*Codes_SRS_CODEFIRST_99_076:[If any Schema APIs fail, CODEFIRST_SCHEMA_ERROR shall be returned.]*/
        result = CODEFIRST_SCHEMA_ERROR;
        LogError("cannot get model %s %s", modelReflectedData->what.model.name, ENUM_TO_STRING(CODEFIRST_RESULT, result));
        goto out;
    }

    for (something = reflectedData->reflectedData; something != NULL; something = something->next)
    {
        /* looking for all elements that belong to a model: properties and actions */
        if ((something->type == REFLECTION_PROPERTY_TYPE) &&
            (strcmp(something->what.property.modelName, modelReflectedData->what.model.name) == 0))
        {
            SCHEMA_MODEL_TYPE_HANDLE childModelHande = Schema_GetModelByName(schemaHandle, something->what.property.type);

            /* if this is a model type use the appropriate APIs for it */
            if (childModelHande != NULL)
            {
                if (Schema_AddModelModel(modelTypeHandle, something->what.property.name, childModelHande) != SCHEMA_OK)
                {
                    /*Codes_SRS_CODEFIRST_99_076:[If any Schema APIs fail, CODEFIRST_SCHEMA_ERROR shall be returned.]*/
                    result = CODEFIRST_SCHEMA_ERROR;
                    LogError("add model failed %s", ENUM_TO_STRING(CODEFIRST_RESULT, result));
                    goto out;
                }
            }
            else
            {
                if (Schema_AddModelProperty(modelTypeHandle, something->what.property.name, something->what.property.type) != SCHEMA_OK)
                {
                    /*Codes_SRS_CODEFIRST_99_076:[If any Schema APIs fail, CODEFIRST_SCHEMA_ERROR shall be returned.]*/
                    result = CODEFIRST_SCHEMA_ERROR;
                    LogError("add property failed %s", ENUM_TO_STRING(CODEFIRST_RESULT, result));
                    goto out;
                }
            }
        }

        if ((something->type == REFLECTION_ACTION_TYPE) &&
            (strcmp(something->what.action.modelName, modelReflectedData->what.model.name) == 0))
        {
            SCHEMA_ACTION_HANDLE actionHandle;
            size_t i;

            if ((actionHandle = Schema_CreateModelAction(modelTypeHandle, something->what.action.name)) == NULL)
            {
                /*Codes_SRS_CODEFIRST_99_076:[If any Schema APIs fail, CODEFIRST_SCHEMA_ERROR shall be returned.]*/
                result = CODEFIRST_SCHEMA_ERROR;
                LogError("add model action failed %s", ENUM_TO_STRING(CODEFIRST_RESULT, result));
                goto out;
            }

            for (i = 0; i < something->what.action.nArguments; i++)
            {
                if (Schema_AddModelActionArgument(actionHandle, something->what.action.arguments[i].name, something->what.action.arguments[i].type) != SCHEMA_OK)
                {
                    /*Codes_SRS_CODEFIRST_99_076:[If any Schema APIs fail, CODEFIRST_SCHEMA_ERROR shall be returned.]*/
                    result = CODEFIRST_SCHEMA_ERROR;
                    LogError("add model action argument failed %s", ENUM_TO_STRING(CODEFIRST_RESULT, result));
                    goto out;
                }
            }
        }
    }

out:
    return result;
}

static CODEFIRST_RESULT buildModelTypes(SCHEMA_HANDLE schemaHandle, const REFLECTED_DATA_FROM_DATAPROVIDER* reflectedData)
{
    CODEFIRST_RESULT result = CODEFIRST_OK;
    const REFLECTED_SOMETHING* something;

    /* first have a pass and add all the model types */
    for (something = reflectedData->reflectedData; something != NULL; something = something->next)
    {
        if (something->type == REFLECTION_MODEL_TYPE)
        {
            if (Schema_CreateModelType(schemaHandle, something->what.model.name) == NULL)
            {
                /*Codes_SRS_CODEFIRST_99_076:[If any Schema APIs fail, CODEFIRST_SCHEMA_ERROR shall be returned.]*/
                result = CODEFIRST_SCHEMA_ERROR;
                LogError("create mdoel failed %s", ENUM_TO_STRING(CODEFIRST_RESULT, result));
                goto out;
            }
        }
    }

    for (something = reflectedData->reflectedData; something != NULL; something = something->next)
    {
        if (something->type == REFLECTION_MODEL_TYPE)
        {
            result = buildModel(schemaHandle, reflectedData, something);
            if (result != CODEFIRST_OK)
            {
                break;
            }
        }
    }

out:
    return result;
}

/*Codes_SRS_CODEFIRST_99_002:[ CodeFirst_Init shall initialize the CodeFirst module. If initialization is successful, it shall return CODEFIRST_OK.]*/
CODEFIRST_RESULT CodeFirst_Init(const char* overrideSchemaNamespace)
{
    /*shall build the default EntityContainer*/
    CODEFIRST_RESULT result;

    if (g_state != CODEFIRST_STATE_NOT_INIT)
    {
        /*Codes_SRS_CODEFIRST_99_003:[ If the module is already initialized, the initialization shall fail and the return value shall be CODEFIRST_ALREADY_INIT.]*/
        result = CODEFIRST_ALREADY_INIT;
        LogError("CodeFirst was already init %s", ENUM_TO_STRING(CODEFIRST_RESULT, result));
    }
    else
    {
        g_DeviceCount = 0;
        g_OverrideSchemaNamespace = overrideSchemaNamespace;
        g_Devices = NULL;

        /*Codes_SRS_CODEFIRST_99_002:[ CodeFirst_Init shall initialize the CodeFirst module. If initialization is successful, it shall return CODEFIRST_OK.]*/
        g_state = CODEFIRST_STATE_INIT;
        result = CODEFIRST_OK;
    }

    return result;
}

void CodeFirst_Deinit(void)
{
    /*Codes_SRS_CODEFIRST_99_006:[If the module is not previously initialed, CodeFirst_Deinit shall do nothing.]*/
    if (g_state != CODEFIRST_STATE_INIT)
    {
        LogError("CodeFirst_Deinit called when CodeFirst was not INIT\r\n");
    }
    else
    {
        size_t i;

        /*Codes_SRS_CODEFIRST_99_005:[ CodeFirst_Deinit shall deinitialize the module, freeing all the resources and placing the module in an uninitialized state.]*/
        for (i = 0; i < g_DeviceCount; i++)
        {
            DestroyDevice(g_Devices[i]);
        }

        free(g_Devices);
        g_Devices = NULL;
        g_DeviceCount = 0;

        g_state = CODEFIRST_STATE_NOT_INIT;
    }
}

static const REFLECTED_SOMETHING* FindModelInCodeFirstMetadata(const REFLECTED_SOMETHING* reflectedData, const char* modelName)
{
    const REFLECTED_SOMETHING* result;

    for (result = reflectedData; result != NULL; result = result->next)
    {
        if ((result->type == REFLECTION_MODEL_TYPE) &&
            (strcmp(result->what.model.name, modelName) == 0))
        {
            /* found model type */
            break;
        }
    }

    return result;
}

static const REFLECTED_SOMETHING* FindChildModelInCodeFirstMetadata(const REFLECTED_SOMETHING* reflectedData, const REFLECTED_SOMETHING* startModel, const char* relativePath, size_t* offset)
{
    const REFLECTED_SOMETHING* result = startModel;
    *offset = 0;

    /* Codes_SRS_CODEFIRST_99_139:[If the relativeActionPath is empty then the action shall be looked up in the device model.] */
    while ((*relativePath != 0) && (result != NULL))
    {
        /* Codes_SRS_CODEFIRST_99_142:[The relativeActionPath argument shall be in the format "childModel1/childModel2/.../childModelN".] */
        const REFLECTED_SOMETHING* childModelProperty;
        size_t propertyNameLength;
        const char* slashPos = strchr(relativePath, '/');
        if (slashPos == NULL)
        {
            slashPos = &relativePath[strlen(relativePath)];
        }

        propertyNameLength = slashPos - relativePath;

        for (childModelProperty = reflectedData; childModelProperty != NULL; childModelProperty = childModelProperty->next)
        {
            if ((childModelProperty->type == REFLECTION_PROPERTY_TYPE) &&
                (strcmp(childModelProperty->what.property.modelName, result->what.model.name) == 0) &&
                (strncmp(childModelProperty->what.property.name, relativePath, propertyNameLength) == 0) &&
                (strlen(childModelProperty->what.property.name) == propertyNameLength))
            {
                /* property found, now let's find the model */
                /* Codes_SRS_CODEFIRST_99_140:[CodeFirst_InvokeAction shall pass to the action wrapper that it calls a pointer to the model where the action is defined.] */
                *offset += childModelProperty->what.property.offset;
                break;
            }
        }

        if (childModelProperty == NULL)
        {
            /* not found */
            result = NULL;
        }
        else
        {
            result = FindModelInCodeFirstMetadata(reflectedData, childModelProperty->what.property.type);
        }

        relativePath = slashPos;
    }

    return result;
}

EXECUTE_COMMAND_RESULT CodeFirst_InvokeAction(void* deviceHandle, void* callbackUserContext, const char* relativeActionPath, const char* actionName, size_t parameterCount, const AGENT_DATA_TYPE* parameterValues)
{
    EXECUTE_COMMAND_RESULT result;
    DEVICE_HEADER_DATA* deviceHeader = (DEVICE_HEADER_DATA*)callbackUserContext;

    /*Codes_SRS_CODEFIRST_99_068:[ If the function is called before CodeFirst is initialized then EXECUTE_COMMAND_ERROR shall be returned.] */
    if (g_state != CODEFIRST_STATE_INIT)
    {
        result = EXECUTE_COMMAND_ERROR;
        LogError("CodeFirst_InvokeAction called before init has an error %s \r\n", ENUM_TO_STRING(EXECUTE_COMMAND_RESULT, result));
    }
    /*Codes_SRS_CODEFIRST_99_066:[ If actionName, relativeActionPath or deviceHandle is NULL then EXECUTE_COMMAND_ERROR shall be returned*/
    else if ((actionName == NULL) ||
        (deviceHandle == NULL) ||
        (relativeActionPath == NULL))
    {
        result = EXECUTE_COMMAND_ERROR;
        LogError("action Name is NULL %s \r\n", ENUM_TO_STRING(EXECUTE_COMMAND_RESULT, result));
    }
    /*Codes_SRS_CODEFIRST_99_067:[ If parameterCount is greater than zero and parameterValues is NULL then EXECUTE_COMMAND_ERROR shall be returned.]*/
    else if ((parameterCount > 0) && (parameterValues == NULL))
    {
        result = EXECUTE_COMMAND_ERROR;
        LogError("parameterValues error %s \r\n", ENUM_TO_STRING(EXECUTE_COMMAND_RESULT, result));
    }
    else
    {
        const REFLECTED_SOMETHING* something;
        const REFLECTED_SOMETHING* childModel;
        const char* modelName;
        size_t offset;

        modelName = Schema_GetModelName(deviceHeader->ModelHandle);

        if (((childModel = FindModelInCodeFirstMetadata(deviceHeader->ReflectedData->reflectedData, modelName)) == NULL) ||
            /* Codes_SRS_CODEFIRST_99_138:[The relativeActionPath argument shall be used by CodeFirst_InvokeAction to find the child model where the action is declared.] */
            ((childModel = FindChildModelInCodeFirstMetadata(deviceHeader->ReflectedData->reflectedData, childModel, relativeActionPath, &offset)) == NULL))
        {
            /*Codes_SRS_CODEFIRST_99_141:[If a child model specified in the relativeActionPath argument cannot be found by CodeFirst_InvokeAction, it shall return EXECUTE_COMMAND_ERROR.] */
            result = EXECUTE_COMMAND_ERROR;
            LogError("action %s was not found %s \r\n", actionName, ENUM_TO_STRING(EXECUTE_COMMAND_RESULT, result));
        }
        else
        {
            /* Codes_SRS_CODEFIRST_99_062:[ When CodeFirst_InvokeAction is called it shall look through the codefirst metadata associated with a specific device for a previously declared action (function) named actionName.]*/
            /* Codes_SRS_CODEFIRST_99_078:[If such a function is not found then the function shall return EXECUTE_COMMAND_ERROR.]*/
            result = EXECUTE_COMMAND_ERROR;
            for (something = deviceHeader->ReflectedData->reflectedData; something != NULL; something = something->next)
            {
                if ((something->type == REFLECTION_ACTION_TYPE) &&
                    (strcmp(actionName, something->what.action.name) == 0) &&
                    (strcmp(childModel->what.model.name, something->what.action.modelName) == 0))
                {
                    /*Codes_SRS_CODEFIRST_99_063:[ If the function is found, then CodeFirst shall call the wrapper of the found function inside the data provider. The wrapper is linked in the reflected data to the function name. The wrapper shall be called with the same arguments as CodeFirst_InvokeAction has been called.]*/
                    /*Codes_SRS_CODEFIRST_99_064:[ If the wrapper call succeeds then CODEFIRST_OK shall be returned. ]*/
                    /*Codes_SRS_CODEFIRST_99_065:[ For all the other return values CODEFIRST_ACTION_EXECUTION_ERROR shall be returned.]*/
                    /* Codes_SRS_CODEFIRST_99_140:[CodeFirst_InvokeAction shall pass to the action wrapper that it calls a pointer to the model where the action is defined.] */
                    /*Codes_SRS_CODEFIRST_02_013: [The wrapper's return value shall be returned.]*/
                    result = something->what.action.wrapper(deviceHeader->data + offset, parameterCount, parameterValues);
                    break;
                }
            }
        }
    }

    if (result == EXECUTE_COMMAND_ERROR)
    {
        LogError(" %s \r\n", ENUM_TO_STRING(EXECUTE_COMMAND_RESULT, result));
    }
    return result;
}

/* Codes_SRS_CODEFIRST_99_002:[ CodeFirst_RegisterSchema shall create the schema information and give it to the Schema module for one schema, identified by the metadata argument. On success, it shall return a handle to the schema.] */
SCHEMA_HANDLE CodeFirst_RegisterSchema(const char* schemaNamespace, const REFLECTED_DATA_FROM_DATAPROVIDER* metadata)
{
    SCHEMA_HANDLE result;

    if (g_OverrideSchemaNamespace != NULL)
    {
        schemaNamespace = g_OverrideSchemaNamespace;
    }

    /* Codes_SRS_CODEFIRST_99_121:[If the schema has already been registered, CodeFirst_RegisterSchema shall return its handle.] */
    result = Schema_GetSchemaByNamespace(schemaNamespace);
    if (result == NULL)
    {
        if ((result = Schema_Create(schemaNamespace)) == NULL)
        {
            /* Codes_SRS_CODEFIRST_99_076:[If any Schema APIs fail, CodeFirst_RegisterSchema shall return NULL.] */
            result = NULL;
            LogError("schema init failed %s\r\n", ENUM_TO_STRING(CODEFIRST_RESULT, CODEFIRST_SCHEMA_ERROR));
        }
        else
        {
            if ((buildStructTypes(result, metadata) != CODEFIRST_OK) ||
                (buildModelTypes(result, metadata) != CODEFIRST_OK))
            {
                Schema_Destroy(result);
                result = NULL;
            }
            else
            {
                /* do nothing, everything is OK */
            }
        }
    }

    return result;
}

AGENT_DATA_TYPE_TYPE CodeFirst_GetPrimitiveType(const char* typeName)
{
    if (strcmp(typeName, "double") == 0)
    {
        return EDM_DOUBLE_TYPE;
    }
    else if (strcmp(typeName, "float") == 0)
    {
        return EDM_SINGLE_TYPE;
    }
    else if (strcmp(typeName, "int") == 0)
    {
        return EDM_INT32_TYPE;
    }
    else if (strcmp(typeName, "long") == 0)
    {
        return EDM_INT64_TYPE;
    }
    else if (strcmp(typeName, "int8_t") == 0)
    {
        return EDM_SBYTE_TYPE;
    }
    else if (strcmp(typeName, "uint8_t") == 0)
    {
        return EDM_BYTE_TYPE;
    }
    else if (strcmp(typeName, "int16_t") == 0)
    {
        return EDM_INT16_TYPE;
    }
    else if (strcmp(typeName, "int32_t") == 0)
    {
        return EDM_INT32_TYPE;
    }
    else if (strcmp(typeName, "int64_t") == 0)
    {
        return EDM_INT64_TYPE;
    }
    else if (
        (strcmp(typeName, "_Bool") == 0) ||
        (strcmp(typeName, "bool") == 0)
        )
    {
        return EDM_BOOLEAN_TYPE;
    }
    else if (strcmp(typeName, "ascii_char_ptr") == 0)
    {
        return EDM_STRING_TYPE;
    }
    else if (strcmp(typeName, "ascii_char_ptr_no_quotes") == 0)
    {
        return EDM_STRING_NO_QUOTES_TYPE;
    }
    else if (strcmp(typeName, "EDM_DATE_TIME_OFFSET") == 0)
    {
        return EDM_DATE_TIME_OFFSET_TYPE;
    }
    else if (strcmp(typeName, "EDM_GUID") == 0)
    {
        return EDM_GUID_TYPE;
    }
    else if (strcmp(typeName, "EDM_BINARY") == 0)
    {
        return EDM_BINARY_TYPE;
    }
    else
    {
        return EDM_NO_TYPE;
    }
}

/* Codes_SRS_CODEFIRST_99_079:[CodeFirst_CreateDevice shall create a device and allocate a memory block that should hold the device data.] */
void* CodeFirst_CreateDevice(SCHEMA_MODEL_TYPE_HANDLE model, const REFLECTED_DATA_FROM_DATAPROVIDER* metadata, size_t dataSize, bool includePropertyPath)
{
    void* result;
    DEVICE_HEADER_DATA* deviceHeader;

    /* Codes_SRS_CODEFIRST_99_080:[If CodeFirst_CreateDevice is invoked with a NULL model, it shall return NULL.]*/
    if (
        (model == NULL))
    {
        result = NULL;
        LogError(" %s \r\n", ENUM_TO_STRING(CODEFIRST_RESULT, CODEFIRST_INVALID_ARG));
    }
    /* Codes_SRS_CODEFIRST_99_106:[If CodeFirst_CreateDevice is called when the modules is not initialized is shall return NULL.] */
    else if (g_state != CODEFIRST_STATE_INIT)
    {
        result = NULL;
        LogError(" %s \r\n", ENUM_TO_STRING(CODEFIRST_RESULT, CODEFIRST_NOT_INIT));
    }
    else if ((deviceHeader = (DEVICE_HEADER_DATA*)malloc(sizeof(DEVICE_HEADER_DATA))) == NULL)
    {
        /* Codes_SRS_CODEFIRST_99_102:[On any other errors, Device_Create shall return NULL.] */
        result = NULL;
        LogError(" %s \r\n", ENUM_TO_STRING(CODEFIRST_RESULT, CODEFIRST_ERROR));
    }
    /* Codes_SRS_CODEFIRST_99_081:[CodeFirst_CreateDevice shall use Device_Create to create a device handle.] */
    /* Codes_SRS_CODEFIRST_99_082:[CodeFirst_CreateDevice shall pass to Device_Create the function CodeFirst_InvokeAction as action callback argument.] */
    else
    {
        if ((deviceHeader->data = malloc(dataSize))==NULL)
        {
            free(deviceHeader);
            deviceHeader = NULL;
            result = NULL;
            LogError(" %s \r\n", ENUM_TO_STRING(CODEFIRST_RESULT, CODEFIRST_ERROR));
        }
        else
        {
            DEVICE_HEADER_DATA** newDevices;

            if (Device_Create(model, CodeFirst_InvokeAction, deviceHeader,
                includePropertyPath, &deviceHeader->DeviceHandle) != DEVICE_OK)
            {
                free(deviceHeader->data);
                free(deviceHeader);

                /* Codes_SRS_CODEFIRST_99_084:[If Device_Create fails, CodeFirst_CreateDevice shall return NULL.] */
                result = NULL;
                LogError(" %s \r\n", ENUM_TO_STRING(CODEFIRST_RESULT, CODEFIRST_DEVICE_FAILED));
            }
            else if ((newDevices = (DEVICE_HEADER_DATA**)realloc(g_Devices, sizeof(DEVICE_HEADER_DATA*) * (g_DeviceCount + 1))) == NULL)
            {
                Device_Destroy(deviceHeader->DeviceHandle);
                free(deviceHeader->data);
                free(deviceHeader);

                /* Codes_SRS_CODEFIRST_99_102:[On any other errors, Device_Create shall return NULL.] */
                result = NULL;
                LogError(" %s \r\n", ENUM_TO_STRING(CODEFIRST_RESULT, CODEFIRST_ERROR));
            }
            else
            {
                SCHEMA_RESULT schemaResult;
                deviceHeader->ReflectedData = metadata;
                deviceHeader->DataSize = dataSize;
                deviceHeader->ModelHandle = model;
                schemaResult = Schema_AddDeviceRef(model);
                if (schemaResult != SCHEMA_OK)
                {
                    Device_Destroy(deviceHeader->DeviceHandle);
                    free(deviceHeader->data);
                    free(deviceHeader);

                    /* Codes_SRS_CODEFIRST_99_102:[On any other errors, Device_Create shall return NULL.] */
                    result = NULL;
                }
                else
                {
                    g_Devices = newDevices;
                    g_Devices[g_DeviceCount] = deviceHeader;
                    g_DeviceCount++;

                    /* Codes_SRS_CODEFIRST_99_101:[On success, CodeFirst_CreateDevice shall return a non NULL pointer to the device data.] */
                    result = deviceHeader->data;
                }
            }
        }
    }

    return result;
}

void CodeFirst_DestroyDevice(void* device)
{
    /* Codes_SRS_CODEFIRST_99_086:[If the argument is NULL, CodeFirst_DestroyDevice shall do nothing.] */
    if (device != NULL)
    {
        size_t i;

        for (i = 0; i < g_DeviceCount; i++)
        {
            if (g_Devices[i]->data == device)
            {
                Schema_ReleaseDeviceRef(g_Devices[i]->ModelHandle);

                // Delete the Created Schema if all the devices are unassociated
                Schema_DestroyIfUnused(g_Devices[i]->ModelHandle);

                DestroyDevice(g_Devices[i]);
                memcpy(&g_Devices[i], &g_Devices[i + 1], (g_DeviceCount - i - 1) * sizeof(DEVICE_HEADER_DATA*));
                g_DeviceCount--;
                break;
            }
        }
    }
}

static DEVICE_HEADER_DATA* FindDevice(void* value)
{
    size_t i;
    DEVICE_HEADER_DATA* result = NULL;

    for (i = 0; i < g_DeviceCount; i++)
    {
        if ((g_Devices[i]->data <= (unsigned char*)value) &&
            (g_Devices[i]->data + g_Devices[i]->DataSize > (unsigned char*)value))
        {
            result = g_Devices[i];
            break;
        }
    }

    return result;
}

static const REFLECTED_SOMETHING* FindValue(DEVICE_HEADER_DATA* deviceHeader, void* value, const char* modelName, size_t startOffset, STRING_HANDLE valuePath)
{
    const REFLECTED_SOMETHING* result;
    size_t valueOffset = (size_t)((unsigned char*)value - (unsigned char*)deviceHeader->data) - startOffset;

    for (result = deviceHeader->ReflectedData->reflectedData; result != NULL; result = result->next)
    {
        if (result->type == REFLECTION_PROPERTY_TYPE &&
            (strcmp(result->what.property.modelName, modelName) == 0) &&
            (result->what.property.offset <= valueOffset) &&
            (result->what.property.offset + result->what.property.size > valueOffset))
        {
            if (startOffset != 0)
            {
                STRING_concat(valuePath, "/");
            }

            STRING_concat(valuePath, result->what.property.name);
            break;
        }
    }

    if (result != NULL)
    {
        /* Codes_SRS_CODEFIRST_99_133:[CodeFirst_SendAsync shall allow sending of properties that are part of a child model.] */
        if (result->what.property.offset < valueOffset)
        {
            /* find recursively the property in the inner model, if there is one */
            result = FindValue(deviceHeader, value, result->what.property.type, startOffset + result->what.property.offset, valuePath);
        }
    }

    return result;
}

/* Codes_SRS_CODEFIRST_99_130:[If a pointer to the beginning of a device block is passed to CodeFirst_SendAsync instead of a pointer to a property, CodeFirst_SendAsync shall send all the properties that belong to that device.] */
/* Codes_SRS_CODEFIRST_99_131:[The properties shall be given to Device as one transaction, as if they were all passed as individual arguments to Code_First.] */
static CODEFIRST_RESULT SendAllDeviceProperties(DEVICE_HEADER_DATA* deviceHeader, TRANSACTION_HANDLE transaction)
{
    const char* modelName = Schema_GetModelName(deviceHeader->ModelHandle);
    const REFLECTED_SOMETHING* something;
    unsigned char* deviceAddress = (unsigned char*)deviceHeader->data;
    CODEFIRST_RESULT result = CODEFIRST_OK;

    for (something = deviceHeader->ReflectedData->reflectedData; something != NULL; something = something->next)
    {
        if ((something->type == REFLECTION_PROPERTY_TYPE) &&
            (strcmp(something->what.property.modelName, modelName) == 0))
        {
            AGENT_DATA_TYPE agentDataType;

            /* Codes_SRS_CODEFIRST_99_097:[For each value marshalling to AGENT_DATA_TYPE shall be performed.] */
            /* Codes_SRS_CODEFIRST_99_098:[The marshalling shall be done by calling the Create_AGENT_DATA_TYPE_from_Ptr function associated with the property.] */
            if (something->what.property.Create_AGENT_DATA_TYPE_from_Ptr(deviceAddress + something->what.property.offset, &agentDataType) != AGENT_DATA_TYPES_OK)
            {
                /* Codes_SRS_CODEFIRST_99_099:[If Create_AGENT_DATA_TYPE_from_Ptr fails, CodeFirst_SendAsync shall return CODEFIRST_AGENT_DATA_TYPE_ERROR.] */
                result = CODEFIRST_AGENT_DATA_TYPE_ERROR;
                LOG_CODEFIRST_ERROR;
                break;
            }
            else
            {
                /* Codes_SRS_CODEFIRST_99_092:[CodeFirst shall publish each value by using Device_PublishTransacted.] */
                if (Device_PublishTransacted(transaction, something->what.property.name, &agentDataType) != DEVICE_OK)
                {
                    Destroy_AGENT_DATA_TYPE(&agentDataType);

                    /* Codes_SRS_CODEFIRST_99_094:[If any Device API fail, CodeFirst_SendAsync shall return CODEFIRST_DEVICE_PUBLISH_FAILED.] */
                    result = CODEFIRST_DEVICE_PUBLISH_FAILED;
                    LOG_CODEFIRST_ERROR;
                    break;
                }

                Destroy_AGENT_DATA_TYPE(&agentDataType);
            }
        }
    }

    return result;
}

/* Codes_SRS_CODEFIRST_99_088:[CodeFirst_SendAsync shall send to the Device module a set of properties, a destination and a destinationSize.]*/
CODEFIRST_RESULT CodeFirst_SendAsync(unsigned char** destination, size_t* destinationSize, size_t numProperties, ...)
{
    CODEFIRST_RESULT result;
    va_list ap;

    if (
        (numProperties == 0) || 
        (destination == NULL) || 
        (destinationSize == NULL)
        )
    {
        /* Codes_SRS_CODEFIRST_04_002: [If CodeFirst_SendAsync receives destination or destinationSize NULL, CodeFirst_SendAsync shall return Invalid Argument.]*/
        /* Codes_SRS_CODEFIRST_99_103:[If CodeFirst_SendAsync is called with numProperties being zero, CODEFIRST_INVALID_ARG shall be returned.] */
        result = CODEFIRST_INVALID_ARG;
        LOG_CODEFIRST_ERROR;
    }
    else
    {
        DEVICE_HEADER_DATA* deviceHeader = NULL;
        size_t i;
        TRANSACTION_HANDLE transaction = NULL;
        result = CODEFIRST_OK;

        /* Codes_SRS_CODEFIRST_99_105:[The properties are passed as pointers to the memory locations where the data exists in the device block allocated by CodeFirst_CreateDevice.] */
        va_start(ap, numProperties);

        /* Codes_SRS_CODEFIRST_99_089:[The numProperties argument shall indicate how many properties are to be sent.] */
        for (i = 0; i < numProperties; i++)
        {
            void* value = (void*)va_arg(ap, void*);

            /* Codes_SRS_CODEFIRST_99_095:[For each value passed to it, CodeFirst_SendAsync shall look up to which device the value belongs.] */
            DEVICE_HEADER_DATA* currentValueDeviceHeader = FindDevice(value);
            if (currentValueDeviceHeader == NULL)
            {
                /* Codes_SRS_CODEFIRST_99_104:[If a property cannot be associated with a device, CodeFirst_SendAsync shall return CODEFIRST_INVALID_ARG.] */
                result = CODEFIRST_INVALID_ARG;
                LOG_CODEFIRST_ERROR;
                break;
            }
            else if ((deviceHeader != NULL) &&
                (currentValueDeviceHeader != deviceHeader))
            {
                /* Codes_SRS_CODEFIRST_99_096:[All values have to belong to the same device, otherwise CodeFirst_SendAsync shall return CODEFIRST_VALUES_FROM_DIFFERENT_DEVICES_ERROR.] */
                result = CODEFIRST_VALUES_FROM_DIFFERENT_DEVICES_ERROR;
                LOG_CODEFIRST_ERROR;
                break;
            }
            /* Codes_SRS_CODEFIRST_99_090:[All the properties shall be sent together by using the transacted APIs of the device.] */
            /* Codes_SRS_CODEFIRST_99_091:[CodeFirst_SendAsync shall start a transaction by calling Device_StartTransaction.] */
            else if ((deviceHeader == NULL) &&
                ((transaction = Device_StartTransaction(currentValueDeviceHeader->DeviceHandle)) == NULL))
            {
                /* Codes_SRS_CODEFIRST_99_094:[If any Device API fail, CodeFirst_SendAsync shall return CODEFIRST_DEVICE_PUBLISH_FAILED.] */
                result = CODEFIRST_DEVICE_PUBLISH_FAILED;
                LOG_CODEFIRST_ERROR;
                break;
            }
            else
            {
                deviceHeader = currentValueDeviceHeader;

                if (value == ((unsigned char*)deviceHeader->data))
                {
                    /* we got a full device, send all its state data */
                    result = SendAllDeviceProperties(deviceHeader, transaction);
                    if (result != CODEFIRST_OK)
                    {
                        LOG_CODEFIRST_ERROR;
                        break;
                    }
                }
                else
                {
                    const REFLECTED_SOMETHING* propertyReflectedData;
                    const char* modelName;
                    STRING_HANDLE valuePath;

                    if ((valuePath = STRING_new()) == NULL)
                    {
                        /* Codes_SRS_CODEFIRST_99_134:[If CodeFirst_Notify fails for any other reason it shall return CODEFIRST_ERROR.] */
                        result = CODEFIRST_ERROR;
                        LOG_CODEFIRST_ERROR;
                        break;
                    }
                    else
                    {
                        if ((modelName = Schema_GetModelName(deviceHeader->ModelHandle)) == NULL)
                        {
                            /* Codes_SRS_CODEFIRST_99_134:[If CodeFirst_Notify fails for any other reason it shall return CODEFIRST_ERROR.] */
                            result = CODEFIRST_ERROR;
                            LOG_CODEFIRST_ERROR;
                            STRING_delete(valuePath);
                            break;
                        }
                        else if ((propertyReflectedData = FindValue(deviceHeader, value, modelName, 0, valuePath)) == NULL)
                        {
                            /* Codes_SRS_CODEFIRST_99_104:[If a property cannot be associated with a device, CodeFirst_SendAsync shall return CODEFIRST_INVALID_ARG.] */
                            result = CODEFIRST_INVALID_ARG;
                            LOG_CODEFIRST_ERROR;
                            STRING_delete(valuePath);
                            break;
                        }
                        else
                        {
                            AGENT_DATA_TYPE agentDataType;

                            /* Codes_SRS_CODEFIRST_99_097:[For each value marshalling to AGENT_DATA_TYPE shall be performed.] */
                            /* Codes_SRS_CODEFIRST_99_098:[The marshalling shall be done by calling the Create_AGENT_DATA_TYPE_from_Ptr function associated with the property.] */
                            if (propertyReflectedData->what.property.Create_AGENT_DATA_TYPE_from_Ptr(value, &agentDataType) != AGENT_DATA_TYPES_OK)
                            {
                                /* Codes_SRS_CODEFIRST_99_099:[If Create_AGENT_DATA_TYPE_from_Ptr fails, CodeFirst_SendAsync shall return CODEFIRST_AGENT_DATA_TYPE_ERROR.] */
                                result = CODEFIRST_AGENT_DATA_TYPE_ERROR;
                                LOG_CODEFIRST_ERROR;
                                STRING_delete(valuePath);
                                break;
                            }
                            else
                            {
                                /* Codes_SRS_CODEFIRST_99_092:[CodeFirst shall publish each value by using Device_PublishTransacted.] */
                                /* Codes_SRS_CODEFIRST_99_136:[CodeFirst_SendAsync shall build the full path for each property and then pass it to Device_PublishTransacted.] */
                                if (Device_PublishTransacted(transaction, STRING_c_str(valuePath), &agentDataType) != DEVICE_OK)
                                {
                                    Destroy_AGENT_DATA_TYPE(&agentDataType);

                                    /* Codes_SRS_CODEFIRST_99_094:[If any Device API fail, CodeFirst_SendAsync shall return CODEFIRST_DEVICE_PUBLISH_FAILED.] */
                                    result = CODEFIRST_DEVICE_PUBLISH_FAILED;
                                    LOG_CODEFIRST_ERROR;
                                    STRING_delete(valuePath);
                                    break;
                                }
                                else
                                {
                                    STRING_delete(valuePath); /*anyway*/
                                }

                                Destroy_AGENT_DATA_TYPE(&agentDataType);
                            }
                        }
                    }
                }
            }
        }

        if (i < numProperties)
        {
            if (transaction != NULL)
            {
                (void)Device_CancelTransaction(transaction);
            }
        }
        /* Codes_SRS_CODEFIRST_99_093:[After all values have been published, Device_EndTransaction shall be called.] */
        else if (Device_EndTransaction(transaction, destination, destinationSize) != DEVICE_OK)
        {
            /* Codes_SRS_CODEFIRST_99_094:[If any Device API fail, CodeFirst_SendAsync shall return CODEFIRST_DEVICE_PUBLISH_FAILED.] */
            result = CODEFIRST_DEVICE_PUBLISH_FAILED;
            LOG_CODEFIRST_ERROR;
        }
        else
        {
            /* Codes_SRS_CODEFIRST_99_117:[On success, CodeFirst_SendAsync shall return CODEFIRST_OK.] */
            result = CODEFIRST_OK;
        }

        va_end(ap);
    }

    return result;
}

EXECUTE_COMMAND_RESULT CodeFirst_ExecuteCommand(void* device, const char* command)
{
    EXECUTE_COMMAND_RESULT result;
    /*Codes_SRS_CODEFIRST_02_014: [If parameter device or command is NULL then CodeFirst_ExecuteCommand shall return EXECUTE_COMMAND_ERROR.] */
    if (
        (device == NULL) ||
        (command == NULL)
        )
    {
        result = EXECUTE_COMMAND_ERROR;
        LogError("invalid argument (NULL) passed to CodeFirst_ExecuteCommand void* device = %p, const char* command = %p\r\n", device, command);
    }
    else
    {
        /*Codes_SRS_CODEFIRST_02_015: [CodeFirst_ExecuteCommand shall find the device.]*/
        DEVICE_HEADER_DATA* deviceHeader = FindDevice(device);
        if(deviceHeader == NULL)
        {
            /*Codes_SRS_CODEFIRST_02_016: [If finding the device fails, then CodeFirst_ExecuteCommand shall return EXECUTE_COMMAND_ERROR.]*/
            result = EXECUTE_COMMAND_ERROR;
            LogError("unable to find the device given by address %p\r\n", device);
        }
        else
        {
            /*Codes_SRS_CODEFIRST_02_017: [Otherwise CodeFirst_ExecuteCommand shall call Device_ExecuteCommand and return what Device_ExecuteCommand is returning.] */
            result = Device_ExecuteCommand(deviceHeader->DeviceHandle, command);
        }
    }
    return result;
}