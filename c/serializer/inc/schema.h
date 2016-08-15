// Copyright (c) Microsoft. All rights reserved.MOCKABLE_FUNCTION(,
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef SCHEMA_H
#define SCHEMA_H

#include "azure_c_shared_utility/macro_utils.h"
#include "azure_c_shared_utility/crt_abstractions.h"

#ifdef __cplusplus
#include <cstddef>
extern "C" {
#else
#include <stddef.h>
#endif

/* Codes_SRS_SCHEMA_99_095: [Schema shall expose the following API:] */

typedef struct SCHEMA_HANDLE_DATA_TAG* SCHEMA_HANDLE;
typedef struct SCHEMA_MODEL_TYPE_HANDLE_DATA_TAG* SCHEMA_MODEL_TYPE_HANDLE;
typedef struct SCHEMA_STRUCT_TYPE_HANDLE_DATA_TAG* SCHEMA_STRUCT_TYPE_HANDLE;
typedef struct SCHEMA_PROPERTY_HANDLE_DATA_TAG* SCHEMA_PROPERTY_HANDLE;
typedef struct SCHEMA_ACTION_HANDLE_DATA_TAG* SCHEMA_ACTION_HANDLE;
typedef struct SCHEMA_ACTION_ARGUMENT_HANDLE_DATA_TAG* SCHEMA_ACTION_ARGUMENT_HANDLE;

#define SCHEMA_RESULT_VALUES    \
SCHEMA_OK,                      \
SCHEMA_INVALID_ARG,             \
SCHEMA_DUPLICATE_ELEMENT,       \
SCHEMA_ELEMENT_NOT_FOUND,       \
SCHEMA_MODEL_IN_USE,            \
SCHEMA_DEVICE_COUNT_ZERO,       \
SCHEMA_ERROR

DEFINE_ENUM(SCHEMA_RESULT, SCHEMA_RESULT_VALUES)

#include "azure_c_shared_utility/umock_c_prod.h"
MOCKABLE_FUNCTION(, SCHEMA_HANDLE, Schema_Create, const char*, schemaNamespace);
MOCKABLE_FUNCTION(, size_t, Schema_GetSchemaCount);
MOCKABLE_FUNCTION(, SCHEMA_HANDLE, Schema_GetSchemaByNamespace, const char*, schemaNamespace);
MOCKABLE_FUNCTION(, const char*, Schema_GetSchemaNamespace, SCHEMA_HANDLE, schemaHandle);
MOCKABLE_FUNCTION(, SCHEMA_RESULT, Schema_AddDeviceRef, SCHEMA_MODEL_TYPE_HANDLE, modelTypeHandle);
MOCKABLE_FUNCTION(, SCHEMA_RESULT, Schema_ReleaseDeviceRef, SCHEMA_MODEL_TYPE_HANDLE, modelTypeHandle);

MOCKABLE_FUNCTION(, SCHEMA_MODEL_TYPE_HANDLE, Schema_CreateModelType, SCHEMA_HANDLE, schemaHandle, const char*, modelName);
MOCKABLE_FUNCTION(, SCHEMA_HANDLE, Schema_GetSchemaForModelType, SCHEMA_MODEL_TYPE_HANDLE, modelTypeHandle);
MOCKABLE_FUNCTION(, const char*, Schema_GetModelName, SCHEMA_MODEL_TYPE_HANDLE, modelTypeHandle);

MOCKABLE_FUNCTION(, SCHEMA_STRUCT_TYPE_HANDLE, Schema_CreateStructType, SCHEMA_HANDLE, schemaHandle, const char*, structTypeName);

MOCKABLE_FUNCTION(, const char*, Schema_GetStructTypeName, SCHEMA_STRUCT_TYPE_HANDLE, structTypeHandle);

MOCKABLE_FUNCTION(, SCHEMA_RESULT, Schema_AddStructTypeProperty, SCHEMA_STRUCT_TYPE_HANDLE, structTypeHandle, const char*, propertyName, const char*, propertyType);

MOCKABLE_FUNCTION(, SCHEMA_RESULT, Schema_AddModelProperty, SCHEMA_MODEL_TYPE_HANDLE, modelTypeHandle, const char*, propertyName, const char*, propertyType);
MOCKABLE_FUNCTION(, SCHEMA_RESULT, Schema_AddModelReportedProperty, SCHEMA_MODEL_TYPE_HANDLE, modelTypeHandle, const char*, reportedPropertyName, const char*, reportedPropertyType);
MOCKABLE_FUNCTION(, SCHEMA_RESULT, Schema_AddModelModel, SCHEMA_MODEL_TYPE_HANDLE, modelTypeHandle, const char*, propertyName, SCHEMA_MODEL_TYPE_HANDLE, modelType);
MOCKABLE_FUNCTION(, SCHEMA_ACTION_HANDLE, Schema_CreateModelAction, SCHEMA_MODEL_TYPE_HANDLE, modelTypeHandle, const char*, actionName);
MOCKABLE_FUNCTION(, SCHEMA_RESULT, Schema_AddModelActionArgument, SCHEMA_ACTION_HANDLE, actionHandle, const char*, argumentName, const char*, argumentType);

MOCKABLE_FUNCTION(, SCHEMA_RESULT, Schema_GetModelCount, SCHEMA_HANDLE, schemaHandle, size_t*, modelCount);
MOCKABLE_FUNCTION(, SCHEMA_MODEL_TYPE_HANDLE, Schema_GetModelByName, SCHEMA_HANDLE, schemaHandle, const char*, modelName);
MOCKABLE_FUNCTION(, SCHEMA_MODEL_TYPE_HANDLE, Schema_GetModelByIndex, SCHEMA_HANDLE, schemaHandle, size_t, index);

MOCKABLE_FUNCTION(, SCHEMA_RESULT, Schema_GetModelPropertyCount, SCHEMA_MODEL_TYPE_HANDLE, modelTypeHandle, size_t*, propertyCount);
MOCKABLE_FUNCTION(, SCHEMA_PROPERTY_HANDLE, Schema_GetModelPropertyByName, SCHEMA_MODEL_TYPE_HANDLE, modelTypeHandle, const char*, propertyName);
MOCKABLE_FUNCTION(, SCHEMA_PROPERTY_HANDLE, Schema_GetModelPropertyByIndex, SCHEMA_MODEL_TYPE_HANDLE, modelTypeHandle, size_t, index);

MOCKABLE_FUNCTION(, SCHEMA_RESULT, Schema_GetModelReportedPropertyCount, SCHEMA_MODEL_TYPE_HANDLE, modelTypeHandle, size_t*, reportedPropertyCount);
MOCKABLE_FUNCTION(, SCHEMA_PROPERTY_HANDLE, Schema_GetModelReportedPropertyByName, SCHEMA_MODEL_TYPE_HANDLE, modelTypeHandle, const char*, reportedPropertyName);
MOCKABLE_FUNCTION(, SCHEMA_PROPERTY_HANDLE, Schema_GetModelReportedPropertyByIndex, SCHEMA_MODEL_TYPE_HANDLE, modelTypeHandle, size_t, index);

MOCKABLE_FUNCTION(, SCHEMA_RESULT, Schema_GetModelModelCount, SCHEMA_MODEL_TYPE_HANDLE, modelTypeHandle, size_t*, modelCount);
MOCKABLE_FUNCTION(, SCHEMA_MODEL_TYPE_HANDLE, Schema_GetModelModelByName, SCHEMA_MODEL_TYPE_HANDLE, modelTypeHandle, const char*, propertyName);
MOCKABLE_FUNCTION(, SCHEMA_MODEL_TYPE_HANDLE, Schema_GetModelModelyByIndex, SCHEMA_MODEL_TYPE_HANDLE, modelTypeHandle, size_t, index);
MOCKABLE_FUNCTION(, const char*, Schema_GetModelModelPropertyNameByIndex, SCHEMA_MODEL_TYPE_HANDLE, modelTypeHandle, size_t, index);

MOCKABLE_FUNCTION(, bool, Schema_ModelPropertyByPathExists, SCHEMA_MODEL_TYPE_HANDLE, modelTypeHandle, const char*, propertyPath);

MOCKABLE_FUNCTION(, bool, Schema_ModelReportedPropertyByPathExists, SCHEMA_MODEL_TYPE_HANDLE, modelTypeHandle, const char*, reportedPropertyPath);

MOCKABLE_FUNCTION(, SCHEMA_RESULT, Schema_GetModelActionCount, SCHEMA_MODEL_TYPE_HANDLE, modelTypeHandle, size_t*, actionCount);
MOCKABLE_FUNCTION(, SCHEMA_ACTION_HANDLE, Schema_GetModelActionByName, SCHEMA_MODEL_TYPE_HANDLE, modelTypeHandle, const char*, actionName);
MOCKABLE_FUNCTION(, SCHEMA_ACTION_HANDLE, Schema_GetModelActionByIndex, SCHEMA_MODEL_TYPE_HANDLE, modelTypeHandle, size_t, index);

MOCKABLE_FUNCTION(, SCHEMA_RESULT, Schema_GetModelActionArgumentCount, SCHEMA_ACTION_HANDLE, actionHandle, size_t*, argumentCount);
MOCKABLE_FUNCTION(, const char*, Schema_GetModelActionName, SCHEMA_ACTION_HANDLE, actionHandle);

MOCKABLE_FUNCTION(, SCHEMA_ACTION_ARGUMENT_HANDLE, Schema_GetModelActionArgumentByName, SCHEMA_ACTION_HANDLE, actionHandle, const char*, actionArgumentName);
MOCKABLE_FUNCTION(, SCHEMA_ACTION_ARGUMENT_HANDLE, Schema_GetModelActionArgumentByIndex, SCHEMA_ACTION_HANDLE, actionHandle, size_t, argumentIndex);
MOCKABLE_FUNCTION(, const char*, Schema_GetActionArgumentName, SCHEMA_ACTION_ARGUMENT_HANDLE, actionArgumentHandle);
MOCKABLE_FUNCTION(, const char*, Schema_GetActionArgumentType, SCHEMA_ACTION_ARGUMENT_HANDLE, actionArgumentHandle);

MOCKABLE_FUNCTION(, SCHEMA_RESULT, Schema_GetStructTypeCount, SCHEMA_HANDLE, schemaHandle, size_t*, structTypeCount);
MOCKABLE_FUNCTION(, SCHEMA_STRUCT_TYPE_HANDLE, Schema_GetStructTypeByName, SCHEMA_HANDLE, schemaHandle, const char*, structTypeName);
MOCKABLE_FUNCTION(, SCHEMA_STRUCT_TYPE_HANDLE, Schema_GetStructTypeByIndex, SCHEMA_HANDLE, schemaHandle, size_t, index);

MOCKABLE_FUNCTION(, SCHEMA_RESULT, Schema_GetStructTypePropertyCount, SCHEMA_STRUCT_TYPE_HANDLE, structTypeHandle, size_t*, propertyCount);
MOCKABLE_FUNCTION(, SCHEMA_PROPERTY_HANDLE, Schema_GetStructTypePropertyByName, SCHEMA_STRUCT_TYPE_HANDLE, structTypeHandle, const char*, propertyName);
MOCKABLE_FUNCTION(, SCHEMA_PROPERTY_HANDLE, Schema_GetStructTypePropertyByIndex, SCHEMA_STRUCT_TYPE_HANDLE, structTypeHandle, size_t, index);
MOCKABLE_FUNCTION(, const char*, Schema_GetPropertyName, SCHEMA_PROPERTY_HANDLE, propertyHandle);
MOCKABLE_FUNCTION(, const char*, Schema_GetPropertyType, SCHEMA_PROPERTY_HANDLE, propertyHandle);

MOCKABLE_FUNCTION(, void, Schema_Destroy, SCHEMA_HANDLE, schemaHandle);
MOCKABLE_FUNCTION(, SCHEMA_RESULT, Schema_DestroyIfUnused,SCHEMA_MODEL_TYPE_HANDLE, modelHandle);

#ifdef __cplusplus
}
#endif

#endif /* SCHEMA_H */
