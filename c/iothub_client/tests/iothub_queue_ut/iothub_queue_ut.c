// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

void* my_gballoc_malloc(size_t size)
{
    void *result = malloc(size);
    return result;
}

void my_gballoc_free(void* ptr)
{
    free(ptr);
}

#include "testrunnerswitcher.h"
#include "azure_c_shared_utility/macro_utils.h"

#include "umock_c.h"
#include "umocktypes_charptr.h"
#include "umock_c_negative_tests.h"
#include "umocktypes.h"
#include "umocktypes_c.h"


#define ENABLE_MOCKS

#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/doublylinkedlist.h"

#undef ENABLE_MOCKS

#include "iothub_queue.h"

#ifdef __cplusplus
extern "C"
{
#endif
    void real_DList_InitializeListHead(PDLIST_ENTRY listHead);
    int real_DList_IsListEmpty(const PDLIST_ENTRY listHead);
    void real_DList_InsertTailList(PDLIST_ENTRY listHead, PDLIST_ENTRY listEntry);
    void real_DList_InsertHeadList(PDLIST_ENTRY listHead, PDLIST_ENTRY listEntry);
    void real_DList_AppendTailList(PDLIST_ENTRY listHead, PDLIST_ENTRY ListToAppend);
    int real_DList_RemoveEntryList(PDLIST_ENTRY listEntry);
    PDLIST_ENTRY real_DList_RemoveHeadList(PDLIST_ENTRY listHead);
#ifdef __cplusplus
}
#endif

static const IOTHUB_MESSAGE_LIST* TEST_IOTHUB_MESSAGE_ITEM = (IOTHUB_MESSAGE_LIST*)0x11;
static const IOTHUB_DEVICE_TWIN* TEST_IOTHUB_DEVICE_TWIN_ITEM = (IOTHUB_DEVICE_TWIN*)0x12;
static const IOTHUB_MESSAGE_LIST* TEST_IOTHUB_MESSAGE_ITEM_1 = (IOTHUB_MESSAGE_LIST*)0x13;
static const IOTHUB_DEVICE_TWIN* TEST_IOTHUB_DEVICE_TWIN_ITEM_1 = (IOTHUB_DEVICE_TWIN*)0x14;
static const IOTHUB_MESSAGE_LIST* TEST_IOTHUB_MESSAGE_ITEM_2 = (IOTHUB_MESSAGE_LIST*)0x15;
static const IOTHUB_DEVICE_TWIN* TEST_IOTHUB_DEVICE_TWIN_ITEM_2 = (IOTHUB_DEVICE_TWIN*)0x16;

static TEST_MUTEX_HANDLE g_testByTest;
static TEST_MUTEX_HANDLE g_dllByDll;

static size_t g_destroy_callback_called;

TEST_DEFINE_ENUM_TYPE(IOTHUB_QUEUE_RESULT, IOTHUB_QUEUE_RESULT_VALUES);
IMPLEMENT_UMOCK_C_ENUM_TYPE(IOTHUB_QUEUE_RESULT, IOTHUB_QUEUE_RESULT_VALUES);

DEFINE_ENUM_STRINGS(UMOCK_C_ERROR_CODE, UMOCK_C_ERROR_CODE_VALUES)

static void on_umock_c_error(UMOCK_C_ERROR_CODE error_code)
{
    char temp_str[256];
    (void)snprintf(temp_str, sizeof(temp_str), "umock_c reported error :%s", ENUM_TO_STRING(UMOCK_C_ERROR_CODE, error_code));
    ASSERT_FAIL(temp_str);
}

BEGIN_TEST_SUITE(iothubclient_queue_ut)

TEST_SUITE_INITIALIZE(TestClassInitialize)
{
    TEST_INITIALIZE_MEMORY_DEBUG(g_dllByDll);
    g_testByTest = TEST_MUTEX_CREATE();
    ASSERT_IS_NOT_NULL(g_testByTest);

    umock_c_init(on_umock_c_error);

    umocktypes_charptr_register_types();

    REGISTER_TYPE(IOTHUB_QUEUE_RESULT, IOTHUB_QUEUE_RESULT);

    REGISTER_UMOCK_ALIAS_TYPE(IOTHUB_QUEUE_HANDLE, void*);
    REGISTER_UMOCK_ALIAS_TYPE(PDLIST_ENTRY, void*);
    REGISTER_UMOCK_ALIAS_TYPE(const PDLIST_ENTRY, void*);
    REGISTER_UMOCK_ALIAS_TYPE(PDLIST_ENTRY, void*);

    REGISTER_GLOBAL_MOCK_HOOK(gballoc_malloc, my_gballoc_malloc);
    REGISTER_GLOBAL_MOCK_HOOK(gballoc_free, my_gballoc_free);

    REGISTER_GLOBAL_MOCK_HOOK(DList_InitializeListHead, real_DList_InitializeListHead);
    REGISTER_GLOBAL_MOCK_HOOK(DList_IsListEmpty, real_DList_IsListEmpty);
    REGISTER_GLOBAL_MOCK_HOOK(DList_InsertTailList, real_DList_InsertTailList);
    REGISTER_GLOBAL_MOCK_HOOK(DList_InsertHeadList, real_DList_InsertHeadList);
    REGISTER_GLOBAL_MOCK_HOOK(DList_AppendTailList, real_DList_AppendTailList);
    REGISTER_GLOBAL_MOCK_HOOK(DList_RemoveEntryList, real_DList_RemoveEntryList);
    REGISTER_GLOBAL_MOCK_HOOK(DList_RemoveHeadList, real_DList_RemoveHeadList);
}

TEST_SUITE_CLEANUP(TestClassCleanup)
{
    umock_c_deinit();

    TEST_MUTEX_DESTROY(g_testByTest);
    TEST_DEINITIALIZE_MEMORY_DEBUG(g_dllByDll);
}

TEST_FUNCTION_INITIALIZE(TestMethodInitialize)
{
    if (TEST_MUTEX_ACQUIRE(g_testByTest))
    {
        ASSERT_FAIL("our mutex is ABANDONED. Failure in test framework");
    }

    g_destroy_callback_called = 0;

    umock_c_reset_all_calls();
}

TEST_FUNCTION_CLEANUP(TestMethodCleanup)
{
    TEST_MUTEX_RELEASE(g_testByTest);
}

static void my_destroy_callback(void* queue_item)
{
    (void)queue_item;
    g_destroy_callback_called++;
}

static int should_skip_index(size_t current_index, const size_t skip_array[], size_t length)
{
    int result = 0;
    for (size_t index = 0; index < length; index++)
    {
        if (current_index == skip_array[index])
        {
            result = __LINE__;
            break;
        }
    }
    return result;
}

/* Tests_SRS_IOTHUB_QUEUE_07_002: [On Success IoTHubQueue_Create_Queue shall return a non-NULL handle that refers to a Queue.] */
TEST_FUNCTION(IoTHubQueue_Create_Queue_Succeed)
{
    //arrange
    STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG)).IgnoreArgument_size();
    STRICT_EXPECTED_CALL(DList_InitializeListHead(IGNORED_PTR_ARG)).IgnoreArgument_listHead();

    //act
    IOTHUB_QUEUE_HANDLE handle = IoTHubQueue_Create_Queue();

    //assert
    ASSERT_IS_NOT_NULL(handle);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    IoTHubQueue_Destroy_Queue(handle, NULL);
}

/* Tests_SRS_IOTHUB_QUEUE_07_001: [If an error is encountered IoTHubQueue_Create_Queue shall return NULL.] */
TEST_FUNCTION(IoTHubQueue_Create_Queue_fails)
{
    //arrange
    int negativeTestsInitResult = umock_c_negative_tests_init();
    ASSERT_ARE_EQUAL(int, 0, negativeTestsInitResult);

    STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG)).IgnoreArgument_size();
    STRICT_EXPECTED_CALL(DList_InitializeListHead(IGNORED_PTR_ARG)).IgnoreArgument_listHead();

    umock_c_negative_tests_snapshot();

    size_t calls_cannot_fail[] = { 1 };

    //act
    size_t count = umock_c_negative_tests_call_count();
    for (size_t index = 0; index < count; index++)
    {
        if (should_skip_index(index, calls_cannot_fail, sizeof(calls_cannot_fail)/sizeof(calls_cannot_fail[0])) != 0)
        {
            continue;
        }

        umock_c_negative_tests_reset();
        umock_c_negative_tests_fail_call(index);

        char tmp_msg[64];
        sprintf(tmp_msg, "IoTHubQueue_Create_Queue failure in test %zu/%zu", index, count);

        IOTHUB_QUEUE_HANDLE handle = IoTHubQueue_Create_Queue();

        //assert
        ASSERT_IS_NULL(handle);
    }

    //cleanup
    umock_c_negative_tests_deinit();
}

/* Codes_SRS_IOTHUB_QUEUE_07_003: [If handle is NULL then IoTHubQueue_Destroy_Queue shall do nothing.] */
TEST_FUNCTION(IoTHubQueue_Destroy_Queue_NULL_handle_fails)
{
    //arrange

    //act
    IoTHubQueue_Destroy_Queue(NULL, NULL);

    //assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
}

/* Tests_SRS_IOTHUB_QUEUE_07_004: [IoTHubQueue_Destroy_Queue shall only free memory allocated within this compilation unit.] */
TEST_FUNCTION(IoTHubQueue_Destroy_Queue_Empty_List_Succeed)
{
    //arrange
    IOTHUB_QUEUE_HANDLE handle = IoTHubQueue_Create_Queue();
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(DList_IsListEmpty(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    //act
    IoTHubQueue_Destroy_Queue(handle, my_destroy_callback);

    //assert
    ASSERT_ARE_EQUAL(size_t, 0, g_destroy_callback_called);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
}

/* Tests_SRS_IOTHUB_QUEUE_07_005: [If destroy_callback is not NULL, IoTHubQueue_Destroy_Queue shall call destroy_callback with the item to be deleted.] */
TEST_FUNCTION(IoTHubQueue_Destroy_Queue_nonEmpty_List_Succeed)
{
    //arrange
    IOTHUB_QUEUE_HANDLE handle = IoTHubQueue_Create_Queue();
    IOTHUB_QUEUE_RESULT queue_result = IoTHubQueue_Add_Item(handle, (void*)TEST_IOTHUB_MESSAGE_ITEM);
    ASSERT_ARE_EQUAL(IOTHUB_QUEUE_RESULT, IOTHUB_QUEUE_OK, queue_result);

    umock_c_reset_all_calls();

    //EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(DList_IsListEmpty(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(DList_RemoveHeadList(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
        .IgnoreArgument_ptr();
    STRICT_EXPECTED_CALL(DList_IsListEmpty(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
        .IgnoreArgument_ptr();

    //act
    IoTHubQueue_Destroy_Queue(handle, my_destroy_callback);

    //assert
    ASSERT_ARE_EQUAL(size_t, 1, g_destroy_callback_called);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
}

/* Tests_SRS_IOTHUB_QUEUE_07_006: [If handle or queue_item are NULL IoTHubQueue_Add_Item shall return IOTHUB_QUEUE_INVALID_ARG.] */
TEST_FUNCTION(IoTHubQueue_Add_Item_Queue_NULL_handle_fails)
{
    //arrange

    //act
    IOTHUB_QUEUE_RESULT queue_result = IoTHubQueue_Add_Item(NULL, (void*)TEST_IOTHUB_MESSAGE_ITEM);

    //assert
    ASSERT_ARE_NOT_EQUAL(IOTHUB_QUEUE_RESULT, IOTHUB_QUEUE_OK, queue_result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
}

/* Tests_SRS_IOTHUB_QUEUE_07_006: [If handle or queue_item are NULL IoTHubQueue_Add_Item shall return IOTHUB_QUEUE_INVALID_ARG.] */
TEST_FUNCTION(IoTHubQueue_Add_Item_Queue_NULL_queue_item_fails)
{
    //arrange
    IOTHUB_QUEUE_HANDLE handle = IoTHubQueue_Create_Queue();
    umock_c_reset_all_calls();

    //act
    IOTHUB_QUEUE_RESULT queue_result = IoTHubQueue_Add_Item(handle, NULL);

    //assert
    ASSERT_ARE_NOT_EQUAL(IOTHUB_QUEUE_RESULT, IOTHUB_QUEUE_OK, queue_result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    IoTHubQueue_Destroy_Queue(handle, NULL);
}

/* Tests_SRS_IOTHUB_QUEUE_07_007: [IoTHubQueue_Add_Item shall allocate an queue item and store the item into the queue.] */
/* Tests_SRS_IOTHUB_QUEUE_07_008: [On success IoTHubQueue_Add_Item shall return IOTHUB_QUEUE_OK.] */
TEST_FUNCTION(IoTHubQueue_Add_Item_succeed)
{
    //arrange
    IOTHUB_QUEUE_HANDLE handle = IoTHubQueue_Create_Queue();
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG)).IgnoreArgument_size();
    STRICT_EXPECTED_CALL(DList_InsertTailList(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument_listEntry()
        .IgnoreArgument_listHead();

    //act
    IOTHUB_QUEUE_RESULT queue_result = IoTHubQueue_Add_Item(handle, (void*)TEST_IOTHUB_DEVICE_TWIN_ITEM);

    //assert
    ASSERT_ARE_EQUAL(IOTHUB_QUEUE_RESULT, IOTHUB_QUEUE_OK, queue_result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    IoTHubQueue_Destroy_Queue(handle, NULL);
}

/* Tests_SRS_IOTHUB_QUEUE_07_009: [If any error is encountered IoTHubQueue_Add_Item shall return IOTHUB_QUEUE_ERROR.] */
TEST_FUNCTION(IoTHubQueue_Add_Item_fail)
{
    //arrange
    int negativeTestsInitResult = umock_c_negative_tests_init();
    ASSERT_ARE_EQUAL(int, 0, negativeTestsInitResult);

    IOTHUB_QUEUE_HANDLE handle = IoTHubQueue_Create_Queue();
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG)).IgnoreArgument_size();
    STRICT_EXPECTED_CALL(DList_InsertTailList(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument_listEntry()
        .IgnoreArgument_listHead();

    umock_c_negative_tests_snapshot();

    size_t calls_cannot_fail[] = { 1 };

    //act
    size_t count = umock_c_negative_tests_call_count();
    for (size_t index = 0; index < count; index++)
    {
        if (should_skip_index(index, calls_cannot_fail, sizeof(calls_cannot_fail)/sizeof(calls_cannot_fail[0])) != 0)
        {
            continue;
        }

        umock_c_negative_tests_reset();
        umock_c_negative_tests_fail_call(index);

        char tmp_msg[64];
        sprintf(tmp_msg, "IoTHubQueue_Create_Queue failure in test %zu/%zu", index, count);

        IOTHUB_QUEUE_RESULT queue_result = IoTHubQueue_Add_Item(handle, (void*)TEST_IOTHUB_DEVICE_TWIN_ITEM);

        //assert
        ASSERT_ARE_NOT_EQUAL(IOTHUB_QUEUE_RESULT, IOTHUB_QUEUE_OK, queue_result);
    }

    //cleanup
    IoTHubQueue_Destroy_Queue(handle, NULL);
    umock_c_negative_tests_deinit();
}

/* Tests_SRS_IOTHUB_QUEUE_07_010: [If handle is NULL IoTHubQueue_Get_Queue_Item shall return NULL.] */
TEST_FUNCTION(IoTHubQueue_Get_Queue_Item_NULL_handle_fail)
{
    //arrange

    //act
    const void* client_queue_item = IoTHubQueue_Get_Queue_Item(NULL);

    //assert
    ASSERT_IS_NULL(client_queue_item);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
}

/* Tests_SRS_IOTHUB_QUEUE_07_012: [If the IOTHUB_QUEUE_ITEM is retrieved from the queue IoTHubQueue_Get_Queue_Item shall return a const IOTHUB_QUEUE_ITEM.] */
TEST_FUNCTION(IoTHubQueue_Get_Queue_Item_succeed)
{
    //arrange
    IOTHUB_QUEUE_HANDLE handle = IoTHubQueue_Create_Queue();
    IOTHUB_QUEUE_RESULT queue_result = IoTHubQueue_Add_Item(handle, (void*)TEST_IOTHUB_DEVICE_TWIN_ITEM);
    ASSERT_ARE_EQUAL(IOTHUB_QUEUE_RESULT, IOTHUB_QUEUE_OK, queue_result);
    umock_c_reset_all_calls();

    //act
    const void* client_queue_item = IoTHubQueue_Get_Queue_Item(handle);

    //assert
    ASSERT_ARE_EQUAL(void_ptr, TEST_IOTHUB_DEVICE_TWIN_ITEM, (IOTHUB_DEVICE_TWIN*)client_queue_item);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    IoTHubQueue_Destroy_Queue(handle, NULL);
}

/* Tests_SRS_IOTHUB_QUEUE_07_012: [If the IOTHUB_QUEUE_ITEM is retrieved from the queue IoTHubQueue_Get_Queue_Item shall return a const IOTHUB_QUEUE_ITEM.] */
TEST_FUNCTION(IoTHubQueue_Get_Queue_Item_always_return_item_succeed)
{
    //arrange
    IOTHUB_QUEUE_HANDLE handle = IoTHubQueue_Create_Queue();
    IOTHUB_QUEUE_RESULT queue_result = IoTHubQueue_Add_Item(handle, (void*)TEST_IOTHUB_DEVICE_TWIN_ITEM);
    ASSERT_ARE_EQUAL(IOTHUB_QUEUE_RESULT, IOTHUB_QUEUE_OK, queue_result);
    queue_result = IoTHubQueue_Add_Item(handle, (void*)TEST_IOTHUB_DEVICE_TWIN_ITEM_1);
    ASSERT_ARE_EQUAL(IOTHUB_QUEUE_RESULT, IOTHUB_QUEUE_OK, queue_result);
    queue_result = IoTHubQueue_Add_Item(handle, (void*)TEST_IOTHUB_DEVICE_TWIN_ITEM_2);
    ASSERT_ARE_EQUAL(IOTHUB_QUEUE_RESULT, IOTHUB_QUEUE_OK, queue_result);
    umock_c_reset_all_calls();

    //act
    const void* client_queue_item = IoTHubQueue_Get_Queue_Item(handle);
    ASSERT_ARE_EQUAL(void_ptr, TEST_IOTHUB_DEVICE_TWIN_ITEM, client_queue_item);

    const void* client_queue_item_2nd = IoTHubQueue_Get_Queue_Item(handle);
    ASSERT_ARE_EQUAL(void_ptr, TEST_IOTHUB_DEVICE_TWIN_ITEM, client_queue_item_2nd);

    //assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    IoTHubQueue_Destroy_Queue(handle, NULL);
}

/* Tests_SRS_IOTHUB_QUEUE_07_011: [If the queue is empty, IoTHubQueue_Get_Queue_Item shall return NULL.] */
TEST_FUNCTION(IoTHubQueue_Get_Queue_Item_empty_queue_fail)
{
    //arrange
    IOTHUB_QUEUE_HANDLE handle = IoTHubQueue_Create_Queue();
    umock_c_reset_all_calls();

    //act
    const void* client_queue_item = IoTHubQueue_Get_Queue_Item(handle);

    //assert
    ASSERT_IS_NULL(client_queue_item);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    IoTHubQueue_Destroy_Queue(handle, NULL);
}

/* Tests_SRS_IOTHUB_QUEUE_07_013: [If handle is NULL IoTHubQueue_Remove_Next_Item shall return IOTHUB_QUEUE_INVALID_ARG.] */
TEST_FUNCTION(IoTHubQueue_Remove_Item_NULL_handle_fail)
{
    //arrange

    //act
    IOTHUB_QUEUE_RESULT queue_result = IoTHubQueue_Remove_Next_Item(NULL, my_destroy_callback);

    //assert
    ASSERT_ARE_NOT_EQUAL(IOTHUB_QUEUE_RESULT, IOTHUB_QUEUE_OK, queue_result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
}

/* Tests_SRS_IOTHUB_QUEUE_07_016: [On success IoTHubQueue_Remove_Next_Item shall return IOTHUB_QUEUE_OK.] */
/* Tests_SRS_IOTHUB_QUEUE_07_015: [If the Item is successfully Removed from the queue and destroy_callback is not NULL, IoTHubQueue_Remove_Next_Item will call destroy_callback and deallocate the item.] */
TEST_FUNCTION(IoTHubQueue_Remove_Item_succeed)
{
    //arrange
    IOTHUB_QUEUE_HANDLE handle = IoTHubQueue_Create_Queue();
    IOTHUB_QUEUE_RESULT queue_result = IoTHubQueue_Add_Item(handle, (void*)TEST_IOTHUB_DEVICE_TWIN_ITEM);
    ASSERT_ARE_EQUAL(IOTHUB_QUEUE_RESULT, IOTHUB_QUEUE_OK, queue_result);
    queue_result = IoTHubQueue_Add_Item(handle, (void*)TEST_IOTHUB_DEVICE_TWIN_ITEM_1);
    ASSERT_ARE_EQUAL(IOTHUB_QUEUE_RESULT, IOTHUB_QUEUE_OK, queue_result);
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(DList_RemoveEntryList(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
        .IgnoreArgument_ptr();

    //act
    queue_result = IoTHubQueue_Remove_Next_Item(handle, my_destroy_callback);

    //assert
    ASSERT_ARE_EQUAL(size_t, 1, g_destroy_callback_called);
    ASSERT_ARE_EQUAL(IOTHUB_QUEUE_RESULT, IOTHUB_QUEUE_OK, queue_result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    IoTHubQueue_Destroy_Queue(handle, NULL);
}

/* Tests_SRS_IOTHUB_QUEUE_07_014: [If the queue is empty, IoTHubQueue_Remove_Next_Item shall return IOTHUB_QUEUE_QUEUE_EMPTY.] */
TEST_FUNCTION(IoTHubQueue_Remove_Item_on_empty_queue_fail)
{
    //arrange
    IOTHUB_QUEUE_HANDLE handle = IoTHubQueue_Create_Queue();
    umock_c_reset_all_calls();

    //act
    IOTHUB_QUEUE_RESULT queue_result = IoTHubQueue_Remove_Next_Item(handle, my_destroy_callback);

    //assert
    ASSERT_ARE_EQUAL(size_t, 0, g_destroy_callback_called);
    ASSERT_ARE_EQUAL(IOTHUB_QUEUE_RESULT, IOTHUB_QUEUE_QUEUE_EMPTY, queue_result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    IoTHubQueue_Destroy_Queue(handle, NULL);
}

/* Tests_SRS_IOTHUB_QUEUE_07_017: [If handle is NULL IoTHubQueue_Enum_Queue shall return NULL.] */
TEST_FUNCTION(IoTHubQueue_Enum_Queue_handle_fail)
{
    //arrange

    //act
    IOTHUB_QUEUE_ENUM_HANDLE enum_handle = IoTHubQueue_Enum_Queue(NULL);

    //assert
    ASSERT_IS_NULL(enum_handle);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
}

/* Tests_SRS_IOTHUB_QUEUE_07_025: [If the queue referenced by handle is empty IoTHubQueue_Enum_Queue shall return NULL.] */
TEST_FUNCTION(IoTHubQueue_Enum_Queue_on_empty_list_NULL)
{
    //arrange
    IOTHUB_QUEUE_HANDLE handle = IoTHubQueue_Create_Queue();
    umock_c_reset_all_calls();

    //act
    IOTHUB_QUEUE_ENUM_HANDLE enum_handle = IoTHubQueue_Enum_Queue(handle);

    //assert
    ASSERT_IS_NULL(enum_handle);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    IoTHubQueue_Enum_Close(enum_handle);
    IoTHubQueue_Destroy_Queue(handle, NULL);
}

/* Tests_SRS_IOTHUB_QUEUE_07_018: [IoTHubQueue_Enum_Queue shall allocate and initialize the data neccessary for enumeration of the queue.] */
/* Tests_SRS_IOTHUB_QUEUE_07_019: [On Success IoTHubQueue_Enum_Queue shall return a IOTHUB_QUEUE_ENUM_HANDLE.] */
TEST_FUNCTION(IoTHubQueue_Enum_Queue_success)
{
    //arrange
    IOTHUB_QUEUE_HANDLE handle = IoTHubQueue_Create_Queue();
    IOTHUB_QUEUE_RESULT queue_result = IoTHubQueue_Add_Item(handle, (void*)TEST_IOTHUB_DEVICE_TWIN_ITEM);
    ASSERT_ARE_EQUAL(IOTHUB_QUEUE_RESULT, IOTHUB_QUEUE_OK, queue_result);
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG)).IgnoreArgument_size();

    //act
    IOTHUB_QUEUE_ENUM_HANDLE enum_handle = IoTHubQueue_Enum_Queue(handle);

    //assert
    ASSERT_IS_NOT_NULL(enum_handle);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    IoTHubQueue_Enum_Close(enum_handle);
    IoTHubQueue_Destroy_Queue(handle, NULL);
}

/* Tests_SRS_IOTHUB_QUEUE_07_026: [If any error is encountered IoTHubQueue_Enum_Queue shall return NULL.] */
TEST_FUNCTION(IoTHubQueue_Enum_Queue_fail)
{
    //arrange
    int negativeTestsInitResult = umock_c_negative_tests_init();
    ASSERT_ARE_EQUAL(int, 0, negativeTestsInitResult);

    IOTHUB_QUEUE_HANDLE handle = IoTHubQueue_Create_Queue();
    IOTHUB_QUEUE_RESULT queue_result = IoTHubQueue_Add_Item(handle, (void*)TEST_IOTHUB_DEVICE_TWIN_ITEM);
    ASSERT_ARE_EQUAL(IOTHUB_QUEUE_RESULT, IOTHUB_QUEUE_OK, queue_result);
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG)).IgnoreArgument_size();

    umock_c_negative_tests_snapshot();

    //act
    size_t count = umock_c_negative_tests_call_count();
    for (size_t index = 0; index < count; index++)
    {
        umock_c_negative_tests_reset();
        umock_c_negative_tests_fail_call(index);

        char tmp_msg[64];
        sprintf(tmp_msg, "IoTHubQueue_Enum_Queue failure in test %zu/%zu", index, count);

        IOTHUB_QUEUE_ENUM_HANDLE enum_handle = IoTHubQueue_Enum_Queue(handle);

        //assert
        ASSERT_IS_NULL(enum_handle);
    }

    //cleanup
    IoTHubQueue_Destroy_Queue(handle, NULL);
    umock_c_negative_tests_deinit();
}

/* Tests_SRS_IOTHUB_QUEUE_07_020: [If enum_handle is NULL IoTHubQueue_Enum_Next_Item shall return NULL.] */
TEST_FUNCTION(IoTHubQueue_Enum_Next_Item_handle_null_fail)
{
    //arrange
    umock_c_reset_all_calls();

    //act
    const void* queue_item = IoTHubQueue_Enum_Next_Item(NULL);

    //assert
    ASSERT_IS_NULL(queue_item);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
}

/* Tests_SRS_IOTHUB_QUEUE_07_021: [If the end of the queue is reached IoTHubQueue_Enum_Next_Item shall return NULL.] */
TEST_FUNCTION(IoTHubQueue_Enum_Next_Item_empty_list_success)
{
    //arrange
    IOTHUB_QUEUE_HANDLE handle = IoTHubQueue_Create_Queue();

    IOTHUB_QUEUE_ENUM_HANDLE enum_handle = IoTHubQueue_Enum_Queue(handle);
    umock_c_reset_all_calls();

    //act
    const void* queue_item = IoTHubQueue_Enum_Next_Item(enum_handle);
    ASSERT_IS_NULL(queue_item);

    queue_item = IoTHubQueue_Enum_Next_Item(enum_handle);

    //assert
    ASSERT_IS_NULL(queue_item);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    IoTHubQueue_Enum_Close(enum_handle);
    IoTHubQueue_Destroy_Queue(handle, NULL);
}

/* Tests_SRS_IOTHUB_QUEUE_07_022: [On success IoTHubQueue_Enum_Next_Item shall return the next queue item from the previous call to IoTHubQueue_Enum_Next_Item.] */
TEST_FUNCTION(IoTHubQueue_Enum_Next_Item_success)
{
    //arrange
    IOTHUB_QUEUE_HANDLE handle = IoTHubQueue_Create_Queue();
    IOTHUB_QUEUE_RESULT queue_result = IoTHubQueue_Add_Item(handle, (void*)TEST_IOTHUB_DEVICE_TWIN_ITEM);
    ASSERT_ARE_EQUAL(IOTHUB_QUEUE_RESULT, IOTHUB_QUEUE_OK, queue_result);
    queue_result = IoTHubQueue_Add_Item(handle, (void*)TEST_IOTHUB_DEVICE_TWIN_ITEM_1);
    ASSERT_ARE_EQUAL(IOTHUB_QUEUE_RESULT, IOTHUB_QUEUE_OK, queue_result);
    queue_result = IoTHubQueue_Add_Item(handle, (void*)TEST_IOTHUB_DEVICE_TWIN_ITEM_2);
    ASSERT_ARE_EQUAL(IOTHUB_QUEUE_RESULT, IOTHUB_QUEUE_OK, queue_result);

    IOTHUB_QUEUE_ENUM_HANDLE enum_handle = IoTHubQueue_Enum_Queue(handle);
    umock_c_reset_all_calls();

    //act
    const void* queue_item = IoTHubQueue_Enum_Next_Item(enum_handle);
    ASSERT_ARE_EQUAL(void_ptr, TEST_IOTHUB_DEVICE_TWIN_ITEM, (IOTHUB_DEVICE_TWIN*)queue_item);

    queue_item = IoTHubQueue_Enum_Next_Item(enum_handle);
    ASSERT_ARE_EQUAL(void_ptr, TEST_IOTHUB_DEVICE_TWIN_ITEM_1, (IOTHUB_DEVICE_TWIN*)queue_item);

    queue_item = IoTHubQueue_Enum_Next_Item(enum_handle);
    ASSERT_ARE_EQUAL(void_ptr, TEST_IOTHUB_DEVICE_TWIN_ITEM_2, (IOTHUB_DEVICE_TWIN*)queue_item);

    queue_item = IoTHubQueue_Enum_Next_Item(enum_handle);

    //assert
    ASSERT_IS_NULL(queue_item);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    IoTHubQueue_Enum_Close(enum_handle);
    IoTHubQueue_Destroy_Queue(handle, NULL);
}

/* Tests_SRS_IOTHUB_QUEUE_07_023: [If enum_handle is NULL IoTHubQueue_Enum_Close shall do nothing.] */
TEST_FUNCTION(IoTHubQueue_Enum_Close_enum_handle_NULL_success)
{
    //arrange

    //act
    IoTHubQueue_Enum_Close(NULL);

    //assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
}

/* Tests_SRS_IOTHUB_QUEUE_07_024: [IoTHubQueue_Enum_Close shall deallocate any information that has been allocated in IoTHubQueue_Enum_Queue.] */
TEST_FUNCTION(IoTHubQueue_Enum_Close_success)
{
    //arrange
    IOTHUB_QUEUE_HANDLE handle = IoTHubQueue_Create_Queue();
    IOTHUB_QUEUE_RESULT queue_result = IoTHubQueue_Add_Item(handle, (void*)TEST_IOTHUB_DEVICE_TWIN_ITEM);
    ASSERT_ARE_EQUAL(IOTHUB_QUEUE_RESULT, IOTHUB_QUEUE_OK, queue_result);

    IOTHUB_QUEUE_ENUM_HANDLE enum_handle = IoTHubQueue_Enum_Queue(handle);
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    //act
    IoTHubQueue_Enum_Close(enum_handle);

    //assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    IoTHubQueue_Destroy_Queue(handle, NULL);
}

END_TEST_SUITE(iothubclient_queue_ut)
