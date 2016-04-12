// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <cstdlib>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "testrunnerswitcher.h"
#include "micromock.h"
#include "micromockcharstararenullterminatedstrings.h"
// our test framework defines NO_ERROR, so does LWM2M.  
#ifdef NO_ERROR
#undef NO_ERROR
#endif 

extern "C" 
{
#include "iotdm_internal.h"
}

typedef struct _TestOject {
    uint16_t InstanceId;
    uint16_t ObjectId;
} TestObject;

BEGIN_TEST_SUITE(iotdm_dmlist_unittests)

    LIST_HANDLE objectlist = NULL;

    TEST_SUITE_INITIALIZE(TestClassInitialize)
    {
    }

    TEST_SUITE_CLEANUP(TestClassCleanup)
    {
    }

    TEST_FUNCTION_INITIALIZE(TestMethodInitialize)
    {
        (void)test_clear_dm_object_list(&objectlist, NULL);
    }

    TEST_FUNCTION_CLEANUP(TestMethodCleanup)
    {
    }

    TestObject *create_object(uint16_t ObjectId, uint16_t InstanceId)
    {
        TestObject *result = (TestObject*)lwm2m_malloc(sizeof(TestObject));
        if (result)
        {
            memset(result, 0, sizeof(*result));
            result->ObjectId = ObjectId;
            result->InstanceId = InstanceId;
        }
        return result;
    }

    TEST_FUNCTION(iotdm_dmlist_list_has_zero_entries_at_startup)
    {
        uint16_t count = get_total_dm_object_count(objectlist);

        ASSERT_IS_TRUE(count == 0);
    }

    TEST_FUNCTION(iotdm_dmlist_test_clear_dm_object_list_can_be_called_twice)
    {
        test_clear_dm_object_list(&objectlist, NULL);
        test_clear_dm_object_list(&objectlist, NULL);
    }

    TEST_FUNCTION(iotdm_dmlist_reate_object_succeeds)
    {
        TestObject *obj = create_object(1, 1);
        ASSERT_IS_NOT_NULL(obj);
        lwm2m_free(obj);
    }

    TEST_FUNCTION(iotdm_dmlist_add_dm_object_returns_success)
    {
        TestObject *obj = create_object(1, 1);
        IOTHUB_CLIENT_RESULT result = add_dm_object(&objectlist, obj);

        ASSERT_IS_TRUE(result == IOTHUB_CLIENT_OK);
    }

    TEST_FUNCTION(iotdm_dmlist_returns_correct_count_when_1_object_added)
    {
        TestObject *obj = create_object(1, 1);
        IOTHUB_CLIENT_RESULT result = add_dm_object(&objectlist, obj);

        uint16_t count = get_total_dm_object_count(objectlist);

        ASSERT_IS_TRUE(count == 1);
    }

    TEST_FUNCTION(iotdm_dmlist_add_2_void_objects_returns_success)
    {
        TestObject *obj = create_object(1, 1);
        IOTHUB_CLIENT_RESULT result = add_dm_object(&objectlist, obj);

        obj = create_object(1, 2);
        result = add_dm_object(&objectlist, obj);

        ASSERT_IS_TRUE(result == IOTHUB_CLIENT_OK);
    }

    TEST_FUNCTION(iotdm_dmlist_returns_correct_count_when_2_objects_added)
    {
        TestObject *obj = create_object(1, 1);
        IOTHUB_CLIENT_RESULT result = add_dm_object(&objectlist, obj);

        obj = create_object(1, 2);
        result = add_dm_object(&objectlist, obj);

        uint16_t count = get_total_dm_object_count(objectlist);

        ASSERT_IS_TRUE(count == 2);
    }

    TEST_FUNCTION(iotdm_dmlist_clear_object_list_resets_count_to_zero)
    {
        TestObject *obj = create_object(1, 1);
        IOTHUB_CLIENT_RESULT result = add_dm_object(&objectlist, obj);

        obj = create_object(1, 2);
        result = add_dm_object(&objectlist, obj);

        uint16_t count = get_total_dm_object_count(objectlist);

        ASSERT_IS_TRUE(count == 2);

        test_clear_dm_object_list(&objectlist, NULL);

        count = get_total_dm_object_count(objectlist);

        ASSERT_IS_TRUE(count == 0);
    }

    TEST_FUNCTION(iotdm_dmlist_getting_object_from_empty_list_returns_NULL)
    {
        TestObject *obj = NULL;
        IOTHUB_CLIENT_RESULT result = get_dm_object(objectlist, 2, (void**)&obj);

        ASSERT_IS_NULL(obj);
        ASSERT_IS_FALSE(result == IOTHUB_CLIENT_OK);
    }

    TEST_FUNCTION(iotdm_dmlist_getting_object_not_in_list_returns_NULL)
    {
        TestObject *obj = create_object(1, 1);
        IOTHUB_CLIENT_RESULT result = add_dm_object(&objectlist, obj);

        obj = NULL;
        result = get_dm_object(objectlist, 2, (void**)&obj);

        ASSERT_IS_NULL(obj);
        ASSERT_IS_FALSE(result == IOTHUB_CLIENT_OK);
    }


    TEST_FUNCTION(iotdm_dmlist_get_dm_object_returns_correct_value)
    {
        TestObject *obj = create_object(1, 1);
        IOTHUB_CLIENT_RESULT result = add_dm_object(&objectlist, obj);

        obj = create_object(1, 2);
        result = add_dm_object(&objectlist, obj);

        obj = NULL;
        result = get_dm_object(objectlist, 2, (void**)&obj);
        ASSERT_IS_TRUE(obj->ObjectId == 1);
        ASSERT_IS_TRUE(obj->InstanceId == 2);

        obj = NULL;
        result = get_dm_object(objectlist, 1, (void**)&obj);
        ASSERT_IS_TRUE(obj->ObjectId == 1);
        ASSERT_IS_TRUE(obj->InstanceId == 1);
    }



    END_TEST_SUITE(iotdm_dmlist_unittests)

