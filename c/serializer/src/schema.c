// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#include "gballoc.h"

#include "schema.h"
#include "crt_abstractions.h"
#include "iot_logging.h"
#include "vector.h"


DEFINE_ENUM_STRINGS(SCHEMA_RESULT, SCHEMA_RESULT_VALUES);

typedef struct PROPERTY_TAG
{
    const char* PropertyName;
    const char* PropertyType;
} PROPERTY;

typedef struct SCHEMA_ACTION_ARGUMENT_TAG
{
    const char* Name;
    const char* Type;
} SCHEMA_ACTION_ARGUMENT;

typedef struct ACTION_TAG
{
    const char* ActionName;
    size_t ArgumentCount;
    SCHEMA_ACTION_ARGUMENT_HANDLE* ArgumentHandles;
} ACTION;

typedef struct MODEL_IN_MODEL_TAG
{
    const char* propertyName;
    SCHEMA_MODEL_TYPE_HANDLE modelHandle;
} MODEL_IN_MODEL;

typedef struct MODEL_TYPE_TAG
{
    const char* Name;
    SCHEMA_HANDLE SchemaHandle;
    SCHEMA_PROPERTY_HANDLE* Properties;
    size_t PropertyCount;
    SCHEMA_ACTION_HANDLE* Actions;
    size_t ActionCount;
    VECTOR_HANDLE models;
    size_t DeviceCount;
} MODEL_TYPE;

typedef struct STRUCT_TYPE_TAG
{
    const char* Name;
    SCHEMA_PROPERTY_HANDLE* Properties;
    size_t PropertyCount;
} STRUCT_TYPE;

typedef struct SCHEMA_TAG
{
    const char* Namespace;
    SCHEMA_MODEL_TYPE_HANDLE* ModelTypes;
    size_t ModelTypeCount;
    SCHEMA_STRUCT_TYPE_HANDLE* StructTypes;
    size_t StructTypeCount;
} SCHEMA;

static VECTOR_HANDLE g_schemas = NULL;

static void DestroyProperty(SCHEMA_PROPERTY_HANDLE propertyHandle)
{
    PROPERTY* propertyType = (PROPERTY*)propertyHandle;
    free((void*)propertyType->PropertyName);
    free((void*)propertyType->PropertyType);
    free(propertyType);
}

static void DestroyActionArgument(SCHEMA_ACTION_ARGUMENT_HANDLE actionArgumentHandle)
{
    SCHEMA_ACTION_ARGUMENT* actionArgument = (SCHEMA_ACTION_ARGUMENT*)actionArgumentHandle;
    if (actionArgument != NULL)
    {
        free((void*)actionArgument->Name);
        free((void*)actionArgument->Type);
        free(actionArgument);
    }
}

static void DestroyAction(SCHEMA_ACTION_HANDLE actionHandle)
{
    ACTION* action = (ACTION*)actionHandle;
    if (action != NULL)
    {
        size_t j;
        
        for (j = 0; j < action->ArgumentCount; j++)
        {
            DestroyActionArgument(action->ArgumentHandles[j]);
        }
        free(action->ArgumentHandles);

        free((void*)action->ActionName);
        free(action);
    }
}

static void DestroyStruct(SCHEMA_STRUCT_TYPE_HANDLE structTypeHandle)
{
    size_t i;
    STRUCT_TYPE* structType = (STRUCT_TYPE*)structTypeHandle;
    if (structType != NULL)
    {
        for (i = 0; i < structType->PropertyCount; i++)
        {
            DestroyProperty(structType->Properties[i]);
        }
        free(structType->Properties);

        free((void*)structType->Name);

        free(structType);
    }
}

static void DestroyModel(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle)
{
    MODEL_TYPE* modelType = (MODEL_TYPE*)modelTypeHandle;
    size_t i;

    free((void*)modelType->Name);
    modelType->Name = NULL;

    for (i = 0; i < modelType->PropertyCount; i++)
    {
        DestroyProperty(modelType->Properties[i]);
    }

    free(modelType->Properties);

    for (i = 0; i < modelType->ActionCount; i++)
    {
        DestroyAction(modelType->Actions[i]);
    }

    /*destroy the vector holding the added models. This does not destroy the said models, however, their names shall be*/
    for (i = 0; i < VECTOR_size(modelType->models); i++)
    {
        MODEL_IN_MODEL* temp = (MODEL_IN_MODEL*)VECTOR_element(modelType->models, i);
        free((void*)temp->propertyName);
    }
    VECTOR_clear(modelType->models);
    VECTOR_destroy(modelType->models);

    free(modelType->Actions);
    free(modelType);
}

static SCHEMA_RESULT AddModelProperty(MODEL_TYPE* modelType, const char* name, const char* type)
{
    SCHEMA_RESULT result;

    /* Codes_SRS_SCHEMA_99_013:[If any of the arguments is NULL, Schema_AddModelProperty shall return SCHEMA_INVALID_ARG.] */
    if ((modelType == NULL) ||
        (name == NULL) ||
        (type == NULL))
    {
        result = SCHEMA_INVALID_ARG;
        LogError("(result = %s)\r\n", ENUM_TO_STRING(SCHEMA_RESULT, result));
    }
    else
    {
        size_t i;

        /* Codes_SRS_SCHEMA_99_015:[The property name shall be unique per model, if the same property name is added twice to a model, SCHEMA_DUPLICATE_ELEMENT shall be returned.] */
        for (i = 0; i < modelType->PropertyCount; i++)
        {
            PROPERTY* property = (PROPERTY*)modelType->Properties[i];
            if (strcmp(property->PropertyName, name) == 0)
            {
                break;
            }
        }

        if (i < modelType->PropertyCount)
        {
            result = SCHEMA_DUPLICATE_ELEMENT;
            LogError("(result = %s)\r\n", ENUM_TO_STRING(SCHEMA_RESULT, result));
        }
        else
        {
            SCHEMA_PROPERTY_HANDLE* newProperties = (SCHEMA_PROPERTY_HANDLE*)realloc(modelType->Properties, sizeof(SCHEMA_PROPERTY_HANDLE) * (modelType->PropertyCount + 1));
            if (newProperties == NULL)
            {
                /* Codes_SRS_SCHEMA_99_014:[On any other error, Schema_AddModelProperty shall return SCHEMA_ERROR.] */
                result = SCHEMA_ERROR;
                LogError("(result = %s)\r\n", ENUM_TO_STRING(SCHEMA_RESULT, result));
            }
            else
            {
                PROPERTY* newProperty;

                modelType->Properties = newProperties;
                if ((newProperty = (PROPERTY*)malloc(sizeof(PROPERTY))) == NULL)
                {
                    /* Codes_SRS_SCHEMA_99_014:[On any other error, Schema_AddModelProperty shall return SCHEMA_ERROR.] */
                    result = SCHEMA_ERROR;
                    LogError("(result = %s)\r\n", ENUM_TO_STRING(SCHEMA_RESULT, result));
                }
                else
                {
                    if (mallocAndStrcpy_s((char**)&newProperty->PropertyName, name) != 0)
                    {
                        /* Codes_SRS_SCHEMA_99_014:[On any other error, Schema_AddModelProperty shall return SCHEMA_ERROR.] */
                        free(newProperty);
                        result = SCHEMA_ERROR;
                        LogError("(result = %s)\r\n", ENUM_TO_STRING(SCHEMA_RESULT, result));
                    }
                    else if (mallocAndStrcpy_s((char**)&newProperty->PropertyType, type) != 0)
                    {
                        /* Codes_SRS_SCHEMA_99_014:[On any other error, Schema_AddModelProperty shall return SCHEMA_ERROR.] */
                        free((void*)newProperty->PropertyName);
                        free(newProperty);
                        result = SCHEMA_ERROR;
                        LogError("(result = %s)\r\n", ENUM_TO_STRING(SCHEMA_RESULT, result));
                    }
                    else
                    {
                        modelType->Properties[modelType->PropertyCount] = (SCHEMA_PROPERTY_HANDLE)newProperty;
                        modelType->PropertyCount++;

                        /* Codes_SRS_SCHEMA_99_012:[On success, Schema_AddModelProperty shall return SCHEMA_OK.] */
                        result = SCHEMA_OK;
                    }
                }

                /* If possible, reduce the memory of over allocation */
                if (result != SCHEMA_OK)
                {
                    SCHEMA_PROPERTY_HANDLE* oldProperties = (SCHEMA_PROPERTY_HANDLE*)realloc(modelType->Properties, sizeof(SCHEMA_PROPERTY_HANDLE) * modelType->PropertyCount);
                    if (oldProperties == NULL)
                    {
                        result = SCHEMA_ERROR;
                        LogError("(result = %s)\r\n", ENUM_TO_STRING(SCHEMA_RESULT, result));
                    }
                    else
                    {
                        modelType->Properties = oldProperties;
                    }
                }
            }
        }
    }

    return result;
}

static bool SchemaHandlesMatch(const SCHEMA_HANDLE* handle, const SCHEMA_HANDLE* otherHandle)
{
    return (*handle == *otherHandle);
}

static bool SchemaNamespacesMatch(const SCHEMA_HANDLE* handle, const char* schemaNamespace)
{
    const SCHEMA* schema = (SCHEMA*)*handle;
    return (strcmp(schema->Namespace, schemaNamespace) == 0);
}

/* Codes_SRS_SCHEMA_99_001:[Schema_Create shall initialize a schema with a given namespace.] */
SCHEMA_HANDLE Schema_Create(const char* schemaNamespace)
{
    SCHEMA* result;

    /* Codes_SRS_SCHEMA_99_004:[If schemaNamespace is NULL, Schema_Create shall fail.] */
    if (schemaNamespace == NULL)
    {
        /* Codes_SRS_SCHEMA_99_003:[On failure, NULL shall be returned.] */
        result = NULL;
        LogError("(Error code:%s)\r\n", ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_INVALID_ARG));
    }
    else
    {
        if (g_schemas == NULL && (g_schemas = VECTOR_create(sizeof(SCHEMA*) ) ) == NULL)
        {
            /* Codes_SRS_SCHEMA_99_003:[On failure, NULL shall be returned.] */
            result = NULL;
            LogError("(Error code:%s)\r\n", ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_ERROR));
        }
        else if ((result = (SCHEMA*)malloc(sizeof(SCHEMA))) == NULL)
        {
            /* Codes_SRS_SCHEMA_99_003:[On failure, NULL shall be returned.] */
            LogError("(Error code:%s)\r\n", ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_ERROR));
        }
        else if (mallocAndStrcpy_s((char**)&result->Namespace, schemaNamespace) != 0)
        {
            /* Codes_SRS_SCHEMA_99_003:[On failure, NULL shall be returned.] */
            free(result);
            result = NULL;
            LogError("(Error code:%s)\r\n", ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_ERROR));
        }
        else if (VECTOR_push_back(g_schemas, &result, 1) != 0)
        {
            free((void*)result->Namespace);
            free(result);
            result = NULL;
            LogError("(Error code:%s)\r\n", ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_ERROR));
        }
        else
        {
            /* Codes_SRS_SCHEMA_99_002:[On success a non-NULL handle to the newly created schema shall be returned.] */
            result->ModelTypes = NULL;
            result->ModelTypeCount = 0;
            result->StructTypes = NULL;
            result->StructTypeCount = 0;
        }
    }

    return (SCHEMA_HANDLE)result;
}

size_t Schema_GetSchemaCount(void)
{
    /* Codes_SRS_SCHEMA_99_153: [Schema_GetSchemaCount shall return the number of "active" schemas (all schemas created with Schema_Create
    in the current process, for which Schema_Destroy has not been called).] */
    return VECTOR_size(g_schemas);
}

SCHEMA_HANDLE Schema_GetSchemaByNamespace(const char* schemaNamespace)
{
    /* Codes_SRS_SCHEMA_99_151: [If no active schema matches the schemaNamespace argument, Schema_GetSchemaByNamespace shall return NULL.] */
    SCHEMA_HANDLE result = NULL;

    /* Codes_SRS_SCHEMA_99_150: [If the schemaNamespace argument is NULL, Schema_GetSchemaByNamespace shall return NULL.] */
    if (schemaNamespace != NULL)
    {
        SCHEMA_HANDLE* handle = (SCHEMA_HANDLE*)VECTOR_find_if(g_schemas, (PREDICATE_FUNCTION)SchemaNamespacesMatch, schemaNamespace);
        if (handle != NULL)
        {
            /* Codes_SRS_SCHEMA_99_148: [Schema_GetSchemaByNamespace shall search all active schemas and return the schema with the 
               namespace given by the schemaNamespace argument.] */
            result = *handle;
        }
    }

    return result;
}

const char* Schema_GetSchemaNamespace(SCHEMA_HANDLE schemaHandle)
{
    const char* result;

    /* Codes_SRS_SCHEMA_99_130: [If the schemaHandle argument is NULL, Schema_GetSchemaNamespace shall return NULL.] */
    if (schemaHandle == NULL)
    {
        result = NULL;
    }
    else
    {
        /* Codes_SRS_SCHEMA_99_129: [Schema_GetSchemaNamespace shall return the namespace for the schema identified by schemaHandle.] */
        result = ((SCHEMA*)schemaHandle)->Namespace;
    }

    return result;
}

void Schema_Destroy(SCHEMA_HANDLE schemaHandle)
{
    /* Codes_SRS_SCHEMA_99_006:[If the schemaHandle is NULL, Schema_Destroy shall do nothing.] */
    if (schemaHandle != NULL)
    {
        SCHEMA* schema = (SCHEMA*)schemaHandle;
        size_t i;

        /* Codes_SRS_SCHEMA_99_005:[Schema_Destroy shall free all resources associated with a schema.] */
        for (i = 0; i < schema->ModelTypeCount; i++)
        {
            DestroyModel(schema->ModelTypes[i]);
        }

        free(schema->ModelTypes);

        /* Codes_SRS_SCHEMA_99_005:[Schema_Destroy shall free all resources associated with a schema.] */
        for (i = 0; i < schema->StructTypeCount; i++)
        {
            DestroyStruct(schema->StructTypes[i]);
        }

        free(schema->StructTypes);
        free((void*)schema->Namespace);
        free(schema);

        schema = (SCHEMA*)VECTOR_find_if(g_schemas, (PREDICATE_FUNCTION)SchemaHandlesMatch, &schemaHandle);
        if (schema != NULL)
        {
            VECTOR_erase(g_schemas, schema, 1);
        }
        // If the g_schema is empty then destroy it
        if (VECTOR_size(g_schemas) == 0)
        {
            VECTOR_destroy(g_schemas);
            g_schemas = NULL;
        }
    }
}

SCHEMA_RESULT Schema_DestroyIfUnused(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle)
{
    SCHEMA_RESULT result;

    /* Codes_SRS_SCHEMA_07_189: [If modelHandle variable is NULL Schema_DestroyIfUnused shall do nothing.] */
    if (modelTypeHandle != NULL)
    {
        SCHEMA_HANDLE schemaHandle = Schema_GetSchemaForModelType(modelTypeHandle);
        if (schemaHandle == NULL)
        {
            result = SCHEMA_ERROR;
        }
        else
        {
            SCHEMA* schema = (SCHEMA*)schemaHandle;
            size_t nIndex;

            /* Codes_SRS_SCHEMA_07_190: [Schema_DestroyIfUnused shall iterate through the ModuleTypes objects and if all the DeviceCount variables 0 then it will delete the schemaHandle.] */
            for (nIndex = 0; nIndex < schema->ModelTypeCount; nIndex++)
            {
                MODEL_TYPE* modelType = (MODEL_TYPE*)schema->ModelTypes[nIndex];
                if (modelType->DeviceCount > 0)
                    break;
            }
            /* Codes_SRS_SCHEMA_07_191: [If 1 or more DeviceCount variables are > 0 then Schema_DestroyIfUnused shall do nothing.] */
            if (nIndex == schema->ModelTypeCount)
            {
                Schema_Destroy(schemaHandle);
                result = SCHEMA_OK;
            }
            else
            {
                result = SCHEMA_MODEL_IN_USE;
            }
        }
    }
    else
    {
        result = SCHEMA_INVALID_ARG;
    }
    return result;
}

SCHEMA_RESULT Schema_AddDeviceRef(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle)
{
    SCHEMA_RESULT result;
    if (modelTypeHandle == NULL)
    {
        /* Codes_SRS_SCHEMA_07_187: [Schema_AddDeviceRef shall return SCHEMA_INVALID_ARG if modelTypeHandle is NULL.] */
        result = SCHEMA_INVALID_ARG;
        LogError("(Error code:%s)\r\n", ENUM_TO_STRING(SCHEMA_RESULT, result));
    }
    else
    {
        MODEL_TYPE* model = (MODEL_TYPE*)modelTypeHandle;
        /* Codes_SRS_SCHEMA_07_188: [If the modelTypeHandle is nonNULL, Schema_AddDeviceRef shall increment the MODEL_TYPE DeviceCount variable.] */
        model->DeviceCount++;
        result = SCHEMA_OK;
    }
    return result;
}

SCHEMA_RESULT Schema_ReleaseDeviceRef(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle)
{
    SCHEMA_RESULT result;
    /* Codes_SRS_SCHEMA_07_184: [Schema_DeviceRelease shall do nothing if the supplied modelHandle is NULL.] */
    if (modelTypeHandle == NULL)
    {
        result = SCHEMA_INVALID_ARG;
        LogError("(Error code:%s)\r\n", ENUM_TO_STRING(SCHEMA_RESULT, result));
    }
    else
    {
        MODEL_TYPE* model = (MODEL_TYPE*)modelTypeHandle;
        if (model->DeviceCount > 0)
        {
            /* Codes_SRS_SCHEMA_07_186: [On a nonNULL SCHEMA_MODEL_TYPE_HANDLE if the DeviceCount variable is > 0 then the variable will be decremented.] */
            model->DeviceCount--;
            result = SCHEMA_OK;
        }
        else
        {
            result = SCHEMA_DEVICE_COUNT_ZERO;
            LogError("(Error code:%s)\r\n", ENUM_TO_STRING(SCHEMA_RESULT, result));
        }
    }
    return result;
}

/* Codes_SRS_SCHEMA_99_007:[Schema_CreateModelType shall create a new model type and return a handle to it.] */
SCHEMA_MODEL_TYPE_HANDLE Schema_CreateModelType(SCHEMA_HANDLE schemaHandle, const char* modelName)
{
    SCHEMA_MODEL_TYPE_HANDLE result;

    /* Codes_SRS_SCHEMA_99_010:[If any of the arguments is NULL, Schema_CreateModelType shall fail.] */
    if ((schemaHandle == NULL) ||
        (modelName == NULL))
    {
        /* Codes_SRS_SCHEMA_99_009:[On failure, Schema_CreateModelType shall return NULL.] */
        result = NULL;
        LogError("(Error code:%s)\r\n", ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_INVALID_ARG));
    }
    else
    {
        SCHEMA* schema = (SCHEMA*)schemaHandle;

        /* Codes_SRS_SCHEMA_99_100: [Schema_CreateModelType shall return SCHEMA_DUPLICATE_ELEMENT if modelName already exists.] */
        size_t i;
        for (i = 0; i < schema->ModelTypeCount; i++)
        {
            MODEL_TYPE* model = (MODEL_TYPE*)(schema->ModelTypes[i]);
            if (strcmp(model->Name, modelName) == 0)
            {
                break;
            }
        }

        if (i < schema->ModelTypeCount)
        {
            /* Codes_SRS_SCHEMA_99_009:[On failure, Schema_CreateModelType shall return NULL.] */
            result = NULL;
            LogError("%s Model Name already exists", modelName);
        }

        else
        {
            SCHEMA_MODEL_TYPE_HANDLE* newModelTypes = (SCHEMA_MODEL_TYPE_HANDLE*)realloc(schema->ModelTypes, sizeof(SCHEMA_MODEL_TYPE_HANDLE) * (schema->ModelTypeCount + 1));
            if (newModelTypes == NULL)
            {
                /* Codes_SRS_SCHEMA_99_009:[On failure, Schema_CreateModelType shall return NULL.] */
                result = NULL;
                LogError("(Error code:%s)\r\n", ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_ERROR));
            }
            else
            {
                MODEL_TYPE* modelType;
                schema->ModelTypes = newModelTypes;

                if ((modelType = (MODEL_TYPE*)malloc(sizeof(MODEL_TYPE))) == NULL)
                {

                    /* Codes_SRS_SCHEMA_99_009:[On failure, Schema_CreateModelType shall return NULL.] */
                    result = NULL;
                    LogError("(Error code:%s)\r\n", ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_ERROR));
                }
                else if (mallocAndStrcpy_s((char**)&modelType->Name, modelName) != 0)
                {
                    /* Codes_SRS_SCHEMA_99_009:[On failure, Schema_CreateModelType shall return NULL.] */
                    result = NULL;
                    free(modelType);
                    LogError("(Error code:%s)\r\n", ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_ERROR));
                }
                else
                {
                    modelType->PropertyCount = 0;
                    modelType->Properties = NULL;
                    modelType->ActionCount = 0;
                    modelType->Actions = NULL;
                    modelType->SchemaHandle = schemaHandle;
                    modelType->DeviceCount = 0;
                    modelType->models = VECTOR_create(sizeof(MODEL_IN_MODEL) );
                    schema->ModelTypes[schema->ModelTypeCount] = modelType;
                    schema->ModelTypeCount++;

                    /* Codes_SRS_SCHEMA_99_008:[On success, a non-NULL handle shall be returned.] */
                    result = (SCHEMA_MODEL_TYPE_HANDLE)modelType;
                }

                /* If possible, reduce the memory of over allocation */
                if (result == NULL)
                {
                    SCHEMA_MODEL_TYPE_HANDLE* oldModelTypes = (SCHEMA_MODEL_TYPE_HANDLE*)realloc(schema->ModelTypes, sizeof(SCHEMA_MODEL_TYPE_HANDLE) * schema->ModelTypeCount);
                    if (oldModelTypes == NULL)
                    {
                        result = NULL;
                        LogError("(Error code:%s)\r\n", ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_ERROR));
                    }
                    else
                    {
                        schema->ModelTypes = oldModelTypes;
                    }
                }
            }
        }
    }

    return result;
}

SCHEMA_HANDLE Schema_GetSchemaForModelType(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle)
{
    SCHEMA_HANDLE result;

    if (modelTypeHandle == NULL)
    {
        /* Codes_SRS_SCHEMA_99_132: [If the modelTypeHandle argument is NULL, Schema_GetSchemaForModelType shall return NULL.] */
        result = NULL;
    }
    else
    {
        /* Codes_SRS_SCHEMA_99_131: [Schema_GetSchemaForModelType returns the schema handle for a given model type.] */
        result = ((MODEL_TYPE*)modelTypeHandle)->SchemaHandle;
    }

    return result;
}

/* Codes_SRS_SCHEMA_99_011:[Schema_AddModelProperty shall add one property to the model type identified by modelTypeHandle.] */
SCHEMA_RESULT Schema_AddModelProperty(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, const char* propertyName, const char* propertyType)
{
    return AddModelProperty((MODEL_TYPE*)modelTypeHandle, propertyName, propertyType);
}

SCHEMA_ACTION_HANDLE Schema_CreateModelAction(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, const char* actionName)
{
    SCHEMA_ACTION_HANDLE result;

    /* Codes_SRS_SCHEMA_99_104: [If any of the modelTypeHandle or actionName arguments is NULL, Schema_CreateModelAction shall return NULL.] */
    if ((modelTypeHandle == NULL) ||
        (actionName == NULL))
    {
        result = NULL;
        LogError("(Error code:%s)\r\n", ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_INVALID_ARG));
    }
    else
    {
        MODEL_TYPE* modelType = (MODEL_TYPE*)modelTypeHandle;
        size_t i;

        /* Codes_SRS_SCHEMA_99_105: [The action name shall be unique per model, if the same action name is added twice to a model, Schema_CreateModelAction shall return NULL.] */
        for (i = 0; i < modelType->ActionCount; i++)
        {
            ACTION* action = (ACTION*)modelType->Actions[i];
            if (strcmp(action->ActionName, actionName) == 0)
            {
                break;
            }
        }

        if (i < modelType->ActionCount)
        {
            result = NULL;
            LogError("(Error code:%s)\r\n", ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_DUPLICATE_ELEMENT));
        }
        else
        {
            /* Codes_SRS_SCHEMA_99_102: [Schema_CreateModelAction shall add one action to the model type identified by modelTypeHandle.] */
            SCHEMA_ACTION_HANDLE* newActions = (SCHEMA_ACTION_HANDLE*)realloc(modelType->Actions, sizeof(SCHEMA_ACTION_HANDLE) * (modelType->ActionCount + 1));
            if (newActions == NULL)
            {
                /* Codes_SRS_SCHEMA_99_106: [On any other error, Schema_CreateModelAction shall return NULL.]*/
                result = NULL;
                LogError("(Error code:%s)\r\n", ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_ERROR));
            }
            else
            {
                ACTION* newAction;
                modelType->Actions = newActions;

                /* Codes_SRS_SCHEMA_99_103: [On success, Schema_CreateModelAction shall return a none-NULL SCHEMA_ACTION_HANDLE to the newly created action.] */
                if ((newAction = (ACTION*)malloc(sizeof(ACTION))) == NULL)
                {
                    /* Codes_SRS_SCHEMA_99_106: [On any other error, Schema_CreateModelAction shall return NULL.]*/
                    result = NULL;
                    LogError("(Error code:%s)\r\n", ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_ERROR));
                }
                else
                {
                    if (mallocAndStrcpy_s((char**)&newAction->ActionName, actionName) != 0)
                    {
                        /* Codes_SRS_SCHEMA_99_106: [On any other error, Schema_CreateModelAction shall return NULL.]*/
                        free(newAction);
                        result = NULL;
                        LogError("(Error code:%s)\r\n", ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_ERROR));
                    }
                    else
                    {
                        newAction->ArgumentCount = 0;
                        newAction->ArgumentHandles = NULL;

                        modelType->Actions[modelType->ActionCount] = newAction;
                        modelType->ActionCount++;
                        result = (SCHEMA_ACTION_HANDLE)(newAction);
                    }

                    /* If possible, reduce the memory of over allocation */
                    if (result == NULL)
                    {
                        SCHEMA_ACTION_HANDLE* oldActions = (SCHEMA_ACTION_HANDLE*)realloc(modelType->Actions, sizeof(SCHEMA_ACTION_HANDLE) * modelType->ActionCount);
                        if (oldActions == NULL)
                        {
                            LogError("(Error code:%s)\r\n", ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_ERROR));
                        }
                        else
                        {
                            modelType->Actions = oldActions;
                        }
                    }
                }
            }
        }
    }
    return result;
}

SCHEMA_RESULT Schema_AddModelActionArgument(SCHEMA_ACTION_HANDLE actionHandle, const char* argumentName, const char* argumentType)
{
    SCHEMA_RESULT result;

    /* Codes_SRS_SCHEMA_99_109: [If any of the arguments is NULL, Schema_AddModelActionArgument shall return SCHEMA_INVALID_ARG.] */
    if ((actionHandle == NULL) ||
        (argumentName == NULL) ||
        (argumentType == NULL))
    {
        result = SCHEMA_INVALID_ARG;
        LogError("(result = %s)\r\n", ENUM_TO_STRING(SCHEMA_RESULT, result));
    }
    else
    {
        ACTION* action = (ACTION*)actionHandle;

        /* Codes_SRS_SCHEMA_99_110: [The argument name shall be unique per action, if the same name is added twice to an action, SCHEMA_DUPLICATE_ELEMENT shall be returned.] */
        /* Codes_SRS_SCHEMA_99_111: [Schema_AddModelActionArgument shall accept arguments with different names of the same type.]  */
        size_t i;
        for (i = 0; i < action->ArgumentCount; i++)
        {
            SCHEMA_ACTION_ARGUMENT* actionArgument = (SCHEMA_ACTION_ARGUMENT*)action->ArgumentHandles[i];
            if (strcmp((actionArgument->Name), argumentName) == 0)
            {
                break;
            }
        }

        if (i < action->ArgumentCount)
        {
            result = SCHEMA_DUPLICATE_ELEMENT;
            LogError("(result = %s)\r\n", ENUM_TO_STRING(SCHEMA_RESULT, result));
        }
        else
        {
            /* Codes_SRS_SCHEMA_99_107: [Schema_AddModelActionArgument shall add one argument name & type to an action identified by actionHandle.]  */
            SCHEMA_ACTION_ARGUMENT_HANDLE* newArguments = (SCHEMA_ACTION_ARGUMENT_HANDLE*)realloc(action->ArgumentHandles, sizeof(SCHEMA_ACTION_ARGUMENT_HANDLE) * (action->ArgumentCount + 1));
            if (newArguments == NULL)
            {
                /* Codes_SRS_SCHEMA_99_112: [On any other error, Schema_ AddModelActionArgumet shall return SCHEMA_ERROR.] */
                result = SCHEMA_ERROR;
                LogError("(result = %s)\r\n", ENUM_TO_STRING(SCHEMA_RESULT, result));
            }
            else
            {
                SCHEMA_ACTION_ARGUMENT* newActionArgument;
                if ((newActionArgument = (SCHEMA_ACTION_ARGUMENT*)malloc(sizeof(SCHEMA_ACTION_ARGUMENT))) == NULL)
                {
                    /* Codes_SRS_SCHEMA_99_112: [On any other error, Schema_ AddModelActionArgumet shall return SCHEMA_ERROR.] */
                    result = SCHEMA_ERROR;
                    LogError("(result = %s)\r\n", ENUM_TO_STRING(SCHEMA_RESULT, result));
                }
                else
                {
                    if (mallocAndStrcpy_s((char**)&newActionArgument->Name, argumentName) != 0)
                    {
                        /* Codes_SRS_SCHEMA_99_112: [On any other error, Schema_ AddModelActionArgumet shall return SCHEMA_ERROR.] */
                        free(newActionArgument);
                        result = SCHEMA_ERROR;
                        LogError("(result = %s)\r\n", ENUM_TO_STRING(SCHEMA_RESULT, result));
                    }
                    else if (mallocAndStrcpy_s((char**)&newActionArgument->Type, argumentType) != 0)
                    {
                        /* Codes_SRS_SCHEMA_99_112: [On any other error, Schema_ AddModelActionArgumet shall return SCHEMA_ERROR.] */
                        free((void*)newActionArgument->Name);
                        free(newActionArgument);
                        result = SCHEMA_ERROR;
                        LogError("(result = %s)\r\n", ENUM_TO_STRING(SCHEMA_RESULT, result));
                    }
                    else
                    {
                        action->ArgumentHandles = newArguments;
                        /* Codes_SRS_SCHEMA_99_119: [Schema_AddModelActionArgument shall preserve the order of the action arguments according to the order in which they were added, starting with index 0 for the first added argument.] */
                        action->ArgumentHandles[action->ArgumentCount] = newActionArgument;
                        action->ArgumentCount++;

                        /* Codes_SRS_SCHEMA_99_108: [On success, Schema_AddModelActionArgunent shall return SCHEMA_OK.] */
                        result = SCHEMA_OK;
                    }
                }

                /* If possible, reduce the memory of over allocation */
                if (result == SCHEMA_ERROR)
                {
                    SCHEMA_ACTION_ARGUMENT_HANDLE* oldArguments = (SCHEMA_ACTION_ARGUMENT_HANDLE*)realloc(action->ArgumentHandles, sizeof(SCHEMA_ACTION_ARGUMENT_HANDLE) * action->ArgumentCount);
                    if (oldArguments == NULL)
                    {
                        LogError("(result = %s)\r\n", ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_ERROR));
                    }
                    else
                    {
                        action->ArgumentHandles = oldArguments;
                    }
                }
            }
        }
    }
    return result;
}

SCHEMA_PROPERTY_HANDLE Schema_GetModelPropertyByName(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, const char* propertyName)
{
    SCHEMA_PROPERTY_HANDLE result;

    /* Codes_SRS_SCHEMA_99_038:[Schema_GetModelPropertyByName shall return NULL if unable to find a matching property or if any of the arguments are NULL.] */
    if ((modelTypeHandle == NULL) ||
        (propertyName == NULL))
    {
        result = NULL;
        LogError("(Error code:%s)\r\n", ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_INVALID_ARG));
    }
    else
    {
        size_t i;
        MODEL_TYPE* modelType = (MODEL_TYPE*)modelTypeHandle;

        /* Codes_SRS_SCHEMA_99_036:[Schema_GetModelPropertyByName shall return a non-NULL SCHEMA_PROPERTY_HANDLE corresponding to the model type identified by modelTypeHandle and matching the propertyName argument value.] */
        for (i = 0; i < modelType->PropertyCount; i++)
        {
            PROPERTY* modelProperty = (PROPERTY*)modelType->Properties[i];
            if (strcmp(modelProperty->PropertyName, propertyName) == 0)
            {
                break;
            }
        }

        if (i == modelType->PropertyCount)
        {
            /* Codes_SRS_SCHEMA_99_038:[Schema_GetModelPropertyByName shall return NULL if unable to find a matching property or if any of the arguments are NULL.] */
            result = NULL;
            LogError("(Error code:%s)\r\n", ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_ELEMENT_NOT_FOUND));
        }
        else
        {
            result = (SCHEMA_PROPERTY_HANDLE)(modelType->Properties[i]);
        }
    }

    return result;
}

SCHEMA_RESULT Schema_GetModelPropertyCount(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, size_t* propertyCount)
{
    SCHEMA_RESULT result;

    /* Codes_SRS_SCHEMA_99_092: [Schema_GetModelPropertyCount shall return SCHEMA_INVALID_ARG if any of the arguments is NULL.] */
    if ((modelTypeHandle == NULL) ||
        (propertyCount == NULL))
    {
        result = SCHEMA_INVALID_ARG;
    }
    else
    {
        /* Codes_SRS_SCHEMA_99_089: [Schema_GetModelPropertyCount shall provide the number of properties defined in the model type identified by modelTypeHandle.] */
        MODEL_TYPE* modelType = (MODEL_TYPE*)modelTypeHandle;

        /* Codes_SRS_SCHEMA_99_090: [The count shall be provided via the out argument propertyCount.]*/
        *propertyCount = modelType->PropertyCount;

        /* Codes_SRS_SCHEMA_99_091: [On success, Schema_GetModelPropertyCount shall return SCHEMA_OK.] */
        result = SCHEMA_OK;
    }

    return result;
}

SCHEMA_PROPERTY_HANDLE Schema_GetModelPropertyByIndex(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, size_t index)
{
    SCHEMA_PROPERTY_HANDLE result;
    MODEL_TYPE* modelType = (MODEL_TYPE*)modelTypeHandle;

    /* Codes_SRS_SCHEMA_99_094: [Schema_GetModelProperty shall return NULL if the index specified is outside the valid range or if modelTypeHandle argument is NULL.] */
    if ((modelTypeHandle == NULL) ||
        (index >= modelType->PropertyCount))
    {
        result = NULL;
        LogError("(Error code: %s)\r\n", ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_INVALID_ARG));
    }
    else
    {
        /* Tests_SRS_SCHEMA_99_093: [Schema_GetModelProperty shall return a non-NULL SCHEMA_PROPERTY_HANDLE corresponding to the model type identified by modelTypeHandle and matching the index number provided by the index argument.] */
        /* Codes_SRS_SCHEMA_99_097: [index is zero based, and the order in which actions were added shall be the index in which they will be retrieved.] */
        result = modelType->Properties[index];
    }

    return result;
}

SCHEMA_ACTION_HANDLE Schema_GetModelActionByName(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, const char* actionName)
{
    SCHEMA_ACTION_HANDLE result;

    /* Codes_SRS_SCHEMA_99_041:[Schema_GetModelActionByName shall return NULL if unable to find a matching action, if any of the arguments are NULL.] */
    if ((modelTypeHandle == NULL) ||
        (actionName == NULL))
    {
        result = NULL;
        LogError("(Error code:%s)\r\n", ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_INVALID_ARG));
    }
    else
    {
        size_t i;
        MODEL_TYPE* modelType = (MODEL_TYPE*)modelTypeHandle;

        /* Codes_SRS_SCHEMA_99_040:[Schema_GetModelActionByName shall return a non-NULL SCHEMA_ACTION_HANDLE corresponding to the model type identified by modelTypeHandle and matching the actionName argument value.] */
        for (i = 0; i < modelType->ActionCount; i++)
        {
            ACTION* modelAction = (ACTION*)modelType->Actions[i];
            if (strcmp(modelAction->ActionName, actionName) == 0)
            {
                break;
            }
        }

        if (i == modelType->ActionCount)
        {
            /* Codes_SRS_SCHEMA_99_041:[Schema_GetModelActionByName shall return NULL if unable to find a matching action, if any of the arguments are NULL.] */
            result = NULL;
            LogError("(Error code:%s)\r\n", ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_ELEMENT_NOT_FOUND));
        }
        else
        {
            result = modelType->Actions[i];
        }
    }

    return result;
}

SCHEMA_RESULT Schema_GetModelActionCount(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, size_t* actionCount)
{
    SCHEMA_RESULT result;

    /* Codes_SRS_SCHEMA_99_045:[If any of the modelTypeHandle or actionCount arguments is NULL, Schema_GetModelActionCount shall return SCHEMA_INVALID_ARG.] */
    if ((modelTypeHandle == NULL) ||
        (actionCount == NULL))
    {
        result = SCHEMA_INVALID_ARG;
        LogError("(result=%s)\r\n", ENUM_TO_STRING(SCHEMA_RESULT, result));
    }
    else
    {
        /* Codes_SRS_SCHEMA_99_042:[Schema_GetModelActionCount shall provide the total number of actions defined in a model type identified by the modelTypeHandle.] */
        MODEL_TYPE* modelType = (MODEL_TYPE*)modelTypeHandle;

        /* Codes_SRS_SCHEMA_99_043:[The count shall be provided via the out argument actionCount.] */
        *actionCount = modelType->ActionCount;

        /* Codes_SRS_SCHEMA_99_044:[On success, Schema_GetModelActionCount shall return SCHEMA_OK.] */
        result = SCHEMA_OK;
    }

    return result;
}

SCHEMA_ACTION_HANDLE Schema_GetModelActionByIndex(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, size_t index)
{
    SCHEMA_ACTION_HANDLE result;
    MODEL_TYPE* modelType = (MODEL_TYPE*)modelTypeHandle;

    /* Codes_SRS_SCHEMA_99_048:[Schema_GetModelAction shall return NULL if the index specified is outside the valid range or if modelTypeHandle argument is NULL.] */
    if ((modelType == NULL) ||
        (index >= modelType->ActionCount))
    {
        result = NULL;
        LogError("(Error code:%s)\r\n", ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_INVALID_ARG));
    }
    else
    {
        /* Codes_SRS_SCHEMA_99_047:[Schema_GetModelAction shall return a non-NULL SCHEMA_ACTION_HANDLE corresponding to the model type identified by modelTypeHandle and matching the index number provided by the index argument.] */
        /* Codes_SRS_SCHEMA_99_096: [index is zero based and the order in which actions were added shall be the index in which they will be retrieved.] */
        result = modelType->Actions[index];
    }

    return result;
}

const char* Schema_GetModelActionName(SCHEMA_ACTION_HANDLE actionHandle)
{
    const char* result;

    /* Codes_SRS_SCHEMA_99_050:[If the actionHandle is NULL, Schema_GetModelActionName shall return NULL.] */
    if (actionHandle == NULL)
    {
        result = NULL;
        LogError("(Error code:%s)\r\n", ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_INVALID_ARG));
    }
    else
    {
        ACTION* action = (ACTION*)actionHandle;
        /* Codes_SRS_SCHEMA_99_049:[Schema_GetModelActionName shall return the action name for a given action handle.] */
        result = action->ActionName;
    }

    return result;
}

SCHEMA_RESULT Schema_GetModelActionArgumentCount(SCHEMA_ACTION_HANDLE actionHandle, size_t* argumentCount)
{
    SCHEMA_RESULT result;

    /* Codes_SRS_SCHEMA_99_054:[If any argument is NULL, Schema_GetModelActionArgumentCount shall return SCHEMA_INVALID_ARG.] */
    if ((actionHandle == NULL) ||
        (argumentCount == NULL))
    {
        result = SCHEMA_INVALID_ARG;
        LogError("(result=%s)\r\n", ENUM_TO_STRING(SCHEMA_RESULT, result));
    }
    else
    {
        ACTION* modelAction = (ACTION*)actionHandle;

        /* Codes_SRS_SCHEMA_99_051:[Schema_GetModelActionArgumentCount shall provide the number of arguments for a specific schema action identified by actionHandle.] */
        /* Codes_SRS_SCHEMA_99_052:[The argument count shall be provided via the out argument argumentCount.] */
        *argumentCount = modelAction->ArgumentCount;

        /* Codes_SRS_SCHEMA_99_053:[On success, Schema_GetModelActionArgumentCount shall return SCHEMA_OK.] */
        result = SCHEMA_OK;
    }

    return result;
}

SCHEMA_ACTION_ARGUMENT_HANDLE Schema_GetModelActionArgumentByName(SCHEMA_ACTION_HANDLE actionHandle, const char* actionArgumentName)
{
    SCHEMA_ACTION_ARGUMENT_HANDLE result;

    /* Codes_SRS_SCHEMA_99_118: [Schema_GetModelActionArgumentByName shall return NULL if unable to find a matching argument or if any of the arguments are NULL.] */
    if ((actionHandle == NULL) ||
        (actionArgumentName == NULL))
    {
        result = NULL;
        LogError("(Error code:%s)\r\n", ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_INVALID_ARG));
    }
    else
    {
        /* Codes_SRS_SCHEMA_99_118: [Schema_GetModelActionArgumentByName shall return NULL if unable to find a matching argument or if any of the arguments are NULL.] */
        ACTION* modelAction = (ACTION*)actionHandle;
        size_t i;

        for (i = 0; i < modelAction->ArgumentCount; i++)
        {
            SCHEMA_ACTION_ARGUMENT* actionArgument = (SCHEMA_ACTION_ARGUMENT*)modelAction->ArgumentHandles[i];
            if (strcmp(actionArgument->Name, actionArgumentName) == 0)
            {
                break;
            }
        }

        if (i == modelAction->ArgumentCount)
        {
            /* Codes_SRS_SCHEMA_99_118: [Schema_GetModelActionArgumentByName shall return NULL if unable to find a matching argument or if any of the arguments are NULL.] */
            result = NULL;
            LogError("(Error code:%s)\r\n", ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_ELEMENT_NOT_FOUND));
        }
        else
        {
            /* Codes_SRS_SCHEMA_99_117: [Schema_GetModelActionArgumentByName shall return a non-NULL handle corresponding to an action argument identified by the actionHandle and actionArgumentName.] */
            result = modelAction->ArgumentHandles[i];
        }
    }

    return result;
}

SCHEMA_ACTION_ARGUMENT_HANDLE Schema_GetModelActionArgumentByIndex(SCHEMA_ACTION_HANDLE actionHandle, size_t argumentIndex)
{
    SCHEMA_ACTION_ARGUMENT_HANDLE result;
    ACTION* modelAction = (ACTION*)actionHandle;

    /* Codes_SRS_SCHEMA_99_056:[Schema_GetModelActionArgument shall return NULL if the index specified is outside the valid range or if the actionHandle argument is NULL.] */
    if ((actionHandle == NULL) ||
        (argumentIndex >= modelAction->ArgumentCount))
    {
        result = NULL;
        LogError("(Error code:%s)\r\n", ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_INVALID_ARG));
    }
    else
    {
        /* Codes_SRS_SCHEMA_99_055:[Schema_GetModelActionArgument shall return a non-NULL SCHEMA_ACTION_ARGUMENT_HANDLE corresponding to the action type identified by actionHandle and matching the index number provided by the argumentIndex argument.] */
        result = modelAction->ArgumentHandles[argumentIndex];
    }

    return result;
}

const char* Schema_GetActionArgumentName(SCHEMA_ACTION_ARGUMENT_HANDLE actionArgumentHandle)
{
    const char* result;
    /* Codes_SRS_SCHEMA_99_114: [Schema_GetActionArgumentName shall return NULL if actionArgumentHandle is NULL.] */
    if (actionArgumentHandle == NULL)
    {
        result = NULL;
        LogError("(Error code:%s)\r\n", ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_INVALID_ARG));
    }
    else
    {
        /* Codes_SRS_SCHEMA_99_113: [Schema_GetActionArgumentName shall return the argument name identified by the actionArgumentHandle.] */
        SCHEMA_ACTION_ARGUMENT* actionArgument = (SCHEMA_ACTION_ARGUMENT*)actionArgumentHandle;
        result = actionArgument->Name;
    }
    return result;
}

const char* Schema_GetActionArgumentType(SCHEMA_ACTION_ARGUMENT_HANDLE actionArgumentHandle)
{
    const char* result;
    /* Codes_SRS_SCHEMA_99_116: [Schema_GetActionArgumentType shall return NULL if actionArgumentHandle is NULL.] */
    if (actionArgumentHandle == NULL)
    {
        result = NULL;
        LogError("(Error code:%s)\r\n", ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_INVALID_ARG));
    }
    else
    {
        /* Codes_SRS_SCHEMA_99_115: [Schema_GetActionArgumentType shall return the argument type identified by the actionArgumentHandle.] */
        SCHEMA_ACTION_ARGUMENT* actionArgument = (SCHEMA_ACTION_ARGUMENT*)actionArgumentHandle;
        result = actionArgument->Type;
    }
    return result;
}

SCHEMA_STRUCT_TYPE_HANDLE Schema_CreateStructType(SCHEMA_HANDLE schemaHandle, const char* typeName)
{
    SCHEMA_STRUCT_TYPE_HANDLE result;
    SCHEMA* schema = (SCHEMA*)schemaHandle;

    /* Codes_SRS_SCHEMA_99_060:[If any of the arguments is NULL, Schema_CreateStructType shall return NULL.] */
    if ((schema == NULL) ||
        (typeName == NULL))
    {
        result = NULL;
        LogError("(Error code:%s)\r\n", ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_INVALID_ARG));
    }
    else
    {
        STRUCT_TYPE* structType;
        size_t i;

        /* Codes_SRS_SCHEMA_99_061:[If a struct type with the same name already exists, Schema_CreateStructType shall return NULL.] */
        for (i = 0; i < schema->StructTypeCount; i++)
        {
            structType = (STRUCT_TYPE*)schema->StructTypes[i];
            if (strcmp(structType->Name, typeName) == 0)
            {
                break;
            }
        }

        if (i < schema->StructTypeCount)
        {
            result = NULL;
            LogError("(Error code:%s)\r\n", ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_DUPLICATE_ELEMENT));
        }
        else
        {
            SCHEMA_STRUCT_TYPE_HANDLE* newStructTypes = (SCHEMA_STRUCT_TYPE_HANDLE*)realloc(schema->StructTypes, sizeof(SCHEMA_STRUCT_TYPE_HANDLE) * (schema->StructTypeCount + 1));
            if (newStructTypes == NULL)
            {
                /* Codes_SRS_SCHEMA_99_066:[On any other error, Schema_CreateStructType shall return NULL.] */
                result = NULL;
                LogError("(Error code:%s)\r\n", ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_ERROR));
            }
            else
            {
                schema->StructTypes = newStructTypes;
                if ((structType = (STRUCT_TYPE*)malloc(sizeof(STRUCT_TYPE))) == NULL)
                {
                    /* Codes_SRS_SCHEMA_99_066:[On any other error, Schema_CreateStructType shall return NULL.] */
                    result = NULL;
                    LogError("(Error code:%s)\r\n", ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_ERROR));
                }
                else if (mallocAndStrcpy_s((char**)&structType->Name, typeName) != 0)
                {
                    /* Codes_SRS_SCHEMA_99_066:[On any other error, Schema_CreateStructType shall return NULL.] */
                    result = NULL;
                    free(structType);
                    LogError("(Error code:%s)\r\n", ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_ERROR));
                }
                else
                {
                    /* Codes_SRS_SCHEMA_99_057:[Schema_CreateStructType shall create a new struct type and return a handle to it.] */
                    schema->StructTypes[schema->StructTypeCount] = structType;
                    schema->StructTypeCount++;
                    structType->PropertyCount = 0;
                    structType->Properties = NULL;

                    /* Codes_SRS_SCHEMA_99_058:[On success, a non-NULL handle shall be returned.] */
                    result = (SCHEMA_STRUCT_TYPE_HANDLE)structType;
                }

                /* If possible, reduce the memory of over allocation */
                if (result == NULL)
                {
                    SCHEMA_STRUCT_TYPE_HANDLE* oldStructTypes = (SCHEMA_STRUCT_TYPE_HANDLE*)realloc(schema->StructTypes, sizeof(SCHEMA_STRUCT_TYPE_HANDLE) * schema->StructTypeCount);
                    if (oldStructTypes == NULL)
                    {
                        result = NULL;
                        LogError("(Error code:%s)\r\n", ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_ERROR));
                    }
                    else
                    {
                        schema->StructTypes = oldStructTypes;
                    }
                }
            }
        }
    }

    return result;
}

const char* Schema_GetStructTypeName(SCHEMA_STRUCT_TYPE_HANDLE structTypeHandle)
{
    const char* result;

    /* Codes_SRS_SCHEMA_99_136: [If structTypeHandle is NULL, Schema_GetStructTypeName shall return NULL.] */
    if (structTypeHandle == NULL)
    {
        result = NULL;
        LogError("(Error code:%s)\r\n", ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_INVALID_ARG));
    }
    else
    {
        /* Codes_SRS_SCHEMA_99_135: [Schema_GetStructTypeName shall return the name of a struct type identified by the structTypeHandle argument.] */
        result = ((STRUCT_TYPE*)structTypeHandle)->Name;
    }
    
    return result;
}

SCHEMA_RESULT Schema_GetStructTypeCount(SCHEMA_HANDLE schemaHandle, size_t* structTypeCount)
{
    SCHEMA_RESULT result;

    /* Codes_SRS_SCHEMA_99_140: [Schema_GetStructTypeCount shall return SCHEMA_INVALID_ARG if any of the arguments is NULL.] */
    if ((schemaHandle == NULL) ||
        (structTypeCount == NULL))
    {
        result = SCHEMA_INVALID_ARG;
        LogError("(result = %s)\r\n", ENUM_TO_STRING(SCHEMA_RESULT, result));
    }
    else
    {
        /* Codes_SRS_SCHEMA_99_137: [Schema_GetStructTypeCount shall provide the number of structs defined in the schema identified by schemaHandle.] */
        SCHEMA* schema = (SCHEMA*)schemaHandle;
        /* Codes_SRS_SCHEMA_99_138: [The count shall be provided via the out argument structTypeCount.] */
        *structTypeCount = schema->StructTypeCount;
        /* Codes_SRS_SCHEMA_99_139: [On success, Schema_GetStructTypeCount shall return SCHEMA_OK.] */
        result = SCHEMA_OK;
    }

    return result;
}

SCHEMA_STRUCT_TYPE_HANDLE Schema_GetStructTypeByName(SCHEMA_HANDLE schemaHandle, const char* name)
{
    SCHEMA_ACTION_HANDLE result;
    SCHEMA* schema = (SCHEMA*)schemaHandle;

    /* Codes_SRS_SCHEMA_99_069:[Schema_GetStructTypeByName shall return NULL if unable to find a matching struct or if any of the arguments are NULL.] */
    if ((schemaHandle == NULL) ||
        (name == NULL))
    {
        result = NULL;
        LogError("(Error code:%s)\r\n", ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_INVALID_ARG));
    }
    else
    {
        size_t i;

        /* Codes_SRS_SCHEMA_99_068:[Schema_GetStructTypeByName shall return a non-NULL handle corresponding to the struct type identified by the structTypeName in the schemaHandle schema.] */
        for (i = 0; i < schema->StructTypeCount; i++)
        {
            STRUCT_TYPE* structType = (STRUCT_TYPE*)schema->StructTypes[i];
            if (strcmp(structType->Name, name) == 0)
            {
                break;
            }
        }

        if (i == schema->StructTypeCount)
        {
            /* Codes_SRS_SCHEMA_99_069:[Schema_GetStructTypeByName shall return NULL if unable to find a matching struct or if any of the arguments are NULL.] */
            result = NULL;
            LogError("(Error code:%s)\r\n", ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_ELEMENT_NOT_FOUND));
        }
        else
        {
            result = schema->StructTypes[i];
        }
    }

    return result;
}

SCHEMA_STRUCT_TYPE_HANDLE Schema_GetStructTypeByIndex(SCHEMA_HANDLE schemaHandle, size_t index)
{
    SCHEMA_STRUCT_TYPE_HANDLE result;
    SCHEMA* schema = (SCHEMA*)schemaHandle;

    /* Codes_SRS_SCHEMA_99_143: [Schema_GetStructTypeByIndex shall return NULL if the index specified is outside the valid range or if schemaHandle argument is NULL.] */
    /* Codes_SRS_SCHEMA_99_142: [The index argument is zero based, and the order in which models were added shall be the index in which they will be retrieved.] */
    if ((schemaHandle == NULL) ||
        (index >= schema->StructTypeCount))
    {
        result = NULL;
        LogError("(Error code: %s)\r\n", ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_INVALID_ARG));
    }
    else
    {

        /* Codes_SRS_SCHEMA_99_141: [Schema_GetStructTypeByIndex shall return a non-NULL SCHEMA_STRUCT_TYPE_HANDLE corresponding to the struct type identified by schemaHandle and matching the index number provided by the index argument.] */
        /* Codes_SRS_SCHEMA_99_142: [The index argument is zero based, and the order in which models were added shall be the index in which they will be retrieved.] */
        result = schema->StructTypes[index];
    }
    return result;
}

SCHEMA_RESULT Schema_AddStructTypeProperty(SCHEMA_STRUCT_TYPE_HANDLE structTypeHandle, const char* propertyName, const char* propertyType)
{
    SCHEMA_RESULT result;

    /* Codes_SRS_SCHEMA_99_072:[If any of the arguments is NULL, Schema_AddStructTypeProperty shall return SCHEMA_INVALID_ARG.] */
    if ((structTypeHandle == NULL) ||
        (propertyName == NULL) ||
        (propertyType == NULL))
    {
        result = SCHEMA_INVALID_ARG;
        LogError("(result = %s)\r\n", ENUM_TO_STRING(SCHEMA_RESULT, result));
    }
    else
    {
        size_t i;
        STRUCT_TYPE* structType = (STRUCT_TYPE*)structTypeHandle;

        /* Codes_SRS_SCHEMA_99_074:[The property name shall be unique per struct type, if the same property name is added twice to a struct type, SCHEMA_DUPLICATE_ELEMENT shall be returned.] */
        for (i = 0; i < structType->PropertyCount; i++)
        {
            PROPERTY* property = (PROPERTY*)structType->Properties[i];
            if (strcmp(property->PropertyName, propertyName) == 0)
            {
                break;
            }
        }

        if (i < structType->PropertyCount)
        {
            result = SCHEMA_DUPLICATE_ELEMENT;
            LogError("(result = %s)\r\n", ENUM_TO_STRING(SCHEMA_RESULT, result));
        }
        else
        {
            SCHEMA_PROPERTY_HANDLE* newProperties = (SCHEMA_PROPERTY_HANDLE*)realloc(structType->Properties, sizeof(SCHEMA_PROPERTY_HANDLE) * (structType->PropertyCount + 1));
            if (newProperties == NULL)
            {
                result = SCHEMA_ERROR;
                LogError("(result = %s)\r\n", ENUM_TO_STRING(SCHEMA_RESULT, result));
            }
            else
            {
                PROPERTY* newProperty;

                structType->Properties = newProperties;
                if ((newProperty = (PROPERTY*)malloc(sizeof(PROPERTY))) == NULL)
                {
                    result = SCHEMA_ERROR;
                    LogError("(result = %s)\r\n", ENUM_TO_STRING(SCHEMA_RESULT, result));
                }
                else
                {
                    if (mallocAndStrcpy_s((char**)&newProperty->PropertyName, propertyName) != 0)
                    {
                        free(newProperty);
                        result = SCHEMA_ERROR;
                        LogError("(result = %s)\r\n", ENUM_TO_STRING(SCHEMA_RESULT, result));
                    }
                    else if (mallocAndStrcpy_s((char**)&newProperty->PropertyType, propertyType) != 0)
                    {
                        free((void*)newProperty->PropertyName);
                        free(newProperty);
                        result = SCHEMA_ERROR;
                        LogError("(result = %s)\r\n", ENUM_TO_STRING(SCHEMA_RESULT, result));
                    }
                    else
                    {
                        /* Codes_SRS_SCHEMA_99_070:[Schema_AddStructTypeProperty shall add one property to the struct type identified by structTypeHandle.] */
                        structType->Properties[structType->PropertyCount] = (SCHEMA_PROPERTY_HANDLE)newProperty;
                        structType->PropertyCount++;

                        /* Codes_SRS_SCHEMA_99_071:[On success, Schema_AddStructTypeProperty shall return SCHEMA_OK.] */
                        result = SCHEMA_OK;
                    }
                }

                /* If possible, reduce the memory of over allocation */
                if (result != SCHEMA_OK)
                {
                    SCHEMA_PROPERTY_HANDLE* oldProperties = (SCHEMA_PROPERTY_HANDLE*)realloc(structType->Properties, sizeof(SCHEMA_PROPERTY_HANDLE) * structType->PropertyCount);
                    if (oldProperties == NULL)
                    {
                        result = SCHEMA_ERROR;
                        LogError("(result = %s)\r\n", ENUM_TO_STRING(SCHEMA_RESULT, result));
                    }
                    else
                    {
                        structType->Properties = oldProperties;
                    }
                }
            }
        }
    }

    return result;
}

SCHEMA_PROPERTY_HANDLE Schema_GetStructTypePropertyByName(SCHEMA_STRUCT_TYPE_HANDLE structTypeHandle, const char* propertyName)
{
    SCHEMA_PROPERTY_HANDLE result;

    /* Codes_SRS_SCHEMA_99_076:[Schema_GetStructTypePropertyByName shall return NULL if unable to find a matching property or if any of the arguments are NULL.] */
    if ((structTypeHandle == NULL) ||
        (propertyName == NULL))
    {
        result = NULL;
        LogError("(Error code: %s)\r\n", ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_INVALID_ARG));
    }
    else
    {
        size_t i;
        STRUCT_TYPE* structType = (STRUCT_TYPE*)structTypeHandle;

        for (i = 0; i < structType->PropertyCount; i++)
        {
            PROPERTY* modelProperty = (PROPERTY*)structType->Properties[i];
            if (strcmp(modelProperty->PropertyName, propertyName) == 0)
            {
                break;
            }
        }

        /* Codes_SRS_SCHEMA_99_076:[Schema_GetStructTypePropertyByName shall return NULL if unable to find a matching property or if any of the arguments are NULL.] */
        if (i == structType->PropertyCount)
        {
            result = NULL;
            LogError("(Error code: %s)\r\n", ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_ELEMENT_NOT_FOUND));
        }
        /* Codes_SRS_SCHEMA_99_075:[Schema_GetStructTypePropertyByName shall return a non-NULL handle corresponding to a property identified by the structTypeHandle and propertyName.] */
        else
        {
            result = structType->Properties[i];
        }
    }

    return result;
}

SCHEMA_RESULT Schema_GetStructTypePropertyCount(SCHEMA_STRUCT_TYPE_HANDLE structTypeHandle, size_t* propertyCount)
{
    SCHEMA_RESULT result;

    /* Codes_SRS_SCHEMA_99_079: [Schema_GetStructTypePropertyCount shall return SCHEMA_INVALID_ARG if any of the structlTypeHandle or propertyCount arguments is NULL.] */
    if ((structTypeHandle == NULL) ||
        (propertyCount == NULL))
    {
        result = SCHEMA_INVALID_ARG;
        LogError("(result = %s)\r\n", ENUM_TO_STRING(SCHEMA_RESULT, result));
    }
    else
    {
        STRUCT_TYPE* structType = (STRUCT_TYPE*)structTypeHandle;

        /* Codes_SRS_SCHEMA_99_077: [Schema_GetStructTypePropertyCount shall provide the total number of properties defined in a struct type identified by structTypeHandle. The value is provided via the out argument propertyCount.] */
        /* Codes_SRS_SCHEMA_99_081: [The count shall be provided via the out argument propertyCount.] */
        *propertyCount = structType->PropertyCount;

        /* Codes_SRS_SCHEMA_99_078: [On success, Schema_ GetStructTypePropertyCount shall return SCHEMA_OK.] */
        result = SCHEMA_OK;
    }

    return result;
}

SCHEMA_PROPERTY_HANDLE Schema_GetStructTypePropertyByIndex(SCHEMA_STRUCT_TYPE_HANDLE structTypeHandle, size_t index)
{
    SCHEMA_PROPERTY_HANDLE result;
    STRUCT_TYPE* structType = (STRUCT_TYPE*)structTypeHandle;

    /* Codes_SRS_SCHEMA_99_083: [Schema_ GetStructTypeProperty shall return NULL if the index specified is outside the valid range, if structTypeHandle argument is NULL] */
    if ((structTypeHandle == NULL) ||
        (index >= structType->PropertyCount))
    {
        result = NULL;
        LogError("(Error code: %s)\r\n", ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_INVALID_ARG));
    }
    else
    {
        /* Codes_SRS_SCHEMA_99_082: [Schema_GetStructTypeProperty shall return a non-NULL SCHEMA_PROPERTY_HANDLE corresponding to the struct type identified by strutTypeHandle and matching the index number provided by the index argument.] */
        /* Codes_SRS_SCHEMA_99_098: [index is zero based and the order in which actions were added shall be the index in which they will be retrieved.] */
        result = structType->Properties[index];
    }

    return result;
}

const char* Schema_GetPropertyName(SCHEMA_PROPERTY_HANDLE propertyHandle)
{
    const char* result;

    /* Codes_SRS_SCHEMA_99_086: [If propertyHandle is NULL, Schema_GetPropertyName shall return NULL.] */
    if (propertyHandle == NULL)
    {
        result = NULL;
        LogError("(Error code: %s)\r\n", ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_INVALID_ARG));
    }
    else
    {
        PROPERTY* propertyType = (PROPERTY*)propertyHandle;

        /* Codes_SRS_SCHEMA_99_085: [Schema_GetPropertyName shall return the property name identified by the propertyHandle.] */
        result = propertyType->PropertyName;
    }

    return result;
}

const char* Schema_GetPropertyType(SCHEMA_PROPERTY_HANDLE propertyHandle)
{
    const char* result;

    /* Codes_SRS_SCHEMA_99_088: [If propertyHandle is NULL, Schema_GetPropertyType shall return NULL.] */
    if (propertyHandle == NULL)
    {
        result = NULL;
        LogError("(Error code: %s)\r\n", ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_INVALID_ARG));
    }
    else
    {
        PROPERTY* modelProperty = (PROPERTY*)propertyHandle;

        /* Codes_SRS_SCHEMA_99_087: [Schema_GetPropertyType shall return the property type identified by the propertyHandle.] */
        result = modelProperty->PropertyType;
    }

    return result;
}

SCHEMA_RESULT Schema_GetModelCount(SCHEMA_HANDLE schemaHandle, size_t* modelCount)
{
    SCHEMA_RESULT result;
    /* Codes_SRS_SCHEMA_99_123: [Schema_GetModelCount shall return SCHEMA_INVALID_ARG if any of the arguments is NULL.] */
    if ((schemaHandle == NULL) ||
        (modelCount == NULL) )
    {
        result = SCHEMA_INVALID_ARG;
        LogError("(result = %s)\r\n", ENUM_TO_STRING(SCHEMA_RESULT, result));
    }
    else
    {
        /* Codes_SRS_SCHEMA_99_120: [Schema_GetModelCount shall provide the number of models defined in the schema identified by schemaHandle.] */
        SCHEMA* schema = (SCHEMA*)schemaHandle;
        /* Codes_SRS_SCHEMA_99_121: [The count shall be provided via the out argument modelCount.] */
        *modelCount = schema->ModelTypeCount;
        /* Codes_SRS_SCHEMA_99_122: [On success, Schema_GetModelCount shall return SCHEMA_OK.] */
        result = SCHEMA_OK;
    }
    return result;
}

SCHEMA_MODEL_TYPE_HANDLE Schema_GetModelByName(SCHEMA_HANDLE schemaHandle, const char* modelName)
{
    SCHEMA_MODEL_TYPE_HANDLE result;

    /* Codes_SRS_SCHEMA_99_125: [Schema_GetModelByName shall return NULL if unable to find a matching model, or if any of the arguments are NULL.] */
    if ((schemaHandle == NULL) ||
        (modelName == NULL))
    {
        result = NULL;
        LogError("(Error code: %s)\r\n", ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_INVALID_ARG));
    }
    else
    {
        /* Codes_SRS_SCHEMA_99_124: [Schema_GetModelByName shall return a non-NULL SCHEMA_MODEL_TYPE_HANDLE corresponding to the model identified by schemaHandle and matching the modelName argument value.] */
        SCHEMA* schema = (SCHEMA*)schemaHandle;
        size_t i;
        for (i = 0; i < schema->ModelTypeCount; i++)
        {
            MODEL_TYPE* modelType = (MODEL_TYPE*)schema->ModelTypes[i];
            if (strcmp(modelName, modelType->Name)==0)
            {
                break;
            }
        }
        if (i == schema->ModelTypeCount)
        {
            /* Codes_SRS_SCHEMA_99_125: [Schema_GetModelByName shall return NULL if unable to find a matching model, or if any of the arguments are NULL.] */
            result = NULL;
        }
        else
        {
            result = schema->ModelTypes[i];
        }
    }
    return result;
}

SCHEMA_MODEL_TYPE_HANDLE Schema_GetModelByIndex(SCHEMA_HANDLE schemaHandle, size_t index)
{
    SCHEMA_MODEL_TYPE_HANDLE result;
    SCHEMA* schema = (SCHEMA*)schemaHandle;

    /* Codes_SRS_SCHEMA_99_128: [Schema_GetModelByIndex shall return NULL if the index specified is outside the valid range or if schemaHandle argument is NULL.] */
    /* Codes_SRS_SCHEMA_99_127: [The index argument is zero based, and the order in which models were added shall be the index in which they will be retrieved.] */
    if ((schemaHandle == NULL) ||
        (index >= schema->ModelTypeCount))
    {
        result = NULL;
        LogError("(Error code: %s)\r\n", ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_INVALID_ARG));
    }
    else
    {

        /* Codes_SRS_SCHEMA_99_126: [Schema_GetModelByIndex shall return a non-NULL SCHEMA_MODEL_TYPE_HANDLE corresponding to the model identified by schemaHandle and matching the index number provided by the index argument.] */ 
        /* Codes_SRS_SCHEMA_99_127: [The index argument is zero based, and the order in which models were added shall be the index in which they will be retrieved.] */
        result = schema->ModelTypes[index];
    }
    return result;
}

/*Codes_SRS_SCHEMA_99_160: [Schema_GetModelName shall return the name of the model identified by modelTypeHandle. If the name cannot be retrieved, then NULL shall be returned.]*/
const char* Schema_GetModelName(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle)
{
    const char* result;
    if (modelTypeHandle == NULL)
    {
        result = NULL;
    }
    else
    {
        MODEL_TYPE* modelType = (MODEL_TYPE*)modelTypeHandle;
        result = modelType->Name;
    }
    return result;
}

/*Codes_SRS_SCHEMA_99_163: [Schema_AddModelModel shall insert an existing model, identified by the handle modelType, into the existing model identified by modelTypeHandle under a property having the name propertyName.]*/
SCHEMA_RESULT Schema_AddModelModel(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, const char* propertyName, SCHEMA_MODEL_TYPE_HANDLE modelType)
{
    SCHEMA_RESULT result;
    /*Codes_SRS_SCHEMA_99_165: [If any of the parameters is NULL then Schema_AddModelModel shall return SCHEMA_INVALID_ARG.]*/
    if (
        (modelTypeHandle == NULL) ||
        (propertyName == NULL) ||
        (modelType == NULL)
        )
    {
        result = SCHEMA_INVALID_ARG;
        LogError("(Error code: %s)\r\n", ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_INVALID_ARG));
    }
    else
    {
        MODEL_TYPE* parentModel = (MODEL_TYPE*)modelTypeHandle;
        MODEL_IN_MODEL temp;
        temp.modelHandle = modelType;
        if (mallocAndStrcpy_s((char**)&(temp.propertyName), propertyName) != 0)
        {
            result = SCHEMA_ERROR;
            LogError("(Error code: %s)\r\n", ENUM_TO_STRING(SCHEMA_RESULT, result));
        }
        else if (VECTOR_push_back(parentModel->models, &temp, 1) != 0)
        {
            /*Codes_SRS_SCHEMA_99_174: [The function shall return SCHEMA_ERROR if any other error occurs.]*/
            free((void*)temp.propertyName);
            result = SCHEMA_ERROR;
            LogError("(Error code: %s)\r\n", ENUM_TO_STRING(SCHEMA_RESULT, result));
        }
        else
        {
            /*Codes_SRS_SCHEMA_99_164: [If the function succeeds, then the return value shall be SCHEMA_OK.]*/
            result = SCHEMA_OK;
        }
    }
    return result;
}


SCHEMA_RESULT Schema_GetModelModelCount(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, size_t* modelCount)
{
    SCHEMA_RESULT result;
    /*Codes_SRS_SCHEMA_99_169: [If any of the parameters is NULL, then the function shall return SCHEMA_INVALID_ARG.]*/
    if (
        (modelTypeHandle == NULL) ||
        (modelCount == NULL)
        )
    {
        result = SCHEMA_INVALID_ARG;
        LogError("(Error code: %s)\r\n", ENUM_TO_STRING(SCHEMA_RESULT, result));
    }
    else
    {
        MODEL_TYPE* model = (MODEL_TYPE*)modelTypeHandle;
        /*Codes_SRS_SCHEMA_99_167: [Schema_GetModelModelCount shall return in parameter modelCount the number of models inserted in the model identified by parameter modelTypeHandle.]*/
        *modelCount = VECTOR_size(model->models);
        /*SRS_SCHEMA_99_168: [If the function succeeds, it shall return SCHEMA_OK.]*/
        result = SCHEMA_OK;
    }
    return result;
}

static bool matchModelName(const void* element, const void* value)
{
    MODEL_IN_MODEL* decodedElement = (MODEL_IN_MODEL*)element;
    const char* name = (const char*)value;
    return (strcmp(decodedElement->propertyName, name) == 0);
}

SCHEMA_MODEL_TYPE_HANDLE Schema_GetModelModelByName(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, const char* propertyName)
{
    SCHEMA_MODEL_TYPE_HANDLE result;
    if (
        (modelTypeHandle == NULL) ||
        (propertyName == NULL)
        )
    {
        /*Codes_SRS_SCHEMA_99_171: [If Schema_GetModelModelByName is unable to provide the handle it shall return NULL.]*/
        result = NULL;
        LogError("error SCHEMA_INVALID_ARG");
    }
    else
    {
        MODEL_TYPE* model = (MODEL_TYPE*)modelTypeHandle;
        /*Codes_SRS_SCHEMA_99_170: [Schema_GetModelModelByName shall return a handle to the model identified by the property with the name propertyName in the model identified by the handle modelTypeHandle.]*/
        /*Codes_SRS_SCHEMA_99_171: [If Schema_GetModelModelByName is unable to provide the handle it shall return NULL.]*/
        void* temp = VECTOR_find_if(model->models, matchModelName, propertyName);
        if (temp == NULL)
        {
            LogError("specified propertyName not found (%s)", propertyName);
            result = NULL;
        }
        else
        {
            result = ((MODEL_IN_MODEL*)temp)->modelHandle;
        }
    }
    return result;
}

SCHEMA_MODEL_TYPE_HANDLE Schema_GetModelModelyByIndex(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, size_t index)
{
    SCHEMA_MODEL_TYPE_HANDLE result;
    if (
        (modelTypeHandle == NULL) 
        )
    {
        /*Codes_SRS_SCHEMA_99_173: [Schema_GetModelModelyByIndex shall return NULL in the cases when it cannot provide the handle.]*/
        result = NULL;
        LogError("error SCHEMA_INVALID_ARG");
    }
    else
    {
        MODEL_TYPE* model = (MODEL_TYPE*)modelTypeHandle;
        size_t nModelsInModel;
        /*Codes_SRS_SCHEMA_99_172: [ Schema_GetModelModelyByIndex shall return a handle to the "index"th model inserted in the model identified by the parameter modelTypeHandle.]*/
        /*Codes_SRS_SCHEMA_99_173: [Schema_GetModelModelyByIndex shall return NULL in the cases when it cannot provide the handle.]*/
        nModelsInModel = VECTOR_size(model->models);
        if (index < nModelsInModel)
        {
            result = ((MODEL_IN_MODEL*)VECTOR_element(model->models, index))->modelHandle;
        }
        else
        {
            LogError("attempted out of bounds access in array.");
            result = NULL; /*out of bounds array access*/
        }
    }
    return result;
}

const char* Schema_GetModelModelPropertyNameByIndex(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, size_t index)
{
    const char* result;
    if (modelTypeHandle == NULL)
    {
        /*Codes_SRS_SCHEMA_99_176: [If Schema_GetModelModelPropertyNameByIndex cannot produce the property name, it shall return NULL.]*/
        result = NULL;
    }
    else
    {
        MODEL_TYPE* model = (MODEL_TYPE*)modelTypeHandle;
        size_t nModelsInModel;
        /*Codes_SRS_SCHEMA_99_175: [Schema_GetModelModelPropertyNameByIndex shall return the name of the property for the "index"th model in the model identified by modelTypeHandle parameter.]*/
        /*Codes_SRS_SCHEMA_99_176: [If Schema_GetModelModelPropertyNameByIndex cannot produce the property name, it shall return NULL.]*/
        nModelsInModel = VECTOR_size(model->models);
        if (index < nModelsInModel)
        {
            result = ((MODEL_IN_MODEL*)VECTOR_element(model->models, index))->propertyName;
        }
        else
        {
            LogError("attempted out of bounds access in array.");
            result = NULL; /*out of bounds array access*/
        }
    }
    return result;
}

bool Schema_ModelPropertyByPathExists(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, const char* propertyPath)
{
    bool result;

    /* Codes_SRS_SCHEMA_99_180: [If any of the arguments are NULL, Schema_ModelPropertyByPathExists shall return false.] */
    if ((modelTypeHandle == NULL) ||
        (propertyPath == NULL))
    {
        LogError("error SCHEMA_INVALID_ARG");
        result = false;
    }
    else
    {
        const char* slashPos;
        result = false;

        /* Codes_SRS_SCHEMA_99_182: [A single slash ('/') at the beginning of the path shall be ignored and the path shall still be valid.] */
        if (*propertyPath == '/')
        {
            propertyPath++;
        }

        do
        {
            const char* endPos;
            size_t i;
            size_t modelCount;
            MODEL_TYPE* modelType = (MODEL_TYPE*)modelTypeHandle;

            /* Codes_SRS_SCHEMA_99_179: [The propertyPath shall be assumed to be in the format model1/model2/.../propertyName.] */
            slashPos = strchr(propertyPath, '/');
            endPos = slashPos;
            if (endPos == NULL)
            {
                endPos = &propertyPath[strlen(propertyPath)];
            }

            /* get the child-model */
            modelCount = VECTOR_size(modelType->models);
            for (i = 0; i < modelCount; i++)
            {
                MODEL_IN_MODEL* childModel = (MODEL_IN_MODEL*)VECTOR_element(modelType->models, i);
                if ((strncmp(childModel->propertyName, propertyPath, endPos - propertyPath) == 0) &&
                    (strlen(childModel->propertyName) == (size_t)(endPos - propertyPath)))
                {
                    /* found */
                    modelTypeHandle = childModel->modelHandle;
                    break;
                }
            }

            if (i < modelCount)
            {
                /* model found, check if there is more in the path */
                if (slashPos == NULL)
                {
                    /* this is the last one, so this is the thing we were looking for */
                    result = true;
                    break;
                }
                else
                {
                    /* continue looking, there's more  */
                    propertyPath = slashPos + 1;
                }
            }
            else
            {
                /* no model found, let's see if this is a property */
                /* Codes_SRS_SCHEMA_99_178: [The argument propertyPath shall be used to find the leaf property.] */
                for (i = 0; i < modelType->PropertyCount; i++)
                {
                    PROPERTY* property = (PROPERTY*)modelType->Properties[i];
                    if ((strncmp(property->PropertyName, propertyPath, endPos - propertyPath) == 0) &&
                        (strlen(property->PropertyName) == (size_t)(endPos - propertyPath)))
                    {
                        /* found property */
                        /* Codes_SRS_SCHEMA_99_177: [Schema_ModelPropertyByPathExists shall return true if a leaf property exists in the model modelTypeHandle.] */
                        result = true;
                        break;
                    }
                }

                break;
            }
        } while (slashPos != NULL);
    }

    return result;
}
