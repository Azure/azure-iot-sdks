// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <cstdlib>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "testrunnerswitcher.h"
#include "micromock.h"
#include "micromockcharstararenullterminatedstrings.h"
#include "multitree.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_c_shared_utility/macro_utils.h"
#include "azure_c_shared_utility/lock.h"

#ifndef SIZE_MAX
#define SIZE_MAX ((size_t)~(size_t)0)
#endif

#define GBALLOC_H

extern "C" int gballoc_init(void);
extern "C" void gballoc_deinit(void);
extern "C" void* gballoc_malloc(size_t size);
extern "C" void* gballoc_calloc(size_t nmemb, size_t size);
extern "C" void* gballoc_realloc(void* ptr, size_t size);
extern "C" void gballoc_free(void* ptr);

//
// We do this namespace redirection so that we don't have to repeat the utility string and buffer code in the mocks!
//
namespace BASEIMPLEMENTATION
{
/*if malloc is defined as gballoc_malloc at this moment, there'd be serious trouble*/
#define Lock(x) (LOCK_OK + gballocState - gballocState) /*compiler warning about constant in if condition*/
#define Unlock(x) (LOCK_OK + gballocState - gballocState)
#define Lock_Init() (LOCK_HANDLE)0x42
#define Lock_Deinit(x) (LOCK_OK + gballocState - gballocState)
#include "gballoc.c"
#undef Lock
#undef Unlock
#undef Lock_Init
#undef Lock_Deinit

#include "strings.c"
};

DEFINE_MICROMOCK_ENUM_TO_STRING(MULTITREE_RESULT, MULTITREE_RESULT_VALUES);

static int StringClone(void** destination, const void* source)
{
    return mallocAndStrcpy_s((char**)destination, (const char*)source);
}

static void StringFree(void* string)
{
    gballoc_free(string);
}

#if defined _MSC_VER
#define snprintf _snprintf
#endif

static void MULTITREE_HANDLE_ToString(char* dest, size_t bufferSize, MULTITREE_HANDLE handle)
{
    (void)snprintf(dest, bufferSize, "%p", handle);
}
static bool MULTITREE_HANDLE_Compare(MULTITREE_HANDLE left, MULTITREE_HANDLE right)
{
    return left != right;
}



/*TEST ASSETS*/
/*there will be a "test tree" used through these tests*/
/*this test tree looks like the following*/
/*all below pairs such as root/NULL represent a nodeName/Value*/
/*not all nodes have value, but all must have names*/
/*nodes are immutable, once created, their values cannot be altered (name or value), but children can be added to them*/
/*all leaf nodes MUST have values*/
/*
a
lvl0                                        root/NULL                                                                           a
a
lvl1          child1/v1                    child2/v2                           child3/NULL                                      a
a
lvl2    child11/v11   child12/v12                                               child31/NULL                                     a
a
lvl3                                                   child311/v311   child312/v312   child313/v313   child314/v314            a
a
a
*/

#define CHILD1NAME "child1"
#define CHILD1PATH "/child1"
#define CHILD1PATH_ALTERNATE "child1"
#define CHILD1VALUE "v1"

#define CHILD11NAME "child11"
#define CHILD11PATH "/child1/child11"
#define CHILD11PATH_ALTERNATE "child1/child11"
#define CHILD11VALUE "v11"

#define CHILD12NAME "child12"
#define CHILD12PATH "/child1/child12"
#define CHILD12PATH_ALTERNATE "child1/child12"
#define CHILD12VALUE "v12"

#define CHILD2NAME "child2"
#define CHILD2PATH "/child2"
#define CHILD2PATH_ALTERNATE "child2"
#define CHILD2VALUE "v2"

#define CHILD3NAME "child3"
#define CHILD3PATH "/child3"
#define CHILD3PATH_ALTERNATE "child3"
#define CHILD3VALUE NULL

#define CHILD31NAME "child31"
#define CHILD31PATH "/child3/child31"
#define CHILD31PATH_ALTERNATE "child3/child31"
#define CHILD31VALUE NULL

#define CHILD311NAME "child311"
#define CHILD311PATH "/child3/child31/child311"
#define CHILD311PATH_ALTERNATE "child3/child31/child311"
#define CHILD311VALUE "v311"

#define CHILD312NAME "child312"
#define CHILD312PATH "/child3/child31/child312"
#define CHILD312PATH_ALTERNATE "child3/child31/child312"
#define CHILD312VALUE "v312"

#define CHILD313NAME "child313"
#define CHILD313PATH "/child3/child31/child313"
#define CHILD313PATH_ALTERNATE "child3/child31/child313"
#define CHILD313VALUE "v313"

#define CHILD314NAME "child314"
#define CHILD314PATH "child3/child31/child314"
#define CHILD314PATH_ALTERNATE "child3/child31/child314"
#define CHILD314VALUE "v314"

#define BUFFER_TEMP_SIZE 1000
static STRING_HANDLE global_bufferTemp;
char bufferTemp2[BUFFER_TEMP_SIZE];

static size_t currentmalloc_call;
static size_t whenShallmalloc_fail;

/*different STRING constructors*/
static size_t currentSTRING_new_call;
static size_t whenShallSTRING_new_fail;

static size_t currentSTRING_clone_call;
static size_t whenShallSTRING_clone_fail;

static size_t currentSTRING_construct_call;
static size_t whenShallSTRING_construct_fail;

static size_t currentSTRING_concat_call;
static size_t whenShallSTRING_concat_fail;

static size_t currentSTRING_empty_call;
static size_t whenShallSTRING_empty_fail;

static size_t currentSTRING_concat_with_STRING_call;
static size_t whenShallSTRING_concat_with_STRING_fail;

TYPED_MOCK_CLASS(CMultiTreeMocks, CGlobalMock)
{
public:

    MOCK_STATIC_METHOD_1(, void*, gballoc_malloc, size_t, size)
        void* result2;
    currentmalloc_call++;
    if (whenShallmalloc_fail>0)
    {
        if (currentmalloc_call == whenShallmalloc_fail)
        {
            result2 = (STRING_HANDLE)NULL;
        }
        else
        {
            result2 = BASEIMPLEMENTATION::gballoc_malloc(size);
        }
    }
    else
    {
        result2 = BASEIMPLEMENTATION::gballoc_malloc(size);
    }
    MOCK_METHOD_END(void*, result2);

    MOCK_STATIC_METHOD_2(, void*, gballoc_realloc, void*, ptr, size_t, size)
        MOCK_METHOD_END(void*, BASEIMPLEMENTATION::gballoc_realloc(ptr, size));

    MOCK_STATIC_METHOD_1(, void, gballoc_free, void*, ptr)
        BASEIMPLEMENTATION::gballoc_free(ptr);
    MOCK_VOID_METHOD_END()

        /*Strings*/
        MOCK_STATIC_METHOD_0(, STRING_HANDLE, STRING_new)
        STRING_HANDLE result2;
    currentSTRING_new_call++;
    if (whenShallSTRING_new_fail>0)
    {
        if (currentSTRING_new_call == whenShallSTRING_new_fail)
        {
            result2 = (STRING_HANDLE)NULL;
        }
        else
        {
            result2 = BASEIMPLEMENTATION::STRING_new();
        }
    }
    else
    {
        result2 = BASEIMPLEMENTATION::STRING_new();
    }
    MOCK_METHOD_END(STRING_HANDLE, result2)

        MOCK_STATIC_METHOD_1(, STRING_HANDLE, STRING_clone, STRING_HANDLE, handle)
        STRING_HANDLE result2;
    currentSTRING_clone_call++;
    if (whenShallSTRING_clone_fail>0)
    {
        if (currentSTRING_clone_call == whenShallSTRING_clone_fail)
        {
            result2 = (STRING_HANDLE)NULL;
        }
        else
        {
            result2 = BASEIMPLEMENTATION::STRING_clone(handle);
        }
    }
    else
    {
        result2 = BASEIMPLEMENTATION::STRING_clone(handle);
    }
    MOCK_METHOD_END(STRING_HANDLE, result2)

        MOCK_STATIC_METHOD_1(, STRING_HANDLE, STRING_construct, const char*, source)
        STRING_HANDLE result2;
    currentSTRING_construct_call++;
    if (whenShallSTRING_construct_fail>0)
    {
        if (currentSTRING_construct_call == whenShallSTRING_construct_fail)
        {
            result2 = (STRING_HANDLE)NULL;
        }
        else
        {
            result2 = BASEIMPLEMENTATION::STRING_construct(source);
        }
    }
    else
    {
        result2 = BASEIMPLEMENTATION::STRING_construct(source);
    }
    MOCK_METHOD_END(STRING_HANDLE, result2)

        MOCK_STATIC_METHOD_1(, void, STRING_delete, STRING_HANDLE, s)
        BASEIMPLEMENTATION::STRING_delete(s);
    MOCK_VOID_METHOD_END()

    MOCK_STATIC_METHOD_2(, int, STRING_concat, STRING_HANDLE, s1, const char*, s2)
        currentSTRING_concat_call++;
    MOCK_METHOD_END(int, (((whenShallSTRING_concat_fail>0) && (currentSTRING_concat_call == whenShallSTRING_concat_fail)) ? __LINE__ : BASEIMPLEMENTATION::STRING_concat(s1, s2)));

    MOCK_STATIC_METHOD_2(, int, STRING_concat_with_STRING, STRING_HANDLE, s1, STRING_HANDLE, s2)
        currentSTRING_concat_with_STRING_call++;
    MOCK_METHOD_END(int, (((currentSTRING_concat_with_STRING_call>0) && (currentSTRING_concat_with_STRING_call == whenShallSTRING_concat_with_STRING_fail)) ? __LINE__ : BASEIMPLEMENTATION::STRING_concat_with_STRING(s1, s2)));


    MOCK_STATIC_METHOD_1(, int, STRING_empty, STRING_HANDLE, s)
        currentSTRING_concat_call++;
    MOCK_METHOD_END(int, BASEIMPLEMENTATION::STRING_empty(s));

    MOCK_STATIC_METHOD_1(, const char*, STRING_c_str, STRING_HANDLE, s)
        MOCK_METHOD_END(const char*, BASEIMPLEMENTATION::STRING_c_str(s))
};

DECLARE_GLOBAL_MOCK_METHOD_1(CMultiTreeMocks, , void*, gballoc_malloc, size_t, size);
DECLARE_GLOBAL_MOCK_METHOD_2(CMultiTreeMocks, , void*, gballoc_realloc, void*, ptr, size_t, size);
DECLARE_GLOBAL_MOCK_METHOD_1(CMultiTreeMocks, , void, gballoc_free, void*, ptr)

DECLARE_GLOBAL_MOCK_METHOD_0(CMultiTreeMocks, , STRING_HANDLE, STRING_new);
DECLARE_GLOBAL_MOCK_METHOD_1(CMultiTreeMocks, , STRING_HANDLE, STRING_clone, STRING_HANDLE, handle);
DECLARE_GLOBAL_MOCK_METHOD_1(CMultiTreeMocks, , STRING_HANDLE, STRING_construct, const char*, s);

DECLARE_GLOBAL_MOCK_METHOD_1(CMultiTreeMocks, , void, STRING_delete, STRING_HANDLE, s);
DECLARE_GLOBAL_MOCK_METHOD_2(CMultiTreeMocks, , int, STRING_concat, STRING_HANDLE, s1, const char*, s2);
DECLARE_GLOBAL_MOCK_METHOD_2(CMultiTreeMocks, , int, STRING_concat_with_STRING, STRING_HANDLE, s1, STRING_HANDLE, s2);
DECLARE_GLOBAL_MOCK_METHOD_1(CMultiTreeMocks, , int, STRING_empty, STRING_HANDLE, s1);
DECLARE_GLOBAL_MOCK_METHOD_1(CMultiTreeMocks, , const char*, STRING_c_str, STRING_HANDLE, s);

static MICROMOCK_GLOBAL_SEMAPHORE_HANDLE g_dllByDll;
static MICROMOCK_MUTEX_HANDLE g_testByTest;
BEGIN_TEST_SUITE(MultiTree_ut)

TEST_SUITE_INITIALIZE(BeforeSuite)
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

TEST_FUNCTION_INITIALIZE(init)
{
    int result = BASEIMPLEMENTATION::gballoc_init();
    ASSERT_ARE_EQUAL(int, 0, result);

    global_bufferTemp = BASEIMPLEMENTATION::STRING_new();
    currentmalloc_call = 0;
    whenShallmalloc_fail = 0;

    currentSTRING_new_call = 0;
    whenShallSTRING_new_fail = 0;

    currentSTRING_clone_call = 0;
    whenShallSTRING_clone_fail = 0;

    currentSTRING_construct_call = 0;
    whenShallSTRING_construct_fail = 0;

    currentSTRING_concat_call = 0;
    whenShallSTRING_concat_fail = 0;

    currentSTRING_empty_call = 0;
    whenShallSTRING_empty_fail = 0;

    currentSTRING_concat_with_STRING_call = 0;
    whenShallSTRING_concat_with_STRING_fail = 0; 

}

TEST_FUNCTION_CLEANUP(clean)
{
    BASEIMPLEMENTATION::STRING_delete(global_bufferTemp);

    if (BASEIMPLEMENTATION::gballoc_getCurrentMemoryUsed() != 0)
    {
        char temp[1000];
        sprintf(temp, "Test did not clean memory properly, leaking %lu bytes", (long unsigned int)BASEIMPLEMENTATION::gballoc_getCurrentMemoryUsed());
        BASEIMPLEMENTATION::gballoc_deinit();
        ASSERT_FAIL(temp);
    }

    BASEIMPLEMENTATION::gballoc_deinit();


}

/*Tests_SRS_MULTITREE_99_005:[ MultiTree_Create creates a new tree.]*/
/*Tests_SRS_MULTITREE_99_006:[ MultiTree_Create returns a non-NULL pointer if the tree has been sucesfully created.]*/
TEST_FUNCTION(MultiTree_Create_succeeds)
{
    ///arrange
    CMultiTreeMocks mocks;
    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(0))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    ///act
    auto res = MultiTree_Create(StringClone, StringFree);

    ///assert
    ASSERT_IS_NOT_NULL(res);

    ///cleanup
    MultiTree_Destroy(res);
}

/*Tests_SRS_MULTITREE_99_052:[ If any of the arguments passed to MultiTree_Create is NULL, the call shall return NULL.]*/
TEST_FUNCTION(MultiTree_Create_With_NULL_Clone_Function_Fails)
{
    ///arrange
    CMultiTreeMocks mocks;

    ///act
    auto res = MultiTree_Create(NULL, free);

    ///assert
    ASSERT_IS_NULL(res);
}

/*Tests_SRS_MULTITREE_99_052:[ If any of the arguments passed to MultiTree_Create is NULL, the call shall return NULL.]*/
TEST_FUNCTION(MultiTree_Create_With_NULL_Free_Function_Fails)
{
    ///arrange
    CMultiTreeMocks mocks;

    ///act
    auto res = MultiTree_Create(StringClone, NULL);

    ///assert
    ASSERT_IS_NULL(res);
}

/*SRS_MULTITREE_99_007:[ MultiTree_Create returns NULL if the tree has not been successfully created.]*/
TEST_FUNCTION(MultiTree_Create_if_malloc_fails_then_it_fails)
{
    ///arrange
    CMultiTreeMocks mocks;

    whenShallmalloc_fail = 1;
    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(0))
        .IgnoreArgument(1);
    auto res = MultiTree_Create(StringClone, StringFree);

    ///assert

    ASSERT_IS_NULL(res);

    ///cleanup
    MultiTree_Destroy(res);
}

/*Tests_SRS_MULTITREE_99_018:[ If the treeHandle parameter is NULL, MULTITREE_INVALID_ARG shall be returned.]*/
TEST_FUNCTION(MultiTree_AddLeaf_with_NULL_handle_fails)
{
    ///arrange
    CMultiTreeMocks mocks;

    ///act
    auto res = MultiTree_AddLeaf(NULL, "/child1", (void*)"value");

    ///assert
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_INVALID_ARG, res);
}

/*Tests_SRS_MULTITREE_99_019:[ If parameter destinationPath is NULL, MULTITREE_INVALID_ARG shall be returned.]*/
TEST_FUNCTION(MultiTree_AddLeaf_with_NULL_path_fails)
{
    ///arrange
    CMultiTreeMocks mocks;
    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(0)) /*because MultiTree_Create*/
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*because MultiTree_Destroy*/
        .IgnoreArgument(1);

    auto treeHandle = MultiTree_Create(StringClone, StringFree);

    ///act
    auto res = MultiTree_AddLeaf(treeHandle, NULL, (void*)"value");

    ///assert
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_INVALID_ARG, res);

    //cleanup
    MultiTree_Destroy(treeHandle);
}

/*Tests_SRS_MULTITREE_99_020:[ If parameter value is NULL, MULTITREE_INVALID_ARG shall be returned.]*/
TEST_FUNCTION(MultiTree_AddLeaf_with_NULL_value_fails)
{
    ///arrange
    CMultiTreeMocks mocks;
    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(0)) /*because MultiTree_Create*/
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*because MultiTree_Destroy*/
        .IgnoreArgument(1);

    auto treeHandle = MultiTree_Create(StringClone, StringFree);

    ///act
    auto res = MultiTree_AddLeaf(treeHandle, "/child1", NULL);

    ///assert
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_INVALID_ARG, res);

    //cleanup
    MultiTree_Destroy(treeHandle);
}

/*Tests_SRS_MULTITREE_99_050:[ If destinationPath a string with zero characters, MULTITREE_EMPTY_CHILD_NAME shall be returned.]*/
TEST_FUNCTION(MultiTree_AddLeaf_with_empty_destination_fails)
{
    ///arrange
    CMultiTreeMocks mocks;
    auto treeHandle = MultiTree_Create(StringClone, StringFree);

    ///act
    auto res = MultiTree_AddLeaf(treeHandle, "", (void*)"value1");

    ///assert
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_EMPTY_CHILD_NAME, res);

    //cleanup
    MultiTree_Destroy(treeHandle);
    mocks.ResetAllCalls();
}

/*Tests_SRS_MULTITREE_99_034:[ The function returns MULTITREE_OK when data has been stored in the tree.]*/
TEST_FUNCTION(MultiTree_AddLeaf_creates_a_child_in_root)
{
    ///arrange
    CMultiTreeMocks mocks;
    auto treeHandle = MultiTree_Create(StringClone, StringFree);

    ///act
    auto res = MultiTree_AddLeaf(treeHandle, CHILD1PATH, (void*)CHILD1VALUE);

    ///assert
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_OK, res);

    //cleanup
    MultiTree_Destroy(treeHandle);
    mocks.ResetAllCalls(); /*not caring about what gets called*/
}

/*Tests_SRS_MULTITREE_99_034:[ The function returns MULTITREE_OK when data has been stored in the tree.]*/
TEST_FUNCTION(MultiTree_AddLeaf_creates_a_child_in_root_alternate)
{
    ///arrange
    CMultiTreeMocks mocks;
    auto treeHandle = MultiTree_Create(StringClone, StringFree);

    ///act
    auto res = MultiTree_AddLeaf(treeHandle, CHILD1PATH_ALTERNATE, (void*)CHILD1VALUE);

    ///assert
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_OK, res);

    //cleanup
    MultiTree_Destroy(treeHandle);
    mocks.ResetAllCalls(); /*not caring about what gets called*/
}

/*Tests_SRS_MULTITREE_99_021:[ If the node already has a value assigned to it, MULTITREE_ALREADY_HAS_A_VALUE shall be returned and the existing value shall not be changed.]*/
TEST_FUNCTION(MultiTree_AddLeaf_creates_a_child_in_root_twice_is_error)
{
    ///arrange
    CMultiTreeMocks mocks;
    auto treeHandle = MultiTree_Create(StringClone, StringFree);
    (void)MultiTree_AddLeaf(treeHandle, CHILD1PATH, (void*)CHILD1VALUE);

    ///act
    auto res = MultiTree_AddLeaf(treeHandle, CHILD1PATH, (void*)CHILD1VALUE);

    ///assert
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_ALREADY_HAS_A_VALUE, res);

    //cleanup
    MultiTree_Destroy(treeHandle);
    mocks.ResetAllCalls(); /*not caring about what gets called*/
}

/*Tests_SRS_MULTITREE_99_021:[ If the node already has a value assigned to it, MULTITREE_ALREADY_HAS_A_VALUE shall be returned and the existing value shall not be changed.]*/
TEST_FUNCTION(MultiTree_AddLeaf_creates_a_child_in_root_twice_is_error_alternate)
{
    ///arrange
    CMultiTreeMocks mocks;
    auto treeHandle = MultiTree_Create(StringClone, StringFree);
    (void)MultiTree_AddLeaf(treeHandle, CHILD1PATH_ALTERNATE, (void*)CHILD1VALUE);

    ///act
    auto res = MultiTree_AddLeaf(treeHandle, CHILD1PATH_ALTERNATE, (void*)CHILD1VALUE);

    ///assert
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_ALREADY_HAS_A_VALUE, res);

    //cleanup
    MultiTree_Destroy(treeHandle);
    mocks.ResetAllCalls(); /*not caring about what gets called*/
}

/*Tests_SRS_MULTITREE_99_021:[ If the node already has a value assigned to it, MULTITREE_ALREADY_HAS_A_VALUE shall be returned and the existing value shall not be changed.]*/
TEST_FUNCTION(MultiTree_AddLeaf_creates_a_child_in_root_twice_is_error_alternate_2)
{
    ///arrange
    CMultiTreeMocks mocks;
    auto treeHandle = MultiTree_Create(StringClone, StringFree);
    (void)MultiTree_AddLeaf(treeHandle, CHILD1PATH, (void*)CHILD1VALUE);

    ///act
    auto res = MultiTree_AddLeaf(treeHandle, CHILD1PATH_ALTERNATE, (void*)CHILD1VALUE);

    ///assert
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_ALREADY_HAS_A_VALUE, res);

    //cleanup
    MultiTree_Destroy(treeHandle);
    mocks.ResetAllCalls(); /*not caring about what gets called*/
}

/*Tests_SRS_MULTITREE_99_021:[ If the node already has a value assigned to it, MULTITREE_ALREADY_HAS_A_VALUE shall be returned and the existing value shall not be changed.]*/
TEST_FUNCTION(MultiTree_AddLeaf_creates_a_child_in_root_twice_is_error_alternate_3)
{
    ///arrange
    CMultiTreeMocks mocks;
    auto treeHandle = MultiTree_Create(StringClone, StringFree);
    (void)MultiTree_AddLeaf(treeHandle, CHILD1PATH_ALTERNATE, (void*)CHILD1VALUE);

    ///act
    auto res = MultiTree_AddLeaf(treeHandle, CHILD1PATH, (void*)CHILD1VALUE);

    ///assert
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_ALREADY_HAS_A_VALUE, res);

    //cleanup
    MultiTree_Destroy(treeHandle);
    mocks.ResetAllCalls(); /*not caring about what gets called*/
}

/*Tests_SRS_MULTITREE_99_021:[ If the node already has a value assigned to it, MULTITREE_ALREADY_HAS_A_VALUE shall be returned and the existing value shall not be changed.]*/
/*the following test shall create a child of a child and then attept to assign some value to the father of the leaf*/
/*any assignemnt of values should fail*/
TEST_FUNCTION(MultiTree_AddLeaf_assign_a_value_to_a_leaf_is_error)
{
    ///arrange
    CMultiTreeMocks mocks;
    auto treeHandle = MultiTree_Create(StringClone, StringFree);
    (void)MultiTree_AddLeaf(treeHandle, CHILD11PATH_ALTERNATE, (void*)CHILD11VALUE);

    ///act
    /*auto res = MultiTree_AddLeaf(treeHandle, CHILD1PATH, (void*)CHILD1VALUE);

    ///assert
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_ALREADY_HAS_A_VALUE, res);*/

    //cleanup
    MultiTree_Destroy(treeHandle);
    mocks.ResetAllCalls(); /*not caring about what gets called*/
}

/*Tests_SRS_MULTITREE_99_021:[ If the node already has a value assigned to it, MULTITREE_ALREADY_HAS_A_VALUE shall be returned and the existing value shall not be changed.]*/
/*the following test shall create a child of a child and then attept to assign some value to child of child*/
/*any assignemnt of values should fail*/
TEST_FUNCTION(MultiTree_AddLeaf_assign_a_value_to_a_leaf_is_error_2)
{
    ///arrange
    CMultiTreeMocks mocks;
    auto treeHandle = MultiTree_Create(StringClone, StringFree);
    (void)MultiTree_AddLeaf(treeHandle, CHILD11PATH_ALTERNATE, (void*)CHILD11VALUE);

    ///act
    auto res = MultiTree_AddLeaf(treeHandle, CHILD11PATH, (void*)CHILD11VALUE);

    ///assert
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_ALREADY_HAS_A_VALUE, res);

    //cleanup
    MultiTree_Destroy(treeHandle);
    mocks.ResetAllCalls(); /*not caring about what gets called*/
}

/*Tests_SRS_MULTITREE_99_022:[ If a child along the path does not exist, it shall be created.] */
/*Tests_SRS_MULTITREE_99_023:[ The newly created children along the path shall have a NULL value by default.]*/
/*Tests_SRS_MULTITREE_99_034:[The function returns MULTITREE_OK when data has been stored in the tree.]*/
/*this test goes deep down to child314*/
/*to check if a child is created along the path, several calls are done to check the structure of the tree*/
TEST_FUNCTION(MultiTree_AddLeaf_creates_children_in_the_path)
{
    ///arrange
    CMultiTreeMocks mocks;
    auto treeHandle = MultiTree_Create(StringClone, StringFree);

    ///act
    auto res = MultiTree_AddLeaf(treeHandle, CHILD314PATH, (void*)CHILD314VALUE);

    ///assert
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_OK, res);

    /*for every level the following shall be checked:
    - the name
    - the value is NULL
    - there is 1 child*/

    auto level0_root = treeHandle;
    /*check lvl 0*/
    STRING_empty(global_bufferTemp);
    auto res0_GetName = MultiTree_GetName(level0_root, global_bufferTemp);
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_EMPTY_CHILD_NAME, res0_GetName);  /*because ROOT node cannot have a name*/

    const char* nodeValue;
    auto res0_GetValue = MultiTree_GetValue(level0_root, (const void**)&nodeValue);
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_EMPTY_VALUE, res0_GetValue); /*because ROOT node cannot have a value*/

    size_t level0_ChildCount = 0;
    auto res0_GetChildCount = MultiTree_GetChildCount(level0_root, &level0_ChildCount);
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_OK, res0_GetChildCount);
    ASSERT_ARE_EQUAL(size_t, (size_t)1, level0_ChildCount);

    /*check lvl 1*/
    MULTITREE_HANDLE level1_root;
    auto res1_GetChild = MultiTree_GetChild(level0_root, 0, &level1_root);
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_OK, res1_GetChild);

    STRING_empty(global_bufferTemp);
    auto res1_GetName = MultiTree_GetName(level1_root, global_bufferTemp);
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_OK, res1_GetName);
    ASSERT_ARE_EQUAL(char_ptr, CHILD3NAME, STRING_c_str(global_bufferTemp));

    auto res1_GetValue = MultiTree_GetValue(level1_root, (const void**)&nodeValue);
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_EMPTY_VALUE, res1_GetValue); /*because CHILD3 doesn't have a value*/

    size_t level1_ChildCount = 0;
    auto res1_GetChildCount = MultiTree_GetChildCount(level1_root, &level1_ChildCount);
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_OK, res1_GetChildCount);
    ASSERT_ARE_EQUAL(size_t, (size_t)1, level1_ChildCount);

    /*check lvl 2*/
    MULTITREE_HANDLE level2_root;
    auto res2_GetChild = MultiTree_GetChild(level1_root, 0, &level2_root);
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_OK, res2_GetChild);
    STRING_empty(global_bufferTemp);
    auto res2_GetName = MultiTree_GetName(level2_root, global_bufferTemp);
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_OK, res2_GetName);
    ASSERT_ARE_EQUAL(char_ptr, CHILD31NAME, STRING_c_str(global_bufferTemp));

    auto res2_GetValue = MultiTree_GetValue(level2_root, (const void**)&nodeValue);
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_EMPTY_VALUE, res2_GetValue); /*because CHILD3 doesn't have a value*/

    size_t level2_ChildCount = 0;
    auto res2_GetChildCount = MultiTree_GetChildCount(level2_root, &level2_ChildCount);
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_OK, res2_GetChildCount);
    ASSERT_ARE_EQUAL(size_t, (size_t)1, level2_ChildCount);

    /*check lvl 3*/
    MULTITREE_HANDLE level3_root;
    auto res3_GetChild = MultiTree_GetChild(level2_root, 0, &level3_root);
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_OK, res3_GetChild);
    STRING_empty(global_bufferTemp);
    auto res3_GetName = MultiTree_GetName(level3_root, global_bufferTemp);
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_OK, res3_GetName);
    ASSERT_ARE_EQUAL(char_ptr, CHILD314NAME, STRING_c_str(global_bufferTemp));

    auto res3_GetValue = MultiTree_GetValue(level3_root, (const void**)&nodeValue);
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_OK, res3_GetValue); /*because CHILD3 doesn't have a value*/
    ASSERT_ARE_EQUAL(char_ptr, CHILD314VALUE, nodeValue);

    size_t level3_ChildCount = 0;
    auto res3_GetChildCount = MultiTree_GetChildCount(level3_root, &level3_ChildCount);
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_OK, res3_GetChildCount);
    ASSERT_ARE_EQUAL(size_t, (size_t)0, level3_ChildCount);

    //cleanup
    MultiTree_Destroy(treeHandle);
    mocks.ResetAllCalls(); /*not caring about what gets called*/
}

/*Tests_SRS_MULTITREE_99_022:[ If a child along the path does not exist, it shall be created.] */
/*Tests_SRS_MULTITREE_99_023:[ The newly created children along the path shall have a NULL value by default.]*/
/*Tests_SRS_MULTITREE_99_017:[ Subsequent names designate hierarchical children in the tree. The last child designates the child that will receive the value.]*/
/*this test goes deep down to child314*/
/*to check if a child is created along the path, several calls are done to check the structure of the tree*/
/*this test creates 2 children (314, 312) and does all teh checks*/
TEST_FUNCTION(MultiTree_AddLeaf_creates_children_in_the_path_2)
{
    ///arrange
    CMultiTreeMocks mocks;
    auto treeHandle = MultiTree_Create(StringClone, StringFree);


    ///act
    auto res = MultiTree_AddLeaf(treeHandle, CHILD314PATH, (void*)CHILD314VALUE);
    auto res_2 = MultiTree_AddLeaf(treeHandle, CHILD312PATH, (void*)CHILD312VALUE);

    ///assert
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_OK, res);
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_OK, res_2);

    /*for every level until the last level the following shall be checked:
    - the name
    - the value is NULL
    - there is 1 child
    */

    /*for the last level is hall be checked that there are 2 children and that they don't have other children and that they
    have the values and names*/

    auto level0_root = treeHandle;
    /*check lvl 0*/
    STRING_empty(global_bufferTemp);
    auto res0_GetName = MultiTree_GetName(level0_root, global_bufferTemp);
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_EMPTY_CHILD_NAME, res0_GetName);  /*because ROOT node cannot have a name*/

    const char* nodeValue;
    auto res0_GetValue = MultiTree_GetValue(level0_root, (const void**)&nodeValue);
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_EMPTY_VALUE, res0_GetValue); /*because ROOT node cannot have a value*/

    size_t level0_ChildCount = 0;
    auto res0_GetChildCount = MultiTree_GetChildCount(level0_root, &level0_ChildCount);
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_OK, res0_GetChildCount);
    ASSERT_ARE_EQUAL(size_t, (size_t)1, level0_ChildCount);

    /*check lvl 1*/
    MULTITREE_HANDLE level1_root;
    auto res1_GetChild = MultiTree_GetChild(level0_root, 0, &level1_root);
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_OK, res1_GetChild);
    STRING_empty(global_bufferTemp);
    auto res1_GetName = MultiTree_GetName(level1_root, global_bufferTemp);
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_OK, res1_GetName);
    ASSERT_ARE_EQUAL(char_ptr, CHILD3NAME, STRING_c_str(global_bufferTemp));

    auto res1_GetValue = MultiTree_GetValue(level1_root, (const void**)&nodeValue);
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_EMPTY_VALUE, res1_GetValue); /*because CHILD3 doesn't have a value*/

    size_t level1_ChildCount = 0;
    auto res1_GetChildCount = MultiTree_GetChildCount(level1_root, &level1_ChildCount);
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_OK, res1_GetChildCount);
    ASSERT_ARE_EQUAL(size_t, (size_t)1, level1_ChildCount);

    /*check lvl 2*/
    MULTITREE_HANDLE level2_root;
    auto res2_GetChild = MultiTree_GetChild(level1_root, 0, &level2_root);
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_OK, res2_GetChild);
    STRING_empty(global_bufferTemp);
    auto res2_GetName = MultiTree_GetName(level2_root, global_bufferTemp);
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_OK, res2_GetName);
    ASSERT_ARE_EQUAL(char_ptr, CHILD31NAME, STRING_c_str(global_bufferTemp));

    auto res2_GetValue = MultiTree_GetValue(level2_root, (const void**)&nodeValue);
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_EMPTY_VALUE, res2_GetValue); /*because CHILD3 doesn't have a value*/

    size_t level2_ChildCount = 0;
    auto res2_GetChildCount = MultiTree_GetChildCount(level2_root, &level2_ChildCount);
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_OK, res2_GetChildCount);
    ASSERT_ARE_EQUAL(size_t, (size_t)2, level2_ChildCount);

    /*check lvl 3*/
    MULTITREE_HANDLE level3_root;
    bool child314Matched = false;
    bool child312Matched = false;

    for (size_t i = 0; i < level2_ChildCount; i++)
    {
        auto res3_GetChild = MultiTree_GetChild(level2_root, i, &level3_root);
        ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_OK, res3_GetChild);

        STRING_empty(global_bufferTemp);
        auto res3_GetName = MultiTree_GetName(level3_root, global_bufferTemp);
        ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_OK, res3_GetName);
        if (strcmp(CHILD312NAME, STRING_c_str(global_bufferTemp)) == 0)
        {
            /*so it is  child312...*/
            child312Matched = true;
            auto res3_GetValue = MultiTree_GetValue(level3_root, (const void**)&nodeValue);
            ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_OK, res3_GetValue);
            ASSERT_ARE_EQUAL(char_ptr, CHILD312VALUE, nodeValue);
        }

        if (strcmp(CHILD314NAME, STRING_c_str(global_bufferTemp)) == 0)
        {
            /*so it is  child312...*/
            child314Matched = true;
            auto res3_GetValue = MultiTree_GetValue(level3_root, (const void**)&nodeValue);
            ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_OK, res3_GetValue);
            ASSERT_ARE_EQUAL(char_ptr, CHILD314VALUE, nodeValue);
        }

        /*any child needs to have exactly 0 children on this level*/
        size_t mustBeZero = 1;
        auto res3_GetChildCount = MultiTree_GetChildCount(level3_root, &mustBeZero);
        ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_OK, res3_GetChildCount);
        ASSERT_ARE_EQUAL(size_t, (size_t)0, mustBeZero);

    }

    ASSERT_IS_TRUE(child312Matched);
    ASSERT_IS_TRUE(child314Matched);

    //cleanup
    MultiTree_Destroy(treeHandle);
    mocks.ResetAllCalls(); /*not caring about what gets called*/
}

/*Tests_SRS_MULTITREE_99_024:[ if a child name is empty (such as in  '/child1//child12'), MULTITREE_EMPTY_CHILD_NAME shall be returned.]*/
TEST_FUNCTION(MultiTree_AddLeaf_with_empty_name_fails)
{
    ///arrange
    CMultiTreeMocks mocks;
    auto treeHandle = MultiTree_Create(StringClone, StringFree);

    ///act
    auto res = MultiTree_AddLeaf(treeHandle, "/", (void*)"someValue");

    ///assert
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_EMPTY_CHILD_NAME, res);

    //cleanup
    MultiTree_Destroy(treeHandle);
    mocks.ResetAllCalls();
}

/*Tests_SRS_MULTITREE_99_024:[ if a child name is empty (such as in  '/child1//child12'), MULTITREE_EMPTY_CHILD_NAME shall be returned.]*/
TEST_FUNCTION(MultiTree_AddLeaf_with_empty_name_fails_2)
{
    ///arrange
    CMultiTreeMocks mocks;
    auto treeHandle = MultiTree_Create(StringClone, StringFree);

    ///act
    auto res = MultiTree_AddLeaf(treeHandle, "/child1/", (void*)"someValue");

    ///assert
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_EMPTY_CHILD_NAME, res);

    //cleanup
    MultiTree_Destroy(treeHandle);
    mocks.ResetAllCalls();
}

/*Tests_SRS_MULTITREE_99_024:[ if a child name is empty (such as in  '/child1//child12', MULTITREE_EMPTY_CHILD_NAME shall be returned.]*/
TEST_FUNCTION(MultiTree_AddLeaf_with_empty_name_fails_3)
{
    ///arrange
    CMultiTreeMocks mocks;
    auto treeHandle = MultiTree_Create(StringClone, StringFree);

    ///act
    auto res = MultiTree_AddLeaf(treeHandle, "//child1", (void*)"someValue");

    ///assert
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_EMPTY_CHILD_NAME, res);

    //cleanup
    MultiTree_Destroy(treeHandle);
    mocks.ResetAllCalls();
}

/*Tests_SRS_MULTITREE_99_024:[ if a child name is empty (such as in  '/child1//child12'), MULTITREE_EMPTY_CHILD_NAME shall be returned.]*/
TEST_FUNCTION(MultiTree_AddLeaf_with_empty_name_fails_3_dot_5)
{
    ///arrange
    CMultiTreeMocks mocks;
    auto treeHandle = MultiTree_Create(StringClone, StringFree);

    ///act
    auto res = MultiTree_AddLeaf(treeHandle, "///child1", (void*)"someValue");

    ///assert
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_EMPTY_CHILD_NAME, res);

    //cleanup
    MultiTree_Destroy(treeHandle);
    mocks.ResetAllCalls();
}

/*Tests_SRS_MULTITREE_99_024:[ if a child name is empty (such as in  '/child1//child12'), MULTITREE_EMPTY_CHILD_NAME shall be returned.]*/
TEST_FUNCTION(MultiTree_AddLeaf_with_empty_name_fails_4)
{
    ///arrange
    CMultiTreeMocks mocks;
    auto treeHandle = MultiTree_Create(StringClone, StringFree);

    ///act
    auto res = MultiTree_AddLeaf(treeHandle, "/child1//child12", (void*)"someValue");

    ///assert
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_EMPTY_CHILD_NAME, res);

    //cleanup
    MultiTree_Destroy(treeHandle);
    mocks.ResetAllCalls();
}

/*Tests_SRS_MULTITREE_99_024:[ if a child name is empty (such as in  '/child1//child12'), MULTITREE_EMPTY_CHILD_NAME shall be returned.]*/
TEST_FUNCTION(MultiTree_AddLeaf_with_empty_name_fails_5)
{
    ///arrange
    CMultiTreeMocks mocks;
    auto treeHandle = MultiTree_Create(StringClone, StringFree);

    ///act
    auto res = MultiTree_AddLeaf(treeHandle, "/child1///child12", (void*)"someValue");

    ///assert
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_EMPTY_CHILD_NAME, res);

    //cleanup
    MultiTree_Destroy(treeHandle);
    mocks.ResetAllCalls();
}

/*Tests_SRS_MULTITREE_99_014:[DestinationPath is a string in the following format: /child1/child12 or child1/child12] */
/*we shall use the alternate paths to construct the complete tree*/
TEST_FUNCTION(MultiTree_AddLeaf_with_alternate_names_succeeds)
{
    ///arrange
    CMultiTreeMocks mocks;
    auto treeHandle = MultiTree_Create(StringClone, StringFree);

    ///act
    auto res1 = MultiTree_AddLeaf(treeHandle, CHILD11PATH_ALTERNATE, (void*)CHILD11VALUE);
    auto res2 = MultiTree_AddLeaf(treeHandle, CHILD12PATH_ALTERNATE, (void*)CHILD12VALUE);
    auto res3 = MultiTree_AddLeaf(treeHandle, CHILD2PATH_ALTERNATE, (void*)CHILD11VALUE);
    auto res4 = MultiTree_AddLeaf(treeHandle, CHILD311PATH_ALTERNATE, (void*)CHILD311VALUE);
    auto res5 = MultiTree_AddLeaf(treeHandle, CHILD312PATH_ALTERNATE, (void*)CHILD312VALUE);
    auto res6 = MultiTree_AddLeaf(treeHandle, CHILD313PATH_ALTERNATE, (void*)CHILD313VALUE);
    auto res7 = MultiTree_AddLeaf(treeHandle, CHILD314PATH_ALTERNATE, (void*)CHILD313VALUE);
    
    ///assert
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_OK, res1);
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_OK, res2);
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_OK, res3);
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_OK, res4);
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_OK, res5);
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_OK, res6);
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_OK, res7);
    
    //cleanup
    MultiTree_Destroy(treeHandle);
    mocks.ResetAllCalls();
}

/*Tests_SRS_MULTITREE_99_014:[DestinationPath is a string in the following format: /child1/child12 or child1/child12] */
/*we shall use the alternate paths to construct the complete tree*/
TEST_FUNCTION(MultiTree_AddLeaf_with_natural_names_succeeds)
{
    ///arrange
    CMultiTreeMocks mocks;
    auto treeHandle = MultiTree_Create(StringClone, StringFree);

    ///act
    auto res1 = MultiTree_AddLeaf(treeHandle, CHILD11PATH, (void*)CHILD11VALUE);
    auto res2 = MultiTree_AddLeaf(treeHandle, CHILD12PATH, (void*)CHILD12VALUE);
    auto res3 = MultiTree_AddLeaf(treeHandle, CHILD2PATH, (void*)CHILD11VALUE);
    auto res4 = MultiTree_AddLeaf(treeHandle, CHILD311PATH, (void*)CHILD311VALUE);
    auto res5 = MultiTree_AddLeaf(treeHandle, CHILD312PATH, (void*)CHILD312VALUE);
    auto res6 = MultiTree_AddLeaf(treeHandle, CHILD313PATH, (void*)CHILD313VALUE);
    auto res7 = MultiTree_AddLeaf(treeHandle, CHILD314PATH, (void*)CHILD313VALUE);

    ///assert
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_OK, res1);
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_OK, res2);
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_OK, res3);
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_OK, res4);
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_OK, res5);
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_OK, res6);
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_OK, res7);

    //cleanup
    MultiTree_Destroy(treeHandle);
    mocks.ResetAllCalls();
}

/*Tests_SRS_MULTITREE_99_029:[ This function writes in *count the number of direct children for a tree node specified by the parameter treeHandle]*/
/*Tests_SRS_MULTITREE_99_035:[ The function shall return MULTITREE_OK when *count contains the number of children of the node pointed to be parameter treeHandle.]*/
TEST_FUNCTION(MultiTree_GetChildCount_for_a_new_tree_returns_0)
{
    ///arrange
    CMultiTreeMocks mocks;
    size_t count = 100;
    MULTITREE_HANDLE treeHandle = MultiTree_Create(StringClone, StringFree);

    ///act
    auto res = MultiTree_GetChildCount(treeHandle, &count);

    ///assert
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_OK, res);
    ASSERT_ARE_EQUAL(size_t, (size_t)0, count);

    ///cleanup
    MultiTree_Destroy(treeHandle);
    mocks.ResetAllCalls();
}

/*Tests_SRS_MULTITREE_99_029:[ This function writes in *count the number of direct children for a tree node specified by the parameter treeHandle]*/
/*Tests_SRS_MULTITREE_99_035:[ The function shall return MULTITREE_OK when *count contains the number of children of the node pointed to be parameter treeHandle.]*/
TEST_FUNCTION(MultiTree_GetChildCount_for_a_tree_with_1_child_return_1)
{
    ///arrange
    CMultiTreeMocks mocks;
    size_t count = 100;
    MULTITREE_HANDLE treeHandle = MultiTree_Create(StringClone, StringFree);
    (void)MultiTree_AddLeaf(treeHandle, CHILD1PATH, (void*)CHILD1VALUE);

    ///act
    auto res = MultiTree_GetChildCount(treeHandle, &count);

    ///assert
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_OK, res);
    ASSERT_ARE_EQUAL(size_t, (size_t)1, count);

    ///cleanup
    MultiTree_Destroy(treeHandle);
    mocks.ResetAllCalls();
}

/*Tests_SRS_MULTITREE_99_029:[ This function writes in *count the number of direct children for a tree node specified by the parameter treeHandle]*/
/*Tests_SRS_MULTITREE_99_035:[ The function shall return MULTITREE_OK when *count contains the number of children of the node pointed to be parameter treeHandle.]*/
TEST_FUNCTION(MultiTree_GetChildCount_for_a_tree_with_2_children_return_2)
{
    ///arrange
    CMultiTreeMocks mocks;
    size_t count = 100;
    MULTITREE_HANDLE treeHandle = MultiTree_Create(StringClone, StringFree);
    (void)MultiTree_AddLeaf(treeHandle, CHILD1PATH, (void*)CHILD1VALUE);
    (void)MultiTree_AddLeaf(treeHandle, CHILD2PATH, (void*)CHILD2VALUE);

    ///act
    auto res = MultiTree_GetChildCount(treeHandle, &count);

    ///assert
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_OK, res);
    ASSERT_ARE_EQUAL(size_t, (size_t)2, count);

    ///cleanup
    MultiTree_Destroy(treeHandle);
    mocks.ResetAllCalls();
}

/*Tests_SRS_MULTITREE_99_027:[If treeHandle is NULL, the function returns MULTITREE_INVALID_ARG.]*/
TEST_FUNCTION(MultiTree_GetChildCount_with_NULL_handle_fails)
{
    ///arrange
    CMultiTreeMocks mocks;
    size_t count;

    ///act
    auto res = MultiTree_GetChildCount(NULL, &count);

    ///assert
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_INVALID_ARG, res);
}

/*Tests_SRS_MULTITREE_99_028:[If parameter count is NULL, the function returns MULTITREE_INVALID_ARG.]*/
TEST_FUNCTION(MultiTree_GetChildCount_with_NULL_count_fails)
{
    ///arrange
    CMultiTreeMocks mocks;
    MULTITREE_HANDLE treeHandle = MultiTree_Create(StringClone, StringFree);

    ///act
    auto res = MultiTree_GetChildCount(treeHandle, NULL);

    ///assert
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_INVALID_ARG, res);

    ///cleanup
    MultiTree_Destroy(treeHandle);
    mocks.ResetAllCalls();
}

/*Tests_SRS_MULTITREE_99_031:[ If parameter treeHandle is NULL, the function returns MULTITREE_INVALID_ARG.]*/
TEST_FUNCTION(MultiTree_GetChild_with_NULL_treeHandle_fails)
{
    ///arrange
    CMultiTreeMocks mocks;
    MULTITREE_HANDLE childHandle = NULL;

    ///act
    auto res = MultiTree_GetChild(NULL, 0, &childHandle);

    ///assert
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_INVALID_ARG, res);
}

/*Tests_SRS_MULTITREE_99_032:[ If parameter index is out of range, the function shall return MULTITREE_OUT_OF_RANGE_INDEX]*/
TEST_FUNCTION(MultiTree_GetChild_with_index_out_of_range_fails_1)
{
    ///arrange
    CMultiTreeMocks mocks;
    MULTITREE_HANDLE treeHandle = MultiTree_Create(StringClone, StringFree);
    MULTITREE_HANDLE childHandle = NULL;

    ///act
    auto res = MultiTree_GetChild(treeHandle, 0, &childHandle); /*0 is out of range because the tree has no children*/

    ///assert
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_INVALID_ARG, res);

    ///cleanup
    MultiTree_Destroy(treeHandle);
    mocks.ResetAllCalls();
}

/*Tests_SRS_MULTITREE_99_032:[ If parameter index is out of range, the function shall return MULTITREE_OUT_OF_RANGE_INDEX]*/
TEST_FUNCTION(MultiTree_GetChild_with_index_out_of_range_fails_2)
{
    ///arrange
    CMultiTreeMocks mocks;
    MULTITREE_HANDLE treeHandle = MultiTree_Create(StringClone, StringFree);
    MULTITREE_HANDLE childHandle = NULL;

    ///act
    auto res = MultiTree_GetChild(treeHandle, 1, &childHandle); /*1 is out of range because the tree has no children anyway*/

    ///assert
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_INVALID_ARG, res);

    ///cleanup
    MultiTree_Destroy(treeHandle);
    mocks.ResetAllCalls();
}

/*Tests_SRS_MULTITREE_99_032:[ If parameter index is out of range, the function shall return MULTITREE_OUT_OF_RANGE_INDEX]*/
TEST_FUNCTION(MultiTree_GetChild_with_index_out_of_range_fails_3)
{
    ///arrange
    CMultiTreeMocks mocks;
    MULTITREE_HANDLE treeHandle = MultiTree_Create(StringClone, StringFree);
    MULTITREE_HANDLE childHandle = NULL;
    (void)MultiTree_AddLeaf(treeHandle, "child1", (void*)"v1");

    ///act
    auto res = MultiTree_GetChild(treeHandle, 1, &childHandle); /*1 is out of range because the tree has 1 child (that lives at index 0)*/

    ///assert
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_INVALID_ARG, res);

    ///cleanup
    MultiTree_Destroy(treeHandle);
    mocks.ResetAllCalls();
}

/*Tests_SRS_MULTITREE_99_032:[ If parameter index is out of range, the function shall return MULTITREE_OUT_OF_RANGE_INDEX]*/
TEST_FUNCTION(MultiTree_GetChild_with_index_out_of_range_fails_4)
{
    ///arrange
    CMultiTreeMocks mocks;
    MULTITREE_HANDLE treeHandle = MultiTree_Create(StringClone, StringFree);
    MULTITREE_HANDLE childHandle = NULL;
    (void)MultiTree_AddLeaf(treeHandle, "child1", (void*)"v1");

    ///act
    auto res = MultiTree_GetChild(treeHandle, 2, &childHandle); /*2 is out of range because the tree has 1 child (that lives at index 0)*/

    ///assert
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_INVALID_ARG, res);

    ///cleanup
    MultiTree_Destroy(treeHandle);
    mocks.ResetAllCalls();
}

/*Tests_SRS_MULTITREE_99_032:[ If parameter index is out of range, the function shall return MULTITREE_OUT_OF_RANGE_INDEX]*/
TEST_FUNCTION(MultiTree_GetChild_with_index_out_of_range_fails_5)
{
    ///arrange
    CMultiTreeMocks mocks;
    MULTITREE_HANDLE treeHandle = MultiTree_Create(StringClone, StringFree);
    MULTITREE_HANDLE childHandle = NULL;
    (void)MultiTree_AddLeaf(treeHandle, "child1", (void*)"v1");
    (void)MultiTree_AddLeaf(treeHandle, "child2", (void*)"v2");

    ///act
    auto res = MultiTree_GetChild(treeHandle, 2, &childHandle); /*2 is out of range because the tree has 2 children (that lives at index 0 and 1)*/

    ///assert
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_INVALID_ARG, res);

    ///cleanup
    MultiTree_Destroy(treeHandle);
    mocks.ResetAllCalls();
}

/*Tests_SRS_MULTITREE_99_033:[ If parameter childHandle is NULL, the function shall return MULTITREE_INVALID_ARG.]*/
TEST_FUNCTION(MultiTree_GetChild_with_NULL_child_handle_fails)
{
    ///arrange
    CMultiTreeMocks mocks;
    MULTITREE_HANDLE treeHandle = MultiTree_Create(StringClone, StringFree);
    (void)MultiTree_AddLeaf(treeHandle, "child1", (void*)"v1");

    ///act
    auto res = MultiTree_GetChild(treeHandle, 0, NULL);

    ///assert
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_INVALID_ARG, res);

    ///cleanup
    MultiTree_Destroy(treeHandle);
    mocks.ResetAllCalls();
}

/*Tests_SRS_MULTITREE_99_030:[ This function writes in *childHandle parameter the 'index'th child of the node pointed to by parameter treeHandle]*/
/*Tests_SRS_MULTITREE_99_035:[ The function returns MULTITREE_OK when *childHandle contains a handle to the 'index'th child of the tree designated by parameter treeHandle.]*/
TEST_FUNCTION(MultiTree_GetChild_succeeds)
{
    ///arrange
    CMultiTreeMocks mocks;
    MULTITREE_HANDLE treeHandle = MultiTree_Create(StringClone, StringFree);
    MULTITREE_HANDLE childHandle;
    (void)MultiTree_AddLeaf(treeHandle, CHILD1PATH, (void*)CHILD1VALUE);

    ///act
    auto res = MultiTree_GetChild(treeHandle, 0, &childHandle);

    ///assert
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_OK, res);

    /*verify that the child is really "the one"*/
    const char* nodeValue;
    auto res2 = MultiTree_GetValue(childHandle, (const void**)&nodeValue);
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_OK, res2);
    ASSERT_ARE_EQUAL(char_ptr, CHILD1VALUE, nodeValue);

    auto res3 = MultiTree_GetName(childHandle, global_bufferTemp);
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_OK, res3);
    ASSERT_ARE_EQUAL(char_ptr, CHILD1NAME, STRING_c_str(global_bufferTemp));

    size_t childCount = 111;
    auto res4 = MultiTree_GetChildCount(childHandle, &childCount);
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_OK, res4);
    ASSERT_ARE_EQUAL(size_t, (size_t)0, childCount);

    ///cleanup
    MultiTree_Destroy(treeHandle);
    mocks.ResetAllCalls();
}

/*SRS_MULTITREE_99_037:[ If treeHandle is NULL, the function shall return MULTITREE_INVALID_ARG.]*/
TEST_FUNCTION(MultiTree_GetName_with_NULL_handle_fails)
{
    ///arrange
    CMultiTreeMocks mocks;

    ///act
    auto res = MultiTree_GetName(NULL, global_bufferTemp);

    ///assert
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_INVALID_ARG, res);
}

/*Tests_SRS_MULTITREE_99_038:[If destination is NULL, the function shall return MULTITREE_INVALID_ARG.]*/
TEST_FUNCTION(MultiTree_GetName_with_NULL_destination_fails)
{
    ///arrange
    CMultiTreeMocks mocks;
    MULTITREE_HANDLE treeHandle = MultiTree_Create(StringClone, StringFree);
    (void)MultiTree_AddLeaf(treeHandle, "child1", (void*)"value1");
    MULTITREE_HANDLE childHandle;
    (void)MultiTree_GetChild(treeHandle, 0, &childHandle);

    ///act
    auto res = MultiTree_GetName(childHandle, NULL);

    ///assert
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_INVALID_ARG, res);

    ///cleanup
    MultiTree_Destroy(treeHandle);
    mocks.ResetAllCalls();
}

/*Tests_SRS_MULTITREE_99_051:[ The function returns MULTITREE_EMPTY_CHILD_NAME when used with the root of the tree.]*/
TEST_FUNCTION(MultiTree_GetName_for_root_returns_EMPTY_NAME)
{
    ///arrange
    CMultiTreeMocks mocks;
    MULTITREE_HANDLE treeHandle = MultiTree_Create(StringClone, StringFree);

    ///act
    auto res = MultiTree_GetName(treeHandle, global_bufferTemp);

    ///assert
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_EMPTY_CHILD_NAME, res);

    ///cleanup
    MultiTree_Destroy(treeHandle);
    mocks.ResetAllCalls();
}

/*Tests_SRS_MULTITREE_99_039:[ The function returns MULTITREE_OK when destination contains the name of the root node of the tree designated by treeHandle parameter.]*/
/*Tests_SRS_MULTITREE_99_036:[ This function fills the buffer pointed to by parameter destination with the name of the root node of the tree designated by parameter treeHandle.]*/
TEST_FUNCTION(MultiTree_GetName_succeeds)
{
    ///arrange
    CMultiTreeMocks mocks;
    MULTITREE_HANDLE treeHandle = MultiTree_Create(StringClone, StringFree);
    (void)MultiTree_AddLeaf(treeHandle, "child1", (void*)"value1");
    MULTITREE_HANDLE childHandle;
    (void)MultiTree_GetChild(treeHandle, 0, &childHandle);

    ///act
    auto res = MultiTree_GetName(childHandle, global_bufferTemp);

    ///assert
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_OK, res);
    ASSERT_ARE_EQUAL(char_ptr, "child1", STRING_c_str(global_bufferTemp));

    ///cleanup
    MultiTree_Destroy(treeHandle);
    mocks.ResetAllCalls();
}

/*Tests_SRS_MULTITREE_99_042:[ If treeHandle is NULL, the function shall return MULTITREE_INVALID_ARG.]*/
TEST_FUNCTION(MultiTree_GetValue_with_NULL_handle_fails)
{
    ///arrange
    CMultiTreeMocks mocks;

    ///act
    const char* nodeValue;
    auto res = MultiTree_GetValue(NULL, (const void**)&nodeValue);

    ///assert
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_INVALID_ARG, res);
}

/*Tests_SRS_MULTITREE_99_044:[ If there is no value in the node then MULTITREE_EMPTY_VALUE shall be returned.]*/
TEST_FUNCTION(MultiTree_GetValue_with_empty_value_fails_1)
{
    ///arrange
    CMultiTreeMocks mocks;
    MULTITREE_HANDLE treeHandle = MultiTree_Create(StringClone, StringFree);

    ///act
    const char* nodeValue;
    auto res = MultiTree_GetValue(treeHandle, (const void**)&nodeValue);

    ///assert
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_EMPTY_VALUE, res);

    ///cleanup
    MultiTree_Destroy(treeHandle);
    mocks.ResetAllCalls();
}

/*Tests_SRS_MULTITREE_99_044:[ If there is no value in the node then MULTITREE_EMPTY_VALUE shall be returned.]*/
TEST_FUNCTION(MultiTree_GetValue_with_empty_value_fails_2)
{
    ///arrange
    CMultiTreeMocks mocks;
    MULTITREE_HANDLE childHandle, treeHandle = MultiTree_Create(StringClone, StringFree);
    (void)MultiTree_AddLeaf(treeHandle, "c1/c2", (void*)"v2");
    (void)MultiTree_GetChild(treeHandle, 0, &childHandle);

    ///act
    const char* nodeValue;
    auto res = MultiTree_GetValue(childHandle, (const void**)&nodeValue);

    ///assert
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_EMPTY_VALUE, res);

    ///cleanup
    MultiTree_Destroy(treeHandle);
    mocks.ResetAllCalls();
}


/*Tests_SRS_MULTITREE_99_043:[ If destination is NULL, the function shall return MULTITREE_INVALID_ARG.]*/
TEST_FUNCTION(MultiTree_GetValue_with_NULL_buffer_fails)
{
    ///arrange
    CMultiTreeMocks mocks;
    MULTITREE_HANDLE treeHandle = MultiTree_Create(StringClone, StringFree);

    ///act
    auto res = MultiTree_GetValue(treeHandle, NULL);

    ///assert
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_INVALID_ARG, res);

    ///cleanup
    MultiTree_Destroy(treeHandle);
    mocks.ResetAllCalls();
}

/*Tests_SRS_MULTITREE_99_043:[ If destination is NULL, the function shall return MULTITREE_INVALID_ARG.]*/
TEST_FUNCTION(MultiTree_GetValue_with_root_returns_EMPTY_VALUE)
{
    ///arrange
    CMultiTreeMocks mocks;
    MULTITREE_HANDLE treeHandle = MultiTree_Create(StringClone, StringFree);

    ///act
    const char* nodeValue;
    auto res = MultiTree_GetValue(treeHandle, (const void**)&nodeValue);

    ///assert
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_EMPTY_VALUE, res);

    ///cleanup
    MultiTree_Destroy(treeHandle);
    mocks.ResetAllCalls();
}

/*Tests_SRS_MULTITREE_99_045:[ The function shall return MULTITREE_OK when destination contains the value of the root node of the tree designated by treeHandle parameter.]*/
/*Tests_SRS_MULTITREE_99_041:[This function updates the *destination parameter to the internally stored value.]*/
TEST_FUNCTION(MultiTree_GetValue_suceeds)
{
    ///arrange
    CMultiTreeMocks mocks;
    MULTITREE_HANDLE treeHandle = MultiTree_Create(StringClone, StringFree);
    (void)MultiTree_AddLeaf(treeHandle, "child1", (void*)"value1");
    MULTITREE_HANDLE childHandle;
    (void)MultiTree_GetChild(treeHandle, 0, &childHandle);

    ///act
    const char* nodeValue;
    auto res = MultiTree_GetValue(childHandle, (const void**)&nodeValue);

    ///assert
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_OK, res);
    ASSERT_ARE_EQUAL(char_ptr, "value1", nodeValue);

    ///cleanup
    MultiTree_Destroy(treeHandle);
    mocks.ResetAllCalls();
}

/*Tests_SRS_MULTITREE_99_047:[This function frees any system resource used by the tree designated by parameter treeHandle*/
TEST_FUNCTION(MultiTree_Destroy_with_NULL_handle_Does_Not_Crash_And_Burn)
{
    ///arrange
    CMultiTreeMocks mocks;

    ///act
    MultiTree_Destroy(NULL);

    ///assert
    // no explicit assert, no exception expected
}

/*Tests_SRS_MULTITREE_99_047:[This function frees any system resource used by the tree designated by parameter treeHandle*/
/*Tests_SRS_MULTITREE_99_048:[ The function returns MULTITREE_OK when all system resources used by the tree have been freed.]*/
TEST_FUNCTION(MultiTree_Destroy_with_an_empty_tree_calls_free_once)
{
    ///arrange
    CMultiTreeMocks mocks;
    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(0))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    MULTITREE_HANDLE treeHandle = MultiTree_Create(StringClone, StringFree);


    ///act
    MultiTree_Destroy(treeHandle);

    ///assert

}

/*Tests_SRS_MULTITREE_99_047:[This function frees any system resource used by the tree designated by parameter treeHandle*/
/*Tests_SRS_MULTITREE_99_048:[ The function returns MULTITREE_OK when all system resources used by the tree have been freed.]*/
TEST_FUNCTION(MultiTree_Destroy_with_one_child_tree_calls_free_5_times)
{
    /*the 5 times are:
    -child name
    -child value
    -child itself
    - children in root
    - root itself*/
    ///arrange
    CMultiTreeMocks mocks; 

    MULTITREE_HANDLE treeHandle = MultiTree_Create(StringClone, StringFree);
    (void)MultiTree_AddLeaf(treeHandle, "child", (void*)"value");

    MultiTree_Destroy(treeHandle);

    ///assert
    mocks.ResetAllCalls();

}

/*Tests_SRS_MULTITREE_99_047:[This function frees any system resource used by the tree designated by parameter treeHandle*/
/*Tests_SRS_MULTITREE_99_048:[ The function returns MULTITREE_OK when all system resources used by the tree have been freed.]*/
TEST_FUNCTION(MultiTree_Destroy_with_child_child_tree_calls_free_8_times)
{
    /*the 8 times are:
    -child child name
    -child child value
    -child child itself
    -child name
    -child children   (notice no call to free child value)
    -child itself
    -root children
    -root itself*/
    ///arrange
    CMultiTreeMocks mocks;

    MULTITREE_HANDLE treeHandle = MultiTree_Create(StringClone, StringFree);
    (void)MultiTree_AddLeaf(treeHandle, "child/childChild", (void*)"value");

    ///act
    MultiTree_Destroy(treeHandle);

    ///assert
    mocks.ResetAllCalls();
}

/*Tests_SRS_MULTITREE_99_047:[This function frees any system resource used by the tree designated by parameter treeHandle*/
/*Tests_SRS_MULTITREE_99_048:[ The function returns MULTITREE_OK when all system resources used by the tree have been freed.]*/
TEST_FUNCTION(MultiTree_Destroy_with_2_child_child_tree_calls_free_8_times)
{
    /*the 8 times are:
    -child1 name
    -child1 value
    -child1 itself
    -child2 name
    -child2 value
    -child2 itself
    -root children
    -root itself*/

    ///arrange
    CMultiTreeMocks mocks;

    MULTITREE_HANDLE treeHandle = MultiTree_Create(StringClone, StringFree);
    (void)MultiTree_AddLeaf(treeHandle, "child1", (void*)"value1");
    (void)MultiTree_AddLeaf(treeHandle, "child2", (void*)"value2");

    ///act
    MultiTree_Destroy(treeHandle);

    ///assert
    mocks.ResetAllCalls();
}

/* MultiTree_AddChild */

/* Tests_SRS_MULTITREE_99_055:[ If any argument is NULL, MultiTree_AddChild shall return MULTITREE_INVALID_ARG.] */
TEST_FUNCTION(MultiTree_AddChild_With_NULL_TreeHandle_Fails)
{
    ///arrange
    CMultiTreeMocks mocks;

    MULTITREE_HANDLE childHandle;

    ///act
    MULTITREE_RESULT result = MultiTree_AddChild(NULL, "child", &childHandle);

    ///assert
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_INVALID_ARG, result);
}

/* Tests_SRS_MULTITREE_99_055:[ If any argument is NULL, MultiTree_AddChild shall return MULTITREE_INVALID_ARG.] */
TEST_FUNCTION(MultiTree_AddChild_With_NULL_ChildName_Fails)
{
    ///arrange
    CMultiTreeMocks mocks;
    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(0)) /*because MultiTree_Create*/
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*because MultiTree_Destroy*/
        .IgnoreArgument(1);


    MULTITREE_HANDLE treeHandle = MultiTree_Create(StringClone, StringFree);
    MULTITREE_HANDLE childHandle;

    ///act
    MULTITREE_RESULT result = MultiTree_AddChild(treeHandle, NULL, &childHandle);

    ///assert
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_INVALID_ARG, result);

    MultiTree_Destroy(treeHandle);
}

/* Tests_SRS_MULTITREE_99_055:[ If any argument is NULL, MultiTree_AddChild shall return MULTITREE_INVALID_ARG.] */
TEST_FUNCTION(MultiTree_AddChild_With_NULL_ChildHandle_Fails)
{
    ///arrange
    CMultiTreeMocks mocks;
    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(0)) /*because MultiTree_Create*/
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*because MultiTree_Destroy*/
        .IgnoreArgument(1);

    MULTITREE_HANDLE treeHandle = MultiTree_Create(StringClone, StringFree);

    ///act
    MULTITREE_RESULT result = MultiTree_AddChild(treeHandle, "child", NULL);

    ///assert
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_INVALID_ARG, result);

    MultiTree_Destroy(treeHandle);
}

/* Tests_SRS_MULTITREE_99_053:[ MultiTree_AddChild shall add a new node with the name childName to the multi tree node identified by treeHandle] */
/* Tests_SRS_MULTITREE_99_062:[ The new node handle shall be returned in the childHandle argument.] */
/* Tests_SRS_MULTITREE_99_054:[ On success, MultiTree_AddChild shall return MULTITREE_OK.] */
TEST_FUNCTION(MultiTree_AddChild_With_Correct_Arguments_Succeeds)
{
    ///arrange
    CMultiTreeMocks mocks;
    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(0)) /*because MultiTree_Create*/
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*because MultiTree_Destroy*/
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(0)) /*because create the child*/
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*because destroy the child*/
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, gballoc_realloc(NULL, sizeof(MULTITREE_HANDLE))); /*because insertion of child node in the array of children in the parent*/
    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*because insertion of child node in the array of children in the parent*/
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(6)); /*this is clone of "child" string*/
    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*this is clone of "child" string*/
        .IgnoreArgument(1);

    MULTITREE_HANDLE treeHandle = MultiTree_Create(StringClone, StringFree);
    MULTITREE_HANDLE childHandle;
    const void* nodeValue;

    ///act
    MULTITREE_RESULT result = MultiTree_AddChild(treeHandle, "child", &childHandle);

    ///assert
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_OK, result);
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_EMPTY_VALUE, MultiTree_GetValue(childHandle, &nodeValue));

    MultiTree_Destroy(treeHandle);
}

/* Tests_SRS_MULTITREE_99_066:[ If the childName argument is an empty string, MultiTree_AddChild shall return MULTITREE_EMPTY_CHILD_NAME.] */
TEST_FUNCTION(MultiTree_AddChild_With_Empty_ChildName_Fails)
{
    ///arrange
    CMultiTreeMocks mocks;
    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(0)) /*because MultiTree_Create*/
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*because MultiTree_Destroy*/
        .IgnoreArgument(1);

    MULTITREE_HANDLE treeHandle = MultiTree_Create(StringClone, StringFree);
    MULTITREE_HANDLE childHandle;

    ///act
    MULTITREE_RESULT result = MultiTree_AddChild(treeHandle, "", &childHandle);

    ///assert
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_EMPTY_CHILD_NAME, result);

    MultiTree_Destroy(treeHandle);
}

/* Tests_SRS_MULTITREE_99_061:[ If a child node with the same name already exists, MultiTree_AddChild shall return MULTITREE_ALREADY_HAS_A_VALUE.] */
TEST_FUNCTION(MultiTree_AddChild_With_Same_ChildName_Twice_Fails)
{
    ///arrange
    CMultiTreeMocks mocks;
    MULTITREE_HANDLE treeHandle = MultiTree_Create(StringClone, StringFree);
    MULTITREE_HANDLE childHandle;

    (void)MultiTree_AddChild(treeHandle, "childName", &childHandle);

    ///act
    MULTITREE_RESULT result = MultiTree_AddChild(treeHandle, "childName", &childHandle);

    ///assert
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_ALREADY_HAS_A_VALUE, result);

    MultiTree_Destroy(treeHandle);

    mocks.ResetAllCalls(); /*not caring about what gets called*/
}

/* Tests_SRS_MULTITREE_99_061:[ If a child node with the same name already exists, MultiTree_AddChild shall return MULTITREE_ALREADY_HAS_A_VALUE.] */
TEST_FUNCTION(MultiTree_AddChild_With_2_Different_Child_Names_Succeeds)
{
    ///arrange
    CMultiTreeMocks mocks;
    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(0)) /*because MultiTree_Create*/
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*because MultiTree_Destroy*/
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(0)) /*because create the child 1 */
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*because destroy the child 1 */
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, gballoc_realloc(NULL, sizeof(MULTITREE_HANDLE))); /*because insertion of child 1 node in the array of children in the parent*/
    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(sizeof("childName1"))); /*this is clone of "childName1" string*/
    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*this is clone of "childName1" string*/
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(0)) /*because create the child 2 */
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*because destroy the child 2*/
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, gballoc_realloc(IGNORED_PTR_ARG, 2 * sizeof(MULTITREE_HANDLE))) /*because insertion of child 2 node in the array of children in the parent*/
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(sizeof("childName2"))); /*this is clone of "child" string*/
    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*this is clone of "child" string*/
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*because realloc of children only has 1 free*/
        .IgnoreArgument(1);


    MULTITREE_HANDLE treeHandle = MultiTree_Create(StringClone, StringFree);
    MULTITREE_HANDLE childHandle;

    (void)MultiTree_AddChild(treeHandle, "childName1", &childHandle);

    ///act
    MULTITREE_RESULT result = MultiTree_AddChild(treeHandle, "childName2", &childHandle);

    ///assert
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_OK, result);

    MultiTree_Destroy(treeHandle);
}

/* MultiTree_GetChildByName */

/* Tests_SRS_MULTITREE_99_065:[ If any argument is NULL, MultiTree_GetChildByName shall return MULTITREE_INVALID_ARG.] */
TEST_FUNCTION(MultiTree_GetChildByName_With_NULL_TreeHandle_Fails)
{
    ///arrange
    CMultiTreeMocks mocks;
    MULTITREE_HANDLE childHandle;

    ///act
    MULTITREE_RESULT result = MultiTree_GetChildByName(NULL, "childName", &childHandle);

    ///assert
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_INVALID_ARG, result);

    mocks.ResetAllCalls();
}

/* Tests_SRS_MULTITREE_99_065:[ If any argument is NULL, MultiTree_GetChildByName shall return MULTITREE_INVALID_ARG.] */
TEST_FUNCTION(MultiTree_GetChildByName_With_NULL_ChildName_Fails)
{
    ///arrange
    CMultiTreeMocks mocks;
    MULTITREE_HANDLE treeHandle = MultiTree_Create(StringClone, StringFree);
    MULTITREE_HANDLE childHandle;

    ///act
    MULTITREE_RESULT result = MultiTree_GetChildByName(treeHandle, NULL, &childHandle);

    ///assert
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_INVALID_ARG, result);

    MultiTree_Destroy(treeHandle);
    mocks.ResetAllCalls();
}

/* Tests_SRS_MULTITREE_99_065:[ If any argument is NULL, MultiTree_GetChildByName shall return MULTITREE_INVALID_ARG.] */
TEST_FUNCTION(MultiTree_GetChildByName_With_NULL_ChildHandle_Fails)
{
    ///arrange
    CMultiTreeMocks mocks;
    MULTITREE_HANDLE treeHandle = MultiTree_Create(StringClone, StringFree);

    ///act
    MULTITREE_RESULT result = MultiTree_GetChildByName(treeHandle, "childName", NULL);

    ///assert
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_INVALID_ARG, result);

    MultiTree_Destroy(treeHandle);
    mocks.ResetAllCalls();
}

/* Tests_SRS_MULTITREE_99_063:[ MultiTree_GetChildByName shall retrieve the handle of the child node childName from the treeNode node.] */
/* Tests_SRS_MULTITREE_99_067:[ The child node handle shall be returned in the childHandle argument.] */
/* Tests_SRS_MULTITREE_99_064:[ On success, MultiTree_GetChildByName shall return MULTITREE_OK.] */
TEST_FUNCTION(MultiTree_GetChildByName_When_The_Child_Exists_Succeeds)
{
    ///arrange
    CMultiTreeMocks mocks;
    MULTITREE_HANDLE treeHandle = MultiTree_Create(StringClone, StringFree);
    MULTITREE_HANDLE expectedChildHandle;
    MULTITREE_HANDLE actualChildHandle;

    (void)MultiTree_AddChild(treeHandle, "childName", &expectedChildHandle);

    ///act
    MULTITREE_RESULT result = MultiTree_GetChildByName(treeHandle, "childName", &actualChildHandle);

    ///assert
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_OK, result);
    ASSERT_ARE_EQUAL(MULTITREE_HANDLE, actualChildHandle, expectedChildHandle);

    MultiTree_Destroy(treeHandle);
    mocks.ResetAllCalls();
}

/* Tests_SRS_MULTITREE_99_068:[ If the specified child is not found, MultiTree_GetChildByName shall return MULTITREE_CHILD_NOT_FOUND.] */
TEST_FUNCTION(MultiTree_GetChildByName_When_No_Child_Exists_Fails)
{
    ///arrange
    CMultiTreeMocks mocks;
    MULTITREE_HANDLE treeHandle = MultiTree_Create(StringClone, StringFree);
    MULTITREE_HANDLE childHandle;

    ///act
    MULTITREE_RESULT result = MultiTree_GetChildByName(treeHandle, "childName", &childHandle);

    ///assert
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_CHILD_NOT_FOUND, result);

    MultiTree_Destroy(treeHandle);
    mocks.ResetAllCalls();
}

/* Tests_SRS_MULTITREE_99_068:[ If the specified child is not found, MultiTree_GetChildByName shall return MULTITREE_CHILD_NOT_FOUND.] */
TEST_FUNCTION(MultiTree_GetChildByName_When_The_Child_Is_Not_Found_In_The_Children_List_Fails)
{
    ///arrange
    CMultiTreeMocks mocks;
    MULTITREE_HANDLE treeHandle = MultiTree_Create(StringClone, StringFree);
    MULTITREE_HANDLE child1Handle;
    MULTITREE_HANDLE child2Handle;

    (void)MultiTree_AddChild(treeHandle, "childName1", &child1Handle);

    ///act
    MULTITREE_RESULT result = MultiTree_GetChildByName(treeHandle, "childName2", &child2Handle);

    ///assert
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_CHILD_NOT_FOUND, result);

    MultiTree_Destroy(treeHandle);
    mocks.ResetAllCalls();
}

/* MultiTree_GetLeafValue */

/* Tests_SRS_MULTITREE_99_055:[ If any argument is NULL, MultiTree_GetLeafValue shall return MULTITREE_INVALID_ARG.] */
TEST_FUNCTION(MultiTree_GetLeafValue_When_The_TreeHandle_Is_NULL_Fails)
{
    ///arrange
    CMultiTreeMocks mocks;
    const void* leafValue;

    ///act
    MULTITREE_RESULT result = MultiTree_GetLeafValue(NULL, "/child1", &leafValue);

    ///assert
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_INVALID_ARG, result);
}

/* Tests_SRS_MULTITREE_99_055:[ If any argument is NULL, MultiTree_GetLeafValue shall return MULTITREE_INVALID_ARG.] */
TEST_FUNCTION(MultiTree_GetLeafValue_When_The_LeafPath_Is_NULL_Fails)
{
    ///arrange
    CMultiTreeMocks mocks;
    MULTITREE_HANDLE treeHandle = MultiTree_Create(StringClone, StringFree);
    const void* leafValue;

    (void)MultiTree_AddLeaf(treeHandle, "child1", "hagauaga");

    ///act
    MULTITREE_RESULT result = MultiTree_GetLeafValue(treeHandle, NULL, &leafValue);

    ///assert
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_INVALID_ARG, result);

    MultiTree_Destroy(treeHandle);
    mocks.ResetAllCalls();
}

/* Tests_SRS_MULTITREE_99_055:[ If any argument is NULL, MultiTree_GetLeafValue shall return MULTITREE_INVALID_ARG.] */
TEST_FUNCTION(MultiTree_GetLeafValue_When_The_Value_Buffer_Is_NULL_Fails)
{
    ///arrange
    CMultiTreeMocks mocks;
    MULTITREE_HANDLE treeHandle = MultiTree_Create(StringClone, StringFree);

    (void)MultiTree_AddLeaf(treeHandle, "child1", "hagauaga");

    ///act
    MULTITREE_RESULT result = MultiTree_GetLeafValue(treeHandle, "/child1", NULL);

    ///assert
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_INVALID_ARG, result);

    MultiTree_Destroy(treeHandle);
    mocks.ResetAllCalls();
}

/* Tests_SRS_MULTITREE_99_058:[ The last child designates the child that will receive the value. If a child name is empty (such as in  '/child1//child12'), MULTITREE_EMPTY_CHILD_NAME shall be returned.] */
TEST_FUNCTION(MultiTree_GetLeafValue_With_An_Empty_Leaf_Path_Fails)
{
    ///arrange
    CMultiTreeMocks mocks;
    MULTITREE_HANDLE treeHandle = MultiTree_Create(StringClone, StringFree);
    const void* leafValue;

    (void)MultiTree_AddLeaf(treeHandle, "child1", "hagauaga");

    ///act
    MULTITREE_RESULT result = MultiTree_GetLeafValue(treeHandle, "", &leafValue);

    ///assert
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_EMPTY_CHILD_NAME, result);

    MultiTree_Destroy(treeHandle);
    mocks.ResetAllCalls();
}

/* Tests_SRS_MULTITREE_99_058:[ The last child designates the child that will receive the value. If a child name is empty (such as in  '/child1//child12'), MULTITREE_EMPTY_CHILD_NAME shall be returned.] */
TEST_FUNCTION(MultiTree_GetLeafValue_With_A_Slash_As_Leaf_Path_Fails)
{
    ///arrange
    CMultiTreeMocks mocks;
    MULTITREE_HANDLE treeHandle = MultiTree_Create(StringClone, StringFree);
    const void* leafValue;

    (void)MultiTree_AddLeaf(treeHandle, "child1", "hagauaga");

    ///act
    MULTITREE_RESULT result = MultiTree_GetLeafValue(treeHandle, "/", &leafValue);

    ///assert
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_EMPTY_CHILD_NAME, result);

    MultiTree_Destroy(treeHandle);
    mocks.ResetAllCalls();
}

/* Tests_SRS_MULTITREE_99_053:[ MultiTree_GetLeafValue shall copy into the destination argument the value of the node identified by the leafPath argument.] */
/* Tests_SRS_MULTITREE_99_054:[ On success, MultiTree_GetLeafValue shall return MULTITREE_OK.] */
TEST_FUNCTION(MultiTree_GetLeafValue_With_A_Correct_Child_Path_Succeeds)
{
    ///arrange
    CMultiTreeMocks mocks;
    MULTITREE_HANDLE treeHandle = MultiTree_Create(StringClone, StringFree);
    const char* leafValue;

    (void)MultiTree_AddLeaf(treeHandle, "child1", "hagauaga");

    ///act
    MULTITREE_RESULT result = MultiTree_GetLeafValue(treeHandle, "/child1", (const void**)&leafValue);

    ///assert
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_OK, result);
    ASSERT_ARE_EQUAL(char_ptr, "hagauaga", leafValue);

    MultiTree_Destroy(treeHandle);
    mocks.ResetAllCalls();
}

/* Tests_SRS_MULTITREE_99_053:[ MultiTree_GetLeafValue shall copy into the destination argument the value of the node identified by the leafPath argument.] */
/* Tests_SRS_MULTITREE_99_054:[ On success, MultiTree_GetLeafValue shall return MULTITREE_OK.] */
/* Tests_SRS_MULTITREE_99_056:[ The leafPath argument is a string in the following format: /child1/child12 or child1/child12.] */
/* Tests_SRS_MULTITREE_99_057:[ Subsequent names designate hierarchical children in the tree.] */
/* Tests_SRS_MULTITREE_99_058:[ The last child designates the child that will receive the value.] */
TEST_FUNCTION(MultiTree_GetLeafValue_With_A_Correct_Child_Path_For_A_Tree_With_2_Levels_Succeeds)
{
    ///arrange
    CMultiTreeMocks mocks;
    MULTITREE_HANDLE treeHandle = MultiTree_Create(StringClone, StringFree);
    const char* leafValue;

    (void)MultiTree_AddLeaf(treeHandle, "child1/child2", "hagauaga");

    ///act
    MULTITREE_RESULT result = MultiTree_GetLeafValue(treeHandle, "/child1/child2", (const void**)&leafValue);

    ///assert
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_OK, result);
    ASSERT_ARE_EQUAL(char_ptr, "hagauaga", leafValue);

    MultiTree_Destroy(treeHandle);
    mocks.ResetAllCalls();
}

/* Tests_SRS_MULTITREE_99_056:[ The leafPath argument is a string in the following format: /child1/child12 or child1/child12.] */
/* Tests_SRS_MULTITREE_99_057:[ Subsequent names designate hierarchical children in the tree.] */
/* Tests_SRS_MULTITREE_99_058:[ The last child designates the child that will receive the value.] */
/* Tests_SRS_MULTITREE_99_071:[ When the child node is not found, MultiTree_GetLeafValue shall return MULTITREE_CHILD_NOT_FOUND.] */
TEST_FUNCTION(MultiTree_GetLeafValue_For_A_Child_That_Does_Not_Exist_But_All_Nodes_Up_To_Last_Exist_Fails)
{
    ///arrange
    CMultiTreeMocks mocks;
    MULTITREE_HANDLE treeHandle = MultiTree_Create(StringClone, StringFree);
    const char* leafValue;

    (void)MultiTree_AddLeaf(treeHandle, "child1/child3", "hagauaga");

    ///act
    MULTITREE_RESULT result = MultiTree_GetLeafValue(treeHandle, "/child1/child2", (const void**)&leafValue);

    ///assert
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_CHILD_NOT_FOUND, result);

    MultiTree_Destroy(treeHandle);
    mocks.ResetAllCalls();
}

/* Tests_SRS_MULTITREE_99_056:[ The leafPath argument is a string in the following format: /child1/child12 or child1/child12.] */
/* Tests_SRS_MULTITREE_99_057:[ Subsequent names designate hierarchical children in the tree.] */
/* Tests_SRS_MULTITREE_99_058:[ The last child designates the child that will receive the value.] */
/* Tests_SRS_MULTITREE_99_071:[ When the child node is not found, MultiTree_GetLeafValue shall return MULTITREE_CHILD_NOT_FOUND.] */
TEST_FUNCTION(MultiTree_GetLeafValue_For_A_Child_With_Bad_Leaf_Path_On_First_Level_Fails)
{
    ///arrange
    CMultiTreeMocks mocks;
    MULTITREE_HANDLE treeHandle = MultiTree_Create(StringClone, StringFree);
    const char* leafValue;

    (void)MultiTree_AddLeaf(treeHandle, "child1/child2", "hagauaga");

    ///act
    MULTITREE_RESULT result = MultiTree_GetLeafValue(treeHandle, "/childx/child2", (const void**)&leafValue);

    ///assert
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_CHILD_NOT_FOUND, result);

    MultiTree_Destroy(treeHandle);
    mocks.ResetAllCalls();
}

/* Tests_SRS_MULTITREE_99_069:[ If a child name is empty (such as in  '/child1//child12'), MULTITREE_EMPTY_CHILD_NAME shall be returned.] */
TEST_FUNCTION(MultiTree_GetLeafValue_With_An_Empty_Child_Name_At_Level_1_Fails)
{
    ///arrange
    CMultiTreeMocks mocks;
    MULTITREE_HANDLE treeHandle = MultiTree_Create(StringClone, StringFree);
    const char* leafValue;

    (void)MultiTree_AddLeaf(treeHandle, "child1/child2", "hagauaga");

    ///act
    MULTITREE_RESULT result = MultiTree_GetLeafValue(treeHandle, "//child2", (const void**)&leafValue);

    ///assert
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_EMPTY_CHILD_NAME, result);

    MultiTree_Destroy(treeHandle);
    mocks.ResetAllCalls();
}

/* Tests_SRS_MULTITREE_99_071:[ When the child node is not found, MultiTree_GetLeafValue shall return MULTITREE_CHILD_NOT_FOUND.] */
TEST_FUNCTION(MultiTree_GetLeafValue_On_A_Node_With_No_Children_Fails)
{
    ///arrange
    CMultiTreeMocks mocks;
    MULTITREE_HANDLE treeHandle = MultiTree_Create(StringClone, StringFree);
    MULTITREE_HANDLE childHandle;
    const char* leafValue;

    (void)MultiTree_AddChild(treeHandle, "child1", &childHandle);

    ///act
    MULTITREE_RESULT result = MultiTree_GetLeafValue(treeHandle, "/child1/child2", (const void**)&leafValue);

    ///assert
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_CHILD_NOT_FOUND, result);

    MultiTree_Destroy(treeHandle);
    mocks.ResetAllCalls();
}

/* Tests_SRS_MULTITREE_99_070:[ If an attempt is made to get the value for a node that does not have a value set, then MultiTree_GetLeafValue shall return MULTITREE_EMPTY_VALUE.] */
TEST_FUNCTION(MultiTree_GetLeafValue_On_A_Child_Node_With_No_Value_Fails)
{
    ///arrange
    CMultiTreeMocks mocks;
    MULTITREE_HANDLE treeHandle = MultiTree_Create(StringClone, StringFree);
    MULTITREE_HANDLE childHandle;
    const char* leafValue;

    (void)MultiTree_AddChild(treeHandle, "child1", &childHandle);

    ///act
    MULTITREE_RESULT result = MultiTree_GetLeafValue(treeHandle, "/child1", (const void**)&leafValue);

    ///assert
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_EMPTY_VALUE, result);

    MultiTree_Destroy(treeHandle);
    mocks.ResetAllCalls();
}

/* MultiTree_SetValue */

/* Tests_SRS_MULTITREE_99_074:[ If any argument is NULL, MultiTree_SetValue shall return MULTITREE_INVALID_ARG.] */
TEST_FUNCTION(MultiTree_SetValue_With_A_NULL_treeHandle_Fails)
{
    ///arrange
    CMultiTreeMocks mocks;

    ///act
    MULTITREE_RESULT result = MultiTree_SetValue(NULL, (void*)"hagauaga");

    ///assert
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_INVALID_ARG, result);
}

/* Tests_SRS_MULTITREE_99_074:[ If any argument is NULL, MultiTree_SetValue shall return MULTITREE_INVALID_ARG.] */
TEST_FUNCTION(MultiTree_SetValue_With_A_NULL_value_Fails)
{
    ///arrange
    CMultiTreeMocks mocks;
    MULTITREE_HANDLE treeHandle = MultiTree_Create(StringClone, StringFree);
    MULTITREE_HANDLE childHandle;
    (void)MultiTree_AddChild(treeHandle, "child1", &childHandle);

    ///act
    MULTITREE_RESULT result = MultiTree_SetValue(childHandle, NULL);

    ///assert
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_INVALID_ARG, result);

    MultiTree_Destroy(treeHandle);
    mocks.ResetAllCalls(); /*not caring about what gets called*/
}

/* Tests_SRS_MULTITREE_99_072:[ MultiTree_SetValue shall set the value of the node indicated by the treeHandle argument to the value of the argument value.] */
/* Tests_SRS_MULTITREE_99_073:[ On success, MultiTree_SetValue shall return MULTITREE_OK.] */
TEST_FUNCTION(MultiTree_SetValue_With_Valid_Arguments_Succeeds)
{
    ///arrange
    CMultiTreeMocks mocks;
    MULTITREE_HANDLE treeHandle = MultiTree_Create(StringClone, StringFree);
    const char* actualValue;
    MULTITREE_HANDLE childHandle;
    (void)MultiTree_AddChild(treeHandle, "child1", &childHandle);

    ///act
    MULTITREE_RESULT result = MultiTree_SetValue(childHandle, (void*)"hagauaga");

    ///assert
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_OK, result);
    MultiTree_GetValue(childHandle, (const void**)&actualValue);
    ASSERT_ARE_EQUAL(char_ptr, "hagauaga", actualValue);

    MultiTree_Destroy(treeHandle);
    mocks.ResetAllCalls(); /*not caring about what gets called*/
}

/* Tests_SRS_MULTITREE_99_072:[ MultiTree_SetValue shall set the value of the node indicated by the treeHandle argument to the value of the argument value.] */
/* Tests_SRS_MULTITREE_99_073:[ On success, MultiTree_SetValue shall return MULTITREE_OK.] */
TEST_FUNCTION(MultiTree_SetValue_On_A_Node_That_Has_A_Value_Fails)
{
    ///arrange
    CMultiTreeMocks mocks;
    MULTITREE_HANDLE treeHandle = MultiTree_Create(StringClone, StringFree);
    MULTITREE_HANDLE childHandle;
    (void)MultiTree_AddChild(treeHandle, "child1", &childHandle);

    MultiTree_SetValue(childHandle, (void*)"hagauaga");

    ///act
    MULTITREE_RESULT result = MultiTree_SetValue(childHandle, (void*)"hagauaga");

    ///assert
    ASSERT_ARE_EQUAL(MULTITREE_RESULT, MULTITREE_ALREADY_HAS_A_VALUE, result);

    MultiTree_Destroy(treeHandle);
    mocks.ResetAllCalls(); /*not caring about what gets called*/
}

END_TEST_SUITE(MultiTree_ut)
