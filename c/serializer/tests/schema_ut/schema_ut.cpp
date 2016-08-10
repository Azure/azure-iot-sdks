// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <cstdlib>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "testrunnerswitcher.h"
#include "micromock.h"
#include "micromockcharstararenullterminatedstrings.h"
#include "schema.h"


static MICROMOCK_MUTEX_HANDLE g_testByTest;

DEFINE_MICROMOCK_ENUM_TO_STRING(SCHEMA_RESULT, SCHEMA_RESULT_VALUES);

static const char SCHEMA_NAMESPACE[] = "TestNamespace";
static const char MODEL_NAME[] = "TestModelName";

static MICROMOCK_GLOBAL_SEMAPHORE_HANDLE g_dllByDll;

BEGIN_TEST_SUITE(Schema_ut)

    TEST_SUITE_INITIALIZE(TestClassInitialize)
    {
        TEST_INITIALIZE_MEMORY_DEBUG(g_dllByDll);

        g_testByTest = MicroMockCreateMutex();
        ASSERT_IS_NOT_NULL(g_testByTest);
    }

    TEST_SUITE_CLEANUP(TestClassCleanup)
    {
        MicroMockDestroyMutex(g_testByTest);
        TEST_DEINITIALIZE_MEMORY_DEBUG(g_dllByDll);

    }

    TEST_FUNCTION_INITIALIZE(TestMethodInitialize)
    {
        if (!MicroMockAcquireMutex(g_testByTest))
        {
            ASSERT_FAIL("our mutex is ABANDONED. Failure in test framework");
        }
    }

    TEST_FUNCTION_CLEANUP(TestMethodCleanup)
    {
        if (!MicroMockReleaseMutex(g_testByTest))
        {
            ASSERT_FAIL("failure in test framework at ReleaseMutex");
        }
    }

    /* Schema_Create */

    /* Tests_SRS_SCHEMA_99_001:[Schema_Create shall initialize a schema with a given namespace.] */
    /* Tests_SRS_SCHEMA_99_002:[On success a non-NULL handle to the newly created schema shall be returned.] */
    TEST_FUNCTION(Schema_Create_Initializes_A_Schema_Returns_Non_Null_Handle)
    {
        // arrange

        // act
        SCHEMA_HANDLE result = Schema_Create(SCHEMA_NAMESPACE);

        // assert
        ASSERT_IS_NOT_NULL(result);

        // cleanup
        Schema_Destroy(result);
    }

    /* Tests_SRS_SCHEMA_99_003:[On failure, NULL shall be returned.] */
    /* Tests_SRS_SCHEMA_99_004:[If schemaNamespace is NULL, Schema_Create shall fail.] */
    TEST_FUNCTION(Schema_Create_Initializes_A_Schema_With_NULL_Returns_NULL_Handle)
    {
        // arrange

        // act
        SCHEMA_HANDLE result = Schema_Create(NULL);

        // assert
        ASSERT_IS_NULL(result);
    }

    /* Tests_SRS_SCHEMA_99_001:[Schema_Create shall initialize a schema with a given namespace.] */
    TEST_FUNCTION(Schema_Create_Called_Twice_With_Different_NameSpace_Succeeds)
    {
        // arrange

        // act
        SCHEMA_HANDLE result1 = Schema_Create("Namespace1");
        SCHEMA_HANDLE result2 = Schema_Create("Namespace2");

        // assert
        ASSERT_IS_NOT_NULL(result1);
        ASSERT_IS_NOT_NULL(result2);
        ASSERT_ARE_NOT_EQUAL(void_ptr, (void*)result1, (void*)result2);

        // cleanup
        Schema_Destroy(result1);
        Schema_Destroy(result2);
    }

    /* Tests_SRS_SCHEMA_99_001:[Schema_Create shall initialize a schema with a given namespace.] */
    TEST_FUNCTION(Schema_Create_Called_Twice_With_Same_NameSpace_Succeeds)
    {
        // arrange

        // act
        SCHEMA_HANDLE result1 = Schema_Create("Namespace");
        SCHEMA_HANDLE result2 = Schema_Create("Namespace");

        // assert
        ASSERT_IS_NOT_NULL(result1);
        ASSERT_IS_NOT_NULL(result2);
        ASSERT_ARE_NOT_EQUAL(void_ptr, (void*)result1, (void*)result2);

        // cleanup
        Schema_Destroy(result1);
        Schema_Destroy(result2);
    }

    /* Schema_GetSchemaCount */

    /* Tests_SRS_SCHEMA_99_153: [Schema_GetSchemaCount shall return the number of "active" schemas (all schemas created with Schema_Create 
       in the current process, for which Schema_Destroy has not been called).] */
    TEST_FUNCTION(Schema_GetSchemaCount_should_return_zero_when_no_schemas_were_created)
    {
        // arrange

        // act
        size_t count = Schema_GetSchemaCount();

        // assert
        ASSERT_ARE_EQUAL(size_t, 0, count);
    }

    /* Tests_SRS_SCHEMA_99_153: [Schema_GetSchemaCount shall return the number of "active" schemas (all schemas created with Schema_Create
    in the current process, for which Schema_Destroy has not been called).] */
    TEST_FUNCTION(Schema_GetSchemaCount_should_return_the_number_of_active_schemas)
    {
        // arrange
        SCHEMA_HANDLE h1 = Schema_Create("one");
        SCHEMA_HANDLE h2 = Schema_Create("two");

        // act
        size_t count = Schema_GetSchemaCount();

        // assert
        ASSERT_ARE_EQUAL(size_t, 2, count);

        // cleanup
        Schema_Destroy(h1);
        Schema_Destroy(h2);
    }

    /* Tests_SRS_SCHEMA_99_153: [Schema_GetSchemaCount shall return the number of "active" schemas (all schemas created with Schema_Create
    in the current process, for which Schema_Destroy has not been called).] */
    TEST_FUNCTION(Schema_GetSchemaCount_should_return_the_correct_number_after_a_schema_is_destroyed)
    {
        // arrange
        SCHEMA_HANDLE h1 = Schema_Create("one");
        SCHEMA_HANDLE h2 = Schema_Create("two");
        Schema_Destroy(h1);

        // act
        size_t count = Schema_GetSchemaCount();

        // assert
        ASSERT_ARE_EQUAL(size_t, 1, count);

        // cleanup
        Schema_Destroy(h2);
    }

    /* Tests_SRS_SCHEMA_99_153: [Schema_GetSchemaCount shall return the number of "active" schemas (all schemas created with Schema_Create
    in the current process, for which Schema_Destroy has not been called).] */
    TEST_FUNCTION(Schema_GetSchemaCount_should_return_the_correct_number_after_a_bounce)
    {
        // arrange
        SCHEMA_HANDLE h1 = Schema_Create("one");
        Schema_Destroy(h1);
        SCHEMA_HANDLE h2 = Schema_Create("two");

        // act
        size_t count = Schema_GetSchemaCount();

        // assert
        ASSERT_ARE_EQUAL(size_t, 1, count);

        // cleanup
        Schema_Destroy(h2);
    }

    /* Schema_GetSchemaByNamespace */

    /* Tests_SRS_SCHEMA_99_150: [If the schemaNamespace argument is NULL, Schema_GetSchemaByNamespace shall return NULL.] */
    TEST_FUNCTION(Schema_GetSchemaByNamespace_with_a_NULL_namespace_argument_returns_NULL)
    {
        // arrange

        // act
        SCHEMA_HANDLE schema = Schema_GetSchemaByNamespace(NULL);

        // assert
        ASSERT_IS_NULL(schema);
    }

    /* Tests_SRS_SCHEMA_99_151: [If no active schema matches the schemaNamespace argument, Schema_GetSchemaByNamespace shall return NULL.] */
    TEST_FUNCTION(Schema_GetSchemaByNamespace_with_a_nonmatching_namespace_argument_should_return_NULL)
    {
        // arrange

        // act
        SCHEMA_HANDLE handle = Schema_GetSchemaByNamespace("namespace");

        // assert
        ASSERT_IS_NULL(handle);
    }

    /* Tests_SRS_SCHEMA_99_148: [Schema_GetSchemaByNamespace shall search all active schemas and return the schema with the 
       namespace given by the schemaNamespace argument.] */
    TEST_FUNCTION(Schema_GetSchemaByNamespace_with_a_matching_namespace_argument_should_return_the_schema)
    {
        // arrange
        SCHEMA_HANDLE create1 = Schema_Create("one");
        SCHEMA_HANDLE create2 = Schema_Create("two");
        SCHEMA_HANDLE create3 = Schema_Create("three");

        // act
        SCHEMA_HANDLE get2 = Schema_GetSchemaByNamespace("two");

        // assert
        ASSERT_ARE_EQUAL(void_ptr, create2, get2);

        // cleanup
        Schema_Destroy(create1);
        Schema_Destroy(create2);
        Schema_Destroy(create3);
    }

    /* Schema_GetSchemaNamespace */

    /* Tests_SRS_SCHEMA_99_130: [If the schemaHandle argument is NULL, Schema_GetSchemaNamespace shall return NULL.] */
    TEST_FUNCTION(Schema_GetSchemaNamespace_With_A_NULL_Handle_Fails)
    {
        // arrange

        // act
        const char* result = Schema_GetSchemaNamespace(NULL);

        // assert
        ASSERT_IS_NULL(result);
    }

    /* Tests_SRS_SCHEMA_99_129: [Schema_GetSchemaNamespace shall return the namespace for the schema identified by schemaHandle.] */
    TEST_FUNCTION(Schema_GetSchemaNamespace_With_A_Valid_Handle_Returns_The_Schema_Namespace)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);

        // act
        const char* result = Schema_GetSchemaNamespace(schemaHandle);

        // assert
        ASSERT_ARE_EQUAL(char_ptr, SCHEMA_NAMESPACE, result);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Schema_Destroy */

    /* Tests_SRS_SCHEMA_99_006:[If the schemaHandle is NULL, Schema_Destroy shall do nothing.] */
    TEST_FUNCTION(Schema_Deinit_With_A_NULL_Handle_Raises_No_Exceptions)
    {
        // arrange

        // act
        Schema_Destroy(NULL);

        // assert
        // no explicit assert
    }

    /* Tests_SRS_SCHEMA_99_005:[Schema_Destroy shall free all resources associated with a schema.] */
    TEST_FUNCTION(Schema_Deinit_With_A_Valid_Handle_Succeeds)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create("Test");
        (void)Schema_CreateModelType(schemaHandle, "ModelName");
        (void)Schema_CreateStructType(schemaHandle, "StructName");

        // act
        Schema_Destroy(schemaHandle);

        // assert
        // no explicit assert
    }

    /* Schema_CreateModelType */

    /* Tests_SRS_SCHEMA_99_009:[On failure, Schema_CreateModelType shall return NULL.] */
    /* Tests_SRS_SCHEMA_99_010:[If any of the arguments is NULL, Schema_CreateModelType shall fail.] */
    TEST_FUNCTION(Schema_CreateModelType_With_schemaHandle_NULL_returns_NULL)
    {
        // arrange

        // act
        SCHEMA_MODEL_TYPE_HANDLE result = Schema_CreateModelType(NULL, MODEL_NAME);

        // assert
        ASSERT_IS_NULL(result);
    }

    /* Tests_SRS_SCHEMA_99_009:[On failure, Schema_CreateModelType shall return NULL.] */
    /* Tests_SRS_SCHEMA_99_010:[If any of the arguments is NULL, Schema_CreateModelType shall fail.] */
    TEST_FUNCTION(Schema_CreateModelType_With_modelNamespace_NULL_returns_NULL)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);

        // act
        SCHEMA_MODEL_TYPE_HANDLE result = Schema_CreateModelType(schemaHandle, NULL);

        // assert
        ASSERT_IS_NULL(result);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests_SRS_SCHEMA_99_007:[Schema_CreateModelType shall create a new model type and return a handle to it.] */
    /* Tests_SRS_SCHEMA_99_008:[On success, a non-NULL handle shall be returned.] */
    TEST_FUNCTION(Schema_CreateModelType_With_Valid_Arguments_Succeeds)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);

        // act
        SCHEMA_MODEL_TYPE_HANDLE result = Schema_CreateModelType(schemaHandle, MODEL_NAME);

        // assert
        ASSERT_IS_NOT_NULL(result);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests_SRS_SCHEMA_99_001:[Schema_Create shall initialize a schema with a given namespace.] */
    TEST_FUNCTION(Schema_CreateModelType_Twice_With_Different_Model_Names_Succeeds)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);

        // act
        SCHEMA_MODEL_TYPE_HANDLE modelType1 = Schema_CreateModelType(schemaHandle, "ModelName1");
        SCHEMA_MODEL_TYPE_HANDLE modelType2 = Schema_CreateModelType(schemaHandle, "ModelName2");

        // assert
        ASSERT_IS_NOT_NULL(modelType1);
        ASSERT_IS_NOT_NULL(modelType2);
        ASSERT_ARE_NOT_EQUAL(void_ptr, (void*)modelType1, (void*)modelType2);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests_SRS_SCHEMA_99_100: [Schema_CreateModelType shall return SCHEMA_DUPLICATE_ELEMENT if modelName already exists.] */
    TEST_FUNCTION(Schema_CreateModelType_Twice_Same_Model_Name_Fails)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);

        // act
        SCHEMA_MODEL_TYPE_HANDLE modelType1 = Schema_CreateModelType(schemaHandle, "ModelName");
        SCHEMA_MODEL_TYPE_HANDLE modelType2 = Schema_CreateModelType(schemaHandle, "ModelName");

        // assert
        ASSERT_IS_NOT_NULL(modelType1);
        ASSERT_IS_NULL(modelType2);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Schema_GetSchemaForModelType */

    /* Tests_SRS_SCHEMA_99_132: [If the modelTypeHandle argument is NULL, Schema_GetSchemaForModelType shall return NULL.] */
    TEST_FUNCTION(Schema_GetSchemaForModelType_With_NULL_Handle_Fails)
    {
        // arrange

        // act
        SCHEMA_HANDLE result = Schema_GetSchemaForModelType(NULL);

        // assert
        ASSERT_IS_NULL(result);
    }

    /* Tests_SRS_SCHEMA_99_131: [Schema_GetSchemaForModelType returns the schema handle for a given model type.] */
    TEST_FUNCTION(Schema_GetSchemaForModelType_With_Valid_Handle_Returns_The_Schema_Handle)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_MODEL_TYPE_HANDLE modelType = Schema_CreateModelType(schemaHandle, "Model");

        // act
        SCHEMA_HANDLE result = Schema_GetSchemaForModelType(modelType);

        // assert
        ASSERT_ARE_EQUAL(void_ptr, (void*)schemaHandle, result);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Schema_AddModelProperty */

    /* Tests_SRS_SCHEMA_99_013:[If any of the arguments is NULL, Schema_AddModelProperty shall return SCHEMA_INVALID_ARG.] */
    TEST_FUNCTION(Schema_AddModelProperty_With_NULL_ModelType_Fails)
    {
        // arrange

        // act
        SCHEMA_RESULT result = Schema_AddModelProperty(NULL, "MyName", "SomeType");

        // assert
        ASSERT_ARE_EQUAL(SCHEMA_RESULT, SCHEMA_INVALID_ARG, result);
    }

    /* Tests_SRS_SCHEMA_99_013:[If any of the arguments is NULL, Schema_AddModelProperty shall return SCHEMA_INVALID_ARG.] */
    TEST_FUNCTION(Schema_AddModelProperty_With_NULL_Property_Name_Fails)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_MODEL_TYPE_HANDLE modelType = Schema_CreateModelType(schemaHandle, "Model");

        // act
        SCHEMA_RESULT result = Schema_AddModelProperty(modelType, NULL, "SomeType");

        // assert
        ASSERT_ARE_EQUAL(SCHEMA_RESULT, SCHEMA_INVALID_ARG, result);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests_SRS_SCHEMA_99_013:[If any of the arguments is NULL, Schema_AddModelProperty shall return SCHEMA_INVALID_ARG.] */
    TEST_FUNCTION(Schema_AddModelProperty_With_NULL_Property_Type_Fails)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_MODEL_TYPE_HANDLE modelType = Schema_CreateModelType(schemaHandle, "Model");

        // act
        SCHEMA_RESULT result = Schema_AddModelProperty(modelType, "MyName", NULL);

        // assert
        ASSERT_ARE_EQUAL(SCHEMA_RESULT, SCHEMA_INVALID_ARG, result);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests_SRS_SCHEMA_99_011:[Schema_AddModelProperty shall add one property to the model type identified by modelTypeHandle.] */
    /* Tests_SRS_SCHEMA_99_012:[On success, Schema_AddModelProperty shall return SCHEMA_OK.] */
    TEST_FUNCTION(Schema_AddModelProperty_With_Valid_Arguments_Succeeds)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_MODEL_TYPE_HANDLE modelType = Schema_CreateModelType(schemaHandle, "Model");

        // act
        SCHEMA_RESULT result = Schema_AddModelProperty(modelType, "MyName", "SomeType");

        // assert
        ASSERT_ARE_EQUAL(SCHEMA_RESULT, SCHEMA_OK, result);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests_SRS_SCHEMA_99_011:[Schema_AddModelProperty shall add one property to the model type identified by modelTypeHandle.] */
    /* Tests_SRS_SCHEMA_99_012:[On success, Schema_AddModelProperty shall return SCHEMA_OK.] */
    TEST_FUNCTION(Schema_AddModelProperty_Adding_2_Properties_With_Different_Names_To_The_Same_Model_Succeeds)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_MODEL_TYPE_HANDLE modelType = Schema_CreateModelType(schemaHandle, "Model");
        (void)Schema_AddModelProperty(modelType, "MyName", "SomeType");

        // act
        SCHEMA_RESULT result = Schema_AddModelProperty(modelType, "MyName2", "SomeType");

        // assert
        ASSERT_ARE_EQUAL(SCHEMA_RESULT, SCHEMA_OK, result);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests_SRS_SCHEMA_99_015:[The property name shall be unique per model, if the same property name is added twice to a model, SCHEMA_DUPLICATE_ELEMENT shall be returned.] */
    TEST_FUNCTION(Schema_AddModelProperty_Adding_The_Same_Property_In_The_Same_Model_Fails)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_MODEL_TYPE_HANDLE modelType = Schema_CreateModelType(schemaHandle, "Model");
        (void)Schema_AddModelProperty(modelType, "PropertyName", "PropertyType");

        // act
        SCHEMA_RESULT result = Schema_AddModelProperty(modelType, "PropertyName", "PropertyType");

        // assert
        ASSERT_ARE_EQUAL(SCHEMA_RESULT, SCHEMA_DUPLICATE_ELEMENT, result);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests_SRS_SCHEMA_99_015:[The property name shall be unique per model, if the same property name is added twice to a model, SCHEMA_DUPLICATE_ELEMENT shall be returned.] */
    TEST_FUNCTION(Schema_AddModelProperty_Adding_The_Same_PropertyName_With_Different_PropertyType_In_The_Same_Model_Fails)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_MODEL_TYPE_HANDLE modelType = Schema_CreateModelType(schemaHandle, "Model");
        (void)Schema_AddModelProperty(modelType, "PropertyName", "PropertyType1");

        // act
        SCHEMA_RESULT result = Schema_AddModelProperty(modelType, "PropertyName", "PropertyType2");

        // assert
        ASSERT_ARE_EQUAL(SCHEMA_RESULT, SCHEMA_DUPLICATE_ELEMENT, result);

        // cleanup
        Schema_Destroy(schemaHandle);
    }
    /* Tests_SRS_SCHEMA_99_015:[The property name shall be unique per model, if the same property name is added twice to a model, SCHEMA_DUPLICATE_ELEMENT shall be returned.] */
    TEST_FUNCTION(Schema_AddModelProperty_Adding_The_Same_Property_In_The_Same_Model_When_The_Duplicate_Property_Is_Not_First_Fails)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_MODEL_TYPE_HANDLE modelType = Schema_CreateModelType(schemaHandle, "Model");
        (void)Schema_AddModelProperty(modelType, "x", "SomeType");
        (void)Schema_AddModelProperty(modelType, "MyName", "SomeType");

        // act
        SCHEMA_RESULT result = Schema_AddModelProperty(modelType, "MyName", "SomeType");

        // assert
        ASSERT_ARE_EQUAL(SCHEMA_RESULT, SCHEMA_DUPLICATE_ELEMENT, result);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests_SRS_SCHEMA_99_015:[The property name shall be unique per model, if the same property name is added twice to a model, SCHEMA_DUPLICATE_ELEMENT shall be returned.] */
    TEST_FUNCTION(Schema_AddModelProperty_Adding_The_Same_Property_In_Different_Models_Succeeds)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_MODEL_TYPE_HANDLE modelType1 = Schema_CreateModelType(schemaHandle, "ModelName1");
        SCHEMA_MODEL_TYPE_HANDLE modelType2 = Schema_CreateModelType(schemaHandle, "ModelName2");
        (void)Schema_AddModelProperty(modelType1, "MyName", "SomeType");

        // act
        SCHEMA_RESULT result = Schema_AddModelProperty(modelType2, "MyName", "SomeType");

        // assert
        ASSERT_ARE_EQUAL(SCHEMA_RESULT, SCHEMA_OK, result);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Schema_CreateModelAction */

    /* Tests_SRS_SCHEMA_99_104: [If any of the modelTypeHandle or actionName arguments is NULL, Schema_CreateModelAction shall return NULL.] */
    TEST_FUNCTION(Schema_CreateModelAction_with_NULL_modelTypeHandle_Fails)
    {
        // arrange

        // act
        SCHEMA_ACTION_HANDLE result = Schema_CreateModelAction(NULL, "ActionName");

        // assert
        ASSERT_IS_NULL(result);
    }

    /* Tests_SRS_SCHEMA_99_104: [If any of the modelTypeHandle or actionName arguments is NULL, Schema_CreateModelAction shall return NULL.] */
    TEST_FUNCTION(Schema_CreateModelAction_with_NULL_actionName_Fails)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle = Schema_CreateModelType(schemaHandle, "ModelName");

        // act
        SCHEMA_ACTION_HANDLE result = Schema_CreateModelAction(modelTypeHandle, NULL);

        // assert
        ASSERT_IS_NULL(result);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests SRS_SCHEMA_99_102: [Schema_CreateModelAction shall add one action to the model type identified by modelTypeHandle.] */
    /* Tests_SRS_SCHEMA_99_103: [On success, Schema_CreateModelAction shall return a none-NULL SCHEMA_ACTION_HANDLE to the newly created action.] */
    TEST_FUNCTION(Schema_CreateModelAction_with_valid_arguments_Succeeds)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle = Schema_CreateModelType(schemaHandle, "ModelName");

        // act
        SCHEMA_ACTION_HANDLE result = Schema_CreateModelAction(modelTypeHandle, "ActionName");

        // assert
        ASSERT_IS_NOT_NULL(result);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests_SRS_SCHEMA_99_105: [The action name shall be unique per model, if the same action name is added twice to a model, Schema_CreateModelAction shall return NULL.] */
    TEST_FUNCTION(Schema_CreateModelAction_with_Different_Action_Names_Succeed)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle = Schema_CreateModelType(schemaHandle, "ModelName");

        // act
        SCHEMA_ACTION_HANDLE result1 = Schema_CreateModelAction(modelTypeHandle, "ActionName1");
        SCHEMA_ACTION_HANDLE result2 = Schema_CreateModelAction(modelTypeHandle, "ActionName2");

        // assert
        ASSERT_IS_NOT_NULL(result1);
        ASSERT_IS_NOT_NULL(result2);
        ASSERT_ARE_NOT_EQUAL(void_ptr, (void*)result1, (void*)result2);

        // cleanup
        Schema_Destroy(schemaHandle);
    }
    
    /* Tests_SRS_SCHEMA_99_105: [The action name shall be unique per model, if the same action name is added twice to a model, Schema_CreateModelAction shall return NULL.] */
    TEST_FUNCTION(Schema_CreateModelAction_with_Same_Action_To_Same_Model_Fail)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle = Schema_CreateModelType(schemaHandle, "ModelName");

        // act
        SCHEMA_ACTION_HANDLE result1 = Schema_CreateModelAction(modelTypeHandle, "ActionName");
        SCHEMA_ACTION_HANDLE result2 = Schema_CreateModelAction(modelTypeHandle, "ActionName");

        // assert
        ASSERT_IS_NOT_NULL(result1);
        ASSERT_IS_NULL(result2);
        ASSERT_ARE_NOT_EQUAL(void_ptr, (void*)result1, (void*)result2);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests_SRS_SCHEMA_99_105: [The action name shall be unique per model, if the same action name is added twice to a model, Schema_CreateModelAction shall return NULL.] */
    TEST_FUNCTION(Schema_CreateModelAction_With_Same_Action_To_Different_Models_Succeeds)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle1 = Schema_CreateModelType(schemaHandle, "ModelName1");
        SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle2 = Schema_CreateModelType(schemaHandle, "ModelName2");

        // act
        SCHEMA_ACTION_HANDLE result1 = Schema_CreateModelAction(modelTypeHandle1, "ActionName");
        SCHEMA_ACTION_HANDLE result2 = Schema_CreateModelAction(modelTypeHandle2, "ActionName");

        // assert
        ASSERT_IS_NOT_NULL(result1);
        ASSERT_IS_NOT_NULL(result2);
        ASSERT_ARE_NOT_EQUAL(void_ptr, (void*)result1, (void*)result2);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Schema_AddModelActionArgument */
    /* Tests_SRS_SCHEMA_99_109: [If any of the arguments is NULL, Schema_AddModelActionArgument shall return SCHEMA_INVALID_ARG.] */
    TEST_FUNCTION(Schema_AddModelActionArgument_with_NULL_actionHandle_fails)
    {
        // arrange

        // act
        SCHEMA_RESULT result = Schema_AddModelActionArgument(NULL, "ArgumentName", "ArgumentType");

        // assert
        ASSERT_ARE_EQUAL(SCHEMA_RESULT, SCHEMA_INVALID_ARG, result);
    }

    /* Tests_SRS_SCHEMA_99_109: [If any of the arguments is NULL, Schema_AddModelActionArgument shall return SCHEMA_INVALID_ARG.] */
    TEST_FUNCTION(Schema_AddModelActionArgument_with_NULL_argumentName_fails)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle = Schema_CreateModelType(schemaHandle, "ModelName");
        SCHEMA_ACTION_HANDLE actionHandle = Schema_CreateModelAction(modelTypeHandle, "ActionName");

        // act
        SCHEMA_RESULT result = Schema_AddModelActionArgument(actionHandle, NULL, "ArgumentType");

        // assert
        ASSERT_ARE_EQUAL(SCHEMA_RESULT, SCHEMA_INVALID_ARG, result);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests_SRS_SCHEMA_99_109: [If any of the arguments is NULL, Schema_AddModelActionArgument shall return SCHEMA_INVALID_ARG.] */
    TEST_FUNCTION(Schema_AddModelActionArgument_with_NULL_argumentType_fails)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle = Schema_CreateModelType(schemaHandle, "ModelName");
        SCHEMA_ACTION_HANDLE actionHandle = Schema_CreateModelAction(modelTypeHandle, "ActionName");

        // act
        SCHEMA_RESULT result = Schema_AddModelActionArgument(actionHandle, "ArgumentName", NULL);

        // assert
        ASSERT_ARE_EQUAL(SCHEMA_RESULT, SCHEMA_INVALID_ARG, result);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests_SRS_SCHEMA_99_107: [Schema_AddModelActionArgument shall add one argument name & type to an action identified by actionHandle.] */
    /* Tests_SRS_SCHEMA_99_108: [On success, Schema_AddModelActionArgunent shall return SCHEMA_OK.] */
    TEST_FUNCTION(Schema_AddModelActionArgument_with_valid_arguments_Succeeds)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle = Schema_CreateModelType(schemaHandle, "ModelName");
        SCHEMA_ACTION_HANDLE actionHandle = Schema_CreateModelAction(modelTypeHandle, "ActionName");

        // act
        SCHEMA_RESULT result = Schema_AddModelActionArgument(actionHandle, "ArgumentName", "ArgumentType");

        // assert
        ASSERT_ARE_EQUAL(SCHEMA_RESULT, SCHEMA_OK, result);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests_SRS_SCHEMA_99_110: [The argument name shall be unique per action, if the same name is added twice to an action, SCHEMA_DUPLICATE_ELEMENT shall be returned.] */
    TEST_FUNCTION(Schema_AddModelActionArgument_when_adding_two_different_argument_names_Succeeds)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle = Schema_CreateModelType(schemaHandle, "ModelName");
        SCHEMA_ACTION_HANDLE actionHandle = Schema_CreateModelAction(modelTypeHandle, "ActionName");

        // act
        SCHEMA_RESULT result1 = Schema_AddModelActionArgument(actionHandle, "ArgumentName1", "ArgumentType1");
        SCHEMA_RESULT result2 = Schema_AddModelActionArgument(actionHandle, "ArgumentName2", "ArgumentType2");

        // assert
        ASSERT_ARE_EQUAL(SCHEMA_RESULT, SCHEMA_OK, result1);
        ASSERT_ARE_EQUAL(SCHEMA_RESULT, SCHEMA_OK, result2);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests_SRS_SCHEMA_99_110: [The argument name shall be unique per action, if the same name is added twice to an action, SCHEMA_DUPLICATE_ELEMENT shall be returned.] */
    TEST_FUNCTION(Schema_AddModelActionArgument_when_adding_same_argument_name_twice_Fails)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle = Schema_CreateModelType(schemaHandle, "ModelName");
        SCHEMA_ACTION_HANDLE actionHandle = Schema_CreateModelAction(modelTypeHandle, "ActionName");

        // act
        SCHEMA_RESULT result1 = Schema_AddModelActionArgument(actionHandle, "ArgumentName", "ArgumentType1");
        SCHEMA_RESULT result2 = Schema_AddModelActionArgument(actionHandle, "ArgumentName", "ArgumentType2");

        // assert
        ASSERT_ARE_EQUAL(SCHEMA_RESULT, SCHEMA_OK, result1);
        ASSERT_ARE_EQUAL(SCHEMA_RESULT, SCHEMA_DUPLICATE_ELEMENT, result2);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests_SRS_SCHEMA_99_111: [Schema_AddModelActionArgument shall accept arguments with different names of the same type.]  */
    TEST_FUNCTION(Schema_AddModelActionArgument_when_adding_two_different_argument_names_with_same_type_Succeeds)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle = Schema_CreateModelType(schemaHandle, "ModelName");
        SCHEMA_ACTION_HANDLE actionHandle = Schema_CreateModelAction(modelTypeHandle, "ActionName");

        // act
        SCHEMA_RESULT result1 = Schema_AddModelActionArgument(actionHandle, "ArgumentName1", "ArgumentType");
        SCHEMA_RESULT result2 = Schema_AddModelActionArgument(actionHandle, "ArgumentName2", "ArgumentType");

        // assert
        ASSERT_ARE_EQUAL(SCHEMA_RESULT, SCHEMA_OK, result1);
        ASSERT_ARE_EQUAL(SCHEMA_RESULT, SCHEMA_OK, result2);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Schema_GetModelCount */

    /* Tests_SRS_SCHEMA_99_123: [Schema_GetModelCount shall return SCHEMA_INVALID_ARG if any of the arguments is NULL.] */
    TEST_FUNCTION(Schema_GetModelCount_with_NULL_schemaHandle_Fails)
    {
        // arrange
        size_t modelCount;

        // act
        SCHEMA_RESULT result = Schema_GetModelCount(NULL, &modelCount);

        // assert
        ASSERT_ARE_EQUAL(SCHEMA_RESULT, SCHEMA_INVALID_ARG, result);
    }
    
    /* Tests_SRS_SCHEMA_99_123: [Schema_GetModelCount shall return SCHEMA_INVALID_ARG if any of the arguments is NULL.] */
    TEST_FUNCTION(Schema_GetModelCount_with_NULL_modelCount_Fails)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        (void)Schema_CreateModelType(schemaHandle, "ModelName1");
        (void)Schema_CreateModelType(schemaHandle, "ModelName2");

        // act
        SCHEMA_RESULT result = Schema_GetModelCount(schemaHandle, NULL);

        // assert
        ASSERT_ARE_EQUAL(SCHEMA_RESULT, SCHEMA_INVALID_ARG, result);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests_SRS_SCHEMA_99_120: [Schema_GetModelCount shall provide the number of models defined in the schema identified by schemaHandle.] */
    /* Tests_SRS_SCHEMA_99_121: [The count shall be provided via the out argument modelCount.] */ 
    /* Tests_SRS_SCHEMA_99_122: [On success, Schema_GetModelCount shall return SCHEMA_OK.] */
    TEST_FUNCTION(Schema_GetModelCount_With_Zero_Models_Defined_Yields_0)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        size_t modelCount = 42;

        // act
        SCHEMA_RESULT result = Schema_GetModelCount(schemaHandle, &modelCount);

        // assert
        ASSERT_ARE_EQUAL(SCHEMA_RESULT, SCHEMA_OK, result);
        ASSERT_ARE_EQUAL(size_t, (size_t)0, modelCount);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests_SRS_SCHEMA_99_120: [Schema_GetModelCount shall provide the number of models defined in the schema identified by schemaHandle.] */
    /* Tests_SRS_SCHEMA_99_121: [The count shall be provided via the out argument modelCount.] */
    /* Tests_SRS_SCHEMA_99_122: [On success, Schema_GetModelCount shall return SCHEMA_OK.] */
    TEST_FUNCTION(Schema_GetModelCount_With_Two_Models_Defined_Yields_2)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        (void)Schema_CreateModelType(schemaHandle, "ModelName1");
        (void)Schema_CreateModelType(schemaHandle, "ModelName2");
        size_t modelCount = 42;

        // act
        SCHEMA_RESULT result = Schema_GetModelCount(schemaHandle, &modelCount);

        // assert
        ASSERT_ARE_EQUAL(SCHEMA_RESULT, SCHEMA_OK, result);
        ASSERT_ARE_EQUAL(size_t, (size_t)2, modelCount);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests_SRS_SCHEMA_99_120: [Schema_GetModelCount shall provide the number of models defined in the schema identified by schemaHandle.] */
    /* Tests_SRS_SCHEMA_99_121: [The count shall be provided via the out argument modelCount.] */
    /* Tests_SRS_SCHEMA_99_122: [On success, Schema_GetModelCount shall return SCHEMA_OK.] */
    TEST_FUNCTION(Schema_GetModelCount_With_Two_Models_Added_with_the_same_modelName_Yields_1)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        (void)Schema_CreateModelType(schemaHandle, "ModelName");
        (void)Schema_CreateModelType(schemaHandle, "ModelName");
        size_t modelCount = 42;

        // act
        SCHEMA_RESULT result = Schema_GetModelCount(schemaHandle, &modelCount);

        // assert
        ASSERT_ARE_EQUAL(SCHEMA_RESULT, SCHEMA_OK, result);
        ASSERT_ARE_EQUAL(size_t, (size_t)1, modelCount);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Schema_GetModelByName */

    /* Tests_SRS_SCHEMA_99_125: [Schema_GetModelByName shall return NULL if unable to find a matching model, or if any of the arguments are NULL.] */
    TEST_FUNCTION(Schema_GetModelByName_with_NULL_schemaHandle_Fails)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        (void)Schema_CreateModelType(schemaHandle, MODEL_NAME);

        // act
        SCHEMA_MODEL_TYPE_HANDLE result = Schema_GetModelByName(NULL, MODEL_NAME);

        // assert
        ASSERT_IS_NULL(result);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests_SRS_SCHEMA_99_125: [Schema_GetModelByName shall return NULL if unable to find a matching model, or if any of the arguments are NULL.] */
    TEST_FUNCTION(Schema_GetModelByName_with_NULL_modelName_Fails)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        (void)Schema_CreateModelType(schemaHandle, MODEL_NAME);

        // act
        SCHEMA_MODEL_TYPE_HANDLE result = Schema_GetModelByName(schemaHandle, NULL);

        // assert
        ASSERT_IS_NULL(result);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests_SRS_SCHEMA_99_125: [Schema_GetModelByName shall return NULL if unable to find a matching model, or if any of the arguments are NULL.] */
    TEST_FUNCTION(Schema_GetModelByName_With_Zero_Models_Defined_Fails)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);

        // act
        SCHEMA_MODEL_TYPE_HANDLE result = Schema_GetModelByName(schemaHandle, MODEL_NAME);

        // assert
        ASSERT_IS_NULL(result);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests_SRS_SCHEMA_99_125: [Schema_GetModelByName shall return NULL if unable to find a matching model, or if any of the arguments are NULL.] */
    TEST_FUNCTION(Schema_GetModelByName_With_3_Models_Defined_And_modelName_Not_Matching_Fails)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        (void)Schema_CreateModelType(schemaHandle, "ModelName1");
        (void)Schema_CreateModelType(schemaHandle, "ModelName2");
        (void)Schema_CreateModelType(schemaHandle, "ModelName3");

        // act
        SCHEMA_MODEL_TYPE_HANDLE result = Schema_GetModelByName(schemaHandle, "ModelName");

        // assert
        ASSERT_IS_NULL(result);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests_SRS_SCHEMA_99_124: [Schema_GetModelByName shall return a non-NULL SCHEMA_MODEL_TYPE_HANDLE corresponding to the model identified by schemaHandle and matching the modelName argument value.] */
    TEST_FUNCTION(Schema_GetModelByName_With_Valid_Arguments_And_modelName_Matching_Succeeds)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        (void)Schema_CreateModelType(schemaHandle, "ModelName1");
        (void)Schema_CreateModelType(schemaHandle, "ModelName2");
        (void)Schema_CreateModelType(schemaHandle, "ModelName3");

        // act
        SCHEMA_MODEL_TYPE_HANDLE result = Schema_GetModelByName(schemaHandle, "ModelName2");

        // assert
        ASSERT_IS_NOT_NULL(result);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Schema_GetModelByIndex */

    /* Tests_SRS_SCHEMA_99_128: [Schema_GetModelByIndex shall return NULL if the index specified is outside the valid range or if schemaHandle argument is NULL.] */
    TEST_FUNCTION(Schema_GetModelByIndex_With_NULL_schemaHandle_Fails)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        (void)Schema_CreateModelType(schemaHandle, MODEL_NAME);

        // act
        SCHEMA_MODEL_TYPE_HANDLE result = Schema_GetModelByIndex(NULL, 0);

        // assert
        ASSERT_IS_NULL(result);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests_SRS_SCHEMA_99_128: [Schema_GetModelByIndex shall return NULL if the index specified is outside the valid range or if schemaHandle argument is NULL.] */
    TEST_FUNCTION(Schema_GetModelByIndex_With_No_Models_Defined_schemaHandle_Fails)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);

        // act
        SCHEMA_MODEL_TYPE_HANDLE result = Schema_GetModelByIndex(schemaHandle, 0);

        // assert
        ASSERT_IS_NULL(result);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests_SRS_SCHEMA_99_128: [Schema_GetModelByIndex shall return NULL if the index specified is outside the valid range or if schemaHandle argument is NULL.] */
    /* Tests_SRS_SCHEMA_99_127: [The index argument is zero based, and the order in which models were added shall be the index in which they will be retrieved.] */
    TEST_FUNCTION(Schema_GetModelByIndex_With_Index_Equals_The_Number_Of_Defined_Models_Fails)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        (void)Schema_CreateModelType(schemaHandle, "ModelName1");
        (void)Schema_CreateModelType(schemaHandle, "ModelName2");
        (void)Schema_CreateModelType(schemaHandle, "ModelName3");

        // act
        SCHEMA_MODEL_TYPE_HANDLE result = Schema_GetModelByIndex(schemaHandle, 3);

        // assert
        ASSERT_IS_NULL(result);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests_SRS_SCHEMA_99_128: [Schema_GetModelByIndex shall return NULL if the index specified is outside the valid range or if schemaHandle argument is NULL.] */
    /* Tests_SRS_SCHEMA_99_127: [The index argument is zero based, and the order in which models were added shall be the index in which they will be retrieved.] */
    TEST_FUNCTION(Schema_GetModelByIndex_With_Index_Greater_Than_The_Number_Of_Defined_Models_Fails)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        (void)Schema_CreateModelType(schemaHandle, "ModelName1");
        (void)Schema_CreateModelType(schemaHandle, "ModelName2");
        (void)Schema_CreateModelType(schemaHandle, "ModelName3");

        // act
        SCHEMA_MODEL_TYPE_HANDLE result = Schema_GetModelByIndex(schemaHandle, 4);

        // assert
        ASSERT_IS_NULL(result);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests_SRS_SCHEMA_99_126: [Schema_GetModelByIndex shall return a non-NULL SCHEMA_MODEL_TYPE_HANDLE corresponding to the model identified by schemaHandle and matching the index number provided by the index argument.] */
    /* Tests_SRS_SCHEMA_99_127: [The index argument is zero based, and the order in which models were added shall be the index in which they will be retrieved.] */
    TEST_FUNCTION(Schema_GetModelByIndex_With_Index_Within_The_Number_Of_Defined_Models_Succeeds)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        (void)Schema_CreateModelType(schemaHandle, "ModelName1");
        (void)Schema_CreateModelType(schemaHandle, "ModelName2");
        (void)Schema_CreateModelType(schemaHandle, "ModelName3");

        // act
        SCHEMA_MODEL_TYPE_HANDLE result = Schema_GetModelByIndex(schemaHandle, 2);

        // assert
        ASSERT_IS_NOT_NULL(result);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Schema_GetModelPropertyByName */

    /* Tests_SRS_SCHEMA_99_038:[Schema_GetModelPropertyByName shall return NULL if unable to find a matching property or if any of the arguments are NULL.] */
    TEST_FUNCTION(Schema_GetModelPropertyByName_With_A_NULL_ModelHandle_Fails)
    {
        // arrange

        // act
        SCHEMA_PROPERTY_HANDLE result = Schema_GetModelPropertyByName(NULL, "Name");

        // assert
        ASSERT_IS_NULL(result);
    }

    /* Tests_SRS_SCHEMA_99_038:[Schema_GetModelPropertyByName shall return NULL if unable to find a matching property or if any of the arguments are NULL.] */
    TEST_FUNCTION(Schema_GetModelPropertyByName_With_A_NULL_Name_Fails)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_MODEL_TYPE_HANDLE modelType = Schema_CreateModelType(schemaHandle, "Model");
        (void)Schema_AddModelProperty(modelType, "MyName", "SomeType");

        // act
        SCHEMA_PROPERTY_HANDLE result = Schema_GetModelPropertyByName(modelType, NULL);

        // assert
        ASSERT_IS_NULL(result);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests_SRS_SCHEMA_99_038:[Schema_GetModelPropertyByName shall return NULL if unable to find a matching property or if any of the arguments are NULL.] */
    TEST_FUNCTION(Schema_GetModelPropertyByName_When_The_Property_Is_Not_Found_Fails)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_MODEL_TYPE_HANDLE modelType = Schema_CreateModelType(schemaHandle, "Model");
        (void)Schema_AddModelProperty(modelType, "MyName", "SomeType");

        // act
        SCHEMA_PROPERTY_HANDLE result = Schema_GetModelPropertyByName(modelType, "AnotherName");

        // assert
        ASSERT_IS_NULL(result);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests_SRS_SCHEMA_99_038:[Schema_GetModelPropertyByName shall return NULL if unable to find a matching property or if any of the arguments are NULL.] */
    TEST_FUNCTION(Schema_GetModelPropertyByName_When_Model_Is_Empty_Fails)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_MODEL_TYPE_HANDLE modelType = Schema_CreateModelType(schemaHandle, "Model");

        // act
        SCHEMA_PROPERTY_HANDLE result = Schema_GetModelPropertyByName(modelType, "PropertyName");

        // assert
        ASSERT_IS_NULL(result);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests_SRS_SCHEMA_99_036:[Schema_GetModelPropertyByName shall return a non-NULL SCHEMA_PROPERTY_HANDLE corresponding to the model type identified by modelTypeHandle and matching the propertyName argument value.] */
    TEST_FUNCTION(Schema_GetModelPropertyByName_When_The_Property_Is_Found_Returns_A_Property_Handle)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_MODEL_TYPE_HANDLE modelType = Schema_CreateModelType(schemaHandle, "Model");
        (void)Schema_AddModelProperty(modelType, "MyName", "SomeType");

        // act
        SCHEMA_PROPERTY_HANDLE result = Schema_GetModelPropertyByName(modelType, "MyName");

        // assert
        ASSERT_IS_NOT_NULL(result);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Schema_GetModelPropertyCount */
    /* Tests_SRS_SCHEMA_99_092: [Schema_GetModelPropertyCount shall return SCHEMA_INVALID_ARG if any of the arguments is NULL.] */
    TEST_FUNCTION(Schema_GetModelPropertyCount_With_NULL_modelTypeHandle_Fails)
    {
        // arrange
        size_t propertyCount = 42;

        // act
        SCHEMA_RESULT result = Schema_GetModelPropertyCount(NULL, &propertyCount);

        // assert
        ASSERT_ARE_EQUAL(SCHEMA_RESULT, SCHEMA_INVALID_ARG, result);
    }

    /* Tests_SRS_SCHEMA_99_092: [Schema_GetModelPropertyCount shall return SCHEMA_INVALID_ARG if any of the arguments is NULL.] */
    TEST_FUNCTION(Schema_GetModelPropertyCount_With_NULL_propertyCount_Fails)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create("NameSpace");
        SCHEMA_MODEL_TYPE_HANDLE schemaModelTypeHandle = Schema_CreateModelType(schemaHandle, "ModelName");

        // act
        SCHEMA_RESULT result = Schema_GetModelPropertyCount(schemaModelTypeHandle, NULL);

        // assert
        ASSERT_ARE_EQUAL(SCHEMA_RESULT, SCHEMA_INVALID_ARG, result);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests_SRS_SCHEMA_99_092: [Schema_GetModelPropertyCount shall return SCHEMA_INVALID_ARG if any of the arguments is NULL.] */
    TEST_FUNCTION(Schema_GetModelPropertyCount_With_Both_NULL_Arguments_Fails)
    {
        // arrange

        // act
        SCHEMA_RESULT result = Schema_GetModelPropertyCount(NULL, NULL);

        // assert
        ASSERT_ARE_EQUAL(SCHEMA_RESULT, SCHEMA_INVALID_ARG, result);

    }

    /* Tests_SRS_SCHEMA_99_089: [Schema_GetModelPropertyCount shall provide the number of properties defined in the model type identified by modelTypeHandle.] */ 
    /* Tests_SRS_SCHEMA_99_090: [The count shall be provided via the out argument propertyCount.]*/ 
    /* Tests_SRS_SCHEMA_99_091: [On success, Schema_GetModelPropertyCount shall return SCHEMA_OK.] */
    TEST_FUNCTION(Schema_GetModelPropertyCount_With_No_Properties_Defined_Yields_0)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create("NameSpace");
        SCHEMA_MODEL_TYPE_HANDLE schemaModelTypeHandle = Schema_CreateModelType(schemaHandle, "ModelName");
        size_t propertyCount = 42;

        // act
        SCHEMA_RESULT result = Schema_GetModelPropertyCount(schemaModelTypeHandle, &propertyCount);

        // assert
        ASSERT_ARE_EQUAL(SCHEMA_RESULT, SCHEMA_OK, result);
        ASSERT_ARE_EQUAL(size_t, (size_t)0, propertyCount);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests_SRS_SCHEMA_99_089: [Schema_GetModelPropertyCount shall provide the number of properties defined in the model type identified by modelTypeHandle.] */
    /* Tests_SRS_SCHEMA_99_090: [The count shall be provided via the out argument propertyCount.]*/
    /* Tests_SRS_SCHEMA_99_091: [On success, Schema_GetModelPropertyCount shall return SCHEMA_OK.] */
    TEST_FUNCTION(Schema_GetModelPropertyCount_With_One_Property_Defined_Yields_1)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create("NameSpace");
        SCHEMA_MODEL_TYPE_HANDLE schemaModelTypeHandle = Schema_CreateModelType(schemaHandle, "ModelName");
        (void)Schema_AddModelProperty(schemaModelTypeHandle, "PropertyName1", "PropertyType1");
        size_t propertyCount = 42;

        // act
        SCHEMA_RESULT result = Schema_GetModelPropertyCount(schemaModelTypeHandle, &propertyCount);

        // assert
        ASSERT_ARE_EQUAL(SCHEMA_RESULT, SCHEMA_OK, result);
        ASSERT_ARE_EQUAL(size_t, (size_t)1, propertyCount);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests_SRS_SCHEMA_99_089: [Schema_GetModelPropertyCount shall provide the number of properties defined in the model type identified by modelTypeHandle.] */
    /* Tests_SRS_SCHEMA_99_090: [The count shall be provided via the out argument propertyCount.]*/
    /* Tests_SRS_SCHEMA_99_091: [On success, Schema_GetModelPropertyCount shall return SCHEMA_OK.] */
    TEST_FUNCTION(Schema_GetModelPropertyCount_With_Two_Properties_Defined_Yields_2)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create("NameSpace");
        SCHEMA_MODEL_TYPE_HANDLE schemaModelTypeHandle = Schema_CreateModelType(schemaHandle, "ModelName");
        (void)Schema_AddModelProperty(schemaModelTypeHandle, "PropertyName1", "PropertyType1");
        (void)Schema_AddModelProperty(schemaModelTypeHandle, "PropertyName2", "PropertyType2");
        size_t propertyCount = 42;

        // act
        SCHEMA_RESULT result = Schema_GetModelPropertyCount(schemaModelTypeHandle, &propertyCount);

        // assert
        ASSERT_ARE_EQUAL(SCHEMA_RESULT, SCHEMA_OK, result);
        ASSERT_ARE_EQUAL(size_t, (size_t)2, propertyCount);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Schema_GetModelProperty */

    /* Tests_SRS_SCHEMA_99_094: [Schema_GetModelProperty shall return NULL if the index specified is outside the valid range or if modelTypeHandle argument is NULL.] */
    TEST_FUNCTION(Schema_GetModelProperty_with_NULL_modelTypeHandle_Fails)
    {
        // arrange

        // act
        SCHEMA_PROPERTY_HANDLE result = Schema_GetModelPropertyByIndex(NULL, 0);

        // assert
        ASSERT_IS_NULL(result);
    }

    /* Tests_SRS_SCHEMA_99_094: [Schema_GetModelProperty shall return NULL if the index specified is outside the valid range or if modelTypeHandle argument is NULL.] */
    TEST_FUNCTION(Schema_GetModelProperty_With_A_Zero_Index_But_No_Properties_Fails)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle = Schema_CreateModelType(schemaHandle, "Model");

        // act
        SCHEMA_ACTION_HANDLE result = Schema_GetModelPropertyByIndex(modelTypeHandle, 0);

        // assert
        ASSERT_IS_NULL(result);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests_SRS_SCHEMA_99_094: [Schema_GetModelProperty shall return NULL if the index specified is outside the valid range or if modelTypeHandle argument is NULL.] */
    TEST_FUNCTION(Schema_GetModelPropety_With_Index_Out_Of_Range_Fails)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle = Schema_CreateModelType(schemaHandle, "Model");
        (void)Schema_AddModelProperty(modelTypeHandle, "PropertyName", "PropertyType");

        // act
        SCHEMA_ACTION_HANDLE result = Schema_GetModelPropertyByIndex(modelTypeHandle, 1);

        // assert
        ASSERT_IS_NULL(result);

        // cleanup
        Schema_Destroy(schemaHandle);
    }
    
    /* Tests_SRS_SCHEMA_99_093: [Schema_GetModelProperty shall return a non-NULL SCHEMA_PROPERTY_HANDLE corresponding to the model type identified by modelTypeHandle and matching the index number provided by the index argument.] */
    /* Tests_SRS_SCHEMA_99_097: [index is zero based, and the order in which actions were added shall be the index in which they will be retrieved.] */
    TEST_FUNCTION(Schema_GetModelProperty_With_Valid_Index_Succeeds)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle = Schema_CreateModelType(schemaHandle, "Model");
        (void)Schema_AddModelProperty(modelTypeHandle, "PropertyName", "PropertyType");

        // act
        SCHEMA_ACTION_HANDLE result = Schema_GetModelPropertyByIndex(modelTypeHandle, 0);

        // assert
        ASSERT_IS_NOT_NULL(result);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests_SRS_SCHEMA_99_093: [Schema_GetModelProperty shall return a non-NULL SCHEMA_PROPERTY_HANDLE corresponding to the model type identified by modelTypeHandle and matching the index number provided by the index argument.] */
    /* Tests_SRS_SCHEMA_99_097: [index is zero based, and the order in which actions were added shall be the index in which they will be retrieved.] */
    TEST_FUNCTION(Schema_GetModelProperty_Returns_Different_Handles_For_Different_Properties)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle = Schema_CreateModelType(schemaHandle, "Model");
        (void)Schema_AddModelProperty(modelTypeHandle, "PropertyName1", "PropertyType1");
        (void)Schema_AddModelProperty(modelTypeHandle, "PropertyName2", "PropertyType2");

        // act
        SCHEMA_ACTION_HANDLE result1 = Schema_GetModelPropertyByIndex(modelTypeHandle, 0);
        SCHEMA_ACTION_HANDLE result2 = Schema_GetModelPropertyByIndex(modelTypeHandle, 1);

        // assert
        ASSERT_IS_NOT_NULL(result1);
        ASSERT_IS_NOT_NULL(result2);
        ASSERT_ARE_NOT_EQUAL(void_ptr, (void*)result1, (void*)result2);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Schema_GetModelActionByName */

    /* Tests_SRS_SCHEMA_99_041:[Schema_GetModelActionByName shall return NULL if unable to find a matching action, if any of the arguments are NULL.] */
    TEST_FUNCTION(Schema_GetModelActionByName_With_A_NULL_ModelHandle_Fails)
    {
        // arrange

        // act
        SCHEMA_PROPERTY_HANDLE result = Schema_GetModelActionByName(NULL, "Name");

        // assert
        ASSERT_IS_NULL(result);
    }

    /* Tests_SRS_SCHEMA_99_041:[Schema_GetModelActionByName shall return NULL if unable to find a matching action, if any of the arguments are NULL.] */
    TEST_FUNCTION(Schema_GetModelActionByName_With_A_NULL_Name_Fails)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_MODEL_TYPE_HANDLE modelType = Schema_CreateModelType(schemaHandle, "Model");

        // act
        SCHEMA_PROPERTY_HANDLE result = Schema_GetModelActionByName(modelType, NULL);

        // assert
        ASSERT_IS_NULL(result);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests_SRS_SCHEMA_99_041:[Schema_GetModelActionByName shall return NULL if unable to find a matching action, if any of the arguments are NULL.] */
    TEST_FUNCTION(Schema_GetModelActionByName_When_The_Action_Is_Not_Found_Fails)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_MODEL_TYPE_HANDLE modelType = Schema_CreateModelType(schemaHandle, "Model");
        (void)Schema_CreateModelAction(modelType, "ActionName");

        // act
        SCHEMA_PROPERTY_HANDLE result = Schema_GetModelActionByName(modelType, "AnotherName");

        // assert
        ASSERT_IS_NULL(result);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests_SRS_SCHEMA_99_040:[Schema_GetModelActionByName shall return a non-NULL SCHEMA_ACTION_HANDLE corresponding to the model type identified by modelTypeHandle and matching the actionName argument value.] */
    TEST_FUNCTION(Schema_GetModelActionByName_When_The_Property_Is_Found_Returns_A_Property_Handle)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_MODEL_TYPE_HANDLE modelType = Schema_CreateModelType(schemaHandle, "Model");
        (void)Schema_CreateModelAction(modelType, "ActionName");

        // act
        SCHEMA_PROPERTY_HANDLE result = Schema_GetModelActionByName(modelType, "ActionName");

        // assert
        ASSERT_IS_NOT_NULL(result);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Schema_GetModelActionCount */

    /* Tests_SRS_SCHEMA_99_045:[If any of the modelTypeHandle or actionCount arguments is NULL, Schema_GetModelActionCount shall return SCHEMA_INVALID_ARG.] */
    TEST_FUNCTION(Schema_GetModelActionCount_With_A_NULL_ModelType_Handle_Fails)
    {
        // arrange
        size_t actionCount = 42;

        // act
        SCHEMA_RESULT result = Schema_GetModelActionCount(NULL, &actionCount);

        // assert
        ASSERT_ARE_EQUAL(SCHEMA_RESULT, SCHEMA_INVALID_ARG, result);
    }

    /* Tests_SRS_SCHEMA_99_045:[If any of the modelTypeHandle or actionCount arguments is NULL, Schema_GetModelActionCount shall return SCHEMA_INVALID_ARG.] */
    TEST_FUNCTION(Schema_GetModelActionCount_With_A_NULL_ActionCount_Fails)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_MODEL_TYPE_HANDLE modelType = Schema_CreateModelType(schemaHandle, "Model");

        // act
        SCHEMA_RESULT result = Schema_GetModelActionCount(modelType, NULL);

        // assert
        ASSERT_ARE_EQUAL(SCHEMA_RESULT, SCHEMA_INVALID_ARG, result);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests_SRS_SCHEMA_99_042:[Schema_GetModelActionCount shall provide the total number of actions defined in a model type identified by the modelTypeHandle.] */
    /* Tests_SRS_SCHEMA_99_044:[On success, Schema_GetModelActionCount shall return SCHEMA_OK.] */
    /* Tests_SRS_SCHEMA_99_043:[The count shall be provided via the out argument actionCount.] */
    TEST_FUNCTION(Schema_GetModelActionCount_When_No_Actions_Are_Defined_Yields_0)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_MODEL_TYPE_HANDLE modelType = Schema_CreateModelType(schemaHandle, "Model");
        size_t actionCount = 42;

        // act
        SCHEMA_RESULT result = Schema_GetModelActionCount(modelType, &actionCount);

        // assert
        ASSERT_ARE_EQUAL(SCHEMA_RESULT, SCHEMA_OK, result);
        ASSERT_ARE_EQUAL(size_t, (size_t)0, actionCount);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests_SRS_SCHEMA_99_042:[Schema_GetModelActionCount shall provide the total number of actions defined in a model type identified by the modelTypeHandle.] */
    /* Tests_SRS_SCHEMA_99_044:[On success, Schema_GetModelActionCount shall return SCHEMA_OK.] */
    /* Tests_SRS_SCHEMA_99_043:[The count shall be provided via the out argument actionCount.] */
    TEST_FUNCTION(Schema_GetModelActionCount_When_1_Action_Is_Defined_Yields_1)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_MODEL_TYPE_HANDLE modelType = Schema_CreateModelType(schemaHandle, "Model");
        (void)Schema_CreateModelAction(modelType, "MyName");
        size_t actionCount = 42;

        // act
        SCHEMA_RESULT result = Schema_GetModelActionCount(modelType, &actionCount);

        // assert
        ASSERT_ARE_EQUAL(SCHEMA_RESULT, SCHEMA_OK, result);
        ASSERT_ARE_EQUAL(size_t, (size_t)1, actionCount);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests_SRS_SCHEMA_99_042:[Schema_GetModelActionCount shall provide the total number of actions defined in a model type identified by the modelTypeHandle.] */
    /* Tests_SRS_SCHEMA_99_044:[On success, Schema_GetModelActionCount shall return SCHEMA_OK.] */
    /* Tests_SRS_SCHEMA_99_043:[The count shall be provided via the out argument actionCount.] */
    TEST_FUNCTION(Schema_GetModelActionCount_When_2_Action_Is_Defined_Yields_2)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_MODEL_TYPE_HANDLE modelType = Schema_CreateModelType(schemaHandle, "Model");
        (void)Schema_CreateModelAction(modelType, "MyName");
        (void)Schema_CreateModelAction(modelType, "MyName2");
        size_t actionCount = 42;

        // act
        SCHEMA_RESULT result = Schema_GetModelActionCount(modelType, &actionCount);

        // assert
        ASSERT_ARE_EQUAL(SCHEMA_RESULT, SCHEMA_OK, result);
        ASSERT_ARE_EQUAL(size_t, (size_t)2, actionCount);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Schema_GetModelAction */

    /* Tests_SRS_SCHEMA_99_048:[Schema_GetModelAction shall return NULL if the index specified is outside the valid range or if modelTypeHandle argument is NULL.] */
    TEST_FUNCTION(Schema_GetModelAction_With_A_NULL_ModelType_Handle_Fails)
    {
        // arrange

        // act
        SCHEMA_ACTION_HANDLE result = Schema_GetModelActionByIndex(NULL, 0);

        // assert
        ASSERT_IS_NULL(result);
    }

    /* Tests_SRS_SCHEMA_99_048:[Schema_GetModelAction shall return NULL if the index specified is outside the valid range or if modelTypeHandle argument is  NULL.] */
    TEST_FUNCTION(Schema_GetModelAction_With_A_Zero_Index_But_No_Actions_Fails)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_MODEL_TYPE_HANDLE modelType = Schema_CreateModelType(schemaHandle, "Model");

        // act
        SCHEMA_ACTION_HANDLE result = Schema_GetModelActionByIndex(modelType, 0);

        // assert
        ASSERT_IS_NULL(result);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests_SRS_SCHEMA_99_048:[Schema_GetModelAction shall return NULL if the index specified is outside the valid range or if modelTypeHandle argument is NULL.] */
    TEST_FUNCTION(Schema_GetModelAction_With_Index_Out_Of_Range_Fails)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_MODEL_TYPE_HANDLE modelType = Schema_CreateModelType(schemaHandle, "Model");
        (void)Schema_CreateModelAction(modelType, "MyName");

        // act
        SCHEMA_ACTION_HANDLE result = Schema_GetModelActionByIndex(modelType, 1);

        // assert
        ASSERT_IS_NULL(result);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests_SRS_SCHEMA_99_047:[Schema_GetModelAction shall return a non-NULL SCHEMA_ACTION_HANDLE corresponding to the model type identified by modelTypeHandle and matching the index number provided by the index argument.] */
    /* Tests_SRS_SCHEMA_99_096: [index is zero based and the order in which actions were added shall be the index in which they will be retrieved.] */
    TEST_FUNCTION(Schema_GetModelAction_With_Valid_Index_Succeeds)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_MODEL_TYPE_HANDLE modelType = Schema_CreateModelType(schemaHandle, "Model");
        (void)Schema_CreateModelAction(modelType, "MyName");

        // act
        SCHEMA_ACTION_HANDLE result = Schema_GetModelActionByIndex(modelType, 0);

        // assert
        ASSERT_IS_NOT_NULL(result);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests_SRS_SCHEMA_99_047:[Schema_GetModelAction shall return a non-NULL SCHEMA_ACTION_HANDLE corresponding to the model type identified by modelTypeHandle and matching the index number provided by the index argument.] */
    /* Tests_SRS_SCHEMA_99_096: [index is zero based and the order in which actions were added shall be the index in which they will be retrieved.] */
    TEST_FUNCTION(Schema_GetModelAction_Returns_Different_Handles_For_Different_Actions)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_MODEL_TYPE_HANDLE modelType = Schema_CreateModelType(schemaHandle, "Model");
        (void)Schema_CreateModelAction(modelType, "MyName");
        (void)Schema_CreateModelAction(modelType, "MyName2");

        // act
        SCHEMA_ACTION_HANDLE result1 = Schema_GetModelActionByIndex(modelType, 0);
        SCHEMA_ACTION_HANDLE result2 = Schema_GetModelActionByIndex(modelType, 1);

        // assert
        ASSERT_IS_NOT_NULL(result1);
        ASSERT_IS_NOT_NULL(result2);
        ASSERT_ARE_NOT_EQUAL(void_ptr, (void*)result1, (void*)result2);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Schema_GetModelActionName */

    /* Tests_SRS_SCHEMA_99_050:[If the actionHandle is NULL, Schema_GetModelActionName shall return NULL.] */
    TEST_FUNCTION(Schema_GetModelActionName_With_NULL_Handle_Fails)
    {
        // arrange

        // act
        const char* result = Schema_GetModelActionName(NULL);

        // assert
        ASSERT_IS_NULL(result);
    }

    /* Tests_SRS_SCHEMA_99_050:[If the actionHandle is NULL, Schema_GetModelActionName shall return NULL.] */
    TEST_FUNCTION(Schema_GetModelActionName_With_A_Valid_Action_Handle_Returns_The_Action_Name)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_MODEL_TYPE_HANDLE modelType = Schema_CreateModelType(schemaHandle, "Model");
        (void)Schema_CreateModelAction(modelType, "MyName");

        // act
        SCHEMA_ACTION_HANDLE actionHandle = Schema_GetModelActionByIndex(modelType, 0);
        const char* result = Schema_GetModelActionName(actionHandle);

        // assert
        ASSERT_ARE_EQUAL(char_ptr, "MyName", result);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests_SRS_SCHEMA_99_050:[If the actionHandle is NULL, Schema_GetModelActionName shall return NULL.] */
    TEST_FUNCTION(Schema_GetModelActionName_With_A_Valid_Action_Handle_Returns_The_Action_Name_2)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_MODEL_TYPE_HANDLE modelType = Schema_CreateModelType(schemaHandle, "Model");
        (void)Schema_CreateModelAction(modelType, "MyName");
        (void)Schema_CreateModelAction(modelType, "MyName2");

        // act
        SCHEMA_ACTION_HANDLE actionHandle = Schema_GetModelActionByIndex(modelType, 1);
        const char* result = Schema_GetModelActionName(actionHandle);

        // assert
        ASSERT_ARE_EQUAL(char_ptr, "MyName2", result);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Schema_GetModelActionArgumentCount */

    /* Tests_SRS_SCHEMA_99_054:[If any argument is NULL, Schema_GetModelActionArgumentCount shall return SCHEMA_INVALID_ARG.] */
    TEST_FUNCTION(Schema_GetModelActionArgumentCount_With_A_NULL_Action_Handle_Fails)
    {
        // arrange
        size_t argumentCount = 42;

        // act
        SCHEMA_RESULT result = Schema_GetModelActionArgumentCount(NULL, &argumentCount);

        // assert
        ASSERT_ARE_EQUAL(SCHEMA_RESULT, SCHEMA_INVALID_ARG, result);
    }

    /* Tests_SRS_SCHEMA_99_054:[If any argument is NULL, Schema_GetModelActionArgumentCount shall return SCHEMA_INVALID_ARG.] */
    TEST_FUNCTION(Schema_GetModelActionArgumentCount_With_A_NULL_ArgumentCount_Fails)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_MODEL_TYPE_HANDLE modelType = Schema_CreateModelType(schemaHandle, "Model");
        (void)Schema_CreateModelAction(modelType, "MyName");
        SCHEMA_ACTION_HANDLE actionHandle = Schema_GetModelActionByIndex(modelType, 0);

        // act
        SCHEMA_RESULT result = Schema_GetModelActionArgumentCount(actionHandle, NULL);

        // assert
        ASSERT_ARE_EQUAL(SCHEMA_RESULT, SCHEMA_INVALID_ARG, result);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests_SRS_SCHEMA_99_051:[Schema_GetModelActionArgumentCount shall provide the number of arguments for a specific schema action identified by actionHandle.] */
    /* Tests_SRS_SCHEMA_99_052:[The argument count shall be provided via the out argument argumentCount.] */
    /* Tests_SRS_SCHEMA_99_053:[On success, Schema_GetModelActionArgumentCount shall return SCHEMA_OK.] */
    TEST_FUNCTION(Schema_GetModelActionArgumentCount_With_A_Valid_Action_Handle_And_No_Action_Args_Yields_0)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_MODEL_TYPE_HANDLE modelType = Schema_CreateModelType(schemaHandle, "Model");
        (void)Schema_CreateModelAction(modelType, "MyName");
        SCHEMA_ACTION_HANDLE actionHandle = Schema_GetModelActionByIndex(modelType, 0);
        size_t argumentCount = 42;

        // act
        SCHEMA_RESULT result = Schema_GetModelActionArgumentCount(actionHandle, &argumentCount);

        // assert
        ASSERT_ARE_EQUAL(SCHEMA_RESULT, SCHEMA_OK, result);
        ASSERT_ARE_EQUAL(size_t, (size_t)0, argumentCount);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests_SRS_SCHEMA_99_051:[Schema_GetModelActionArgumentCount shall provide the number of arguments for a specific schema action identified by actionHandle.] */
    /* Tests_SRS_SCHEMA_99_052:[The argument count shall be provided via the out argument argumentCount.] */
    /* Tests_SRS_SCHEMA_99_053:[On success, Schema_GetModelActionArgumentCount shall return SCHEMA_OK.] */
    TEST_FUNCTION(Schema_GetModelActionArgumentCount_With_A_Valid_Action_Handle_And_1_Action_Arg_Yields_1)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_MODEL_TYPE_HANDLE modelType = Schema_CreateModelType(schemaHandle, "Model");
        SCHEMA_ACTION_HANDLE actionHandle = Schema_CreateModelAction(modelType, "MyName");
        (void)Schema_AddModelActionArgument(actionHandle, "ArgName", "ArgType");
        size_t argumentCount = 42;

        // act
        SCHEMA_RESULT result = Schema_GetModelActionArgumentCount(actionHandle, &argumentCount);

        // assert
        ASSERT_ARE_EQUAL(SCHEMA_RESULT, SCHEMA_OK, result);
        ASSERT_ARE_EQUAL(size_t, (size_t)1, argumentCount);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Schema_GetModelActionArgumentByName */
    /* Tests_SRS_SCHEMA_99_118: [Schema_GetModelActionArgumentByName shall return NULL if unable to find a matching argument or if any of the arguments are NULL.] */
    TEST_FUNCTION(Schema_GetModelActionArgumentByName_with_NULL_actionHandle_Fails)
    {
        // arrange

        // act
        SCHEMA_ACTION_ARGUMENT_HANDLE result = Schema_GetModelActionArgumentByName(NULL, 0);

        // assert
        ASSERT_IS_NULL(result);
    }

    /* Tests_SRS_SCHEMA_99_118: [Schema_GetModelActionArgumentByName shall return NULL if unable to find a matching argument or if any of the arguments are NULL.] */
    TEST_FUNCTION(Schema_GetModelActionArgumentByName_with_NULL_actionArgumentName_Fails)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_MODEL_TYPE_HANDLE modelType = Schema_CreateModelType(schemaHandle, "Model");
        SCHEMA_ACTION_HANDLE actionHandle = Schema_CreateModelAction(modelType, "ActionName");

        // act
        SCHEMA_ACTION_ARGUMENT_HANDLE result = Schema_GetModelActionArgumentByName(actionHandle, NULL);

        // assert
        ASSERT_IS_NULL(result);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests_SRS_SCHEMA_99_118: [Schema_GetModelActionArgumentByName shall return NULL if unable to find a matching argument or if any of the arguments are NULL.] */
    TEST_FUNCTION(Schema_GetModelActionArgumentByName_with_None_Matching_actionArgumentName_Fails)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_MODEL_TYPE_HANDLE modelType = Schema_CreateModelType(schemaHandle, "Model");
        SCHEMA_ACTION_HANDLE actionHandle = Schema_CreateModelAction(modelType, "ActionName");
        (void)Schema_AddModelActionArgument(actionHandle, "ArgumentName", "ArgumentType");

        // act
        SCHEMA_ACTION_ARGUMENT_HANDLE result = Schema_GetModelActionArgumentByName(actionHandle, "ArgumentInvalidName");

        // assert
        ASSERT_IS_NULL(result);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests_SRS_SCHEMA_99_117: [Schema_GetModelActionArgumentByName shall return a non-NULL handle corresponding to an action argument identified by the actionHandle and actionArgumentName.] */
    TEST_FUNCTION(Schema_GetModelActionArgumentByName_with_valid_arguments_Succeeds)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_MODEL_TYPE_HANDLE modelType = Schema_CreateModelType(schemaHandle, "Model");
        SCHEMA_ACTION_HANDLE actionHandle = Schema_CreateModelAction(modelType, "ActionName");
        (void)Schema_AddModelActionArgument(actionHandle, "ArgumentName", "ArgumentType");

        // act
        SCHEMA_ACTION_ARGUMENT_HANDLE result = Schema_GetModelActionArgumentByName(actionHandle, "ArgumentName");

        // assert
        ASSERT_IS_NOT_NULL(result);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Schema_GetModelActionArgumentByIndex */

    /* Tests_SRS_SCHEMA_99_056:[Schema_GetModelActionArgumentByIndex shall return NULL if the index specified is outside the valid range or if the actionHandle argument is NULL.] */
    TEST_FUNCTION(Schema_GetModelActionArgumentByIndex_With_A_NULL_Handle_Fails)
    {
        // arrange

        // act
        SCHEMA_ACTION_ARGUMENT_HANDLE result = Schema_GetModelActionArgumentByIndex(NULL, 0);

        // assert
        ASSERT_IS_NULL(result);
    }

    /* Tests_SRS_SCHEMA_99_056:[Schema_GetModelActionArgumentByIndex shall return NULL if the index specified is outside the valid range or if the actionHandle argument is NULL.] */
    TEST_FUNCTION(Schema_GetModelActionArgumentByIndex_With_Zero_Index_When_No_Args_Fails)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_MODEL_TYPE_HANDLE modelType = Schema_CreateModelType(schemaHandle, "Model");
        SCHEMA_ACTION_HANDLE actionHandle = Schema_CreateModelAction(modelType, "ActionName");

        // act
        SCHEMA_ACTION_ARGUMENT_HANDLE result = Schema_GetModelActionArgumentByIndex(actionHandle, 0);

        // assert
        ASSERT_IS_NULL(result);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests_SRS_SCHEMA_99_056:[Schema_GetModelActionArgumentByIndex shall return NULL if the index specified is outside the valid range or if the actionHandle argument is NULL.] */
    TEST_FUNCTION(Schema_GetModelActionArgumentByIndex_With_Index_Out_Of_Range_Fails)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_MODEL_TYPE_HANDLE modelType = Schema_CreateModelType(schemaHandle, "Model");
        SCHEMA_ACTION_HANDLE actionHandle = Schema_CreateModelAction(modelType, "ActionName");
        (void)Schema_AddModelActionArgument(actionHandle, "ArgumentName", "ArgumentType");

        // act
        SCHEMA_ACTION_ARGUMENT_HANDLE result = Schema_GetModelActionArgumentByIndex(actionHandle, 1);

        // assert
        ASSERT_IS_NULL(result);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests_SRS_SCHEMA_99_055:[Schema_GetModelActionArgumentByIndex shall return a non-NULL SCHEMA_ACTION_ARGUMENT_HANDLE corresponding to the action type identified by actionHandle and matching the index number provided by the argumentIndex argument.] */
    TEST_FUNCTION(Schema_GetModelActionArgumentByIndex_With_Valid_Arguments_Succeeds)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_MODEL_TYPE_HANDLE modelType = Schema_CreateModelType(schemaHandle, "Model");
        SCHEMA_ACTION_HANDLE actionHandle = Schema_CreateModelAction(modelType, "ActionName");
        (void)Schema_AddModelActionArgument(actionHandle, "ArgumentName", "ArgumentType");

        // act
        SCHEMA_ACTION_ARGUMENT_HANDLE result = Schema_GetModelActionArgumentByIndex(actionHandle, 0);

        // assert
        ASSERT_IS_NOT_NULL(result);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Schema_GetActionArgumentName */
    /* Tests_SRS_SCHEMA_99_114: [Schema_GetActionArgumentName shall return NULL if actionArgumentHandle is NULL.] */
    TEST_FUNCTION(Schema_GetActionArgumentName_with_NULL_actionArgumentHandle_Fails)
    {
        // arrange

        // act
        const char* result = Schema_GetActionArgumentName(NULL);

        // assert
        ASSERT_IS_NULL(result);
    }

    /* Tests_SRS_SCHEMA_99_113: [Schema_GetActionArgumentName shall return the argument name identified by the actionArgumentHandle.] */
    TEST_FUNCTION(Schema_GetActionArgumentName_With_A_Valid_Handle_Returns_The_Name)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_MODEL_TYPE_HANDLE modelType = Schema_CreateModelType(schemaHandle, "Model");
        SCHEMA_ACTION_HANDLE actionHandle = Schema_CreateModelAction(modelType, "ActionName");
        (void)Schema_AddModelActionArgument(actionHandle, "ArgumentName", "ArgumentType");
        SCHEMA_ACTION_ARGUMENT_HANDLE actionArgumentHandle = Schema_GetModelActionArgumentByIndex(actionHandle, 0);

        // act
        const char* result = Schema_GetActionArgumentName(actionArgumentHandle);

        // assert
        ASSERT_ARE_EQUAL(char_ptr, "ArgumentName", result);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Schema_GetActionArgumentType */
    /* Tests_SRS_SCHEMA_99_116: [Schema_GetActionArgumentType shall return NULL if actionArgumentHandle is NULL.] */
    TEST_FUNCTION(Schema_GetActionArgumentType_with_NULL_actionArgumentHandle_Fails)
    {
        // arrange

        // act
        const char* result = Schema_GetActionArgumentType(NULL);

        // assert
        ASSERT_IS_NULL(result);
    }

    /* Tests_SRS_SCHEMA_99_115: [Schema_GetActionArgumentType shall return the argument type identified by the actionArgumentHandle.] */
    TEST_FUNCTION(Schema_GetActionArgumentType_With_A_Valid_Handle_Returns_The_Type)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_MODEL_TYPE_HANDLE modelType = Schema_CreateModelType(schemaHandle, "Model");
        SCHEMA_ACTION_HANDLE actionHandle = Schema_CreateModelAction(modelType, "ActionName");
        (void)Schema_AddModelActionArgument(actionHandle, "ArgumentName", "ArgumentType");
        SCHEMA_ACTION_ARGUMENT_HANDLE actionArgumentHandle = Schema_GetModelActionArgumentByIndex(actionHandle, 0);

        // act
        const char* result = Schema_GetActionArgumentType(actionArgumentHandle);

        // assert
        ASSERT_ARE_EQUAL(char_ptr, "ArgumentType", result);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests_SRS_SCHEMA_99_115: [Schema_GetActionArgumentType shall return the argument type identified by the actionArgumentHandle.] */
    TEST_FUNCTION(Schema_GetActionArgumentType_With_A_Valid_Handle_For_Two_Arguments_Same_Type_Returns_The_Type)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_MODEL_TYPE_HANDLE modelType = Schema_CreateModelType(schemaHandle, "Model");
        SCHEMA_ACTION_HANDLE actionHandle = Schema_CreateModelAction(modelType, "ActionName");
        (void)Schema_AddModelActionArgument(actionHandle, "ArgumentName1", "ArgumentType");
        (void)Schema_AddModelActionArgument(actionHandle, "ArgumentName2", "ArgumentType");

        SCHEMA_ACTION_ARGUMENT_HANDLE actionArgumentHandle1 = Schema_GetModelActionArgumentByIndex(actionHandle, 0);
        SCHEMA_ACTION_ARGUMENT_HANDLE actionArgumentHandle2 = Schema_GetModelActionArgumentByIndex(actionHandle, 1);

        // act
        const char* result1 = Schema_GetActionArgumentType(actionArgumentHandle1);
        const char* result2 = Schema_GetActionArgumentType(actionArgumentHandle2);

        // assert
        ASSERT_ARE_EQUAL(char_ptr, "ArgumentType", result1);
        ASSERT_ARE_EQUAL(char_ptr, "ArgumentType", result2);

        // cleanup
        Schema_Destroy(schemaHandle);
    }


    /* Schema_CreateStructType */

    /* Tests_SRS_SCHEMA_99_060:[If any of the arguments is NULL, Schema_CreateStructType shall return NULL.] */
    TEST_FUNCTION(Schema_CreateStructType_With_A_NULL_Schema_Handle_Fails)
    {
        // arrange

        // act
        SCHEMA_STRUCT_TYPE_HANDLE result = Schema_CreateStructType(NULL, "StructType");

        // assert
        ASSERT_IS_NULL(result);
    }

    /* Tests_SRS_SCHEMA_99_060:[If any of the arguments is NULL, Schema_CreateStructType shall return NULL.] */
    TEST_FUNCTION(Schema_CreateStructType_With_A_NULL_Name_Fails)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);

        // act
        SCHEMA_STRUCT_TYPE_HANDLE result = Schema_CreateStructType(schemaHandle, NULL);

        // assert
        ASSERT_IS_NULL(result);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests_SRS_SCHEMA_99_057:[Schema_CreateStructType shall create a new struct type and return a handle to it.] */
    /* Tests_SRS_SCHEMA_99_058:[On success, a non-NULL handle shall be returned.] */
    TEST_FUNCTION(Schema_CreateStructType_With_Valid_Arguments_Succeeds)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);

        // act
        SCHEMA_STRUCT_TYPE_HANDLE result = Schema_CreateStructType(schemaHandle, "AStruct");

        // assert
        ASSERT_IS_NOT_NULL(result);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests_SRS_SCHEMA_99_057:[Schema_CreateStructType shall create a new struct type and return a handle to it.] */
    /* Tests_SRS_SCHEMA_99_058:[On success, a non-NULL handle shall be returned.] */
    TEST_FUNCTION(Schema_CreateStructType_Creating_2_Different_Structs_Succeeds)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);

        // act
        SCHEMA_STRUCT_TYPE_HANDLE result1 = Schema_CreateStructType(schemaHandle, "AStruct");
        SCHEMA_STRUCT_TYPE_HANDLE result2 = Schema_CreateStructType(schemaHandle, "AnotherStruct");

        // assert
        ASSERT_IS_NOT_NULL(result1);
        ASSERT_IS_NOT_NULL(result2);
        ASSERT_ARE_NOT_EQUAL(void_ptr, (void*)result1, (void*)result2);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests_SRS_SCHEMA_99_057:[Schema_CreateStructType shall create a new struct type and return a handle to it.] */
    /* Tests_SRS_SCHEMA_99_058:[On success, a non-NULL handle shall be returned.] */
    TEST_FUNCTION(Schema_CreateStructType_Creating_2_Different_Structs_In_Different_Schemas_Succeeds)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle1 = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_HANDLE schemaHandle2 = Schema_Create(SCHEMA_NAMESPACE);

        // act
        SCHEMA_STRUCT_TYPE_HANDLE result1 = Schema_CreateStructType(schemaHandle1, "Struct");
        SCHEMA_STRUCT_TYPE_HANDLE result2 = Schema_CreateStructType(schemaHandle2, "Struct");

        // assert
        ASSERT_IS_NOT_NULL(result1);
        ASSERT_IS_NOT_NULL(result2);
        ASSERT_ARE_NOT_EQUAL(void_ptr, (void*)result1, (void*)result2);

        // cleanup
        Schema_Destroy(schemaHandle1);
        Schema_Destroy(schemaHandle2);
    }

    /* Tests_SRS_SCHEMA_99_061:[If a struct type with the same name already exists, Schema_CreateStructType shall return NULL.] */
    TEST_FUNCTION(Schema_CreateStructType_Creating_The_Same_Struct_Twice_In_The_Same_Schema_Fails)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        (void)Schema_CreateStructType(schemaHandle, "Struct");

        // act
        SCHEMA_STRUCT_TYPE_HANDLE result = Schema_CreateStructType(schemaHandle, "Struct");

        // assert
        ASSERT_IS_NULL(result);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Schema_GetStructTypeName */

    /* Tests_SRS_SCHEMA_99_136: [If structTypeHandle is NULL, Schema_GetStructTypeName shall return NULL.] */
    TEST_FUNCTION(Schema_GetStructTypeName_With_A_NULL_Handle_Fails)
    {
        // arrange

        // act
        const char* result = Schema_GetStructTypeName(NULL);

        // assert
        ASSERT_IS_NULL(result);
    }

    /* Tests_SRS_SCHEMA_99_136: [If structTypeHandle is NULL, Schema_GetStructTypeName shall return NULL.] */
    /* Tests_SRS_SCHEMA_99_135: [Schema_GetStructTypeName shall return the name of a struct type identified by the structTypeHandle argument.] */
    TEST_FUNCTION(Schema_GetStructTypeName_With_A_Valid_Struct_Type_Handle_Returns_The_Struct_Type_Name)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_STRUCT_TYPE_HANDLE structTypeHandle = Schema_CreateStructType(schemaHandle, "TestStruct");

        // act
        const char* result = Schema_GetStructTypeName(structTypeHandle);

        // assert
        ASSERT_ARE_EQUAL(char_ptr, "TestStruct", result);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Schema_GetStructTypeByName */

    /* Tests_SRS_SCHEMA_99_069:[Schema_GetStructTypeByName shall return NULL if unable to find a matching struct or if any of the arguments are NULL.] */
    TEST_FUNCTION(Schema_GetStructTypeByName_With_A_NULL_Schema_Handle_Fails)
    {
        // arrange

        // act
        SCHEMA_STRUCT_TYPE_HANDLE result = Schema_GetStructTypeByName(NULL, "Struct");

        // assert
        ASSERT_IS_NULL(result);
    }

    /* Tests_SRS_SCHEMA_99_069:[Schema_GetStructTypeByName shall return NULL if unable to find a matching struct or if any of the arguments are NULL.] */
    TEST_FUNCTION(Schema_GetStructTypeByName_With_A_NULL_Name_Fails)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);

        // act
        SCHEMA_STRUCT_TYPE_HANDLE result = Schema_GetStructTypeByName(schemaHandle, NULL);

        // assert
        ASSERT_IS_NULL(result);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests_SRS_SCHEMA_99_069:[Schema_GetStructTypeByName shall return NULL if unable to find a matching struct or if any of the arguments are NULL.] */
    TEST_FUNCTION(Schema_GetStructTypeByName_With_Valid_Args_But_No_Structs_Exist_In_Schema_Fails)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);

        // act
        SCHEMA_STRUCT_TYPE_HANDLE result = Schema_GetStructTypeByName(schemaHandle, "Struct");

        // assert
        ASSERT_IS_NULL(result);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests_SRS_SCHEMA_99_069:[Schema_GetStructTypeByName shall return NULL if unable to find a matching struct or if any of the arguments are NULL.] */
    TEST_FUNCTION(Schema_GetStructTypeByName_With_A_Struct_That_Does_Not_Exist_Fails)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        (void)Schema_CreateStructType(schemaHandle, "x");

        // act
        SCHEMA_STRUCT_TYPE_HANDLE result = Schema_GetStructTypeByName(schemaHandle, "y");

        // assert
        ASSERT_IS_NULL(result);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests_SRS_SCHEMA_99_069:[Schema_GetStructTypeByName shall return NULL if unable to find a matching struct or if any of the arguments are NULL.] */
    TEST_FUNCTION(Schema_GetStructTypeByName_With_A_Struct_That_Exists_Returns_A_Non_NULL_Handle)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        (void)Schema_CreateStructType(schemaHandle, "StructName");

        // act
        SCHEMA_STRUCT_TYPE_HANDLE result = Schema_GetStructTypeByName(schemaHandle, "StructName");

        // assert
        ASSERT_IS_NOT_NULL(result);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Schema_AddStructTypeProperty */

    /* Tests_SRS_SCHEMA_99_072:[If any of the arguments is NULL, Schema_AddStructTypeProperty shall return SCHEMA_INVALID_ARG.] */
    TEST_FUNCTION(Schema_AddStructTypeProperty_With_NULL_StructType_Fails)
    {
        // arrange

        // act
        SCHEMA_RESULT result = Schema_AddStructTypeProperty(NULL, "MyName", "SomeType");

        // assert
        ASSERT_ARE_EQUAL(SCHEMA_RESULT, SCHEMA_INVALID_ARG, result);
    }

    /* Tests_SRS_SCHEMA_99_072:[If any of the arguments is NULL, Schema_AddStructTypeProperty shall return SCHEMA_INVALID_ARG.] */
    TEST_FUNCTION(Schema_AddStructTypeProperty_With_NULL_Property_Name_Fails)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_MODEL_TYPE_HANDLE structType = Schema_CreateStructType(schemaHandle, "Struct");

        // act
        SCHEMA_RESULT result = Schema_AddStructTypeProperty(structType, NULL, "SomeType");

        // assert
        ASSERT_ARE_EQUAL(SCHEMA_RESULT, SCHEMA_INVALID_ARG, result);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests_SRS_SCHEMA_99_072:[If any of the arguments is NULL, Schema_AddStructTypeProperty shall return SCHEMA_INVALID_ARG.] */
    TEST_FUNCTION(Schema_AddStructTypeProperty_With_NULL_Property_Type_Fails)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_STRUCT_TYPE_HANDLE structType = Schema_CreateStructType(schemaHandle, "Struct");

        // act
        SCHEMA_RESULT result = Schema_AddStructTypeProperty(structType, "MyName", NULL);

        // assert
        ASSERT_ARE_EQUAL(SCHEMA_RESULT, SCHEMA_INVALID_ARG, result);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests_SRS_SCHEMA_99_070:[Schema_AddStructTypeProperty shall add one property to the struct type identified by structTypeHandle.] */
    /* Tests_SRS_SCHEMA_99_071:[On success, Schema_AddStructTypeProperty shall return SCHEMA_OK.] */
    TEST_FUNCTION(Schema_AddStructTypeProperty_With_Valid_Arguments_For_A_StructType_Succeeds)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_STRUCT_TYPE_HANDLE structType = Schema_CreateStructType(schemaHandle, "Struct");

        // act
        SCHEMA_RESULT result = Schema_AddStructTypeProperty(structType, "MyName", "SomeType");

        // assert
        ASSERT_ARE_EQUAL(SCHEMA_RESULT, SCHEMA_OK, result);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests_SRS_SCHEMA_99_070:[Schema_AddStructTypeProperty shall add one property to the struct type identified by structTypeHandle.] */
    /* Tests_SRS_SCHEMA_99_071:[On success, Schema_AddStructTypeProperty shall return SCHEMA_OK.] */
    TEST_FUNCTION(Schema_AddStructTypeProperty_Adding_2_Properties_With_Different_Names_To_The_Same_StructType_Succeeds)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_STRUCT_TYPE_HANDLE structType = Schema_CreateStructType(schemaHandle, "Struct");
        (void)Schema_AddStructTypeProperty(structType, "MyName", "SomeType");

        // act
        SCHEMA_RESULT result = Schema_AddStructTypeProperty(structType, "MyName2", "SomeType");

        // assert
        ASSERT_ARE_EQUAL(SCHEMA_RESULT, SCHEMA_OK, result);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests_SRS_SCHEMA_99_074:[The property name shall be unique per struct type, if the same property name is added twice to a struct type, SCHEMA_DUPLICATE_ELEMENT shall be returned.] */
    TEST_FUNCTION(Schema_AddStructTypeProperty_Adding_The_Same_Property_In_The_Same_StructType_Fails)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_STRUCT_TYPE_HANDLE structType = Schema_CreateStructType(schemaHandle, "Struct");
        (void)Schema_AddStructTypeProperty(structType, "MyName", "SomeType");

        // act
        SCHEMA_RESULT result = Schema_AddStructTypeProperty(structType, "MyName", "SomeType");

        // assert
        ASSERT_ARE_EQUAL(SCHEMA_RESULT, SCHEMA_DUPLICATE_ELEMENT, result);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests_SRS_SCHEMA_99_074:[The property name shall be unique per struct type, if the same property name is added twice to a struct type, SCHEMA_DUPLICATE_ELEMENT shall be returned.] */
    TEST_FUNCTION(Schema_AddStructTypeProperty_Adding_The_Same_Property_With_Different_Type_In_The_Same_StructType_Fails)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_STRUCT_TYPE_HANDLE structType = Schema_CreateStructType(schemaHandle, "Struct");
        (void)Schema_AddStructTypeProperty(structType, "MyName", "SomeType");

        // act
        SCHEMA_RESULT result = Schema_AddStructTypeProperty(structType, "MyName", "SomeType2");

        // assert
        ASSERT_ARE_EQUAL(SCHEMA_RESULT, SCHEMA_DUPLICATE_ELEMENT, result);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests_SRS_SCHEMA_99_074:[The property name shall be unique per struct type, if the same property name is added twice to a struct type, SCHEMA_DUPLICATE_ELEMENT shall be returned.] */
    TEST_FUNCTION(Schema_AddStructTypeProperty_Adding_The_Same_Property_In_The_Same_StructType_When_The_Duplicate_Property_Is_Not_First_Fails)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_STRUCT_TYPE_HANDLE structType = Schema_CreateStructType(schemaHandle, "Struct");
        (void)Schema_AddStructTypeProperty(structType, "PropertyName1", "PropertyType1");
        (void)Schema_AddStructTypeProperty(structType, "PropertyName2", "PropertyType2");

        // act
        SCHEMA_RESULT result = Schema_AddStructTypeProperty(structType, "PropertyName2", "PropertyType2");

        // assert
        ASSERT_ARE_EQUAL(SCHEMA_RESULT, SCHEMA_DUPLICATE_ELEMENT, result);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests_SRS_SCHEMA_99_074:[The property name shall be unique per struct type, if the same property name is added twice to a struct type, SCHEMA_DUPLICATE_ELEMENT shall be returned.] */
    TEST_FUNCTION(Schema_AddStructTypeProperty_Adding_The_Same_Property_In_Different_Structs_Succeeds)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_STRUCT_TYPE_HANDLE structType1 = Schema_CreateStructType(schemaHandle, "Struct1");
        SCHEMA_STRUCT_TYPE_HANDLE structType2 = Schema_CreateStructType(schemaHandle, "Struct2");
        (void)Schema_AddStructTypeProperty(structType1, "MyName", "SomeType");

        // act
        SCHEMA_RESULT result = Schema_AddStructTypeProperty(structType2, "MyName", "SomeType");

        // assert
        ASSERT_ARE_EQUAL(SCHEMA_RESULT, SCHEMA_OK, result);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Schema_GetStructTypeCount */

    /* Tests_SRS_SCHEMA_99_140: [Schema_GetStructTypeCount shall return SCHEMA_INVALID_ARG if any of the arguments is NULL.] */
    TEST_FUNCTION(Schema_GetStructTypeCount_with_NULL_schemaHandle_Fails)
    {
        // arrange
        size_t structTypeCount;

        // act
        SCHEMA_RESULT result = Schema_GetStructTypeCount(NULL, &structTypeCount);

        // assert
        ASSERT_ARE_EQUAL(SCHEMA_RESULT, SCHEMA_INVALID_ARG, result);
    }

    /* Tests_SRS_SCHEMA_99_140: [Schema_GetStructTypeCount shall return SCHEMA_INVALID_ARG if any of the arguments is NULL.] */
    TEST_FUNCTION(Schema_GetStructTypeCount_with_NULL_structTypeCount_Fails)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);

        // act
        SCHEMA_RESULT result = Schema_GetStructTypeCount(schemaHandle, NULL);

        // assert
        ASSERT_ARE_EQUAL(SCHEMA_RESULT, SCHEMA_INVALID_ARG, result);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests_SRS_SCHEMA_99_137: [Schema_GetStructTypeCount shall provide the number of structs defined in the schema identified by schemaHandle.] */
    /* Tests_SRS_SCHEMA_99_138: [The count shall be provided via the out argument structTypeCount.] */
    /* Tests_SRS_SCHEMA_99_139: [On success, Schema_GetStructTypeCount shall return SCHEMA_OK.] */
    TEST_FUNCTION(Schema_GetStructTypeCount_With_Zero_Struct_Types_Defined_Yields_0)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        size_t structTypeCount = 42;

        // act
        SCHEMA_RESULT result = Schema_GetStructTypeCount(schemaHandle, &structTypeCount);

        // assert
        ASSERT_ARE_EQUAL(SCHEMA_RESULT, SCHEMA_OK, result);
        ASSERT_ARE_EQUAL(size_t, (size_t)0, structTypeCount);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests_SRS_SCHEMA_99_137: [Schema_GetStructTypeCount shall provide the number of structs defined in the schema identified by schemaHandle.] */
    /* Tests_SRS_SCHEMA_99_138: [The count shall be provided via the out argument structTypeCount.] */
    /* Tests_SRS_SCHEMA_99_139: [On success, Schema_GetStructTypeCount shall return SCHEMA_OK.] */
    TEST_FUNCTION(Schema_GetStructTypeCount_With_Two_Struct_Types_Defined_Yields_2)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        (void)Schema_CreateStructType(schemaHandle, "StructName1");
        (void)Schema_CreateStructType(schemaHandle, "StructName2");
        size_t structTypeCount = 42;

        // act
        SCHEMA_RESULT result = Schema_GetStructTypeCount(schemaHandle, &structTypeCount);

        // assert
        ASSERT_ARE_EQUAL(SCHEMA_RESULT, SCHEMA_OK, result);
        ASSERT_ARE_EQUAL(size_t, (size_t)2, structTypeCount);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests_SRS_SCHEMA_99_137: [Schema_GetStructTypeCount shall provide the number of structs defined in the schema identified by schemaHandle.] */
    /* Tests_SRS_SCHEMA_99_138: [The count shall be provided via the out argument structTypeCount.] */
    /* Tests_SRS_SCHEMA_99_139: [On success, Schema_GetStructTypeCount shall return SCHEMA_OK.] */
    TEST_FUNCTION(Schema_GetStructTypeCount_With_Two_Struct_Types_Added_with_the_same_structTypeName_Yields_1)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        (void)Schema_CreateStructType(schemaHandle, "StructName");
        (void)Schema_CreateStructType(schemaHandle, "StructName");
        size_t structTypeCount = 42;

        // act
        SCHEMA_RESULT result = Schema_GetStructTypeCount(schemaHandle, &structTypeCount);

        // assert
        ASSERT_ARE_EQUAL(SCHEMA_RESULT, SCHEMA_OK, result);
        ASSERT_ARE_EQUAL(size_t, (size_t)1, structTypeCount);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Schema_GetStructTypeByIndex */

    /* Tests_SRS_SCHEMA_99_143: [Schema_GetStructTypeByIndex shall return NULL if the index specified is outside the valid range or if schemaHandle argument is NULL.] */
    TEST_FUNCTION(Schema_GetStructTypeByIndex_With_NULL_schemaHandle_Fails)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        (void)Schema_CreateStructType(schemaHandle, "TestStruct");

        // act
        SCHEMA_STRUCT_TYPE_HANDLE result = Schema_GetStructTypeByIndex(NULL, 0);

        // assert
        ASSERT_IS_NULL(result);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests_SRS_SCHEMA_99_143: [Schema_GetStructTypeByIndex shall return NULL if the index specified is outside the valid range or if schemaHandle argument is NULL.] */
    TEST_FUNCTION(Schema_GetStructTypeByIndex_With_No_Struct_Types_Defined_Fails)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);

        // act
        SCHEMA_STRUCT_TYPE_HANDLE result = Schema_GetStructTypeByIndex(schemaHandle, 0);

        // assert
        ASSERT_IS_NULL(result);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests_SRS_SCHEMA_99_143: [Schema_GetStructTypeByIndex shall return NULL if the index specified is outside the valid range or if schemaHandle argument is NULL.] */
    /* Tests_SRS_SCHEMA_99_142: [The index argument is zero based, and the order in which models were added shall be the index in which they will be retrieved.] */
    TEST_FUNCTION(Schema_GetStructTypeByIndex_With_Index_Equals_The_Number_Of_Defined_StructTypes_Fails)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        (void)Schema_CreateStructType(schemaHandle, "StructName1");
        (void)Schema_CreateStructType(schemaHandle, "StructName2");
        (void)Schema_CreateStructType(schemaHandle, "StructName3");

        // act
        SCHEMA_MODEL_TYPE_HANDLE result = Schema_GetStructTypeByIndex(schemaHandle, 3);

        // assert
        ASSERT_IS_NULL(result);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests_SRS_SCHEMA_99_143: [Schema_GetStructTypeByIndex shall return NULL if the index specified is outside the valid range or if schemaHandle argument is NULL.] */
    /* Tests_SRS_SCHEMA_99_142: [The index argument is zero based, and the order in which models were added shall be the index in which they will be retrieved.] */
    TEST_FUNCTION(Schema_GetStructTypeByIndex_With_Index_Greater_Than_The_Number_Of_Defined_Struct_Types_Fails)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        (void)Schema_CreateStructType(schemaHandle, "StructName1");
        (void)Schema_CreateStructType(schemaHandle, "StructName2");
        (void)Schema_CreateStructType(schemaHandle, "StructName3");

        // act
        SCHEMA_MODEL_TYPE_HANDLE result = Schema_GetStructTypeByIndex(schemaHandle, 4);

        // assert
        ASSERT_IS_NULL(result);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests_SRS_SCHEMA_99_141: [Schema_GetStructTypeByIndex shall return a non-NULL SCHEMA_STRUCT_TYPE_HANDLE corresponding to the struct type identified by schemaHandle and matching the index number provided by the index argument.] */
    /* Tests_SRS_SCHEMA_99_142: [The index argument is zero based, and the order in which models were added shall be the index in which they will be retrieved.] */
    TEST_FUNCTION(Schema_GetStructTypeByIndex_With_Index_Within_The_Number_Of_Defined_Struct_Types_Succeeds)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        (void)Schema_CreateStructType(schemaHandle, "StructName1");
        (void)Schema_CreateStructType(schemaHandle, "StructName2");
        (void)Schema_CreateStructType(schemaHandle, "StructName3");

        // act
        SCHEMA_MODEL_TYPE_HANDLE result = Schema_GetStructTypeByIndex(schemaHandle, 2);

        // assert
        ASSERT_IS_NOT_NULL(result);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Schema_GetStructTypePropertyByName */

    /* Tests_SRS_SCHEMA_99_076:[Schema_GetStructTypePropertyByName shall return NULL if unable to find a matching property or if any of the arguments are NULL.] */
    TEST_FUNCTION(Schema_GetStructTypePropertyByName_with_NULL_structTypeHandle_Fails)
    {
        // arrange

        // act
        SCHEMA_PROPERTY_HANDLE result = Schema_GetStructTypePropertyByName(NULL, "PropertyName");

        // assert
        ASSERT_IS_NULL(result);
    }

    /* Tests_SRS_SCHEMA_99_076:[Schema_GetStructTypePropertyByName shall return NULL if unable to find a matching property or if any of the arguments are NULL.] */
    TEST_FUNCTION(Schema_GetStructTypePropertyByName_with_NULL_propertyName_Fails)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_STRUCT_TYPE_HANDLE structType = Schema_CreateStructType(schemaHandle, "StructName");
        (void)Schema_AddStructTypeProperty(structType, "PropertyName", "PropertyType");

       // act
        SCHEMA_PROPERTY_HANDLE result = Schema_GetStructTypePropertyByName(structType, NULL);

        // assert
        ASSERT_IS_NULL(result);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests_SRS_SCHEMA_99_076:[Schema_GetStructTypePropertyByName shall return NULL if unable to find a matching property or if any of the arguments are NULL.] */
    TEST_FUNCTION(Schema_GetStructTypePropertyByName_When_The_Property_Is_Not_Found_Fails)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_STRUCT_TYPE_HANDLE structType = Schema_CreateStructType(schemaHandle, "StructName");
        (void)Schema_AddStructTypeProperty(structType, "PropertyName", "PropertyType");

        // act
        SCHEMA_PROPERTY_HANDLE result = Schema_GetStructTypePropertyByName(structType, "AnotherPropertyName");

        // assert
        ASSERT_IS_NULL(result);

        // cleanup
        Schema_Destroy(schemaHandle);

    }

    /* Tests_SRS_SCHEMA_99_075:[Schema_GetStructTypePropertyByName shall return a non-NULL handle corresponding to a property identified by the structTypeHandle and propertyName.] */
    TEST_FUNCTION(Schema_GetStructTypePropertyByName_For_A_StructType_When_The_Property_Is_Found_Returns_A_Property_Handle)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_STRUCT_TYPE_HANDLE structType = Schema_CreateStructType(schemaHandle, "StructName");
        (void)Schema_AddStructTypeProperty(structType, "PropertyName", "PropertyType");

        // act
        SCHEMA_PROPERTY_HANDLE result = Schema_GetStructTypePropertyByName(structType, "PropertyName");

        // assert
        ASSERT_IS_NOT_NULL(result);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests_SRS_SCHEMA_99_075:[Schema_GetStructTypePropertyByName shall return a non-NULL handle corresponding to a property identified by the structTypeHandle and propertyName.] */
    TEST_FUNCTION(Schema_GetStructTypePropertyByName_When_The_Property_Is_Found_After_Adding_2_Properties_Returns_A_Property_Handle)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_STRUCT_TYPE_HANDLE structType = Schema_CreateStructType(schemaHandle, "StructName");
        (void)Schema_AddStructTypeProperty(structType, "PropertyName1", "PropertyType1");
        (void)Schema_AddStructTypeProperty(structType, "PropertyName2", "PropertyType2");

        // act
        SCHEMA_PROPERTY_HANDLE result1 = Schema_GetStructTypePropertyByName(structType, "PropertyName1");
        SCHEMA_PROPERTY_HANDLE result2 = Schema_GetStructTypePropertyByName(structType, "PropertyName2");

        // assert
        ASSERT_IS_NOT_NULL(result1);
        ASSERT_IS_NOT_NULL(result2);

        // cleanup
        Schema_Destroy(schemaHandle);
    }
    
    /* Schema_GetStructTypePropertyCount */
    /* Tests_SRS_SCHEMA_99_079: [Schema_GetStructTypePropertyCount shall return SCHEMA_INVALID_ARG if any of the structlTypeHandle or propertyCount arguments is NULL.] */
    TEST_FUNCTION(Schema_GetStructTypePropertyCount_With_NULL_structTypeHandle_Fails)
    {
        // arrange
        size_t propertyCount = 42;
        // act
        SCHEMA_RESULT result = Schema_GetStructTypePropertyCount(NULL, &propertyCount);

        // assert
        ASSERT_ARE_EQUAL(SCHEMA_RESULT, SCHEMA_INVALID_ARG, result);
    }

    /* Tests_SRS_SCHEMA_99_079: [Schema_GetStructTypePropertyCount shall return SCHEMA_INVALID_ARG if any of the structlTypeHandle or propertyCount arguments is NULL.] */
    TEST_FUNCTION(Schema_GetStructTypePropertyCount_With_NULL_propertyCount_Fails)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_STRUCT_TYPE_HANDLE structType = Schema_CreateStructType(schemaHandle, "StructName");

        // act
        SCHEMA_RESULT result = Schema_GetStructTypePropertyCount(structType, NULL);

        // assert
        ASSERT_ARE_EQUAL(SCHEMA_RESULT, SCHEMA_INVALID_ARG, result);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests_SRS_SCHEMA_99_079: [Schema_GetStructTypePropertyCount shall return SCHEMA_INVALID_ARG if any of the structlTypeHandle or propertyCount arguments is NULL.] */
    TEST_FUNCTION(Schema_GetStructTypePropertyCount_With_Both_NULL_Arguments_Fails)
    {
        // arrange

        // act
        SCHEMA_RESULT result = Schema_GetStructTypePropertyCount(NULL, NULL);

        // assert
        ASSERT_ARE_EQUAL(SCHEMA_RESULT, SCHEMA_INVALID_ARG, result);
    }

    /* Tests_SRS_SCHEMA_99_077: [Schema_GetStructTypePropertyCount shall provide the total number of properties defined in a struct type identified by structTypeHandle. The value is provided via the out argument propertyCount.] */
    /* Tests_SRS_SCHEMA_99_081: [The count shall be provided via the out argument propertyCount.] */
    /* Tests_SRS_SCHEMA_99_078: [On success, Schema_ GetStructTypePropertyCount shall return SCHEMA_OK.] */
    TEST_FUNCTION(Schema_GetStructTypePropertyCount_With_No_Properties_Defined_Yields_0)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_STRUCT_TYPE_HANDLE structType = Schema_CreateStructType(schemaHandle, "StructName");
        size_t propertyCount = 42;

        // act
        SCHEMA_RESULT result = Schema_GetStructTypePropertyCount(structType, &propertyCount);

        // assert
        ASSERT_ARE_EQUAL(SCHEMA_RESULT, SCHEMA_OK, result);
        ASSERT_ARE_EQUAL(size_t, (size_t)0, propertyCount);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests_SRS_SCHEMA_99_077: [Schema_GetStructTypePropertyCount shall provide the total number of properties defined in a struct type identified by structTypeHandle. The value is provided via the out argument propertyCount.] */
    /* Tests_SRS_SCHEMA_99_081: [The count shall be provided via the out argument propertyCount.] */
    /* Tests_SRS_SCHEMA_99_078: [On success, Schema_ GetStructTypePropertyCount shall return SCHEMA_OK.] */
    TEST_FUNCTION(Schema_GetStructTypePropertyCount_With_1_Property_Defined_Yields_1)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_STRUCT_TYPE_HANDLE structType = Schema_CreateStructType(schemaHandle, "StructName");
        (void)Schema_AddStructTypeProperty(structType, "PropertyName1", "PropertyType1");
        size_t propertyCount = 42;

        // act
        SCHEMA_RESULT result = Schema_GetStructTypePropertyCount(structType, &propertyCount);

        // assert
        ASSERT_ARE_EQUAL(SCHEMA_RESULT, SCHEMA_OK, result);
        ASSERT_ARE_EQUAL(size_t, (size_t)1, propertyCount);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests_SRS_SCHEMA_99_077: [Schema_GetStructTypePropertyCount shall provide the total number of properties defined in a struct type identified by structTypeHandle. The value is provided via the out argument propertyCount.] */
    /* Tests_SRS_SCHEMA_99_081: [The count shall be provided via the out argument propertyCount.] */
    /* Tests_SRS_SCHEMA_99_078: [On success, Schema_ GetStructTypePropertyCount shall return SCHEMA_OK.] */
    TEST_FUNCTION(Schema_GetStructTypePropertyCount_With_2_Properties_Defined_Yields_2)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_STRUCT_TYPE_HANDLE structType = Schema_CreateStructType(schemaHandle, "StructName");
        (void)Schema_AddStructTypeProperty(structType, "PropertyName1", "PropertyType1");
        (void)Schema_AddStructTypeProperty(structType, "PropertyName2", "PropertyType2");
        size_t propertyCount = 42;

        // act
        SCHEMA_RESULT result = Schema_GetStructTypePropertyCount(structType, &propertyCount);

        // assert
        ASSERT_ARE_EQUAL(SCHEMA_RESULT, SCHEMA_OK, result);
        ASSERT_ARE_EQUAL(size_t, (size_t)2, propertyCount);

        // cleanup
        Schema_Destroy(schemaHandle);
    }


    /* Schema_GetStructTypeProperty */
    /* Tests_SRS_SCHEMA_99_083: [Schema_ GetStructTypeProperty shall return NULL if the index specified is outside the valid range, if structTypeHandle argument is NULL] */
    TEST_FUNCTION(Schema_GetStructTypeProperty_With_NULL_propertyHandle_Fails)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_STRUCT_TYPE_HANDLE structType = Schema_CreateStructType(schemaHandle, "StructName");
        (void)Schema_AddStructTypeProperty(structType, "PropertyName1", "PropertyType1");
        (void)Schema_AddStructTypeProperty(structType, "PropertyName2", "PropertyType2");

        // act
        SCHEMA_PROPERTY_HANDLE result = Schema_GetStructTypePropertyByIndex(NULL, 1);

        // assert
        ASSERT_IS_NULL(result);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests_SRS_SCHEMA_99_083: [Schema_ GetStructTypeProperty shall return NULL if the index specified is outside the valid range, if structTypeHandle argument is NULL] */
    TEST_FUNCTION(Schema_GetStructTypeProperty_With_No_Properies_Defined_Fails)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_STRUCT_TYPE_HANDLE structType = Schema_CreateStructType(schemaHandle, "StructName");

        // act
        SCHEMA_PROPERTY_HANDLE result = Schema_GetStructTypePropertyByIndex(structType, 0);

        // assert
        ASSERT_IS_NULL(result);

        // cleanup
        Schema_Destroy(schemaHandle);
    }
    
    /* Tests_SRS_SCHEMA_99_083: [Schema_ GetStructTypeProperty shall return NULL if the index specified is outside the valid range, if structTypeHandle argument is NULL] */
    TEST_FUNCTION(Schema_GetStructTypeProperty_With_Index_Greater_Than_Number_OF_Properties_Fails)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_STRUCT_TYPE_HANDLE structType = Schema_CreateStructType(schemaHandle, "StructName");
        (void)Schema_AddStructTypeProperty(structType, "PropertyName1", "PropertyType1");
        (void)Schema_AddStructTypeProperty(structType, "PropertyName2", "PropertyType2");

        // act
        SCHEMA_PROPERTY_HANDLE result = Schema_GetStructTypePropertyByIndex(structType, 3);

        // assert
        ASSERT_IS_NULL(result);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests_SRS_SCHEMA_99_083: [Schema_ GetStructTypeProperty shall return NULL if the index specified is outside the valid range, if structTypeHandle argument is NULL] */
    TEST_FUNCTION(Schema_GetStructTypeProperty_With_Index_Equal_To_Number_OF_Properties_Fails)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_STRUCT_TYPE_HANDLE structType = Schema_CreateStructType(schemaHandle, "StructName");
        (void)Schema_AddStructTypeProperty(structType, "PropertyName1", "PropertyType1");
        (void)Schema_AddStructTypeProperty(structType, "PropertyName2", "PropertyType2");

        // act
        SCHEMA_PROPERTY_HANDLE result = Schema_GetStructTypePropertyByIndex(structType, 2);

        // assert
        ASSERT_IS_NULL(result);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests_SRS_SCHEMA_99_082: [Schema_GetStructTypeProperty shall return a non-NULL SCHEMA_PROPERTY_HANDLE corresponding to the struct type identified by strutTypeHandle and matching the index number provided by the index argument.] */
    /* Tests_SRS_SCHEMA_99_098: [index is zero based and the order in which actions were added shall be the index in which they will be retrieved.] */
    TEST_FUNCTION(Schema_GetStructTypeProperty_With_Valid_Arguments_Succeeds)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_STRUCT_TYPE_HANDLE structType = Schema_CreateStructType(schemaHandle, "StructName");
        (void)Schema_AddStructTypeProperty(structType, "PropertyName1", "PropertyType1");

        // act
        SCHEMA_PROPERTY_HANDLE result = Schema_GetStructTypePropertyByIndex(structType, 0);

        // assert
        ASSERT_IS_NOT_NULL(result);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests_SRS_SCHEMA_99_082: [Schema_GetStructTypeProperty shall return a non-NULL SCHEMA_PROPERTY_HANDLE corresponding to the struct type identified by strutTypeHandle and matching the index number provided by the index argument.] */
    /* Tests_SRS_SCHEMA_99_098: [index is zero based and the order in which actions were added shall be the index in which they will be retrieved.] */
    TEST_FUNCTION(Schema_GetStructTypeProperty_For_2_Different_Properties_Returns_Different_Handles)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_STRUCT_TYPE_HANDLE structType = Schema_CreateStructType(schemaHandle, "StructName");
        (void)Schema_AddStructTypeProperty(structType, "PropertyName1", "PropertyType1");
        (void)Schema_AddStructTypeProperty(structType, "PropertyName2", "PropertyType2");

        // act
        SCHEMA_PROPERTY_HANDLE result1 = Schema_GetStructTypePropertyByIndex(structType, 0);
        SCHEMA_PROPERTY_HANDLE result2 = Schema_GetStructTypePropertyByIndex(structType, 1);

        // assert
        ASSERT_IS_NOT_NULL(result1);
        ASSERT_IS_NOT_NULL(result2);
        ASSERT_ARE_NOT_EQUAL(void_ptr, (void*)result1, (void*)result2);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Schema_GetPropertyName */

    /* Tests_SRS_SCHEMA_99_085: [Schema_GetPropertyName shall return the property name identified by the propertyHandle.] */
    TEST_FUNCTION(Schema_GetPropertyName_With_A_NULL_Handle_Fails)
    {
        // arrange

        // act
        const char* result = Schema_GetPropertyName(NULL);

        // assert
        ASSERT_IS_NULL(result);
    }

    /* Tests_SRS_SCHEMA_99_085: [Schema_GetPropertyName shall return the property name identified by the propertyHandle.] */
    TEST_FUNCTION(Schema_GetPropertyName_With_A_Valid_Handle_Returns_The_Name)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_STRUCT_TYPE_HANDLE structType = Schema_CreateStructType(schemaHandle, "StructName");
        (void)Schema_AddStructTypeProperty(structType, "PropertyName1", "PropertyType1");

        // act
        const char* result = Schema_GetPropertyName(Schema_GetStructTypePropertyByIndex(structType, 0));

        // assert
        ASSERT_ARE_EQUAL(char_ptr, "PropertyName1", result);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Schema_GetPropertyType */

    /* Tests_SRS_SCHEMA_99_088: [If propertyHandle is NULL, Schema_GetPropertyType shall return NULL.] */
    TEST_FUNCTION(Schema_GetPropertyType_With_A_NULL_Handle_Fails)
    {
        // arrange

        // act
        const char* result = Schema_GetPropertyType(NULL);

        // assert
        ASSERT_IS_NULL(result);
    }

    /* Tests_SRS_SCHEMA_99_087: [Schema_GetPropertyType shall return the property type identified by the propertyHandle.] */
    TEST_FUNCTION(Schema_GetPropertyType_With_A_Valid_Handle_Returns_The_Name)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_STRUCT_TYPE_HANDLE structType = Schema_CreateStructType(schemaHandle, "StructName");
        (void)Schema_AddStructTypeProperty(structType, "PropertyName1", "PropertyType1");

        // act
        const char* result = Schema_GetPropertyType(Schema_GetStructTypePropertyByIndex(structType, 0));

        // assert
        ASSERT_ARE_EQUAL(char_ptr, "PropertyType1", result);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /*Tests_SRS_SCHEMA_99_160: [Schema_GetModelName shall return the name of the model identified by modelTypeHandle. If the name cannot be retrieved, then NULL shall be returned.]*/
    TEST_FUNCTION(Schema_GetModelName_with_NULL_parameter_returns_NULL)
    {
        // arrange

        // act
        const char* result = Schema_GetModelName(NULL);

        // assert
        ASSERT_IS_NULL(result);
    }

    /*Tests_SRS_SCHEMA_99_160: [Schema_GetModelName shall return the name of the model identified by modelTypeHandle. If the name cannot be retrieved, then NULL shall be returned.]*/
    TEST_FUNCTION(Schema_GetModelName_with_non_NULL_parameter_returns_the_name_of_the_parameter)
    {
        // arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_MODEL_TYPE_HANDLE modelType = Schema_CreateModelType(schemaHandle, "someModel");

        // act
        const char* result = Schema_GetModelName(modelType);

        // assert
        ASSERT_ARE_EQUAL(char_ptr, "someModel", result);

        // cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Schema_AddModelModel */


    /*Tests_SRS_SCHEMA_99_163: [Schema_AddModelModel shall insert an existing model, identified by the handle modelType, into the existing model identified by modelTypeHandle under a property having the name propertyName.]*/
    /*Tests_SRS_SCHEMA_99_164: [If the function succeeds, then the return value shall be SCHEMA_OK.]*/
    TEST_FUNCTION(Schema_AddModelModel_happy_path_succeeds)
    {
        ///arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_MODEL_TYPE_HANDLE model = Schema_CreateModelType(schemaHandle, "someModel");
        SCHEMA_MODEL_TYPE_HANDLE minerModel = Schema_CreateModelType(schemaHandle, "someMinerModel");

        ///act
        auto result = Schema_AddModelModel(model, "ManicMiner", minerModel);

        ///assert
        ASSERT_ARE_EQUAL(SCHEMA_RESULT, SCHEMA_OK, result);

        ///cleanup
        Schema_Destroy(schemaHandle);

    }

    /*Codes_SRS_SCHEMA_99_165: [If any of the parameters is NULL then Schema_AddModelModel shall return SCHEMA_INVALID_ARG.]*/
    TEST_FUNCTION(Schema_AddModelModel_with_invalid_arg1_fails)
    {
        ///arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_MODEL_TYPE_HANDLE minerModel = Schema_CreateModelType(schemaHandle, "someMinerModel");

        ///act
        auto result = Schema_AddModelModel(NULL, "ManicMiner", minerModel);

        ///assert
        ASSERT_ARE_EQUAL(SCHEMA_RESULT, SCHEMA_INVALID_ARG, result);

        ///cleanup
        Schema_Destroy(schemaHandle);

    }

    /*Codes_SRS_SCHEMA_99_165: [If any of the parameters is NULL then Schema_AddModelModel shall return SCHEMA_INVALID_ARG.]*/
    TEST_FUNCTION(Schema_AddModelModel_with_invalid_arg2_fails)
    {
        ///arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_MODEL_TYPE_HANDLE model = Schema_CreateModelType(schemaHandle, "someModel");
        SCHEMA_MODEL_TYPE_HANDLE minerModel = Schema_CreateModelType(schemaHandle, "someMinerModel");

        ///act
        auto result = Schema_AddModelModel(model, NULL, minerModel);

        ///assert
        ASSERT_ARE_EQUAL(SCHEMA_RESULT, SCHEMA_INVALID_ARG, result);

        ///cleanup
        Schema_Destroy(schemaHandle);

    }

    /*Codes_SRS_SCHEMA_99_165: [If any of the parameters is NULL then Schema_AddModelModel shall return SCHEMA_INVALID_ARG.]*/
    TEST_FUNCTION(Schema_AddModelModel_with_invalid_arg3_fails)
    {
        ///arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_MODEL_TYPE_HANDLE model = Schema_CreateModelType(schemaHandle, "someModel");

        ///act
        auto result = Schema_AddModelModel(model, "ManicMiner", NULL);

        ///assert
        ASSERT_ARE_EQUAL(SCHEMA_RESULT, SCHEMA_INVALID_ARG, result);

        ///cleanup
        Schema_Destroy(schemaHandle);

    }


    /*Tests_SRS_SCHEMA_99_167: [Schema_GetModelModelCount shall return in parameter modelCount the number of models inserted in the model identified by parameter modelTypeHandle.]*/
    /*Tests_SRS_SCHEMA_99_168: [If the function succeeds, it shall return SCHEMA_OK.]*/
    TEST_FUNCTION(Schema_GetModelModelCount_with_no_model_returns_0)
    {
        ///arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_MODEL_TYPE_HANDLE model = Schema_CreateModelType(schemaHandle, "someModel");
        (void)Schema_CreateModelType(schemaHandle, "someMinerModel");
        
        size_t nModels = 444;

        ///act
        auto result = Schema_GetModelModelCount(model, &nModels);

        ///assert
        ASSERT_ARE_EQUAL(SCHEMA_RESULT, SCHEMA_OK, result);
        ASSERT_ARE_EQUAL(size_t, 0, nModels);

        ///cleanup
        Schema_Destroy(schemaHandle);
    }

    /*Tests_SRS_SCHEMA_99_167: [Schema_GetModelModelCount shall return in parameter modelCount the number of models inserted in the model identified by parameter modelTypeHandle.]*/
    /*Tests_SRS_SCHEMA_99_168: [If the function succeeds, it shall return SCHEMA_OK.]*/
    TEST_FUNCTION(Schema_GetModelModelCount_with_1_model_returns_1)
    {
        ///arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_MODEL_TYPE_HANDLE model = Schema_CreateModelType(schemaHandle, "someModel");
        SCHEMA_MODEL_TYPE_HANDLE minerModel = Schema_CreateModelType(schemaHandle, "someMinerModel");
        size_t nModels = 444;
        (void)Schema_AddModelModel(model, "ManicMiner", minerModel);

        ///act
        auto result = Schema_GetModelModelCount(model, &nModels);

        ///assert
        ASSERT_ARE_EQUAL(SCHEMA_RESULT, SCHEMA_OK, result);
        ASSERT_ARE_EQUAL(size_t, 1, nModels);

        ///cleanup
        Schema_Destroy(schemaHandle);
    }

    /*Tests_SRS_SCHEMA_99_167: [Schema_GetModelModelCount shall return in parameter modelCount the number of models inserted in the model identified by parameter modelTypeHandle.]*/
    /*Tests_SRS_SCHEMA_99_168: [If the function succeeds, it shall return SCHEMA_OK.]*/
    TEST_FUNCTION(Schema_GetModelModelCount_with_2_models_returns_2)
    {
        ///arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_MODEL_TYPE_HANDLE model = Schema_CreateModelType(schemaHandle, "someModel");
        SCHEMA_MODEL_TYPE_HANDLE minerModel = Schema_CreateModelType(schemaHandle, "someMinerModel");
        size_t nModels = 444;
        (void)Schema_AddModelModel(model, "ManicMiner", minerModel);
        (void)Schema_AddModelModel(model, "ManicMiner2", minerModel);

        ///act
        auto result = Schema_GetModelModelCount(model, &nModels);

        ///assert
        ASSERT_ARE_EQUAL(SCHEMA_RESULT, SCHEMA_OK, result);
        ASSERT_ARE_EQUAL(size_t, 2, nModels);

        ///cleanup
        Schema_Destroy(schemaHandle);
    }

    /*Tests_SRS_SCHEMA_99_169: [If any of the parameters is NULL, then the function shall return SCHEMA_INVALID_ARG.]*/
    TEST_FUNCTION(Schema_GetModelModelCount_with_invalid_arg_fail_1)
    {
        ///arrange
        size_t nModels = 444;

        ///act
        auto result = Schema_GetModelModelCount(NULL, &nModels);

        ///assert
        ASSERT_ARE_EQUAL(SCHEMA_RESULT, SCHEMA_INVALID_ARG, result);

    }

    /*Tests_SRS_SCHEMA_99_169: [If any of the parameters is NULL, then the function shall return SCHEMA_INVALID_ARG.]*/
    TEST_FUNCTION(Schema_GetModelModelCount_with_invalid_arg_fail_2)
    {
        ///arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_MODEL_TYPE_HANDLE model = Schema_CreateModelType(schemaHandle, "someModel");
        SCHEMA_MODEL_TYPE_HANDLE minerModel = Schema_CreateModelType(schemaHandle, "someMinerModel");
        (void)Schema_AddModelModel(model, "ManicMiner", minerModel);
        (void)Schema_AddModelModel(model, "ManicMiner2", minerModel);

        ///act
        auto result = Schema_GetModelModelCount(model, NULL);

        ///assert
        ASSERT_ARE_EQUAL(SCHEMA_RESULT, SCHEMA_INVALID_ARG, result);

        ///cleanup
        Schema_Destroy(schemaHandle);
    }

    /*Tests_SRS_SCHEMA_99_170: [Schema_GetModelModelByName shall return a handle to the model identified by the property with the name propertyName in the model identified by the handle modelTypeHandle.]*/
    /*Tests_SRS_SCHEMA_99_171: [If Schema_GetModelModelByName is unable to provide the handle it shall return NULL.]*/
    TEST_FUNCTION(Schema_GetModelModelByName_succeeds)
    {
        ///arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_MODEL_TYPE_HANDLE model = Schema_CreateModelType(schemaHandle, "someModel");
        SCHEMA_MODEL_TYPE_HANDLE minerModel = Schema_CreateModelType(schemaHandle, "someMinerModel");
        (void)Schema_AddModelModel(model, "ManicMiner", minerModel);
        (void)Schema_AddModelModel(model, "ManicMiner2", minerModel);

        ///act
        auto result1 = Schema_GetModelModelByName(model, "ManicMiner");
        auto result2 = Schema_GetModelModelByName(model, "ManicMiner2");
        auto result_intruder = Schema_GetModelModelByName(model, "INTRUDER_ALERT");

        ///assert
        ASSERT_IS_NOT_NULL(result1);
        ASSERT_IS_NOT_NULL(result2);
        ASSERT_IS_NULL(result_intruder);

        ///cleanup
        Schema_Destroy(schemaHandle);
    }

    /*Tests_SRS_SCHEMA_99_170: [Schema_GetModelModelByName shall return a handle to the model identified by the property with the name propertyName in the model identified by the handle modelTypeHandle.]*/
    TEST_FUNCTION(Schema_GetModelModelByName_fails_with_NULL_parameters)
    {
        ///arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_MODEL_TYPE_HANDLE model = Schema_CreateModelType(schemaHandle, "someModel");
        SCHEMA_MODEL_TYPE_HANDLE minerModel = Schema_CreateModelType(schemaHandle, "someMinerModel");
        (void)Schema_AddModelModel(model, "ManicMiner", minerModel);
        (void)Schema_AddModelModel(model, "ManicMiner2", minerModel);

        ///act
        auto result1 = Schema_GetModelModelByName(NULL, "ManicMiner");
        auto result2 = Schema_GetModelModelByName(model, NULL);

        ///assert
        ASSERT_IS_NULL(result1);
        ASSERT_IS_NULL(result2);

        ///cleanup
        Schema_Destroy(schemaHandle);
    }

    /*Tests_SRS_SCHEMA_99_172: [ Schema_GetModelModelyByIndex shall return a handle to the "index"th model inserted in the model identified by the parameter modelTypeHandle.]*/
    /*Tests_SRS_SCHEMA_99_173: [Schema_GetModelModelyByIndex shall return NULL in the cases when it cannot provide the handle.]*/
    TEST_FUNCTION(Schema_GetModelModelByIndex_succeeds)
    {
        ///arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_MODEL_TYPE_HANDLE model = Schema_CreateModelType(schemaHandle, "someModel");
        SCHEMA_MODEL_TYPE_HANDLE minerModel = Schema_CreateModelType(schemaHandle, "someMinerModel");
        (void)Schema_AddModelModel(model, "ManicMiner", minerModel);
        (void)Schema_AddModelModel(model, "ManicMiner2", minerModel);

        ///act
        auto result1 = Schema_GetModelModelyByIndex(model, 0);
        auto result2 = Schema_GetModelModelyByIndex(model, 1);
        auto result_intruder1 = Schema_GetModelModelyByIndex(model, 2);
        auto result_intruder2 = Schema_GetModelModelyByIndex(NULL, 0);

        ///assert
        ASSERT_IS_NOT_NULL(result1); 
        ASSERT_ARE_EQUAL(char_ptr, "someMinerModel", Schema_GetModelName(result1));
        ASSERT_IS_NOT_NULL(result2);
        ASSERT_ARE_EQUAL(char_ptr, "someMinerModel", Schema_GetModelName(result2));
        
        ASSERT_IS_NULL(result_intruder1);
        ASSERT_IS_NULL(result_intruder2);

        ///cleanup
        Schema_Destroy(schemaHandle);
    }

    /*Tests_SRS_SCHEMA_99_175: [Schema_GetModelModelPropertyNameByIndex shall return the name of the property for the "index"th model in the model identified by modelTypeHandle parameter.]*/
    /*Tests_SRS_SCHEMA_99_176: [If Schema_GetModelModelPropertyNameByIndex cannot produce the property name, it shall return NULL.]*/
    TEST_FUNCTION(Schema_GetModelModelPropertyNameByIndex_succeeds)
    {
        ///arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_MODEL_TYPE_HANDLE model = Schema_CreateModelType(schemaHandle, "someModel");
        SCHEMA_MODEL_TYPE_HANDLE minerModel = Schema_CreateModelType(schemaHandle, "someMinerModel");
        (void)Schema_AddModelModel(model, "ManicMiner", minerModel);
        (void)Schema_AddModelModel(model, "ManicMiner2", minerModel);

        ///act
        auto result1 = Schema_GetModelModelPropertyNameByIndex(model, 0);
        auto result2 = Schema_GetModelModelPropertyNameByIndex(model, 1);
        auto result_intruder1 = Schema_GetModelModelPropertyNameByIndex(model, 2);
        auto result_intruder2 = Schema_GetModelModelPropertyNameByIndex(NULL, 0);

        ///assert
        ASSERT_ARE_EQUAL(char_ptr, "ManicMiner", result1);
        ASSERT_ARE_EQUAL(char_ptr, "ManicMiner2", result2);

        ASSERT_IS_NULL(result_intruder1);
        ASSERT_IS_NULL(result_intruder2);

        ///cleanup
        Schema_Destroy(schemaHandle);
    }

    /*Tests_SRS_SCHEMA_99_163: [Schema_AddModelModel shall insert an existing model, identified by the handle modelType, into the existing model identified by modelTypeHandle under a property having the name propertyName.]*/
    /*Tests_SRS_SCHEMA_99_164: [If the function succeeds, then the return value shall be SCHEMA_OK.]*/
    TEST_FUNCTION(Schema_AddModelModel_model_in_mode_in_model_succeeds)
    {
        ///arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_MODEL_TYPE_HANDLE bigModel = Schema_CreateModelType(schemaHandle, "someBigModel");
        SCHEMA_MODEL_TYPE_HANDLE mediumModel = Schema_CreateModelType(schemaHandle, "someMediumModel");
        SCHEMA_MODEL_TYPE_HANDLE smallModel = Schema_CreateModelType(schemaHandle, "someSmallModel");


        ///act
        auto result1 = Schema_AddModelModel(mediumModel, "theSmallModel", smallModel);
        auto result2 = Schema_AddModelModel(bigModel, "theMediumModem", mediumModel);

        ///assert
        ASSERT_ARE_EQUAL(SCHEMA_RESULT, SCHEMA_OK, result1);
        ASSERT_ARE_EQUAL(SCHEMA_RESULT, SCHEMA_OK, result2);

        size_t nModelsInBigModel = 444;
        size_t nModelsInMediumModel = 444;
        size_t nModelsInSmallModel = 444;
        (void)Schema_GetModelModelCount(bigModel, &nModelsInBigModel);
        (void)Schema_GetModelModelCount(mediumModel, &nModelsInMediumModel);
        (void)Schema_GetModelModelCount(smallModel, &nModelsInSmallModel);
        ASSERT_ARE_EQUAL(size_t, 1, nModelsInBigModel);
        ASSERT_ARE_EQUAL(size_t, 1, nModelsInMediumModel);
        ASSERT_ARE_EQUAL(size_t, 0, nModelsInSmallModel);

        ASSERT_ARE_EQUAL(char_ptr, "theMediumModem", Schema_GetModelModelPropertyNameByIndex(bigModel, 0));
        ASSERT_ARE_EQUAL(char_ptr, "theSmallModel", Schema_GetModelModelPropertyNameByIndex(mediumModel, 0));


        ///cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Schema_ModelPropertyByPathExists */

    /* Tests_SRS_SCHEMA_99_180: [If any of the arguments are NULL, Schema_ModelPropertyByPathExists shall return false.] */
    TEST_FUNCTION(Schema_ModelPropertyByPathExists_With_NULL_ModelHandle_Fails)
    {
        ///arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_MODEL_TYPE_HANDLE bigModel = Schema_CreateModelType(schemaHandle, "someBigModel");
        (void)Schema_AddModelProperty(bigModel, "propertyName", "type");

        ///act
        bool result = Schema_ModelPropertyByPathExists(NULL, "propertyName");

        ///assert
        ASSERT_IS_FALSE(result);

        ///cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests_SRS_SCHEMA_99_180: [If any of the arguments are NULL, Schema_ModelPropertyByPathExists shall return false.] */
    TEST_FUNCTION(Schema_ModelPropertyByPathExists_With_NULL_PropertyPath_Fails)
    {
        ///arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_MODEL_TYPE_HANDLE bigModel = Schema_CreateModelType(schemaHandle, "someBigModel");
        (void)Schema_AddModelProperty(bigModel, "propertyName", "type");

        ///act
        bool result = Schema_ModelPropertyByPathExists(bigModel, NULL);

        ///assert
        ASSERT_IS_FALSE(result);

        ///cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests_SRS_SCHEMA_99_177: [Schema_ModelPropertyByPathExists shall return true if a leaf property exists in the model modelTypeHandle.] */
    /* Tests_SRS_SCHEMA_99_178: [The argument propertyPath shall be used to find the leaf property.] */
    /* Tests_SRS_SCHEMA_99_179: [The propertyPath shall be assumed to be in the format model1/model2/.../propertyName.] */
    TEST_FUNCTION(Schema_When_Property_Is_Found_At_Root_Schema_ModelPropertyByPathExists_Returns_True)
    {
        ///arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_MODEL_TYPE_HANDLE bigModel = Schema_CreateModelType(schemaHandle, "someBigModel");
        (void)Schema_AddModelProperty(bigModel, "propertyName", "type");

        ///act
        bool result = Schema_ModelPropertyByPathExists(bigModel, "propertyName");

        ///assert
        ASSERT_IS_TRUE(result);

        ///cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests_SRS_SCHEMA_99_181: [If the property cannot be found Schema_ModelPropertyByPathExists shall return false.] */
    TEST_FUNCTION(Schema_When_Property_Is_Not_Found_At_Root_Schema_ModelPropertyByPathExists_Returns_False)
    {
        ///arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_MODEL_TYPE_HANDLE bigModel = Schema_CreateModelType(schemaHandle, "someBigModel");
        (void)Schema_AddModelProperty(bigModel, "propertyName", "type");

        ///act
        bool result = Schema_ModelPropertyByPathExists(bigModel, "p");

        ///assert
        ASSERT_IS_FALSE(result);

        ///cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests_SRS_SCHEMA_99_177: [Schema_ModelPropertyByPathExists shall return true if a leaf property exists in the model modelTypeHandle.] */
    /* Tests_SRS_SCHEMA_99_178: [The argument propertyPath shall be used to find the leaf property.] */
    /* Tests_SRS_SCHEMA_99_179: [The propertyPath shall be assumed to be in the format model1/model2/.../propertyName.] */
    TEST_FUNCTION(Schema_When_Property_Is_Found_In_A_Child_Model_Schema_ModelPropertyByPathExists_Returns_True)
    {
        ///arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_MODEL_TYPE_HANDLE bigModel = Schema_CreateModelType(schemaHandle, "someBigModel");
        SCHEMA_MODEL_TYPE_HANDLE mediumModel = Schema_CreateModelType(schemaHandle, "someMediumModel");
        SCHEMA_MODEL_TYPE_HANDLE smallModel = Schema_CreateModelType(schemaHandle, "someSmallModel");
        (void)Schema_AddModelModel(mediumModel, "theSmallModel", smallModel);
        (void)Schema_AddModelModel(bigModel, "theMediumModel", mediumModel);
        (void)Schema_AddModelProperty(mediumModel, "propertyName", "type");

        ///act
        bool result = Schema_ModelPropertyByPathExists(bigModel, "theMediumModel/propertyName");

        ///assert
        ASSERT_IS_TRUE(result);

        ///cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests_SRS_SCHEMA_99_181: [If the property cannot be found Schema_ModelPropertyByPathExists shall return false.] */
    TEST_FUNCTION(Schema_When_Property_Is_Not_Found_In_A_Child_Model_Schema_ModelPropertyByPathExists_Returns_True)
    {
        ///arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_MODEL_TYPE_HANDLE bigModel = Schema_CreateModelType(schemaHandle, "someBigModel");
        SCHEMA_MODEL_TYPE_HANDLE mediumModel = Schema_CreateModelType(schemaHandle, "someMediumModel");
        SCHEMA_MODEL_TYPE_HANDLE smallModel = Schema_CreateModelType(schemaHandle, "someSmallModel");
        (void)Schema_AddModelModel(mediumModel, "theSmallModel", smallModel);
        (void)Schema_AddModelModel(bigModel, "theMediumModel", mediumModel);
        (void)Schema_AddModelProperty(mediumModel, "propertyName", "type");

        ///act
        bool result = Schema_ModelPropertyByPathExists(bigModel, "theMediumModel/p");

        ///assert
        ASSERT_IS_FALSE(result);

        ///cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests_SRS_SCHEMA_99_177: [Schema_ModelPropertyByPathExists shall return true if a leaf property exists in the model modelTypeHandle.] */
    /* Tests_SRS_SCHEMA_99_178: [The argument propertyPath shall be used to find the leaf property.] */
    /* Tests_SRS_SCHEMA_99_179: [The propertyPath shall be assumed to be in the format model1/model2/.../propertyName.] */
    TEST_FUNCTION(Schema_When_Property_Is_Found_In_A_2nd_Level_Child_Model_Schema_ModelPropertyByPathExists_Returns_True)
    {
        ///arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_MODEL_TYPE_HANDLE bigModel = Schema_CreateModelType(schemaHandle, "someBigModel");
        SCHEMA_MODEL_TYPE_HANDLE mediumModel = Schema_CreateModelType(schemaHandle, "someMediumModel");
        SCHEMA_MODEL_TYPE_HANDLE smallModel = Schema_CreateModelType(schemaHandle, "someSmallModel");
        (void)Schema_AddModelModel(mediumModel, "theSmallModel", smallModel);
        (void)Schema_AddModelModel(bigModel, "theMediumModel", mediumModel);
        (void)Schema_AddModelProperty(smallModel, "propertyName", "type");

        ///act
        bool result = Schema_ModelPropertyByPathExists(bigModel, "theMediumModel/theSmallModel/propertyName");

        ///assert
        ASSERT_IS_TRUE(result);

        ///cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests_SRS_SCHEMA_99_181: [If the property cannot be found Schema_ModelPropertyByPathExists shall return false.] */
    TEST_FUNCTION(Schema_When_Property_Is_Not_Found_In_A_2nd_Level_Child_Model_Schema_ModelPropertyByPathExists_Returns_True)
    {
        ///arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_MODEL_TYPE_HANDLE bigModel = Schema_CreateModelType(schemaHandle, "someBigModel");
        SCHEMA_MODEL_TYPE_HANDLE mediumModel = Schema_CreateModelType(schemaHandle, "someMediumModel");
        SCHEMA_MODEL_TYPE_HANDLE smallModel = Schema_CreateModelType(schemaHandle, "someSmallModel");
        (void)Schema_AddModelModel(mediumModel, "theSmallModel", smallModel);
        (void)Schema_AddModelModel(bigModel, "theMediumModel", mediumModel);
        (void)Schema_AddModelProperty(smallModel, "propertyName", "type");

        ///act
        bool result = Schema_ModelPropertyByPathExists(bigModel, "theMediumModel/theSmallModel/p");

        ///assert
        ASSERT_IS_FALSE(result);

        ///cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests_SRS_SCHEMA_99_181: [If the property cannot be found Schema_ModelPropertyByPathExists shall return false.] */
    TEST_FUNCTION(Schema_When_A_ModelName_Is_Not_Fount_Schema_ModelPropertyByPathExists_Fails)
    {
        ///arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_MODEL_TYPE_HANDLE bigModel = Schema_CreateModelType(schemaHandle, "someBigModel");
        SCHEMA_MODEL_TYPE_HANDLE mediumModel = Schema_CreateModelType(schemaHandle, "someMediumModel");
        SCHEMA_MODEL_TYPE_HANDLE smallModel = Schema_CreateModelType(schemaHandle, "someSmallModel");
        (void)Schema_AddModelModel(mediumModel, "theSmallModel", smallModel);
        (void)Schema_AddModelModel(bigModel, "theMediumModel", mediumModel);
        (void)Schema_AddModelProperty(smallModel, "propertyName", "type");

        ///act
        bool result = Schema_ModelPropertyByPathExists(bigModel, "theMediumModel/small/propertyName");

        ///assert
        ASSERT_IS_FALSE(result);

        ///cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests_SRS_SCHEMA_99_181: [If the property cannot be found Schema_ModelPropertyByPathExists shall return false.] */
    TEST_FUNCTION(Schema_When_A_ModelName_Is_Only_A_Partial_Match_Schema_ModelPropertyByPathExists_Fails)
    {
        ///arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_MODEL_TYPE_HANDLE bigModel = Schema_CreateModelType(schemaHandle, "someBigModel");
        SCHEMA_MODEL_TYPE_HANDLE mediumModel = Schema_CreateModelType(schemaHandle, "someMediumModel");
        SCHEMA_MODEL_TYPE_HANDLE smallModel = Schema_CreateModelType(schemaHandle, "someSmallModel");
        (void)Schema_AddModelModel(mediumModel, "theSmallModel", smallModel);
        (void)Schema_AddModelModel(bigModel, "theMediumModel", mediumModel);
        (void)Schema_AddModelProperty(smallModel, "propertyName", "type");

        ///act
        bool result = Schema_ModelPropertyByPathExists(bigModel, "theMediumModel/theSmallMode/propertyName");

        ///assert
        ASSERT_IS_FALSE(result);

        ///cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests_SRS_SCHEMA_99_181: [If the property cannot be found Schema_ModelPropertyByPathExists shall return false.] */
    TEST_FUNCTION(Schema_When_An_Empty_Model_Name_Is_In_The_Path_Schema_ModelPropertyByPathExists_Fails)
    {
        ///arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_MODEL_TYPE_HANDLE bigModel = Schema_CreateModelType(schemaHandle, "someBigModel");
        SCHEMA_MODEL_TYPE_HANDLE mediumModel = Schema_CreateModelType(schemaHandle, "someMediumModel");
        SCHEMA_MODEL_TYPE_HANDLE smallModel = Schema_CreateModelType(schemaHandle, "someSmallModel");
        (void)Schema_AddModelModel(mediumModel, "theSmallModel", smallModel);
        (void)Schema_AddModelModel(bigModel, "theMediumModel", mediumModel);
        (void)Schema_AddModelProperty(smallModel, "propertyName", "type");

        ///act
        bool result = Schema_ModelPropertyByPathExists(bigModel, "theMediumModel//propertyName");

        ///assert
        ASSERT_IS_FALSE(result);

        ///cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests_SRS_SCHEMA_99_181: [If the property cannot be found Schema_ModelPropertyByPathExists shall return false.] */
    TEST_FUNCTION(Schema_When_The_Path_Is_An_Empty_String_Schema_ModelPropertyByPathExists_Fails)
    {
        ///arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_MODEL_TYPE_HANDLE bigModel = Schema_CreateModelType(schemaHandle, "someBigModel");
        SCHEMA_MODEL_TYPE_HANDLE mediumModel = Schema_CreateModelType(schemaHandle, "someMediumModel");
        SCHEMA_MODEL_TYPE_HANDLE smallModel = Schema_CreateModelType(schemaHandle, "someSmallModel");
        (void)Schema_AddModelModel(mediumModel, "theSmallModel", smallModel);
        (void)Schema_AddModelModel(bigModel, "theMediumModel", mediumModel);
        (void)Schema_AddModelProperty(smallModel, "propertyName", "type");

        ///act
        bool result = Schema_ModelPropertyByPathExists(bigModel, "");

        ///assert
        ASSERT_IS_FALSE(result);

        ///cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests_SRS_SCHEMA_99_181: [If the property cannot be found Schema_ModelPropertyByPathExists shall return false.] */
    TEST_FUNCTION(Schema_When_The_Path_Is_A_Slash_Schema_ModelPropertyByPathExists_Fails)
    {
        ///arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_MODEL_TYPE_HANDLE bigModel = Schema_CreateModelType(schemaHandle, "someBigModel");
        SCHEMA_MODEL_TYPE_HANDLE mediumModel = Schema_CreateModelType(schemaHandle, "someMediumModel");
        SCHEMA_MODEL_TYPE_HANDLE smallModel = Schema_CreateModelType(schemaHandle, "someSmallModel");
        (void)Schema_AddModelModel(mediumModel, "theSmallModel", smallModel);
        (void)Schema_AddModelModel(bigModel, "theMediumModel", mediumModel);
        (void)Schema_AddModelProperty(smallModel, "propertyName", "type");

        ///act
        bool result = Schema_ModelPropertyByPathExists(bigModel, "/");

        ///assert
        ASSERT_IS_FALSE(result);

        ///cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests_SRS_SCHEMA_99_182: [A single slash ('/') at the beginning of the path shall be ignored and the path shall still be valid.] */
    TEST_FUNCTION(Schema_When_The_First_Slash_In_The_Path_With_Only_A_PropertyName_Is_Ignored)
    {
        ///arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_MODEL_TYPE_HANDLE bigModel = Schema_CreateModelType(schemaHandle, "someBigModel");
        SCHEMA_MODEL_TYPE_HANDLE mediumModel = Schema_CreateModelType(schemaHandle, "someMediumModel");
        (void)Schema_AddModelModel(bigModel, "theMediumModel", mediumModel);
        (void)Schema_AddModelProperty(bigModel, "propertyName", "type");

        ///act
        bool result = Schema_ModelPropertyByPathExists(bigModel, "/propertyName");

        ///assert
        ASSERT_IS_TRUE(result);

        ///cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests_SRS_SCHEMA_99_182: [A single slash ('/') at the beginning of the path shall be ignored and the path shall still be valid.] */
    TEST_FUNCTION(Schema_When_The_First_Slash_In_The_Path_With_A_ModelName_And_A_PropertyName_Is_Ignored)
    {
        ///arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_MODEL_TYPE_HANDLE bigModel = Schema_CreateModelType(schemaHandle, "someBigModel");
        SCHEMA_MODEL_TYPE_HANDLE mediumModel = Schema_CreateModelType(schemaHandle, "someMediumModel");
        (void)Schema_AddModelModel(bigModel, "theMediumModel", mediumModel);
        (void)Schema_AddModelProperty(mediumModel, "propertyName", "type");

        ///act
        bool result = Schema_ModelPropertyByPathExists(bigModel, "/theMediumModel/propertyName");

        ///assert
        ASSERT_IS_TRUE(result);

        ///cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Tests_SRS_SCHEMA_99_183: [If the path propertyPathpoints to a sub-model, Schema_ModelPropertyByPathExists shall return true.] */
    TEST_FUNCTION(Schema_When_The_Path_Points_To_A_Model_Schema_ModelPropertyByPathExists_returns_true)
    {
        ///arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_MODEL_TYPE_HANDLE bigModel = Schema_CreateModelType(schemaHandle, "someBigModel");
        SCHEMA_MODEL_TYPE_HANDLE mediumModel = Schema_CreateModelType(schemaHandle, "someMediumModel");
        (void)Schema_AddModelModel(bigModel, "theMediumModel", mediumModel);
        (void)Schema_AddModelProperty(mediumModel, "propertyName", "type");

        ///act
        bool result = Schema_ModelPropertyByPathExists(bigModel, "theMediumModel");

        ///assert
        ASSERT_IS_TRUE(result);

        ///cleanup
        Schema_Destroy(schemaHandle);
    }

    /* Test_SRS_SCHEMA_07_187: [Schema_AddDeviceRef shall return SCHEMA_INVALID_ARG if schemaHandle is NULL.] */
    TEST_FUNCTION(Schema_AddDeviceRef_NULL_SCHEMA_HANDLE_Fail)
    {
        ///arrange
        SCHEMA_RESULT result;

        ///act
        result = Schema_AddDeviceRef(NULL);

        ///assert
        ASSERT_ARE_EQUAL(SCHEMA_RESULT, SCHEMA_INVALID_ARG, result);
    }

    TEST_FUNCTION(Schema_AddDeviceRef_SUCCEED)
    {
        ///arrange
        SCHEMA_RESULT result;
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_MODEL_TYPE_HANDLE bigModel = Schema_CreateModelType(schemaHandle, "someBigModel");
        SCHEMA_MODEL_TYPE_HANDLE mediumModel = Schema_CreateModelType(schemaHandle, "someMediumModel");
        (void)Schema_AddModelModel(bigModel, "theMediumModel", mediumModel);
        (void)Schema_AddModelProperty(mediumModel, "propertyName", "type");

        ///act
        result = Schema_AddDeviceRef(bigModel);

        ///assert
        ASSERT_ARE_EQUAL(SCHEMA_RESULT, SCHEMA_OK, result);

        ///cleanup
        Schema_Destroy(schemaHandle);
    }

    TEST_FUNCTION(Schema_ReleaseDeviceRef_NULL_SCHEMA_MODEL_TYPE_HANDLE_Fail)
    {
        ///arrange

        ///act
        SCHEMA_RESULT result = Schema_ReleaseDeviceRef(NULL);

        ///assert
        ASSERT_ARE_EQUAL(SCHEMA_RESULT, SCHEMA_INVALID_ARG, result);
    }

    TEST_FUNCTION(Schema_ReleaseDeviceRef_To_Call_Fail)
    {
        ///arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_MODEL_TYPE_HANDLE bigModel = Schema_CreateModelType(schemaHandle, "someBigModel");
        SCHEMA_MODEL_TYPE_HANDLE mediumModel = Schema_CreateModelType(schemaHandle, "someMediumModel");
        (void)Schema_AddModelModel(bigModel, "theMediumModel", mediumModel);
        (void)Schema_AddModelProperty(mediumModel, "propertyName", "type");

        SCHEMA_RESULT result = Schema_AddDeviceRef(bigModel);
        ASSERT_ARE_EQUAL(SCHEMA_RESULT, SCHEMA_OK, result);

        ///act
        result = Schema_ReleaseDeviceRef(bigModel);
        ASSERT_ARE_EQUAL(SCHEMA_RESULT, SCHEMA_OK, result);

        result = Schema_ReleaseDeviceRef(bigModel);
        ASSERT_ARE_EQUAL(SCHEMA_RESULT, SCHEMA_DEVICE_COUNT_ZERO, result);

        ///assert
        result = Schema_DestroyIfUnused(bigModel);
        ASSERT_ARE_EQUAL(SCHEMA_RESULT, SCHEMA_OK, result);
    }

    TEST_FUNCTION(Schema_ReleaseDeviceRef_Succeed)
    {
        ///arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_MODEL_TYPE_HANDLE bigModel = Schema_CreateModelType(schemaHandle, "someBigModel");
        SCHEMA_MODEL_TYPE_HANDLE mediumModel = Schema_CreateModelType(schemaHandle, "someMediumModel");
        (void)Schema_AddModelModel(bigModel, "theMediumModel", mediumModel);
        (void)Schema_AddModelProperty(mediumModel, "propertyName", "type");

        SCHEMA_RESULT result = Schema_AddDeviceRef(bigModel);
        ASSERT_ARE_EQUAL(SCHEMA_RESULT, SCHEMA_OK, result);

        ///act
        result = Schema_ReleaseDeviceRef(bigModel);
        ASSERT_ARE_EQUAL(SCHEMA_RESULT, SCHEMA_OK, result);

        ///assert
        result = Schema_DestroyIfUnused(bigModel);
        ASSERT_ARE_EQUAL(SCHEMA_RESULT, SCHEMA_OK, result);
    }

    TEST_FUNCTION(Schema_ReleaseDeviceRef_2_Device_Addition_Succeed)
    {
        ///arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_MODEL_TYPE_HANDLE bigModel = Schema_CreateModelType(schemaHandle, "someBigModel");
        SCHEMA_MODEL_TYPE_HANDLE mediumModel = Schema_CreateModelType(schemaHandle, "someMediumModel");
        (void)Schema_AddModelModel(bigModel, "theMediumModel", mediumModel);
        (void)Schema_AddModelProperty(mediumModel, "propertyName", "type");

        SCHEMA_RESULT result = Schema_AddDeviceRef(bigModel);
        ASSERT_ARE_EQUAL(SCHEMA_RESULT, SCHEMA_OK, result);

        result = Schema_AddDeviceRef(bigModel);
        ASSERT_ARE_EQUAL(SCHEMA_RESULT, SCHEMA_OK, result);

        ///act
        result = Schema_ReleaseDeviceRef(bigModel);
        ASSERT_ARE_EQUAL(SCHEMA_RESULT, SCHEMA_OK, result);

        // Second Delete
        result = Schema_ReleaseDeviceRef(bigModel);
        ASSERT_ARE_EQUAL(SCHEMA_RESULT, SCHEMA_OK, result);

        ///assert
        result = Schema_DestroyIfUnused(bigModel);
        ASSERT_ARE_EQUAL(SCHEMA_RESULT, SCHEMA_OK, result);
    }

    TEST_FUNCTION(Schema_DestroyIfUnused_SCHEMA_MODEL_TYPE_HANDLE_Fail)
    {
        ///arrange
        SCHEMA_RESULT result;

        ///act
        result = Schema_DestroyIfUnused(NULL);

        ///assert
        ASSERT_ARE_EQUAL(SCHEMA_RESULT, SCHEMA_INVALID_ARG, result);
    }

    TEST_FUNCTION(Schema_DestroyIfUnused_Succeed)
    {
        ///arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_MODEL_TYPE_HANDLE bigModel = Schema_CreateModelType(schemaHandle, "someBigModel");
        SCHEMA_MODEL_TYPE_HANDLE mediumModel = Schema_CreateModelType(schemaHandle, "someMediumModel");
        (void)Schema_AddModelModel(bigModel, "theMediumModel", mediumModel);
        (void)Schema_AddModelProperty(mediumModel, "propertyName", "type");

        SCHEMA_RESULT result = Schema_AddDeviceRef(bigModel);
        ASSERT_ARE_EQUAL(SCHEMA_RESULT, SCHEMA_OK, result);

        ///act
        result = Schema_ReleaseDeviceRef(bigModel);
        ASSERT_ARE_EQUAL(SCHEMA_RESULT, SCHEMA_OK, result);

        result = Schema_DestroyIfUnused(bigModel);

        ///assert
        ASSERT_ARE_EQUAL(SCHEMA_RESULT, SCHEMA_OK, result);
    }

    TEST_FUNCTION(Schema_DestroyUnused_2_Calls_Succeed)
    {
        ///arrange
        SCHEMA_HANDLE schemaHandle = Schema_Create(SCHEMA_NAMESPACE);
        SCHEMA_MODEL_TYPE_HANDLE bigModel = Schema_CreateModelType(schemaHandle, "someBigModel");
        SCHEMA_MODEL_TYPE_HANDLE mediumModel = Schema_CreateModelType(schemaHandle, "someMediumModel");
        (void)Schema_AddModelModel(bigModel, "theMediumModel", mediumModel);
        (void)Schema_AddModelProperty(mediumModel, "propertyName", "type");

        SCHEMA_RESULT result = Schema_AddDeviceRef(bigModel);
        ASSERT_ARE_EQUAL(SCHEMA_RESULT, SCHEMA_OK, result);

        result = Schema_AddDeviceRef(bigModel);
        ASSERT_ARE_EQUAL(SCHEMA_RESULT, SCHEMA_OK, result);

        ///act
        result = Schema_ReleaseDeviceRef(bigModel);
        ASSERT_ARE_EQUAL(SCHEMA_RESULT, SCHEMA_OK, result);

        result = Schema_DestroyIfUnused(bigModel);
        ASSERT_ARE_EQUAL(SCHEMA_RESULT, SCHEMA_MODEL_IN_USE, result);

        // Second Delete
        result = Schema_ReleaseDeviceRef(bigModel);
        ASSERT_ARE_EQUAL(SCHEMA_RESULT, SCHEMA_OK, result);

        result = Schema_DestroyIfUnused(bigModel);

        ///assert
        ASSERT_ARE_EQUAL(SCHEMA_RESULT, SCHEMA_OK, result);
    }

END_TEST_SUITE(Schema_ut)
