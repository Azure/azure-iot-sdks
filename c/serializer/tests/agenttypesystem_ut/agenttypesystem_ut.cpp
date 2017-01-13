// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <cstdlib>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#define CTEST_USE_STDINT

#include "testrunnerswitcher.h"
#include "micromock.h"
#include "micromockcharstararenullterminatedstrings.h"

#include "agenttypesystem.h"
#include <limits>
#include <cfloat>
#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_c_shared_utility/strings.h"

#include "jsondecoder.h"
#include "jsonencoder.h"

#ifndef INTMAX_MIN
#define INTMAX_MIN ((int_least64_t)0x8000000000000000)
#endif

using namespace std;

static MICROMOCK_MUTEX_HANDLE g_testByTest;

DEFINE_MICROMOCK_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_RESULT_VALUES);

DEFINE_MICROMOCK_ENUM_TO_STRING(EDM_BOOLEANS, EDM_BOOLEANS_VALUES)

DEFINE_MICROMOCK_ENUM_TO_STRING(JSON_DECODER_RESULT, JSON_DECODER_RESULT_VALUES);

DEFINE_MICROMOCK_ENUM_TO_STRING(AGENT_DATA_TYPE_TYPE, AGENT_DATA_TYPE_TYPE_VALUES);

/*Tests_SRS_AGENT_TYPE_SYSTEM_99_001:[AGENT_TYPE_SYSTEM shall have the following interface]*/
/*if the file compiles, than the interface is tested that it exists*/

const static MULTITREE_HANDLE MULTITREE_HANDLE_VALID = (MULTITREE_HANDLE)0x44;

//
// We do this namespace redirection so that we don't have to repeat the utility string and buffer code in the mocks!
//
namespace BASEIMPLEMENTATION
{
#include "strings.c"
};

//
// We'll use the following globals to help us target specific STRING_new's to inject an error result.
//
static size_t currentSTRING_new_call;
static size_t whenShallSTRING_new_fail;

static bool currentSTRING_concat_called_from_another_mock;
static size_t currentSTRING_concat_call;
static size_t whenShallSTRING_concat_fail;

static size_t nSTRING_new_calls = 0;
static size_t nSTRING_delete_calls = 0;

static size_t currentSTRING_clone_call;
static size_t whenShallSTRING_clone_fail;

static size_t currentSTRING_construct_call;
static size_t whenShallSTRING_construct_fail;


static size_t currentSTRING_concat_with_STRING_call;
static size_t whenShallSTRING_concat_with_STRING_fail;

static size_t currentSTRING_construct_n_call;
static size_t whenShallSTRING_construct_n_fail;


TYPED_MOCK_CLASS(CMocksForAgentTypeSytem, CGlobalMock)
{
public:
    MOCK_STATIC_METHOD_1(, struct tm*, get_gmtime, time_t*, currentTime)
    MOCK_METHOD_END(struct tm*, gmtime(currentTime));

    MOCK_STATIC_METHOD_1(, void, MultiTree_Destroy, MULTITREE_HANDLE, treeHandle)
    MOCK_VOID_METHOD_END();

    MOCK_STATIC_METHOD_2(, MULTITREE_HANDLE, MultiTree_Create, MULTITREE_CLONE_FUNCTION, cloneFunction, MULTITREE_FREE_FUNCTION, freeFunction)
    MOCK_METHOD_END(MULTITREE_HANDLE, NULL);

    MOCK_STATIC_METHOD_3(, MULTITREE_RESULT, MultiTree_AddLeaf, MULTITREE_HANDLE, treeHandle, const char*, destinationPath, const void*, value)
    MOCK_METHOD_END(MULTITREE_RESULT, MULTITREE_ERROR);

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

    MOCK_STATIC_METHOD_1(, const char*, STRING_c_str, STRING_HANDLE, s)
    MOCK_METHOD_END(const char*, BASEIMPLEMENTATION::STRING_c_str(s))

    MOCK_STATIC_METHOD_1(, int, STRING_empty, STRING_HANDLE, s)
    MOCK_METHOD_END(int, BASEIMPLEMENTATION::STRING_empty(s))

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

    MOCK_STATIC_METHOD_2(, int, STRING_concat_with_STRING, STRING_HANDLE, s1, STRING_HANDLE, s2)
        currentSTRING_concat_with_STRING_call++;
    MOCK_METHOD_END(int, (((currentSTRING_concat_with_STRING_call>0) && (currentSTRING_concat_with_STRING_call == whenShallSTRING_concat_with_STRING_fail)) ? __LINE__ : BASEIMPLEMENTATION::STRING_concat_with_STRING(s1, s2)));

    MOCK_STATIC_METHOD_2(, STRING_HANDLE, STRING_construct_n, const char*, source, size_t, n)
        STRING_HANDLE result2;
    currentSTRING_construct_n_call++;
    if (whenShallSTRING_construct_n_fail>0)
    {
        if (currentSTRING_construct_n_call == whenShallSTRING_construct_n_fail)
        {
            result2 = (STRING_HANDLE)NULL;
        }
        else
        {
            result2 = BASEIMPLEMENTATION::STRING_construct_n(source,n);
        }
    }
    else
    {
        result2 = BASEIMPLEMENTATION::STRING_construct_n(source,n);
    }
    MOCK_METHOD_END(STRING_HANDLE, result2)

    /* JSONEncoder mocks */
    MOCK_STATIC_METHOD_3(, JSON_ENCODER_RESULT, JSONEncoder_EncodeTree, MULTITREE_HANDLE, treeHandle, STRING_HANDLE, buffer, JSON_ENCODER_TOSTRING_FUNC, toStringFunc)
    MOCK_METHOD_END(JSON_ENCODER_RESULT, JSON_ENCODER_OK)
    MOCK_STATIC_METHOD_2(, JSON_ENCODER_TOSTRING_RESULT, JSONEncoder_CharPtr_ToString, STRING_HANDLE, destination, const void*, value)
    MOCK_METHOD_END(JSON_ENCODER_TOSTRING_RESULT, JSON_ENCODER_TOSTRING_OK)
};

DECLARE_GLOBAL_MOCK_METHOD_1(CMocksForAgentTypeSytem, , struct tm*, get_gmtime, time_t*, currentTime);
DECLARE_GLOBAL_MOCK_METHOD_1(CMocksForAgentTypeSytem, , void, MultiTree_Destroy, MULTITREE_HANDLE, treeHandle);
DECLARE_GLOBAL_MOCK_METHOD_2(CMocksForAgentTypeSytem, , MULTITREE_HANDLE, MultiTree_Create, MULTITREE_CLONE_FUNCTION, cloneFunction, MULTITREE_FREE_FUNCTION, freeFunction);
DECLARE_GLOBAL_MOCK_METHOD_3(CMocksForAgentTypeSytem, , MULTITREE_RESULT, MultiTree_AddLeaf, MULTITREE_HANDLE, treeHandle, const char*, destinationPath, const void*, value);

DECLARE_GLOBAL_MOCK_METHOD_3(CMocksForAgentTypeSytem, , JSON_ENCODER_RESULT, JSONEncoder_EncodeTree, MULTITREE_HANDLE, treeHandle, STRING_HANDLE, buffer, JSON_ENCODER_TOSTRING_FUNC, toStringFunc);
DECLARE_GLOBAL_MOCK_METHOD_2(CMocksForAgentTypeSytem, , JSON_ENCODER_TOSTRING_RESULT, JSONEncoder_CharPtr_ToString, STRING_HANDLE, destination, const void*, value);
                             
DECLARE_GLOBAL_MOCK_METHOD_0(CMocksForAgentTypeSytem, , STRING_HANDLE, STRING_new);
DECLARE_GLOBAL_MOCK_METHOD_1(CMocksForAgentTypeSytem, , void, STRING_delete, STRING_HANDLE, s);
DECLARE_GLOBAL_MOCK_METHOD_2(CMocksForAgentTypeSytem, , int, STRING_concat, STRING_HANDLE, s1, const char*, s2);
DECLARE_GLOBAL_MOCK_METHOD_1(CMocksForAgentTypeSytem, , const char*, STRING_c_str, STRING_HANDLE, s);
DECLARE_GLOBAL_MOCK_METHOD_1(CMocksForAgentTypeSytem, , int, STRING_empty, STRING_HANDLE, s);
DECLARE_GLOBAL_MOCK_METHOD_1(CMocksForAgentTypeSytem, , STRING_HANDLE, STRING_clone, STRING_HANDLE, handle);
DECLARE_GLOBAL_MOCK_METHOD_1(CMocksForAgentTypeSytem, , STRING_HANDLE, STRING_construct, const char*, s);
DECLARE_GLOBAL_MOCK_METHOD_2(CMocksForAgentTypeSytem, , int, STRING_concat_with_STRING, STRING_HANDLE, s1, STRING_HANDLE, s2);
DECLARE_GLOBAL_MOCK_METHOD_2(CMocksForAgentTypeSytem, , STRING_HANDLE, STRING_construct_n, const char*, s, size_t, n);



static bool operator==(EDM_DATE_TIME_OFFSET left, EDM_DATE_TIME_OFFSET right)
{
    return memcmp(&left, &right, sizeof(left)) == 0;
}

static bool operator==(EDM_GUID left, EDM_GUID right)
{
    return memcmp(&left, &right, sizeof(left)) == 0;
}

static bool operator==(EDM_BINARY left, EDM_BINARY right)
{
    if (left.size != right.size)
    {
        return false;
    }
    else
    {
        if (left.size == 0)
        {
            if (
                (left.data != NULL) ||
                (right.data != NULL)
                )
            {
                ASSERT_FAIL("invalid data");
            }
            else
            {
                return true;
            }
        }
        else
        {
            if (
                (left.data == NULL) ||
                (right.data == NULL)
                )
            {
                ASSERT_FAIL("invalid data");
            }
            else
            {
                return (memcmp(left.data, right.data, left.size) == 0);
            }
        }
    }
}

struct
{
    EDM_BINARY inputData;
    const char* expectedOutput;
} testVector_EDM_BINARY[] =
{
    { { 1, (unsigned char *)"\x00" }, "\"AA\"" },
    { { 1, (unsigned char *)"\x01" }, "\"AQ\"" },
    { { 1, (unsigned char *)"\x02" }, "\"Ag\"" },
    { { 1, (unsigned char *)"\x03" }, "\"Aw\"" },
    { { 1, (unsigned char *)"\x04" }, "\"BA\"" },
    { { 1, (unsigned char *)"\x05" }, "\"BQ\"" },
    { { 1, (unsigned char *)"\x06" }, "\"Bg\"" },
    { { 1, (unsigned char *)"\x07" }, "\"Bw\"" },
    { { 1, (unsigned char *)"\x08" }, "\"CA\"" },
    { { 1, (unsigned char *)"\x09" }, "\"CQ\"" },
    { { 1, (unsigned char *)"\x0a" }, "\"Cg\"" },
    { { 1, (unsigned char *)"\x0b" }, "\"Cw\"" },
    { { 1, (unsigned char *)"\x0c" }, "\"DA\"" },
    { { 1, (unsigned char *)"\x0d" }, "\"DQ\"" },
    { { 1, (unsigned char *)"\x0e" }, "\"Dg\"" },
    { { 1, (unsigned char *)"\x0f" }, "\"Dw\"" },
    { { 1, (unsigned char *)"\x10" }, "\"EA\"" },
    { { 1, (unsigned char *)"\x11" }, "\"EQ\"" },
    { { 1, (unsigned char *)"\x12" }, "\"Eg\"" },
    { { 1, (unsigned char *)"\x13" }, "\"Ew\"" },
    { { 1, (unsigned char *)"\x14" }, "\"FA\"" },
    { { 1, (unsigned char *)"\x15" }, "\"FQ\"" },
    { { 1, (unsigned char *)"\x16" }, "\"Fg\"" },
    { { 1, (unsigned char *)"\x17" }, "\"Fw\"" },
    { { 1, (unsigned char *)"\x18" }, "\"GA\"" },
    { { 1, (unsigned char *)"\x19" }, "\"GQ\"" },
    { { 1, (unsigned char *)"\x1a" }, "\"Gg\"" },
    { { 1, (unsigned char *)"\x1b" }, "\"Gw\"" },
    { { 1, (unsigned char *)"\x1c" }, "\"HA\"" },
    { { 1, (unsigned char *)"\x1d" }, "\"HQ\"" },
    { { 1, (unsigned char *)"\x1e" }, "\"Hg\"" },
    { { 1, (unsigned char *)"\x1f" }, "\"Hw\"" },
    { { 1, (unsigned char *)"\x20" }, "\"IA\"" },
    { { 1, (unsigned char *)"\x21" }, "\"IQ\"" },
    { { 1, (unsigned char *)"\x22" }, "\"Ig\"" },
    { { 1, (unsigned char *)"\x23" }, "\"Iw\"" },
    { { 1, (unsigned char *)"\x24" }, "\"JA\"" },
    { { 1, (unsigned char *)"\x25" }, "\"JQ\"" },
    { { 1, (unsigned char *)"\x26" }, "\"Jg\"" },
    { { 1, (unsigned char *)"\x27" }, "\"Jw\"" },
    { { 1, (unsigned char *)"\x28" }, "\"KA\"" },
    { { 1, (unsigned char *)"\x29" }, "\"KQ\"" },
    { { 1, (unsigned char *)"\x2a" }, "\"Kg\"" },
    { { 1, (unsigned char *)"\x2b" }, "\"Kw\"" },
    { { 1, (unsigned char *)"\x2c" }, "\"LA\"" },
    { { 1, (unsigned char *)"\x2d" }, "\"LQ\"" },
    { { 1, (unsigned char *)"\x2e" }, "\"Lg\"" },
    { { 1, (unsigned char *)"\x2f" }, "\"Lw\"" },
    { { 1, (unsigned char *)"\x30" }, "\"MA\"" },
    { { 1, (unsigned char *)"\x31" }, "\"MQ\"" },
    { { 1, (unsigned char *)"\x32" }, "\"Mg\"" },
    { { 1, (unsigned char *)"\x33" }, "\"Mw\"" },
    { { 1, (unsigned char *)"\x34" }, "\"NA\"" },
    { { 1, (unsigned char *)"\x35" }, "\"NQ\"" },
    { { 1, (unsigned char *)"\x36" }, "\"Ng\"" },
    { { 1, (unsigned char *)"\x37" }, "\"Nw\"" },
    { { 1, (unsigned char *)"\x38" }, "\"OA\"" },
    { { 1, (unsigned char *)"\x39" }, "\"OQ\"" },
    { { 1, (unsigned char *)"\x3a" }, "\"Og\"" },
    { { 1, (unsigned char *)"\x3b" }, "\"Ow\"" },
    { { 1, (unsigned char *)"\x3c" }, "\"PA\"" },
    { { 1, (unsigned char *)"\x3d" }, "\"PQ\"" },
    { { 1, (unsigned char *)"\x3e" }, "\"Pg\"" },
    { { 1, (unsigned char *)"\x3f" }, "\"Pw\"" },
    { { 1, (unsigned char *)"\x40" }, "\"QA\"" },
    { { 1, (unsigned char *)"\x41" }, "\"QQ\"" },
    { { 1, (unsigned char *)"\x42" }, "\"Qg\"" },
    { { 1, (unsigned char *)"\x43" }, "\"Qw\"" },
    { { 1, (unsigned char *)"\x44" }, "\"RA\"" },
    { { 1, (unsigned char *)"\x45" }, "\"RQ\"" },
    { { 1, (unsigned char *)"\x46" }, "\"Rg\"" },
    { { 1, (unsigned char *)"\x47" }, "\"Rw\"" },
    { { 1, (unsigned char *)"\x48" }, "\"SA\"" },
    { { 1, (unsigned char *)"\x49" }, "\"SQ\"" },
    { { 1, (unsigned char *)"\x4a" }, "\"Sg\"" },
    { { 1, (unsigned char *)"\x4b" }, "\"Sw\"" },
    { { 1, (unsigned char *)"\x4c" }, "\"TA\"" },
    { { 1, (unsigned char *)"\x4d" }, "\"TQ\"" },
    { { 1, (unsigned char *)"\x4e" }, "\"Tg\"" },
    { { 1, (unsigned char *)"\x4f" }, "\"Tw\"" },
    { { 1, (unsigned char *)"\x50" }, "\"UA\"" },
    { { 1, (unsigned char *)"\x51" }, "\"UQ\"" },
    { { 1, (unsigned char *)"\x52" }, "\"Ug\"" },
    { { 1, (unsigned char *)"\x53" }, "\"Uw\"" },
    { { 1, (unsigned char *)"\x54" }, "\"VA\"" },
    { { 1, (unsigned char *)"\x55" }, "\"VQ\"" },
    { { 1, (unsigned char *)"\x56" }, "\"Vg\"" },
    { { 1, (unsigned char *)"\x57" }, "\"Vw\"" },
    { { 1, (unsigned char *)"\x58" }, "\"WA\"" },
    { { 1, (unsigned char *)"\x59" }, "\"WQ\"" },
    { { 1, (unsigned char *)"\x5a" }, "\"Wg\"" },
    { { 1, (unsigned char *)"\x5b" }, "\"Ww\"" },
    { { 1, (unsigned char *)"\x5c" }, "\"XA\"" },
    { { 1, (unsigned char *)"\x5d" }, "\"XQ\"" },
    { { 1, (unsigned char *)"\x5e" }, "\"Xg\"" },
    { { 1, (unsigned char *)"\x5f" }, "\"Xw\"" },
    { { 1, (unsigned char *)"\x60" }, "\"YA\"" },
    { { 1, (unsigned char *)"\x61" }, "\"YQ\"" },
    { { 1, (unsigned char *)"\x62" }, "\"Yg\"" },
    { { 1, (unsigned char *)"\x63" }, "\"Yw\"" },
    { { 1, (unsigned char *)"\x64" }, "\"ZA\"" },
    { { 1, (unsigned char *)"\x65" }, "\"ZQ\"" },
    { { 1, (unsigned char *)"\x66" }, "\"Zg\"" },
    { { 1, (unsigned char *)"\x67" }, "\"Zw\"" },
    { { 1, (unsigned char *)"\x68" }, "\"aA\"" },
    { { 1, (unsigned char *)"\x69" }, "\"aQ\"" },
    { { 1, (unsigned char *)"\x6a" }, "\"ag\"" },
    { { 1, (unsigned char *)"\x6b" }, "\"aw\"" },
    { { 1, (unsigned char *)"\x6c" }, "\"bA\"" },
    { { 1, (unsigned char *)"\x6d" }, "\"bQ\"" },
    { { 1, (unsigned char *)"\x6e" }, "\"bg\"" },
    { { 1, (unsigned char *)"\x6f" }, "\"bw\"" },
    { { 1, (unsigned char *)"\x70" }, "\"cA\"" },
    { { 1, (unsigned char *)"\x71" }, "\"cQ\"" },
    { { 1, (unsigned char *)"\x72" }, "\"cg\"" },
    { { 1, (unsigned char *)"\x73" }, "\"cw\"" },
    { { 1, (unsigned char *)"\x74" }, "\"dA\"" },
    { { 1, (unsigned char *)"\x75" }, "\"dQ\"" },
    { { 1, (unsigned char *)"\x76" }, "\"dg\"" },
    { { 1, (unsigned char *)"\x77" }, "\"dw\"" },
    { { 1, (unsigned char *)"\x78" }, "\"eA\"" },
    { { 1, (unsigned char *)"\x79" }, "\"eQ\"" },
    { { 1, (unsigned char *)"\x7a" }, "\"eg\"" },
    { { 1, (unsigned char *)"\x7b" }, "\"ew\"" },
    { { 1, (unsigned char *)"\x7c" }, "\"fA\"" },
    { { 1, (unsigned char *)"\x7d" }, "\"fQ\"" },
    { { 1, (unsigned char *)"\x7e" }, "\"fg\"" },
    { { 1, (unsigned char *)"\x7f" }, "\"fw\"" },
    { { 1, (unsigned char *)"\x80" }, "\"gA\"" },
    { { 1, (unsigned char *)"\x81" }, "\"gQ\"" },
    { { 1, (unsigned char *)"\x82" }, "\"gg\"" },
    { { 1, (unsigned char *)"\x83" }, "\"gw\"" },
    { { 1, (unsigned char *)"\x84" }, "\"hA\"" },
    { { 1, (unsigned char *)"\x85" }, "\"hQ\"" },
    { { 1, (unsigned char *)"\x86" }, "\"hg\"" },
    { { 1, (unsigned char *)"\x87" }, "\"hw\"" },
    { { 1, (unsigned char *)"\x88" }, "\"iA\"" },
    { { 1, (unsigned char *)"\x89" }, "\"iQ\"" },
    { { 1, (unsigned char *)"\x8a" }, "\"ig\"" },
    { { 1, (unsigned char *)"\x8b" }, "\"iw\"" },
    { { 1, (unsigned char *)"\x8c" }, "\"jA\"" },
    { { 1, (unsigned char *)"\x8d" }, "\"jQ\"" },
    { { 1, (unsigned char *)"\x8e" }, "\"jg\"" },
    { { 1, (unsigned char *)"\x8f" }, "\"jw\"" },
    { { 1, (unsigned char *)"\x90" }, "\"kA\"" },
    { { 1, (unsigned char *)"\x91" }, "\"kQ\"" },
    { { 1, (unsigned char *)"\x92" }, "\"kg\"" },
    { { 1, (unsigned char *)"\x93" }, "\"kw\"" },
    { { 1, (unsigned char *)"\x94" }, "\"lA\"" },
    { { 1, (unsigned char *)"\x95" }, "\"lQ\"" },
    { { 1, (unsigned char *)"\x96" }, "\"lg\"" },
    { { 1, (unsigned char *)"\x97" }, "\"lw\"" },
    { { 1, (unsigned char *)"\x98" }, "\"mA\"" },
    { { 1, (unsigned char *)"\x99" }, "\"mQ\"" },
    { { 1, (unsigned char *)"\x9a" }, "\"mg\"" },
    { { 1, (unsigned char *)"\x9b" }, "\"mw\"" },
    { { 1, (unsigned char *)"\x9c" }, "\"nA\"" },
    { { 1, (unsigned char *)"\x9d" }, "\"nQ\"" },
    { { 1, (unsigned char *)"\x9e" }, "\"ng\"" },
    { { 1, (unsigned char *)"\x9f" }, "\"nw\"" },
    { { 1, (unsigned char *)"\xa0" }, "\"oA\"" },
    { { 1, (unsigned char *)"\xa1" }, "\"oQ\"" },
    { { 1, (unsigned char *)"\xa2" }, "\"og\"" },
    { { 1, (unsigned char *)"\xa3" }, "\"ow\"" },
    { { 1, (unsigned char *)"\xa4" }, "\"pA\"" },
    { { 1, (unsigned char *)"\xa5" }, "\"pQ\"" },
    { { 1, (unsigned char *)"\xa6" }, "\"pg\"" },
    { { 1, (unsigned char *)"\xa7" }, "\"pw\"" },
    { { 1, (unsigned char *)"\xa8" }, "\"qA\"" },
    { { 1, (unsigned char *)"\xa9" }, "\"qQ\"" },
    { { 1, (unsigned char *)"\xaa" }, "\"qg\"" },
    { { 1, (unsigned char *)"\xab" }, "\"qw\"" },
    { { 1, (unsigned char *)"\xac" }, "\"rA\"" },
    { { 1, (unsigned char *)"\xad" }, "\"rQ\"" },
    { { 1, (unsigned char *)"\xae" }, "\"rg\"" },
    { { 1, (unsigned char *)"\xaf" }, "\"rw\"" },
    { { 1, (unsigned char *)"\xb0" }, "\"sA\"" },
    { { 1, (unsigned char *)"\xb1" }, "\"sQ\"" },
    { { 1, (unsigned char *)"\xb2" }, "\"sg\"" },
    { { 1, (unsigned char *)"\xb3" }, "\"sw\"" },
    { { 1, (unsigned char *)"\xb4" }, "\"tA\"" },
    { { 1, (unsigned char *)"\xb5" }, "\"tQ\"" },
    { { 1, (unsigned char *)"\xb6" }, "\"tg\"" },
    { { 1, (unsigned char *)"\xb7" }, "\"tw\"" },
    { { 1, (unsigned char *)"\xb8" }, "\"uA\"" },
    { { 1, (unsigned char *)"\xb9" }, "\"uQ\"" },
    { { 1, (unsigned char *)"\xba" }, "\"ug\"" },
    { { 1, (unsigned char *)"\xbb" }, "\"uw\"" },
    { { 1, (unsigned char *)"\xbc" }, "\"vA\"" },
    { { 1, (unsigned char *)"\xbd" }, "\"vQ\"" },
    { { 1, (unsigned char *)"\xbe" }, "\"vg\"" },
    { { 1, (unsigned char *)"\xbf" }, "\"vw\"" },
    { { 1, (unsigned char *)"\xc0" }, "\"wA\"" },
    { { 1, (unsigned char *)"\xc1" }, "\"wQ\"" },
    { { 1, (unsigned char *)"\xc2" }, "\"wg\"" },
    { { 1, (unsigned char *)"\xc3" }, "\"ww\"" },
    { { 1, (unsigned char *)"\xc4" }, "\"xA\"" },
    { { 1, (unsigned char *)"\xc5" }, "\"xQ\"" },
    { { 1, (unsigned char *)"\xc6" }, "\"xg\"" },
    { { 1, (unsigned char *)"\xc7" }, "\"xw\"" },
    { { 1, (unsigned char *)"\xc8" }, "\"yA\"" },
    { { 1, (unsigned char *)"\xc9" }, "\"yQ\"" },
    { { 1, (unsigned char *)"\xca" }, "\"yg\"" },
    { { 1, (unsigned char *)"\xcb" }, "\"yw\"" },
    { { 1, (unsigned char *)"\xcc" }, "\"zA\"" },
    { { 1, (unsigned char *)"\xcd" }, "\"zQ\"" },
    { { 1, (unsigned char *)"\xce" }, "\"zg\"" },
    { { 1, (unsigned char *)"\xcf" }, "\"zw\"" },
    { { 1, (unsigned char *)"\xd0" }, "\"0A\"" },
    { { 1, (unsigned char *)"\xd1" }, "\"0Q\"" },
    { { 1, (unsigned char *)"\xd2" }, "\"0g\"" },
    { { 1, (unsigned char *)"\xd3" }, "\"0w\"" },
    { { 1, (unsigned char *)"\xd4" }, "\"1A\"" },
    { { 1, (unsigned char *)"\xd5" }, "\"1Q\"" },
    { { 1, (unsigned char *)"\xd6" }, "\"1g\"" },
    { { 1, (unsigned char *)"\xd7" }, "\"1w\"" },
    { { 1, (unsigned char *)"\xd8" }, "\"2A\"" },
    { { 1, (unsigned char *)"\xd9" }, "\"2Q\"" },
    { { 1, (unsigned char *)"\xda" }, "\"2g\"" },
    { { 1, (unsigned char *)"\xdb" }, "\"2w\"" },
    { { 1, (unsigned char *)"\xdc" }, "\"3A\"" },
    { { 1, (unsigned char *)"\xdd" }, "\"3Q\"" },
    { { 1, (unsigned char *)"\xde" }, "\"3g\"" },
    { { 1, (unsigned char *)"\xdf" }, "\"3w\"" },
    { { 1, (unsigned char *)"\xe0" }, "\"4A\"" },
    { { 1, (unsigned char *)"\xe1" }, "\"4Q\"" },
    { { 1, (unsigned char *)"\xe2" }, "\"4g\"" },
    { { 1, (unsigned char *)"\xe3" }, "\"4w\"" },
    { { 1, (unsigned char *)"\xe4" }, "\"5A\"" },
    { { 1, (unsigned char *)"\xe5" }, "\"5Q\"" },
    { { 1, (unsigned char *)"\xe6" }, "\"5g\"" },
    { { 1, (unsigned char *)"\xe7" }, "\"5w\"" },
    { { 1, (unsigned char *)"\xe8" }, "\"6A\"" },
    { { 1, (unsigned char *)"\xe9" }, "\"6Q\"" },
    { { 1, (unsigned char *)"\xea" }, "\"6g\"" },
    { { 1, (unsigned char *)"\xeb" }, "\"6w\"" },
    { { 1, (unsigned char *)"\xec" }, "\"7A\"" },
    { { 1, (unsigned char *)"\xed" }, "\"7Q\"" },
    { { 1, (unsigned char *)"\xee" }, "\"7g\"" },
    { { 1, (unsigned char *)"\xef" }, "\"7w\"" },
    { { 1, (unsigned char *)"\xf0" }, "\"8A\"" },
    { { 1, (unsigned char *)"\xf1" }, "\"8Q\"" },
    { { 1, (unsigned char *)"\xf2" }, "\"8g\"" },
    { { 1, (unsigned char *)"\xf3" }, "\"8w\"" },
    { { 1, (unsigned char *)"\xf4" }, "\"9A\"" },
    { { 1, (unsigned char *)"\xf5" }, "\"9Q\"" },
    { { 1, (unsigned char *)"\xf6" }, "\"9g\"" },
    { { 1, (unsigned char *)"\xf7" }, "\"9w\"" },
    { { 1, (unsigned char *)"\xf8" }, "\"-A\"" },
    { { 1, (unsigned char *)"\xf9" }, "\"-Q\"" },
    { { 1, (unsigned char *)"\xfa" }, "\"-g\"" },
    { { 1, (unsigned char *)"\xfb" }, "\"-w\"" },
    { { 1, (unsigned char *)"\xfc" }, "\"_A\"" },
    { { 1, (unsigned char *)"\xfd" }, "\"_Q\"" },
    { { 1, (unsigned char *)"\xfe" }, "\"_g\"" },
    { { 1, (unsigned char *)"\xff" }, "\"_w\"" },
    { { 2, (unsigned char *)"\x00\x00" }, "\"AAA\"" },
    { { 2, (unsigned char *)"\x00\x11" }, "\"ABE\"" },
    { { 2, (unsigned char *)"\x00\x22" }, "\"ACI\"" },
    { { 2, (unsigned char *)"\x00\x33" }, "\"ADM\"" },
    { { 2, (unsigned char *)"\x00\x44" }, "\"AEQ\"" },
    { { 2, (unsigned char *)"\x00\x55" }, "\"AFU\"" },
    { { 2, (unsigned char *)"\x00\x66" }, "\"AGY\"" },
    { { 2, (unsigned char *)"\x00\x77" }, "\"AHc\"" },
    { { 2, (unsigned char *)"\x00\x88" }, "\"AIg\"" },
    { { 2, (unsigned char *)"\x00\x99" }, "\"AJk\"" },
    { { 2, (unsigned char *)"\x00\xaa" }, "\"AKo\"" },
    { { 2, (unsigned char *)"\x00\xbb" }, "\"ALs\"" },
    { { 2, (unsigned char *)"\x00\xcc" }, "\"AMw\"" },
    { { 2, (unsigned char *)"\x00\xdd" }, "\"AN0\"" },
    { { 2, (unsigned char *)"\x00\xee" }, "\"AO4\"" },
    { { 2, (unsigned char *)"\x00\xff" }, "\"AP8\"" },
    { { 2, (unsigned char *)"\x11\x00" }, "\"EQA\"" },
    { { 2, (unsigned char *)"\x11\x11" }, "\"ERE\"" },
    { { 2, (unsigned char *)"\x11\x22" }, "\"ESI\"" },
    { { 2, (unsigned char *)"\x11\x33" }, "\"ETM\"" },
    { { 2, (unsigned char *)"\x11\x44" }, "\"EUQ\"" },
    { { 2, (unsigned char *)"\x11\x55" }, "\"EVU\"" },
    { { 2, (unsigned char *)"\x11\x66" }, "\"EWY\"" },
    { { 2, (unsigned char *)"\x11\x77" }, "\"EXc\"" },
    { { 2, (unsigned char *)"\x11\x88" }, "\"EYg\"" },
    { { 2, (unsigned char *)"\x11\x99" }, "\"EZk\"" },
    { { 2, (unsigned char *)"\x11\xaa" }, "\"Eao\"" },
    { { 2, (unsigned char *)"\x11\xbb" }, "\"Ebs\"" },
    { { 2, (unsigned char *)"\x11\xcc" }, "\"Ecw\"" },
    { { 2, (unsigned char *)"\x11\xdd" }, "\"Ed0\"" },
    { { 2, (unsigned char *)"\x11\xee" }, "\"Ee4\"" },
    { { 2, (unsigned char *)"\x11\xff" }, "\"Ef8\"" },
    { { 2, (unsigned char *)"\x22\x00" }, "\"IgA\"" },
    { { 2, (unsigned char *)"\x22\x11" }, "\"IhE\"" },
    { { 2, (unsigned char *)"\x22\x22" }, "\"IiI\"" },
    { { 2, (unsigned char *)"\x22\x33" }, "\"IjM\"" },
    { { 2, (unsigned char *)"\x22\x44" }, "\"IkQ\"" },
    { { 2, (unsigned char *)"\x22\x55" }, "\"IlU\"" },
    { { 2, (unsigned char *)"\x22\x66" }, "\"ImY\"" },
    { { 2, (unsigned char *)"\x22\x77" }, "\"Inc\"" },
    { { 2, (unsigned char *)"\x22\x88" }, "\"Iog\"" },
    { { 2, (unsigned char *)"\x22\x99" }, "\"Ipk\"" },
    { { 2, (unsigned char *)"\x22\xaa" }, "\"Iqo\"" },
    { { 2, (unsigned char *)"\x22\xbb" }, "\"Irs\"" },
    { { 2, (unsigned char *)"\x22\xcc" }, "\"Isw\"" },
    { { 2, (unsigned char *)"\x22\xdd" }, "\"It0\"" },
    { { 2, (unsigned char *)"\x22\xee" }, "\"Iu4\"" },
    { { 2, (unsigned char *)"\x22\xff" }, "\"Iv8\"" },
    { { 2, (unsigned char *)"\x33\x00" }, "\"MwA\"" },
    { { 2, (unsigned char *)"\x33\x11" }, "\"MxE\"" },
    { { 2, (unsigned char *)"\x33\x22" }, "\"MyI\"" },
    { { 2, (unsigned char *)"\x33\x33" }, "\"MzM\"" },
    { { 2, (unsigned char *)"\x33\x44" }, "\"M0Q\"" },
    { { 2, (unsigned char *)"\x33\x55" }, "\"M1U\"" },
    { { 2, (unsigned char *)"\x33\x66" }, "\"M2Y\"" },
    { { 2, (unsigned char *)"\x33\x77" }, "\"M3c\"" },
    { { 2, (unsigned char *)"\x33\x88" }, "\"M4g\"" },
    { { 2, (unsigned char *)"\x33\x99" }, "\"M5k\"" },
    { { 2, (unsigned char *)"\x33\xaa" }, "\"M6o\"" },
    { { 2, (unsigned char *)"\x33\xbb" }, "\"M7s\"" },
    { { 2, (unsigned char *)"\x33\xcc" }, "\"M8w\"" },
    { { 2, (unsigned char *)"\x33\xdd" }, "\"M90\"" },
    { { 2, (unsigned char *)"\x33\xee" }, "\"M-4\"" },
    { { 2, (unsigned char *)"\x33\xff" }, "\"M_8\"" },
    { { 2, (unsigned char *)"\x44\x00" }, "\"RAA\"" },
    { { 2, (unsigned char *)"\x44\x11" }, "\"RBE\"" },
    { { 2, (unsigned char *)"\x44\x22" }, "\"RCI\"" },
    { { 2, (unsigned char *)"\x44\x33" }, "\"RDM\"" },
    { { 2, (unsigned char *)"\x44\x44" }, "\"REQ\"" },
    { { 2, (unsigned char *)"\x44\x55" }, "\"RFU\"" },
    { { 2, (unsigned char *)"\x44\x66" }, "\"RGY\"" },
    { { 2, (unsigned char *)"\x44\x77" }, "\"RHc\"" },
    { { 2, (unsigned char *)"\x44\x88" }, "\"RIg\"" },
    { { 2, (unsigned char *)"\x44\x99" }, "\"RJk\"" },
    { { 2, (unsigned char *)"\x44\xaa" }, "\"RKo\"" },
    { { 2, (unsigned char *)"\x44\xbb" }, "\"RLs\"" },
    { { 2, (unsigned char *)"\x44\xcc" }, "\"RMw\"" },
    { { 2, (unsigned char *)"\x44\xdd" }, "\"RN0\"" },
    { { 2, (unsigned char *)"\x44\xee" }, "\"RO4\"" },
    { { 2, (unsigned char *)"\x44\xff" }, "\"RP8\"" },
    { { 2, (unsigned char *)"\x55\x00" }, "\"VQA\"" },
    { { 2, (unsigned char *)"\x55\x11" }, "\"VRE\"" },
    { { 2, (unsigned char *)"\x55\x22" }, "\"VSI\"" },
    { { 2, (unsigned char *)"\x55\x33" }, "\"VTM\"" },
    { { 2, (unsigned char *)"\x55\x44" }, "\"VUQ\"" },
    { { 2, (unsigned char *)"\x55\x55" }, "\"VVU\"" },
    { { 2, (unsigned char *)"\x55\x66" }, "\"VWY\"" },
    { { 2, (unsigned char *)"\x55\x77" }, "\"VXc\"" },
    { { 2, (unsigned char *)"\x55\x88" }, "\"VYg\"" },
    { { 2, (unsigned char *)"\x55\x99" }, "\"VZk\"" },
    { { 2, (unsigned char *)"\x55\xaa" }, "\"Vao\"" },
    { { 2, (unsigned char *)"\x55\xbb" }, "\"Vbs\"" },
    { { 2, (unsigned char *)"\x55\xcc" }, "\"Vcw\"" },
    { { 2, (unsigned char *)"\x55\xdd" }, "\"Vd0\"" },
    { { 2, (unsigned char *)"\x55\xee" }, "\"Ve4\"" },
    { { 2, (unsigned char *)"\x55\xff" }, "\"Vf8\"" },
    { { 2, (unsigned char *)"\x66\x00" }, "\"ZgA\"" },
    { { 2, (unsigned char *)"\x66\x11" }, "\"ZhE\"" },
    { { 2, (unsigned char *)"\x66\x22" }, "\"ZiI\"" },
    { { 2, (unsigned char *)"\x66\x33" }, "\"ZjM\"" },
    { { 2, (unsigned char *)"\x66\x44" }, "\"ZkQ\"" },
    { { 2, (unsigned char *)"\x66\x55" }, "\"ZlU\"" },
    { { 2, (unsigned char *)"\x66\x66" }, "\"ZmY\"" },
    { { 2, (unsigned char *)"\x66\x77" }, "\"Znc\"" },
    { { 2, (unsigned char *)"\x66\x88" }, "\"Zog\"" },
    { { 2, (unsigned char *)"\x66\x99" }, "\"Zpk\"" },
    { { 2, (unsigned char *)"\x66\xaa" }, "\"Zqo\"" },
    { { 2, (unsigned char *)"\x66\xbb" }, "\"Zrs\"" },
    { { 2, (unsigned char *)"\x66\xcc" }, "\"Zsw\"" },
    { { 2, (unsigned char *)"\x66\xdd" }, "\"Zt0\"" },
    { { 2, (unsigned char *)"\x66\xee" }, "\"Zu4\"" },
    { { 2, (unsigned char *)"\x66\xff" }, "\"Zv8\"" },
    { { 2, (unsigned char *)"\x77\x00" }, "\"dwA\"" },
    { { 2, (unsigned char *)"\x77\x11" }, "\"dxE\"" },
    { { 2, (unsigned char *)"\x77\x22" }, "\"dyI\"" },
    { { 2, (unsigned char *)"\x77\x33" }, "\"dzM\"" },
    { { 2, (unsigned char *)"\x77\x44" }, "\"d0Q\"" },
    { { 2, (unsigned char *)"\x77\x55" }, "\"d1U\"" },
    { { 2, (unsigned char *)"\x77\x66" }, "\"d2Y\"" },
    { { 2, (unsigned char *)"\x77\x77" }, "\"d3c\"" },
    { { 2, (unsigned char *)"\x77\x88" }, "\"d4g\"" },
    { { 2, (unsigned char *)"\x77\x99" }, "\"d5k\"" },
    { { 2, (unsigned char *)"\x77\xaa" }, "\"d6o\"" },
    { { 2, (unsigned char *)"\x77\xbb" }, "\"d7s\"" },
    { { 2, (unsigned char *)"\x77\xcc" }, "\"d8w\"" },
    { { 2, (unsigned char *)"\x77\xdd" }, "\"d90\"" },
    { { 2, (unsigned char *)"\x77\xee" }, "\"d-4\"" },
    { { 2, (unsigned char *)"\x77\xff" }, "\"d_8\"" },
    { { 2, (unsigned char *)"\x88\x00" }, "\"iAA\"" },
    { { 2, (unsigned char *)"\x88\x11" }, "\"iBE\"" },
    { { 2, (unsigned char *)"\x88\x22" }, "\"iCI\"" },
    { { 2, (unsigned char *)"\x88\x33" }, "\"iDM\"" },
    { { 2, (unsigned char *)"\x88\x44" }, "\"iEQ\"" },
    { { 2, (unsigned char *)"\x88\x55" }, "\"iFU\"" },
    { { 2, (unsigned char *)"\x88\x66" }, "\"iGY\"" },
    { { 2, (unsigned char *)"\x88\x77" }, "\"iHc\"" },
    { { 2, (unsigned char *)"\x88\x88" }, "\"iIg\"" },
    { { 2, (unsigned char *)"\x88\x99" }, "\"iJk\"" },
    { { 2, (unsigned char *)"\x88\xaa" }, "\"iKo\"" },
    { { 2, (unsigned char *)"\x88\xbb" }, "\"iLs\"" },
    { { 2, (unsigned char *)"\x88\xcc" }, "\"iMw\"" },
    { { 2, (unsigned char *)"\x88\xdd" }, "\"iN0\"" },
    { { 2, (unsigned char *)"\x88\xee" }, "\"iO4\"" },
    { { 2, (unsigned char *)"\x88\xff" }, "\"iP8\"" },
    { { 2, (unsigned char *)"\x99\x00" }, "\"mQA\"" },
    { { 2, (unsigned char *)"\x99\x11" }, "\"mRE\"" },
    { { 2, (unsigned char *)"\x99\x22" }, "\"mSI\"" },
    { { 2, (unsigned char *)"\x99\x33" }, "\"mTM\"" },
    { { 2, (unsigned char *)"\x99\x44" }, "\"mUQ\"" },
    { { 2, (unsigned char *)"\x99\x55" }, "\"mVU\"" },
    { { 2, (unsigned char *)"\x99\x66" }, "\"mWY\"" },
    { { 2, (unsigned char *)"\x99\x77" }, "\"mXc\"" },
    { { 2, (unsigned char *)"\x99\x88" }, "\"mYg\"" },
    { { 2, (unsigned char *)"\x99\x99" }, "\"mZk\"" },
    { { 2, (unsigned char *)"\x99\xaa" }, "\"mao\"" },
    { { 2, (unsigned char *)"\x99\xbb" }, "\"mbs\"" },
    { { 2, (unsigned char *)"\x99\xcc" }, "\"mcw\"" },
    { { 2, (unsigned char *)"\x99\xdd" }, "\"md0\"" },
    { { 2, (unsigned char *)"\x99\xee" }, "\"me4\"" },
    { { 2, (unsigned char *)"\x99\xff" }, "\"mf8\"" },
    { { 2, (unsigned char *)"\xaa\x00" }, "\"qgA\"" },
    { { 2, (unsigned char *)"\xaa\x11" }, "\"qhE\"" },
    { { 2, (unsigned char *)"\xaa\x22" }, "\"qiI\"" },
    { { 2, (unsigned char *)"\xaa\x33" }, "\"qjM\"" },
    { { 2, (unsigned char *)"\xaa\x44" }, "\"qkQ\"" },
    { { 2, (unsigned char *)"\xaa\x55" }, "\"qlU\"" },
    { { 2, (unsigned char *)"\xaa\x66" }, "\"qmY\"" },
    { { 2, (unsigned char *)"\xaa\x77" }, "\"qnc\"" },
    { { 2, (unsigned char *)"\xaa\x88" }, "\"qog\"" },
    { { 2, (unsigned char *)"\xaa\x99" }, "\"qpk\"" },
    { { 2, (unsigned char *)"\xaa\xaa" }, "\"qqo\"" },
    { { 2, (unsigned char *)"\xaa\xbb" }, "\"qrs\"" },
    { { 2, (unsigned char *)"\xaa\xcc" }, "\"qsw\"" },
    { { 2, (unsigned char *)"\xaa\xdd" }, "\"qt0\"" },
    { { 2, (unsigned char *)"\xaa\xee" }, "\"qu4\"" },
    { { 2, (unsigned char *)"\xaa\xff" }, "\"qv8\"" },
    { { 2, (unsigned char *)"\xbb\x00" }, "\"uwA\"" },
    { { 2, (unsigned char *)"\xbb\x11" }, "\"uxE\"" },
    { { 2, (unsigned char *)"\xbb\x22" }, "\"uyI\"" },
    { { 2, (unsigned char *)"\xbb\x33" }, "\"uzM\"" },
    { { 2, (unsigned char *)"\xbb\x44" }, "\"u0Q\"" },
    { { 2, (unsigned char *)"\xbb\x55" }, "\"u1U\"" },
    { { 2, (unsigned char *)"\xbb\x66" }, "\"u2Y\"" },
    { { 2, (unsigned char *)"\xbb\x77" }, "\"u3c\"" },
    { { 2, (unsigned char *)"\xbb\x88" }, "\"u4g\"" },
    { { 2, (unsigned char *)"\xbb\x99" }, "\"u5k\"" },
    { { 2, (unsigned char *)"\xbb\xaa" }, "\"u6o\"" },
    { { 2, (unsigned char *)"\xbb\xbb" }, "\"u7s\"" },
    { { 2, (unsigned char *)"\xbb\xcc" }, "\"u8w\"" },
    { { 2, (unsigned char *)"\xbb\xdd" }, "\"u90\"" },
    { { 2, (unsigned char *)"\xbb\xee" }, "\"u-4\"" },
    { { 2, (unsigned char *)"\xbb\xff" }, "\"u_8\"" },
    { { 2, (unsigned char *)"\xcc\x00" }, "\"zAA\"" },
    { { 2, (unsigned char *)"\xcc\x11" }, "\"zBE\"" },
    { { 2, (unsigned char *)"\xcc\x22" }, "\"zCI\"" },
    { { 2, (unsigned char *)"\xcc\x33" }, "\"zDM\"" },
    { { 2, (unsigned char *)"\xcc\x44" }, "\"zEQ\"" },
    { { 2, (unsigned char *)"\xcc\x55" }, "\"zFU\"" },
    { { 2, (unsigned char *)"\xcc\x66" }, "\"zGY\"" },
    { { 2, (unsigned char *)"\xcc\x77" }, "\"zHc\"" },
    { { 2, (unsigned char *)"\xcc\x88" }, "\"zIg\"" },
    { { 2, (unsigned char *)"\xcc\x99" }, "\"zJk\"" },
    { { 2, (unsigned char *)"\xcc\xaa" }, "\"zKo\"" },
    { { 2, (unsigned char *)"\xcc\xbb" }, "\"zLs\"" },
    { { 2, (unsigned char *)"\xcc\xcc" }, "\"zMw\"" },
    { { 2, (unsigned char *)"\xcc\xdd" }, "\"zN0\"" },
    { { 2, (unsigned char *)"\xcc\xee" }, "\"zO4\"" },
    { { 2, (unsigned char *)"\xcc\xff" }, "\"zP8\"" },
    { { 2, (unsigned char *)"\xdd\x00" }, "\"3QA\"" },
    { { 2, (unsigned char *)"\xdd\x11" }, "\"3RE\"" },
    { { 2, (unsigned char *)"\xdd\x22" }, "\"3SI\"" },
    { { 2, (unsigned char *)"\xdd\x33" }, "\"3TM\"" },
    { { 2, (unsigned char *)"\xdd\x44" }, "\"3UQ\"" },
    { { 2, (unsigned char *)"\xdd\x55" }, "\"3VU\"" },
    { { 2, (unsigned char *)"\xdd\x66" }, "\"3WY\"" },
    { { 2, (unsigned char *)"\xdd\x77" }, "\"3Xc\"" },
    { { 2, (unsigned char *)"\xdd\x88" }, "\"3Yg\"" },
    { { 2, (unsigned char *)"\xdd\x99" }, "\"3Zk\"" },
    { { 2, (unsigned char *)"\xdd\xaa" }, "\"3ao\"" },
    { { 2, (unsigned char *)"\xdd\xbb" }, "\"3bs\"" },
    { { 2, (unsigned char *)"\xdd\xcc" }, "\"3cw\"" },
    { { 2, (unsigned char *)"\xdd\xdd" }, "\"3d0\"" },
    { { 2, (unsigned char *)"\xdd\xee" }, "\"3e4\"" },
    { { 2, (unsigned char *)"\xdd\xff" }, "\"3f8\"" },
    { { 2, (unsigned char *)"\xee\x00" }, "\"7gA\"" },
    { { 2, (unsigned char *)"\xee\x11" }, "\"7hE\"" },
    { { 2, (unsigned char *)"\xee\x22" }, "\"7iI\"" },
    { { 2, (unsigned char *)"\xee\x33" }, "\"7jM\"" },
    { { 2, (unsigned char *)"\xee\x44" }, "\"7kQ\"" },
    { { 2, (unsigned char *)"\xee\x55" }, "\"7lU\"" },
    { { 2, (unsigned char *)"\xee\x66" }, "\"7mY\"" },
    { { 2, (unsigned char *)"\xee\x77" }, "\"7nc\"" },
    { { 2, (unsigned char *)"\xee\x88" }, "\"7og\"" },
    { { 2, (unsigned char *)"\xee\x99" }, "\"7pk\"" },
    { { 2, (unsigned char *)"\xee\xaa" }, "\"7qo\"" },
    { { 2, (unsigned char *)"\xee\xbb" }, "\"7rs\"" },
    { { 2, (unsigned char *)"\xee\xcc" }, "\"7sw\"" },
    { { 2, (unsigned char *)"\xee\xdd" }, "\"7t0\"" },
    { { 2, (unsigned char *)"\xee\xee" }, "\"7u4\"" },
    { { 2, (unsigned char *)"\xee\xff" }, "\"7v8\"" },
    { { 2, (unsigned char *)"\xff\x00" }, "\"_wA\"" },
    { { 2, (unsigned char *)"\xff\x11" }, "\"_xE\"" },
    { { 2, (unsigned char *)"\xff\x22" }, "\"_yI\"" },
    { { 2, (unsigned char *)"\xff\x33" }, "\"_zM\"" },
    { { 2, (unsigned char *)"\xff\x44" }, "\"_0Q\"" },
    { { 2, (unsigned char *)"\xff\x55" }, "\"_1U\"" },
    { { 2, (unsigned char *)"\xff\x66" }, "\"_2Y\"" },
    { { 2, (unsigned char *)"\xff\x77" }, "\"_3c\"" },
    { { 2, (unsigned char *)"\xff\x88" }, "\"_4g\"" },
    { { 2, (unsigned char *)"\xff\x99" }, "\"_5k\"" },
    { { 2, (unsigned char *)"\xff\xaa" }, "\"_6o\"" },
    { { 2, (unsigned char *)"\xff\xbb" }, "\"_7s\"" },
    { { 2, (unsigned char *)"\xff\xcc" }, "\"_8w\"" },
    { { 2, (unsigned char *)"\xff\xdd" }, "\"_90\"" },
    { { 2, (unsigned char *)"\xff\xee" }, "\"_-4\"" },
    { { 2, (unsigned char *)"\xff\xff" }, "\"__8\"" },
    { { 3, (unsigned char *)"\x00\x00\x00" }, "\"AAAA\"" },
    { { 3, (unsigned char *)"\x00\x00\x2f" }, "\"AAAv\"" },
    { { 3, (unsigned char *)"\x00\x00\x5e" }, "\"AABe\"" },
    { { 3, (unsigned char *)"\x00\x00\x8d" }, "\"AACN\"" },
    { { 3, (unsigned char *)"\x00\x00\xbc" }, "\"AAC8\"" },
    { { 3, (unsigned char *)"\x00\x00\xeb" }, "\"AADr\"" },
    { { 3, (unsigned char *)"\x00\x2f\x00" }, "\"AC8A\"" },
    { { 3, (unsigned char *)"\x00\x2f\x2f" }, "\"AC8v\"" },
    { { 3, (unsigned char *)"\x00\x2f\x5e" }, "\"AC9e\"" },
    { { 3, (unsigned char *)"\x00\x2f\x8d" }, "\"AC-N\"" },
    { { 3, (unsigned char *)"\x00\x2f\xbc" }, "\"AC-8\"" },
    { { 3, (unsigned char *)"\x00\x2f\xeb" }, "\"AC_r\"" },
    { { 3, (unsigned char *)"\x00\x5e\x00" }, "\"AF4A\"" },
    { { 3, (unsigned char *)"\x00\x5e\x2f" }, "\"AF4v\"" },
    { { 3, (unsigned char *)"\x00\x5e\x5e" }, "\"AF5e\"" },
    { { 3, (unsigned char *)"\x00\x5e\x8d" }, "\"AF6N\"" },
    { { 3, (unsigned char *)"\x00\x5e\xbc" }, "\"AF68\"" },
    { { 3, (unsigned char *)"\x00\x5e\xeb" }, "\"AF7r\"" },
    { { 3, (unsigned char *)"\x00\x8d\x00" }, "\"AI0A\"" },
    { { 3, (unsigned char *)"\x00\x8d\x2f" }, "\"AI0v\"" },
    { { 3, (unsigned char *)"\x00\x8d\x5e" }, "\"AI1e\"" },
    { { 3, (unsigned char *)"\x00\x8d\x8d" }, "\"AI2N\"" },
    { { 3, (unsigned char *)"\x00\x8d\xbc" }, "\"AI28\"" },
    { { 3, (unsigned char *)"\x00\x8d\xeb" }, "\"AI3r\"" },
    { { 3, (unsigned char *)"\x00\xbc\x00" }, "\"ALwA\"" },
    { { 3, (unsigned char *)"\x00\xbc\x2f" }, "\"ALwv\"" },
    { { 3, (unsigned char *)"\x00\xbc\x5e" }, "\"ALxe\"" },
    { { 3, (unsigned char *)"\x00\xbc\x8d" }, "\"ALyN\"" },
    { { 3, (unsigned char *)"\x00\xbc\xbc" }, "\"ALy8\"" },
    { { 3, (unsigned char *)"\x00\xbc\xeb" }, "\"ALzr\"" },
    { { 3, (unsigned char *)"\x00\xeb\x00" }, "\"AOsA\"" },
    { { 3, (unsigned char *)"\x00\xeb\x2f" }, "\"AOsv\"" },
    { { 3, (unsigned char *)"\x00\xeb\x5e" }, "\"AOte\"" },
    { { 3, (unsigned char *)"\x00\xeb\x8d" }, "\"AOuN\"" },
    { { 3, (unsigned char *)"\x00\xeb\xbc" }, "\"AOu8\"" },
    { { 3, (unsigned char *)"\x00\xeb\xeb" }, "\"AOvr\"" },
    { { 3, (unsigned char *)"\x2f\x00\x00" }, "\"LwAA\"" },
    { { 3, (unsigned char *)"\x2f\x00\x2f" }, "\"LwAv\"" },
    { { 3, (unsigned char *)"\x2f\x00\x5e" }, "\"LwBe\"" },
    { { 3, (unsigned char *)"\x2f\x00\x8d" }, "\"LwCN\"" },
    { { 3, (unsigned char *)"\x2f\x00\xbc" }, "\"LwC8\"" },
    { { 3, (unsigned char *)"\x2f\x00\xeb" }, "\"LwDr\"" },
    { { 3, (unsigned char *)"\x2f\x2f\x00" }, "\"Ly8A\"" },
    { { 3, (unsigned char *)"\x2f\x2f\x2f" }, "\"Ly8v\"" },
    { { 3, (unsigned char *)"\x2f\x2f\x5e" }, "\"Ly9e\"" },
    { { 3, (unsigned char *)"\x2f\x2f\x8d" }, "\"Ly-N\"" },
    { { 3, (unsigned char *)"\x2f\x2f\xbc" }, "\"Ly-8\"" },
    { { 3, (unsigned char *)"\x2f\x2f\xeb" }, "\"Ly_r\"" },
    { { 3, (unsigned char *)"\x2f\x5e\x00" }, "\"L14A\"" },
    { { 3, (unsigned char *)"\x2f\x5e\x2f" }, "\"L14v\"" },
    { { 3, (unsigned char *)"\x2f\x5e\x5e" }, "\"L15e\"" },
    { { 3, (unsigned char *)"\x2f\x5e\x8d" }, "\"L16N\"" },
    { { 3, (unsigned char *)"\x2f\x5e\xbc" }, "\"L168\"" },
    { { 3, (unsigned char *)"\x2f\x5e\xeb" }, "\"L17r\"" },
    { { 3, (unsigned char *)"\x2f\x8d\x00" }, "\"L40A\"" },
    { { 3, (unsigned char *)"\x2f\x8d\x2f" }, "\"L40v\"" },
    { { 3, (unsigned char *)"\x2f\x8d\x5e" }, "\"L41e\"" },
    { { 3, (unsigned char *)"\x2f\x8d\x8d" }, "\"L42N\"" },
    { { 3, (unsigned char *)"\x2f\x8d\xbc" }, "\"L428\"" },
    { { 3, (unsigned char *)"\x2f\x8d\xeb" }, "\"L43r\"" },
    { { 3, (unsigned char *)"\x2f\xbc\x00" }, "\"L7wA\"" },
    { { 3, (unsigned char *)"\x2f\xbc\x2f" }, "\"L7wv\"" },
    { { 3, (unsigned char *)"\x2f\xbc\x5e" }, "\"L7xe\"" },
    { { 3, (unsigned char *)"\x2f\xbc\x8d" }, "\"L7yN\"" },
    { { 3, (unsigned char *)"\x2f\xbc\xbc" }, "\"L7y8\"" },
    { { 3, (unsigned char *)"\x2f\xbc\xeb" }, "\"L7zr\"" },
    { { 3, (unsigned char *)"\x2f\xeb\x00" }, "\"L-sA\"" },
    { { 3, (unsigned char *)"\x2f\xeb\x2f" }, "\"L-sv\"" },
    { { 3, (unsigned char *)"\x2f\xeb\x5e" }, "\"L-te\"" },
    { { 3, (unsigned char *)"\x2f\xeb\x8d" }, "\"L-uN\"" },
    { { 3, (unsigned char *)"\x2f\xeb\xbc" }, "\"L-u8\"" },
    { { 3, (unsigned char *)"\x2f\xeb\xeb" }, "\"L-vr\"" },
    { { 3, (unsigned char *)"\x5e\x00\x00" }, "\"XgAA\"" },
    { { 3, (unsigned char *)"\x5e\x00\x2f" }, "\"XgAv\"" },
    { { 3, (unsigned char *)"\x5e\x00\x5e" }, "\"XgBe\"" },
    { { 3, (unsigned char *)"\x5e\x00\x8d" }, "\"XgCN\"" },
    { { 3, (unsigned char *)"\x5e\x00\xbc" }, "\"XgC8\"" },
    { { 3, (unsigned char *)"\x5e\x00\xeb" }, "\"XgDr\"" },
    { { 3, (unsigned char *)"\x5e\x2f\x00" }, "\"Xi8A\"" },
    { { 3, (unsigned char *)"\x5e\x2f\x2f" }, "\"Xi8v\"" },
    { { 3, (unsigned char *)"\x5e\x2f\x5e" }, "\"Xi9e\"" },
    { { 3, (unsigned char *)"\x5e\x2f\x8d" }, "\"Xi-N\"" },
    { { 3, (unsigned char *)"\x5e\x2f\xbc" }, "\"Xi-8\"" },
    { { 3, (unsigned char *)"\x5e\x2f\xeb" }, "\"Xi_r\"" },
    { { 3, (unsigned char *)"\x5e\x5e\x00" }, "\"Xl4A\"" },
    { { 3, (unsigned char *)"\x5e\x5e\x2f" }, "\"Xl4v\"" },
    { { 3, (unsigned char *)"\x5e\x5e\x5e" }, "\"Xl5e\"" },
    { { 3, (unsigned char *)"\x5e\x5e\x8d" }, "\"Xl6N\"" },
    { { 3, (unsigned char *)"\x5e\x5e\xbc" }, "\"Xl68\"" },
    { { 3, (unsigned char *)"\x5e\x5e\xeb" }, "\"Xl7r\"" },
    { { 3, (unsigned char *)"\x5e\x8d\x00" }, "\"Xo0A\"" },
    { { 3, (unsigned char *)"\x5e\x8d\x2f" }, "\"Xo0v\"" },
    { { 3, (unsigned char *)"\x5e\x8d\x5e" }, "\"Xo1e\"" },
    { { 3, (unsigned char *)"\x5e\x8d\x8d" }, "\"Xo2N\"" },
    { { 3, (unsigned char *)"\x5e\x8d\xbc" }, "\"Xo28\"" },
    { { 3, (unsigned char *)"\x5e\x8d\xeb" }, "\"Xo3r\"" },
    { { 3, (unsigned char *)"\x5e\xbc\x00" }, "\"XrwA\"" },
    { { 3, (unsigned char *)"\x5e\xbc\x2f" }, "\"Xrwv\"" },
    { { 3, (unsigned char *)"\x5e\xbc\x5e" }, "\"Xrxe\"" },
    { { 3, (unsigned char *)"\x5e\xbc\x8d" }, "\"XryN\"" },
    { { 3, (unsigned char *)"\x5e\xbc\xbc" }, "\"Xry8\"" },
    { { 3, (unsigned char *)"\x5e\xbc\xeb" }, "\"Xrzr\"" },
    { { 3, (unsigned char *)"\x5e\xeb\x00" }, "\"XusA\"" },
    { { 3, (unsigned char *)"\x5e\xeb\x2f" }, "\"Xusv\"" },
    { { 3, (unsigned char *)"\x5e\xeb\x5e" }, "\"Xute\"" },
    { { 3, (unsigned char *)"\x5e\xeb\x8d" }, "\"XuuN\"" },
    { { 3, (unsigned char *)"\x5e\xeb\xbc" }, "\"Xuu8\"" },
    { { 3, (unsigned char *)"\x5e\xeb\xeb" }, "\"Xuvr\"" },
    { { 3, (unsigned char *)"\x8d\x00\x00" }, "\"jQAA\"" },
    { { 3, (unsigned char *)"\x8d\x00\x2f" }, "\"jQAv\"" },
    { { 3, (unsigned char *)"\x8d\x00\x5e" }, "\"jQBe\"" },
    { { 3, (unsigned char *)"\x8d\x00\x8d" }, "\"jQCN\"" },
    { { 3, (unsigned char *)"\x8d\x00\xbc" }, "\"jQC8\"" },
    { { 3, (unsigned char *)"\x8d\x00\xeb" }, "\"jQDr\"" },
    { { 3, (unsigned char *)"\x8d\x2f\x00" }, "\"jS8A\"" },
    { { 3, (unsigned char *)"\x8d\x2f\x2f" }, "\"jS8v\"" },
    { { 3, (unsigned char *)"\x8d\x2f\x5e" }, "\"jS9e\"" },
    { { 3, (unsigned char *)"\x8d\x2f\x8d" }, "\"jS-N\"" },
    { { 3, (unsigned char *)"\x8d\x2f\xbc" }, "\"jS-8\"" },
    { { 3, (unsigned char *)"\x8d\x2f\xeb" }, "\"jS_r\"" },
    { { 3, (unsigned char *)"\x8d\x5e\x00" }, "\"jV4A\"" },
    { { 3, (unsigned char *)"\x8d\x5e\x2f" }, "\"jV4v\"" },
    { { 3, (unsigned char *)"\x8d\x5e\x5e" }, "\"jV5e\"" },
    { { 3, (unsigned char *)"\x8d\x5e\x8d" }, "\"jV6N\"" },
    { { 3, (unsigned char *)"\x8d\x5e\xbc" }, "\"jV68\"" },
    { { 3, (unsigned char *)"\x8d\x5e\xeb" }, "\"jV7r\"" },
    { { 3, (unsigned char *)"\x8d\x8d\x00" }, "\"jY0A\"" },
    { { 3, (unsigned char *)"\x8d\x8d\x2f" }, "\"jY0v\"" },
    { { 3, (unsigned char *)"\x8d\x8d\x5e" }, "\"jY1e\"" },
    { { 3, (unsigned char *)"\x8d\x8d\x8d" }, "\"jY2N\"" },
    { { 3, (unsigned char *)"\x8d\x8d\xbc" }, "\"jY28\"" },
    { { 3, (unsigned char *)"\x8d\x8d\xeb" }, "\"jY3r\"" },
    { { 3, (unsigned char *)"\x8d\xbc\x00" }, "\"jbwA\"" },
    { { 3, (unsigned char *)"\x8d\xbc\x2f" }, "\"jbwv\"" },
    { { 3, (unsigned char *)"\x8d\xbc\x5e" }, "\"jbxe\"" },
    { { 3, (unsigned char *)"\x8d\xbc\x8d" }, "\"jbyN\"" },
    { { 3, (unsigned char *)"\x8d\xbc\xbc" }, "\"jby8\"" },
    { { 3, (unsigned char *)"\x8d\xbc\xeb" }, "\"jbzr\"" },
    { { 3, (unsigned char *)"\x8d\xeb\x00" }, "\"jesA\"" },
    { { 3, (unsigned char *)"\x8d\xeb\x2f" }, "\"jesv\"" },
    { { 3, (unsigned char *)"\x8d\xeb\x5e" }, "\"jete\"" },
    { { 3, (unsigned char *)"\x8d\xeb\x8d" }, "\"jeuN\"" },
    { { 3, (unsigned char *)"\x8d\xeb\xbc" }, "\"jeu8\"" },
    { { 3, (unsigned char *)"\x8d\xeb\xeb" }, "\"jevr\"" },
    { { 3, (unsigned char *)"\xbc\x00\x00" }, "\"vAAA\"" },
    { { 3, (unsigned char *)"\xbc\x00\x2f" }, "\"vAAv\"" },
    { { 3, (unsigned char *)"\xbc\x00\x5e" }, "\"vABe\"" },
    { { 3, (unsigned char *)"\xbc\x00\x8d" }, "\"vACN\"" },
    { { 3, (unsigned char *)"\xbc\x00\xbc" }, "\"vAC8\"" },
    { { 3, (unsigned char *)"\xbc\x00\xeb" }, "\"vADr\"" },
    { { 3, (unsigned char *)"\xbc\x2f\x00" }, "\"vC8A\"" },
    { { 3, (unsigned char *)"\xbc\x2f\x2f" }, "\"vC8v\"" },
    { { 3, (unsigned char *)"\xbc\x2f\x5e" }, "\"vC9e\"" },
    { { 3, (unsigned char *)"\xbc\x2f\x8d" }, "\"vC-N\"" },
    { { 3, (unsigned char *)"\xbc\x2f\xbc" }, "\"vC-8\"" },
    { { 3, (unsigned char *)"\xbc\x2f\xeb" }, "\"vC_r\"" },
    { { 3, (unsigned char *)"\xbc\x5e\x00" }, "\"vF4A\"" },
    { { 3, (unsigned char *)"\xbc\x5e\x2f" }, "\"vF4v\"" },
    { { 3, (unsigned char *)"\xbc\x5e\x5e" }, "\"vF5e\"" },
    { { 3, (unsigned char *)"\xbc\x5e\x8d" }, "\"vF6N\"" },
    { { 3, (unsigned char *)"\xbc\x5e\xbc" }, "\"vF68\"" },
    { { 3, (unsigned char *)"\xbc\x5e\xeb" }, "\"vF7r\"" },
    { { 3, (unsigned char *)"\xbc\x8d\x00" }, "\"vI0A\"" },
    { { 3, (unsigned char *)"\xbc\x8d\x2f" }, "\"vI0v\"" },
    { { 3, (unsigned char *)"\xbc\x8d\x5e" }, "\"vI1e\"" },
    { { 3, (unsigned char *)"\xbc\x8d\x8d" }, "\"vI2N\"" },
    { { 3, (unsigned char *)"\xbc\x8d\xbc" }, "\"vI28\"" },
    { { 3, (unsigned char *)"\xbc\x8d\xeb" }, "\"vI3r\"" },
    { { 3, (unsigned char *)"\xbc\xbc\x00" }, "\"vLwA\"" },
    { { 3, (unsigned char *)"\xbc\xbc\x2f" }, "\"vLwv\"" },
    { { 3, (unsigned char *)"\xbc\xbc\x5e" }, "\"vLxe\"" },
    { { 3, (unsigned char *)"\xbc\xbc\x8d" }, "\"vLyN\"" },
    { { 3, (unsigned char *)"\xbc\xbc\xbc" }, "\"vLy8\"" },
    { { 3, (unsigned char *)"\xbc\xbc\xeb" }, "\"vLzr\"" },
    { { 3, (unsigned char *)"\xbc\xeb\x00" }, "\"vOsA\"" },
    { { 3, (unsigned char *)"\xbc\xeb\x2f" }, "\"vOsv\"" },
    { { 3, (unsigned char *)"\xbc\xeb\x5e" }, "\"vOte\"" },
    { { 3, (unsigned char *)"\xbc\xeb\x8d" }, "\"vOuN\"" },
    { { 3, (unsigned char *)"\xbc\xeb\xbc" }, "\"vOu8\"" },
    { { 3, (unsigned char *)"\xbc\xeb\xeb" }, "\"vOvr\"" },
    { { 3, (unsigned char *)"\xeb\x00\x00" }, "\"6wAA\"" },
    { { 3, (unsigned char *)"\xeb\x00\x2f" }, "\"6wAv\"" },
    { { 3, (unsigned char *)"\xeb\x00\x5e" }, "\"6wBe\"" },
    { { 3, (unsigned char *)"\xeb\x00\x8d" }, "\"6wCN\"" },
    { { 3, (unsigned char *)"\xeb\x00\xbc" }, "\"6wC8\"" },
    { { 3, (unsigned char *)"\xeb\x00\xeb" }, "\"6wDr\"" },
    { { 3, (unsigned char *)"\xeb\x2f\x00" }, "\"6y8A\"" },
    { { 3, (unsigned char *)"\xeb\x2f\x2f" }, "\"6y8v\"" },
    { { 3, (unsigned char *)"\xeb\x2f\x5e" }, "\"6y9e\"" },
    { { 3, (unsigned char *)"\xeb\x2f\x8d" }, "\"6y-N\"" },
    { { 3, (unsigned char *)"\xeb\x2f\xbc" }, "\"6y-8\"" },
    { { 3, (unsigned char *)"\xeb\x2f\xeb" }, "\"6y_r\"" },
    { { 3, (unsigned char *)"\xeb\x5e\x00" }, "\"614A\"" },
    { { 3, (unsigned char *)"\xeb\x5e\x2f" }, "\"614v\"" },
    { { 3, (unsigned char *)"\xeb\x5e\x5e" }, "\"615e\"" },
    { { 3, (unsigned char *)"\xeb\x5e\x8d" }, "\"616N\"" },
    { { 3, (unsigned char *)"\xeb\x5e\xbc" }, "\"6168\"" },
    { { 3, (unsigned char *)"\xeb\x5e\xeb" }, "\"617r\"" },
    { { 3, (unsigned char *)"\xeb\x8d\x00" }, "\"640A\"" },
    { { 3, (unsigned char *)"\xeb\x8d\x2f" }, "\"640v\"" },
    { { 3, (unsigned char *)"\xeb\x8d\x5e" }, "\"641e\"" },
    { { 3, (unsigned char *)"\xeb\x8d\x8d" }, "\"642N\"" },
    { { 3, (unsigned char *)"\xeb\x8d\xbc" }, "\"6428\"" },
    { { 3, (unsigned char *)"\xeb\x8d\xeb" }, "\"643r\"" },
    { { 3, (unsigned char *)"\xeb\xbc\x00" }, "\"67wA\"" },
    { { 3, (unsigned char *)"\xeb\xbc\x2f" }, "\"67wv\"" },
    { { 3, (unsigned char *)"\xeb\xbc\x5e" }, "\"67xe\"" },
    { { 3, (unsigned char *)"\xeb\xbc\x8d" }, "\"67yN\"" },
    { { 3, (unsigned char *)"\xeb\xbc\xbc" }, "\"67y8\"" },
    { { 3, (unsigned char *)"\xeb\xbc\xeb" }, "\"67zr\"" },
    { { 3, (unsigned char *)"\xeb\xeb\x00" }, "\"6-sA\"" },
    { { 3, (unsigned char *)"\xeb\xeb\x2f" }, "\"6-sv\"" },
    { { 3, (unsigned char *)"\xeb\xeb\x5e" }, "\"6-te\"" },
    { { 3, (unsigned char *)"\xeb\xeb\x8d" }, "\"6-uN\"" },
    { { 3, (unsigned char *)"\xeb\xeb\xbc" }, "\"6-u8\"" },
    { { 3, (unsigned char *)"\xeb\xeb\xeb" }, "\"6-vr\"" },
    { { 4, (unsigned char *)"\x00\x00\x00\x00" }, "\"AAAAAA\"" },
    { { 4, (unsigned char *)"\x00\x00\x00\x55" }, "\"AAAAVQ\"" },
    { { 4, (unsigned char *)"\x00\x00\x00\xaa" }, "\"AAAAqg\"" },
    { { 4, (unsigned char *)"\x00\x00\x00\xff" }, "\"AAAA_w\"" },
    { { 4, (unsigned char *)"\x00\x00\x55\x00" }, "\"AABVAA\"" },
    { { 4, (unsigned char *)"\x00\x00\x55\x55" }, "\"AABVVQ\"" },
    { { 4, (unsigned char *)"\x00\x00\x55\xaa" }, "\"AABVqg\"" },
    { { 4, (unsigned char *)"\x00\x00\x55\xff" }, "\"AABV_w\"" },
    { { 4, (unsigned char *)"\x00\x00\xaa\x00" }, "\"AACqAA\"" },
    { { 4, (unsigned char *)"\x00\x00\xaa\x55" }, "\"AACqVQ\"" },
    { { 4, (unsigned char *)"\x00\x00\xaa\xaa" }, "\"AACqqg\"" },
    { { 4, (unsigned char *)"\x00\x00\xaa\xff" }, "\"AACq_w\"" },
    { { 4, (unsigned char *)"\x00\x00\xff\x00" }, "\"AAD_AA\"" },
    { { 4, (unsigned char *)"\x00\x00\xff\x55" }, "\"AAD_VQ\"" },
    { { 4, (unsigned char *)"\x00\x00\xff\xaa" }, "\"AAD_qg\"" },
    { { 4, (unsigned char *)"\x00\x00\xff\xff" }, "\"AAD__w\"" },
    { { 4, (unsigned char *)"\x00\x55\x00\x00" }, "\"AFUAAA\"" },
    { { 4, (unsigned char *)"\x00\x55\x00\x55" }, "\"AFUAVQ\"" },
    { { 4, (unsigned char *)"\x00\x55\x00\xaa" }, "\"AFUAqg\"" },
    { { 4, (unsigned char *)"\x00\x55\x00\xff" }, "\"AFUA_w\"" },
    { { 4, (unsigned char *)"\x00\x55\x55\x00" }, "\"AFVVAA\"" },
    { { 4, (unsigned char *)"\x00\x55\x55\x55" }, "\"AFVVVQ\"" },
    { { 4, (unsigned char *)"\x00\x55\x55\xaa" }, "\"AFVVqg\"" },
    { { 4, (unsigned char *)"\x00\x55\x55\xff" }, "\"AFVV_w\"" },
    { { 4, (unsigned char *)"\x00\x55\xaa\x00" }, "\"AFWqAA\"" },
    { { 4, (unsigned char *)"\x00\x55\xaa\x55" }, "\"AFWqVQ\"" },
    { { 4, (unsigned char *)"\x00\x55\xaa\xaa" }, "\"AFWqqg\"" },
    { { 4, (unsigned char *)"\x00\x55\xaa\xff" }, "\"AFWq_w\"" },
    { { 4, (unsigned char *)"\x00\x55\xff\x00" }, "\"AFX_AA\"" },
    { { 4, (unsigned char *)"\x00\x55\xff\x55" }, "\"AFX_VQ\"" },
    { { 4, (unsigned char *)"\x00\x55\xff\xaa" }, "\"AFX_qg\"" },
    { { 4, (unsigned char *)"\x00\x55\xff\xff" }, "\"AFX__w\"" },
    { { 4, (unsigned char *)"\x00\xaa\x00\x00" }, "\"AKoAAA\"" },
    { { 4, (unsigned char *)"\x00\xaa\x00\x55" }, "\"AKoAVQ\"" },
    { { 4, (unsigned char *)"\x00\xaa\x00\xaa" }, "\"AKoAqg\"" },
    { { 4, (unsigned char *)"\x00\xaa\x00\xff" }, "\"AKoA_w\"" },
    { { 4, (unsigned char *)"\x00\xaa\x55\x00" }, "\"AKpVAA\"" },
    { { 4, (unsigned char *)"\x00\xaa\x55\x55" }, "\"AKpVVQ\"" },
    { { 4, (unsigned char *)"\x00\xaa\x55\xaa" }, "\"AKpVqg\"" },
    { { 4, (unsigned char *)"\x00\xaa\x55\xff" }, "\"AKpV_w\"" },
    { { 4, (unsigned char *)"\x00\xaa\xaa\x00" }, "\"AKqqAA\"" },
    { { 4, (unsigned char *)"\x00\xaa\xaa\x55" }, "\"AKqqVQ\"" },
    { { 4, (unsigned char *)"\x00\xaa\xaa\xaa" }, "\"AKqqqg\"" },
    { { 4, (unsigned char *)"\x00\xaa\xaa\xff" }, "\"AKqq_w\"" },
    { { 4, (unsigned char *)"\x00\xaa\xff\x00" }, "\"AKr_AA\"" },
    { { 4, (unsigned char *)"\x00\xaa\xff\x55" }, "\"AKr_VQ\"" },
    { { 4, (unsigned char *)"\x00\xaa\xff\xaa" }, "\"AKr_qg\"" },
    { { 4, (unsigned char *)"\x00\xaa\xff\xff" }, "\"AKr__w\"" },
    { { 4, (unsigned char *)"\x00\xff\x00\x00" }, "\"AP8AAA\"" },
    { { 4, (unsigned char *)"\x00\xff\x00\x55" }, "\"AP8AVQ\"" },
    { { 4, (unsigned char *)"\x00\xff\x00\xaa" }, "\"AP8Aqg\"" },
    { { 4, (unsigned char *)"\x00\xff\x00\xff" }, "\"AP8A_w\"" },
    { { 4, (unsigned char *)"\x00\xff\x55\x00" }, "\"AP9VAA\"" },
    { { 4, (unsigned char *)"\x00\xff\x55\x55" }, "\"AP9VVQ\"" },
    { { 4, (unsigned char *)"\x00\xff\x55\xaa" }, "\"AP9Vqg\"" },
    { { 4, (unsigned char *)"\x00\xff\x55\xff" }, "\"AP9V_w\"" },
    { { 4, (unsigned char *)"\x00\xff\xaa\x00" }, "\"AP-qAA\"" },
    { { 4, (unsigned char *)"\x00\xff\xaa\x55" }, "\"AP-qVQ\"" },
    { { 4, (unsigned char *)"\x00\xff\xaa\xaa" }, "\"AP-qqg\"" },
    { { 4, (unsigned char *)"\x00\xff\xaa\xff" }, "\"AP-q_w\"" },
    { { 4, (unsigned char *)"\x00\xff\xff\x00" }, "\"AP__AA\"" },
    { { 4, (unsigned char *)"\x00\xff\xff\x55" }, "\"AP__VQ\"" },
    { { 4, (unsigned char *)"\x00\xff\xff\xaa" }, "\"AP__qg\"" },
    { { 4, (unsigned char *)"\x00\xff\xff\xff" }, "\"AP___w\"" },
    { { 4, (unsigned char *)"\x55\x00\x00\x00" }, "\"VQAAAA\"" },
    { { 4, (unsigned char *)"\x55\x00\x00\x55" }, "\"VQAAVQ\"" },
    { { 4, (unsigned char *)"\x55\x00\x00\xaa" }, "\"VQAAqg\"" },
    { { 4, (unsigned char *)"\x55\x00\x00\xff" }, "\"VQAA_w\"" },
    { { 4, (unsigned char *)"\x55\x00\x55\x00" }, "\"VQBVAA\"" },
    { { 4, (unsigned char *)"\x55\x00\x55\x55" }, "\"VQBVVQ\"" },
    { { 4, (unsigned char *)"\x55\x00\x55\xaa" }, "\"VQBVqg\"" },
    { { 4, (unsigned char *)"\x55\x00\x55\xff" }, "\"VQBV_w\"" },
    { { 4, (unsigned char *)"\x55\x00\xaa\x00" }, "\"VQCqAA\"" },
    { { 4, (unsigned char *)"\x55\x00\xaa\x55" }, "\"VQCqVQ\"" },
    { { 4, (unsigned char *)"\x55\x00\xaa\xaa" }, "\"VQCqqg\"" },
    { { 4, (unsigned char *)"\x55\x00\xaa\xff" }, "\"VQCq_w\"" },
    { { 4, (unsigned char *)"\x55\x00\xff\x00" }, "\"VQD_AA\"" },
    { { 4, (unsigned char *)"\x55\x00\xff\x55" }, "\"VQD_VQ\"" },
    { { 4, (unsigned char *)"\x55\x00\xff\xaa" }, "\"VQD_qg\"" },
    { { 4, (unsigned char *)"\x55\x00\xff\xff" }, "\"VQD__w\"" },
    { { 4, (unsigned char *)"\x55\x55\x00\x00" }, "\"VVUAAA\"" },
    { { 4, (unsigned char *)"\x55\x55\x00\x55" }, "\"VVUAVQ\"" },
    { { 4, (unsigned char *)"\x55\x55\x00\xaa" }, "\"VVUAqg\"" },
    { { 4, (unsigned char *)"\x55\x55\x00\xff" }, "\"VVUA_w\"" },
    { { 4, (unsigned char *)"\x55\x55\x55\x00" }, "\"VVVVAA\"" },
    { { 4, (unsigned char *)"\x55\x55\x55\x55" }, "\"VVVVVQ\"" },
    { { 4, (unsigned char *)"\x55\x55\x55\xaa" }, "\"VVVVqg\"" },
    { { 4, (unsigned char *)"\x55\x55\x55\xff" }, "\"VVVV_w\"" },
    { { 4, (unsigned char *)"\x55\x55\xaa\x00" }, "\"VVWqAA\"" },
    { { 4, (unsigned char *)"\x55\x55\xaa\x55" }, "\"VVWqVQ\"" },
    { { 4, (unsigned char *)"\x55\x55\xaa\xaa" }, "\"VVWqqg\"" },
    { { 4, (unsigned char *)"\x55\x55\xaa\xff" }, "\"VVWq_w\"" },
    { { 4, (unsigned char *)"\x55\x55\xff\x00" }, "\"VVX_AA\"" },
    { { 4, (unsigned char *)"\x55\x55\xff\x55" }, "\"VVX_VQ\"" },
    { { 4, (unsigned char *)"\x55\x55\xff\xaa" }, "\"VVX_qg\"" },
    { { 4, (unsigned char *)"\x55\x55\xff\xff" }, "\"VVX__w\"" },
    { { 4, (unsigned char *)"\x55\xaa\x00\x00" }, "\"VaoAAA\"" },
    { { 4, (unsigned char *)"\x55\xaa\x00\x55" }, "\"VaoAVQ\"" },
    { { 4, (unsigned char *)"\x55\xaa\x00\xaa" }, "\"VaoAqg\"" },
    { { 4, (unsigned char *)"\x55\xaa\x00\xff" }, "\"VaoA_w\"" },
    { { 4, (unsigned char *)"\x55\xaa\x55\x00" }, "\"VapVAA\"" },
    { { 4, (unsigned char *)"\x55\xaa\x55\x55" }, "\"VapVVQ\"" },
    { { 4, (unsigned char *)"\x55\xaa\x55\xaa" }, "\"VapVqg\"" },
    { { 4, (unsigned char *)"\x55\xaa\x55\xff" }, "\"VapV_w\"" },
    { { 4, (unsigned char *)"\x55\xaa\xaa\x00" }, "\"VaqqAA\"" },
    { { 4, (unsigned char *)"\x55\xaa\xaa\x55" }, "\"VaqqVQ\"" },
    { { 4, (unsigned char *)"\x55\xaa\xaa\xaa" }, "\"Vaqqqg\"" },
    { { 4, (unsigned char *)"\x55\xaa\xaa\xff" }, "\"Vaqq_w\"" },
    { { 4, (unsigned char *)"\x55\xaa\xff\x00" }, "\"Var_AA\"" },
    { { 4, (unsigned char *)"\x55\xaa\xff\x55" }, "\"Var_VQ\"" },
    { { 4, (unsigned char *)"\x55\xaa\xff\xaa" }, "\"Var_qg\"" },
    { { 4, (unsigned char *)"\x55\xaa\xff\xff" }, "\"Var__w\"" },
    { { 4, (unsigned char *)"\x55\xff\x00\x00" }, "\"Vf8AAA\"" },
    { { 4, (unsigned char *)"\x55\xff\x00\x55" }, "\"Vf8AVQ\"" },
    { { 4, (unsigned char *)"\x55\xff\x00\xaa" }, "\"Vf8Aqg\"" },
    { { 4, (unsigned char *)"\x55\xff\x00\xff" }, "\"Vf8A_w\"" },
    { { 4, (unsigned char *)"\x55\xff\x55\x00" }, "\"Vf9VAA\"" },
    { { 4, (unsigned char *)"\x55\xff\x55\x55" }, "\"Vf9VVQ\"" },
    { { 4, (unsigned char *)"\x55\xff\x55\xaa" }, "\"Vf9Vqg\"" },
    { { 4, (unsigned char *)"\x55\xff\x55\xff" }, "\"Vf9V_w\"" },
    { { 4, (unsigned char *)"\x55\xff\xaa\x00" }, "\"Vf-qAA\"" },
    { { 4, (unsigned char *)"\x55\xff\xaa\x55" }, "\"Vf-qVQ\"" },
    { { 4, (unsigned char *)"\x55\xff\xaa\xaa" }, "\"Vf-qqg\"" },
    { { 4, (unsigned char *)"\x55\xff\xaa\xff" }, "\"Vf-q_w\"" },
    { { 4, (unsigned char *)"\x55\xff\xff\x00" }, "\"Vf__AA\"" },
    { { 4, (unsigned char *)"\x55\xff\xff\x55" }, "\"Vf__VQ\"" },
    { { 4, (unsigned char *)"\x55\xff\xff\xaa" }, "\"Vf__qg\"" },
    { { 4, (unsigned char *)"\x55\xff\xff\xff" }, "\"Vf___w\"" },
    { { 4, (unsigned char *)"\xaa\x00\x00\x00" }, "\"qgAAAA\"" },
    { { 4, (unsigned char *)"\xaa\x00\x00\x55" }, "\"qgAAVQ\"" },
    { { 4, (unsigned char *)"\xaa\x00\x00\xaa" }, "\"qgAAqg\"" },
    { { 4, (unsigned char *)"\xaa\x00\x00\xff" }, "\"qgAA_w\"" },
    { { 4, (unsigned char *)"\xaa\x00\x55\x00" }, "\"qgBVAA\"" },
    { { 4, (unsigned char *)"\xaa\x00\x55\x55" }, "\"qgBVVQ\"" },
    { { 4, (unsigned char *)"\xaa\x00\x55\xaa" }, "\"qgBVqg\"" },
    { { 4, (unsigned char *)"\xaa\x00\x55\xff" }, "\"qgBV_w\"" },
    { { 4, (unsigned char *)"\xaa\x00\xaa\x00" }, "\"qgCqAA\"" },
    { { 4, (unsigned char *)"\xaa\x00\xaa\x55" }, "\"qgCqVQ\"" },
    { { 4, (unsigned char *)"\xaa\x00\xaa\xaa" }, "\"qgCqqg\"" },
    { { 4, (unsigned char *)"\xaa\x00\xaa\xff" }, "\"qgCq_w\"" },
    { { 4, (unsigned char *)"\xaa\x00\xff\x00" }, "\"qgD_AA\"" },
    { { 4, (unsigned char *)"\xaa\x00\xff\x55" }, "\"qgD_VQ\"" },
    { { 4, (unsigned char *)"\xaa\x00\xff\xaa" }, "\"qgD_qg\"" },
    { { 4, (unsigned char *)"\xaa\x00\xff\xff" }, "\"qgD__w\"" },
    { { 4, (unsigned char *)"\xaa\x55\x00\x00" }, "\"qlUAAA\"" },
    { { 4, (unsigned char *)"\xaa\x55\x00\x55" }, "\"qlUAVQ\"" },
    { { 4, (unsigned char *)"\xaa\x55\x00\xaa" }, "\"qlUAqg\"" },
    { { 4, (unsigned char *)"\xaa\x55\x00\xff" }, "\"qlUA_w\"" },
    { { 4, (unsigned char *)"\xaa\x55\x55\x00" }, "\"qlVVAA\"" },
    { { 4, (unsigned char *)"\xaa\x55\x55\x55" }, "\"qlVVVQ\"" },
    { { 4, (unsigned char *)"\xaa\x55\x55\xaa" }, "\"qlVVqg\"" },
    { { 4, (unsigned char *)"\xaa\x55\x55\xff" }, "\"qlVV_w\"" },
    { { 4, (unsigned char *)"\xaa\x55\xaa\x00" }, "\"qlWqAA\"" },
    { { 4, (unsigned char *)"\xaa\x55\xaa\x55" }, "\"qlWqVQ\"" },
    { { 4, (unsigned char *)"\xaa\x55\xaa\xaa" }, "\"qlWqqg\"" },
    { { 4, (unsigned char *)"\xaa\x55\xaa\xff" }, "\"qlWq_w\"" },
    { { 4, (unsigned char *)"\xaa\x55\xff\x00" }, "\"qlX_AA\"" },
    { { 4, (unsigned char *)"\xaa\x55\xff\x55" }, "\"qlX_VQ\"" },
    { { 4, (unsigned char *)"\xaa\x55\xff\xaa" }, "\"qlX_qg\"" },
    { { 4, (unsigned char *)"\xaa\x55\xff\xff" }, "\"qlX__w\"" },
    { { 4, (unsigned char *)"\xaa\xaa\x00\x00" }, "\"qqoAAA\"" },
    { { 4, (unsigned char *)"\xaa\xaa\x00\x55" }, "\"qqoAVQ\"" },
    { { 4, (unsigned char *)"\xaa\xaa\x00\xaa" }, "\"qqoAqg\"" },
    { { 4, (unsigned char *)"\xaa\xaa\x00\xff" }, "\"qqoA_w\"" },
    { { 4, (unsigned char *)"\xaa\xaa\x55\x00" }, "\"qqpVAA\"" },
    { { 4, (unsigned char *)"\xaa\xaa\x55\x55" }, "\"qqpVVQ\"" },
    { { 4, (unsigned char *)"\xaa\xaa\x55\xaa" }, "\"qqpVqg\"" },
    { { 4, (unsigned char *)"\xaa\xaa\x55\xff" }, "\"qqpV_w\"" },
    { { 4, (unsigned char *)"\xaa\xaa\xaa\x00" }, "\"qqqqAA\"" },
    { { 4, (unsigned char *)"\xaa\xaa\xaa\x55" }, "\"qqqqVQ\"" },
    { { 4, (unsigned char *)"\xaa\xaa\xaa\xaa" }, "\"qqqqqg\"" },
    { { 4, (unsigned char *)"\xaa\xaa\xaa\xff" }, "\"qqqq_w\"" },
    { { 4, (unsigned char *)"\xaa\xaa\xff\x00" }, "\"qqr_AA\"" },
    { { 4, (unsigned char *)"\xaa\xaa\xff\x55" }, "\"qqr_VQ\"" },
    { { 4, (unsigned char *)"\xaa\xaa\xff\xaa" }, "\"qqr_qg\"" },
    { { 4, (unsigned char *)"\xaa\xaa\xff\xff" }, "\"qqr__w\"" },
    { { 4, (unsigned char *)"\xaa\xff\x00\x00" }, "\"qv8AAA\"" },
    { { 4, (unsigned char *)"\xaa\xff\x00\x55" }, "\"qv8AVQ\"" },
    { { 4, (unsigned char *)"\xaa\xff\x00\xaa" }, "\"qv8Aqg\"" },
    { { 4, (unsigned char *)"\xaa\xff\x00\xff" }, "\"qv8A_w\"" },
    { { 4, (unsigned char *)"\xaa\xff\x55\x00" }, "\"qv9VAA\"" },
    { { 4, (unsigned char *)"\xaa\xff\x55\x55" }, "\"qv9VVQ\"" },
    { { 4, (unsigned char *)"\xaa\xff\x55\xaa" }, "\"qv9Vqg\"" },
    { { 4, (unsigned char *)"\xaa\xff\x55\xff" }, "\"qv9V_w\"" },
    { { 4, (unsigned char *)"\xaa\xff\xaa\x00" }, "\"qv-qAA\"" },
    { { 4, (unsigned char *)"\xaa\xff\xaa\x55" }, "\"qv-qVQ\"" },
    { { 4, (unsigned char *)"\xaa\xff\xaa\xaa" }, "\"qv-qqg\"" },
    { { 4, (unsigned char *)"\xaa\xff\xaa\xff" }, "\"qv-q_w\"" },
    { { 4, (unsigned char *)"\xaa\xff\xff\x00" }, "\"qv__AA\"" },
    { { 4, (unsigned char *)"\xaa\xff\xff\x55" }, "\"qv__VQ\"" },
    { { 4, (unsigned char *)"\xaa\xff\xff\xaa" }, "\"qv__qg\"" },
    { { 4, (unsigned char *)"\xaa\xff\xff\xff" }, "\"qv___w\"" },
    { { 4, (unsigned char *)"\xff\x00\x00\x00" }, "\"_wAAAA\"" },
    { { 4, (unsigned char *)"\xff\x00\x00\x55" }, "\"_wAAVQ\"" },
    { { 4, (unsigned char *)"\xff\x00\x00\xaa" }, "\"_wAAqg\"" },
    { { 4, (unsigned char *)"\xff\x00\x00\xff" }, "\"_wAA_w\"" },
    { { 4, (unsigned char *)"\xff\x00\x55\x00" }, "\"_wBVAA\"" },
    { { 4, (unsigned char *)"\xff\x00\x55\x55" }, "\"_wBVVQ\"" },
    { { 4, (unsigned char *)"\xff\x00\x55\xaa" }, "\"_wBVqg\"" },
    { { 4, (unsigned char *)"\xff\x00\x55\xff" }, "\"_wBV_w\"" },
    { { 4, (unsigned char *)"\xff\x00\xaa\x00" }, "\"_wCqAA\"" },
    { { 4, (unsigned char *)"\xff\x00\xaa\x55" }, "\"_wCqVQ\"" },
    { { 4, (unsigned char *)"\xff\x00\xaa\xaa" }, "\"_wCqqg\"" },
    { { 4, (unsigned char *)"\xff\x00\xaa\xff" }, "\"_wCq_w\"" },
    { { 4, (unsigned char *)"\xff\x00\xff\x00" }, "\"_wD_AA\"" },
    { { 4, (unsigned char *)"\xff\x00\xff\x55" }, "\"_wD_VQ\"" },
    { { 4, (unsigned char *)"\xff\x00\xff\xaa" }, "\"_wD_qg\"" },
    { { 4, (unsigned char *)"\xff\x00\xff\xff" }, "\"_wD__w\"" },
    { { 4, (unsigned char *)"\xff\x55\x00\x00" }, "\"_1UAAA\"" },
    { { 4, (unsigned char *)"\xff\x55\x00\x55" }, "\"_1UAVQ\"" },
    { { 4, (unsigned char *)"\xff\x55\x00\xaa" }, "\"_1UAqg\"" },
    { { 4, (unsigned char *)"\xff\x55\x00\xff" }, "\"_1UA_w\"" },
    { { 4, (unsigned char *)"\xff\x55\x55\x00" }, "\"_1VVAA\"" },
    { { 4, (unsigned char *)"\xff\x55\x55\x55" }, "\"_1VVVQ\"" },
    { { 4, (unsigned char *)"\xff\x55\x55\xaa" }, "\"_1VVqg\"" },
    { { 4, (unsigned char *)"\xff\x55\x55\xff" }, "\"_1VV_w\"" },
    { { 4, (unsigned char *)"\xff\x55\xaa\x00" }, "\"_1WqAA\"" },
    { { 4, (unsigned char *)"\xff\x55\xaa\x55" }, "\"_1WqVQ\"" },
    { { 4, (unsigned char *)"\xff\x55\xaa\xaa" }, "\"_1Wqqg\"" },
    { { 4, (unsigned char *)"\xff\x55\xaa\xff" }, "\"_1Wq_w\"" },
    { { 4, (unsigned char *)"\xff\x55\xff\x00" }, "\"_1X_AA\"" },
    { { 4, (unsigned char *)"\xff\x55\xff\x55" }, "\"_1X_VQ\"" },
    { { 4, (unsigned char *)"\xff\x55\xff\xaa" }, "\"_1X_qg\"" },
    { { 4, (unsigned char *)"\xff\x55\xff\xff" }, "\"_1X__w\"" },
    { { 4, (unsigned char *)"\xff\xaa\x00\x00" }, "\"_6oAAA\"" },
    { { 4, (unsigned char *)"\xff\xaa\x00\x55" }, "\"_6oAVQ\"" },
    { { 4, (unsigned char *)"\xff\xaa\x00\xaa" }, "\"_6oAqg\"" },
    { { 4, (unsigned char *)"\xff\xaa\x00\xff" }, "\"_6oA_w\"" },
    { { 4, (unsigned char *)"\xff\xaa\x55\x00" }, "\"_6pVAA\"" },
    { { 4, (unsigned char *)"\xff\xaa\x55\x55" }, "\"_6pVVQ\"" },
    { { 4, (unsigned char *)"\xff\xaa\x55\xaa" }, "\"_6pVqg\"" },
    { { 4, (unsigned char *)"\xff\xaa\x55\xff" }, "\"_6pV_w\"" },
    { { 4, (unsigned char *)"\xff\xaa\xaa\x00" }, "\"_6qqAA\"" },
    { { 4, (unsigned char *)"\xff\xaa\xaa\x55" }, "\"_6qqVQ\"" },
    { { 4, (unsigned char *)"\xff\xaa\xaa\xaa" }, "\"_6qqqg\"" },
    { { 4, (unsigned char *)"\xff\xaa\xaa\xff" }, "\"_6qq_w\"" },
    { { 4, (unsigned char *)"\xff\xaa\xff\x00" }, "\"_6r_AA\"" },
    { { 4, (unsigned char *)"\xff\xaa\xff\x55" }, "\"_6r_VQ\"" },
    { { 4, (unsigned char *)"\xff\xaa\xff\xaa" }, "\"_6r_qg\"" },
    { { 4, (unsigned char *)"\xff\xaa\xff\xff" }, "\"_6r__w\"" },
    { { 4, (unsigned char *)"\xff\xff\x00\x00" }, "\"__8AAA\"" },
    { { 4, (unsigned char *)"\xff\xff\x00\x55" }, "\"__8AVQ\"" },
    { { 4, (unsigned char *)"\xff\xff\x00\xaa" }, "\"__8Aqg\"" },
    { { 4, (unsigned char *)"\xff\xff\x00\xff" }, "\"__8A_w\"" },
    { { 4, (unsigned char *)"\xff\xff\x55\x00" }, "\"__9VAA\"" },
    { { 4, (unsigned char *)"\xff\xff\x55\x55" }, "\"__9VVQ\"" },
    { { 4, (unsigned char *)"\xff\xff\x55\xaa" }, "\"__9Vqg\"" },
    { { 4, (unsigned char *)"\xff\xff\x55\xff" }, "\"__9V_w\"" },
    { { 4, (unsigned char *)"\xff\xff\xaa\x00" }, "\"__-qAA\"" },
    { { 4, (unsigned char *)"\xff\xff\xaa\x55" }, "\"__-qVQ\"" },
    { { 4, (unsigned char *)"\xff\xff\xaa\xaa" }, "\"__-qqg\"" },
    { { 4, (unsigned char *)"\xff\xff\xaa\xff" }, "\"__-q_w\"" },
    { { 4, (unsigned char *)"\xff\xff\xff\x00" }, "\"____AA\"" },
    { { 4, (unsigned char *)"\xff\xff\xff\x55" }, "\"____VQ\"" },
    { { 4, (unsigned char *)"\xff\xff\xff\xaa" }, "\"____qg\"" },
    { { 4, (unsigned char *)"\xff\xff\xff\xff" }, "\"_____w\"" },
    { { 5, (unsigned char *)"\x00\x00\x00\x00\x00" }, "\"AAAAAAA\"" },
    { { 5, (unsigned char *)"\x00\x00\x00\x00\x7e" }, "\"AAAAAH4\"" },
    { { 5, (unsigned char *)"\x00\x00\x00\x00\xfc" }, "\"AAAAAPw\"" },
    { { 5, (unsigned char *)"\x00\x00\x00\x7e\x00" }, "\"AAAAfgA\"" },
    { { 5, (unsigned char *)"\x00\x00\x00\x7e\x7e" }, "\"AAAAfn4\"" },
    { { 5, (unsigned char *)"\x00\x00\x00\x7e\xfc" }, "\"AAAAfvw\"" },
    { { 5, (unsigned char *)"\x00\x00\x00\xfc\x00" }, "\"AAAA_AA\"" },
    { { 5, (unsigned char *)"\x00\x00\x00\xfc\x7e" }, "\"AAAA_H4\"" },
    { { 5, (unsigned char *)"\x00\x00\x00\xfc\xfc" }, "\"AAAA_Pw\"" },
    { { 5, (unsigned char *)"\x00\x00\x7e\x00\x00" }, "\"AAB-AAA\"" },
    { { 5, (unsigned char *)"\x00\x00\x7e\x00\x7e" }, "\"AAB-AH4\"" },
    { { 5, (unsigned char *)"\x00\x00\x7e\x00\xfc" }, "\"AAB-APw\"" },
    { { 5, (unsigned char *)"\x00\x00\x7e\x7e\x00" }, "\"AAB-fgA\"" },
    { { 5, (unsigned char *)"\x00\x00\x7e\x7e\x7e" }, "\"AAB-fn4\"" },
    { { 5, (unsigned char *)"\x00\x00\x7e\x7e\xfc" }, "\"AAB-fvw\"" },
    { { 5, (unsigned char *)"\x00\x00\x7e\xfc\x00" }, "\"AAB-_AA\"" },
    { { 5, (unsigned char *)"\x00\x00\x7e\xfc\x7e" }, "\"AAB-_H4\"" },
    { { 5, (unsigned char *)"\x00\x00\x7e\xfc\xfc" }, "\"AAB-_Pw\"" },
    { { 5, (unsigned char *)"\x00\x00\xfc\x00\x00" }, "\"AAD8AAA\"" },
    { { 5, (unsigned char *)"\x00\x00\xfc\x00\x7e" }, "\"AAD8AH4\"" },
    { { 5, (unsigned char *)"\x00\x00\xfc\x00\xfc" }, "\"AAD8APw\"" },
    { { 5, (unsigned char *)"\x00\x00\xfc\x7e\x00" }, "\"AAD8fgA\"" },
    { { 5, (unsigned char *)"\x00\x00\xfc\x7e\x7e" }, "\"AAD8fn4\"" },
    { { 5, (unsigned char *)"\x00\x00\xfc\x7e\xfc" }, "\"AAD8fvw\"" },
    { { 5, (unsigned char *)"\x00\x00\xfc\xfc\x00" }, "\"AAD8_AA\"" },
    { { 5, (unsigned char *)"\x00\x00\xfc\xfc\x7e" }, "\"AAD8_H4\"" },
    { { 5, (unsigned char *)"\x00\x00\xfc\xfc\xfc" }, "\"AAD8_Pw\"" },
    { { 5, (unsigned char *)"\x00\x7e\x00\x00\x00" }, "\"AH4AAAA\"" },
    { { 5, (unsigned char *)"\x00\x7e\x00\x00\x7e" }, "\"AH4AAH4\"" },
    { { 5, (unsigned char *)"\x00\x7e\x00\x00\xfc" }, "\"AH4AAPw\"" },
    { { 5, (unsigned char *)"\x00\x7e\x00\x7e\x00" }, "\"AH4AfgA\"" },
    { { 5, (unsigned char *)"\x00\x7e\x00\x7e\x7e" }, "\"AH4Afn4\"" },
    { { 5, (unsigned char *)"\x00\x7e\x00\x7e\xfc" }, "\"AH4Afvw\"" },
    { { 5, (unsigned char *)"\x00\x7e\x00\xfc\x00" }, "\"AH4A_AA\"" },
    { { 5, (unsigned char *)"\x00\x7e\x00\xfc\x7e" }, "\"AH4A_H4\"" },
    { { 5, (unsigned char *)"\x00\x7e\x00\xfc\xfc" }, "\"AH4A_Pw\"" },
    { { 5, (unsigned char *)"\x00\x7e\x7e\x00\x00" }, "\"AH5-AAA\"" },
    { { 5, (unsigned char *)"\x00\x7e\x7e\x00\x7e" }, "\"AH5-AH4\"" },
    { { 5, (unsigned char *)"\x00\x7e\x7e\x00\xfc" }, "\"AH5-APw\"" },
    { { 5, (unsigned char *)"\x00\x7e\x7e\x7e\x00" }, "\"AH5-fgA\"" },
    { { 5, (unsigned char *)"\x00\x7e\x7e\x7e\x7e" }, "\"AH5-fn4\"" },
    { { 5, (unsigned char *)"\x00\x7e\x7e\x7e\xfc" }, "\"AH5-fvw\"" },
    { { 5, (unsigned char *)"\x00\x7e\x7e\xfc\x00" }, "\"AH5-_AA\"" },
    { { 5, (unsigned char *)"\x00\x7e\x7e\xfc\x7e" }, "\"AH5-_H4\"" },
    { { 5, (unsigned char *)"\x00\x7e\x7e\xfc\xfc" }, "\"AH5-_Pw\"" },
    { { 5, (unsigned char *)"\x00\x7e\xfc\x00\x00" }, "\"AH78AAA\"" },
    { { 5, (unsigned char *)"\x00\x7e\xfc\x00\x7e" }, "\"AH78AH4\"" },
    { { 5, (unsigned char *)"\x00\x7e\xfc\x00\xfc" }, "\"AH78APw\"" },
    { { 5, (unsigned char *)"\x00\x7e\xfc\x7e\x00" }, "\"AH78fgA\"" },
    { { 5, (unsigned char *)"\x00\x7e\xfc\x7e\x7e" }, "\"AH78fn4\"" },
    { { 5, (unsigned char *)"\x00\x7e\xfc\x7e\xfc" }, "\"AH78fvw\"" },
    { { 5, (unsigned char *)"\x00\x7e\xfc\xfc\x00" }, "\"AH78_AA\"" },
    { { 5, (unsigned char *)"\x00\x7e\xfc\xfc\x7e" }, "\"AH78_H4\"" },
    { { 5, (unsigned char *)"\x00\x7e\xfc\xfc\xfc" }, "\"AH78_Pw\"" },
    { { 5, (unsigned char *)"\x00\xfc\x00\x00\x00" }, "\"APwAAAA\"" },
    { { 5, (unsigned char *)"\x00\xfc\x00\x00\x7e" }, "\"APwAAH4\"" },
    { { 5, (unsigned char *)"\x00\xfc\x00\x00\xfc" }, "\"APwAAPw\"" },
    { { 5, (unsigned char *)"\x00\xfc\x00\x7e\x00" }, "\"APwAfgA\"" },
    { { 5, (unsigned char *)"\x00\xfc\x00\x7e\x7e" }, "\"APwAfn4\"" },
    { { 5, (unsigned char *)"\x00\xfc\x00\x7e\xfc" }, "\"APwAfvw\"" },
    { { 5, (unsigned char *)"\x00\xfc\x00\xfc\x00" }, "\"APwA_AA\"" },
    { { 5, (unsigned char *)"\x00\xfc\x00\xfc\x7e" }, "\"APwA_H4\"" },
    { { 5, (unsigned char *)"\x00\xfc\x00\xfc\xfc" }, "\"APwA_Pw\"" },
    { { 5, (unsigned char *)"\x00\xfc\x7e\x00\x00" }, "\"APx-AAA\"" },
    { { 5, (unsigned char *)"\x00\xfc\x7e\x00\x7e" }, "\"APx-AH4\"" },
    { { 5, (unsigned char *)"\x00\xfc\x7e\x00\xfc" }, "\"APx-APw\"" },
    { { 5, (unsigned char *)"\x00\xfc\x7e\x7e\x00" }, "\"APx-fgA\"" },
    { { 5, (unsigned char *)"\x00\xfc\x7e\x7e\x7e" }, "\"APx-fn4\"" },
    { { 5, (unsigned char *)"\x00\xfc\x7e\x7e\xfc" }, "\"APx-fvw\"" },
    { { 5, (unsigned char *)"\x00\xfc\x7e\xfc\x00" }, "\"APx-_AA\"" },
    { { 5, (unsigned char *)"\x00\xfc\x7e\xfc\x7e" }, "\"APx-_H4\"" },
    { { 5, (unsigned char *)"\x00\xfc\x7e\xfc\xfc" }, "\"APx-_Pw\"" },
    { { 5, (unsigned char *)"\x00\xfc\xfc\x00\x00" }, "\"APz8AAA\"" },
    { { 5, (unsigned char *)"\x00\xfc\xfc\x00\x7e" }, "\"APz8AH4\"" },
    { { 5, (unsigned char *)"\x00\xfc\xfc\x00\xfc" }, "\"APz8APw\"" },
    { { 5, (unsigned char *)"\x00\xfc\xfc\x7e\x00" }, "\"APz8fgA\"" },
    { { 5, (unsigned char *)"\x00\xfc\xfc\x7e\x7e" }, "\"APz8fn4\"" },
    { { 5, (unsigned char *)"\x00\xfc\xfc\x7e\xfc" }, "\"APz8fvw\"" },
    { { 5, (unsigned char *)"\x00\xfc\xfc\xfc\x00" }, "\"APz8_AA\"" },
    { { 5, (unsigned char *)"\x00\xfc\xfc\xfc\x7e" }, "\"APz8_H4\"" },
    { { 5, (unsigned char *)"\x00\xfc\xfc\xfc\xfc" }, "\"APz8_Pw\"" },
    { { 5, (unsigned char *)"\x7e\x00\x00\x00\x00" }, "\"fgAAAAA\"" },
    { { 5, (unsigned char *)"\x7e\x00\x00\x00\x7e" }, "\"fgAAAH4\"" },
    { { 5, (unsigned char *)"\x7e\x00\x00\x00\xfc" }, "\"fgAAAPw\"" },
    { { 5, (unsigned char *)"\x7e\x00\x00\x7e\x00" }, "\"fgAAfgA\"" },
    { { 5, (unsigned char *)"\x7e\x00\x00\x7e\x7e" }, "\"fgAAfn4\"" },
    { { 5, (unsigned char *)"\x7e\x00\x00\x7e\xfc" }, "\"fgAAfvw\"" },
    { { 5, (unsigned char *)"\x7e\x00\x00\xfc\x00" }, "\"fgAA_AA\"" },
    { { 5, (unsigned char *)"\x7e\x00\x00\xfc\x7e" }, "\"fgAA_H4\"" },
    { { 5, (unsigned char *)"\x7e\x00\x00\xfc\xfc" }, "\"fgAA_Pw\"" },
    { { 5, (unsigned char *)"\x7e\x00\x7e\x00\x00" }, "\"fgB-AAA\"" },
    { { 5, (unsigned char *)"\x7e\x00\x7e\x00\x7e" }, "\"fgB-AH4\"" },
    { { 5, (unsigned char *)"\x7e\x00\x7e\x00\xfc" }, "\"fgB-APw\"" },
    { { 5, (unsigned char *)"\x7e\x00\x7e\x7e\x00" }, "\"fgB-fgA\"" },
    { { 5, (unsigned char *)"\x7e\x00\x7e\x7e\x7e" }, "\"fgB-fn4\"" },
    { { 5, (unsigned char *)"\x7e\x00\x7e\x7e\xfc" }, "\"fgB-fvw\"" },
    { { 5, (unsigned char *)"\x7e\x00\x7e\xfc\x00" }, "\"fgB-_AA\"" },
    { { 5, (unsigned char *)"\x7e\x00\x7e\xfc\x7e" }, "\"fgB-_H4\"" },
    { { 5, (unsigned char *)"\x7e\x00\x7e\xfc\xfc" }, "\"fgB-_Pw\"" },
    { { 5, (unsigned char *)"\x7e\x00\xfc\x00\x00" }, "\"fgD8AAA\"" },
    { { 5, (unsigned char *)"\x7e\x00\xfc\x00\x7e" }, "\"fgD8AH4\"" },
    { { 5, (unsigned char *)"\x7e\x00\xfc\x00\xfc" }, "\"fgD8APw\"" },
    { { 5, (unsigned char *)"\x7e\x00\xfc\x7e\x00" }, "\"fgD8fgA\"" },
    { { 5, (unsigned char *)"\x7e\x00\xfc\x7e\x7e" }, "\"fgD8fn4\"" },
    { { 5, (unsigned char *)"\x7e\x00\xfc\x7e\xfc" }, "\"fgD8fvw\"" },
    { { 5, (unsigned char *)"\x7e\x00\xfc\xfc\x00" }, "\"fgD8_AA\"" },
    { { 5, (unsigned char *)"\x7e\x00\xfc\xfc\x7e" }, "\"fgD8_H4\"" },
    { { 5, (unsigned char *)"\x7e\x00\xfc\xfc\xfc" }, "\"fgD8_Pw\"" },
    { { 5, (unsigned char *)"\x7e\x7e\x00\x00\x00" }, "\"fn4AAAA\"" },
    { { 5, (unsigned char *)"\x7e\x7e\x00\x00\x7e" }, "\"fn4AAH4\"" },
    { { 5, (unsigned char *)"\x7e\x7e\x00\x00\xfc" }, "\"fn4AAPw\"" },
    { { 5, (unsigned char *)"\x7e\x7e\x00\x7e\x00" }, "\"fn4AfgA\"" },
    { { 5, (unsigned char *)"\x7e\x7e\x00\x7e\x7e" }, "\"fn4Afn4\"" },
    { { 5, (unsigned char *)"\x7e\x7e\x00\x7e\xfc" }, "\"fn4Afvw\"" },
    { { 5, (unsigned char *)"\x7e\x7e\x00\xfc\x00" }, "\"fn4A_AA\"" },
    { { 5, (unsigned char *)"\x7e\x7e\x00\xfc\x7e" }, "\"fn4A_H4\"" },
    { { 5, (unsigned char *)"\x7e\x7e\x00\xfc\xfc" }, "\"fn4A_Pw\"" },
    { { 5, (unsigned char *)"\x7e\x7e\x7e\x00\x00" }, "\"fn5-AAA\"" },
    { { 5, (unsigned char *)"\x7e\x7e\x7e\x00\x7e" }, "\"fn5-AH4\"" },
    { { 5, (unsigned char *)"\x7e\x7e\x7e\x00\xfc" }, "\"fn5-APw\"" },
    { { 5, (unsigned char *)"\x7e\x7e\x7e\x7e\x00" }, "\"fn5-fgA\"" },
    { { 5, (unsigned char *)"\x7e\x7e\x7e\x7e\x7e" }, "\"fn5-fn4\"" },
    { { 5, (unsigned char *)"\x7e\x7e\x7e\x7e\xfc" }, "\"fn5-fvw\"" },
    { { 5, (unsigned char *)"\x7e\x7e\x7e\xfc\x00" }, "\"fn5-_AA\"" },
    { { 5, (unsigned char *)"\x7e\x7e\x7e\xfc\x7e" }, "\"fn5-_H4\"" },
    { { 5, (unsigned char *)"\x7e\x7e\x7e\xfc\xfc" }, "\"fn5-_Pw\"" },
    { { 5, (unsigned char *)"\x7e\x7e\xfc\x00\x00" }, "\"fn78AAA\"" },
    { { 5, (unsigned char *)"\x7e\x7e\xfc\x00\x7e" }, "\"fn78AH4\"" },
    { { 5, (unsigned char *)"\x7e\x7e\xfc\x00\xfc" }, "\"fn78APw\"" },
    { { 5, (unsigned char *)"\x7e\x7e\xfc\x7e\x00" }, "\"fn78fgA\"" },
    { { 5, (unsigned char *)"\x7e\x7e\xfc\x7e\x7e" }, "\"fn78fn4\"" },
    { { 5, (unsigned char *)"\x7e\x7e\xfc\x7e\xfc" }, "\"fn78fvw\"" },
    { { 5, (unsigned char *)"\x7e\x7e\xfc\xfc\x00" }, "\"fn78_AA\"" },
    { { 5, (unsigned char *)"\x7e\x7e\xfc\xfc\x7e" }, "\"fn78_H4\"" },
    { { 5, (unsigned char *)"\x7e\x7e\xfc\xfc\xfc" }, "\"fn78_Pw\"" },
    { { 5, (unsigned char *)"\x7e\xfc\x00\x00\x00" }, "\"fvwAAAA\"" },
    { { 5, (unsigned char *)"\x7e\xfc\x00\x00\x7e" }, "\"fvwAAH4\"" },
    { { 5, (unsigned char *)"\x7e\xfc\x00\x00\xfc" }, "\"fvwAAPw\"" },
    { { 5, (unsigned char *)"\x7e\xfc\x00\x7e\x00" }, "\"fvwAfgA\"" },
    { { 5, (unsigned char *)"\x7e\xfc\x00\x7e\x7e" }, "\"fvwAfn4\"" },
    { { 5, (unsigned char *)"\x7e\xfc\x00\x7e\xfc" }, "\"fvwAfvw\"" },
    { { 5, (unsigned char *)"\x7e\xfc\x00\xfc\x00" }, "\"fvwA_AA\"" },
    { { 5, (unsigned char *)"\x7e\xfc\x00\xfc\x7e" }, "\"fvwA_H4\"" },
    { { 5, (unsigned char *)"\x7e\xfc\x00\xfc\xfc" }, "\"fvwA_Pw\"" },
    { { 5, (unsigned char *)"\x7e\xfc\x7e\x00\x00" }, "\"fvx-AAA\"" },
    { { 5, (unsigned char *)"\x7e\xfc\x7e\x00\x7e" }, "\"fvx-AH4\"" },
    { { 5, (unsigned char *)"\x7e\xfc\x7e\x00\xfc" }, "\"fvx-APw\"" },
    { { 5, (unsigned char *)"\x7e\xfc\x7e\x7e\x00" }, "\"fvx-fgA\"" },
    { { 5, (unsigned char *)"\x7e\xfc\x7e\x7e\x7e" }, "\"fvx-fn4\"" },
    { { 5, (unsigned char *)"\x7e\xfc\x7e\x7e\xfc" }, "\"fvx-fvw\"" },
    { { 5, (unsigned char *)"\x7e\xfc\x7e\xfc\x00" }, "\"fvx-_AA\"" },
    { { 5, (unsigned char *)"\x7e\xfc\x7e\xfc\x7e" }, "\"fvx-_H4\"" },
    { { 5, (unsigned char *)"\x7e\xfc\x7e\xfc\xfc" }, "\"fvx-_Pw\"" },
    { { 5, (unsigned char *)"\x7e\xfc\xfc\x00\x00" }, "\"fvz8AAA\"" },
    { { 5, (unsigned char *)"\x7e\xfc\xfc\x00\x7e" }, "\"fvz8AH4\"" },
    { { 5, (unsigned char *)"\x7e\xfc\xfc\x00\xfc" }, "\"fvz8APw\"" },
    { { 5, (unsigned char *)"\x7e\xfc\xfc\x7e\x00" }, "\"fvz8fgA\"" },
    { { 5, (unsigned char *)"\x7e\xfc\xfc\x7e\x7e" }, "\"fvz8fn4\"" },
    { { 5, (unsigned char *)"\x7e\xfc\xfc\x7e\xfc" }, "\"fvz8fvw\"" },
    { { 5, (unsigned char *)"\x7e\xfc\xfc\xfc\x00" }, "\"fvz8_AA\"" },
    { { 5, (unsigned char *)"\x7e\xfc\xfc\xfc\x7e" }, "\"fvz8_H4\"" },
    { { 5, (unsigned char *)"\x7e\xfc\xfc\xfc\xfc" }, "\"fvz8_Pw\"" },
    { { 5, (unsigned char *)"\xfc\x00\x00\x00\x00" }, "\"_AAAAAA\"" },
    { { 5, (unsigned char *)"\xfc\x00\x00\x00\x7e" }, "\"_AAAAH4\"" },
    { { 5, (unsigned char *)"\xfc\x00\x00\x00\xfc" }, "\"_AAAAPw\"" },
    { { 5, (unsigned char *)"\xfc\x00\x00\x7e\x00" }, "\"_AAAfgA\"" },
    { { 5, (unsigned char *)"\xfc\x00\x00\x7e\x7e" }, "\"_AAAfn4\"" },
    { { 5, (unsigned char *)"\xfc\x00\x00\x7e\xfc" }, "\"_AAAfvw\"" },
    { { 5, (unsigned char *)"\xfc\x00\x00\xfc\x00" }, "\"_AAA_AA\"" },
    { { 5, (unsigned char *)"\xfc\x00\x00\xfc\x7e" }, "\"_AAA_H4\"" },
    { { 5, (unsigned char *)"\xfc\x00\x00\xfc\xfc" }, "\"_AAA_Pw\"" },
    { { 5, (unsigned char *)"\xfc\x00\x7e\x00\x00" }, "\"_AB-AAA\"" },
    { { 5, (unsigned char *)"\xfc\x00\x7e\x00\x7e" }, "\"_AB-AH4\"" },
    { { 5, (unsigned char *)"\xfc\x00\x7e\x00\xfc" }, "\"_AB-APw\"" },
    { { 5, (unsigned char *)"\xfc\x00\x7e\x7e\x00" }, "\"_AB-fgA\"" },
    { { 5, (unsigned char *)"\xfc\x00\x7e\x7e\x7e" }, "\"_AB-fn4\"" },
    { { 5, (unsigned char *)"\xfc\x00\x7e\x7e\xfc" }, "\"_AB-fvw\"" },
    { { 5, (unsigned char *)"\xfc\x00\x7e\xfc\x00" }, "\"_AB-_AA\"" },
    { { 5, (unsigned char *)"\xfc\x00\x7e\xfc\x7e" }, "\"_AB-_H4\"" },
    { { 5, (unsigned char *)"\xfc\x00\x7e\xfc\xfc" }, "\"_AB-_Pw\"" },
    { { 5, (unsigned char *)"\xfc\x00\xfc\x00\x00" }, "\"_AD8AAA\"" },
    { { 5, (unsigned char *)"\xfc\x00\xfc\x00\x7e" }, "\"_AD8AH4\"" },
    { { 5, (unsigned char *)"\xfc\x00\xfc\x00\xfc" }, "\"_AD8APw\"" },
    { { 5, (unsigned char *)"\xfc\x00\xfc\x7e\x00" }, "\"_AD8fgA\"" },
    { { 5, (unsigned char *)"\xfc\x00\xfc\x7e\x7e" }, "\"_AD8fn4\"" },
    { { 5, (unsigned char *)"\xfc\x00\xfc\x7e\xfc" }, "\"_AD8fvw\"" },
    { { 5, (unsigned char *)"\xfc\x00\xfc\xfc\x00" }, "\"_AD8_AA\"" },
    { { 5, (unsigned char *)"\xfc\x00\xfc\xfc\x7e" }, "\"_AD8_H4\"" },
    { { 5, (unsigned char *)"\xfc\x00\xfc\xfc\xfc" }, "\"_AD8_Pw\"" },
    { { 5, (unsigned char *)"\xfc\x7e\x00\x00\x00" }, "\"_H4AAAA\"" },
    { { 5, (unsigned char *)"\xfc\x7e\x00\x00\x7e" }, "\"_H4AAH4\"" },
    { { 5, (unsigned char *)"\xfc\x7e\x00\x00\xfc" }, "\"_H4AAPw\"" },
    { { 5, (unsigned char *)"\xfc\x7e\x00\x7e\x00" }, "\"_H4AfgA\"" },
    { { 5, (unsigned char *)"\xfc\x7e\x00\x7e\x7e" }, "\"_H4Afn4\"" },
    { { 5, (unsigned char *)"\xfc\x7e\x00\x7e\xfc" }, "\"_H4Afvw\"" },
    { { 5, (unsigned char *)"\xfc\x7e\x00\xfc\x00" }, "\"_H4A_AA\"" },
    { { 5, (unsigned char *)"\xfc\x7e\x00\xfc\x7e" }, "\"_H4A_H4\"" },
    { { 5, (unsigned char *)"\xfc\x7e\x00\xfc\xfc" }, "\"_H4A_Pw\"" },
    { { 5, (unsigned char *)"\xfc\x7e\x7e\x00\x00" }, "\"_H5-AAA\"" },
    { { 5, (unsigned char *)"\xfc\x7e\x7e\x00\x7e" }, "\"_H5-AH4\"" },
    { { 5, (unsigned char *)"\xfc\x7e\x7e\x00\xfc" }, "\"_H5-APw\"" },
    { { 5, (unsigned char *)"\xfc\x7e\x7e\x7e\x00" }, "\"_H5-fgA\"" },
    { { 5, (unsigned char *)"\xfc\x7e\x7e\x7e\x7e" }, "\"_H5-fn4\"" },
    { { 5, (unsigned char *)"\xfc\x7e\x7e\x7e\xfc" }, "\"_H5-fvw\"" },
    { { 5, (unsigned char *)"\xfc\x7e\x7e\xfc\x00" }, "\"_H5-_AA\"" },
    { { 5, (unsigned char *)"\xfc\x7e\x7e\xfc\x7e" }, "\"_H5-_H4\"" },
    { { 5, (unsigned char *)"\xfc\x7e\x7e\xfc\xfc" }, "\"_H5-_Pw\"" },
    { { 5, (unsigned char *)"\xfc\x7e\xfc\x00\x00" }, "\"_H78AAA\"" },
    { { 5, (unsigned char *)"\xfc\x7e\xfc\x00\x7e" }, "\"_H78AH4\"" },
    { { 5, (unsigned char *)"\xfc\x7e\xfc\x00\xfc" }, "\"_H78APw\"" },
    { { 5, (unsigned char *)"\xfc\x7e\xfc\x7e\x00" }, "\"_H78fgA\"" },
    { { 5, (unsigned char *)"\xfc\x7e\xfc\x7e\x7e" }, "\"_H78fn4\"" },
    { { 5, (unsigned char *)"\xfc\x7e\xfc\x7e\xfc" }, "\"_H78fvw\"" },
    { { 5, (unsigned char *)"\xfc\x7e\xfc\xfc\x00" }, "\"_H78_AA\"" },
    { { 5, (unsigned char *)"\xfc\x7e\xfc\xfc\x7e" }, "\"_H78_H4\"" },
    { { 5, (unsigned char *)"\xfc\x7e\xfc\xfc\xfc" }, "\"_H78_Pw\"" },
    { { 5, (unsigned char *)"\xfc\xfc\x00\x00\x00" }, "\"_PwAAAA\"" },
    { { 5, (unsigned char *)"\xfc\xfc\x00\x00\x7e" }, "\"_PwAAH4\"" },
    { { 5, (unsigned char *)"\xfc\xfc\x00\x00\xfc" }, "\"_PwAAPw\"" },
    { { 5, (unsigned char *)"\xfc\xfc\x00\x7e\x00" }, "\"_PwAfgA\"" },
    { { 5, (unsigned char *)"\xfc\xfc\x00\x7e\x7e" }, "\"_PwAfn4\"" },
    { { 5, (unsigned char *)"\xfc\xfc\x00\x7e\xfc" }, "\"_PwAfvw\"" },
    { { 5, (unsigned char *)"\xfc\xfc\x00\xfc\x00" }, "\"_PwA_AA\"" },
    { { 5, (unsigned char *)"\xfc\xfc\x00\xfc\x7e" }, "\"_PwA_H4\"" },
    { { 5, (unsigned char *)"\xfc\xfc\x00\xfc\xfc" }, "\"_PwA_Pw\"" },
    { { 5, (unsigned char *)"\xfc\xfc\x7e\x00\x00" }, "\"_Px-AAA\"" },
    { { 5, (unsigned char *)"\xfc\xfc\x7e\x00\x7e" }, "\"_Px-AH4\"" },
    { { 5, (unsigned char *)"\xfc\xfc\x7e\x00\xfc" }, "\"_Px-APw\"" },
    { { 5, (unsigned char *)"\xfc\xfc\x7e\x7e\x00" }, "\"_Px-fgA\"" },
    { { 5, (unsigned char *)"\xfc\xfc\x7e\x7e\x7e" }, "\"_Px-fn4\"" },
    { { 5, (unsigned char *)"\xfc\xfc\x7e\x7e\xfc" }, "\"_Px-fvw\"" },
    { { 5, (unsigned char *)"\xfc\xfc\x7e\xfc\x00" }, "\"_Px-_AA\"" },
    { { 5, (unsigned char *)"\xfc\xfc\x7e\xfc\x7e" }, "\"_Px-_H4\"" },
    { { 5, (unsigned char *)"\xfc\xfc\x7e\xfc\xfc" }, "\"_Px-_Pw\"" },
    { { 5, (unsigned char *)"\xfc\xfc\xfc\x00\x00" }, "\"_Pz8AAA\"" },
    { { 5, (unsigned char *)"\xfc\xfc\xfc\x00\x7e" }, "\"_Pz8AH4\"" },
    { { 5, (unsigned char *)"\xfc\xfc\xfc\x00\xfc" }, "\"_Pz8APw\"" },
    { { 5, (unsigned char *)"\xfc\xfc\xfc\x7e\x00" }, "\"_Pz8fgA\"" },
    { { 5, (unsigned char *)"\xfc\xfc\xfc\x7e\x7e" }, "\"_Pz8fn4\"" },
    { { 5, (unsigned char *)"\xfc\xfc\xfc\x7e\xfc" }, "\"_Pz8fvw\"" },
    { { 5, (unsigned char *)"\xfc\xfc\xfc\xfc\x00" }, "\"_Pz8_AA\"" },
    { { 5, (unsigned char *)"\xfc\xfc\xfc\xfc\x7e" }, "\"_Pz8_H4\"" },
    { { 5, (unsigned char *)"\xfc\xfc\xfc\xfc\xfc" }, "\"_Pz8_Pw\"" },
    { { 6, (unsigned char *)"\x00\x00\x00\x00\x00\x00" }, "\"AAAAAAAA\"" },
    { { 6, (unsigned char *)"\x00\x00\x00\x00\x00\xa8" }, "\"AAAAAACo\"" },
    { { 6, (unsigned char *)"\x00\x00\x00\x00\xa8\x00" }, "\"AAAAAKgA\"" },
    { { 6, (unsigned char *)"\x00\x00\x00\x00\xa8\xa8" }, "\"AAAAAKio\"" },
    { { 6, (unsigned char *)"\x00\x00\x00\xa8\x00\x00" }, "\"AAAAqAAA\"" },
    { { 6, (unsigned char *)"\x00\x00\x00\xa8\x00\xa8" }, "\"AAAAqACo\"" },
    { { 6, (unsigned char *)"\x00\x00\x00\xa8\xa8\x00" }, "\"AAAAqKgA\"" },
    { { 6, (unsigned char *)"\x00\x00\x00\xa8\xa8\xa8" }, "\"AAAAqKio\"" },
    { { 6, (unsigned char *)"\x00\x00\xa8\x00\x00\x00" }, "\"AACoAAAA\"" },
    { { 6, (unsigned char *)"\x00\x00\xa8\x00\x00\xa8" }, "\"AACoAACo\"" },
    { { 6, (unsigned char *)"\x00\x00\xa8\x00\xa8\x00" }, "\"AACoAKgA\"" },
    { { 6, (unsigned char *)"\x00\x00\xa8\x00\xa8\xa8" }, "\"AACoAKio\"" },
    { { 6, (unsigned char *)"\x00\x00\xa8\xa8\x00\x00" }, "\"AACoqAAA\"" },
    { { 6, (unsigned char *)"\x00\x00\xa8\xa8\x00\xa8" }, "\"AACoqACo\"" },
    { { 6, (unsigned char *)"\x00\x00\xa8\xa8\xa8\x00" }, "\"AACoqKgA\"" },
    { { 6, (unsigned char *)"\x00\x00\xa8\xa8\xa8\xa8" }, "\"AACoqKio\"" },
    { { 6, (unsigned char *)"\x00\xa8\x00\x00\x00\x00" }, "\"AKgAAAAA\"" },
    { { 6, (unsigned char *)"\x00\xa8\x00\x00\x00\xa8" }, "\"AKgAAACo\"" },
    { { 6, (unsigned char *)"\x00\xa8\x00\x00\xa8\x00" }, "\"AKgAAKgA\"" },
    { { 6, (unsigned char *)"\x00\xa8\x00\x00\xa8\xa8" }, "\"AKgAAKio\"" },
    { { 6, (unsigned char *)"\x00\xa8\x00\xa8\x00\x00" }, "\"AKgAqAAA\"" },
    { { 6, (unsigned char *)"\x00\xa8\x00\xa8\x00\xa8" }, "\"AKgAqACo\"" },
    { { 6, (unsigned char *)"\x00\xa8\x00\xa8\xa8\x00" }, "\"AKgAqKgA\"" },
    { { 6, (unsigned char *)"\x00\xa8\x00\xa8\xa8\xa8" }, "\"AKgAqKio\"" },
    { { 6, (unsigned char *)"\x00\xa8\xa8\x00\x00\x00" }, "\"AKioAAAA\"" },
    { { 6, (unsigned char *)"\x00\xa8\xa8\x00\x00\xa8" }, "\"AKioAACo\"" },
    { { 6, (unsigned char *)"\x00\xa8\xa8\x00\xa8\x00" }, "\"AKioAKgA\"" },
    { { 6, (unsigned char *)"\x00\xa8\xa8\x00\xa8\xa8" }, "\"AKioAKio\"" },
    { { 6, (unsigned char *)"\x00\xa8\xa8\xa8\x00\x00" }, "\"AKioqAAA\"" },
    { { 6, (unsigned char *)"\x00\xa8\xa8\xa8\x00\xa8" }, "\"AKioqACo\"" },
    { { 6, (unsigned char *)"\x00\xa8\xa8\xa8\xa8\x00" }, "\"AKioqKgA\"" },
    { { 6, (unsigned char *)"\x00\xa8\xa8\xa8\xa8\xa8" }, "\"AKioqKio\"" },
    { { 6, (unsigned char *)"\xa8\x00\x00\x00\x00\x00" }, "\"qAAAAAAA\"" },
    { { 6, (unsigned char *)"\xa8\x00\x00\x00\x00\xa8" }, "\"qAAAAACo\"" },
    { { 6, (unsigned char *)"\xa8\x00\x00\x00\xa8\x00" }, "\"qAAAAKgA\"" },
    { { 6, (unsigned char *)"\xa8\x00\x00\x00\xa8\xa8" }, "\"qAAAAKio\"" },
    { { 6, (unsigned char *)"\xa8\x00\x00\xa8\x00\x00" }, "\"qAAAqAAA\"" },
    { { 6, (unsigned char *)"\xa8\x00\x00\xa8\x00\xa8" }, "\"qAAAqACo\"" },
    { { 6, (unsigned char *)"\xa8\x00\x00\xa8\xa8\x00" }, "\"qAAAqKgA\"" },
    { { 6, (unsigned char *)"\xa8\x00\x00\xa8\xa8\xa8" }, "\"qAAAqKio\"" },
    { { 6, (unsigned char *)"\xa8\x00\xa8\x00\x00\x00" }, "\"qACoAAAA\"" },
    { { 6, (unsigned char *)"\xa8\x00\xa8\x00\x00\xa8" }, "\"qACoAACo\"" },
    { { 6, (unsigned char *)"\xa8\x00\xa8\x00\xa8\x00" }, "\"qACoAKgA\"" },
    { { 6, (unsigned char *)"\xa8\x00\xa8\x00\xa8\xa8" }, "\"qACoAKio\"" },
    { { 6, (unsigned char *)"\xa8\x00\xa8\xa8\x00\x00" }, "\"qACoqAAA\"" },
    { { 6, (unsigned char *)"\xa8\x00\xa8\xa8\x00\xa8" }, "\"qACoqACo\"" },
    { { 6, (unsigned char *)"\xa8\x00\xa8\xa8\xa8\x00" }, "\"qACoqKgA\"" },
    { { 6, (unsigned char *)"\xa8\x00\xa8\xa8\xa8\xa8" }, "\"qACoqKio\"" },
    { { 6, (unsigned char *)"\xa8\xa8\x00\x00\x00\x00" }, "\"qKgAAAAA\"" },
    { { 6, (unsigned char *)"\xa8\xa8\x00\x00\x00\xa8" }, "\"qKgAAACo\"" },
    { { 6, (unsigned char *)"\xa8\xa8\x00\x00\xa8\x00" }, "\"qKgAAKgA\"" },
    { { 6, (unsigned char *)"\xa8\xa8\x00\x00\xa8\xa8" }, "\"qKgAAKio\"" },
    { { 6, (unsigned char *)"\xa8\xa8\x00\xa8\x00\x00" }, "\"qKgAqAAA\"" },
    { { 6, (unsigned char *)"\xa8\xa8\x00\xa8\x00\xa8" }, "\"qKgAqACo\"" },
    { { 6, (unsigned char *)"\xa8\xa8\x00\xa8\xa8\x00" }, "\"qKgAqKgA\"" },
    { { 6, (unsigned char *)"\xa8\xa8\x00\xa8\xa8\xa8" }, "\"qKgAqKio\"" },
    { { 6, (unsigned char *)"\xa8\xa8\xa8\x00\x00\x00" }, "\"qKioAAAA\"" },
    { { 6, (unsigned char *)"\xa8\xa8\xa8\x00\x00\xa8" }, "\"qKioAACo\"" },
    { { 6, (unsigned char *)"\xa8\xa8\xa8\x00\xa8\x00" }, "\"qKioAKgA\"" },
    { { 6, (unsigned char *)"\xa8\xa8\xa8\x00\xa8\xa8" }, "\"qKioAKio\"" },
    { { 6, (unsigned char *)"\xa8\xa8\xa8\xa8\x00\x00" }, "\"qKioqAAA\"" },
    { { 6, (unsigned char *)"\xa8\xa8\xa8\xa8\x00\xa8" }, "\"qKioqACo\"" },
    { { 6, (unsigned char *)"\xa8\xa8\xa8\xa8\xa8\x00" }, "\"qKioqKgA\"" },
    { { 6, (unsigned char *)"\xa8\xa8\xa8\xa8\xa8\xa8" }, "\"qKioqKio\"" },
    { { 7, (unsigned char *)"\x00\x00\x00\x00\x00\x00\x00" }, "\"AAAAAAAAAA\"" },
    { { 7, (unsigned char *)"\x00\x00\x00\x00\x00\x00\xd3" }, "\"AAAAAAAA0w\"" },
    { { 7, (unsigned char *)"\x00\x00\x00\x00\x00\xd3\x00" }, "\"AAAAAADTAA\"" },
    { { 7, (unsigned char *)"\x00\x00\x00\x00\x00\xd3\xd3" }, "\"AAAAAADT0w\"" },
    { { 7, (unsigned char *)"\x00\x00\x00\x00\xd3\x00\x00" }, "\"AAAAANMAAA\"" },
    { { 7, (unsigned char *)"\x00\x00\x00\x00\xd3\x00\xd3" }, "\"AAAAANMA0w\"" },
    { { 7, (unsigned char *)"\x00\x00\x00\x00\xd3\xd3\x00" }, "\"AAAAANPTAA\"" },
    { { 7, (unsigned char *)"\x00\x00\x00\x00\xd3\xd3\xd3" }, "\"AAAAANPT0w\"" },
    { { 7, (unsigned char *)"\x00\x00\x00\xd3\x00\x00\x00" }, "\"AAAA0wAAAA\"" },
    { { 7, (unsigned char *)"\x00\x00\x00\xd3\x00\x00\xd3" }, "\"AAAA0wAA0w\"" },
    { { 7, (unsigned char *)"\x00\x00\x00\xd3\x00\xd3\x00" }, "\"AAAA0wDTAA\"" },
    { { 7, (unsigned char *)"\x00\x00\x00\xd3\x00\xd3\xd3" }, "\"AAAA0wDT0w\"" },
    { { 7, (unsigned char *)"\x00\x00\x00\xd3\xd3\x00\x00" }, "\"AAAA09MAAA\"" },
    { { 7, (unsigned char *)"\x00\x00\x00\xd3\xd3\x00\xd3" }, "\"AAAA09MA0w\"" },
    { { 7, (unsigned char *)"\x00\x00\x00\xd3\xd3\xd3\x00" }, "\"AAAA09PTAA\"" },
    { { 7, (unsigned char *)"\x00\x00\x00\xd3\xd3\xd3\xd3" }, "\"AAAA09PT0w\"" },
    { { 7, (unsigned char *)"\x00\x00\xd3\x00\x00\x00\x00" }, "\"AADTAAAAAA\"" },
    { { 7, (unsigned char *)"\x00\x00\xd3\x00\x00\x00\xd3" }, "\"AADTAAAA0w\"" },
    { { 7, (unsigned char *)"\x00\x00\xd3\x00\x00\xd3\x00" }, "\"AADTAADTAA\"" },
    { { 7, (unsigned char *)"\x00\x00\xd3\x00\x00\xd3\xd3" }, "\"AADTAADT0w\"" },
    { { 7, (unsigned char *)"\x00\x00\xd3\x00\xd3\x00\x00" }, "\"AADTANMAAA\"" },
    { { 7, (unsigned char *)"\x00\x00\xd3\x00\xd3\x00\xd3" }, "\"AADTANMA0w\"" },
    { { 7, (unsigned char *)"\x00\x00\xd3\x00\xd3\xd3\x00" }, "\"AADTANPTAA\"" },
    { { 7, (unsigned char *)"\x00\x00\xd3\x00\xd3\xd3\xd3" }, "\"AADTANPT0w\"" },
    { { 7, (unsigned char *)"\x00\x00\xd3\xd3\x00\x00\x00" }, "\"AADT0wAAAA\"" },
    { { 7, (unsigned char *)"\x00\x00\xd3\xd3\x00\x00\xd3" }, "\"AADT0wAA0w\"" },
    { { 7, (unsigned char *)"\x00\x00\xd3\xd3\x00\xd3\x00" }, "\"AADT0wDTAA\"" },
    { { 7, (unsigned char *)"\x00\x00\xd3\xd3\x00\xd3\xd3" }, "\"AADT0wDT0w\"" },
    { { 7, (unsigned char *)"\x00\x00\xd3\xd3\xd3\x00\x00" }, "\"AADT09MAAA\"" },
    { { 7, (unsigned char *)"\x00\x00\xd3\xd3\xd3\x00\xd3" }, "\"AADT09MA0w\"" },
    { { 7, (unsigned char *)"\x00\x00\xd3\xd3\xd3\xd3\x00" }, "\"AADT09PTAA\"" },
    { { 7, (unsigned char *)"\x00\x00\xd3\xd3\xd3\xd3\xd3" }, "\"AADT09PT0w\"" },
    { { 7, (unsigned char *)"\x00\xd3\x00\x00\x00\x00\x00" }, "\"ANMAAAAAAA\"" },
    { { 7, (unsigned char *)"\x00\xd3\x00\x00\x00\x00\xd3" }, "\"ANMAAAAA0w\"" },
    { { 7, (unsigned char *)"\x00\xd3\x00\x00\x00\xd3\x00" }, "\"ANMAAADTAA\"" },
    { { 7, (unsigned char *)"\x00\xd3\x00\x00\x00\xd3\xd3" }, "\"ANMAAADT0w\"" },
    { { 7, (unsigned char *)"\x00\xd3\x00\x00\xd3\x00\x00" }, "\"ANMAANMAAA\"" },
    { { 7, (unsigned char *)"\x00\xd3\x00\x00\xd3\x00\xd3" }, "\"ANMAANMA0w\"" },
    { { 7, (unsigned char *)"\x00\xd3\x00\x00\xd3\xd3\x00" }, "\"ANMAANPTAA\"" },
    { { 7, (unsigned char *)"\x00\xd3\x00\x00\xd3\xd3\xd3" }, "\"ANMAANPT0w\"" },
    { { 7, (unsigned char *)"\x00\xd3\x00\xd3\x00\x00\x00" }, "\"ANMA0wAAAA\"" },
    { { 7, (unsigned char *)"\x00\xd3\x00\xd3\x00\x00\xd3" }, "\"ANMA0wAA0w\"" },
    { { 7, (unsigned char *)"\x00\xd3\x00\xd3\x00\xd3\x00" }, "\"ANMA0wDTAA\"" },
    { { 7, (unsigned char *)"\x00\xd3\x00\xd3\x00\xd3\xd3" }, "\"ANMA0wDT0w\"" },
    { { 7, (unsigned char *)"\x00\xd3\x00\xd3\xd3\x00\x00" }, "\"ANMA09MAAA\"" },
    { { 7, (unsigned char *)"\x00\xd3\x00\xd3\xd3\x00\xd3" }, "\"ANMA09MA0w\"" },
    { { 7, (unsigned char *)"\x00\xd3\x00\xd3\xd3\xd3\x00" }, "\"ANMA09PTAA\"" },
    { { 7, (unsigned char *)"\x00\xd3\x00\xd3\xd3\xd3\xd3" }, "\"ANMA09PT0w\"" },
    { { 7, (unsigned char *)"\x00\xd3\xd3\x00\x00\x00\x00" }, "\"ANPTAAAAAA\"" },
    { { 7, (unsigned char *)"\x00\xd3\xd3\x00\x00\x00\xd3" }, "\"ANPTAAAA0w\"" },
    { { 7, (unsigned char *)"\x00\xd3\xd3\x00\x00\xd3\x00" }, "\"ANPTAADTAA\"" },
    { { 7, (unsigned char *)"\x00\xd3\xd3\x00\x00\xd3\xd3" }, "\"ANPTAADT0w\"" },
    { { 7, (unsigned char *)"\x00\xd3\xd3\x00\xd3\x00\x00" }, "\"ANPTANMAAA\"" },
    { { 7, (unsigned char *)"\x00\xd3\xd3\x00\xd3\x00\xd3" }, "\"ANPTANMA0w\"" },
    { { 7, (unsigned char *)"\x00\xd3\xd3\x00\xd3\xd3\x00" }, "\"ANPTANPTAA\"" },
    { { 7, (unsigned char *)"\x00\xd3\xd3\x00\xd3\xd3\xd3" }, "\"ANPTANPT0w\"" },
    { { 7, (unsigned char *)"\x00\xd3\xd3\xd3\x00\x00\x00" }, "\"ANPT0wAAAA\"" },
    { { 7, (unsigned char *)"\x00\xd3\xd3\xd3\x00\x00\xd3" }, "\"ANPT0wAA0w\"" },
    { { 7, (unsigned char *)"\x00\xd3\xd3\xd3\x00\xd3\x00" }, "\"ANPT0wDTAA\"" },
    { { 7, (unsigned char *)"\x00\xd3\xd3\xd3\x00\xd3\xd3" }, "\"ANPT0wDT0w\"" },
    { { 7, (unsigned char *)"\x00\xd3\xd3\xd3\xd3\x00\x00" }, "\"ANPT09MAAA\"" },
    { { 7, (unsigned char *)"\x00\xd3\xd3\xd3\xd3\x00\xd3" }, "\"ANPT09MA0w\"" },
    { { 7, (unsigned char *)"\x00\xd3\xd3\xd3\xd3\xd3\x00" }, "\"ANPT09PTAA\"" },
    { { 7, (unsigned char *)"\x00\xd3\xd3\xd3\xd3\xd3\xd3" }, "\"ANPT09PT0w\"" },
    { { 7, (unsigned char *)"\xd3\x00\x00\x00\x00\x00\x00" }, "\"0wAAAAAAAA\"" },
    { { 7, (unsigned char *)"\xd3\x00\x00\x00\x00\x00\xd3" }, "\"0wAAAAAA0w\"" },
    { { 7, (unsigned char *)"\xd3\x00\x00\x00\x00\xd3\x00" }, "\"0wAAAADTAA\"" },
    { { 7, (unsigned char *)"\xd3\x00\x00\x00\x00\xd3\xd3" }, "\"0wAAAADT0w\"" },
    { { 7, (unsigned char *)"\xd3\x00\x00\x00\xd3\x00\x00" }, "\"0wAAANMAAA\"" },
    { { 7, (unsigned char *)"\xd3\x00\x00\x00\xd3\x00\xd3" }, "\"0wAAANMA0w\"" },
    { { 7, (unsigned char *)"\xd3\x00\x00\x00\xd3\xd3\x00" }, "\"0wAAANPTAA\"" },
    { { 7, (unsigned char *)"\xd3\x00\x00\x00\xd3\xd3\xd3" }, "\"0wAAANPT0w\"" },
    { { 7, (unsigned char *)"\xd3\x00\x00\xd3\x00\x00\x00" }, "\"0wAA0wAAAA\"" },
    { { 7, (unsigned char *)"\xd3\x00\x00\xd3\x00\x00\xd3" }, "\"0wAA0wAA0w\"" },
    { { 7, (unsigned char *)"\xd3\x00\x00\xd3\x00\xd3\x00" }, "\"0wAA0wDTAA\"" },
    { { 7, (unsigned char *)"\xd3\x00\x00\xd3\x00\xd3\xd3" }, "\"0wAA0wDT0w\"" },
    { { 7, (unsigned char *)"\xd3\x00\x00\xd3\xd3\x00\x00" }, "\"0wAA09MAAA\"" },
    { { 7, (unsigned char *)"\xd3\x00\x00\xd3\xd3\x00\xd3" }, "\"0wAA09MA0w\"" },
    { { 7, (unsigned char *)"\xd3\x00\x00\xd3\xd3\xd3\x00" }, "\"0wAA09PTAA\"" },
    { { 7, (unsigned char *)"\xd3\x00\x00\xd3\xd3\xd3\xd3" }, "\"0wAA09PT0w\"" },
    { { 7, (unsigned char *)"\xd3\x00\xd3\x00\x00\x00\x00" }, "\"0wDTAAAAAA\"" },
    { { 7, (unsigned char *)"\xd3\x00\xd3\x00\x00\x00\xd3" }, "\"0wDTAAAA0w\"" },
    { { 7, (unsigned char *)"\xd3\x00\xd3\x00\x00\xd3\x00" }, "\"0wDTAADTAA\"" },
    { { 7, (unsigned char *)"\xd3\x00\xd3\x00\x00\xd3\xd3" }, "\"0wDTAADT0w\"" },
    { { 7, (unsigned char *)"\xd3\x00\xd3\x00\xd3\x00\x00" }, "\"0wDTANMAAA\"" },
    { { 7, (unsigned char *)"\xd3\x00\xd3\x00\xd3\x00\xd3" }, "\"0wDTANMA0w\"" },
    { { 7, (unsigned char *)"\xd3\x00\xd3\x00\xd3\xd3\x00" }, "\"0wDTANPTAA\"" },
    { { 7, (unsigned char *)"\xd3\x00\xd3\x00\xd3\xd3\xd3" }, "\"0wDTANPT0w\"" },
    { { 7, (unsigned char *)"\xd3\x00\xd3\xd3\x00\x00\x00" }, "\"0wDT0wAAAA\"" },
    { { 7, (unsigned char *)"\xd3\x00\xd3\xd3\x00\x00\xd3" }, "\"0wDT0wAA0w\"" },
    { { 7, (unsigned char *)"\xd3\x00\xd3\xd3\x00\xd3\x00" }, "\"0wDT0wDTAA\"" },
    { { 7, (unsigned char *)"\xd3\x00\xd3\xd3\x00\xd3\xd3" }, "\"0wDT0wDT0w\"" },
    { { 7, (unsigned char *)"\xd3\x00\xd3\xd3\xd3\x00\x00" }, "\"0wDT09MAAA\"" },
    { { 7, (unsigned char *)"\xd3\x00\xd3\xd3\xd3\x00\xd3" }, "\"0wDT09MA0w\"" },
    { { 7, (unsigned char *)"\xd3\x00\xd3\xd3\xd3\xd3\x00" }, "\"0wDT09PTAA\"" },
    { { 7, (unsigned char *)"\xd3\x00\xd3\xd3\xd3\xd3\xd3" }, "\"0wDT09PT0w\"" },
    { { 7, (unsigned char *)"\xd3\xd3\x00\x00\x00\x00\x00" }, "\"09MAAAAAAA\"" },
    { { 7, (unsigned char *)"\xd3\xd3\x00\x00\x00\x00\xd3" }, "\"09MAAAAA0w\"" },
    { { 7, (unsigned char *)"\xd3\xd3\x00\x00\x00\xd3\x00" }, "\"09MAAADTAA\"" },
    { { 7, (unsigned char *)"\xd3\xd3\x00\x00\x00\xd3\xd3" }, "\"09MAAADT0w\"" },
    { { 7, (unsigned char *)"\xd3\xd3\x00\x00\xd3\x00\x00" }, "\"09MAANMAAA\"" },
    { { 7, (unsigned char *)"\xd3\xd3\x00\x00\xd3\x00\xd3" }, "\"09MAANMA0w\"" },
    { { 7, (unsigned char *)"\xd3\xd3\x00\x00\xd3\xd3\x00" }, "\"09MAANPTAA\"" },
    { { 7, (unsigned char *)"\xd3\xd3\x00\x00\xd3\xd3\xd3" }, "\"09MAANPT0w\"" },
    { { 7, (unsigned char *)"\xd3\xd3\x00\xd3\x00\x00\x00" }, "\"09MA0wAAAA\"" },
    { { 7, (unsigned char *)"\xd3\xd3\x00\xd3\x00\x00\xd3" }, "\"09MA0wAA0w\"" },
    { { 7, (unsigned char *)"\xd3\xd3\x00\xd3\x00\xd3\x00" }, "\"09MA0wDTAA\"" },
    { { 7, (unsigned char *)"\xd3\xd3\x00\xd3\x00\xd3\xd3" }, "\"09MA0wDT0w\"" },
    { { 7, (unsigned char *)"\xd3\xd3\x00\xd3\xd3\x00\x00" }, "\"09MA09MAAA\"" },
    { { 7, (unsigned char *)"\xd3\xd3\x00\xd3\xd3\x00\xd3" }, "\"09MA09MA0w\"" },
    { { 7, (unsigned char *)"\xd3\xd3\x00\xd3\xd3\xd3\x00" }, "\"09MA09PTAA\"" },
    { { 7, (unsigned char *)"\xd3\xd3\x00\xd3\xd3\xd3\xd3" }, "\"09MA09PT0w\"" },
    { { 7, (unsigned char *)"\xd3\xd3\xd3\x00\x00\x00\x00" }, "\"09PTAAAAAA\"" },
    { { 7, (unsigned char *)"\xd3\xd3\xd3\x00\x00\x00\xd3" }, "\"09PTAAAA0w\"" },
    { { 7, (unsigned char *)"\xd3\xd3\xd3\x00\x00\xd3\x00" }, "\"09PTAADTAA\"" },
    { { 7, (unsigned char *)"\xd3\xd3\xd3\x00\x00\xd3\xd3" }, "\"09PTAADT0w\"" },
    { { 7, (unsigned char *)"\xd3\xd3\xd3\x00\xd3\x00\x00" }, "\"09PTANMAAA\"" },
    { { 7, (unsigned char *)"\xd3\xd3\xd3\x00\xd3\x00\xd3" }, "\"09PTANMA0w\"" },
    { { 7, (unsigned char *)"\xd3\xd3\xd3\x00\xd3\xd3\x00" }, "\"09PTANPTAA\"" },
    { { 7, (unsigned char *)"\xd3\xd3\xd3\x00\xd3\xd3\xd3" }, "\"09PTANPT0w\"" },
    { { 7, (unsigned char *)"\xd3\xd3\xd3\xd3\x00\x00\x00" }, "\"09PT0wAAAA\"" },
    { { 7, (unsigned char *)"\xd3\xd3\xd3\xd3\x00\x00\xd3" }, "\"09PT0wAA0w\"" },
    { { 7, (unsigned char *)"\xd3\xd3\xd3\xd3\x00\xd3\x00" }, "\"09PT0wDTAA\"" },
    { { 7, (unsigned char *)"\xd3\xd3\xd3\xd3\x00\xd3\xd3" }, "\"09PT0wDT0w\"" },
    { { 7, (unsigned char *)"\xd3\xd3\xd3\xd3\xd3\x00\x00" }, "\"09PT09MAAA\"" },
    { { 7, (unsigned char *)"\xd3\xd3\xd3\xd3\xd3\x00\xd3" }, "\"09PT09MA0w\"" },
    { { 7, (unsigned char *)"\xd3\xd3\xd3\xd3\xd3\xd3\x00" }, "\"09PT09PTAA\"" },
    { { 7, (unsigned char *)"\xd3\xd3\xd3\xd3\xd3\xd3\xd3" }, "\"09PT09PT0w\"" },
    { { 8, (unsigned char *)"\x00\x00\x00\x00\x00\x00\x00\x00" }, "\"AAAAAAAAAAA\"" },
    { { 9, (unsigned char *)"\x00\x00\x00\x00\x00\x00\x00\x00\x00" }, "\"AAAAAAAAAAAA\"" },
    { { 10, (unsigned char *)"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" }, "\"AAAAAAAAAAAAAA\"" },
},
testVector_EDM_BINARY_with_equal_signs[] =
    {
        { { 1, (unsigned char *)"\x00" }, "\"AA==\"" },
        { { 1, (unsigned char *)"\x01" }, "\"AQ==\"" },
        { { 1, (unsigned char *)"\x02" }, "\"Ag==\"" },
        { { 1, (unsigned char *)"\x03" }, "\"Aw==\"" },
        { { 1, (unsigned char *)"\x04" }, "\"BA==\"" },
        { { 1, (unsigned char *)"\x05" }, "\"BQ==\"" },
        { { 1, (unsigned char *)"\x06" }, "\"Bg==\"" },
        { { 1, (unsigned char *)"\x07" }, "\"Bw==\"" },
        { { 1, (unsigned char *)"\x08" }, "\"CA==\"" },
        { { 1, (unsigned char *)"\x09" }, "\"CQ==\"" },
        { { 1, (unsigned char *)"\x0a" }, "\"Cg==\"" },
        { { 1, (unsigned char *)"\x0b" }, "\"Cw==\"" },
        { { 1, (unsigned char *)"\x0c" }, "\"DA==\"" },
        { { 1, (unsigned char *)"\x0d" }, "\"DQ==\"" },
        { { 1, (unsigned char *)"\x0e" }, "\"Dg==\"" },
        { { 1, (unsigned char *)"\x0f" }, "\"Dw==\"" },
        { { 1, (unsigned char *)"\x10" }, "\"EA==\"" },
        { { 1, (unsigned char *)"\x11" }, "\"EQ==\"" },
        { { 1, (unsigned char *)"\x12" }, "\"Eg==\"" },
        { { 1, (unsigned char *)"\x13" }, "\"Ew==\"" },
        { { 1, (unsigned char *)"\x14" }, "\"FA==\"" },
        { { 1, (unsigned char *)"\x15" }, "\"FQ==\"" },
        { { 1, (unsigned char *)"\x16" }, "\"Fg==\"" },
        { { 1, (unsigned char *)"\x17" }, "\"Fw==\"" },
        { { 1, (unsigned char *)"\x18" }, "\"GA==\"" },
        { { 1, (unsigned char *)"\x19" }, "\"GQ==\"" },
        { { 1, (unsigned char *)"\x1a" }, "\"Gg==\"" },
        { { 1, (unsigned char *)"\x1b" }, "\"Gw==\"" },
        { { 1, (unsigned char *)"\x1c" }, "\"HA==\"" },
        { { 1, (unsigned char *)"\x1d" }, "\"HQ==\"" },
        { { 1, (unsigned char *)"\x1e" }, "\"Hg==\"" },
        { { 1, (unsigned char *)"\x1f" }, "\"Hw==\"" },
        { { 1, (unsigned char *)"\x20" }, "\"IA==\"" },
        { { 1, (unsigned char *)"\x21" }, "\"IQ==\"" },
        { { 1, (unsigned char *)"\x22" }, "\"Ig==\"" },
        { { 1, (unsigned char *)"\x23" }, "\"Iw==\"" },
        { { 1, (unsigned char *)"\x24" }, "\"JA==\"" },
        { { 1, (unsigned char *)"\x25" }, "\"JQ==\"" },
        { { 1, (unsigned char *)"\x26" }, "\"Jg==\"" },
        { { 1, (unsigned char *)"\x27" }, "\"Jw==\"" },
        { { 1, (unsigned char *)"\x28" }, "\"KA==\"" },
        { { 1, (unsigned char *)"\x29" }, "\"KQ==\"" },
        { { 1, (unsigned char *)"\x2a" }, "\"Kg==\"" },
        { { 1, (unsigned char *)"\x2b" }, "\"Kw==\"" },
        { { 1, (unsigned char *)"\x2c" }, "\"LA==\"" },
        { { 1, (unsigned char *)"\x2d" }, "\"LQ==\"" },
        { { 1, (unsigned char *)"\x2e" }, "\"Lg==\"" },
        { { 1, (unsigned char *)"\x2f" }, "\"Lw==\"" },
        { { 1, (unsigned char *)"\x30" }, "\"MA==\"" },
        { { 1, (unsigned char *)"\x31" }, "\"MQ==\"" },
        { { 1, (unsigned char *)"\x32" }, "\"Mg==\"" },
        { { 1, (unsigned char *)"\x33" }, "\"Mw==\"" },
        { { 1, (unsigned char *)"\x34" }, "\"NA==\"" },
        { { 1, (unsigned char *)"\x35" }, "\"NQ==\"" },
        { { 1, (unsigned char *)"\x36" }, "\"Ng==\"" },
        { { 1, (unsigned char *)"\x37" }, "\"Nw==\"" },
        { { 1, (unsigned char *)"\x38" }, "\"OA==\"" },
        { { 1, (unsigned char *)"\x39" }, "\"OQ==\"" },
        { { 1, (unsigned char *)"\x3a" }, "\"Og==\"" },
        { { 1, (unsigned char *)"\x3b" }, "\"Ow==\"" },
        { { 1, (unsigned char *)"\x3c" }, "\"PA==\"" },
        { { 1, (unsigned char *)"\x3d" }, "\"PQ==\"" },
        { { 1, (unsigned char *)"\x3e" }, "\"Pg==\"" },
        { { 1, (unsigned char *)"\x3f" }, "\"Pw==\"" },
        { { 1, (unsigned char *)"\x40" }, "\"QA==\"" },
        { { 1, (unsigned char *)"\x41" }, "\"QQ==\"" },
        { { 1, (unsigned char *)"\x42" }, "\"Qg==\"" },
        { { 1, (unsigned char *)"\x43" }, "\"Qw==\"" },
        { { 1, (unsigned char *)"\x44" }, "\"RA==\"" },
        { { 1, (unsigned char *)"\x45" }, "\"RQ==\"" },
        { { 1, (unsigned char *)"\x46" }, "\"Rg==\"" },
        { { 1, (unsigned char *)"\x47" }, "\"Rw==\"" },
        { { 1, (unsigned char *)"\x48" }, "\"SA==\"" },
        { { 1, (unsigned char *)"\x49" }, "\"SQ==\"" },
        { { 1, (unsigned char *)"\x4a" }, "\"Sg==\"" },
        { { 1, (unsigned char *)"\x4b" }, "\"Sw==\"" },
        { { 1, (unsigned char *)"\x4c" }, "\"TA==\"" },
        { { 1, (unsigned char *)"\x4d" }, "\"TQ==\"" },
        { { 1, (unsigned char *)"\x4e" }, "\"Tg==\"" },
        { { 1, (unsigned char *)"\x4f" }, "\"Tw==\"" },
        { { 1, (unsigned char *)"\x50" }, "\"UA==\"" },
        { { 1, (unsigned char *)"\x51" }, "\"UQ==\"" },
        { { 1, (unsigned char *)"\x52" }, "\"Ug==\"" },
        { { 1, (unsigned char *)"\x53" }, "\"Uw==\"" },
        { { 1, (unsigned char *)"\x54" }, "\"VA==\"" },
        { { 1, (unsigned char *)"\x55" }, "\"VQ==\"" },
        { { 1, (unsigned char *)"\x56" }, "\"Vg==\"" },
        { { 1, (unsigned char *)"\x57" }, "\"Vw==\"" },
        { { 1, (unsigned char *)"\x58" }, "\"WA==\"" },
        { { 1, (unsigned char *)"\x59" }, "\"WQ==\"" },
        { { 1, (unsigned char *)"\x5a" }, "\"Wg==\"" },
        { { 1, (unsigned char *)"\x5b" }, "\"Ww==\"" },
        { { 1, (unsigned char *)"\x5c" }, "\"XA==\"" },
        { { 1, (unsigned char *)"\x5d" }, "\"XQ==\"" },
        { { 1, (unsigned char *)"\x5e" }, "\"Xg==\"" },
        { { 1, (unsigned char *)"\x5f" }, "\"Xw==\"" },
        { { 1, (unsigned char *)"\x60" }, "\"YA==\"" },
        { { 1, (unsigned char *)"\x61" }, "\"YQ==\"" },
        { { 1, (unsigned char *)"\x62" }, "\"Yg==\"" },
        { { 1, (unsigned char *)"\x63" }, "\"Yw==\"" },
        { { 1, (unsigned char *)"\x64" }, "\"ZA==\"" },
        { { 1, (unsigned char *)"\x65" }, "\"ZQ==\"" },
        { { 1, (unsigned char *)"\x66" }, "\"Zg==\"" },
        { { 1, (unsigned char *)"\x67" }, "\"Zw==\"" },
        { { 1, (unsigned char *)"\x68" }, "\"aA==\"" },
        { { 1, (unsigned char *)"\x69" }, "\"aQ==\"" },
        { { 1, (unsigned char *)"\x6a" }, "\"ag==\"" },
        { { 1, (unsigned char *)"\x6b" }, "\"aw==\"" },
        { { 1, (unsigned char *)"\x6c" }, "\"bA==\"" },
        { { 1, (unsigned char *)"\x6d" }, "\"bQ==\"" },
        { { 1, (unsigned char *)"\x6e" }, "\"bg==\"" },
        { { 1, (unsigned char *)"\x6f" }, "\"bw==\"" },
        { { 1, (unsigned char *)"\x70" }, "\"cA==\"" },
        { { 1, (unsigned char *)"\x71" }, "\"cQ==\"" },
        { { 1, (unsigned char *)"\x72" }, "\"cg==\"" },
        { { 1, (unsigned char *)"\x73" }, "\"cw==\"" },
        { { 1, (unsigned char *)"\x74" }, "\"dA==\"" },
        { { 1, (unsigned char *)"\x75" }, "\"dQ==\"" },
        { { 1, (unsigned char *)"\x76" }, "\"dg==\"" },
        { { 1, (unsigned char *)"\x77" }, "\"dw==\"" },
        { { 1, (unsigned char *)"\x78" }, "\"eA==\"" },
        { { 1, (unsigned char *)"\x79" }, "\"eQ==\"" },
        { { 1, (unsigned char *)"\x7a" }, "\"eg==\"" },
        { { 1, (unsigned char *)"\x7b" }, "\"ew==\"" },
        { { 1, (unsigned char *)"\x7c" }, "\"fA==\"" },
        { { 1, (unsigned char *)"\x7d" }, "\"fQ==\"" },
        { { 1, (unsigned char *)"\x7e" }, "\"fg==\"" },
        { { 1, (unsigned char *)"\x7f" }, "\"fw==\"" },
        { { 1, (unsigned char *)"\x80" }, "\"gA==\"" },
        { { 1, (unsigned char *)"\x81" }, "\"gQ==\"" },
        { { 1, (unsigned char *)"\x82" }, "\"gg==\"" },
        { { 1, (unsigned char *)"\x83" }, "\"gw==\"" },
        { { 1, (unsigned char *)"\x84" }, "\"hA==\"" },
        { { 1, (unsigned char *)"\x85" }, "\"hQ==\"" },
        { { 1, (unsigned char *)"\x86" }, "\"hg==\"" },
        { { 1, (unsigned char *)"\x87" }, "\"hw==\"" },
        { { 1, (unsigned char *)"\x88" }, "\"iA==\"" },
        { { 1, (unsigned char *)"\x89" }, "\"iQ==\"" },
        { { 1, (unsigned char *)"\x8a" }, "\"ig==\"" },
        { { 1, (unsigned char *)"\x8b" }, "\"iw==\"" },
        { { 1, (unsigned char *)"\x8c" }, "\"jA==\"" },
        { { 1, (unsigned char *)"\x8d" }, "\"jQ==\"" },
        { { 1, (unsigned char *)"\x8e" }, "\"jg==\"" },
        { { 1, (unsigned char *)"\x8f" }, "\"jw==\"" },
        { { 1, (unsigned char *)"\x90" }, "\"kA==\"" },
        { { 1, (unsigned char *)"\x91" }, "\"kQ==\"" },
        { { 1, (unsigned char *)"\x92" }, "\"kg==\"" },
        { { 1, (unsigned char *)"\x93" }, "\"kw==\"" },
        { { 1, (unsigned char *)"\x94" }, "\"lA==\"" },
        { { 1, (unsigned char *)"\x95" }, "\"lQ==\"" },
        { { 1, (unsigned char *)"\x96" }, "\"lg==\"" },
        { { 1, (unsigned char *)"\x97" }, "\"lw==\"" },
        { { 1, (unsigned char *)"\x98" }, "\"mA==\"" },
        { { 1, (unsigned char *)"\x99" }, "\"mQ==\"" },
        { { 1, (unsigned char *)"\x9a" }, "\"mg==\"" },
        { { 1, (unsigned char *)"\x9b" }, "\"mw==\"" },
        { { 1, (unsigned char *)"\x9c" }, "\"nA==\"" },
        { { 1, (unsigned char *)"\x9d" }, "\"nQ==\"" },
        { { 1, (unsigned char *)"\x9e" }, "\"ng==\"" },
        { { 1, (unsigned char *)"\x9f" }, "\"nw==\"" },
        { { 1, (unsigned char *)"\xa0" }, "\"oA==\"" },
        { { 1, (unsigned char *)"\xa1" }, "\"oQ==\"" },
        { { 1, (unsigned char *)"\xa2" }, "\"og==\"" },
        { { 1, (unsigned char *)"\xa3" }, "\"ow==\"" },
        { { 1, (unsigned char *)"\xa4" }, "\"pA==\"" },
        { { 1, (unsigned char *)"\xa5" }, "\"pQ==\"" },
        { { 1, (unsigned char *)"\xa6" }, "\"pg==\"" },
        { { 1, (unsigned char *)"\xa7" }, "\"pw==\"" },
        { { 1, (unsigned char *)"\xa8" }, "\"qA==\"" },
        { { 1, (unsigned char *)"\xa9" }, "\"qQ==\"" },
        { { 1, (unsigned char *)"\xaa" }, "\"qg==\"" },
        { { 1, (unsigned char *)"\xab" }, "\"qw==\"" },
        { { 1, (unsigned char *)"\xac" }, "\"rA==\"" },
        { { 1, (unsigned char *)"\xad" }, "\"rQ==\"" },
        { { 1, (unsigned char *)"\xae" }, "\"rg==\"" },
        { { 1, (unsigned char *)"\xaf" }, "\"rw==\"" },
        { { 1, (unsigned char *)"\xb0" }, "\"sA==\"" },
        { { 1, (unsigned char *)"\xb1" }, "\"sQ==\"" },
        { { 1, (unsigned char *)"\xb2" }, "\"sg==\"" },
        { { 1, (unsigned char *)"\xb3" }, "\"sw==\"" },
        { { 1, (unsigned char *)"\xb4" }, "\"tA==\"" },
        { { 1, (unsigned char *)"\xb5" }, "\"tQ==\"" },
        { { 1, (unsigned char *)"\xb6" }, "\"tg==\"" },
        { { 1, (unsigned char *)"\xb7" }, "\"tw==\"" },
        { { 1, (unsigned char *)"\xb8" }, "\"uA==\"" },
        { { 1, (unsigned char *)"\xb9" }, "\"uQ==\"" },
        { { 1, (unsigned char *)"\xba" }, "\"ug==\"" },
        { { 1, (unsigned char *)"\xbb" }, "\"uw==\"" },
        { { 1, (unsigned char *)"\xbc" }, "\"vA==\"" },
        { { 1, (unsigned char *)"\xbd" }, "\"vQ==\"" },
        { { 1, (unsigned char *)"\xbe" }, "\"vg==\"" },
        { { 1, (unsigned char *)"\xbf" }, "\"vw==\"" },
        { { 1, (unsigned char *)"\xc0" }, "\"wA==\"" },
        { { 1, (unsigned char *)"\xc1" }, "\"wQ==\"" },
        { { 1, (unsigned char *)"\xc2" }, "\"wg==\"" },
        { { 1, (unsigned char *)"\xc3" }, "\"ww==\"" },
        { { 1, (unsigned char *)"\xc4" }, "\"xA==\"" },
        { { 1, (unsigned char *)"\xc5" }, "\"xQ==\"" },
        { { 1, (unsigned char *)"\xc6" }, "\"xg==\"" },
        { { 1, (unsigned char *)"\xc7" }, "\"xw==\"" },
        { { 1, (unsigned char *)"\xc8" }, "\"yA==\"" },
        { { 1, (unsigned char *)"\xc9" }, "\"yQ==\"" },
        { { 1, (unsigned char *)"\xca" }, "\"yg==\"" },
        { { 1, (unsigned char *)"\xcb" }, "\"yw==\"" },
        { { 1, (unsigned char *)"\xcc" }, "\"zA==\"" },
        { { 1, (unsigned char *)"\xcd" }, "\"zQ==\"" },
        { { 1, (unsigned char *)"\xce" }, "\"zg==\"" },
        { { 1, (unsigned char *)"\xcf" }, "\"zw==\"" },
        { { 1, (unsigned char *)"\xd0" }, "\"0A==\"" },
        { { 1, (unsigned char *)"\xd1" }, "\"0Q==\"" },
        { { 1, (unsigned char *)"\xd2" }, "\"0g==\"" },
        { { 1, (unsigned char *)"\xd3" }, "\"0w==\"" },
        { { 1, (unsigned char *)"\xd4" }, "\"1A==\"" },
        { { 1, (unsigned char *)"\xd5" }, "\"1Q==\"" },
        { { 1, (unsigned char *)"\xd6" }, "\"1g==\"" },
        { { 1, (unsigned char *)"\xd7" }, "\"1w==\"" },
        { { 1, (unsigned char *)"\xd8" }, "\"2A==\"" },
        { { 1, (unsigned char *)"\xd9" }, "\"2Q==\"" },
        { { 1, (unsigned char *)"\xda" }, "\"2g==\"" },
        { { 1, (unsigned char *)"\xdb" }, "\"2w==\"" },
        { { 1, (unsigned char *)"\xdc" }, "\"3A==\"" },
        { { 1, (unsigned char *)"\xdd" }, "\"3Q==\"" },
        { { 1, (unsigned char *)"\xde" }, "\"3g==\"" },
        { { 1, (unsigned char *)"\xdf" }, "\"3w==\"" },
        { { 1, (unsigned char *)"\xe0" }, "\"4A==\"" },
        { { 1, (unsigned char *)"\xe1" }, "\"4Q==\"" },
        { { 1, (unsigned char *)"\xe2" }, "\"4g==\"" },
        { { 1, (unsigned char *)"\xe3" }, "\"4w==\"" },
        { { 1, (unsigned char *)"\xe4" }, "\"5A==\"" },
        { { 1, (unsigned char *)"\xe5" }, "\"5Q==\"" },
        { { 1, (unsigned char *)"\xe6" }, "\"5g==\"" },
        { { 1, (unsigned char *)"\xe7" }, "\"5w==\"" },
        { { 1, (unsigned char *)"\xe8" }, "\"6A==\"" },
        { { 1, (unsigned char *)"\xe9" }, "\"6Q==\"" },
        { { 1, (unsigned char *)"\xea" }, "\"6g==\"" },
        { { 1, (unsigned char *)"\xeb" }, "\"6w==\"" },
        { { 1, (unsigned char *)"\xec" }, "\"7A==\"" },
        { { 1, (unsigned char *)"\xed" }, "\"7Q==\"" },
        { { 1, (unsigned char *)"\xee" }, "\"7g==\"" },
        { { 1, (unsigned char *)"\xef" }, "\"7w==\"" },
        { { 1, (unsigned char *)"\xf0" }, "\"8A==\"" },
        { { 1, (unsigned char *)"\xf1" }, "\"8Q==\"" },
        { { 1, (unsigned char *)"\xf2" }, "\"8g==\"" },
        { { 1, (unsigned char *)"\xf3" }, "\"8w==\"" },
        { { 1, (unsigned char *)"\xf4" }, "\"9A==\"" },
        { { 1, (unsigned char *)"\xf5" }, "\"9Q==\"" },
        { { 1, (unsigned char *)"\xf6" }, "\"9g==\"" },
        { { 1, (unsigned char *)"\xf7" }, "\"9w==\"" },
        { { 1, (unsigned char *)"\xf8" }, "\"-A==\"" },
        { { 1, (unsigned char *)"\xf9" }, "\"-Q==\"" },
        { { 1, (unsigned char *)"\xfa" }, "\"-g==\"" },
        { { 1, (unsigned char *)"\xfb" }, "\"-w==\"" },
        { { 1, (unsigned char *)"\xfc" }, "\"_A==\"" },
        { { 1, (unsigned char *)"\xfd" }, "\"_Q==\"" },
        { { 1, (unsigned char *)"\xfe" }, "\"_g==\"" },
        { { 1, (unsigned char *)"\xff" }, "\"_w==\"" },
        { { 2, (unsigned char *)"\x00\x00" }, "\"AAA=\"" },
        { { 2, (unsigned char *)"\x00\x11" }, "\"ABE=\"" },
        { { 2, (unsigned char *)"\x00\x22" }, "\"ACI=\"" },
        { { 2, (unsigned char *)"\x00\x33" }, "\"ADM=\"" },
        { { 2, (unsigned char *)"\x00\x44" }, "\"AEQ=\"" },
        { { 2, (unsigned char *)"\x00\x55" }, "\"AFU=\"" },
        { { 2, (unsigned char *)"\x00\x66" }, "\"AGY=\"" },
        { { 2, (unsigned char *)"\x00\x77" }, "\"AHc=\"" },
        { { 2, (unsigned char *)"\x00\x88" }, "\"AIg=\"" },
        { { 2, (unsigned char *)"\x00\x99" }, "\"AJk=\"" },
        { { 2, (unsigned char *)"\x00\xaa" }, "\"AKo=\"" },
        { { 2, (unsigned char *)"\x00\xbb" }, "\"ALs=\"" },
        { { 2, (unsigned char *)"\x00\xcc" }, "\"AMw=\"" },
        { { 2, (unsigned char *)"\x00\xdd" }, "\"AN0=\"" },
        { { 2, (unsigned char *)"\x00\xee" }, "\"AO4=\"" },
        { { 2, (unsigned char *)"\x00\xff" }, "\"AP8=\"" },
        { { 2, (unsigned char *)"\x11\x00" }, "\"EQA=\"" },
        { { 2, (unsigned char *)"\x11\x11" }, "\"ERE=\"" },
        { { 2, (unsigned char *)"\x11\x22" }, "\"ESI=\"" },
        { { 2, (unsigned char *)"\x11\x33" }, "\"ETM=\"" },
        { { 2, (unsigned char *)"\x11\x44" }, "\"EUQ=\"" },
        { { 2, (unsigned char *)"\x11\x55" }, "\"EVU=\"" },
        { { 2, (unsigned char *)"\x11\x66" }, "\"EWY=\"" },
        { { 2, (unsigned char *)"\x11\x77" }, "\"EXc=\"" },
        { { 2, (unsigned char *)"\x11\x88" }, "\"EYg=\"" },
        { { 2, (unsigned char *)"\x11\x99" }, "\"EZk=\"" },
        { { 2, (unsigned char *)"\x11\xaa" }, "\"Eao=\"" },
        { { 2, (unsigned char *)"\x11\xbb" }, "\"Ebs=\"" },
        { { 2, (unsigned char *)"\x11\xcc" }, "\"Ecw=\"" },
        { { 2, (unsigned char *)"\x11\xdd" }, "\"Ed0=\"" },
        { { 2, (unsigned char *)"\x11\xee" }, "\"Ee4=\"" },
        { { 2, (unsigned char *)"\x11\xff" }, "\"Ef8=\"" },
        { { 2, (unsigned char *)"\x22\x00" }, "\"IgA=\"" },
        { { 2, (unsigned char *)"\x22\x11" }, "\"IhE=\"" },
        { { 2, (unsigned char *)"\x22\x22" }, "\"IiI=\"" },
        { { 2, (unsigned char *)"\x22\x33" }, "\"IjM=\"" },
        { { 2, (unsigned char *)"\x22\x44" }, "\"IkQ=\"" },
        { { 2, (unsigned char *)"\x22\x55" }, "\"IlU=\"" },
        { { 2, (unsigned char *)"\x22\x66" }, "\"ImY=\"" },
        { { 2, (unsigned char *)"\x22\x77" }, "\"Inc=\"" },
        { { 2, (unsigned char *)"\x22\x88" }, "\"Iog=\"" },
        { { 2, (unsigned char *)"\x22\x99" }, "\"Ipk=\"" },
        { { 2, (unsigned char *)"\x22\xaa" }, "\"Iqo=\"" },
        { { 2, (unsigned char *)"\x22\xbb" }, "\"Irs=\"" },
        { { 2, (unsigned char *)"\x22\xcc" }, "\"Isw=\"" },
        { { 2, (unsigned char *)"\x22\xdd" }, "\"It0=\"" },
        { { 2, (unsigned char *)"\x22\xee" }, "\"Iu4=\"" },
        { { 2, (unsigned char *)"\x22\xff" }, "\"Iv8=\"" },
        { { 2, (unsigned char *)"\x33\x00" }, "\"MwA=\"" },
        { { 2, (unsigned char *)"\x33\x11" }, "\"MxE=\"" },
        { { 2, (unsigned char *)"\x33\x22" }, "\"MyI=\"" },
        { { 2, (unsigned char *)"\x33\x33" }, "\"MzM=\"" },
        { { 2, (unsigned char *)"\x33\x44" }, "\"M0Q=\"" },
        { { 2, (unsigned char *)"\x33\x55" }, "\"M1U=\"" },
        { { 2, (unsigned char *)"\x33\x66" }, "\"M2Y=\"" },
        { { 2, (unsigned char *)"\x33\x77" }, "\"M3c=\"" },
        { { 2, (unsigned char *)"\x33\x88" }, "\"M4g=\"" },
        { { 2, (unsigned char *)"\x33\x99" }, "\"M5k=\"" },
        { { 2, (unsigned char *)"\x33\xaa" }, "\"M6o=\"" },
        { { 2, (unsigned char *)"\x33\xbb" }, "\"M7s=\"" },
        { { 2, (unsigned char *)"\x33\xcc" }, "\"M8w=\"" },
        { { 2, (unsigned char *)"\x33\xdd" }, "\"M90=\"" },
        { { 2, (unsigned char *)"\x33\xee" }, "\"M-4=\"" },
        { { 2, (unsigned char *)"\x33\xff" }, "\"M_8=\"" },
        { { 2, (unsigned char *)"\x44\x00" }, "\"RAA=\"" },
        { { 2, (unsigned char *)"\x44\x11" }, "\"RBE=\"" },
        { { 2, (unsigned char *)"\x44\x22" }, "\"RCI=\"" },
        { { 2, (unsigned char *)"\x44\x33" }, "\"RDM=\"" },
        { { 2, (unsigned char *)"\x44\x44" }, "\"REQ=\"" },
        { { 2, (unsigned char *)"\x44\x55" }, "\"RFU=\"" },
        { { 2, (unsigned char *)"\x44\x66" }, "\"RGY=\"" },
        { { 2, (unsigned char *)"\x44\x77" }, "\"RHc=\"" },
        { { 2, (unsigned char *)"\x44\x88" }, "\"RIg=\"" },
        { { 2, (unsigned char *)"\x44\x99" }, "\"RJk=\"" },
        { { 2, (unsigned char *)"\x44\xaa" }, "\"RKo=\"" },
        { { 2, (unsigned char *)"\x44\xbb" }, "\"RLs=\"" },
        { { 2, (unsigned char *)"\x44\xcc" }, "\"RMw=\"" },
        { { 2, (unsigned char *)"\x44\xdd" }, "\"RN0=\"" },
        { { 2, (unsigned char *)"\x44\xee" }, "\"RO4=\"" },
        { { 2, (unsigned char *)"\x44\xff" }, "\"RP8=\"" },
        { { 2, (unsigned char *)"\x55\x00" }, "\"VQA=\"" },
        { { 2, (unsigned char *)"\x55\x11" }, "\"VRE=\"" },
        { { 2, (unsigned char *)"\x55\x22" }, "\"VSI=\"" },
        { { 2, (unsigned char *)"\x55\x33" }, "\"VTM=\"" },
        { { 2, (unsigned char *)"\x55\x44" }, "\"VUQ=\"" },
        { { 2, (unsigned char *)"\x55\x55" }, "\"VVU=\"" },
        { { 2, (unsigned char *)"\x55\x66" }, "\"VWY=\"" },
        { { 2, (unsigned char *)"\x55\x77" }, "\"VXc=\"" },
        { { 2, (unsigned char *)"\x55\x88" }, "\"VYg=\"" },
        { { 2, (unsigned char *)"\x55\x99" }, "\"VZk=\"" },
        { { 2, (unsigned char *)"\x55\xaa" }, "\"Vao=\"" },
        { { 2, (unsigned char *)"\x55\xbb" }, "\"Vbs=\"" },
        { { 2, (unsigned char *)"\x55\xcc" }, "\"Vcw=\"" },
        { { 2, (unsigned char *)"\x55\xdd" }, "\"Vd0=\"" },
        { { 2, (unsigned char *)"\x55\xee" }, "\"Ve4=\"" },
        { { 2, (unsigned char *)"\x55\xff" }, "\"Vf8=\"" },
        { { 2, (unsigned char *)"\x66\x00" }, "\"ZgA=\"" },
        { { 2, (unsigned char *)"\x66\x11" }, "\"ZhE=\"" },
        { { 2, (unsigned char *)"\x66\x22" }, "\"ZiI=\"" },
        { { 2, (unsigned char *)"\x66\x33" }, "\"ZjM=\"" },
        { { 2, (unsigned char *)"\x66\x44" }, "\"ZkQ=\"" },
        { { 2, (unsigned char *)"\x66\x55" }, "\"ZlU=\"" },
        { { 2, (unsigned char *)"\x66\x66" }, "\"ZmY=\"" },
        { { 2, (unsigned char *)"\x66\x77" }, "\"Znc=\"" },
        { { 2, (unsigned char *)"\x66\x88" }, "\"Zog=\"" },
        { { 2, (unsigned char *)"\x66\x99" }, "\"Zpk=\"" },
        { { 2, (unsigned char *)"\x66\xaa" }, "\"Zqo=\"" },
        { { 2, (unsigned char *)"\x66\xbb" }, "\"Zrs=\"" },
        { { 2, (unsigned char *)"\x66\xcc" }, "\"Zsw=\"" },
        { { 2, (unsigned char *)"\x66\xdd" }, "\"Zt0=\"" },
        { { 2, (unsigned char *)"\x66\xee" }, "\"Zu4=\"" },
        { { 2, (unsigned char *)"\x66\xff" }, "\"Zv8=\"" },
        { { 2, (unsigned char *)"\x77\x00" }, "\"dwA=\"" },
        { { 2, (unsigned char *)"\x77\x11" }, "\"dxE=\"" },
        { { 2, (unsigned char *)"\x77\x22" }, "\"dyI=\"" },
        { { 2, (unsigned char *)"\x77\x33" }, "\"dzM=\"" },
        { { 2, (unsigned char *)"\x77\x44" }, "\"d0Q=\"" },
        { { 2, (unsigned char *)"\x77\x55" }, "\"d1U=\"" },
        { { 2, (unsigned char *)"\x77\x66" }, "\"d2Y=\"" },
        { { 2, (unsigned char *)"\x77\x77" }, "\"d3c=\"" },
        { { 2, (unsigned char *)"\x77\x88" }, "\"d4g=\"" },
        { { 2, (unsigned char *)"\x77\x99" }, "\"d5k=\"" },
        { { 2, (unsigned char *)"\x77\xaa" }, "\"d6o=\"" },
        { { 2, (unsigned char *)"\x77\xbb" }, "\"d7s=\"" },
        { { 2, (unsigned char *)"\x77\xcc" }, "\"d8w=\"" },
        { { 2, (unsigned char *)"\x77\xdd" }, "\"d90=\"" },
        { { 2, (unsigned char *)"\x77\xee" }, "\"d-4=\"" },
        { { 2, (unsigned char *)"\x77\xff" }, "\"d_8=\"" },
        { { 2, (unsigned char *)"\x88\x00" }, "\"iAA=\"" },
        { { 2, (unsigned char *)"\x88\x11" }, "\"iBE=\"" },
        { { 2, (unsigned char *)"\x88\x22" }, "\"iCI=\"" },
        { { 2, (unsigned char *)"\x88\x33" }, "\"iDM=\"" },
        { { 2, (unsigned char *)"\x88\x44" }, "\"iEQ=\"" },
        { { 2, (unsigned char *)"\x88\x55" }, "\"iFU=\"" },
        { { 2, (unsigned char *)"\x88\x66" }, "\"iGY=\"" },
        { { 2, (unsigned char *)"\x88\x77" }, "\"iHc=\"" },
        { { 2, (unsigned char *)"\x88\x88" }, "\"iIg=\"" },
        { { 2, (unsigned char *)"\x88\x99" }, "\"iJk=\"" },
        { { 2, (unsigned char *)"\x88\xaa" }, "\"iKo=\"" },
        { { 2, (unsigned char *)"\x88\xbb" }, "\"iLs=\"" },
        { { 2, (unsigned char *)"\x88\xcc" }, "\"iMw=\"" },
        { { 2, (unsigned char *)"\x88\xdd" }, "\"iN0=\"" },
        { { 2, (unsigned char *)"\x88\xee" }, "\"iO4=\"" },
        { { 2, (unsigned char *)"\x88\xff" }, "\"iP8=\"" },
        { { 2, (unsigned char *)"\x99\x00" }, "\"mQA=\"" },
        { { 2, (unsigned char *)"\x99\x11" }, "\"mRE=\"" },
        { { 2, (unsigned char *)"\x99\x22" }, "\"mSI=\"" },
        { { 2, (unsigned char *)"\x99\x33" }, "\"mTM=\"" },
        { { 2, (unsigned char *)"\x99\x44" }, "\"mUQ=\"" },
        { { 2, (unsigned char *)"\x99\x55" }, "\"mVU=\"" },
        { { 2, (unsigned char *)"\x99\x66" }, "\"mWY=\"" },
        { { 2, (unsigned char *)"\x99\x77" }, "\"mXc=\"" },
        { { 2, (unsigned char *)"\x99\x88" }, "\"mYg=\"" },
        { { 2, (unsigned char *)"\x99\x99" }, "\"mZk=\"" },
        { { 2, (unsigned char *)"\x99\xaa" }, "\"mao=\"" },
        { { 2, (unsigned char *)"\x99\xbb" }, "\"mbs=\"" },
        { { 2, (unsigned char *)"\x99\xcc" }, "\"mcw=\"" },
        { { 2, (unsigned char *)"\x99\xdd" }, "\"md0=\"" },
        { { 2, (unsigned char *)"\x99\xee" }, "\"me4=\"" },
        { { 2, (unsigned char *)"\x99\xff" }, "\"mf8=\"" },
        { { 2, (unsigned char *)"\xaa\x00" }, "\"qgA=\"" },
        { { 2, (unsigned char *)"\xaa\x11" }, "\"qhE=\"" },
        { { 2, (unsigned char *)"\xaa\x22" }, "\"qiI=\"" },
        { { 2, (unsigned char *)"\xaa\x33" }, "\"qjM=\"" },
        { { 2, (unsigned char *)"\xaa\x44" }, "\"qkQ=\"" },
        { { 2, (unsigned char *)"\xaa\x55" }, "\"qlU=\"" },
        { { 2, (unsigned char *)"\xaa\x66" }, "\"qmY=\"" },
        { { 2, (unsigned char *)"\xaa\x77" }, "\"qnc=\"" },
        { { 2, (unsigned char *)"\xaa\x88" }, "\"qog=\"" },
        { { 2, (unsigned char *)"\xaa\x99" }, "\"qpk=\"" },
        { { 2, (unsigned char *)"\xaa\xaa" }, "\"qqo=\"" },
        { { 2, (unsigned char *)"\xaa\xbb" }, "\"qrs=\"" },
        { { 2, (unsigned char *)"\xaa\xcc" }, "\"qsw=\"" },
        { { 2, (unsigned char *)"\xaa\xdd" }, "\"qt0=\"" },
        { { 2, (unsigned char *)"\xaa\xee" }, "\"qu4=\"" },
        { { 2, (unsigned char *)"\xaa\xff" }, "\"qv8=\"" },
        { { 2, (unsigned char *)"\xbb\x00" }, "\"uwA=\"" },
        { { 2, (unsigned char *)"\xbb\x11" }, "\"uxE=\"" },
        { { 2, (unsigned char *)"\xbb\x22" }, "\"uyI=\"" },
        { { 2, (unsigned char *)"\xbb\x33" }, "\"uzM=\"" },
        { { 2, (unsigned char *)"\xbb\x44" }, "\"u0Q=\"" },
        { { 2, (unsigned char *)"\xbb\x55" }, "\"u1U=\"" },
        { { 2, (unsigned char *)"\xbb\x66" }, "\"u2Y=\"" },
        { { 2, (unsigned char *)"\xbb\x77" }, "\"u3c=\"" },
        { { 2, (unsigned char *)"\xbb\x88" }, "\"u4g=\"" },
        { { 2, (unsigned char *)"\xbb\x99" }, "\"u5k=\"" },
        { { 2, (unsigned char *)"\xbb\xaa" }, "\"u6o=\"" },
        { { 2, (unsigned char *)"\xbb\xbb" }, "\"u7s=\"" },
        { { 2, (unsigned char *)"\xbb\xcc" }, "\"u8w=\"" },
        { { 2, (unsigned char *)"\xbb\xdd" }, "\"u90=\"" },
        { { 2, (unsigned char *)"\xbb\xee" }, "\"u-4=\"" },
        { { 2, (unsigned char *)"\xbb\xff" }, "\"u_8=\"" },
        { { 2, (unsigned char *)"\xcc\x00" }, "\"zAA=\"" },
        { { 2, (unsigned char *)"\xcc\x11" }, "\"zBE=\"" },
        { { 2, (unsigned char *)"\xcc\x22" }, "\"zCI=\"" },
        { { 2, (unsigned char *)"\xcc\x33" }, "\"zDM=\"" },
        { { 2, (unsigned char *)"\xcc\x44" }, "\"zEQ=\"" },
        { { 2, (unsigned char *)"\xcc\x55" }, "\"zFU=\"" },
        { { 2, (unsigned char *)"\xcc\x66" }, "\"zGY=\"" },
        { { 2, (unsigned char *)"\xcc\x77" }, "\"zHc=\"" },
        { { 2, (unsigned char *)"\xcc\x88" }, "\"zIg=\"" },
        { { 2, (unsigned char *)"\xcc\x99" }, "\"zJk=\"" },
        { { 2, (unsigned char *)"\xcc\xaa" }, "\"zKo=\"" },
        { { 2, (unsigned char *)"\xcc\xbb" }, "\"zLs=\"" },
        { { 2, (unsigned char *)"\xcc\xcc" }, "\"zMw=\"" },
        { { 2, (unsigned char *)"\xcc\xdd" }, "\"zN0=\"" },
        { { 2, (unsigned char *)"\xcc\xee" }, "\"zO4=\"" },
        { { 2, (unsigned char *)"\xcc\xff" }, "\"zP8=\"" },
        { { 2, (unsigned char *)"\xdd\x00" }, "\"3QA=\"" },
        { { 2, (unsigned char *)"\xdd\x11" }, "\"3RE=\"" },
        { { 2, (unsigned char *)"\xdd\x22" }, "\"3SI=\"" },
        { { 2, (unsigned char *)"\xdd\x33" }, "\"3TM=\"" },
        { { 2, (unsigned char *)"\xdd\x44" }, "\"3UQ=\"" },
        { { 2, (unsigned char *)"\xdd\x55" }, "\"3VU=\"" },
        { { 2, (unsigned char *)"\xdd\x66" }, "\"3WY=\"" },
        { { 2, (unsigned char *)"\xdd\x77" }, "\"3Xc=\"" },
        { { 2, (unsigned char *)"\xdd\x88" }, "\"3Yg=\"" },
        { { 2, (unsigned char *)"\xdd\x99" }, "\"3Zk=\"" },
        { { 2, (unsigned char *)"\xdd\xaa" }, "\"3ao=\"" },
        { { 2, (unsigned char *)"\xdd\xbb" }, "\"3bs=\"" },
        { { 2, (unsigned char *)"\xdd\xcc" }, "\"3cw=\"" },
        { { 2, (unsigned char *)"\xdd\xdd" }, "\"3d0=\"" },
        { { 2, (unsigned char *)"\xdd\xee" }, "\"3e4=\"" },
        { { 2, (unsigned char *)"\xdd\xff" }, "\"3f8=\"" },
        { { 2, (unsigned char *)"\xee\x00" }, "\"7gA=\"" },
        { { 2, (unsigned char *)"\xee\x11" }, "\"7hE=\"" },
        { { 2, (unsigned char *)"\xee\x22" }, "\"7iI=\"" },
        { { 2, (unsigned char *)"\xee\x33" }, "\"7jM=\"" },
        { { 2, (unsigned char *)"\xee\x44" }, "\"7kQ=\"" },
        { { 2, (unsigned char *)"\xee\x55" }, "\"7lU=\"" },
        { { 2, (unsigned char *)"\xee\x66" }, "\"7mY=\"" },
        { { 2, (unsigned char *)"\xee\x77" }, "\"7nc=\"" },
        { { 2, (unsigned char *)"\xee\x88" }, "\"7og=\"" },
        { { 2, (unsigned char *)"\xee\x99" }, "\"7pk=\"" },
        { { 2, (unsigned char *)"\xee\xaa" }, "\"7qo=\"" },
        { { 2, (unsigned char *)"\xee\xbb" }, "\"7rs=\"" },
        { { 2, (unsigned char *)"\xee\xcc" }, "\"7sw=\"" },
        { { 2, (unsigned char *)"\xee\xdd" }, "\"7t0=\"" },
        { { 2, (unsigned char *)"\xee\xee" }, "\"7u4=\"" },
        { { 2, (unsigned char *)"\xee\xff" }, "\"7v8=\"" },
        { { 2, (unsigned char *)"\xff\x00" }, "\"_wA=\"" },
        { { 2, (unsigned char *)"\xff\x11" }, "\"_xE=\"" },
        { { 2, (unsigned char *)"\xff\x22" }, "\"_yI=\"" },
        { { 2, (unsigned char *)"\xff\x33" }, "\"_zM=\"" },
        { { 2, (unsigned char *)"\xff\x44" }, "\"_0Q=\"" },
        { { 2, (unsigned char *)"\xff\x55" }, "\"_1U=\"" },
        { { 2, (unsigned char *)"\xff\x66" }, "\"_2Y=\"" },
        { { 2, (unsigned char *)"\xff\x77" }, "\"_3c=\"" },
        { { 2, (unsigned char *)"\xff\x88" }, "\"_4g=\"" },
        { { 2, (unsigned char *)"\xff\x99" }, "\"_5k=\"" },
        { { 2, (unsigned char *)"\xff\xaa" }, "\"_6o=\"" },
        { { 2, (unsigned char *)"\xff\xbb" }, "\"_7s=\"" },
        { { 2, (unsigned char *)"\xff\xcc" }, "\"_8w=\"" },
        { { 2, (unsigned char *)"\xff\xdd" }, "\"_90=\"" },
        { { 2, (unsigned char *)"\xff\xee" }, "\"_-4=\"" },
        { { 2, (unsigned char *)"\xff\xff" }, "\"__8=\"" },
        { { 3, (unsigned char *)"\x00\x00\x00" }, "\"AAAA\"" },
        { { 3, (unsigned char *)"\x00\x00\x2f" }, "\"AAAv\"" },
        { { 3, (unsigned char *)"\x00\x00\x5e" }, "\"AABe\"" },
        { { 3, (unsigned char *)"\x00\x00\x8d" }, "\"AACN\"" },
        { { 3, (unsigned char *)"\x00\x00\xbc" }, "\"AAC8\"" },
        { { 3, (unsigned char *)"\x00\x00\xeb" }, "\"AADr\"" },
        { { 3, (unsigned char *)"\x00\x2f\x00" }, "\"AC8A\"" },
        { { 3, (unsigned char *)"\x00\x2f\x2f" }, "\"AC8v\"" },
        { { 3, (unsigned char *)"\x00\x2f\x5e" }, "\"AC9e\"" },
        { { 3, (unsigned char *)"\x00\x2f\x8d" }, "\"AC-N\"" },
        { { 3, (unsigned char *)"\x00\x2f\xbc" }, "\"AC-8\"" },
        { { 3, (unsigned char *)"\x00\x2f\xeb" }, "\"AC_r\"" },
        { { 3, (unsigned char *)"\x00\x5e\x00" }, "\"AF4A\"" },
        { { 3, (unsigned char *)"\x00\x5e\x2f" }, "\"AF4v\"" },
        { { 3, (unsigned char *)"\x00\x5e\x5e" }, "\"AF5e\"" },
        { { 3, (unsigned char *)"\x00\x5e\x8d" }, "\"AF6N\"" },
        { { 3, (unsigned char *)"\x00\x5e\xbc" }, "\"AF68\"" },
        { { 3, (unsigned char *)"\x00\x5e\xeb" }, "\"AF7r\"" },
        { { 3, (unsigned char *)"\x00\x8d\x00" }, "\"AI0A\"" },
        { { 3, (unsigned char *)"\x00\x8d\x2f" }, "\"AI0v\"" },
        { { 3, (unsigned char *)"\x00\x8d\x5e" }, "\"AI1e\"" },
        { { 3, (unsigned char *)"\x00\x8d\x8d" }, "\"AI2N\"" },
        { { 3, (unsigned char *)"\x00\x8d\xbc" }, "\"AI28\"" },
        { { 3, (unsigned char *)"\x00\x8d\xeb" }, "\"AI3r\"" },
        { { 3, (unsigned char *)"\x00\xbc\x00" }, "\"ALwA\"" },
        { { 3, (unsigned char *)"\x00\xbc\x2f" }, "\"ALwv\"" },
        { { 3, (unsigned char *)"\x00\xbc\x5e" }, "\"ALxe\"" },
        { { 3, (unsigned char *)"\x00\xbc\x8d" }, "\"ALyN\"" },
        { { 3, (unsigned char *)"\x00\xbc\xbc" }, "\"ALy8\"" },
        { { 3, (unsigned char *)"\x00\xbc\xeb" }, "\"ALzr\"" },
        { { 3, (unsigned char *)"\x00\xeb\x00" }, "\"AOsA\"" },
        { { 3, (unsigned char *)"\x00\xeb\x2f" }, "\"AOsv\"" },
        { { 3, (unsigned char *)"\x00\xeb\x5e" }, "\"AOte\"" },
        { { 3, (unsigned char *)"\x00\xeb\x8d" }, "\"AOuN\"" },
        { { 3, (unsigned char *)"\x00\xeb\xbc" }, "\"AOu8\"" },
        { { 3, (unsigned char *)"\x00\xeb\xeb" }, "\"AOvr\"" },
        { { 3, (unsigned char *)"\x2f\x00\x00" }, "\"LwAA\"" },
        { { 3, (unsigned char *)"\x2f\x00\x2f" }, "\"LwAv\"" },
        { { 3, (unsigned char *)"\x2f\x00\x5e" }, "\"LwBe\"" },
        { { 3, (unsigned char *)"\x2f\x00\x8d" }, "\"LwCN\"" },
        { { 3, (unsigned char *)"\x2f\x00\xbc" }, "\"LwC8\"" },
        { { 3, (unsigned char *)"\x2f\x00\xeb" }, "\"LwDr\"" },
        { { 3, (unsigned char *)"\x2f\x2f\x00" }, "\"Ly8A\"" },
        { { 3, (unsigned char *)"\x2f\x2f\x2f" }, "\"Ly8v\"" },
        { { 3, (unsigned char *)"\x2f\x2f\x5e" }, "\"Ly9e\"" },
        { { 3, (unsigned char *)"\x2f\x2f\x8d" }, "\"Ly-N\"" },
        { { 3, (unsigned char *)"\x2f\x2f\xbc" }, "\"Ly-8\"" },
        { { 3, (unsigned char *)"\x2f\x2f\xeb" }, "\"Ly_r\"" },
        { { 3, (unsigned char *)"\x2f\x5e\x00" }, "\"L14A\"" },
        { { 3, (unsigned char *)"\x2f\x5e\x2f" }, "\"L14v\"" },
        { { 3, (unsigned char *)"\x2f\x5e\x5e" }, "\"L15e\"" },
        { { 3, (unsigned char *)"\x2f\x5e\x8d" }, "\"L16N\"" },
        { { 3, (unsigned char *)"\x2f\x5e\xbc" }, "\"L168\"" },
        { { 3, (unsigned char *)"\x2f\x5e\xeb" }, "\"L17r\"" },
        { { 3, (unsigned char *)"\x2f\x8d\x00" }, "\"L40A\"" },
        { { 3, (unsigned char *)"\x2f\x8d\x2f" }, "\"L40v\"" },
        { { 3, (unsigned char *)"\x2f\x8d\x5e" }, "\"L41e\"" },
        { { 3, (unsigned char *)"\x2f\x8d\x8d" }, "\"L42N\"" },
        { { 3, (unsigned char *)"\x2f\x8d\xbc" }, "\"L428\"" },
        { { 3, (unsigned char *)"\x2f\x8d\xeb" }, "\"L43r\"" },
        { { 3, (unsigned char *)"\x2f\xbc\x00" }, "\"L7wA\"" },
        { { 3, (unsigned char *)"\x2f\xbc\x2f" }, "\"L7wv\"" },
        { { 3, (unsigned char *)"\x2f\xbc\x5e" }, "\"L7xe\"" },
        { { 3, (unsigned char *)"\x2f\xbc\x8d" }, "\"L7yN\"" },
        { { 3, (unsigned char *)"\x2f\xbc\xbc" }, "\"L7y8\"" },
        { { 3, (unsigned char *)"\x2f\xbc\xeb" }, "\"L7zr\"" },
        { { 3, (unsigned char *)"\x2f\xeb\x00" }, "\"L-sA\"" },
        { { 3, (unsigned char *)"\x2f\xeb\x2f" }, "\"L-sv\"" },
        { { 3, (unsigned char *)"\x2f\xeb\x5e" }, "\"L-te\"" },
        { { 3, (unsigned char *)"\x2f\xeb\x8d" }, "\"L-uN\"" },
        { { 3, (unsigned char *)"\x2f\xeb\xbc" }, "\"L-u8\"" },
        { { 3, (unsigned char *)"\x2f\xeb\xeb" }, "\"L-vr\"" },
        { { 3, (unsigned char *)"\x5e\x00\x00" }, "\"XgAA\"" },
        { { 3, (unsigned char *)"\x5e\x00\x2f" }, "\"XgAv\"" },
        { { 3, (unsigned char *)"\x5e\x00\x5e" }, "\"XgBe\"" },
        { { 3, (unsigned char *)"\x5e\x00\x8d" }, "\"XgCN\"" },
        { { 3, (unsigned char *)"\x5e\x00\xbc" }, "\"XgC8\"" },
        { { 3, (unsigned char *)"\x5e\x00\xeb" }, "\"XgDr\"" },
        { { 3, (unsigned char *)"\x5e\x2f\x00" }, "\"Xi8A\"" },
        { { 3, (unsigned char *)"\x5e\x2f\x2f" }, "\"Xi8v\"" },
        { { 3, (unsigned char *)"\x5e\x2f\x5e" }, "\"Xi9e\"" },
        { { 3, (unsigned char *)"\x5e\x2f\x8d" }, "\"Xi-N\"" },
        { { 3, (unsigned char *)"\x5e\x2f\xbc" }, "\"Xi-8\"" },
        { { 3, (unsigned char *)"\x5e\x2f\xeb" }, "\"Xi_r\"" },
        { { 3, (unsigned char *)"\x5e\x5e\x00" }, "\"Xl4A\"" },
        { { 3, (unsigned char *)"\x5e\x5e\x2f" }, "\"Xl4v\"" },
        { { 3, (unsigned char *)"\x5e\x5e\x5e" }, "\"Xl5e\"" },
        { { 3, (unsigned char *)"\x5e\x5e\x8d" }, "\"Xl6N\"" },
        { { 3, (unsigned char *)"\x5e\x5e\xbc" }, "\"Xl68\"" },
        { { 3, (unsigned char *)"\x5e\x5e\xeb" }, "\"Xl7r\"" },
        { { 3, (unsigned char *)"\x5e\x8d\x00" }, "\"Xo0A\"" },
        { { 3, (unsigned char *)"\x5e\x8d\x2f" }, "\"Xo0v\"" },
        { { 3, (unsigned char *)"\x5e\x8d\x5e" }, "\"Xo1e\"" },
        { { 3, (unsigned char *)"\x5e\x8d\x8d" }, "\"Xo2N\"" },
        { { 3, (unsigned char *)"\x5e\x8d\xbc" }, "\"Xo28\"" },
        { { 3, (unsigned char *)"\x5e\x8d\xeb" }, "\"Xo3r\"" },
        { { 3, (unsigned char *)"\x5e\xbc\x00" }, "\"XrwA\"" },
        { { 3, (unsigned char *)"\x5e\xbc\x2f" }, "\"Xrwv\"" },
        { { 3, (unsigned char *)"\x5e\xbc\x5e" }, "\"Xrxe\"" },
        { { 3, (unsigned char *)"\x5e\xbc\x8d" }, "\"XryN\"" },
        { { 3, (unsigned char *)"\x5e\xbc\xbc" }, "\"Xry8\"" },
        { { 3, (unsigned char *)"\x5e\xbc\xeb" }, "\"Xrzr\"" },
        { { 3, (unsigned char *)"\x5e\xeb\x00" }, "\"XusA\"" },
        { { 3, (unsigned char *)"\x5e\xeb\x2f" }, "\"Xusv\"" },
        { { 3, (unsigned char *)"\x5e\xeb\x5e" }, "\"Xute\"" },
        { { 3, (unsigned char *)"\x5e\xeb\x8d" }, "\"XuuN\"" },
        { { 3, (unsigned char *)"\x5e\xeb\xbc" }, "\"Xuu8\"" },
        { { 3, (unsigned char *)"\x5e\xeb\xeb" }, "\"Xuvr\"" },
        { { 3, (unsigned char *)"\x8d\x00\x00" }, "\"jQAA\"" },
        { { 3, (unsigned char *)"\x8d\x00\x2f" }, "\"jQAv\"" },
        { { 3, (unsigned char *)"\x8d\x00\x5e" }, "\"jQBe\"" },
        { { 3, (unsigned char *)"\x8d\x00\x8d" }, "\"jQCN\"" },
        { { 3, (unsigned char *)"\x8d\x00\xbc" }, "\"jQC8\"" },
        { { 3, (unsigned char *)"\x8d\x00\xeb" }, "\"jQDr\"" },
        { { 3, (unsigned char *)"\x8d\x2f\x00" }, "\"jS8A\"" },
        { { 3, (unsigned char *)"\x8d\x2f\x2f" }, "\"jS8v\"" },
        { { 3, (unsigned char *)"\x8d\x2f\x5e" }, "\"jS9e\"" },
        { { 3, (unsigned char *)"\x8d\x2f\x8d" }, "\"jS-N\"" },
        { { 3, (unsigned char *)"\x8d\x2f\xbc" }, "\"jS-8\"" },
        { { 3, (unsigned char *)"\x8d\x2f\xeb" }, "\"jS_r\"" },
        { { 3, (unsigned char *)"\x8d\x5e\x00" }, "\"jV4A\"" },
        { { 3, (unsigned char *)"\x8d\x5e\x2f" }, "\"jV4v\"" },
        { { 3, (unsigned char *)"\x8d\x5e\x5e" }, "\"jV5e\"" },
        { { 3, (unsigned char *)"\x8d\x5e\x8d" }, "\"jV6N\"" },
        { { 3, (unsigned char *)"\x8d\x5e\xbc" }, "\"jV68\"" },
        { { 3, (unsigned char *)"\x8d\x5e\xeb" }, "\"jV7r\"" },
        { { 3, (unsigned char *)"\x8d\x8d\x00" }, "\"jY0A\"" },
        { { 3, (unsigned char *)"\x8d\x8d\x2f" }, "\"jY0v\"" },
        { { 3, (unsigned char *)"\x8d\x8d\x5e" }, "\"jY1e\"" },
        { { 3, (unsigned char *)"\x8d\x8d\x8d" }, "\"jY2N\"" },
        { { 3, (unsigned char *)"\x8d\x8d\xbc" }, "\"jY28\"" },
        { { 3, (unsigned char *)"\x8d\x8d\xeb" }, "\"jY3r\"" },
        { { 3, (unsigned char *)"\x8d\xbc\x00" }, "\"jbwA\"" },
        { { 3, (unsigned char *)"\x8d\xbc\x2f" }, "\"jbwv\"" },
        { { 3, (unsigned char *)"\x8d\xbc\x5e" }, "\"jbxe\"" },
        { { 3, (unsigned char *)"\x8d\xbc\x8d" }, "\"jbyN\"" },
        { { 3, (unsigned char *)"\x8d\xbc\xbc" }, "\"jby8\"" },
        { { 3, (unsigned char *)"\x8d\xbc\xeb" }, "\"jbzr\"" },
        { { 3, (unsigned char *)"\x8d\xeb\x00" }, "\"jesA\"" },
        { { 3, (unsigned char *)"\x8d\xeb\x2f" }, "\"jesv\"" },
        { { 3, (unsigned char *)"\x8d\xeb\x5e" }, "\"jete\"" },
        { { 3, (unsigned char *)"\x8d\xeb\x8d" }, "\"jeuN\"" },
        { { 3, (unsigned char *)"\x8d\xeb\xbc" }, "\"jeu8\"" },
        { { 3, (unsigned char *)"\x8d\xeb\xeb" }, "\"jevr\"" },
        { { 3, (unsigned char *)"\xbc\x00\x00" }, "\"vAAA\"" },
        { { 3, (unsigned char *)"\xbc\x00\x2f" }, "\"vAAv\"" },
        { { 3, (unsigned char *)"\xbc\x00\x5e" }, "\"vABe\"" },
        { { 3, (unsigned char *)"\xbc\x00\x8d" }, "\"vACN\"" },
        { { 3, (unsigned char *)"\xbc\x00\xbc" }, "\"vAC8\"" },
        { { 3, (unsigned char *)"\xbc\x00\xeb" }, "\"vADr\"" },
        { { 3, (unsigned char *)"\xbc\x2f\x00" }, "\"vC8A\"" },
        { { 3, (unsigned char *)"\xbc\x2f\x2f" }, "\"vC8v\"" },
        { { 3, (unsigned char *)"\xbc\x2f\x5e" }, "\"vC9e\"" },
        { { 3, (unsigned char *)"\xbc\x2f\x8d" }, "\"vC-N\"" },
        { { 3, (unsigned char *)"\xbc\x2f\xbc" }, "\"vC-8\"" },
        { { 3, (unsigned char *)"\xbc\x2f\xeb" }, "\"vC_r\"" },
        { { 3, (unsigned char *)"\xbc\x5e\x00" }, "\"vF4A\"" },
        { { 3, (unsigned char *)"\xbc\x5e\x2f" }, "\"vF4v\"" },
        { { 3, (unsigned char *)"\xbc\x5e\x5e" }, "\"vF5e\"" },
        { { 3, (unsigned char *)"\xbc\x5e\x8d" }, "\"vF6N\"" },
        { { 3, (unsigned char *)"\xbc\x5e\xbc" }, "\"vF68\"" },
        { { 3, (unsigned char *)"\xbc\x5e\xeb" }, "\"vF7r\"" },
        { { 3, (unsigned char *)"\xbc\x8d\x00" }, "\"vI0A\"" },
        { { 3, (unsigned char *)"\xbc\x8d\x2f" }, "\"vI0v\"" },
        { { 3, (unsigned char *)"\xbc\x8d\x5e" }, "\"vI1e\"" },
        { { 3, (unsigned char *)"\xbc\x8d\x8d" }, "\"vI2N\"" },
        { { 3, (unsigned char *)"\xbc\x8d\xbc" }, "\"vI28\"" },
        { { 3, (unsigned char *)"\xbc\x8d\xeb" }, "\"vI3r\"" },
        { { 3, (unsigned char *)"\xbc\xbc\x00" }, "\"vLwA\"" },
        { { 3, (unsigned char *)"\xbc\xbc\x2f" }, "\"vLwv\"" },
        { { 3, (unsigned char *)"\xbc\xbc\x5e" }, "\"vLxe\"" },
        { { 3, (unsigned char *)"\xbc\xbc\x8d" }, "\"vLyN\"" },
        { { 3, (unsigned char *)"\xbc\xbc\xbc" }, "\"vLy8\"" },
        { { 3, (unsigned char *)"\xbc\xbc\xeb" }, "\"vLzr\"" },
        { { 3, (unsigned char *)"\xbc\xeb\x00" }, "\"vOsA\"" },
        { { 3, (unsigned char *)"\xbc\xeb\x2f" }, "\"vOsv\"" },
        { { 3, (unsigned char *)"\xbc\xeb\x5e" }, "\"vOte\"" },
        { { 3, (unsigned char *)"\xbc\xeb\x8d" }, "\"vOuN\"" },
        { { 3, (unsigned char *)"\xbc\xeb\xbc" }, "\"vOu8\"" },
        { { 3, (unsigned char *)"\xbc\xeb\xeb" }, "\"vOvr\"" },
        { { 3, (unsigned char *)"\xeb\x00\x00" }, "\"6wAA\"" },
        { { 3, (unsigned char *)"\xeb\x00\x2f" }, "\"6wAv\"" },
        { { 3, (unsigned char *)"\xeb\x00\x5e" }, "\"6wBe\"" },
        { { 3, (unsigned char *)"\xeb\x00\x8d" }, "\"6wCN\"" },
        { { 3, (unsigned char *)"\xeb\x00\xbc" }, "\"6wC8\"" },
        { { 3, (unsigned char *)"\xeb\x00\xeb" }, "\"6wDr\"" },
        { { 3, (unsigned char *)"\xeb\x2f\x00" }, "\"6y8A\"" },
        { { 3, (unsigned char *)"\xeb\x2f\x2f" }, "\"6y8v\"" },
        { { 3, (unsigned char *)"\xeb\x2f\x5e" }, "\"6y9e\"" },
        { { 3, (unsigned char *)"\xeb\x2f\x8d" }, "\"6y-N\"" },
        { { 3, (unsigned char *)"\xeb\x2f\xbc" }, "\"6y-8\"" },
        { { 3, (unsigned char *)"\xeb\x2f\xeb" }, "\"6y_r\"" },
        { { 3, (unsigned char *)"\xeb\x5e\x00" }, "\"614A\"" },
        { { 3, (unsigned char *)"\xeb\x5e\x2f" }, "\"614v\"" },
        { { 3, (unsigned char *)"\xeb\x5e\x5e" }, "\"615e\"" },
        { { 3, (unsigned char *)"\xeb\x5e\x8d" }, "\"616N\"" },
        { { 3, (unsigned char *)"\xeb\x5e\xbc" }, "\"6168\"" },
        { { 3, (unsigned char *)"\xeb\x5e\xeb" }, "\"617r\"" },
        { { 3, (unsigned char *)"\xeb\x8d\x00" }, "\"640A\"" },
        { { 3, (unsigned char *)"\xeb\x8d\x2f" }, "\"640v\"" },
        { { 3, (unsigned char *)"\xeb\x8d\x5e" }, "\"641e\"" },
        { { 3, (unsigned char *)"\xeb\x8d\x8d" }, "\"642N\"" },
        { { 3, (unsigned char *)"\xeb\x8d\xbc" }, "\"6428\"" },
        { { 3, (unsigned char *)"\xeb\x8d\xeb" }, "\"643r\"" },
        { { 3, (unsigned char *)"\xeb\xbc\x00" }, "\"67wA\"" },
        { { 3, (unsigned char *)"\xeb\xbc\x2f" }, "\"67wv\"" },
        { { 3, (unsigned char *)"\xeb\xbc\x5e" }, "\"67xe\"" },
        { { 3, (unsigned char *)"\xeb\xbc\x8d" }, "\"67yN\"" },
        { { 3, (unsigned char *)"\xeb\xbc\xbc" }, "\"67y8\"" },
        { { 3, (unsigned char *)"\xeb\xbc\xeb" }, "\"67zr\"" },
        { { 3, (unsigned char *)"\xeb\xeb\x00" }, "\"6-sA\"" },
        { { 3, (unsigned char *)"\xeb\xeb\x2f" }, "\"6-sv\"" },
        { { 3, (unsigned char *)"\xeb\xeb\x5e" }, "\"6-te\"" },
        { { 3, (unsigned char *)"\xeb\xeb\x8d" }, "\"6-uN\"" },
        { { 3, (unsigned char *)"\xeb\xeb\xbc" }, "\"6-u8\"" },
        { { 3, (unsigned char *)"\xeb\xeb\xeb" }, "\"6-vr\"" },
        { { 4, (unsigned char *)"\x00\x00\x00\x00" }, "\"AAAAAA==\"" },
        { { 4, (unsigned char *)"\x00\x00\x00\x55" }, "\"AAAAVQ==\"" },
        { { 4, (unsigned char *)"\x00\x00\x00\xaa" }, "\"AAAAqg==\"" },
        { { 4, (unsigned char *)"\x00\x00\x00\xff" }, "\"AAAA_w==\"" },
        { { 4, (unsigned char *)"\x00\x00\x55\x00" }, "\"AABVAA==\"" },
        { { 4, (unsigned char *)"\x00\x00\x55\x55" }, "\"AABVVQ==\"" },
        { { 4, (unsigned char *)"\x00\x00\x55\xaa" }, "\"AABVqg==\"" },
        { { 4, (unsigned char *)"\x00\x00\x55\xff" }, "\"AABV_w==\"" },
        { { 4, (unsigned char *)"\x00\x00\xaa\x00" }, "\"AACqAA==\"" },
        { { 4, (unsigned char *)"\x00\x00\xaa\x55" }, "\"AACqVQ==\"" },
        { { 4, (unsigned char *)"\x00\x00\xaa\xaa" }, "\"AACqqg==\"" },
        { { 4, (unsigned char *)"\x00\x00\xaa\xff" }, "\"AACq_w==\"" },
        { { 4, (unsigned char *)"\x00\x00\xff\x00" }, "\"AAD_AA==\"" },
        { { 4, (unsigned char *)"\x00\x00\xff\x55" }, "\"AAD_VQ==\"" },
        { { 4, (unsigned char *)"\x00\x00\xff\xaa" }, "\"AAD_qg==\"" },
        { { 4, (unsigned char *)"\x00\x00\xff\xff" }, "\"AAD__w==\"" },
        { { 4, (unsigned char *)"\x00\x55\x00\x00" }, "\"AFUAAA==\"" },
        { { 4, (unsigned char *)"\x00\x55\x00\x55" }, "\"AFUAVQ==\"" },
        { { 4, (unsigned char *)"\x00\x55\x00\xaa" }, "\"AFUAqg==\"" },
        { { 4, (unsigned char *)"\x00\x55\x00\xff" }, "\"AFUA_w==\"" },
        { { 4, (unsigned char *)"\x00\x55\x55\x00" }, "\"AFVVAA==\"" },
        { { 4, (unsigned char *)"\x00\x55\x55\x55" }, "\"AFVVVQ==\"" },
        { { 4, (unsigned char *)"\x00\x55\x55\xaa" }, "\"AFVVqg==\"" },
        { { 4, (unsigned char *)"\x00\x55\x55\xff" }, "\"AFVV_w==\"" },
        { { 4, (unsigned char *)"\x00\x55\xaa\x00" }, "\"AFWqAA==\"" },
        { { 4, (unsigned char *)"\x00\x55\xaa\x55" }, "\"AFWqVQ==\"" },
        { { 4, (unsigned char *)"\x00\x55\xaa\xaa" }, "\"AFWqqg==\"" },
        { { 4, (unsigned char *)"\x00\x55\xaa\xff" }, "\"AFWq_w==\"" },
        { { 4, (unsigned char *)"\x00\x55\xff\x00" }, "\"AFX_AA==\"" },
        { { 4, (unsigned char *)"\x00\x55\xff\x55" }, "\"AFX_VQ==\"" },
        { { 4, (unsigned char *)"\x00\x55\xff\xaa" }, "\"AFX_qg==\"" },
        { { 4, (unsigned char *)"\x00\x55\xff\xff" }, "\"AFX__w==\"" },
        { { 4, (unsigned char *)"\x00\xaa\x00\x00" }, "\"AKoAAA==\"" },
        { { 4, (unsigned char *)"\x00\xaa\x00\x55" }, "\"AKoAVQ==\"" },
        { { 4, (unsigned char *)"\x00\xaa\x00\xaa" }, "\"AKoAqg==\"" },
        { { 4, (unsigned char *)"\x00\xaa\x00\xff" }, "\"AKoA_w==\"" },
        { { 4, (unsigned char *)"\x00\xaa\x55\x00" }, "\"AKpVAA==\"" },
        { { 4, (unsigned char *)"\x00\xaa\x55\x55" }, "\"AKpVVQ==\"" },
        { { 4, (unsigned char *)"\x00\xaa\x55\xaa" }, "\"AKpVqg==\"" },
        { { 4, (unsigned char *)"\x00\xaa\x55\xff" }, "\"AKpV_w==\"" },
        { { 4, (unsigned char *)"\x00\xaa\xaa\x00" }, "\"AKqqAA==\"" },
        { { 4, (unsigned char *)"\x00\xaa\xaa\x55" }, "\"AKqqVQ==\"" },
        { { 4, (unsigned char *)"\x00\xaa\xaa\xaa" }, "\"AKqqqg==\"" },
        { { 4, (unsigned char *)"\x00\xaa\xaa\xff" }, "\"AKqq_w==\"" },
        { { 4, (unsigned char *)"\x00\xaa\xff\x00" }, "\"AKr_AA==\"" },
        { { 4, (unsigned char *)"\x00\xaa\xff\x55" }, "\"AKr_VQ==\"" },
        { { 4, (unsigned char *)"\x00\xaa\xff\xaa" }, "\"AKr_qg==\"" },
        { { 4, (unsigned char *)"\x00\xaa\xff\xff" }, "\"AKr__w==\"" },
        { { 4, (unsigned char *)"\x00\xff\x00\x00" }, "\"AP8AAA==\"" },
        { { 4, (unsigned char *)"\x00\xff\x00\x55" }, "\"AP8AVQ==\"" },
        { { 4, (unsigned char *)"\x00\xff\x00\xaa" }, "\"AP8Aqg==\"" },
        { { 4, (unsigned char *)"\x00\xff\x00\xff" }, "\"AP8A_w==\"" },
        { { 4, (unsigned char *)"\x00\xff\x55\x00" }, "\"AP9VAA==\"" },
        { { 4, (unsigned char *)"\x00\xff\x55\x55" }, "\"AP9VVQ==\"" },
        { { 4, (unsigned char *)"\x00\xff\x55\xaa" }, "\"AP9Vqg==\"" },
        { { 4, (unsigned char *)"\x00\xff\x55\xff" }, "\"AP9V_w==\"" },
        { { 4, (unsigned char *)"\x00\xff\xaa\x00" }, "\"AP-qAA==\"" },
        { { 4, (unsigned char *)"\x00\xff\xaa\x55" }, "\"AP-qVQ==\"" },
        { { 4, (unsigned char *)"\x00\xff\xaa\xaa" }, "\"AP-qqg==\"" },
        { { 4, (unsigned char *)"\x00\xff\xaa\xff" }, "\"AP-q_w==\"" },
        { { 4, (unsigned char *)"\x00\xff\xff\x00" }, "\"AP__AA==\"" },
        { { 4, (unsigned char *)"\x00\xff\xff\x55" }, "\"AP__VQ==\"" },
        { { 4, (unsigned char *)"\x00\xff\xff\xaa" }, "\"AP__qg==\"" },
        { { 4, (unsigned char *)"\x00\xff\xff\xff" }, "\"AP___w==\"" },
        { { 4, (unsigned char *)"\x55\x00\x00\x00" }, "\"VQAAAA==\"" },
        { { 4, (unsigned char *)"\x55\x00\x00\x55" }, "\"VQAAVQ==\"" },
        { { 4, (unsigned char *)"\x55\x00\x00\xaa" }, "\"VQAAqg==\"" },
        { { 4, (unsigned char *)"\x55\x00\x00\xff" }, "\"VQAA_w==\"" },
        { { 4, (unsigned char *)"\x55\x00\x55\x00" }, "\"VQBVAA==\"" },
        { { 4, (unsigned char *)"\x55\x00\x55\x55" }, "\"VQBVVQ==\"" },
        { { 4, (unsigned char *)"\x55\x00\x55\xaa" }, "\"VQBVqg==\"" },
        { { 4, (unsigned char *)"\x55\x00\x55\xff" }, "\"VQBV_w==\"" },
        { { 4, (unsigned char *)"\x55\x00\xaa\x00" }, "\"VQCqAA==\"" },
        { { 4, (unsigned char *)"\x55\x00\xaa\x55" }, "\"VQCqVQ==\"" },
        { { 4, (unsigned char *)"\x55\x00\xaa\xaa" }, "\"VQCqqg==\"" },
        { { 4, (unsigned char *)"\x55\x00\xaa\xff" }, "\"VQCq_w==\"" },
        { { 4, (unsigned char *)"\x55\x00\xff\x00" }, "\"VQD_AA==\"" },
        { { 4, (unsigned char *)"\x55\x00\xff\x55" }, "\"VQD_VQ==\"" },
        { { 4, (unsigned char *)"\x55\x00\xff\xaa" }, "\"VQD_qg==\"" },
        { { 4, (unsigned char *)"\x55\x00\xff\xff" }, "\"VQD__w==\"" },
        { { 4, (unsigned char *)"\x55\x55\x00\x00" }, "\"VVUAAA==\"" },
        { { 4, (unsigned char *)"\x55\x55\x00\x55" }, "\"VVUAVQ==\"" },
        { { 4, (unsigned char *)"\x55\x55\x00\xaa" }, "\"VVUAqg==\"" },
        { { 4, (unsigned char *)"\x55\x55\x00\xff" }, "\"VVUA_w==\"" },
        { { 4, (unsigned char *)"\x55\x55\x55\x00" }, "\"VVVVAA==\"" },
        { { 4, (unsigned char *)"\x55\x55\x55\x55" }, "\"VVVVVQ==\"" },
        { { 4, (unsigned char *)"\x55\x55\x55\xaa" }, "\"VVVVqg==\"" },
        { { 4, (unsigned char *)"\x55\x55\x55\xff" }, "\"VVVV_w==\"" },
        { { 4, (unsigned char *)"\x55\x55\xaa\x00" }, "\"VVWqAA==\"" },
        { { 4, (unsigned char *)"\x55\x55\xaa\x55" }, "\"VVWqVQ==\"" },
        { { 4, (unsigned char *)"\x55\x55\xaa\xaa" }, "\"VVWqqg==\"" },
        { { 4, (unsigned char *)"\x55\x55\xaa\xff" }, "\"VVWq_w==\"" },
        { { 4, (unsigned char *)"\x55\x55\xff\x00" }, "\"VVX_AA==\"" },
        { { 4, (unsigned char *)"\x55\x55\xff\x55" }, "\"VVX_VQ==\"" },
        { { 4, (unsigned char *)"\x55\x55\xff\xaa" }, "\"VVX_qg==\"" },
        { { 4, (unsigned char *)"\x55\x55\xff\xff" }, "\"VVX__w==\"" },
        { { 4, (unsigned char *)"\x55\xaa\x00\x00" }, "\"VaoAAA==\"" },
        { { 4, (unsigned char *)"\x55\xaa\x00\x55" }, "\"VaoAVQ==\"" },
        { { 4, (unsigned char *)"\x55\xaa\x00\xaa" }, "\"VaoAqg==\"" },
        { { 4, (unsigned char *)"\x55\xaa\x00\xff" }, "\"VaoA_w==\"" },
        { { 4, (unsigned char *)"\x55\xaa\x55\x00" }, "\"VapVAA==\"" },
        { { 4, (unsigned char *)"\x55\xaa\x55\x55" }, "\"VapVVQ==\"" },
        { { 4, (unsigned char *)"\x55\xaa\x55\xaa" }, "\"VapVqg==\"" },
        { { 4, (unsigned char *)"\x55\xaa\x55\xff" }, "\"VapV_w==\"" },
        { { 4, (unsigned char *)"\x55\xaa\xaa\x00" }, "\"VaqqAA==\"" },
        { { 4, (unsigned char *)"\x55\xaa\xaa\x55" }, "\"VaqqVQ==\"" },
        { { 4, (unsigned char *)"\x55\xaa\xaa\xaa" }, "\"Vaqqqg==\"" },
        { { 4, (unsigned char *)"\x55\xaa\xaa\xff" }, "\"Vaqq_w==\"" },
        { { 4, (unsigned char *)"\x55\xaa\xff\x00" }, "\"Var_AA==\"" },
        { { 4, (unsigned char *)"\x55\xaa\xff\x55" }, "\"Var_VQ==\"" },
        { { 4, (unsigned char *)"\x55\xaa\xff\xaa" }, "\"Var_qg==\"" },
        { { 4, (unsigned char *)"\x55\xaa\xff\xff" }, "\"Var__w==\"" },
        { { 4, (unsigned char *)"\x55\xff\x00\x00" }, "\"Vf8AAA==\"" },
        { { 4, (unsigned char *)"\x55\xff\x00\x55" }, "\"Vf8AVQ==\"" },
        { { 4, (unsigned char *)"\x55\xff\x00\xaa" }, "\"Vf8Aqg==\"" },
        { { 4, (unsigned char *)"\x55\xff\x00\xff" }, "\"Vf8A_w==\"" },
        { { 4, (unsigned char *)"\x55\xff\x55\x00" }, "\"Vf9VAA==\"" },
        { { 4, (unsigned char *)"\x55\xff\x55\x55" }, "\"Vf9VVQ==\"" },
        { { 4, (unsigned char *)"\x55\xff\x55\xaa" }, "\"Vf9Vqg==\"" },
        { { 4, (unsigned char *)"\x55\xff\x55\xff" }, "\"Vf9V_w==\"" },
        { { 4, (unsigned char *)"\x55\xff\xaa\x00" }, "\"Vf-qAA==\"" },
        { { 4, (unsigned char *)"\x55\xff\xaa\x55" }, "\"Vf-qVQ==\"" },
        { { 4, (unsigned char *)"\x55\xff\xaa\xaa" }, "\"Vf-qqg==\"" },
        { { 4, (unsigned char *)"\x55\xff\xaa\xff" }, "\"Vf-q_w==\"" },
        { { 4, (unsigned char *)"\x55\xff\xff\x00" }, "\"Vf__AA==\"" },
        { { 4, (unsigned char *)"\x55\xff\xff\x55" }, "\"Vf__VQ==\"" },
        { { 4, (unsigned char *)"\x55\xff\xff\xaa" }, "\"Vf__qg==\"" },
        { { 4, (unsigned char *)"\x55\xff\xff\xff" }, "\"Vf___w==\"" },
        { { 4, (unsigned char *)"\xaa\x00\x00\x00" }, "\"qgAAAA==\"" },
        { { 4, (unsigned char *)"\xaa\x00\x00\x55" }, "\"qgAAVQ==\"" },
        { { 4, (unsigned char *)"\xaa\x00\x00\xaa" }, "\"qgAAqg==\"" },
        { { 4, (unsigned char *)"\xaa\x00\x00\xff" }, "\"qgAA_w==\"" },
        { { 4, (unsigned char *)"\xaa\x00\x55\x00" }, "\"qgBVAA==\"" },
        { { 4, (unsigned char *)"\xaa\x00\x55\x55" }, "\"qgBVVQ==\"" },
        { { 4, (unsigned char *)"\xaa\x00\x55\xaa" }, "\"qgBVqg==\"" },
        { { 4, (unsigned char *)"\xaa\x00\x55\xff" }, "\"qgBV_w==\"" },
        { { 4, (unsigned char *)"\xaa\x00\xaa\x00" }, "\"qgCqAA==\"" },
        { { 4, (unsigned char *)"\xaa\x00\xaa\x55" }, "\"qgCqVQ==\"" },
        { { 4, (unsigned char *)"\xaa\x00\xaa\xaa" }, "\"qgCqqg==\"" },
        { { 4, (unsigned char *)"\xaa\x00\xaa\xff" }, "\"qgCq_w==\"" },
        { { 4, (unsigned char *)"\xaa\x00\xff\x00" }, "\"qgD_AA==\"" },
        { { 4, (unsigned char *)"\xaa\x00\xff\x55" }, "\"qgD_VQ==\"" },
        { { 4, (unsigned char *)"\xaa\x00\xff\xaa" }, "\"qgD_qg==\"" },
        { { 4, (unsigned char *)"\xaa\x00\xff\xff" }, "\"qgD__w==\"" },
        { { 4, (unsigned char *)"\xaa\x55\x00\x00" }, "\"qlUAAA==\"" },
        { { 4, (unsigned char *)"\xaa\x55\x00\x55" }, "\"qlUAVQ==\"" },
        { { 4, (unsigned char *)"\xaa\x55\x00\xaa" }, "\"qlUAqg==\"" },
        { { 4, (unsigned char *)"\xaa\x55\x00\xff" }, "\"qlUA_w==\"" },
        { { 4, (unsigned char *)"\xaa\x55\x55\x00" }, "\"qlVVAA==\"" },
        { { 4, (unsigned char *)"\xaa\x55\x55\x55" }, "\"qlVVVQ==\"" },
        { { 4, (unsigned char *)"\xaa\x55\x55\xaa" }, "\"qlVVqg==\"" },
        { { 4, (unsigned char *)"\xaa\x55\x55\xff" }, "\"qlVV_w==\"" },
        { { 4, (unsigned char *)"\xaa\x55\xaa\x00" }, "\"qlWqAA==\"" },
        { { 4, (unsigned char *)"\xaa\x55\xaa\x55" }, "\"qlWqVQ==\"" },
        { { 4, (unsigned char *)"\xaa\x55\xaa\xaa" }, "\"qlWqqg==\"" },
        { { 4, (unsigned char *)"\xaa\x55\xaa\xff" }, "\"qlWq_w==\"" },
        { { 4, (unsigned char *)"\xaa\x55\xff\x00" }, "\"qlX_AA==\"" },
        { { 4, (unsigned char *)"\xaa\x55\xff\x55" }, "\"qlX_VQ==\"" },
        { { 4, (unsigned char *)"\xaa\x55\xff\xaa" }, "\"qlX_qg==\"" },
        { { 4, (unsigned char *)"\xaa\x55\xff\xff" }, "\"qlX__w==\"" },
        { { 4, (unsigned char *)"\xaa\xaa\x00\x00" }, "\"qqoAAA==\"" },
        { { 4, (unsigned char *)"\xaa\xaa\x00\x55" }, "\"qqoAVQ==\"" },
        { { 4, (unsigned char *)"\xaa\xaa\x00\xaa" }, "\"qqoAqg==\"" },
        { { 4, (unsigned char *)"\xaa\xaa\x00\xff" }, "\"qqoA_w==\"" },
        { { 4, (unsigned char *)"\xaa\xaa\x55\x00" }, "\"qqpVAA==\"" },
        { { 4, (unsigned char *)"\xaa\xaa\x55\x55" }, "\"qqpVVQ==\"" },
        { { 4, (unsigned char *)"\xaa\xaa\x55\xaa" }, "\"qqpVqg==\"" },
        { { 4, (unsigned char *)"\xaa\xaa\x55\xff" }, "\"qqpV_w==\"" },
        { { 4, (unsigned char *)"\xaa\xaa\xaa\x00" }, "\"qqqqAA==\"" },
        { { 4, (unsigned char *)"\xaa\xaa\xaa\x55" }, "\"qqqqVQ==\"" },
        { { 4, (unsigned char *)"\xaa\xaa\xaa\xaa" }, "\"qqqqqg==\"" },
        { { 4, (unsigned char *)"\xaa\xaa\xaa\xff" }, "\"qqqq_w==\"" },
        { { 4, (unsigned char *)"\xaa\xaa\xff\x00" }, "\"qqr_AA==\"" },
        { { 4, (unsigned char *)"\xaa\xaa\xff\x55" }, "\"qqr_VQ==\"" },
        { { 4, (unsigned char *)"\xaa\xaa\xff\xaa" }, "\"qqr_qg==\"" },
        { { 4, (unsigned char *)"\xaa\xaa\xff\xff" }, "\"qqr__w==\"" },
        { { 4, (unsigned char *)"\xaa\xff\x00\x00" }, "\"qv8AAA==\"" },
        { { 4, (unsigned char *)"\xaa\xff\x00\x55" }, "\"qv8AVQ==\"" },
        { { 4, (unsigned char *)"\xaa\xff\x00\xaa" }, "\"qv8Aqg==\"" },
        { { 4, (unsigned char *)"\xaa\xff\x00\xff" }, "\"qv8A_w==\"" },
        { { 4, (unsigned char *)"\xaa\xff\x55\x00" }, "\"qv9VAA==\"" },
        { { 4, (unsigned char *)"\xaa\xff\x55\x55" }, "\"qv9VVQ==\"" },
        { { 4, (unsigned char *)"\xaa\xff\x55\xaa" }, "\"qv9Vqg==\"" },
        { { 4, (unsigned char *)"\xaa\xff\x55\xff" }, "\"qv9V_w==\"" },
        { { 4, (unsigned char *)"\xaa\xff\xaa\x00" }, "\"qv-qAA==\"" },
        { { 4, (unsigned char *)"\xaa\xff\xaa\x55" }, "\"qv-qVQ==\"" },
        { { 4, (unsigned char *)"\xaa\xff\xaa\xaa" }, "\"qv-qqg==\"" },
        { { 4, (unsigned char *)"\xaa\xff\xaa\xff" }, "\"qv-q_w==\"" },
        { { 4, (unsigned char *)"\xaa\xff\xff\x00" }, "\"qv__AA==\"" },
        { { 4, (unsigned char *)"\xaa\xff\xff\x55" }, "\"qv__VQ==\"" },
        { { 4, (unsigned char *)"\xaa\xff\xff\xaa" }, "\"qv__qg==\"" },
        { { 4, (unsigned char *)"\xaa\xff\xff\xff" }, "\"qv___w==\"" },
        { { 4, (unsigned char *)"\xff\x00\x00\x00" }, "\"_wAAAA==\"" },
        { { 4, (unsigned char *)"\xff\x00\x00\x55" }, "\"_wAAVQ==\"" },
        { { 4, (unsigned char *)"\xff\x00\x00\xaa" }, "\"_wAAqg==\"" },
        { { 4, (unsigned char *)"\xff\x00\x00\xff" }, "\"_wAA_w==\"" },
        { { 4, (unsigned char *)"\xff\x00\x55\x00" }, "\"_wBVAA==\"" },
        { { 4, (unsigned char *)"\xff\x00\x55\x55" }, "\"_wBVVQ==\"" },
        { { 4, (unsigned char *)"\xff\x00\x55\xaa" }, "\"_wBVqg==\"" },
        { { 4, (unsigned char *)"\xff\x00\x55\xff" }, "\"_wBV_w==\"" },
        { { 4, (unsigned char *)"\xff\x00\xaa\x00" }, "\"_wCqAA==\"" },
        { { 4, (unsigned char *)"\xff\x00\xaa\x55" }, "\"_wCqVQ==\"" },
        { { 4, (unsigned char *)"\xff\x00\xaa\xaa" }, "\"_wCqqg==\"" },
        { { 4, (unsigned char *)"\xff\x00\xaa\xff" }, "\"_wCq_w==\"" },
        { { 4, (unsigned char *)"\xff\x00\xff\x00" }, "\"_wD_AA==\"" },
        { { 4, (unsigned char *)"\xff\x00\xff\x55" }, "\"_wD_VQ==\"" },
        { { 4, (unsigned char *)"\xff\x00\xff\xaa" }, "\"_wD_qg==\"" },
        { { 4, (unsigned char *)"\xff\x00\xff\xff" }, "\"_wD__w==\"" },
        { { 4, (unsigned char *)"\xff\x55\x00\x00" }, "\"_1UAAA==\"" },
        { { 4, (unsigned char *)"\xff\x55\x00\x55" }, "\"_1UAVQ==\"" },
        { { 4, (unsigned char *)"\xff\x55\x00\xaa" }, "\"_1UAqg==\"" },
        { { 4, (unsigned char *)"\xff\x55\x00\xff" }, "\"_1UA_w==\"" },
        { { 4, (unsigned char *)"\xff\x55\x55\x00" }, "\"_1VVAA==\"" },
        { { 4, (unsigned char *)"\xff\x55\x55\x55" }, "\"_1VVVQ==\"" },
        { { 4, (unsigned char *)"\xff\x55\x55\xaa" }, "\"_1VVqg==\"" },
        { { 4, (unsigned char *)"\xff\x55\x55\xff" }, "\"_1VV_w==\"" },
        { { 4, (unsigned char *)"\xff\x55\xaa\x00" }, "\"_1WqAA==\"" },
        { { 4, (unsigned char *)"\xff\x55\xaa\x55" }, "\"_1WqVQ==\"" },
        { { 4, (unsigned char *)"\xff\x55\xaa\xaa" }, "\"_1Wqqg==\"" },
        { { 4, (unsigned char *)"\xff\x55\xaa\xff" }, "\"_1Wq_w==\"" },
        { { 4, (unsigned char *)"\xff\x55\xff\x00" }, "\"_1X_AA==\"" },
        { { 4, (unsigned char *)"\xff\x55\xff\x55" }, "\"_1X_VQ==\"" },
        { { 4, (unsigned char *)"\xff\x55\xff\xaa" }, "\"_1X_qg==\"" },
        { { 4, (unsigned char *)"\xff\x55\xff\xff" }, "\"_1X__w==\"" },
        { { 4, (unsigned char *)"\xff\xaa\x00\x00" }, "\"_6oAAA==\"" },
        { { 4, (unsigned char *)"\xff\xaa\x00\x55" }, "\"_6oAVQ==\"" },
        { { 4, (unsigned char *)"\xff\xaa\x00\xaa" }, "\"_6oAqg==\"" },
        { { 4, (unsigned char *)"\xff\xaa\x00\xff" }, "\"_6oA_w==\"" },
        { { 4, (unsigned char *)"\xff\xaa\x55\x00" }, "\"_6pVAA==\"" },
        { { 4, (unsigned char *)"\xff\xaa\x55\x55" }, "\"_6pVVQ==\"" },
        { { 4, (unsigned char *)"\xff\xaa\x55\xaa" }, "\"_6pVqg==\"" },
        { { 4, (unsigned char *)"\xff\xaa\x55\xff" }, "\"_6pV_w==\"" },
        { { 4, (unsigned char *)"\xff\xaa\xaa\x00" }, "\"_6qqAA==\"" },
        { { 4, (unsigned char *)"\xff\xaa\xaa\x55" }, "\"_6qqVQ==\"" },
        { { 4, (unsigned char *)"\xff\xaa\xaa\xaa" }, "\"_6qqqg==\"" },
        { { 4, (unsigned char *)"\xff\xaa\xaa\xff" }, "\"_6qq_w==\"" },
        { { 4, (unsigned char *)"\xff\xaa\xff\x00" }, "\"_6r_AA==\"" },
        { { 4, (unsigned char *)"\xff\xaa\xff\x55" }, "\"_6r_VQ==\"" },
        { { 4, (unsigned char *)"\xff\xaa\xff\xaa" }, "\"_6r_qg==\"" },
        { { 4, (unsigned char *)"\xff\xaa\xff\xff" }, "\"_6r__w==\"" },
        { { 4, (unsigned char *)"\xff\xff\x00\x00" }, "\"__8AAA==\"" },
        { { 4, (unsigned char *)"\xff\xff\x00\x55" }, "\"__8AVQ==\"" },
        { { 4, (unsigned char *)"\xff\xff\x00\xaa" }, "\"__8Aqg==\"" },
        { { 4, (unsigned char *)"\xff\xff\x00\xff" }, "\"__8A_w==\"" },
        { { 4, (unsigned char *)"\xff\xff\x55\x00" }, "\"__9VAA==\"" },
        { { 4, (unsigned char *)"\xff\xff\x55\x55" }, "\"__9VVQ==\"" },
        { { 4, (unsigned char *)"\xff\xff\x55\xaa" }, "\"__9Vqg==\"" },
        { { 4, (unsigned char *)"\xff\xff\x55\xff" }, "\"__9V_w==\"" },
        { { 4, (unsigned char *)"\xff\xff\xaa\x00" }, "\"__-qAA==\"" },
        { { 4, (unsigned char *)"\xff\xff\xaa\x55" }, "\"__-qVQ==\"" },
        { { 4, (unsigned char *)"\xff\xff\xaa\xaa" }, "\"__-qqg==\"" },
        { { 4, (unsigned char *)"\xff\xff\xaa\xff" }, "\"__-q_w==\"" },
        { { 4, (unsigned char *)"\xff\xff\xff\x00" }, "\"____AA==\"" },
        { { 4, (unsigned char *)"\xff\xff\xff\x55" }, "\"____VQ==\"" },
        { { 4, (unsigned char *)"\xff\xff\xff\xaa" }, "\"____qg==\"" },
        { { 4, (unsigned char *)"\xff\xff\xff\xff" }, "\"_____w==\"" },
        { { 5, (unsigned char *)"\x00\x00\x00\x00\x00" }, "\"AAAAAAA=\"" },
        { { 5, (unsigned char *)"\x00\x00\x00\x00\x7e" }, "\"AAAAAH4=\"" },
        { { 5, (unsigned char *)"\x00\x00\x00\x00\xfc" }, "\"AAAAAPw=\"" },
        { { 5, (unsigned char *)"\x00\x00\x00\x7e\x00" }, "\"AAAAfgA=\"" },
        { { 5, (unsigned char *)"\x00\x00\x00\x7e\x7e" }, "\"AAAAfn4=\"" },
        { { 5, (unsigned char *)"\x00\x00\x00\x7e\xfc" }, "\"AAAAfvw=\"" },
        { { 5, (unsigned char *)"\x00\x00\x00\xfc\x00" }, "\"AAAA_AA=\"" },
        { { 5, (unsigned char *)"\x00\x00\x00\xfc\x7e" }, "\"AAAA_H4=\"" },
        { { 5, (unsigned char *)"\x00\x00\x00\xfc\xfc" }, "\"AAAA_Pw=\"" },
        { { 5, (unsigned char *)"\x00\x00\x7e\x00\x00" }, "\"AAB-AAA=\"" },
        { { 5, (unsigned char *)"\x00\x00\x7e\x00\x7e" }, "\"AAB-AH4=\"" },
        { { 5, (unsigned char *)"\x00\x00\x7e\x00\xfc" }, "\"AAB-APw=\"" },
        { { 5, (unsigned char *)"\x00\x00\x7e\x7e\x00" }, "\"AAB-fgA=\"" },
        { { 5, (unsigned char *)"\x00\x00\x7e\x7e\x7e" }, "\"AAB-fn4=\"" },
        { { 5, (unsigned char *)"\x00\x00\x7e\x7e\xfc" }, "\"AAB-fvw=\"" },
        { { 5, (unsigned char *)"\x00\x00\x7e\xfc\x00" }, "\"AAB-_AA=\"" },
        { { 5, (unsigned char *)"\x00\x00\x7e\xfc\x7e" }, "\"AAB-_H4=\"" },
        { { 5, (unsigned char *)"\x00\x00\x7e\xfc\xfc" }, "\"AAB-_Pw=\"" },
        { { 5, (unsigned char *)"\x00\x00\xfc\x00\x00" }, "\"AAD8AAA=\"" },
        { { 5, (unsigned char *)"\x00\x00\xfc\x00\x7e" }, "\"AAD8AH4=\"" },
        { { 5, (unsigned char *)"\x00\x00\xfc\x00\xfc" }, "\"AAD8APw=\"" },
        { { 5, (unsigned char *)"\x00\x00\xfc\x7e\x00" }, "\"AAD8fgA=\"" },
        { { 5, (unsigned char *)"\x00\x00\xfc\x7e\x7e" }, "\"AAD8fn4=\"" },
        { { 5, (unsigned char *)"\x00\x00\xfc\x7e\xfc" }, "\"AAD8fvw=\"" },
        { { 5, (unsigned char *)"\x00\x00\xfc\xfc\x00" }, "\"AAD8_AA=\"" },
        { { 5, (unsigned char *)"\x00\x00\xfc\xfc\x7e" }, "\"AAD8_H4=\"" },
        { { 5, (unsigned char *)"\x00\x00\xfc\xfc\xfc" }, "\"AAD8_Pw=\"" },
        { { 5, (unsigned char *)"\x00\x7e\x00\x00\x00" }, "\"AH4AAAA=\"" },
        { { 5, (unsigned char *)"\x00\x7e\x00\x00\x7e" }, "\"AH4AAH4=\"" },
        { { 5, (unsigned char *)"\x00\x7e\x00\x00\xfc" }, "\"AH4AAPw=\"" },
        { { 5, (unsigned char *)"\x00\x7e\x00\x7e\x00" }, "\"AH4AfgA=\"" },
        { { 5, (unsigned char *)"\x00\x7e\x00\x7e\x7e" }, "\"AH4Afn4=\"" },
        { { 5, (unsigned char *)"\x00\x7e\x00\x7e\xfc" }, "\"AH4Afvw=\"" },
        { { 5, (unsigned char *)"\x00\x7e\x00\xfc\x00" }, "\"AH4A_AA=\"" },
        { { 5, (unsigned char *)"\x00\x7e\x00\xfc\x7e" }, "\"AH4A_H4=\"" },
        { { 5, (unsigned char *)"\x00\x7e\x00\xfc\xfc" }, "\"AH4A_Pw=\"" },
        { { 5, (unsigned char *)"\x00\x7e\x7e\x00\x00" }, "\"AH5-AAA=\"" },
        { { 5, (unsigned char *)"\x00\x7e\x7e\x00\x7e" }, "\"AH5-AH4=\"" },
        { { 5, (unsigned char *)"\x00\x7e\x7e\x00\xfc" }, "\"AH5-APw=\"" },
        { { 5, (unsigned char *)"\x00\x7e\x7e\x7e\x00" }, "\"AH5-fgA=\"" },
        { { 5, (unsigned char *)"\x00\x7e\x7e\x7e\x7e" }, "\"AH5-fn4=\"" },
        { { 5, (unsigned char *)"\x00\x7e\x7e\x7e\xfc" }, "\"AH5-fvw=\"" },
        { { 5, (unsigned char *)"\x00\x7e\x7e\xfc\x00" }, "\"AH5-_AA=\"" },
        { { 5, (unsigned char *)"\x00\x7e\x7e\xfc\x7e" }, "\"AH5-_H4=\"" },
        { { 5, (unsigned char *)"\x00\x7e\x7e\xfc\xfc" }, "\"AH5-_Pw=\"" },
        { { 5, (unsigned char *)"\x00\x7e\xfc\x00\x00" }, "\"AH78AAA=\"" },
        { { 5, (unsigned char *)"\x00\x7e\xfc\x00\x7e" }, "\"AH78AH4=\"" },
        { { 5, (unsigned char *)"\x00\x7e\xfc\x00\xfc" }, "\"AH78APw=\"" },
        { { 5, (unsigned char *)"\x00\x7e\xfc\x7e\x00" }, "\"AH78fgA=\"" },
        { { 5, (unsigned char *)"\x00\x7e\xfc\x7e\x7e" }, "\"AH78fn4=\"" },
        { { 5, (unsigned char *)"\x00\x7e\xfc\x7e\xfc" }, "\"AH78fvw=\"" },
        { { 5, (unsigned char *)"\x00\x7e\xfc\xfc\x00" }, "\"AH78_AA=\"" },
        { { 5, (unsigned char *)"\x00\x7e\xfc\xfc\x7e" }, "\"AH78_H4=\"" },
        { { 5, (unsigned char *)"\x00\x7e\xfc\xfc\xfc" }, "\"AH78_Pw=\"" },
        { { 5, (unsigned char *)"\x00\xfc\x00\x00\x00" }, "\"APwAAAA=\"" },
        { { 5, (unsigned char *)"\x00\xfc\x00\x00\x7e" }, "\"APwAAH4=\"" },
        { { 5, (unsigned char *)"\x00\xfc\x00\x00\xfc" }, "\"APwAAPw=\"" },
        { { 5, (unsigned char *)"\x00\xfc\x00\x7e\x00" }, "\"APwAfgA=\"" },
        { { 5, (unsigned char *)"\x00\xfc\x00\x7e\x7e" }, "\"APwAfn4=\"" },
        { { 5, (unsigned char *)"\x00\xfc\x00\x7e\xfc" }, "\"APwAfvw=\"" },
        { { 5, (unsigned char *)"\x00\xfc\x00\xfc\x00" }, "\"APwA_AA=\"" },
        { { 5, (unsigned char *)"\x00\xfc\x00\xfc\x7e" }, "\"APwA_H4=\"" },
        { { 5, (unsigned char *)"\x00\xfc\x00\xfc\xfc" }, "\"APwA_Pw=\"" },
        { { 5, (unsigned char *)"\x00\xfc\x7e\x00\x00" }, "\"APx-AAA=\"" },
        { { 5, (unsigned char *)"\x00\xfc\x7e\x00\x7e" }, "\"APx-AH4=\"" },
        { { 5, (unsigned char *)"\x00\xfc\x7e\x00\xfc" }, "\"APx-APw=\"" },
        { { 5, (unsigned char *)"\x00\xfc\x7e\x7e\x00" }, "\"APx-fgA=\"" },
        { { 5, (unsigned char *)"\x00\xfc\x7e\x7e\x7e" }, "\"APx-fn4=\"" },
        { { 5, (unsigned char *)"\x00\xfc\x7e\x7e\xfc" }, "\"APx-fvw=\"" },
        { { 5, (unsigned char *)"\x00\xfc\x7e\xfc\x00" }, "\"APx-_AA=\"" },
        { { 5, (unsigned char *)"\x00\xfc\x7e\xfc\x7e" }, "\"APx-_H4=\"" },
        { { 5, (unsigned char *)"\x00\xfc\x7e\xfc\xfc" }, "\"APx-_Pw=\"" },
        { { 5, (unsigned char *)"\x00\xfc\xfc\x00\x00" }, "\"APz8AAA=\"" },
        { { 5, (unsigned char *)"\x00\xfc\xfc\x00\x7e" }, "\"APz8AH4=\"" },
        { { 5, (unsigned char *)"\x00\xfc\xfc\x00\xfc" }, "\"APz8APw=\"" },
        { { 5, (unsigned char *)"\x00\xfc\xfc\x7e\x00" }, "\"APz8fgA=\"" },
        { { 5, (unsigned char *)"\x00\xfc\xfc\x7e\x7e" }, "\"APz8fn4=\"" },
        { { 5, (unsigned char *)"\x00\xfc\xfc\x7e\xfc" }, "\"APz8fvw=\"" },
        { { 5, (unsigned char *)"\x00\xfc\xfc\xfc\x00" }, "\"APz8_AA=\"" },
        { { 5, (unsigned char *)"\x00\xfc\xfc\xfc\x7e" }, "\"APz8_H4=\"" },
        { { 5, (unsigned char *)"\x00\xfc\xfc\xfc\xfc" }, "\"APz8_Pw=\"" },
        { { 5, (unsigned char *)"\x7e\x00\x00\x00\x00" }, "\"fgAAAAA=\"" },
        { { 5, (unsigned char *)"\x7e\x00\x00\x00\x7e" }, "\"fgAAAH4=\"" },
        { { 5, (unsigned char *)"\x7e\x00\x00\x00\xfc" }, "\"fgAAAPw=\"" },
        { { 5, (unsigned char *)"\x7e\x00\x00\x7e\x00" }, "\"fgAAfgA=\"" },
        { { 5, (unsigned char *)"\x7e\x00\x00\x7e\x7e" }, "\"fgAAfn4=\"" },
        { { 5, (unsigned char *)"\x7e\x00\x00\x7e\xfc" }, "\"fgAAfvw=\"" },
        { { 5, (unsigned char *)"\x7e\x00\x00\xfc\x00" }, "\"fgAA_AA=\"" },
        { { 5, (unsigned char *)"\x7e\x00\x00\xfc\x7e" }, "\"fgAA_H4=\"" },
        { { 5, (unsigned char *)"\x7e\x00\x00\xfc\xfc" }, "\"fgAA_Pw=\"" },
        { { 5, (unsigned char *)"\x7e\x00\x7e\x00\x00" }, "\"fgB-AAA=\"" },
        { { 5, (unsigned char *)"\x7e\x00\x7e\x00\x7e" }, "\"fgB-AH4=\"" },
        { { 5, (unsigned char *)"\x7e\x00\x7e\x00\xfc" }, "\"fgB-APw=\"" },
        { { 5, (unsigned char *)"\x7e\x00\x7e\x7e\x00" }, "\"fgB-fgA=\"" },
        { { 5, (unsigned char *)"\x7e\x00\x7e\x7e\x7e" }, "\"fgB-fn4=\"" },
        { { 5, (unsigned char *)"\x7e\x00\x7e\x7e\xfc" }, "\"fgB-fvw=\"" },
        { { 5, (unsigned char *)"\x7e\x00\x7e\xfc\x00" }, "\"fgB-_AA=\"" },
        { { 5, (unsigned char *)"\x7e\x00\x7e\xfc\x7e" }, "\"fgB-_H4=\"" },
        { { 5, (unsigned char *)"\x7e\x00\x7e\xfc\xfc" }, "\"fgB-_Pw=\"" },
        { { 5, (unsigned char *)"\x7e\x00\xfc\x00\x00" }, "\"fgD8AAA=\"" },
        { { 5, (unsigned char *)"\x7e\x00\xfc\x00\x7e" }, "\"fgD8AH4=\"" },
        { { 5, (unsigned char *)"\x7e\x00\xfc\x00\xfc" }, "\"fgD8APw=\"" },
        { { 5, (unsigned char *)"\x7e\x00\xfc\x7e\x00" }, "\"fgD8fgA=\"" },
        { { 5, (unsigned char *)"\x7e\x00\xfc\x7e\x7e" }, "\"fgD8fn4=\"" },
        { { 5, (unsigned char *)"\x7e\x00\xfc\x7e\xfc" }, "\"fgD8fvw=\"" },
        { { 5, (unsigned char *)"\x7e\x00\xfc\xfc\x00" }, "\"fgD8_AA=\"" },
        { { 5, (unsigned char *)"\x7e\x00\xfc\xfc\x7e" }, "\"fgD8_H4=\"" },
        { { 5, (unsigned char *)"\x7e\x00\xfc\xfc\xfc" }, "\"fgD8_Pw=\"" },
        { { 5, (unsigned char *)"\x7e\x7e\x00\x00\x00" }, "\"fn4AAAA=\"" },
        { { 5, (unsigned char *)"\x7e\x7e\x00\x00\x7e" }, "\"fn4AAH4=\"" },
        { { 5, (unsigned char *)"\x7e\x7e\x00\x00\xfc" }, "\"fn4AAPw=\"" },
        { { 5, (unsigned char *)"\x7e\x7e\x00\x7e\x00" }, "\"fn4AfgA=\"" },
        { { 5, (unsigned char *)"\x7e\x7e\x00\x7e\x7e" }, "\"fn4Afn4=\"" },
        { { 5, (unsigned char *)"\x7e\x7e\x00\x7e\xfc" }, "\"fn4Afvw=\"" },
        { { 5, (unsigned char *)"\x7e\x7e\x00\xfc\x00" }, "\"fn4A_AA=\"" },
        { { 5, (unsigned char *)"\x7e\x7e\x00\xfc\x7e" }, "\"fn4A_H4=\"" },
        { { 5, (unsigned char *)"\x7e\x7e\x00\xfc\xfc" }, "\"fn4A_Pw=\"" },
        { { 5, (unsigned char *)"\x7e\x7e\x7e\x00\x00" }, "\"fn5-AAA=\"" },
        { { 5, (unsigned char *)"\x7e\x7e\x7e\x00\x7e" }, "\"fn5-AH4=\"" },
        { { 5, (unsigned char *)"\x7e\x7e\x7e\x00\xfc" }, "\"fn5-APw=\"" },
        { { 5, (unsigned char *)"\x7e\x7e\x7e\x7e\x00" }, "\"fn5-fgA=\"" },
        { { 5, (unsigned char *)"\x7e\x7e\x7e\x7e\x7e" }, "\"fn5-fn4=\"" },
        { { 5, (unsigned char *)"\x7e\x7e\x7e\x7e\xfc" }, "\"fn5-fvw=\"" },
        { { 5, (unsigned char *)"\x7e\x7e\x7e\xfc\x00" }, "\"fn5-_AA=\"" },
        { { 5, (unsigned char *)"\x7e\x7e\x7e\xfc\x7e" }, "\"fn5-_H4=\"" },
        { { 5, (unsigned char *)"\x7e\x7e\x7e\xfc\xfc" }, "\"fn5-_Pw=\"" },
        { { 5, (unsigned char *)"\x7e\x7e\xfc\x00\x00" }, "\"fn78AAA=\"" },
        { { 5, (unsigned char *)"\x7e\x7e\xfc\x00\x7e" }, "\"fn78AH4=\"" },
        { { 5, (unsigned char *)"\x7e\x7e\xfc\x00\xfc" }, "\"fn78APw=\"" },
        { { 5, (unsigned char *)"\x7e\x7e\xfc\x7e\x00" }, "\"fn78fgA=\"" },
        { { 5, (unsigned char *)"\x7e\x7e\xfc\x7e\x7e" }, "\"fn78fn4=\"" },
        { { 5, (unsigned char *)"\x7e\x7e\xfc\x7e\xfc" }, "\"fn78fvw=\"" },
        { { 5, (unsigned char *)"\x7e\x7e\xfc\xfc\x00" }, "\"fn78_AA=\"" },
        { { 5, (unsigned char *)"\x7e\x7e\xfc\xfc\x7e" }, "\"fn78_H4=\"" },
        { { 5, (unsigned char *)"\x7e\x7e\xfc\xfc\xfc" }, "\"fn78_Pw=\"" },
        { { 5, (unsigned char *)"\x7e\xfc\x00\x00\x00" }, "\"fvwAAAA=\"" },
        { { 5, (unsigned char *)"\x7e\xfc\x00\x00\x7e" }, "\"fvwAAH4=\"" },
        { { 5, (unsigned char *)"\x7e\xfc\x00\x00\xfc" }, "\"fvwAAPw=\"" },
        { { 5, (unsigned char *)"\x7e\xfc\x00\x7e\x00" }, "\"fvwAfgA=\"" },
        { { 5, (unsigned char *)"\x7e\xfc\x00\x7e\x7e" }, "\"fvwAfn4=\"" },
        { { 5, (unsigned char *)"\x7e\xfc\x00\x7e\xfc" }, "\"fvwAfvw=\"" },
        { { 5, (unsigned char *)"\x7e\xfc\x00\xfc\x00" }, "\"fvwA_AA=\"" },
        { { 5, (unsigned char *)"\x7e\xfc\x00\xfc\x7e" }, "\"fvwA_H4=\"" },
        { { 5, (unsigned char *)"\x7e\xfc\x00\xfc\xfc" }, "\"fvwA_Pw=\"" },
        { { 5, (unsigned char *)"\x7e\xfc\x7e\x00\x00" }, "\"fvx-AAA=\"" },
        { { 5, (unsigned char *)"\x7e\xfc\x7e\x00\x7e" }, "\"fvx-AH4=\"" },
        { { 5, (unsigned char *)"\x7e\xfc\x7e\x00\xfc" }, "\"fvx-APw=\"" },
        { { 5, (unsigned char *)"\x7e\xfc\x7e\x7e\x00" }, "\"fvx-fgA=\"" },
        { { 5, (unsigned char *)"\x7e\xfc\x7e\x7e\x7e" }, "\"fvx-fn4=\"" },
        { { 5, (unsigned char *)"\x7e\xfc\x7e\x7e\xfc" }, "\"fvx-fvw=\"" },
        { { 5, (unsigned char *)"\x7e\xfc\x7e\xfc\x00" }, "\"fvx-_AA=\"" },
        { { 5, (unsigned char *)"\x7e\xfc\x7e\xfc\x7e" }, "\"fvx-_H4=\"" },
        { { 5, (unsigned char *)"\x7e\xfc\x7e\xfc\xfc" }, "\"fvx-_Pw=\"" },
        { { 5, (unsigned char *)"\x7e\xfc\xfc\x00\x00" }, "\"fvz8AAA=\"" },
        { { 5, (unsigned char *)"\x7e\xfc\xfc\x00\x7e" }, "\"fvz8AH4=\"" },
        { { 5, (unsigned char *)"\x7e\xfc\xfc\x00\xfc" }, "\"fvz8APw=\"" },
        { { 5, (unsigned char *)"\x7e\xfc\xfc\x7e\x00" }, "\"fvz8fgA=\"" },
        { { 5, (unsigned char *)"\x7e\xfc\xfc\x7e\x7e" }, "\"fvz8fn4=\"" },
        { { 5, (unsigned char *)"\x7e\xfc\xfc\x7e\xfc" }, "\"fvz8fvw=\"" },
        { { 5, (unsigned char *)"\x7e\xfc\xfc\xfc\x00" }, "\"fvz8_AA=\"" },
        { { 5, (unsigned char *)"\x7e\xfc\xfc\xfc\x7e" }, "\"fvz8_H4=\"" },
        { { 5, (unsigned char *)"\x7e\xfc\xfc\xfc\xfc" }, "\"fvz8_Pw=\"" },
        { { 5, (unsigned char *)"\xfc\x00\x00\x00\x00" }, "\"_AAAAAA=\"" },
        { { 5, (unsigned char *)"\xfc\x00\x00\x00\x7e" }, "\"_AAAAH4=\"" },
        { { 5, (unsigned char *)"\xfc\x00\x00\x00\xfc" }, "\"_AAAAPw=\"" },
        { { 5, (unsigned char *)"\xfc\x00\x00\x7e\x00" }, "\"_AAAfgA=\"" },
        { { 5, (unsigned char *)"\xfc\x00\x00\x7e\x7e" }, "\"_AAAfn4=\"" },
        { { 5, (unsigned char *)"\xfc\x00\x00\x7e\xfc" }, "\"_AAAfvw=\"" },
        { { 5, (unsigned char *)"\xfc\x00\x00\xfc\x00" }, "\"_AAA_AA=\"" },
        { { 5, (unsigned char *)"\xfc\x00\x00\xfc\x7e" }, "\"_AAA_H4=\"" },
        { { 5, (unsigned char *)"\xfc\x00\x00\xfc\xfc" }, "\"_AAA_Pw=\"" },
        { { 5, (unsigned char *)"\xfc\x00\x7e\x00\x00" }, "\"_AB-AAA=\"" },
        { { 5, (unsigned char *)"\xfc\x00\x7e\x00\x7e" }, "\"_AB-AH4=\"" },
        { { 5, (unsigned char *)"\xfc\x00\x7e\x00\xfc" }, "\"_AB-APw=\"" },
        { { 5, (unsigned char *)"\xfc\x00\x7e\x7e\x00" }, "\"_AB-fgA=\"" },
        { { 5, (unsigned char *)"\xfc\x00\x7e\x7e\x7e" }, "\"_AB-fn4=\"" },
        { { 5, (unsigned char *)"\xfc\x00\x7e\x7e\xfc" }, "\"_AB-fvw=\"" },
        { { 5, (unsigned char *)"\xfc\x00\x7e\xfc\x00" }, "\"_AB-_AA=\"" },
        { { 5, (unsigned char *)"\xfc\x00\x7e\xfc\x7e" }, "\"_AB-_H4=\"" },
        { { 5, (unsigned char *)"\xfc\x00\x7e\xfc\xfc" }, "\"_AB-_Pw=\"" },
        { { 5, (unsigned char *)"\xfc\x00\xfc\x00\x00" }, "\"_AD8AAA=\"" },
        { { 5, (unsigned char *)"\xfc\x00\xfc\x00\x7e" }, "\"_AD8AH4=\"" },
        { { 5, (unsigned char *)"\xfc\x00\xfc\x00\xfc" }, "\"_AD8APw=\"" },
        { { 5, (unsigned char *)"\xfc\x00\xfc\x7e\x00" }, "\"_AD8fgA=\"" },
        { { 5, (unsigned char *)"\xfc\x00\xfc\x7e\x7e" }, "\"_AD8fn4=\"" },
        { { 5, (unsigned char *)"\xfc\x00\xfc\x7e\xfc" }, "\"_AD8fvw=\"" },
        { { 5, (unsigned char *)"\xfc\x00\xfc\xfc\x00" }, "\"_AD8_AA=\"" },
        { { 5, (unsigned char *)"\xfc\x00\xfc\xfc\x7e" }, "\"_AD8_H4=\"" },
        { { 5, (unsigned char *)"\xfc\x00\xfc\xfc\xfc" }, "\"_AD8_Pw=\"" },
        { { 5, (unsigned char *)"\xfc\x7e\x00\x00\x00" }, "\"_H4AAAA=\"" },
        { { 5, (unsigned char *)"\xfc\x7e\x00\x00\x7e" }, "\"_H4AAH4=\"" },
        { { 5, (unsigned char *)"\xfc\x7e\x00\x00\xfc" }, "\"_H4AAPw=\"" },
        { { 5, (unsigned char *)"\xfc\x7e\x00\x7e\x00" }, "\"_H4AfgA=\"" },
        { { 5, (unsigned char *)"\xfc\x7e\x00\x7e\x7e" }, "\"_H4Afn4=\"" },
        { { 5, (unsigned char *)"\xfc\x7e\x00\x7e\xfc" }, "\"_H4Afvw=\"" },
        { { 5, (unsigned char *)"\xfc\x7e\x00\xfc\x00" }, "\"_H4A_AA=\"" },
        { { 5, (unsigned char *)"\xfc\x7e\x00\xfc\x7e" }, "\"_H4A_H4=\"" },
        { { 5, (unsigned char *)"\xfc\x7e\x00\xfc\xfc" }, "\"_H4A_Pw=\"" },
        { { 5, (unsigned char *)"\xfc\x7e\x7e\x00\x00" }, "\"_H5-AAA=\"" },
        { { 5, (unsigned char *)"\xfc\x7e\x7e\x00\x7e" }, "\"_H5-AH4=\"" },
        { { 5, (unsigned char *)"\xfc\x7e\x7e\x00\xfc" }, "\"_H5-APw=\"" },
        { { 5, (unsigned char *)"\xfc\x7e\x7e\x7e\x00" }, "\"_H5-fgA=\"" },
        { { 5, (unsigned char *)"\xfc\x7e\x7e\x7e\x7e" }, "\"_H5-fn4=\"" },
        { { 5, (unsigned char *)"\xfc\x7e\x7e\x7e\xfc" }, "\"_H5-fvw=\"" },
        { { 5, (unsigned char *)"\xfc\x7e\x7e\xfc\x00" }, "\"_H5-_AA=\"" },
        { { 5, (unsigned char *)"\xfc\x7e\x7e\xfc\x7e" }, "\"_H5-_H4=\"" },
        { { 5, (unsigned char *)"\xfc\x7e\x7e\xfc\xfc" }, "\"_H5-_Pw=\"" },
        { { 5, (unsigned char *)"\xfc\x7e\xfc\x00\x00" }, "\"_H78AAA=\"" },
        { { 5, (unsigned char *)"\xfc\x7e\xfc\x00\x7e" }, "\"_H78AH4=\"" },
        { { 5, (unsigned char *)"\xfc\x7e\xfc\x00\xfc" }, "\"_H78APw=\"" },
        { { 5, (unsigned char *)"\xfc\x7e\xfc\x7e\x00" }, "\"_H78fgA=\"" },
        { { 5, (unsigned char *)"\xfc\x7e\xfc\x7e\x7e" }, "\"_H78fn4=\"" },
        { { 5, (unsigned char *)"\xfc\x7e\xfc\x7e\xfc" }, "\"_H78fvw=\"" },
        { { 5, (unsigned char *)"\xfc\x7e\xfc\xfc\x00" }, "\"_H78_AA=\"" },
        { { 5, (unsigned char *)"\xfc\x7e\xfc\xfc\x7e" }, "\"_H78_H4=\"" },
        { { 5, (unsigned char *)"\xfc\x7e\xfc\xfc\xfc" }, "\"_H78_Pw=\"" },
        { { 5, (unsigned char *)"\xfc\xfc\x00\x00\x00" }, "\"_PwAAAA=\"" },
        { { 5, (unsigned char *)"\xfc\xfc\x00\x00\x7e" }, "\"_PwAAH4=\"" },
        { { 5, (unsigned char *)"\xfc\xfc\x00\x00\xfc" }, "\"_PwAAPw=\"" },
        { { 5, (unsigned char *)"\xfc\xfc\x00\x7e\x00" }, "\"_PwAfgA=\"" },
        { { 5, (unsigned char *)"\xfc\xfc\x00\x7e\x7e" }, "\"_PwAfn4=\"" },
        { { 5, (unsigned char *)"\xfc\xfc\x00\x7e\xfc" }, "\"_PwAfvw=\"" },
        { { 5, (unsigned char *)"\xfc\xfc\x00\xfc\x00" }, "\"_PwA_AA=\"" },
        { { 5, (unsigned char *)"\xfc\xfc\x00\xfc\x7e" }, "\"_PwA_H4=\"" },
        { { 5, (unsigned char *)"\xfc\xfc\x00\xfc\xfc" }, "\"_PwA_Pw=\"" },
        { { 5, (unsigned char *)"\xfc\xfc\x7e\x00\x00" }, "\"_Px-AAA=\"" },
        { { 5, (unsigned char *)"\xfc\xfc\x7e\x00\x7e" }, "\"_Px-AH4=\"" },
        { { 5, (unsigned char *)"\xfc\xfc\x7e\x00\xfc" }, "\"_Px-APw=\"" },
        { { 5, (unsigned char *)"\xfc\xfc\x7e\x7e\x00" }, "\"_Px-fgA=\"" },
        { { 5, (unsigned char *)"\xfc\xfc\x7e\x7e\x7e" }, "\"_Px-fn4=\"" },
        { { 5, (unsigned char *)"\xfc\xfc\x7e\x7e\xfc" }, "\"_Px-fvw=\"" },
        { { 5, (unsigned char *)"\xfc\xfc\x7e\xfc\x00" }, "\"_Px-_AA=\"" },
        { { 5, (unsigned char *)"\xfc\xfc\x7e\xfc\x7e" }, "\"_Px-_H4=\"" },
        { { 5, (unsigned char *)"\xfc\xfc\x7e\xfc\xfc" }, "\"_Px-_Pw=\"" },
        { { 5, (unsigned char *)"\xfc\xfc\xfc\x00\x00" }, "\"_Pz8AAA=\"" },
        { { 5, (unsigned char *)"\xfc\xfc\xfc\x00\x7e" }, "\"_Pz8AH4=\"" },
        { { 5, (unsigned char *)"\xfc\xfc\xfc\x00\xfc" }, "\"_Pz8APw=\"" },
        { { 5, (unsigned char *)"\xfc\xfc\xfc\x7e\x00" }, "\"_Pz8fgA=\"" },
        { { 5, (unsigned char *)"\xfc\xfc\xfc\x7e\x7e" }, "\"_Pz8fn4=\"" },
        { { 5, (unsigned char *)"\xfc\xfc\xfc\x7e\xfc" }, "\"_Pz8fvw=\"" },
        { { 5, (unsigned char *)"\xfc\xfc\xfc\xfc\x00" }, "\"_Pz8_AA=\"" },
        { { 5, (unsigned char *)"\xfc\xfc\xfc\xfc\x7e" }, "\"_Pz8_H4=\"" },
        { { 5, (unsigned char *)"\xfc\xfc\xfc\xfc\xfc" }, "\"_Pz8_Pw=\"" },
        { { 6, (unsigned char *)"\x00\x00\x00\x00\x00\x00" }, "\"AAAAAAAA\"" },
        { { 6, (unsigned char *)"\x00\x00\x00\x00\x00\xa8" }, "\"AAAAAACo\"" },
        { { 6, (unsigned char *)"\x00\x00\x00\x00\xa8\x00" }, "\"AAAAAKgA\"" },
        { { 6, (unsigned char *)"\x00\x00\x00\x00\xa8\xa8" }, "\"AAAAAKio\"" },
        { { 6, (unsigned char *)"\x00\x00\x00\xa8\x00\x00" }, "\"AAAAqAAA\"" },
        { { 6, (unsigned char *)"\x00\x00\x00\xa8\x00\xa8" }, "\"AAAAqACo\"" },
        { { 6, (unsigned char *)"\x00\x00\x00\xa8\xa8\x00" }, "\"AAAAqKgA\"" },
        { { 6, (unsigned char *)"\x00\x00\x00\xa8\xa8\xa8" }, "\"AAAAqKio\"" },
        { { 6, (unsigned char *)"\x00\x00\xa8\x00\x00\x00" }, "\"AACoAAAA\"" },
        { { 6, (unsigned char *)"\x00\x00\xa8\x00\x00\xa8" }, "\"AACoAACo\"" },
        { { 6, (unsigned char *)"\x00\x00\xa8\x00\xa8\x00" }, "\"AACoAKgA\"" },
        { { 6, (unsigned char *)"\x00\x00\xa8\x00\xa8\xa8" }, "\"AACoAKio\"" },
        { { 6, (unsigned char *)"\x00\x00\xa8\xa8\x00\x00" }, "\"AACoqAAA\"" },
        { { 6, (unsigned char *)"\x00\x00\xa8\xa8\x00\xa8" }, "\"AACoqACo\"" },
        { { 6, (unsigned char *)"\x00\x00\xa8\xa8\xa8\x00" }, "\"AACoqKgA\"" },
        { { 6, (unsigned char *)"\x00\x00\xa8\xa8\xa8\xa8" }, "\"AACoqKio\"" },
        { { 6, (unsigned char *)"\x00\xa8\x00\x00\x00\x00" }, "\"AKgAAAAA\"" },
        { { 6, (unsigned char *)"\x00\xa8\x00\x00\x00\xa8" }, "\"AKgAAACo\"" },
        { { 6, (unsigned char *)"\x00\xa8\x00\x00\xa8\x00" }, "\"AKgAAKgA\"" },
        { { 6, (unsigned char *)"\x00\xa8\x00\x00\xa8\xa8" }, "\"AKgAAKio\"" },
        { { 6, (unsigned char *)"\x00\xa8\x00\xa8\x00\x00" }, "\"AKgAqAAA\"" },
        { { 6, (unsigned char *)"\x00\xa8\x00\xa8\x00\xa8" }, "\"AKgAqACo\"" },
        { { 6, (unsigned char *)"\x00\xa8\x00\xa8\xa8\x00" }, "\"AKgAqKgA\"" },
        { { 6, (unsigned char *)"\x00\xa8\x00\xa8\xa8\xa8" }, "\"AKgAqKio\"" },
        { { 6, (unsigned char *)"\x00\xa8\xa8\x00\x00\x00" }, "\"AKioAAAA\"" },
        { { 6, (unsigned char *)"\x00\xa8\xa8\x00\x00\xa8" }, "\"AKioAACo\"" },
        { { 6, (unsigned char *)"\x00\xa8\xa8\x00\xa8\x00" }, "\"AKioAKgA\"" },
        { { 6, (unsigned char *)"\x00\xa8\xa8\x00\xa8\xa8" }, "\"AKioAKio\"" },
        { { 6, (unsigned char *)"\x00\xa8\xa8\xa8\x00\x00" }, "\"AKioqAAA\"" },
        { { 6, (unsigned char *)"\x00\xa8\xa8\xa8\x00\xa8" }, "\"AKioqACo\"" },
        { { 6, (unsigned char *)"\x00\xa8\xa8\xa8\xa8\x00" }, "\"AKioqKgA\"" },
        { { 6, (unsigned char *)"\x00\xa8\xa8\xa8\xa8\xa8" }, "\"AKioqKio\"" },
        { { 6, (unsigned char *)"\xa8\x00\x00\x00\x00\x00" }, "\"qAAAAAAA\"" },
        { { 6, (unsigned char *)"\xa8\x00\x00\x00\x00\xa8" }, "\"qAAAAACo\"" },
        { { 6, (unsigned char *)"\xa8\x00\x00\x00\xa8\x00" }, "\"qAAAAKgA\"" },
        { { 6, (unsigned char *)"\xa8\x00\x00\x00\xa8\xa8" }, "\"qAAAAKio\"" },
        { { 6, (unsigned char *)"\xa8\x00\x00\xa8\x00\x00" }, "\"qAAAqAAA\"" },
        { { 6, (unsigned char *)"\xa8\x00\x00\xa8\x00\xa8" }, "\"qAAAqACo\"" },
        { { 6, (unsigned char *)"\xa8\x00\x00\xa8\xa8\x00" }, "\"qAAAqKgA\"" },
        { { 6, (unsigned char *)"\xa8\x00\x00\xa8\xa8\xa8" }, "\"qAAAqKio\"" },
        { { 6, (unsigned char *)"\xa8\x00\xa8\x00\x00\x00" }, "\"qACoAAAA\"" },
        { { 6, (unsigned char *)"\xa8\x00\xa8\x00\x00\xa8" }, "\"qACoAACo\"" },
        { { 6, (unsigned char *)"\xa8\x00\xa8\x00\xa8\x00" }, "\"qACoAKgA\"" },
        { { 6, (unsigned char *)"\xa8\x00\xa8\x00\xa8\xa8" }, "\"qACoAKio\"" },
        { { 6, (unsigned char *)"\xa8\x00\xa8\xa8\x00\x00" }, "\"qACoqAAA\"" },
        { { 6, (unsigned char *)"\xa8\x00\xa8\xa8\x00\xa8" }, "\"qACoqACo\"" },
        { { 6, (unsigned char *)"\xa8\x00\xa8\xa8\xa8\x00" }, "\"qACoqKgA\"" },
        { { 6, (unsigned char *)"\xa8\x00\xa8\xa8\xa8\xa8" }, "\"qACoqKio\"" },
        { { 6, (unsigned char *)"\xa8\xa8\x00\x00\x00\x00" }, "\"qKgAAAAA\"" },
        { { 6, (unsigned char *)"\xa8\xa8\x00\x00\x00\xa8" }, "\"qKgAAACo\"" },
        { { 6, (unsigned char *)"\xa8\xa8\x00\x00\xa8\x00" }, "\"qKgAAKgA\"" },
        { { 6, (unsigned char *)"\xa8\xa8\x00\x00\xa8\xa8" }, "\"qKgAAKio\"" },
        { { 6, (unsigned char *)"\xa8\xa8\x00\xa8\x00\x00" }, "\"qKgAqAAA\"" },
        { { 6, (unsigned char *)"\xa8\xa8\x00\xa8\x00\xa8" }, "\"qKgAqACo\"" },
        { { 6, (unsigned char *)"\xa8\xa8\x00\xa8\xa8\x00" }, "\"qKgAqKgA\"" },
        { { 6, (unsigned char *)"\xa8\xa8\x00\xa8\xa8\xa8" }, "\"qKgAqKio\"" },
        { { 6, (unsigned char *)"\xa8\xa8\xa8\x00\x00\x00" }, "\"qKioAAAA\"" },
        { { 6, (unsigned char *)"\xa8\xa8\xa8\x00\x00\xa8" }, "\"qKioAACo\"" },
        { { 6, (unsigned char *)"\xa8\xa8\xa8\x00\xa8\x00" }, "\"qKioAKgA\"" },
        { { 6, (unsigned char *)"\xa8\xa8\xa8\x00\xa8\xa8" }, "\"qKioAKio\"" },
        { { 6, (unsigned char *)"\xa8\xa8\xa8\xa8\x00\x00" }, "\"qKioqAAA\"" },
        { { 6, (unsigned char *)"\xa8\xa8\xa8\xa8\x00\xa8" }, "\"qKioqACo\"" },
        { { 6, (unsigned char *)"\xa8\xa8\xa8\xa8\xa8\x00" }, "\"qKioqKgA\"" },
        { { 6, (unsigned char *)"\xa8\xa8\xa8\xa8\xa8\xa8" }, "\"qKioqKio\"" },
        { { 7, (unsigned char *)"\x00\x00\x00\x00\x00\x00\x00" }, "\"AAAAAAAAAA==\"" },
        { { 7, (unsigned char *)"\x00\x00\x00\x00\x00\x00\xd3" }, "\"AAAAAAAA0w==\"" },
        { { 7, (unsigned char *)"\x00\x00\x00\x00\x00\xd3\x00" }, "\"AAAAAADTAA==\"" },
        { { 7, (unsigned char *)"\x00\x00\x00\x00\x00\xd3\xd3" }, "\"AAAAAADT0w==\"" },
        { { 7, (unsigned char *)"\x00\x00\x00\x00\xd3\x00\x00" }, "\"AAAAANMAAA==\"" },
        { { 7, (unsigned char *)"\x00\x00\x00\x00\xd3\x00\xd3" }, "\"AAAAANMA0w==\"" },
        { { 7, (unsigned char *)"\x00\x00\x00\x00\xd3\xd3\x00" }, "\"AAAAANPTAA==\"" },
        { { 7, (unsigned char *)"\x00\x00\x00\x00\xd3\xd3\xd3" }, "\"AAAAANPT0w==\"" },
        { { 7, (unsigned char *)"\x00\x00\x00\xd3\x00\x00\x00" }, "\"AAAA0wAAAA==\"" },
        { { 7, (unsigned char *)"\x00\x00\x00\xd3\x00\x00\xd3" }, "\"AAAA0wAA0w==\"" },
        { { 7, (unsigned char *)"\x00\x00\x00\xd3\x00\xd3\x00" }, "\"AAAA0wDTAA==\"" },
        { { 7, (unsigned char *)"\x00\x00\x00\xd3\x00\xd3\xd3" }, "\"AAAA0wDT0w==\"" },
        { { 7, (unsigned char *)"\x00\x00\x00\xd3\xd3\x00\x00" }, "\"AAAA09MAAA==\"" },
        { { 7, (unsigned char *)"\x00\x00\x00\xd3\xd3\x00\xd3" }, "\"AAAA09MA0w==\"" },
        { { 7, (unsigned char *)"\x00\x00\x00\xd3\xd3\xd3\x00" }, "\"AAAA09PTAA==\"" },
        { { 7, (unsigned char *)"\x00\x00\x00\xd3\xd3\xd3\xd3" }, "\"AAAA09PT0w==\"" },
        { { 7, (unsigned char *)"\x00\x00\xd3\x00\x00\x00\x00" }, "\"AADTAAAAAA==\"" },
        { { 7, (unsigned char *)"\x00\x00\xd3\x00\x00\x00\xd3" }, "\"AADTAAAA0w==\"" },
        { { 7, (unsigned char *)"\x00\x00\xd3\x00\x00\xd3\x00" }, "\"AADTAADTAA==\"" },
        { { 7, (unsigned char *)"\x00\x00\xd3\x00\x00\xd3\xd3" }, "\"AADTAADT0w==\"" },
        { { 7, (unsigned char *)"\x00\x00\xd3\x00\xd3\x00\x00" }, "\"AADTANMAAA==\"" },
        { { 7, (unsigned char *)"\x00\x00\xd3\x00\xd3\x00\xd3" }, "\"AADTANMA0w==\"" },
        { { 7, (unsigned char *)"\x00\x00\xd3\x00\xd3\xd3\x00" }, "\"AADTANPTAA==\"" },
        { { 7, (unsigned char *)"\x00\x00\xd3\x00\xd3\xd3\xd3" }, "\"AADTANPT0w==\"" },
        { { 7, (unsigned char *)"\x00\x00\xd3\xd3\x00\x00\x00" }, "\"AADT0wAAAA==\"" },
        { { 7, (unsigned char *)"\x00\x00\xd3\xd3\x00\x00\xd3" }, "\"AADT0wAA0w==\"" },
        { { 7, (unsigned char *)"\x00\x00\xd3\xd3\x00\xd3\x00" }, "\"AADT0wDTAA==\"" },
        { { 7, (unsigned char *)"\x00\x00\xd3\xd3\x00\xd3\xd3" }, "\"AADT0wDT0w==\"" },
        { { 7, (unsigned char *)"\x00\x00\xd3\xd3\xd3\x00\x00" }, "\"AADT09MAAA==\"" },
        { { 7, (unsigned char *)"\x00\x00\xd3\xd3\xd3\x00\xd3" }, "\"AADT09MA0w==\"" },
        { { 7, (unsigned char *)"\x00\x00\xd3\xd3\xd3\xd3\x00" }, "\"AADT09PTAA==\"" },
        { { 7, (unsigned char *)"\x00\x00\xd3\xd3\xd3\xd3\xd3" }, "\"AADT09PT0w==\"" },
        { { 7, (unsigned char *)"\x00\xd3\x00\x00\x00\x00\x00" }, "\"ANMAAAAAAA==\"" },
        { { 7, (unsigned char *)"\x00\xd3\x00\x00\x00\x00\xd3" }, "\"ANMAAAAA0w==\"" },
        { { 7, (unsigned char *)"\x00\xd3\x00\x00\x00\xd3\x00" }, "\"ANMAAADTAA==\"" },
        { { 7, (unsigned char *)"\x00\xd3\x00\x00\x00\xd3\xd3" }, "\"ANMAAADT0w==\"" },
        { { 7, (unsigned char *)"\x00\xd3\x00\x00\xd3\x00\x00" }, "\"ANMAANMAAA==\"" },
        { { 7, (unsigned char *)"\x00\xd3\x00\x00\xd3\x00\xd3" }, "\"ANMAANMA0w==\"" },
        { { 7, (unsigned char *)"\x00\xd3\x00\x00\xd3\xd3\x00" }, "\"ANMAANPTAA==\"" },
        { { 7, (unsigned char *)"\x00\xd3\x00\x00\xd3\xd3\xd3" }, "\"ANMAANPT0w==\"" },
        { { 7, (unsigned char *)"\x00\xd3\x00\xd3\x00\x00\x00" }, "\"ANMA0wAAAA==\"" },
        { { 7, (unsigned char *)"\x00\xd3\x00\xd3\x00\x00\xd3" }, "\"ANMA0wAA0w==\"" },
        { { 7, (unsigned char *)"\x00\xd3\x00\xd3\x00\xd3\x00" }, "\"ANMA0wDTAA==\"" },
        { { 7, (unsigned char *)"\x00\xd3\x00\xd3\x00\xd3\xd3" }, "\"ANMA0wDT0w==\"" },
        { { 7, (unsigned char *)"\x00\xd3\x00\xd3\xd3\x00\x00" }, "\"ANMA09MAAA==\"" },
        { { 7, (unsigned char *)"\x00\xd3\x00\xd3\xd3\x00\xd3" }, "\"ANMA09MA0w==\"" },
        { { 7, (unsigned char *)"\x00\xd3\x00\xd3\xd3\xd3\x00" }, "\"ANMA09PTAA==\"" },
        { { 7, (unsigned char *)"\x00\xd3\x00\xd3\xd3\xd3\xd3" }, "\"ANMA09PT0w==\"" },
        { { 7, (unsigned char *)"\x00\xd3\xd3\x00\x00\x00\x00" }, "\"ANPTAAAAAA==\"" },
        { { 7, (unsigned char *)"\x00\xd3\xd3\x00\x00\x00\xd3" }, "\"ANPTAAAA0w==\"" },
        { { 7, (unsigned char *)"\x00\xd3\xd3\x00\x00\xd3\x00" }, "\"ANPTAADTAA==\"" },
        { { 7, (unsigned char *)"\x00\xd3\xd3\x00\x00\xd3\xd3" }, "\"ANPTAADT0w==\"" },
        { { 7, (unsigned char *)"\x00\xd3\xd3\x00\xd3\x00\x00" }, "\"ANPTANMAAA==\"" },
        { { 7, (unsigned char *)"\x00\xd3\xd3\x00\xd3\x00\xd3" }, "\"ANPTANMA0w==\"" },
        { { 7, (unsigned char *)"\x00\xd3\xd3\x00\xd3\xd3\x00" }, "\"ANPTANPTAA==\"" },
        { { 7, (unsigned char *)"\x00\xd3\xd3\x00\xd3\xd3\xd3" }, "\"ANPTANPT0w==\"" },
        { { 7, (unsigned char *)"\x00\xd3\xd3\xd3\x00\x00\x00" }, "\"ANPT0wAAAA==\"" },
        { { 7, (unsigned char *)"\x00\xd3\xd3\xd3\x00\x00\xd3" }, "\"ANPT0wAA0w==\"" },
        { { 7, (unsigned char *)"\x00\xd3\xd3\xd3\x00\xd3\x00" }, "\"ANPT0wDTAA==\"" },
        { { 7, (unsigned char *)"\x00\xd3\xd3\xd3\x00\xd3\xd3" }, "\"ANPT0wDT0w==\"" },
        { { 7, (unsigned char *)"\x00\xd3\xd3\xd3\xd3\x00\x00" }, "\"ANPT09MAAA==\"" },
        { { 7, (unsigned char *)"\x00\xd3\xd3\xd3\xd3\x00\xd3" }, "\"ANPT09MA0w==\"" },
        { { 7, (unsigned char *)"\x00\xd3\xd3\xd3\xd3\xd3\x00" }, "\"ANPT09PTAA==\"" },
        { { 7, (unsigned char *)"\x00\xd3\xd3\xd3\xd3\xd3\xd3" }, "\"ANPT09PT0w==\"" },
        { { 7, (unsigned char *)"\xd3\x00\x00\x00\x00\x00\x00" }, "\"0wAAAAAAAA==\"" },
        { { 7, (unsigned char *)"\xd3\x00\x00\x00\x00\x00\xd3" }, "\"0wAAAAAA0w==\"" },
        { { 7, (unsigned char *)"\xd3\x00\x00\x00\x00\xd3\x00" }, "\"0wAAAADTAA==\"" },
        { { 7, (unsigned char *)"\xd3\x00\x00\x00\x00\xd3\xd3" }, "\"0wAAAADT0w==\"" },
        { { 7, (unsigned char *)"\xd3\x00\x00\x00\xd3\x00\x00" }, "\"0wAAANMAAA==\"" },
        { { 7, (unsigned char *)"\xd3\x00\x00\x00\xd3\x00\xd3" }, "\"0wAAANMA0w==\"" },
        { { 7, (unsigned char *)"\xd3\x00\x00\x00\xd3\xd3\x00" }, "\"0wAAANPTAA==\"" },
        { { 7, (unsigned char *)"\xd3\x00\x00\x00\xd3\xd3\xd3" }, "\"0wAAANPT0w==\"" },
        { { 7, (unsigned char *)"\xd3\x00\x00\xd3\x00\x00\x00" }, "\"0wAA0wAAAA==\"" },
        { { 7, (unsigned char *)"\xd3\x00\x00\xd3\x00\x00\xd3" }, "\"0wAA0wAA0w==\"" },
        { { 7, (unsigned char *)"\xd3\x00\x00\xd3\x00\xd3\x00" }, "\"0wAA0wDTAA==\"" },
        { { 7, (unsigned char *)"\xd3\x00\x00\xd3\x00\xd3\xd3" }, "\"0wAA0wDT0w==\"" },
        { { 7, (unsigned char *)"\xd3\x00\x00\xd3\xd3\x00\x00" }, "\"0wAA09MAAA==\"" },
        { { 7, (unsigned char *)"\xd3\x00\x00\xd3\xd3\x00\xd3" }, "\"0wAA09MA0w==\"" },
        { { 7, (unsigned char *)"\xd3\x00\x00\xd3\xd3\xd3\x00" }, "\"0wAA09PTAA==\"" },
        { { 7, (unsigned char *)"\xd3\x00\x00\xd3\xd3\xd3\xd3" }, "\"0wAA09PT0w==\"" },
        { { 7, (unsigned char *)"\xd3\x00\xd3\x00\x00\x00\x00" }, "\"0wDTAAAAAA==\"" },
        { { 7, (unsigned char *)"\xd3\x00\xd3\x00\x00\x00\xd3" }, "\"0wDTAAAA0w==\"" },
        { { 7, (unsigned char *)"\xd3\x00\xd3\x00\x00\xd3\x00" }, "\"0wDTAADTAA==\"" },
        { { 7, (unsigned char *)"\xd3\x00\xd3\x00\x00\xd3\xd3" }, "\"0wDTAADT0w==\"" },
        { { 7, (unsigned char *)"\xd3\x00\xd3\x00\xd3\x00\x00" }, "\"0wDTANMAAA==\"" },
        { { 7, (unsigned char *)"\xd3\x00\xd3\x00\xd3\x00\xd3" }, "\"0wDTANMA0w==\"" },
        { { 7, (unsigned char *)"\xd3\x00\xd3\x00\xd3\xd3\x00" }, "\"0wDTANPTAA==\"" },
        { { 7, (unsigned char *)"\xd3\x00\xd3\x00\xd3\xd3\xd3" }, "\"0wDTANPT0w==\"" },
        { { 7, (unsigned char *)"\xd3\x00\xd3\xd3\x00\x00\x00" }, "\"0wDT0wAAAA==\"" },
        { { 7, (unsigned char *)"\xd3\x00\xd3\xd3\x00\x00\xd3" }, "\"0wDT0wAA0w==\"" },
        { { 7, (unsigned char *)"\xd3\x00\xd3\xd3\x00\xd3\x00" }, "\"0wDT0wDTAA==\"" },
        { { 7, (unsigned char *)"\xd3\x00\xd3\xd3\x00\xd3\xd3" }, "\"0wDT0wDT0w==\"" },
        { { 7, (unsigned char *)"\xd3\x00\xd3\xd3\xd3\x00\x00" }, "\"0wDT09MAAA==\"" },
        { { 7, (unsigned char *)"\xd3\x00\xd3\xd3\xd3\x00\xd3" }, "\"0wDT09MA0w==\"" },
        { { 7, (unsigned char *)"\xd3\x00\xd3\xd3\xd3\xd3\x00" }, "\"0wDT09PTAA==\"" },
        { { 7, (unsigned char *)"\xd3\x00\xd3\xd3\xd3\xd3\xd3" }, "\"0wDT09PT0w==\"" },
        { { 7, (unsigned char *)"\xd3\xd3\x00\x00\x00\x00\x00" }, "\"09MAAAAAAA==\"" },
        { { 7, (unsigned char *)"\xd3\xd3\x00\x00\x00\x00\xd3" }, "\"09MAAAAA0w==\"" },
        { { 7, (unsigned char *)"\xd3\xd3\x00\x00\x00\xd3\x00" }, "\"09MAAADTAA==\"" },
        { { 7, (unsigned char *)"\xd3\xd3\x00\x00\x00\xd3\xd3" }, "\"09MAAADT0w==\"" },
        { { 7, (unsigned char *)"\xd3\xd3\x00\x00\xd3\x00\x00" }, "\"09MAANMAAA==\"" },
        { { 7, (unsigned char *)"\xd3\xd3\x00\x00\xd3\x00\xd3" }, "\"09MAANMA0w==\"" },
        { { 7, (unsigned char *)"\xd3\xd3\x00\x00\xd3\xd3\x00" }, "\"09MAANPTAA==\"" },
        { { 7, (unsigned char *)"\xd3\xd3\x00\x00\xd3\xd3\xd3" }, "\"09MAANPT0w==\"" },
        { { 7, (unsigned char *)"\xd3\xd3\x00\xd3\x00\x00\x00" }, "\"09MA0wAAAA==\"" },
        { { 7, (unsigned char *)"\xd3\xd3\x00\xd3\x00\x00\xd3" }, "\"09MA0wAA0w==\"" },
        { { 7, (unsigned char *)"\xd3\xd3\x00\xd3\x00\xd3\x00" }, "\"09MA0wDTAA==\"" },
        { { 7, (unsigned char *)"\xd3\xd3\x00\xd3\x00\xd3\xd3" }, "\"09MA0wDT0w==\"" },
        { { 7, (unsigned char *)"\xd3\xd3\x00\xd3\xd3\x00\x00" }, "\"09MA09MAAA==\"" },
        { { 7, (unsigned char *)"\xd3\xd3\x00\xd3\xd3\x00\xd3" }, "\"09MA09MA0w==\"" },
        { { 7, (unsigned char *)"\xd3\xd3\x00\xd3\xd3\xd3\x00" }, "\"09MA09PTAA==\"" },
        { { 7, (unsigned char *)"\xd3\xd3\x00\xd3\xd3\xd3\xd3" }, "\"09MA09PT0w==\"" },
        { { 7, (unsigned char *)"\xd3\xd3\xd3\x00\x00\x00\x00" }, "\"09PTAAAAAA==\"" },
        { { 7, (unsigned char *)"\xd3\xd3\xd3\x00\x00\x00\xd3" }, "\"09PTAAAA0w==\"" },
        { { 7, (unsigned char *)"\xd3\xd3\xd3\x00\x00\xd3\x00" }, "\"09PTAADTAA==\"" },
        { { 7, (unsigned char *)"\xd3\xd3\xd3\x00\x00\xd3\xd3" }, "\"09PTAADT0w==\"" },
        { { 7, (unsigned char *)"\xd3\xd3\xd3\x00\xd3\x00\x00" }, "\"09PTANMAAA==\"" },
        { { 7, (unsigned char *)"\xd3\xd3\xd3\x00\xd3\x00\xd3" }, "\"09PTANMA0w==\"" },
        { { 7, (unsigned char *)"\xd3\xd3\xd3\x00\xd3\xd3\x00" }, "\"09PTANPTAA==\"" },
        { { 7, (unsigned char *)"\xd3\xd3\xd3\x00\xd3\xd3\xd3" }, "\"09PTANPT0w==\"" },
        { { 7, (unsigned char *)"\xd3\xd3\xd3\xd3\x00\x00\x00" }, "\"09PT0wAAAA==\"" },
        { { 7, (unsigned char *)"\xd3\xd3\xd3\xd3\x00\x00\xd3" }, "\"09PT0wAA0w==\"" },
        { { 7, (unsigned char *)"\xd3\xd3\xd3\xd3\x00\xd3\x00" }, "\"09PT0wDTAA==\"" },
        { { 7, (unsigned char *)"\xd3\xd3\xd3\xd3\x00\xd3\xd3" }, "\"09PT0wDT0w==\"" },
        { { 7, (unsigned char *)"\xd3\xd3\xd3\xd3\xd3\x00\x00" }, "\"09PT09MAAA==\"" },
        { { 7, (unsigned char *)"\xd3\xd3\xd3\xd3\xd3\x00\xd3" }, "\"09PT09MA0w==\"" },
        { { 7, (unsigned char *)"\xd3\xd3\xd3\xd3\xd3\xd3\x00" }, "\"09PT09PTAA==\"" },
        { { 7, (unsigned char *)"\xd3\xd3\xd3\xd3\xd3\xd3\xd3" }, "\"09PT09PT0w==\"" },
        { { 8, (unsigned char *)"\x00\x00\x00\x00\x00\x00\x00\x00" }, "\"AAAAAAAAAAA=\"" },
        { { 9, (unsigned char *)"\x00\x00\x00\x00\x00\x00\x00\x00\x00" }, "\"AAAAAAAAAAAA\"" },
        { { 10, (unsigned char *)"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" }, "\"AAAAAAAAAAAAAA==\"" },

    };

char NaN[] = { 'N', 'a', 'N', '\0' };
char PInf[] = { 'I', 'N', 'F', '\0' };
char MInf[] = { '-', 'I', 'N', 'F', '\0' };

#define _STR(N) #N
#define STR(N) _STR(N)

#define TEST_YEAR_1 2014
#define TEST_MONTH_1 11
#define TEST_MDAY_1 12
#define TEST_HOUR_1 16
#define TEST_MIN_1 56
#define TEST_SEC_1 59
#define TEST_TIMEZONE_HOUR_1 11
#define TEST_TIMEZONE_MIN_1 12
#define TEST_TIME_1 "\"2014-12-12T16:56:59Z\""
#define TEST_TIME_TIMEZONE_1 "\"2014-12-12T16:56:59+11:12\""


#define TEST_YEAR_2 1970
#define TEST_MONTH_2 2
#define TEST_MDAY_2 3
#define TEST_HOUR_2 0
#define TEST_MIN_2 5
#define TEST_SEC_2 6
#define TEST_TIMEZONE_HOUR_2 -7
#define TEST_TIMEZONE_MIN_2 8
#define TEST_TIME_2 "\"1970-03-03T00:05:06Z\""
#define TEST_TIME_TIMEZONE_2 "\"1970-03-03T00:05:06-07:08\""

#define TEST_DOUBLE_1 10.5
#define TEST_DOUBLE_2 328647.47547929373980211

#define TEST_FLOAT_1 42.5f
#define TEST_FLOAT_2 42.589123f

#define TEST_STRING_1 "one"
#define TEST_STRING_1_JSON "\"one\""

#define TEST_STRING_2 "o\"n\"e"
#define TEST_STRING_2_JSON "\"o\\\"n\\\"e\""

#define TEST_STRING_3 "abc\t\r\n\f\b/\\\"xyz"
#define TEST_STRING_3_JSON "\"abc\\u0009\\u000D\\u000A\\u000C\\u0008\\/\\\\\\\"xyz\""

/*the following test assets shall be used for COMPLEX_TYPES*/

#define TEST_TYPENAME_GEOLOCATION "GeoLocation"
#define TEST_TYPENAME_GEOLOCATION_MEMBER1_NAME "Lat"
#define TEST_TYPENAME_GEOLOCATION_MEMBER2_NAME "Long"
#define TEST_TYPENAME_GEOLOCATION_NMEMBERS 2
const char * TEST_TYPENAME_GEOLOCATION_MEMBERNAMES[TEST_TYPENAME_GEOLOCATION_NMEMBERS] = { TEST_TYPENAME_GEOLOCATION_MEMBER1_NAME, TEST_TYPENAME_GEOLOCATION_MEMBER2_NAME };
#define TEST_TYPENAME_GEOLOCATION_VANCOUVER_LONG -123
#define TEST_TYPENAME_GEOLOCATION_VANCOUVER_LAT 49
/*this complex type shall be nothing more than a usual struct as below
struct GeoPosition
{
int Lat;
int Long;
}.*/



/*this complex type mimics the type "TruckType" in Expressing Device Models as OData Models*/
/*mimics as in "everything is a struct" (not entity type)*/
/*mimics as in "not everything is in" - HeadUnit is left out, VehicleGPS is left out*/
/*notice how this is quite complex in depth...*/
/*and the type of FuelState has been changed from double to "int"*/
/*also Lat and Long have been changed to int because JSONDecoder*/
/*fuelLevel has been changed to int too*/

#define TEST_TRUCK1_key "cheiaDeLaPoartaVerde"
#define TEST_TRUCK1_VIN "zaibar"
#define TEST_TRUCK1_FuelState 42
#define TEST_TRUCK1_AcState true
#define TEST_TRUCK1_truckStoppedStruct_severity 3
#define TEST_TRUCK1_truckStoppedStruct_locationAndFuel_FuelLevel 99
#define TEST_TRUCK1_truckStoppedStruct_locationAndFuel_geoLocation_lat 45
#define TEST_TRUCK1_truckStoppedStruct_locationAndFuel_geoLocation_long 23

AGENT_DATA_TYPE truck1_fields[5];
const char*     truck1_field_names[5] = { "key", "VIN", "FuelState", "AcState", "truckStoppedStruct" };

AGENT_DATA_TYPE truck1_truckStoppedStruct_fields[2];
const char*     truck1_truckStoppedStruct_field_names[2] = { "severity", "locationAndFuel" };

AGENT_DATA_TYPE truck1_truckStoppedStruct_locationAndFuel_fields[2];
const char*     truck1_truckStoppedStruct_locationAndFuel_field_names[2] = { "FuelLevel", "geoLocation" };

AGENT_DATA_TYPE truck1_truckStoppedStruct_locationAndFuel_geoLocation_fields[2];
const char*     truck1_truckStoppedStruct_locationAndFuel_geoLocation_field_names[2] = { "lat", "long" };


#define TEST_TYPENAME_2_GEOLOCATIONTYPE "GeoLocation"
/*struct GeoLocationType
{
    int lat;
    int long;
}*/

#define TEST_TYPENAME_2_LOCATIONANDFUELTYPE "LocationAndFuelType"
/*struct LocationAndFuelType
{
    int FuelLevel;
    GeoLocationType geoLocation;
}*/

#define TEST_TYPENAME_2_TRUCKSTOPPEDStructTYPE "TruckStoppedStructType"
/*struct TruckStoppedStructType
{
    int severity;
    locationAndFuelType locationAndFuel;
}*/

#define TEST_TYPENAME_2_TRUCKTYPE "TruckType"
/*struct TruckType
{
    string key;
    string VIN;
    int FuelState;
    boolean AcState;
    TruckStoppedStructType truckStoppedStruct;
}*/
    AGENT_DATA_TYPE agSingle1;
    AGENT_DATA_TYPE agSingle2;
    AGENT_DATA_TYPE agSingleQuietNaN;
    AGENT_DATA_TYPE agSingleSingalingNaN;
    AGENT_DATA_TYPE agSinglePlusInf;
    AGENT_DATA_TYPE agSingleMinusInf;

    AGENT_DATA_TYPE agDouble1;
    AGENT_DATA_TYPE agDouble2;
    AGENT_DATA_TYPE agDoubleQuietNaN;
    AGENT_DATA_TYPE agDoubleSignalingNaN;
    AGENT_DATA_TYPE agDoublePlusInf;
    AGENT_DATA_TYPE agDoubleMinusInf;

    struct tm some_tm;
    struct tm some_tm2;

    AGENT_DATA_TYPE agDoubleVancouverLat;
    AGENT_DATA_TYPE agDoubleVancouverLong;
    AGENT_DATA_TYPE TEST_TYPENAME_GEOLOCATION_MEMBERVALUES[TEST_TYPENAME_GEOLOCATION_NMEMBERS];
    AGENT_DATA_TYPE agComplexType1;

    AGENT_DATA_TYPE truck1; /*Targu Jiu is a city in Romania*/


static STRING_HANDLE global_bufferTemp;
static STRING_HANDLE bufferTemp2;


static const struct testVector {
    int year;
    int month;
    int day;
    int hour;
    int min;
    int sec;
    uint8_t hasFractionalSecond;
    uint64_t fractionalSecond;
    uint8_t hasTimeZone;
    int8_t timeZoneHour;
    uint8_t timeZoneMin;
    const char* expectedOutput;
} global_testVector[] =
{
    /*1 digit negative year*/
    { -1, 2, 3, 4, 5, 6, 0, 0, 0, 0, 0, "\"-0001-02-03T04:05:06Z\"" },
    /*2 digit negative year*/
    { -11, 2, 3, 4, 5, 6, 0, 0, 0, 0, 0, "\"-0011-02-03T04:05:06Z\"" },
    /*3 digit negative year*/
    { -111, 2, 3, 4, 5, 6, 0, 0, 0, 0, 0, "\"-0111-02-03T04:05:06Z\"" },
    /*4 digit negative year*/
    { -1111, 2, 3, 4, 5, 6, 0, 0, 0, 0, 0, "\"-1111-02-03T04:05:06Z\"" },
    /*year zero*/
    { 0, 2, 3, 4, 5, 6, 0, 0, 0, 0, 0, "\"0000-02-03T04:05:06Z\"" },
    /*1 digit positive year*/
    { 1, 2, 3, 4, 5, 6, 0, 0, 0, 0, 0, "\"0001-02-03T04:05:06Z\"" },
    /*2 digit positive year*/
    { 11, 2, 3, 4, 5, 6, 0, 0, 0, 0, 0, "\"0011-02-03T04:05:06Z\"" },
    /*3 digit positive year*/
    { 111, 2, 3, 4, 5, 6, 0, 0, 0, 0, 0, "\"0111-02-03T04:05:06Z\"" },
    /*4 digit positive year*/
    { 1111, 2, 3, 4, 5, 6, 0, 0, 0, 0, 0, "\"1111-02-03T04:05:06Z\"" },
    /*1 digit month*/
    { 2014, 2, 3, 4, 5, 6, 0, 0, 0, 0, 0, "\"2014-02-03T04:05:06Z\"" },
    /*2 digit month*/
    { 2014, 11, 3, 4, 5, 6, 0, 0, 0, 0, 0, "\"2014-11-03T04:05:06Z\"" },
    /*1 digit day*/
    { 2014, 6, 3, 4, 5, 6, 0, 0, 0, 0, 0, "\"2014-06-03T04:05:06Z\"" },
    /*2 digit day*/
    { 2014, 6, 13, 4, 5, 6, 0, 0, 0, 0, 0, "\"2014-06-13T04:05:06Z\"" },
    /*1 digit hour*/
    { 2014, 6, 18, 4, 5, 6, 0, 0, 0, 0, 0, "\"2014-06-18T04:05:06Z\"" },
    /*2 digit hour*/
    { 2014, 6, 18, 14, 5, 6, 0, 0, 0, 0, 0, "\"2014-06-18T14:05:06Z\"" },
    /*1 digit minute*/
    { 2014, 6, 18, 13, 5, 6, 0, 0, 0, 0, 0, "\"2014-06-18T13:05:06Z\"" },
    /*2 digit minute*/
    { 2014, 6, 18, 13, 15, 6, 0, 0, 0, 0, 0, "\"2014-06-18T13:15:06Z\"" },
    /*1 digit second*/
    { 2014, 6, 18, 13, 15, 9, 0, 0, 0, 0, 0, "\"2014-06-18T13:15:09Z\"" },
    /*2 digit second*/
    { 2014, 6, 18, 13, 15, 59, 0, 0, 0, 0, 0, "\"2014-06-18T13:15:59Z\"" },
    /*hasFractionalSeconds = 0, hasTimeZone=0*/
    { 2014, 6, 18, 13, 15, 59, 0, 11, 0, 0, 0, "\"2014-06-18T13:15:59Z\"" },
    /*hasFractionalSeconds = 1, 1 digit fractional second, hasTimeZone=0*/
    { 2014, 6, 18, 13, 15, 59, 1, 1, 0, 0, 0, "\"2014-06-18T13:15:59.000000000001Z\"" },
    /*hasFractionalSeconds = 1, 2 digit fractional second, hasTimeZone=0*/
    { 2014, 6, 18, 13, 15, 59, 1, 12, 0, 0, 0, "\"2014-06-18T13:15:59.000000000012Z\"" },
    /*hasFractionalSeconds = 1, 12 digit fractional second, hasTimeZone=0*/
    { 2014, 6, 18, 13, 15, 59, 1, 123456789012, 0, 0, 0, "\"2014-06-18T13:15:59.123456789012Z\"" },
    /*hasFractionalSeconds = 0, hasTimeZone=1, 2 digit negative time zone hour, 1 digit minute*/
    { 2014, 6, 18, 13, 15, 59, 0, 11, 1, -22, 1, "\"2014-06-18T13:15:59-22:01\"" },
    /*hasFractionalSeconds = 0, hasTimeZone=1, 2 digit negative time zone hour, 2 digit minute*/
    { 2014, 6, 18, 13, 15, 59, 0, 11, 1, -22, 11, "\"2014-06-18T13:15:59-22:11\"" },
    /*hasFractionalSeconds = 0, hasTimeZone=1, 1 digit positive time zone hour, 1 digit minute*/
    { 2014, 6, 18, 13, 15, 59, 0, 11, 1, 2, 1, "\"2014-06-18T13:15:59+02:01\"" },
    /*hasFractionalSeconds = 0, hasTimeZone=1, 1 digit positive time zone hour, 2 digit minute*/
    { 2014, 6, 18, 13, 15, 59, 0, 11, 1, 2, 13, "\"2014-06-18T13:15:59+02:13\"" },
    /*hasFractionalSeconds = 0, hasTimeZone=1, 2 digit positive time zone hour, 1 digit minute*/
    { 2014, 6, 18, 13, 15, 59, 0, 11, 1, 23, 8, "\"2014-06-18T13:15:59+23:08\"" },
    /*hasFractionalSeconds = 0, hasTimeZone=1, 2 digit positive time zone hour, 2 digit minute*/

    /*hasFractionalSeconds = 1, 1 digit fractional second, hasTimeZone=1, 2 digit negative time zone hour, 1 digit minute*/
    { 2014, 6, 18, 13, 15, 59, 1, 1, 1, -22, 1, "\"2014-06-18T13:15:59.000000000001-22:01\"" },
    /*hasFractionalSeconds = 1, 1 digit fractional second, hasTimeZone=1, 2 digit negative time zone hour, 2 digit minute*/
    { 2014, 6, 18, 13, 15, 59, 1, 1, 1, -22, 11, "\"2014-06-18T13:15:59.000000000001-22:11\"" },
    /*hasFractionalSeconds = 1, 1 digit fractional second, hasTimeZone=1, 1 digit positive time zone hour, 1 digit minute*/
    { 2014, 6, 18, 13, 15, 59, 1, 1, 1, 2, 1, "\"2014-06-18T13:15:59.000000000001+02:01\"" },
    /*hasFractionalSeconds = 1, 1 digit fractional second, hasTimeZone=1, 1 digit positive time zone hour, 2 digit minute*/
    { 2014, 6, 18, 13, 15, 59, 1, 1, 1, 2, 13, "\"2014-06-18T13:15:59.000000000001+02:13\"" },
    /*hasFractionalSeconds = 1, 1 digit fractional second, hasTimeZone=1, 2 digit positive time zone hour, 1 digit minute*/
    { 2014, 6, 18, 13, 15, 59, 1, 1, 1, 23, 8, "\"2014-06-18T13:15:59.000000000001+23:08\"" },
    /*hasFractionalSeconds = 1, 1 digit fractional second, hasTimeZone=1, 2 digit positive time zone hour, 2 digit minute*/

    /*hasFractionalSeconds = 1, 2 digit fractional second, hasTimeZone=1, 2 digit negative time zone hour, 1 digit minute*/
    { 2014, 6, 18, 13, 15, 59, 1, 22, 1, -22, 1, "\"2014-06-18T13:15:59.000000000022-22:01\"" },
    /*hasFractionalSeconds = 1, 2 digit fractional second, hasTimeZone=1, 2 digit negative time zone hour, 2 digit minute*/
    { 2014, 6, 18, 13, 15, 59, 1, 22, 1, -22, 11, "\"2014-06-18T13:15:59.000000000022-22:11\"" },
    /*hasFractionalSeconds = 1, 2 digit fractional second, hasTimeZone=1, 1 digit positive time zone hour, 1 digit minute*/
    { 2014, 6, 18, 13, 15, 59, 1, 22, 1, 2, 1, "\"2014-06-18T13:15:59.000000000022+02:01\"" },
    /*hasFractionalSeconds = 1, 2 digit fractional second, hasTimeZone=1, 1 digit positive time zone hour, 2 digit minute*/
    { 2014, 6, 18, 13, 15, 59, 1, 22, 1, 2, 13, "\"2014-06-18T13:15:59.000000000022+02:13\"" },
    /*hasFractionalSeconds = 1, 2 digit fractional second, hasTimeZone=1, 2 digit positive time zone hour, 1 digit minute*/
    { 2014, 6, 18, 13, 15, 59, 1, 22, 1, 23, 8, "\"2014-06-18T13:15:59.000000000022+23:08\"" },
    /*hasFractionalSeconds = 1, 2 digit fractional second, hasTimeZone=1, 2 digit positive time zone hour, 2 digit minute*/

    /*hasFractionalSeconds = 1, 12 digit fractional second, hasTimeZone=1, 2 digit negative time zone hour, 1 digit minute*/
    { 2014, 6, 18, 13, 15, 59, 1, 999999999999, 1, -22, 1, "\"2014-06-18T13:15:59.999999999999-22:01\"" },
    /*hasFractionalSeconds = 1, 12 digit fractional second, hasTimeZone=1, 2 digit negative time zone hour, 2 digit minute*/
    { 2014, 6, 18, 13, 15, 59, 1, 999999999999, 1, -22, 11, "\"2014-06-18T13:15:59.999999999999-22:11\"" },
    /*hasFractionalSeconds = 1, 12 digit fractional second, hasTimeZone=1, 1 digit positive time zone hour, 1 digit minute*/
    { 2014, 6, 18, 13, 15, 59, 1, 999999999999, 1, 2, 1, "\"2014-06-18T13:15:59.999999999999+02:01\"" },
    /*hasFractionalSeconds = 1, 12 digit fractional second, hasTimeZone=1, 1 digit positive time zone hour, 2 digit minute*/
    { 2014, 6, 18, 13, 15, 59, 1, 999999999999, 1, 2, 13, "\"2014-06-18T13:15:59.999999999999+02:13\"" },
    /*hasFractionalSeconds = 1, 12 digit fractional second, hasTimeZone=1, 2 digit positive time zone hour, 1 digit minute*/
    { 2014, 6, 18, 13, 15, 59, 1, 999999999999, 1, 23, 8, "\"2014-06-18T13:15:59.999999999999+23:08\"" },
    /*hasFractionalSeconds = 1, 12 digit fractional second, hasTimeZone=1, 2 digit positive time zone hour, 2 digit minute*/

};

static CMocksForAgentTypeSytem * mocks = NULL;

static MICROMOCK_GLOBAL_SEMAPHORE_HANDLE g_dllByDll;

BEGIN_TEST_SUITE(AgentTypeSystem_ut)

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
            mocks = new CMocksForAgentTypeSytem;
            global_bufferTemp = BASEIMPLEMENTATION::STRING_new();
            bufferTemp2 = BASEIMPLEMENTATION::STRING_new();
            mocks->ResetAllCalls();

            currentSTRING_new_call = 0;
            whenShallSTRING_new_fail = 0;

            currentSTRING_concat_called_from_another_mock = 0;
            currentSTRING_concat_call = 0;
            whenShallSTRING_concat_fail = 0;

            nSTRING_new_calls = 0;
            nSTRING_delete_calls = 0;

            currentSTRING_clone_call = 0;
            whenShallSTRING_clone_fail = 0;

            currentSTRING_construct_call = 0;
            whenShallSTRING_construct_fail = 0;

            currentSTRING_concat_with_STRING_call = 0;
            whenShallSTRING_concat_with_STRING_fail = 0;

            currentSTRING_construct_n_call = 0;
            whenShallSTRING_construct_n_fail = 0;

            (void)Create_AGENT_DATA_TYPE_from_FLOAT(&agSingle1, TEST_FLOAT_1);
            (void)Create_AGENT_DATA_TYPE_from_FLOAT(&agSingle2, TEST_FLOAT_2);
            (void)Create_AGENT_DATA_TYPE_from_DOUBLE(&agDouble1, TEST_DOUBLE_1);
            (void)Create_AGENT_DATA_TYPE_from_DOUBLE(&agDouble2, TEST_DOUBLE_2);
            some_tm.tm_hour = TEST_HOUR_1;
            some_tm.tm_isdst = 0;
            some_tm.tm_mday = TEST_MDAY_1;
            some_tm.tm_min = TEST_MIN_1;
            some_tm.tm_mon = TEST_MONTH_1;
            some_tm.tm_sec = TEST_SEC_1;
            some_tm.tm_year = TEST_YEAR_1 - 1900;

            some_tm2.tm_hour = TEST_HOUR_2;
            some_tm2.tm_isdst = 0;
            some_tm2.tm_mday = TEST_MDAY_2;
            some_tm2.tm_min = TEST_MIN_2;
            some_tm2.tm_mon = TEST_MONTH_2;
            some_tm2.tm_sec = TEST_SEC_2;
            some_tm2.tm_year = TEST_YEAR_2 - 1900;

            static_assert(numeric_limits<double>::has_quiet_NaN && numeric_limits<float>::has_quiet_NaN, "unsupported platform for testing, where has_quiet_nan doesn't exist");
            static_assert(numeric_limits<double>::has_signaling_NaN && numeric_limits<float>::has_signaling_NaN, "unsupported platform for testing, where has_signaling_NaN doesn't exist");
            static_assert(numeric_limits<double>::has_infinity && numeric_limits<float>::has_infinity, "unsupported platform for testing, where has_infinity doesn't exist");
            static_assert(numeric_limits<double>::is_iec559 && numeric_limits<float>::is_iec559, "I cannot write (-1)*inf unless it is IEC559");

            /*C++ to the rescue!!!*/
            (void)Create_AGENT_DATA_TYPE_from_FLOAT(&agSingleQuietNaN, numeric_limits<float>::quiet_NaN());
            (void)Create_AGENT_DATA_TYPE_from_DOUBLE(&agDoubleQuietNaN, numeric_limits<double>::quiet_NaN());

            (void)Create_AGENT_DATA_TYPE_from_FLOAT(&agSingleSingalingNaN, numeric_limits<float>::signaling_NaN());
            (void)Create_AGENT_DATA_TYPE_from_DOUBLE(&agDoubleSignalingNaN, numeric_limits<double>::signaling_NaN());

            (void)Create_AGENT_DATA_TYPE_from_FLOAT(&agSinglePlusInf, numeric_limits<float>::infinity());
            (void)Create_AGENT_DATA_TYPE_from_DOUBLE(&agDoublePlusInf, numeric_limits<double>::infinity());

            (void)Create_AGENT_DATA_TYPE_from_FLOAT(&agSingleMinusInf, -numeric_limits<float>::infinity());
            (void)Create_AGENT_DATA_TYPE_from_DOUBLE(&agDoubleMinusInf, -numeric_limits<double>::infinity());

            (void)Create_AGENT_DATA_TYPE_from_SINT16(&TEST_TYPENAME_GEOLOCATION_MEMBERVALUES[0], TEST_TYPENAME_GEOLOCATION_VANCOUVER_LAT);
            (void)Create_AGENT_DATA_TYPE_from_SINT16(&TEST_TYPENAME_GEOLOCATION_MEMBERVALUES[1], TEST_TYPENAME_GEOLOCATION_VANCOUVER_LONG);

            (void)Create_AGENT_DATA_TYPE_from_Members(
                &agComplexType1,
                TEST_TYPENAME_GEOLOCATION,
                TEST_TYPENAME_GEOLOCATION_NMEMBERS,
                TEST_TYPENAME_GEOLOCATION_MEMBERNAMES,
                TEST_TYPENAME_GEOLOCATION_MEMBERVALUES);

            /*create truck1_truckStoppedStruct_locationAndFuel_geoLocation*/
            (void)Create_AGENT_DATA_TYPE_from_SINT32(&truck1_truckStoppedStruct_locationAndFuel_geoLocation_fields[0], TEST_TRUCK1_truckStoppedStruct_locationAndFuel_geoLocation_lat);
            (void)Create_AGENT_DATA_TYPE_from_SINT32(&truck1_truckStoppedStruct_locationAndFuel_geoLocation_fields[1], TEST_TRUCK1_truckStoppedStruct_locationAndFuel_geoLocation_long);

            /*create truck1_truckStoppedStruct_locationAndFuel*/
            (void)Create_AGENT_DATA_TYPE_from_SINT32(&truck1_truckStoppedStruct_locationAndFuel_fields[0], TEST_TRUCK1_truckStoppedStruct_locationAndFuel_FuelLevel);
            (void)Create_AGENT_DATA_TYPE_from_Members(&truck1_truckStoppedStruct_locationAndFuel_fields[1], "GeoLocationType", 2, truck1_truckStoppedStruct_locationAndFuel_geoLocation_field_names, truck1_truckStoppedStruct_locationAndFuel_geoLocation_fields);

            /*create truck1_truckStoppedStruct*/
            (void)Create_AGENT_DATA_TYPE_from_SINT32(&truck1_truckStoppedStruct_fields[0], TEST_TRUCK1_truckStoppedStruct_severity);
            (void)Create_AGENT_DATA_TYPE_from_Members(&truck1_truckStoppedStruct_fields[1], "locationAndFuelType", 2, truck1_truckStoppedStruct_locationAndFuel_field_names, truck1_truckStoppedStruct_locationAndFuel_fields);

            /*create the truck in Targu Jiu*/
            /*{ "key", "VIN", "FuelState", "AcState", "truckStoppedStruct" };*/
            (void)Create_AGENT_DATA_TYPE_from_charz(&truck1_fields[0], TEST_TRUCK1_key);
            (void)Create_AGENT_DATA_TYPE_from_charz(&truck1_fields[1], TEST_TRUCK1_VIN);
            (void)Create_AGENT_DATA_TYPE_from_SINT32(&truck1_fields[2], TEST_TRUCK1_FuelState);
            (void)Create_EDM_BOOLEAN_from_int(&truck1_fields[3], (TEST_TRUCK1_AcState != 0) ? 4 : 0);
            (void)Create_AGENT_DATA_TYPE_from_Members(&truck1_fields[4], "TruckStoppedStructType", 2, truck1_truckStoppedStruct_field_names, truck1_truckStoppedStruct_fields);

            (void)Create_AGENT_DATA_TYPE_from_Members(&truck1, "TruckType", 5, truck1_field_names, truck1_fields);
        }

        TEST_FUNCTION_CLEANUP(TestMethodCleanup)
        {

            size_t i;
            Destroy_AGENT_DATA_TYPE(&agSingle1);
            Destroy_AGENT_DATA_TYPE(&agSingle2);
            Destroy_AGENT_DATA_TYPE(&agDouble1);
            Destroy_AGENT_DATA_TYPE(&agDouble2);
            Destroy_AGENT_DATA_TYPE(&agSingleQuietNaN);
            Destroy_AGENT_DATA_TYPE(&agDoubleQuietNaN);

            Destroy_AGENT_DATA_TYPE(&agSingleSingalingNaN);
            Destroy_AGENT_DATA_TYPE(&agDoubleSignalingNaN);

            Destroy_AGENT_DATA_TYPE(&agSinglePlusInf);
            Destroy_AGENT_DATA_TYPE(&agDoublePlusInf);

            Destroy_AGENT_DATA_TYPE(&agSingleMinusInf);
            Destroy_AGENT_DATA_TYPE(&agDoubleMinusInf);

            for (i = 0; i < TEST_TYPENAME_GEOLOCATION_NMEMBERS; i++)
            {
                Destroy_AGENT_DATA_TYPE(&TEST_TYPENAME_GEOLOCATION_MEMBERVALUES[i]);
            }

            Destroy_AGENT_DATA_TYPE(&agComplexType1);

            Destroy_AGENT_DATA_TYPE(&truck1);

            Destroy_AGENT_DATA_TYPE(&truck1_fields[4]);
            Destroy_AGENT_DATA_TYPE(&truck1_fields[3]);
            Destroy_AGENT_DATA_TYPE(&truck1_fields[2]);
            Destroy_AGENT_DATA_TYPE(&truck1_fields[1]);
            Destroy_AGENT_DATA_TYPE(&truck1_fields[0]);

            Destroy_AGENT_DATA_TYPE(&truck1_truckStoppedStruct_fields[1]);
            Destroy_AGENT_DATA_TYPE(&truck1_truckStoppedStruct_fields[0]);

            Destroy_AGENT_DATA_TYPE(&truck1_truckStoppedStruct_locationAndFuel_fields[1]);
            Destroy_AGENT_DATA_TYPE(&truck1_truckStoppedStruct_locationAndFuel_fields[0]);


            Destroy_AGENT_DATA_TYPE(&truck1_truckStoppedStruct_locationAndFuel_geoLocation_fields[1]);
            Destroy_AGENT_DATA_TYPE(&truck1_truckStoppedStruct_locationAndFuel_geoLocation_fields[0]);

            BASEIMPLEMENTATION::STRING_delete(global_bufferTemp);
            BASEIMPLEMENTATION::STRING_delete(bufferTemp2);
            mocks->SetIgnoreUnexpectedCalls(true);
            delete mocks;
            if (!MicroMockReleaseMutex(g_testByTest))
            {
                ASSERT_FAIL("failure in test framework at ReleaseMutex");
            }
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_013:[ All the Create_... functions shall check their parameters for validity. When an invalid parameter is detected, a code of AGENT_DATA_TYPES_INVALID_ARG shall be returned ]*/
        TEST_FUNCTION(Create_EDM_BOOLEAN_from_int_with_NULL_agentData_fails)
        {
            ///arrange

            ///act
            auto result = Create_EDM_BOOLEAN_from_int(NULL, 1);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_013:[ All the Create_... functions shall check their parameters for validity. When an invalid parameter is detected, a code of AGENT_DATA_TYPES_INVALID_ARG shall be returned ]*/
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_UINT8_with_NULL_agentData_fails)
        {
            ///arrange

            ///act
            auto result = Create_AGENT_DATA_TYPE_from_UINT8(NULL, 1);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_013:[ All the Create_... functions shall check their parameters for validity. When an invalid parameter is detected, a code of AGENT_DATA_TYPES_INVALID_ARG shall be returned ]*/
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_EDM_DATE_TIME_OFFSET_with_NULL_agentData_fails)
        {
            ///arrange
            EDM_DATE_TIME_OFFSET something = { { 0 } };

            ///act
            auto result = Create_AGENT_DATA_TYPE_from_EDM_DATE_TIME_OFFSET(NULL, something);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_013:[ All the Create_... functions shall check their parameters for validity. When an invalid parameter is detected, a code of AGENT_DATA_TYPES_INVALID_ARG shall be returned ]*/
        TEST_FUNCTION(Create_EDM_DECIMAL_from_SINT64_UINT64_with_NULL_agentData_fails)
        {
            ///arrange

            ///act
            auto result = Create_EDM_DECIMAL_from_charz(NULL, "1.2");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_013:[ All the Create_... functions shall check their parameters for validity. When an invalid parameter is detected, a code of AGENT_DATA_TYPES_INVALID_ARG shall be returned ]*/
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_SINT16_with_NULL_agentData_fails)
        {
            ///arrange

            ///act
            auto result = Create_AGENT_DATA_TYPE_from_SINT16(NULL, 0);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_013:[ All the Create_... functions shall check their parameters for validity. When an invalid parameter is detected, a code of AGENT_DATA_TYPES_INVALID_ARG shall be returned ]*/
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_SINT32_with_NULL_agentData_fails)
        {
            ///arrange

            ///act
            auto result = Create_AGENT_DATA_TYPE_from_SINT32(NULL, 0);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_013:[ All the Create_... functions shall check their parameters for validity. When an invalid parameter is detected, a code of AGENT_DATA_TYPES_INVALID_ARG shall be returned ]*/
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_SINT64_with_NULL_agentData_fails)
        {
            ///arrange

            ///act
            auto result = Create_AGENT_DATA_TYPE_from_SINT64(NULL, 0);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }


        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_013:[ All the Create_... functions shall check their parameters for validity. When an invalid parameter is detected, a code of AGENT_DATA_TYPES_INVALID_ARG shall be returned ]*/
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_SINT8_with_NULL_agentData_fails)
        {
            ///arrange

            ///act
            auto result = Create_AGENT_DATA_TYPE_from_SINT8(NULL, 0);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_013:[ All the Create_... functions shall check their parameters for validity. When an invalid parameter is detected, a code of AGENT_DATA_TYPES_INVALID_ARG shall be returned ]*/
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_FLOAT_with_NULL_agentData_fails)
        {
            ///arrange

            ///act
            auto result = Create_AGENT_DATA_TYPE_from_FLOAT(NULL, 0);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_013:[ All the Create_... functions shall check their parameters for validity. When an invalid parameter is detected, a code of AGENT_DATA_TYPES_INVALID_ARG shall be returned ]*/
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_charz_with_NULL_agentData_fails)
        {
            ///arrange

            ///act
            auto result = Create_AGENT_DATA_TYPE_from_charz(NULL, "sometext");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_013:[ All the Create_... functions shall check their parameters for validity. When an invalid parameter is detected, a code of AGENT_DATA_TYPES_INVALID_ARG shall be returned ]*/
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_charz_with_NULL_value_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;
            ///act
            auto result = Create_AGENT_DATA_TYPE_from_charz(&ag, NULL);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_013:[ All the Create_... functions shall check their parameters for validity. When an invalid parameter is detected, a code of AGENT_DATA_TYPES_INVALID_ARG shall be returned ]*/
        TEST_FUNCTION(Destroy_AGENT_DATA_TYPE_with_already_destoyed_Data_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;
            (void)Create_AGENT_DATA_TYPE_from_FLOAT(&ag, 3.14f);
            Destroy_AGENT_DATA_TYPE(&ag);

            ///act
            Destroy_AGENT_DATA_TYPE(&ag);

            ///assert
            ///no crashes, no exception, no runtime errors... 
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_015:[If destination parameter is NULL, AgentDataTypes_ToString shall return AGENT_DATA_TYPES_INVALID_ARG.]*/
        TEST_FUNCTION(AgentDataTypes_ToString_with_NULL_destination_fails)
        {
            ///arrange

            ///act
            auto res = AgentDataTypes_ToString(NULL, &agSingle1);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }
        
        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_053:[If value is NULL or has been destroyed or otherwise doesn't contain valid data, AGENT_DATA_TYPES_INVALID_ARG shall be returned.]*/
        TEST_FUNCTION(AgentDataTypes_ToString_with_NULL_value_fails)
        {
            ///arrange
            STRING_HANDLE destination = STRING_new();
            ///act
            auto res = AgentDataTypes_ToString(destination, NULL);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);

            ///cleanup
            STRING_delete(destination);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_053:[If value is NULL or has been destroyed or otherwise doesn't contain valid data, AGENT_DATA_TYPES_INVALID_ARG shall be returned.]*/
        TEST_FUNCTION(AgentDataTypes_ToString_with_destroyed_value_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;
            Create_AGENT_DATA_TYPE_from_FLOAT(&ag, 3.14f);
            Destroy_AGENT_DATA_TYPE(&ag);
            STRING_HANDLE destination = STRING_new();

            ///act
            auto res = AgentDataTypes_ToString(destination, &ag);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);

            ///cleanup
            STRING_delete(destination);
        }
        
        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_031:[ Creates a AGENT_DATA_TYPE representing an EDM_BOOLEAN.]*/
        TEST_FUNCTION(Create_EDM_BOOLEAN_from_int_succeeds_1)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_BOOLEAN_from_int(&ag, 0);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&ag);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_031:[ Creates a AGENT_DATA_TYPE representing an EDM_BOOLEAN.]*/
        TEST_FUNCTION(Create_EDM_BOOLEAN_from_int_succeeds_2)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_BOOLEAN_from_int(&ag, 1);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&ag);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_029:[If v 0 then the AGENT_DATA_TYPE shall have the value "false" Boolean.]*/
        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_017:[ EDM_BOOLEAN: as in (odata-abnf-construction-rules, 2013), booleanValue = "true" / "false"]*/
        TEST_FUNCTION(AgentDataTypes_ToString_for_BOOLEAN_succeeds_with_false)
        {
            ///arrange
            AGENT_DATA_TYPE ag;
            (void)Create_EDM_BOOLEAN_from_int(&ag, 0);

            ///act
            auto res = AgentDataTypes_ToString(global_bufferTemp, &ag);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);
            ASSERT_ARE_EQUAL(char_ptr,"false", STRING_c_str(global_bufferTemp));

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&ag);
        }
        /*SRS_AGENT_TYPE_SYSTEM_99_101:[ EDM_NULL_TYPE shall return the unquoted string null.]*/
        TEST_FUNCTION(AgentDataTypes_ToString_for_null_succeeds)
        {
            ///arrange
            AGENT_DATA_TYPE ag;
            (void)Create_NULL_AGENT_DATA_TYPE(&ag);

            ///act
            auto res = AgentDataTypes_ToString(global_bufferTemp, &ag);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);
            ASSERT_ARE_EQUAL(char_ptr, "null", STRING_c_str(global_bufferTemp));

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&ag);
        }
        TEST_FUNCTION(Create_NULL_AGENT_DATA_TYPE_with_null_fails)
        {
            ///arrange

            ///act
            auto res = Create_NULL_AGENT_DATA_TYPE(NULL);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);

            ///cleanup
        }
        TEST_FUNCTION(Create_NULL_AGENT_DATA_TYPE_succeeds)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_NULL_AGENT_DATA_TYPE(&ag);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_NULL_TYPE, ag.type);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&ag);

        }
        TEST_FUNCTION(CreateAgentDataType_From_String_null_to_not_EDM_NULL_TYPE_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = CreateAgentDataType_From_String("null", EDM_INT64_TYPE, &ag);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);

        }

        TEST_FUNCTION(CreateAgentDataType_From_String_null_to_EDM_NULL_TYPE_succeeds)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = CreateAgentDataType_From_String("null", EDM_NULL_TYPE, &ag);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_NULL_TYPE, ag.type);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&ag);

        }
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_AGENT_DATA_TYPE_EDM_NULL_TYPE_succeeds)
        {
            ///arrange
            AGENT_DATA_TYPE ag;
            AGENT_DATA_TYPE bg;
            (void)Create_NULL_AGENT_DATA_TYPE(&ag);

            ///act
            auto res = Create_AGENT_DATA_TYPE_from_AGENT_DATA_TYPE(&bg, &ag);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, bg.type, ag.type);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&ag);

        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_030:[ If v is different than 0 then the AGENT_DATA_TYPE shall have the value "true".]*/
        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_017:[ EDM_BOOLEAN: as in (odata-abnf-construction-rules, 2013), booleanValue = "true" / "false"]*/
        TEST_FUNCTION(AgentDataTypes_ToString_for_BOOLEAN__succeeds_with_true)
        {
            ///arrange
            AGENT_DATA_TYPE ag;
            (void)Create_EDM_BOOLEAN_from_int(&ag, ~0);

            ///act
            auto res = AgentDataTypes_ToString(global_bufferTemp, &ag);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);
            ASSERT_ARE_EQUAL( char_ptr,"true", STRING_c_str(global_bufferTemp));

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&ag);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_016:[ When the value cannot be converted to a string AgentDataTypes_ToString shall return AGENT_DATA_TYPES_ERROR.]*/
        TEST_FUNCTION(AgentDataTypes_ToString_for_BOOLEAN_fails_when_strcpy_s_fails_1)
        {
            ///arrange
            AGENT_DATA_TYPE ag;
            (void)Create_EDM_BOOLEAN_from_int(&ag, ~0);

            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .SetReturn(1);

            ///act
            auto res = AgentDataTypes_ToString(global_bufferTemp, &ag);

            ///assert

            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_ERROR, res);

            ///cleanup
            
            Destroy_AGENT_DATA_TYPE(&ag);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_016:[ When the value cannot be converted to a string AgentDataTypes_ToString shall return AGENT_DATA_TYPES_ERROR.]*/
        TEST_FUNCTION(AgentDataTypes_ToString_for_BOOLEAN_fails_when_strcpy_s_fails_2)
        {
            ///arrange
            AGENT_DATA_TYPE ag;
            (void)Create_EDM_BOOLEAN_from_int(&ag, 0);

            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .SetReturn(1);

            ///act
            auto res = AgentDataTypes_ToString(global_bufferTemp, &ag);

            ///assert

            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_ERROR, res);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&ag);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_033:[ Creates an AGENT_DATA_TYPE from a uint8_t.]*/
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_UINT8_with_0_succeeds)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_AGENT_DATA_TYPE_from_UINT8(&ag, 0);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&ag);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_033:[ Creates an AGENT_DATA_TYPE from a uint8_t.]*/
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_UINT8_with_255_succeeds)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_AGENT_DATA_TYPE_from_UINT8(&ag, 255);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&ag);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_018:[ EDM_BYTE: as in (odata-abnf-construction-rules, 2013), byteValue  = 1*3DIGIT ; numbers in the range from 0 to 255]*/
        TEST_FUNCTION(AgentDataTypes_ToString_with_UINT8_succeeds_0)
        {
            ///arrange
            AGENT_DATA_TYPE ag;
            (void)Create_AGENT_DATA_TYPE_from_UINT8(&ag, 0);

            ///act
            auto res = AgentDataTypes_ToString(global_bufferTemp, &ag);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);
            ASSERT_ARE_EQUAL( char_ptr,"0", STRING_c_str(global_bufferTemp));

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&ag);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_018:[ EDM_BYTE: as in (odata-abnf-construction-rules, 2013), byteValue  = 1*3DIGIT ; numbers in the range from 0 to 255]*/
        TEST_FUNCTION(AgentDataTypes_ToString_with_UINT8_succeeds_255)
        {
            ///arrange
            AGENT_DATA_TYPE ag;
            (void)Create_AGENT_DATA_TYPE_from_UINT8(&ag, 255);

            ///act
            auto res = AgentDataTypes_ToString(global_bufferTemp, &ag);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);
            ASSERT_ARE_EQUAL(char_ptr, "255", STRING_c_str(global_bufferTemp));

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&ag);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_018:[ EDM_BYTE: as in (odata-abnf-construction-rules, 2013), byteValue  = 1*3DIGIT ; numbers in the range from 0 to 255]*/
        TEST_FUNCTION(AgentDataTypes_ToString_with_UINT8_unsufficient_buffer_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;
            (void)Create_AGENT_DATA_TYPE_from_UINT8(&ag, 255);

            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .SetReturn(1);

            ///act
            auto res = AgentDataTypes_ToString(global_bufferTemp, &ag);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_ERROR, res);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&ag);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_018:[ EDM_BYTE: as in (odata-abnf-construction-rules, 2013), byteValue  = 1*3DIGIT ; numbers in the range from 0 to 255]*/
        TEST_FUNCTION(AgentDataTypes_ToString_with_UINT8_barely_sufficient_buffer_succeeds)
        {
            ///arrange
            AGENT_DATA_TYPE ag;
            (void)Create_AGENT_DATA_TYPE_from_UINT8(&ag, 255);

            ///act
            auto res = AgentDataTypes_ToString(global_bufferTemp, &ag);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);
            ASSERT_ARE_EQUAL(char_ptr, "255", STRING_c_str(global_bufferTemp));

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&ag);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_034:[ Creates an AGENT_DATA_TYPE containing an EDM_DATE_TIME_OFFSET from a time_t.]*/
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_EDM_DATE_TIME_OFFSET_succeeds)
        {
            AGENT_DATA_TYPE ag;
            EDM_DATE_TIME_OFFSET someDateTimeOffset;
            someDateTimeOffset.dateTime.tm_year = 114; /*so 2014*/
            someDateTimeOffset.dateTime.tm_mon = 6 - 1; 
            someDateTimeOffset.dateTime.tm_mday = 18;
            someDateTimeOffset.dateTime.tm_hour = 9;
            someDateTimeOffset.dateTime.tm_min = 41;
            someDateTimeOffset.dateTime.tm_sec = 23;
            someDateTimeOffset.hasFractionalSecond = 0;
            someDateTimeOffset.fractionalSecond = 0;
            someDateTimeOffset.hasTimeZone = 0;
            someDateTimeOffset.timeZoneHour = 0;
            someDateTimeOffset.timeZoneMinute = 0;

            ///act
            auto res = Create_AGENT_DATA_TYPE_from_EDM_DATE_TIME_OFFSET(&ag, someDateTimeOffset);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&ag);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_019:[EDM_DATE_TIME_OFFSET:dateTimeOffsetValue = year "-" month "-" day "T" hour ":" minute[":" second["." fractionalSeconds]]("Z" / sign hour ":" minute)]*/
        TEST_FUNCTION(AgentDataTypes_ToString_for_EDM_DATE_TIME_OFFSET_with_insufficient_buffer_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;
            ///arrange

            EDM_DATE_TIME_OFFSET someDateTimeOffset;
            someDateTimeOffset.dateTime.tm_year = 114; /*so 2014*/
            someDateTimeOffset.dateTime.tm_mon = 6 -1 ;
            someDateTimeOffset.dateTime.tm_mday = 18;
            someDateTimeOffset.dateTime.tm_hour = 9;
            someDateTimeOffset.dateTime.tm_min = 41;
            someDateTimeOffset.dateTime.tm_sec = 23;
            someDateTimeOffset.hasFractionalSecond = 0;
            someDateTimeOffset.fractionalSecond = 0;
            someDateTimeOffset.hasTimeZone = 0;
            someDateTimeOffset.timeZoneHour = 0;
            someDateTimeOffset.timeZoneMinute = 0;
            (void)Create_AGENT_DATA_TYPE_from_EDM_DATE_TIME_OFFSET(&ag, someDateTimeOffset);

            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .SetReturn(1);
            ///act
            auto res = AgentDataTypes_ToString(global_bufferTemp, &ag);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_ERROR, res);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&ag);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_019:[EDM_DATE_TIME_OFFSET:dateTimeOffsetValue = year "-" month "-" day "T" hour ":" minute[":" second["." fractionalSeconds]]("Z" / sign hour ":" minute)]*/
        TEST_FUNCTION(AgentDataTypes_ToString_for_EDM_DATE_TIME_OFFSET_fails_when_sprintf_s_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;
            EDM_DATE_TIME_OFFSET someDateTimeOffset;
            someDateTimeOffset.dateTime.tm_year = 114; /*so 2014*/
            someDateTimeOffset.dateTime.tm_mon = 6 - 1;
            someDateTimeOffset.dateTime.tm_mday = 18;
            someDateTimeOffset.dateTime.tm_hour = 9;
            someDateTimeOffset.dateTime.tm_min = 41;
            someDateTimeOffset.dateTime.tm_sec = 23;
            someDateTimeOffset.hasFractionalSecond = 0;
            someDateTimeOffset.fractionalSecond = 0;
            someDateTimeOffset.hasTimeZone = 0;
            someDateTimeOffset.timeZoneHour = 0;
            someDateTimeOffset.timeZoneMinute = 0;
            (void)Create_AGENT_DATA_TYPE_from_EDM_DATE_TIME_OFFSET(&ag, someDateTimeOffset);
            (*mocks).ResetAllCalls();

            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .SetReturn(1);

            ///act
            auto res = AgentDataTypes_ToString(global_bufferTemp, &ag);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_ERROR, res);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&ag);
        }

#ifndef NO_FLOATS
        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_041:[ Creates an AGENT_DATA_TYPE containing an EDM_DOUBLE from double]*/
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_DOUBLE_succeeds_1)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_AGENT_DATA_TYPE_from_DOUBLE(&ag, TEST_DOUBLE_1);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&ag);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_041:[ Creates an AGENT_DATA_TYPE containing an EDM_DOUBLE from double]*/
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_DOUBLE_succeeds_2)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_AGENT_DATA_TYPE_from_DOUBLE(&ag, TEST_DOUBLE_2);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&ag);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_042:[Values of NaN, -INF, +INF are accepted]*/
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_NaN_succeeds_1)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_AGENT_DATA_TYPE_from_DOUBLE(&ag, numeric_limits<double>::quiet_NaN());

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&ag);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_042:[Values of NaN, -INF, +INF are accepted]*/
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_NaN_succeeds_2)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_AGENT_DATA_TYPE_from_DOUBLE(&ag, numeric_limits<double>::signaling_NaN());

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&ag);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_042:[Values of NaN, -INF, +INF are accepted]*/
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_plusInf_succeeds)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_AGENT_DATA_TYPE_from_DOUBLE(&ag, numeric_limits<double>::infinity());

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&ag);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_042:[Values of NaN, -INF, +INF are accepted]*/
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_minusInf_succeeds)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_AGENT_DATA_TYPE_from_DOUBLE(&ag, -numeric_limits<double>::infinity());

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&ag);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_022:[ EDM_DOUBLE: doubleValue = decimalValue [ "e" [SIGN] 1*DIGIT ] / nanInfinity ; IEEE 754 binary64 floating-point number (15-17 decimal digits). The representation shall use DBL_DIG C #define*/
        TEST_FUNCTION(AgentDataTypes_ToString_DOUBLE_with_SignallingNan_succeeds)
        {
            ///arrange

            ///act 
            auto res = AgentDataTypes_ToString(global_bufferTemp, &agDoubleSignalingNaN);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);
            ASSERT_ARE_EQUAL(char_ptr, "NaN", STRING_c_str(global_bufferTemp));
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_022:[ EDM_DOUBLE: doubleValue = decimalValue [ "e" [SIGN] 1*DIGIT ] / nanInfinity ; IEEE 754 binary64 floating-point number (15-17 decimal digits). The representation shall use DBL_DIG C #define*/
        TEST_FUNCTION(AgentDataTypes_ToString_DOUBLE_with_SignallingNan_insuficient_buffer_fails)
        {
            ///arrange
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .SetReturn(1);
            ///act 
            auto res = AgentDataTypes_ToString(global_bufferTemp, &agDoubleSignalingNaN);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_ERROR, res);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_022:[ EDM_DOUBLE: doubleValue = decimalValue [ "e" [SIGN] 1*DIGIT ] / nanInfinity ; IEEE 754 binary64 floating-point number (15-17 decimal digits). The representation shall use DBL_DIG C #define*/
        TEST_FUNCTION(AgentDataTypes_ToString_DOUBLE_with_QuietNan_succeeds)
        {
            ///arrange

            ///act 
            auto res = AgentDataTypes_ToString(global_bufferTemp, &agDoubleQuietNaN);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);
            ASSERT_ARE_EQUAL(char_ptr, "NaN", STRING_c_str(global_bufferTemp));
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_022:[ EDM_DOUBLE: doubleValue = decimalValue [ "e" [SIGN] 1*DIGIT ] / nanInfinity ; IEEE 754 binary64 floating-point number (15-17 decimal digits). The representation shall use DBL_DIG C #define*/
        TEST_FUNCTION(AgentDataTypes_ToString_DOUBLE_with_QuietNan_insuficient_buffer_fails)
        {
            ///arrange
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .SetReturn(1);
            ///act 
            auto res = AgentDataTypes_ToString(global_bufferTemp, &agDoubleQuietNaN);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_ERROR, res);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_022:[ EDM_DOUBLE: doubleValue = decimalValue [ "e" [SIGN] 1*DIGIT ] / nanInfinity ; IEEE 754 binary64 floating-point number (15-17 decimal digits). The representation shall use DBL_DIG C #define*/
        TEST_FUNCTION(AgentDataTypes_ToString_DOUBLE_with_minusInf_succeeds)
        {
            ///arrange

            ///act 
            auto res = AgentDataTypes_ToString(global_bufferTemp, &agDoubleMinusInf);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);
            ASSERT_ARE_EQUAL(char_ptr, "-INF", STRING_c_str(global_bufferTemp));
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_022:[ EDM_DOUBLE: doubleValue = decimalValue [ "e" [SIGN] 1*DIGIT ] / nanInfinity ; IEEE 754 binary64 floating-point number (15-17 decimal digits). The representation shall use DBL_DIG C #define*/
        TEST_FUNCTION(AgentDataTypes_ToString_DOUBLE_with_minusInf_insuficient_buffer_fails)
        {
            ///arrange
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .SetReturn(1);

            ///act 
            auto res = AgentDataTypes_ToString(global_bufferTemp, &agDoubleMinusInf);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_ERROR, res);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_022:[ EDM_DOUBLE: doubleValue = decimalValue [ "e" [SIGN] 1*DIGIT ] / nanInfinity ; IEEE 754 binary64 floating-point number (15-17 decimal digits). The representation shall use DBL_DIG C #define*/
        TEST_FUNCTION(AgentDataTypes_ToString_DOUBLE_with_plusInf_succeeds)
        {
            ///arrange

            ///act 
            auto res = AgentDataTypes_ToString(global_bufferTemp, &agDoublePlusInf);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);
            ASSERT_ARE_EQUAL(char_ptr, "INF", STRING_c_str(global_bufferTemp));
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_022:[ EDM_DOUBLE: doubleValue = decimalValue [ "e" [SIGN] 1*DIGIT ] / nanInfinity ; IEEE 754 binary64 floating-point number (15-17 decimal digits). The representation shall use DBL_DIG C #define*/
        TEST_FUNCTION(AgentDataTypes_ToString_DOUBLE_with_plusInf_insuficient_buffer_fails)
        {
            ///arrange
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .SetReturn(1);
            ///act 
            auto res = AgentDataTypes_ToString(global_bufferTemp, &agDoublePlusInf);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_ERROR, res);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_022:[ EDM_DOUBLE: doubleValue = decimalValue [ "e" [SIGN] 1*DIGIT ] / nanInfinity ; IEEE 754 binary64 floating-point number (15-17 decimal digits). The representation shall use DBL_DIG C #define*/
        TEST_FUNCTION(AgentDataTypes_ToString_DOUBLE_succeeds_1)
        {
            ///arrange

            ///act 
            auto res = AgentDataTypes_ToString(global_bufferTemp, &agDouble1);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);
            ASSERT_ARE_EQUAL(double, TEST_DOUBLE_1, atof(STRING_c_str(global_bufferTemp)));
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_022:[ EDM_DOUBLE: doubleValue = decimalValue [ "e" [SIGN] 1*DIGIT ] / nanInfinity ; IEEE 754 binary64 floating-point number (15-17 decimal digits). The representation shall use DBL_DIG C #define*/
        TEST_FUNCTION(AgentDataTypes_ToString_DOUBLE_succeeds_2)
        {
            ///arrange

            ///act 
            auto res = AgentDataTypes_ToString(global_bufferTemp, &agDouble2);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);
            ASSERT_ARE_EQUAL(double, TEST_DOUBLE_2, atof(STRING_c_str(global_bufferTemp)));
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_047:[ Creates an AGENT_DATA_TYPE containing an EDM_SINGLE from float]*/
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_FLOAT_succeeds_1)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_AGENT_DATA_TYPE_from_FLOAT(&ag, TEST_FLOAT_1);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&ag);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_047:[ Creates an AGENT_DATA_TYPE containing an EDM_SINGLE from float]*/
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_FLOAT_succeeds_2)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_AGENT_DATA_TYPE_from_FLOAT(&ag, TEST_FLOAT_2);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&ag);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_048:[ Value of NaN, +Inf, -Inf are accepted.]*/
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_FLOAT_NaN_succeeds_1)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_AGENT_DATA_TYPE_from_FLOAT(&ag, numeric_limits<float>::quiet_NaN());

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&ag);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_048:[ Value of NaN, +Inf, -Inf are accepted.]*/
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_FLOAT_NaN_succeeds_2)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_AGENT_DATA_TYPE_from_FLOAT(&ag, numeric_limits<float>::signaling_NaN());

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&ag);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_048:[ Value of NaN, +Inf, -Inf are accepted.]*/
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_FLOAT_plusInf_succeeds)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_AGENT_DATA_TYPE_from_FLOAT(&ag, numeric_limits<float>::infinity());

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&ag);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_048:[ Value of NaN, +Inf, -Inf are accepted.]*/
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_FLOAT_minusInf_succeeds)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_AGENT_DATA_TYPE_from_FLOAT(&ag, -numeric_limits<float>::infinity());

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&ag);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_027:[ EDM_SINGLE: singleValue = doubleValue ; IEEE 754 binary32 floating-point number (6-9 decimal digits). The representatiuon shall use FLT_DIG.]*/
        TEST_FUNCTION(AgentDataTypes_ToString_FLOAT_with_SignallingNan_succeeds)
        {
            ///arrange

            ///act 
            auto res = AgentDataTypes_ToString(global_bufferTemp, &agSingleSingalingNaN);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);
            ASSERT_ARE_EQUAL(char_ptr, "NaN", STRING_c_str(global_bufferTemp));
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_027:[ EDM_SINGLE: singleValue = doubleValue ; IEEE 754 binary32 floating-point number (6-9 decimal digits). The representatiuon shall use FLT_DIG.]*/
        TEST_FUNCTION(AgentDataTypes_ToString_FLOAT_with_SignallingNan_insuficient_buffer_fails)
        {
            ///arrange
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .SetReturn(1);

            ///act 
            auto res = AgentDataTypes_ToString(global_bufferTemp, &agSingleSingalingNaN);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_ERROR, res);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_027:[ EDM_SINGLE: singleValue = doubleValue ; IEEE 754 binary32 floating-point number (6-9 decimal digits). The representatiuon shall use FLT_DIG.]*/
        TEST_FUNCTION(AgentDataTypes_ToString_FLOAT_with_QuietNan_succeeds)
        {
            ///arrange

            ///act 
            auto res = AgentDataTypes_ToString(global_bufferTemp, &agSingleQuietNaN);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);
            ASSERT_ARE_EQUAL(char_ptr, "NaN", STRING_c_str(global_bufferTemp));
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_027:[ EDM_SINGLE: singleValue = doubleValue ; IEEE 754 binary32 floating-point number (6-9 decimal digits). The representatiuon shall use FLT_DIG.]*/
        TEST_FUNCTION(AgentDataTypes_ToString_FLOAT_with_QuietNan_insuficient_buffer_fails)
        {
            ///arrange
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .SetReturn(1);

            ///act 
            auto res = AgentDataTypes_ToString(global_bufferTemp, &agSingleQuietNaN);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_ERROR, res);

        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_027:[ EDM_SINGLE: singleValue = doubleValue ; IEEE 754 binary32 floating-point number (6-9 decimal digits). The representatiuon shall use FLT_DIG.]*/
        TEST_FUNCTION(AgentDataTypes_ToString_FLOAT_with_minusInf_succeeds)
        {
            ///arrange

            ///act 
            auto res = AgentDataTypes_ToString(global_bufferTemp, &agSingleMinusInf);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);
            ASSERT_ARE_EQUAL(char_ptr, "-INF", STRING_c_str(global_bufferTemp));
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_027:[ EDM_SINGLE: singleValue = doubleValue ; IEEE 754 binary32 floating-point number (6-9 decimal digits). The representatiuon shall use FLT_DIG.]*/
        TEST_FUNCTION(AgentDataTypes_ToString_FLOAT_with_minusInf_insuficient_buffer_fails)
        {
            ///arrange
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .SetReturn(1);

            ///act 
            auto res = AgentDataTypes_ToString(global_bufferTemp, &agSingleMinusInf);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_ERROR, res);


        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_027:[ EDM_SINGLE: singleValue = doubleValue ; IEEE 754 binary32 floating-point number (6-9 decimal digits). The representatiuon shall use FLT_DIG.]*/
        TEST_FUNCTION(AgentDataTypes_ToString_FLOAT_with_plusInf_succeeds)
        {
            ///arrange

            ///act 
            auto res = AgentDataTypes_ToString(global_bufferTemp, &agSinglePlusInf);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);
            ASSERT_ARE_EQUAL(char_ptr, "INF", STRING_c_str(global_bufferTemp));
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_027:[ EDM_SINGLE: singleValue = doubleValue ; IEEE 754 binary32 floating-point number (6-9 decimal digits). The representatiuon shall use FLT_DIG.]*/
        TEST_FUNCTION(AgentDataTypes_ToString_FLOAT_with_plusInf_insuficient_buffer_fails)
        {
            ///arrange
            EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .SetReturn(1);

            ///act 
            auto res = AgentDataTypes_ToString(global_bufferTemp, &agSinglePlusInf);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_ERROR, res);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_027:[ EDM_SINGLE: singleValue = doubleValue ; IEEE 754 binary32 floating-point number (6-9 decimal digits). The representatiuon shall use FLT_DIG.]*/
        TEST_FUNCTION(AgentDataTypes_ToString_FLOAT_succeeds_1)
        {
            ///arrange

            ///act 
            auto res = AgentDataTypes_ToString(global_bufferTemp, &agSingle1);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);
            ASSERT_ARE_EQUAL(double, (double)TEST_FLOAT_1, atof(STRING_c_str(global_bufferTemp)));

        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_027:[ EDM_SINGLE: singleValue = doubleValue ; IEEE 754 binary32 floating-point number (6-9 decimal digits). The representatiuon shall use FLT_DIG.]*/
        TEST_FUNCTION(AgentDataTypes_ToString_FLOAT_succeeds_2)
        {
            ///arrange

            ///act 
            auto res = AgentDataTypes_ToString(global_bufferTemp, &agSingle2);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);
            ASSERT_ARE_EQUAL(float, TEST_FLOAT_2, (float)atof(STRING_c_str(global_bufferTemp)));

        }
#endif

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_043:[ Creates an AGENT_DATA_TYPE containing an EDM_INT16 from int16_t]*/
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_SINT16_succeeds)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_AGENT_DATA_TYPE_from_SINT16(&ag, -1);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);

            //cleanup
            Destroy_AGENT_DATA_TYPE(&ag);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_043:[Creates an AGENT_DATA_TYPE containing an EDM_INT16 from int16_t] */
        TEST_FUNCTION(AgentDataTypes_ToString_INT16_succeeds_1)
        {
            ///arrange
            AGENT_DATA_TYPE ag;
            (void)Create_AGENT_DATA_TYPE_from_SINT16(&ag, -1);

            ///act
            auto res = AgentDataTypes_ToString(global_bufferTemp, &ag);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);
            ASSERT_ARE_EQUAL(char_ptr, "-1", STRING_c_str(global_bufferTemp));

            //cleanup
            Destroy_AGENT_DATA_TYPE(&ag);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_023:[ EDM_INT16: int16Value = [ sign ] 1*5DIGIT  ; numbers in the range from -32768 to 32767]*/
        TEST_FUNCTION(AgentDataTypes_ToString_INT16_succeeds_2)
        {
            ///arrange
            AGENT_DATA_TYPE ag;
            (void)Create_AGENT_DATA_TYPE_from_SINT16(&ag, -32768);

            ///act
            auto res = AgentDataTypes_ToString(global_bufferTemp, &ag);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);
            ASSERT_ARE_EQUAL(char_ptr, "-32768", STRING_c_str(global_bufferTemp));

            //cleanup
            Destroy_AGENT_DATA_TYPE(&ag);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_023:[ EDM_INT16: int16Value = [ sign ] 1*5DIGIT  ; numbers in the range from -32768 to 32767]*/
        TEST_FUNCTION(AgentDataTypes_ToString_INT16_succeeds_3)
        {
            ///arrange
            AGENT_DATA_TYPE ag;
            (void)Create_AGENT_DATA_TYPE_from_SINT16(&ag, 32767);

            ///act
            auto res = AgentDataTypes_ToString(global_bufferTemp, &ag);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);
            ASSERT_ARE_EQUAL(char_ptr, "32767", STRING_c_str(global_bufferTemp));

            //cleanup
            Destroy_AGENT_DATA_TYPE(&ag);
        }

        /* Create_AGENT_DATA_TYPE_from_charz */

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_049:[ Creates an AGENT_DATA_TYPE containing an EDM_STRING from an ASCII zero terminated string.]*/
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_charz_fails_with_NULL_string)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_AGENT_DATA_TYPE_from_charz(&ag, NULL);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_049:[ Creates an AGENT_DATA_TYPE containing an EDM_STRING from an ASCII zero terminated string.]*/
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_charz_suceeds)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_AGENT_DATA_TYPE_from_charz(&ag, TEST_STRING_1);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&ag);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_049:[ Creates an AGENT_DATA_TYPE containing an EDM_STRING from an ASCII zero terminated string.]*/
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_charz_suceeds_1)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_AGENT_DATA_TYPE_from_charz(&ag, TEST_STRING_1);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&ag);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_049:[ Creates an AGENT_DATA_TYPE containing an EDM_STRING from an ASCII zero terminated string.]*/
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_charz_suceeds_2)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_AGENT_DATA_TYPE_from_charz(&ag, TEST_STRING_2);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&ag);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_049:[ Creates an AGENT_DATA_TYPE containing an EDM_STRING from an ASCII zero terminated string.]*/
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_charz_suceeds_3)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_AGENT_DATA_TYPE_from_charz(&ag, TEST_STRING_3);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&ag);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_049:[ Creates an AGENT_DATA_TYPE containing an EDM_STRING from an ASCII zero terminated string.]*/
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_charz_With_One_Slash_Succeeds)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_AGENT_DATA_TYPE_from_charz(&ag, "/");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&ag);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_049:[ Creates an AGENT_DATA_TYPE containing an EDM_STRING from an ASCII zero terminated string.]*/
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_charz_With_2_Slashes_Succeeds)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_AGENT_DATA_TYPE_from_charz(&ag, "//");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&ag);
        }

        /* Create_AGENT_DATA_TYPE_from_charz_no_quotes */

        /* Tests_SRS_AGENT_TYPE_SYSTEM_01_001: [Creates an AGENT_DATA_TYPE containing an EDM_STRING from an ASCII zero terminated string.] */
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_charz_no_quotes_with_NULL_agent_data_type_fails)
        {
            ///arrange

            ///act
            auto res = Create_AGENT_DATA_TYPE_from_charz_no_quotes(NULL, "a");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_01_001: [Creates an AGENT_DATA_TYPE containing an EDM_STRING from an ASCII zero terminated string.] */
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_charz_no_quotes_with_NULL_value_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_AGENT_DATA_TYPE_from_charz_no_quotes(&ag, NULL);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_01_001: [Creates an AGENT_DATA_TYPE containing an EDM_STRING from an ASCII zero terminated string.] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_01_003: [EDM_STRING_no_quotes: the string is copied as given when the AGENT_DATA_TYPE was created.] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_01_002: [When serialized, this type is not enclosed with quotes.] */
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_charz_no_quotes_with_an_empty_string_succeeds)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_AGENT_DATA_TYPE_from_charz_no_quotes(&ag, "");
            (void)AgentDataTypes_ToString(global_bufferTemp, &ag);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);
            ASSERT_ARE_EQUAL(char_ptr, "", STRING_c_str(global_bufferTemp));

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&ag);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_01_001: [Creates an AGENT_DATA_TYPE containing an EDM_STRING from an ASCII zero terminated string.] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_01_003: [EDM_STRING_no_quotes: the string is copied as given when the AGENT_DATA_TYPE was created.] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_01_002: [When serialized, this type is not enclosed with quotes.] */
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_charz_no_quotes_with_one_char_string_succeeds)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_AGENT_DATA_TYPE_from_charz_no_quotes(&ag, "a");
            (void)AgentDataTypes_ToString(global_bufferTemp, &ag);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);
            ASSERT_ARE_EQUAL(char_ptr, "a", STRING_c_str(global_bufferTemp));

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&ag);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_01_001: [Creates an AGENT_DATA_TYPE containing an EDM_STRING from an ASCII zero terminated string.] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_01_003: [EDM_STRING_no_quotes: the string is copied as given when the AGENT_DATA_TYPE was created.] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_01_002: [When serialized, this type is not enclosed with quotes.] */
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_charz_no_quotes_with_a_longer_string_with_quotes_in_it_et_all_succeeds)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_AGENT_DATA_TYPE_from_charz_no_quotes(&ag, "[{\"someLongerString\"}]");
            (void)AgentDataTypes_ToString(global_bufferTemp, &ag);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);
            ASSERT_ARE_EQUAL(char_ptr, "[{\"someLongerString\"}]", STRING_c_str(global_bufferTemp));

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&ag);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_028:[ EDM_STRING: string           = SQUOTE *( SQUOTE-in-string / pchar-no-SQUOTE ) SQUOTE]*/
        TEST_FUNCTION(AgentDataTypes_ToString_with_charz_succeeds_1)
        {
            ///arrange
            AGENT_DATA_TYPE ag;
            Create_AGENT_DATA_TYPE_from_charz(&ag, TEST_STRING_1);
            
            ///act
            auto res = AgentDataTypes_ToString(global_bufferTemp, &ag);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);
            ASSERT_ARE_EQUAL(char_ptr, TEST_STRING_1_JSON, STRING_c_str(global_bufferTemp));

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&ag);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_028:[ EDM_STRING: string           = SQUOTE *( SQUOTE-in-string / pchar-no-SQUOTE ) SQUOTE]*/
        TEST_FUNCTION(AgentDataTypes_ToString_with_charz_succeeds_2)
        {
            ///arrange
            AGENT_DATA_TYPE ag;
            Create_AGENT_DATA_TYPE_from_charz(&ag, TEST_STRING_2);

            ///act
            auto res = AgentDataTypes_ToString(global_bufferTemp, &ag);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);
            ASSERT_ARE_EQUAL(char_ptr, TEST_STRING_2_JSON, STRING_c_str(global_bufferTemp));

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&ag);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_028:[ EDM_STRING: string           = SQUOTE *( SQUOTE-in-string / pchar-no-SQUOTE ) SQUOTE]*/
        TEST_FUNCTION(AgentDataTypes_ToString_with_charz_succeeds_3)
        {
            ///arrange
            AGENT_DATA_TYPE ag;
            Create_AGENT_DATA_TYPE_from_charz(&ag, TEST_STRING_3);

            ///act
            auto res = AgentDataTypes_ToString(global_bufferTemp, &ag);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);
            ASSERT_ARE_EQUAL(char_ptr, TEST_STRING_3_JSON, STRING_c_str(global_bufferTemp));

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&ag);
        }

        /*SRS_AGENT_TYPE_SYSTEM_99_044:[ Creates an AGENT_DATA_TYPE containing an EDM_INT32 from int32_t]*/
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_SINT32_succeeds_1)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_AGENT_DATA_TYPE_from_SINT32(&ag, -2147483647-1);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&ag);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_044:[ Creates an AGENT_DATA_TYPE containing an EDM_INT32 from int32_t]*/
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_SINT32_succeeds_2)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_AGENT_DATA_TYPE_from_SINT32(&ag, 2147483647);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&ag);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_044:[ Creates an AGENT_DATA_TYPE containing an EDM_INT32 from int32_t]*/
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_SINT32_succeeds_3)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_AGENT_DATA_TYPE_from_SINT32(&ag, 0);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&ag);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_024:[ EDM_INT32: int32Value = [ sign ] 1*10DIGIT ; numbers in the range from -2147483648 to 2147483647]*/
        TEST_FUNCTION(AgentDataTypes_ToString_SINT32_succeeds_1)
        {
            ///arrange
            AGENT_DATA_TYPE ag;
            (void)Create_AGENT_DATA_TYPE_from_SINT32(&ag, -2147483647-1);

            ///act
            auto res = AgentDataTypes_ToString(global_bufferTemp, &ag);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);
            ASSERT_ARE_EQUAL(char_ptr, "-2147483648", STRING_c_str(global_bufferTemp));

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&ag);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_024:[ EDM_INT32: int32Value = [ sign ] 1*10DIGIT ; numbers in the range from -2147483648 to 2147483647]*/
        TEST_FUNCTION(AgentDataTypes_ToString_SINT32_succeeds_2)
        {
            ///arrange
            AGENT_DATA_TYPE ag;
            (void)Create_AGENT_DATA_TYPE_from_SINT32(&ag, 2147483647);

            ///act
            auto res = AgentDataTypes_ToString(global_bufferTemp, &ag);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);
            ASSERT_ARE_EQUAL(char_ptr, "2147483647", STRING_c_str(global_bufferTemp));

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&ag);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_024:[ EDM_INT32: int32Value = [ sign ] 1*10DIGIT ; numbers in the range from -2147483648 to 2147483647]*/
        TEST_FUNCTION(AgentDataTypes_ToString_SINT32_succeeds_3)
        {
            ///arrange
            AGENT_DATA_TYPE ag;
            (void)Create_AGENT_DATA_TYPE_from_SINT32(&ag, 0);

            ///act
            auto res = AgentDataTypes_ToString(global_bufferTemp, &ag);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);
            ASSERT_ARE_EQUAL(char_ptr, "0", STRING_c_str(global_bufferTemp));

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&ag);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_045:[ Creates an AGENT_DATA_TYPE containing an EDM_INT64 from int64_t]*/
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_SINT64_succeeds_1)
        {
            ///arrange
            AGENT_DATA_TYPE ag;
            
            ///act
            auto res = Create_AGENT_DATA_TYPE_from_SINT64(&ag, 0);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);

            ///clenup
            Destroy_AGENT_DATA_TYPE(&ag);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_045:[ Creates an AGENT_DATA_TYPE containing an EDM_INT64 from int64_t]*/
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_SINT64_succeeds_2)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_AGENT_DATA_TYPE_from_SINT64(&ag, -9223372036854775807LL-1);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);

            ///clenup
            Destroy_AGENT_DATA_TYPE(&ag);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_045:[ Creates an AGENT_DATA_TYPE containing an EDM_INT64 from int64_t]*/
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_SINT64_succeeds_3)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_AGENT_DATA_TYPE_from_SINT64(&ag, 9223372036854775807LL);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);

            ///clenup
            Destroy_AGENT_DATA_TYPE(&ag);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_025:[EDM_INT64:int64Value = [sign] 1 * 19DIGIT; numbers in the range from - 9223372036854775808 to 9223372036854775807]*/
        TEST_FUNCTION(AgentDataTypes_ToString_SINT64_succeeds_1)
        {
            ///arrange
            AGENT_DATA_TYPE ag;
            (void)Create_AGENT_DATA_TYPE_from_SINT64(&ag, -9223372036854775807LL - 1);

            ///act
            auto res = AgentDataTypes_ToString(global_bufferTemp, &ag);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);
            ASSERT_ARE_EQUAL(char_ptr, "-9223372036854775808", STRING_c_str(global_bufferTemp));

            ///clenup
            Destroy_AGENT_DATA_TYPE(&ag);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_025:[EDM_INT64:int64Value = [sign] 1 * 19DIGIT; numbers in the range from - 9223372036854775808 to 9223372036854775807]*/
        TEST_FUNCTION(AgentDataTypes_ToString_SINT64_succeeds_2)
        {
            ///arrange
            AGENT_DATA_TYPE ag;
            (void)Create_AGENT_DATA_TYPE_from_SINT64(&ag, 9223372036854775807LL);

            ///act
            auto res = AgentDataTypes_ToString(global_bufferTemp, &ag);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);
            ASSERT_ARE_EQUAL(char_ptr, "9223372036854775807", STRING_c_str(global_bufferTemp));

            ///clenup
            Destroy_AGENT_DATA_TYPE(&ag);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_046:[ Creates an AGENT_DATA_TYPE containing an EDM_SBYTE from int8_t]*/
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_SINT8_succeeds_1)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_AGENT_DATA_TYPE_from_SINT8(&ag, -128);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);

            ///clenup
            Destroy_AGENT_DATA_TYPE(&ag);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_046:[ Creates an AGENT_DATA_TYPE containing an EDM_SBYTE from int8_t]*/
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_SINT8_succeeds_2)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_AGENT_DATA_TYPE_from_SINT8(&ag, 127);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);

            ///clenup
            Destroy_AGENT_DATA_TYPE(&ag);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_046:[ Creates an AGENT_DATA_TYPE containing an EDM_SBYTE from int8_t]*/
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_SINT8_succeeds_3)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_AGENT_DATA_TYPE_from_SINT8(&ag, 0);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);

            ///clenup
            Destroy_AGENT_DATA_TYPE(&ag);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_046:[ Creates an AGENT_DATA_TYPE containing an EDM_SBYTE from int8_t]*/
        TEST_FUNCTION(AgentDataTypes_ToString_from_SINT8_succeeds_1)
        {
            ///arrange
            AGENT_DATA_TYPE ag;
            (void)Create_AGENT_DATA_TYPE_from_SINT8(&ag, -128);

            ///act
            auto res = AgentDataTypes_ToString(global_bufferTemp, &ag);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);
            ASSERT_ARE_EQUAL(char_ptr, "-128", STRING_c_str(global_bufferTemp));

            ///clenup
            Destroy_AGENT_DATA_TYPE(&ag);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_026:[ EDM_SBYTE: sbyteValue = [ sign ] 1*3DIGIT  ; numbers in the range from -128 to 127]*/
        TEST_FUNCTION(AgentDataTypes_ToString_from_SINT8_succeeds_2)
        {
            ///arrange
            AGENT_DATA_TYPE ag;
            (void)Create_AGENT_DATA_TYPE_from_SINT8(&ag, 0);

            ///act
            auto res = AgentDataTypes_ToString(global_bufferTemp, &ag);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);
            ASSERT_ARE_EQUAL(char_ptr, "0", STRING_c_str(global_bufferTemp));

            ///clenup
            Destroy_AGENT_DATA_TYPE(&ag);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_013:[ All the functions shall check their parameters for validity. When an invalid parameter is detected, the value AGENT_DATA_TYPES_INVALID_ARG shall be returned ].*/
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_Members_with_NULL_agentData_fails)
        {
            ///arrange

            ///act
            auto res = Create_AGENT_DATA_TYPE_from_Members(
                NULL,
                TEST_TYPENAME_GEOLOCATION,
                TEST_TYPENAME_GEOLOCATION_NMEMBERS,
                TEST_TYPENAME_GEOLOCATION_MEMBERNAMES,
                TEST_TYPENAME_GEOLOCATION_MEMBERVALUES);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_055:[If typeName is NULL, the function shall return AGENT_DATA_TYPES_INVALID_ARG .]*/
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_Members_with_NULL_typename_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_AGENT_DATA_TYPE_from_Members(
                &ag,
                NULL,
                TEST_TYPENAME_GEOLOCATION_NMEMBERS,
                TEST_TYPENAME_GEOLOCATION_MEMBERNAMES,
                TEST_TYPENAME_GEOLOCATION_MEMBERVALUES);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_056:[ If nMembers is 0, the function shall return AGENT_DATA_TYPES_INVALID_ARG .]*/
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_Members_with_0_nMembers_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_AGENT_DATA_TYPE_from_Members(
                &ag,
                TEST_TYPENAME_GEOLOCATION,
                0,
                TEST_TYPENAME_GEOLOCATION_MEMBERNAMES,
                TEST_TYPENAME_GEOLOCATION_MEMBERVALUES);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_057:[ If memberNames is NULL, the function shall return AGENT_DATA_TYPES_INVALID_ARG .]*/
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_Members_with_NULL_memberNames_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_AGENT_DATA_TYPE_from_Members(
                &ag,
                TEST_TYPENAME_GEOLOCATION,
                TEST_TYPENAME_GEOLOCATION_NMEMBERS,
                NULL,
                TEST_TYPENAME_GEOLOCATION_MEMBERVALUES);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_058:[ If any of the memberNames[i] is NULL, the function shall return AGENT_DATA_TYPES_INVALID_ARG .]*/
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_Members_with_first_memberName_NULL_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;
            const char * TEST_TYPENAME_GEOLOCATION_MEMBERNAMES_FIRST_NULL[TEST_TYPENAME_GEOLOCATION_NMEMBERS] = { NULL, TEST_TYPENAME_GEOLOCATION_MEMBER2_NAME };

            ///act
            auto res = Create_AGENT_DATA_TYPE_from_Members(
                &ag,
                TEST_TYPENAME_GEOLOCATION,
                TEST_TYPENAME_GEOLOCATION_NMEMBERS,
                TEST_TYPENAME_GEOLOCATION_MEMBERNAMES_FIRST_NULL,
                TEST_TYPENAME_GEOLOCATION_MEMBERVALUES);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_058:[ If any of the memberNames[i] is NULL, the function shall return AGENT_DATA_TYPES_INVALID_ARG .]*/
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_Members_with_second_memberName_NULL_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;
            const char * TEST_TYPENAME_GEOLOCATION_MEMBERNAMES_SECOND_NULL[TEST_TYPENAME_GEOLOCATION_NMEMBERS] = { TEST_TYPENAME_GEOLOCATION_MEMBER1_NAME, NULL };

            ///act
            auto res = Create_AGENT_DATA_TYPE_from_Members(
                &ag,
                TEST_TYPENAME_GEOLOCATION,
                TEST_TYPENAME_GEOLOCATION_NMEMBERS,
                TEST_TYPENAME_GEOLOCATION_MEMBERNAMES_SECOND_NULL,
                TEST_TYPENAME_GEOLOCATION_MEMBERVALUES);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_059:[ If memberValues is NULL, the function shall return AGENT_DATA_TYPES_INVALID_ARG .]*/
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_Members_with_memberValues_NULL_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_AGENT_DATA_TYPE_from_Members(
                &ag,
                TEST_TYPENAME_GEOLOCATION,
                TEST_TYPENAME_GEOLOCATION_NMEMBERS,
                TEST_TYPENAME_GEOLOCATION_MEMBERNAMES,
                NULL);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_063:[If there are two memberNames with the same name, then the function shall return  AGENT_DATA_TYPES_INVALID_ARG.]*/
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_Members_with_two_memberValues_equals_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;
            const char * TEST_TYPENAME_GEOLOCATION_MEMBERNAMES_SAME_NAMES[TEST_TYPENAME_GEOLOCATION_NMEMBERS] = { TEST_TYPENAME_GEOLOCATION_MEMBER1_NAME, TEST_TYPENAME_GEOLOCATION_MEMBER1_NAME };

            ///act
            auto res = Create_AGENT_DATA_TYPE_from_Members(
                &ag,
                TEST_TYPENAME_GEOLOCATION,
                TEST_TYPENAME_GEOLOCATION_NMEMBERS,
                TEST_TYPENAME_GEOLOCATION_MEMBERNAMES_SAME_NAMES,
                TEST_TYPENAME_GEOLOCATION_MEMBERVALUES);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        /* Create_AGENT_DATA_TYPE_from_MemberPointers */

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_109:[ AGENT_DATA_TYPES_INVALID_ARG shall be returned if memberPointerValues parameter is NULL.] */
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_MemberPointers_with_NULL_memberPointerValues_Fails)
        {
            ///arrange
            AGENT_DATA_TYPE agentData;
            const size_t numberOfMembers = 3;
            const char* fields[numberOfMembers] = { "Field0", "Field1", "Field3" };

            ///act
            AGENT_DATA_TYPES_RESULT result = Create_AGENT_DATA_TYPE_from_MemberPointers(&agentData, "MyComplexTypeName", numberOfMembers, fields, NULL);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_111:[ AGENT_DATA_TYPES_OK shall be returned upon success.] */
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_MemberPointers_with_valid_parameters_Succeeds)
        {
            ///arrange
            AGENT_DATA_TYPE agentData;
            const size_t numberOfMembers = 3;
            const char* fieldsNames[numberOfMembers] = { "Field0", "Field1", "Field3" };
            AGENT_DATA_TYPE field0;
            AGENT_DATA_TYPE field1;
            AGENT_DATA_TYPE field2;
            Create_AGENT_DATA_TYPE_from_UINT8(&field0, 8);
            Create_AGENT_DATA_TYPE_from_SINT32(&field1, 32);
            Create_AGENT_DATA_TYPE_from_SINT64(&field2, 64);

            const AGENT_DATA_TYPE* agentDataFields[numberOfMembers] = { &field0, &field1, &field2 };

            ///act
            AGENT_DATA_TYPES_RESULT result = Create_AGENT_DATA_TYPE_from_MemberPointers(&agentData, "MyComplexTypeName", numberOfMembers, fieldsNames, agentDataFields);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_050:[ Destroy_AGENT_DATA_TYPE shall deallocate all allocated resources used to represent the type.]*/
        TEST_FUNCTION(Destroy_AGENT_DATA_TYPE_for_ComplexType_suceeds)
        {
            ///arrange
            AGENT_DATA_TYPE ag;


            (void)Create_AGENT_DATA_TYPE_from_Members(
                &ag,
                TEST_TYPENAME_GEOLOCATION,
                TEST_TYPENAME_GEOLOCATION_NMEMBERS,
                TEST_TYPENAME_GEOLOCATION_MEMBERNAMES,
                TEST_TYPENAME_GEOLOCATION_MEMBERVALUES);

            ///act
            Destroy_AGENT_DATA_TYPE(&ag);

            ///assert
            ///no exceptions, no runtime errors etc
        }


        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_062:[ If the AGENT_DATA_TYPE represents a "complex type", then the JSON marshaller shall produce the following JSON value [...]*/
        TEST_FUNCTION(AgentDataTypes_ToString_for_ComplexType_suceeds)
        {
            ///arrange
            AGENT_DATA_TYPE ag;
            (void)Create_AGENT_DATA_TYPE_from_Members(
                &ag,
                TEST_TYPENAME_GEOLOCATION,
                TEST_TYPENAME_GEOLOCATION_NMEMBERS,
                TEST_TYPENAME_GEOLOCATION_MEMBERNAMES,
                TEST_TYPENAME_GEOLOCATION_MEMBERVALUES);


            STRICT_EXPECTED_CALL((*mocks), MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreArgument(1)
                .IgnoreArgument(2)
                .SetReturn(MULTITREE_HANDLE_VALID);

            STRICT_EXPECTED_CALL((*mocks), MultiTree_AddLeaf(MULTITREE_HANDLE_VALID, TEST_TYPENAME_GEOLOCATION_MEMBER1_NAME, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                .SetReturn(MULTITREE_OK);

            STRICT_EXPECTED_CALL((*mocks), MultiTree_AddLeaf(MULTITREE_HANDLE_VALID, TEST_TYPENAME_GEOLOCATION_MEMBER2_NAME, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                .SetReturn(MULTITREE_OK);

            STRICT_EXPECTED_CALL((*mocks), JSONEncoder_EncodeTree(MULTITREE_HANDLE_VALID, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                .IgnoreArgument(3)
                .SetReturn(JSON_ENCODER_OK);

            STRICT_EXPECTED_CALL((*mocks), MultiTree_Destroy(MULTITREE_HANDLE_VALID));

            ///act
            auto res = AgentDataTypes_ToString(global_bufferTemp, &ag);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&ag);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_016:[ When the value cannot be converted to a string AgentDataTypes_ToString shall return AGENT_DATA_TYPES_ERROR.]*/
        TEST_FUNCTION(AgentDataTypes_ToString_for_ComplexType_fails_when_MultiTree_Create_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;
            (void)Create_AGENT_DATA_TYPE_from_Members(
                &ag,
                TEST_TYPENAME_GEOLOCATION,
                TEST_TYPENAME_GEOLOCATION_NMEMBERS,
                TEST_TYPENAME_GEOLOCATION_MEMBERNAMES,
                TEST_TYPENAME_GEOLOCATION_MEMBERVALUES);


            STRICT_EXPECTED_CALL((*mocks), MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreArgument(1)
                .IgnoreArgument(2)
                .SetReturn((MULTITREE_HANDLE)(NULL));

            ///act
            auto res = AgentDataTypes_ToString(global_bufferTemp, &ag);

            ///assert - assertions have ot be made with JSON parser, because else... not valid JSON maybe
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_ERROR, res);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&ag);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_016:[ When the value cannot be converted to a string AgentDataTypes_ToString shall return AGENT_DATA_TYPES_ERROR.]*/
        TEST_FUNCTION(AgentDataTypes_ToString_for_ComplexType_fails_when_MultiTree_Add_Leaf_fails_1)
        {
            ///arrange

            AGENT_DATA_TYPE ag;
            (void)Create_AGENT_DATA_TYPE_from_Members(
                &ag,
                TEST_TYPENAME_GEOLOCATION,
                TEST_TYPENAME_GEOLOCATION_NMEMBERS,
                TEST_TYPENAME_GEOLOCATION_MEMBERNAMES,
                TEST_TYPENAME_GEOLOCATION_MEMBERVALUES);

            STRICT_EXPECTED_CALL((*mocks), MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreArgument(1)
                .IgnoreArgument(2)
                .SetReturn(MULTITREE_HANDLE_VALID);

            STRICT_EXPECTED_CALL((*mocks), MultiTree_AddLeaf(MULTITREE_HANDLE_VALID, TEST_TYPENAME_GEOLOCATION_MEMBER1_NAME, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                .SetReturn(MULTITREE_ERROR);

            //Destroy has to appear in all cases
            STRICT_EXPECTED_CALL((*mocks), MultiTree_Destroy(MULTITREE_HANDLE_VALID));

            ///act
            auto res = AgentDataTypes_ToString(global_bufferTemp, &ag);

            ///assert - assertions have ot be made with JSON parser, because else... not valid JSON maybe
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_ERROR, res);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&ag);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_016:[ When the value cannot be converted to a string AgentDataTypes_ToString shall return AGENT_DATA_TYPES_ERROR.]*/
        TEST_FUNCTION(AgentDataTypes_ToString_for_ComplexType_fails_when_MultiTree_Add_Leaf_fails_2)
        {
            ///arrange
            AGENT_DATA_TYPE ag;
            (void)Create_AGENT_DATA_TYPE_from_Members(
                &ag,
                TEST_TYPENAME_GEOLOCATION,
                TEST_TYPENAME_GEOLOCATION_NMEMBERS,
                TEST_TYPENAME_GEOLOCATION_MEMBERNAMES,
                TEST_TYPENAME_GEOLOCATION_MEMBERVALUES);


            STRICT_EXPECTED_CALL((*mocks), MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreArgument(1)
                .IgnoreArgument(2)
                .SetReturn(MULTITREE_HANDLE_VALID);

            STRICT_EXPECTED_CALL((*mocks), MultiTree_AddLeaf(MULTITREE_HANDLE_VALID, TEST_TYPENAME_GEOLOCATION_MEMBER1_NAME, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                .SetReturn(MULTITREE_OK);

            STRICT_EXPECTED_CALL((*mocks), MultiTree_AddLeaf(MULTITREE_HANDLE_VALID, TEST_TYPENAME_GEOLOCATION_MEMBER2_NAME, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                .SetReturn(MULTITREE_ERROR);

            STRICT_EXPECTED_CALL((*mocks), MultiTree_Destroy(MULTITREE_HANDLE_VALID));

            ///act
            auto res = AgentDataTypes_ToString(global_bufferTemp, &ag);

            ///assert - assertions have ot be made with JSON parser, because else... not valid JSON maybe
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_ERROR, res);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&ag);
        }



        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_016:[ When the value cannot be converted to a string AgentDataTypes_ToString shall return AGENT_DATA_TYPES_ERROR.]*/
        TEST_FUNCTION(AgentDataTypes_ToString_for_ComplexType_fails_when_JSON_Encoder_fails)
        {
            ///arrange

            AGENT_DATA_TYPE ag;
            (void)Create_AGENT_DATA_TYPE_from_Members(
                &ag,
                TEST_TYPENAME_GEOLOCATION,
                TEST_TYPENAME_GEOLOCATION_NMEMBERS,
                TEST_TYPENAME_GEOLOCATION_MEMBERNAMES,
                TEST_TYPENAME_GEOLOCATION_MEMBERVALUES);


            STRICT_EXPECTED_CALL((*mocks), MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreArgument(1)
                .IgnoreArgument(2)
                .SetReturn(MULTITREE_HANDLE_VALID);

            STRICT_EXPECTED_CALL((*mocks), MultiTree_AddLeaf(MULTITREE_HANDLE_VALID, TEST_TYPENAME_GEOLOCATION_MEMBER1_NAME, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                .SetReturn(MULTITREE_OK);

            STRICT_EXPECTED_CALL((*mocks), MultiTree_AddLeaf(MULTITREE_HANDLE_VALID, TEST_TYPENAME_GEOLOCATION_MEMBER2_NAME, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                .SetReturn(MULTITREE_OK);

            STRICT_EXPECTED_CALL((*mocks), JSONEncoder_EncodeTree(MULTITREE_HANDLE_VALID, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                .IgnoreArgument(3)
                .SetReturn(JSON_ENCODER_ERROR);

            STRICT_EXPECTED_CALL((*mocks), MultiTree_Destroy(MULTITREE_HANDLE_VALID));

            ///act
            auto res = AgentDataTypes_ToString(global_bufferTemp, &ag);

            ///assert - assertions have ot be made with JSON parser, because else... not valid JSON maybe
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_ERROR, res);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&ag);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_016:[ When the value cannot be converted to a string AgentDataTypes_ToString shall return AGENT_DATA_TYPES_ERROR.]*/
        TEST_FUNCTION(AgentDataTypes_ToString_for_ComplexType_fails_when_MultiTree_create_failss)
        {
            ///arrange
            /*we are just going to use truck1... because*/

            STRICT_EXPECTED_CALL((*mocks), MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreArgument(1)
                .IgnoreArgument(2)
                ;

            ///act
            auto res = AgentDataTypes_ToString(global_bufferTemp, &truck1);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_ERROR, res);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_016:[ When the value cannot be converted to a string AgentDataTypes_ToString shall return AGENT_DATA_TYPES_ERROR.]*/
        TEST_FUNCTION(AgentDataTypes_ToString_for_ComplexType_fails_when_MultiTree_AddLeaf_fails_1)
        {
            ///arrange
            /*we are just going to use truck1... because*/

            STRICT_EXPECTED_CALL((*mocks), MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreArgument(1)
                .IgnoreArgument(2)
                .SetReturn(MULTITREE_HANDLE_VALID)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_AddLeaf(MULTITREE_HANDLE_VALID, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                .IgnoreArgument(3)
                .SetReturn(MULTITREE_ERROR)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_Destroy(MULTITREE_HANDLE_VALID));

            ///act
            auto res = AgentDataTypes_ToString(global_bufferTemp, &truck1);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_ERROR, res);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_016:[ When the value cannot be converted to a string AgentDataTypes_ToString shall return AGENT_DATA_TYPES_ERROR.]*/
        TEST_FUNCTION(AgentDataTypes_ToString_for_ComplexType_fails_when_MultiTree_AddLeaf_fails_2)
        {
            ///arrange
            /*we are just going to use truck1... because*/
            size_t nSuccessAdds = 1;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreArgument(1)
                .IgnoreArgument(2)
                .SetReturn(MULTITREE_HANDLE_VALID)
                ;

            for (size_t i = 0; i < nSuccessAdds; i++)
            {
                STRICT_EXPECTED_CALL((*mocks), MultiTree_AddLeaf(MULTITREE_HANDLE_VALID, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                    .IgnoreArgument(2)
                    .IgnoreArgument(3)
                    .SetReturn(MULTITREE_OK)
                    ;
            }

            STRICT_EXPECTED_CALL((*mocks), MultiTree_AddLeaf(MULTITREE_HANDLE_VALID, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                .IgnoreArgument(3)
                .SetReturn(MULTITREE_ERROR)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_Destroy(MULTITREE_HANDLE_VALID));

            ///act
            auto res = AgentDataTypes_ToString(global_bufferTemp, &truck1);

            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_ERROR, res);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_016:[ When the value cannot be converted to a string AgentDataTypes_ToString shall return AGENT_DATA_TYPES_ERROR.]*/
        TEST_FUNCTION(AgentDataTypes_ToString_for_ComplexType_fails_when_MultiTree_AddLeaf_fails_3)
        {
            ///arrange
            /*we are just going to use truck1... because*/
            size_t nSuccessAdds = 2;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreArgument(1)
                .IgnoreArgument(2)
                .SetReturn(MULTITREE_HANDLE_VALID)
                ;

            for (size_t i = 0; i < nSuccessAdds; i++)
            {
                STRICT_EXPECTED_CALL((*mocks), MultiTree_AddLeaf(MULTITREE_HANDLE_VALID, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                    .IgnoreArgument(2)
                    .IgnoreArgument(3)
                    .SetReturn(MULTITREE_OK)
                    ;
            }

            STRICT_EXPECTED_CALL((*mocks), MultiTree_AddLeaf(MULTITREE_HANDLE_VALID, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                .IgnoreArgument(3)
                .SetReturn(MULTITREE_ERROR)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_Destroy(MULTITREE_HANDLE_VALID));

            ///act
            auto res = AgentDataTypes_ToString(global_bufferTemp, &truck1);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_ERROR, res);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_016:[ When the value cannot be converted to a string AgentDataTypes_ToString shall return AGENT_DATA_TYPES_ERROR.]*/
        TEST_FUNCTION(AgentDataTypes_ToString_for_ComplexType_fails_when_MultiTree_AddLeaf_fails_4)
        {
            ///arrange
            /*we are just going to use truck1... because*/
            size_t nSuccessAdds = 3;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreArgument(1)
                .IgnoreArgument(2)
                .SetReturn(MULTITREE_HANDLE_VALID)
                ;

            for (size_t i = 0; i < nSuccessAdds; i++)
            {
                STRICT_EXPECTED_CALL((*mocks), MultiTree_AddLeaf(MULTITREE_HANDLE_VALID, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                    .IgnoreArgument(2)
                    .IgnoreArgument(3)
                    .SetReturn(MULTITREE_OK)
                    ;
            }

            STRICT_EXPECTED_CALL((*mocks), MultiTree_AddLeaf(MULTITREE_HANDLE_VALID, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                .IgnoreArgument(3)
                .SetReturn(MULTITREE_ERROR)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_Destroy(MULTITREE_HANDLE_VALID));

            ///act
            auto res = AgentDataTypes_ToString(global_bufferTemp, &truck1);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_ERROR, res);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_016:[ When the value cannot be converted to a string AgentDataTypes_ToString shall return AGENT_DATA_TYPES_ERROR.]*/
        TEST_FUNCTION(AgentDataTypes_ToString_for_ComplexType_fails_when_MultiTree_AddLeaf_fails_5)
        {
            ///arrange
            /*we are just going to use truck1... because*/
            size_t nSuccessAdds = 4;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreArgument(1)
                .IgnoreArgument(2)
                .SetReturn(MULTITREE_HANDLE_VALID)
                ;

            for (size_t i = 0; i < nSuccessAdds; i++)
            {
                STRICT_EXPECTED_CALL((*mocks), MultiTree_AddLeaf(MULTITREE_HANDLE_VALID, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                    .IgnoreArgument(2)
                    .IgnoreArgument(3)
                    .SetReturn(MULTITREE_OK)
                    ;
            }

            STRICT_EXPECTED_CALL((*mocks), MultiTree_AddLeaf(MULTITREE_HANDLE_VALID, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                .IgnoreArgument(3)
                .SetReturn(MULTITREE_ERROR)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_Destroy(MULTITREE_HANDLE_VALID));

            ///act
            auto res = AgentDataTypes_ToString(global_bufferTemp,  &truck1);

            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_ERROR, res);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_016:[ When the value cannot be converted to a string AgentDataTypes_ToString shall return AGENT_DATA_TYPES_ERROR.]*/
        TEST_FUNCTION(AgentDataTypes_ToString_for_ComplexType_fails_when_JSONEncoder_EncodeTree_fails)
        {
            ///arrange
            /*we are just going to use truck1... because*/

            size_t nSuccessAdds = 5;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreArgument(1)
                .IgnoreArgument(2)
                .SetReturn(MULTITREE_HANDLE_VALID)
                ;

            for (size_t i = 0; i < nSuccessAdds; i++)
            {
                STRICT_EXPECTED_CALL((*mocks), MultiTree_AddLeaf(MULTITREE_HANDLE_VALID, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                    .IgnoreArgument(2)
                    .IgnoreArgument(3)
                    .SetReturn(MULTITREE_OK)
                    ;
            }

            STRICT_EXPECTED_CALL((*mocks), JSONEncoder_EncodeTree(MULTITREE_HANDLE_VALID, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreArgument(2)
                .IgnoreArgument(3)
                .SetReturn(JSON_ENCODER_ERROR)
                ;

            STRICT_EXPECTED_CALL((*mocks), MultiTree_Destroy(MULTITREE_HANDLE_VALID));

            ///act
            auto res = AgentDataTypes_ToString(global_bufferTemp,  &truck1);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_ERROR, res);
        }

        /* Create_AGENT_DATA_TYPE_from_AGENT_DATA_TYPE */

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_064:[If any argument is NULL Create_AGENT_DATA_TYPE_from_AGENT_DATA_TYPE shall return AGENT_DATA_TYPES_INVALID_ARG.] */
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_From_AGENT_DATA_TYPE_With_NULL_Source_Fails)
        {
            ///arrange
            AGENT_DATA_TYPE dst;

            ///act
            AGENT_DATA_TYPES_RESULT result = Create_AGENT_DATA_TYPE_from_AGENT_DATA_TYPE(&dst, NULL);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_064:[If any argument is NULL Create_AGENT_DATA_TYPE_from_AGENT_DATA_TYPE shall return AGENT_DATA_TYPES_INVALID_ARG.] */
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_From_AGENT_DATA_TYPE_With_NULL_Destination_Fails)
        {
            ///arrange
            AGENT_DATA_TYPE src;

            ///act
            AGENT_DATA_TYPES_RESULT result = Create_AGENT_DATA_TYPE_from_AGENT_DATA_TYPE(NULL, &src);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_065:[Create_AGENT_DATA_TYPE_from_AGENT_DATA_TYPE shall clone the value of an existing agent data.] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_066:[On success Create_AGENT_DATA_TYPE_from_AGENT_DATA_TYPE shall return AGENT_DATA_TYPE_OK.] */
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_From_AGENT_DATA_TYPE_With_Boolean_Succeeds)
        {
            ///arrange
            AGENT_DATA_TYPE src;
            AGENT_DATA_TYPE dst;

            (void)Create_EDM_BOOLEAN_from_int(&src, 0);

            ///act
            AGENT_DATA_TYPES_RESULT result = Create_AGENT_DATA_TYPE_from_AGENT_DATA_TYPE(&dst, &src);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_BOOLEAN_TYPE, dst.type);
            ASSERT_ARE_EQUAL(EDM_BOOLEANS, EDM_FALSE, dst.value.edmBoolean.value);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&src);
            Destroy_AGENT_DATA_TYPE(&dst);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_065:[Create_AGENT_DATA_TYPE_from_AGENT_DATA_TYPE shall clone the value of an existing agent data.] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_066:[On success Create_AGENT_DATA_TYPE_from_AGENT_DATA_TYPE shall return AGENT_DATA_TYPE_OK.] */
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_From_AGENT_DATA_TYPE_With_SInt8_Succeeds)
        {
            ///arrange
            AGENT_DATA_TYPE src;
            AGENT_DATA_TYPE dst;

            (void)Create_AGENT_DATA_TYPE_from_SINT8(&src, 42);

            ///act
            AGENT_DATA_TYPES_RESULT result = Create_AGENT_DATA_TYPE_from_AGENT_DATA_TYPE(&dst, &src);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_SBYTE_TYPE, dst.type);
            ASSERT_ARE_EQUAL(int8_t,(int8_t)42, dst.value.edmSbyte.value);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&src);
            Destroy_AGENT_DATA_TYPE(&dst);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_065:[Create_AGENT_DATA_TYPE_from_AGENT_DATA_TYPE shall clone the value of an existing agent data.] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_066:[On success Create_AGENT_DATA_TYPE_from_AGENT_DATA_TYPE shall return AGENT_DATA_TYPE_OK.] */
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_From_AGENT_DATA_TYPE_With_UInt8_Succeeds)
        {
            ///arrange
            AGENT_DATA_TYPE src;
            AGENT_DATA_TYPE dst;

            (void)Create_AGENT_DATA_TYPE_from_UINT8(&src, 42);

            ///act
            AGENT_DATA_TYPES_RESULT result = Create_AGENT_DATA_TYPE_from_AGENT_DATA_TYPE(&dst, &src);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_BYTE_TYPE, dst.type);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)42, dst.value.edmByte.value);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&src);
            Destroy_AGENT_DATA_TYPE(&dst);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_065:[Create_AGENT_DATA_TYPE_from_AGENT_DATA_TYPE shall clone the value of an existing agent data.] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_066:[On success Create_AGENT_DATA_TYPE_from_AGENT_DATA_TYPE shall return AGENT_DATA_TYPE_OK.] */
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_From_AGENT_DATA_TYPE_With_SInt16_Succeeds)
        {
            ///arrange
            AGENT_DATA_TYPE src;
            AGENT_DATA_TYPE dst;

            (void)Create_AGENT_DATA_TYPE_from_SINT16(&src, 4242);

            ///act
            AGENT_DATA_TYPES_RESULT result = Create_AGENT_DATA_TYPE_from_AGENT_DATA_TYPE(&dst, &src);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_INT16_TYPE, dst.type);
            ASSERT_ARE_EQUAL(int16_t, (int16_t)4242, dst.value.edmInt16.value);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&src);
            Destroy_AGENT_DATA_TYPE(&dst);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_065:[Create_AGENT_DATA_TYPE_from_AGENT_DATA_TYPE shall clone the value of an existing agent data.] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_066:[On success Create_AGENT_DATA_TYPE_from_AGENT_DATA_TYPE shall return AGENT_DATA_TYPE_OK.] */
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_From_AGENT_DATA_TYPE_With_Int32_Succeeds)
        {
            ///arrange
            AGENT_DATA_TYPE src;
            AGENT_DATA_TYPE dst;

            (void)Create_AGENT_DATA_TYPE_from_SINT32(&src, 4242);

            ///act
            AGENT_DATA_TYPES_RESULT result = Create_AGENT_DATA_TYPE_from_AGENT_DATA_TYPE(&dst, &src);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_INT32_TYPE, dst.type);
            ASSERT_ARE_EQUAL(int32_t, (int32_t)4242, dst.value.edmInt32.value);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&src);
            Destroy_AGENT_DATA_TYPE(&dst);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_065:[Create_AGENT_DATA_TYPE_from_AGENT_DATA_TYPE shall clone the value of an existing agent data.] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_066:[On success Create_AGENT_DATA_TYPE_from_AGENT_DATA_TYPE shall return AGENT_DATA_TYPE_OK.] */
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_From_AGENT_DATA_TYPE_With_Int64_Succeeds)
        {
            ///arrange
            AGENT_DATA_TYPE src;
            AGENT_DATA_TYPE dst;

            (void)Create_AGENT_DATA_TYPE_from_SINT64(&src, 42424242);

            ///act
            AGENT_DATA_TYPES_RESULT result = Create_AGENT_DATA_TYPE_from_AGENT_DATA_TYPE(&dst, &src);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_INT64_TYPE, dst.type);
            ASSERT_ARE_EQUAL(int32_t, (int32_t)42424242, (int32_t)dst.value.edmInt64.value);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&src);
            Destroy_AGENT_DATA_TYPE(&dst);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_065:[Create_AGENT_DATA_TYPE_from_AGENT_DATA_TYPE shall clone the value of an existing agent data.] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_066:[On success Create_AGENT_DATA_TYPE_from_AGENT_DATA_TYPE shall return AGENT_DATA_TYPE_OK.] */
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_From_AGENT_DATA_TYPE_With_charz_Succeeds)
        {
            ///arrange
            AGENT_DATA_TYPE src;
            AGENT_DATA_TYPE dst;

            (void)Create_AGENT_DATA_TYPE_from_charz(&src, "42424242");

            ///act
            AGENT_DATA_TYPES_RESULT result = Create_AGENT_DATA_TYPE_from_AGENT_DATA_TYPE(&dst, &src);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_STRING_TYPE, dst.type);
            ASSERT_ARE_EQUAL(char_ptr, "42424242", (char*)dst.value.edmString.chars);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&src);
            Destroy_AGENT_DATA_TYPE(&dst);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_065:[Create_AGENT_DATA_TYPE_from_AGENT_DATA_TYPE shall clone the value of an existing agent data.] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_066:[On success Create_AGENT_DATA_TYPE_from_AGENT_DATA_TYPE shall return AGENT_DATA_TYPE_OK.] */
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_From_AGENT_DATA_TYPE_With_DOUBLE_Succeeds)
        {
            ///arrange
            AGENT_DATA_TYPE src;
            AGENT_DATA_TYPE dst;

            (void)Create_AGENT_DATA_TYPE_from_DOUBLE(&src, 42.42);

            ///act
            AGENT_DATA_TYPES_RESULT result = Create_AGENT_DATA_TYPE_from_AGENT_DATA_TYPE(&dst, &src);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_DOUBLE_TYPE, dst.type);
            ASSERT_ARE_EQUAL(double, 42.42, dst.value.edmDouble.value);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&src);
            Destroy_AGENT_DATA_TYPE(&dst);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_065:[Create_AGENT_DATA_TYPE_from_AGENT_DATA_TYPE shall clone the value of an existing agent data.] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_066:[On success Create_AGENT_DATA_TYPE_from_AGENT_DATA_TYPE shall return AGENT_DATA_TYPE_OK.] */
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_From_AGENT_DATA_TYPE_With_Single_Succeeds)
        {
            ///arrange
            AGENT_DATA_TYPE src;
            AGENT_DATA_TYPE dst;

            (void)Create_AGENT_DATA_TYPE_from_FLOAT(&src, 42.42f);

            ///act
            AGENT_DATA_TYPES_RESULT result = Create_AGENT_DATA_TYPE_from_AGENT_DATA_TYPE(&dst, &src);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_SINGLE_TYPE, dst.type);
            ASSERT_ARE_EQUAL(float, (float)42.42, dst.value.edmSingle.value);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&src);
            Destroy_AGENT_DATA_TYPE(&dst);
        }


        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_065:[Create_AGENT_DATA_TYPE_from_AGENT_DATA_TYPE shall clone the value of an existing agent data.] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_066:[On success Create_AGENT_DATA_TYPE_from_AGENT_DATA_TYPE shall return AGENT_DATA_TYPE_OK.] */
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_From_AGENT_DATA_TYPE_With_DateTimeOffset_Succeeds)
        {
            ///arrange
            AGENT_DATA_TYPE src;
            AGENT_DATA_TYPE dst;

            EDM_DATE_TIME_OFFSET someDateTimeOffset;
            someDateTimeOffset.dateTime.tm_year = 114; /*so 2014*/
            someDateTimeOffset.dateTime.tm_mon = 6 - 1;
            someDateTimeOffset.dateTime.tm_mday = 18;
            someDateTimeOffset.dateTime.tm_hour = 9;
            someDateTimeOffset.dateTime.tm_min = 41;
            someDateTimeOffset.dateTime.tm_sec = 23;
            someDateTimeOffset.hasFractionalSecond = 0;
            someDateTimeOffset.fractionalSecond = 0;
            someDateTimeOffset.hasTimeZone = 0;
            someDateTimeOffset.timeZoneHour = 0;
            someDateTimeOffset.timeZoneMinute = 0;

            (void)Create_AGENT_DATA_TYPE_from_EDM_DATE_TIME_OFFSET(&src, someDateTimeOffset);

            ///act
            AGENT_DATA_TYPES_RESULT result = Create_AGENT_DATA_TYPE_from_AGENT_DATA_TYPE(&dst, &src);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_DATE_TIME_OFFSET_TYPE, dst.type);
            ASSERT_ARE_EQUAL(int, src.value.edmDateTimeOffset.dateTime.tm_year,               dst.value.edmDateTimeOffset.dateTime.tm_year);
            ASSERT_ARE_EQUAL(int, src.value.edmDateTimeOffset.dateTime.tm_mon,                dst.value.edmDateTimeOffset.dateTime.tm_mon);
            ASSERT_ARE_EQUAL(int, src.value.edmDateTimeOffset.dateTime.tm_mday,               dst.value.edmDateTimeOffset.dateTime.tm_mday);
            ASSERT_ARE_EQUAL(int, src.value.edmDateTimeOffset.dateTime.tm_hour,               dst.value.edmDateTimeOffset.dateTime.tm_hour);
            ASSERT_ARE_EQUAL(int, src.value.edmDateTimeOffset.dateTime.tm_min,                dst.value.edmDateTimeOffset.dateTime.tm_min);
            ASSERT_ARE_EQUAL(int, src.value.edmDateTimeOffset.dateTime.tm_sec,                dst.value.edmDateTimeOffset.dateTime.tm_sec);
            ASSERT_ARE_EQUAL(int, src.value.edmDateTimeOffset.hasFractionalSecond,   dst.value.edmDateTimeOffset.hasFractionalSecond);
            ASSERT_ARE_EQUAL(int, src.value.edmDateTimeOffset.hasTimeZone,           dst.value.edmDateTimeOffset.hasTimeZone);
            ASSERT_ARE_EQUAL(int, src.value.edmDateTimeOffset.timeZoneHour,          dst.value.edmDateTimeOffset.timeZoneHour);
            ASSERT_ARE_EQUAL(int, src.value.edmDateTimeOffset.timeZoneMinute,        dst.value.edmDateTimeOffset.timeZoneMinute);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&src);
            Destroy_AGENT_DATA_TYPE(&dst);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_065:[Create_AGENT_DATA_TYPE_from_AGENT_DATA_TYPE shall clone the value of an existing agent data.] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_066:[On success Create_AGENT_DATA_TYPE_from_AGENT_DATA_TYPE shall return AGENT_DATA_TYPE_OK.] */
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_From_AGENT_DATA_TYPE_With_ComplexType_One_Member_Succeeds)
        {
            ///arrange
            AGENT_DATA_TYPE srcMember;
            AGENT_DATA_TYPE srcStruct;
            AGENT_DATA_TYPE dst;
            const char* memberNames[] = { "a" };

            time_t t;

            time(&t);

            (void)Create_AGENT_DATA_TYPE_from_SINT32(&srcMember, 42);
            Create_AGENT_DATA_TYPE_from_Members(&srcStruct, "SomeStruct", 1, memberNames, &srcMember);

            ///act
            AGENT_DATA_TYPES_RESULT result = Create_AGENT_DATA_TYPE_from_AGENT_DATA_TYPE(&dst, &srcStruct);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_COMPLEX_TYPE_TYPE, dst.type);
            ASSERT_ARE_EQUAL(size_t, (size_t)1, dst.value.edmComplexType.nMembers);
            ASSERT_ARE_EQUAL(char_ptr, "a", dst.value.edmComplexType.fields[0].fieldName);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_INT32_TYPE, dst.value.edmComplexType.fields[0].value->type);
            ASSERT_ARE_EQUAL(int32_t, 42, dst.value.edmComplexType.fields[0].value->value.edmInt32.value);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&srcStruct);
            Destroy_AGENT_DATA_TYPE(&srcMember);
            Destroy_AGENT_DATA_TYPE(&dst);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_065:[Create_AGENT_DATA_TYPE_from_AGENT_DATA_TYPE shall clone the value of an existing agent data.] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_066:[On success Create_AGENT_DATA_TYPE_from_AGENT_DATA_TYPE shall return AGENT_DATA_TYPE_OK.] */
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_From_AGENT_DATA_TYPE_With_ComplexType_2_Members_Succeeds)
        {
            ///arrange
            AGENT_DATA_TYPE srcMember[2];
            AGENT_DATA_TYPE srcStruct;
            AGENT_DATA_TYPE dst;
            const char* memberNames[] = { "a", "b" };

            time_t t;

            time(&t);

            (void)Create_AGENT_DATA_TYPE_from_SINT32(&srcMember[0], 42);
            (void)Create_AGENT_DATA_TYPE_from_FLOAT(&srcMember[1], 42.42f);
            Create_AGENT_DATA_TYPE_from_Members(&srcStruct, "SomeStruct", 2, memberNames, srcMember);

            ///act
            AGENT_DATA_TYPES_RESULT result = Create_AGENT_DATA_TYPE_from_AGENT_DATA_TYPE(&dst, &srcStruct);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_COMPLEX_TYPE_TYPE, dst.type);
            ASSERT_ARE_EQUAL(size_t, (size_t)2, dst.value.edmComplexType.nMembers);
            ASSERT_ARE_EQUAL(char_ptr, "a", dst.value.edmComplexType.fields[0].fieldName);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_INT32_TYPE, dst.value.edmComplexType.fields[0].value->type);
            ASSERT_ARE_EQUAL(int32_t, 42, dst.value.edmComplexType.fields[0].value->value.edmInt32.value);
            ASSERT_ARE_EQUAL(char_ptr, "b", dst.value.edmComplexType.fields[1].fieldName);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_SINGLE_TYPE, dst.value.edmComplexType.fields[1].value->type);
            ASSERT_ARE_EQUAL(float, 42.42f, dst.value.edmComplexType.fields[1].value->value.edmSingle.value);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&srcStruct);
            Destroy_AGENT_DATA_TYPE(&dst);
            Destroy_AGENT_DATA_TYPE(&srcMember[0]);
            Destroy_AGENT_DATA_TYPE(&srcMember[1]);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_039:[ Creates an AGENT_DATA_TYPE containing an EDM_DECIMAL from a null-terminated string.]*/ /*this and the next few hundred lines of code*/
        TEST_FUNCTION(Create_EDM_DECIMAL_From_NULL_string_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, NULL);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_zero_length_string_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        /*the following test strings shall create succesfully EDM_DECIMAL (n means 1 digit)
1       n
2       -n
3       +n

4       nn
5       -nn
6       +nn

7       n.n
8       -n.n
9       +n.n

0       nn.n
1       -nn.n
2       +nn.n

3       n.nn
4       -n.nn
5       +n.nn

6       nn.nn
7       -nn.nn
8       +nn.nn

        On top of these tests, the following alterations shall be done to make the creation fail: 
        a means a letter such as 'z'
        p means '+'
        m means '-'
        d means '.'
        These can be added before a good number or after and sometimes they will fail

1       an           na       N/A        np        N/A           nm       dn        nd
2       a-n          -na      p-n        -np       m-n           -nm      d-n       -nd
3       a+n          +na      p+n        +np       m+n           +nm      d+n       +nd
                                                                                    
4       ann          nna      N/A        nnp       N/A           nnm      dnn       nnd
5       a-nn         -nna     p-nn       -nnp      m-nn          -nnm     d-nn      -nnd
6       a+nn         +nna     p+nn       +nnp      m+nn          +nnm     d+nn      +nnd
                                                                                    
7       an.n          n.na    N/A        n.np      N/A           n.nm     dn.n      n.nd
8       a-n.n        -n.na    p-n.n      -n.np     m-n.n         -n.nm    d-n.n     -n.nd
9       a+n.n        +n.na    p+n.n      +n.np     m+n.n         +n.nm    d+n.n     +n.nd

0       ann.n        nn.na    N/A        nn.np     mnn.n         nn.nm    dnn.n     nn.nd
1       a-nn.n       -nn.na   p-nn.n     -nn.np    m-nn.n        -nn.nm   d-nn.n    -nn.nd
2       a+nn.n       +nn.na   p+nn.n     +nn.np    m+nn.n        +nn.nm   d+nn.n    +nn.nd
                                                                                    
3       an.nn        n.nna    N/A        n.nnp     N/A           n.nnm    dn.nn     n.nnd
4       a-n.nn       -n.nna   p-n.nn     -n.nnp    m-n.nn        -n.nnm   d-n.nn    -n.nnd
5       a+n.nn       +n.nna   p+n.nn     +n.nnp    m+n.nn        +n.nnm   d+n.nn    +n.nnd

6       ann.nn       nn.nna   N/A        nn.nnp    N/A           nn.nnm   dnn.nn    nn.nnd
7       a-nn.nn      -nn.nna  p-nn.nn    -nn.nnp   m-nn.nn       -nn.nnm  d-nn.nn   -nn.nnd
8       a+nn.nn      +nn.nna  p+nn.nn    +nn.nnp   m+nn.nn       +nn.nnm  d+nn.nn   +nn.nnd

        The test names shall follow the above notation so m+nn.nn will becomes
        Create_EDM_DECIMAL_From_m_plus_nn_dot_nn. Notice how the "good" is full string, the wrong is just "m"

        */

        /*Codes_SRS_AGENT_TYPE_SYSTEM_99_067:[ If the string indicated by the parameter v does not match exactly an ODATA string representation, AGENT_DATA_TYPES_INVALID_ARG shall be returned.]*/
        TEST_FUNCTION(Create_EDM_DECIMAL_From_n_succeeds)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "1");
            auto res2 = AgentDataTypes_ToString(global_bufferTemp, &ag);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res2);
            ASSERT_ARE_EQUAL(int, 0, strcmp(STRING_c_str(global_bufferTemp), "1"));

            //cleanup
            Destroy_AGENT_DATA_TYPE(&ag);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_minus_n_succeeds)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "-2");
            auto res2 = AgentDataTypes_ToString(global_bufferTemp, &ag);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res2);
            ASSERT_ARE_EQUAL(int, 0, strcmp(STRING_c_str(global_bufferTemp), "-2"));

            //cleanup
            Destroy_AGENT_DATA_TYPE(&ag);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_plus_n_succeeds)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "+3");
            auto res2 = AgentDataTypes_ToString(global_bufferTemp, &ag);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res2);
            ASSERT_ARE_EQUAL(int, 0, strcmp(STRING_c_str(global_bufferTemp), "+3"));

            //cleanup
            Destroy_AGENT_DATA_TYPE(&ag);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_n_n_succeeds)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "44");
            auto res2 = AgentDataTypes_ToString(global_bufferTemp, &ag);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res2);
            ASSERT_ARE_EQUAL(int, 0, strcmp(STRING_c_str(global_bufferTemp), "44"));

            //cleanup
            Destroy_AGENT_DATA_TYPE(&ag);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_minus_n_n_succeeds)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "-55");
            auto res2 = AgentDataTypes_ToString(global_bufferTemp, &ag);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res2);
            ASSERT_ARE_EQUAL(int, 0, strcmp(STRING_c_str(global_bufferTemp), "-55"));

            //cleanup
            Destroy_AGENT_DATA_TYPE(&ag);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_plus_n_n_succeeds)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "+66");
            auto res2 = AgentDataTypes_ToString(global_bufferTemp, &ag);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res2);
            ASSERT_ARE_EQUAL(int, 0, strcmp(STRING_c_str(global_bufferTemp), "+66"));

            //cleanup
            Destroy_AGENT_DATA_TYPE(&ag);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_n_dot_n_succeeds)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "7.7");
            auto res2 = AgentDataTypes_ToString(global_bufferTemp, &ag);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res2);
            ASSERT_ARE_EQUAL(int, 0, strcmp(STRING_c_str(global_bufferTemp), "7.7"));

            //cleanup
            Destroy_AGENT_DATA_TYPE(&ag);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_minus_n_dot_n_succeeds)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "-8.8");
            auto res2 = AgentDataTypes_ToString(global_bufferTemp, &ag);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res2);
            ASSERT_ARE_EQUAL(int, 0, strcmp(STRING_c_str(global_bufferTemp), "-8.8"));

            //cleanup
            Destroy_AGENT_DATA_TYPE(&ag);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_plus_n_dot_n_succeeds)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "+9.9");
            auto res2 = AgentDataTypes_ToString(global_bufferTemp, &ag);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res2);
            ASSERT_ARE_EQUAL(int, 0, strcmp(STRING_c_str(global_bufferTemp), "+9.9"));

            //cleanup
            Destroy_AGENT_DATA_TYPE(&ag);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_n_n_dot_n_succeeds)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "00.0");
            auto res2 = AgentDataTypes_ToString(global_bufferTemp, &ag);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res2);
            ASSERT_ARE_EQUAL(int, 0, strcmp(STRING_c_str(global_bufferTemp), "00.0"));

            //cleanup
            Destroy_AGENT_DATA_TYPE(&ag);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_minus_n_n_dot_n_succeeds)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "-11.1");
            auto res2 = AgentDataTypes_ToString(global_bufferTemp, &ag);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res2);
            ASSERT_ARE_EQUAL(int, 0, strcmp(STRING_c_str(global_bufferTemp), "-11.1"));

            //cleanup
            Destroy_AGENT_DATA_TYPE(&ag);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_plus_n_n_dot_n_succeeds)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "+22.2");
            auto res2 = AgentDataTypes_ToString(global_bufferTemp, &ag);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res2);
            ASSERT_ARE_EQUAL(int, 0, strcmp(STRING_c_str(global_bufferTemp), "+22.2"));

            //cleanup
            Destroy_AGENT_DATA_TYPE(&ag);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_n_dot_n_n_succeeds)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "3.33");
            auto res2 = AgentDataTypes_ToString(global_bufferTemp, &ag);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res2);
            ASSERT_ARE_EQUAL(int, 0, strcmp(STRING_c_str(global_bufferTemp), "3.33"));

            //cleanup
            Destroy_AGENT_DATA_TYPE(&ag);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_minus_n_dot_n_n_succeeds)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "-4.44");
            auto res2 = AgentDataTypes_ToString(global_bufferTemp, &ag);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res2);
            ASSERT_ARE_EQUAL(int, 0, strcmp(STRING_c_str(global_bufferTemp), "-4.44"));

            //cleanup
            Destroy_AGENT_DATA_TYPE(&ag);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_plus_n_dot_n_n_succeeds)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "+5.55");
            auto res2 = AgentDataTypes_ToString(global_bufferTemp, &ag);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res2);
            ASSERT_ARE_EQUAL(int, 0, strcmp(STRING_c_str(global_bufferTemp), "+5.55"));

            //cleanup
            Destroy_AGENT_DATA_TYPE(&ag);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_n_n_dot_n_n_succeeds)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "66.66");
            auto res2 = AgentDataTypes_ToString(global_bufferTemp, &ag);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res2);
            ASSERT_ARE_EQUAL(int, 0, strcmp(STRING_c_str(global_bufferTemp), "66.66"));

            //cleanup
            Destroy_AGENT_DATA_TYPE(&ag);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_minus_n_n_dot_n_n_succeeds)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "-77.77");
            auto res2 = AgentDataTypes_ToString(global_bufferTemp, &ag);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res2);
            ASSERT_ARE_EQUAL(int, 0, strcmp(STRING_c_str(global_bufferTemp), "-77.77"));

            //cleanup
            Destroy_AGENT_DATA_TYPE(&ag);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_plus_n_n_dot_n_n_succeeds)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "+88.88");
            auto res2 = AgentDataTypes_ToString(global_bufferTemp, &ag);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res2);
            ASSERT_ARE_EQUAL(int, 0, strcmp(STRING_c_str(global_bufferTemp), "+88.88"));

            //cleanup
            Destroy_AGENT_DATA_TYPE(&ag);
        }


        /*all the below tests have to fail*/
        TEST_FUNCTION(Create_EDM_DECIMAL_From_a_n_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "a1");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_a_minus_n_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "b-2");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_a_plus_n_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "c+3");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_a_n_n_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "d44");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_a_minus_n_n_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "e-55");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_a_plus_n_n_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "f+66");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_a_n_dot_n_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "g7.7");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_a_minus_n_dot_n_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "h-8.8");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_a_plus_n_dot_n_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "i+9.9");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_a_n_n_dot_n_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "j00.0");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_a_minus_n_n_dot_n_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "k-11.1");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_a_plus_n_n_dot_n_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "l+22.2");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_a_n_dot_n_n_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "m3.33");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_a_minus_n_dot_n_n_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "n-4.44");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_a_plus_n_dot_n_n_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "o+5.55");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_a_n_n_dot_n_n_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "p66.66");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_a_minus_n_n_dot_n_n_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "q-77.77");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_a_plus_n_n_dot_n_n_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "r+88.88");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_n_a_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "1a");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_minus_n_a_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "-2b");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_plus_n_a_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "+3c");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_n_n_a_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "44d");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_minus_n_n_a_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "-55e");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_plus_n_n_a_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "+66f");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_n_dot_n_a_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "7.7g");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_minus_n_dot_n_a_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "-8.8h");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_plus_n_dot_n_a_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "+9.9i");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_n_n_dot_n_a_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "00.0j");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_minus_n_n_dot_n_a_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "-11.1k");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_plus_n_n_dot_n_a_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "+22.2l");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_n_dot_n_n_a_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "3.33m");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_minus_n_dot_n_n_a_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "-4.44n");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_plus_n_dot_n_n_a_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "+5.55o");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_n_n_dot_n_n_a_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "66.66p");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_minus_n_n_dot_n_n_a_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "-77.77q");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_plus_n_n_dot_n_n_a_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "+88.88r");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_p_minus_n_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "+-2");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_p_plus_n_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "++3");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_p_minus_n_n_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "+-55");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_p_plus_n_n_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "++66");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_p_minus_n_dot_n_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "+-8.8");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_p_plus_n_dot_n_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "++9.9");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_p_minus_n_n_dot_n_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "+-11.1");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_p_plus_n_n_dot_n_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "++22.2");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_p_minus_n_dot_n_n_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "+-4.44");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_p_plus_n_dot_n_n_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "++5.55");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_p_minus_n_n_dot_n_n_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "+-77.77");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_p_plus_n_n_dot_n_n_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "++88.88");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_n_p_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "1+");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_minus_n_p_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "-2+");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_plus_n_p_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "+3+");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_n_n_p_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "44+");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_minus_n_n_p_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "-55p");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_plus_n_n_p_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "+66+");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_n_dot_n_p_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "7.7+");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_minus_n_dot_n_p_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "-8.8+");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_plus_n_dot_n_p_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "+9.9+");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_n_n_dot_n_p_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "00.0+");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_minus_n_n_dot_n_p_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "-11.1+");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_plus_n_n_dot_n_p_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "+22.2+");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_n_dot_n_n_p_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "3.33+");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_minus_n_dot_n_n_p_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "-4.44+");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_plus_n_dot_n_n_p_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "+5.55+");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_n_n_dot_n_n_p_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "66.66+");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_a_minus_n_n_dot_n_n_p_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "-77.77+");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_plus_n_n_dot_n_n_p_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "+88.88+");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_m_minus_n_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "--2");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_m_plus_n_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "-+3");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_m_minus_n_n_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "--55");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_m_plus_n_n_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "-+66");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_m_minus_n_dot_n_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "--8.8");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_m_plus_n_dot_n_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "-+9.9");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_m_minus_n_n_dot_n_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "--11.1");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_m_plus_n_n_dot_n_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "-+22.2");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_m_minus_n_dot_n_n_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "--4.44");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_m_plus_n_dot_n_n_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "-+5.55");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_m_minus_n_n_dot_n_n_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "--77.77");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_m_plus_n_n_dot_n_n_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "-+88.88");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_n_m_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "1-");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_minus_n_m_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "-2-");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_plus_n_m_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "+3-");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_n_n_m_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "44-");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_minus_n_n_m_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "-55-");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_plus_n_n_m_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "+66-");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_n_dot_n_m_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "7.7-");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_minus_n_dot_n_m_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "-8.8-");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_plus_n_dot_n_m_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "+9.9-");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_n_n_dot_n_m_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "00.0-");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_minus_n_n_dot_n_m_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "-11.1-");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_plus_n_n_dot_n_m_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "+22.2-");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_n_dot_n_n_m_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "3.33-");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_minus_n_dot_n_n_m_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "-4.44-");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_plus_n_dot_n_n_m_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "+5.55-");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_n_n_dot_n_n_m_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "66.66-");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_minus_n_n_dot_n_n_m_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "-77.77-");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_plus_n_n_dot_n_n_m_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "+88.88-");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_d_n_fails)
        {
                ///arrange
                AGENT_DATA_TYPE ag;

                ///act
                auto res = Create_EDM_DECIMAL_from_charz(&ag, ".1");

                ///assert
                ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_d_minus_n_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "d-2");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_d_plus_n_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, ".+3");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_d_n_n_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, ".44");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_d_minus_n_n_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, ".-55");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_d_plus_n_n_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, ".+66");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_d_n_dot_n_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, ".7.7");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_d_minus_n_dot_n_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, ".-8.8");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_d_plus_n_dot_n_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, ".+9.9");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_d_n_n_dot_n_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, ".00.0");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_d_minus_n_n_dot_n_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, ".-11.1");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_d_plus_n_n_dot_n_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, ".+22.2");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_d_n_dot_n_n_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, ".3.33");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_d_minus_n_dot_n_n_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, ".-4.44");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_d_plus_n_dot_n_n_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, ".+5.55");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_d_n_n_dot_n_n_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, ".66.66");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_d_minus_n_n_dot_n_n_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, ".-77.77");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_d_plus_n_n_dot_n_n_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, ".+88.88");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }


        TEST_FUNCTION(Create_EDM_DECIMAL_From_n_d_fails)
        {
                ///arrange
                AGENT_DATA_TYPE ag;

                ///act
                auto res = Create_EDM_DECIMAL_from_charz(&ag, "1.");

                ///assert
                ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_minus_n_d_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "-2.");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_plus_n_d_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "+3.");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_n_n_d_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "44.");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_minus_n_n_d_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "-55.");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_plus_n_n_d_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "+66.");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_n_dot_n_d_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "7.7.");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_minus_n_dot_n_d_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "-8.8.");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_plus_n_dot_n_d_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "+9.9.");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_n_n_dot_n_d_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "00.0.");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_minus_n_n_dot_n_d_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "-11.1.");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_plus_n_n_dot_n_d_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "+22.2.");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_n_dot_n_n_d_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "3.33.");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_minus_n_dot_n_n_d_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "-4.44.");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_plus_n_dot_n_n_d_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "+5.55.");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_n_n_dot_n_n_d_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "66.66.");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_minus_n_n_dot_n_n_d_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "-77.77.");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_From_plus_n_n_dot_n_n_d_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "+88.88.");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        TEST_FUNCTION(AgentDataType_ToString_EDM_DECIMAL_fails_when_STRING_concat_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;
            (void)Create_EDM_DECIMAL_from_charz(&ag, "3.14");

            whenShallSTRING_concat_with_STRING_fail = 1;
            EXPECTED_CALL((*mocks), STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .SetReturn(1);

            ///act
            auto res = AgentDataTypes_ToString(global_bufferTemp, &ag);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_ERROR, res);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&ag);
        }

        TEST_FUNCTION(Create_EDM_DECIMAL_fails_when_STRING_cosntruc_n_fails)
        {
            ///arrange
            AGENT_DATA_TYPE ag;
            whenShallSTRING_construct_fail = 1;

            ///act
            auto res = Create_EDM_DECIMAL_from_charz(&ag, "3.14");

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_ERROR, res);
        }

        TEST_FUNCTION(Create_AGENT_DATA_TYPE_From_AGENT_DATA_TYPE_From_EDM_DECIMAL_succeeds)
        {
            ///arrange
            AGENT_DATA_TYPE src, dest;
            (void)Create_EDM_DECIMAL_from_charz(&src, "+3.67");


            ///act
            auto res = Create_AGENT_DATA_TYPE_from_AGENT_DATA_TYPE(&dest, &src);
            auto res2 = AgentDataTypes_ToString(global_bufferTemp,  &dest);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res2);
            ASSERT_ARE_EQUAL(int, 0, strcmp(STRING_c_str(global_bufferTemp), "+3.67"));

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&src);
            Destroy_AGENT_DATA_TYPE(&dest);
        }

        TEST_FUNCTION(Create_AGENT_DATA_TYPE_From_AGENT_DATA_TYPE_From_EDM_DECIMAL_fails_when_strcpy_s_fails)
        {
            ///arrange
            AGENT_DATA_TYPE src, dest;
            (void)Create_EDM_DECIMAL_from_charz(&src, "+3.67");
            
            whenShallSTRING_clone_fail = 1;
            STRICT_EXPECTED_CALL((*mocks), STRING_clone(IGNORED_PTR_ARG))
                .IgnoreArgument(1);

            ///act
            auto res = Create_AGENT_DATA_TYPE_from_AGENT_DATA_TYPE(&dest, &src);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_ERROR, res);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&src);
        }


        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_013:[ All the functions shall check their parameters for validity. When an invalid parameter is detected, the value AGENT_DATA_TYPES_INVALID_ARG shall be returned ].*/
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_date_with_NULL_agentData_fails)
        {
            ///arrange

            ///act
            auto res = Create_AGENT_DATA_TYPE_from_date(NULL, TEST_YEAR_1, TEST_MONTH_1, TEST_MDAY_1);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_070:[ If year-month-date does not indicate a valid day (for example 31 Feb 2013), then AGENT_DATA_TYPES_INVALID_ARG shall be returned.]*/
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_date_with_0_day_fails)
        {
            ///arrange
            AGENT_DATA_TYPE agentData;

            ///act
            auto res = Create_AGENT_DATA_TYPE_from_date(&agentData, TEST_YEAR_1, TEST_MONTH_1, 0);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_070:[ If year-month-date does not indicate a valid day (for example 31 Feb 2013), then AGENT_DATA_TYPES_INVALID_ARG shall be returned.]*/
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_date_with_13_month_fails)
        {
            ///arrange
            AGENT_DATA_TYPE agentData;

            ///act
            auto res = Create_AGENT_DATA_TYPE_from_date(&agentData, TEST_YEAR_1, 13, 1);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_070:[ If year-month-date does not indicate a valid day (for example 31 Feb 2013), then AGENT_DATA_TYPES_INVALID_ARG shall be returned.]*/
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_date_with_32_day_for_January_fails)
        {
            ///arrange
            AGENT_DATA_TYPE agentData;

            ///act
            auto res = Create_AGENT_DATA_TYPE_from_date(&agentData, TEST_YEAR_1, 1, 32);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_069:[ Creates an AGENT_DATA_TYPE containing an EDM_DATE from a year, a month and a day of the month.]*/
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_date_with_31_day_for_January_succeeds)
        {
            ///arrange
            AGENT_DATA_TYPE agentData;

            ///act
            auto res = Create_AGENT_DATA_TYPE_from_date(&agentData, TEST_YEAR_1, 1, 31);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);

            ///cleanup 
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_013:[ All the functions shall check their parameters for validity. When an invalid parameter is detected, the value AGENT_DATA_TYPES_INVALID_ARG shall be returned ].*/
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_date_with_29_day_for_February_non_leap_year_fails)
        {
            ///arrange
            AGENT_DATA_TYPE agentData;

            ///act
            auto res = Create_AGENT_DATA_TYPE_from_date(&agentData, 2011, 2, 29);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_069:[ Creates an AGENT_DATA_TYPE containing an EDM_DATE from a year, a month and a day of the month.]*/
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_date_with_28_day_for_February_non_leap_year_succeeds)
        {
            ///arrange
            AGENT_DATA_TYPE agentData;

            ///act
            auto res = Create_AGENT_DATA_TYPE_from_date(&agentData, 2011, 2, 28);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);

            ///cleanup 
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_013:[ All the functions shall check their parameters for validity. When an invalid parameter is detected, the value AGENT_DATA_TYPES_INVALID_ARG shall be returned ].*/
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_date_with_30_day_for_February_leap_year_fails)
        {
            ///arrange
            AGENT_DATA_TYPE agentData;

            ///act
            auto res = Create_AGENT_DATA_TYPE_from_date(&agentData, 2012, 2, 30);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_069:[ Creates an AGENT_DATA_TYPE containing an EDM_DATE from a year, a month and a day of the month.]*/
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_date_with_29_day_for_February_leap_year_succeeds)
        {
            ///arrange
            AGENT_DATA_TYPE agentData;

            ///act
            auto res = Create_AGENT_DATA_TYPE_from_date(&agentData, 2012, 2, 29);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);

            ///cleanup 
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_013:[ All the functions shall check their parameters for validity. When an invalid parameter is detected, the value AGENT_DATA_TYPES_INVALID_ARG shall be returned ].*/
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_date_with_29_day_for_February_leap_non_year_fails_2)
        {
            ///arrange
            AGENT_DATA_TYPE agentData;

            ///act
            auto res = Create_AGENT_DATA_TYPE_from_date(&agentData, 2100, 2, 29);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_069:[ Creates an AGENT_DATA_TYPE containing an EDM_DATE from a year, a month and a day of the month.]*/
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_date_with_28_day_for_February_non_leap_year_succeeds_2)
        {
            ///arrange
            AGENT_DATA_TYPE agentData;

            ///act
            auto res = Create_AGENT_DATA_TYPE_from_date(&agentData, 2100, 2, 28);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);

            ///cleanup 
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_013:[ All the functions shall check their parameters for validity. When an invalid parameter is detected, the value AGENT_DATA_TYPES_INVALID_ARG shall be returned ].*/
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_date_with_30_day_for_February_leap_year_fails_3)
        {
            ///arrange
            AGENT_DATA_TYPE agentData;

            ///act
            auto res = Create_AGENT_DATA_TYPE_from_date(&agentData, 2000, 2, 30);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_069:[ Creates an AGENT_DATA_TYPE containing an EDM_DATE from a year, a month and a day of the month.]*/
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_date_with_28_day_for_February_leap_year_succeeds_3)
        {
            ///arrange
            AGENT_DATA_TYPE agentData;

            ///act
            auto res = Create_AGENT_DATA_TYPE_from_date(&agentData, 2000, 2, 29);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);

            ///cleanup 
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_013:[ All the functions shall check their parameters for validity. When an invalid parameter is detected, the value AGENT_DATA_TYPES_INVALID_ARG shall be returned ].*/
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_date_with_32_day_for_March_fails)
        {
            ///arrange
            AGENT_DATA_TYPE agentData;

            ///act
            auto res = Create_AGENT_DATA_TYPE_from_date(&agentData, TEST_YEAR_1, 3, 32);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_069:[ Creates an AGENT_DATA_TYPE containing an EDM_DATE from a year, a month and a day of the month.]*/
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_date_with_31_day_for_March_succeeds)
        {
            ///arrange
            AGENT_DATA_TYPE agentData;

            ///act
            auto res = Create_AGENT_DATA_TYPE_from_date(&agentData, TEST_YEAR_1, 3, 31);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);

            ///cleanup 
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_013:[ All the functions shall check their parameters for validity. When an invalid parameter is detected, the value AGENT_DATA_TYPES_INVALID_ARG shall be returned ].*/
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_date_with_31_day_for_April_fails)
        {
            ///arrange
            AGENT_DATA_TYPE agentData;

            ///act
            auto res = Create_AGENT_DATA_TYPE_from_date(&agentData, TEST_YEAR_1, 4, 31);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_069:[ Creates an AGENT_DATA_TYPE containing an EDM_DATE from a year, a month and a day of the month.]*/
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_date_with_31_day_for_April_succeeds)
        {
            ///arrange
            AGENT_DATA_TYPE agentData;

            ///act
            auto res = Create_AGENT_DATA_TYPE_from_date(&agentData, TEST_YEAR_1, 4, 30);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);

            ///cleanup 
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_013:[ All the functions shall check their parameters for validity. When an invalid parameter is detected, the value AGENT_DATA_TYPES_INVALID_ARG shall be returned ].*/
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_date_with_32_day_for_May_fails)
        {
            ///arrange
            AGENT_DATA_TYPE agentData;

            ///act
            auto res = Create_AGENT_DATA_TYPE_from_date(&agentData, TEST_YEAR_1, 5, 32);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_069:[ Creates an AGENT_DATA_TYPE containing an EDM_DATE from a year, a month and a day of the month.]*/
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_date_with_31_day_for_May_succeeds)
        {
            ///arrange
            AGENT_DATA_TYPE agentData;

            ///act
            auto res = Create_AGENT_DATA_TYPE_from_date(&agentData, TEST_YEAR_1, 5, 31);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);

            ///cleanup 
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_013:[ All the functions shall check their parameters for validity. When an invalid parameter is detected, the value AGENT_DATA_TYPES_INVALID_ARG shall be returned ].*/
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_date_with_31_day_for_June_fails)
        {
            ///arrange
            AGENT_DATA_TYPE agentData;

            ///act
            auto res = Create_AGENT_DATA_TYPE_from_date(&agentData, TEST_YEAR_1, 6, 31);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_069:[ Creates an AGENT_DATA_TYPE containing an EDM_DATE from a year, a month and a day of the month.]*/
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_date_with_30_day_for_June_succeeds)
        {
            ///arrange
            AGENT_DATA_TYPE agentData;

            ///act
            auto res = Create_AGENT_DATA_TYPE_from_date(&agentData, TEST_YEAR_1, 6, 30);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);

            ///cleanup 
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_013:[ All the functions shall check their parameters for validity. When an invalid parameter is detected, the value AGENT_DATA_TYPES_INVALID_ARG shall be returned ].*/
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_date_with_32_day_for_July_fails)
        {
            ///arrange
            AGENT_DATA_TYPE agentData;

            ///act
            auto res = Create_AGENT_DATA_TYPE_from_date(&agentData, TEST_YEAR_1, 7, 32);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_069:[ Creates an AGENT_DATA_TYPE containing an EDM_DATE from a year, a month and a day of the month.]*/
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_date_with_31_day_for_July_succeeds)
        {
            ///arrange
            AGENT_DATA_TYPE agentData;

            ///act
            auto res = Create_AGENT_DATA_TYPE_from_date(&agentData, TEST_YEAR_1, 7, 31);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);

            ///cleanup 
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_013:[ All the functions shall check their parameters for validity. When an invalid parameter is detected, the value AGENT_DATA_TYPES_INVALID_ARG shall be returned ].*/
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_date_with_32_day_for_August_fails)
        {
            ///arrange
            AGENT_DATA_TYPE agentData;

            ///act
            auto res = Create_AGENT_DATA_TYPE_from_date(&agentData, TEST_YEAR_1, 8, 32);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_069:[ Creates an AGENT_DATA_TYPE containing an EDM_DATE from a year, a month and a day of the month.]*/
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_date_with_31_day_for_August_succeeds)
        {
            ///arrange
            AGENT_DATA_TYPE agentData;

            ///act
            auto res = Create_AGENT_DATA_TYPE_from_date(&agentData, TEST_YEAR_1, 8, 31);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);

            ///cleanup 
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_013:[ All the functions shall check their parameters for validity. When an invalid parameter is detected, the value AGENT_DATA_TYPES_INVALID_ARG shall be returned ].*/
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_date_with_31_day_for_September_fails)
        {
            ///arrange
            AGENT_DATA_TYPE agentData;

            ///act
            auto res = Create_AGENT_DATA_TYPE_from_date(&agentData, TEST_YEAR_1, 9, 31);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_069:[ Creates an AGENT_DATA_TYPE containing an EDM_DATE from a year, a month and a day of the month.]*/
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_date_with_30_day_for_September_succeeds)
        {
            ///arrange
            AGENT_DATA_TYPE agentData;

            ///act
            auto res = Create_AGENT_DATA_TYPE_from_date(&agentData, TEST_YEAR_1, 9, 30);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);

            ///cleanup 
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_013:[ All the functions shall check their parameters for validity. When an invalid parameter is detected, the value AGENT_DATA_TYPES_INVALID_ARG shall be returned ].*/
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_date_with_32_day_for_October_fails)
        {
            ///arrange
            AGENT_DATA_TYPE agentData;

            ///act
            auto res = Create_AGENT_DATA_TYPE_from_date(&agentData, TEST_YEAR_1, 10, 32);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_069:[ Creates an AGENT_DATA_TYPE containing an EDM_DATE from a year, a month and a day of the month.]*/
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_date_with_31_day_for_October_succeeds)
        {
            ///arrange
            AGENT_DATA_TYPE agentData;

            ///act
            auto res = Create_AGENT_DATA_TYPE_from_date(&agentData, TEST_YEAR_1, 10, 31);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);

            ///cleanup 
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_013:[ All the functions shall check their parameters for validity. When an invalid parameter is detected, the value AGENT_DATA_TYPES_INVALID_ARG shall be returned ].*/
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_date_with_31_day_for_November_fails)
        {
            ///arrange
            AGENT_DATA_TYPE agentData;

            ///act
            auto res = Create_AGENT_DATA_TYPE_from_date(&agentData, TEST_YEAR_1, 11, 31);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_069:[ Creates an AGENT_DATA_TYPE containing an EDM_DATE from a year, a month and a day of the month.]*/
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_date_with_30_day_for_November_succeeds)
        {
            ///arrange
            AGENT_DATA_TYPE agentData;

            ///act
            auto res = Create_AGENT_DATA_TYPE_from_date(&agentData, TEST_YEAR_1, 11, 30);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);

            ///cleanup 
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_013:[ All the functions shall check their parameters for validity. When an invalid parameter is detected, the value AGENT_DATA_TYPES_INVALID_ARG shall be returned ].*/
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_date_with_32_day_for_December_fails)
        {
            ///arrange
            AGENT_DATA_TYPE agentData;

            ///act
            auto res = Create_AGENT_DATA_TYPE_from_date(&agentData, TEST_YEAR_1, 12, 32);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_069:[ Creates an AGENT_DATA_TYPE containing an EDM_DATE from a year, a month and a day of the month.]*/
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_date_with_31_day_for_December_succeeds)
        {
            ///arrange
            AGENT_DATA_TYPE agentData;

            ///act
            auto res = Create_AGENT_DATA_TYPE_from_date(&agentData, TEST_YEAR_1, 12, 31);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);

            ///cleanup 
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_013:[ All the functions shall check their parameters for validity. When an invalid parameter is detected, the value AGENT_DATA_TYPES_INVALID_ARG shall be returned ].*/
        /*must fail because ODATA-ABNF has only 4 digits for the year*/
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_date_with_year_minus_10000_fails)
        {
            ///arrange
            AGENT_DATA_TYPE agentData;

            ///act
            auto res = Create_AGENT_DATA_TYPE_from_date(&agentData, -10000, 12, 31);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_013:[ All the functions shall check their parameters for validity. When an invalid parameter is detected, the value AGENT_DATA_TYPES_INVALID_ARG shall be returned ].*/
        /*must fail because ODATA-ABNF has only 4 digits for the year*/
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_date_with_year_10000_fails)
        {
            ///arrange
            AGENT_DATA_TYPE agentData;

            ///act
            auto res = Create_AGENT_DATA_TYPE_from_date(&agentData, 10000, 1, 1);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, res);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_052:[ Destroy_AGENT_DATA_TYPE return AGENT_DATA_TYPE_OK when the request to destroy the agent data has been successful, in all the other cases returns a code different than AGENT_DATA_TYPE_OK.]*/
        TEST_FUNCTION(Destroy_AGENT_DATA_TYPE_from_date_succeeds)
        {
            ///arrange
            AGENT_DATA_TYPE agentData;
            (void)Create_AGENT_DATA_TYPE_from_date(&agentData, TEST_YEAR_1, TEST_MONTH_1, TEST_MDAY_1);

            ///act
            Destroy_AGENT_DATA_TYPE(&agentData);

            ///assert
            ///no exceptions, no runtimeerrors.
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_068:[ EDM_DATE: dateValue = year "-" month "-" day.]*/
        TEST_FUNCTION(AgentDataTypes_ToString_EDM_DATE_with_2014_11_12_succeeds)
        {
            ///arrange
            AGENT_DATA_TYPE agentData;
            (void)Create_AGENT_DATA_TYPE_from_date(&agentData, TEST_YEAR_1, TEST_MONTH_1, TEST_MDAY_1);

            ///act
            auto res = AgentDataTypes_ToString(global_bufferTemp,  &agentData);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);
            ASSERT_ARE_EQUAL(char_ptr, "\"2014-11-12\"", STRING_c_str(global_bufferTemp));

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_068:[ EDM_DATE: dateValue = year "-" month "-" day.]*/
        TEST_FUNCTION(AgentDataTypes_ToString_EDM_DATE_with_2014_02_09_succeeds)
        {
            ///arrange
            AGENT_DATA_TYPE agentData;
            (void)Create_AGENT_DATA_TYPE_from_date(&agentData, TEST_YEAR_1, 2, 9);

            ///act
            auto res = AgentDataTypes_ToString(global_bufferTemp,  &agentData);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);
            ASSERT_ARE_EQUAL(char_ptr, "\"2014-02-09\"", STRING_c_str(global_bufferTemp));

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_068:[ EDM_DATE: dateValue = year "-" month "-" day.]*/
        TEST_FUNCTION(AgentDataTypes_ToString_EDM_DATE_with_4_02_09_succeeds)
        {
            ///arrange
            AGENT_DATA_TYPE agentData;
            (void)Create_AGENT_DATA_TYPE_from_date(&agentData, 4, 2, 9); /*4 is <<was>> a year too... */

            ///act
            auto res = AgentDataTypes_ToString(global_bufferTemp,  &agentData);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);
            ASSERT_ARE_EQUAL(char_ptr, "\"0004-02-09\"", STRING_c_str(global_bufferTemp));

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_068:[ EDM_DATE: dateValue = year "-" month "-" day.]*/
        TEST_FUNCTION(AgentDataTypes_ToString_EDM_DATE_with_minus_4_02_09_succeeds)
        {
            ///arrange
            AGENT_DATA_TYPE agentData;
            (void)Create_AGENT_DATA_TYPE_from_date(&agentData, -4, 2, 9); /*-4 is <<was>> a year too... a long time ago*/

            ///act
            auto res = AgentDataTypes_ToString(global_bufferTemp,  &agentData);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);
            ASSERT_ARE_EQUAL(char_ptr, "\"-0004-02-09\"", STRING_c_str(global_bufferTemp));

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_068:[ EDM_DATE: dateValue = year "-" month "-" day.]*/
        TEST_FUNCTION(AgentDataTypes_ToString_EDM_DATE_with_minus_3200_02_09_succeeds)
        {
            ///arrange
            AGENT_DATA_TYPE agentData;
            (void)Create_AGENT_DATA_TYPE_from_date(&agentData, -3200, 2, 9); /*the two kingdoms of Egypt were united in c 3200BC*/

            ///act
            auto res = AgentDataTypes_ToString(global_bufferTemp,  &agentData);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res);
            ASSERT_ARE_EQUAL(char_ptr, "\"-3200-02-09\"", STRING_c_str(global_bufferTemp));

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /* CreateAgentDataType_From_String */

        /* The following 2 requirements are tested by the virtue of all the tests below */
        /* Codes_SRS_AGENT_TYPE_SYSTEM_99_071:[ CreateAgentDataType_From_String shall create an AGENT_DATA_TYPE from a char* representation of the type indicated by type parameter.] */
        /* Codes_SRS_AGENT_TYPE_SYSTEM_99_072:[ The implementation for the transformation of the char* source into AGENT_DATA_TYPE shall be type specific.] */

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_073:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is NULL.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_With_NULL_source_Argument_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(NULL, EDM_BYTE_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_074:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if agentData is NULL.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_With_NULL_agentData_Argument_Fails)
        {
            // arrange
            const char* source = "42";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_BYTE_TYPE, NULL);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_075:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_NOT_IMPLEMENTED if type is not a supported type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_With_EDM_No_Type_Type_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "42";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_NO_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_NOT_IMPLEMENTED, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_086:[ EDM_STRING] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_STRING_1_char_long_Succeeds)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"4\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_STRING_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_STRING_TYPE, agentData.type);
            ASSERT_ARE_EQUAL(size_t, (size_t)1, agentData.value.edmString.length);
            ASSERT_ARE_EQUAL(char_ptr, "4", agentData.value.edmString.chars);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_086:[ EDM_STRING] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_STRING_Empty_String_Succeeds)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_STRING_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_STRING_TYPE, agentData.type);
            ASSERT_ARE_EQUAL(size_t, (size_t)0, agentData.value.edmString.length);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_086:[ EDM_STRING] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_STRING_1_Char_Long_No_Quotes_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "a";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_STRING_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_086:[ EDM_STRING] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_STRING_With_Only_Starting_Quote_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_STRING_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_086:[ EDM_STRING] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_STRING_With_Only_Starting_Quote_And_Another_Char_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"a";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_STRING_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_086:[ EDM_STRING] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_STRING_Without_Endind_Quote_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"abcd";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_STRING_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_01_004: [EDM_STRING_NO_QUOTES] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_STRING_NO_QUOTES_Empty_String_Succeeds)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_STRING_NO_QUOTES_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_STRING_NO_QUOTES_TYPE, agentData.type);
            ASSERT_ARE_EQUAL(size_t, (size_t)0, agentData.value.edmString.length);
            ASSERT_ARE_EQUAL(char_ptr, "", agentData.value.edmString.chars);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_01_004: [EDM_STRING_NO_QUOTES] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_STRING_NO_QUOTES_1_char_string_Succeeds)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "a";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_STRING_NO_QUOTES_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_STRING_NO_QUOTES_TYPE, agentData.type);
            ASSERT_ARE_EQUAL(size_t, (size_t)strlen(source), agentData.value.edmString.length);
            ASSERT_ARE_EQUAL(char_ptr, source, agentData.value.edmString.chars);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_01_004: [EDM_STRING_NO_QUOTES] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_STRING_NO_QUOTES_longer_string__with_quotes_and_brances_Succeeds)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "[{\"someLongerStringa\"}]";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_STRING_NO_QUOTES_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_STRING_NO_QUOTES_TYPE, agentData.type);
            ASSERT_ARE_EQUAL(size_t, (size_t)strlen(source), agentData.value.edmString.length);
            ASSERT_ARE_EQUAL(char_ptr, source, agentData.value.edmString.chars);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_076:[ EDM_BOOLEAN_TYPE] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_Boolean_true_Succeeds)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "true";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_BOOLEAN_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_BOOLEAN_TYPE, agentData.type);
            ASSERT_ARE_EQUAL(EDM_BOOLEANS, EDM_TRUE, agentData.value.edmBoolean.value);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_076:[ EDM_BOOLEAN_TYPE] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_Boolean_false_Succeeds)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "false";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_BOOLEAN_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_BOOLEAN_TYPE, agentData.type);
            ASSERT_ARE_EQUAL(EDM_BOOLEANS, EDM_FALSE, agentData.value.edmBoolean.value);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_076:[ EDM_BOOLEAN_TYPE] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_Boolean_SomeOtherValue_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "42";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_BOOLEAN_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_084:[ EDM_SBYTE] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_SBYTE_Empty_String_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_SBYTE_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_084:[ EDM_SBYTE] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_SBYTE_Some_Bad_Text_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "BlahBlah";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_SBYTE_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_084:[ EDM_SBYTE] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_SBYTE_Minus_128_Succeeds)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "-128";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_SBYTE_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_SBYTE_TYPE, agentData.type);
            ASSERT_ARE_EQUAL(int, (int)-128, (int)agentData.value.edmSbyte.value);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_084:[ EDM_SBYTE] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_SBYTE_127_Succeeds)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "127";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_SBYTE_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_SBYTE_TYPE, agentData.type);
            ASSERT_ARE_EQUAL(int, (int)127, (int)agentData.value.edmSbyte.value);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_084:[ EDM_SBYTE] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_SBYTE_Minus_129_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "-129";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_SBYTE_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_084:[ EDM_SBYTE] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_SBYTE_128_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "128";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_SBYTE_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_084:[ EDM_SBYTE] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_SBYTE_Plus_127_Succeeds)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "+127";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_SBYTE_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_SBYTE_TYPE, agentData.type);
            ASSERT_ARE_EQUAL(int, (int)127, (int)agentData.value.edmSbyte.value);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_077:[ EDM_BYTE] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_BYTE_Empty_String_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_BYTE_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_077:[ EDM_BYTE] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_BYTE_Some_Bad_Text_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "blahblah";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_BYTE_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_077:[ EDM_BYTE] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_BYTE_Zero_Succeeds)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "0";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_BYTE_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_BYTE_TYPE, agentData.type);
            ASSERT_ARE_EQUAL(int, (int)0, (int)agentData.value.edmByte.value);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_077:[ EDM_BYTE] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_BYTE_255_Succeeds)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "255";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_BYTE_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_BYTE_TYPE, agentData.type);
            ASSERT_ARE_EQUAL(int, (int)255, (int)agentData.value.edmByte.value);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_077:[ EDM_BYTE] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_BYTE_Minus_1_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "-1";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_BYTE_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_077:[ EDM_BYTE] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_BYTE_256_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "256";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_BYTE_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_077:[ EDM_BYTE] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_BYTE_Plus_255_Succeeds)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "+255";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_BYTE_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_BYTE_TYPE, agentData.type);
            ASSERT_ARE_EQUAL(int, (int)255, (int)agentData.value.edmByte.value);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_081:[ EDM_INT16] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_INT16_Empty_String_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_INT16_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_081:[ EDM_INT16] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_INT16_Some_Bad_Text_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "blahblah";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_INT16_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_081:[ EDM_INT16] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_INT16_Minus_32768_Succeeds)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "-32768";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_INT16_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_INT16_TYPE, agentData.type);
            ASSERT_ARE_EQUAL(int, (int)-32768, (int)agentData.value.edmInt16.value);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_081:[ EDM_INT16] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_INT16_32767_Succeeds)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "32767";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_INT16_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_INT16_TYPE, agentData.type);
            ASSERT_ARE_EQUAL(int, (int)32767, (int)agentData.value.edmInt16.value);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_081:[ EDM_INT16] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_INT16_Minus_32769_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "-32769";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_INT16_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_081:[ EDM_INT16] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_INT16_32768_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "32768";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_INT16_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_081:[ EDM_INT16] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_INT16_Plus_32767_Succeeds)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "+32767";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_INT16_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_INT16_TYPE, agentData.type);
            ASSERT_ARE_EQUAL(int, (int)32767, (int)agentData.value.edmInt16.value);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_082:[ EDM_INT32] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_INT32_Empty_String_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_INT32_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_082:[ EDM_INT32] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_INT32_Some_Bad_Text_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "blahblah";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_INT32_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_082:[ EDM_INT32] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_INT32_Minus_2147483648_Succeeds)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "-2147483648";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_INT32_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_INT32_TYPE, agentData.type);
            ASSERT_ARE_EQUAL(long, (long)-2147483648LL, (long)agentData.value.edmInt32.value);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_082:[ EDM_INT32] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_INT32_2147483647_Succeeds)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "2147483647";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_INT32_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_INT32_TYPE, agentData.type);
            ASSERT_ARE_EQUAL(int, (int)2147483647, (int)agentData.value.edmInt32.value);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_082:[ EDM_INT32] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_INT32_Minus_2147483649_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "-2147483649";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_INT32_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_082:[ EDM_INT32] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_INT32_2147483648_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "2147483648";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_INT32_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_082:[ EDM_INT32] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_INT32_Plus_2147483647_Succeeds)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "+2147483647";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_INT32_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_INT32_TYPE, agentData.type);
            ASSERT_ARE_EQUAL(int, (int)2147483647, (int)agentData.value.edmInt32.value);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_083:[ EDM_INT64] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_INT64_Empty_String_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_INT64_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_083:[ EDM_INT64] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_INT64_Some_Bad_Text_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "blahblah";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_INT64_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_083:[ EDM_INT64] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_INT64_Minus_9223372036854775808_Succeeds)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "-9223372036854775808";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_INT64_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_INT64_TYPE, agentData.type);
            ASSERT_IS_TRUE(INTMAX_MIN == agentData.value.edmInt64.value);
        
            ///cleanup
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_083:[ EDM_INT64] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_INT64_9223372036854775807_Succeeds)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "9223372036854775807";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_INT64_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_INT64_TYPE, agentData.type);
            ASSERT_IS_TRUE(9223372036854775807LL == agentData.value.edmInt64.value);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_083:[ EDM_INT64] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_INT64_Minus_9223372036854775809_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "-9223372036854775809";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_INT64_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_083:[ EDM_INT64] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_INT64_9223372036854775808_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "9223372036854775808";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_INT64_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_083:[ EDM_INT64] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_INT64_Minus_10223372036854775808_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "-10223372036854775808";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_INT64_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_083:[ EDM_INT64] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_INT64_10223372036854775808_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "10223372036854775808";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_INT64_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_083:[ EDM_INT64] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_INT64_Plus_9223372036854775807_Succeeds)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "+9223372036854775807";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_INT64_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_INT64_TYPE, agentData.type);
            ASSERT_IS_TRUE(9223372036854775807LL == agentData.value.edmInt64.value);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_085:[ EDM_DATE] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_Empty_String_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_085:[ EDM_DATE] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_Some_Bad_Text_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"blahblah\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_085:[ EDM_DATE] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_Only_Starting_Quote_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_085:[ EDM_DATE] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_Missing_End_Quote_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1978-09-01";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_085:[ EDM_DATE] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_Good_Date_Succeeds)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1978-09-01\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_DATE_TYPE, agentData.type);
            ASSERT_ARE_EQUAL(int16_t, (int16_t)1978, agentData.value.edmDate.year);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)9, agentData.value.edmDate.month);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)1, agentData.value.edmDate.day);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_085:[ EDM_DATE] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_Zero_Day_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1978-09-00\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_085:[ EDM_DATE] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_Jan_Day_32_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1978-01-32\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_085:[ EDM_DATE] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_Feb_Day_29_Not_Leap_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1978-02-29\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_085:[ EDM_DATE] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_Feb_Day_30_Leap_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1904-02-30\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_085:[ EDM_DATE] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_Feb_Day_29_Not_Leap_Hundred_Divisible_Year_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1900-02-29\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_085:[ EDM_DATE] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_Feb_Day_30_Leap_4_Hundred_Divisible_Year_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1600-02-30\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_085:[ EDM_DATE] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_Mar_Day_32_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1600-03-32\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_085:[ EDM_DATE] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_Apr_Day_31_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1600-04-31\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_085:[ EDM_DATE] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_May_Day_32_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1600-05-32\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_085:[ EDM_DATE] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_Jun_Day_31_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1600-06-31\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_085:[ EDM_DATE] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_Jul_Day_32_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1600-07-32\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_085:[ EDM_DATE] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_Aug_Day_32_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1600-08-32\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_085:[ EDM_DATE] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_Sep_Day_31_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1600-09-31\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_085:[ EDM_DATE] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_Oct_Day_32_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1600-10-32\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_085:[ EDM_DATE] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_Nov_Day_31_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1600-11-31\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_085:[ EDM_DATE] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_Dec_Day_32_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1600-12-32\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_085:[ EDM_DATE] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_Negative_Day_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1600-01--1\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_085:[ EDM_DATE] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_Negative_Month_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1600--1-01\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_085:[ EDM_DATE] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_Year_Too_Short_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"300-01-01\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_085:[ EDM_DATE] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_Jan_Day_31_Succeeds)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1978-01-31\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_DATE_TYPE, agentData.type);
            ASSERT_ARE_EQUAL(int16_t, (int16_t)1978, agentData.value.edmDate.year);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)1, agentData.value.edmDate.month);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)31, agentData.value.edmDate.day);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_085:[ EDM_DATE] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_Feb_Day_28_Not_Leap_Succeeds)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1978-02-28\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_DATE_TYPE, agentData.type);
            ASSERT_ARE_EQUAL(int16_t, (int16_t)1978, agentData.value.edmDate.year);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)2, agentData.value.edmDate.month);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)28, agentData.value.edmDate.day);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_085:[ EDM_DATE] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_Feb_Day_29_Leap_Succeeds)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1904-02-29\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_DATE_TYPE, agentData.type);
            ASSERT_ARE_EQUAL(int16_t, (int16_t)1904, agentData.value.edmDate.year);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)2, agentData.value.edmDate.month);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)29, agentData.value.edmDate.day);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_085:[ EDM_DATE] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_Feb_Day_28_Not_Leap_Hundred_Divisible_Year_Succeeds)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1900-02-28\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_DATE_TYPE, agentData.type);
            ASSERT_ARE_EQUAL(int16_t, (int16_t)1900, agentData.value.edmDate.year);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)2, agentData.value.edmDate.month);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)28, agentData.value.edmDate.day);
        
            ///cleanup
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_085:[ EDM_DATE] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_Feb_Day_29_Leap_4_Hundred_Divisible_Year_Succeeds)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1600-02-29\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_DATE_TYPE, agentData.type);
            ASSERT_ARE_EQUAL(int16_t, (int16_t)1600, agentData.value.edmDate.year);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)2, agentData.value.edmDate.month);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)29, agentData.value.edmDate.day);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_085:[ EDM_DATE] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_Mar_Day_31_Succeeds)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1600-03-31\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_DATE_TYPE, agentData.type);
            ASSERT_ARE_EQUAL(int16_t, (int16_t)1600, agentData.value.edmDate.year);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)3, agentData.value.edmDate.month);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)31, agentData.value.edmDate.day);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_085:[ EDM_DATE] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_Apr_Day_30_Succeeds)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1600-04-30\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_DATE_TYPE, agentData.type);
            ASSERT_ARE_EQUAL(int16_t, (int16_t)1600, agentData.value.edmDate.year);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)4, agentData.value.edmDate.month);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)30, agentData.value.edmDate.day);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_085:[ EDM_DATE] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_May_Day_31_Succeeds)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1600-05-31\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_DATE_TYPE, agentData.type);
            ASSERT_ARE_EQUAL(int16_t, (int16_t)1600, agentData.value.edmDate.year);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)5, agentData.value.edmDate.month);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)31, agentData.value.edmDate.day);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_085:[ EDM_DATE] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_Jun_Day_30_Succeeds)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1600-06-30\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_DATE_TYPE, agentData.type);
            ASSERT_ARE_EQUAL(int16_t, (int16_t)1600, agentData.value.edmDate.year);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)6, agentData.value.edmDate.month);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)30, agentData.value.edmDate.day);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_085:[ EDM_DATE] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_Jul_Day_31_Succeeds)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1600-07-31\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_DATE_TYPE, agentData.type);
            ASSERT_ARE_EQUAL(int16_t, (int16_t)1600, agentData.value.edmDate.year);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)7, agentData.value.edmDate.month);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)31, agentData.value.edmDate.day);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_085:[ EDM_DATE] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_Aug_Day_31_Succeeds)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1600-08-31\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_DATE_TYPE, agentData.type);
            ASSERT_ARE_EQUAL(int16_t, (int16_t)1600, agentData.value.edmDate.year);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)8, agentData.value.edmDate.month);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)31, agentData.value.edmDate.day);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_085:[ EDM_DATE] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_Sep_Day_30_Succeeds)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1600-09-30\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_DATE_TYPE, agentData.type);
            ASSERT_ARE_EQUAL(int16_t, (int16_t)1600, agentData.value.edmDate.year);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)9, agentData.value.edmDate.month);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)30, agentData.value.edmDate.day);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_085:[ EDM_DATE] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_Oct_Day_31_Succeeds)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1600-10-31\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_DATE_TYPE, agentData.type);
            ASSERT_ARE_EQUAL(int16_t, (int16_t)1600, agentData.value.edmDate.year);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)10, agentData.value.edmDate.month);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)31, agentData.value.edmDate.day);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_085:[ EDM_DATE] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_Nov_Day_30_Succeeds)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1600-11-30\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_DATE_TYPE, agentData.type);
            ASSERT_ARE_EQUAL(int16_t, (int16_t)1600, agentData.value.edmDate.year);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)11, agentData.value.edmDate.month);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)30, agentData.value.edmDate.day);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_085:[ EDM_DATE] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_Dec_Day_31_Succeeds)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1600-12-31\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_DATE_TYPE, agentData.type);
            ASSERT_ARE_EQUAL(int16_t, (int16_t)1600, agentData.value.edmDate.year);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)12, agentData.value.edmDate.month);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)31, agentData.value.edmDate.day);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_085:[ EDM_DATE] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_Month_Zero_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1600-00-31\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_085:[ EDM_DATE] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_Month_13_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1600-13-01\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_085:[ EDM_DATE] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_Year_Minus_10000_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"-10000-01-01\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_085:[ EDM_DATE] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_Year_10000_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"10000-01-01\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_085:[ EDM_DATE] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_Year_Minus_9999_Succeeds)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"-9999-01-01\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_DATE_TYPE, agentData.type);
            ASSERT_ARE_EQUAL(int16_t, (int16_t)-9999, agentData.value.edmDate.year);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)1, agentData.value.edmDate.month);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)1, agentData.value.edmDate.day);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_085:[ EDM_DATE] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_Year_9999_Succeeds)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"9999-01-01\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_DATE_TYPE, agentData.type);
            ASSERT_ARE_EQUAL(int16_t, (int16_t)9999, agentData.value.edmDate.year);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)1, agentData.value.edmDate.month);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)1, agentData.value.edmDate.day);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_085:[ EDM_DATE] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_Year_To_Month_Dash_Is_Bad_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"9999x01-01\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_085:[ EDM_DATE] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_Month_To_Day_Dash_Is_Bad_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"9999-01x01\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_085:[ EDM_DATE] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_Year_Has_Extra_Zeroes_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"009999-01-01\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_085:[ EDM_DATE] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_Month_Has_Extra_Zeroes_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"9999-001-01\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_085:[ EDM_DATE] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_Day_Has_Extra_Zeroes_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"9999-01-001\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_078:[ EDM_DATE_TIME_OFFSET] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_TIME_OFFSET_Empty_String_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TIME_OFFSET_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_078:[ EDM_DATE_TIME_OFFSET] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_TIME_OFFSET_Some_Bad_Text_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"blahblah\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TIME_OFFSET_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_078:[ EDM_DATE_TIME_OFFSET] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_TIME_OFFSET_Only_Starting_Quote_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TIME_OFFSET_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_078:[ EDM_DATE_TIME_OFFSET] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_TIME_OFFSET_Missing_End_Quote_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1978-09-01T00:00Z";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TIME_OFFSET_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_078:[ EDM_DATE_TIME_OFFSET] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_TIME_OFFSET_Good_Date_Succeeds)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1978-09-01T00:00Z\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TIME_OFFSET_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_DATE_TIME_OFFSET_TYPE, agentData.type);
            ASSERT_ARE_EQUAL(int, 78, agentData.value.edmDateTimeOffset.dateTime.tm_year);
            ASSERT_ARE_EQUAL(int, 8, agentData.value.edmDateTimeOffset.dateTime.tm_mon);
            ASSERT_ARE_EQUAL(int, 1, agentData.value.edmDateTimeOffset.dateTime.tm_mday); /*this date is 1 sept 1978, and that was on a Friday*/
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_hour);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_min);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_sec);
            ASSERT_ARE_EQUAL(int8_t, (int8_t)0, agentData.value.edmDateTimeOffset.timeZoneHour);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)0, agentData.value.edmDateTimeOffset.timeZoneMinute);
            ASSERT_ARE_EQUAL(int, (int)5, agentData.value.edmDateTimeOffset.dateTime.tm_wday);
            ASSERT_ARE_EQUAL(int, (int)243, agentData.value.edmDateTimeOffset.dateTime.tm_yday);

            // cleanup
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_078:[ EDM_DATE_TIME_OFFSET] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_TIME_OFFSET_Zero_Day_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1978-09-00T00:00Z\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TIME_OFFSET_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_078:[ EDM_DATE_TIME_OFFSET] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_TIME_OFFSET_Jan_Day_32_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1978-01-32T00:00Z\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TIME_OFFSET_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_078:[ EDM_DATE_TIME_OFFSET] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_TIME_OFFSET_Feb_Day_29_Not_Leap_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1978-02-29T00:00Z\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TIME_OFFSET_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_078:[ EDM_DATE_TIME_OFFSET] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_TIME_OFFSET_Feb_Day_30_Leap_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1904-02-30T00:00Z\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TIME_OFFSET_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_078:[ EDM_DATE_TIME_OFFSET] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_TIME_OFFSET_Feb_Day_29_Not_Leap_Hundred_Divisible_Year_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1900-02-29T00:00Z\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TIME_OFFSET_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_078:[ EDM_DATE_TIME_OFFSET] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_TIME_OFFSET_Feb_Day_30_Leap_4_Hundred_Divisible_Year_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1600-02-30T00:00Z\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TIME_OFFSET_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_078:[ EDM_DATE_TIME_OFFSET] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_TIME_OFFSET_Mar_Day_32_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1600-03-32T00:00Z\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TIME_OFFSET_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_078:[ EDM_DATE_TIME_OFFSET] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_TIME_OFFSET_Apr_Day_31_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1600-04-31T00:00Z\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TIME_OFFSET_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_078:[ EDM_DATE_TIME_OFFSET] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_TIME_OFFSET_May_Day_32_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1600-05-32T00:00Z\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TIME_OFFSET_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_078:[ EDM_DATE_TIME_OFFSET] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_TIME_OFFSET_Jun_Day_31_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1600-06-31T00:00Z\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TIME_OFFSET_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_078:[ EDM_DATE_TIME_OFFSET] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_TIME_OFFSET_Jul_Day_32_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1600-07-32T00:00Z\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TIME_OFFSET_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_078:[ EDM_DATE_TIME_OFFSET] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_TIME_OFFSET_Aug_Day_32_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1600-08-32T00:00Z\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TIME_OFFSET_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_078:[ EDM_DATE_TIME_OFFSET] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_TIME_OFFSET_Sep_Day_31_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1600-09-31T00:00Z\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TIME_OFFSET_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_078:[ EDM_DATE_TIME_OFFSET] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_TIME_OFFSET_Oct_Day_32_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1600-10-32T00:00Z\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TIME_OFFSET_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_078:[ EDM_DATE_TIME_OFFSET] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_TIME_OFFSET_Nov_Day_31_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1600-11-31T00:00Z\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TIME_OFFSET_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_078:[ EDM_DATE_TIME_OFFSET] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_TIME_OFFSET_Dec_Day_32_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1600-12-32T00:00Z\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TIME_OFFSET_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_078:[ EDM_DATE_TIME_OFFSET] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_TIME_OFFSET_Negative_Month_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1900--1-01T00:00Z\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TIME_OFFSET_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_078:[ EDM_DATE_TIME_OFFSET] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_TIME_OFFSET_Negative_Day_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1900-01--9T00:00Z\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TIME_OFFSET_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_078:[ EDM_DATE_TIME_OFFSET] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_TIME_OFFSET_Year_Too_Short_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"300-01-01T00:00Z\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TIME_OFFSET_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_078:[ EDM_DATE_TIME_OFFSET] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_TIME_OFFSET_Jan_Day_31_Succeeds)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1978-01-31T00:00Z\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TIME_OFFSET_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_DATE_TIME_OFFSET_TYPE, agentData.type);
            ASSERT_ARE_EQUAL(int, 78, agentData.value.edmDateTimeOffset.dateTime.tm_year);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_mon);
            ASSERT_ARE_EQUAL(int, 31, agentData.value.edmDateTimeOffset.dateTime.tm_mday);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_hour);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_min);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_sec);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)0, agentData.value.edmDateTimeOffset.hasFractionalSecond);
            ASSERT_ARE_EQUAL(int8_t, (int8_t)0, agentData.value.edmDateTimeOffset.timeZoneHour);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)0, agentData.value.edmDateTimeOffset.timeZoneMinute);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)0, agentData.value.edmDateTimeOffset.hasTimeZone);

            // cleanup
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_078:[ EDM_DATE_TIME_OFFSET] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_TIME_OFFSET_Feb_Day_28_Not_Leap_Succeeds)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1978-02-28T00:00Z\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TIME_OFFSET_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_DATE_TIME_OFFSET_TYPE, agentData.type);
            ASSERT_ARE_EQUAL(int, 78, agentData.value.edmDateTimeOffset.dateTime.tm_year);
            ASSERT_ARE_EQUAL(int, 1, agentData.value.edmDateTimeOffset.dateTime.tm_mon);
            ASSERT_ARE_EQUAL(int, 28, agentData.value.edmDateTimeOffset.dateTime.tm_mday);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_hour);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_min);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_sec);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)0, agentData.value.edmDateTimeOffset.hasFractionalSecond);
            ASSERT_ARE_EQUAL(int8_t, (int8_t)0, agentData.value.edmDateTimeOffset.timeZoneHour);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)0, agentData.value.edmDateTimeOffset.timeZoneMinute);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)0, agentData.value.edmDateTimeOffset.hasTimeZone);

            // cleanup
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_078:[ EDM_DATE_TIME_OFFSET] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_TIME_OFFSET_Feb_Day_29_Leap_Succeeds)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1904-02-29T00:00Z\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TIME_OFFSET_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_DATE_TIME_OFFSET_TYPE, agentData.type);
            ASSERT_ARE_EQUAL(int, 4, agentData.value.edmDateTimeOffset.dateTime.tm_year);
            ASSERT_ARE_EQUAL(int, 1, agentData.value.edmDateTimeOffset.dateTime.tm_mon);
            ASSERT_ARE_EQUAL(int, 29, agentData.value.edmDateTimeOffset.dateTime.tm_mday);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_hour);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_min);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_sec);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)0, agentData.value.edmDateTimeOffset.hasFractionalSecond);
            ASSERT_ARE_EQUAL(int8_t, (int8_t)0, agentData.value.edmDateTimeOffset.timeZoneHour);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)0, agentData.value.edmDateTimeOffset.timeZoneMinute);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)0, agentData.value.edmDateTimeOffset.hasTimeZone);

            // cleanup
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_078:[ EDM_DATE_TIME_OFFSET] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_TIME_OFFSET_Feb_Day_28_Not_Leap_Hundred_Divisible_Year_Succeeds)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1900-02-28T00:00Z\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TIME_OFFSET_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_DATE_TIME_OFFSET_TYPE, agentData.type);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_year);
            ASSERT_ARE_EQUAL(int, 1, agentData.value.edmDateTimeOffset.dateTime.tm_mon);
            ASSERT_ARE_EQUAL(int, 28, agentData.value.edmDateTimeOffset.dateTime.tm_mday);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_hour);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_min);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_sec);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)0, agentData.value.edmDateTimeOffset.hasFractionalSecond);
            ASSERT_ARE_EQUAL(int8_t, (int8_t)0, agentData.value.edmDateTimeOffset.timeZoneHour);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)0, agentData.value.edmDateTimeOffset.timeZoneMinute);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)0, agentData.value.edmDateTimeOffset.hasTimeZone);

            // cleanup
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_078:[ EDM_DATE_TIME_OFFSET] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_TIME_OFFSET_Feb_Day_29_Leap_4_Hundred_Divisible_Year_Succeeds)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1600-02-29T00:00Z\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TIME_OFFSET_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_DATE_TIME_OFFSET_TYPE, agentData.type);
            ASSERT_ARE_EQUAL(int, -300, agentData.value.edmDateTimeOffset.dateTime.tm_year);
            ASSERT_ARE_EQUAL(int, 1, agentData.value.edmDateTimeOffset.dateTime.tm_mon);
            ASSERT_ARE_EQUAL(int, 29, agentData.value.edmDateTimeOffset.dateTime.tm_mday);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_hour);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_min);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_sec);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)0, agentData.value.edmDateTimeOffset.hasFractionalSecond);
            ASSERT_ARE_EQUAL(int8_t, (int8_t)0, agentData.value.edmDateTimeOffset.timeZoneHour);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)0, agentData.value.edmDateTimeOffset.timeZoneMinute);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)0, agentData.value.edmDateTimeOffset.hasTimeZone);

            // cleanup
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_078:[ EDM_DATE_TIME_OFFSET] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_TIME_OFFSET_Mar_Day_31_Succeeds)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1600-03-31T00:00Z\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TIME_OFFSET_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_DATE_TIME_OFFSET_TYPE, agentData.type);
            ASSERT_ARE_EQUAL(int, -300, agentData.value.edmDateTimeOffset.dateTime.tm_year);
            ASSERT_ARE_EQUAL(int, 2, agentData.value.edmDateTimeOffset.dateTime.tm_mon);
            ASSERT_ARE_EQUAL(int, 31, agentData.value.edmDateTimeOffset.dateTime.tm_mday);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_hour);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_min);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_sec);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)0, agentData.value.edmDateTimeOffset.hasFractionalSecond);
            ASSERT_ARE_EQUAL(int8_t, (int8_t)0, agentData.value.edmDateTimeOffset.timeZoneHour);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)0, agentData.value.edmDateTimeOffset.timeZoneMinute);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)0, agentData.value.edmDateTimeOffset.hasTimeZone);

            // cleanup
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_078:[ EDM_DATE_TIME_OFFSET] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_TIME_OFFSET_Apr_Day_30_Succeeds)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1600-04-30T00:00Z\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TIME_OFFSET_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_DATE_TIME_OFFSET_TYPE, agentData.type);
            ASSERT_ARE_EQUAL(int, -300, agentData.value.edmDateTimeOffset.dateTime.tm_year);
            ASSERT_ARE_EQUAL(int, 3, agentData.value.edmDateTimeOffset.dateTime.tm_mon);
            ASSERT_ARE_EQUAL(int, 30, agentData.value.edmDateTimeOffset.dateTime.tm_mday);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_hour);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_min);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_sec);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)0, agentData.value.edmDateTimeOffset.hasFractionalSecond);
            ASSERT_ARE_EQUAL(int8_t, (int8_t)0, agentData.value.edmDateTimeOffset.timeZoneHour);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)0, agentData.value.edmDateTimeOffset.timeZoneMinute);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)0, agentData.value.edmDateTimeOffset.hasTimeZone);

            // cleanup
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_078:[ EDM_DATE_TIME_OFFSET] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_TIME_OFFSET_May_Day_31_Succeeds)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1600-05-31T00:00Z\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TIME_OFFSET_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_DATE_TIME_OFFSET_TYPE, agentData.type);
            ASSERT_ARE_EQUAL(int, -300, agentData.value.edmDateTimeOffset.dateTime.tm_year);
            ASSERT_ARE_EQUAL(int, 4, agentData.value.edmDateTimeOffset.dateTime.tm_mon);
            ASSERT_ARE_EQUAL(int, 31, agentData.value.edmDateTimeOffset.dateTime.tm_mday);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_hour);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_min);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_sec);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)0, agentData.value.edmDateTimeOffset.hasFractionalSecond);
            ASSERT_ARE_EQUAL(int8_t, (int8_t)0, agentData.value.edmDateTimeOffset.timeZoneHour);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)0, agentData.value.edmDateTimeOffset.timeZoneMinute);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)0, agentData.value.edmDateTimeOffset.hasTimeZone);

            // cleanup
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_078:[ EDM_DATE_TIME_OFFSET] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_TIME_OFFSET_Jun_Day_30_Succeeds)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1600-06-30T00:00Z\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TIME_OFFSET_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_DATE_TIME_OFFSET_TYPE, agentData.type);
            ASSERT_ARE_EQUAL(int, -300, agentData.value.edmDateTimeOffset.dateTime.tm_year);
            ASSERT_ARE_EQUAL(int, 5, agentData.value.edmDateTimeOffset.dateTime.tm_mon);
            ASSERT_ARE_EQUAL(int, 30, agentData.value.edmDateTimeOffset.dateTime.tm_mday);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_hour);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_min);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_sec);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)0, agentData.value.edmDateTimeOffset.hasFractionalSecond);
            ASSERT_ARE_EQUAL(int8_t, (int8_t)0, agentData.value.edmDateTimeOffset.timeZoneHour);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)0, agentData.value.edmDateTimeOffset.timeZoneMinute);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)0, agentData.value.edmDateTimeOffset.hasTimeZone);

            // cleanup
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_078:[ EDM_DATE_TIME_OFFSET] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_TIME_OFFSET_Jul_Day_31_Succeeds)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1600-07-31T00:00Z\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TIME_OFFSET_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_DATE_TIME_OFFSET_TYPE, agentData.type);
            ASSERT_ARE_EQUAL(int, -300, agentData.value.edmDateTimeOffset.dateTime.tm_year);
            ASSERT_ARE_EQUAL(int, 6, agentData.value.edmDateTimeOffset.dateTime.tm_mon);
            ASSERT_ARE_EQUAL(int, 31, agentData.value.edmDateTimeOffset.dateTime.tm_mday);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_hour);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_min);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_sec);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)0, agentData.value.edmDateTimeOffset.hasFractionalSecond);
            ASSERT_ARE_EQUAL(int8_t, (int8_t)0, agentData.value.edmDateTimeOffset.timeZoneHour);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)0, agentData.value.edmDateTimeOffset.timeZoneMinute);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)0, agentData.value.edmDateTimeOffset.hasTimeZone);

            // cleanup
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_078:[ EDM_DATE_TIME_OFFSET] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_TIME_OFFSET_Aug_Day_31_Succeeds)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1600-08-31T00:00Z\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TIME_OFFSET_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_DATE_TIME_OFFSET_TYPE, agentData.type);
            ASSERT_ARE_EQUAL(int, -300, agentData.value.edmDateTimeOffset.dateTime.tm_year);
            ASSERT_ARE_EQUAL(int, 7, agentData.value.edmDateTimeOffset.dateTime.tm_mon);
            ASSERT_ARE_EQUAL(int, 31, agentData.value.edmDateTimeOffset.dateTime.tm_mday);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_hour);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_min);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_sec);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)0, agentData.value.edmDateTimeOffset.hasFractionalSecond);
            ASSERT_ARE_EQUAL(int8_t, (int8_t)0, agentData.value.edmDateTimeOffset.timeZoneHour);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)0, agentData.value.edmDateTimeOffset.timeZoneMinute);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)0, agentData.value.edmDateTimeOffset.hasTimeZone);

            // cleanup
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_078:[ EDM_DATE_TIME_OFFSET] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_TIME_OFFSET_Sep_Day_30_Succeeds)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1600-09-30T00:00Z\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TIME_OFFSET_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_DATE_TIME_OFFSET_TYPE, agentData.type);
            ASSERT_ARE_EQUAL(int, -300, agentData.value.edmDateTimeOffset.dateTime.tm_year);
            ASSERT_ARE_EQUAL(int, 8, agentData.value.edmDateTimeOffset.dateTime.tm_mon);
            ASSERT_ARE_EQUAL(int, 30, agentData.value.edmDateTimeOffset.dateTime.tm_mday);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_hour);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_min);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_sec);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)0, agentData.value.edmDateTimeOffset.hasFractionalSecond);
            ASSERT_ARE_EQUAL(int8_t, (int8_t)0, agentData.value.edmDateTimeOffset.timeZoneHour);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)0, agentData.value.edmDateTimeOffset.timeZoneMinute);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)0, agentData.value.edmDateTimeOffset.hasTimeZone);

            // cleanup
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_078:[ EDM_DATE_TIME_OFFSET] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_TIME_OFFSET_Oct_Day_31_Succeeds)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1600-10-31T00:00Z\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TIME_OFFSET_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_DATE_TIME_OFFSET_TYPE, agentData.type);
            ASSERT_ARE_EQUAL(int, -300, agentData.value.edmDateTimeOffset.dateTime.tm_year);
            ASSERT_ARE_EQUAL(int, 9, agentData.value.edmDateTimeOffset.dateTime.tm_mon);
            ASSERT_ARE_EQUAL(int, 31, agentData.value.edmDateTimeOffset.dateTime.tm_mday);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_hour);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_min);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_sec);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)0, agentData.value.edmDateTimeOffset.hasFractionalSecond);
            ASSERT_ARE_EQUAL(int8_t, (int8_t)0, agentData.value.edmDateTimeOffset.timeZoneHour);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)0, agentData.value.edmDateTimeOffset.timeZoneMinute);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)0, agentData.value.edmDateTimeOffset.hasTimeZone);

            // cleanup
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_078:[ EDM_DATE_TIME_OFFSET] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_TIME_OFFSET_Nov_Day_30_Succeeds)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1600-11-30T00:00Z\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TIME_OFFSET_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_DATE_TIME_OFFSET_TYPE, agentData.type);
            ASSERT_ARE_EQUAL(int, -300, agentData.value.edmDateTimeOffset.dateTime.tm_year);
            ASSERT_ARE_EQUAL(int, 10, agentData.value.edmDateTimeOffset.dateTime.tm_mon);
            ASSERT_ARE_EQUAL(int, 30, agentData.value.edmDateTimeOffset.dateTime.tm_mday);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_hour);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_min);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_sec);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)0, agentData.value.edmDateTimeOffset.hasFractionalSecond);
            ASSERT_ARE_EQUAL(int8_t, (int8_t)0, agentData.value.edmDateTimeOffset.timeZoneHour);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)0, agentData.value.edmDateTimeOffset.timeZoneMinute);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)0, agentData.value.edmDateTimeOffset.hasTimeZone);

            // cleanup
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_078:[ EDM_DATE_TIME_OFFSET] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_TIME_OFFSET_Dec_Day_31_Succeeds)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1600-12-31T00:00Z\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TIME_OFFSET_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_DATE_TIME_OFFSET_TYPE, agentData.type);
            ASSERT_ARE_EQUAL(int, -300, agentData.value.edmDateTimeOffset.dateTime.tm_year);
            ASSERT_ARE_EQUAL(int, 11, agentData.value.edmDateTimeOffset.dateTime.tm_mon);
            ASSERT_ARE_EQUAL(int, 31, agentData.value.edmDateTimeOffset.dateTime.tm_mday);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_hour);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_min);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_sec);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)0, agentData.value.edmDateTimeOffset.hasFractionalSecond);
            ASSERT_ARE_EQUAL(int8_t, (int8_t)0, agentData.value.edmDateTimeOffset.timeZoneHour);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)0, agentData.value.edmDateTimeOffset.timeZoneMinute);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)0, agentData.value.edmDateTimeOffset.hasTimeZone);

            // cleanup
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_078:[ EDM_DATE_TIME_OFFSET] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_TIME_OFFSET_Month_Zero_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1600-00-31T00:00Z\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TIME_OFFSET_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_078:[ EDM_DATE_TIME_OFFSET] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_TIME_OFFSET_Month_13_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1600-13-01T00:00Z\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TIME_OFFSET_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_078:[ EDM_DATE_TIME_OFFSET] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_TIME_OFFSET_Year_Minus_10000_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"-10000-01-01T00:00Z\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TIME_OFFSET_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_078:[ EDM_DATE_TIME_OFFSET] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_TIME_OFFSET_Year_10000_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"10000-01-01T00:00Z\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TIME_OFFSET_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_078:[ EDM_DATE_TIME_OFFSET] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_TIME_OFFSET_Year_Minus_9999_Succeeds)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"-9999-01-01T00:00Z\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TIME_OFFSET_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_DATE_TIME_OFFSET_TYPE, agentData.type);
            ASSERT_ARE_EQUAL(int, -9999-1900, agentData.value.edmDateTimeOffset.dateTime.tm_year);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_mon);
            ASSERT_ARE_EQUAL(int, 1, agentData.value.edmDateTimeOffset.dateTime.tm_mday);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_hour);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_min);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_sec);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)0, agentData.value.edmDateTimeOffset.hasFractionalSecond);
            ASSERT_ARE_EQUAL(int8_t, (int8_t)0, agentData.value.edmDateTimeOffset.timeZoneHour);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)0, agentData.value.edmDateTimeOffset.timeZoneMinute);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)0, agentData.value.edmDateTimeOffset.hasTimeZone);

            // cleanup
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_078:[ EDM_DATE_TIME_OFFSET] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_TIME_OFFSET_Year_9999_Succeeds)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"9999-01-01T00:00Z\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TIME_OFFSET_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_DATE_TIME_OFFSET_TYPE, agentData.type);
            ASSERT_ARE_EQUAL(int, 9999-1900, agentData.value.edmDateTimeOffset.dateTime.tm_year);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_mon);
            ASSERT_ARE_EQUAL(int, 1, agentData.value.edmDateTimeOffset.dateTime.tm_mday);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_hour);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_min);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_sec);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)0, agentData.value.edmDateTimeOffset.hasFractionalSecond);
            ASSERT_ARE_EQUAL(int8_t, (int8_t)0, agentData.value.edmDateTimeOffset.timeZoneHour);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)0, agentData.value.edmDateTimeOffset.timeZoneMinute);

            // cleanup
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_078:[ EDM_DATE_TIME_OFFSET] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_TIME_OFFSET_Year_To_Month_Dash_Is_Bad_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"9999x01-01T00:00Z\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TIME_OFFSET_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_078:[ EDM_DATE_TIME_OFFSET] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_TIME_OFFSET_Month_To_Day_Dash_Is_Bad_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"9999-01x01T00:00Z\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TIME_OFFSET_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_078:[ EDM_DATE_TIME_OFFSET] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_TIME_OFFSET_Year_Has_Extra_Zeroes_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"009999-01-01T00:00Z\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TIME_OFFSET_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_078:[ EDM_DATE_TIME_OFFSET] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_TIME_OFFSET_Month_Has_Extra_Zeroes_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"9999-001-01T00:00Z\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TIME_OFFSET_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_078:[ EDM_DATE_TIME_OFFSET] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_TIME_OFFSET_Day_Has_Extra_Zeroes_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"9999-01-001T00:00Z\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TIME_OFFSET_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_078:[ EDM_DATE_TIME_OFFSET] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_TIME_OFFSET_Missing_Z_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"9999-01-01T00:00\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TIME_OFFSET_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_078:[ EDM_DATE_TIME_OFFSET] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_TIME_OFFSET_Negative_Hour_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1900-01-01T-1:00Z\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TIME_OFFSET_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_078:[ EDM_DATE_TIME_OFFSET] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_TIME_OFFSET_Hour_24_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1900-01-01T24:00Z\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TIME_OFFSET_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_078:[ EDM_DATE_TIME_OFFSET] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_TIME_OFFSET_Hour_23_Succeeds)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1900-01-01T23:00Z\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TIME_OFFSET_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_DATE_TIME_OFFSET_TYPE, agentData.type);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_year);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_mon);
            ASSERT_ARE_EQUAL(int, 1, agentData.value.edmDateTimeOffset.dateTime.tm_mday);
            ASSERT_ARE_EQUAL(int, 23, agentData.value.edmDateTimeOffset.dateTime.tm_hour);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_min);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_sec);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)0, agentData.value.edmDateTimeOffset.hasFractionalSecond);
            ASSERT_ARE_EQUAL(int8_t, (int8_t)0, agentData.value.edmDateTimeOffset.timeZoneHour);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)0, agentData.value.edmDateTimeOffset.timeZoneMinute);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)0, agentData.value.edmDateTimeOffset.hasTimeZone);

            // cleanup
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_078:[ EDM_DATE_TIME_OFFSET] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_TIME_OFFSET_Negative_Minute_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1900-01-01T00:-1Z\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TIME_OFFSET_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_078:[ EDM_DATE_TIME_OFFSET] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_TIME_OFFSET_Minute_60_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1900-01-01T00:60Z\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TIME_OFFSET_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_078:[ EDM_DATE_TIME_OFFSET] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_TIME_OFFSET_Minute_59_Succeeds)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1900-01-01T23:59Z\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TIME_OFFSET_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_DATE_TIME_OFFSET_TYPE, agentData.type);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_year);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_mon);
            ASSERT_ARE_EQUAL(int, 1, agentData.value.edmDateTimeOffset.dateTime.tm_mday);
            ASSERT_ARE_EQUAL(int, 23, agentData.value.edmDateTimeOffset.dateTime.tm_hour);
            ASSERT_ARE_EQUAL(int, 59, agentData.value.edmDateTimeOffset.dateTime.tm_min);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_sec);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)0, agentData.value.edmDateTimeOffset.hasFractionalSecond);
            ASSERT_ARE_EQUAL(int8_t, (int8_t)0, agentData.value.edmDateTimeOffset.timeZoneHour);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)0, agentData.value.edmDateTimeOffset.timeZoneMinute);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)0, agentData.value.edmDateTimeOffset.hasTimeZone);

            // cleanup
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_078:[ EDM_DATE_TIME_OFFSET] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_TIME_OFFSET_Negative_Seconds_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1900-01-01T00:00:-1Z\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TIME_OFFSET_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_078:[ EDM_DATE_TIME_OFFSET] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_TIME_OFFSET_60_Seconds_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1900-01-01T00:00:60Z\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TIME_OFFSET_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_078:[ EDM_DATE_TIME_OFFSET] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_TIME_OFFSET_Second_59_Succeeds)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1900-01-01T23:59:59Z\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TIME_OFFSET_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_DATE_TIME_OFFSET_TYPE, agentData.type);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_year);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_mon);
            ASSERT_ARE_EQUAL(int, 1, agentData.value.edmDateTimeOffset.dateTime.tm_mday);
            ASSERT_ARE_EQUAL(int, 23, agentData.value.edmDateTimeOffset.dateTime.tm_hour);
            ASSERT_ARE_EQUAL(int, 59, agentData.value.edmDateTimeOffset.dateTime.tm_min);
            ASSERT_ARE_EQUAL(int, 59, agentData.value.edmDateTimeOffset.dateTime.tm_sec);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)0, agentData.value.edmDateTimeOffset.hasFractionalSecond);
            ASSERT_ARE_EQUAL(int8_t, (int8_t)0, agentData.value.edmDateTimeOffset.timeZoneHour);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)0, agentData.value.edmDateTimeOffset.timeZoneMinute);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)0, agentData.value.edmDateTimeOffset.hasTimeZone);

            // cleanup
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_078:[ EDM_DATE_TIME_OFFSET] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_TIME_OFFSET_Negative_Fractional_Seconds_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1900-01-01T00:00:00.-1Z\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TIME_OFFSET_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_078:[ EDM_DATE_TIME_OFFSET] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_TIME_OFFSET_Max_Fractional_Seconds_Succeeds)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1900-01-01T00:00:00.999999999999Z\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TIME_OFFSET_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_DATE_TIME_OFFSET_TYPE, agentData.type);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_year);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_mon);
            ASSERT_ARE_EQUAL(int, 1, agentData.value.edmDateTimeOffset.dateTime.tm_mday);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_hour);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_min);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_sec);
            ASSERT_ARE_EQUAL(uint64_t, (uint64_t)999999999999, agentData.value.edmDateTimeOffset.fractionalSecond);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)1, agentData.value.edmDateTimeOffset.hasFractionalSecond);
            ASSERT_ARE_EQUAL(int8_t, (int8_t)0, agentData.value.edmDateTimeOffset.timeZoneHour);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)0, agentData.value.edmDateTimeOffset.timeZoneMinute);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)0, agentData.value.edmDateTimeOffset.hasTimeZone);

            // cleanup
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_078:[ EDM_DATE_TIME_OFFSET] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_TIME_OFFSET_Max_Fractional_Seconds_Plus_1_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1900-01-01T00:00:00.1000000000000Z\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TIME_OFFSET_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_078:[ EDM_DATE_TIME_OFFSET] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_TIME_OFFSET_Hour_Offset_Minus_24_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1900-01-01T00:00:00.1-24:00\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TIME_OFFSET_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_078:[ EDM_DATE_TIME_OFFSET] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_TIME_OFFSET_Hour_Offset_Plus_24_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1900-01-01T00:00:00.1+24:00\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TIME_OFFSET_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_078:[ EDM_DATE_TIME_OFFSET] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_TIME_OFFSET_Hour_Offset_Minus_23_Succeeds)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1900-01-01T00:00:00.1-23:00\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TIME_OFFSET_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_DATE_TIME_OFFSET_TYPE, agentData.type);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_year);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_mon);
            ASSERT_ARE_EQUAL(int, 1, agentData.value.edmDateTimeOffset.dateTime.tm_mday);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_hour);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_min);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_sec);
            ASSERT_ARE_EQUAL(uint64_t, (uint64_t)1, agentData.value.edmDateTimeOffset.fractionalSecond);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)1, agentData.value.edmDateTimeOffset.hasFractionalSecond);
            ASSERT_ARE_EQUAL(int8_t, (int8_t)-23, agentData.value.edmDateTimeOffset.timeZoneHour);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)0, agentData.value.edmDateTimeOffset.timeZoneMinute);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)1, agentData.value.edmDateTimeOffset.hasTimeZone);

            // cleanup
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_078:[ EDM_DATE_TIME_OFFSET] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_TIME_OFFSET_Hour_Offset_Plus_23_Succeeds)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1900-01-01T00:00:00.1+23:00\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TIME_OFFSET_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_DATE_TIME_OFFSET_TYPE, agentData.type);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_year);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_mon);
            ASSERT_ARE_EQUAL(int, 1, agentData.value.edmDateTimeOffset.dateTime.tm_mday);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_hour);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_min);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_sec);
            ASSERT_ARE_EQUAL(uint64_t, (uint64_t)1, agentData.value.edmDateTimeOffset.fractionalSecond);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)1, agentData.value.edmDateTimeOffset.hasFractionalSecond);
            ASSERT_ARE_EQUAL(int8_t, (int8_t)23, agentData.value.edmDateTimeOffset.timeZoneHour);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)0, agentData.value.edmDateTimeOffset.timeZoneMinute);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)1, agentData.value.edmDateTimeOffset.hasTimeZone);

            // cleanup
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_078:[ EDM_DATE_TIME_OFFSET] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_TIME_OFFSET_Minute_Offset_Minus_1_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1900-01-01T00:00:00.1+23:-1\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TIME_OFFSET_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_078:[ EDM_DATE_TIME_OFFSET] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_TIME_OFFSET_Minute_Offset_60_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1900-01-01T00:00:00.1+23:60\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TIME_OFFSET_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_078:[ EDM_DATE_TIME_OFFSET] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_TIME_OFFSET_Minute_Offset_59_Succeeds)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1900-01-01T00:00:00.1+23:59\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TIME_OFFSET_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_DATE_TIME_OFFSET_TYPE, agentData.type);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_year);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_mon);
            ASSERT_ARE_EQUAL(int, 1, agentData.value.edmDateTimeOffset.dateTime.tm_mday);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_hour);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_min);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_sec);
            ASSERT_ARE_EQUAL(uint64_t, (uint64_t)1, agentData.value.edmDateTimeOffset.fractionalSecond);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)1, agentData.value.edmDateTimeOffset.hasFractionalSecond);
            ASSERT_ARE_EQUAL(int8_t, (int8_t)23, agentData.value.edmDateTimeOffset.timeZoneHour);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)59, agentData.value.edmDateTimeOffset.timeZoneMinute);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)1, agentData.value.edmDateTimeOffset.hasTimeZone);

            // cleanup
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_078:[ EDM_DATE_TIME_OFFSET] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_TIME_OFFSET_Missing_Fractional_SecondsWith_TimeOffset_Succeeds)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1900-01-01T00:00:00+23:59\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TIME_OFFSET_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_DATE_TIME_OFFSET_TYPE, agentData.type);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_year);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_mon);
            ASSERT_ARE_EQUAL(int, 1, agentData.value.edmDateTimeOffset.dateTime.tm_mday);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_hour);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_min);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_sec);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)0, agentData.value.edmDateTimeOffset.hasFractionalSecond);
            ASSERT_ARE_EQUAL(int8_t, (int8_t)23, agentData.value.edmDateTimeOffset.timeZoneHour);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)59, agentData.value.edmDateTimeOffset.timeZoneMinute);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)1, agentData.value.edmDateTimeOffset.hasTimeZone);

            // cleanup
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_078:[ EDM_DATE_TIME_OFFSET] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_TIME_OFFSET_Missing_SecondsWith_TimeOffset_Succeeds)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1900-01-01T00:00+23:59\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TIME_OFFSET_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_DATE_TIME_OFFSET_TYPE, agentData.type);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_year);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_mon);
            ASSERT_ARE_EQUAL(int, 1, agentData.value.edmDateTimeOffset.dateTime.tm_mday);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_hour);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_min);
            ASSERT_ARE_EQUAL(int, 0, agentData.value.edmDateTimeOffset.dateTime.tm_sec);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)0, agentData.value.edmDateTimeOffset.hasFractionalSecond);
            ASSERT_ARE_EQUAL(int8_t, (int8_t)23, agentData.value.edmDateTimeOffset.timeZoneHour);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)59, agentData.value.edmDateTimeOffset.timeZoneMinute);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)1, agentData.value.edmDateTimeOffset.hasTimeZone);

            // cleanup
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_078:[ EDM_DATE_TIME_OFFSET] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_With_EDM_DATE_TIME_OFFSET_Sets_dst_to_a_negative_value)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"1600-04-30T00:00Z\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DATE_TIME_OFFSET_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_IS_TRUE(agentData.value.edmDateTimeOffset.dateTime.tm_isdst < 0);

            Destroy_AGENT_DATA_TYPE(&agentData);
        }

#ifndef NO_FLOATS
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_080:[ EDM_DOUBLE] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DOUBLE_Positive_Value_Succeeds)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "42.42";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DOUBLE_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_DOUBLE_TYPE, agentData.type);
            ASSERT_ARE_EQUAL(double, (double)42.42, agentData.value.edmDouble.value);

            // cleanup
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_080:[ EDM_DOUBLE] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DOUBLE_Negative_Value_Succeeds)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "-42.42";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DOUBLE_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_DOUBLE_TYPE, agentData.type);
            ASSERT_ARE_EQUAL(double, (double)-42.42, agentData.value.edmDouble.value);

            // cleanup
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_080:[ EDM_DOUBLE] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DOUBLE_Max_Positive_Value_Succeeds)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "1.7976931348623158e+308";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DOUBLE_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_DOUBLE_TYPE, agentData.type);
            ASSERT_ARE_EQUAL(double, DBL_MAX, agentData.value.edmDouble.value);

            // cleanup
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_089:[EDM_SINGLE] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DOUBLE_Overflow_Max_Positive_Value_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "1.7976931348623159e+308";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DOUBLE_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_080:[ EDM_DOUBLE] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DOUBLE_Min_Positive_Value_Succeeds)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "2.2250738585072014e-308";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DOUBLE_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_DOUBLE_TYPE, agentData.type);
            ASSERT_ARE_EQUAL(double, DBL_MIN, agentData.value.edmDouble.value);

            // cleanup
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_080:[ EDM_DOUBLE] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DOUBLE_Bad_Text_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "BlahBlah";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DOUBLE_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_080:[ EDM_DOUBLE] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DOUBLE_NaN_Succeeds)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"NaN\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DOUBLE_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_DOUBLE_TYPE, agentData.type);
            ASSERT_IS_TRUE(ISNAN(agentData.value.edmDouble.value) != 0);

            // cleanup
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_080:[ EDM_DOUBLE] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DOUBLE_Positive_Infinity_Succeeds)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"INF\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DOUBLE_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_DOUBLE_TYPE, agentData.type);
            ASSERT_IS_TRUE(ISPOSITIVEINFINITY(agentData.value.edmDouble.value) != 0);

            // cleanup
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_080:[ EDM_DOUBLE] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DOUBLE_Negative_Infinity_Succeeds)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"-INF\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DOUBLE_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_DOUBLE_TYPE, agentData.type);
            ASSERT_IS_TRUE(ISNEGATIVEINFINITY(agentData.value.edmDouble.value) != 0);

            // cleanup
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_080:[ EDM_DOUBLE] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DOUBLE_Positive_e_Value_Succeeds)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "1.0e5";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DOUBLE_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_DOUBLE_TYPE, agentData.type);
            ASSERT_ARE_EQUAL(double, (double)100000.0, agentData.value.edmDouble.value);

            // cleanup
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_089:[EDM_SINGLE] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_SINGLE_Positive_Value_Succeeds)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "42.42";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_SINGLE_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_SINGLE_TYPE, agentData.type);
            ASSERT_ARE_EQUAL(float, (float)42.42, agentData.value.edmSingle.value);

            // cleanup
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_089:[EDM_SINGLE] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_SINGLE_Negative_Value_Succeeds)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "-42.42";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_SINGLE_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_SINGLE_TYPE, agentData.type);
            ASSERT_ARE_EQUAL(float, (float)-42.42, agentData.value.edmSingle.value);

            // cleanup
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_089:[EDM_SINGLE] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_SINGLE_Max_Positive_Value_Succeeds)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "3.402823466e+38";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_SINGLE_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_SINGLE_TYPE, agentData.type);
            ASSERT_ARE_EQUAL(float, (float)FLT_MAX, agentData.value.edmSingle.value);

            // cleanup
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_089:[EDM_SINGLE] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_SINGLE_Min_Negative_Value_Succeeds)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "-3.402823466e+38";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_SINGLE_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_SINGLE_TYPE, agentData.type);
            ASSERT_ARE_EQUAL(float, (FLT_MAX * (-1)), agentData.value.edmSingle.value);

            // cleanup
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_089:[EDM_SINGLE] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_SINGLE_Min_Positive_Value_Succeeds)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "1.17549435e-38";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_SINGLE_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_SINGLE_TYPE, agentData.type);
            ASSERT_ARE_EQUAL(float, (float)FLT_MIN, agentData.value.edmSingle.value);

            // cleanup
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_089:[EDM_SINGLE] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_SINGLE_Overflow_Max_Positive_Value_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "3.5e+38";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_SINGLE_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_089:[EDM_SINGLE] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_SINGLE_Exponential_Overflow_Max_Positive_Value_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "1.0e39";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_SINGLE_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_089:[EDM_SINGLE] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_SINGLE_Bad_Text_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "BlahBlah";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_SINGLE_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_089:[EDM_SINGLE] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_SINGLE_NaN_Succeeds)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"NaN\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_SINGLE_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_SINGLE_TYPE, agentData.type);
            ASSERT_IS_TRUE(ISNAN(agentData.value.edmSingle.value) != 0);

            // cleanup
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_089:[EDM_SINGLE] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_SINGLE_Positive_Infinity_Succeeds)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"INF\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_SINGLE_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_SINGLE_TYPE, agentData.type);
            ASSERT_IS_TRUE(ISPOSITIVEINFINITY(agentData.value.edmSingle.value) != 0);

            // cleanup
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_089:[EDM_SINGLE] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_SINGLE_Negative_Infinity_Succeeds)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"-INF\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_SINGLE_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_SINGLE_TYPE, agentData.type);
            ASSERT_IS_TRUE(ISNEGATIVEINFINITY(agentData.value.edmSingle.value) != 0);

            // cleanup
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_089:[EDM_SINGLE] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_SINGLE_e_Value_Succeeds)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "1.0e5";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_SINGLE_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_SINGLE_TYPE, agentData.type);
            ASSERT_ARE_EQUAL(float, (float)100000.0, agentData.value.edmSingle.value);

            // cleanup
            Destroy_AGENT_DATA_TYPE(&agentData);
        }
#endif

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_079:[ EDM_DECIMAL] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DECIMAL_Empty_String_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DECIMAL_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_079:[ EDM_DECIMAL] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DECIMAL_One_Quote_Only_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DECIMAL_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_079:[ EDM_DECIMAL] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DECIMAL_2_Quotes_Only_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DECIMAL_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_079:[ EDM_DECIMAL] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DECIMAL_Valid_Number_Missing_Start_Quote_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "42.42\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DECIMAL_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_079:[ EDM_DECIMAL] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DECIMAL_Valid_Number_Missing_End_Quote_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"42.42";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DECIMAL_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_079:[ EDM_DECIMAL] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DECIMAL_Bad_Text_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"BlahBlah\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DECIMAL_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_079:[ EDM_DECIMAL] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DECIMAL_2_Dots_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"42.42.\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DECIMAL_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_079:[ EDM_DECIMAL] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DECIMAL_No_Digits_After_Dot_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"42.\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DECIMAL_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_079:[ EDM_DECIMAL] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DECIMAL_Other_Sign_Than_Char_First_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"x42\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DECIMAL_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_079:[ EDM_DECIMAL] */
        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_087:[ CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type.] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DECIMAL_Other_Sign_Than_Dot_Fails)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"42x42\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DECIMAL_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_079:[ EDM_DECIMAL] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DECIMAL_Good_Value_Succeeds)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"42.42\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DECIMAL_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_DECIMAL_TYPE, agentData.type);
            ASSERT_ARE_EQUAL(char_ptr, "42.42", STRING_c_str(agentData.value.edmDecimal.value));

            // cleanup
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_079:[ EDM_DECIMAL] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DECIMAL_Only_Integer_Part_Succeeds)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"42\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DECIMAL_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_DECIMAL_TYPE, agentData.type);
            ASSERT_ARE_EQUAL(char_ptr, "42", STRING_c_str(agentData.value.edmDecimal.value));

            // cleanup
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_079:[ EDM_DECIMAL] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DECIMAL_Only_Negative_Integer_Part_Succeeds)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"-42\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DECIMAL_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_DECIMAL_TYPE, agentData.type);
            ASSERT_ARE_EQUAL(char_ptr, "-42", STRING_c_str(agentData.value.edmDecimal.value));

            // cleanup
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_079:[ EDM_DECIMAL] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DECIMAL_Negative_Number_Succeeds)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"-42.42\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DECIMAL_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_DECIMAL_TYPE, agentData.type);
            ASSERT_ARE_EQUAL(char_ptr, "-42.42", STRING_c_str(agentData.value.edmDecimal.value));

            // cleanup
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /* Tests_SRS_AGENT_TYPE_SYSTEM_99_079:[ EDM_DECIMAL] */
        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DECIMAL_Positive_Sign_Succeeds)
        {
            // arrange
            AGENT_DATA_TYPE agentData;
            const char* source = "\"+42.42\"";

            // act
            AGENT_DATA_TYPES_RESULT result = CreateAgentDataType_From_String(source, EDM_DECIMAL_TYPE, &agentData);

            // assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_DECIMAL_TYPE, agentData.type);
            ASSERT_ARE_EQUAL(char_ptr, "+42.42", STRING_c_str(agentData.value.edmDecimal.value));

            // cleanup
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_091:[Creates an AGENT_DATA_TYPE containing an Edm.DateTimeOffset from an EDM_DATE_TIME_OFFSET.]*/
        TEST_FUNCTION(AgentTypeSystem_CREATE_AGENT_DATA_TYPE_from_EDM_DATE_TIME_OFFSET_succeeds)
        {
            ///arrange
            EDM_DATE_TIME_OFFSET test = { { 0 } };
            test.dateTime.tm_year = 114;
            test.dateTime.tm_mon = 6 - 1;
            test.dateTime.tm_mday = 17;
            test.dateTime.tm_hour = 8;
            test.dateTime.tm_min = 51;
            test.dateTime.tm_sec = 23;
            test.hasFractionalSecond = 1;
            test.fractionalSecond = 5;
            test.hasTimeZone = 1;
            test.timeZoneHour = -8;
            test.timeZoneMinute = 1;
            AGENT_DATA_TYPE agentData;

            ///act
            auto result = Create_AGENT_DATA_TYPE_from_EDM_DATE_TIME_OFFSET(&agentData, test);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_DATE_TIME_OFFSET_TYPE, agentData.type);
            ASSERT_ARE_EQUAL(int, 114, agentData.value.edmDateTimeOffset.dateTime.tm_year);
            ASSERT_ARE_EQUAL(int, 5, agentData.value.edmDateTimeOffset.dateTime.tm_mon);
            ASSERT_ARE_EQUAL(int, 17, agentData.value.edmDateTimeOffset.dateTime.tm_mday);
            ASSERT_ARE_EQUAL(int, 8, agentData.value.edmDateTimeOffset.dateTime.tm_hour);
            ASSERT_ARE_EQUAL(int, 51, agentData.value.edmDateTimeOffset.dateTime.tm_min);
            ASSERT_ARE_EQUAL(int, 23, agentData.value.edmDateTimeOffset.dateTime.tm_sec);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)1, agentData.value.edmDateTimeOffset.hasFractionalSecond);
            ASSERT_ARE_EQUAL(uint64_t, (uint64_t)5, agentData.value.edmDateTimeOffset.fractionalSecond);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)1, agentData.value.edmDateTimeOffset.hasTimeZone);
            ASSERT_ARE_EQUAL(int8_t, (int8_t)-8, agentData.value.edmDateTimeOffset.timeZoneHour);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)1, agentData.value.edmDateTimeOffset.timeZoneMinute);

            ///cleanup 
            Destroy_AGENT_DATA_TYPE(&agentData);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_092:[ The structure shall be validated to be conforming to OData specifications (odata-abnf-construction-rules, 2013), and if found invalid, AGENT_DATA_TYPES_INVALID_ARG shall be returned.]*/
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_EDM_DATE_TIME_OFFSET_when_date_is_invalid_fails)
        {
            ///arrange
            AGENT_DATA_TYPE agentData;

            struct PointInTime {
                int year; 
                int month; 
                int day;
            } testVectorShallFail[] =
            {
                { TEST_YEAR_1, TEST_MONTH_1, 0 },
                { TEST_YEAR_1, TEST_MONTH_1, -1 },
                { TEST_YEAR_1, -1, 1 },
                { TEST_YEAR_1, 0, 1 },
                { TEST_YEAR_1, 13, 1 },
                { TEST_YEAR_1, 1, 32 },
                { 2011, 2, 29 },
                { 2012, 2, 30 },
                { 2100, 2, 29 },
                { 2000, 2, 30 },
                { TEST_YEAR_1, 3, 32 },
                { TEST_YEAR_1, 4, 31 },
                { TEST_YEAR_1, 5, 32 },
                { TEST_YEAR_1, 6, 31 },
                { TEST_YEAR_1, 7, 32 },
                { TEST_YEAR_1, 8, 32 },
                { TEST_YEAR_1, 9, 31 },
                { TEST_YEAR_1, 10, 32 },
                { TEST_YEAR_1, 11, 31 },
                { TEST_YEAR_1, 12, 32 },
                { -10000, 12, 31 },
                { 10000, 1, 1 }
            };

            {
                for (size_t i = 0; i < sizeof(testVectorShallFail) / sizeof(testVectorShallFail[0]); i++)
                {
                    EDM_DATE_TIME_OFFSET temp = { { 0 } };
                    temp.dateTime.tm_year = testVectorShallFail[i].year - 1900;
                    temp.dateTime.tm_mon = testVectorShallFail[i].month - 1;
                    temp.dateTime.tm_mday = testVectorShallFail[i].day;
                    ///act
                    
                    auto result = Create_AGENT_DATA_TYPE_from_EDM_DATE_TIME_OFFSET(&agentData, temp);

                    ///assert
                    ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);

                    ///cleanup 
                }
            }
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_092:[ The structure shall be validated to be conforming to OData specifications (odata-abnf-construction-rules, 2013), and if found invalid, AGENT_DATA_TYPES_INVALID_ARG shall be returned.]*/
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_EDM_DATE_TIME_OFFSET_when_time_is_invalid)
        {
            ///arrange
            AGENT_DATA_TYPE agentData;

            struct PointInTime {
                int year;
                int month;
                int day;
                int hour;
                int min;
                int sec;
            }  testVectorShallFail[] =
            {
                { TEST_YEAR_1, TEST_MONTH_1, TEST_MDAY_1, TEST_HOUR_1, TEST_MIN_1, 60 },
                { TEST_YEAR_1, TEST_MONTH_1, TEST_MDAY_1, TEST_HOUR_1, TEST_MIN_1, -1 },
                { TEST_YEAR_1, TEST_MONTH_1, TEST_MDAY_1, TEST_HOUR_1, 60, TEST_SEC_1 },
                { TEST_YEAR_1, TEST_MONTH_1, TEST_MDAY_1, TEST_HOUR_1, -1, TEST_SEC_1 },
                { TEST_YEAR_1, TEST_MONTH_1, TEST_MDAY_1, 24, TEST_MIN_1, TEST_SEC_1 },
                { TEST_YEAR_1, TEST_MONTH_1, TEST_MDAY_1, -1, TEST_MIN_1, TEST_SEC_1 },
            };

            {
                for (size_t i = 0; i < sizeof(testVectorShallFail) / sizeof(testVectorShallFail[0]); i++)
                {
                    EDM_DATE_TIME_OFFSET temp = { { 0 } };
                    temp.dateTime.tm_year = testVectorShallFail[i].year - 1900; 
                    temp.dateTime.tm_mon = testVectorShallFail[i].month - 1;
                    temp.dateTime.tm_mday = testVectorShallFail[i].day;
                    temp.dateTime.tm_hour = testVectorShallFail[i].hour;
                    temp.dateTime.tm_min = testVectorShallFail[i].min;
                    temp.dateTime.tm_sec = testVectorShallFail[i].sec;
                    ///act
                    auto result = Create_AGENT_DATA_TYPE_from_EDM_DATE_TIME_OFFSET(&agentData, temp);

                    ///assert
                    ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);

                    ///cleanup 
                }
            }
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_092:[ The structure shall be validated to be conforming to OData specifications (odata-abnf-construction-rules, 2013), and if found invalid, AGENT_DATA_TYPES_INVALID_ARG shall be returned.]*/
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_EDM_DATE_TIME_OFFSET_when_fractional_seconds_is_invalid_fails)
        {
            /*the only reasonw hy fractional seconds would fail is because of it has more than 12 digits.*/
            ///arrange
            AGENT_DATA_TYPE agentData;

            EDM_DATE_TIME_OFFSET testVectorShallFail[2];
            /*NO DESIGNATED INITIALIZERS IN C++*/
            testVectorShallFail[0].dateTime.tm_year = TEST_YEAR_1 - 1900;
            testVectorShallFail[0].dateTime.tm_mon = TEST_MONTH_1 - 1;
            testVectorShallFail[0].dateTime.tm_mday = TEST_MDAY_1;
            testVectorShallFail[0].dateTime.tm_hour = TEST_HOUR_1;
            testVectorShallFail[0].dateTime.tm_min = TEST_MIN_1;
            testVectorShallFail[0].dateTime.tm_sec = TEST_SEC_1;
            testVectorShallFail[0].hasFractionalSecond = 1;
            testVectorShallFail[0].fractionalSecond = 1234567890123;

            testVectorShallFail[1].dateTime.tm_year = TEST_YEAR_1 - 1900;
            testVectorShallFail[1].dateTime.tm_mon = TEST_MONTH_1 - 1;
            testVectorShallFail[1].dateTime.tm_mday = TEST_MDAY_1;
            testVectorShallFail[1].dateTime.tm_hour = TEST_HOUR_1;
            testVectorShallFail[1].dateTime.tm_min = TEST_MIN_1;
            testVectorShallFail[1].dateTime.tm_sec = TEST_SEC_1;
            testVectorShallFail[1].hasFractionalSecond = 1;
            testVectorShallFail[1].fractionalSecond = 999999999999 + 1;

            {
                for (size_t i = 0; i < sizeof(testVectorShallFail) / sizeof(testVectorShallFail[0]); i++)
                {
                    ///act
                    auto result = Create_AGENT_DATA_TYPE_from_EDM_DATE_TIME_OFFSET(&agentData, testVectorShallFail[i]);

                    ///assert
                    ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
                }
            }
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_092:[ The structure shall be validated to be conforming to OData specifications (odata-abnf-construction-rules, 2013), and if found invalid, AGENT_DATA_TYPES_INVALID_ARG shall be returned.]*/
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_EDM_DATE_TIME_OFFSET_when_time_zone_is_invalid_fails)
        {
            ///arrange
            AGENT_DATA_TYPE agentData;

            struct PointInTime {
                int year;
                int month;
                int day;
                int hour;
                int min;
                int sec;
                uint8_t hasFractionalSecond;
                uint64_t fractionalSecond;
                uint8_t hasTimeZone;
                int8_t timeZoneHour;
                uint8_t timeZoneMin;
            }
            testVectorShallFail[] =
            {
                { TEST_YEAR_1, TEST_MONTH_1, TEST_MDAY_1, TEST_HOUR_1, TEST_MIN_1, TEST_SEC_1, 0, 0, 1, -23, 60 }, /*minutes are invalid*/
                { TEST_YEAR_1, TEST_MONTH_1, TEST_MDAY_1, TEST_HOUR_1, TEST_MIN_1, TEST_SEC_1, 0, 0, 1, +23, 60 }, /*minutes are invalid*/
                { TEST_YEAR_1, TEST_MONTH_1, TEST_MDAY_1, TEST_HOUR_1, TEST_MIN_1, TEST_SEC_1, 0, 0, 1, -24, 0 }, /*hour is invalid*/
                { TEST_YEAR_1, TEST_MONTH_1, TEST_MDAY_1, TEST_HOUR_1, TEST_MIN_1, TEST_SEC_1, 0, 0, 1, +24, 0 }, /*hour is invalid*/
            };

            {
                for (size_t i = 0; i < sizeof(testVectorShallFail) / sizeof(testVectorShallFail[0]); i++)
                {
                    EDM_DATE_TIME_OFFSET temp;
                    temp.dateTime.tm_year = testVectorShallFail[i].year-1900;
                    temp.dateTime.tm_mon = testVectorShallFail[i].month-1;
                    temp.dateTime.tm_mday = testVectorShallFail[i].day;
                    temp.dateTime.tm_hour = testVectorShallFail[i].hour;
                    temp.dateTime.tm_min = testVectorShallFail[i].min;
                    temp.dateTime.tm_sec = testVectorShallFail[i].sec;
                    temp.hasFractionalSecond = testVectorShallFail[i].hasFractionalSecond;
                    temp.fractionalSecond = testVectorShallFail[i].fractionalSecond;
                    temp.hasTimeZone = testVectorShallFail[i].hasTimeZone;
                    temp.timeZoneHour = testVectorShallFail[i].timeZoneHour;
                    temp.timeZoneMinute = testVectorShallFail[i].timeZoneMin;

                    ///act
                    auto result = Create_AGENT_DATA_TYPE_from_EDM_DATE_TIME_OFFSET(&agentData, temp);

                    ///assert
                    ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
                }
            }
        }

        TEST_FUNCTION(AgentDataTypes_ToString_EDM_DATE_TIME_OFFSET_succeeds)
        {
            ///arrange
            AGENT_DATA_TYPE agentData;
            
            
            for (size_t i = 0; i < sizeof(global_testVector) / sizeof(global_testVector[0]); i++)
            {
                ///arrange
                EDM_DATE_TIME_OFFSET temp;
                temp.dateTime.tm_year = global_testVector[i].year - 1900;
                temp.dateTime.tm_mon = global_testVector[i].month - 1;
                temp.dateTime.tm_mday = global_testVector[i].day;
                temp.dateTime.tm_hour = global_testVector[i].hour;
                temp.dateTime.tm_min = global_testVector[i].min;
                temp.dateTime.tm_sec = global_testVector[i].sec;
                temp.hasFractionalSecond = global_testVector[i].hasFractionalSecond;
                temp.fractionalSecond = global_testVector[i].fractionalSecond;
                temp.hasTimeZone= global_testVector[i].hasTimeZone;
                temp.timeZoneHour = global_testVector[i].timeZoneHour;
                temp.timeZoneMinute = global_testVector[i].timeZoneMin;
                STRING_empty(global_bufferTemp);
                auto resultCreate = Create_AGENT_DATA_TYPE_from_EDM_DATE_TIME_OFFSET(&agentData, temp);

                ///act
                auto resToString = AgentDataTypes_ToString(global_bufferTemp,  &agentData);

                ///assert
                ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, resultCreate);
                ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, resToString);
                ASSERT_ARE_EQUAL(char_ptr, global_testVector[i].expectedOutput, STRING_c_str(global_bufferTemp));

                ///cleanup
                Destroy_AGENT_DATA_TYPE(&agentData);
            }
        }

        TEST_FUNCTION(CreateAgentDataType_From_String_EDM_DATE_TIME_OFFSET_succeeds)
        {
            ///arrange
            AGENT_DATA_TYPE agentData;


            for (size_t i = 0; i < sizeof(global_testVector) / sizeof(global_testVector[0]); i++)
            {
                
                ///act
                auto resFromString = CreateAgentDataType_From_String(global_testVector[i].expectedOutput, EDM_DATE_TIME_OFFSET_TYPE, &agentData);

                ///assert
                ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, resFromString);
                ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_DATE_TIME_OFFSET_TYPE, agentData.type);
                ASSERT_ARE_EQUAL(int, global_testVector[i].year-1900, agentData.value.edmDateTimeOffset.dateTime.tm_year);
                ASSERT_ARE_EQUAL(int, global_testVector[i].month-1, agentData.value.edmDateTimeOffset.dateTime.tm_mon);
                ASSERT_ARE_EQUAL(int, global_testVector[i].day, agentData.value.edmDateTimeOffset.dateTime.tm_mday);
                ASSERT_ARE_EQUAL(int, global_testVector[i].hour, agentData.value.edmDateTimeOffset.dateTime.tm_hour);
                ASSERT_ARE_EQUAL(int, global_testVector[i].min, agentData.value.edmDateTimeOffset.dateTime.tm_min);
                ASSERT_ARE_EQUAL(int, global_testVector[i].sec, agentData.value.edmDateTimeOffset.dateTime.tm_sec);
                ASSERT_ARE_EQUAL(int, global_testVector[i].hasFractionalSecond, global_testVector[i].hasFractionalSecond);
                ASSERT_ARE_EQUAL(uint8_t, (uint8_t)global_testVector[i].fractionalSecond, (uint8_t)global_testVector[i].fractionalSecond);
                if (global_testVector[i].hasFractionalSecond)
                {
                    ASSERT_ARE_EQUAL(uint64_t, global_testVector[i].fractionalSecond, agentData.value.edmDateTimeOffset.fractionalSecond);
                }
                ASSERT_ARE_EQUAL(uint8_t, (uint8_t)global_testVector[i].hasTimeZone, (uint8_t)agentData.value.edmDateTimeOffset.hasTimeZone);
                if (global_testVector[i].hasTimeZone)
                {
                    ASSERT_ARE_EQUAL(int8_t, (int8_t)global_testVector[i].timeZoneHour, (int8_t)agentData.value.edmDateTimeOffset.timeZoneHour);
                    ASSERT_ARE_EQUAL(uint8_t, (uint8_t)global_testVector[i].timeZoneMin, (uint8_t)agentData.value.edmDateTimeOffset.timeZoneMinute);
                }
                ///cleanup
                Destroy_AGENT_DATA_TYPE(&agentData);
            }
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_013:[ All the functions shall check their parameters for validity. When an invalid parameter is detected, the value AGENT_DATA_TYPES_INVALID_ARG shall be returned ].*/
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_EDM_GUID_with_NULL_agent_data_type_fails)
        {
            ///arrange
            EDM_GUID guid = { { 0 } };

            ///act
            auto result = Create_AGENT_DATA_TYPE_from_EDM_GUID(NULL, guid);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_013:[ All the functions shall check their parameters for validity. When an invalid parameter is detected, the value AGENT_DATA_TYPES_INVALID_ARG shall be returned ].*/
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_EDM_BINARY_with_NULL_agent_data_type_fails)
        {
            ///arrange
            EDM_BINARY binary = { 0 };

            ///act
            auto result = Create_AGENT_DATA_TYPE_from_EDM_BINARY(NULL, binary);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_094:[ Creates and AGENT_DATA_TYPE containing a EDM_GUID from a memory buffer of 16 bytes.]*/
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_EDM_GUID_succeeds)
        {
            ///arrange
            EDM_GUID guid = { { 0 } };
            AGENT_DATA_TYPE ag;

            ///act
            auto result = Create_AGENT_DATA_TYPE_from_EDM_GUID(&ag, guid);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&ag);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_098:[ Creates an AGENT_DATA_TYPE containing a EDM_BINARY from a EDM_BINARY.]*/
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_EDM_BINARY_succeeds)
        {
            ///arrange
            unsigned char c = 123;
            EDM_BINARY binary= { 1, &c };
            AGENT_DATA_TYPE ag;

            ///act
            auto result = Create_AGENT_DATA_TYPE_from_EDM_BINARY(&ag, binary);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&ag);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_093:[ EDM_GUID: 8HEXDIG "-" 4HEXDIG "-" 4HEXDIG "-" 4HEXDIG "-" 12HEXDIG]*/
        TEST_FUNCTION(AgentDataTypes_ToString_GUID_succeeds)
        {
            ///arrange
            EDM_GUID guid = { { 0x21, 0xEC, 0x20, 0x20, 0x3A, 0xEA, 0x10, 0x69, 0xA2, 0xDD, 0x08, 0x00, 0x2B, 0x30, 0x30, 0x9D } };
            AGENT_DATA_TYPE ag;
            (void)Create_AGENT_DATA_TYPE_from_EDM_GUID(&ag, guid);

            ///act
            auto result = AgentDataTypes_ToString(global_bufferTemp, &ag);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(char_ptr, "\"21EC2020-3AEA-1069-A2DD-08002B30309D\"", STRING_c_str(global_bufferTemp));

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&ag);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_099:[ EDM_BINARY: = *(4base64char) [ base64b16  / base64b8 ]]*/
        TEST_FUNCTION(AgentDataTypes_ToString_BINARY_succeeds)
        {
            ///arrange
            AGENT_DATA_TYPE ag;
            
            for (size_t i = 0; i < sizeof(testVector_EDM_BINARY) / sizeof(testVector_EDM_BINARY[0]); i++)
            {
                ///arrange
                STRING_empty(global_bufferTemp);

                ///act
                auto res1 = Create_AGENT_DATA_TYPE_from_EDM_BINARY(&ag, testVector_EDM_BINARY[i].inputData);
                auto res2 = AgentDataTypes_ToString(global_bufferTemp, &ag);

                ///assert
                ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res1);
                ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res2);
                ASSERT_ARE_EQUAL(char_ptr, testVector_EDM_BINARY_with_equal_signs[i].expectedOutput, STRING_c_str(global_bufferTemp));

                ///cleanup
                Destroy_AGENT_DATA_TYPE(&ag);
            }
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_013:[ All the functions shall check their parameters for validity. When an invalid parameter is detected, the value AGENT_DATA_TYPES_INVALID_ARG shall be returned ]*/
        TEST_FUNCTION(AgentDataTypes_ToString_EDM_BINARY_with_insufficient_buffer_fails)
        {
            {
                ///arrange
                AGENT_DATA_TYPE ag;
                struct
                {
                    EDM_BINARY inputData;
                    const char* expectedOutput;
                } testVector[] =
                {
                    { { 1, (unsigned char *)"\x00" }, "AA" },
                    { { 1, (unsigned char *)"\xff" }, "_w" },
                    { { 2, (unsigned char *)"\xff\xff" }, "__8" },
                    { { 3, (unsigned char *)"\xeb\xeb\xeb" }, "6-vr" },
                    { { 4, (unsigned char *)"\xff\xff\xff\xff" }, "_____w" },
                    { { 5, (unsigned char *)"\xfc\xfc\xfc\xfc\xfc" }, "_Pz8_Pw" },
                    { { 6, (unsigned char *)"\xa8\xa8\xa8\xa8\xa8\xa8" }, "qKioqKio" },
                    { { 7, (unsigned char *)"\xd3\xd3\xd3\xd3\xd3\xd3\xd3" }, "09PT09PT0w" },
                    { { 8, (unsigned char *)"\x00\x00\x00\x00\x00\x00\x00\x00" }, "AAAAAAAAAAA" },
                    { { 9, (unsigned char *)"\x00\x00\x00\x00\x00\x00\x00\x00\x00" }, "AAAAAAAAAAAA" },
                    { { 10, (unsigned char *)"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" }, "AAAAAAAAAAAAAA" },
                };

                for (size_t i = 0; i < sizeof(testVector) / sizeof(testVector[0]); i++)
                {
                    ///act
                    auto res1 = Create_AGENT_DATA_TYPE_from_EDM_BINARY(&ag, testVector[i].inputData);

                    EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                        .SetReturn(1);
                    auto res2 = AgentDataTypes_ToString(global_bufferTemp,  &ag); /*notice here the missing buffer size*/

                    ///assert
                    ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, res1);
                    ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_ERROR, res2);

                    ///cleanup
                    Destroy_AGENT_DATA_TYPE(&ag);
                }
            }
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_016:[ When the value cannot be converted to a string AgentDataTypes_ToString shall return AGENT_DATA_TYPES_ERROR.]*/
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_AGENT_DATA_TYPE_with_a_GUID_succeeds)
        {
            ///arrange
            EDM_GUID guid = { { 0x21, 0xEC, 0x20, 0x20, 0x3A, 0xEA, 0x10, 0x69, 0xA2, 0xDD, 0x08, 0x00, 0x2B, 0x30, 0x30, 0x9E } };
            AGENT_DATA_TYPE src, dest;
            (void)Create_AGENT_DATA_TYPE_from_EDM_GUID(&src, guid);

            ///act
            auto result = Create_AGENT_DATA_TYPE_from_AGENT_DATA_TYPE(&dest, &src);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_GUID_TYPE, dest.type);
            ASSERT_ARE_EQUAL(int, 0, memcmp(src.value.edmGuid.GUID, dest.value.edmGuid.GUID, 16));

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&src);
            Destroy_AGENT_DATA_TYPE(&dest);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_016:[ When the value cannot be converted to a string AgentDataTypes_ToString shall return AGENT_DATA_TYPES_ERROR.]*/
        TEST_FUNCTION(Create_AGENT_DATA_TYPE_from_AGENT_DATA_TYPE_with_a_EDM_BINARY_succeeds)
        {
            ///arrange
            unsigned char a = 'a';
            EDM_BINARY rawSrc = { 1, &a };
            AGENT_DATA_TYPE src, dest;
            (void)Create_AGENT_DATA_TYPE_from_EDM_BINARY(&src, rawSrc);

            ///act
            auto result = Create_AGENT_DATA_TYPE_from_AGENT_DATA_TYPE(&dest, &src);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_BINARY_TYPE, dest.type);
            ASSERT_ARE_EQUAL(size_t, (size_t)1, dest.value.edmBinary.size);
            ASSERT_ARE_EQUAL(int, (int)'a', (int)dest.value.edmBinary.data[0]);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&src);
            Destroy_AGENT_DATA_TYPE(&dest);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_050:[ Destroy_AGENT_DATA_TYPE shall deallocate all allocated resources used to represent the type.]*/
        TEST_FUNCTION(Destroy_AGENT_DATA_TYPE_for_a_GUID_succeeds)
        {
            ///arrange
            EDM_GUID guid= { { 0x21, 0xEC, 0x20, 0x20, 0x3A, 0xEA, 0x10, 0x69, 0xA2, 0xDD, 0x08, 0x00, 0x2B, 0x30, 0x30, 0x9E } };
            AGENT_DATA_TYPE ag;
            (void)Create_AGENT_DATA_TYPE_from_EDM_GUID(&ag, guid);

            ///act
            Destroy_AGENT_DATA_TYPE(&ag);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_NO_TYPE, ag.type);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_050:[ Destroy_AGENT_DATA_TYPE shall deallocate all allocated resources used to represent the type.]*/
        TEST_FUNCTION(Destroy_AGENT_DATA_TYPE_for_a_EDM_BINARY_succeeds)
        {
            ///arrange
            int some_int = 42;
            EDM_BINARY src = { sizeof(int), (unsigned char*)&some_int };
            AGENT_DATA_TYPE ag;
            (void)Create_AGENT_DATA_TYPE_from_EDM_BINARY(&ag, src);

            ///act
            Destroy_AGENT_DATA_TYPE(&ag);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_NO_TYPE, ag.type);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_097:[ EDM_GUID]*/
        TEST_FUNCTION(CreateAgentDataType_From_String_for_a_GUID_succeeds)
        {
            ///arrange
            const char* guidAsString = "\"21EC2020-3AEA-1069-A2DD-08002B30309D\"";
            AGENT_DATA_TYPE ag;

            ///act
            auto result = CreateAgentDataType_From_String(guidAsString, EDM_GUID_TYPE, &ag);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_GUID_TYPE, ag.type);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)0x21, ag.value.edmGuid.GUID[0]);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)0xEC, ag.value.edmGuid.GUID[1]);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)0x20, ag.value.edmGuid.GUID[2]);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)0x20, ag.value.edmGuid.GUID[3]);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)0x3A, ag.value.edmGuid.GUID[4]);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)0xEA, ag.value.edmGuid.GUID[5]);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)0x10, ag.value.edmGuid.GUID[6]);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)0x69, ag.value.edmGuid.GUID[7]);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)0xA2, ag.value.edmGuid.GUID[8]);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)0xDD, ag.value.edmGuid.GUID[9]);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)0x08, ag.value.edmGuid.GUID[10]);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)0x00, ag.value.edmGuid.GUID[11]);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)0x2B, ag.value.edmGuid.GUID[12]);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)0x30, ag.value.edmGuid.GUID[13]);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)0x30, ag.value.edmGuid.GUID[14]);
            ASSERT_ARE_EQUAL(uint8_t, (uint8_t)0x9D, ag.value.edmGuid.GUID[15]);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&ag);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_100:[ EDM_BINARY]*/
        TEST_FUNCTION(CreateAgentDataType_From_String_for_a_EDM_BINARY_succeeds)
        {
            ///arrange
            size_t i;
            AGENT_DATA_TYPE ag;

            for (i = 0; i < sizeof(testVector_EDM_BINARY) / sizeof(testVector_EDM_BINARY[0]); i++)
            {
                ///act
                auto result = CreateAgentDataType_From_String(testVector_EDM_BINARY[i].expectedOutput, EDM_BINARY_TYPE, &ag);

                ///assert
                ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
                ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_BINARY_TYPE, ag.type);
                ASSERT_ARE_EQUAL(size_t, (size_t)testVector_EDM_BINARY[i].inputData.size, ag.value.edmBinary.size);
                ASSERT_ARE_EQUAL(int, 0, memcmp(testVector_EDM_BINARY[i].inputData.data, ag.value.edmBinary.data, testVector_EDM_BINARY[i].inputData.size));

                ///cleanup
                Destroy_AGENT_DATA_TYPE(&ag);
            }
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_100:[ EDM_BINARY]*/
        TEST_FUNCTION(CreateAgentDataType_From_String_for_a_EDM_BINARY_ending_with_equal_signs_succeeds)
        {
            ///arrange
            size_t i;
            AGENT_DATA_TYPE ag;

            for (i = 0; i < sizeof(testVector_EDM_BINARY_with_equal_signs) / sizeof(testVector_EDM_BINARY_with_equal_signs[0]); i++)
            {
                ///act
                auto result = CreateAgentDataType_From_String(testVector_EDM_BINARY_with_equal_signs[i].expectedOutput, EDM_BINARY_TYPE, &ag);

                ///assert
                ASSERT_ARE_EQUAL_WITH_MSG(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result, (std::to_string(i)).c_str());
                ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_BINARY_TYPE, ag.type);
                ASSERT_ARE_EQUAL(size_t, (size_t)testVector_EDM_BINARY_with_equal_signs[i].inputData.size, ag.value.edmBinary.size);
                ASSERT_ARE_EQUAL(int, 0, memcmp(testVector_EDM_BINARY_with_equal_signs[i].inputData.data, ag.value.edmBinary.data, testVector_EDM_BINARY_with_equal_signs[i].inputData.size));

                ///cleanup
                Destroy_AGENT_DATA_TYPE(&ag);
            }
        }

        /*validating base64 decode with invalid base64 encoded strings*/
        TEST_FUNCTION(CreateAgentDataType_From_String_for_a_EDM_BINARY_when_input_string_contains_1_garbage_character_inserted_fails)
        {
            ///arrange
            size_t i;
            AGENT_DATA_TYPE ag;
            char bufferTemp[1000];
            /* the below loops use some steps so that we do not run through all variants exhaustively
            so that the test is kept within some decent runtime boundaries*/
            for (i = 0; i < sizeof(testVector_EDM_BINARY) / sizeof(testVector_EDM_BINARY[0]); i+=9)
            {
                ///arrange
                int garbageCharacter;
                for (garbageCharacter = 1; garbageCharacter <= 255; garbageCharacter+=5)
                {
                    if (
                        (('A' <= garbageCharacter) && (garbageCharacter <= 'Z')) ||
                        (('a' <= garbageCharacter) && (garbageCharacter <= 'z')) ||
                        (('0' <= garbageCharacter) && (garbageCharacter <= '9')) ||
                        ('-' == garbageCharacter) ||
                        ('_' == garbageCharacter) ||
                        ('=' == garbageCharacter)
                        )
                    {
                        continue;
                    }
                    size_t garbageCharacterPosition; /*means "insert garbageCharacter before the character in garbageCharacterPosition index*/
                    for (garbageCharacterPosition = 0; garbageCharacterPosition < strlen(testVector_EDM_BINARY[i].expectedOutput); garbageCharacterPosition++)
                    {
                        memcpy(bufferTemp, testVector_EDM_BINARY[i].expectedOutput, garbageCharacterPosition);
                        bufferTemp[garbageCharacterPosition] = (char)garbageCharacter;
                        memcpy(bufferTemp + garbageCharacterPosition + 1, testVector_EDM_BINARY[i].expectedOutput + garbageCharacterPosition, strlen(testVector_EDM_BINARY[i].expectedOutput) - garbageCharacterPosition + 1);

                        ///act
                        auto result = CreateAgentDataType_From_String(bufferTemp, EDM_BINARY_TYPE, &ag);

                        ///assert

                        if (AGENT_DATA_TYPES_INVALID_ARG != result) /*time optimization - without this "if the test takes 23 seconds. With it takes 4 seconds*/
                        {
                            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
                        }
                    }
                }
            }
        }

        /*validating base64 decode with invalid base64 encoded strings*/
        TEST_FUNCTION(CreateAgentDataType_From_String_for_a_EDM_BINARY_when_input_string_contains_1_garbage_character_inserted_at_the_end_of_the_string_fails)
        {
            ///arrange
            size_t i;
            AGENT_DATA_TYPE ag;
            char bufferTemp[1000];

            /* the below loops use some steps so that we do not run through all variants exhaustively
            so that the test is kept within some decent runtime boundaries*/
            for (i = 0; i < sizeof(testVector_EDM_BINARY) / sizeof(testVector_EDM_BINARY[0]); i += 10)
            {
                ///arrange
                int garbageCharacter;
                for (garbageCharacter = 1; garbageCharacter <= 255; garbageCharacter+=7)
                {
                    if (
                        (('A' <= garbageCharacter) && (garbageCharacter <= 'Z')) ||
                        (('a' <= garbageCharacter) && (garbageCharacter <= 'z')) ||
                        (('0' <= garbageCharacter) && (garbageCharacter <= '9')) ||
                        ('-' == garbageCharacter) ||
                        ('_' == garbageCharacter) ||
                        ('=' == garbageCharacter)
                        )
                    {
                        continue;
                    }

                    memcpy(bufferTemp, testVector_EDM_BINARY[i].expectedOutput, strlen(testVector_EDM_BINARY[i].expectedOutput));
                    bufferTemp[strlen(testVector_EDM_BINARY[i].expectedOutput)] = (char)garbageCharacter;
                    bufferTemp[strlen(testVector_EDM_BINARY[i].expectedOutput)+1] = '\0';

                    ///act
                    auto result = CreateAgentDataType_From_String(bufferTemp, EDM_BINARY_TYPE, &ag);

                    ///assert
                    ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
                }
            }
        }

        /*validating base64 decode with invalid base64 encoded strings*/
        TEST_FUNCTION(CreateAgentDataType_From_String_for_a_EDM_BINARY_when_input_string_contains_1_replaced_with_garbage_character_fails)
        {
            ///arrange
            size_t i;
            AGENT_DATA_TYPE ag;
            char bufferTemp[1000];

            /* the below loops use some steps so that we do not run through all variants exhaustively
            so that the test is kept within some decent runtime boundaries*/
            for (i = 0; i < sizeof(testVector_EDM_BINARY_with_equal_signs) / sizeof(testVector_EDM_BINARY_with_equal_signs[0]); i += 20)
            {
                size_t replacedCharacterPosition;
                for (replacedCharacterPosition = 0; replacedCharacterPosition < strlen(testVector_EDM_BINARY_with_equal_signs[i].expectedOutput); replacedCharacterPosition+=3)
                {
                    int garbageCharacter;
                    for (garbageCharacter = 1; garbageCharacter <= 255; garbageCharacter += 7)
                    {
                        if (
                            (('A' <= garbageCharacter) && (garbageCharacter <= 'Z')) ||
                            (('a' <= garbageCharacter) && (garbageCharacter <= 'z')) ||
                            (('0' <= garbageCharacter) && (garbageCharacter <= '9')) ||    /*these aren't really garbage characters*/
                            ('-' == garbageCharacter) ||
                            ('_' == garbageCharacter) ||
                            ('=' == garbageCharacter) ||
                            ('"' == garbageCharacter)
                            )
                        {
                            continue;
                        }

                        strcpy(bufferTemp, testVector_EDM_BINARY_with_equal_signs[i].expectedOutput);
                        bufferTemp[replacedCharacterPosition] = (char)garbageCharacter;
                        ///act
                        auto result = CreateAgentDataType_From_String(bufferTemp, EDM_BINARY_TYPE, &ag);

                        ///assert
                        ASSERT_ARE_EQUAL_WITH_MSG(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result, (std::to_string(i) + " " + std::to_string(replacedCharacterPosition)+ " " + std::to_string((int)garbageCharacter) + " ").c_str());
                    }
                }
            }
        }

        /*validating base64 decode with valid base64 encoded strings*/
        TEST_FUNCTION(CreateAgentDataType_From_String_for_a_EDM_BINARY_withjust_quotes_succeeds)
        {
            ///arrange
            AGENT_DATA_TYPE ag;

            ///act
            auto result = CreateAgentDataType_From_String("\"\"", EDM_BINARY_TYPE, &ag);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPE_TYPE, EDM_BINARY_TYPE, ag.type);
            ASSERT_ARE_EQUAL(size_t, (size_t)0, ag.value.edmBinary.size);
            ASSERT_ARE_EQUAL(void_ptr, (void*)NULL, (void*)ag.value.edmBinary.data);

            ///cleanup
            Destroy_AGENT_DATA_TYPE(&ag);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_097:[ EDM_GUID]*/
        TEST_FUNCTION(CreateAgentDataType_with_not_enough_characters_for_a_GUID_fails)
        {
            ///arrange
            const char* guidAsString = "\"21EC2020-3AEA-1069-A2DD-08002B30309\""; /*notice missing "D" from test above*/
            AGENT_DATA_TYPE ag;

            ///act
            auto result = CreateAgentDataType_From_String(guidAsString, EDM_GUID_TYPE, &ag);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_097:[ EDM_GUID]*/
        TEST_FUNCTION(CreateAgentDataType_with_too_many_characters_for_a_GUID_fails)
        {
            ///arrange
            const char* guidAsString = "\"21EC2020-3AEA-1069-A2DD-08002B30309DD\""; /*notice additional "D" from test GUID*/
            AGENT_DATA_TYPE ag;

            ///act
            auto result = CreateAgentDataType_From_String(guidAsString, EDM_GUID_TYPE, &ag);

            ///assert
            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
        }

        /*Tests_SRS_AGENT_TYPE_SYSTEM_99_097:[ EDM_GUID]*/
        TEST_FUNCTION(CreateAgentDataType_with_wrong_characters_fails)
        {
            ///arrange
            
            const char* badGuids[] = {
                /* good guid  "\"21EC2020-3AEA-1069-A2DD-08002B30309D\""                                                  */
                "a21EC2020-3AEA-1069-A2DD-08002B30309D\""
                "\"a1EC2020-3AEA-1069-A2DD-08002B30309D\"", /*[1] is non-hex ('a' is not the same with 'A' in OData)*/
                "\"2aEC2020-3AEA-1069-A2DD-08002B30309D\"", /*[2] is non-hex*/
                "\"21aC2020-3AEA-1069-A2DD-08002B30309D\"", /*...*/
                "\"21Ea2020-3AEA-1069-A2DD-08002B30309D\"",
                "\"21ECa020-3AEA-1069-A2DD-08002B30309D\"",
                "\"21EC2a20-3AEA-1069-A2DD-08002B30309D\"",
                "\"21EC20a0-3AEA-1069-A2DD-08002B30309D\"",
                "\"21EC202a-3AEA-1069-A2DD-08002B30309D\"",
                "\"21EC2020a3AEA-1069-A2DD-08002B30309D\"" /*- missing and being replaced by "a"*/
                "\"21EC2020-aAEA-1069-A2DD-08002B30309D\"",
                "\"21EC2020-3aEA-1069-A2DD-08002B30309D\"",
                "\"21EC2020-3AaA-1069-A2DD-08002B30309D\"",
                "\"21EC2020-3AEa-1069-A2DD-08002B30309D\"",
                "\"21EC2020-3AEAa1069-A2DD-08002B30309D\"", /*- missing and being replaced by "a"*/
                "\"21EC2020-3AEA-a069-A2DD-08002B30309D\"",
                "\"21EC2020-3AEA-1a69-A2DD-08002B30309D\"",
                "\"21EC2020-3AEA-10a9-A2DD-08002B30309D\"",
                "\"21EC2020-3AEA-106a-A2DD-08002B30309D\"",
                "\"21EC2020-3AEA-1069aA2DD-08002B30309D\"",
                "\"21EC2020-3AEA-1069-a2DD-08002B30309D\"",
                "\"21EC2020-3AEA-1069-AaDD-08002B30309D\"",
                "\"21EC2020-3AEA-1069-A2aD-08002B30309D\"",
                "\"21EC2020-3AEA-1069-A2Da-08002B30309D\"",
                "\"21EC2020-3AEA-1069-A2DDa08002B30309D\""
                "\"21EC2020-3AEA-1069-A2DD-a8002B30309D\"",
                "\"21EC2020-3AEA-1069-A2DD-0a002B30309D\"",
                "\"21EC2020-3AEA-1069-A2DD-08a02B30309D\"",
                "\"21EC2020-3AEA-1069-A2DD-080a2B30309D\"",
                "\"21EC2020-3AEA-1069-A2DD-0800aB30309D\"",
                "\"21EC2020-3AEA-1069-A2DD-08002a30309D\"",
                "\"21EC2020-3AEA-1069-A2DD-08002Ba0309D\"",
                "\"21EC2020-3AEA-1069-A2DD-08002B3a309D\"",
                "\"21EC2020-3AEA-1069-A2DD-08002B30a09D\"",
                "\"21EC2020-3AEA-1069-A2DD-08002B303a9D\"",
                "\"21EC2020-3AEA-1069-A2DD-08002B3030aD\"",
                "\"21EC2020-3AEA-1069-A2DD-08002B30309a\"",
                "\"21EC2020-3AEA-1069-A2DD-08002B30309Da",
                /*these are tests that have a capital non-hex digit, such as 'H'*/
                "H21EC2020-AAEA-1069-A2DD-08002B30309D\"",
                "\"H1EC2020-AAEA-1069-A2DD-08002B30309D\"",
                "\"2HEC2020-AAEA-1069-A2DD-08002B30309D\"",
                "\"21HC2020-AAEA-1069-A2DD-08002B30309D\"",
                "\"21EH2020-AAEA-1069-A2DD-08002B30309D\"",
                "\"21ECH020-AAEA-1069-A2DD-08002B30309D\"",
                "\"21EC2H20-AAEA-1069-A2DD-08002B30309D\"",
                "\"21EC20H0-AAEA-1069-A2DD-08002B30309D\"",
                "\"21EC202H-AAEA-1069-A2DD-08002B30309D\"",
                "\"21EC2020HAAEA-1069-A2DD-08002B30309D\"",
                "\"21EC2020-HAEA-1069-A2DD-08002B30309D\"",
                "\"21EC2020-AHEA-1069-A2DD-08002B30309D\"",
                "\"21EC2020-AAHA-1069-A2DD-08002B30309D\"",
                "\"21EC2020-AAEH-1069-A2DD-08002B30309D\"",
                "\"21EC2020-AAEAH1069-A2DD-08002B30309D\"",
                "\"21EC2020-AAEA-H069-A2DD-08002B30309D\"",
                "\"21EC2020-AAEA-1H69-A2DD-08002B30309D\"",
                "\"21EC2020-AAEA-10H9-A2DD-08002B30309D\"",
                "\"21EC2020-AAEA-106H-A2DD-08002B30309D\"",
                "\"21EC2020-AAEA-1069HA2DD-08002B30309D\"",
                "\"21EC2020-AAEA-1069-H2DD-08002B30309D\"",
                "\"21EC2020-AAEA-1069-AHDD-08002B30309D\"",
                "\"21EC2020-AAEA-1069-A2HD-08002B30309D\"",
                "\"21EC2020-AAEA-1069-A2DH-08002B30309D\"",
                "\"21EC2020-AAEA-1069-A2DDH08002B30309D\"",
                "\"21EC2020-AAEA-1069-A2DD-H8002B30309D\"",
                "\"21EC2020-AAEA-1069-A2DD-0H002B30309D\"",
                "\"21EC2020-AAEA-1069-A2DD-08H02B30309D\"",
                "\"21EC2020-AAEA-1069-A2DD-080H2B30309D\"",
                "\"21EC2020-AAEA-1069-A2DD-0800HB30309D\"",
                "\"21EC2020-AAEA-1069-A2DD-08002H30309D\"",
                "\"21EC2020-AAEA-1069-A2DD-08002BH0309D\"",
                "\"21EC2020-AAEA-1069-A2DD-08002B3H309D\"",
                "\"21EC2020-AAEA-1069-A2DD-08002B30H09D\"",
                "\"21EC2020-AAEA-1069-A2DD-08002B303H9D\"",
                "\"21EC2020-AAEA-1069-A2DD-08002B3030HD\"",
                "\"21EC2020-AAEA-1069-A2DD-08002B30309H\"",
                "\"21EC2020-AAEA-1069-A2DD-08002B30309DH",
            };
            AGENT_DATA_TYPE ag;


            for (size_t i = 0; i < sizeof(badGuids) / sizeof(badGuids[0]); i++)
            {
                ///act
                auto result = CreateAgentDataType_From_String(badGuids[i], EDM_GUID_TYPE, &ag);

                ///assert
                ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
            }
        }

        static bool isLeapYear(int year)
        {
            if (year % 400 == 0)
            {
                return true;
            }
            else
            {
                if (year % 4 == 0)
                {
                    if (year % 100 == 0)
                    {
                        return false;
                    }
                    else
                    {
                        return true;
                    }
                }
                else
                {
                    return false;
                }
            }
        }

        TEST_FUNCTION(AgentTypeSystem_CreateAgentDataType_From_String_EDM_DATE_TIME_OFFSET_All_Years_Fills_In_tm_yday_and_tm_wday)
        {
            AGENT_DATA_TYPE agentData;
            char bufferTemp[1000];
            const int daysInMonth[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

            int continousDays = 1; /*first day of the calendar was a ... MONDAY*/

            for (int year = -9999; year <= 9999; year++) //full ODATA range
            {
                int yday = 0;
                for (int month = 1; month <= 12; month++) //full month range 
                {
                    int days[3]; /*test the first day, some day in the middle, and the last day of the month*/
                    days[0] = 1;
                    days[1] = 10;
                    days[2] = daysInMonth[month - 1] + (((month == 2) && isLeapYear(year)) ? 1 : 0);
                    for (int index_day = 0; index_day <= 2; index_day++)
                    {
                        ///arrange
                        int day = days[index_day];
                        struct tm witness = { 0 };
                        witness.tm_year = year - 1900;
                        witness.tm_mon = month - 1;
                        witness.tm_mday = day;
                        witness.tm_hour = 12;
                        witness.tm_min = 0;
                        witness.tm_sec = 0;
                        //the below code is equivalent to:
                        //sprintf(bufferTemp, "\"%4.4d-%02d-%02dT12:00Z\"", year, month, day);
                        size_t pos = 0;
                        bufferTemp[pos++] = '"';
                        int yearCopy = year;
                        if (year < 0)
                        {
                            yearCopy = -year;
                            bufferTemp[pos++] = '-';
                        }
                        bufferTemp[pos++] = (char)('0' + yearCopy / 1000);
                        bufferTemp[pos++] = (char)('0' + (yearCopy % 1000) / 100);
                        bufferTemp[pos++] = '0' + (yearCopy % 100) / 10;
                        bufferTemp[pos++] = '0' + (yearCopy % 10);
                        bufferTemp[pos++] = '-';
                        bufferTemp[pos++] = (char)('0' + (month / 10));
                        bufferTemp[pos++] = '0' + (month % 10);
                        bufferTemp[pos++] = '-';
                        bufferTemp[pos++] = (char)('0' + (day / 10));
                        bufferTemp[pos++] = '0' + (day % 10);
                        bufferTemp[pos++] = 'T';
                        bufferTemp[pos++] = '1';
                        bufferTemp[pos++] = '2';
                        bufferTemp[pos++] = ':';
                        bufferTemp[pos++] = '0';
                        bufferTemp[pos++] = '0';
                        bufferTemp[pos++] = 'Z';
                        bufferTemp[pos++] = '"';
                        bufferTemp[pos++] = '\0';

                        ///act
                        auto result = CreateAgentDataType_From_String(bufferTemp, EDM_DATE_TIME_OFFSET_TYPE, &agentData);

                        ///assert
                        if (AGENT_DATA_TYPES_OK != result) /*if because time optimization*/
                        {
                            ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result/*, "year=" + std::to_string(year) + " month= " + std::to_string(month) + " day = " + std::to_string(day)*/);
                        }

                        if (yday != agentData.value.edmDateTimeOffset.dateTime.tm_yday)
                        {
                            ASSERT_ARE_EQUAL(int, yday, agentData.value.edmDateTimeOffset.dateTime.tm_yday/*, "year=" + std::to_string(year) + " month= " + std::to_string(month) + " day = " + std::to_string(day)*/);
                        }

                        if ((continousDays % 7) != agentData.value.edmDateTimeOffset.dateTime.tm_wday)
                        {
                           ASSERT_ARE_EQUAL(int, continousDays % 7, agentData.value.edmDateTimeOffset.dateTime.tm_wday);
                        }

                        ///errr... go on!

                        if (index_day == 0)
                        {
                            yday += (days[1] - days[0]);
                            continousDays += (days[1] - days[0]);
                        }
                        else if(index_day == 1)
                        {
                            yday += (days[2] - days[1]);
                            continousDays += (days[2] - days[1]);
                        }
                        else
                        {
                            yday ++;
                            continousDays ++;
                        }

                        ///cleanup
                        Destroy_AGENT_DATA_TYPE(&agentData);
                    }
                }
            }
        }


END_TEST_SUITE(AgentTypeSystem_ut)
