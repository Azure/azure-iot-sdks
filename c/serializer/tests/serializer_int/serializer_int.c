
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "serializer.h"
#include "macro_utils.h"
#include "testrunnerswitcher.h"

#include "umock_c.h"
#include "umocktypes_charptr.h"
#include "umocktypes_bool.h"
#include "umocktypes_stdint.h"
#include "umock_c_negative_tests.h"

#include "parson.h"

static TEST_MUTEX_HANDLE g_testByTest;
static TEST_MUTEX_HANDLE g_dllByDll;

#ifdef CPP_UNITTEST
/*apparently CppUniTest.h does not define the below which is needed for int64_t asserts*/
template <> static std::wstring Microsoft::VisualStudio::CppUnitTestFramework::ToString < int64_t >(const int64_t& q)
{
    std::wstring result;
    std::wostringstream o;
    o << q;
    return o.str();
}
#endif

BEGIN_NAMESPACE(basic1)

DECLARE_MODEL(basicModel_WithData1,
    WITH_DATA(double, with_data_double1),
    WITH_DATA(int, with_data_int1),
    WITH_DATA(float, with_data_float1),
    WITH_DATA(long, with_data_long1),
    WITH_DATA(int8_t, with_data_sint8_t1),
    WITH_DATA(uint8_t, with_data_uint8_t1),
    WITH_DATA(int16_t, with_data_int16_t1),
    WITH_DATA(int32_t, with_data_int32_t1),
    WITH_DATA(int64_t, with_data_int64_t1),
    WITH_DATA(bool, with_data_bool1),
    WITH_DATA(ascii_char_ptr, with_data_ascii_char_ptr1),
    WITH_DATA(ascii_char_ptr_no_quotes, with_data_ascii_char_ptr_no_quotes1),
    WITH_DATA(EDM_DATE_TIME_OFFSET, with_data_EdmDateTimeOffset1),
    WITH_DATA(EDM_GUID, with_data_EdmGuid1),
    WITH_DATA(EDM_BINARY, with_data_EdmBinary1)
)
END_NAMESPACE(basic1)


BEGIN_NAMESPACE(basic2)

DECLARE_STRUCT(struct_WithData2,
    double, with_data_double2,
    int, with_data_int2,
    float, with_data_float2,
    long, with_data_long2,
    int8_t, with_data_sint8_t2,
    uint8_t, with_data_uint8_t2,
    int16_t, with_data_int16_t2,
    int32_t, with_data_int32_t2,
    int64_t, with_data_int64_t2,
    bool, with_data_bool2,
    ascii_char_ptr, with_data_ascii_char_ptr2,
    ascii_char_ptr_no_quotes, with_data_ascii_char_ptr_no_quotes2,
    EDM_DATE_TIME_OFFSET, with_data_EdmDateTimeOffset2,
    EDM_GUID, with_data_EdmGuid2,
    EDM_BINARY, with_data_EdmBinary2
    )
DECLARE_MODEL(basicModel_WithStruct2,
    WITH_DATA(struct_WithData2, structure2)
)
END_NAMESPACE(basic2)

BEGIN_NAMESPACE(basic3)

DECLARE_MODEL(model_WithData3,
    WITH_DATA(double, with_data_double3),
    WITH_DATA(int, with_data_int3),
    WITH_DATA(float, with_data_float3),
    WITH_DATA(long, with_data_long3),
    WITH_DATA(int8_t, with_data_sint8_t3),
    WITH_DATA(uint8_t, with_data_uint8_t3),
    WITH_DATA(int16_t, with_data_int16_t3),
    WITH_DATA(int32_t, with_data_int32_t3),
    WITH_DATA(int64_t, with_data_int64_t3),
    WITH_DATA(bool, with_data_bool3),
    WITH_DATA(ascii_char_ptr, with_data_ascii_char_ptr3),
    WITH_DATA(ascii_char_ptr_no_quotes, with_data_ascii_char_ptr_no_quotes3),
    WITH_DATA(EDM_DATE_TIME_OFFSET, with_data_EdmDateTimeOffset3),
    WITH_DATA(EDM_GUID, with_data_EdmGuid3),
    WITH_DATA(EDM_BINARY, with_data_EdmBinary3)
    )
DECLARE_MODEL(basicModel_WithModel3,
    WITH_DATA(model_WithData3, model3)
)
END_NAMESPACE(basic3)

BEGIN_NAMESPACE(basic4)

DECLARE_STRUCT(struct_WithData4,
    double, with_data_double4,
    int, with_data_int4,
    float, with_data_float4,
    long, with_data_long4,
    int8_t, with_data_sint8_t4,
    uint8_t, with_data_uint8_t4,
    int16_t, with_data_int16_t4,
    int32_t, with_data_int32_t4,
    int64_t, with_data_int64_t4,
    bool, with_data_bool4,
    ascii_char_ptr, with_data_ascii_char_ptr4,
    ascii_char_ptr_no_quotes, with_data_ascii_char_ptr_no_quotes4,
    EDM_DATE_TIME_OFFSET, with_data_EdmDateTimeOffset4,
    EDM_GUID, with_data_EdmGuid4,
    EDM_BINARY, with_data_EdmBinary4
)
DECLARE_MODEL(innerModel4,
    WITH_DATA(struct_WithData4, structure4)
)
DECLARE_MODEL(outerModel4,
    WITH_DATA(innerModel4, inner_model4)
)

END_NAMESPACE(basic4)

BEGIN_NAMESPACE(basic5)

DECLARE_MODEL(basicModel_WithReportedProperty5,
    WITH_REPORTED_PROPERTY(double, with_reported_property_double5),
    WITH_REPORTED_PROPERTY(int, with_reported_property_int5),
    WITH_REPORTED_PROPERTY(float, with_reported_property_float5),
    WITH_REPORTED_PROPERTY(long, with_reported_property_long5),
    WITH_REPORTED_PROPERTY(int8_t, with_reported_property_sint8_t5),
    WITH_REPORTED_PROPERTY(uint8_t, with_reported_property_uint8_t5),
    WITH_REPORTED_PROPERTY(int16_t, with_reported_property_int16_t5),
    WITH_REPORTED_PROPERTY(int32_t, with_reported_property_int32_t5),
    WITH_REPORTED_PROPERTY(int64_t, with_reported_property_int64_t5),
    WITH_REPORTED_PROPERTY(bool, with_reported_property_bool5),
    WITH_REPORTED_PROPERTY(ascii_char_ptr, with_reported_property_ascii_char_ptr5),
    WITH_REPORTED_PROPERTY(ascii_char_ptr_no_quotes, with_reported_property_ascii_char_ptr_no_quotes5),
    WITH_REPORTED_PROPERTY(EDM_DATE_TIME_OFFSET, with_reported_property_EdmDateTimeOffset5),
    WITH_REPORTED_PROPERTY(EDM_GUID, with_reported_property_EdmGuid5),
    WITH_REPORTED_PROPERTY(EDM_BINARY, with_reported_property_EdmBinary5)
    )
END_NAMESPACE(basic5)

BEGIN_NAMESPACE(basic6)

DECLARE_STRUCT(struct_WithReportedPropery6,
    double, with_reported_property_double6,
    int, with_reported_property_int6,
    float, with_reported_property_float6,
    long, with_reported_property_long6,
    int8_t, with_reported_property_sint8_t6,
    uint8_t, with_reported_property_uint8_t6,
    int16_t, with_reported_property_int16_t6,
    int32_t, with_reported_property_int32_t6,
    int64_t, with_reported_property_int64_t6,
    bool, with_reported_property_bool6,
    ascii_char_ptr, with_reported_property_ascii_char_ptr6,
    ascii_char_ptr_no_quotes, with_reported_property_ascii_char_ptr_no_quotes6,
    EDM_DATE_TIME_OFFSET, with_reported_property_EdmDateTimeOffset6,
    EDM_GUID, with_reported_property_EdmGuid6,
    EDM_BINARY, with_reported_property_EdmBinary6
    )
DECLARE_MODEL(basicModel_WithStruct6,
    WITH_REPORTED_PROPERTY(struct_WithReportedPropery6, structure6)
    )
END_NAMESPACE(basic6)

BEGIN_NAMESPACE(basic7)

DECLARE_MODEL(model_WithReported7,
    WITH_REPORTED_PROPERTY(double, with_reported_property_double7),
    WITH_REPORTED_PROPERTY(int, with_reported_property_int7),
    WITH_REPORTED_PROPERTY(float, with_reported_property_float7),
    WITH_REPORTED_PROPERTY(long, with_reported_property_long7),
    WITH_REPORTED_PROPERTY(int8_t, with_reported_property_sint8_t7),
    WITH_REPORTED_PROPERTY(uint8_t, with_reported_property_uint8_t7),
    WITH_REPORTED_PROPERTY(int16_t, with_reported_property_int16_t7),
    WITH_REPORTED_PROPERTY(int32_t, with_reported_property_int32_t7),
    WITH_REPORTED_PROPERTY(int64_t, with_reported_property_int64_t7),
    WITH_REPORTED_PROPERTY(bool, with_reported_property_bool7),
    WITH_REPORTED_PROPERTY(ascii_char_ptr, with_reported_property_ascii_char_ptr7),
    WITH_REPORTED_PROPERTY(ascii_char_ptr_no_quotes, with_reported_property_ascii_char_ptr_no_quotes7),
    WITH_REPORTED_PROPERTY(EDM_DATE_TIME_OFFSET, with_reported_property_EdmDateTimeOffset7),
    WITH_REPORTED_PROPERTY(EDM_GUID, with_reported_property_EdmGuid7),
    WITH_REPORTED_PROPERTY(EDM_BINARY, with_reported_property_EdmBinary7)
)
DECLARE_MODEL(basicModel_WithReportedProperty7,
    WITH_REPORTED_PROPERTY(model_WithReported7, model7)
)
END_NAMESPACE(basic7)

BEGIN_NAMESPACE(basic8)

DECLARE_STRUCT(struct_WithData8,
    double, with_reported_property_double8,
    int, with_reported_property_int8,
    float, with_reported_property_float8,
    long, with_reported_property_long8,
    int8_t, with_reported_property_sint8_t8,
    uint8_t, with_reported_property_uint8_t8,
    int16_t, with_reported_property_int16_t8,
    int32_t, with_reported_property_int32_t8,
    int64_t, with_reported_property_int64_t8,
    bool, with_reported_property_bool8,
    ascii_char_ptr, with_reported_property_ascii_char_ptr8,
    ascii_char_ptr_no_quotes, with_reported_property_ascii_char_ptr_no_quotes8,
    EDM_DATE_TIME_OFFSET, with_reported_property_EdmDateTimeOffset8,
    EDM_GUID, with_reported_property_EdmGuid8,
    EDM_BINARY, with_reported_property_EdmBinary8
)
DECLARE_MODEL(innerModel8,
    WITH_REPORTED_PROPERTY(struct_WithData8, structure8)
)
DECLARE_MODEL(outerModel8,
    WITH_REPORTED_PROPERTY(innerModel8, inner_model8)
)

END_NAMESPACE(basic8)


BEGIN_NAMESPACE(basic9)

DECLARE_MODEL(basicModel_WithData9,
    WITH_DESIRED_PROPERTY(double, with_desired_property_double9),
    WITH_DESIRED_PROPERTY(int, with_desired_property_int9),
    WITH_DESIRED_PROPERTY(float, with_desired_property_float9),
    WITH_DESIRED_PROPERTY(long, with_desired_property_long9),
    WITH_DESIRED_PROPERTY(int8_t, with_desired_property_sint8_t9),
    WITH_DESIRED_PROPERTY(uint8_t, with_desired_property_uint8_t9),
    WITH_DESIRED_PROPERTY(int16_t, with_desired_property_int16_t9),
    WITH_DESIRED_PROPERTY(int32_t, with_desired_property_int32_t9),
    WITH_DESIRED_PROPERTY(int64_t, with_desired_property_int64_t9),
    WITH_DESIRED_PROPERTY(bool, with_desired_property_bool9),
    WITH_DESIRED_PROPERTY(ascii_char_ptr, with_desired_property_ascii_char_ptr9),
    WITH_DESIRED_PROPERTY(ascii_char_ptr_no_quotes, with_desired_property_ascii_char_ptr_no_quotes9),
    WITH_DESIRED_PROPERTY(EDM_DATE_TIME_OFFSET, with_desired_property_EdmDateTimeOffset9),
    WITH_DESIRED_PROPERTY(EDM_GUID, with_desired_property_EdmGuid9),
    WITH_DESIRED_PROPERTY(EDM_BINARY, with_desired_property_EdmBinary9)
)

END_NAMESPACE(basic9)

BEGIN_NAMESPACE(basic10)

DECLARE_STRUCT(struct_WithData10,
    double, with_desired_property_double10,
    int, with_desired_property_int10,
    float, with_desired_property_float10,
    long, with_desired_property_long10,
    int8_t, with_desired_property_sint8_t10,
    uint8_t, with_desired_property_uint8_t10,
    int16_t, with_desired_property_int16_t10,
    int32_t, with_desired_property_int32_t10,
    int64_t, with_desired_property_int64_t10,
    bool, with_desired_property_bool10,
    ascii_char_ptr, with_desired_property_ascii_char_ptr10,
    ascii_char_ptr_no_quotes, with_desired_property_ascii_char_ptr_no_quotes10,
    EDM_DATE_TIME_OFFSET, with_desired_property_EdmDateTimeOffset10,
    EDM_GUID, with_desired_property_EdmGuid10,
    EDM_BINARY, with_desired_property_EdmBinary10
)
DECLARE_MODEL(basicModel_WithStruct10,
    WITH_DESIRED_PROPERTY(struct_WithData10, structure10)
)
END_NAMESPACE(basic10)

BEGIN_NAMESPACE(basic11)

DECLARE_MODEL(model_WithData11,
    WITH_DESIRED_PROPERTY(double, with_desired_property_double11),
    WITH_DESIRED_PROPERTY(int, with_desired_property_int11),
    WITH_DESIRED_PROPERTY(float, with_desired_property_float11),
    WITH_DESIRED_PROPERTY(long, with_desired_property_long11),
    WITH_DESIRED_PROPERTY(int8_t, with_desired_property_sint8_t11),
    WITH_DESIRED_PROPERTY(uint8_t, with_desired_property_uint8_t11),
    WITH_DESIRED_PROPERTY(int16_t, with_desired_property_int16_t11),
    WITH_DESIRED_PROPERTY(int32_t, with_desired_property_int32_t11),
    WITH_DESIRED_PROPERTY(int64_t, with_desired_property_int64_t11),
    WITH_DESIRED_PROPERTY(bool, with_desired_property_bool11),
    WITH_DESIRED_PROPERTY(ascii_char_ptr, with_desired_property_ascii_char_ptr11),
    WITH_DESIRED_PROPERTY(ascii_char_ptr_no_quotes, with_desired_property_ascii_char_ptr_no_quotes11),
    WITH_DESIRED_PROPERTY(EDM_DATE_TIME_OFFSET, with_desired_property_EdmDateTimeOffset11),
    WITH_DESIRED_PROPERTY(EDM_GUID, with_desired_property_EdmGuid11),
    WITH_DESIRED_PROPERTY(EDM_BINARY, with_desired_property_EdmBinary11)
)
DECLARE_MODEL(basicModel_WithModel11,
    WITH_DESIRED_PROPERTY(model_WithData11, model11)
)
END_NAMESPACE(basic11)


BEGIN_NAMESPACE(basic12)

DECLARE_STRUCT(struct_WithData12,
    double, with_desired_property_double12,
    int, with_desired_property_int12,
    float, with_desired_property_float12,
    long, with_desired_property_long12,
    int8_t, with_desired_property_sint8_t12,
    uint8_t, with_desired_property_uint8_t12,
    int16_t, with_desired_property_int16_t12,
    int32_t, with_desired_property_int32_t12,
    int64_t, with_desired_property_int64_t12,
    bool, with_desired_property_bool12,
    ascii_char_ptr, with_desired_property_ascii_char_ptr12,
    ascii_char_ptr_no_quotes, with_desired_property_ascii_char_ptr_no_quotes12,
    EDM_DATE_TIME_OFFSET, with_desired_property_EdmDateTimeOffset12,
    EDM_GUID, with_desired_property_EdmGuid12,
    EDM_BINARY, with_desired_property_EdmBinary12
)
DECLARE_MODEL(innerModel12,
    WITH_DESIRED_PROPERTY(struct_WithData12, structure12)
)
DECLARE_MODEL(outerModel12,
    WITH_DESIRED_PROPERTY(innerModel12, inner_model12)
)

END_NAMESPACE(basic12)

BEGIN_NAMESPACE(basic13)

DECLARE_MODEL(model_WithAction13,
    WITH_ACTION(action13, 
        double, double13,
        int, int13,
        float, float13,
        long, long13,
        int8_t, sint8_t13,
        uint8_t, uint8_t13,
        int16_t, int16_t13,
        int32_t, int32_t13,
        int64_t, int64_t13,
        bool, bool13,
        ascii_char_ptr, ascii_char_ptr13,
        ascii_char_ptr_no_quotes, ascii_char_ptr_no_quotes13,
        EDM_DATE_TIME_OFFSET, EdmDateTimeOffset13,
        EDM_GUID, EdmGuid13, 
        EDM_BINARY, EdmBinary13)
    )

END_NAMESPACE(basic13)

EXECUTE_COMMAND_RESULT action13(model_WithAction13* model,
    double double13,
    int int13,
    float float13,
    long long13,
    int8_t sint8_t13,
    uint8_t uint8_t13,
    int16_t int16_t13,
    int32_t int32_t13,
    int64_t int64_t13,
    bool bool13,
    ascii_char_ptr ascii_char_ptr13,
    ascii_char_ptr_no_quotes ascii_char_ptr_no_quotes13,
    EDM_DATE_TIME_OFFSET EdmDateTimeOffset13,
    EDM_GUID EdmGuid13,
    EDM_BINARY EdmBinary13
)
{
    (void)(model);
    ASSERT_ARE_EQUAL(double,    1.0,           double13);
    ASSERT_ARE_EQUAL(int,       2,             int13);
    ASSERT_ARE_EQUAL(float,     3.0,           float13);
    ASSERT_ARE_EQUAL(long,      4,             long13);
    ASSERT_ARE_EQUAL(int8_t,    5,             sint8_t13);
    ASSERT_ARE_EQUAL(uint8_t,   6,             uint8_t13);
    ASSERT_ARE_EQUAL(int16_t,   7,             int16_t13);
    ASSERT_ARE_EQUAL(int32_t,   8,             int32_t13);
    ASSERT_ARE_EQUAL(int64_t,   9,             int64_t13);
    ASSERT_IS_TRUE(bool13);
    ASSERT_ARE_EQUAL(char_ptr,  "eleven",       ascii_char_ptr13);
    ASSERT_ARE_EQUAL(char_ptr,  "\"twelve\"",   ascii_char_ptr_no_quotes13);
    ASSERT_ARE_EQUAL(int,       114,            EdmDateTimeOffset13.dateTime.tm_year);
    ASSERT_ARE_EQUAL(int,       6-1,            EdmDateTimeOffset13.dateTime.tm_mon);
    ASSERT_ARE_EQUAL(int,       17,             EdmDateTimeOffset13.dateTime.tm_mday);
    ASSERT_ARE_EQUAL(int,       8,              EdmDateTimeOffset13.dateTime.tm_hour);
    ASSERT_ARE_EQUAL(int,       51,             EdmDateTimeOffset13.dateTime.tm_min);
    ASSERT_ARE_EQUAL(int,       23,             EdmDateTimeOffset13.dateTime.tm_sec);
    ASSERT_ARE_EQUAL(uint8_t,   1,              EdmDateTimeOffset13.hasFractionalSecond);
    ASSERT_ARE_EQUAL(uint64_t,  5,              EdmDateTimeOffset13.fractionalSecond);
    ASSERT_ARE_EQUAL(uint8_t,   1,              EdmDateTimeOffset13.hasTimeZone);
    ASSERT_ARE_EQUAL(int8_t,    -8,             EdmDateTimeOffset13.timeZoneHour);
    ASSERT_ARE_EQUAL(uint8_t,   1,              EdmDateTimeOffset13.timeZoneMinute);
    ASSERT_ARE_EQUAL(uint8_t,   0x00,           EdmGuid13.GUID[0]);
    ASSERT_ARE_EQUAL(uint8_t,   0x11,           EdmGuid13.GUID[1]);
    ASSERT_ARE_EQUAL(uint8_t,   0x22,           EdmGuid13.GUID[2]);
    ASSERT_ARE_EQUAL(uint8_t,   0x33,           EdmGuid13.GUID[3]);
    ASSERT_ARE_EQUAL(uint8_t,   0x44,           EdmGuid13.GUID[4]);
    ASSERT_ARE_EQUAL(uint8_t,   0x55,           EdmGuid13.GUID[5]);
    ASSERT_ARE_EQUAL(uint8_t,   0x66,           EdmGuid13.GUID[6]);
    ASSERT_ARE_EQUAL(uint8_t,   0x77,           EdmGuid13.GUID[7]);
    ASSERT_ARE_EQUAL(uint8_t,   0x88,           EdmGuid13.GUID[8]);
    ASSERT_ARE_EQUAL(uint8_t,   0x99,           EdmGuid13.GUID[9]);
    ASSERT_ARE_EQUAL(uint8_t,   0xAA,           EdmGuid13.GUID[10]);
    ASSERT_ARE_EQUAL(uint8_t,   0xBB,           EdmGuid13.GUID[11]);
    ASSERT_ARE_EQUAL(uint8_t,   0xCC,           EdmGuid13.GUID[12]);
    ASSERT_ARE_EQUAL(uint8_t,   0xDD,           EdmGuid13.GUID[13]);
    ASSERT_ARE_EQUAL(uint8_t,   0xEE,           EdmGuid13.GUID[14]);
    ASSERT_ARE_EQUAL(uint8_t,   0xFF,           EdmGuid13.GUID[15]);

    ASSERT_ARE_EQUAL(size_t,     3,             EdmBinary13.size);
    ASSERT_ARE_EQUAL(uint8_t,   '3',            EdmBinary13.data[0]);
    ASSERT_ARE_EQUAL(uint8_t,   '4',            EdmBinary13.data[1]);
    ASSERT_ARE_EQUAL(uint8_t,   '5',            EdmBinary13.data[2]);
    return EXECUTE_COMMAND_SUCCESS;
}

BEGIN_NAMESPACE(basic14)

DECLARE_MODEL(model_WithAction14,
    WITH_ACTION(action14,
        double, double14,
        int, int14,
        float, float14,
        long, long14,
        int8_t, sint8_t14,
        uint8_t, uint8_t14,
        int16_t, int16_t14,
        int32_t, int32_t14,
        int64_t, int64_t14,
        bool, bool14,
        ascii_char_ptr, ascii_char_ptr14,
        ascii_char_ptr_no_quotes, ascii_char_ptr_no_quotes14,
        EDM_DATE_TIME_OFFSET, EdmDateTimeOffset14,
        EDM_GUID, EdmGuid14,
        EDM_BINARY, EdmBinary14)
    );

DECLARE_MODEL(outerModel14,
    WITH_DATA(model_WithAction14, modelAction14)
);
END_NAMESPACE(basic14)

EXECUTE_COMMAND_RESULT action14(model_WithAction14* model,
    double double14,
    int int14,
    float float14,
    long long14,
    int8_t sint8_t14,
    uint8_t uint8_t14,
    int16_t int16_t14,
    int32_t int32_t14,
    int64_t int64_t14,
    bool bool14,
    ascii_char_ptr ascii_char_ptr14,
    ascii_char_ptr_no_quotes ascii_char_ptr_no_quotes14,
    EDM_DATE_TIME_OFFSET EdmDateTimeOffset14,
    EDM_GUID EdmGuid14,
    EDM_BINARY EdmBinary14
)
{
    (void)(model);
    ASSERT_ARE_EQUAL(double,    1.0,           double14);
    ASSERT_ARE_EQUAL(int,       2,             int14);
    ASSERT_ARE_EQUAL(float,     3.0,           float14);
    ASSERT_ARE_EQUAL(long,      4,             long14);
    ASSERT_ARE_EQUAL(int8_t,    5,             sint8_t14);
    ASSERT_ARE_EQUAL(uint8_t,   6,             uint8_t14);
    ASSERT_ARE_EQUAL(int16_t,   7,             int16_t14);
    ASSERT_ARE_EQUAL(int32_t,   8,             int32_t14);
    ASSERT_ARE_EQUAL(int64_t,   9,             int64_t14);
    ASSERT_IS_TRUE(bool14);
    ASSERT_ARE_EQUAL(char_ptr,  "eleven",       ascii_char_ptr14);
    ASSERT_ARE_EQUAL(char_ptr,  "\"twelve\"",   ascii_char_ptr_no_quotes14);
    ASSERT_ARE_EQUAL(int,       114,            EdmDateTimeOffset14.dateTime.tm_year);
    ASSERT_ARE_EQUAL(int,       6-1,            EdmDateTimeOffset14.dateTime.tm_mon);
    ASSERT_ARE_EQUAL(int,       17,             EdmDateTimeOffset14.dateTime.tm_mday);
    ASSERT_ARE_EQUAL(int,       8,              EdmDateTimeOffset14.dateTime.tm_hour);
    ASSERT_ARE_EQUAL(int,       51,             EdmDateTimeOffset14.dateTime.tm_min);
    ASSERT_ARE_EQUAL(int,       23,             EdmDateTimeOffset14.dateTime.tm_sec);
    ASSERT_ARE_EQUAL(uint8_t,   1,              EdmDateTimeOffset14.hasFractionalSecond);
    ASSERT_ARE_EQUAL(uint64_t,  5,              EdmDateTimeOffset14.fractionalSecond);
    ASSERT_ARE_EQUAL(uint8_t,   1,              EdmDateTimeOffset14.hasTimeZone);
    ASSERT_ARE_EQUAL(int8_t,    -8,             EdmDateTimeOffset14.timeZoneHour);
    ASSERT_ARE_EQUAL(uint8_t,   1,              EdmDateTimeOffset14.timeZoneMinute);
    ASSERT_ARE_EQUAL(uint8_t,   0x00,           EdmGuid14.GUID[0]);
    ASSERT_ARE_EQUAL(uint8_t,   0x11,           EdmGuid14.GUID[1]);
    ASSERT_ARE_EQUAL(uint8_t,   0x22,           EdmGuid14.GUID[2]);
    ASSERT_ARE_EQUAL(uint8_t,   0x33,           EdmGuid14.GUID[3]);
    ASSERT_ARE_EQUAL(uint8_t,   0x44,           EdmGuid14.GUID[4]);
    ASSERT_ARE_EQUAL(uint8_t,   0x55,           EdmGuid14.GUID[5]);
    ASSERT_ARE_EQUAL(uint8_t,   0x66,           EdmGuid14.GUID[6]);
    ASSERT_ARE_EQUAL(uint8_t,   0x77,           EdmGuid14.GUID[7]);
    ASSERT_ARE_EQUAL(uint8_t,   0x88,           EdmGuid14.GUID[8]);
    ASSERT_ARE_EQUAL(uint8_t,   0x99,           EdmGuid14.GUID[9]);
    ASSERT_ARE_EQUAL(uint8_t,   0xAA,           EdmGuid14.GUID[10]);
    ASSERT_ARE_EQUAL(uint8_t,   0xBB,           EdmGuid14.GUID[11]);
    ASSERT_ARE_EQUAL(uint8_t,   0xCC,           EdmGuid14.GUID[12]);
    ASSERT_ARE_EQUAL(uint8_t,   0xDD,           EdmGuid14.GUID[13]);
    ASSERT_ARE_EQUAL(uint8_t,   0xEE,           EdmGuid14.GUID[14]);
    ASSERT_ARE_EQUAL(uint8_t,   0xFF,           EdmGuid14.GUID[15]);

    ASSERT_ARE_EQUAL(size_t,     3,             EdmBinary14.size);
    ASSERT_ARE_EQUAL(uint8_t,   '3',            EdmBinary14.data[0]);
    ASSERT_ARE_EQUAL(uint8_t,   '4',            EdmBinary14.data[1]);
    ASSERT_ARE_EQUAL(uint8_t,   '5',            EdmBinary14.data[2]);
    return EXECUTE_COMMAND_SUCCESS;
}

BEGIN_NAMESPACE(basic15)

DECLARE_MODEL(basicModel_WithData15,
    WITH_DESIRED_PROPERTY(double, with_desired_property_double15, on_desired_property_double15),
    WITH_DESIRED_PROPERTY(int, with_desired_property_int15, on_desired_property_int15),
    WITH_DESIRED_PROPERTY(float, with_desired_property_float15, on_desired_property_float15),
    WITH_DESIRED_PROPERTY(long, with_desired_property_long15, on_desired_property_long15),
    WITH_DESIRED_PROPERTY(int8_t, with_desired_property_sint8_t15, on_desired_property_sint8_t15),
    WITH_DESIRED_PROPERTY(uint8_t, with_desired_property_uint8_t15, on_desired_property_uint8_t15),
    WITH_DESIRED_PROPERTY(int16_t, with_desired_property_int16_t15, on_desired_property_int16_t15),
    WITH_DESIRED_PROPERTY(int32_t, with_desired_property_int32_t15, on_desired_property_int32_t15),
    WITH_DESIRED_PROPERTY(int64_t, with_desired_property_int64_t15, on_desired_property_int64_t15),
    WITH_DESIRED_PROPERTY(bool, with_desired_property_bool15, on_desired_property_bool15),
    WITH_DESIRED_PROPERTY(ascii_char_ptr, with_desired_property_ascii_char_ptr15, on_desired_property_ascii_char_ptr15),
    WITH_DESIRED_PROPERTY(ascii_char_ptr_no_quotes, with_desired_property_ascii_char_ptr_no_quotes15, on_desired_property_ascii_char_ptr_no_quotes15),
    WITH_DESIRED_PROPERTY(EDM_DATE_TIME_OFFSET, with_desired_property_EdmDateTimeOffset15, on_desired_property_EdmDateTimeOffset15),
    WITH_DESIRED_PROPERTY(EDM_GUID, with_desired_property_EdmGuid15, on_desired_property_EdmGuid15),
    WITH_DESIRED_PROPERTY(EDM_BINARY, with_desired_property_EdmBinary15, on_desired_property_EdmBinary15)
)
END_NAMESPACE(basic15)

#define ENABLE_MOCKS
    #include "azure_c_shared_utility/umock_c_prod.h"
    MOCKABLE_FUNCTION(, void, on_desired_property_double15, void*, v);
    MOCKABLE_FUNCTION(, void, on_desired_property_int15, void*, v);
    MOCKABLE_FUNCTION(, void, on_desired_property_float15, void*, v);
    MOCKABLE_FUNCTION(, void, on_desired_property_long15, void*, v);
    MOCKABLE_FUNCTION(, void, on_desired_property_sint8_t15, void*, v);
    MOCKABLE_FUNCTION(, void, on_desired_property_uint8_t15, void*, v);
    MOCKABLE_FUNCTION(, void, on_desired_property_int16_t15, void*, v);
    MOCKABLE_FUNCTION(, void, on_desired_property_int32_t15, void*, v);
    MOCKABLE_FUNCTION(, void, on_desired_property_int64_t15, void*, v);
    MOCKABLE_FUNCTION(, void, on_desired_property_bool15, void*, v);
    MOCKABLE_FUNCTION(, void, on_desired_property_ascii_char_ptr15, void*, v);
    MOCKABLE_FUNCTION(, void, on_desired_property_ascii_char_ptr_no_quotes15, void*, v);
    MOCKABLE_FUNCTION(, void, on_desired_property_EdmDateTimeOffset15, void*, v);
    MOCKABLE_FUNCTION(, void, on_desired_property_EdmGuid15, void*, v);
    MOCKABLE_FUNCTION(, void, on_desired_property_EdmBinary15, void*, v);
#undef ENABLE_MOCKS

BEGIN_NAMESPACE(basic16)

DECLARE_STRUCT(struct_WithData16,
    double, with_desired_property_double16,
    int, with_desired_property_int16,
    float, with_desired_property_float16,
    long, with_desired_property_long16,
    int8_t, with_desired_property_sint8_t16,
    uint8_t, with_desired_property_uint8_t16,
    int16_t, with_desired_property_int16_t16,
    int32_t, with_desired_property_int32_t16,
    int64_t, with_desired_property_int64_t16,
    bool, with_desired_property_bool16,
    ascii_char_ptr, with_desired_property_ascii_char_ptr16,
    ascii_char_ptr_no_quotes, with_desired_property_ascii_char_ptr_no_quotes16,
    EDM_DATE_TIME_OFFSET, with_desired_property_EdmDateTimeOffset16,
    EDM_GUID, with_desired_property_EdmGuid16,
    EDM_BINARY, with_desired_property_EdmBinary16
)
DECLARE_MODEL(innerModel16,
    WITH_DESIRED_PROPERTY(struct_WithData16, structure16, on_structure16)
)
DECLARE_MODEL(outerModel16,
    WITH_DESIRED_PROPERTY(innerModel16, inner_model16, on_inner_model16)
)

END_NAMESPACE(basic16)

BEGIN_NAMESPACE(basic17)

/*this declares a struct whose type shall used, reused and abused in the following declarations*/
DECLARE_STRUCT(totallyNotUnique,
    double, totallyNotUnique
)
DECLARE_MODEL(totallyNotUnique_model1,
    WITH_DATA(totallyNotUnique, tnu)
)

DECLARE_MODEL(totallyNotUnique_model3,
    WITH_ACTION(A, totallyNotUnique, tnu)
)

DECLARE_MODEL(totallyNotUnique_model4,
    WITH_REPORTED_PROPERTY(totallyNotUnique, tnu)
)
    
DECLARE_MODEL(totallyNotUnique_model6,
    WITH_DESIRED_PROPERTY(totallyNotUnique, tnu)
)

DECLARE_MODEL(totallyNotUnique_model7,
    WITH_DATA(totallyNotUnique, tnu)
)

DECLARE_MODEL(totallyNotUnique_model8,
    WITH_ACTION(A2, totallyNotUnique, tnu)
)

DECLARE_MODEL(totallyNotUnique_model9,
    WITH_REPORTED_PROPERTY(totallyNotUnique, tnu)
    )

DECLARE_MODEL(totallyNotUnique_model10,
    WITH_DESIRED_PROPERTY(totallyNotUnique, tnu)
)

END_NAMESPACE(basic17)

EXECUTE_COMMAND_RESULT A(totallyNotUnique_model3* m3, totallyNotUnique tnu)
{
    (void)m3, tnu;
    return EXECUTE_COMMAND_SUCCESS;
}

EXECUTE_COMMAND_RESULT A2(totallyNotUnique_model8* m8, totallyNotUnique tnu)
{
    (void)m8, tnu;
    return EXECUTE_COMMAND_SUCCESS;
}

#define ENABLE_MOCKS
#include "azure_c_shared_utility/umock_c_prod.h"
    MOCKABLE_FUNCTION(, void, on_structure16, void*, v);
    MOCKABLE_FUNCTION(, void, on_inner_model16, void*, v);
#undef ENABLE_MOCKS

TEST_DEFINE_ENUM_TYPE(SERIALIZER_RESULT, SERIALIZER_RESULT_VALUES);
TEST_DEFINE_ENUM_TYPE(CODEFIRST_RESULT, CODEFIRST_RESULT_VALUES);

/*returns 0 is the two jsons are not equal, any other value means they are equal*/
/*typically "left" is the output of SERIALIZE... and right is hand-coded JSON*/
static bool areTwoJsonsEqual(const unsigned char* left, size_t leftSize, const char* right)
{
    bool result;

    char* cloneOfLeft = (char*)malloc(leftSize + 1); /*because of out SERIALIZE... there is a byte array that is NOT '\0' terminated*/
    ASSERT_IS_NOT_NULL(cloneOfLeft);
    
    memcpy(cloneOfLeft, left, leftSize);
    cloneOfLeft[leftSize] = '\0';
    
    JSON_Value* actualJson = json_parse_string((char*)cloneOfLeft);
    ASSERT_IS_NOT_NULL(actualJson);
    JSON_Value* expectedJson = json_parse_string(right);
    ASSERT_IS_NOT_NULL(expectedJson);

    result = (json_value_equals(expectedJson, actualJson) != 0);
    
    json_value_free(expectedJson);
    json_value_free(actualJson);
    free(cloneOfLeft);

    return result;
}

DEFINE_ENUM_STRINGS(UMOCK_C_ERROR_CODE, UMOCK_C_ERROR_CODE_VALUES)
static void on_umock_c_error(UMOCK_C_ERROR_CODE error_code)
{
    char temp_str[256];
    (void)snprintf(temp_str, sizeof(temp_str), "umock_c reported error :%s", ENUM_TO_STRING(UMOCK_C_ERROR_CODE, error_code));
    ASSERT_FAIL(temp_str);
}

BEGIN_TEST_SUITE(serializer_int)

    TEST_SUITE_INITIALIZE(TestClassInitialize)
    {
        TEST_INITIALIZE_MEMORY_DEBUG(g_dllByDll);
        g_testByTest = TEST_MUTEX_CREATE();
        ASSERT_IS_NOT_NULL(g_testByTest);

        (void)umock_c_init(on_umock_c_error);

        (void)umocktypes_bool_register_types();
        (void)umocktypes_charptr_register_types();
        (void)umocktypes_stdint_register_types();
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

        umock_c_reset_all_calls();

    }

    TEST_FUNCTION_CLEANUP(TestMethodCleanup)
    {
        TEST_MUTEX_RELEASE(g_testByTest);
    }
    /*the following test has a model consisting only of root level WITH_DATA properties of all types*/
    /*conceptually:
    MODEL
    |--- with_data_double
    |--- with_data_int
    ...
    */
    TEST_FUNCTION(WITH_DATA_IN_ROOT_MODEL)
    {
        ///arrange
        basicModel_WithData1 *modelWithData = CREATE_MODEL_INSTANCE(basic1, basicModel_WithData1, true);

        /*setting values to the model instance*/
        modelWithData->with_data_double1 = 1.0;
        modelWithData->with_data_int1 = 2;
        modelWithData->with_data_float1 = 3.0;
        modelWithData->with_data_long1 = 4;
        modelWithData->with_data_sint8_t1 = 5;
        modelWithData->with_data_uint8_t1 = 6;
        modelWithData->with_data_int16_t1 = 7;
        modelWithData->with_data_int32_t1 = 8;
        modelWithData->with_data_int64_t1 = 9;
        modelWithData->with_data_bool1 = true;
        modelWithData->with_data_ascii_char_ptr1 = "eleven";
        modelWithData->with_data_ascii_char_ptr_no_quotes1 = "\"twelve\"";
        modelWithData->with_data_EdmDateTimeOffset1.dateTime.tm_year = 114;
        modelWithData->with_data_EdmDateTimeOffset1.dateTime.tm_mon = 6 - 1;
        modelWithData->with_data_EdmDateTimeOffset1.dateTime.tm_mday = 17;
        modelWithData->with_data_EdmDateTimeOffset1.dateTime.tm_hour = 8;
        modelWithData->with_data_EdmDateTimeOffset1.dateTime.tm_min = 51;
        modelWithData->with_data_EdmDateTimeOffset1.dateTime.tm_sec = 23;
        modelWithData->with_data_EdmDateTimeOffset1.hasFractionalSecond = 1;
        modelWithData->with_data_EdmDateTimeOffset1.fractionalSecond = 5;
        modelWithData->with_data_EdmDateTimeOffset1.hasTimeZone = 1;
        modelWithData->with_data_EdmDateTimeOffset1.timeZoneHour = -8;
        modelWithData->with_data_EdmDateTimeOffset1.timeZoneMinute = 1;
        modelWithData->with_data_EdmGuid1.GUID[0] = 0x00;
        modelWithData->with_data_EdmGuid1.GUID[1] = 0x11;
        modelWithData->with_data_EdmGuid1.GUID[2] = 0x22;
        modelWithData->with_data_EdmGuid1.GUID[3] = 0x33;
        modelWithData->with_data_EdmGuid1.GUID[4] = 0x44;
        modelWithData->with_data_EdmGuid1.GUID[5] = 0x55;
        modelWithData->with_data_EdmGuid1.GUID[6] = 0x66;
        modelWithData->with_data_EdmGuid1.GUID[7] = 0x77;
        modelWithData->with_data_EdmGuid1.GUID[8] = 0x88;
        modelWithData->with_data_EdmGuid1.GUID[9] = 0x99;
        modelWithData->with_data_EdmGuid1.GUID[10] = 0xAA;
        modelWithData->with_data_EdmGuid1.GUID[11] = 0xBB;
        modelWithData->with_data_EdmGuid1.GUID[12] = 0xCC;
        modelWithData->with_data_EdmGuid1.GUID[13] = 0xDD;
        modelWithData->with_data_EdmGuid1.GUID[14] = 0xEE;
        modelWithData->with_data_EdmGuid1.GUID[15] = 0xFF;

        unsigned char edmBinary[3] = { '3', '4', '5' };
        modelWithData->with_data_EdmBinary1.data = edmBinary;
        modelWithData->with_data_EdmBinary1.size = 3;

        const char* expectedJsonAsString =
            "{                                                                                   \
            \"with_data_double1\" : 1.0,                                                          \
            \"with_data_int1\" : 2,                                                               \
            \"with_data_float1\" : 3.000000,                                                      \
            \"with_data_long1\" : 4,                                                              \
            \"with_data_sint8_t1\" : 5,                                                           \
            \"with_data_uint8_t1\" : 6,                                                           \
            \"with_data_int16_t1\" : 7,                                                           \
            \"with_data_int32_t1\" : 8,                                                           \
            \"with_data_int64_t1\" : 9,                                                           \
            \"with_data_bool1\" : true,                                                           \
            \"with_data_ascii_char_ptr1\" : \"eleven\",                                           \
            \"with_data_ascii_char_ptr_no_quotes1\" : \"twelve\",                                 \
            \"with_data_EdmDateTimeOffset1\" : \"2014-06-17T08:51:23.000000000005-08:01\",        \
            \"with_data_EdmGuid1\" : \"00112233-4455-6677-8899-AABBCCDDEEFF\",                    \
            \"with_data_EdmBinary1\": \"MzQ1\"                                                    \
        }";

        unsigned char* destination;
        size_t destinationSize;

        ///act
        CODEFIRST_RESULT result = SERIALIZE(&destination, &destinationSize, *modelWithData);

        ///assert
        ASSERT_ARE_EQUAL(CODEFIRST_RESULT, CODEFIRST_OK, result);
        ASSERT_IS_TRUE(areTwoJsonsEqual(destination, destinationSize, expectedJsonAsString));

        ///clean
        free(destination);
        DESTROY_MODEL_INSTANCE(modelWithData);
    }

    /*the following test has a model that has a single WITH_DATA of structure type having fields of all types*/
    /*conceptually:
    MODEL
    |--- STRUCT
         |--- with_data_int
         |--- with_data_float
         ...
    */
    TEST_FUNCTION(WITH_DATA_IN_STRUCT_IN_ROOT_MODEL)
    {
        ///arrange
        basicModel_WithStruct2 *modelWithStruct = CREATE_MODEL_INSTANCE(basic2, basicModel_WithStruct2, true);

        /*setting values to the model instance*/
        modelWithStruct->structure2.with_data_double2 = 1.0;
        modelWithStruct->structure2.with_data_int2 = 2;
        modelWithStruct->structure2.with_data_float2 = 3.0;
        modelWithStruct->structure2.with_data_long2 = 4;
        modelWithStruct->structure2.with_data_sint8_t2 = 5;
        modelWithStruct->structure2.with_data_uint8_t2 = 6;
        modelWithStruct->structure2.with_data_int16_t2 = 7;
        modelWithStruct->structure2.with_data_int32_t2 = 8;
        modelWithStruct->structure2.with_data_int64_t2 = 9;
        modelWithStruct->structure2.with_data_bool2 = true;
        modelWithStruct->structure2.with_data_ascii_char_ptr2 = "eleven";
        modelWithStruct->structure2.with_data_ascii_char_ptr_no_quotes2 = "\"twelve\"";
        modelWithStruct->structure2.with_data_EdmDateTimeOffset2.dateTime.tm_year = 114;
        modelWithStruct->structure2.with_data_EdmDateTimeOffset2.dateTime.tm_mon = 6 - 1;
        modelWithStruct->structure2.with_data_EdmDateTimeOffset2.dateTime.tm_mday = 17;
        modelWithStruct->structure2.with_data_EdmDateTimeOffset2.dateTime.tm_hour = 8;
        modelWithStruct->structure2.with_data_EdmDateTimeOffset2.dateTime.tm_min = 51;
        modelWithStruct->structure2.with_data_EdmDateTimeOffset2.dateTime.tm_sec = 23;
        modelWithStruct->structure2.with_data_EdmDateTimeOffset2.hasFractionalSecond = 1;
        modelWithStruct->structure2.with_data_EdmDateTimeOffset2.fractionalSecond = 5;
        modelWithStruct->structure2.with_data_EdmDateTimeOffset2.hasTimeZone = 1;
        modelWithStruct->structure2.with_data_EdmDateTimeOffset2.timeZoneHour = -8;
        modelWithStruct->structure2.with_data_EdmDateTimeOffset2.timeZoneMinute = 1;
        modelWithStruct->structure2.with_data_EdmGuid2.GUID[0] = 0x00;
        modelWithStruct->structure2.with_data_EdmGuid2.GUID[1] = 0x11;
        modelWithStruct->structure2.with_data_EdmGuid2.GUID[2] = 0x22;
        modelWithStruct->structure2.with_data_EdmGuid2.GUID[3] = 0x33;
        modelWithStruct->structure2.with_data_EdmGuid2.GUID[4] = 0x44;
        modelWithStruct->structure2.with_data_EdmGuid2.GUID[5] = 0x55;
        modelWithStruct->structure2.with_data_EdmGuid2.GUID[6] = 0x66;
        modelWithStruct->structure2.with_data_EdmGuid2.GUID[7] = 0x77;
        modelWithStruct->structure2.with_data_EdmGuid2.GUID[8] = 0x88;
        modelWithStruct->structure2.with_data_EdmGuid2.GUID[9] = 0x99;
        modelWithStruct->structure2.with_data_EdmGuid2.GUID[10] = 0xAA;
        modelWithStruct->structure2.with_data_EdmGuid2.GUID[11] = 0xBB;
        modelWithStruct->structure2.with_data_EdmGuid2.GUID[12] = 0xCC;
        modelWithStruct->structure2.with_data_EdmGuid2.GUID[13] = 0xDD;
        modelWithStruct->structure2.with_data_EdmGuid2.GUID[14] = 0xEE;
        modelWithStruct->structure2.with_data_EdmGuid2.GUID[15] = 0xFF;

        unsigned char edmBinary[3] = { '3', '4', '5' };
        modelWithStruct->structure2.with_data_EdmBinary2.data = edmBinary;
        modelWithStruct->structure2.with_data_EdmBinary2.size = 3;

        const char* expectedJsonAsString =
            "{                                                                                       \
            \"structure2\":                                                                          \
            {                                                                                        \
                \"with_data_double2\" : 1.0,                                                         \
                \"with_data_int2\" : 2,                                                              \
                \"with_data_float2\" : 3.000000,                                                     \
                \"with_data_long2\" : 4,                                                             \
                \"with_data_sint8_t2\" : 5,                                                          \
                \"with_data_uint8_t2\" : 6,                                                          \
                \"with_data_int16_t2\" : 7,                                                          \
                \"with_data_int32_t2\" : 8,                                                          \
                \"with_data_int64_t2\" : 9,                                                          \
                \"with_data_bool2\" : true,                                                          \
                \"with_data_ascii_char_ptr2\" : \"eleven\",                                          \
                \"with_data_ascii_char_ptr_no_quotes2\" : \"twelve\",                                \
                \"with_data_EdmDateTimeOffset2\" : \"2014-06-17T08:51:23.000000000005-08:01\",       \
                \"with_data_EdmGuid2\" : \"00112233-4455-6677-8899-AABBCCDDEEFF\",                   \
                \"with_data_EdmBinary2\": \"MzQ1\"                                                   \
            }                                                                                        \
        }";

        unsigned char* destination;
        size_t destinationSize;

        ///act
        CODEFIRST_RESULT result = SERIALIZE(&destination, &destinationSize, *modelWithStruct);

        ///assert
        ASSERT_ARE_EQUAL(CODEFIRST_RESULT, CODEFIRST_OK, result);
        ASSERT_IS_TRUE(areTwoJsonsEqual(destination, destinationSize, expectedJsonAsString));
        
        ///clean
        free(destination);
        DESTROY_MODEL_INSTANCE(modelWithStruct);
    }

    /*the following test has a model that has a single WITH_DATA of structure type having fields of all types*/
    /*conceptually:
    MODEL
    |--- MODEL
         |--- with_data_int
         |--- with_data_float
         ...
    */
    TEST_FUNCTION(WITH_DATA_IN_MODEL_IN_MODEL)
    {
        ///arrange
        basicModel_WithModel3 *modelWithModel = CREATE_MODEL_INSTANCE(basic3, basicModel_WithModel3, true);

        /*setting values to the model instance*/
        modelWithModel->model3.with_data_double3 = 1.0;
        modelWithModel->model3.with_data_int3 = 2;
        modelWithModel->model3.with_data_float3 = 3.0;
        modelWithModel->model3.with_data_long3 = 4;
        modelWithModel->model3.with_data_sint8_t3 = 5;
        modelWithModel->model3.with_data_uint8_t3 = 6;
        modelWithModel->model3.with_data_int16_t3 = 7;
        modelWithModel->model3.with_data_int32_t3 = 8;
        modelWithModel->model3.with_data_int64_t3 = 9;
        modelWithModel->model3.with_data_bool3 = true;
        modelWithModel->model3.with_data_ascii_char_ptr3 = "eleven";
        modelWithModel->model3.with_data_ascii_char_ptr_no_quotes3 = "\"twelve\"";
        modelWithModel->model3.with_data_EdmDateTimeOffset3.dateTime.tm_year = 114;
        modelWithModel->model3.with_data_EdmDateTimeOffset3.dateTime.tm_mon = 6 - 1;
        modelWithModel->model3.with_data_EdmDateTimeOffset3.dateTime.tm_mday = 17;
        modelWithModel->model3.with_data_EdmDateTimeOffset3.dateTime.tm_hour = 8;
        modelWithModel->model3.with_data_EdmDateTimeOffset3.dateTime.tm_min = 51;
        modelWithModel->model3.with_data_EdmDateTimeOffset3.dateTime.tm_sec = 23;
        modelWithModel->model3.with_data_EdmDateTimeOffset3.hasFractionalSecond = 1;
        modelWithModel->model3.with_data_EdmDateTimeOffset3.fractionalSecond = 5;
        modelWithModel->model3.with_data_EdmDateTimeOffset3.hasTimeZone = 1;
        modelWithModel->model3.with_data_EdmDateTimeOffset3.timeZoneHour = -8;
        modelWithModel->model3.with_data_EdmDateTimeOffset3.timeZoneMinute = 1;
        modelWithModel->model3.with_data_EdmGuid3.GUID[1] = 0x11;
        modelWithModel->model3.with_data_EdmGuid3.GUID[0] = 0x00;
        modelWithModel->model3.with_data_EdmGuid3.GUID[2] = 0x22;
        modelWithModel->model3.with_data_EdmGuid3.GUID[3] = 0x33;
        modelWithModel->model3.with_data_EdmGuid3.GUID[4] = 0x44;
        modelWithModel->model3.with_data_EdmGuid3.GUID[5] = 0x55;
        modelWithModel->model3.with_data_EdmGuid3.GUID[6] = 0x66;
        modelWithModel->model3.with_data_EdmGuid3.GUID[7] = 0x77;
        modelWithModel->model3.with_data_EdmGuid3.GUID[8] = 0x88;
        modelWithModel->model3.with_data_EdmGuid3.GUID[9] = 0x99;
        modelWithModel->model3.with_data_EdmGuid3.GUID[10] = 0xAA;
        modelWithModel->model3.with_data_EdmGuid3.GUID[11] = 0xBB;
        modelWithModel->model3.with_data_EdmGuid3.GUID[12] = 0xCC;
        modelWithModel->model3.with_data_EdmGuid3.GUID[13] = 0xDD;
        modelWithModel->model3.with_data_EdmGuid3.GUID[14] = 0xEE;
        modelWithModel->model3.with_data_EdmGuid3.GUID[15] = 0xFF;

        unsigned char edmBinary[3] = { '3', '4', '5' };
        modelWithModel->model3.with_data_EdmBinary3.data = edmBinary;
        modelWithModel->model3.with_data_EdmBinary3.size = 3;

        const char* expectedJsonAsString =
            "{                                                                                        \
            \"model3\":                                                                               \
            {                                                                                         \
                \"with_data_double3\" : 1.0,                                                          \
                \"with_data_int3\" : 2,                                                               \
                \"with_data_float3\" : 3.000000,                                                      \
                \"with_data_long3\" : 4,                                                              \
                \"with_data_sint8_t3\" : 5,                                                           \
                \"with_data_uint8_t3\" : 6,                                                           \
                \"with_data_int16_t3\" : 7,                                                           \
                \"with_data_int32_t3\" : 8,                                                           \
                \"with_data_int64_t3\" : 9,                                                           \
                \"with_data_bool3\" : true,                                                           \
                \"with_data_ascii_char_ptr3\" : \"eleven\",                                           \
                \"with_data_ascii_char_ptr_no_quotes3\" : \"twelve\",                                 \
                \"with_data_EdmDateTimeOffset3\" : \"2014-06-17T08:51:23.000000000005-08:01\",        \
                \"with_data_EdmGuid3\" : \"00112233-4455-6677-8899-AABBCCDDEEFF\",                    \
                \"with_data_EdmBinary3\": \"MzQ1\"                                                    \
            }                                                                                         \
        }";

        unsigned char* destination;
        size_t destinationSize;

        ///act
        CODEFIRST_RESULT result = SERIALIZE(&destination, &destinationSize, *modelWithModel);

        ///assert
        ASSERT_ARE_EQUAL(CODEFIRST_RESULT, CODEFIRST_OK, result);
        ASSERT_IS_TRUE(areTwoJsonsEqual(destination, destinationSize, expectedJsonAsString));

        ///clean
        free(destination);
        DESTROY_MODEL_INSTANCE(modelWithModel);
    }

     /*the following test has a model that has a single WITH_DATA of structure type having fields of all types*/
    /*conceptually:
    MODEL
    |--- MODEL
         |--- STRUCT
              |--- with_data_float
              |--- with_data_XXX
    */
    TEST_FUNCTION(WITH_DATA_IN_STRUCT_IN_MODEL_IN_MODEL)
    {
        ///arrange
        outerModel4 *modelWithModel = CREATE_MODEL_INSTANCE(basic4, outerModel4, true);

        /*setting values to the model instance*/
        modelWithModel->inner_model4.structure4.with_data_double4 = 1.0;
        modelWithModel->inner_model4.structure4.with_data_int4 = 2;
        modelWithModel->inner_model4.structure4.with_data_float4 = 3.0;
        modelWithModel->inner_model4.structure4.with_data_long4 = 4;
        modelWithModel->inner_model4.structure4.with_data_sint8_t4 = 5;
        modelWithModel->inner_model4.structure4.with_data_uint8_t4 = 6;
        modelWithModel->inner_model4.structure4.with_data_int16_t4 = 7;
        modelWithModel->inner_model4.structure4.with_data_int32_t4 = 8;
        modelWithModel->inner_model4.structure4.with_data_int64_t4 = 9;
        modelWithModel->inner_model4.structure4.with_data_bool4 = true;
        modelWithModel->inner_model4.structure4.with_data_ascii_char_ptr4 = "eleven";
        modelWithModel->inner_model4.structure4.with_data_ascii_char_ptr_no_quotes4 = "\"twelve\"";
        modelWithModel->inner_model4.structure4.with_data_EdmDateTimeOffset4.dateTime.tm_year = 114;
        modelWithModel->inner_model4.structure4.with_data_EdmDateTimeOffset4.dateTime.tm_mon = 6 - 1;
        modelWithModel->inner_model4.structure4.with_data_EdmDateTimeOffset4.dateTime.tm_mday = 17;
        modelWithModel->inner_model4.structure4.with_data_EdmDateTimeOffset4.dateTime.tm_hour = 8;
        modelWithModel->inner_model4.structure4.with_data_EdmDateTimeOffset4.dateTime.tm_min = 51;
        modelWithModel->inner_model4.structure4.with_data_EdmDateTimeOffset4.dateTime.tm_sec = 23;
        modelWithModel->inner_model4.structure4.with_data_EdmDateTimeOffset4.hasFractionalSecond = 1;
        modelWithModel->inner_model4.structure4.with_data_EdmDateTimeOffset4.fractionalSecond = 5;
        modelWithModel->inner_model4.structure4.with_data_EdmDateTimeOffset4.hasTimeZone = 1;
        modelWithModel->inner_model4.structure4.with_data_EdmDateTimeOffset4.timeZoneHour = -8;
        modelWithModel->inner_model4.structure4.with_data_EdmDateTimeOffset4.timeZoneMinute = 1;
        modelWithModel->inner_model4.structure4.with_data_EdmGuid4.GUID[0] = 0x00;
        modelWithModel->inner_model4.structure4.with_data_EdmGuid4.GUID[1] = 0x11;
        modelWithModel->inner_model4.structure4.with_data_EdmGuid4.GUID[2] = 0x22;
        modelWithModel->inner_model4.structure4.with_data_EdmGuid4.GUID[3] = 0x33;
        modelWithModel->inner_model4.structure4.with_data_EdmGuid4.GUID[4] = 0x44;
        modelWithModel->inner_model4.structure4.with_data_EdmGuid4.GUID[5] = 0x55;
        modelWithModel->inner_model4.structure4.with_data_EdmGuid4.GUID[6] = 0x66;
        modelWithModel->inner_model4.structure4.with_data_EdmGuid4.GUID[7] = 0x77;
        modelWithModel->inner_model4.structure4.with_data_EdmGuid4.GUID[8] = 0x88;
        modelWithModel->inner_model4.structure4.with_data_EdmGuid4.GUID[9] = 0x99;
        modelWithModel->inner_model4.structure4.with_data_EdmGuid4.GUID[10] = 0xAA;
        modelWithModel->inner_model4.structure4.with_data_EdmGuid4.GUID[11] = 0xBB;
        modelWithModel->inner_model4.structure4.with_data_EdmGuid4.GUID[12] = 0xCC;
        modelWithModel->inner_model4.structure4.with_data_EdmGuid4.GUID[13] = 0xDD;
        modelWithModel->inner_model4.structure4.with_data_EdmGuid4.GUID[14] = 0xEE;
        modelWithModel->inner_model4.structure4.with_data_EdmGuid4.GUID[15] = 0xFF;

        unsigned char edmBinary[3] = { '3', '4', '5' };
        modelWithModel->inner_model4.structure4.with_data_EdmBinary4.data = edmBinary;
        modelWithModel->inner_model4.structure4.with_data_EdmBinary4.size = 3;

        const char* expectedJsonAsString =
        "{                                                                                            \
            \"inner_model4\":                                                                         \
            {                                                                                         \
                \"structure4\":                                                                       \
                {                                                                                     \
                    \"with_data_double4\" : 1.0,                                                      \
                    \"with_data_int4\" : 2,                                                           \
                    \"with_data_float4\" : 3.000000,                                                  \
                    \"with_data_long4\" : 4,                                                          \
                    \"with_data_sint8_t4\" : 5,                                                       \
                    \"with_data_uint8_t4\" : 6,                                                       \
                    \"with_data_int16_t4\" : 7,                                                       \
                    \"with_data_int32_t4\" : 8,                                                       \
                    \"with_data_int64_t4\" : 9,                                                       \
                    \"with_data_bool4\" : true,                                                       \
                    \"with_data_ascii_char_ptr4\" : \"eleven\",                                       \
                    \"with_data_ascii_char_ptr_no_quotes4\" : \"twelve\",                             \
                    \"with_data_EdmDateTimeOffset4\" : \"2014-06-17T08:51:23.000000000005-08:01\",    \
                    \"with_data_EdmGuid4\" : \"00112233-4455-6677-8899-AABBCCDDEEFF\",                \
                    \"with_data_EdmBinary4\": \"MzQ1\"                                                \
                }                                                                                     \
            }                                                                                         \
        }";

        unsigned char* destination;
        size_t destinationSize;

        ///act
        CODEFIRST_RESULT result = SERIALIZE(&destination, &destinationSize, *modelWithModel);

        ///assert
        ASSERT_ARE_EQUAL(CODEFIRST_RESULT, CODEFIRST_OK, result);
        ASSERT_IS_TRUE(areTwoJsonsEqual(destination, destinationSize, expectedJsonAsString));

        ///clean
        free(destination);
        DESTROY_MODEL_INSTANCE(modelWithModel);
    }

    /*the following test has a model consisting only of root level WITH_REPORTED_PROPERTY properties of all types*/
    /*conceptually:
    MODEL
    |--- with_reported_property_double
    |--- with_reported_property_int
    ...
    */
    TEST_FUNCTION(WITH_REPORTED_PROPERTY_IN_ROOT_MODEL)
    {
        ///arrange
        basicModel_WithReportedProperty5 *modelWithReportedProperty = CREATE_MODEL_INSTANCE(basic5, basicModel_WithReportedProperty5, true);

        /*setting values to the model instance*/
        modelWithReportedProperty->with_reported_property_double5 = 1.0;
        modelWithReportedProperty->with_reported_property_int5 = 2;
        modelWithReportedProperty->with_reported_property_float5 = 3.0;
        modelWithReportedProperty->with_reported_property_long5 = 4;
        modelWithReportedProperty->with_reported_property_sint8_t5 = 5;
        modelWithReportedProperty->with_reported_property_uint8_t5 = 6;
        modelWithReportedProperty->with_reported_property_int16_t5 = 7;
        modelWithReportedProperty->with_reported_property_int32_t5 = 8;
        modelWithReportedProperty->with_reported_property_int64_t5 = 9;
        modelWithReportedProperty->with_reported_property_bool5 = true;
        modelWithReportedProperty->with_reported_property_ascii_char_ptr5 = "eleven";
        modelWithReportedProperty->with_reported_property_ascii_char_ptr_no_quotes5 = "\"twelve\"";
        modelWithReportedProperty->with_reported_property_EdmDateTimeOffset5.dateTime.tm_year = 114;
        modelWithReportedProperty->with_reported_property_EdmDateTimeOffset5.dateTime.tm_mon = 6 - 1;
        modelWithReportedProperty->with_reported_property_EdmDateTimeOffset5.dateTime.tm_mday = 17;
        modelWithReportedProperty->with_reported_property_EdmDateTimeOffset5.dateTime.tm_hour = 8;
        modelWithReportedProperty->with_reported_property_EdmDateTimeOffset5.dateTime.tm_min = 51;
        modelWithReportedProperty->with_reported_property_EdmDateTimeOffset5.dateTime.tm_sec = 23;
        modelWithReportedProperty->with_reported_property_EdmDateTimeOffset5.hasFractionalSecond = 1;
        modelWithReportedProperty->with_reported_property_EdmDateTimeOffset5.fractionalSecond = 5;
        modelWithReportedProperty->with_reported_property_EdmDateTimeOffset5.hasTimeZone = 1;
        modelWithReportedProperty->with_reported_property_EdmDateTimeOffset5.timeZoneHour = -8;
        modelWithReportedProperty->with_reported_property_EdmDateTimeOffset5.timeZoneMinute = 1;
        modelWithReportedProperty->with_reported_property_EdmGuid5.GUID[0] = 0x00;
        modelWithReportedProperty->with_reported_property_EdmGuid5.GUID[1] = 0x11;
        modelWithReportedProperty->with_reported_property_EdmGuid5.GUID[2] = 0x22;
        modelWithReportedProperty->with_reported_property_EdmGuid5.GUID[3] = 0x33;
        modelWithReportedProperty->with_reported_property_EdmGuid5.GUID[4] = 0x44;
        modelWithReportedProperty->with_reported_property_EdmGuid5.GUID[5] = 0x55;
        modelWithReportedProperty->with_reported_property_EdmGuid5.GUID[6] = 0x66;
        modelWithReportedProperty->with_reported_property_EdmGuid5.GUID[7] = 0x77;
        modelWithReportedProperty->with_reported_property_EdmGuid5.GUID[8] = 0x88;
        modelWithReportedProperty->with_reported_property_EdmGuid5.GUID[9] = 0x99;
        modelWithReportedProperty->with_reported_property_EdmGuid5.GUID[10] = 0xAA;
        modelWithReportedProperty->with_reported_property_EdmGuid5.GUID[11] = 0xBB;
        modelWithReportedProperty->with_reported_property_EdmGuid5.GUID[12] = 0xCC;
        modelWithReportedProperty->with_reported_property_EdmGuid5.GUID[13] = 0xDD;
        modelWithReportedProperty->with_reported_property_EdmGuid5.GUID[14] = 0xEE;
        modelWithReportedProperty->with_reported_property_EdmGuid5.GUID[15] = 0xFF;

        unsigned char edmBinary[3] = { '3', '4', '5' };
        modelWithReportedProperty->with_reported_property_EdmBinary5.data = edmBinary;
        modelWithReportedProperty->with_reported_property_EdmBinary5.size = 3;

        const char* expectedJsonAsString =
            "{                                                                                   \
            \"with_reported_property_double5\" : 1.0,                                                          \
            \"with_reported_property_int5\" : 2,                                                               \
            \"with_reported_property_float5\" : 3.000000,                                                      \
            \"with_reported_property_long5\" : 4,                                                              \
            \"with_reported_property_sint8_t5\" : 5,                                                           \
            \"with_reported_property_uint8_t5\" : 6,                                                           \
            \"with_reported_property_int16_t5\" : 7,                                                           \
            \"with_reported_property_int32_t5\" : 8,                                                           \
            \"with_reported_property_int64_t5\" : 9,                                                           \
            \"with_reported_property_bool5\" : true,                                                           \
            \"with_reported_property_ascii_char_ptr5\" : \"eleven\",                                           \
            \"with_reported_property_ascii_char_ptr_no_quotes5\" : \"twelve\",                                 \
            \"with_reported_property_EdmDateTimeOffset5\" : \"2014-06-17T08:51:23.000000000005-08:01\",        \
            \"with_reported_property_EdmGuid5\" : \"00112233-4455-6677-8899-AABBCCDDEEFF\",                    \
            \"with_reported_property_EdmBinary5\": \"MzQ1\"                                                    \
        }";

        unsigned char* destination;
        size_t destinationSize;

        ///act
        CODEFIRST_RESULT result = SERIALIZE_REPORTED_PROPERTIES(&destination, &destinationSize, *modelWithReportedProperty);

        ///assert
        ASSERT_ARE_EQUAL(CODEFIRST_RESULT, CODEFIRST_OK, result);
        ASSERT_IS_TRUE(areTwoJsonsEqual(destination, destinationSize, expectedJsonAsString));

        ///clean
        free(destination);
        DESTROY_MODEL_INSTANCE(modelWithReportedProperty);
    }

    /*the following test has a model that has a single WITH_REPORTED_PROPERTY of structure type having fields of all types*/
    /*conceptually:
    MODEL
    |--- STRUCT
         |--- with_reported_property_int
         |--- with_reported_property_float
    ...
    */
    TEST_FUNCTION(WITH_REPORTED_PROPERTY_IN_STRUCT_IN_ROOT_MODEL)
    {
        ///arrange
        basicModel_WithStruct6 *modelWithStruct = CREATE_MODEL_INSTANCE(basic6, basicModel_WithStruct6, true);

        /*setting values to the model instance*/
        modelWithStruct->structure6.with_reported_property_double6 = 1.0;
        modelWithStruct->structure6.with_reported_property_int6 = 2;
        modelWithStruct->structure6.with_reported_property_float6 = 3.0;
        modelWithStruct->structure6.with_reported_property_long6 = 4;
        modelWithStruct->structure6.with_reported_property_sint8_t6 = 5;
        modelWithStruct->structure6.with_reported_property_uint8_t6 = 6;
        modelWithStruct->structure6.with_reported_property_int16_t6 = 7;
        modelWithStruct->structure6.with_reported_property_int32_t6 = 8;
        modelWithStruct->structure6.with_reported_property_int64_t6 = 9;
        modelWithStruct->structure6.with_reported_property_bool6 = true;
        modelWithStruct->structure6.with_reported_property_ascii_char_ptr6 = "eleven";
        modelWithStruct->structure6.with_reported_property_ascii_char_ptr_no_quotes6 = "\"twelve\"";
        modelWithStruct->structure6.with_reported_property_EdmDateTimeOffset6.dateTime.tm_year = 114;
        modelWithStruct->structure6.with_reported_property_EdmDateTimeOffset6.dateTime.tm_mon = 6 - 1;
        modelWithStruct->structure6.with_reported_property_EdmDateTimeOffset6.dateTime.tm_mday = 17;
        modelWithStruct->structure6.with_reported_property_EdmDateTimeOffset6.dateTime.tm_hour = 8;
        modelWithStruct->structure6.with_reported_property_EdmDateTimeOffset6.dateTime.tm_min = 51;
        modelWithStruct->structure6.with_reported_property_EdmDateTimeOffset6.dateTime.tm_sec = 23;
        modelWithStruct->structure6.with_reported_property_EdmDateTimeOffset6.hasFractionalSecond = 1;
        modelWithStruct->structure6.with_reported_property_EdmDateTimeOffset6.fractionalSecond = 5;
        modelWithStruct->structure6.with_reported_property_EdmDateTimeOffset6.hasTimeZone = 1;
        modelWithStruct->structure6.with_reported_property_EdmDateTimeOffset6.timeZoneHour = -8;
        modelWithStruct->structure6.with_reported_property_EdmDateTimeOffset6.timeZoneMinute = 1;
        modelWithStruct->structure6.with_reported_property_EdmGuid6.GUID[0] = 0x00;
        modelWithStruct->structure6.with_reported_property_EdmGuid6.GUID[1] = 0x11;
        modelWithStruct->structure6.with_reported_property_EdmGuid6.GUID[2] = 0x22;
        modelWithStruct->structure6.with_reported_property_EdmGuid6.GUID[3] = 0x33;
        modelWithStruct->structure6.with_reported_property_EdmGuid6.GUID[4] = 0x44;
        modelWithStruct->structure6.with_reported_property_EdmGuid6.GUID[5] = 0x55;
        modelWithStruct->structure6.with_reported_property_EdmGuid6.GUID[6] = 0x66;
        modelWithStruct->structure6.with_reported_property_EdmGuid6.GUID[7] = 0x77;
        modelWithStruct->structure6.with_reported_property_EdmGuid6.GUID[8] = 0x88;
        modelWithStruct->structure6.with_reported_property_EdmGuid6.GUID[9] = 0x99;
        modelWithStruct->structure6.with_reported_property_EdmGuid6.GUID[10] = 0xAA;
        modelWithStruct->structure6.with_reported_property_EdmGuid6.GUID[11] = 0xBB;
        modelWithStruct->structure6.with_reported_property_EdmGuid6.GUID[12] = 0xCC;
        modelWithStruct->structure6.with_reported_property_EdmGuid6.GUID[13] = 0xDD;
        modelWithStruct->structure6.with_reported_property_EdmGuid6.GUID[14] = 0xEE;
        modelWithStruct->structure6.with_reported_property_EdmGuid6.GUID[15] = 0xFF;

        unsigned char edmBinary[3] = { '3', '4', '5' };
        modelWithStruct->structure6.with_reported_property_EdmBinary6.data = edmBinary;
        modelWithStruct->structure6.with_reported_property_EdmBinary6.size = 3;

        const char* expectedJsonAsString =
            "{                                                                                                 \
            \"structure6\":                                                                                    \
            {                                                                                                  \
                \"with_reported_property_double6\" : 1.0,                                                      \
                \"with_reported_property_int6\" : 2,                                                           \
                \"with_reported_property_float6\" : 3.000000,                                                  \
                \"with_reported_property_long6\" : 4,                                                          \
                \"with_reported_property_sint8_t6\" : 5,                                                       \
                \"with_reported_property_uint8_t6\" : 6,                                                       \
                \"with_reported_property_int16_t6\" : 7,                                                       \
                \"with_reported_property_int32_t6\" : 8,                                                       \
                \"with_reported_property_int64_t6\" : 9,                                                       \
                \"with_reported_property_bool6\" : true,                                                       \
                \"with_reported_property_ascii_char_ptr6\" : \"eleven\",                                       \
                \"with_reported_property_ascii_char_ptr_no_quotes6\" : \"twelve\",                             \
                \"with_reported_property_EdmDateTimeOffset6\" : \"2014-06-17T08:51:23.000000000005-08:01\",    \
                \"with_reported_property_EdmGuid6\" : \"00112233-4455-6677-8899-AABBCCDDEEFF\",                \
                \"with_reported_property_EdmBinary6\": \"MzQ1\"                                                \
            }                                                                                                  \
        }";

        unsigned char* destination;
        size_t destinationSize;

        ///act
        CODEFIRST_RESULT result = SERIALIZE_REPORTED_PROPERTIES(&destination, &destinationSize, *modelWithStruct);

        ///assert
        ASSERT_ARE_EQUAL(CODEFIRST_RESULT, CODEFIRST_OK, result);
        ASSERT_IS_TRUE(areTwoJsonsEqual(destination, destinationSize, expectedJsonAsString));

        ///clean
        free(destination);
        DESTROY_MODEL_INSTANCE(modelWithStruct);
    }

    /*the following test has a model that has a single WITH_REPORTED_PROPERTY of model type having fields of all types*/
    /*conceptually:
    MODEL
    |--- MODEL
         |--- with_reported_property_int
         |--- with_reported_property_float
    ...
    */
    TEST_FUNCTION(WITH_REPORTED_PROPERTY_IN_MODEL_IN_MODEL)
    {
        ///arrange
        basicModel_WithReportedProperty7 *basicModel_WithModel = CREATE_MODEL_INSTANCE(basic7, basicModel_WithReportedProperty7, true);

        /*setting values to the model instance*/
        basicModel_WithModel->model7.with_reported_property_double7 = 1.0;
        basicModel_WithModel->model7.with_reported_property_int7 = 2;
        basicModel_WithModel->model7.with_reported_property_float7 = 3.0;
        basicModel_WithModel->model7.with_reported_property_long7 = 4;
        basicModel_WithModel->model7.with_reported_property_sint8_t7 = 5;
        basicModel_WithModel->model7.with_reported_property_uint8_t7 = 6;
        basicModel_WithModel->model7.with_reported_property_int16_t7 = 7;
        basicModel_WithModel->model7.with_reported_property_int32_t7 = 8;
        basicModel_WithModel->model7.with_reported_property_int64_t7 = 9;
        basicModel_WithModel->model7.with_reported_property_bool7 = true;
        basicModel_WithModel->model7.with_reported_property_ascii_char_ptr7 = "eleven";
        basicModel_WithModel->model7.with_reported_property_ascii_char_ptr_no_quotes7 = "\"twelve\"";
        basicModel_WithModel->model7.with_reported_property_EdmDateTimeOffset7.dateTime.tm_year = 114;
        basicModel_WithModel->model7.with_reported_property_EdmDateTimeOffset7.dateTime.tm_mon = 6 - 1;
        basicModel_WithModel->model7.with_reported_property_EdmDateTimeOffset7.dateTime.tm_mday = 17;
        basicModel_WithModel->model7.with_reported_property_EdmDateTimeOffset7.dateTime.tm_hour = 8;
        basicModel_WithModel->model7.with_reported_property_EdmDateTimeOffset7.dateTime.tm_min = 51;
        basicModel_WithModel->model7.with_reported_property_EdmDateTimeOffset7.dateTime.tm_sec = 23;
        basicModel_WithModel->model7.with_reported_property_EdmDateTimeOffset7.hasFractionalSecond = 1;
        basicModel_WithModel->model7.with_reported_property_EdmDateTimeOffset7.fractionalSecond = 5;
        basicModel_WithModel->model7.with_reported_property_EdmDateTimeOffset7.hasTimeZone = 1;
        basicModel_WithModel->model7.with_reported_property_EdmDateTimeOffset7.timeZoneHour = -8;
        basicModel_WithModel->model7.with_reported_property_EdmDateTimeOffset7.timeZoneMinute = 1;
        basicModel_WithModel->model7.with_reported_property_EdmGuid7.GUID[0] = 0x00;
        basicModel_WithModel->model7.with_reported_property_EdmGuid7.GUID[1] = 0x11;
        basicModel_WithModel->model7.with_reported_property_EdmGuid7.GUID[2] = 0x22;
        basicModel_WithModel->model7.with_reported_property_EdmGuid7.GUID[3] = 0x33;
        basicModel_WithModel->model7.with_reported_property_EdmGuid7.GUID[4] = 0x44;
        basicModel_WithModel->model7.with_reported_property_EdmGuid7.GUID[5] = 0x55;
        basicModel_WithModel->model7.with_reported_property_EdmGuid7.GUID[6] = 0x66;
        basicModel_WithModel->model7.with_reported_property_EdmGuid7.GUID[7] = 0x77;
        basicModel_WithModel->model7.with_reported_property_EdmGuid7.GUID[8] = 0x88;
        basicModel_WithModel->model7.with_reported_property_EdmGuid7.GUID[9] = 0x99;
        basicModel_WithModel->model7.with_reported_property_EdmGuid7.GUID[10] = 0xAA;
        basicModel_WithModel->model7.with_reported_property_EdmGuid7.GUID[11] = 0xBB;
        basicModel_WithModel->model7.with_reported_property_EdmGuid7.GUID[12] = 0xCC;
        basicModel_WithModel->model7.with_reported_property_EdmGuid7.GUID[13] = 0xDD;
        basicModel_WithModel->model7.with_reported_property_EdmGuid7.GUID[14] = 0xEE;
        basicModel_WithModel->model7.with_reported_property_EdmGuid7.GUID[15] = 0xFF;

        unsigned char edmBinary[3] = { '3', '4', '5' };
        basicModel_WithModel->model7.with_reported_property_EdmBinary7.data = edmBinary;
        basicModel_WithModel->model7.with_reported_property_EdmBinary7.size = 3;

        const char* expectedJsonAsString =
        "{                                                                                                  \
            \"model7\":                                                                                     \
            {                                                                                               \
                \"with_reported_property_double7\" : 1.0,                                                   \
                \"with_reported_property_int7\" : 2,                                                        \
                \"with_reported_property_float7\" : 3.000000,                                               \
                \"with_reported_property_long7\" : 4,                                                       \
                \"with_reported_property_sint8_t7\" : 5,                                                    \
                \"with_reported_property_uint8_t7\" : 6,                                                    \
                \"with_reported_property_int16_t7\" : 7,                                                    \
                \"with_reported_property_int32_t7\" : 8,                                                    \
                \"with_reported_property_int64_t7\" : 9,                                                    \
                \"with_reported_property_bool7\" : true,                                                    \
                \"with_reported_property_ascii_char_ptr7\" : \"eleven\",                                    \
                \"with_reported_property_ascii_char_ptr_no_quotes7\" : \"twelve\",                          \
                \"with_reported_property_EdmDateTimeOffset7\" : \"2014-06-17T08:51:23.000000000005-08:01\", \
                \"with_reported_property_EdmGuid7\" : \"00112233-4455-6677-8899-AABBCCDDEEFF\",             \
                \"with_reported_property_EdmBinary7\": \"MzQ1\"                                             \
            }                                                                                               \
        }";

        unsigned char* destination;
        size_t destinationSize;

        ///act
        CODEFIRST_RESULT result = SERIALIZE_REPORTED_PROPERTIES(&destination, &destinationSize, *basicModel_WithModel);
        
        ///assert
        ASSERT_ARE_EQUAL(CODEFIRST_RESULT, CODEFIRST_OK, result);
        ASSERT_IS_TRUE(areTwoJsonsEqual(destination, destinationSize, expectedJsonAsString));
        
        ///clean
        free(destination);
        DESTROY_MODEL_INSTANCE(basicModel_WithModel);
    }

    /*the following test has a model that has a single WITH_REPORTED_PROPERTY of structure type having fields of all types*/
    /*conceptually:
    MODEL
    |--- MODEL
         |--- STRUCT
              |--- with_reported_property_float
              |--- with_reported_property_XXX
    */
    TEST_FUNCTION(WITH_REPORTED_PROPERTY_IN_STRUCT_IN_MODEL_IN_MODEL)
    {
        ///arrange
        outerModel8 *modelWithModel = CREATE_MODEL_INSTANCE(basic8, outerModel8, true);

        /*setting values to the model instance*/
        modelWithModel->inner_model8.structure8.with_reported_property_double8 = 1.0;
        modelWithModel->inner_model8.structure8.with_reported_property_int8 = 2;
        modelWithModel->inner_model8.structure8.with_reported_property_float8 = 3.0;
        modelWithModel->inner_model8.structure8.with_reported_property_long8 = 4;
        modelWithModel->inner_model8.structure8.with_reported_property_sint8_t8 = 5;
        modelWithModel->inner_model8.structure8.with_reported_property_uint8_t8 = 6;
        modelWithModel->inner_model8.structure8.with_reported_property_int16_t8 = 7;
        modelWithModel->inner_model8.structure8.with_reported_property_int32_t8 = 8;
        modelWithModel->inner_model8.structure8.with_reported_property_int64_t8 = 9;
        modelWithModel->inner_model8.structure8.with_reported_property_bool8 = true;
        modelWithModel->inner_model8.structure8.with_reported_property_ascii_char_ptr8 = "eleven";
        modelWithModel->inner_model8.structure8.with_reported_property_ascii_char_ptr_no_quotes8 = "\"twelve\"";
        modelWithModel->inner_model8.structure8.with_reported_property_EdmDateTimeOffset8.dateTime.tm_year = 114;
        modelWithModel->inner_model8.structure8.with_reported_property_EdmDateTimeOffset8.dateTime.tm_mon = 6 - 1;
        modelWithModel->inner_model8.structure8.with_reported_property_EdmDateTimeOffset8.dateTime.tm_mday = 17;
        modelWithModel->inner_model8.structure8.with_reported_property_EdmDateTimeOffset8.dateTime.tm_hour = 8;
        modelWithModel->inner_model8.structure8.with_reported_property_EdmDateTimeOffset8.dateTime.tm_min = 51;
        modelWithModel->inner_model8.structure8.with_reported_property_EdmDateTimeOffset8.dateTime.tm_sec = 23;
        modelWithModel->inner_model8.structure8.with_reported_property_EdmDateTimeOffset8.hasFractionalSecond = 1;
        modelWithModel->inner_model8.structure8.with_reported_property_EdmDateTimeOffset8.fractionalSecond = 5;
        modelWithModel->inner_model8.structure8.with_reported_property_EdmDateTimeOffset8.hasTimeZone = 1;
        modelWithModel->inner_model8.structure8.with_reported_property_EdmDateTimeOffset8.timeZoneHour = -8;
        modelWithModel->inner_model8.structure8.with_reported_property_EdmDateTimeOffset8.timeZoneMinute = 1;
        modelWithModel->inner_model8.structure8.with_reported_property_EdmGuid8.GUID[0] = 0x00;
        modelWithModel->inner_model8.structure8.with_reported_property_EdmGuid8.GUID[1] = 0x11;
        modelWithModel->inner_model8.structure8.with_reported_property_EdmGuid8.GUID[2] = 0x22;
        modelWithModel->inner_model8.structure8.with_reported_property_EdmGuid8.GUID[3] = 0x33;
        modelWithModel->inner_model8.structure8.with_reported_property_EdmGuid8.GUID[4] = 0x44;
        modelWithModel->inner_model8.structure8.with_reported_property_EdmGuid8.GUID[5] = 0x55;
        modelWithModel->inner_model8.structure8.with_reported_property_EdmGuid8.GUID[6] = 0x66;
        modelWithModel->inner_model8.structure8.with_reported_property_EdmGuid8.GUID[7] = 0x77;
        modelWithModel->inner_model8.structure8.with_reported_property_EdmGuid8.GUID[8] = 0x88;
        modelWithModel->inner_model8.structure8.with_reported_property_EdmGuid8.GUID[9] = 0x99;
        modelWithModel->inner_model8.structure8.with_reported_property_EdmGuid8.GUID[10] = 0xAA;
        modelWithModel->inner_model8.structure8.with_reported_property_EdmGuid8.GUID[11] = 0xBB;
        modelWithModel->inner_model8.structure8.with_reported_property_EdmGuid8.GUID[12] = 0xCC;
        modelWithModel->inner_model8.structure8.with_reported_property_EdmGuid8.GUID[13] = 0xDD;
        modelWithModel->inner_model8.structure8.with_reported_property_EdmGuid8.GUID[14] = 0xEE;
        modelWithModel->inner_model8.structure8.with_reported_property_EdmGuid8.GUID[15] = 0xFF;

        unsigned char edmBinary[3] = { '3', '4', '5' };
        modelWithModel->inner_model8.structure8.with_reported_property_EdmBinary8.data = edmBinary;
        modelWithModel->inner_model8.structure8.with_reported_property_EdmBinary8.size = 3;

        const char* expectedJsonAsString =
        "{                                                                                                         \
            \"inner_model8\":                                                                                      \
            {                                                                                                      \
                \"structure8\":                                                                                    \
                {                                                                                                  \
                    \"with_reported_property_double8\" : 1.0,                                                      \
                    \"with_reported_property_int8\" : 2,                                                           \
                    \"with_reported_property_float8\" : 3.000000,                                                  \
                    \"with_reported_property_long8\" : 4,                                                          \
                    \"with_reported_property_sint8_t8\" : 5,                                                       \
                    \"with_reported_property_uint8_t8\" : 6,                                                       \
                    \"with_reported_property_int16_t8\" : 7,                                                       \
                    \"with_reported_property_int32_t8\" : 8,                                                       \
                    \"with_reported_property_int64_t8\" : 9,                                                       \
                    \"with_reported_property_bool8\" : true,                                                       \
                    \"with_reported_property_ascii_char_ptr8\" : \"eleven\",                                       \
                    \"with_reported_property_ascii_char_ptr_no_quotes8\" : \"twelve\",                             \
                    \"with_reported_property_EdmDateTimeOffset8\" : \"2014-06-17T08:51:23.000000000005-08:01\",    \
                    \"with_reported_property_EdmGuid8\" : \"00112233-4455-6677-8899-AABBCCDDEEFF\",                \
                    \"with_reported_property_EdmBinary8\": \"MzQ1\"                                                \
                }                                                                                                  \
            }                                                                                                      \
        }";

        unsigned char* destination;
        size_t destinationSize;

        ///act
        CODEFIRST_RESULT result = SERIALIZE_REPORTED_PROPERTIES(&destination, &destinationSize, *modelWithModel);

        ///assert
        ASSERT_ARE_EQUAL(CODEFIRST_RESULT, CODEFIRST_OK, result);
        ASSERT_IS_TRUE(areTwoJsonsEqual(destination, destinationSize, expectedJsonAsString));

        ///clean
        free(destination);
        DESTROY_MODEL_INSTANCE(modelWithModel);
    }

    TEST_FUNCTION(WITH_DESIRED_PROPERTY_IN_ROOT_MODEL)
    {
        ///arrange
        basicModel_WithData9 *modelWithData = CREATE_MODEL_INSTANCE(basic9, basicModel_WithData9, true);

        const char* inputJsonAsString =
        "{                                                                                                    \
            \"with_desired_property_double9\" : 1.0,                                                          \
            \"with_desired_property_int9\" : 2,                                                               \
            \"with_desired_property_float9\" : 3.000000,                                                      \
            \"with_desired_property_long9\" : 4,                                                              \
            \"with_desired_property_sint8_t9\" : 5,                                                           \
            \"with_desired_property_uint8_t9\" : 6,                                                           \
            \"with_desired_property_int16_t9\" : 7,                                                           \
            \"with_desired_property_int32_t9\" : 8,                                                           \
            \"with_desired_property_int64_t9\" : 9,                                                           \
            \"with_desired_property_bool9\" : true,                                                           \
            \"with_desired_property_ascii_char_ptr9\" : \"eleven\",                                           \
            \"with_desired_property_ascii_char_ptr_no_quotes9\" : \"twelve\",                                 \
            \"with_desired_property_EdmDateTimeOffset9\" : \"2014-06-17T08:51:23.000000000005-08:01\",        \
            \"with_desired_property_EdmGuid9\" : \"00112233-4455-6677-8899-AABBCCDDEEFF\",                    \
            \"with_desired_property_EdmBinary9\": \"MzQ1\"                                                    \
        }";

        ///act
        CODEFIRST_RESULT result = INGEST_DESIRED_PROPERTIES(modelWithData, inputJsonAsString);

        ///assert
        ASSERT_ARE_EQUAL(CODEFIRST_RESULT, CODEFIRST_OK, result);

        /*setting values to the model instance*/
        ASSERT_ARE_EQUAL(double,    1.0,            modelWithData->with_desired_property_double9);
        ASSERT_ARE_EQUAL(int,       2,              modelWithData->with_desired_property_int9);
        ASSERT_ARE_EQUAL(float,     3.0,            modelWithData->with_desired_property_float9);
        ASSERT_ARE_EQUAL(long,      4,              modelWithData->with_desired_property_long9);
        ASSERT_ARE_EQUAL(int8_t,    5,              modelWithData->with_desired_property_sint8_t9);
        ASSERT_ARE_EQUAL(uint8_t,   6,              modelWithData->with_desired_property_uint8_t9);
        ASSERT_ARE_EQUAL(int16_t,   7,              modelWithData->with_desired_property_int16_t9);
        ASSERT_ARE_EQUAL(int32_t,   8,              modelWithData->with_desired_property_int32_t9);
        ASSERT_ARE_EQUAL(int64_t,   9,              modelWithData->with_desired_property_int64_t9);
        ASSERT_IS_TRUE(modelWithData->with_desired_property_bool9);
        ASSERT_ARE_EQUAL(char_ptr,  "eleven",       modelWithData->with_desired_property_ascii_char_ptr9);
        ASSERT_ARE_EQUAL(char_ptr,  "\"twelve\"",   modelWithData->with_desired_property_ascii_char_ptr_no_quotes9);
        ASSERT_ARE_EQUAL(int,       114,            modelWithData->with_desired_property_EdmDateTimeOffset9.dateTime.tm_year);
        ASSERT_ARE_EQUAL(int,       6-1,            modelWithData->with_desired_property_EdmDateTimeOffset9.dateTime.tm_mon);
        ASSERT_ARE_EQUAL(int,       17,             modelWithData->with_desired_property_EdmDateTimeOffset9.dateTime.tm_mday);
        ASSERT_ARE_EQUAL(int,       8,              modelWithData->with_desired_property_EdmDateTimeOffset9.dateTime.tm_hour);
        ASSERT_ARE_EQUAL(int,       51,             modelWithData->with_desired_property_EdmDateTimeOffset9.dateTime.tm_min);
        ASSERT_ARE_EQUAL(int,       23,             modelWithData->with_desired_property_EdmDateTimeOffset9.dateTime.tm_sec);
        ASSERT_ARE_EQUAL(uint8_t,   1,              modelWithData->with_desired_property_EdmDateTimeOffset9.hasFractionalSecond);
        ASSERT_ARE_EQUAL(uint64_t,  5,              modelWithData->with_desired_property_EdmDateTimeOffset9.fractionalSecond);
        ASSERT_ARE_EQUAL(uint8_t,   1,              modelWithData->with_desired_property_EdmDateTimeOffset9.hasTimeZone);
        ASSERT_ARE_EQUAL(int8_t,    -8,             modelWithData->with_desired_property_EdmDateTimeOffset9.timeZoneHour);
        ASSERT_ARE_EQUAL(uint8_t,   1,              modelWithData->with_desired_property_EdmDateTimeOffset9.timeZoneMinute);
        ASSERT_ARE_EQUAL(uint8_t,   0x00,           modelWithData->with_desired_property_EdmGuid9.GUID[0]);
        ASSERT_ARE_EQUAL(uint8_t,   0x11,           modelWithData->with_desired_property_EdmGuid9.GUID[1]);
        ASSERT_ARE_EQUAL(uint8_t,   0x22,           modelWithData->with_desired_property_EdmGuid9.GUID[2]);
        ASSERT_ARE_EQUAL(uint8_t,   0x33,           modelWithData->with_desired_property_EdmGuid9.GUID[3]);
        ASSERT_ARE_EQUAL(uint8_t,   0x44,           modelWithData->with_desired_property_EdmGuid9.GUID[4]);
        ASSERT_ARE_EQUAL(uint8_t,   0x55,           modelWithData->with_desired_property_EdmGuid9.GUID[5]);
        ASSERT_ARE_EQUAL(uint8_t,   0x66,           modelWithData->with_desired_property_EdmGuid9.GUID[6]);
        ASSERT_ARE_EQUAL(uint8_t,   0x77,           modelWithData->with_desired_property_EdmGuid9.GUID[7]);
        ASSERT_ARE_EQUAL(uint8_t,   0x88,           modelWithData->with_desired_property_EdmGuid9.GUID[8]);
        ASSERT_ARE_EQUAL(uint8_t,   0x99,           modelWithData->with_desired_property_EdmGuid9.GUID[9]);
        ASSERT_ARE_EQUAL(uint8_t,   0xAA,           modelWithData->with_desired_property_EdmGuid9.GUID[10]);
        ASSERT_ARE_EQUAL(uint8_t,   0xBB,           modelWithData->with_desired_property_EdmGuid9.GUID[11]);
        ASSERT_ARE_EQUAL(uint8_t,   0xCC,           modelWithData->with_desired_property_EdmGuid9.GUID[12]);
        ASSERT_ARE_EQUAL(uint8_t,   0xDD,           modelWithData->with_desired_property_EdmGuid9.GUID[13]);
        ASSERT_ARE_EQUAL(uint8_t,   0xEE,           modelWithData->with_desired_property_EdmGuid9.GUID[14]);
        ASSERT_ARE_EQUAL(uint8_t,   0xFF,           modelWithData->with_desired_property_EdmGuid9.GUID[15]);

        ASSERT_ARE_EQUAL(size_t,     3,             modelWithData->with_desired_property_EdmBinary9.size);
        ASSERT_ARE_EQUAL(uint8_t,   '3',            modelWithData->with_desired_property_EdmBinary9.data[0]);
        ASSERT_ARE_EQUAL(uint8_t,   '4',            modelWithData->with_desired_property_EdmBinary9.data[1]);
        ASSERT_ARE_EQUAL(uint8_t,   '5',            modelWithData->with_desired_property_EdmBinary9.data[2]);

        ///clean
        DESTROY_MODEL_INSTANCE(modelWithData);
    }

    TEST_FUNCTION(WITH_DESIRED_PROPERTY_IN_STRUCT_IN_ROOT_MODEL)
    {
        ///arrange
        basicModel_WithStruct10 *modelWithData = CREATE_MODEL_INSTANCE(basic10, basicModel_WithStruct10, true);

        const char* inputJsonAsString =
        "{                                                                                                         \
            \"structure10\":                                                                                       \
            {                                                                                                      \
                \"with_desired_property_double10\" : 1.0,                                                          \
                \"with_desired_property_int10\" : 2,                                                               \
                \"with_desired_property_float10\" : 3.000000,                                                      \
                \"with_desired_property_long10\" : 4,                                                              \
                \"with_desired_property_sint8_t10\" : 5,                                                           \
                \"with_desired_property_uint8_t10\" : 6,                                                           \
                \"with_desired_property_int16_t10\" : 7,                                                           \
                \"with_desired_property_int32_t10\" : 8,                                                           \
                \"with_desired_property_int64_t10\" : 9,                                                           \
                \"with_desired_property_bool10\" : true,                                                           \
                \"with_desired_property_ascii_char_ptr10\" : \"eleven\",                                           \
                \"with_desired_property_ascii_char_ptr_no_quotes10\" : \"twelve\",                                 \
                \"with_desired_property_EdmDateTimeOffset10\" : \"2014-06-17T08:51:23.000000000005-08:01\",        \
                \"with_desired_property_EdmGuid10\" : \"00112233-4455-6677-8899-AABBCCDDEEFF\",                    \
                \"with_desired_property_EdmBinary10\": \"MzQ1\"                                                    \
            }                                                                                                      \
        }";

        ///act
        CODEFIRST_RESULT result = INGEST_DESIRED_PROPERTIES(modelWithData, inputJsonAsString);

        ///assert
        ASSERT_ARE_EQUAL(CODEFIRST_RESULT, CODEFIRST_OK, result);

        ASSERT_ARE_EQUAL(double,    1.0,            modelWithData->structure10.with_desired_property_double10);
        ASSERT_ARE_EQUAL(int,       2,              modelWithData->structure10.with_desired_property_int10);
        ASSERT_ARE_EQUAL(float,     3.0,            modelWithData->structure10.with_desired_property_float10);
        ASSERT_ARE_EQUAL(long,      4,              modelWithData->structure10.with_desired_property_long10);
        ASSERT_ARE_EQUAL(int8_t,    5,              modelWithData->structure10.with_desired_property_sint8_t10);
        ASSERT_ARE_EQUAL(uint8_t,   6,              modelWithData->structure10.with_desired_property_uint8_t10);
        ASSERT_ARE_EQUAL(int16_t,   7,              modelWithData->structure10.with_desired_property_int16_t10);
        ASSERT_ARE_EQUAL(int32_t,   8,              modelWithData->structure10.with_desired_property_int32_t10);
        ASSERT_ARE_EQUAL(int64_t,   9,              modelWithData->structure10.with_desired_property_int64_t10);
        ASSERT_IS_TRUE(modelWithData->structure10.with_desired_property_bool10);
        ASSERT_ARE_EQUAL(char_ptr,  "eleven",       modelWithData->structure10.with_desired_property_ascii_char_ptr10);
        ASSERT_ARE_EQUAL(char_ptr,  "\"twelve\"",   modelWithData->structure10.with_desired_property_ascii_char_ptr_no_quotes10);
        ASSERT_ARE_EQUAL(int,       114,            modelWithData->structure10.with_desired_property_EdmDateTimeOffset10.dateTime.tm_year);
        ASSERT_ARE_EQUAL(int,       6-1,            modelWithData->structure10.with_desired_property_EdmDateTimeOffset10.dateTime.tm_mon);
        ASSERT_ARE_EQUAL(int,       17,             modelWithData->structure10.with_desired_property_EdmDateTimeOffset10.dateTime.tm_mday);
        ASSERT_ARE_EQUAL(int,       8,              modelWithData->structure10.with_desired_property_EdmDateTimeOffset10.dateTime.tm_hour);
        ASSERT_ARE_EQUAL(int,       51,             modelWithData->structure10.with_desired_property_EdmDateTimeOffset10.dateTime.tm_min);
        ASSERT_ARE_EQUAL(int,       23,             modelWithData->structure10.with_desired_property_EdmDateTimeOffset10.dateTime.tm_sec);
        ASSERT_ARE_EQUAL(uint8_t,   1,              modelWithData->structure10.with_desired_property_EdmDateTimeOffset10.hasFractionalSecond);
        ASSERT_ARE_EQUAL(uint64_t,  5,              modelWithData->structure10.with_desired_property_EdmDateTimeOffset10.fractionalSecond);
        ASSERT_ARE_EQUAL(uint8_t,   1,              modelWithData->structure10.with_desired_property_EdmDateTimeOffset10.hasTimeZone);
        ASSERT_ARE_EQUAL(int8_t,    -8,             modelWithData->structure10.with_desired_property_EdmDateTimeOffset10.timeZoneHour);
        ASSERT_ARE_EQUAL(uint8_t,   1,              modelWithData->structure10.with_desired_property_EdmDateTimeOffset10.timeZoneMinute);
        ASSERT_ARE_EQUAL(uint8_t,   0x00,           modelWithData->structure10.with_desired_property_EdmGuid10.GUID[0]);
        ASSERT_ARE_EQUAL(uint8_t,   0x11,           modelWithData->structure10.with_desired_property_EdmGuid10.GUID[1]);
        ASSERT_ARE_EQUAL(uint8_t,   0x22,           modelWithData->structure10.with_desired_property_EdmGuid10.GUID[2]);
        ASSERT_ARE_EQUAL(uint8_t,   0x33,           modelWithData->structure10.with_desired_property_EdmGuid10.GUID[3]);
        ASSERT_ARE_EQUAL(uint8_t,   0x44,           modelWithData->structure10.with_desired_property_EdmGuid10.GUID[4]);
        ASSERT_ARE_EQUAL(uint8_t,   0x55,           modelWithData->structure10.with_desired_property_EdmGuid10.GUID[5]);
        ASSERT_ARE_EQUAL(uint8_t,   0x66,           modelWithData->structure10.with_desired_property_EdmGuid10.GUID[6]);
        ASSERT_ARE_EQUAL(uint8_t,   0x77,           modelWithData->structure10.with_desired_property_EdmGuid10.GUID[7]);
        ASSERT_ARE_EQUAL(uint8_t,   0x88,           modelWithData->structure10.with_desired_property_EdmGuid10.GUID[8]);
        ASSERT_ARE_EQUAL(uint8_t,   0x99,           modelWithData->structure10.with_desired_property_EdmGuid10.GUID[9]);
        ASSERT_ARE_EQUAL(uint8_t,   0xAA,           modelWithData->structure10.with_desired_property_EdmGuid10.GUID[10]);
        ASSERT_ARE_EQUAL(uint8_t,   0xBB,           modelWithData->structure10.with_desired_property_EdmGuid10.GUID[11]);
        ASSERT_ARE_EQUAL(uint8_t,   0xCC,           modelWithData->structure10.with_desired_property_EdmGuid10.GUID[12]);
        ASSERT_ARE_EQUAL(uint8_t,   0xDD,           modelWithData->structure10.with_desired_property_EdmGuid10.GUID[13]);
        ASSERT_ARE_EQUAL(uint8_t,   0xEE,           modelWithData->structure10.with_desired_property_EdmGuid10.GUID[14]);
        ASSERT_ARE_EQUAL(uint8_t,   0xFF,           modelWithData->structure10.with_desired_property_EdmGuid10.GUID[15]);

        ASSERT_ARE_EQUAL(size_t,     3,             modelWithData->structure10.with_desired_property_EdmBinary10.size);
        ASSERT_ARE_EQUAL(uint8_t,   '3',            modelWithData->structure10.with_desired_property_EdmBinary10.data[0]);
        ASSERT_ARE_EQUAL(uint8_t,   '4',            modelWithData->structure10.with_desired_property_EdmBinary10.data[1]);
        ASSERT_ARE_EQUAL(uint8_t,   '5',            modelWithData->structure10.with_desired_property_EdmBinary10.data[2]);

        ///clean
        DESTROY_MODEL_INSTANCE(modelWithData);
    }

    TEST_FUNCTION(WITH_DESIRED_PROPERTY_IN_MODEL_IN_MODEL)
    {
        ///arrange
        basicModel_WithModel11 *modelWithData = CREATE_MODEL_INSTANCE(basic11, basicModel_WithModel11, true);

        const char* inputJsonAsString =
        "{                                                                                                         \
            \"model11\":                                                                                           \
            {                                                                                                      \
                \"with_desired_property_double11\" : 1.0,                                                          \
                \"with_desired_property_int11\" : 2,                                                               \
                \"with_desired_property_float11\" : 3.000000,                                                      \
                \"with_desired_property_long11\" : 4,                                                              \
                \"with_desired_property_sint8_t11\" : 5,                                                           \
                \"with_desired_property_uint8_t11\" : 6,                                                           \
                \"with_desired_property_int16_t11\" : 7,                                                           \
                \"with_desired_property_int32_t11\" : 8,                                                           \
                \"with_desired_property_int64_t11\" : 9,                                                           \
                \"with_desired_property_bool11\" : true,                                                           \
                \"with_desired_property_ascii_char_ptr11\" : \"eleven\",                                           \
                \"with_desired_property_ascii_char_ptr_no_quotes11\" : \"twelve\",                                 \
                \"with_desired_property_EdmDateTimeOffset11\" : \"2014-06-17T08:51:23.000000000005-08:01\",        \
                \"with_desired_property_EdmGuid11\" : \"00112233-4455-6677-8899-AABBCCDDEEFF\",                    \
                \"with_desired_property_EdmBinary11\": \"MzQ1\"                                                    \
            }                                                                                                      \
        }";

        ///act
        CODEFIRST_RESULT result = INGEST_DESIRED_PROPERTIES(modelWithData, inputJsonAsString);

        ///assert
        ASSERT_ARE_EQUAL(CODEFIRST_RESULT, CODEFIRST_OK, result);

        ASSERT_ARE_EQUAL(double,    1.0,            modelWithData->model11.with_desired_property_double11);
        ASSERT_ARE_EQUAL(int,       2,              modelWithData->model11.with_desired_property_int11);
        ASSERT_ARE_EQUAL(float,     3.0,            modelWithData->model11.with_desired_property_float11);
        ASSERT_ARE_EQUAL(long,      4,              modelWithData->model11.with_desired_property_long11);
        ASSERT_ARE_EQUAL(int8_t,    5,              modelWithData->model11.with_desired_property_sint8_t11);
        ASSERT_ARE_EQUAL(uint8_t,   6,              modelWithData->model11.with_desired_property_uint8_t11);
        ASSERT_ARE_EQUAL(int16_t,   7,              modelWithData->model11.with_desired_property_int16_t11);
        ASSERT_ARE_EQUAL(int32_t,   8,              modelWithData->model11.with_desired_property_int32_t11);
        ASSERT_ARE_EQUAL(int64_t,   9,              modelWithData->model11.with_desired_property_int64_t11);
        ASSERT_IS_TRUE(modelWithData->model11.with_desired_property_bool11);
        ASSERT_ARE_EQUAL(char_ptr,  "eleven",       modelWithData->model11.with_desired_property_ascii_char_ptr11);
        ASSERT_ARE_EQUAL(char_ptr,  "\"twelve\"",   modelWithData->model11.with_desired_property_ascii_char_ptr_no_quotes11);
        ASSERT_ARE_EQUAL(int,       114,            modelWithData->model11.with_desired_property_EdmDateTimeOffset11.dateTime.tm_year);
        ASSERT_ARE_EQUAL(int,       6-1,            modelWithData->model11.with_desired_property_EdmDateTimeOffset11.dateTime.tm_mon);
        ASSERT_ARE_EQUAL(int,       17,             modelWithData->model11.with_desired_property_EdmDateTimeOffset11.dateTime.tm_mday);
        ASSERT_ARE_EQUAL(int,       8,              modelWithData->model11.with_desired_property_EdmDateTimeOffset11.dateTime.tm_hour);
        ASSERT_ARE_EQUAL(int,       51,             modelWithData->model11.with_desired_property_EdmDateTimeOffset11.dateTime.tm_min);
        ASSERT_ARE_EQUAL(int,       23,             modelWithData->model11.with_desired_property_EdmDateTimeOffset11.dateTime.tm_sec);
        ASSERT_ARE_EQUAL(uint8_t,   1,              modelWithData->model11.with_desired_property_EdmDateTimeOffset11.hasFractionalSecond);
        ASSERT_ARE_EQUAL(uint64_t,  5,              modelWithData->model11.with_desired_property_EdmDateTimeOffset11.fractionalSecond);
        ASSERT_ARE_EQUAL(uint8_t,   1,              modelWithData->model11.with_desired_property_EdmDateTimeOffset11.hasTimeZone);
        ASSERT_ARE_EQUAL(int8_t,    -8,             modelWithData->model11.with_desired_property_EdmDateTimeOffset11.timeZoneHour);
        ASSERT_ARE_EQUAL(uint8_t,   1,              modelWithData->model11.with_desired_property_EdmDateTimeOffset11.timeZoneMinute);
        ASSERT_ARE_EQUAL(uint8_t,   0x00,           modelWithData->model11.with_desired_property_EdmGuid11.GUID[0]);
        ASSERT_ARE_EQUAL(uint8_t,   0x11,           modelWithData->model11.with_desired_property_EdmGuid11.GUID[1]);
        ASSERT_ARE_EQUAL(uint8_t,   0x22,           modelWithData->model11.with_desired_property_EdmGuid11.GUID[2]);
        ASSERT_ARE_EQUAL(uint8_t,   0x33,           modelWithData->model11.with_desired_property_EdmGuid11.GUID[3]);
        ASSERT_ARE_EQUAL(uint8_t,   0x44,           modelWithData->model11.with_desired_property_EdmGuid11.GUID[4]);
        ASSERT_ARE_EQUAL(uint8_t,   0x55,           modelWithData->model11.with_desired_property_EdmGuid11.GUID[5]);
        ASSERT_ARE_EQUAL(uint8_t,   0x66,           modelWithData->model11.with_desired_property_EdmGuid11.GUID[6]);
        ASSERT_ARE_EQUAL(uint8_t,   0x77,           modelWithData->model11.with_desired_property_EdmGuid11.GUID[7]);
        ASSERT_ARE_EQUAL(uint8_t,   0x88,           modelWithData->model11.with_desired_property_EdmGuid11.GUID[8]);
        ASSERT_ARE_EQUAL(uint8_t,   0x99,           modelWithData->model11.with_desired_property_EdmGuid11.GUID[9]);
        ASSERT_ARE_EQUAL(uint8_t,   0xAA,           modelWithData->model11.with_desired_property_EdmGuid11.GUID[10]);
        ASSERT_ARE_EQUAL(uint8_t,   0xBB,           modelWithData->model11.with_desired_property_EdmGuid11.GUID[11]);
        ASSERT_ARE_EQUAL(uint8_t,   0xCC,           modelWithData->model11.with_desired_property_EdmGuid11.GUID[12]);
        ASSERT_ARE_EQUAL(uint8_t,   0xDD,           modelWithData->model11.with_desired_property_EdmGuid11.GUID[13]);
        ASSERT_ARE_EQUAL(uint8_t,   0xEE,           modelWithData->model11.with_desired_property_EdmGuid11.GUID[14]);
        ASSERT_ARE_EQUAL(uint8_t,   0xFF,           modelWithData->model11.with_desired_property_EdmGuid11.GUID[15]);

        ASSERT_ARE_EQUAL(size_t,     3,             modelWithData->model11.with_desired_property_EdmBinary11.size);
        ASSERT_ARE_EQUAL(uint8_t,   '3',            modelWithData->model11.with_desired_property_EdmBinary11.data[0]);
        ASSERT_ARE_EQUAL(uint8_t,   '4',            modelWithData->model11.with_desired_property_EdmBinary11.data[1]);
        ASSERT_ARE_EQUAL(uint8_t,   '5',            modelWithData->model11.with_desired_property_EdmBinary11.data[2]);

        ///clean
        DESTROY_MODEL_INSTANCE(modelWithData);
    }

    TEST_FUNCTION(WITH_DESIRED_PROPERTY_IN_STRUCT_IN_MODEL_IN_MODEL)
    {
        ///arrange
        outerModel12 *modelWithData = CREATE_MODEL_INSTANCE(basic12, outerModel12, true);

        const char* inputJsonAsString =
        "{                                                                                                             \
            \"inner_model12\":                                                                                         \
            {                                                                                                          \
                \"structure12\":                                                                                       \
                {                                                                                                      \
                    \"with_desired_property_double12\" : 1.0,                                                          \
                    \"with_desired_property_int12\" : 2,                                                               \
                    \"with_desired_property_float12\" : 3.000000,                                                      \
                    \"with_desired_property_long12\" : 4,                                                              \
                    \"with_desired_property_sint8_t12\" : 5,                                                           \
                    \"with_desired_property_uint8_t12\" : 6,                                                           \
                    \"with_desired_property_int16_t12\" : 7,                                                           \
                    \"with_desired_property_int32_t12\" : 8,                                                           \
                    \"with_desired_property_int64_t12\" : 9,                                                           \
                    \"with_desired_property_bool12\" : true,                                                           \
                    \"with_desired_property_ascii_char_ptr12\" : \"eleven\",                                           \
                    \"with_desired_property_ascii_char_ptr_no_quotes12\" : \"twelve\",                                 \
                    \"with_desired_property_EdmDateTimeOffset12\" : \"2014-06-17T08:51:23.000000000005-08:01\",        \
                    \"with_desired_property_EdmGuid12\" : \"00112233-4455-6677-8899-AABBCCDDEEFF\",                    \
                    \"with_desired_property_EdmBinary12\": \"MzQ1\"                                                    \
                }                                                                                                      \
            }                                                                                                          \
        }";

        ///act
        CODEFIRST_RESULT result = INGEST_DESIRED_PROPERTIES(modelWithData, inputJsonAsString);

        ///assert
        ASSERT_ARE_EQUAL(CODEFIRST_RESULT, CODEFIRST_OK, result);

        ASSERT_ARE_EQUAL(double,    1.0,            modelWithData->inner_model12.structure12.with_desired_property_double12);
        ASSERT_ARE_EQUAL(int,       2,              modelWithData->inner_model12.structure12.with_desired_property_int12);
        ASSERT_ARE_EQUAL(float,     3.0,            modelWithData->inner_model12.structure12.with_desired_property_float12);
        ASSERT_ARE_EQUAL(long,      4,              modelWithData->inner_model12.structure12.with_desired_property_long12);
        ASSERT_ARE_EQUAL(int8_t,    5,              modelWithData->inner_model12.structure12.with_desired_property_sint8_t12);
        ASSERT_ARE_EQUAL(uint8_t,   6,              modelWithData->inner_model12.structure12.with_desired_property_uint8_t12);
        ASSERT_ARE_EQUAL(int16_t,   7,              modelWithData->inner_model12.structure12.with_desired_property_int16_t12);
        ASSERT_ARE_EQUAL(int32_t,   8,              modelWithData->inner_model12.structure12.with_desired_property_int32_t12);
        ASSERT_ARE_EQUAL(int64_t,   9,              modelWithData->inner_model12.structure12.with_desired_property_int64_t12);
        ASSERT_IS_TRUE(modelWithData->inner_model12.structure12.with_desired_property_bool12);
        ASSERT_ARE_EQUAL(char_ptr,  "eleven",       modelWithData->inner_model12.structure12.with_desired_property_ascii_char_ptr12);
        ASSERT_ARE_EQUAL(char_ptr,  "\"twelve\"",   modelWithData->inner_model12.structure12.with_desired_property_ascii_char_ptr_no_quotes12);
        ASSERT_ARE_EQUAL(int,       114,            modelWithData->inner_model12.structure12.with_desired_property_EdmDateTimeOffset12.dateTime.tm_year);
        ASSERT_ARE_EQUAL(int,       6-1,            modelWithData->inner_model12.structure12.with_desired_property_EdmDateTimeOffset12.dateTime.tm_mon);
        ASSERT_ARE_EQUAL(int,       17,             modelWithData->inner_model12.structure12.with_desired_property_EdmDateTimeOffset12.dateTime.tm_mday);
        ASSERT_ARE_EQUAL(int,       8,              modelWithData->inner_model12.structure12.with_desired_property_EdmDateTimeOffset12.dateTime.tm_hour);
        ASSERT_ARE_EQUAL(int,       51,             modelWithData->inner_model12.structure12.with_desired_property_EdmDateTimeOffset12.dateTime.tm_min);
        ASSERT_ARE_EQUAL(int,       23,             modelWithData->inner_model12.structure12.with_desired_property_EdmDateTimeOffset12.dateTime.tm_sec);
        ASSERT_ARE_EQUAL(uint8_t,   1,              modelWithData->inner_model12.structure12.with_desired_property_EdmDateTimeOffset12.hasFractionalSecond);
        ASSERT_ARE_EQUAL(uint64_t,  5,              modelWithData->inner_model12.structure12.with_desired_property_EdmDateTimeOffset12.fractionalSecond);
        ASSERT_ARE_EQUAL(uint8_t,   1,              modelWithData->inner_model12.structure12.with_desired_property_EdmDateTimeOffset12.hasTimeZone);
        ASSERT_ARE_EQUAL(int8_t,    -8,             modelWithData->inner_model12.structure12.with_desired_property_EdmDateTimeOffset12.timeZoneHour);
        ASSERT_ARE_EQUAL(uint8_t,   1,              modelWithData->inner_model12.structure12.with_desired_property_EdmDateTimeOffset12.timeZoneMinute);
        ASSERT_ARE_EQUAL(uint8_t,   0x00,           modelWithData->inner_model12.structure12.with_desired_property_EdmGuid12.GUID[0]);
        ASSERT_ARE_EQUAL(uint8_t,   0x11,           modelWithData->inner_model12.structure12.with_desired_property_EdmGuid12.GUID[1]);
        ASSERT_ARE_EQUAL(uint8_t,   0x22,           modelWithData->inner_model12.structure12.with_desired_property_EdmGuid12.GUID[2]);
        ASSERT_ARE_EQUAL(uint8_t,   0x33,           modelWithData->inner_model12.structure12.with_desired_property_EdmGuid12.GUID[3]);
        ASSERT_ARE_EQUAL(uint8_t,   0x44,           modelWithData->inner_model12.structure12.with_desired_property_EdmGuid12.GUID[4]);
        ASSERT_ARE_EQUAL(uint8_t,   0x55,           modelWithData->inner_model12.structure12.with_desired_property_EdmGuid12.GUID[5]);
        ASSERT_ARE_EQUAL(uint8_t,   0x66,           modelWithData->inner_model12.structure12.with_desired_property_EdmGuid12.GUID[6]);
        ASSERT_ARE_EQUAL(uint8_t,   0x77,           modelWithData->inner_model12.structure12.with_desired_property_EdmGuid12.GUID[7]);
        ASSERT_ARE_EQUAL(uint8_t,   0x88,           modelWithData->inner_model12.structure12.with_desired_property_EdmGuid12.GUID[8]);
        ASSERT_ARE_EQUAL(uint8_t,   0x99,           modelWithData->inner_model12.structure12.with_desired_property_EdmGuid12.GUID[9]);
        ASSERT_ARE_EQUAL(uint8_t,   0xAA,           modelWithData->inner_model12.structure12.with_desired_property_EdmGuid12.GUID[10]);
        ASSERT_ARE_EQUAL(uint8_t,   0xBB,           modelWithData->inner_model12.structure12.with_desired_property_EdmGuid12.GUID[11]);
        ASSERT_ARE_EQUAL(uint8_t,   0xCC,           modelWithData->inner_model12.structure12.with_desired_property_EdmGuid12.GUID[12]);
        ASSERT_ARE_EQUAL(uint8_t,   0xDD,           modelWithData->inner_model12.structure12.with_desired_property_EdmGuid12.GUID[13]);
        ASSERT_ARE_EQUAL(uint8_t,   0xEE,           modelWithData->inner_model12.structure12.with_desired_property_EdmGuid12.GUID[14]);
        ASSERT_ARE_EQUAL(uint8_t,   0xFF,           modelWithData->inner_model12.structure12.with_desired_property_EdmGuid12.GUID[15]);

        ASSERT_ARE_EQUAL(size_t,     3,             modelWithData->inner_model12.structure12.with_desired_property_EdmBinary12.size);
        ASSERT_ARE_EQUAL(uint8_t,   '3',            modelWithData->inner_model12.structure12.with_desired_property_EdmBinary12.data[0]);
        ASSERT_ARE_EQUAL(uint8_t,   '4',            modelWithData->inner_model12.structure12.with_desired_property_EdmBinary12.data[1]);
        ASSERT_ARE_EQUAL(uint8_t,   '5',            modelWithData->inner_model12.structure12.with_desired_property_EdmBinary12.data[2]);

        ///clean
        DESTROY_MODEL_INSTANCE(modelWithData);
    }

    TEST_FUNCTION(WITH_ACTION_IN_ROOT_MODEL)
    {
        ///arrange
        model_WithAction13 *modelWithData = CREATE_MODEL_INSTANCE(basic13, model_WithAction13, true);

        const char* inputJsonAsString =
        "{                                                                                       \
            \"Name\": \"action13\",                                                              \
            \"Parameters\":                                                                      \
            {                                                                                    \
                \"double13\" : 1.0,                                                              \
                \"int13\" : 2,                                                                   \
                \"float13\" : 3.000000,                                                          \
                \"long13\" : 4,                                                                  \
                \"sint8_t13\" : 5,                                                               \
                \"uint8_t13\" : 6,                                                               \
                \"int16_t13\" : 7,                                                               \
                \"int32_t13\" : 8,                                                               \
                \"int64_t13\" : 9,                                                               \
                \"bool13\" : true,                                                               \
                \"ascii_char_ptr13\" : \"eleven\",                                               \
                \"ascii_char_ptr_no_quotes13\" : \"twelve\",                                     \
                \"EdmDateTimeOffset13\" : \"2014-06-17T08:51:23.000000000005-08:01\",            \
                \"EdmGuid13\" : \"00112233-4455-6677-8899-AABBCCDDEEFF\",                        \
                \"EdmBinary13\": \"MzQ1\"                                                        \
            }                                                                                    \
        }";

        ///act
        CODEFIRST_RESULT result = (EXECUTE_COMMAND(modelWithData, inputJsonAsString) == EXECUTE_COMMAND_SUCCESS) ? CODEFIRST_OK : CODEFIRST_ERROR;

        ///assert (rest of asserts are in the action)
        ASSERT_ARE_EQUAL(CODEFIRST_RESULT, CODEFIRST_OK, result);

        ///clean
        DESTROY_MODEL_INSTANCE(modelWithData);
    }

    TEST_FUNCTION(WITH_ACTION_IN_MODEL_IN_MODEL)
    {
        ///arrange
        outerModel14 *modelWithData = CREATE_MODEL_INSTANCE(basic14, outerModel14, true);

        const char* inputJsonAsString =
            "{                                                                                   \
            \"Name\": \"modelAction14/action14\",                                               \
            \"Parameters\":                                                                      \
            {                                                                                    \
                \"double14\" : 1.0,                                                              \
                \"int14\" : 2,                                                                   \
                \"float14\" : 3.000000,                                                          \
                \"long14\" : 4,                                                                  \
                \"sint8_t14\" : 5,                                                               \
                \"uint8_t14\" : 6,                                                               \
                \"int16_t14\" : 7,                                                               \
                \"int32_t14\" : 8,                                                               \
                \"int64_t14\" : 9,                                                               \
                \"bool14\" : true,                                                               \
                \"ascii_char_ptr14\" : \"eleven\",                                               \
                \"ascii_char_ptr_no_quotes14\" : \"twelve\",                                     \
                \"EdmDateTimeOffset14\" : \"2014-06-17T08:51:23.000000000005-08:01\",            \
                \"EdmGuid14\" : \"00112233-4455-6677-8899-AABBCCDDEEFF\",                        \
                \"EdmBinary14\": \"MzQ1\"                                                        \
            }                                                                                    \
        }";

        ///act
        CODEFIRST_RESULT result = (EXECUTE_COMMAND(modelWithData, inputJsonAsString) == EXECUTE_COMMAND_SUCCESS) ? CODEFIRST_OK : CODEFIRST_ERROR;

        ///assert (rest of asserts are in the action)
        ASSERT_ARE_EQUAL(CODEFIRST_RESULT, CODEFIRST_OK, result);

        ///clean
        DESTROY_MODEL_INSTANCE(modelWithData);
    }

    /*Tests_SRS_CODEFIRST_02_036: [ CodeFirst_CreateDevice shall initialize all the desired properties to their default values. ]*/
    TEST_FUNCTION(CodeFirst_CreateDevice_initialize_all_desired_properties_to_default_values)
    {
        ///arrange
        
        ///act
        basicModel_WithData9 *modelWithData = CREATE_MODEL_INSTANCE(basic9, basicModel_WithData9, true);

        ///assert
        ASSERT_ARE_EQUAL(double,    0,              modelWithData->with_desired_property_double9);
        ASSERT_ARE_EQUAL(int,       0,              modelWithData->with_desired_property_int9);
        ASSERT_ARE_EQUAL(float,     0,              modelWithData->with_desired_property_float9);
        ASSERT_ARE_EQUAL(long,      0,              modelWithData->with_desired_property_long9);
        ASSERT_ARE_EQUAL(int8_t,    0,              modelWithData->with_desired_property_sint8_t9);
        ASSERT_ARE_EQUAL(uint8_t,   0,              modelWithData->with_desired_property_uint8_t9);
        ASSERT_ARE_EQUAL(int16_t,   0,              modelWithData->with_desired_property_int16_t9);
        ASSERT_ARE_EQUAL(int32_t,   0,              modelWithData->with_desired_property_int32_t9);
        ASSERT_ARE_EQUAL(int64_t,   0,              modelWithData->with_desired_property_int64_t9);
        ASSERT_IS_FALSE(modelWithData->with_desired_property_bool9);
        ASSERT_IS_NULL(modelWithData->with_desired_property_ascii_char_ptr9);
        ASSERT_IS_NULL(modelWithData->with_desired_property_ascii_char_ptr_no_quotes9);
        ASSERT_ARE_EQUAL(int,       0,              modelWithData->with_desired_property_EdmDateTimeOffset9.dateTime.tm_year);
        ASSERT_ARE_EQUAL(int,       0,              modelWithData->with_desired_property_EdmDateTimeOffset9.dateTime.tm_mon);
        ASSERT_ARE_EQUAL(int,       0,              modelWithData->with_desired_property_EdmDateTimeOffset9.dateTime.tm_mday);
        ASSERT_ARE_EQUAL(int,       0,              modelWithData->with_desired_property_EdmDateTimeOffset9.dateTime.tm_hour);
        ASSERT_ARE_EQUAL(int,       0,              modelWithData->with_desired_property_EdmDateTimeOffset9.dateTime.tm_min);
        ASSERT_ARE_EQUAL(int,       0,              modelWithData->with_desired_property_EdmDateTimeOffset9.dateTime.tm_sec);
        ASSERT_ARE_EQUAL(uint8_t,   0,              modelWithData->with_desired_property_EdmDateTimeOffset9.hasFractionalSecond);
        ASSERT_ARE_EQUAL(uint64_t,  0,              modelWithData->with_desired_property_EdmDateTimeOffset9.fractionalSecond);
        ASSERT_ARE_EQUAL(uint8_t,   0,              modelWithData->with_desired_property_EdmDateTimeOffset9.hasTimeZone);
        ASSERT_ARE_EQUAL(int8_t,    0,              modelWithData->with_desired_property_EdmDateTimeOffset9.timeZoneHour);
        ASSERT_ARE_EQUAL(uint8_t,   0,              modelWithData->with_desired_property_EdmDateTimeOffset9.timeZoneMinute);
        ASSERT_ARE_EQUAL(uint8_t,   0,              modelWithData->with_desired_property_EdmGuid9.GUID[0]);
        ASSERT_ARE_EQUAL(uint8_t,   0,              modelWithData->with_desired_property_EdmGuid9.GUID[1]);
        ASSERT_ARE_EQUAL(uint8_t,   0,              modelWithData->with_desired_property_EdmGuid9.GUID[2]);
        ASSERT_ARE_EQUAL(uint8_t,   0,              modelWithData->with_desired_property_EdmGuid9.GUID[3]);
        ASSERT_ARE_EQUAL(uint8_t,   0,              modelWithData->with_desired_property_EdmGuid9.GUID[4]);
        ASSERT_ARE_EQUAL(uint8_t,   0,              modelWithData->with_desired_property_EdmGuid9.GUID[5]);
        ASSERT_ARE_EQUAL(uint8_t,   0,              modelWithData->with_desired_property_EdmGuid9.GUID[6]);
        ASSERT_ARE_EQUAL(uint8_t,   0,              modelWithData->with_desired_property_EdmGuid9.GUID[7]);
        ASSERT_ARE_EQUAL(uint8_t,   0,              modelWithData->with_desired_property_EdmGuid9.GUID[8]);
        ASSERT_ARE_EQUAL(uint8_t,   0,              modelWithData->with_desired_property_EdmGuid9.GUID[9]);
        ASSERT_ARE_EQUAL(uint8_t,   0,              modelWithData->with_desired_property_EdmGuid9.GUID[10]);
        ASSERT_ARE_EQUAL(uint8_t,   0,              modelWithData->with_desired_property_EdmGuid9.GUID[11]);
        ASSERT_ARE_EQUAL(uint8_t,   0,              modelWithData->with_desired_property_EdmGuid9.GUID[12]);
        ASSERT_ARE_EQUAL(uint8_t,   0,              modelWithData->with_desired_property_EdmGuid9.GUID[13]);
        ASSERT_ARE_EQUAL(uint8_t,   0,              modelWithData->with_desired_property_EdmGuid9.GUID[14]);
        ASSERT_ARE_EQUAL(uint8_t,   0,              modelWithData->with_desired_property_EdmGuid9.GUID[15]);
        ASSERT_ARE_EQUAL(size_t,     0,             modelWithData->with_desired_property_EdmBinary9.size);
        ASSERT_IS_NULL(modelWithData->with_desired_property_EdmBinary9.data);

        ///clean
        DESTROY_MODEL_INSTANCE(modelWithData);
    }

    /*this function tests that a regular desired properties of a model can have a desired property callback that is invoked*/
    TEST_FUNCTION(WITH_DESIRED_PROPERTY_IN_ROOT_MODEL_CALLS_ON_DESIRED_PROPERTY)
    {
        ///arrange
        basicModel_WithData15 *modelWithData = CREATE_MODEL_INSTANCE(basic15, basicModel_WithData15, true);

        const char* inputJsonAsString =
        "{                                                                                                    \
            \"with_desired_property_double15\" : 1.0,                                                          \
            \"with_desired_property_int15\" : 2,                                                               \
            \"with_desired_property_float15\" : 3.000000,                                                      \
            \"with_desired_property_long15\" : 4,                                                              \
            \"with_desired_property_sint8_t15\" : 5,                                                           \
            \"with_desired_property_uint8_t15\" : 6,                                                           \
            \"with_desired_property_int16_t15\" : 7,                                                           \
            \"with_desired_property_int32_t15\" : 8,                                                           \
            \"with_desired_property_int64_t15\" : 9,                                                           \
            \"with_desired_property_bool15\" : true,                                                           \
            \"with_desired_property_ascii_char_ptr15\" : \"eleven\",                                           \
            \"with_desired_property_ascii_char_ptr_no_quotes15\" : \"twelve\",                                 \
            \"with_desired_property_EdmDateTimeOffset15\" : \"2014-06-17T08:51:23.000000000005-08:01\",        \
            \"with_desired_property_EdmGuid15\" : \"00112233-4455-6677-8899-AABBCCDDEEFF\",                    \
            \"with_desired_property_EdmBinary15\": \"MzQ1\"                                                    \
        }";

        STRICT_EXPECTED_CALL(on_desired_property_double15(modelWithData));
        STRICT_EXPECTED_CALL(on_desired_property_int15(modelWithData));
        STRICT_EXPECTED_CALL(on_desired_property_float15(modelWithData));
        STRICT_EXPECTED_CALL(on_desired_property_long15(modelWithData));
        STRICT_EXPECTED_CALL(on_desired_property_sint8_t15(modelWithData));
        STRICT_EXPECTED_CALL(on_desired_property_uint8_t15(modelWithData));
        STRICT_EXPECTED_CALL(on_desired_property_int16_t15(modelWithData));
        STRICT_EXPECTED_CALL(on_desired_property_int32_t15(modelWithData));
        STRICT_EXPECTED_CALL(on_desired_property_int64_t15(modelWithData));
        STRICT_EXPECTED_CALL(on_desired_property_bool15(modelWithData));
        STRICT_EXPECTED_CALL(on_desired_property_ascii_char_ptr15(modelWithData));
        STRICT_EXPECTED_CALL(on_desired_property_ascii_char_ptr_no_quotes15(modelWithData));
        STRICT_EXPECTED_CALL(on_desired_property_EdmDateTimeOffset15(modelWithData));
        STRICT_EXPECTED_CALL(on_desired_property_EdmGuid15(modelWithData));
        STRICT_EXPECTED_CALL(on_desired_property_EdmBinary15(modelWithData));

        ///act
        CODEFIRST_RESULT result = INGEST_DESIRED_PROPERTIES(modelWithData, inputJsonAsString);

        ///assert
        ASSERT_ARE_EQUAL(CODEFIRST_RESULT, CODEFIRST_OK, result);

        /*setting values to the model instance*/
        ASSERT_ARE_EQUAL(double,    1.0,            modelWithData->with_desired_property_double15);
        ASSERT_ARE_EQUAL(int,       2,              modelWithData->with_desired_property_int15);
        ASSERT_ARE_EQUAL(float,     3.0,            modelWithData->with_desired_property_float15);
        ASSERT_ARE_EQUAL(long,      4,              modelWithData->with_desired_property_long15);
        ASSERT_ARE_EQUAL(int8_t,    5,              modelWithData->with_desired_property_sint8_t15);
        ASSERT_ARE_EQUAL(uint8_t,   6,              modelWithData->with_desired_property_uint8_t15);
        ASSERT_ARE_EQUAL(int16_t,   7,              modelWithData->with_desired_property_int16_t15);
        ASSERT_ARE_EQUAL(int32_t,   8,              modelWithData->with_desired_property_int32_t15);
        ASSERT_ARE_EQUAL(int64_t,   9,              modelWithData->with_desired_property_int64_t15);
        ASSERT_IS_TRUE(modelWithData->with_desired_property_bool15);
        ASSERT_ARE_EQUAL(char_ptr,  "eleven",       modelWithData->with_desired_property_ascii_char_ptr15);
        ASSERT_ARE_EQUAL(char_ptr,  "\"twelve\"",   modelWithData->with_desired_property_ascii_char_ptr_no_quotes15);
        ASSERT_ARE_EQUAL(int,       114,            modelWithData->with_desired_property_EdmDateTimeOffset15.dateTime.tm_year);
        ASSERT_ARE_EQUAL(int,       6-1,            modelWithData->with_desired_property_EdmDateTimeOffset15.dateTime.tm_mon);
        ASSERT_ARE_EQUAL(int,       17,             modelWithData->with_desired_property_EdmDateTimeOffset15.dateTime.tm_mday);
        ASSERT_ARE_EQUAL(int,       8,              modelWithData->with_desired_property_EdmDateTimeOffset15.dateTime.tm_hour);
        ASSERT_ARE_EQUAL(int,       51,             modelWithData->with_desired_property_EdmDateTimeOffset15.dateTime.tm_min);
        ASSERT_ARE_EQUAL(int,       23,             modelWithData->with_desired_property_EdmDateTimeOffset15.dateTime.tm_sec);
        ASSERT_ARE_EQUAL(uint8_t,   1,              modelWithData->with_desired_property_EdmDateTimeOffset15.hasFractionalSecond);
        ASSERT_ARE_EQUAL(uint64_t,  5,              modelWithData->with_desired_property_EdmDateTimeOffset15.fractionalSecond);
        ASSERT_ARE_EQUAL(uint8_t,   1,              modelWithData->with_desired_property_EdmDateTimeOffset15.hasTimeZone);
        ASSERT_ARE_EQUAL(int8_t,    -8,             modelWithData->with_desired_property_EdmDateTimeOffset15.timeZoneHour);
        ASSERT_ARE_EQUAL(uint8_t,   1,              modelWithData->with_desired_property_EdmDateTimeOffset15.timeZoneMinute);
        ASSERT_ARE_EQUAL(uint8_t,   0x00,           modelWithData->with_desired_property_EdmGuid15.GUID[0]);
        ASSERT_ARE_EQUAL(uint8_t,   0x11,           modelWithData->with_desired_property_EdmGuid15.GUID[1]);
        ASSERT_ARE_EQUAL(uint8_t,   0x22,           modelWithData->with_desired_property_EdmGuid15.GUID[2]);
        ASSERT_ARE_EQUAL(uint8_t,   0x33,           modelWithData->with_desired_property_EdmGuid15.GUID[3]);
        ASSERT_ARE_EQUAL(uint8_t,   0x44,           modelWithData->with_desired_property_EdmGuid15.GUID[4]);
        ASSERT_ARE_EQUAL(uint8_t,   0x55,           modelWithData->with_desired_property_EdmGuid15.GUID[5]);
        ASSERT_ARE_EQUAL(uint8_t,   0x66,           modelWithData->with_desired_property_EdmGuid15.GUID[6]);
        ASSERT_ARE_EQUAL(uint8_t,   0x77,           modelWithData->with_desired_property_EdmGuid15.GUID[7]);
        ASSERT_ARE_EQUAL(uint8_t,   0x88,           modelWithData->with_desired_property_EdmGuid15.GUID[8]);
        ASSERT_ARE_EQUAL(uint8_t,   0x99,           modelWithData->with_desired_property_EdmGuid15.GUID[9]);
        ASSERT_ARE_EQUAL(uint8_t,   0xAA,           modelWithData->with_desired_property_EdmGuid15.GUID[10]);
        ASSERT_ARE_EQUAL(uint8_t,   0xBB,           modelWithData->with_desired_property_EdmGuid15.GUID[11]);
        ASSERT_ARE_EQUAL(uint8_t,   0xCC,           modelWithData->with_desired_property_EdmGuid15.GUID[12]);
        ASSERT_ARE_EQUAL(uint8_t,   0xDD,           modelWithData->with_desired_property_EdmGuid15.GUID[13]);
        ASSERT_ARE_EQUAL(uint8_t,   0xEE,           modelWithData->with_desired_property_EdmGuid15.GUID[14]);
        ASSERT_ARE_EQUAL(uint8_t,   0xFF,           modelWithData->with_desired_property_EdmGuid15.GUID[15]);

        ASSERT_ARE_EQUAL(size_t,     3,             modelWithData->with_desired_property_EdmBinary15.size);
        ASSERT_ARE_EQUAL(uint8_t,   '3',            modelWithData->with_desired_property_EdmBinary15.data[0]);
        ASSERT_ARE_EQUAL(uint8_t,   '4',            modelWithData->with_desired_property_EdmBinary15.data[1]);
        ASSERT_ARE_EQUAL(uint8_t,   '5',            modelWithData->with_desired_property_EdmBinary15.data[2]);

        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///clean
        DESTROY_MODEL_INSTANCE(modelWithData);
    }

    /*this function tests that model in model desired properties can have callbacks that are invoked*/
    TEST_FUNCTION(WITH_DESIRED_PROPERTY_IN_STRUCT_IN_MODEL_IN_MODEL_DESIRED_PROPERTY_CALLBACK)
    {
        ///arrange
        outerModel16 *modelWithData = CREATE_MODEL_INSTANCE(basic16, outerModel16, true);

        const char* inputJsonAsString =
        "{                                                                                                             \
            \"inner_model16\":                                                                                         \
            {                                                                                                          \
                \"structure16\":                                                                                       \
                {                                                                                                      \
                    \"with_desired_property_double16\" : 1.0,                                                          \
                    \"with_desired_property_int16\" : 2,                                                               \
                    \"with_desired_property_float16\" : 3.000000,                                                      \
                    \"with_desired_property_long16\" : 4,                                                              \
                    \"with_desired_property_sint8_t16\" : 5,                                                           \
                    \"with_desired_property_uint8_t16\" : 6,                                                           \
                    \"with_desired_property_int16_t16\" : 7,                                                           \
                    \"with_desired_property_int32_t16\" : 8,                                                           \
                    \"with_desired_property_int64_t16\" : 9,                                                           \
                    \"with_desired_property_bool16\" : true,                                                           \
                    \"with_desired_property_ascii_char_ptr16\" : \"eleven\",                                           \
                    \"with_desired_property_ascii_char_ptr_no_quotes16\" : \"twelve\",                                 \
                    \"with_desired_property_EdmDateTimeOffset16\" : \"2014-06-17T08:51:23.000000000005-08:01\",        \
                    \"with_desired_property_EdmGuid16\" : \"00112233-4455-6677-8899-AABBCCDDEEFF\",                    \
                    \"with_desired_property_EdmBinary16\": \"MzQ1\"                                                    \
                }                                                                                                      \
            }                                                                                                          \
        }";

        STRICT_EXPECTED_CALL(on_structure16(&(modelWithData->inner_model16)));
        STRICT_EXPECTED_CALL(on_inner_model16(modelWithData));

        ///act
        CODEFIRST_RESULT result = INGEST_DESIRED_PROPERTIES(modelWithData, inputJsonAsString);

        ///assert
        ASSERT_ARE_EQUAL(CODEFIRST_RESULT, CODEFIRST_OK, result);

        ASSERT_ARE_EQUAL(double,    1.0,            modelWithData->inner_model16.structure16.with_desired_property_double16);
        ASSERT_ARE_EQUAL(int,       2,              modelWithData->inner_model16.structure16.with_desired_property_int16);
        ASSERT_ARE_EQUAL(float,     3.0,            modelWithData->inner_model16.structure16.with_desired_property_float16);
        ASSERT_ARE_EQUAL(long,      4,              modelWithData->inner_model16.structure16.with_desired_property_long16);
        ASSERT_ARE_EQUAL(int8_t,    5,              modelWithData->inner_model16.structure16.with_desired_property_sint8_t16);
        ASSERT_ARE_EQUAL(uint8_t,   6,              modelWithData->inner_model16.structure16.with_desired_property_uint8_t16);
        ASSERT_ARE_EQUAL(int16_t,   7,              modelWithData->inner_model16.structure16.with_desired_property_int16_t16);
        ASSERT_ARE_EQUAL(int32_t,   8,              modelWithData->inner_model16.structure16.with_desired_property_int32_t16);
        ASSERT_ARE_EQUAL(int64_t,   9,              modelWithData->inner_model16.structure16.with_desired_property_int64_t16);
        ASSERT_IS_TRUE(modelWithData->inner_model16.structure16.with_desired_property_bool16);
        ASSERT_ARE_EQUAL(char_ptr,  "eleven",       modelWithData->inner_model16.structure16.with_desired_property_ascii_char_ptr16);
        ASSERT_ARE_EQUAL(char_ptr,  "\"twelve\"",   modelWithData->inner_model16.structure16.with_desired_property_ascii_char_ptr_no_quotes16);
        ASSERT_ARE_EQUAL(int,       114,            modelWithData->inner_model16.structure16.with_desired_property_EdmDateTimeOffset16.dateTime.tm_year);
        ASSERT_ARE_EQUAL(int,       6-1,            modelWithData->inner_model16.structure16.with_desired_property_EdmDateTimeOffset16.dateTime.tm_mon);
        ASSERT_ARE_EQUAL(int,       17,             modelWithData->inner_model16.structure16.with_desired_property_EdmDateTimeOffset16.dateTime.tm_mday);
        ASSERT_ARE_EQUAL(int,       8,              modelWithData->inner_model16.structure16.with_desired_property_EdmDateTimeOffset16.dateTime.tm_hour);
        ASSERT_ARE_EQUAL(int,       51,             modelWithData->inner_model16.structure16.with_desired_property_EdmDateTimeOffset16.dateTime.tm_min);
        ASSERT_ARE_EQUAL(int,       23,             modelWithData->inner_model16.structure16.with_desired_property_EdmDateTimeOffset16.dateTime.tm_sec);
        ASSERT_ARE_EQUAL(uint8_t,   1,              modelWithData->inner_model16.structure16.with_desired_property_EdmDateTimeOffset16.hasFractionalSecond);
        ASSERT_ARE_EQUAL(uint64_t,  5,              modelWithData->inner_model16.structure16.with_desired_property_EdmDateTimeOffset16.fractionalSecond);
        ASSERT_ARE_EQUAL(uint8_t,   1,              modelWithData->inner_model16.structure16.with_desired_property_EdmDateTimeOffset16.hasTimeZone);
        ASSERT_ARE_EQUAL(int8_t,    -8,             modelWithData->inner_model16.structure16.with_desired_property_EdmDateTimeOffset16.timeZoneHour);
        ASSERT_ARE_EQUAL(uint8_t,   1,              modelWithData->inner_model16.structure16.with_desired_property_EdmDateTimeOffset16.timeZoneMinute);
        ASSERT_ARE_EQUAL(uint8_t,   0x00,           modelWithData->inner_model16.structure16.with_desired_property_EdmGuid16.GUID[0]);
        ASSERT_ARE_EQUAL(uint8_t,   0x11,           modelWithData->inner_model16.structure16.with_desired_property_EdmGuid16.GUID[1]);
        ASSERT_ARE_EQUAL(uint8_t,   0x22,           modelWithData->inner_model16.structure16.with_desired_property_EdmGuid16.GUID[2]);
        ASSERT_ARE_EQUAL(uint8_t,   0x33,           modelWithData->inner_model16.structure16.with_desired_property_EdmGuid16.GUID[3]);
        ASSERT_ARE_EQUAL(uint8_t,   0x44,           modelWithData->inner_model16.structure16.with_desired_property_EdmGuid16.GUID[4]);
        ASSERT_ARE_EQUAL(uint8_t,   0x55,           modelWithData->inner_model16.structure16.with_desired_property_EdmGuid16.GUID[5]);
        ASSERT_ARE_EQUAL(uint8_t,   0x66,           modelWithData->inner_model16.structure16.with_desired_property_EdmGuid16.GUID[6]);
        ASSERT_ARE_EQUAL(uint8_t,   0x77,           modelWithData->inner_model16.structure16.with_desired_property_EdmGuid16.GUID[7]);
        ASSERT_ARE_EQUAL(uint8_t,   0x88,           modelWithData->inner_model16.structure16.with_desired_property_EdmGuid16.GUID[8]);
        ASSERT_ARE_EQUAL(uint8_t,   0x99,           modelWithData->inner_model16.structure16.with_desired_property_EdmGuid16.GUID[9]);
        ASSERT_ARE_EQUAL(uint8_t,   0xAA,           modelWithData->inner_model16.structure16.with_desired_property_EdmGuid16.GUID[10]);
        ASSERT_ARE_EQUAL(uint8_t,   0xBB,           modelWithData->inner_model16.structure16.with_desired_property_EdmGuid16.GUID[11]);
        ASSERT_ARE_EQUAL(uint8_t,   0xCC,           modelWithData->inner_model16.structure16.with_desired_property_EdmGuid16.GUID[12]);
        ASSERT_ARE_EQUAL(uint8_t,   0xDD,           modelWithData->inner_model16.structure16.with_desired_property_EdmGuid16.GUID[13]);
        ASSERT_ARE_EQUAL(uint8_t,   0xEE,           modelWithData->inner_model16.structure16.with_desired_property_EdmGuid16.GUID[14]);
        ASSERT_ARE_EQUAL(uint8_t,   0xFF,           modelWithData->inner_model16.structure16.with_desired_property_EdmGuid16.GUID[15]);

        ASSERT_ARE_EQUAL(size_t,     3,             modelWithData->inner_model16.structure16.with_desired_property_EdmBinary16.size);
        ASSERT_ARE_EQUAL(uint8_t,   '3',            modelWithData->inner_model16.structure16.with_desired_property_EdmBinary16.data[0]);
        ASSERT_ARE_EQUAL(uint8_t,   '4',            modelWithData->inner_model16.structure16.with_desired_property_EdmBinary16.data[1]);
        ASSERT_ARE_EQUAL(uint8_t,   '5',            modelWithData->inner_model16.structure16.with_desired_property_EdmBinary16.data[2]);

        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///clean
        DESTROY_MODEL_INSTANCE(modelWithData);
    }

END_TEST_SUITE(serializer_int)
