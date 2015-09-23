// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef SCHEMA_H
#define SCHEMA_H

#include "macro_utils.h"
#include "crt_abstractions.h"

#ifdef __cplusplus
#include <cstddef>
extern "C" {
#else
#include <stddef.h>
#endif

/* Codes_SRS_SCHEMA_99_095: [Schema shall expose the following API:] */

typedef void* SCHEMA_HANDLE;
typedef void* SCHEMA_MODEL_TYPE_HANDLE;
typedef void* SCHEMA_STRUCT_TYPE_HANDLE;
typedef void* SCHEMA_PROPERTY_HANDLE;
typedef void* SCHEMA_ACTION_HANDLE;
typedef void* SCHEMA_ACTION_ARGUMENT_HANDLE;

#define SCHEMA_RESULT_VALUES    \
SCHEMA_OK,                      \
SCHEMA_INVALID_ARG,             \
SCHEMA_DUPLICATE_ELEMENT,       \
SCHEMA_ELEMENT_NOT_FOUND,       \
SCHEMA_MODEL_IN_USE,            \
SCHEMA_DEVICE_COUNT_ZERO,       \
SCHEMA_ERROR

DEFINE_ENUM(SCHEMA_RESULT, SCHEMA_RESULT_VALUES)

extern SCHEMA_HANDLE Schema_Create(const char* schemaNamespace);
extern size_t Schema_GetSchemaCount(void);
extern SCHEMA_HANDLE Schema_GetSchemaByNamespace(const char* schemaNamespace);
extern const char* Schema_GetSchemaNamespace(SCHEMA_HANDLE schemaHandle);
extern SCHEMA_RESULT Schema_AddDeviceRef(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle);
extern SCHEMA_RESULT Schema_ReleaseDeviceRef(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle);

extern SCHEMA_MODEL_TYPE_HANDLE Schema_CreateModelType(SCHEMA_HANDLE schemaHandle, const char* modelName);
extern SCHEMA_HANDLE Schema_GetSchemaForModelType(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle);
extern const char* Schema_GetModelName(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle);

extern SCHEMA_STRUCT_TYPE_HANDLE Schema_CreateStructType(SCHEMA_HANDLE schemaHandle, const char* structTypeName);

extern const char* Schema_GetStructTypeName(SCHEMA_STRUCT_TYPE_HANDLE structTypeHandle);

extern SCHEMA_RESULT Schema_AddStructTypeProperty(SCHEMA_STRUCT_TYPE_HANDLE structTypeHandle, const char* propertyName, const char* propertyType);

extern SCHEMA_RESULT Schema_AddModelProperty(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, const char* propertyName, const char* propertyType);
extern SCHEMA_RESULT Schema_AddModelModel(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, const char* propertyName, SCHEMA_MODEL_TYPE_HANDLE modelType);
extern SCHEMA_ACTION_HANDLE Schema_CreateModelAction(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, const char* actionName);
extern SCHEMA_RESULT Schema_AddModelActionArgument(SCHEMA_ACTION_HANDLE actionHandle, const char* argumentName, const char* argumentType);

extern SCHEMA_RESULT Schema_GetModelCount(SCHEMA_HANDLE schemaHandle, size_t* modelCount);
extern SCHEMA_MODEL_TYPE_HANDLE Schema_GetModelByName(SCHEMA_HANDLE schemaHandle, const char* modelName);
extern SCHEMA_MODEL_TYPE_HANDLE Schema_GetModelByIndex(SCHEMA_HANDLE schemaHandle, size_t index);

extern SCHEMA_RESULT Schema_GetModelPropertyCount(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, size_t* propertyCount);
extern SCHEMA_PROPERTY_HANDLE Schema_GetModelPropertyByName(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, const char* propertyName);
extern SCHEMA_PROPERTY_HANDLE Schema_GetModelPropertyByIndex(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, size_t index);

extern SCHEMA_RESULT Schema_GetModelModelCount(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, size_t* modelCount);
extern SCHEMA_MODEL_TYPE_HANDLE Schema_GetModelModelByName(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, const char* propertyName);
extern SCHEMA_MODEL_TYPE_HANDLE Schema_GetModelModelyByIndex(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, size_t index);
extern const char* Schema_GetModelModelPropertyNameByIndex(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, size_t index);

extern bool Schema_ModelPropertyByPathExists(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, const char* propertyPath);

extern SCHEMA_RESULT Schema_GetModelActionCount(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, size_t* actionCount);
extern SCHEMA_ACTION_HANDLE Schema_GetModelActionByName(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, const char* actionName);
extern SCHEMA_ACTION_HANDLE Schema_GetModelActionByIndex(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, size_t index);

extern SCHEMA_RESULT Schema_GetModelActionArgumentCount(SCHEMA_ACTION_HANDLE actionHandle, size_t* argumentCount);
extern const char* Schema_GetModelActionName(SCHEMA_ACTION_HANDLE actionHandle);

extern SCHEMA_ACTION_ARGUMENT_HANDLE Schema_GetModelActionArgumentByName(SCHEMA_ACTION_HANDLE actionHandle, const char* actionArgumentName);
extern SCHEMA_ACTION_ARGUMENT_HANDLE Schema_GetModelActionArgumentByIndex(SCHEMA_ACTION_HANDLE actionHandle, size_t argumentIndex);
extern const char* Schema_GetActionArgumentName(SCHEMA_ACTION_ARGUMENT_HANDLE actionArgumentHandle);
extern const char* Schema_GetActionArgumentType(SCHEMA_ACTION_ARGUMENT_HANDLE actionArgumentHandle);

extern SCHEMA_RESULT Schema_GetStructTypeCount(SCHEMA_HANDLE schemaHandle, size_t* structTypeCount);
extern SCHEMA_STRUCT_TYPE_HANDLE Schema_GetStructTypeByName(SCHEMA_HANDLE schemaHandle, const char* structTypeName);
extern SCHEMA_STRUCT_TYPE_HANDLE Schema_GetStructTypeByIndex(SCHEMA_HANDLE schemaHandle, size_t index);

extern SCHEMA_RESULT Schema_GetStructTypePropertyCount(SCHEMA_STRUCT_TYPE_HANDLE structTypeHandle, size_t* propertyCount);
extern SCHEMA_PROPERTY_HANDLE Schema_GetStructTypePropertyByName(SCHEMA_STRUCT_TYPE_HANDLE structTypeHandle, const char* propertyName);
extern SCHEMA_PROPERTY_HANDLE Schema_GetStructTypePropertyByIndex(SCHEMA_STRUCT_TYPE_HANDLE structTypeHandle, size_t index);
extern const char* Schema_GetPropertyName(SCHEMA_PROPERTY_HANDLE propertyHandle);
extern const char* Schema_GetPropertyType(SCHEMA_PROPERTY_HANDLE propertyHandle);

extern void Schema_Destroy(SCHEMA_HANDLE schemaHandle);
extern SCHEMA_RESULT Schema_DestroyIfUnused(SCHEMA_MODEL_TYPE_HANDLE modelHandle);

#ifdef __cplusplus
}
#endif

#endif /* SCHEMA_H */
