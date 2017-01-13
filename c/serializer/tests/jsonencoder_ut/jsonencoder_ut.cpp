// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <cstdlib>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "testrunnerswitcher.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "micromock.h"
#include "micromockcharstararenullterminatedstrings.h"
#include <stdexcept>
#include "multitree.h"
#include "azure_c_shared_utility/buffer_.h"

/*this is what we test*/
#include "jsonencoder.h"


DEFINE_MICROMOCK_ENUM_TO_STRING(JSON_ENCODER_RESULT, JSON_ENCODER_RESULT_VALUES);

DEFINE_MICROMOCK_ENUM_TO_STRING(JSON_ENCODER_TOSTRING_RESULT, JSON_ENCODER_TOSTRING_RESULT_VALUES);

/*These will be the data we shall use for tests.

            TREE CONTENT                                                                                            TREE HANDLE

            1. there will be a tree without children => {}                                                                      TEST_MULTITREE_HANDLE_1
            2. there shall be a tree with 1 child => {"child1":"value1"}                                                        TEST_MULTITREE_HANDLE_2
            3. there shall be a tree with 2 children => {"child1":"value1", "child2":"value2"}                                  TEST_MULTITREE_HANDLE_3
            4. there shall be a tree with 3 children => {"child1":"value1", "child2":"value2", "child3:value3"}                 TEST_MULTITREE_HANDLE_4
            5. there shall be a subtree with 2 children, inserted in every of the above trees in all possible places:
            this is the form of the subtree: {"child4":"value4", "child5":"value5"}                                             TEST_MULTITREE_HANDLE_5
            so the following subtrees shall be tested:
            5.1 {"subtree": {"child4":"value4", "child5":"value5"}}                                                             TEST_MULTITREE_HANDLE_5_1
            5.2.1 {"subtree": {"child4":"value4", "child5":"value5"}, "child1":"value1"}                                        TEST_MULTITREE_HANDLE_5_2_1
            5.2.2 {"child1":"value1", "subtree": {"child4":"value4", "child5":"value5"}}                                        TEST_MULTITREE_HANDLE_5_2_2
            5.3.1 {"subtree": {"child4":"value4", "child5":"value5"},"child1":"value1", "child2":"value2"}                      TEST_MULTITREE_HANDLE_5_3_1
            5.3.2 {"child1":"value1","subtree": {"child4":"value4", "child5":"value5"}, "child2":"value2"}                      TEST_MULTITREE_HANDLE_5_3_2
            5.3.3 {"child1":"value1", "child2":"value2","subtree": {"child4":"value4", "child5":"value5"}}                      TEST_MULTITREE_HANDLE_5_3_3
            5.4.1 {"subtree": {"child4":"value4", "child5":"value5"}, "child1":"value1", "child2":"value2", "child3:value3"}    TEST_MULTITREE_HANDLE_5_4_1
            5.4.2 {"child1":"value1", "subtree": {"child4":"value4", "child5":"value5"}, "child2":"value2", "child3:value3"}    TEST_MULTITREE_HANDLE_5_4_2
            5.4.3 {"child1":"value1", "child2":"value2", "subtree": {"child4":"value4", "child5":"value5"}, "child3:value3"}    TEST_MULTITREE_HANDLE_5_4_3
            5.4.4 {"child1":"value1", "child2":"value2", "child3:value3", "subtree": {"child4":"value4", "child5":"value5"}}    TEST_MULTITREE_HANDLE_5_4_4
            */
    
/*all the childx:valuex shall have the handles A4D1E0Cx*/


/*Every step of the algo in SRS shall be tested for all these trees... including the ones that are recursive*/
/*Well - where it makes sense*/


#define TEST_MULTITREE_HANDLE_1             ((MULTITREE_HANDLE)0xA4D1E001d)
#define TEST_MULTITREE_HANDLE_2             ((MULTITREE_HANDLE)0xA4D1E002d)
#define TEST_MULTITREE_HANDLE_3             ((MULTITREE_HANDLE)0xA4D1E003d)
#define TEST_MULTITREE_HANDLE_4             ((MULTITREE_HANDLE)0xA4D1E004d)

#define TEST_MULTITREE_HANDLE_5             ((MULTITREE_HANDLE)0xA4D1E005d)

#define TEST_MULTITREE_HANDLE_5_1           ((MULTITREE_HANDLE)0xA4D1E051d)
#define TEST_MULTITREE_HANDLE_5_2_1         ((MULTITREE_HANDLE)0xA4D1E521d)
#define TEST_MULTITREE_HANDLE_5_2_2         ((MULTITREE_HANDLE)0xA4D1E522d)
#define TEST_MULTITREE_HANDLE_5_3_1         ((MULTITREE_HANDLE)0xA4D1E531d)
#define TEST_MULTITREE_HANDLE_5_3_2         ((MULTITREE_HANDLE)0xA4D1E532d)
#define TEST_MULTITREE_HANDLE_5_3_3         ((MULTITREE_HANDLE)0xA4D1E533d)
#define TEST_MULTITREE_HANDLE_5_4_1         ((MULTITREE_HANDLE)0xA4D1E541d)
#define TEST_MULTITREE_HANDLE_5_4_2         ((MULTITREE_HANDLE)0xA4D1E542d)
#define TEST_MULTITREE_HANDLE_5_4_3         ((MULTITREE_HANDLE)0xA4D1E543d)
#define TEST_MULTITREE_HANDLE_5_4_4         ((MULTITREE_HANDLE)0xA4D1E544d)

#define TEST_MULTITREE_HANDLE_CHILD_1       ((MULTITREE_HANDLE)0xA4D1E0C1d)
#define TEST_MULTITREE_HANDLE_CHILD_2       ((MULTITREE_HANDLE)0xA4D1E0C2d)
#define TEST_MULTITREE_HANDLE_CHILD_3       ((MULTITREE_HANDLE)0xA4D1E0C3d)
#define TEST_MULTITREE_HANDLE_CHILD_4       ((MULTITREE_HANDLE)0xA4D1E0C4d)
#define TEST_MULTITREE_HANDLE_CHILD_5       ((MULTITREE_HANDLE)0xA4D1E0C5d)

#ifdef UNICODE 
#define tchar_ptr wchar_ptr 
#else 
#define tchar_ptr char_ptr 
#endif 

#ifdef _WIN32_WCE 
#define snprintf _snprintf 
#endif 

#ifdef USE_CTEST 
static int wchar_ptr_Compare(const wchar_t* left, const wchar_t* right)
{
    return (wcscmp(left, right) != 0);
}

void wchar_ptr_ToString(char* string, size_t bufferSize, const wchar_t* val)
{
    (void)snprintf(string, bufferSize, "%S", val);
}
#endif 

//
// We do this namespace redirection so that we don't have to repeat the utility string and buffer code in the mocks!
//
namespace BASEIMPLEMENTATION
{
#include "strings.c"
#include "buffer.c"
};

//
// We'll use the following globals to help us target specific STRING_new's to inject an error result.
//
static size_t currentSTRING_new_call;
static size_t whenShallSTRING_new_fail;

static bool currentSTRING_concat_called_from_another_mock;
static size_t currentSTRING_concat_call;
static size_t whenShallSTRING_concat_fail;

static size_t currentSTRING_concat_with_STRING_call;
static size_t whenShallSTRING_concat_with_STRING_fail;

static size_t nSTRING_new_calls=0;
static size_t nSTRING_delete_calls=0;

TYPED_MOCK_CLASS(CJSONMocks, CGlobalMock)
{
public:
    /* MultiTree mocks */
    MOCK_STATIC_METHOD_2(, MULTITREE_HANDLE, MultiTree_Create, MULTITREE_CLONE_FUNCTION, cloneFunction, MULTITREE_FREE_FUNCTION, freeFunction)
    MOCK_METHOD_END(MULTITREE_HANDLE, TEST_MULTITREE_HANDLE_1)
    MOCK_STATIC_METHOD_3(, MULTITREE_RESULT, MultiTree_AddLeaf, MULTITREE_HANDLE, treeHandle, const char*, destinationPath, const void*, value)
    MOCK_METHOD_END(MULTITREE_RESULT, MULTITREE_OK)
    MOCK_STATIC_METHOD_1(, void, MultiTree_Destroy, MULTITREE_HANDLE, treeHandle)
    MOCK_VOID_METHOD_END()
    MOCK_STATIC_METHOD_2(, MULTITREE_RESULT, MultiTree_GetChildCount, MULTITREE_HANDLE, treeHandle, size_t*, count)
    {
        /*simulator*/
        if (count != NULL)
        {
            *count =
                (treeHandle == TEST_MULTITREE_HANDLE_1) ? 0 :
                (treeHandle == TEST_MULTITREE_HANDLE_2) ? 1 :
                (treeHandle == TEST_MULTITREE_HANDLE_3) ? 2 :
                (treeHandle == TEST_MULTITREE_HANDLE_4) ? 3 :
                (treeHandle == TEST_MULTITREE_HANDLE_5) ? 2 :
                (treeHandle == TEST_MULTITREE_HANDLE_5_1) ? 1 :
                (treeHandle == TEST_MULTITREE_HANDLE_5_2_1) ? 2 :
                (treeHandle == TEST_MULTITREE_HANDLE_5_2_2) ? 2 :
                (treeHandle == TEST_MULTITREE_HANDLE_5_3_1) ? 3 :
                (treeHandle == TEST_MULTITREE_HANDLE_5_3_2) ? 3 :
                (treeHandle == TEST_MULTITREE_HANDLE_5_3_3) ? 3 :
                (treeHandle == TEST_MULTITREE_HANDLE_5_4_1) ? 4 :
                (treeHandle == TEST_MULTITREE_HANDLE_5_4_2) ? 4 :
                (treeHandle == TEST_MULTITREE_HANDLE_5_4_3) ? 4 :
                (treeHandle == TEST_MULTITREE_HANDLE_5_4_4) ? 4 :
                
                (treeHandle == TEST_MULTITREE_HANDLE_CHILD_1)? 0 : 
                (treeHandle == TEST_MULTITREE_HANDLE_CHILD_2) ? 0 :
                (treeHandle == TEST_MULTITREE_HANDLE_CHILD_3) ? 0 :
                (treeHandle == TEST_MULTITREE_HANDLE_CHILD_4) ? 0 :
                (treeHandle == TEST_MULTITREE_HANDLE_CHILD_5) ? 0 :
                throw std::runtime_error("not a known tree handle for this simulator"); /*it is quite awesome to be able to throw in ?:*/
        }
    }
    MOCK_METHOD_END(MULTITREE_RESULT, MULTITREE_OK)

    MOCK_STATIC_METHOD_3(, MULTITREE_RESULT, MultiTree_GetChild, MULTITREE_HANDLE, treeHandle, size_t, index, MULTITREE_HANDLE*, childHandle)
    {
        if (treeHandle == TEST_MULTITREE_HANDLE_1)
        {
            throw std::runtime_error("TEST_MULTITREE_HANDLE_1 has no children, and this function should nbever be called");
        }
        else if (treeHandle == TEST_MULTITREE_HANDLE_2)
        {
            if (index == 0)
            {
                *childHandle = TEST_MULTITREE_HANDLE_CHILD_1;
            }
            else
            {
                throw std::runtime_error("this tree only has 1 child");
            }
        }
        else if (treeHandle == TEST_MULTITREE_HANDLE_3)
        {
            if (index == 0)
            {
                *childHandle = TEST_MULTITREE_HANDLE_CHILD_1;
            }
            else if (index == 1)
            {
                *childHandle = TEST_MULTITREE_HANDLE_CHILD_2;
            }
            else
            {
                throw std::runtime_error("this tree only has 2 child");
            }
        }
        else if (treeHandle == TEST_MULTITREE_HANDLE_4)
        {
            if (index == 0)
            {
                *childHandle = TEST_MULTITREE_HANDLE_CHILD_1;
            }
            else if (index == 1)
            {
                *childHandle = TEST_MULTITREE_HANDLE_CHILD_2;
            }
            else if (index == 2)
            {
                *childHandle = TEST_MULTITREE_HANDLE_CHILD_3;
            }

            else
            {
                throw std::runtime_error("this tree only has 3 child");
            }
        }
        else if (treeHandle == TEST_MULTITREE_HANDLE_5_1)
        {
            if (index == 0)
            {
                *childHandle = TEST_MULTITREE_HANDLE_5;
            }
            else
            {
                throw std::runtime_error("this tree only has 3 child");
            }
        }
        else if (treeHandle == TEST_MULTITREE_HANDLE_5)
        {
            if (index == 0)
            {
                *childHandle = TEST_MULTITREE_HANDLE_CHILD_4;
            }
            else if (index == 1)
            {
                *childHandle = TEST_MULTITREE_HANDLE_CHILD_5;
            }
            else
            {
                throw std::runtime_error("this tree only has 2 child");
            }
        }
        else if (treeHandle == TEST_MULTITREE_HANDLE_5_2_1)
        {
            if (index == 0)
            {
                *childHandle = TEST_MULTITREE_HANDLE_5;
            }
            else if (index == 1)
            {
                *childHandle = TEST_MULTITREE_HANDLE_CHILD_1;
            }
            else
            {
                throw std::runtime_error("this tree only has 2 child");
            }
        }
        else if (treeHandle == TEST_MULTITREE_HANDLE_5_2_2)
        {
            if (index == 0)
            {
                *childHandle =  TEST_MULTITREE_HANDLE_CHILD_1;
            }
            else if (index == 1)
            {
                *childHandle = TEST_MULTITREE_HANDLE_5;
            }
            else
            {
                throw std::runtime_error("this tree only has 2 child");
            }
        }
        else if (treeHandle == TEST_MULTITREE_HANDLE_5_3_1)
        {
            if (index == 0)
            {
                *childHandle =  TEST_MULTITREE_HANDLE_5;
            }
            else if (index == 1)
            {
                *childHandle = TEST_MULTITREE_HANDLE_CHILD_1;
            }
            else if (index == 2)
            {
                *childHandle = TEST_MULTITREE_HANDLE_CHILD_2;
            }
            else
            {
                throw std::runtime_error("this tree only has 3 child");
            }
        }
        else if (treeHandle == TEST_MULTITREE_HANDLE_5_3_2)
        {
            if (index == 0)
            {
                *childHandle = TEST_MULTITREE_HANDLE_CHILD_1 ;
            }
            else if (index == 1)
            {
                *childHandle = TEST_MULTITREE_HANDLE_5;
            }
            else if (index == 2)
            {
                *childHandle = TEST_MULTITREE_HANDLE_CHILD_2;
            }
            else
            {
                throw std::runtime_error("this tree only has 3 child");
            }
        }
        else if (treeHandle == TEST_MULTITREE_HANDLE_5_3_3)
        {
            if (index == 0)
            {
                *childHandle = TEST_MULTITREE_HANDLE_CHILD_1;
            }
            else if (index == 1)
            {
                *childHandle = TEST_MULTITREE_HANDLE_CHILD_2;
            }
            else if (index == 2)
            {
                *childHandle = TEST_MULTITREE_HANDLE_5 ;
            }
            else
            {
                throw std::runtime_error("this tree only has 3 child");
            }
        }
        else if (treeHandle == TEST_MULTITREE_HANDLE_5_4_1 )
        {
            if (index == 0)
            {
                *childHandle = TEST_MULTITREE_HANDLE_5 ;
            }
            else if (index == 1)
            {
                *childHandle = TEST_MULTITREE_HANDLE_CHILD_1 ;
            }
            else if (index == 2)
            {
                *childHandle = TEST_MULTITREE_HANDLE_CHILD_2 ;
            }
            else if (index == 3)
            {
                *childHandle = TEST_MULTITREE_HANDLE_CHILD_3;
            }
            else
            {
                throw std::runtime_error("this tree only has 4 child");
            }
        }
        else if (treeHandle == TEST_MULTITREE_HANDLE_5_4_2)
        {
            if (index == 0)
            {
                *childHandle = TEST_MULTITREE_HANDLE_CHILD_1 ;
            }
            else if (index == 1)
            {
                *childHandle = TEST_MULTITREE_HANDLE_5;
            }
            else if (index == 2)
            {
                *childHandle = TEST_MULTITREE_HANDLE_CHILD_2;
            }
            else if (index == 3)
            {
                *childHandle = TEST_MULTITREE_HANDLE_CHILD_3;
            }
            else
            {
                throw std::runtime_error("this tree only has 4 child");
            }
        }
        else if (treeHandle == TEST_MULTITREE_HANDLE_5_4_3)
        {
            if (index == 0)
            {
                *childHandle = TEST_MULTITREE_HANDLE_CHILD_1;
            }
            else if (index == 1)
            {
                *childHandle = TEST_MULTITREE_HANDLE_CHILD_2 ;
            }
            else if (index == 2)
            {
                *childHandle = TEST_MULTITREE_HANDLE_5;
            }
            else if (index == 3)
            {
                *childHandle = TEST_MULTITREE_HANDLE_CHILD_3;
            }
            else
            {
                throw std::runtime_error("this tree only has 4 child");
            }
        }
        else if (treeHandle == TEST_MULTITREE_HANDLE_5_4_4)
        {
            if (index == 0)
            {
                *childHandle = TEST_MULTITREE_HANDLE_CHILD_1;
            }
            else if (index == 1)
            {
                *childHandle = TEST_MULTITREE_HANDLE_CHILD_2;
            }
            else if (index == 2)
            {
                *childHandle = TEST_MULTITREE_HANDLE_CHILD_3 ;
            }
            else if (index == 3)
            {
                *childHandle = TEST_MULTITREE_HANDLE_5;
            }
            else
            {
                throw std::runtime_error("this tree only has 4 child");
            }
        }
        
        else
        {
            throw std::runtime_error("unpreparea treeHandle");
        }
    }
    MOCK_METHOD_END(MULTITREE_RESULT, MULTITREE_OK)

    MOCK_STATIC_METHOD_2(, MULTITREE_RESULT, MultiTree_GetName, MULTITREE_HANDLE, treeHandle, STRING_HANDLE, destination)
    {
        if (treeHandle == TEST_MULTITREE_HANDLE_CHILD_1)
        {
            currentSTRING_concat_called_from_another_mock = true;
            STRING_concat(destination, "child1");
            currentSTRING_concat_called_from_another_mock = false;
        }
        else if (treeHandle == TEST_MULTITREE_HANDLE_CHILD_2)
        {
            currentSTRING_concat_called_from_another_mock = true;
            STRING_concat(destination, "child2");
            currentSTRING_concat_called_from_another_mock = false;
        }
        else if (treeHandle == TEST_MULTITREE_HANDLE_CHILD_3)
        {
            currentSTRING_concat_called_from_another_mock = true;
            STRING_concat(destination, "child3");
            currentSTRING_concat_called_from_another_mock = false;
        }
        else if (treeHandle == TEST_MULTITREE_HANDLE_5)
        {
            currentSTRING_concat_called_from_another_mock = true;
            STRING_concat(destination, "subtree");
            currentSTRING_concat_called_from_another_mock = false;
        }
        else if (treeHandle == TEST_MULTITREE_HANDLE_CHILD_4)
        {
            currentSTRING_concat_called_from_another_mock = true;
            STRING_concat(destination, "child4");
            currentSTRING_concat_called_from_another_mock = false;
        }
        else if (treeHandle == TEST_MULTITREE_HANDLE_CHILD_5)
        {
            currentSTRING_concat_called_from_another_mock = true;
            STRING_concat(destination, "child5");
            currentSTRING_concat_called_from_another_mock = false;
        }
        else
        {
            throw std::runtime_error("unprepared treeHandle");
        }
        

    }
    MOCK_METHOD_END(MULTITREE_RESULT, MULTITREE_OK)


    MOCK_STATIC_METHOD_2(, MULTITREE_RESULT, MultiTree_GetValue, MULTITREE_HANDLE, treeHandle, const void**, destination)
    {
        if (treeHandle == TEST_MULTITREE_HANDLE_CHILD_1)
        {
            *destination = (void*)"\"value1\"";
        }
        else if (treeHandle == TEST_MULTITREE_HANDLE_CHILD_2)
        {
            *destination = (void*)"\"value2\"";
        }
        else if (treeHandle == TEST_MULTITREE_HANDLE_CHILD_3)
        {
            *destination = (void*)"\"value3\"";
        }
        else if (treeHandle == TEST_MULTITREE_HANDLE_CHILD_4)
        {
            *destination = (void*)"\"value4\"";
        }
        else if (treeHandle == TEST_MULTITREE_HANDLE_CHILD_5)
        {
            *destination = (void*)"\"value5\"";
        }
        else
        {
            throw std::runtime_error("unprepared treeHandle");
        }
    }
    MOCK_METHOD_END(MULTITREE_RESULT, MULTITREE_OK)


    MOCK_STATIC_METHOD_2(, JSON_ENCODER_TOSTRING_RESULT, TestFunc_NodesAreStrings, STRING_HANDLE, destination, const void *, value)
    {
        if ((destination != NULL) && (value != NULL))
        {
            currentSTRING_concat_called_from_another_mock = true;
            (void)STRING_concat(destination, (const char*)value);
            currentSTRING_concat_called_from_another_mock = false;
        }
        else
        {
            throw std::runtime_error("this is expected to be called only with non-NULL arguments.");
        }
    }
    MOCK_METHOD_END(JSON_ENCODER_TOSTRING_RESULT, JSON_ENCODER_TOSTRING_OK)

    /*Strings*/
    //
    // Set messWithString_new to true when you what to have a test function cause some particular STRING_new to fail (simulating an out of memory condition).
    // Set whenShallSTRING_new_fail to some number >= 1 where the number will denote which STRING_new sequential order for the test will be simulating an out
    // of memory condition.
    //
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
            nSTRING_new_calls++;
            result2 = BASEIMPLEMENTATION::STRING_new();
        }
    }
    else
    {
        nSTRING_new_calls++;
        result2 = BASEIMPLEMENTATION::STRING_new();
    }
    MOCK_METHOD_END(STRING_HANDLE, result2)
    MOCK_STATIC_METHOD_1(, void, STRING_delete, STRING_HANDLE, s)
        nSTRING_delete_calls++;
        BASEIMPLEMENTATION::STRING_delete(s);
    MOCK_VOID_METHOD_END()

    MOCK_STATIC_METHOD_2(, int, STRING_concat, STRING_HANDLE, s1, const char*, s2)
        currentSTRING_concat_call += (currentSTRING_concat_called_from_another_mock)?0:1;
    MOCK_METHOD_END(int, (((whenShallSTRING_concat_fail>0) && (currentSTRING_concat_call == whenShallSTRING_concat_fail)) ? __LINE__ : BASEIMPLEMENTATION::STRING_concat(s1, s2)));

    MOCK_STATIC_METHOD_2(, int, STRING_concat_with_STRING, STRING_HANDLE, s1, STRING_HANDLE, s2)
        currentSTRING_concat_with_STRING_call++;
    MOCK_METHOD_END(int, (((whenShallSTRING_concat_with_STRING_fail>0) && (currentSTRING_concat_with_STRING_call == whenShallSTRING_concat_with_STRING_fail)) ? __LINE__ : BASEIMPLEMENTATION::STRING_concat_with_STRING(s1, s2)));

    MOCK_STATIC_METHOD_1(, const char*, STRING_c_str, STRING_HANDLE, s)
    MOCK_METHOD_END(const char*, BASEIMPLEMENTATION::STRING_c_str(s))

    /*BUFFER*/
    MOCK_STATIC_METHOD_0(, BUFFER_HANDLE, BUFFER_new)
    MOCK_METHOD_END(BUFFER_HANDLE, BASEIMPLEMENTATION::BUFFER_new())
    MOCK_STATIC_METHOD_1(, void, BUFFER_delete, BUFFER_HANDLE, b)
        BASEIMPLEMENTATION::BUFFER_delete(b);
    MOCK_VOID_METHOD_END()
    MOCK_STATIC_METHOD_2(, int, BUFFER_enlarge, BUFFER_HANDLE, b, size_t, enlargeSize)
    MOCK_METHOD_END(int, BASEIMPLEMENTATION::BUFFER_enlarge(b, enlargeSize))
    MOCK_STATIC_METHOD_2(, int, BUFFER_content, BUFFER_HANDLE, b, const unsigned char**, content)
    MOCK_METHOD_END(int, BASEIMPLEMENTATION::BUFFER_content(b, content))
    MOCK_STATIC_METHOD_2(, int, BUFFER_size, BUFFER_HANDLE, b, size_t*, size)
    MOCK_METHOD_END(int, BASEIMPLEMENTATION::BUFFER_size(b, size))
};

DECLARE_GLOBAL_MOCK_METHOD_2(CJSONMocks, , MULTITREE_HANDLE, MultiTree_Create, MULTITREE_CLONE_FUNCTION, cloneFunction, MULTITREE_FREE_FUNCTION, freeFunction);
DECLARE_GLOBAL_MOCK_METHOD_3(CJSONMocks, , MULTITREE_RESULT, MultiTree_AddLeaf, MULTITREE_HANDLE, treeHandle, const char*, destinationPath, const void*, value);
DECLARE_GLOBAL_MOCK_METHOD_1(CJSONMocks, , void, MultiTree_Destroy, MULTITREE_HANDLE, treeHandle);
DECLARE_GLOBAL_MOCK_METHOD_2(CJSONMocks, , MULTITREE_RESULT, MultiTree_GetChildCount, MULTITREE_HANDLE, treeHandle, size_t*, count);
DECLARE_GLOBAL_MOCK_METHOD_3(CJSONMocks, , MULTITREE_RESULT, MultiTree_GetChild, MULTITREE_HANDLE, treeHandle, size_t, index, MULTITREE_HANDLE*, childHandle);
DECLARE_GLOBAL_MOCK_METHOD_2(CJSONMocks, , MULTITREE_RESULT, MultiTree_GetName, MULTITREE_HANDLE, treeHandle, STRING_HANDLE, destination);
DECLARE_GLOBAL_MOCK_METHOD_2(CJSONMocks, , MULTITREE_RESULT, MultiTree_GetValue, MULTITREE_HANDLE, treeHandle, const void**, destination);
DECLARE_GLOBAL_MOCK_METHOD_2(CJSONMocks, , JSON_ENCODER_TOSTRING_RESULT, TestFunc_NodesAreStrings, STRING_HANDLE, destination, const void *, value);
DECLARE_GLOBAL_MOCK_METHOD_0(CJSONMocks, , STRING_HANDLE, STRING_new);
DECLARE_GLOBAL_MOCK_METHOD_1(CJSONMocks, , void, STRING_delete, STRING_HANDLE, s);
DECLARE_GLOBAL_MOCK_METHOD_2(CJSONMocks, , int, STRING_concat, STRING_HANDLE, s1, const char*, s2);
DECLARE_GLOBAL_MOCK_METHOD_2(CJSONMocks, , int, STRING_concat_with_STRING, STRING_HANDLE, s1, STRING_HANDLE, s2);
DECLARE_GLOBAL_MOCK_METHOD_1(CJSONMocks, , const char*, STRING_c_str, STRING_HANDLE, s);

/*all (applicable) tests in this file also test this: Tests_SRS_JSON_ENCODER_99_022:[ There is no hierarchy defined in the string. All strings are considered to be "root" level.]
 because they test that the objects created are of type "NUMBER" of "STRING" and not JSON_DATATYPE_OBJECT for example*/

/*by design there's no way to handle two JSON objects at the same time, so 
Tests_SRS_JSON_ENCODER_99_002:[ The module shall produce only one JSON object at a time.] */

static int NoCloneFunction(void** destination, const void* source)
{
    *destination = (void*)source;
    return 0;
}

static void NoFreeFunction(void* value)
{
    (void)value;
}

static STRING_HANDLE global_bufferTemp=NULL;

static CJSONMocks* mocks;

static MICROMOCK_MUTEX_HANDLE g_testByTest;

static MICROMOCK_GLOBAL_SEMAPHORE_HANDLE g_dllByDll;

BEGIN_TEST_SUITE(JSONEncoder_ut)

        TEST_SUITE_INITIALIZE(TestClassInitialize)
        {
            TEST_INITIALIZE_MEMORY_DEBUG(g_dllByDll);

            g_testByTest = MicroMockCreateMutex();
            ASSERT_IS_NOT_NULL(g_testByTest);
            mocks = new CJSONMocks();
        }

        TEST_SUITE_CLEANUP(TestClassCleanup)
        {
            delete mocks;
            MicroMockDestroyMutex(g_testByTest);
            TEST_DEINITIALIZE_MEMORY_DEBUG(g_dllByDll);

        }

        /* JSONEncoder_EncodeTree */
        TEST_FUNCTION_INITIALIZE(setsBufferTempSize)
        {
            if (!MicroMockAcquireMutex(g_testByTest))
            {
                ASSERT_FAIL("our mutex is ABANDONED. Failure in test framework");
            }
            global_bufferTemp = STRING_new();
            nSTRING_new_calls = 0;
            nSTRING_delete_calls = 0;
            currentSTRING_new_call = 0;
            whenShallSTRING_new_fail = 0;

            currentSTRING_concat_called_from_another_mock = false;
            currentSTRING_concat_call = 0;
            whenShallSTRING_concat_fail = 0;
            currentSTRING_concat_with_STRING_call = 0;
            whenShallSTRING_concat_with_STRING_fail = 0;

            mocks->ResetAllCalls(); /*so it is fresh and new*/
        }

        TEST_FUNCTION_CLEANUP(cleans)
        {
            ASSERT_ARE_EQUAL(size_t, nSTRING_new_calls, nSTRING_delete_calls);
            STRING_delete(global_bufferTemp);
            if (!MicroMockReleaseMutex(g_testByTest))
            {
                ASSERT_FAIL("failure in test framework at ReleaseMutex");
            }
        }

        /* Tests_SRS_JSON_ENCODER_99_032:[If any of the arguments passed to JSONEncoder_EncodeTree is NULL then JSON_ENCODER_INVALID_ARG shall be returned.] */
        TEST_FUNCTION(JSONEncoder_When_TreeHandle_Is_NULL_EncodeTree_Fails)
        {
            // arrange

            // act
            JSON_ENCODER_RESULT result = JSONEncoder_EncodeTree(NULL, global_bufferTemp, TestFunc_NodesAreStrings);

            // assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_INVALID_ARG, result);
            mocks->AssertActualAndExpectedCalls();
        }
        /* Tests_SRS_JSON_ENCODER_99_032:[If any of the arguments passed to JSONEncoder_EncodeTree is NULL then JSON_ENCODER_INVALID_ARG shall be returned.] */
        TEST_FUNCTION(JSONEncoder_When_Buffer_Is_NULL_EncodeTree_Fails)
        {
            // arrange

            // act
            JSON_ENCODER_RESULT result = JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_1, NULL, TestFunc_NodesAreStrings);

            // assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_INVALID_ARG, result);

            mocks->AssertActualAndExpectedCalls();
        }
        /* Tests_SRS_JSON_ENCODER_99_032:[If any of the arguments passed to JSONEncoder_EncodeTree is NULL then JSON_ENCODER_INVALID_ARG shall be returned.] */
        TEST_FUNCTION(JSONEncoder_When_ToString_Function_Is_NULL_EncodeTree_Fails)
        {
            // arrange
            
            // act
            JSON_ENCODER_RESULT result = JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_1, global_bufferTemp, NULL);

            // assert + cleanup

            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_INVALID_ARG, result);

            mocks->AssertActualAndExpectedCalls();
        }
       

        /*Tests_SRS_JSON_ENCODER_99_035:[ JSON encoder shall inquire the number of child nodes (further called childCount) of the current node (given by parameter treeHandle.]*/
        TEST_FUNCTION(JSONEncoder_EncodeTree_when_MultiTree_GetChildCount_fails_fails)
        {
            ///arrange
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                .SetReturn(MULTITREE_ERROR)
                ;

            ///act
            auto result = JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_1, global_bufferTemp, TestFunc_NodesAreStrings);
            
            ///assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_MULTITREE_ERROR, result);
            mocks->AssertActualAndExpectedCalls();
        }

        /*Tests_SRS_JSON_ENCODER_99_036:[ The string "{" shall be added to the output;]*/
        TEST_FUNCTION(JSONEncoder_EncodeTree_when_adding_open_paranthesis_fails_fails)
        {
            ///arrange
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;
            STRICT_EXPECTED_CALL((*mocks), STRING_concat(global_bufferTemp, "{"))
                .SetReturn(1);

            ///act
            auto result = JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_1, global_bufferTemp, TestFunc_NodesAreStrings);

            ///assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_ERROR, result);
            ASSERT_ARE_EQUAL(tchar_ptr, _T(""), mocks->CompareActualAndExpectedCalls().c_str());
        }

        /*Tests_SRS_JSON_ENCODER_99_036:[ The string "{" shall be added to the output;]*/
        TEST_FUNCTION(JSONEncoder_EncodeTree_adds_an_opening_curly_paranthesis)
        {
            ///arrange
            STRICT_EXPECTED_CALL((*mocks), STRING_concat(global_bufferTemp, "{"))
                .SetReturn(1);

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;
            
            ///act
            (void)JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_1, global_bufferTemp, TestFunc_NodesAreStrings);

            ///assert
            ASSERT_ARE_EQUAL(tchar_ptr, _T(""), mocks->CompareActualAndExpectedCalls().c_str());
        }
        /*Tests_SRS_JSON_ENCODER_99_037:[ For every child, the following actions shall take place in order to produce name:value:]*/
        TEST_FUNCTION(JSONEncoder_EncodeTree_iterates_over_children_1) /*notice how this tree has NO children*/
        {
            ///arrange
            STRICT_EXPECTED_CALL((*mocks), STRING_concat(global_bufferTemp, "{"));
            STRICT_EXPECTED_CALL((*mocks), STRING_concat(global_bufferTemp, "}"));

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            ///act
            (void)JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_1, global_bufferTemp, TestFunc_NodesAreStrings);

            ///assert
            //basically not throws from the mocking simulators
            ASSERT_ARE_EQUAL(tchar_ptr, _T(""), mocks->CompareActualAndExpectedCalls().c_str());
        }
        /*Tests_SRS_JSON_ENCODER_99_037:[ For every child, the following actions shall take place in order to produce name:value:]*/
        /*Tests_SRS_JSON_ENCODER_99_039:[ The child name shall be retrieved by a call to MultiTree_GetChild followed by a MultiTree_GetName.]*/
        /*Tests_SRS_JSON_ENCODER_99_040:[ After retrieval, it shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_041:[ A "\":" (single quote followed by colon) shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_042:[ If the child has zero children, then the function toStringFunc shall be invoked for the value (as computed by MultiTree_GetValue) of that child]*/
        /*Tests_SRS_JSON_ENCODER_99_047:[If toString function returns an error then JSON_Encoder_EncodeTree shall return JSON_ENCODER_TOSTRING_FUNCTION_ERROR]*/
        /*Tests_SRS_JSON_ENCODER_99_043:[ If the child has children, then JSONEncoder_EncodeTree  shall be invoked for every of these children.]*/
        /*Tests_SRS_JSON_ENCODER_99_044:[ A "," shall be added for every child, except the last one.]*/
        /*Tests_SRS_JSON_ENCODER_99_045:[ The string "}" shall be added to the output]*/
        /*This is the baseline for tree_2*/
        /*The below test shall be copy&pasted several times (as many as filing points there are, that
        is the number of STRICT_EXPECTED_CALLS + the number of blablalballbaFunction_call_count;*/
        TEST_FUNCTION(JSONEncoder_EncodeTree_2_success)
        {
            ///arrange

            //EXPECTED_CALL((*mocks), STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
            EXPECTED_CALL((*mocks), STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
            EXPECTED_CALL((*mocks), STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG));

            EXPECTED_CALL((*mocks), STRING_new());
            EXPECTED_CALL((*mocks), STRING_delete(IGNORED_PTR_ARG));
            EXPECTED_CALL((*mocks), STRING_new());
            EXPECTED_CALL((*mocks), STRING_delete(IGNORED_PTR_ARG));

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_2, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_2, 0, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetValue(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), TestFunc_NodesAreStrings(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreAllArguments()
                ;


            ///act
            auto result = JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_2, global_bufferTemp, TestFunc_NodesAreStrings);

            ///assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_OK, result);
            ASSERT_ARE_EQUAL(char_ptr, "{\"child1\":\"value1\"}", BASEIMPLEMENTATION::STRING_c_str(global_bufferTemp));
            
            

            ASSERT_ARE_EQUAL(tchar_ptr, _T(""), mocks->CompareActualAndExpectedCalls().c_str());
        }
        /*Tests_SRS_JSON_ENCODER_99_037:[ For every child, the following actions shall take place in order to produce name:value:]*/
        /*Tests_SRS_JSON_ENCODER_99_039:[ The child name shall be retrieved by a call to MultiTree_GetChild followed by a MultiTree_GetName.]*/
        /*Tests_SRS_JSON_ENCODER_99_040:[ After retrieval, it shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_041:[ A "\":" (single quote followed by colon) shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_042:[ If the child has zero children, then the function toStringFunc shall be invoked for the value (as computed by MultiTree_GetValue) of that child]*/
        /*Tests_SRS_JSON_ENCODER_99_047:[If toString function returns an error then JSON_Encoder_EncodeTree shall return JSON_ENCODER_TOSTRING_FUNCTION_ERROR]*/
        /*Tests_SRS_JSON_ENCODER_99_043:[ If the child has children, then JSONEncoder_EncodeTree  shall be invoked for every of these children.]*/
        /*Tests_SRS_JSON_ENCODER_99_044:[ A "," shall be added for every child, except the last one.]*/
        /*Tests_SRS_JSON_ENCODER_99_045:[ The string "}" shall be added to the output]*/
        /*This is the baseline for tree_2*/
        /*The below test shall be copy&pasted several times (as many as filing points there are, that
        is the number of STRICT_EXPECTED_CALLS + the number of blablalballbaFunction_call_count;*/
        TEST_FUNCTION(JSONEncoder_EncodeTree_2_fails_when_MultiTree_GetChildCount_fails)
        {
            ///arrange

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_2, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                .SetReturn(MULTITREE_ERROR)
                ;

            ///act
            auto result = JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_2, global_bufferTemp, TestFunc_NodesAreStrings);

            ///assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_MULTITREE_ERROR, result);
            ASSERT_ARE_EQUAL(tchar_ptr, _T(""), mocks->CompareActualAndExpectedCalls().c_str());
        }
        /*Tests_SRS_JSON_ENCODER_99_037:[ For every child, the following actions shall take place in order to produce name:value:]*/
        /*Tests_SRS_JSON_ENCODER_99_039:[ The child name shall be retrieved by a call to MultiTree_GetChild followed by a MultiTree_GetName.]*/
        /*Tests_SRS_JSON_ENCODER_99_040:[ After retrieval, it shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_041:[ A "\":" (single quote followed by colon) shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_042:[ If the child has zero children, then the function toStringFunc shall be invoked for the value (as computed by MultiTree_GetValue) of that child]*/
        /*Tests_SRS_JSON_ENCODER_99_047:[If toString function returns an error then JSON_Encoder_EncodeTree shall return JSON_ENCODER_TOSTRING_FUNCTION_ERROR]*/
        /*Tests_SRS_JSON_ENCODER_99_043:[ If the child has children, then JSONEncoder_EncodeTree  shall be invoked for every of these children.]*/
        /*Tests_SRS_JSON_ENCODER_99_044:[ A "," shall be added for every child, except the last one.]*/
        /*Tests_SRS_JSON_ENCODER_99_045:[ The string "}" shall be added to the output]*/
        /*This is the baseline for tree_2*/
        /*The below test shall be copy&pasted several times (as many as filing points there are, that
        is the number of STRICT_EXPECTED_CALLS + the number of blablalballbaFunction_call_count;*/
        TEST_FUNCTION(JSONEncoder_EncodeTree_2_fails_when_strcat_s_fails_1)
        {
            ///arrange
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG));

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_2, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            whenShallSTRING_concat_fail = 1;

            ///act
            auto result = JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_2, global_bufferTemp, TestFunc_NodesAreStrings);

            ///assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_ERROR, result);

            
            ASSERT_ARE_EQUAL(tchar_ptr, _T(""), mocks->CompareActualAndExpectedCalls().c_str());
        }
        /*Tests_SRS_JSON_ENCODER_99_037:[ For every child, the following actions shall take place in order to produce name:value:]*/
        /*Tests_SRS_JSON_ENCODER_99_039:[ The child name shall be retrieved by a call to MultiTree_GetChild followed by a MultiTree_GetName.]*/
        /*Tests_SRS_JSON_ENCODER_99_040:[ After retrieval, it shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_041:[ A "\":" (single quote followed by colon) shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_042:[ If the child has zero children, then the function toStringFunc shall be invoked for the value (as computed by MultiTree_GetValue) of that child]*/
        /*Tests_SRS_JSON_ENCODER_99_047:[If toString function returns an error then JSON_Encoder_EncodeTree shall return JSON_ENCODER_TOSTRING_FUNCTION_ERROR]*/
        /*Tests_SRS_JSON_ENCODER_99_043:[ If the child has children, then JSONEncoder_EncodeTree  shall be invoked for every of these children.]*/
        /*Tests_SRS_JSON_ENCODER_99_044:[ A "," shall be added for every child, except the last one.]*/
        /*Tests_SRS_JSON_ENCODER_99_045:[ The string "}" shall be added to the output]*/
        /*This is the baseline for tree_2*/
        /*The below test shall be copy&pasted several times (as many as filing points there are, that
        is the number of STRICT_EXPECTED_CALLS + the number of blablalballbaFunction_call_count;*/
        TEST_FUNCTION(JSONEncoder_EncodeTree_2_fails_when_MultiTree_GetChild_fails)
        {
            ///arrange
            EXPECTED_CALL((*mocks), STRING_new());
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
                
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
            EXPECTED_CALL((*mocks), STRING_delete(IGNORED_PTR_ARG));

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_2, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_2, 0, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                .SetReturn(MULTITREE_ERROR)
                ;

            ///act
            auto result = JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_2, global_bufferTemp, TestFunc_NodesAreStrings);

            ///assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_MULTITREE_ERROR, result);

            

            ASSERT_ARE_EQUAL(tchar_ptr, _T(""), mocks->CompareActualAndExpectedCalls().c_str());
        }
        /*Tests_SRS_JSON_ENCODER_99_037:[ For every child, the following actions shall take place in order to produce name:value:]*/
        /*Tests_SRS_JSON_ENCODER_99_039:[ The child name shall be retrieved by a call to MultiTree_GetChild followed by a MultiTree_GetName.]*/
        /*Tests_SRS_JSON_ENCODER_99_040:[ After retrieval, it shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_041:[ A "\":" (single quote followed by colon) shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_042:[ If the child has zero children, then the function toStringFunc shall be invoked for the value (as computed by MultiTree_GetValue) of that child]*/
        /*Tests_SRS_JSON_ENCODER_99_047:[If toString function returns an error then JSON_Encoder_EncodeTree shall return JSON_ENCODER_TOSTRING_FUNCTION_ERROR]*/
        /*Tests_SRS_JSON_ENCODER_99_043:[ If the child has children, then JSONEncoder_EncodeTree  shall be invoked for every of these children.]*/
        /*Tests_SRS_JSON_ENCODER_99_044:[ A "," shall be added for every child, except the last one.]*/
        /*Tests_SRS_JSON_ENCODER_99_045:[ The string "}" shall be added to the output]*/
        /*This is the baseline for tree_2*/
        /*The below test shall be copy&pasted several times (as many as filing points there are, that
        is the number of STRICT_EXPECTED_CALLS + the number of blablalballbaFunction_call_count;*/
        TEST_FUNCTION(JSONEncoder_EncodeTree_2_fails_when_MultiTree_GetName_fails)
        {
            ///arrange
            /*mocks non-argument checked*/
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
            EXPECTED_CALL((*mocks), STRING_new());
            EXPECTED_CALL((*mocks), STRING_delete(IGNORED_PTR_ARG));



            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_2, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_2, 0, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                .SetReturn(MULTITREE_ERROR)
                ;


            ///act
            auto result = JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_2, global_bufferTemp, TestFunc_NodesAreStrings);

            ///assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_MULTITREE_ERROR, result);

            

            ASSERT_ARE_EQUAL(tchar_ptr, _T(""), mocks->CompareActualAndExpectedCalls().c_str());
        }
        /*Tests_SRS_JSON_ENCODER_99_037:[ For every child, the following actions shall take place in order to produce name:value:]*/
        /*Tests_SRS_JSON_ENCODER_99_039:[ The child name shall be retrieved by a call to MultiTree_GetChild followed by a MultiTree_GetName.]*/
        /*Tests_SRS_JSON_ENCODER_99_040:[ After retrieval, it shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_041:[ A "\":" (single quote followed by colon) shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_042:[ If the child has zero children, then the function toStringFunc shall be invoked for the value (as computed by MultiTree_GetValue) of that child]*/
        /*Tests_SRS_JSON_ENCODER_99_047:[If toString function returns an error then JSON_Encoder_EncodeTree shall return JSON_ENCODER_TOSTRING_FUNCTION_ERROR]*/
        /*Tests_SRS_JSON_ENCODER_99_043:[ If the child has children, then JSONEncoder_EncodeTree  shall be invoked for every of these children.]*/
        /*Tests_SRS_JSON_ENCODER_99_044:[ A "," shall be added for every child, except the last one.]*/
        /*Tests_SRS_JSON_ENCODER_99_045:[ The string "}" shall be added to the output]*/
        /*This is the baseline for tree_2*/
        /*The below test shall be copy&pasted several times (as many as filing points there are, that
        is the number of STRICT_EXPECTED_CALLS + the number of blablalballbaFunction_call_count;*/
        TEST_FUNCTION(JSONEncoder_EncodeTree_2_fails_when_strcat_s_fails_2)
        {
            ///arrange

            EXPECTED_CALL((*mocks), STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG));

            EXPECTED_CALL((*mocks), STRING_new());
            EXPECTED_CALL((*mocks), STRING_delete(IGNORED_PTR_ARG));


            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_2, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_2, 0, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            whenShallSTRING_concat_with_STRING_fail = 1;

            ///act
            auto result = JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_2, global_bufferTemp,  TestFunc_NodesAreStrings);

            ///assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_ERROR, result);

            
            

            ASSERT_ARE_EQUAL(tchar_ptr, _T(""), mocks->CompareActualAndExpectedCalls().c_str());
        }
        /*Tests_SRS_JSON_ENCODER_99_037:[ For every child, the following actions shall take place in order to produce name:value:]*/
        /*Tests_SRS_JSON_ENCODER_99_039:[ The child name shall be retrieved by a call to MultiTree_GetChild followed by a MultiTree_GetName.]*/
        /*Tests_SRS_JSON_ENCODER_99_040:[ After retrieval, it shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_041:[ A "\":" (single quote followed by colon) shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_042:[ If the child has zero children, then the function toStringFunc shall be invoked for the value (as computed by MultiTree_GetValue) of that child]*/
        /*Tests_SRS_JSON_ENCODER_99_047:[If toString function returns an error then JSON_Encoder_EncodeTree shall return JSON_ENCODER_TOSTRING_FUNCTION_ERROR]*/
        /*Tests_SRS_JSON_ENCODER_99_043:[ If the child has children, then JSONEncoder_EncodeTree  shall be invoked for every of these children.]*/
        /*Tests_SRS_JSON_ENCODER_99_044:[ A "," shall be added for every child, except the last one.]*/
        /*Tests_SRS_JSON_ENCODER_99_045:[ The string "}" shall be added to the output]*/
        /*This is the baseline for tree_2*/
        /*The below test shall be copy&pasted several times (as many as filing points there are, that
        is the number of STRICT_EXPECTED_CALLS + the number of blablalballbaFunction_call_count;*/
        TEST_FUNCTION(JSONEncoder_EncodeTree_2_fails_when_MultiTree_GetChildCount_fails_2)
        {
            ///arrange

            /*mocks non-argument checked*/
            EXPECTED_CALL((*mocks), STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG));

            EXPECTED_CALL((*mocks), STRING_new());
            EXPECTED_CALL((*mocks), STRING_delete(IGNORED_PTR_ARG));


            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_2, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_2, 0, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                .SetReturn(MULTITREE_ERROR)
                ;


            ///act
            auto result = JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_2, global_bufferTemp, TestFunc_NodesAreStrings);

            ///assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_MULTITREE_ERROR, result);

            
            

            ASSERT_ARE_EQUAL(tchar_ptr, _T(""), mocks->CompareActualAndExpectedCalls().c_str());
        }
        /*Tests_SRS_JSON_ENCODER_99_037:[ For every child, the following actions shall take place in order to produce name:value:]*/
        /*Tests_SRS_JSON_ENCODER_99_039:[ The child name shall be retrieved by a call to MultiTree_GetChild followed by a MultiTree_GetName.]*/
        /*Tests_SRS_JSON_ENCODER_99_040:[ After retrieval, it shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_041:[ A "\":" (single quote followed by colon) shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_042:[ If the child has zero children, then the function toStringFunc shall be invoked for the value (as computed by MultiTree_GetValue) of that child]*/
        /*Tests_SRS_JSON_ENCODER_99_047:[If toString function returns an error then JSON_Encoder_EncodeTree shall return JSON_ENCODER_TOSTRING_FUNCTION_ERROR]*/
        /*Tests_SRS_JSON_ENCODER_99_043:[ If the child has children, then JSONEncoder_EncodeTree  shall be invoked for every of these children.]*/
        /*Tests_SRS_JSON_ENCODER_99_044:[ A "," shall be added for every child, except the last one.]*/
        /*Tests_SRS_JSON_ENCODER_99_045:[ The string "}" shall be added to the output]*/
        /*This is the baseline for tree_2*/
        /*The below test shall be copy&pasted several times (as many as filing points there are, that
        is the number of STRICT_EXPECTED_CALLS + the number of blablalballbaFunction_call_count;*/
        TEST_FUNCTION(JSONEncoder_EncodeTree_2_fails_when_MultiTree_GetValue_fails)
        {
            ///arrange

            EXPECTED_CALL((*mocks), STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG));

            EXPECTED_CALL((*mocks), STRING_new());
            EXPECTED_CALL((*mocks), STRING_delete(IGNORED_PTR_ARG));

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_2, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_2, 0, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetValue(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                .SetReturn(MULTITREE_ERROR)
                ;


            ///act
            auto result = JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_2, global_bufferTemp,  TestFunc_NodesAreStrings);

            ///assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_MULTITREE_ERROR, result);

            
            

            ASSERT_ARE_EQUAL(tchar_ptr, _T(""), mocks->CompareActualAndExpectedCalls().c_str());
        }
        /*Tests_SRS_JSON_ENCODER_99_037:[ For every child, the following actions shall take place in order to produce name:value:]*/
        /*Tests_SRS_JSON_ENCODER_99_039:[ The child name shall be retrieved by a call to MultiTree_GetChild followed by a MultiTree_GetName.]*/
        /*Tests_SRS_JSON_ENCODER_99_040:[ After retrieval, it shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_041:[ A "\":" (single quote followed by colon) shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_042:[ If the child has zero children, then the function toStringFunc shall be invoked for the value (as computed by MultiTree_GetValue) of that child]*/
        /*Tests_SRS_JSON_ENCODER_99_047:[If toString function returns an error then JSON_Encoder_EncodeTree shall return JSON_ENCODER_TOSTRING_FUNCTION_ERROR]*/
        /*Tests_SRS_JSON_ENCODER_99_043:[ If the child has children, then JSONEncoder_EncodeTree  shall be invoked for every of these children.]*/
        /*Tests_SRS_JSON_ENCODER_99_044:[ A "," shall be added for every child, except the last one.]*/
        /*Tests_SRS_JSON_ENCODER_99_045:[ The string "}" shall be added to the output]*/
        /*This is the baseline for tree_2*/
        /*The below test shall be copy&pasted several times (as many as filing points there are, that
        is the number of STRICT_EXPECTED_CALLS + the number of blablalballbaFunction_call_count;*/
        TEST_FUNCTION(JSONEncoder_EncodeTree_2_fails_when_toString_fails)
        {
            ///arrange

            EXPECTED_CALL((*mocks), STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG));

            EXPECTED_CALL((*mocks), STRING_new());
            EXPECTED_CALL((*mocks), STRING_delete(IGNORED_PTR_ARG));
            EXPECTED_CALL((*mocks), STRING_new());
            EXPECTED_CALL((*mocks), STRING_delete(IGNORED_PTR_ARG));

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_2, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_2, 0, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetValue(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), TestFunc_NodesAreStrings(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreAllArguments()
                .SetReturn(JSON_ENCODER_TOSTRING_ERROR)
                ;


            ///act
            auto result = JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_2, global_bufferTemp, TestFunc_NodesAreStrings);

            ///assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_TOSTRING_FUNCTION_ERROR, result);

            
            
            

            ASSERT_ARE_EQUAL(tchar_ptr, _T(""), mocks->CompareActualAndExpectedCalls().c_str());
        }
        /*Tests_SRS_JSON_ENCODER_99_037:[ For every child, the following actions shall take place in order to produce name:value:]*/
        /*Tests_SRS_JSON_ENCODER_99_039:[ The child name shall be retrieved by a call to MultiTree_GetChild followed by a MultiTree_GetName.]*/
        /*Tests_SRS_JSON_ENCODER_99_040:[ After retrieval, it shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_041:[ A "\":" (single quote followed by colon) shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_042:[ If the child has zero children, then the function toStringFunc shall be invoked for the value (as computed by MultiTree_GetValue) of that child]*/
        /*Tests_SRS_JSON_ENCODER_99_047:[If toString function returns an error then JSON_Encoder_EncodeTree shall return JSON_ENCODER_TOSTRING_FUNCTION_ERROR]*/
        /*Tests_SRS_JSON_ENCODER_99_043:[ If the child has children, then JSONEncoder_EncodeTree  shall be invoked for every of these children.]*/
        /*Tests_SRS_JSON_ENCODER_99_044:[ A "," shall be added for every child, except the last one.]*/
        /*Tests_SRS_JSON_ENCODER_99_045:[ The string "}" shall be added to the output]*/
        /*This is the baseline for tree_2*/
        /*The below test shall be copy&pasted several times (as many as filing points there are, that
        is the number of STRICT_EXPECTED_CALLS + the number of blablalballbaFunction_call_count;*/
        TEST_FUNCTION(JSONEncoder_EncodeTree_2_fails_when_strcat_s_fails_3)
        {
            ///arrange

            EXPECTED_CALL((*mocks), STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
            EXPECTED_CALL((*mocks), STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG));

            EXPECTED_CALL((*mocks), STRING_new());
            EXPECTED_CALL((*mocks), STRING_delete(IGNORED_PTR_ARG));
            EXPECTED_CALL((*mocks), STRING_new());
            EXPECTED_CALL((*mocks), STRING_delete(IGNORED_PTR_ARG));

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_2, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_2, 0, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetValue(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), TestFunc_NodesAreStrings(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreAllArguments()
                ;

            whenShallSTRING_concat_with_STRING_fail = 2;

            ///act
            auto result = JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_2, global_bufferTemp, TestFunc_NodesAreStrings);

            ///assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_ERROR, result);

            ASSERT_ARE_EQUAL(tchar_ptr, _T(""), mocks->CompareActualAndExpectedCalls().c_str());
        }        
        /*Tests_SRS_JSON_ENCODER_99_037:[ For every child, the following actions shall take place in order to produce name:value:]*/
        /*Tests_SRS_JSON_ENCODER_99_039:[ The child name shall be retrieved by a call to MultiTree_GetChild followed by a MultiTree_GetName.]*/
        /*Tests_SRS_JSON_ENCODER_99_040:[ After retrieval, it shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_041:[ A "\":" (single quote followed by colon) shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_042:[ If the child has zero children, then the function toStringFunc shall be invoked for the value (as computed by MultiTree_GetValue) of that child]*/
        /*Tests_SRS_JSON_ENCODER_99_047:[If toString function returns an error then JSON_Encoder_EncodeTree shall return JSON_ENCODER_TOSTRING_FUNCTION_ERROR]*/
        /*Tests_SRS_JSON_ENCODER_99_043:[ If the child has children, then JSONEncoder_EncodeTree  shall be invoked for every of these children.]*/
        /*Tests_SRS_JSON_ENCODER_99_044:[ A "," shall be added for every child, except the last one.]*/
        /*Tests_SRS_JSON_ENCODER_99_045:[ The string "}" shall be added to the output]*/
        /*This is the baseline for tree_3*/
        /*The below test shall be copy&pasted several times (as many as filing points there are, that
        is the number of STRICT_EXPECTED_CALLS + the number of blablalballbaFunction_call_count;*/
        TEST_FUNCTION(JSONEncoder_EncodeTree_3_success)
        {
            ///arrange
            EXPECTED_CALL((*mocks), STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(4);
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(11);
            EXPECTED_CALL((*mocks), STRING_new())
                .ExpectedTimesExactly(4);
            EXPECTED_CALL((*mocks), STRING_delete(IGNORED_PTR_ARG))
                .ExpectedTimesExactly(4);

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_3, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_3, 0, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetValue(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), TestFunc_NodesAreStrings(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreAllArguments()
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_3, 1, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_2, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_CHILD_2, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetValue(TEST_MULTITREE_HANDLE_CHILD_2, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), TestFunc_NodesAreStrings(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreAllArguments()
                ;


            ///act
            auto result = JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_3, global_bufferTemp, TestFunc_NodesAreStrings);

            ///assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_OK, result);
            ASSERT_ARE_EQUAL(char_ptr, "{\"child1\":\"value1\", \"child2\":\"value2\"}", BASEIMPLEMENTATION::STRING_c_str(global_bufferTemp));
                                                                        
                                                                
            
            

            
            

            ASSERT_ARE_EQUAL(tchar_ptr, _T(""), mocks->CompareActualAndExpectedCalls().c_str());
        }
        /*Tests_SRS_JSON_ENCODER_99_037:[ For every child, the following actions shall take place in order to produce name:value:]*/
        /*Tests_SRS_JSON_ENCODER_99_039:[ The child name shall be retrieved by a call to MultiTree_GetChild followed by a MultiTree_GetName.]*/
        /*Tests_SRS_JSON_ENCODER_99_040:[ After retrieval, it shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_041:[ A "\":" (single quote followed by colon) shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_042:[ If the child has zero children, then the function toStringFunc shall be invoked for the value (as computed by MultiTree_GetValue) of that child]*/
        /*Tests_SRS_JSON_ENCODER_99_047:[If toString function returns an error then JSON_Encoder_EncodeTree shall return JSON_ENCODER_TOSTRING_FUNCTION_ERROR]*/
        /*Tests_SRS_JSON_ENCODER_99_043:[ If the child has children, then JSONEncoder_EncodeTree  shall be invoked for every of these children.]*/
        /*Tests_SRS_JSON_ENCODER_99_044:[ A "," shall be added for every child, except the last one.]*/
        /*Tests_SRS_JSON_ENCODER_99_045:[ The string "}" shall be added to the output]*/
        /*This is the baseline for tree_3*/
        /*The below test shall be copy&pasted several times (as many as filing points there are, that
        is the number of STRICT_EXPECTED_CALLS + the number of blablalballbaFunction_call_count;*/
        TEST_FUNCTION(JSONEncoder_EncodeTree_fails_3_when_MultiTree_GetChildCount_fails_1)
        {
            ///arrange

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_3, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                .SetReturn(MULTITREE_ERROR)
                ;
            

            ///act
            auto result = JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_3, global_bufferTemp, TestFunc_NodesAreStrings);

            ///assert
            
            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_MULTITREE_ERROR, result);
            ASSERT_ARE_EQUAL(tchar_ptr, _T(""), mocks->CompareActualAndExpectedCalls().c_str());
        }
        /*Tests_SRS_JSON_ENCODER_99_037:[ For every child, the following actions shall take place in order to produce name:value:]*/
        /*Tests_SRS_JSON_ENCODER_99_039:[ The child name shall be retrieved by a call to MultiTree_GetChild followed by a MultiTree_GetName.]*/
        /*Tests_SRS_JSON_ENCODER_99_040:[ After retrieval, it shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_041:[ A "\":" (single quote followed by colon) shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_042:[ If the child has zero children, then the function toStringFunc shall be invoked for the value (as computed by MultiTree_GetValue) of that child]*/
        /*Tests_SRS_JSON_ENCODER_99_047:[If toString function returns an error then JSON_Encoder_EncodeTree shall return JSON_ENCODER_TOSTRING_FUNCTION_ERROR]*/
        /*Tests_SRS_JSON_ENCODER_99_043:[ If the child has children, then JSONEncoder_EncodeTree  shall be invoked for every of these children.]*/
        /*Tests_SRS_JSON_ENCODER_99_044:[ A "," shall be added for every child, except the last one.]*/
        /*Tests_SRS_JSON_ENCODER_99_045:[ The string "}" shall be added to the output]*/
        /*This is the baseline for tree_3*/
        /*The below test shall be copy&pasted several times (as many as filing points there are, that
        is the number of STRICT_EXPECTED_CALLS + the number of blablalballbaFunction_call_count;*/
        TEST_FUNCTION(JSONEncoder_EncodeTree_3_fails_when_strcpy_s_fails_1)
        {
            ///arrange

            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(1);
            EXPECTED_CALL((*mocks), STRING_c_str(IGNORED_PTR_ARG))
                .ExpectedTimesExactly(0);
            EXPECTED_CALL((*mocks), STRING_new())
                .ExpectedTimesExactly(0);
            EXPECTED_CALL((*mocks), STRING_delete(IGNORED_PTR_ARG))
                .ExpectedTimesExactly(0);

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_3, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            whenShallSTRING_concat_fail = 1;

            ///act
            auto result = JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_3, global_bufferTemp, TestFunc_NodesAreStrings);

            ///assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_ERROR, result);
            
            ASSERT_ARE_EQUAL(tchar_ptr, _T(""), mocks->CompareActualAndExpectedCalls().c_str());
        }
        /*Tests_SRS_JSON_ENCODER_99_037:[ For every child, the following actions shall take place in order to produce name:value:]*/
        /*Tests_SRS_JSON_ENCODER_99_039:[ The child name shall be retrieved by a call to MultiTree_GetChild followed by a MultiTree_GetName.]*/
        /*Tests_SRS_JSON_ENCODER_99_040:[ After retrieval, it shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_041:[ A "\":" (single quote followed by colon) shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_042:[ If the child has zero children, then the function toStringFunc shall be invoked for the value (as computed by MultiTree_GetValue) of that child]*/
        /*Tests_SRS_JSON_ENCODER_99_047:[If toString function returns an error then JSON_Encoder_EncodeTree shall return JSON_ENCODER_TOSTRING_FUNCTION_ERROR]*/
        /*Tests_SRS_JSON_ENCODER_99_043:[ If the child has children, then JSONEncoder_EncodeTree  shall be invoked for every of these children.]*/
        /*Tests_SRS_JSON_ENCODER_99_044:[ A "," shall be added for every child, except the last one.]*/
        /*Tests_SRS_JSON_ENCODER_99_045:[ The string "}" shall be added to the output]*/
        /*This is the baseline for tree_3*/
        /*The below test shall be copy&pasted several times (as many as filing points there are, that
        is the number of STRICT_EXPECTED_CALLS + the number of blablalballbaFunction_call_count;*/
        TEST_FUNCTION(JSONEncoder_EncodeTree_3_fails_when_strcat_s_fails_1)
        {
            ///arrange

            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(1);
            EXPECTED_CALL((*mocks), STRING_c_str(IGNORED_PTR_ARG))
                .ExpectedTimesExactly(0);
            EXPECTED_CALL((*mocks), STRING_new())
                .ExpectedTimesExactly(0);
            EXPECTED_CALL((*mocks), STRING_delete(IGNORED_PTR_ARG))
                .ExpectedTimesExactly(0);

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_3, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            whenShallSTRING_concat_fail = 1;

            ///act
            auto result = JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_3, global_bufferTemp, TestFunc_NodesAreStrings);

            ///assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_ERROR, result);
            
            
            ASSERT_ARE_EQUAL(tchar_ptr, _T(""), mocks->CompareActualAndExpectedCalls().c_str());
        }        
        /*Tests_SRS_JSON_ENCODER_99_037:[ For every child, the following actions shall take place in order to produce name:value:]*/
        /*Tests_SRS_JSON_ENCODER_99_039:[ The child name shall be retrieved by a call to MultiTree_GetChild followed by a MultiTree_GetName.]*/
        /*Tests_SRS_JSON_ENCODER_99_040:[ After retrieval, it shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_041:[ A "\":" (single quote followed by colon) shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_042:[ If the child has zero children, then the function toStringFunc shall be invoked for the value (as computed by MultiTree_GetValue) of that child]*/
        /*Tests_SRS_JSON_ENCODER_99_047:[If toString function returns an error then JSON_Encoder_EncodeTree shall return JSON_ENCODER_TOSTRING_FUNCTION_ERROR]*/
        /*Tests_SRS_JSON_ENCODER_99_043:[ If the child has children, then JSONEncoder_EncodeTree  shall be invoked for every of these children.]*/
        /*Tests_SRS_JSON_ENCODER_99_044:[ A "," shall be added for every child, except the last one.]*/
        /*Tests_SRS_JSON_ENCODER_99_045:[ The string "}" shall be added to the output]*/
        /*This is the baseline for tree_3*/
        /*The below test shall be copy&pasted several times (as many as filing points there are, that
        is the number of STRICT_EXPECTED_CALLS + the number of blablalballbaFunction_call_count;*/
        TEST_FUNCTION(JSONEncoder_EncodeTree_3_fails_when_MultiTree_GetChild_fails_1)
        {
            ///arrange

            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG));

            EXPECTED_CALL((*mocks), STRING_new());
            EXPECTED_CALL((*mocks), STRING_delete(IGNORED_PTR_ARG));

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_3, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_3, 0, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                .SetReturn(MULTITREE_ERROR)
                ;


            ///act
            auto result = JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_3, global_bufferTemp, TestFunc_NodesAreStrings);

            ///assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_MULTITREE_ERROR, result);
            
            
            ASSERT_ARE_EQUAL(tchar_ptr, _T(""), mocks->CompareActualAndExpectedCalls().c_str());
        }
        /*Tests_SRS_JSON_ENCODER_99_037:[ For every child, the following actions shall take place in order to produce name:value:]*/
        /*Tests_SRS_JSON_ENCODER_99_039:[ The child name shall be retrieved by a call to MultiTree_GetChild followed by a MultiTree_GetName.]*/
        /*Tests_SRS_JSON_ENCODER_99_040:[ After retrieval, it shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_041:[ A "\":" (single quote followed by colon) shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_042:[ If the child has zero children, then the function toStringFunc shall be invoked for the value (as computed by MultiTree_GetValue) of that child]*/
        /*Tests_SRS_JSON_ENCODER_99_047:[If toString function returns an error then JSON_Encoder_EncodeTree shall return JSON_ENCODER_TOSTRING_FUNCTION_ERROR]*/
        /*Tests_SRS_JSON_ENCODER_99_043:[ If the child has children, then JSONEncoder_EncodeTree  shall be invoked for every of these children.]*/
        /*Tests_SRS_JSON_ENCODER_99_044:[ A "," shall be added for every child, except the last one.]*/
        /*Tests_SRS_JSON_ENCODER_99_045:[ The string "}" shall be added to the output]*/
        /*This is the baseline for tree_3*/
        /*The below test shall be copy&pasted several times (as many as filing points there are, that
        is the number of STRICT_EXPECTED_CALLS + the number of blablalballbaFunction_call_count;*/
        TEST_FUNCTION(JSONEncoder_EncodeTree_3_fails_when_MultiTree_GetName_fails)
        {
            ///arrange

            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(3);
            EXPECTED_CALL((*mocks), STRING_c_str(IGNORED_PTR_ARG))
                .ExpectedTimesExactly(0);
            EXPECTED_CALL((*mocks), STRING_new())
                .ExpectedTimesExactly(1);
            EXPECTED_CALL((*mocks), STRING_delete(IGNORED_PTR_ARG))
                .ExpectedTimesExactly(1);

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_3, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_3, 0, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                .SetReturn(MULTITREE_ERROR);
                ;



            ///act
            auto result = JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_3, global_bufferTemp, TestFunc_NodesAreStrings);

            ///assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_MULTITREE_ERROR, result);


            
            ASSERT_ARE_EQUAL(tchar_ptr, _T(""), mocks->CompareActualAndExpectedCalls().c_str());
        }
        /*Tests_SRS_JSON_ENCODER_99_037:[ For every child, the following actions shall take place in order to produce name:value:]*/
        /*Tests_SRS_JSON_ENCODER_99_039:[ The child name shall be retrieved by a call to MultiTree_GetChild followed by a MultiTree_GetName.]*/
        /*Tests_SRS_JSON_ENCODER_99_040:[ After retrieval, it shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_041:[ A "\":" (single quote followed by colon) shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_042:[ If the child has zero children, then the function toStringFunc shall be invoked for the value (as computed by MultiTree_GetValue) of that child]*/
        /*Tests_SRS_JSON_ENCODER_99_047:[If toString function returns an error then JSON_Encoder_EncodeTree shall return JSON_ENCODER_TOSTRING_FUNCTION_ERROR]*/
        /*Tests_SRS_JSON_ENCODER_99_043:[ If the child has children, then JSONEncoder_EncodeTree  shall be invoked for every of these children.]*/
        /*Tests_SRS_JSON_ENCODER_99_044:[ A "," shall be added for every child, except the last one.]*/
        /*Tests_SRS_JSON_ENCODER_99_045:[ The string "}" shall be added to the output]*/
        /*This is the baseline for tree_3*/
        /*The below test shall be copy&pasted several times (as many as filing points there are, that
        is the number of STRICT_EXPECTED_CALLS + the number of blablalballbaFunction_call_count;*/
        TEST_FUNCTION(JSONEncoder_EncodeTree_3_fails_when_strcat_s_fails_2)
        {
            ///arrange
            EXPECTED_CALL((*mocks), STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(1);
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(3);
            EXPECTED_CALL((*mocks), STRING_new())
                .ExpectedTimesExactly(1);
            EXPECTED_CALL((*mocks), STRING_delete(IGNORED_PTR_ARG))
                .ExpectedTimesExactly(1);

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_3, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_3, 0, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            whenShallSTRING_concat_with_STRING_fail = 1;

            ///act
            auto result = JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_3, global_bufferTemp, TestFunc_NodesAreStrings);

            ///assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_ERROR, result);
            

            
            
            ASSERT_ARE_EQUAL(tchar_ptr, _T(""), mocks->CompareActualAndExpectedCalls().c_str());
        }
        /*Tests_SRS_JSON_ENCODER_99_037:[ For every child, the following actions shall take place in order to produce name:value:]*/
        /*Tests_SRS_JSON_ENCODER_99_039:[ The child name shall be retrieved by a call to MultiTree_GetChild followed by a MultiTree_GetName.]*/
        /*Tests_SRS_JSON_ENCODER_99_040:[ After retrieval, it shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_041:[ A "\":" (single quote followed by colon) shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_042:[ If the child has zero children, then the function toStringFunc shall be invoked for the value (as computed by MultiTree_GetValue) of that child]*/
        /*Tests_SRS_JSON_ENCODER_99_047:[If toString function returns an error then JSON_Encoder_EncodeTree shall return JSON_ENCODER_TOSTRING_FUNCTION_ERROR]*/
        /*Tests_SRS_JSON_ENCODER_99_043:[ If the child has children, then JSONEncoder_EncodeTree  shall be invoked for every of these children.]*/
        /*Tests_SRS_JSON_ENCODER_99_044:[ A "," shall be added for every child, except the last one.]*/
        /*Tests_SRS_JSON_ENCODER_99_045:[ The string "}" shall be added to the output]*/
        /*This is the baseline for tree_3*/
        /*The below test shall be copy&pasted several times (as many as filing points there are, that
        is the number of STRICT_EXPECTED_CALLS + the number of blablalballbaFunction_call_count;*/
        TEST_FUNCTION(JSONEncoder_EncodeTree_3_fails_when_MultiTree_GetChildCount_fails_2)
        {
            ///arrange
            EXPECTED_CALL((*mocks), STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(1);
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(4);
            EXPECTED_CALL((*mocks), STRING_new())
                .ExpectedTimesExactly(1);
            EXPECTED_CALL((*mocks), STRING_delete(IGNORED_PTR_ARG))
                .ExpectedTimesExactly(1);

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_3, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_3, 0, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                .SetReturn(MULTITREE_ERROR)
                ;


            ///act
            auto result = JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_3, global_bufferTemp, TestFunc_NodesAreStrings);

            ///assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_MULTITREE_ERROR, result);
            

            
            
            ASSERT_ARE_EQUAL(tchar_ptr, _T(""), mocks->CompareActualAndExpectedCalls().c_str());
        }
        /*Tests_SRS_JSON_ENCODER_99_037:[ For every child, the following actions shall take place in order to produce name:value:]*/
        /*Tests_SRS_JSON_ENCODER_99_039:[ The child name shall be retrieved by a call to MultiTree_GetChild followed by a MultiTree_GetName.]*/
        /*Tests_SRS_JSON_ENCODER_99_040:[ After retrieval, it shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_041:[ A "\":" (single quote followed by colon) shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_042:[ If the child has zero children, then the function toStringFunc shall be invoked for the value (as computed by MultiTree_GetValue) of that child]*/
        /*Tests_SRS_JSON_ENCODER_99_047:[If toString function returns an error then JSON_Encoder_EncodeTree shall return JSON_ENCODER_TOSTRING_FUNCTION_ERROR]*/
        /*Tests_SRS_JSON_ENCODER_99_043:[ If the child has children, then JSONEncoder_EncodeTree  shall be invoked for every of these children.]*/
        /*Tests_SRS_JSON_ENCODER_99_044:[ A "," shall be added for every child, except the last one.]*/
        /*Tests_SRS_JSON_ENCODER_99_045:[ The string "}" shall be added to the output]*/
        /*This is the baseline for tree_3*/
        /*The below test shall be copy&pasted several times (as many as filing points there are, that
        is the number of STRICT_EXPECTED_CALLS + the number of blablalballbaFunction_call_count;*/
        TEST_FUNCTION(JSONEncoder_EncodeTree_3_fails_when_MultiTree_GetValue_fails_1)
        {
            ///arrange
            EXPECTED_CALL((*mocks), STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(1);
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(4);
            EXPECTED_CALL((*mocks), STRING_new())
                .ExpectedTimesExactly(1);
            EXPECTED_CALL((*mocks), STRING_delete(IGNORED_PTR_ARG))
                .ExpectedTimesExactly(1);

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_3, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_3, 0, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetValue(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                .SetReturn(MULTITREE_ERROR)
                ;


            ///act
            auto result = JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_3, global_bufferTemp, TestFunc_NodesAreStrings);

            ///assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_MULTITREE_ERROR, result);
            

            
            
            ASSERT_ARE_EQUAL(tchar_ptr, _T(""), mocks->CompareActualAndExpectedCalls().c_str());
        }
        /*Tests_SRS_JSON_ENCODER_99_037:[ For every child, the following actions shall take place in order to produce name:value:]*/
        /*Tests_SRS_JSON_ENCODER_99_039:[ The child name shall be retrieved by a call to MultiTree_GetChild followed by a MultiTree_GetName.]*/
        /*Tests_SRS_JSON_ENCODER_99_040:[ After retrieval, it shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_041:[ A "\":" (single quote followed by colon) shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_042:[ If the child has zero children, then the function toStringFunc shall be invoked for the value (as computed by MultiTree_GetValue) of that child]*/
        /*Tests_SRS_JSON_ENCODER_99_047:[If toString function returns an error then JSON_Encoder_EncodeTree shall return JSON_ENCODER_TOSTRING_FUNCTION_ERROR]*/
        /*Tests_SRS_JSON_ENCODER_99_043:[ If the child has children, then JSONEncoder_EncodeTree  shall be invoked for every of these children.]*/
        /*Tests_SRS_JSON_ENCODER_99_044:[ A "," shall be added for every child, except the last one.]*/
        /*Tests_SRS_JSON_ENCODER_99_045:[ The string "}" shall be added to the output]*/
        /*This is the baseline for tree_3*/
        /*The below test shall be copy&pasted several times (as many as filing points there are, that
        is the number of STRICT_EXPECTED_CALLS + the number of blablalballbaFunction_call_count;*/
        TEST_FUNCTION(JSONEncoder_EncodeTree_3_fails_when_toStringFunc_fails)
        {
            ///arrange
            EXPECTED_CALL((*mocks), STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(1);
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(5);
            EXPECTED_CALL((*mocks), STRING_new())
                .ExpectedTimesExactly(2);
            EXPECTED_CALL((*mocks), STRING_delete(IGNORED_PTR_ARG))
                .ExpectedTimesExactly(2);

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_3, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_3, 0, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetValue(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), TestFunc_NodesAreStrings(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreAllArguments()
                .SetReturn(JSON_ENCODER_TOSTRING_ERROR)
                ;


            ///act
            auto result = JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_3, global_bufferTemp, TestFunc_NodesAreStrings);

            ///assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_TOSTRING_FUNCTION_ERROR, result);
            
            ASSERT_ARE_EQUAL(tchar_ptr, _T(""), mocks->CompareActualAndExpectedCalls().c_str());
        }
        /*Tests_SRS_JSON_ENCODER_99_037:[ For every child, the following actions shall take place in order to produce name:value:]*/
        /*Tests_SRS_JSON_ENCODER_99_039:[ The child name shall be retrieved by a call to MultiTree_GetChild followed by a MultiTree_GetName.]*/
        /*Tests_SRS_JSON_ENCODER_99_040:[ After retrieval, it shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_041:[ A "\":" (single quote followed by colon) shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_042:[ If the child has zero children, then the function toStringFunc shall be invoked for the value (as computed by MultiTree_GetValue) of that child]*/
        /*Tests_SRS_JSON_ENCODER_99_047:[If toString function returns an error then JSON_Encoder_EncodeTree shall return JSON_ENCODER_TOSTRING_FUNCTION_ERROR]*/
        /*Tests_SRS_JSON_ENCODER_99_043:[ If the child has children, then JSONEncoder_EncodeTree  shall be invoked for every of these children.]*/
        /*Tests_SRS_JSON_ENCODER_99_044:[ A "," shall be added for every child, except the last one.]*/
        /*Tests_SRS_JSON_ENCODER_99_045:[ The string "}" shall be added to the output]*/
        /*This is the baseline for tree_3*/
        /*The below test shall be copy&pasted several times (as many as filing points there are, that
        is the number of STRICT_EXPECTED_CALLS + the number of blablalballbaFunction_call_count;*/
        /*until now, the child1 has been added, this start adding the child2*/
        TEST_FUNCTION(JSONEncoder_EncodeTree_3_fails_when_strcat_s_fails_3)
        {
            ///arrange
            EXPECTED_CALL((*mocks), STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(2);
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(5);
            EXPECTED_CALL((*mocks), STRING_new())
                .ExpectedTimesExactly(2);
            EXPECTED_CALL((*mocks), STRING_delete(IGNORED_PTR_ARG))
                .ExpectedTimesExactly(2);

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_3, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_3, 0, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetValue(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), TestFunc_NodesAreStrings(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreAllArguments()
                ;

            whenShallSTRING_concat_with_STRING_fail = 2;

            ///act
            auto result = JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_3, global_bufferTemp, TestFunc_NodesAreStrings);

            ///assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_ERROR, result);
            

            
            

            ASSERT_ARE_EQUAL(tchar_ptr, _T(""), mocks->CompareActualAndExpectedCalls().c_str());
        }
        /*Tests_SRS_JSON_ENCODER_99_037:[ For every child, the following actions shall take place in order to produce name:value:]*/
        /*Tests_SRS_JSON_ENCODER_99_039:[ The child name shall be retrieved by a call to MultiTree_GetChild followed by a MultiTree_GetName.]*/
        /*Tests_SRS_JSON_ENCODER_99_040:[ After retrieval, it shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_041:[ A "\":" (single quote followed by colon) shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_042:[ If the child has zero children, then the function toStringFunc shall be invoked for the value (as computed by MultiTree_GetValue) of that child]*/
        /*Tests_SRS_JSON_ENCODER_99_047:[If toString function returns an error then JSON_Encoder_EncodeTree shall return JSON_ENCODER_TOSTRING_FUNCTION_ERROR]*/
        /*Tests_SRS_JSON_ENCODER_99_043:[ If the child has children, then JSONEncoder_EncodeTree  shall be invoked for every of these children.]*/
        /*Tests_SRS_JSON_ENCODER_99_044:[ A "," shall be added for every child, except the last one.]*/
        /*Tests_SRS_JSON_ENCODER_99_045:[ The string "}" shall be added to the output]*/
        /*This is the baseline for tree_3*/
        /*The below test shall be copy&pasted several times (as many as filing points there are, that
        is the number of STRICT_EXPECTED_CALLS + the number of blablalballbaFunction_call_count;*/
        TEST_FUNCTION(JSONEncoder_EncodeTree_3_fails_when_strcat_s_fails_4)
        {
            ///arrange
            EXPECTED_CALL((*mocks), STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(2);
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(5);
            EXPECTED_CALL((*mocks), STRING_new())
                .ExpectedTimesExactly(2);
            EXPECTED_CALL((*mocks), STRING_delete(IGNORED_PTR_ARG))
                .ExpectedTimesExactly(2);

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_3, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_3, 0, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetValue(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), TestFunc_NodesAreStrings(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreAllArguments()
                ;

            whenShallSTRING_concat_with_STRING_fail = 2;

            ///act
            auto result = JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_3, global_bufferTemp, TestFunc_NodesAreStrings);

            ///assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_ERROR, result);

            ASSERT_ARE_EQUAL(tchar_ptr, _T(""), mocks->CompareActualAndExpectedCalls().c_str());
        }
        /*Tests_SRS_JSON_ENCODER_99_037:[ For every child, the following actions shall take place in order to produce name:value:]*/
        /*Tests_SRS_JSON_ENCODER_99_039:[ The child name shall be retrieved by a call to MultiTree_GetChild followed by a MultiTree_GetName.]*/
        /*Tests_SRS_JSON_ENCODER_99_040:[ After retrieval, it shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_041:[ A "\":" (single quote followed by colon) shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_042:[ If the child has zero children, then the function toStringFunc shall be invoked for the value (as computed by MultiTree_GetValue) of that child]*/
        /*Tests_SRS_JSON_ENCODER_99_047:[If toString function returns an error then JSON_Encoder_EncodeTree shall return JSON_ENCODER_TOSTRING_FUNCTION_ERROR]*/
        /*Tests_SRS_JSON_ENCODER_99_043:[ If the child has children, then JSONEncoder_EncodeTree  shall be invoked for every of these children.]*/
        /*Tests_SRS_JSON_ENCODER_99_044:[ A "," shall be added for every child, except the last one.]*/
        /*Tests_SRS_JSON_ENCODER_99_045:[ The string "}" shall be added to the output]*/
        /*This is the baseline for tree_3*/
        /*The below test shall be copy&pasted several times (as many as filing points there are, that
        is the number of STRICT_EXPECTED_CALLS + the number of blablalballbaFunction_call_count;*/
        TEST_FUNCTION(JSONEncoder_EncodeTree_3_fails_when_MultiTree_GetChild_fails_2)
        {
            ///arrange
            EXPECTED_CALL((*mocks), STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(2);
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(7);
            EXPECTED_CALL((*mocks), STRING_new())
                .ExpectedTimesExactly(3);
            EXPECTED_CALL((*mocks), STRING_delete(IGNORED_PTR_ARG))
                .ExpectedTimesExactly(3);

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_3, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_3, 0, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetValue(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), TestFunc_NodesAreStrings(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreAllArguments()
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_3, 1, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                .SetReturn(MULTITREE_ERROR)
                ;


            ///act
            auto result = JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_3, global_bufferTemp, TestFunc_NodesAreStrings);

            ///assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_MULTITREE_ERROR, result);
            

            
            

            
            ASSERT_ARE_EQUAL(tchar_ptr, _T(""), mocks->CompareActualAndExpectedCalls().c_str());
        }
        /*Tests_SRS_JSON_ENCODER_99_037:[ For every child, the following actions shall take place in order to produce name:value:]*/
        /*Tests_SRS_JSON_ENCODER_99_039:[ The child name shall be retrieved by a call to MultiTree_GetChild followed by a MultiTree_GetName.]*/
        /*Tests_SRS_JSON_ENCODER_99_040:[ After retrieval, it shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_041:[ A "\":" (single quote followed by colon) shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_042:[ If the child has zero children, then the function toStringFunc shall be invoked for the value (as computed by MultiTree_GetValue) of that child]*/
        /*Tests_SRS_JSON_ENCODER_99_047:[If toString function returns an error then JSON_Encoder_EncodeTree shall return JSON_ENCODER_TOSTRING_FUNCTION_ERROR]*/
        /*Tests_SRS_JSON_ENCODER_99_043:[ If the child has children, then JSONEncoder_EncodeTree  shall be invoked for every of these children.]*/
        /*Tests_SRS_JSON_ENCODER_99_044:[ A "," shall be added for every child, except the last one.]*/
        /*Tests_SRS_JSON_ENCODER_99_045:[ The string "}" shall be added to the output]*/
        /*This is the baseline for tree_3*/
        /*The below test shall be copy&pasted several times (as many as filing points there are, that
        is the number of STRICT_EXPECTED_CALLS + the number of blablalballbaFunction_call_count;*/
        TEST_FUNCTION(JSONEncoder_EncodeTree_3_fails_when_MultiTree_GetName_fails_2)
        {
            ///arrange
            EXPECTED_CALL((*mocks), STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(2);
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(8);
            EXPECTED_CALL((*mocks), STRING_new())
                .ExpectedTimesExactly(3);
            EXPECTED_CALL((*mocks), STRING_delete(IGNORED_PTR_ARG))
                .ExpectedTimesExactly(3);

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_3, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_3, 0, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetValue(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), TestFunc_NodesAreStrings(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreAllArguments()
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_3, 1, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_2, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                .SetReturn(MULTITREE_ERROR)
                ;


            ///act
            auto result = JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_3, global_bufferTemp,  TestFunc_NodesAreStrings);

            ///assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_MULTITREE_ERROR, result);
            

            
            

            
            ASSERT_ARE_EQUAL(tchar_ptr, _T(""), mocks->CompareActualAndExpectedCalls().c_str());
        }
        /*Tests_SRS_JSON_ENCODER_99_037:[ For every child, the following actions shall take place in order to produce name:value:]*/
        /*Tests_SRS_JSON_ENCODER_99_039:[ The child name shall be retrieved by a call to MultiTree_GetChild followed by a MultiTree_GetName.]*/
        /*Tests_SRS_JSON_ENCODER_99_040:[ After retrieval, it shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_041:[ A "\":" (single quote followed by colon) shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_042:[ If the child has zero children, then the function toStringFunc shall be invoked for the value (as computed by MultiTree_GetValue) of that child]*/
        /*Tests_SRS_JSON_ENCODER_99_047:[If toString function returns an error then JSON_Encoder_EncodeTree shall return JSON_ENCODER_TOSTRING_FUNCTION_ERROR]*/
        /*Tests_SRS_JSON_ENCODER_99_043:[ If the child has children, then JSONEncoder_EncodeTree  shall be invoked for every of these children.]*/
        /*Tests_SRS_JSON_ENCODER_99_044:[ A "," shall be added for every child, except the last one.]*/
        /*Tests_SRS_JSON_ENCODER_99_045:[ The string "}" shall be added to the output]*/
        /*This is the baseline for tree_3*/
        /*The below test shall be copy&pasted several times (as many as filing points there are, that
        is the number of STRICT_EXPECTED_CALLS + the number of blablalballbaFunction_call_count;*/
        TEST_FUNCTION(JSONEncoder_EncodeTree_3_fails_when_strcat_s_fails_5)
        {
            ///arrange
            EXPECTED_CALL((*mocks), STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(3);
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(8);
            EXPECTED_CALL((*mocks), STRING_new())
                .ExpectedTimesExactly(3);
            EXPECTED_CALL((*mocks), STRING_delete(IGNORED_PTR_ARG))
                .ExpectedTimesExactly(3);


            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_3, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_3, 0, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetValue(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), TestFunc_NodesAreStrings(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreAllArguments()
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_3, 1, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_2, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            whenShallSTRING_concat_with_STRING_fail = 3;

            ///act
            auto result = JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_3, global_bufferTemp,  TestFunc_NodesAreStrings);

            ///assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_ERROR, result);
            

            
            

            
            
            ASSERT_ARE_EQUAL(tchar_ptr, _T(""), mocks->CompareActualAndExpectedCalls().c_str());
        }
        /*Tests_SRS_JSON_ENCODER_99_037:[ For every child, the following actions shall take place in order to produce name:value:]*/
        /*Tests_SRS_JSON_ENCODER_99_039:[ The child name shall be retrieved by a call to MultiTree_GetChild followed by a MultiTree_GetName.]*/
        /*Tests_SRS_JSON_ENCODER_99_040:[ After retrieval, it shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_041:[ A "\":" (single quote followed by colon) shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_042:[ If the child has zero children, then the function toStringFunc shall be invoked for the value (as computed by MultiTree_GetValue) of that child]*/
        /*Tests_SRS_JSON_ENCODER_99_047:[If toString function returns an error then JSON_Encoder_EncodeTree shall return JSON_ENCODER_TOSTRING_FUNCTION_ERROR]*/
        /*Tests_SRS_JSON_ENCODER_99_043:[ If the child has children, then JSONEncoder_EncodeTree  shall be invoked for every of these children.]*/
        /*Tests_SRS_JSON_ENCODER_99_044:[ A "," shall be added for every child, except the last one.]*/
        /*Tests_SRS_JSON_ENCODER_99_045:[ The string "}" shall be added to the output]*/
        /*This is the baseline for tree_3*/
        /*The below test shall be copy&pasted several times (as many as filing points there are, that
        is the number of STRICT_EXPECTED_CALLS + the number of blablalballbaFunction_call_count;*/
        TEST_FUNCTION(JSONEncoder_EncodeTree_3_fails_when_MultiTree_GetChildCount_fails_3)
        {
            ///arrange
            EXPECTED_CALL((*mocks), STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(3);
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(9);
            EXPECTED_CALL((*mocks), STRING_new())
                .ExpectedTimesExactly(3);
            EXPECTED_CALL((*mocks), STRING_delete(IGNORED_PTR_ARG))
                .ExpectedTimesExactly(3);

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_3, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_3, 0, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetValue(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), TestFunc_NodesAreStrings(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreAllArguments()
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_3, 1, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_2, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_CHILD_2, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                .SetReturn(MULTITREE_ERROR)
                ;


            ///act
            auto result = JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_3, global_bufferTemp,  TestFunc_NodesAreStrings);

            ///assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_MULTITREE_ERROR, result);
            
            
            

            
            
            ASSERT_ARE_EQUAL(tchar_ptr, _T(""), mocks->CompareActualAndExpectedCalls().c_str());
        }
        /*Tests_SRS_JSON_ENCODER_99_037:[ For every child, the following actions shall take place in order to produce name:value:]*/
        /*Tests_SRS_JSON_ENCODER_99_039:[ The child name shall be retrieved by a call to MultiTree_GetChild followed by a MultiTree_GetName.]*/
        /*Tests_SRS_JSON_ENCODER_99_040:[ After retrieval, it shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_041:[ A "\":" (single quote followed by colon) shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_042:[ If the child has zero children, then the function toStringFunc shall be invoked for the value (as computed by MultiTree_GetValue) of that child]*/
        /*Tests_SRS_JSON_ENCODER_99_047:[If toString function returns an error then JSON_Encoder_EncodeTree shall return JSON_ENCODER_TOSTRING_FUNCTION_ERROR]*/
        /*Tests_SRS_JSON_ENCODER_99_043:[ If the child has children, then JSONEncoder_EncodeTree  shall be invoked for every of these children.]*/
        /*Tests_SRS_JSON_ENCODER_99_044:[ A "," shall be added for every child, except the last one.]*/
        /*Tests_SRS_JSON_ENCODER_99_045:[ The string "}" shall be added to the output]*/
        /*This is the baseline for tree_3*/
        /*The below test shall be copy&pasted several times (as many as filing points there are, that
        is the number of STRICT_EXPECTED_CALLS + the number of blablalballbaFunction_call_count;*/
        TEST_FUNCTION(JSONEncoder_EncodeTree_3_fails_when_MultiTree_GetValue_fails_2)
        {
            ///arrange
            EXPECTED_CALL((*mocks), STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(3);
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(9);
            EXPECTED_CALL((*mocks), STRING_new())
                .ExpectedTimesExactly(3);
            EXPECTED_CALL((*mocks), STRING_delete(IGNORED_PTR_ARG))
                .ExpectedTimesExactly(3);

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_3, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_3, 0, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetValue(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), TestFunc_NodesAreStrings(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreAllArguments()
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_3, 1, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_2, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_CHILD_2, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetValue(TEST_MULTITREE_HANDLE_CHILD_2, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                .SetReturn(MULTITREE_ERROR)
                ;


            ///act
            auto result = JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_3, global_bufferTemp,  TestFunc_NodesAreStrings);

            ///assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_MULTITREE_ERROR, result);
            
            
            

            
            
            ASSERT_ARE_EQUAL(tchar_ptr, _T(""), mocks->CompareActualAndExpectedCalls().c_str());
        }
        /*Tests_SRS_JSON_ENCODER_99_037:[ For every child, the following actions shall take place in order to produce name:value:]*/
        /*Tests_SRS_JSON_ENCODER_99_039:[ The child name shall be retrieved by a call to MultiTree_GetChild followed by a MultiTree_GetName.]*/
        /*Tests_SRS_JSON_ENCODER_99_040:[ After retrieval, it shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_041:[ A "\":" (single quote followed by colon) shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_042:[ If the child has zero children, then the function toStringFunc shall be invoked for the value (as computed by MultiTree_GetValue) of that child]*/
        /*Tests_SRS_JSON_ENCODER_99_047:[If toString function returns an error then JSON_Encoder_EncodeTree shall return JSON_ENCODER_TOSTRING_FUNCTION_ERROR]*/
        /*Tests_SRS_JSON_ENCODER_99_043:[ If the child has children, then JSONEncoder_EncodeTree  shall be invoked for every of these children.]*/
        /*Tests_SRS_JSON_ENCODER_99_044:[ A "," shall be added for every child, except the last one.]*/
        /*Tests_SRS_JSON_ENCODER_99_045:[ The string "}" shall be added to the output]*/
        /*This is the baseline for tree_3*/
        /*The below test shall be copy&pasted several times (as many as filing points there are, that
        is the number of STRICT_EXPECTED_CALLS + the number of blablalballbaFunction_call_count;*/
        TEST_FUNCTION(JSONEncoder_EncodeTree_3_fails_when_toStringFunc_fails_2)
        {
            ///arrange
            EXPECTED_CALL((*mocks), STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(3);
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(10);
            EXPECTED_CALL((*mocks), STRING_new())
                .ExpectedTimesExactly(4);
            EXPECTED_CALL((*mocks), STRING_delete(IGNORED_PTR_ARG))
                .ExpectedTimesExactly(4);

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_3, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_3, 0, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetValue(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), TestFunc_NodesAreStrings(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreAllArguments()
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_3, 1, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_2, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_CHILD_2, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetValue(TEST_MULTITREE_HANDLE_CHILD_2, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), TestFunc_NodesAreStrings(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreAllArguments()
                .SetReturn(JSON_ENCODER_TOSTRING_ERROR)
                ;


            ///act
            auto result = JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_3, global_bufferTemp,  TestFunc_NodesAreStrings);

            ///assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_TOSTRING_FUNCTION_ERROR, result);
            
            
            

            
            
            ASSERT_ARE_EQUAL(tchar_ptr, _T(""), mocks->CompareActualAndExpectedCalls().c_str());
        }
        /*Tests_SRS_JSON_ENCODER_99_037:[ For every child, the following actions shall take place in order to produce name:value:]*/
        /*Tests_SRS_JSON_ENCODER_99_039:[ The child name shall be retrieved by a call to MultiTree_GetChild followed by a MultiTree_GetName.]*/
        /*Tests_SRS_JSON_ENCODER_99_040:[ After retrieval, it shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_041:[ A "\":" (single quote followed by colon) shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_042:[ If the child has zero children, then the function toStringFunc shall be invoked for the value (as computed by MultiTree_GetValue) of that child]*/
        /*Tests_SRS_JSON_ENCODER_99_047:[If toString function returns an error then JSON_Encoder_EncodeTree shall return JSON_ENCODER_TOSTRING_FUNCTION_ERROR]*/
        /*Tests_SRS_JSON_ENCODER_99_043:[ If the child has children, then JSONEncoder_EncodeTree  shall be invoked for every of these children.]*/
        /*Tests_SRS_JSON_ENCODER_99_044:[ A "," shall be added for every child, except the last one.]*/
        /*Tests_SRS_JSON_ENCODER_99_045:[ The string "}" shall be added to the output]*/
        /*This is the baseline for tree_3*/
        /*The below test shall be copy&pasted several times (as many as filing points there are, that
        is the number of STRICT_EXPECTED_CALLS + the number of blablalballbaFunction_call_count;*/
        TEST_FUNCTION(JSONEncoder_EncodeTree_3_fails_when_strcat_s_fails_6)
        {
            ///arrange
            EXPECTED_CALL((*mocks), STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(4);
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(10);
            EXPECTED_CALL((*mocks), STRING_new())
                .ExpectedTimesExactly(4);
            EXPECTED_CALL((*mocks), STRING_delete(IGNORED_PTR_ARG))
                .ExpectedTimesExactly(4);

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_3, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_3, 0, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetValue(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), TestFunc_NodesAreStrings(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreAllArguments()
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_3, 1, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_2, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_CHILD_2, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetValue(TEST_MULTITREE_HANDLE_CHILD_2, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), TestFunc_NodesAreStrings(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreAllArguments()
                ;

            whenShallSTRING_concat_with_STRING_fail = 4;

            ///act
            auto result = JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_3, global_bufferTemp,  TestFunc_NodesAreStrings);

            ///assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_ERROR, result);
            

            
            

            
            

            ASSERT_ARE_EQUAL(tchar_ptr, _T(""), mocks->CompareActualAndExpectedCalls().c_str());
        }
        /*Tests_SRS_JSON_ENCODER_99_037:[ For every child, the following actions shall take place in order to produce name:value:]*/
        /*Tests_SRS_JSON_ENCODER_99_039:[ The child name shall be retrieved by a call to MultiTree_GetChild followed by a MultiTree_GetName.]*/
        /*Tests_SRS_JSON_ENCODER_99_040:[ After retrieval, it shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_041:[ A "\":" (single quote followed by colon) shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_042:[ If the child has zero children, then the function toStringFunc shall be invoked for the value (as computed by MultiTree_GetValue) of that child]*/
        /*Tests_SRS_JSON_ENCODER_99_047:[If toString function returns an error then JSON_Encoder_EncodeTree shall return JSON_ENCODER_TOSTRING_FUNCTION_ERROR]*/
        /*Tests_SRS_JSON_ENCODER_99_043:[ If the child has children, then JSONEncoder_EncodeTree  shall be invoked for every of these children.]*/
        /*Tests_SRS_JSON_ENCODER_99_044:[ A "," shall be added for every child, except the last one.]*/
        /*Tests_SRS_JSON_ENCODER_99_045:[ The string "}" shall be added to the output]*/
        /*This is the baseline for tree_3*/
        /*The below test shall be copy&pasted several times (as many as filing points there are, that
        is the number of STRICT_EXPECTED_CALLS + the number of blablalballbaFunction_call_count;*/
        /*hahha, tree 3 had 1000 lines of code. this one might have more. ANd I dare not think of those bigger trees*/
        TEST_FUNCTION(JSONEncoder_EncodeTree_4_success)
        {
            ///arrange
            EXPECTED_CALL((*mocks), STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(6);
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(16);
            EXPECTED_CALL((*mocks), STRING_new())
                .ExpectedTimesExactly(6);
            EXPECTED_CALL((*mocks), STRING_delete(IGNORED_PTR_ARG))
                .ExpectedTimesExactly(6);

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_4, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_4, 0, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetValue(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), TestFunc_NodesAreStrings(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreAllArguments()
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_4, 1, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_2, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_CHILD_2, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetValue(TEST_MULTITREE_HANDLE_CHILD_2, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), TestFunc_NodesAreStrings(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreAllArguments()
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_4, 2, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_3, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_CHILD_3, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetValue(TEST_MULTITREE_HANDLE_CHILD_3, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), TestFunc_NodesAreStrings(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreAllArguments()
                ;


            ///act
            auto result = JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_4, global_bufferTemp,  TestFunc_NodesAreStrings);

            ///assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_OK, result);
            ASSERT_ARE_EQUAL(char_ptr, "{\"child1\":\"value1\", \"child2\":\"value2\", \"child3\":\"value3\"}", BASEIMPLEMENTATION::STRING_c_str(global_bufferTemp));
            

            
            

            
            

            
            

            ASSERT_ARE_EQUAL(tchar_ptr, _T(""), mocks->CompareActualAndExpectedCalls().c_str());
        }
        

        /*Tests_SRS_JSON_ENCODER_99_037:[ For every child, the following actions shall take place in order to produce name:value:]*/
        /*Tests_SRS_JSON_ENCODER_99_039:[ The child name shall be retrieved by a call to MultiTree_GetChild followed by a MultiTree_GetName.]*/
        /*Tests_SRS_JSON_ENCODER_99_040:[ After retrieval, it shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_041:[ A "\":" (single quote followed by colon) shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_042:[ If the child has zero children, then the function toStringFunc shall be invoked for the value (as computed by MultiTree_GetValue) of that child]*/
        /*Tests_SRS_JSON_ENCODER_99_047:[If toString function returns an error then JSON_Encoder_EncodeTree shall return JSON_ENCODER_TOSTRING_FUNCTION_ERROR]*/
        /*Tests_SRS_JSON_ENCODER_99_043:[ If the child has children, then JSONEncoder_EncodeTree  shall be invoked for every of these children.]*/
        /*Tests_SRS_JSON_ENCODER_99_044:[ A "," shall be added for every child, except the last one.]*/
        /*Tests_SRS_JSON_ENCODER_99_045:[ The string "}" shall be added to the output]*/
        /*This is the baseline for tree_4*/
        /*The below test shall be copy&pasted several times (as many as filing points there are, that
        is the number of STRICT_EXPECTED_CALLS + the number of blablalballbaFunction_call_count;*/
        TEST_FUNCTION(JSONEncoder_EncodeTree_4_fails_when_MultiTree_GetChildCount_fails)
        {
            ///arrange

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_4, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                .SetReturn(MULTITREE_ERROR)
                ;


            ///act
            auto result = JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_4, global_bufferTemp,  TestFunc_NodesAreStrings);

            ///assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_MULTITREE_ERROR, result);
            ASSERT_ARE_EQUAL(tchar_ptr, _T(""), mocks->CompareActualAndExpectedCalls().c_str());
        }
        /*Tests_SRS_JSON_ENCODER_99_037:[ For every child, the following actions shall take place in order to produce name:value:]*/
        /*Tests_SRS_JSON_ENCODER_99_039:[ The child name shall be retrieved by a call to MultiTree_GetChild followed by a MultiTree_GetName.]*/
        /*Tests_SRS_JSON_ENCODER_99_040:[ After retrieval, it shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_041:[ A "\":" (single quote followed by colon) shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_042:[ If the child has zero children, then the function toStringFunc shall be invoked for the value (as computed by MultiTree_GetValue) of that child]*/
        /*Tests_SRS_JSON_ENCODER_99_047:[If toString function returns an error then JSON_Encoder_EncodeTree shall return JSON_ENCODER_TOSTRING_FUNCTION_ERROR]*/
        /*Tests_SRS_JSON_ENCODER_99_043:[ If the child has children, then JSONEncoder_EncodeTree  shall be invoked for every of these children.]*/
        /*Tests_SRS_JSON_ENCODER_99_044:[ A "," shall be added for every child, except the last one.]*/
        /*Tests_SRS_JSON_ENCODER_99_045:[ The string "}" shall be added to the output]*/
        /*This is the baseline for tree_4*/
        /*The below test shall be copy&pasted several times (as many as filing points there are, that
        is the number of STRICT_EXPECTED_CALLS + the number of blablalballbaFunction_call_count;*/
        TEST_FUNCTION(JSONEncoder_EncodeTree_4_fails_when_strcpy_s_fails_1)
        {
            ///arrange

            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(1);
            EXPECTED_CALL((*mocks), STRING_c_str(IGNORED_PTR_ARG))
                .ExpectedTimesExactly(0);
            EXPECTED_CALL((*mocks), STRING_new())
                .ExpectedTimesExactly(0);
            EXPECTED_CALL((*mocks), STRING_delete(IGNORED_PTR_ARG))
                .ExpectedTimesExactly(0);

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_4, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            whenShallSTRING_concat_fail = 1;

            ///act
            auto result = JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_4, global_bufferTemp,  TestFunc_NodesAreStrings);

            ///assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_ERROR, result);
            
            ASSERT_ARE_EQUAL(tchar_ptr, _T(""), mocks->CompareActualAndExpectedCalls().c_str());
        }
        /*Tests_SRS_JSON_ENCODER_99_037:[ For every child, the following actions shall take place in order to produce name:value:]*/
        /*Tests_SRS_JSON_ENCODER_99_039:[ The child name shall be retrieved by a call to MultiTree_GetChild followed by a MultiTree_GetName.]*/
        /*Tests_SRS_JSON_ENCODER_99_040:[ After retrieval, it shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_041:[ A "\":" (single quote followed by colon) shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_042:[ If the child has zero children, then the function toStringFunc shall be invoked for the value (as computed by MultiTree_GetValue) of that child]*/
        /*Tests_SRS_JSON_ENCODER_99_047:[If toString function returns an error then JSON_Encoder_EncodeTree shall return JSON_ENCODER_TOSTRING_FUNCTION_ERROR]*/
        /*Tests_SRS_JSON_ENCODER_99_043:[ If the child has children, then JSONEncoder_EncodeTree  shall be invoked for every of these children.]*/
        /*Tests_SRS_JSON_ENCODER_99_044:[ A "," shall be added for every child, except the last one.]*/
        /*Tests_SRS_JSON_ENCODER_99_045:[ The string "}" shall be added to the output]*/
        /*This is the baseline for tree_4*/
        /*The below test shall be copy&pasted several times (as many as filing points there are, that
        is the number of STRICT_EXPECTED_CALLS + the number of blablalballbaFunction_call_count;*/
        TEST_FUNCTION(JSONEncoder_EncodeTree_4_fails_when_MultiTree_GetChild_fails_1)
        {
            ///arrange

            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(2);
            EXPECTED_CALL((*mocks), STRING_c_str(IGNORED_PTR_ARG))
                .ExpectedTimesExactly(0);
            EXPECTED_CALL((*mocks), STRING_new())
                .ExpectedTimesExactly(1);
            EXPECTED_CALL((*mocks), STRING_delete(IGNORED_PTR_ARG))
                .ExpectedTimesExactly(1);

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_4, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_4, 0, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                .SetReturn(MULTITREE_ERROR)
                ;


            ///act
            auto result = JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_4, global_bufferTemp,  TestFunc_NodesAreStrings);

            ///assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_MULTITREE_ERROR, result);
            

            
            ASSERT_ARE_EQUAL(tchar_ptr, _T(""), mocks->CompareActualAndExpectedCalls().c_str());
        }
        /*Tests_SRS_JSON_ENCODER_99_037:[ For every child, the following actions shall take place in order to produce name:value:]*/
        /*Tests_SRS_JSON_ENCODER_99_039:[ The child name shall be retrieved by a call to MultiTree_GetChild followed by a MultiTree_GetName.]*/
        /*Tests_SRS_JSON_ENCODER_99_040:[ After retrieval, it shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_041:[ A "\":" (single quote followed by colon) shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_042:[ If the child has zero children, then the function toStringFunc shall be invoked for the value (as computed by MultiTree_GetValue) of that child]*/
        /*Tests_SRS_JSON_ENCODER_99_047:[If toString function returns an error then JSON_Encoder_EncodeTree shall return JSON_ENCODER_TOSTRING_FUNCTION_ERROR]*/
        /*Tests_SRS_JSON_ENCODER_99_043:[ If the child has children, then JSONEncoder_EncodeTree  shall be invoked for every of these children.]*/
        /*Tests_SRS_JSON_ENCODER_99_044:[ A "," shall be added for every child, except the last one.]*/
        /*Tests_SRS_JSON_ENCODER_99_045:[ The string "}" shall be added to the output]*/
        /*This is the baseline for tree_4*/
        /*The below test shall be copy&pasted several times (as many as filing points there are, that
        is the number of STRICT_EXPECTED_CALLS + the number of blablalballbaFunction_call_count;*/
        TEST_FUNCTION(JSONEncoder_EncodeTree_4_fails_when_MultiTree_GetName_fails_1)
        {
            ///arrange

            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(3);
            EXPECTED_CALL((*mocks), STRING_c_str(IGNORED_PTR_ARG))
                .ExpectedTimesExactly(0);
            EXPECTED_CALL((*mocks), STRING_new())
                .ExpectedTimesExactly(1);
            EXPECTED_CALL((*mocks), STRING_delete(IGNORED_PTR_ARG))
                .ExpectedTimesExactly(1);


            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_4, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_4, 0, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                .SetReturn(MULTITREE_ERROR)
                ;


            ///act
            auto result = JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_4, global_bufferTemp,  TestFunc_NodesAreStrings);

            ///assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_MULTITREE_ERROR, result);
            

            
            ASSERT_ARE_EQUAL(tchar_ptr, _T(""), mocks->CompareActualAndExpectedCalls().c_str());
        }
        /*Tests_SRS_JSON_ENCODER_99_037:[ For every child, the following actions shall take place in order to produce name:value:]*/
        /*Tests_SRS_JSON_ENCODER_99_039:[ The child name shall be retrieved by a call to MultiTree_GetChild followed by a MultiTree_GetName.]*/
        /*Tests_SRS_JSON_ENCODER_99_040:[ After retrieval, it shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_041:[ A "\":" (single quote followed by colon) shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_042:[ If the child has zero children, then the function toStringFunc shall be invoked for the value (as computed by MultiTree_GetValue) of that child]*/
        /*Tests_SRS_JSON_ENCODER_99_047:[If toString function returns an error then JSON_Encoder_EncodeTree shall return JSON_ENCODER_TOSTRING_FUNCTION_ERROR]*/
        /*Tests_SRS_JSON_ENCODER_99_043:[ If the child has children, then JSONEncoder_EncodeTree  shall be invoked for every of these children.]*/
        /*Tests_SRS_JSON_ENCODER_99_044:[ A "," shall be added for every child, except the last one.]*/
        /*Tests_SRS_JSON_ENCODER_99_045:[ The string "}" shall be added to the output]*/
        /*This is the baseline for tree_4*/
        /*The below test shall be copy&pasted several times (as many as filing points there are, that
        is the number of STRICT_EXPECTED_CALLS + the number of blablalballbaFunction_call_count;*/
        TEST_FUNCTION(JSONEncoder_EncodeTree_4_fails_when_strcat_s_fails_2)
        {
            ///arrange
            EXPECTED_CALL((*mocks), STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(1);
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(3);
            EXPECTED_CALL((*mocks), STRING_new())
                .ExpectedTimesExactly(1);
            EXPECTED_CALL((*mocks), STRING_delete(IGNORED_PTR_ARG))
                .ExpectedTimesExactly(1);


            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_4, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_4, 0, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            whenShallSTRING_concat_with_STRING_fail = 1;

            ///act
            auto result = JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_4, global_bufferTemp,  TestFunc_NodesAreStrings);

            ///assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_ERROR, result);
            

            
            
            ASSERT_ARE_EQUAL(tchar_ptr, _T(""), mocks->CompareActualAndExpectedCalls().c_str());
        }
        /*Tests_SRS_JSON_ENCODER_99_037:[ For every child, the following actions shall take place in order to produce name:value:]*/
        /*Tests_SRS_JSON_ENCODER_99_039:[ The child name shall be retrieved by a call to MultiTree_GetChild followed by a MultiTree_GetName.]*/
        /*Tests_SRS_JSON_ENCODER_99_040:[ After retrieval, it shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_041:[ A "\":" (single quote followed by colon) shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_042:[ If the child has zero children, then the function toStringFunc shall be invoked for the value (as computed by MultiTree_GetValue) of that child]*/
        /*Tests_SRS_JSON_ENCODER_99_047:[If toString function returns an error then JSON_Encoder_EncodeTree shall return JSON_ENCODER_TOSTRING_FUNCTION_ERROR]*/
        /*Tests_SRS_JSON_ENCODER_99_043:[ If the child has children, then JSONEncoder_EncodeTree  shall be invoked for every of these children.]*/
        /*Tests_SRS_JSON_ENCODER_99_044:[ A "," shall be added for every child, except the last one.]*/
        /*Tests_SRS_JSON_ENCODER_99_045:[ The string "}" shall be added to the output]*/
        /*This is the baseline for tree_4*/
        /*The below test shall be copy&pasted several times (as many as filing points there are, that
        is the number of STRICT_EXPECTED_CALLS + the number of blablalballbaFunction_call_count;*/
        TEST_FUNCTION(JSONEncoder_EncodeTree_4_fails_when_MultiTree_GetChildCount_fails_2)
        {
            ///arrange
            EXPECTED_CALL((*mocks), STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(1);
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(4);
            EXPECTED_CALL((*mocks), STRING_new())
                .ExpectedTimesExactly(1);
            EXPECTED_CALL((*mocks), STRING_delete(IGNORED_PTR_ARG))
                .ExpectedTimesExactly(1);

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_4, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_4, 0, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                .SetReturn(MULTITREE_ERROR)
                ;


            ///act
            auto result = JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_4, global_bufferTemp,  TestFunc_NodesAreStrings);

            ///assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_MULTITREE_ERROR, result);
            
            
            
            ASSERT_ARE_EQUAL(tchar_ptr, _T(""), mocks->CompareActualAndExpectedCalls().c_str());
        }
        /*Tests_SRS_JSON_ENCODER_99_037:[ For every child, the following actions shall take place in order to produce name:value:]*/
        /*Tests_SRS_JSON_ENCODER_99_039:[ The child name shall be retrieved by a call to MultiTree_GetChild followed by a MultiTree_GetName.]*/
        /*Tests_SRS_JSON_ENCODER_99_040:[ After retrieval, it shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_041:[ A "\":" (single quote followed by colon) shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_042:[ If the child has zero children, then the function toStringFunc shall be invoked for the value (as computed by MultiTree_GetValue) of that child]*/
        /*Tests_SRS_JSON_ENCODER_99_047:[If toString function returns an error then JSON_Encoder_EncodeTree shall return JSON_ENCODER_TOSTRING_FUNCTION_ERROR]*/
        /*Tests_SRS_JSON_ENCODER_99_043:[ If the child has children, then JSONEncoder_EncodeTree  shall be invoked for every of these children.]*/
        /*Tests_SRS_JSON_ENCODER_99_044:[ A "," shall be added for every child, except the last one.]*/
        /*Tests_SRS_JSON_ENCODER_99_045:[ The string "}" shall be added to the output]*/
        /*This is the baseline for tree_4*/
        /*The below test shall be copy&pasted several times (as many as filing points there are, that
        is the number of STRICT_EXPECTED_CALLS + the number of blablalballbaFunction_call_count;*/
        TEST_FUNCTION(JSONEncoder_EncodeTree_4_fails_when_MultiTree_GetValue_fails_1)
        {
            ///arrange
            EXPECTED_CALL((*mocks), STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(1);
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(4);
            EXPECTED_CALL((*mocks), STRING_new())
                .ExpectedTimesExactly(1);
            EXPECTED_CALL((*mocks), STRING_delete(IGNORED_PTR_ARG))
                .ExpectedTimesExactly(1);

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_4, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_4, 0, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetValue(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                .SetReturn(MULTITREE_ERROR)
                ;


            ///act
            auto result = JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_4, global_bufferTemp,  TestFunc_NodesAreStrings);

            ///assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_MULTITREE_ERROR, result);
            

            
            
            ASSERT_ARE_EQUAL(tchar_ptr, _T(""), mocks->CompareActualAndExpectedCalls().c_str());
        }
        /*Tests_SRS_JSON_ENCODER_99_037:[ For every child, the following actions shall take place in order to produce name:value:]*/
        /*Tests_SRS_JSON_ENCODER_99_039:[ The child name shall be retrieved by a call to MultiTree_GetChild followed by a MultiTree_GetName.]*/
        /*Tests_SRS_JSON_ENCODER_99_040:[ After retrieval, it shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_041:[ A "\":" (single quote followed by colon) shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_042:[ If the child has zero children, then the function toStringFunc shall be invoked for the value (as computed by MultiTree_GetValue) of that child]*/
        /*Tests_SRS_JSON_ENCODER_99_047:[If toString function returns an error then JSON_Encoder_EncodeTree shall return JSON_ENCODER_TOSTRING_FUNCTION_ERROR]*/
        /*Tests_SRS_JSON_ENCODER_99_043:[ If the child has children, then JSONEncoder_EncodeTree  shall be invoked for every of these children.]*/
        /*Tests_SRS_JSON_ENCODER_99_044:[ A "," shall be added for every child, except the last one.]*/
        /*Tests_SRS_JSON_ENCODER_99_045:[ The string "}" shall be added to the output]*/
        /*This is the baseline for tree_4*/
        /*The below test shall be copy&pasted several times (as many as filing points there are, that
        is the number of STRICT_EXPECTED_CALLS + the number of blablalballbaFunction_call_count;*/
        TEST_FUNCTION(JSONEncoder_EncodeTree_4_fails_when_toStringFunc_fails_1)
        {
            ///arrange
            EXPECTED_CALL((*mocks), STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(1);
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(5);
            EXPECTED_CALL((*mocks), STRING_new())
                .ExpectedTimesExactly(2);
            EXPECTED_CALL((*mocks), STRING_delete(IGNORED_PTR_ARG))
                .ExpectedTimesExactly(2);

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_4, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_4, 0, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetValue(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), TestFunc_NodesAreStrings(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreAllArguments()
                .SetReturn(JSON_ENCODER_TOSTRING_ERROR)
                ;


            ///act
            auto result = JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_4, global_bufferTemp,  TestFunc_NodesAreStrings);

            ///assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_TOSTRING_FUNCTION_ERROR, result);
            

            
            
            ASSERT_ARE_EQUAL(tchar_ptr, _T(""), mocks->CompareActualAndExpectedCalls().c_str());
        }
        /*Tests_SRS_JSON_ENCODER_99_037:[ For every child, the following actions shall take place in order to produce name:value:]*/
        /*Tests_SRS_JSON_ENCODER_99_039:[ The child name shall be retrieved by a call to MultiTree_GetChild followed by a MultiTree_GetName.]*/
        /*Tests_SRS_JSON_ENCODER_99_040:[ After retrieval, it shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_041:[ A "\":" (single quote followed by colon) shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_042:[ If the child has zero children, then the function toStringFunc shall be invoked for the value (as computed by MultiTree_GetValue) of that child]*/
        /*Tests_SRS_JSON_ENCODER_99_047:[If toString function returns an error then JSON_Encoder_EncodeTree shall return JSON_ENCODER_TOSTRING_FUNCTION_ERROR]*/
        /*Tests_SRS_JSON_ENCODER_99_043:[ If the child has children, then JSONEncoder_EncodeTree  shall be invoked for every of these children.]*/
        /*Tests_SRS_JSON_ENCODER_99_044:[ A "," shall be added for every child, except the last one.]*/
        /*Tests_SRS_JSON_ENCODER_99_045:[ The string "}" shall be added to the output]*/
        /*This is the baseline for tree_4*/
        /*The below test shall be copy&pasted several times (as many as filing points there are, that
        is the number of STRICT_EXPECTED_CALLS + the number of blablalballbaFunction_call_count;*/
        TEST_FUNCTION(JSONEncoder_EncodeTree_4_fails_when_strcat_s_fails_3)
        {
            ///arrange
            EXPECTED_CALL((*mocks), STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(2);
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(5);
            EXPECTED_CALL((*mocks), STRING_new())
                .ExpectedTimesExactly(2);
            EXPECTED_CALL((*mocks), STRING_delete(IGNORED_PTR_ARG))
                .ExpectedTimesExactly(2);


            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_4, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_4, 0, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetValue(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), TestFunc_NodesAreStrings(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreAllArguments()
                ;

            whenShallSTRING_concat_with_STRING_fail = 2;

            ///act
            auto result = JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_4, global_bufferTemp,  TestFunc_NodesAreStrings);

            ///assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_ERROR, result);
            
            ASSERT_ARE_EQUAL(tchar_ptr, _T(""), mocks->CompareActualAndExpectedCalls().c_str());
        }
        /*Tests_SRS_JSON_ENCODER_99_037:[ For every child, the following actions shall take place in order to produce name:value:]*/
        /*Tests_SRS_JSON_ENCODER_99_039:[ The child name shall be retrieved by a call to MultiTree_GetChild followed by a MultiTree_GetName.]*/
        /*Tests_SRS_JSON_ENCODER_99_040:[ After retrieval, it shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_041:[ A "\":" (single quote followed by colon) shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_042:[ If the child has zero children, then the function toStringFunc shall be invoked for the value (as computed by MultiTree_GetValue) of that child]*/
        /*Tests_SRS_JSON_ENCODER_99_047:[If toString function returns an error then JSON_Encoder_EncodeTree shall return JSON_ENCODER_TOSTRING_FUNCTION_ERROR]*/
        /*Tests_SRS_JSON_ENCODER_99_043:[ If the child has children, then JSONEncoder_EncodeTree  shall be invoked for every of these children.]*/
        /*Tests_SRS_JSON_ENCODER_99_044:[ A "," shall be added for every child, except the last one.]*/
        /*Tests_SRS_JSON_ENCODER_99_045:[ The string "}" shall be added to the output]*/
        /*This is the baseline for tree_4*/
        /*The below test shall be copy&pasted several times (as many as filing points there are, that
        is the number of STRICT_EXPECTED_CALLS + the number of blablalballbaFunction_call_count;*/
        TEST_FUNCTION(JSONEncoder_EncodeTree_4_fails_when_strcat_s_fails_4)
        {
            ///arrange
            EXPECTED_CALL((*mocks), STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(2);
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(5);
            EXPECTED_CALL((*mocks), STRING_new())
                .ExpectedTimesExactly(2);
            EXPECTED_CALL((*mocks), STRING_delete(IGNORED_PTR_ARG))
                .ExpectedTimesExactly(2);

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_4, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_4, 0, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetValue(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), TestFunc_NodesAreStrings(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreAllArguments()
                ;

            whenShallSTRING_concat_with_STRING_fail = 2;

            ///act
            auto result = JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_4, global_bufferTemp,  TestFunc_NodesAreStrings);

            ///assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_ERROR, result);
            

            
            

            
            ASSERT_ARE_EQUAL(tchar_ptr, _T(""), mocks->CompareActualAndExpectedCalls().c_str());
        }
        /*Tests_SRS_JSON_ENCODER_99_037:[ For every child, the following actions shall take place in order to produce name:value:]*/
        /*Tests_SRS_JSON_ENCODER_99_039:[ The child name shall be retrieved by a call to MultiTree_GetChild followed by a MultiTree_GetName.]*/
        /*Tests_SRS_JSON_ENCODER_99_040:[ After retrieval, it shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_041:[ A "\":" (single quote followed by colon) shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_042:[ If the child has zero children, then the function toStringFunc shall be invoked for the value (as computed by MultiTree_GetValue) of that child]*/
        /*Tests_SRS_JSON_ENCODER_99_047:[If toString function returns an error then JSON_Encoder_EncodeTree shall return JSON_ENCODER_TOSTRING_FUNCTION_ERROR]*/
        /*Tests_SRS_JSON_ENCODER_99_043:[ If the child has children, then JSONEncoder_EncodeTree  shall be invoked for every of these children.]*/
        /*Tests_SRS_JSON_ENCODER_99_044:[ A "," shall be added for every child, except the last one.]*/
        /*Tests_SRS_JSON_ENCODER_99_045:[ The string "}" shall be added to the output]*/
        /*This is the baseline for tree_4*/
        /*The below test shall be copy&pasted several times (as many as filing points there are, that
        is the number of STRICT_EXPECTED_CALLS + the number of blablalballbaFunction_call_count;*/
        TEST_FUNCTION(JSONEncoder_EncodeTree_4_fails_when_MultiTree_GetChild_fails_2)
        {
            ///arrange
            EXPECTED_CALL((*mocks), STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(2);
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(7);
            EXPECTED_CALL((*mocks), STRING_new())
                .ExpectedTimesExactly(3);
            EXPECTED_CALL((*mocks), STRING_delete(IGNORED_PTR_ARG))
                .ExpectedTimesExactly(3);

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_4, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_4, 0, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetValue(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), TestFunc_NodesAreStrings(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreAllArguments()
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_4, 1, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                .SetReturn(MULTITREE_ERROR)
                ;


            ///act
            auto result = JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_4, global_bufferTemp,  TestFunc_NodesAreStrings);

            ///assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_MULTITREE_ERROR, result);
            

            
            

            
            ASSERT_ARE_EQUAL(tchar_ptr, _T(""), mocks->CompareActualAndExpectedCalls().c_str());
        }
        /*Tests_SRS_JSON_ENCODER_99_037:[ For every child, the following actions shall take place in order to produce name:value:]*/
        /*Tests_SRS_JSON_ENCODER_99_039:[ The child name shall be retrieved by a call to MultiTree_GetChild followed by a MultiTree_GetName.]*/
        /*Tests_SRS_JSON_ENCODER_99_040:[ After retrieval, it shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_041:[ A "\":" (single quote followed by colon) shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_042:[ If the child has zero children, then the function toStringFunc shall be invoked for the value (as computed by MultiTree_GetValue) of that child]*/
        /*Tests_SRS_JSON_ENCODER_99_047:[If toString function returns an error then JSON_Encoder_EncodeTree shall return JSON_ENCODER_TOSTRING_FUNCTION_ERROR]*/
        /*Tests_SRS_JSON_ENCODER_99_043:[ If the child has children, then JSONEncoder_EncodeTree  shall be invoked for every of these children.]*/
        /*Tests_SRS_JSON_ENCODER_99_044:[ A "," shall be added for every child, except the last one.]*/
        /*Tests_SRS_JSON_ENCODER_99_045:[ The string "}" shall be added to the output]*/
        /*This is the baseline for tree_4*/
        /*The below test shall be copy&pasted several times (as many as filing points there are, that
        is the number of STRICT_EXPECTED_CALLS + the number of blablalballbaFunction_call_count;*/
        TEST_FUNCTION(JSONEncoder_EncodeTree_4_fails_when_MultiTree_GetName_fails_2)
        {
            ///arrange
            EXPECTED_CALL((*mocks), STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(2);
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(8);
            EXPECTED_CALL((*mocks), STRING_new())
                .ExpectedTimesExactly(3);
            EXPECTED_CALL((*mocks), STRING_delete(IGNORED_PTR_ARG))
                .ExpectedTimesExactly(3);


            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_4, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_4, 0, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetValue(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), TestFunc_NodesAreStrings(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreAllArguments()
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_4, 1, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_2, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                .SetReturn(MULTITREE_ERROR)
                ;


            ///act
            auto result = JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_4, global_bufferTemp,  TestFunc_NodesAreStrings);

            ///assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_MULTITREE_ERROR, result);
            

            
            

            
            ASSERT_ARE_EQUAL(tchar_ptr, _T(""), mocks->CompareActualAndExpectedCalls().c_str());
        }
        /*Tests_SRS_JSON_ENCODER_99_037:[ For every child, the following actions shall take place in order to produce name:value:]*/
        /*Tests_SRS_JSON_ENCODER_99_039:[ The child name shall be retrieved by a call to MultiTree_GetChild followed by a MultiTree_GetName.]*/
        /*Tests_SRS_JSON_ENCODER_99_040:[ After retrieval, it shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_041:[ A "\":" (single quote followed by colon) shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_042:[ If the child has zero children, then the function toStringFunc shall be invoked for the value (as computed by MultiTree_GetValue) of that child]*/
        /*Tests_SRS_JSON_ENCODER_99_047:[If toString function returns an error then JSON_Encoder_EncodeTree shall return JSON_ENCODER_TOSTRING_FUNCTION_ERROR]*/
        /*Tests_SRS_JSON_ENCODER_99_043:[ If the child has children, then JSONEncoder_EncodeTree  shall be invoked for every of these children.]*/
        /*Tests_SRS_JSON_ENCODER_99_044:[ A "," shall be added for every child, except the last one.]*/
        /*Tests_SRS_JSON_ENCODER_99_045:[ The string "}" shall be added to the output]*/
        /*This is the baseline for tree_4*/
        /*The below test shall be copy&pasted several times (as many as filing points there are, that
        is the number of STRICT_EXPECTED_CALLS + the number of blablalballbaFunction_call_count;*/
        TEST_FUNCTION(JSONEncoder_EncodeTree_4_fails_when_strcat_s_fails_5)
        {
            ///arrange
            EXPECTED_CALL((*mocks), STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(3);
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(8);
            EXPECTED_CALL((*mocks), STRING_new())
                .ExpectedTimesExactly(3);
            EXPECTED_CALL((*mocks), STRING_delete(IGNORED_PTR_ARG))
                .ExpectedTimesExactly(3);

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_4, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_4, 0, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetValue(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), TestFunc_NodesAreStrings(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreAllArguments()
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_4, 1, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_2, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            whenShallSTRING_concat_with_STRING_fail = 3;

            ///act
            auto result = JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_4, global_bufferTemp,  TestFunc_NodesAreStrings);

            ///assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_ERROR, result);
            
            ASSERT_ARE_EQUAL(tchar_ptr, _T(""), mocks->CompareActualAndExpectedCalls().c_str());
        }
        /*Tests_SRS_JSON_ENCODER_99_037:[ For every child, the following actions shall take place in order to produce name:value:]*/
        /*Tests_SRS_JSON_ENCODER_99_039:[ The child name shall be retrieved by a call to MultiTree_GetChild followed by a MultiTree_GetName.]*/
        /*Tests_SRS_JSON_ENCODER_99_040:[ After retrieval, it shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_041:[ A "\":" (single quote followed by colon) shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_042:[ If the child has zero children, then the function toStringFunc shall be invoked for the value (as computed by MultiTree_GetValue) of that child]*/
        /*Tests_SRS_JSON_ENCODER_99_047:[If toString function returns an error then JSON_Encoder_EncodeTree shall return JSON_ENCODER_TOSTRING_FUNCTION_ERROR]*/
        /*Tests_SRS_JSON_ENCODER_99_043:[ If the child has children, then JSONEncoder_EncodeTree  shall be invoked for every of these children.]*/
        /*Tests_SRS_JSON_ENCODER_99_044:[ A "," shall be added for every child, except the last one.]*/
        /*Tests_SRS_JSON_ENCODER_99_045:[ The string "}" shall be added to the output]*/
        /*This is the baseline for tree_4*/
        /*The below test shall be copy&pasted several times (as many as filing points there are, that
        is the number of STRICT_EXPECTED_CALLS + the number of blablalballbaFunction_call_count;*/
        TEST_FUNCTION(JSONEncoder_EncodeTree_4_fails_when_MultiTree_GetChildCount_fails_3)
        {
            ///arrange
            EXPECTED_CALL((*mocks), STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(3);
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(9);
            EXPECTED_CALL((*mocks), STRING_new())
                .ExpectedTimesExactly(3);
            EXPECTED_CALL((*mocks), STRING_delete(IGNORED_PTR_ARG))
                .ExpectedTimesExactly(3);

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_4, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_4, 0, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetValue(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), TestFunc_NodesAreStrings(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreAllArguments()
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_4, 1, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_2, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_CHILD_2, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                .SetReturn(MULTITREE_ERROR)
                ;


            ///act
            auto result = JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_4, global_bufferTemp,  TestFunc_NodesAreStrings);

            ///assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_MULTITREE_ERROR, result);
            

            
            

            
            
            ASSERT_ARE_EQUAL(tchar_ptr, _T(""), mocks->CompareActualAndExpectedCalls().c_str());
        }
        /*Tests_SRS_JSON_ENCODER_99_037:[ For every child, the following actions shall take place in order to produce name:value:]*/
        /*Tests_SRS_JSON_ENCODER_99_039:[ The child name shall be retrieved by a call to MultiTree_GetChild followed by a MultiTree_GetName.]*/
        /*Tests_SRS_JSON_ENCODER_99_040:[ After retrieval, it shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_041:[ A "\":" (single quote followed by colon) shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_042:[ If the child has zero children, then the function toStringFunc shall be invoked for the value (as computed by MultiTree_GetValue) of that child]*/
        /*Tests_SRS_JSON_ENCODER_99_047:[If toString function returns an error then JSON_Encoder_EncodeTree shall return JSON_ENCODER_TOSTRING_FUNCTION_ERROR]*/
        /*Tests_SRS_JSON_ENCODER_99_043:[ If the child has children, then JSONEncoder_EncodeTree  shall be invoked for every of these children.]*/
        /*Tests_SRS_JSON_ENCODER_99_044:[ A "," shall be added for every child, except the last one.]*/
        /*Tests_SRS_JSON_ENCODER_99_045:[ The string "}" shall be added to the output]*/
        /*This is the baseline for tree_4*/
        /*The below test shall be copy&pasted several times (as many as filing points there are, that
        is the number of STRICT_EXPECTED_CALLS + the number of blablalballbaFunction_call_count;*/
        TEST_FUNCTION(JSONEncoder_EncodeTree_4_fails_when_MultiTree_GetValue_fails_2)
        {
            ///arrange
            EXPECTED_CALL((*mocks), STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(3);
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(9);
            EXPECTED_CALL((*mocks), STRING_new())
                .ExpectedTimesExactly(3);
            EXPECTED_CALL((*mocks), STRING_delete(IGNORED_PTR_ARG))
                .ExpectedTimesExactly(3);


            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_4, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_4, 0, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetValue(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), TestFunc_NodesAreStrings(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreAllArguments()
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_4, 1, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_2, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_CHILD_2, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetValue(TEST_MULTITREE_HANDLE_CHILD_2, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                .SetReturn(MULTITREE_ERROR)
                ;


            ///act
            auto result = JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_4, global_bufferTemp,  TestFunc_NodesAreStrings);

            ///assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_MULTITREE_ERROR, result);
           
            ASSERT_ARE_EQUAL(tchar_ptr, _T(""), mocks->CompareActualAndExpectedCalls().c_str());
        }
        /*Tests_SRS_JSON_ENCODER_99_037:[ For every child, the following actions shall take place in order to produce name:value:]*/
        /*Tests_SRS_JSON_ENCODER_99_039:[ The child name shall be retrieved by a call to MultiTree_GetChild followed by a MultiTree_GetName.]*/
        /*Tests_SRS_JSON_ENCODER_99_040:[ After retrieval, it shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_041:[ A "\":" (single quote followed by colon) shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_042:[ If the child has zero children, then the function toStringFunc shall be invoked for the value (as computed by MultiTree_GetValue) of that child]*/
        /*Tests_SRS_JSON_ENCODER_99_047:[If toString function returns an error then JSON_Encoder_EncodeTree shall return JSON_ENCODER_TOSTRING_FUNCTION_ERROR]*/
        /*Tests_SRS_JSON_ENCODER_99_043:[ If the child has children, then JSONEncoder_EncodeTree  shall be invoked for every of these children.]*/
        /*Tests_SRS_JSON_ENCODER_99_044:[ A "," shall be added for every child, except the last one.]*/
        /*Tests_SRS_JSON_ENCODER_99_045:[ The string "}" shall be added to the output]*/
        /*This is the baseline for tree_4*/
        /*The below test shall be copy&pasted several times (as many as filing points there are, that
        is the number of STRICT_EXPECTED_CALLS + the number of blablalballbaFunction_call_count;*/
        TEST_FUNCTION(JSONEncoder_EncodeTree_4_fails_when_toStringFunc_fails_2)
        {
            ///arrange
            EXPECTED_CALL((*mocks), STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(3);
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(10);
            EXPECTED_CALL((*mocks), STRING_new())
                .ExpectedTimesExactly(4);
            EXPECTED_CALL((*mocks), STRING_delete(IGNORED_PTR_ARG))
                .ExpectedTimesExactly(4);

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_4, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_4, 0, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetValue(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), TestFunc_NodesAreStrings(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreAllArguments()
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_4, 1, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_2, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_CHILD_2, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetValue(TEST_MULTITREE_HANDLE_CHILD_2, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), TestFunc_NodesAreStrings(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreAllArguments()
                .SetReturn(JSON_ENCODER_TOSTRING_ERROR)
                ;


            ///act
            auto result = JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_4, global_bufferTemp,  TestFunc_NodesAreStrings);

            ///assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_TOSTRING_FUNCTION_ERROR, result);
            

            
            

            
            
            ASSERT_ARE_EQUAL(tchar_ptr, _T(""), mocks->CompareActualAndExpectedCalls().c_str());
        }
        /*Tests_SRS_JSON_ENCODER_99_037:[ For every child, the following actions shall take place in order to produce name:value:]*/
        /*Tests_SRS_JSON_ENCODER_99_039:[ The child name shall be retrieved by a call to MultiTree_GetChild followed by a MultiTree_GetName.]*/
        /*Tests_SRS_JSON_ENCODER_99_040:[ After retrieval, it shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_041:[ A "\":" (single quote followed by colon) shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_042:[ If the child has zero children, then the function toStringFunc shall be invoked for the value (as computed by MultiTree_GetValue) of that child]*/
        /*Tests_SRS_JSON_ENCODER_99_047:[If toString function returns an error then JSON_Encoder_EncodeTree shall return JSON_ENCODER_TOSTRING_FUNCTION_ERROR]*/
        /*Tests_SRS_JSON_ENCODER_99_043:[ If the child has children, then JSONEncoder_EncodeTree  shall be invoked for every of these children.]*/
        /*Tests_SRS_JSON_ENCODER_99_044:[ A "," shall be added for every child, except the last one.]*/
        /*Tests_SRS_JSON_ENCODER_99_045:[ The string "}" shall be added to the output]*/
        /*This is the baseline for tree_4*/
        /*The below test shall be copy&pasted several times (as many as filing points there are, that
        is the number of STRICT_EXPECTED_CALLS + the number of blablalballbaFunction_call_count;*/
        TEST_FUNCTION(JSONEncoder_EncodeTree_4_fails_when_strcat_s_fails_6)
        {
            ///arrange
            EXPECTED_CALL((*mocks), STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(4);
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(10);
            EXPECTED_CALL((*mocks), STRING_new())
                .ExpectedTimesExactly(4);
            EXPECTED_CALL((*mocks), STRING_delete(IGNORED_PTR_ARG))
                .ExpectedTimesExactly(4);

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_4, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_4, 0, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetValue(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), TestFunc_NodesAreStrings(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreAllArguments()
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_4, 1, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_2, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_CHILD_2, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetValue(TEST_MULTITREE_HANDLE_CHILD_2, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), TestFunc_NodesAreStrings(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreAllArguments()
                ;

            whenShallSTRING_concat_with_STRING_fail = 4;

            ///act
            auto result = JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_4, global_bufferTemp,  TestFunc_NodesAreStrings);

            ///assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_ERROR, result);
            
            ASSERT_ARE_EQUAL(tchar_ptr, _T(""), mocks->CompareActualAndExpectedCalls().c_str());
        }
        /*Tests_SRS_JSON_ENCODER_99_037:[ For every child, the following actions shall take place in order to produce name:value:]*/
        /*Tests_SRS_JSON_ENCODER_99_039:[ The child name shall be retrieved by a call to MultiTree_GetChild followed by a MultiTree_GetName.]*/
        /*Tests_SRS_JSON_ENCODER_99_040:[ After retrieval, it shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_041:[ A "\":" (single quote followed by colon) shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_042:[ If the child has zero children, then the function toStringFunc shall be invoked for the value (as computed by MultiTree_GetValue) of that child]*/
        /*Tests_SRS_JSON_ENCODER_99_047:[If toString function returns an error then JSON_Encoder_EncodeTree shall return JSON_ENCODER_TOSTRING_FUNCTION_ERROR]*/
        /*Tests_SRS_JSON_ENCODER_99_043:[ If the child has children, then JSONEncoder_EncodeTree  shall be invoked for every of these children.]*/
        /*Tests_SRS_JSON_ENCODER_99_044:[ A "," shall be added for every child, except the last one.]*/
        /*Tests_SRS_JSON_ENCODER_99_045:[ The string "}" shall be added to the output]*/
        /*This is the baseline for tree_4*/
        /*The below test shall be copy&pasted several times (as many as filing points there are, that
        is the number of STRICT_EXPECTED_CALLS + the number of blablalballbaFunction_call_count;*/
        TEST_FUNCTION(JSONEncoder_EncodeTree_4_fails_when_strcat_s_fails_7)
        {
            ///arrange
            EXPECTED_CALL((*mocks), STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(4);
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(10);
            EXPECTED_CALL((*mocks), STRING_new())
                .ExpectedTimesExactly(4);
            EXPECTED_CALL((*mocks), STRING_delete(IGNORED_PTR_ARG))
                .ExpectedTimesExactly(4);


            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_4, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_4, 0, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetValue(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), TestFunc_NodesAreStrings(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreAllArguments()
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_4, 1, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_2, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_CHILD_2, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetValue(TEST_MULTITREE_HANDLE_CHILD_2, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), TestFunc_NodesAreStrings(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreAllArguments()
                ;

            whenShallSTRING_concat_with_STRING_fail = 4;

            ///act
            auto result = JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_4, global_bufferTemp,  TestFunc_NodesAreStrings);

            ///assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_ERROR, result);
            

            
            

            
            

            
            ASSERT_ARE_EQUAL(tchar_ptr, _T(""), mocks->CompareActualAndExpectedCalls().c_str());
        }
        /*Tests_SRS_JSON_ENCODER_99_037:[ For every child, the following actions shall take place in order to produce name:value:]*/
        /*Tests_SRS_JSON_ENCODER_99_039:[ The child name shall be retrieved by a call to MultiTree_GetChild followed by a MultiTree_GetName.]*/
        /*Tests_SRS_JSON_ENCODER_99_040:[ After retrieval, it shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_041:[ A "\":" (single quote followed by colon) shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_042:[ If the child has zero children, then the function toStringFunc shall be invoked for the value (as computed by MultiTree_GetValue) of that child]*/
        /*Tests_SRS_JSON_ENCODER_99_047:[If toString function returns an error then JSON_Encoder_EncodeTree shall return JSON_ENCODER_TOSTRING_FUNCTION_ERROR]*/
        /*Tests_SRS_JSON_ENCODER_99_043:[ If the child has children, then JSONEncoder_EncodeTree  shall be invoked for every of these children.]*/
        /*Tests_SRS_JSON_ENCODER_99_044:[ A "," shall be added for every child, except the last one.]*/
        /*Tests_SRS_JSON_ENCODER_99_045:[ The string "}" shall be added to the output]*/
        /*This is the baseline for tree_4*/
        /*The below test shall be copy&pasted several times (as many as filing points there are, that
        is the number of STRICT_EXPECTED_CALLS + the number of blablalballbaFunction_call_count;*/
        TEST_FUNCTION(JSONEncoder_EncodeTree_4_fails_when_MultiTree_GetChild_fails_3)
        {
            ///arrange
            EXPECTED_CALL((*mocks), STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(4);
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(12);
            EXPECTED_CALL((*mocks), STRING_new())
                .ExpectedTimesExactly(5);
            EXPECTED_CALL((*mocks), STRING_delete(IGNORED_PTR_ARG))
                .ExpectedTimesExactly(5);

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_4, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_4, 0, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetValue(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), TestFunc_NodesAreStrings(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreAllArguments()
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_4, 1, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_2, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_CHILD_2, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetValue(TEST_MULTITREE_HANDLE_CHILD_2, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), TestFunc_NodesAreStrings(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreAllArguments()
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_4, 2, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                .SetReturn(MULTITREE_ERROR)
                ;


            ///act
            auto result = JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_4, global_bufferTemp,  TestFunc_NodesAreStrings);

            ///assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_MULTITREE_ERROR, result);
            

            
            

            
            

            
            ASSERT_ARE_EQUAL(tchar_ptr, _T(""), mocks->CompareActualAndExpectedCalls().c_str());
        }
        /*Tests_SRS_JSON_ENCODER_99_037:[ For every child, the following actions shall take place in order to produce name:value:]*/
        /*Tests_SRS_JSON_ENCODER_99_039:[ The child name shall be retrieved by a call to MultiTree_GetChild followed by a MultiTree_GetName.]*/
        /*Tests_SRS_JSON_ENCODER_99_040:[ After retrieval, it shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_041:[ A "\":" (single quote followed by colon) shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_042:[ If the child has zero children, then the function toStringFunc shall be invoked for the value (as computed by MultiTree_GetValue) of that child]*/
        /*Tests_SRS_JSON_ENCODER_99_047:[If toString function returns an error then JSON_Encoder_EncodeTree shall return JSON_ENCODER_TOSTRING_FUNCTION_ERROR]*/
        /*Tests_SRS_JSON_ENCODER_99_043:[ If the child has children, then JSONEncoder_EncodeTree  shall be invoked for every of these children.]*/
        /*Tests_SRS_JSON_ENCODER_99_044:[ A "," shall be added for every child, except the last one.]*/
        /*Tests_SRS_JSON_ENCODER_99_045:[ The string "}" shall be added to the output]*/
        /*This is the baseline for tree_4*/
        /*The below test shall be copy&pasted several times (as many as filing points there are, that
        is the number of STRICT_EXPECTED_CALLS + the number of blablalballbaFunction_call_count;*/
        TEST_FUNCTION(JSONEncoder_EncodeTree_4_fails_when_MultiTree_GetName_fails_3)
        {
            ///arrange
            EXPECTED_CALL((*mocks), STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(4);
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(13);
            EXPECTED_CALL((*mocks), STRING_new())
                .ExpectedTimesExactly(5);
            EXPECTED_CALL((*mocks), STRING_delete(IGNORED_PTR_ARG))
                .ExpectedTimesExactly(5);


            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_4, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_4, 0, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetValue(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), TestFunc_NodesAreStrings(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreAllArguments()
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_4, 1, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_2, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_CHILD_2, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetValue(TEST_MULTITREE_HANDLE_CHILD_2, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), TestFunc_NodesAreStrings(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreAllArguments()
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_4, 2, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_3, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                .SetReturn(MULTITREE_ERROR)
                ;


            ///act
            auto result = JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_4, global_bufferTemp,  TestFunc_NodesAreStrings);

            ///assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_MULTITREE_ERROR, result);
            

            
            

            
            

            
            ASSERT_ARE_EQUAL(tchar_ptr, _T(""), mocks->CompareActualAndExpectedCalls().c_str());
        }
        /*Tests_SRS_JSON_ENCODER_99_037:[ For every child, the following actions shall take place in order to produce name:value:]*/
        /*Tests_SRS_JSON_ENCODER_99_039:[ The child name shall be retrieved by a call to MultiTree_GetChild followed by a MultiTree_GetName.]*/
        /*Tests_SRS_JSON_ENCODER_99_040:[ After retrieval, it shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_041:[ A "\":" (single quote followed by colon) shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_042:[ If the child has zero children, then the function toStringFunc shall be invoked for the value (as computed by MultiTree_GetValue) of that child]*/
        /*Tests_SRS_JSON_ENCODER_99_047:[If toString function returns an error then JSON_Encoder_EncodeTree shall return JSON_ENCODER_TOSTRING_FUNCTION_ERROR]*/
        /*Tests_SRS_JSON_ENCODER_99_043:[ If the child has children, then JSONEncoder_EncodeTree  shall be invoked for every of these children.]*/
        /*Tests_SRS_JSON_ENCODER_99_044:[ A "," shall be added for every child, except the last one.]*/
        /*Tests_SRS_JSON_ENCODER_99_045:[ The string "}" shall be added to the output]*/
        /*This is the baseline for tree_4*/
        /*The below test shall be copy&pasted several times (as many as filing points there are, that
        is the number of STRICT_EXPECTED_CALLS + the number of blablalballbaFunction_call_count;*/
        TEST_FUNCTION(JSONEncoder_EncodeTree_4_fails_when_strcat_s_fails_8)
        {
            ///arrange
            EXPECTED_CALL((*mocks), STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(5);
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(13);
            EXPECTED_CALL((*mocks), STRING_new())
                .ExpectedTimesExactly(5);
            EXPECTED_CALL((*mocks), STRING_delete(IGNORED_PTR_ARG))
                .ExpectedTimesExactly(5);

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_4, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_4, 0, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetValue(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), TestFunc_NodesAreStrings(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreAllArguments()
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_4, 1, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_2, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_CHILD_2, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetValue(TEST_MULTITREE_HANDLE_CHILD_2, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), TestFunc_NodesAreStrings(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreAllArguments()
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_4, 2, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_3, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            whenShallSTRING_concat_with_STRING_fail = 5;

            ///act
            auto result = JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_4, global_bufferTemp,  TestFunc_NodesAreStrings);

            ///assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_ERROR, result);
            
            
            

            
            

            
            
            ASSERT_ARE_EQUAL(tchar_ptr, _T(""), mocks->CompareActualAndExpectedCalls().c_str());
        }
        /*Tests_SRS_JSON_ENCODER_99_037:[ For every child, the following actions shall take place in order to produce name:value:]*/
        /*Tests_SRS_JSON_ENCODER_99_039:[ The child name shall be retrieved by a call to MultiTree_GetChild followed by a MultiTree_GetName.]*/
        /*Tests_SRS_JSON_ENCODER_99_040:[ After retrieval, it shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_041:[ A "\":" (single quote followed by colon) shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_042:[ If the child has zero children, then the function toStringFunc shall be invoked for the value (as computed by MultiTree_GetValue) of that child]*/
        /*Tests_SRS_JSON_ENCODER_99_047:[If toString function returns an error then JSON_Encoder_EncodeTree shall return JSON_ENCODER_TOSTRING_FUNCTION_ERROR]*/
        /*Tests_SRS_JSON_ENCODER_99_043:[ If the child has children, then JSONEncoder_EncodeTree  shall be invoked for every of these children.]*/
        /*Tests_SRS_JSON_ENCODER_99_044:[ A "," shall be added for every child, except the last one.]*/
        /*Tests_SRS_JSON_ENCODER_99_045:[ The string "}" shall be added to the output]*/
        /*This is the baseline for tree_4*/
        /*The below test shall be copy&pasted several times (as many as filing points there are, that
        is the number of STRICT_EXPECTED_CALLS + the number of blablalballbaFunction_call_count;*/
        TEST_FUNCTION(JSONEncoder_EncodeTree_4_fails_when_MultiTree_GetChildCount_fails_4)
        {
            ///arrange
            EXPECTED_CALL((*mocks), STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(5);
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(14);
            EXPECTED_CALL((*mocks), STRING_new())
                .ExpectedTimesExactly(5);
            EXPECTED_CALL((*mocks), STRING_delete(IGNORED_PTR_ARG))
                .ExpectedTimesExactly(5);


            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_4, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_4, 0, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetValue(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), TestFunc_NodesAreStrings(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreAllArguments()
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_4, 1, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_2, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_CHILD_2, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetValue(TEST_MULTITREE_HANDLE_CHILD_2, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), TestFunc_NodesAreStrings(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreAllArguments()
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_4, 2, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_3, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_CHILD_3, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                .SetReturn(MULTITREE_ERROR)
                ;


            ///act
            auto result = JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_4, global_bufferTemp,  TestFunc_NodesAreStrings);

            ///assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_MULTITREE_ERROR, result);
            

            
            

            
            

            
            
            ASSERT_ARE_EQUAL(tchar_ptr, _T(""), mocks->CompareActualAndExpectedCalls().c_str());
        }
        /*Tests_SRS_JSON_ENCODER_99_037:[ For every child, the following actions shall take place in order to produce name:value:]*/
        /*Tests_SRS_JSON_ENCODER_99_039:[ The child name shall be retrieved by a call to MultiTree_GetChild followed by a MultiTree_GetName.]*/
        /*Tests_SRS_JSON_ENCODER_99_040:[ After retrieval, it shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_041:[ A "\":" (single quote followed by colon) shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_042:[ If the child has zero children, then the function toStringFunc shall be invoked for the value (as computed by MultiTree_GetValue) of that child]*/
        /*Tests_SRS_JSON_ENCODER_99_047:[If toString function returns an error then JSON_Encoder_EncodeTree shall return JSON_ENCODER_TOSTRING_FUNCTION_ERROR]*/
        /*Tests_SRS_JSON_ENCODER_99_043:[ If the child has children, then JSONEncoder_EncodeTree  shall be invoked for every of these children.]*/
        /*Tests_SRS_JSON_ENCODER_99_044:[ A "," shall be added for every child, except the last one.]*/
        /*Tests_SRS_JSON_ENCODER_99_045:[ The string "}" shall be added to the output]*/
        /*This is the baseline for tree_4*/
        /*The below test shall be copy&pasted several times (as many as filing points there are, that
        is the number of STRICT_EXPECTED_CALLS + the number of blablalballbaFunction_call_count;*/
        TEST_FUNCTION(JSONEncoder_EncodeTree_4_fails_when_MultiTree_GetValue_fails_3)
        {
            ///arrange
            EXPECTED_CALL((*mocks), STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(5);
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(14);
            EXPECTED_CALL((*mocks), STRING_new())
                .ExpectedTimesExactly(5);
            EXPECTED_CALL((*mocks), STRING_delete(IGNORED_PTR_ARG))
                .ExpectedTimesExactly(5);


            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_4, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_4, 0, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetValue(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), TestFunc_NodesAreStrings(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreAllArguments()
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_4, 1, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_2, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_CHILD_2, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetValue(TEST_MULTITREE_HANDLE_CHILD_2, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), TestFunc_NodesAreStrings(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreAllArguments()
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_4, 2, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_3, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_CHILD_3, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetValue(TEST_MULTITREE_HANDLE_CHILD_3, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                .SetReturn(MULTITREE_ERROR)
                ;


            ///act
            auto result = JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_4, global_bufferTemp,  TestFunc_NodesAreStrings);

            ///assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_MULTITREE_ERROR, result);
            

            
            

            
            

            
            
            ASSERT_ARE_EQUAL(tchar_ptr, _T(""), mocks->CompareActualAndExpectedCalls().c_str());
        }
        /*Tests_SRS_JSON_ENCODER_99_037:[ For every child, the following actions shall take place in order to produce name:value:]*/
        /*Tests_SRS_JSON_ENCODER_99_039:[ The child name shall be retrieved by a call to MultiTree_GetChild followed by a MultiTree_GetName.]*/
        /*Tests_SRS_JSON_ENCODER_99_040:[ After retrieval, it shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_041:[ A "\":" (single quote followed by colon) shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_042:[ If the child has zero children, then the function toStringFunc shall be invoked for the value (as computed by MultiTree_GetValue) of that child]*/
        /*Tests_SRS_JSON_ENCODER_99_047:[If toString function returns an error then JSON_Encoder_EncodeTree shall return JSON_ENCODER_TOSTRING_FUNCTION_ERROR]*/
        /*Tests_SRS_JSON_ENCODER_99_043:[ If the child has children, then JSONEncoder_EncodeTree  shall be invoked for every of these children.]*/
        /*Tests_SRS_JSON_ENCODER_99_044:[ A "," shall be added for every child, except the last one.]*/
        /*Tests_SRS_JSON_ENCODER_99_045:[ The string "}" shall be added to the output]*/
        /*This is the baseline for tree_4*/
        /*The below test shall be copy&pasted several times (as many as filing points there are, that
        is the number of STRICT_EXPECTED_CALLS + the number of blablalballbaFunction_call_count;*/
        TEST_FUNCTION(JSONEncoder_EncodeTree_4_fails_when_toStringFunc_fails_3)
        {
            ///arrange
            EXPECTED_CALL((*mocks), STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(5);
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(15);
            EXPECTED_CALL((*mocks), STRING_new())
                .ExpectedTimesExactly(6);
            EXPECTED_CALL((*mocks), STRING_delete(IGNORED_PTR_ARG))
                .ExpectedTimesExactly(6);

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_4, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_4, 0, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetValue(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), TestFunc_NodesAreStrings(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreAllArguments()
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_4, 1, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_2, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_CHILD_2, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetValue(TEST_MULTITREE_HANDLE_CHILD_2, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), TestFunc_NodesAreStrings(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreAllArguments()
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_4, 2, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_3, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_CHILD_3, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetValue(TEST_MULTITREE_HANDLE_CHILD_3, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), TestFunc_NodesAreStrings(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreAllArguments()
                .SetReturn(JSON_ENCODER_TOSTRING_ERROR)
                ;


            ///act
            auto result = JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_4, global_bufferTemp,  TestFunc_NodesAreStrings);

            ///assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_TOSTRING_FUNCTION_ERROR, result);
            
            ASSERT_ARE_EQUAL(tchar_ptr, _T(""), mocks->CompareActualAndExpectedCalls().c_str());
        }
        /*Tests_SRS_JSON_ENCODER_99_037:[ For every child, the following actions shall take place in order to produce name:value:]*/
        /*Tests_SRS_JSON_ENCODER_99_039:[ The child name shall be retrieved by a call to MultiTree_GetChild followed by a MultiTree_GetName.]*/
        /*Tests_SRS_JSON_ENCODER_99_040:[ After retrieval, it shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_041:[ A "\":" (single quote followed by colon) shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_042:[ If the child has zero children, then the function toStringFunc shall be invoked for the value (as computed by MultiTree_GetValue) of that child]*/
        /*Tests_SRS_JSON_ENCODER_99_047:[If toString function returns an error then JSON_Encoder_EncodeTree shall return JSON_ENCODER_TOSTRING_FUNCTION_ERROR]*/
        /*Tests_SRS_JSON_ENCODER_99_043:[ If the child has children, then JSONEncoder_EncodeTree  shall be invoked for every of these children.]*/
        /*Tests_SRS_JSON_ENCODER_99_044:[ A "," shall be added for every child, except the last one.]*/
        /*Tests_SRS_JSON_ENCODER_99_045:[ The string "}" shall be added to the output]*/
        /*This is the baseline for tree_4*/
        /*The below test shall be copy&pasted several times (as many as filing points there are, that
        is the number of STRICT_EXPECTED_CALLS + the number of blablalballbaFunction_call_count;*/
        TEST_FUNCTION(JSONEncoder_EncodeTree_4_fails_when_strcat_s_fails_9)
        {
            ///arrange
            EXPECTED_CALL((*mocks), STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(6);
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(15);
            EXPECTED_CALL((*mocks), STRING_new())
                .ExpectedTimesExactly(6);
            EXPECTED_CALL((*mocks), STRING_delete(IGNORED_PTR_ARG))
                .ExpectedTimesExactly(6);

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_4, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_4, 0, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetValue(TEST_MULTITREE_HANDLE_CHILD_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), TestFunc_NodesAreStrings(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreAllArguments()
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_4, 1, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_2, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_CHILD_2, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetValue(TEST_MULTITREE_HANDLE_CHILD_2, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), TestFunc_NodesAreStrings(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreAllArguments()
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_4, 2, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_3, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_CHILD_3, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetValue(TEST_MULTITREE_HANDLE_CHILD_3, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), TestFunc_NodesAreStrings(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreAllArguments()
                ;

            whenShallSTRING_concat_with_STRING_fail = 6;

            ///act
            auto result = JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_4, global_bufferTemp,  TestFunc_NodesAreStrings);

            ///assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_ERROR, result);
            

            
            

            
            

            
            

            ASSERT_ARE_EQUAL(tchar_ptr, _T(""), mocks->CompareActualAndExpectedCalls().c_str());
        }
        /*Tests_SRS_JSON_ENCODER_99_037:[ For every child, the following actions shall take place in order to produce name:value:]*/
        /*Tests_SRS_JSON_ENCODER_99_039:[ The child name shall be retrieved by a call to MultiTree_GetChild followed by a MultiTree_GetName.]*/
        /*Tests_SRS_JSON_ENCODER_99_040:[ After retrieval, it shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_041:[ A "\":" (single quote followed by colon) shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_042:[ If the child has zero children, then the function toStringFunc shall be invoked for the value (as computed by MultiTree_GetValue) of that child]*/
        /*Tests_SRS_JSON_ENCODER_99_047:[If toString function returns an error then JSON_Encoder_EncodeTree shall return JSON_ENCODER_TOSTRING_FUNCTION_ERROR]*/
        /*Tests_SRS_JSON_ENCODER_99_043:[ If the child has children, then JSONEncoder_EncodeTree  shall be invoked for every of these children.]*/
        /*Tests_SRS_JSON_ENCODER_99_044:[ A "," shall be added for every child, except the last one.]*/
        /*Tests_SRS_JSON_ENCODER_99_045:[ The string "}" shall be added to the output]*/
        /*This is the baseline for tree_5_1*/
        /*The below test shall be copy&pasted several times (as many as failing points there are, that
        is the number of STRICT_EXPECTED_CALLS + the number of blablalballbaFunction_call_count;*/
        TEST_FUNCTION(JSONEncoder_EncodeTree_5_1_success)
        {
            ///arrange
            EXPECTED_CALL((*mocks), STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(6);
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(16);
            EXPECTED_CALL((*mocks), STRING_new())
                .ExpectedTimesExactly(6);
            EXPECTED_CALL((*mocks), STRING_delete(IGNORED_PTR_ARG))
                .ExpectedTimesExactly(6);


            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_5_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;
            
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_5_1, 0, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_5, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_5, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            /*below are because of recursive call of Encode Tree*/
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_5, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            /*child 4*/
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_5, 0, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_4, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_CHILD_4, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetValue(TEST_MULTITREE_HANDLE_CHILD_4, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), TestFunc_NodesAreStrings(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreAllArguments()
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_5, 1, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;

            /*child 5*/
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_5, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_CHILD_5, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetValue(TEST_MULTITREE_HANDLE_CHILD_5, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), TestFunc_NodesAreStrings(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreAllArguments()
                ;


            ///act
            auto result = JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_5_1, global_bufferTemp,  TestFunc_NodesAreStrings);

            ///assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_OK, result);
            ASSERT_ARE_EQUAL(char_ptr, "{\"subtree\":{\"child4\":\"value4\", \"child5\":\"value5\"}}", BASEIMPLEMENTATION::STRING_c_str(global_bufferTemp));
            
            

            
            
            
            

            
            


            ASSERT_ARE_EQUAL(tchar_ptr, _T(""), mocks->CompareActualAndExpectedCalls().c_str());
        }
        /*Tests_SRS_JSON_ENCODER_99_037:[ For every child, the following actions shall take place in order to produce name:value:]*/
        /*Tests_SRS_JSON_ENCODER_99_039:[ The child name shall be retrieved by a call to MultiTree_GetChild followed by a MultiTree_GetName.]*/
        /*Tests_SRS_JSON_ENCODER_99_040:[ After retrieval, it shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_041:[ A "\":" (single quote followed by colon) shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_042:[ If the child has zero children, then the function toStringFunc shall be invoked for the value (as computed by MultiTree_GetValue) of that child]*/
        /*Tests_SRS_JSON_ENCODER_99_047:[If toString function returns an error then JSON_Encoder_EncodeTree shall return JSON_ENCODER_TOSTRING_FUNCTION_ERROR]*/
        /*Tests_SRS_JSON_ENCODER_99_043:[ If the child has children, then JSONEncoder_EncodeTree  shall be invoked for every of these children.]*/
        /*Tests_SRS_JSON_ENCODER_99_044:[ A "," shall be added for every child, except the last one.]*/
        /*Tests_SRS_JSON_ENCODER_99_045:[ The string "}" shall be added to the output]*/
        /*This is the baseline for tree_5_1*/
        /*The below test shall be copy&pasted several times (as many as failing points there are, that
        is the number of STRICT_EXPECTED_CALLS + the number of blablalballbaFunction_call_count;*/
        TEST_FUNCTION(JSONEncoder_EncodeTree_5_1_fails_when_MultiTree_GetChildCount_fails_1)
        {
            ///arrange

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_5_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                .SetReturn(MULTITREE_ERROR)
                ;


            ///act
            auto result = JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_5_1, global_bufferTemp,  TestFunc_NodesAreStrings);

            ///assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_MULTITREE_ERROR, result);
            ASSERT_ARE_EQUAL(tchar_ptr, _T(""), mocks->CompareActualAndExpectedCalls().c_str());
        }
        /*Tests_SRS_JSON_ENCODER_99_037:[ For every child, the following actions shall take place in order to produce name:value:]*/
        /*Tests_SRS_JSON_ENCODER_99_039:[ The child name shall be retrieved by a call to MultiTree_GetChild followed by a MultiTree_GetName.]*/
        /*Tests_SRS_JSON_ENCODER_99_040:[ After retrieval, it shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_041:[ A "\":" (single quote followed by colon) shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_042:[ If the child has zero children, then the function toStringFunc shall be invoked for the value (as computed by MultiTree_GetValue) of that child]*/
        /*Tests_SRS_JSON_ENCODER_99_047:[If toString function returns an error then JSON_Encoder_EncodeTree shall return JSON_ENCODER_TOSTRING_FUNCTION_ERROR]*/
        /*Tests_SRS_JSON_ENCODER_99_043:[ If the child has children, then JSONEncoder_EncodeTree  shall be invoked for every of these children.]*/
        /*Tests_SRS_JSON_ENCODER_99_044:[ A "," shall be added for every child, except the last one.]*/
        /*Tests_SRS_JSON_ENCODER_99_045:[ The string "}" shall be added to the output]*/
        /*This is the baseline for tree_5_1*/
        /*The below test shall be copy&pasted several times (as many as failing points there are, that
        is the number of STRICT_EXPECTED_CALLS + the number of blablalballbaFunction_call_count;*/
        TEST_FUNCTION(JSONEncoder_EncodeTree_5_1_fails_when_strcpy_s_fails_1)
        {
            ///arrange
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(1);
            EXPECTED_CALL((*mocks), STRING_c_str(IGNORED_PTR_ARG))
                .ExpectedTimesExactly(0);
            EXPECTED_CALL((*mocks), STRING_new())
                .ExpectedTimesExactly(0);
            EXPECTED_CALL((*mocks), STRING_delete(IGNORED_PTR_ARG))
                .ExpectedTimesExactly(0);

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_5_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            whenShallSTRING_concat_fail = 1;

            ///act
            auto result = JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_5_1, global_bufferTemp,  TestFunc_NodesAreStrings);

            ///assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_ERROR, result);
            
            ASSERT_ARE_EQUAL(tchar_ptr, _T(""), mocks->CompareActualAndExpectedCalls().c_str());
        }
        /*Tests_SRS_JSON_ENCODER_99_037:[ For every child, the following actions shall take place in order to produce name:value:]*/
        /*Tests_SRS_JSON_ENCODER_99_039:[ The child name shall be retrieved by a call to MultiTree_GetChild followed by a MultiTree_GetName.]*/
        /*Tests_SRS_JSON_ENCODER_99_040:[ After retrieval, it shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_041:[ A "\":" (single quote followed by colon) shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_042:[ If the child has zero children, then the function toStringFunc shall be invoked for the value (as computed by MultiTree_GetValue) of that child]*/
        /*Tests_SRS_JSON_ENCODER_99_047:[If toString function returns an error then JSON_Encoder_EncodeTree shall return JSON_ENCODER_TOSTRING_FUNCTION_ERROR]*/
        /*Tests_SRS_JSON_ENCODER_99_043:[ If the child has children, then JSONEncoder_EncodeTree  shall be invoked for every of these children.]*/
        /*Tests_SRS_JSON_ENCODER_99_044:[ A "," shall be added for every child, except the last one.]*/
        /*Tests_SRS_JSON_ENCODER_99_045:[ The string "}" shall be added to the output]*/
        /*This is the baseline for tree_5_1*/
        /*The below test shall be copy&pasted several times (as many as failing points there are, that
        is the number of STRICT_EXPECTED_CALLS + the number of blablalballbaFunction_call_count;*/
        TEST_FUNCTION(JSONEncoder_EncodeTree_5_1_fails_when_strcat_s_fail_1)
        {
            ///arrange

            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(1);
            EXPECTED_CALL((*mocks), STRING_c_str(IGNORED_PTR_ARG))
                .ExpectedTimesExactly(0);
            EXPECTED_CALL((*mocks), STRING_new())
                .ExpectedTimesExactly(0);
            EXPECTED_CALL((*mocks), STRING_delete(IGNORED_PTR_ARG))
                .ExpectedTimesExactly(0);


            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_5_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;


            whenShallSTRING_concat_fail = 1;

            ///act
            auto result = JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_5_1, global_bufferTemp,  TestFunc_NodesAreStrings);

            ///assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_ERROR, result);
            
            
            ASSERT_ARE_EQUAL(tchar_ptr, _T(""), mocks->CompareActualAndExpectedCalls().c_str());
        }
        /*Tests_SRS_JSON_ENCODER_99_037:[ For every child, the following actions shall take place in order to produce name:value:]*/
        /*Tests_SRS_JSON_ENCODER_99_039:[ The child name shall be retrieved by a call to MultiTree_GetChild followed by a MultiTree_GetName.]*/
        /*Tests_SRS_JSON_ENCODER_99_040:[ After retrieval, it shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_041:[ A "\":" (single quote followed by colon) shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_042:[ If the child has zero children, then the function toStringFunc shall be invoked for the value (as computed by MultiTree_GetValue) of that child]*/
        /*Tests_SRS_JSON_ENCODER_99_047:[If toString function returns an error then JSON_Encoder_EncodeTree shall return JSON_ENCODER_TOSTRING_FUNCTION_ERROR]*/
        /*Tests_SRS_JSON_ENCODER_99_043:[ If the child has children, then JSONEncoder_EncodeTree  shall be invoked for every of these children.]*/
        /*Tests_SRS_JSON_ENCODER_99_044:[ A "," shall be added for every child, except the last one.]*/
        /*Tests_SRS_JSON_ENCODER_99_045:[ The string "}" shall be added to the output]*/
        /*This is the baseline for tree_5_1*/
        /*The below test shall be copy&pasted several times (as many as failing points there are, that
        is the number of STRICT_EXPECTED_CALLS + the number of blablalballbaFunction_call_count;*/
        TEST_FUNCTION(JSONEncoder_EncodeTree_5_1_fails_when_MultiTree_GetChild_fails_1)
        {
            ///arrange

            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(2);
            EXPECTED_CALL((*mocks), STRING_c_str(IGNORED_PTR_ARG))
                .ExpectedTimesExactly(0);
            EXPECTED_CALL((*mocks), STRING_new())
                .ExpectedTimesExactly(1);
            EXPECTED_CALL((*mocks), STRING_delete(IGNORED_PTR_ARG))
                .ExpectedTimesExactly(1);

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_5_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_5_1, 0, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                .SetReturn(MULTITREE_ERROR)
                ;


            ///act
            auto result = JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_5_1, global_bufferTemp,  TestFunc_NodesAreStrings);

            ///assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_MULTITREE_ERROR, result);
            

            
            ASSERT_ARE_EQUAL(tchar_ptr, _T(""), mocks->CompareActualAndExpectedCalls().c_str());
        }
        /*Tests_SRS_JSON_ENCODER_99_037:[ For every child, the following actions shall take place in order to produce name:value:]*/
        /*Tests_SRS_JSON_ENCODER_99_039:[ The child name shall be retrieved by a call to MultiTree_GetChild followed by a MultiTree_GetName.]*/
        /*Tests_SRS_JSON_ENCODER_99_040:[ After retrieval, it shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_041:[ A "\":" (single quote followed by colon) shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_042:[ If the child has zero children, then the function toStringFunc shall be invoked for the value (as computed by MultiTree_GetValue) of that child]*/
        /*Tests_SRS_JSON_ENCODER_99_047:[If toString function returns an error then JSON_Encoder_EncodeTree shall return JSON_ENCODER_TOSTRING_FUNCTION_ERROR]*/
        /*Tests_SRS_JSON_ENCODER_99_043:[ If the child has children, then JSONEncoder_EncodeTree  shall be invoked for every of these children.]*/
        /*Tests_SRS_JSON_ENCODER_99_044:[ A "," shall be added for every child, except the last one.]*/
        /*Tests_SRS_JSON_ENCODER_99_045:[ The string "}" shall be added to the output]*/
        /*This is the baseline for tree_5_1*/
        /*The below test shall be copy&pasted several times (as many as failing points there are, that
        is the number of STRICT_EXPECTED_CALLS + the number of blablalballbaFunction_call_count;*/
        TEST_FUNCTION(JSONEncoder_EncodeTree_5_1_fails_when_MultiTree_GetName_fails_1)
        {
            ///arrange

            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(3);
            EXPECTED_CALL((*mocks), STRING_c_str(IGNORED_PTR_ARG))
                .ExpectedTimesExactly(0);
            EXPECTED_CALL((*mocks), STRING_new())
                .ExpectedTimesExactly(1);
            EXPECTED_CALL((*mocks), STRING_delete(IGNORED_PTR_ARG))
                .ExpectedTimesExactly(1);

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_5_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_5_1, 0, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_5, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                .SetReturn(MULTITREE_ERROR)
                ;

            ///act
            auto result = JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_5_1, global_bufferTemp,  TestFunc_NodesAreStrings);

            ///assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_MULTITREE_ERROR, result);
            

            
            ASSERT_ARE_EQUAL(tchar_ptr, _T(""), mocks->CompareActualAndExpectedCalls().c_str());
        }
        /*Tests_SRS_JSON_ENCODER_99_037:[ For every child, the following actions shall take place in order to produce name:value:]*/
        /*Tests_SRS_JSON_ENCODER_99_039:[ The child name shall be retrieved by a call to MultiTree_GetChild followed by a MultiTree_GetName.]*/
        /*Tests_SRS_JSON_ENCODER_99_040:[ After retrieval, it shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_041:[ A "\":" (single quote followed by colon) shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_042:[ If the child has zero children, then the function toStringFunc shall be invoked for the value (as computed by MultiTree_GetValue) of that child]*/
        /*Tests_SRS_JSON_ENCODER_99_047:[If toString function returns an error then JSON_Encoder_EncodeTree shall return JSON_ENCODER_TOSTRING_FUNCTION_ERROR]*/
        /*Tests_SRS_JSON_ENCODER_99_043:[ If the child has children, then JSONEncoder_EncodeTree  shall be invoked for every of these children.]*/
        /*Tests_SRS_JSON_ENCODER_99_044:[ A "," shall be added for every child, except the last one.]*/
        /*Tests_SRS_JSON_ENCODER_99_045:[ The string "}" shall be added to the output]*/
        /*This is the baseline for tree_5_1*/
        /*The below test shall be copy&pasted several times (as many as failing points there are, that
        is the number of STRICT_EXPECTED_CALLS + the number of blablalballbaFunction_call_count;*/
        TEST_FUNCTION(JSONEncoder_EncodeTree_5_1_fails_when_strcat_s_fails_2)
        {
            ///arrange
            EXPECTED_CALL((*mocks), STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(1);
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(3);
            EXPECTED_CALL((*mocks), STRING_new())
                .ExpectedTimesExactly(1);
            EXPECTED_CALL((*mocks), STRING_delete(IGNORED_PTR_ARG))
                .ExpectedTimesExactly(1);

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_5_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_5_1, 0, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_5, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

           
            whenShallSTRING_concat_with_STRING_fail = 1;

            ///act
            auto result = JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_5_1, global_bufferTemp,  TestFunc_NodesAreStrings);

            ///assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_ERROR, result);
            

            
            
            ASSERT_ARE_EQUAL(tchar_ptr, _T(""), mocks->CompareActualAndExpectedCalls().c_str());
        }
        /*Tests_SRS_JSON_ENCODER_99_037:[ For every child, the following actions shall take place in order to produce name:value:]*/
        /*Tests_SRS_JSON_ENCODER_99_039:[ The child name shall be retrieved by a call to MultiTree_GetChild followed by a MultiTree_GetName.]*/
        /*Tests_SRS_JSON_ENCODER_99_040:[ After retrieval, it shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_041:[ A "\":" (single quote followed by colon) shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_042:[ If the child has zero children, then the function toStringFunc shall be invoked for the value (as computed by MultiTree_GetValue) of that child]*/
        /*Tests_SRS_JSON_ENCODER_99_047:[If toString function returns an error then JSON_Encoder_EncodeTree shall return JSON_ENCODER_TOSTRING_FUNCTION_ERROR]*/
        /*Tests_SRS_JSON_ENCODER_99_043:[ If the child has children, then JSONEncoder_EncodeTree  shall be invoked for every of these children.]*/
        /*Tests_SRS_JSON_ENCODER_99_044:[ A "," shall be added for every child, except the last one.]*/
        /*Tests_SRS_JSON_ENCODER_99_045:[ The string "}" shall be added to the output]*/
        /*This is the baseline for tree_5_1*/
        /*The below test shall be copy&pasted several times (as many as failing points there are, that
        is the number of STRICT_EXPECTED_CALLS + the number of blablalballbaFunction_call_count;*/
        TEST_FUNCTION(JSONEncoder_EncodeTree_5_1_fails_when_MultiTree_GetChildCount_fails_2)
        {
            ///arrange
            EXPECTED_CALL((*mocks), STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(1);
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(4);
            EXPECTED_CALL((*mocks), STRING_new())
                .ExpectedTimesExactly(1);
            EXPECTED_CALL((*mocks), STRING_delete(IGNORED_PTR_ARG))
                .ExpectedTimesExactly(1);

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_5_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_5_1, 0, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_5, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_5, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                .SetReturn(MULTITREE_ERROR)
                ;


            ///act
            auto result = JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_5_1, global_bufferTemp,  TestFunc_NodesAreStrings);

            ///assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_MULTITREE_ERROR, result);
            
            
            
            ASSERT_ARE_EQUAL(tchar_ptr, _T(""), mocks->CompareActualAndExpectedCalls().c_str());
        }
        /*Tests_SRS_JSON_ENCODER_99_037:[ For every child, the following actions shall take place in order to produce name:value:]*/
        /*Tests_SRS_JSON_ENCODER_99_039:[ The child name shall be retrieved by a call to MultiTree_GetChild followed by a MultiTree_GetName.]*/
        /*Tests_SRS_JSON_ENCODER_99_040:[ After retrieval, it shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_041:[ A "\":" (single quote followed by colon) shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_042:[ If the child has zero children, then the function toStringFunc shall be invoked for the value (as computed by MultiTree_GetValue) of that child]*/
        /*Tests_SRS_JSON_ENCODER_99_047:[If toString function returns an error then JSON_Encoder_EncodeTree shall return JSON_ENCODER_TOSTRING_FUNCTION_ERROR]*/
        /*Tests_SRS_JSON_ENCODER_99_043:[ If the child has children, then JSONEncoder_EncodeTree  shall be invoked for every of these children.]*/
        /*Tests_SRS_JSON_ENCODER_99_044:[ A "," shall be added for every child, except the last one.]*/
        /*Tests_SRS_JSON_ENCODER_99_045:[ The string "}" shall be added to the output]*/
        /*This is the baseline for tree_5_1*/
        /*The below test shall be copy&pasted several times (as many as failing points there are, that
        is the number of STRICT_EXPECTED_CALLS + the number of blablalballbaFunction_call_count;*/
        TEST_FUNCTION(JSONEncoder_EncodeTree_5_1_fails_when_MultiTree_GetChildCount_fails_3) /*this is first recursive call*/
        {
            ///arrange
            EXPECTED_CALL((*mocks), STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(1);
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(4);
            EXPECTED_CALL((*mocks), STRING_new())
                .ExpectedTimesExactly(2);
            EXPECTED_CALL((*mocks), STRING_delete(IGNORED_PTR_ARG))
                .ExpectedTimesExactly(2);

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_5_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_5_1, 0, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_5, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_5, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            /*below are because of recursive call of Encode Tree*/
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_5, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                .SetReturn(MULTITREE_ERROR)
                ;


            ///act
            auto result = JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_5_1, global_bufferTemp,  TestFunc_NodesAreStrings);

            ///assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_MULTITREE_ERROR, result);
            

            
            
            ASSERT_ARE_EQUAL(tchar_ptr, _T(""), mocks->CompareActualAndExpectedCalls().c_str());
        }
        /*Tests_SRS_JSON_ENCODER_99_037:[ For every child, the following actions shall take place in order to produce name:value:]*/
        /*Tests_SRS_JSON_ENCODER_99_039:[ The child name shall be retrieved by a call to MultiTree_GetChild followed by a MultiTree_GetName.]*/
        /*Tests_SRS_JSON_ENCODER_99_040:[ After retrieval, it shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_041:[ A "\":" (single quote followed by colon) shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_042:[ If the child has zero children, then the function toStringFunc shall be invoked for the value (as computed by MultiTree_GetValue) of that child]*/
        /*Tests_SRS_JSON_ENCODER_99_047:[If toString function returns an error then JSON_Encoder_EncodeTree shall return JSON_ENCODER_TOSTRING_FUNCTION_ERROR]*/
        /*Tests_SRS_JSON_ENCODER_99_043:[ If the child has children, then JSONEncoder_EncodeTree  shall be invoked for every of these children.]*/
        /*Tests_SRS_JSON_ENCODER_99_044:[ A "," shall be added for every child, except the last one.]*/
        /*Tests_SRS_JSON_ENCODER_99_045:[ The string "}" shall be added to the output]*/
        /*This is the baseline for tree_5_1*/
        /*The below test shall be copy&pasted several times (as many as failing points there are, that
        is the number of STRICT_EXPECTED_CALLS + the number of blablalballbaFunction_call_count;*/
        TEST_FUNCTION(JSONEncoder_EncodeTree_5_1_fails_when_strcpy_s_fails_2)
        {
            ///arrange
            EXPECTED_CALL((*mocks), STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(1);
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(6);
            EXPECTED_CALL((*mocks), STRING_new())
                .ExpectedTimesExactly(2);
            EXPECTED_CALL((*mocks), STRING_delete(IGNORED_PTR_ARG))
                .ExpectedTimesExactly(2);

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_5_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_5_1, 0, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_5, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_5, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_5, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            whenShallSTRING_concat_fail = 5;

            ///act
            auto result = JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_5_1, global_bufferTemp,  TestFunc_NodesAreStrings);

            ///assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_ERROR, result);

            ASSERT_ARE_EQUAL(tchar_ptr, _T(""), mocks->CompareActualAndExpectedCalls().c_str());
        }
        /*Tests_SRS_JSON_ENCODER_99_037:[ For every child, the following actions shall take place in order to produce name:value:]*/
        /*Tests_SRS_JSON_ENCODER_99_039:[ The child name shall be retrieved by a call to MultiTree_GetChild followed by a MultiTree_GetName.]*/
        /*Tests_SRS_JSON_ENCODER_99_040:[ After retrieval, it shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_041:[ A "\":" (single quote followed by colon) shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_042:[ If the child has zero children, then the function toStringFunc shall be invoked for the value (as computed by MultiTree_GetValue) of that child]*/
        /*Tests_SRS_JSON_ENCODER_99_047:[If toString function returns an error then JSON_Encoder_EncodeTree shall return JSON_ENCODER_TOSTRING_FUNCTION_ERROR]*/
        /*Tests_SRS_JSON_ENCODER_99_043:[ If the child has children, then JSONEncoder_EncodeTree  shall be invoked for every of these children.]*/
        /*Tests_SRS_JSON_ENCODER_99_044:[ A "," shall be added for every child, except the last one.]*/
        /*Tests_SRS_JSON_ENCODER_99_045:[ The string "}" shall be added to the output]*/
        /*This is the baseline for tree_5_1*/
        /*The below test shall be copy&pasted several times (as many as failing points there are, that
        is the number of STRICT_EXPECTED_CALLS + the number of blablalballbaFunction_call_count;*/
        TEST_FUNCTION(JSONEncoder_EncodeTree_5_1_fails_when_strcat_s_fails_3)
        {
            ///arrange
            EXPECTED_CALL((*mocks), STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(1);
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(6);
            EXPECTED_CALL((*mocks), STRING_new())
                .ExpectedTimesExactly(2);
            EXPECTED_CALL((*mocks), STRING_delete(IGNORED_PTR_ARG))
                .ExpectedTimesExactly(2);

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_5_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_5_1, 0, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_5, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_5, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            /*below are because of recursive call of Encode Tree*/
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_5, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            //whenShallSTRING_concat_with_STRING_fail = 1;
            whenShallSTRING_concat_fail = 5;

            ///act
            auto result = JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_5_1, global_bufferTemp,  TestFunc_NodesAreStrings);

            ///assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_ERROR, result);

           ASSERT_ARE_EQUAL(tchar_ptr, _T(""), mocks->CompareActualAndExpectedCalls().c_str());
        }
        /*Tests_SRS_JSON_ENCODER_99_037:[ For every child, the following actions shall take place in order to produce name:value:]*/
        /*Tests_SRS_JSON_ENCODER_99_039:[ The child name shall be retrieved by a call to MultiTree_GetChild followed by a MultiTree_GetName.]*/
        /*Tests_SRS_JSON_ENCODER_99_040:[ After retrieval, it shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_041:[ A "\":" (single quote followed by colon) shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_042:[ If the child has zero children, then the function toStringFunc shall be invoked for the value (as computed by MultiTree_GetValue) of that child]*/
        /*Tests_SRS_JSON_ENCODER_99_047:[If toString function returns an error then JSON_Encoder_EncodeTree shall return JSON_ENCODER_TOSTRING_FUNCTION_ERROR]*/
        /*Tests_SRS_JSON_ENCODER_99_043:[ If the child has children, then JSONEncoder_EncodeTree  shall be invoked for every of these children.]*/
        /*Tests_SRS_JSON_ENCODER_99_044:[ A "," shall be added for every child, except the last one.]*/
        /*Tests_SRS_JSON_ENCODER_99_045:[ The string "}" shall be added to the output]*/
        /*This is the baseline for tree_5_1*/
        /*The below test shall be copy&pasted several times (as many as failing points there are, that
        is the number of STRICT_EXPECTED_CALLS + the number of blablalballbaFunction_call_count;*/
        TEST_FUNCTION(JSONEncoder_EncodeTree_5_1_fais_when_MultiTree_GetChild_fails_2)
        {
            ///arrange
            EXPECTED_CALL((*mocks), STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(1);
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(6);
            EXPECTED_CALL((*mocks), STRING_new())
                .ExpectedTimesExactly(3);
            EXPECTED_CALL((*mocks), STRING_delete(IGNORED_PTR_ARG))
                .ExpectedTimesExactly(3);

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_5_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_5_1, 0, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_5, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_5, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            /*below are because of recursive call of Encode Tree*/
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_5, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            /*child 4*/
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_5, 0, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                .SetReturn(MULTITREE_ERROR)
                ;


            ///act
            auto result = JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_5_1, global_bufferTemp,  TestFunc_NodesAreStrings);

            ///assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_MULTITREE_ERROR, result);
            
            
            
            
            
            ASSERT_ARE_EQUAL(tchar_ptr, _T(""), mocks->CompareActualAndExpectedCalls().c_str());
        }
        /*Tests_SRS_JSON_ENCODER_99_037:[ For every child, the following actions shall take place in order to produce name:value:]*/
        /*Tests_SRS_JSON_ENCODER_99_039:[ The child name shall be retrieved by a call to MultiTree_GetChild followed by a MultiTree_GetName.]*/
        /*Tests_SRS_JSON_ENCODER_99_040:[ After retrieval, it shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_041:[ A "\":" (single quote followed by colon) shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_042:[ If the child has zero children, then the function toStringFunc shall be invoked for the value (as computed by MultiTree_GetValue) of that child]*/
        /*Tests_SRS_JSON_ENCODER_99_047:[If toString function returns an error then JSON_Encoder_EncodeTree shall return JSON_ENCODER_TOSTRING_FUNCTION_ERROR]*/
        /*Tests_SRS_JSON_ENCODER_99_043:[ If the child has children, then JSONEncoder_EncodeTree  shall be invoked for every of these children.]*/
        /*Tests_SRS_JSON_ENCODER_99_044:[ A "," shall be added for every child, except the last one.]*/
        /*Tests_SRS_JSON_ENCODER_99_045:[ The string "}" shall be added to the output]*/
        /*This is the baseline for tree_5_1*/
        /*The below test shall be copy&pasted several times (as many as failing points there are, that
        is the number of STRICT_EXPECTED_CALLS + the number of blablalballbaFunction_call_count;*/
        TEST_FUNCTION(JSONEncoder_EncodeTree_5_1_fails_when_MultiTree_GetName_fails_2)
        {
            ///arrange
            EXPECTED_CALL((*mocks), STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(1);
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(7);
            EXPECTED_CALL((*mocks), STRING_new())
                .ExpectedTimesExactly(3);
            EXPECTED_CALL((*mocks), STRING_delete(IGNORED_PTR_ARG))
                .ExpectedTimesExactly(3);

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_5_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_5_1, 0, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_5, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_5, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            /*below are because of recursive call of Encode Tree*/
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_5, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            /*child 4*/
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_5, 0, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_4, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                .SetReturn(MULTITREE_ERROR)
                ;



            ///act
            auto result = JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_5_1, global_bufferTemp,  TestFunc_NodesAreStrings);

            ///assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_MULTITREE_ERROR, result);
            
            
            
            
            

            ASSERT_ARE_EQUAL(tchar_ptr, _T(""), mocks->CompareActualAndExpectedCalls().c_str());
        }
        /*Tests_SRS_JSON_ENCODER_99_037:[ For every child, the following actions shall take place in order to produce name:value:]*/
        /*Tests_SRS_JSON_ENCODER_99_039:[ The child name shall be retrieved by a call to MultiTree_GetChild followed by a MultiTree_GetName.]*/
        /*Tests_SRS_JSON_ENCODER_99_040:[ After retrieval, it shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_041:[ A "\":" (single quote followed by colon) shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_042:[ If the child has zero children, then the function toStringFunc shall be invoked for the value (as computed by MultiTree_GetValue) of that child]*/
        /*Tests_SRS_JSON_ENCODER_99_047:[If toString function returns an error then JSON_Encoder_EncodeTree shall return JSON_ENCODER_TOSTRING_FUNCTION_ERROR]*/
        /*Tests_SRS_JSON_ENCODER_99_043:[ If the child has children, then JSONEncoder_EncodeTree  shall be invoked for every of these children.]*/
        /*Tests_SRS_JSON_ENCODER_99_044:[ A "," shall be added for every child, except the last one.]*/
        /*Tests_SRS_JSON_ENCODER_99_045:[ The string "}" shall be added to the output]*/
        /*This is the baseline for tree_5_1*/
        /*The below test shall be copy&pasted several times (as many as failing points there are, that
        is the number of STRICT_EXPECTED_CALLS + the number of blablalballbaFunction_call_count;*/
        TEST_FUNCTION(JSONEncoder_EncodeTree_5_1_fails_when_strcat_s_fails_4)
        {
            ///arrange
            EXPECTED_CALL((*mocks), STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(2);
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(7);
            EXPECTED_CALL((*mocks), STRING_new())
                .ExpectedTimesExactly(3);
            EXPECTED_CALL((*mocks), STRING_delete(IGNORED_PTR_ARG))
                .ExpectedTimesExactly(3);

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_5_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_5_1, 0, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_5, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_5, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            /*below are because of recursive call of Encode Tree*/
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_5, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            /*child 4*/
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_5, 0, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_4, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            whenShallSTRING_concat_with_STRING_fail = 2;

            ///act
            auto result = JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_5_1, global_bufferTemp,  TestFunc_NodesAreStrings);

            ///assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_ERROR, result);
            
            
            
            
            
            
            ASSERT_ARE_EQUAL(tchar_ptr, _T(""), mocks->CompareActualAndExpectedCalls().c_str());
        }
        /*Tests_SRS_JSON_ENCODER_99_037:[ For every child, the following actions shall take place in order to produce name:value:]*/
        /*Tests_SRS_JSON_ENCODER_99_039:[ The child name shall be retrieved by a call to MultiTree_GetChild followed by a MultiTree_GetName.]*/
        /*Tests_SRS_JSON_ENCODER_99_040:[ After retrieval, it shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_041:[ A "\":" (single quote followed by colon) shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_042:[ If the child has zero children, then the function toStringFunc shall be invoked for the value (as computed by MultiTree_GetValue) of that child]*/
        /*Tests_SRS_JSON_ENCODER_99_047:[If toString function returns an error then JSON_Encoder_EncodeTree shall return JSON_ENCODER_TOSTRING_FUNCTION_ERROR]*/
        /*Tests_SRS_JSON_ENCODER_99_043:[ If the child has children, then JSONEncoder_EncodeTree  shall be invoked for every of these children.]*/
        /*Tests_SRS_JSON_ENCODER_99_044:[ A "," shall be added for every child, except the last one.]*/
        /*Tests_SRS_JSON_ENCODER_99_045:[ The string "}" shall be added to the output]*/
        /*This is the baseline for tree_5_1*/
        /*The below test shall be copy&pasted several times (as many as failing points there are, that
        is the number of STRICT_EXPECTED_CALLS + the number of blablalballbaFunction_call_count;*/
        TEST_FUNCTION(JSONEncoder_EncodeTree_5_1_fails_when_MultiTree_GetChildCount_fails)
        {
            ///arrange
            EXPECTED_CALL((*mocks), STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(2);
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(8);
            EXPECTED_CALL((*mocks), STRING_new())
                .ExpectedTimesExactly(3);
            EXPECTED_CALL((*mocks), STRING_delete(IGNORED_PTR_ARG))
                .ExpectedTimesExactly(3);

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_5_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_5_1, 0, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_5, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_5, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            /*below are because of recursive call of Encode Tree*/
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_5, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            /*child 4*/
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_5, 0, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_4, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_CHILD_4, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                .SetReturn(MULTITREE_ERROR)
                ;


            ///act
            auto result = JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_5_1, global_bufferTemp,  TestFunc_NodesAreStrings);

            ///assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_MULTITREE_ERROR, result);
            
            
            
            
            
            
            ASSERT_ARE_EQUAL(tchar_ptr, _T(""), mocks->CompareActualAndExpectedCalls().c_str());
        }
        /*Tests_SRS_JSON_ENCODER_99_037:[ For every child, the following actions shall take place in order to produce name:value:]*/
        /*Tests_SRS_JSON_ENCODER_99_039:[ The child name shall be retrieved by a call to MultiTree_GetChild followed by a MultiTree_GetName.]*/
        /*Tests_SRS_JSON_ENCODER_99_040:[ After retrieval, it shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_041:[ A "\":" (single quote followed by colon) shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_042:[ If the child has zero children, then the function toStringFunc shall be invoked for the value (as computed by MultiTree_GetValue) of that child]*/
        /*Tests_SRS_JSON_ENCODER_99_047:[If toString function returns an error then JSON_Encoder_EncodeTree shall return JSON_ENCODER_TOSTRING_FUNCTION_ERROR]*/
        /*Tests_SRS_JSON_ENCODER_99_043:[ If the child has children, then JSONEncoder_EncodeTree  shall be invoked for every of these children.]*/
        /*Tests_SRS_JSON_ENCODER_99_044:[ A "," shall be added for every child, except the last one.]*/
        /*Tests_SRS_JSON_ENCODER_99_045:[ The string "}" shall be added to the output]*/
        /*This is the baseline for tree_5_1*/
        /*The below test shall be copy&pasted several times (as many as failing points there are, that
        is the number of STRICT_EXPECTED_CALLS + the number of blablalballbaFunction_call_count;*/
        TEST_FUNCTION(JSONEncoder_EncodeTree_5_1_fails_when_MultiTree_GetValue_fails_1)
        {
            ///arrange
            EXPECTED_CALL((*mocks), STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(2);
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(8);
            EXPECTED_CALL((*mocks), STRING_new())
                .ExpectedTimesExactly(3);
            EXPECTED_CALL((*mocks), STRING_delete(IGNORED_PTR_ARG))
                .ExpectedTimesExactly(3);

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_5_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_5_1, 0, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_5, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_5, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            /*below are because of recursive call of Encode Tree*/
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_5, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            /*child 4*/
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_5, 0, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_4, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_CHILD_4, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetValue(TEST_MULTITREE_HANDLE_CHILD_4, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                .SetReturn(MULTITREE_ERROR)
                ;


            ///act
            auto result = JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_5_1, global_bufferTemp,  TestFunc_NodesAreStrings);

            ///assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_MULTITREE_ERROR, result);
            
            
            
            
            
            
            ASSERT_ARE_EQUAL(tchar_ptr, _T(""), mocks->CompareActualAndExpectedCalls().c_str());
        }
        /*Tests_SRS_JSON_ENCODER_99_037:[ For every child, the following actions shall take place in order to produce name:value:]*/
        /*Tests_SRS_JSON_ENCODER_99_039:[ The child name shall be retrieved by a call to MultiTree_GetChild followed by a MultiTree_GetName.]*/
        /*Tests_SRS_JSON_ENCODER_99_040:[ After retrieval, it shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_041:[ A "\":" (single quote followed by colon) shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_042:[ If the child has zero children, then the function toStringFunc shall be invoked for the value (as computed by MultiTree_GetValue) of that child]*/
        /*Tests_SRS_JSON_ENCODER_99_047:[If toString function returns an error then JSON_Encoder_EncodeTree shall return JSON_ENCODER_TOSTRING_FUNCTION_ERROR]*/
        /*Tests_SRS_JSON_ENCODER_99_043:[ If the child has children, then JSONEncoder_EncodeTree  shall be invoked for every of these children.]*/
        /*Tests_SRS_JSON_ENCODER_99_044:[ A "," shall be added for every child, except the last one.]*/
        /*Tests_SRS_JSON_ENCODER_99_045:[ The string "}" shall be added to the output]*/
        /*This is the baseline for tree_5_1*/
        /*The below test shall be copy&pasted several times (as many as failing points there are, that
        is the number of STRICT_EXPECTED_CALLS + the number of blablalballbaFunction_call_count;*/
        TEST_FUNCTION(JSONEncoder_EncodeTree_5_1_fails_when_toStringFunc_fails_1)
        {
            ///arrange
            EXPECTED_CALL((*mocks), STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(2);
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(9);
            EXPECTED_CALL((*mocks), STRING_new())
                .ExpectedTimesExactly(4);
            EXPECTED_CALL((*mocks), STRING_delete(IGNORED_PTR_ARG))
                .ExpectedTimesExactly(4);

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_5_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_5_1, 0, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_5, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_5, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            /*below are because of recursive call of Encode Tree*/
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_5, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            /*child 4*/
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_5, 0, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_4, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_CHILD_4, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetValue(TEST_MULTITREE_HANDLE_CHILD_4, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), TestFunc_NodesAreStrings(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreAllArguments()
                .SetReturn(JSON_ENCODER_TOSTRING_ERROR)
                ;

            ///act
            auto result = JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_5_1, global_bufferTemp,  TestFunc_NodesAreStrings);

            ///assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_TOSTRING_FUNCTION_ERROR, result);

            ASSERT_ARE_EQUAL(tchar_ptr, _T(""), mocks->CompareActualAndExpectedCalls().c_str());
        }
        /*Tests_SRS_JSON_ENCODER_99_037:[ For every child, the following actions shall take place in order to produce name:value:]*/
        /*Tests_SRS_JSON_ENCODER_99_039:[ The child name shall be retrieved by a call to MultiTree_GetChild followed by a MultiTree_GetName.]*/
        /*Tests_SRS_JSON_ENCODER_99_040:[ After retrieval, it shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_041:[ A "\":" (single quote followed by colon) shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_042:[ If the child has zero children, then the function toStringFunc shall be invoked for the value (as computed by MultiTree_GetValue) of that child]*/
        /*Tests_SRS_JSON_ENCODER_99_047:[If toString function returns an error then JSON_Encoder_EncodeTree shall return JSON_ENCODER_TOSTRING_FUNCTION_ERROR]*/
        /*Tests_SRS_JSON_ENCODER_99_043:[ If the child has children, then JSONEncoder_EncodeTree  shall be invoked for every of these children.]*/
        /*Tests_SRS_JSON_ENCODER_99_044:[ A "," shall be added for every child, except the last one.]*/
        /*Tests_SRS_JSON_ENCODER_99_045:[ The string "}" shall be added to the output]*/
        /*This is the baseline for tree_5_1*/
        /*The below test shall be copy&pasted several times (as many as failing points there are, that
        is the number of STRICT_EXPECTED_CALLS + the number of blablalballbaFunction_call_count;*/
        TEST_FUNCTION(JSONEncoder_EncodeTree_5_1_fails_when_strcat_s_fails_5)
        {
            ///arrange
            EXPECTED_CALL((*mocks), STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(3);
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(9);
            EXPECTED_CALL((*mocks), STRING_new())
                .ExpectedTimesExactly(4);
            EXPECTED_CALL((*mocks), STRING_delete(IGNORED_PTR_ARG))
                .ExpectedTimesExactly(4);

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_5_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_5_1, 0, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_5, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_5, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            /*below are because of recursive call of Encode Tree*/
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_5, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            /*child 4*/
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_5, 0, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_4, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_CHILD_4, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetValue(TEST_MULTITREE_HANDLE_CHILD_4, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), TestFunc_NodesAreStrings(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreAllArguments()
                ;

            whenShallSTRING_concat_with_STRING_fail = 3;

            ///act
            auto result = JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_5_1, global_bufferTemp,  TestFunc_NodesAreStrings);

            ///assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_ERROR, result);
            
            
            
            
            
            

            ASSERT_ARE_EQUAL(tchar_ptr, _T(""), mocks->CompareActualAndExpectedCalls().c_str());
        }
        /*Tests_SRS_JSON_ENCODER_99_037:[ For every child, the following actions shall take place in order to produce name:value:]*/
        /*Tests_SRS_JSON_ENCODER_99_039:[ The child name shall be retrieved by a call to MultiTree_GetChild followed by a MultiTree_GetName.]*/
        /*Tests_SRS_JSON_ENCODER_99_040:[ After retrieval, it shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_041:[ A "\":" (single quote followed by colon) shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_042:[ If the child has zero children, then the function toStringFunc shall be invoked for the value (as computed by MultiTree_GetValue) of that child]*/
        /*Tests_SRS_JSON_ENCODER_99_047:[If toString function returns an error then JSON_Encoder_EncodeTree shall return JSON_ENCODER_TOSTRING_FUNCTION_ERROR]*/
        /*Tests_SRS_JSON_ENCODER_99_043:[ If the child has children, then JSONEncoder_EncodeTree  shall be invoked for every of these children.]*/
        /*Tests_SRS_JSON_ENCODER_99_044:[ A "," shall be added for every child, except the last one.]*/
        /*Tests_SRS_JSON_ENCODER_99_045:[ The string "}" shall be added to the output]*/
        /*This is the baseline for tree_5_1*/
        /*The below test shall be copy&pasted several times (as many as failing points there are, that
        is the number of STRICT_EXPECTED_CALLS + the number of blablalballbaFunction_call_count;*/
        TEST_FUNCTION(JSONEncoder_EncodeTree_5_1_fails_when_strcat_s_fails_6)
        {
            ///arrange
            EXPECTED_CALL((*mocks), STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(3);
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(9);
            EXPECTED_CALL((*mocks), STRING_new())
                .ExpectedTimesExactly(4);
            EXPECTED_CALL((*mocks), STRING_delete(IGNORED_PTR_ARG))
                .ExpectedTimesExactly(4);

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_5_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_5_1, 0, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_5, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_5, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            /*below are because of recursive call of Encode Tree*/
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_5, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            /*child 4*/
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_5, 0, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_4, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_CHILD_4, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetValue(TEST_MULTITREE_HANDLE_CHILD_4, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), TestFunc_NodesAreStrings(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreAllArguments()
                ;

            whenShallSTRING_concat_with_STRING_fail = 3;

            ///act
            auto result = JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_5_1, global_bufferTemp,  TestFunc_NodesAreStrings);

            ///assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_ERROR, result);
                        
            ASSERT_ARE_EQUAL(tchar_ptr, _T(""), mocks->CompareActualAndExpectedCalls().c_str());
        }
        /*Tests_SRS_JSON_ENCODER_99_037:[ For every child, the following actions shall take place in order to produce name:value:]*/
        /*Tests_SRS_JSON_ENCODER_99_039:[ The child name shall be retrieved by a call to MultiTree_GetChild followed by a MultiTree_GetName.]*/
        /*Tests_SRS_JSON_ENCODER_99_040:[ After retrieval, it shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_041:[ A "\":" (single quote followed by colon) shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_042:[ If the child has zero children, then the function toStringFunc shall be invoked for the value (as computed by MultiTree_GetValue) of that child]*/
        /*Tests_SRS_JSON_ENCODER_99_047:[If toString function returns an error then JSON_Encoder_EncodeTree shall return JSON_ENCODER_TOSTRING_FUNCTION_ERROR]*/
        /*Tests_SRS_JSON_ENCODER_99_043:[ If the child has children, then JSONEncoder_EncodeTree  shall be invoked for every of these children.]*/
        /*Tests_SRS_JSON_ENCODER_99_044:[ A "," shall be added for every child, except the last one.]*/
        /*Tests_SRS_JSON_ENCODER_99_045:[ The string "}" shall be added to the output]*/
        /*This is the baseline for tree_5_1*/
        /*The below test shall be copy&pasted several times (as many as failing points there are, that
        is the number of STRICT_EXPECTED_CALLS + the number of blablalballbaFunction_call_count;*/
        TEST_FUNCTION(JSONEncoder_EncodeTree_5_1_fails_when_MultiTree_GetChild_fails_3)
        {
            ///arrange
            EXPECTED_CALL((*mocks), STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(3);
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(11);
            EXPECTED_CALL((*mocks), STRING_new())
                .ExpectedTimesExactly(5);
            EXPECTED_CALL((*mocks), STRING_delete(IGNORED_PTR_ARG))
                .ExpectedTimesExactly(5);

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_5_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_5_1, 0, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_5, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_5, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            /*below are because of recursive call of Encode Tree*/
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_5, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            /*child 4*/
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_5, 0, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_4, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_CHILD_4, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetValue(TEST_MULTITREE_HANDLE_CHILD_4, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), TestFunc_NodesAreStrings(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreAllArguments()
                ;

            /*child 5*/

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_5, 1, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                .SetReturn(MULTITREE_ERROR)
                ;


            ///act
            auto result = JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_5_1, global_bufferTemp,  TestFunc_NodesAreStrings);

            ///assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_MULTITREE_ERROR, result);
            
            
            
            
            

            
            ASSERT_ARE_EQUAL(tchar_ptr, _T(""), mocks->CompareActualAndExpectedCalls().c_str());
        }
        /*Tests_SRS_JSON_ENCODER_99_037:[ For every child, the following actions shall take place in order to produce name:value:]*/
        /*Tests_SRS_JSON_ENCODER_99_039:[ The child name shall be retrieved by a call to MultiTree_GetChild followed by a MultiTree_GetName.]*/
        /*Tests_SRS_JSON_ENCODER_99_040:[ After retrieval, it shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_041:[ A "\":" (single quote followed by colon) shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_042:[ If the child has zero children, then the function toStringFunc shall be invoked for the value (as computed by MultiTree_GetValue) of that child]*/
        /*Tests_SRS_JSON_ENCODER_99_047:[If toString function returns an error then JSON_Encoder_EncodeTree shall return JSON_ENCODER_TOSTRING_FUNCTION_ERROR]*/
        /*Tests_SRS_JSON_ENCODER_99_043:[ If the child has children, then JSONEncoder_EncodeTree  shall be invoked for every of these children.]*/
        /*Tests_SRS_JSON_ENCODER_99_044:[ A "," shall be added for every child, except the last one.]*/
        /*Tests_SRS_JSON_ENCODER_99_045:[ The string "}" shall be added to the output]*/
        /*This is the baseline for tree_5_1*/
        /*The below test shall be copy&pasted several times (as many as failing points there are, that
        is the number of STRICT_EXPECTED_CALLS + the number of blablalballbaFunction_call_count;*/
        TEST_FUNCTION(JSONEncoder_EncodeTree_5_1_fails_when_MultiTree_GetName_fails_3)
        {
            ///arrange
            EXPECTED_CALL((*mocks), STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(3);
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(12);
            EXPECTED_CALL((*mocks), STRING_new())
                .ExpectedTimesExactly(5);
            EXPECTED_CALL((*mocks), STRING_delete(IGNORED_PTR_ARG))
                .ExpectedTimesExactly(5);

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_5_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_5_1, 0, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_5, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_5, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            /*below are because of recursive call of Encode Tree*/
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_5, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            /*child 4*/
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_5, 0, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_4, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_CHILD_4, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetValue(TEST_MULTITREE_HANDLE_CHILD_4, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), TestFunc_NodesAreStrings(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreAllArguments()
                ;

            /*child 5*/

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_5, 1, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;


            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_5, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                .SetReturn(MULTITREE_ERROR)
                ;


            ///act
            auto result = JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_5_1, global_bufferTemp,  TestFunc_NodesAreStrings);

            ///assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_MULTITREE_ERROR, result);
            
            
            
            
            
            

            
            ASSERT_ARE_EQUAL(tchar_ptr, _T(""), mocks->CompareActualAndExpectedCalls().c_str());
        }
        /*Tests_SRS_JSON_ENCODER_99_037:[ For every child, the following actions shall take place in order to produce name:value:]*/
        /*Tests_SRS_JSON_ENCODER_99_039:[ The child name shall be retrieved by a call to MultiTree_GetChild followed by a MultiTree_GetName.]*/
        /*Tests_SRS_JSON_ENCODER_99_040:[ After retrieval, it shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_041:[ A "\":" (single quote followed by colon) shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_042:[ If the child has zero children, then the function toStringFunc shall be invoked for the value (as computed by MultiTree_GetValue) of that child]*/
        /*Tests_SRS_JSON_ENCODER_99_047:[If toString function returns an error then JSON_Encoder_EncodeTree shall return JSON_ENCODER_TOSTRING_FUNCTION_ERROR]*/
        /*Tests_SRS_JSON_ENCODER_99_043:[ If the child has children, then JSONEncoder_EncodeTree  shall be invoked for every of these children.]*/
        /*Tests_SRS_JSON_ENCODER_99_044:[ A "," shall be added for every child, except the last one.]*/
        /*Tests_SRS_JSON_ENCODER_99_045:[ The string "}" shall be added to the output]*/
        /*This is the baseline for tree_5_1*/
        /*The below test shall be copy&pasted several times (as many as failing points there are, that
        is the number of STRICT_EXPECTED_CALLS + the number of blablalballbaFunction_call_count;*/
        TEST_FUNCTION(JSONEncoder_EncodeTree_5_1_fails_when_strcat_s_fails_7)
        {
            ///arrange
            EXPECTED_CALL((*mocks), STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(4);
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(12);
            EXPECTED_CALL((*mocks), STRING_new())
                .ExpectedTimesExactly(5);
            EXPECTED_CALL((*mocks), STRING_delete(IGNORED_PTR_ARG))
                .ExpectedTimesExactly(5);

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_5_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_5_1, 0, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_5, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_5, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            /*below are because of recursive call of Encode Tree*/
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_5, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            /*child 4*/
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_5, 0, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_4, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_CHILD_4, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetValue(TEST_MULTITREE_HANDLE_CHILD_4, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), TestFunc_NodesAreStrings(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreAllArguments()
                ;

            /*child 5*/

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_5, 1, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;


            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_5, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            whenShallSTRING_concat_with_STRING_fail = 4;

            ///act
            auto result = JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_5_1, global_bufferTemp,  TestFunc_NodesAreStrings);

            ///assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_ERROR, result);
            
            ASSERT_ARE_EQUAL(tchar_ptr, _T(""), mocks->CompareActualAndExpectedCalls().c_str());
        }
        /*Tests_SRS_JSON_ENCODER_99_037:[ For every child, the following actions shall take place in order to produce name:value:]*/
        /*Tests_SRS_JSON_ENCODER_99_039:[ The child name shall be retrieved by a call to MultiTree_GetChild followed by a MultiTree_GetName.]*/
        /*Tests_SRS_JSON_ENCODER_99_040:[ After retrieval, it shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_041:[ A "\":" (single quote followed by colon) shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_042:[ If the child has zero children, then the function toStringFunc shall be invoked for the value (as computed by MultiTree_GetValue) of that child]*/
        /*Tests_SRS_JSON_ENCODER_99_047:[If toString function returns an error then JSON_Encoder_EncodeTree shall return JSON_ENCODER_TOSTRING_FUNCTION_ERROR]*/
        /*Tests_SRS_JSON_ENCODER_99_043:[ If the child has children, then JSONEncoder_EncodeTree  shall be invoked for every of these children.]*/
        /*Tests_SRS_JSON_ENCODER_99_044:[ A "," shall be added for every child, except the last one.]*/
        /*Tests_SRS_JSON_ENCODER_99_045:[ The string "}" shall be added to the output]*/
        /*This is the baseline for tree_5_1*/
        /*The below test shall be copy&pasted several times (as many as failing points there are, that
        is the number of STRICT_EXPECTED_CALLS + the number of blablalballbaFunction_call_count;*/
        TEST_FUNCTION(JSONEncoder_EncodeTree_5_1_fails_when_MultiTree_GetChildCount_fails_4)
        {
            ///arrange
            EXPECTED_CALL((*mocks), STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(4);
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(13);
            EXPECTED_CALL((*mocks), STRING_new())
                .ExpectedTimesExactly(5);
            EXPECTED_CALL((*mocks), STRING_delete(IGNORED_PTR_ARG))
                .ExpectedTimesExactly(5);

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_5_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_5_1, 0, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_5, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_5, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            /*below are because of recursive call of Encode Tree*/
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_5, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            /*child 4*/
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_5, 0, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_4, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_CHILD_4, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetValue(TEST_MULTITREE_HANDLE_CHILD_4, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), TestFunc_NodesAreStrings(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreAllArguments()
                ;

            /*child 5*/

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_5, 1, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;


            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_5, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_CHILD_5, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                .SetReturn(MULTITREE_ERROR)
                ;


            ///act
            auto result = JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_5_1, global_bufferTemp,  TestFunc_NodesAreStrings);

            ///assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_MULTITREE_ERROR, result);
            
            
            
            
            
            

            
            
            ASSERT_ARE_EQUAL(tchar_ptr, _T(""), mocks->CompareActualAndExpectedCalls().c_str());
        }
        /*Tests_SRS_JSON_ENCODER_99_037:[ For every child, the following actions shall take place in order to produce name:value:]*/
        /*Tests_SRS_JSON_ENCODER_99_039:[ The child name shall be retrieved by a call to MultiTree_GetChild followed by a MultiTree_GetName.]*/
        /*Tests_SRS_JSON_ENCODER_99_040:[ After retrieval, it shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_041:[ A "\":" (single quote followed by colon) shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_042:[ If the child has zero children, then the function toStringFunc shall be invoked for the value (as computed by MultiTree_GetValue) of that child]*/
        /*Tests_SRS_JSON_ENCODER_99_047:[If toString function returns an error then JSON_Encoder_EncodeTree shall return JSON_ENCODER_TOSTRING_FUNCTION_ERROR]*/
        /*Tests_SRS_JSON_ENCODER_99_043:[ If the child has children, then JSONEncoder_EncodeTree  shall be invoked for every of these children.]*/
        /*Tests_SRS_JSON_ENCODER_99_044:[ A "," shall be added for every child, except the last one.]*/
        /*Tests_SRS_JSON_ENCODER_99_045:[ The string "}" shall be added to the output]*/
        /*This is the baseline for tree_5_1*/
        /*The below test shall be copy&pasted several times (as many as failing points there are, that
        is the number of STRICT_EXPECTED_CALLS + the number of blablalballbaFunction_call_count;*/
        TEST_FUNCTION(JSONEncoder_EncodeTree_5_1_fails_when_MultiTree_GetValue_fails_2)
        {
            ///arrange
            EXPECTED_CALL((*mocks), STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(4);
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(13);
            EXPECTED_CALL((*mocks), STRING_new())
                .ExpectedTimesExactly(5);
            EXPECTED_CALL((*mocks), STRING_delete(IGNORED_PTR_ARG))
                .ExpectedTimesExactly(5);

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_5_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_5_1, 0, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_5, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_5, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            /*below are because of recursive call of Encode Tree*/
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_5, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            /*child 4*/
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_5, 0, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_4, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_CHILD_4, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetValue(TEST_MULTITREE_HANDLE_CHILD_4, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), TestFunc_NodesAreStrings(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreAllArguments()
                ;

            /*child 5*/

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_5, 1, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;


            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_5, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_CHILD_5, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetValue(TEST_MULTITREE_HANDLE_CHILD_5, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                .SetReturn(MULTITREE_ERROR)
                ;


            ///act
            auto result = JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_5_1, global_bufferTemp,  TestFunc_NodesAreStrings);

            ///assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_MULTITREE_ERROR, result);
            
            
            
            
            
            

            
            
            ASSERT_ARE_EQUAL(tchar_ptr, _T(""), mocks->CompareActualAndExpectedCalls().c_str());
        }
        /*Tests_SRS_JSON_ENCODER_99_037:[ For every child, the following actions shall take place in order to produce name:value:]*/
        /*Tests_SRS_JSON_ENCODER_99_039:[ The child name shall be retrieved by a call to MultiTree_GetChild followed by a MultiTree_GetName.]*/
        /*Tests_SRS_JSON_ENCODER_99_040:[ After retrieval, it shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_041:[ A "\":" (single quote followed by colon) shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_042:[ If the child has zero children, then the function toStringFunc shall be invoked for the value (as computed by MultiTree_GetValue) of that child]*/
        /*Tests_SRS_JSON_ENCODER_99_047:[If toString function returns an error then JSON_Encoder_EncodeTree shall return JSON_ENCODER_TOSTRING_FUNCTION_ERROR]*/
        /*Tests_SRS_JSON_ENCODER_99_043:[ If the child has children, then JSONEncoder_EncodeTree  shall be invoked for every of these children.]*/
        /*Tests_SRS_JSON_ENCODER_99_044:[ A "," shall be added for every child, except the last one.]*/
        /*Tests_SRS_JSON_ENCODER_99_045:[ The string "}" shall be added to the output]*/
        /*This is the baseline for tree_5_1*/
        /*The below test shall be copy&pasted several times (as many as failing points there are, that
        is the number of STRICT_EXPECTED_CALLS + the number of blablalballbaFunction_call_count;*/
        TEST_FUNCTION(JSONEncoder_EncodeTree_5_1_fails_when_toStringFunc_fails_2)
        {
            ///arrange
            EXPECTED_CALL((*mocks), STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(4);
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(14);
            EXPECTED_CALL((*mocks), STRING_new())
                .ExpectedTimesExactly(6);
            EXPECTED_CALL((*mocks), STRING_delete(IGNORED_PTR_ARG))
                .ExpectedTimesExactly(6);

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_5_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_5_1, 0, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_5, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_5, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            /*below are because of recursive call of Encode Tree*/
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_5, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            /*child 4*/
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_5, 0, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_4, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_CHILD_4, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetValue(TEST_MULTITREE_HANDLE_CHILD_4, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), TestFunc_NodesAreStrings(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreAllArguments()
                ;

            /*child 5*/

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_5, 1, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;


            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_5, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_CHILD_5, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetValue(TEST_MULTITREE_HANDLE_CHILD_5, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), TestFunc_NodesAreStrings(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreAllArguments()
                .SetReturn(JSON_ENCODER_TOSTRING_ERROR)
                ;

            ///act
            auto result = JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_5_1, global_bufferTemp,  TestFunc_NodesAreStrings);

            ///assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_TOSTRING_FUNCTION_ERROR, result);
            
            ASSERT_ARE_EQUAL(tchar_ptr, _T(""), mocks->CompareActualAndExpectedCalls().c_str());
        }
        /*Tests_SRS_JSON_ENCODER_99_037:[ For every child, the following actions shall take place in order to produce name:value:]*/
        /*Tests_SRS_JSON_ENCODER_99_039:[ The child name shall be retrieved by a call to MultiTree_GetChild followed by a MultiTree_GetName.]*/
        /*Tests_SRS_JSON_ENCODER_99_040:[ After retrieval, it shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_041:[ A "\":" (single quote followed by colon) shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_042:[ If the child has zero children, then the function toStringFunc shall be invoked for the value (as computed by MultiTree_GetValue) of that child]*/
        /*Tests_SRS_JSON_ENCODER_99_047:[If toString function returns an error then JSON_Encoder_EncodeTree shall return JSON_ENCODER_TOSTRING_FUNCTION_ERROR]*/
        /*Tests_SRS_JSON_ENCODER_99_043:[ If the child has children, then JSONEncoder_EncodeTree  shall be invoked for every of these children.]*/
        /*Tests_SRS_JSON_ENCODER_99_044:[ A "," shall be added for every child, except the last one.]*/
        /*Tests_SRS_JSON_ENCODER_99_045:[ The string "}" shall be added to the output]*/
        /*This is the baseline for tree_5_1*/
        /*The below test shall be copy&pasted several times (as many as failing points there are, that
        is the number of STRICT_EXPECTED_CALLS + the number of blablalballbaFunction_call_count;*/
        TEST_FUNCTION(JSONEncoder_EncodeTree_5_1_fails_when_strcat_s_fails_8)
        {
            ///arrange
            EXPECTED_CALL((*mocks), STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(5);
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(14);
            EXPECTED_CALL((*mocks), STRING_new())
                .ExpectedTimesExactly(6);
            EXPECTED_CALL((*mocks), STRING_delete(IGNORED_PTR_ARG))
                .ExpectedTimesExactly(6);

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_5_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_5_1, 0, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_5, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_5, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            /*below are because of recursive call of Encode Tree*/
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_5, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            /*child 4*/
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_5, 0, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_4, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_CHILD_4, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetValue(TEST_MULTITREE_HANDLE_CHILD_4, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), TestFunc_NodesAreStrings(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreAllArguments()
                ;

            /*child 5*/

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_5, 1, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;


            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_5, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_CHILD_5, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetValue(TEST_MULTITREE_HANDLE_CHILD_5, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), TestFunc_NodesAreStrings(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreAllArguments()
                ;

            whenShallSTRING_concat_with_STRING_fail = 5;

            ///act
            auto result = JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_5_1, global_bufferTemp,  TestFunc_NodesAreStrings);

            ///assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_ERROR, result);

            ASSERT_ARE_EQUAL(tchar_ptr, _T(""), mocks->CompareActualAndExpectedCalls().c_str());
        }
        /*Tests_SRS_JSON_ENCODER_99_037:[ For every child, the following actions shall take place in order to produce name:value:]*/
        /*Tests_SRS_JSON_ENCODER_99_039:[ The child name shall be retrieved by a call to MultiTree_GetChild followed by a MultiTree_GetName.]*/
        /*Tests_SRS_JSON_ENCODER_99_040:[ After retrieval, it shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_041:[ A "\":" (single quote followed by colon) shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_042:[ If the child has zero children, then the function toStringFunc shall be invoked for the value (as computed by MultiTree_GetValue) of that child]*/
        /*Tests_SRS_JSON_ENCODER_99_047:[If toString function returns an error then JSON_Encoder_EncodeTree shall return JSON_ENCODER_TOSTRING_FUNCTION_ERROR]*/
        /*Tests_SRS_JSON_ENCODER_99_043:[ If the child has children, then JSONEncoder_EncodeTree  shall be invoked for every of these children.]*/
        /*Tests_SRS_JSON_ENCODER_99_044:[ A "," shall be added for every child, except the last one.]*/
        /*Tests_SRS_JSON_ENCODER_99_045:[ The string "}" shall be added to the output]*/
        /*This is the baseline for tree_5_1*/
        /*The below test shall be copy&pasted several times (as many as failing points there are, that
        is the number of STRICT_EXPECTED_CALLS + the number of blablalballbaFunction_call_count;*/
        TEST_FUNCTION(JSONEncoder_EncodeTree_5_1_fails_when_strcat_s_fails_9)
        {
            ///arrange
            EXPECTED_CALL((*mocks), STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(5);
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ExpectedTimesExactly(14);
            EXPECTED_CALL((*mocks), STRING_new())
                .ExpectedTimesExactly(6);
            EXPECTED_CALL((*mocks), STRING_delete(IGNORED_PTR_ARG))
                .ExpectedTimesExactly(6);

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_5_1, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_5_1, 0, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_5, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_5, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            /*below are because of recursive call of Encode Tree*/
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_5, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            /*child 4*/
            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_5, 0, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_4, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_CHILD_4, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetValue(TEST_MULTITREE_HANDLE_CHILD_4, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), TestFunc_NodesAreStrings(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreAllArguments()
                ;

            /*child 5*/

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChild(TEST_MULTITREE_HANDLE_5, 1, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                ;


            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetName(TEST_MULTITREE_HANDLE_CHILD_5, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildCount(TEST_MULTITREE_HANDLE_CHILD_5, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_GetValue(TEST_MULTITREE_HANDLE_CHILD_5, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                ;

            STRICT_EXPECTED_CALL((*mocks), TestFunc_NodesAreStrings(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreAllArguments()
                ;

            whenShallSTRING_concat_with_STRING_fail = 5;

            ///act
            auto result = JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_5_1, global_bufferTemp,  TestFunc_NodesAreStrings);

            ///assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_ERROR, result);

            ASSERT_ARE_EQUAL(tchar_ptr, _T(""), mocks->CompareActualAndExpectedCalls().c_str());
        }        

        /*so it turns out I can write tests at about 8 pair of child:value / day*/
        /*at this point in my initial testing plan there are precisley 38 more pairs*/
        /*so that would be like the rest of the week I would do these tests*/
        /*after the previous test (recursive), the ROI is pretty low in writing all those tests*/

        /*so the focus will be on getting the right value out of the encoder*/

        /*Tests_SRS_JSON_ENCODER_99_037:[ For every child, the following actions shall take place in order to produce name:value:]*/
        /*Tests_SRS_JSON_ENCODER_99_039:[ The child name shall be retrieved by a call to MultiTree_GetChild followed by a MultiTree_GetName.]*/
        /*Tests_SRS_JSON_ENCODER_99_040:[ After retrieval, it shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_041:[ A "\":" (single quote followed by colon) shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_042:[ If the child has zero children, then the function toStringFunc shall be invoked for the value (as computed by MultiTree_GetValue) of that child]*/
        /*Tests_SRS_JSON_ENCODER_99_047:[If toString function returns an error then JSON_Encoder_EncodeTree shall return JSON_ENCODER_TOSTRING_FUNCTION_ERROR]*/
        /*Tests_SRS_JSON_ENCODER_99_043:[ If the child has children, then JSONEncoder_EncodeTree  shall be invoked for every of these children.]*/
        /*Tests_SRS_JSON_ENCODER_99_044:[ A "," shall be added for every child, except the last one.]*/
        /*Tests_SRS_JSON_ENCODER_99_045:[ The string "}" shall be added to the output]*/
        /*This is the baseline for tree_5_1*/
        /*The below test shall be copy&pasted several times (as many as failing points there are, that
        is the number of STRICT_EXPECTED_CALLS + the number of blablalballbaFunction_call_count;*/
        TEST_FUNCTION(JSONEncoder_EncodeTree_5_2_1_success)
        {
            ///arrange
            
            ///act
            auto result = JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_5_2_1, global_bufferTemp,  TestFunc_NodesAreStrings);

            ///assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_OK, result);
            ASSERT_ARE_EQUAL(char_ptr, "{\"subtree\":{\"child4\":\"value4\", \"child5\":\"value5\"}, \"child1\":\"value1\"}", STRING_c_str(global_bufferTemp));

        }
        /*Tests_SRS_JSON_ENCODER_99_037:[ For every child, the following actions shall take place in order to produce name:value:]*/
        /*Tests_SRS_JSON_ENCODER_99_039:[ The child name shall be retrieved by a call to MultiTree_GetChild followed by a MultiTree_GetName.]*/
        /*Tests_SRS_JSON_ENCODER_99_040:[ After retrieval, it shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_041:[ A "\":" (single quote followed by colon) shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_042:[ If the child has zero children, then the function toStringFunc shall be invoked for the value (as computed by MultiTree_GetValue) of that child]*/
        /*Tests_SRS_JSON_ENCODER_99_047:[If toString function returns an error then JSON_Encoder_EncodeTree shall return JSON_ENCODER_TOSTRING_FUNCTION_ERROR]*/
        /*Tests_SRS_JSON_ENCODER_99_043:[ If the child has children, then JSONEncoder_EncodeTree  shall be invoked for every of these children.]*/
        /*Tests_SRS_JSON_ENCODER_99_044:[ A "," shall be added for every child, except the last one.]*/
        /*Tests_SRS_JSON_ENCODER_99_045:[ The string "}" shall be added to the output]*/
        /*This is the baseline for tree_5_1*/
        /*The below test shall be copy&pasted several times (as many as failing points there are, that
        is the number of STRICT_EXPECTED_CALLS + the number of blablalballbaFunction_call_count;*/
        TEST_FUNCTION(JSONEncoder_EncodeTree_5_2_2_success)
        {
            ///arrange

            ///act
            auto result = JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_5_2_2, global_bufferTemp,  TestFunc_NodesAreStrings);

            ///assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_OK, result);
            ASSERT_ARE_EQUAL(char_ptr, "{\"child1\":\"value1\", \"subtree\":{\"child4\":\"value4\", \"child5\":\"value5\"}}", STRING_c_str(global_bufferTemp));
        }
        /*Tests_SRS_JSON_ENCODER_99_037:[ For every child, the following actions shall take place in order to produce name:value:]*/
        /*Tests_SRS_JSON_ENCODER_99_039:[ The child name shall be retrieved by a call to MultiTree_GetChild followed by a MultiTree_GetName.]*/
        /*Tests_SRS_JSON_ENCODER_99_040:[ After retrieval, it shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_041:[ A "\":" (single quote followed by colon) shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_042:[ If the child has zero children, then the function toStringFunc shall be invoked for the value (as computed by MultiTree_GetValue) of that child]*/
        /*Tests_SRS_JSON_ENCODER_99_047:[If toString function returns an error then JSON_Encoder_EncodeTree shall return JSON_ENCODER_TOSTRING_FUNCTION_ERROR]*/
        /*Tests_SRS_JSON_ENCODER_99_043:[ If the child has children, then JSONEncoder_EncodeTree  shall be invoked for every of these children.]*/
        /*Tests_SRS_JSON_ENCODER_99_044:[ A "," shall be added for every child, except the last one.]*/
        /*Tests_SRS_JSON_ENCODER_99_045:[ The string "}" shall be added to the output]*/
        /*This is the baseline for tree_5_1*/
        /*The below test shall be copy&pasted several times (as many as failing points there are, that
        is the number of STRICT_EXPECTED_CALLS + the number of blablalballbaFunction_call_count;*/
        TEST_FUNCTION(JSONEncoder_EncodeTree_5_3_1_success)
        {
            ///arrange

            ///act
            auto result = JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_5_3_1, global_bufferTemp,  TestFunc_NodesAreStrings);

            ///assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_OK, result);
            ASSERT_ARE_EQUAL(char_ptr, "{\"subtree\":{\"child4\":\"value4\", \"child5\":\"value5\"}, \"child1\":\"value1\", \"child2\":\"value2\"}", STRING_c_str(global_bufferTemp));
        }
        /*Tests_SRS_JSON_ENCODER_99_037:[ For every child, the following actions shall take place in order to produce name:value:]*/
        /*Tests_SRS_JSON_ENCODER_99_039:[ The child name shall be retrieved by a call to MultiTree_GetChild followed by a MultiTree_GetName.]*/
        /*Tests_SRS_JSON_ENCODER_99_040:[ After retrieval, it shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_041:[ A "\":" (single quote followed by colon) shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_042:[ If the child has zero children, then the function toStringFunc shall be invoked for the value (as computed by MultiTree_GetValue) of that child]*/
        /*Tests_SRS_JSON_ENCODER_99_047:[If toString function returns an error then JSON_Encoder_EncodeTree shall return JSON_ENCODER_TOSTRING_FUNCTION_ERROR]*/
        /*Tests_SRS_JSON_ENCODER_99_043:[ If the child has children, then JSONEncoder_EncodeTree  shall be invoked for every of these children.]*/
        /*Tests_SRS_JSON_ENCODER_99_044:[ A "," shall be added for every child, except the last one.]*/
        /*Tests_SRS_JSON_ENCODER_99_045:[ The string "}" shall be added to the output]*/
        /*This is the baseline for tree_5_1*/
        /*The below test shall be copy&pasted several times (as many as failing points there are, that
        is the number of STRICT_EXPECTED_CALLS + the number of blablalballbaFunction_call_count;*/
        TEST_FUNCTION(JSONEncoder_EncodeTree_5_3_2_success)
        {
            ///arrange

            ///act
            auto result = JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_5_3_2, global_bufferTemp,  TestFunc_NodesAreStrings);

            ///assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_OK, result);
            ASSERT_ARE_EQUAL(char_ptr, "{\"child1\":\"value1\", \"subtree\":{\"child4\":\"value4\", \"child5\":\"value5\"}, \"child2\":\"value2\"}", STRING_c_str(global_bufferTemp));
        }
        /*Tests_SRS_JSON_ENCODER_99_037:[ For every child, the following actions shall take place in order to produce name:value:]*/
        /*Tests_SRS_JSON_ENCODER_99_039:[ The child name shall be retrieved by a call to MultiTree_GetChild followed by a MultiTree_GetName.]*/
        /*Tests_SRS_JSON_ENCODER_99_040:[ After retrieval, it shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_041:[ A "\":" (single quote followed by colon) shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_042:[ If the child has zero children, then the function toStringFunc shall be invoked for the value (as computed by MultiTree_GetValue) of that child]*/
        /*Tests_SRS_JSON_ENCODER_99_047:[If toString function returns an error then JSON_Encoder_EncodeTree shall return JSON_ENCODER_TOSTRING_FUNCTION_ERROR]*/
        /*Tests_SRS_JSON_ENCODER_99_043:[ If the child has children, then JSONEncoder_EncodeTree  shall be invoked for every of these children.]*/
        /*Tests_SRS_JSON_ENCODER_99_044:[ A "," shall be added for every child, except the last one.]*/
        /*Tests_SRS_JSON_ENCODER_99_045:[ The string "}" shall be added to the output]*/
        /*This is the baseline for tree_5_1*/
        /*The below test shall be copy&pasted several times (as many as failing points there are, that
        is the number of STRICT_EXPECTED_CALLS + the number of blablalballbaFunction_call_count;*/
        TEST_FUNCTION(JSONEncoder_EncodeTree_5_3_3_success)
        {
            ///arrange

            ///act
            auto result = JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_5_3_3, global_bufferTemp,  TestFunc_NodesAreStrings);

            ///assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_OK, result);
            ASSERT_ARE_EQUAL(char_ptr, "{\"child1\":\"value1\", \"child2\":\"value2\", \"subtree\":{\"child4\":\"value4\", \"child5\":\"value5\"}}", STRING_c_str(global_bufferTemp));
        }        
        /*Tests_SRS_JSON_ENCODER_99_037:[ For every child, the following actions shall take place in order to produce name:value:]*/
        /*Tests_SRS_JSON_ENCODER_99_039:[ The child name shall be retrieved by a call to MultiTree_GetChild followed by a MultiTree_GetName.]*/
        /*Tests_SRS_JSON_ENCODER_99_040:[ After retrieval, it shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_041:[ A "\":" (single quote followed by colon) shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_042:[ If the child has zero children, then the function toStringFunc shall be invoked for the value (as computed by MultiTree_GetValue) of that child]*/
        /*Tests_SRS_JSON_ENCODER_99_047:[If toString function returns an error then JSON_Encoder_EncodeTree shall return JSON_ENCODER_TOSTRING_FUNCTION_ERROR]*/
        /*Tests_SRS_JSON_ENCODER_99_043:[ If the child has children, then JSONEncoder_EncodeTree  shall be invoked for every of these children.]*/
        /*Tests_SRS_JSON_ENCODER_99_044:[ A "," shall be added for every child, except the last one.]*/
        /*Tests_SRS_JSON_ENCODER_99_045:[ The string "}" shall be added to the output]*/
        /*This is the baseline for tree_5_1*/
        /*The below test shall be copy&pasted several times (as many as failing points there are, that
        is the number of STRICT_EXPECTED_CALLS + the number of blablalballbaFunction_call_count;*/
        TEST_FUNCTION(JSONEncoder_EncodeTree_5_4_1_success)
        {
            ///arrange

            ///act
            auto result = JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_5_4_1, global_bufferTemp, TestFunc_NodesAreStrings);

            ///assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_OK, result);
            ASSERT_ARE_EQUAL(char_ptr, "{\"subtree\":{\"child4\":\"value4\", \"child5\":\"value5\"}, \"child1\":\"value1\", \"child2\":\"value2\", \"child3\":\"value3\"}", STRING_c_str(global_bufferTemp));
        }
        /*Tests_SRS_JSON_ENCODER_99_037:[ For every child, the following actions shall take place in order to produce name:value:]*/
        /*Tests_SRS_JSON_ENCODER_99_039:[ The child name shall be retrieved by a call to MultiTree_GetChild followed by a MultiTree_GetName.]*/
        /*Tests_SRS_JSON_ENCODER_99_040:[ After retrieval, it shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_041:[ A "\":" (single quote followed by colon) shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_042:[ If the child has zero children, then the function toStringFunc shall be invoked for the value (as computed by MultiTree_GetValue) of that child]*/
        /*Tests_SRS_JSON_ENCODER_99_047:[If toString function returns an error then JSON_Encoder_EncodeTree shall return JSON_ENCODER_TOSTRING_FUNCTION_ERROR]*/
        /*Tests_SRS_JSON_ENCODER_99_043:[ If the child has children, then JSONEncoder_EncodeTree  shall be invoked for every of these children.]*/
        /*Tests_SRS_JSON_ENCODER_99_044:[ A "," shall be added for every child, except the last one.]*/
        /*Tests_SRS_JSON_ENCODER_99_045:[ The string "}" shall be added to the output]*/
        /*This is the baseline for tree_5_1*/
        /*The below test shall be copy&pasted several times (as many as failing points there are, that
        is the number of STRICT_EXPECTED_CALLS + the number of blablalballbaFunction_call_count;*/
        TEST_FUNCTION(JSONEncoder_EncodeTree_5_4_2_success)
        {
            ///arrange

            ///act
            auto result = JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_5_4_2, global_bufferTemp, TestFunc_NodesAreStrings);

            ///assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_OK, result);
            ASSERT_ARE_EQUAL(char_ptr, "{\"child1\":\"value1\", \"subtree\":{\"child4\":\"value4\", \"child5\":\"value5\"}, \"child2\":\"value2\", \"child3\":\"value3\"}", STRING_c_str(global_bufferTemp));
        }        
        /*Tests_SRS_JSON_ENCODER_99_037:[ For every child, the following actions shall take place in order to produce name:value:]*/
        /*Tests_SRS_JSON_ENCODER_99_039:[ The child name shall be retrieved by a call to MultiTree_GetChild followed by a MultiTree_GetName.]*/
        /*Tests_SRS_JSON_ENCODER_99_040:[ After retrieval, it shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_041:[ A "\":" (single quote followed by colon) shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_042:[ If the child has zero children, then the function toStringFunc shall be invoked for the value (as computed by MultiTree_GetValue) of that child]*/
        /*Tests_SRS_JSON_ENCODER_99_047:[If toString function returns an error then JSON_Encoder_EncodeTree shall return JSON_ENCODER_TOSTRING_FUNCTION_ERROR]*/
        /*Tests_SRS_JSON_ENCODER_99_043:[ If the child has children, then JSONEncoder_EncodeTree  shall be invoked for every of these children.]*/
        /*Tests_SRS_JSON_ENCODER_99_044:[ A "," shall be added for every child, except the last one.]*/
        /*Tests_SRS_JSON_ENCODER_99_045:[ The string "}" shall be added to the output]*/
        /*This is the baseline for tree_5_1*/
        /*The below test shall be copy&pasted several times (as many as failing points there are, that
        is the number of STRICT_EXPECTED_CALLS + the number of blablalballbaFunction_call_count;*/
        TEST_FUNCTION(JSONEncoder_EncodeTree_5_4_3_success)
        {
            ///arrange

            ///act
            auto result = JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_5_4_3, global_bufferTemp,  TestFunc_NodesAreStrings);

            ///assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_OK, result);
            ASSERT_ARE_EQUAL(char_ptr, "{\"child1\":\"value1\", \"child2\":\"value2\", \"subtree\":{\"child4\":\"value4\", \"child5\":\"value5\"}, \"child3\":\"value3\"}", STRING_c_str(global_bufferTemp));
        }
        /*Tests_SRS_JSON_ENCODER_99_037:[ For every child, the following actions shall take place in order to produce name:value:]*/
        /*Tests_SRS_JSON_ENCODER_99_039:[ The child name shall be retrieved by a call to MultiTree_GetChild followed by a MultiTree_GetName.]*/
        /*Tests_SRS_JSON_ENCODER_99_040:[ After retrieval, it shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_041:[ A "\":" (single quote followed by colon) shall be added to the output.]*/
        /*Tests_SRS_JSON_ENCODER_99_042:[ If the child has zero children, then the function toStringFunc shall be invoked for the value (as computed by MultiTree_GetValue) of that child]*/
        /*Tests_SRS_JSON_ENCODER_99_047:[If toString function returns an error then JSON_Encoder_EncodeTree shall return JSON_ENCODER_TOSTRING_FUNCTION_ERROR]*/
        /*Tests_SRS_JSON_ENCODER_99_043:[ If the child has children, then JSONEncoder_EncodeTree  shall be invoked for every of these children.]*/
        /*Tests_SRS_JSON_ENCODER_99_044:[ A "," shall be added for every child, except the last one.]*/
        /*Tests_SRS_JSON_ENCODER_99_045:[ The string "}" shall be added to the output]*/
        /*This is the baseline for tree_5_1*/
        /*The below test shall be copy&pasted several times (as many as failing points there are, that
        is the number of STRICT_EXPECTED_CALLS + the number of blablalballbaFunction_call_count;*/
        TEST_FUNCTION(JSONEncoder_EncodeTree_5_4_4_success)
        {
            ///arrange

            ///act
            auto result = JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE_5_4_4, global_bufferTemp,  TestFunc_NodesAreStrings);

            ///assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_RESULT, JSON_ENCODER_OK, result);
            ASSERT_ARE_EQUAL(char_ptr, "{\"child1\":\"value1\", \"child2\":\"value2\", \"child3\":\"value3\", \"subtree\":{\"child4\":\"value4\", \"child5\":\"value5\"}}", STRING_c_str(global_bufferTemp));
        }
        /*Tests_SRS_JSON_ENCODER_99_047:[ JSONEncoder_CharPtr_ToString shall return JSON_ENCODER_TOSTRING_INVALID_ARG if destination or value parameters passed to it are NULL.]*/
        TEST_FUNCTION(JSONEncoder_CharPtr_ToString_with_NULL_destination_fails)
        {
            ///arrange
            char t = 't';
            ///act
            auto result = JSONEncoder_CharPtr_ToString(NULL, &t);

            ///assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_TOSTRING_RESULT, JSON_ENCODER_TOSTRING_INVALID_ARG, result);
            ASSERT_ARE_EQUAL(tchar_ptr, _T(""), mocks->CompareActualAndExpectedCalls().c_str());
        }
        /*Tests_SRS_JSON_ENCODER_99_047:[ JSONEncoder_CharPtr_ToString shall return JSON_ENCODER_TOSTRING_INVALID_ARG if destination or value parameters passed to it are NULL.]*/
        TEST_FUNCTION(JSONEncoder_CharPtr_ToString_with_NULL_value_fails)
        {
            ///arrange

            ///act
            auto result = JSONEncoder_CharPtr_ToString(global_bufferTemp,  NULL);

            ///assert
            ASSERT_ARE_EQUAL(JSON_ENCODER_TOSTRING_RESULT, JSON_ENCODER_TOSTRING_INVALID_ARG, result);
            ASSERT_ARE_EQUAL(tchar_ptr, _T(""), mocks->CompareActualAndExpectedCalls().c_str());
        }        
        /*Tests_SRS_JSON_ENCODER_99_048:[JSONEncoder_CharPtr_ToString shall use strcpy_s to copy from value to destination.]*/
        /*Tests_SRS_JSON_ENCODER_99_050:[ If strcpy_s doesn't fail, then JSONEncoder_CharPtr_ToString shall return JSON_ENCODER_TOSTRING_OK]*/
        TEST_FUNCTION(JSONEncoder_CharPtr_ToString_uses_strcpy_s)
        {
            ///arrange
            char t2[] = { 'q', '\0' };

            STRICT_EXPECTED_CALL((*mocks), STRING_concat(global_bufferTemp, "q"));

            ///act
            auto result = JSONEncoder_CharPtr_ToString(global_bufferTemp, t2);

            ///assert

            ASSERT_ARE_EQUAL(JSON_ENCODER_TOSTRING_RESULT, JSON_ENCODER_TOSTRING_OK, result);

            ASSERT_ARE_EQUAL(tchar_ptr, _T(""), mocks->CompareActualAndExpectedCalls().c_str());
        }
        /*Tests_SRS_JSON_ENCODER_99_048:[JSONEncoder_CharPtr_ToString shall use strcpy_s to copy from value to destination.]*/
        TEST_FUNCTION(JSONEncoder_CharPtr_ToString_fails_when_strcpy_s_fails)
        {
            ///arrange
            char t2[] = { 'q', '\0' };

            STRICT_EXPECTED_CALL((*mocks), STRING_concat(global_bufferTemp, "q"))
                .SetReturn(1);

            ///act
            auto result = JSONEncoder_CharPtr_ToString(global_bufferTemp, t2);

            ///assert

            ASSERT_ARE_EQUAL(JSON_ENCODER_TOSTRING_RESULT, JSON_ENCODER_TOSTRING_ERROR, result);

            ASSERT_ARE_EQUAL(tchar_ptr, _T(""), mocks->CompareActualAndExpectedCalls().c_str());
        }

END_TEST_SUITE(JSONEncoder_ut)
