// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <cstdlib>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include <ostream>
#include "testrunnerswitcher.h"
#include "datamarshaller.h"
#include "jsonencoder.h"
#include "multitree.h"
#include "schema.h"
#include "micromock.h"
#include "micromockcharstararenullterminatedstrings.h"
#include "azure_c_shared_utility/strings.h"

static MICROMOCK_MUTEX_HANDLE g_testByTest;

DEFINE_MICROMOCK_ENUM_TO_STRING(DATA_MARSHALLER_RESULT, DATA_MARSHALLER_RESULT_VALUES);

#define DEFAULT_ENTITY_SET_NAME_SPACE "defaultEntitySetNameSpace"
#define DEFAULT_ENTITY_SET_NAME "defaultEntitySetName"

static const SCHEMA_MODEL_TYPE_HANDLE TEST_MODEL_HANDLE = (SCHEMA_MODEL_TYPE_HANDLE)0x4242;
static const SCHEMA_HANDLE TEST_SCHEMA_HANDLE = (SCHEMA_HANDLE)0x4243;

#define DEFAULT_PROPERTY_NAME "defaultPropertyName"
#define DEFAULT_PROPERTY_NAME_2 "blahBlah"

#define DEFAULT_TIMESTAMP_STR "SomeTimeSTamp T 10:52:32Z"

static STRING_HANDLE DEFAULT_ENCODED_VALUES = NULL;
static const size_t DEFAULT_ENCODED_VALUES_LENGTH = sizeof(DEFAULT_ENCODED_VALUES) - 1;

static STRING_HANDLE DEFAULT_ENCODED_PAYLOAD;
static const size_t DEFAULT_ENCODED_PAYLOAD_LENGTH = sizeof(DEFAULT_ENCODED_PAYLOAD) - 1;

static const char* DEFAULT_JSON_ENCODED_VALUES = "{ Some JSON values }";

static AGENT_DATA_TYPE floatValid;
static AGENT_DATA_TYPE intValid;
static AGENT_DATA_TYPE structTypeValue;
static AGENT_DATA_TYPE structTypeValue2Members;
static const char* floatValidAsCharArray = "10.500000"; /*depends on FLT_DIG of the platform*/
static time_t currentTime;

std::ostream& operator<<(std::ostream& left, EDM_DATE_TIME_OFFSET dateTimeOffset)
{
    return left << "struct tm = (" <<
        dateTimeOffset.dateTime.tm_year << "-" <<
        dateTimeOffset.dateTime.tm_mon << "-" <<
        dateTimeOffset.dateTime.tm_mday << " " <<
        dateTimeOffset.dateTime.tm_hour << ":" <<
        dateTimeOffset.dateTime.tm_min << ":" <<
        dateTimeOffset.dateTime.tm_sec << " ) " <<
        dateTimeOffset.hasFractionalSecond << " " <<
        dateTimeOffset.fractionalSecond << " " <<
        dateTimeOffset.hasTimeZone << " " <<
        dateTimeOffset.timeZoneHour << " " <<
        dateTimeOffset.timeZoneMinute;
}

std::wostream& operator<<(std::wostream& left, EDM_DATE_TIME_OFFSET dateTimeOffset)
{
    return left << L"struct tm = (" <<
        dateTimeOffset.dateTime.tm_year << L"-" <<
        dateTimeOffset.dateTime.tm_mon << L"-" <<
        dateTimeOffset.dateTime.tm_mday << L" " <<
        dateTimeOffset.dateTime.tm_hour << L":" <<
        dateTimeOffset.dateTime.tm_min << L":" <<
        dateTimeOffset.dateTime.tm_sec << L" ) " <<
        dateTimeOffset.hasFractionalSecond << L" " <<
        dateTimeOffset.fractionalSecond << L" " <<
        dateTimeOffset.hasTimeZone << L" " <<
        dateTimeOffset.timeZoneHour << L" " <<
        dateTimeOffset.timeZoneMinute;
}


static bool operator==(EDM_DATE_TIME_OFFSET left, EDM_DATE_TIME_OFFSET right)
{
    return memcmp(&left, &right, sizeof(left)) == 0;
}

static struct tm someStructTm;

#define DEFAULT_JSON_ENCODER_PAYLOAD_LENGTH 10
#define TEST_JSON_ENCODER_HANDLE_0x42 (void*)0x42
static MULTITREE_HANDLE TEST_MULTITREE_HANDLE = (MULTITREE_HANDLE)0x4442;
static MULTITREE_HANDLE TEST_ENVELOPE_MULTITREE_HANDLE = (MULTITREE_HANDLE)0x4443;

#define GBALLOC_H
namespace BASEIMPLEMENTATION
{
#include "strings.c"
};

static size_t currentSTRING_new_call;
static size_t whenShallSTRING_new_fail;

static size_t nSTRING_new_calls = 0;
static size_t nSTRING_delete_calls = 0;

TYPED_MOCK_CLASS(CDataMarshallerMocks, CGlobalMock)
{
public:

    /* JSONEncoder mocks */
    MOCK_STATIC_METHOD_4(, JSON_ENCODER_RESULT, JSONEncoder_EncodeTree, MULTITREE_HANDLE, treeHandle, char*, buffer, size_t*, byteCount, JSON_ENCODER_TOSTRING_FUNC, toStringFunc)
    MOCK_METHOD_END(JSON_ENCODER_RESULT, JSON_ENCODER_OK)
    MOCK_STATIC_METHOD_3(, JSON_ENCODER_TOSTRING_RESULT, JSONEncoder_CharPtr_ToString, char*, destination, size_t, destinationSize, const void*, value)
    MOCK_METHOD_END(JSON_ENCODER_TOSTRING_RESULT, JSON_ENCODER_TOSTRING_OK)

    /* MultiTree mocks */
    MOCK_STATIC_METHOD_2(, MULTITREE_HANDLE, MultiTree_Create, MULTITREE_CLONE_FUNCTION, cloneFunction, MULTITREE_FREE_FUNCTION, freeFunction)
    MOCK_METHOD_END(MULTITREE_HANDLE, TEST_MULTITREE_HANDLE)
    MOCK_STATIC_METHOD_3(, MULTITREE_RESULT, MultiTree_AddLeaf, MULTITREE_HANDLE, treeHandle, const char*, destinationPath, const void*, value)
    MOCK_METHOD_END(MULTITREE_RESULT, MULTITREE_OK)
    MOCK_STATIC_METHOD_1(, void, MultiTree_Destroy, MULTITREE_HANDLE, treeHandle)
    MOCK_VOID_METHOD_END()

    /* AgentTypeSystem mocks */
    MOCK_STATIC_METHOD_2(, AGENT_DATA_TYPES_RESULT, Create_AGENT_DATA_TYPE_from_charz, AGENT_DATA_TYPE*, agentData, const char*, v)
    MOCK_METHOD_END(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK)
    MOCK_STATIC_METHOD_1(, void, Destroy_AGENT_DATA_TYPE, AGENT_DATA_TYPE*, agentData)
    MOCK_VOID_METHOD_END()
    MOCK_STATIC_METHOD_2(, AGENT_DATA_TYPES_RESULT, AgentDataTypes_ToString, STRING_HANDLE, destination, const AGENT_DATA_TYPE*, value)
    MOCK_METHOD_END(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK)

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

    MOCK_STATIC_METHOD_1(, const char*, STRING_c_str, STRING_HANDLE, s)
    MOCK_METHOD_END(const char*, BASEIMPLEMENTATION::STRING_c_str(s))

    MOCK_STATIC_METHOD_1(, size_t, STRING_length, STRING_HANDLE, s)
    MOCK_METHOD_END(size_t, BASEIMPLEMENTATION::STRING_length(s))

    /* JSONEncoder mocks */
    MOCK_STATIC_METHOD_3(, JSON_ENCODER_RESULT, JSONEncoder_EncodeTree, MULTITREE_HANDLE, treeHandle, STRING_HANDLE, buffer, JSON_ENCODER_TOSTRING_FUNC, toStringFunc)
    MOCK_METHOD_END(JSON_ENCODER_RESULT, JSON_ENCODER_OK)
    MOCK_STATIC_METHOD_2(, JSON_ENCODER_TOSTRING_RESULT, JSONEncoder_CharPtr_ToString, STRING_HANDLE, destination, const void*, value)
    MOCK_METHOD_END(JSON_ENCODER_TOSTRING_RESULT, JSON_ENCODER_TOSTRING_OK)

};


DECLARE_GLOBAL_MOCK_METHOD_3(CDataMarshallerMocks, , JSON_ENCODER_RESULT, JSONEncoder_EncodeTree, MULTITREE_HANDLE, treeHandle, STRING_HANDLE, buffer, JSON_ENCODER_TOSTRING_FUNC, toStringFunc);
DECLARE_GLOBAL_MOCK_METHOD_2(CDataMarshallerMocks, , JSON_ENCODER_TOSTRING_RESULT, JSONEncoder_CharPtr_ToString, STRING_HANDLE, destination, const void*, value);

DECLARE_GLOBAL_MOCK_METHOD_0(CDataMarshallerMocks, , STRING_HANDLE, STRING_new);
DECLARE_GLOBAL_MOCK_METHOD_1(CDataMarshallerMocks, , void, STRING_delete, STRING_HANDLE, s);
DECLARE_GLOBAL_MOCK_METHOD_1(CDataMarshallerMocks, , const char*, STRING_c_str, STRING_HANDLE, s);
DECLARE_GLOBAL_MOCK_METHOD_1(CDataMarshallerMocks, , size_t, STRING_length, STRING_HANDLE, s);

DECLARE_GLOBAL_MOCK_METHOD_2(CDataMarshallerMocks, , MULTITREE_HANDLE, MultiTree_Create, MULTITREE_CLONE_FUNCTION, cloneFunction, MULTITREE_FREE_FUNCTION, freeFunction);
DECLARE_GLOBAL_MOCK_METHOD_3(CDataMarshallerMocks, , MULTITREE_RESULT, MultiTree_AddLeaf, MULTITREE_HANDLE, treeHandle, const char*, destinationPath, const void*, value);
DECLARE_GLOBAL_MOCK_METHOD_1(CDataMarshallerMocks, , void, MultiTree_Destroy, MULTITREE_HANDLE, treeHandle);

DECLARE_GLOBAL_MOCK_METHOD_2(CDataMarshallerMocks, , AGENT_DATA_TYPES_RESULT, Create_AGENT_DATA_TYPE_from_charz, AGENT_DATA_TYPE*, agentData, const char*, v);
DECLARE_GLOBAL_MOCK_METHOD_1(CDataMarshallerMocks, , void, Destroy_AGENT_DATA_TYPE, AGENT_DATA_TYPE*, agentData);
DECLARE_GLOBAL_MOCK_METHOD_2(CDataMarshallerMocks, , AGENT_DATA_TYPES_RESULT, AgentDataTypes_ToString, STRING_HANDLE, destination, const AGENT_DATA_TYPE*, value);


static MICROMOCK_GLOBAL_SEMAPHORE_HANDLE g_dllByDll;

COMPLEX_TYPE_FIELD_TYPE members = { "x", &floatValid };
COMPLEX_TYPE_FIELD_TYPE two_members[] = { { "x", &floatValid }, { "y", &intValid } };

BEGIN_TEST_SUITE(DataMarshaller_ut)

        TEST_SUITE_INITIALIZE(TestClassInitialize)
        {
            TEST_INITIALIZE_MEMORY_DEBUG(g_dllByDll);
            g_testByTest = MicroMockCreateMutex();
            ASSERT_IS_NOT_NULL(g_testByTest);
            floatValid.type = EDM_SINGLE_TYPE;
            floatValid.value.edmSingle.value = 10.5;
            intValid.type = EDM_INT32_TYPE;
            intValid.value.edmInt32.value = 10;
            structTypeValue.type = EDM_COMPLEX_TYPE_TYPE;
            structTypeValue.value.edmComplexType.nMembers = 1;
            structTypeValue2Members.value.edmComplexType.fields = &members;
            structTypeValue2Members.type = EDM_COMPLEX_TYPE_TYPE;
            structTypeValue2Members.value.edmComplexType.nMembers = COUNT_OF(two_members);
            structTypeValue2Members.value.edmComplexType.fields = two_members;
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
            currentSTRING_new_call = 0;
            whenShallSTRING_new_fail = 0;
        }

        TEST_FUNCTION_CLEANUP(TestMethodCleanup)
        {
            if (DEFAULT_ENCODED_VALUES != NULL)
            {
                BASEIMPLEMENTATION::STRING_delete(DEFAULT_ENCODED_VALUES);
                DEFAULT_ENCODED_VALUES = NULL;
            }

            ASSERT_ARE_EQUAL(size_t, nSTRING_new_calls, nSTRING_delete_calls);

            if (!MicroMockReleaseMutex(g_testByTest))
            {
                ASSERT_FAIL("failure in test framework at ReleaseMutex");
            }
        }

        /* DataMarshaller_Create */

        /*Tests_SRS_DATA_MARSHALLER_99_019:[ DataMarshaller_Create shall return NULL if any argument is NULL.]*/
        TEST_FUNCTION(DataMarshaller_Create_with_NULL_Model_Handle_fails)
        {
            ///arrange
            CDataMarshallerMocks mocks;

            ///act
            auto res = DataMarshaller_Create(NULL, true);

            ///assert
            ASSERT_IS_NULL(res);
        }

        /* Tests_SRS_DATA_MARSHALLER_99_018:[ DataMarshaller_Create shall create a new DataMarshaller instance and on success it shall return a non NULL handle.]*/
        /* Tests_SRS_DATA_MARSHALLER_99_051:[DataMarshaller_Create shall initialize a BufferProcess instance and associate it with the newly created DataMarshaller instance.] */
        TEST_FUNCTION(DataMarshaller_Create_succeeds)
        {
            ///arrange
            CDataMarshallerMocks mocks;

            ///act
            auto res = DataMarshaller_Create(TEST_MODEL_HANDLE, true);

            ///assert
            ASSERT_IS_NOT_NULL(res);
            mocks.AssertActualAndExpectedCalls();

            ///cleanup
            DataMarshaller_Destroy(res);
        }

        /* Tests_SRS_DATA_MARSHALLER_99_018:[ DataMarshaller_Create shall create a new DataMarshaller instance and on success it shall return a non NULL handle.]*/

        /* Tests_SRS_DATA_MARSHALLER_99_051:[DataMarshaller_Create shall initialize a BufferProcess instance and associate it with the newly created DataMarshaller instance.] */
        TEST_FUNCTION(DataMarshaller_Create_Twice_Yields_2_Different_Handles)
        {
            ///arrange
            CDataMarshallerMocks mocks;

            ///act
            auto handle1 = DataMarshaller_Create(TEST_MODEL_HANDLE, true);
            auto handle2 = DataMarshaller_Create(TEST_MODEL_HANDLE, true);

            ///assert
            ASSERT_IS_NOT_NULL(handle1);
            ASSERT_IS_NOT_NULL(handle2);
            ASSERT_ARE_NOT_EQUAL(void_ptr, (void_ptr)handle1, (void_ptr)handle2);
            mocks.AssertActualAndExpectedCalls();

            ///cleanup
            DataMarshaller_Destroy(handle1);
            DataMarshaller_Destroy(handle2);
        }

        /* DataMarshaller_Destroy */

        /*Tests_SRS_DATA_MARSHALLER_99_022:[ DataMarshaller_Destroy shall free all resources associated with the dataMarshallerHandle argument.]*/
        TEST_FUNCTION(DataMarshaller_Destroy_succeeds_1)
        {
            ///arrange
            CDataMarshallerMocks mocks;
            DATA_MARSHALLER_HANDLE handle = DataMarshaller_Create(TEST_MODEL_HANDLE, true);
            mocks.ResetAllCalls();

            ///act
            DataMarshaller_Destroy(handle);

            ///assert
            ///no explicit assert, uMock checks the calls
        }

        /*Tests_SRS_DATA_MARSHALLER_99_024:[ When called with a NULL handle, DataMarshaller_Destroy shall do nothing.]*/
        TEST_FUNCTION(DataMarshaller_Destroy_With_A_NULL_Handle_Does_Nothing)
        {
            /*Comments: "shall do nothing" is impossible to test, but we can try something*/

            ///arrange
            CDataMarshallerMocks mocks;

            ///act
            DataMarshaller_Destroy(NULL);

            ///assert
            /// no explicit assert
        }

        /* DataMarshaller_SendData */

        /* Tests_SRS_DATA_MARSHALLER_99_033:[ DATA_MARSHALLER_INVALID_ARG shall be returned if the valueCount is zero.] */
        /* Tests_SRS_DATA_MARSHALLER_99_034:[All argument checks shall be performed before calling any other modules.] */
        TEST_FUNCTION(DataMarshaller_SendData_with_Zero_Value_Count_Fails)
        {
            ///arrange
            CNiceCallComparer<CDataMarshallerMocks> mocks;
            DATA_MARSHALLER_HANDLE handle = DataMarshaller_Create(TEST_MODEL_HANDLE, true);
            unsigned char* destination;
            size_t destinationSize;
            mocks.ResetAllCalls();

            const DATA_MARSHALLER_VALUE value = { DEFAULT_PROPERTY_NAME, &floatValid };

            ///act
            auto result = DataMarshaller_SendData(handle, 0, &value, &destination, &destinationSize);
            
            ///assert
            ASSERT_ARE_EQUAL(DATA_MARSHALLER_RESULT, DATA_MARSHALLER_INVALID_ARG, result);

            /// cleanup
            DataMarshaller_Destroy(handle);
        }

        /*Tests_SRS_DATA_MARSHALLER_99_007:[ DATA_MARSHALLER_INVALID_MODEL_PROPERTY shall be returned when any of the items in values contain invalid data]*/
        /*Tests_SRS_DATA_MARSHALLER_99_034:[All argument checks shall be performed before calling any other modules.]*/
        TEST_FUNCTION(DataMarshaller_SendData_with_PropertyName_NULL_fails)
        {
            ///arrange
            CNiceCallComparer<CDataMarshallerMocks> mocks;
            DATA_MARSHALLER_HANDLE handle = DataMarshaller_Create(TEST_MODEL_HANDLE, true);
            unsigned char* destination;
            size_t destinationSize;
            mocks.ResetAllCalls();

            DATA_MARSHALLER_VALUE value = { NULL, &floatValid };

            ///act
            auto result = DataMarshaller_SendData(handle, 1, &value, &destination, &destinationSize);
            
            ///assert
            ASSERT_ARE_EQUAL(DATA_MARSHALLER_RESULT, DATA_MARSHALLER_INVALID_MODEL_PROPERTY, result);
            mocks.AssertActualAndExpectedCalls();

            ///cleanup
            DataMarshaller_Destroy(handle);
        }

        /*Tests_SRS_DATA_MARSHALLER_99_007:[ DATA_MARSHALLER_INVALID_MODEL_PROPERTY shall be returned when any of the items in values contain invalid data]*/
        /*Tests_SRS_DATA_MARSHALLER_99_034:[All argument checks shall be performed before calling any other modules.]*/
        TEST_FUNCTION(DataMarshaller_SendData_with_Value_NULL_In_The_DATA_MARSHALLER_VALUE_fails)
        {
            ///arrange
            CNiceCallComparer<CDataMarshallerMocks> mocks;
            DATA_MARSHALLER_HANDLE handle = DataMarshaller_Create(TEST_MODEL_HANDLE, true);
            unsigned char* destination;
            size_t destinationSize;
            mocks.ResetAllCalls();

            DATA_MARSHALLER_VALUE value = { DEFAULT_PROPERTY_NAME, NULL };

            ///act
            auto result = DataMarshaller_SendData(handle, 1, &value, &destination, &destinationSize);

            ///assert
            ASSERT_ARE_EQUAL(DATA_MARSHALLER_RESULT, DATA_MARSHALLER_INVALID_MODEL_PROPERTY, result);
            mocks.AssertActualAndExpectedCalls();

            ///cleanup
            DataMarshaller_Destroy(handle);
        }

        /*Tests_SRS_DATA_MARSHALLER_99_007:[ DATA_MARSHALLER_INVALID_MODEL_PROPERTY shall be returned when any of the items in values contain invalid data]*/
        /*Tests_SRS_DATA_MARSHALLER_99_034:[All argument checks shall be performed before calling any other modules.]*/
        TEST_FUNCTION(DataMarshaller_SendData_with_Second_PropertyName_NULL_fails)
        {
            ///arrange
            CNiceCallComparer<CDataMarshallerMocks> mocks;
            DATA_MARSHALLER_HANDLE handle = DataMarshaller_Create(TEST_MODEL_HANDLE, true);
            unsigned char* destination;
            size_t destinationSize;
            mocks.ResetAllCalls();

            DATA_MARSHALLER_VALUE values[] = {
                { DEFAULT_PROPERTY_NAME, &floatValid },
                { NULL, &floatValid } };

            ///act
            auto result = DataMarshaller_SendData(handle, sizeof(values) / sizeof(values[0]), values, &destination, &destinationSize);

            ///assert
            ASSERT_ARE_EQUAL(DATA_MARSHALLER_RESULT, DATA_MARSHALLER_INVALID_MODEL_PROPERTY, result);
            mocks.AssertActualAndExpectedCalls();

            ///cleanup
            DataMarshaller_Destroy(handle);
        }

        /*Tests_SRS_DATA_MARSHALLER_99_007:[ DATA_MARSHALLER_INVALID_MODEL_PROPERTY shall be returned when any of the items in values contain invalid data]*/
        /*Tests_SRS_DATA_MARSHALLER_99_034:[All argument checks shall be performed before calling any other modules.]*/
        TEST_FUNCTION(DataMarshaller_SendData_with_Second_Value_NULL_In_The_DATA_MARSHALLER_VALUE_fails)
        {
            ///arrange
            CNiceCallComparer<CDataMarshallerMocks> mocks;
            DATA_MARSHALLER_HANDLE handle = DataMarshaller_Create(TEST_MODEL_HANDLE, true);
            unsigned char* destination;
            size_t destinationSize;
            mocks.ResetAllCalls();

            DATA_MARSHALLER_VALUE values[] = {
                { DEFAULT_PROPERTY_NAME, &floatValid },
                { DEFAULT_PROPERTY_NAME_2, NULL } };

            ///act
            auto result = DataMarshaller_SendData(handle, sizeof(values) / sizeof(values[0]), values, &destination, &destinationSize);

            ///assert
            ASSERT_ARE_EQUAL(DATA_MARSHALLER_RESULT, DATA_MARSHALLER_INVALID_MODEL_PROPERTY, result);
            mocks.AssertActualAndExpectedCalls();

            ///cleanup
            DataMarshaller_Destroy(handle);
        }

        /*Tests_SRS_DATA_MARSHALLER_99_004:[ DATA_MARSHALLER_INVALID_ARG shall be returned when the function has detected an invalid parameter (NULL) being passed to the function.]*/
        /*Tests_SRS_DATA_MARSHALLER_99_034:[All argument checks shall be performed before calling any other modules.]*/
        TEST_FUNCTION(DataMarshaller_SendData_with_NULL_values_fails)
        {
            ///arrange
            CDataMarshallerMocks mocks;
            DATA_MARSHALLER_HANDLE handle = DataMarshaller_Create(TEST_MODEL_HANDLE, true);
            unsigned char* destination;
            size_t destinationSize;
            mocks.ResetAllCalls();

            ///act
            auto result = DataMarshaller_SendData(handle, 1, NULL, &destination, &destinationSize);

            ///assert
            mocks.ResetAllCalls(); /*because for this test we don't care about the calls in other layers*/
            ASSERT_ARE_EQUAL(DATA_MARSHALLER_RESULT, DATA_MARSHALLER_INVALID_ARG, result);
            mocks.AssertActualAndExpectedCalls();

            ///cleanup
            DataMarshaller_Destroy(handle);
        }

        /*Tests_SRS_DATA_MARSHALLER_99_004:[ DATA_MARSHALLER_INVALID_ARG shall be returned when the function has detected an invalid parameter (NULL) being passed to the function.]*/
        TEST_FUNCTION(DataMarshaller_SendData_with_NULL_destination_fails)
        {
            ///arrange
            CDataMarshallerMocks mocks;
            DATA_MARSHALLER_HANDLE handle = DataMarshaller_Create(TEST_MODEL_HANDLE, true);
            size_t destinationSize;
            DATA_MARSHALLER_VALUE value = { DEFAULT_PROPERTY_NAME, &floatValid };
            mocks.ResetAllCalls();

            ///act
            auto result = DataMarshaller_SendData(handle, 1, &value, NULL, &destinationSize);

            ///assert
            mocks.ResetAllCalls(); /*because for this test we don't care about the calls in other layers*/
            ASSERT_ARE_EQUAL(DATA_MARSHALLER_RESULT, DATA_MARSHALLER_INVALID_ARG, result);
            mocks.AssertActualAndExpectedCalls();

            ///cleanup
            DataMarshaller_Destroy(handle);
        }

        /*Tests_SRS_DATA_MARSHALLER_99_004:[ DATA_MARSHALLER_INVALID_ARG shall be returned when the function has detected an invalid parameter (NULL) being passed to the function.]*/
        TEST_FUNCTION(DataMarshaller_SendData_with_NULL_destinationSize_fails)
        {
            ///arrange
            CDataMarshallerMocks mocks;
            DATA_MARSHALLER_HANDLE handle = DataMarshaller_Create(TEST_MODEL_HANDLE, true);
            unsigned char* destination;
            DATA_MARSHALLER_VALUE value = { DEFAULT_PROPERTY_NAME, &floatValid };
            mocks.ResetAllCalls();

            ///act
            auto result = DataMarshaller_SendData(handle, 1, &value, &destination, NULL);

            ///assert
            mocks.ResetAllCalls(); /*because for this test we don't care about the calls in other layers*/
            ASSERT_ARE_EQUAL(DATA_MARSHALLER_RESULT, DATA_MARSHALLER_INVALID_ARG, result);
            mocks.AssertActualAndExpectedCalls();

            ///cleanup
            DataMarshaller_Destroy(handle);
        }

        /* Tests_SRS_DATA_MARSHALLER_99_035:[DATA_MARSHALLER_MULTITREE_ERROR shall be returned in case any MultiTree API call fails.] */
        TEST_FUNCTION(DataMarshaller_SendData_When_MultiTree_Create_Fails_Then_Fails)
        {
            ///arrange
            CDataMarshallerMocks mocks;
            DATA_MARSHALLER_HANDLE handle = DataMarshaller_Create(TEST_MODEL_HANDLE, true);
            unsigned char* destination;
            size_t destinationSize;
            mocks.ResetAllCalls();

            DATA_MARSHALLER_VALUE value = { DEFAULT_PROPERTY_NAME, &floatValid };

            EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .SetReturn((MULTITREE_HANDLE)NULL);

            ///act
            auto result = DataMarshaller_SendData(handle, 1, &value, &destination, &destinationSize);

            ///assert
            ASSERT_ARE_EQUAL(DATA_MARSHALLER_RESULT, DATA_MARSHALLER_MULTITREE_ERROR, result);
            mocks.AssertActualAndExpectedCalls();

            ///cleanup
            DataMarshaller_Destroy(handle);
        }

        /* Tests_SRS_DATA_MARSHALLER_99_035:[DATA_MARSHALLER_MULTITREE_ERROR shall be returned in case any MultiTree API call fails.] */
        TEST_FUNCTION(DataMarshaller_SendData_When_MultiTree_AddLeaf_With_Property_Value_Fails_Then_Fails)
        {
            ///arrange
            CDataMarshallerMocks mocks;
            DATA_MARSHALLER_HANDLE handle = DataMarshaller_Create(TEST_MODEL_HANDLE, true);
            unsigned char* destination;
            size_t destinationSize;
            mocks.ResetAllCalls();

            DATA_MARSHALLER_VALUE value = { DEFAULT_PROPERTY_NAME, &floatValid };

            EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));

            STRICT_EXPECTED_CALL(mocks, MultiTree_AddLeaf(TEST_MULTITREE_HANDLE, DEFAULT_PROPERTY_NAME, &floatValid))
                .SetReturn(MULTITREE_ERROR);

            STRICT_EXPECTED_CALL(mocks, MultiTree_Destroy(TEST_MULTITREE_HANDLE));

            ///act
            auto result = DataMarshaller_SendData(handle, 1, &value, &destination, &destinationSize);

            ///assert
            ASSERT_ARE_EQUAL(DATA_MARSHALLER_RESULT, DATA_MARSHALLER_MULTITREE_ERROR, result);
            mocks.AssertActualAndExpectedCalls();

            ///cleanup
            DataMarshaller_Destroy(handle);
        }

        /* Tests_SRS_DATA_MARSHALLER_99_035:[DATA_MARSHALLER_MULTITREE_ERROR shall be returned in case any MultiTree API call fails.] */
        TEST_FUNCTION(DataMarshaller_SendData_When_MultiTree_AddLeaf_With_The_Second_Property_Value_Fails_Then_Fails)
        {
            ///arrange
            CDataMarshallerMocks mocks;
            DATA_MARSHALLER_HANDLE handle = DataMarshaller_Create(TEST_MODEL_HANDLE, true);
            unsigned char* destination;
            size_t destinationSize;
            mocks.ResetAllCalls();
            AGENT_DATA_TYPE floatValid2;

            DATA_MARSHALLER_VALUE values[] = {
                { DEFAULT_PROPERTY_NAME, &floatValid },
                { DEFAULT_PROPERTY_NAME_2, &floatValid2 }
            };

            EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));

            STRICT_EXPECTED_CALL(mocks, MultiTree_AddLeaf(TEST_MULTITREE_HANDLE, DEFAULT_PROPERTY_NAME, &floatValid));
            STRICT_EXPECTED_CALL(mocks, MultiTree_AddLeaf(TEST_MULTITREE_HANDLE, DEFAULT_PROPERTY_NAME_2, &floatValid2))
                .SetReturn(MULTITREE_ERROR);

            STRICT_EXPECTED_CALL(mocks, MultiTree_Destroy(TEST_MULTITREE_HANDLE));

            ///act
            auto result = DataMarshaller_SendData(handle, sizeof(values) / sizeof(values[0]), values, &destination, &destinationSize);

            ///assert
            ASSERT_ARE_EQUAL(DATA_MARSHALLER_RESULT, DATA_MARSHALLER_MULTITREE_ERROR, result);
            mocks.AssertActualAndExpectedCalls();

            ///cleanup
            DataMarshaller_Destroy(handle);
        }

        /* Tests_SRS_DATA_MARSHALLER_99_027:[ DATA_MARSHALLER_JSON_ENCODER_ERROR shall be returned when JSONEncoder returns an error code.] */
        TEST_FUNCTION(DataMarshaller_SendData_When_Encoding_The_Values_Tree_To_JSON_Fails_Then_Fails)
        {
            ///arrange
            CDataMarshallerMocks mocks;
            DATA_MARSHALLER_HANDLE handle = DataMarshaller_Create(TEST_MODEL_HANDLE, true);
            mocks.ResetAllCalls();
            unsigned char* destination;
            size_t destinationSize;
            DATA_MARSHALLER_VALUE value = { DEFAULT_PROPERTY_NAME, &floatValid };

            EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));

            STRICT_EXPECTED_CALL(mocks, MultiTree_AddLeaf(TEST_MULTITREE_HANDLE, DEFAULT_PROPERTY_NAME, &floatValid));
            EXPECTED_CALL(mocks, JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ValidateArgument(1)
                .SetReturn(JSON_ENCODER_ERROR);

            EXPECTED_CALL(mocks, STRING_new())
                .ExpectedTimesExactly(1);
            EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
                .ExpectedTimesExactly(1);

            STRICT_EXPECTED_CALL(mocks, MultiTree_Destroy(TEST_MULTITREE_HANDLE));

            ///act
            auto result = DataMarshaller_SendData(handle, 1, &value, &destination, &destinationSize);

            ///assert
            ASSERT_ARE_EQUAL(DATA_MARSHALLER_RESULT, DATA_MARSHALLER_JSON_ENCODER_ERROR, result);
            mocks.AssertActualAndExpectedCalls();

            ///cleanup
            DataMarshaller_Destroy(handle);
        }

        /* Tests_SRS_DATAMARSHALLER_01_002: [If the includePropertyPath argument passed to DataMarshaller_Create was false and the number of values passed to SendData is greater than 1 and at least one of them is a struct, DataMarshaller_SendData shall fallback to  including the complete property path in the output JSON.] */
        /*Tests_SRS_DATAMARSHALLER_02_007: [DataMarshaller_SendData shall copy in the output parameters *destination, *destinationSize the content and the content length of the encoded JSON tree.] */
        TEST_FUNCTION(when_includepropertypath_is_false_and_value_count_is_greater_than_1_and_one_of_them_is_a_struct_the_property_path_is_included)
        {
            ///arrange
            CDataMarshallerMocks mocks;
            DATA_MARSHALLER_HANDLE handle = DataMarshaller_Create(TEST_MODEL_HANDLE, false);
            unsigned char* destination;
            size_t destinationSize;
            mocks.ResetAllCalls();
            DATA_MARSHALLER_VALUE value[] = { { DEFAULT_PROPERTY_NAME, &floatValid }, { DEFAULT_PROPERTY_NAME_2, &structTypeValue } };
            char json_payload[] = "Test";

            EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));

            STRICT_EXPECTED_CALL(mocks, MultiTree_AddLeaf(TEST_MULTITREE_HANDLE, DEFAULT_PROPERTY_NAME, &floatValid));
            STRICT_EXPECTED_CALL(mocks, MultiTree_AddLeaf(TEST_MULTITREE_HANDLE, DEFAULT_PROPERTY_NAME_2, &structTypeValue));
            EXPECTED_CALL(mocks, STRING_new());
            EXPECTED_CALL(mocks, JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ValidateArgument(1);

            EXPECTED_CALL(mocks, STRING_length(IGNORED_PTR_ARG))
                .SetReturn(strlen(json_payload));

            EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
                .SetReturn(json_payload);

            EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
            STRICT_EXPECTED_CALL(mocks, MultiTree_Destroy(TEST_MULTITREE_HANDLE));

            ///act
            auto result = DataMarshaller_SendData(handle, 2, value, &destination, &destinationSize);

            ///assert
            ASSERT_ARE_EQUAL(DATA_MARSHALLER_RESULT, DATA_MARSHALLER_OK, result);
            ASSERT_ARE_EQUAL(size_t, strlen(json_payload), destinationSize);
            ASSERT_ARE_EQUAL(int, 0, memcmp(destination, json_payload, destinationSize));
            mocks.AssertActualAndExpectedCalls();

            ///cleanup
            free(destination);
            DataMarshaller_Destroy(handle);
        }

        /*Tests_SRS_DATAMARSHALLER_02_006: [The complete JSON object shall be handed over to IoTHubClient by a call to IoTHubClient_LL_SendEventAsync if parameter transportType of _Create was TRANSPORT_LL.] */
        TEST_FUNCTION(DataMarshaller_SendData_sends_to_LL_layer_succeeds)
        {
            ///arrange
            CDataMarshallerMocks mocks;
            DATA_MARSHALLER_HANDLE handle = DataMarshaller_Create(TEST_MODEL_HANDLE, false);
            unsigned char* destination;
            size_t destinationSize;
            mocks.ResetAllCalls();
            DATA_MARSHALLER_VALUE value[] = { { DEFAULT_PROPERTY_NAME, &floatValid }, { DEFAULT_PROPERTY_NAME_2, &structTypeValue } };
            char json_payload[] = "Test";

            EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));

            STRICT_EXPECTED_CALL(mocks, MultiTree_AddLeaf(TEST_MULTITREE_HANDLE, DEFAULT_PROPERTY_NAME, &floatValid));
            STRICT_EXPECTED_CALL(mocks, MultiTree_AddLeaf(TEST_MULTITREE_HANDLE, DEFAULT_PROPERTY_NAME_2, &structTypeValue));
            EXPECTED_CALL(mocks, STRING_new());
            EXPECTED_CALL(mocks, JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ValidateArgument(1);

            EXPECTED_CALL(mocks, STRING_length(IGNORED_PTR_ARG))
                .SetReturn(strlen(json_payload));

            EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
                .SetReturn(json_payload);

            EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
            STRICT_EXPECTED_CALL(mocks, MultiTree_Destroy(TEST_MULTITREE_HANDLE));

            ///act
            auto result = DataMarshaller_SendData(handle, 2, value, &destination, &destinationSize);

            ///assert
            ASSERT_ARE_EQUAL(DATA_MARSHALLER_RESULT, DATA_MARSHALLER_OK, result);
            mocks.AssertActualAndExpectedCalls();

            ///cleanup
            free(destination);
            DataMarshaller_Destroy(handle);
        }

        /* Tests_SRS_DATAMARSHALLER_01_002: [If the includePropertyPath argument passed to DataMarshaller_Create was false and the number of values passed to SendData is greater than 1 and at least one of them is a struct, DataMarshaller_SendData shall fallback to  including the complete property path in the output JSON.] */
        TEST_FUNCTION(when_includepropertypath_is_false_and_value_count_is_greater_than_1_and_one_but_no_structs_SendData_succeeds)
        {
            ///arrange
            CDataMarshallerMocks mocks;
            DATA_MARSHALLER_HANDLE handle = DataMarshaller_Create(TEST_MODEL_HANDLE, false);
            unsigned char* destination;
            size_t destinationSize;
            mocks.ResetAllCalls();
            DATA_MARSHALLER_VALUE value[] = { { DEFAULT_PROPERTY_NAME, &floatValid }, { DEFAULT_PROPERTY_NAME_2, &floatValid } };
            char json_payload[] = "Test";

            EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));

            STRICT_EXPECTED_CALL(mocks, MultiTree_AddLeaf(TEST_MULTITREE_HANDLE, DEFAULT_PROPERTY_NAME, &floatValid));
            STRICT_EXPECTED_CALL(mocks, MultiTree_AddLeaf(TEST_MULTITREE_HANDLE, DEFAULT_PROPERTY_NAME_2, &floatValid));
            EXPECTED_CALL(mocks, STRING_new());
            EXPECTED_CALL(mocks, JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ValidateArgument(1);

            EXPECTED_CALL(mocks, STRING_length(IGNORED_PTR_ARG))
                .SetReturn(strlen(json_payload));

            EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
                .SetReturn(json_payload);

            EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
            STRICT_EXPECTED_CALL(mocks, MultiTree_Destroy(TEST_MULTITREE_HANDLE));

            ///act
            auto result = DataMarshaller_SendData(handle, 2, value, &destination, &destinationSize);

            ///assert
            ASSERT_ARE_EQUAL(DATA_MARSHALLER_RESULT, DATA_MARSHALLER_OK, result);
            mocks.AssertActualAndExpectedCalls();

            ///cleanup
            free(destination);
            DataMarshaller_Destroy(handle);
        }

        /* Tests_SRS_DATA_MARSHALLER_99_039:[ If the includePropertyPath argument passed to DataMarshaller_Create was true each property shall be placed in the appropriate position in the JSON according to its path in the model.] */
        TEST_FUNCTION(when_includePropertyPath_is_true_the_property_name_is_placed_in_the_JSON_and_SendAsync_is_called)
        {
            ///arrange
            CDataMarshallerMocks mocks;
            DATA_MARSHALLER_HANDLE handle = DataMarshaller_Create(TEST_MODEL_HANDLE, true);
            unsigned char* destination;
            size_t destinationSize;
            mocks.ResetAllCalls();
            DATA_MARSHALLER_VALUE value = { DEFAULT_PROPERTY_NAME, &floatValid };
            char json_payload[] = "Test";

            EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));

            STRICT_EXPECTED_CALL(mocks, MultiTree_AddLeaf(TEST_MULTITREE_HANDLE, DEFAULT_PROPERTY_NAME, &floatValid));
            EXPECTED_CALL(mocks, STRING_new());
            EXPECTED_CALL(mocks, JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ValidateArgument(1);

            EXPECTED_CALL(mocks, STRING_length(IGNORED_PTR_ARG))
                .SetReturn(strlen(json_payload));

            EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
                .SetReturn(json_payload);

            EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
            STRICT_EXPECTED_CALL(mocks, MultiTree_Destroy(TEST_MULTITREE_HANDLE));

            ///act
            auto result = DataMarshaller_SendData(handle, 1, &value, &destination, &destinationSize);

            ///assert
            ASSERT_ARE_EQUAL(DATA_MARSHALLER_RESULT, DATA_MARSHALLER_OK, result);
            mocks.AssertActualAndExpectedCalls();

            ///cleanup
            free(destination);
            DataMarshaller_Destroy(handle);
        }

        /* Tests_SRS_DATAMARSHALLER_01_001: [If the includePropertyPath argument passed to DataMarshaller_Create was false and only one struct is being sent, the relative path of the value passed to DataMarshaller_SendData - including property name - shall be ignored and the value shall be placed at JSON root.] */
        /* Tests_SRS_DATAMARSHALLER_01_004: [In this case the members of the struct shall be added as leafs into the MultiTree, each leaf having the name of the struct member.] */
        TEST_FUNCTION(when_includePropertyPath_is_false_and_one_struct_is_being_sent_the_property_name_is_not_placed_in_the_JSON_and_SendAsync_is_called)
        {
            ///arrange
            CDataMarshallerMocks mocks;
            DATA_MARSHALLER_HANDLE handle = DataMarshaller_Create(TEST_MODEL_HANDLE, false);
            unsigned char* destination;
            size_t destinationSize;
            mocks.ResetAllCalls();
            DATA_MARSHALLER_VALUE value = { DEFAULT_PROPERTY_NAME, &structTypeValue2Members };
            char json_payload[] = "Test";

            EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));

            STRICT_EXPECTED_CALL(mocks, MultiTree_AddLeaf(TEST_MULTITREE_HANDLE, "x", structTypeValue2Members.value.edmComplexType.fields[0].value));
            STRICT_EXPECTED_CALL(mocks, MultiTree_AddLeaf(TEST_MULTITREE_HANDLE, "y", structTypeValue2Members.value.edmComplexType.fields[1].value));
            EXPECTED_CALL(mocks, STRING_new());
            EXPECTED_CALL(mocks, JSONEncoder_EncodeTree(TEST_MULTITREE_HANDLE, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .ValidateArgument(1);

            EXPECTED_CALL(mocks, STRING_length(IGNORED_PTR_ARG))
                .SetReturn(strlen(json_payload));

            EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
                .SetReturn(json_payload);
            EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
            STRICT_EXPECTED_CALL(mocks, MultiTree_Destroy(TEST_MULTITREE_HANDLE));

            ///act
            auto result = DataMarshaller_SendData(handle, 1, &value, &destination, &destinationSize);

            ///assert
            ASSERT_ARE_EQUAL(DATA_MARSHALLER_RESULT, DATA_MARSHALLER_OK, result);
            mocks.AssertActualAndExpectedCalls();

            ///cleanup
            free(destination);
            DataMarshaller_Destroy(handle);
        }

        /* Tests_SRS_DATA_MARSHALLER_99_035:[DATA_MARSHALLER_MULTITREE_ERROR shall be returned in case any MultiTree API call fails.] */
        TEST_FUNCTION(when_adding_the_first_member_of_the_struct_fails_then_senddata_fails)
        {
            ///arrange
            CDataMarshallerMocks mocks;
            DATA_MARSHALLER_HANDLE handle = DataMarshaller_Create(TEST_MODEL_HANDLE, false);
            unsigned char* destination;
            size_t destinationSize;
            mocks.ResetAllCalls();
            DATA_MARSHALLER_VALUE value = { DEFAULT_PROPERTY_NAME, &structTypeValue2Members };

            EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));

            STRICT_EXPECTED_CALL(mocks, MultiTree_AddLeaf(TEST_MULTITREE_HANDLE, "x", structTypeValue2Members.value.edmComplexType.fields[0].value))
                .SetReturn(MULTITREE_ERROR);

            STRICT_EXPECTED_CALL(mocks, MultiTree_Destroy(TEST_MULTITREE_HANDLE));

            ///act
            auto result = DataMarshaller_SendData(handle, 1, &value, &destination, &destinationSize);

            ///assert
            ASSERT_ARE_EQUAL(DATA_MARSHALLER_RESULT, DATA_MARSHALLER_MULTITREE_ERROR, result);
            mocks.AssertActualAndExpectedCalls();

            ///cleanup
            DataMarshaller_Destroy(handle);
        }

        /* Tests_SRS_DATA_MARSHALLER_99_035:[DATA_MARSHALLER_MULTITREE_ERROR shall be returned in case any MultiTree API call fails.] */
        TEST_FUNCTION(when_adding_the_second_member_of_the_struct_fails_then_senddata_fails)
        {
            ///arrange
            CDataMarshallerMocks mocks;
            DATA_MARSHALLER_HANDLE handle = DataMarshaller_Create(TEST_MODEL_HANDLE, false);
            unsigned char* destination;
            size_t destinationSize;
            mocks.ResetAllCalls();
            DATA_MARSHALLER_VALUE value = { DEFAULT_PROPERTY_NAME, &structTypeValue2Members };

            EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));

            STRICT_EXPECTED_CALL(mocks, MultiTree_AddLeaf(TEST_MULTITREE_HANDLE, "x", structTypeValue2Members.value.edmComplexType.fields[0].value));
            STRICT_EXPECTED_CALL(mocks, MultiTree_AddLeaf(TEST_MULTITREE_HANDLE, "y", structTypeValue2Members.value.edmComplexType.fields[1].value))
                .SetReturn(MULTITREE_ERROR);

            STRICT_EXPECTED_CALL(mocks, MultiTree_Destroy(TEST_MULTITREE_HANDLE));

            ///act
            auto result = DataMarshaller_SendData(handle, 1, &value, &destination, &destinationSize);

            ///assert
            ASSERT_ARE_EQUAL(DATA_MARSHALLER_RESULT, DATA_MARSHALLER_MULTITREE_ERROR, result);
            mocks.AssertActualAndExpectedCalls();

            ///cleanup
            DataMarshaller_Destroy(handle);
        }

        /* Tests_SRS_DATAMARSHALLER_01_003: [DATA_MARSHALLER_ERROR shall be returned for any errors when calling IoTHubMessage APIs.] */
        TEST_FUNCTION(when_STRING_new_fails_SendData_Fails)
        {
            ///arrange
            CDataMarshallerMocks mocks;
            DATA_MARSHALLER_HANDLE handle = DataMarshaller_Create(TEST_MODEL_HANDLE, false);
            unsigned char* destination;
            size_t destinationSize;
            mocks.ResetAllCalls();
            DATA_MARSHALLER_VALUE value = { DEFAULT_PROPERTY_NAME, &floatValid };
            whenShallSTRING_new_fail = 1;

            EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));

            STRICT_EXPECTED_CALL(mocks, MultiTree_AddLeaf(TEST_MULTITREE_HANDLE, DEFAULT_PROPERTY_NAME, &floatValid));
            EXPECTED_CALL(mocks, STRING_new());
            STRICT_EXPECTED_CALL(mocks, MultiTree_Destroy(TEST_MULTITREE_HANDLE));

            ///act
            auto result = DataMarshaller_SendData(handle, 1, &value, &destination, &destinationSize);

            ///assert
            ASSERT_ARE_EQUAL(DATA_MARSHALLER_RESULT, DATA_MARSHALLER_ERROR, result);
            mocks.AssertActualAndExpectedCalls();

            ///cleanup
            DataMarshaller_Destroy(handle);
        }

END_TEST_SUITE(DataMarshaller_ut)
