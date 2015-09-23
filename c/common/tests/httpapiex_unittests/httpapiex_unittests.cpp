// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <cstdlib>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "testrunnerswitcher.h"
#include "micromock.h"
#include "micromockcharstararenullterminatedstrings.h"
/*the below is a horrible hack*/
#include "macro_utils.h"
#undef DEFINE_ENUM
#define DEFINE_ENUM(enumName, ...) typedef enum C2(enumName, _TAG) { FOR_EACH_1(DEFINE_ENUMERATION_CONSTANT, __VA_ARGS__)} enumName; 

#include "map.h"
#include "httpapiex.h"
#include "lock.h"
#include "strings.h"
#include "crt_abstractions.h"
#include "vector.h"



DEFINE_MICROMOCK_ENUM_TO_STRING(HTTPAPIEX_RESULT, HTTPAPIEX_RESULT_VALUES);

#define GBALLOC_H

extern "C" int gballoc_init(void);
extern "C" void gballoc_deinit(void);
extern "C" void* gballoc_malloc(size_t size);
extern "C" void* gballoc_calloc(size_t nmemb, size_t size);
extern "C" void* gballoc_realloc(void* ptr, size_t size);
extern "C" void gballoc_free(void* ptr);


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
#include "buffer.c"
#include "vector.c"
};

static size_t currentmalloc_call;
static size_t whenShallmalloc_fail;

static size_t currentrealloc_call;
static size_t whenShallrealloc_fail;

static size_t currentHTTPAPI_SaveOption_call;
static size_t whenShallHTTPAPI_SaveOption_fail;

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

static size_t currentBUFFER_new_call;
static size_t whenShallBUFFER_new_fail;

static size_t currentBUFFER_build_call;
static size_t whenShallBUFFER_build_fail;

static size_t currentBUFFER_content_call;
static size_t whenShallBUFFER_content_fail;

static size_t currentBUFFER_size_call;
static size_t whenShallBUFFER_size_fail;

static size_t currentHTTPHeaders_Alloc_call;
static size_t whenShallHTTPHeaders_Alloc_fail;

static size_t currentVECTOR_create_call;
static size_t whenShallVECTOR_create_fail;

static size_t currentVECTOR_push_back_call;
static size_t whenShallVECTOR_push_back_fail;

#define N_MAX_FAILS 5
static size_t currentHTTPAPI_CreateConnection_call;
static size_t whenShallHTTPAPI_CreateConnection_fail[N_MAX_FAILS];

static size_t currentHTTPAPI_Init_call;
static size_t whenShallHTTPAPI_Init_fail[N_MAX_FAILS];
static size_t HTTPAPI_Init_calls;

#define TEST_HOSTNAME "aaa"
#define TEST_RELATIVE_PATH "nothing/to/see/here/devices"
#define TEST_REQUEST_HTTP_HEADERS (HTTP_HEADERS_HANDLE) 0x42
#define TEST_REQUEST_BODY (BUFFER_HANDLE) 0x43
#define TEST_RESPONSE_HTTP_HEADERS (HTTP_HEADERS_HANDLE) 0x45
#define TEST_RESPONSE_BODY (BUFFER_HANDLE) 0x46
#define TEST_HTTP_HEADERS_HANDLE (HTTP_HEADERS_HANDLE) 0x47
#define TEST_BUFFER "333333"
#define TEST_BUFFER_SIZE 6

TYPED_MOCK_CLASS(CHTTPAPIEXMocks, CGlobalMock)
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
        void* result2;
    currentrealloc_call++;
    if (whenShallrealloc_fail>0)
    {
        if (currentrealloc_call == whenShallrealloc_fail)
        {
            result2 = (STRING_HANDLE)NULL;
        }
        else
        {
            result2 = BASEIMPLEMENTATION::gballoc_realloc(ptr,size);
        }
    }
    else
    {
        result2 = BASEIMPLEMENTATION::gballoc_realloc(ptr, size);
    }
    MOCK_METHOD_END(void*,result2);

    MOCK_STATIC_METHOD_1(, void, gballoc_free, void*, ptr)
        BASEIMPLEMENTATION::gballoc_free(ptr);
    MOCK_VOID_METHOD_END()

        /*Strings*/
    MOCK_STATIC_METHOD_0(, STRING_HANDLE, STRING_new)
        STRING_HANDLE result2;
        currentSTRING_new_call++;
        if (whenShallSTRING_new_fail > 0)
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
        if (whenShallSTRING_clone_fail > 0)
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
        if (whenShallSTRING_construct_fail > 0)
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
    MOCK_METHOD_END(int, (((whenShallSTRING_concat_fail > 0) && (currentSTRING_concat_call == whenShallSTRING_concat_fail)) ? __LINE__ : BASEIMPLEMENTATION::STRING_concat(s1, s2)));

    MOCK_STATIC_METHOD_2(, int, STRING_concat_with_STRING, STRING_HANDLE, s1, STRING_HANDLE, s2)
        currentSTRING_concat_with_STRING_call++;
    MOCK_METHOD_END(int, (((currentSTRING_concat_with_STRING_call > 0) && (currentSTRING_concat_with_STRING_call == whenShallSTRING_concat_with_STRING_fail)) ? __LINE__ : BASEIMPLEMENTATION::STRING_concat_with_STRING(s1, s2)));

    MOCK_STATIC_METHOD_1(, int, STRING_empty, STRING_HANDLE, s)
        currentSTRING_concat_call++;
    MOCK_METHOD_END(int, BASEIMPLEMENTATION::STRING_empty(s));

    MOCK_STATIC_METHOD_1(, const char*, STRING_c_str, STRING_HANDLE, s)
        MOCK_METHOD_END(const char*, BASEIMPLEMENTATION::STRING_c_str(s))

        /* HTTPHeaders mocks */
    MOCK_STATIC_METHOD_0(, HTTP_HEADERS_HANDLE, HTTPHeaders_Alloc)
        HTTP_HEADERS_HANDLE result2;
        currentHTTPHeaders_Alloc_call++;
        if ((whenShallHTTPHeaders_Alloc_fail > 0) && (whenShallHTTPHeaders_Alloc_fail == currentHTTPHeaders_Alloc_call))
        {
            result2 = NULL;
        }
        else
        {
            result2 = malloc(1);
        }
    MOCK_METHOD_END(HTTP_HEADERS_HANDLE, result2)

    MOCK_STATIC_METHOD_1(, void, HTTPHeaders_Free, HTTP_HEADERS_HANDLE, httpHeadersHandle)
        free(httpHeadersHandle);
    MOCK_VOID_METHOD_END()

    MOCK_STATIC_METHOD_3(, HTTP_HEADERS_RESULT, HTTPHeaders_AddHeaderNameValuePair, HTTP_HEADERS_HANDLE, httpHeadersHandle, const char*, name, const char*, value)
    MOCK_METHOD_END(HTTP_HEADERS_RESULT, HTTP_HEADERS_OK)

    MOCK_STATIC_METHOD_3(, HTTP_HEADERS_RESULT, HTTPHeaders_ReplaceHeaderNameValuePair, HTTP_HEADERS_HANDLE, httpHeadersHandle, const char*, name, const char*, value)
    MOCK_METHOD_END(HTTP_HEADERS_RESULT, HTTP_HEADERS_OK)

        /* BUFFER Mocks */
    MOCK_STATIC_METHOD_0(, BUFFER_HANDLE, BUFFER_new)
        BUFFER_HANDLE result2;
        currentBUFFER_new_call++;
        if (whenShallBUFFER_new_fail > 0)
        {
            if (currentBUFFER_new_call == whenShallBUFFER_new_fail)
            {
                result2 = (BUFFER_HANDLE)NULL;
            }
            else
            {
                result2 = BASEIMPLEMENTATION::BUFFER_new();
            }
        }
        else
        {
            result2 = BASEIMPLEMENTATION::BUFFER_new();
        }
    MOCK_METHOD_END(BUFFER_HANDLE, result2)

    MOCK_STATIC_METHOD_1(, void, BUFFER_delete, BUFFER_HANDLE, s)
        BASEIMPLEMENTATION::BUFFER_delete(s);
    MOCK_VOID_METHOD_END()

    MOCK_STATIC_METHOD_1(, unsigned char*, BUFFER_u_char, BUFFER_HANDLE, handle);
        unsigned char* result2;
         result2 = BASEIMPLEMENTATION::BUFFER_u_char(handle);
    MOCK_METHOD_END(unsigned char*, result2)
    
    MOCK_STATIC_METHOD_1(, size_t, BUFFER_length, BUFFER_HANDLE, handle);
        size_t result2;
        result2 = BASEIMPLEMENTATION::BUFFER_length(handle);
    MOCK_METHOD_END(size_t, result2)

    MOCK_STATIC_METHOD_3(, int, BUFFER_build, BUFFER_HANDLE, handle, const unsigned char*, source, size_t, size)
    MOCK_METHOD_END(int, BASEIMPLEMENTATION::BUFFER_build(handle, source, size))

    /* HTTPAPI mocks */
    
    MOCK_STATIC_METHOD_0(, HTTPAPI_RESULT, HTTPAPI_Init)
    HTTPAPI_RESULT result2;
        currentHTTPAPI_Init_call++;
        size_t i;
        for (i = 0; i < N_MAX_FAILS; i++)
        {
            if (whenShallHTTPAPI_Init_fail[i] > 0)
            {
                if (currentHTTPAPI_Init_call == whenShallHTTPAPI_Init_fail[i])
                {
                    break;
                }
            }
        }

        if (i == N_MAX_FAILS)
        {
            HTTPAPI_Init_calls++;
            result2 = HTTPAPI_OK;
        }
        else
        {
            result2 = HTTPAPI_ERROR;
        }
    MOCK_METHOD_END(HTTPAPI_RESULT, result2)

    MOCK_STATIC_METHOD_0(, void, HTTPAPI_Deinit)
        HTTPAPI_Init_calls--;
    MOCK_VOID_METHOD_END()

    MOCK_STATIC_METHOD_1(, HTTP_HANDLE, HTTPAPI_CreateConnection, const char*, hostName)
        HTTP_HANDLE result2;
        currentHTTPAPI_CreateConnection_call++;
        size_t i;
        for (i = 0; i < N_MAX_FAILS; i++)
        {
            if (whenShallHTTPAPI_CreateConnection_fail[i] > 0)
            {
                if (currentHTTPAPI_CreateConnection_call == whenShallHTTPAPI_CreateConnection_fail[i])
                {
                    result2 = (HTTP_HANDLE)NULL;
                    break;
                }
            }
        }

        if (i==N_MAX_FAILS)
        {
            result2 = malloc(1);
        }
        MOCK_METHOD_END(HTTP_HANDLE, result2)

    MOCK_STATIC_METHOD_1(, void, HTTPAPI_CloseConnection, HTTP_HANDLE, handle)
        free(handle);
    MOCK_VOID_METHOD_END()

    MOCK_STATIC_METHOD_9(, HTTPAPI_RESULT, HTTPAPI_ExecuteRequest, HTTP_HANDLE, handle, HTTPAPI_REQUEST_TYPE, requestType, const char*, relativePath,
    HTTP_HEADERS_HANDLE, httpHeadersHandle, const unsigned char*, content, size_t, contentLength, unsigned int*, statusCode,
    HTTP_HEADERS_HANDLE, responseHeadersHandle, BUFFER_HANDLE, responseContent)
    MOCK_METHOD_END(HTTPAPI_RESULT, HTTPAPI_OK)

    MOCK_STATIC_METHOD_3(, HTTPAPI_RESULT, HTTPAPI_SetOption, HTTP_HANDLE, handle, const char*, optionName, const void*, value)
    MOCK_METHOD_END(HTTPAPI_RESULT, HTTPAPI_OK)

    MOCK_STATIC_METHOD_3(, HTTPAPI_RESULT, HTTPAPI_CloneOption, const char*, optionName, const void*, value, const void**, savedValue)
        HTTPAPI_RESULT result2;
        currentHTTPAPI_SaveOption_call++;
        if (currentHTTPAPI_SaveOption_call == whenShallHTTPAPI_SaveOption_fail)
        {
            result2 = HTTPAPI_ERROR;
        }
        else
        {
            result2 = HTTPAPI_OK;
            if (strcmp("someOption", optionName) == 0)
            {
                char* temp;
                temp = (char *)BASEIMPLEMENTATION::gballoc_malloc(strlen((const char*)value) + 1);
                strcpy(temp, (const char*)value);
                *savedValue = temp;
            }
            else if (strcmp("someOption1", optionName) == 0)
            {
                char* temp;
                temp = (char *)BASEIMPLEMENTATION::gballoc_malloc(strlen((const char*)value) + 1);
                strcpy(temp, (const char*)value);
                *savedValue = temp;
            }
            else if (strcmp("someOption2", optionName) == 0)
            {
                char* temp;
                temp = (char *)BASEIMPLEMENTATION::gballoc_malloc(strlen((const char*)value) + 1);
                strcpy(temp, (const char*)value);
                *savedValue = temp;
            }
            else
            {
                result2 = HTTPAPI_INVALID_ARG;
            }
        }
    MOCK_METHOD_END(HTTPAPI_RESULT, result2)

    MOCK_STATIC_METHOD_1(, VECTOR_HANDLE, VECTOR_create, size_t, elementSize)
    VECTOR_HANDLE result2;
    currentVECTOR_create_call++;
    if (currentVECTOR_create_call == whenShallVECTOR_create_fail)
    {
        result2 = NULL;
    }
    else
    {
        result2 = BASEIMPLEMENTATION::VECTOR_create(elementSize);
    }
    MOCK_METHOD_END(VECTOR_HANDLE, result2)

    MOCK_STATIC_METHOD_1(, void, VECTOR_destroy, VECTOR_HANDLE, handle)
        BASEIMPLEMENTATION::VECTOR_destroy(handle);
    MOCK_VOID_METHOD_END()

    MOCK_STATIC_METHOD_3(, int, VECTOR_push_back, VECTOR_HANDLE, handle, const void*, elements, size_t, numElements)
    int result2;
    currentVECTOR_push_back_call++;
    if (currentVECTOR_push_back_call == whenShallVECTOR_push_back_fail)
    {
        result2 = __LINE__;
    }
    else
    {
        result2 = BASEIMPLEMENTATION::VECTOR_push_back(handle, elements, numElements);
    }
    MOCK_METHOD_END(int, result2)

    MOCK_STATIC_METHOD_2(, void*, VECTOR_element, VECTOR_HANDLE, handle, size_t, index)
    MOCK_METHOD_END(void*, BASEIMPLEMENTATION::VECTOR_element(handle, index))

    MOCK_STATIC_METHOD_3(, void*, VECTOR_find_if, VECTOR_HANDLE, handle, PREDICATE_FUNCTION, pred, const void*, value)
    MOCK_METHOD_END(void*, BASEIMPLEMENTATION::VECTOR_find_if(handle, pred, value))

    MOCK_STATIC_METHOD_1(, size_t, VECTOR_size, VECTOR_HANDLE, handle)
    MOCK_METHOD_END(size_t, BASEIMPLEMENTATION::VECTOR_size(handle))
};

DECLARE_GLOBAL_MOCK_METHOD_1(CHTTPAPIEXMocks, , void*, gballoc_malloc, size_t, size);
DECLARE_GLOBAL_MOCK_METHOD_2(CHTTPAPIEXMocks, , void*, gballoc_realloc, void*, ptr, size_t, size);
DECLARE_GLOBAL_MOCK_METHOD_1(CHTTPAPIEXMocks, , void, gballoc_free, void*, ptr);

DECLARE_GLOBAL_MOCK_METHOD_0(CHTTPAPIEXMocks, , STRING_HANDLE, STRING_new);
DECLARE_GLOBAL_MOCK_METHOD_1(CHTTPAPIEXMocks, , STRING_HANDLE, STRING_clone, STRING_HANDLE, handle);
DECLARE_GLOBAL_MOCK_METHOD_1(CHTTPAPIEXMocks, , STRING_HANDLE, STRING_construct, const char*, s);

DECLARE_GLOBAL_MOCK_METHOD_1(CHTTPAPIEXMocks, , void, STRING_delete, STRING_HANDLE, s);
DECLARE_GLOBAL_MOCK_METHOD_2(CHTTPAPIEXMocks, , int, STRING_concat, STRING_HANDLE, s1, const char*, s2);
DECLARE_GLOBAL_MOCK_METHOD_2(CHTTPAPIEXMocks, , int, STRING_concat_with_STRING, STRING_HANDLE, s1, STRING_HANDLE, s2);
DECLARE_GLOBAL_MOCK_METHOD_1(CHTTPAPIEXMocks, , int, STRING_empty, STRING_HANDLE, s1);
DECLARE_GLOBAL_MOCK_METHOD_1(CHTTPAPIEXMocks, , const char*, STRING_c_str, STRING_HANDLE, s);

DECLARE_GLOBAL_MOCK_METHOD_0(CHTTPAPIEXMocks, , HTTP_HEADERS_HANDLE, HTTPHeaders_Alloc);
DECLARE_GLOBAL_MOCK_METHOD_1(CHTTPAPIEXMocks, , void, HTTPHeaders_Free, HTTP_HEADERS_HANDLE, httpHeadersHandle);
DECLARE_GLOBAL_MOCK_METHOD_3(CHTTPAPIEXMocks, , HTTP_HEADERS_RESULT, HTTPHeaders_AddHeaderNameValuePair, HTTP_HEADERS_HANDLE, httpHeadersHandle, const char*, name, const char*, value);
DECLARE_GLOBAL_MOCK_METHOD_3(CHTTPAPIEXMocks, , HTTP_HEADERS_RESULT, HTTPHeaders_ReplaceHeaderNameValuePair, HTTP_HEADERS_HANDLE, httpHeadersHandle, const char*, name, const char*, value);

DECLARE_GLOBAL_MOCK_METHOD_0(CHTTPAPIEXMocks, , BUFFER_HANDLE, BUFFER_new);
DECLARE_GLOBAL_MOCK_METHOD_1(CHTTPAPIEXMocks, , void, BUFFER_delete, BUFFER_HANDLE, handle);
DECLARE_GLOBAL_MOCK_METHOD_1(CHTTPAPIEXMocks, , unsigned char*, BUFFER_u_char, BUFFER_HANDLE, handle);
DECLARE_GLOBAL_MOCK_METHOD_1(CHTTPAPIEXMocks, , size_t, BUFFER_length, BUFFER_HANDLE, handle);
DECLARE_GLOBAL_MOCK_METHOD_3(CHTTPAPIEXMocks, , int, BUFFER_build, BUFFER_HANDLE, handle, const unsigned char*, source, size_t, size)

DECLARE_GLOBAL_MOCK_METHOD_0(CHTTPAPIEXMocks, , HTTPAPI_RESULT, HTTPAPI_Init);
DECLARE_GLOBAL_MOCK_METHOD_0(CHTTPAPIEXMocks, , void, HTTPAPI_Deinit);
DECLARE_GLOBAL_MOCK_METHOD_1(CHTTPAPIEXMocks, , HTTP_HANDLE, HTTPAPI_CreateConnection, const char*, hostName);
DECLARE_GLOBAL_MOCK_METHOD_1(CHTTPAPIEXMocks, , void, HTTPAPI_CloseConnection, HTTP_HANDLE, handle);
DECLARE_GLOBAL_MOCK_METHOD_9(CHTTPAPIEXMocks, , HTTPAPI_RESULT, HTTPAPI_ExecuteRequest, HTTP_HANDLE, handle, HTTPAPI_REQUEST_TYPE, requestType, const char*, relativePath,HTTP_HEADERS_HANDLE, httpHeadersHandle, const unsigned char*, content,size_t, contentLength, unsigned int*, statusCode,HTTP_HEADERS_HANDLE, responseHeadersHandle, BUFFER_HANDLE, responseContent);
DECLARE_GLOBAL_MOCK_METHOD_3(CHTTPAPIEXMocks, , HTTPAPI_RESULT, HTTPAPI_SetOption, HTTP_HANDLE, handle, const char*, optionName, const void*, value)
DECLARE_GLOBAL_MOCK_METHOD_3(CHTTPAPIEXMocks, , HTTPAPI_RESULT, HTTPAPI_CloneOption, const char*, optionName, const void*, value, const void**, savedValue);

DECLARE_GLOBAL_MOCK_METHOD_1(CHTTPAPIEXMocks, , VECTOR_HANDLE, VECTOR_create, size_t, elementSize);
DECLARE_GLOBAL_MOCK_METHOD_1(CHTTPAPIEXMocks, , void, VECTOR_destroy, VECTOR_HANDLE, handle);
DECLARE_GLOBAL_MOCK_METHOD_3(CHTTPAPIEXMocks, , int, VECTOR_push_back, VECTOR_HANDLE, handle, const void*, elements, size_t, numElements)
DECLARE_GLOBAL_MOCK_METHOD_2(CHTTPAPIEXMocks, , void*, VECTOR_element, VECTOR_HANDLE, handle, size_t, index)
DECLARE_GLOBAL_MOCK_METHOD_3(CHTTPAPIEXMocks, , void*, VECTOR_find_if, VECTOR_HANDLE, handle, PREDICATE_FUNCTION, pred, const void*, value)
DECLARE_GLOBAL_MOCK_METHOD_1(CHTTPAPIEXMocks, , size_t, VECTOR_size, VECTOR_HANDLE, handle);

static MICROMOCK_MUTEX_HANDLE g_testByTest;
static MICROMOCK_GLOBAL_SEMAPHORE_HANDLE g_dllByDll;


static void createHttpObjects(HTTP_HEADERS_HANDLE* requestHttpHeaders, BUFFER_HANDLE* requestHttpBody, HTTP_HEADERS_HANDLE* responseHttpHeaders, BUFFER_HANDLE* responseHttpBody)
{
    /*assumed to never fail*/
    *requestHttpHeaders = HTTPHeaders_Alloc();
    *requestHttpBody = BUFFER_new();
    BUFFER_build(*requestHttpBody, (const unsigned char*)TEST_BUFFER, TEST_BUFFER_SIZE);
    *responseHttpHeaders = HTTPHeaders_Alloc();
    *responseHttpBody = BUFFER_new();
    if (
        (*requestHttpHeaders == NULL) ||
        (*requestHttpBody == NULL) ||
        (*responseHttpHeaders == NULL)||
        (*responseHttpBody == NULL)
        )
    {
        ASSERT_FAIL("unable to build test prerequisites");
    }
}

static void destroyHttpObjects(HTTP_HEADERS_HANDLE* requestHttpHeaders, BUFFER_HANDLE* requestHttpBody, HTTP_HEADERS_HANDLE* responseHttpHeaders, BUFFER_HANDLE* responseHttpBody)
{
    HTTPHeaders_Free(*requestHttpHeaders);
    *requestHttpHeaders = NULL;
    BUFFER_delete(*requestHttpBody);
    *requestHttpBody = NULL;
    HTTPHeaders_Free(*responseHttpHeaders);
    *responseHttpHeaders = NULL;
    BUFFER_delete(*responseHttpBody);
    *responseHttpBody = NULL;
}

/*this function takes care of setting expected call for httpapiex_executerequest assuming all the parameters are non-null*/
static void setupAllCallBeforeHTTPsequence(CHTTPAPIEXMocks* mocks)
{
    (void)(*mocks);
    /*this is building the host and content-length for the http request headers*/
    STRICT_EXPECTED_CALL((*mocks), BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL((*mocks), STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL((*mocks), HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Host", TEST_HOSTNAME))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL((*mocks), HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Content-Length", TOSTRING(TEST_BUFFER_SIZE)))
        .IgnoreArgument(1);
}

static void setupAllCallForHTTPsequence(CHTTPAPIEXMocks* mocks, const char* relativePath, HTTP_HEADERS_HANDLE requestHttpHeaders, BUFFER_HANDLE requestHttpBody, HTTP_HEADERS_HANDLE responseHttpHeaders, BUFFER_HANDLE responseHttpBody)
{
    (void)(*mocks, requestHttpBody);
    STRICT_EXPECTED_CALL((*mocks), HTTPAPI_Init());

    /*this is getting the hostname for the HTTAPI_connect call)*/
    STRICT_EXPECTED_CALL((*mocks), STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL((*mocks), HTTPAPI_CreateConnection(TEST_HOSTNAME));
    STRICT_EXPECTED_CALL((*mocks), VECTOR_size(IGNORED_PTR_ARG)) /*this is passing the options*/ /*there are none saved in the regular sequences*/
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL((*mocks), BUFFER_length(requestHttpBody));
    STRICT_EXPECTED_CALL((*mocks), BUFFER_length(requestHttpBody)); /*two times because once in here, one in the code under test*/
    size_t requestHttpBodyLength = BUFFER_length(requestHttpBody);

    STRICT_EXPECTED_CALL((*mocks), BUFFER_u_char(requestHttpBody));
    STRICT_EXPECTED_CALL((*mocks), BUFFER_u_char(requestHttpBody)); /*two times because once in here, one in the code under test*/
    const unsigned char* requestHttpBodyContent = BUFFER_u_char(requestHttpBody);



    /*this is getting the buffer content and buffer length to pass to httpapi_executerequest*/
    STRICT_EXPECTED_CALL((*mocks), HTTPAPI_ExecuteRequest(
        IGNORED_PTR_ARG,
        HTTPAPI_REQUEST_PATCH,
        relativePath,
        requestHttpHeaders,
        requestHttpBodyContent, 
        requestHttpBodyLength,
        IGNORED_PTR_ARG,
        responseHttpHeaders,
        responseHttpBody))
        .IgnoreArgument(1)
        .IgnoreArgument(7)
        ;

}

/*every time HttpApi_Execute request is executed several things will be auto-aupdated by the code*/
/*request headers to match the content-length, host to match hostname*/
static void prepareHTTPAPIEX_ExecuteRequest(CHTTPAPIEXMocks* mocks, unsigned int *asGivenyHttpApi, HTTP_HEADERS_HANDLE requestHttpHeaders, HTTP_HEADERS_HANDLE responseHttpHeaders, BUFFER_HANDLE responseHttpBody, HTTPAPI_RESULT resultToBeUsed)
{
    (void)(*mocks);
    /*this is building the host and content-length for the http request headers, this happens every time*/
    STRICT_EXPECTED_CALL((*mocks), BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL((*mocks), STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL((*mocks), HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Host", TEST_HOSTNAME))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL((*mocks), HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Content-Length", TOSTRING(TEST_BUFFER_SIZE)))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL((*mocks), BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL((*mocks), BUFFER_u_char(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL((*mocks), HTTPAPI_ExecuteRequest(
        IGNORED_PTR_ARG,
        HTTPAPI_REQUEST_PATCH,
        TEST_RELATIVE_PATH,
        requestHttpHeaders,
        IGNORED_PTR_ARG,
        TEST_BUFFER_SIZE,
        IGNORED_PTR_ARG,
        responseHttpHeaders,
        responseHttpBody))
        .IgnoreArgument(1)
        .IgnoreArgument(4)
        .IgnoreArgument(5)
        .IgnoreArgument(7)
        .ValidateArgumentBuffer(5, TEST_BUFFER, TEST_BUFFER_SIZE)
        .CopyOutArgumentBuffer(7, asGivenyHttpApi, sizeof(*asGivenyHttpApi))
        .SetReturn(resultToBeUsed);
        ;
}

BEGIN_TEST_SUITE(httpapiex_unittests)

TEST_SUITE_INITIALIZE(TestClassInitialize)
{
    INITIALIZE_MEMORY_DEBUG(g_dllByDll);
    g_testByTest = MicroMockCreateMutex();
    ASSERT_IS_NOT_NULL(g_testByTest);
}

TEST_SUITE_CLEANUP(TestClassCleanup)
{
    MicroMockDestroyMutex(g_testByTest);
    DEINITIALIZE_MEMORY_DEBUG(g_dllByDll);
}

TEST_FUNCTION_INITIALIZE(TestMethodInitialize)
{
    if (!MicroMockAcquireMutex(g_testByTest))
    {
        ASSERT_FAIL("our mutex is ABANDONED. Failure in test framework");
    }

    currentmalloc_call = 0;
    whenShallmalloc_fail = 0;

    currentrealloc_call = 0;
    whenShallrealloc_fail = 0;

    currentHTTPAPI_SaveOption_call = 0;
    whenShallHTTPAPI_SaveOption_fail = 0;

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

    currentBUFFER_new_call = 0;
    whenShallBUFFER_new_fail = 0;

    currentBUFFER_build_call = 0;
    whenShallBUFFER_build_fail = 0;

    currentBUFFER_content_call = 0;
    whenShallBUFFER_content_fail = 0;

    currentBUFFER_size_call = 0;
    whenShallBUFFER_size_fail = 0;
    
    currentHTTPHeaders_Alloc_call = 0;
    whenShallHTTPHeaders_Alloc_fail = 0;

    currentVECTOR_create_call = 0;
    whenShallVECTOR_create_fail = 0;

    currentVECTOR_push_back_call = 0;
    whenShallVECTOR_push_back_fail = 0;

    HTTPAPI_Init_calls = 0;

    currentHTTPAPI_CreateConnection_call = 0;
    for(size_t i=0;i<N_MAX_FAILS;i++) whenShallHTTPAPI_CreateConnection_fail[i] = 0;

    currentHTTPAPI_Init_call = 0;
    for (size_t i = 0; i<N_MAX_FAILS; i++) whenShallHTTPAPI_Init_fail[i] = 0;
}

TEST_FUNCTION_CLEANUP(TestMethodCleanup)
{
    ASSERT_ARE_EQUAL(size_t, 0, HTTPAPI_Init_calls);

    if (!MicroMockReleaseMutex(g_testByTest))
    {
        ASSERT_FAIL("failure in test framework at ReleaseMutex");
    }
}

/*Tests_SRS_HTTPAPIEX_02_001: [If parameter hostName is NULL then HTTPAPIEX_Create shall return NULL.] */
TEST_FUNCTION(HTTPAPIEX_Create_with_NULL_name_fails)
{
    /// arrange

    /// act
    auto result = HTTPAPIEX_Create(NULL);

    /// assert
    ASSERT_ARE_EQUAL(void_ptr, NULL, result);
}

/*Tests_SRS_HTTPAPIEX_02_002: [Parameter hostName shall be saved.] */
/*Tests_SRS_HTTPAPIEX_02_004: [Otherwise, HTTPAPIEX_Create shall return a HTTAPIEX_HANDLE suitable for further calls to the module.]*/
TEST_FUNCTION(HTTPAPIEX_Create_succeeds)
{
    /// arrange
    CHTTPAPIEXMocks mocks;

    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_HOSTNAME));
    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(0))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, VECTOR_create(IGNORED_NUM_ARG))
        .IgnoreArgument(1);

    /// act
    auto result = HTTPAPIEX_Create(TEST_HOSTNAME);

    /// assert
    ASSERT_ARE_NOT_EQUAL(void_ptr, NULL, result);
    mocks.AssertActualAndExpectedCalls();

    ///destroy
    HTTPAPIEX_Destroy(result);
}

/*Tests_SRS_HTTPAPIEX_02_042: [HTTPAPIEX_Destroy shall free all the resources used by HTTAPIEX_HANDLE.] */
TEST_FUNCTION(HTTPAPIEX_Destroy_frees_resources_1) /*this is destroy after created*/
{
    /// arrange
    CHTTPAPIEXMocks mocks;
    auto handle = HTTPAPIEX_Create(TEST_HOSTNAME);
    mocks.ResetAllCalls();

    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG)) /*this is the copy of the hostName*/
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, VECTOR_size(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, VECTOR_destroy(IGNORED_PTR_ARG)) /*these are the options vector*/
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, gballoc_free(handle)); /*this is handle data*/

    /// act
    HTTPAPIEX_Destroy(handle);
    
    /// assert
    mocks.AssertActualAndExpectedCalls();
}

/*Tests_SRS_HTTPAPIEX_02_042: [HTTPAPIEX_Destroy shall free all the resources used by HTTAPIEX_HANDLE.] */
TEST_FUNCTION(HTTPAPIEX_Destroy_frees_resources_2) /*this is destroy after setting options*/
{
    /// arrange
    CHTTPAPIEXMocks mocks;
    auto handle = HTTPAPIEX_Create(TEST_HOSTNAME);
    (void)HTTPAPIEX_SetOption(handle, "notAnOption", "notAValue"); /*this is not something handled*/
    (void)HTTPAPIEX_SetOption(handle, "someOption1", "someValue"); /*this is something handled*/
    mocks.ResetAllCalls();

    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG)) /*this is the copy of the hostName*/
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, VECTOR_size(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, VECTOR_element(IGNORED_PTR_ARG, 0))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*this is "someValue"*/
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*this is "someOption"*/
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, VECTOR_destroy(IGNORED_PTR_ARG)) /*these are the options vector*/
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, gballoc_free(handle)); /*this is handle data*/

    /// act
    HTTPAPIEX_Destroy(handle);

    /// assert
    mocks.AssertActualAndExpectedCalls();
}

/*Tests_SRS_HTTPAPIEX_02_042: [HTTPAPIEX_Destroy shall free all the resources used by HTTAPIEX_HANDLE.] */
TEST_FUNCTION(HTTPAPIEX_Destroy_frees_resources_3) /*this is destroy after having a sequence build*/
{
    /// arrange
    CHTTPAPIEXMocks mocks;
    auto httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);

    unsigned int httpStatusCode;
    HTTP_HEADERS_HANDLE requestHttpHeaders;
    BUFFER_HANDLE requestHttpBody;
    HTTP_HEADERS_HANDLE responseHttpHeaders;
    BUFFER_HANDLE responseHttpBody;
    createHttpObjects(&requestHttpHeaders, &requestHttpBody, &responseHttpHeaders, &responseHttpBody);
    setupAllCallBeforeHTTPsequence(&mocks);
    setupAllCallForHTTPsequence(&mocks, TEST_RELATIVE_PATH, requestHttpHeaders, requestHttpBody, responseHttpHeaders, responseHttpBody);
    (void)HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, requestHttpHeaders, requestHttpBody, &httpStatusCode, responseHttpHeaders, responseHttpBody);
    mocks.ResetAllCalls();

    STRICT_EXPECTED_CALL(mocks, HTTPAPI_CloseConnection(IGNORED_PTR_ARG)) /*closing the conenction*/
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPAPI_Deinit()); /*deinit */
        
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG)) /*this is hostname*/
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, VECTOR_size(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, VECTOR_destroy(IGNORED_PTR_ARG)) /*these are the options vector*/
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, gballoc_free(httpapiexhandle)); /*this is the handle*/

    /// act
    HTTPAPIEX_Destroy(httpapiexhandle);

    ///assert

    mocks.AssertActualAndExpectedCalls();

    ///destroy
    destroyHttpObjects(&requestHttpHeaders, &requestHttpBody, &responseHttpHeaders, &responseHttpBody);
    
}

/*Tests_SRS_HTTPAPIEX_02_005: [If creating the handle fails for any reason, then HTTAPIEX_Create shall return NULL.] */
TEST_FUNCTION(HTTPAPIEX_Create_fails_when_malloc_fails)
{
    /// arrange
    CHTTPAPIEXMocks mocks;

    whenShallmalloc_fail = currentmalloc_call + 1;
    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(0))
        .IgnoreArgument(1);

    /// act
    auto result = HTTPAPIEX_Create(TEST_HOSTNAME);

    /// assert
    ASSERT_ARE_EQUAL(void_ptr, NULL, result);
    mocks.AssertActualAndExpectedCalls();

    ///destroy
}

/*Tests_SRS_HTTPAPIEX_02_003: [If saving the parameter hostName fails for any reason, then HTTPAPIEX_Create shall return NULL.] */
TEST_FUNCTION(HTTPAPIEX_Create_fails_when_STRING_construct_fails)
{
    /// arrange
    CHTTPAPIEXMocks mocks;

    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(0))
        .IgnoreArgument(1);


    whenShallSTRING_construct_fail = currentSTRING_construct_call + 1;
    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_HOSTNAME));

    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    /// act
    auto result = HTTPAPIEX_Create(TEST_HOSTNAME);

    /// assert
    ASSERT_ARE_EQUAL(void_ptr, NULL, result);
    mocks.AssertActualAndExpectedCalls();

    ///destroy
}

/*Tests_SRS_HTTPAPIEX_02_005: [If creating the handle fails for any reason, then HTTAPIEX_Create shall return NULL.] */
TEST_FUNCTION(HTTPAPIEX_Create_fails_when_VECTOR_create_fails)
{
    /// arrange
    CHTTPAPIEXMocks mocks;

    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(0))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_HOSTNAME));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    whenShallVECTOR_create_fail = currentVECTOR_create_call + 1;
    STRICT_EXPECTED_CALL(mocks, VECTOR_create(IGNORED_NUM_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    /// act
    auto result = HTTPAPIEX_Create(TEST_HOSTNAME);

    /// assert
    ASSERT_ARE_EQUAL(void_ptr, NULL, result);
    mocks.AssertActualAndExpectedCalls();

    ///destroy
}


/*Tests_SRS_HTTPAPIEX_02_006: [If parameter handle is NULL then HTTPAPIEX_ExecuteRequest shall fail and return HTTPAPIEX_INVALID_ARG.]*/
TEST_FUNCTION(HTTPAPIEX_ExecuteRequest_with_NULL_handle_fails)
{
    ///arrange
    unsigned int httpStatusCode;

    ///act
    auto result = HTTPAPIEX_ExecuteRequest(NULL, HTTPAPI_REQUEST_POST, TEST_RELATIVE_PATH, TEST_REQUEST_HTTP_HEADERS, TEST_REQUEST_BODY, &httpStatusCode, TEST_RESPONSE_HTTP_HEADERS, TEST_RESPONSE_BODY);

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_INVALID_ARG, result);
}

/*Tests_SRS_HTTPAPIEX_02_007: [If parameter requestType does not indicate a valid request, HTTPAPIEX_ExecuteRequest shall fail and return HTTPAPIEX_INVALID_ARG.] */
TEST_FUNCTION(HTTPAPIEX_ExecuteRequest_fails_with_invalid_request_type)
{
    /// arrange
    CHTTPAPIEXMocks mocks;
    unsigned int httpStatusCode;

    auto httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);
    mocks.ResetAllCalls();

    /// act
    auto result = HTTPAPIEX_ExecuteRequest(httpapiexhandle, (HTTPAPI_REQUEST_TYPE)(COUNT_ARG(HTTPAPI_REQUEST_TYPE_VALUES)), TEST_RELATIVE_PATH, TEST_REQUEST_HTTP_HEADERS, TEST_REQUEST_BODY, &httpStatusCode, TEST_RESPONSE_HTTP_HEADERS, TEST_RESPONSE_BODY);

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_INVALID_ARG, result);
    mocks.AssertActualAndExpectedCalls();

    ///destroy
    HTTPAPIEX_Destroy(httpapiexhandle);
}

/*Tests_SRS_HTTPAPIEX_02_023: [HTTPAPIEX_ExecuteRequest shall try to execute the HTTP call by ensuring the following API call sequence is respected:]*/
TEST_FUNCTION(HTTPAPIEX_ExecuteRequest_happy_path_with_all_non_NULL_parameters)
{
    /// arrange
    CHTTPAPIEXMocks mocks;
    auto httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);

    unsigned int httpStatusCode;
    HTTP_HEADERS_HANDLE requestHttpHeaders;
    BUFFER_HANDLE requestHttpBody;
    HTTP_HEADERS_HANDLE responseHttpHeaders;
    BUFFER_HANDLE responseHttpBody;
    createHttpObjects(&requestHttpHeaders, &requestHttpBody, &responseHttpHeaders, &responseHttpBody);
    mocks.ResetAllCalls();

    setupAllCallBeforeHTTPsequence(&mocks);
    setupAllCallForHTTPsequence(&mocks, TEST_RELATIVE_PATH, requestHttpHeaders, requestHttpBody, responseHttpHeaders, responseHttpBody);

    /// act
    auto result = HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, requestHttpHeaders, requestHttpBody, &httpStatusCode, responseHttpHeaders, responseHttpBody);

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_OK, result);
    mocks.AssertActualAndExpectedCalls();

    ///destroy
    destroyHttpObjects(&requestHttpHeaders, &requestHttpBody, &responseHttpHeaders, &responseHttpBody);
    HTTPAPIEX_Destroy(httpapiexhandle);
}

/*Tests_SRS_HTTPAPIEX_02_008: [If parameter relativePath is NULL then HTTPAPIEX_INVALID_ARG shall not assume a relative path - that is, it will assume an empty path ("").] */
TEST_FUNCTION(HTTPAPIEX_ExecuteRequest_with_NULL_relativePath_uses_empty)
{
    /// arrange
    CHTTPAPIEXMocks mocks;
    auto httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);

    unsigned int httpStatusCode;
    HTTP_HEADERS_HANDLE requestHttpHeaders;
    BUFFER_HANDLE requestHttpBody;
    HTTP_HEADERS_HANDLE responseHttpHeaders;
    BUFFER_HANDLE responseHttpBody;
    createHttpObjects(&requestHttpHeaders, &requestHttpBody, &responseHttpHeaders, &responseHttpBody);
    mocks.ResetAllCalls();

    setupAllCallBeforeHTTPsequence(&mocks);
    setupAllCallForHTTPsequence(&mocks, "", requestHttpHeaders, requestHttpBody, responseHttpHeaders, responseHttpBody);

    /// act
    auto result = HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, NULL, requestHttpHeaders, requestHttpBody, &httpStatusCode, responseHttpHeaders, responseHttpBody);

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_OK, result);
    mocks.AssertActualAndExpectedCalls();

    ///destroy
    destroyHttpObjects(&requestHttpHeaders, &requestHttpBody, &responseHttpHeaders, &responseHttpBody);
    HTTPAPIEX_Destroy(httpapiexhandle);
}

/*Tests_SRS_HTTPAPIEX_02_009: [If parameter requestHttpHeadersHandle is NULL then HTTPAPIEX_ExecuteRequest shall allocate a temporary internal instance of HTTPHEADERS, shall add to that instance the following headers
	Host:{hostname} - as it was indicated by the call to HTTPAPIEX_Create API call
	Content-Length:the size of the requestContent parameter, and use this instance to all the subsequent calls to HTTPAPI_ExecuteRequest as parameter httpHeadersHandle.] 
*/
/*Tests_SRS_HTTPAPIEX_02_013: [If requestContent is NULL then HTTPAPIEX_ExecuteRequest shall behave as if a buffer of zero size would have been used, that is, it shall call HTTPAPI_ExecuteRequest with parameter content = NULL and contentLength = 0.]*/
TEST_FUNCTION(HTTPAPIEX_ExecuteRequest_with_NULL_request_headers_and_NULL_requestBody_succeeds)
{
    /// arrange
    CHTTPAPIEXMocks mocks;
    auto httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);

    unsigned int httpStatusCode;
    HTTP_HEADERS_HANDLE requestHttpHeaders;
    BUFFER_HANDLE requestHttpBody;
    HTTP_HEADERS_HANDLE responseHttpHeaders;
    BUFFER_HANDLE responseHttpBody;
    createHttpObjects(&requestHttpHeaders, &requestHttpBody, &responseHttpHeaders, &responseHttpBody);
    mocks.ResetAllCalls();

    STRICT_EXPECTED_CALL(mocks, BUFFER_new()); /*because it makes a fake buffer*/
    STRICT_EXPECTED_CALL(mocks, BUFFER_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc()); /*because it makes fakes request headers*/
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    /*this is building the host and content-length for the http request headers*/
    STRICT_EXPECTED_CALL(mocks, BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Host", TEST_HOSTNAME))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Content-Length", "0"))
        .IgnoreArgument(1);

    /*this is getting the buffer content and buffer length to pass to httpapi_executerequest*/
    STRICT_EXPECTED_CALL(mocks, HTTPAPI_Init());
    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPAPI_CreateConnection(TEST_HOSTNAME));
    STRICT_EXPECTED_CALL(mocks, VECTOR_size(IGNORED_PTR_ARG)) /*this is passing the options*/ /*there are none saved in the regular sequences*/
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, BUFFER_u_char(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, HTTPAPI_ExecuteRequest(
        IGNORED_PTR_ARG,
        HTTPAPI_REQUEST_PATCH,
        TEST_RELATIVE_PATH,
        IGNORED_PTR_ARG,
        IGNORED_PTR_ARG,
        0,
        IGNORED_PTR_ARG,
        responseHttpHeaders,
        responseHttpBody))
        .IgnoreArgument(1)
        .IgnoreArgument(4)
        .IgnoreArgument(5)
        .IgnoreArgument(7)
        ;

    /// act
    auto result = HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, NULL, NULL, &httpStatusCode, responseHttpHeaders, responseHttpBody);

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_OK, result);
    mocks.AssertActualAndExpectedCalls();

    ///destroy
    destroyHttpObjects(&requestHttpHeaders, &requestHttpBody, &responseHttpHeaders, &responseHttpBody);
    HTTPAPIEX_Destroy(httpapiexhandle);
}

/*Tests_SRS_HTTPAPIEX_02_010: [If any of the operations in SRS_HTTAPIEX_02_009 fails, then HTTPAPIEX_ExecuteRequest shall return HTTPAPIEX_ERROR.]*/
TEST_FUNCTION(HTTPAPIEX_ExecuteRequest_with_NULL_request_headers_and_NULL_requestBody_fails_when_HTTPHeaders_ReplaceHeaderNameValuePair_fails)
{
    /// arrange
    CHTTPAPIEXMocks mocks;
    auto httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);

    unsigned int httpStatusCode;
    HTTP_HEADERS_HANDLE requestHttpHeaders;
    BUFFER_HANDLE requestHttpBody;
    HTTP_HEADERS_HANDLE responseHttpHeaders;
    BUFFER_HANDLE responseHttpBody;
    createHttpObjects(&requestHttpHeaders, &requestHttpBody, &responseHttpHeaders, &responseHttpBody);
    mocks.ResetAllCalls();

    STRICT_EXPECTED_CALL(mocks, BUFFER_new()); /*because it makes a fake buffer*/
    STRICT_EXPECTED_CALL(mocks, BUFFER_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc()); /*because it makes fakes request headers*/
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    /*this is building the host and content-length for the http request headers*/
    STRICT_EXPECTED_CALL(mocks, BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Host", TEST_HOSTNAME))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Content-Length", "0"))
        .IgnoreArgument(1)
        .SetReturn(HTTP_HEADERS_ERROR);

    /// act
    auto result = HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, NULL, NULL, &httpStatusCode, responseHttpHeaders, responseHttpBody);

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_ERROR, result);
    mocks.AssertActualAndExpectedCalls();

    ///destroy
    destroyHttpObjects(&requestHttpHeaders, &requestHttpBody, &responseHttpHeaders, &responseHttpBody);
    HTTPAPIEX_Destroy(httpapiexhandle);
}

/*Tests_SRS_HTTPAPIEX_02_010: [If any of the operations in SRS_HTTAPIEX_02_009 fails, then HTTPAPIEX_ExecuteRequest shall return HTTPAPIEX_ERROR.]*/
TEST_FUNCTION(HTTPAPIEX_ExecuteRequest_with_NULL_request_headers_and_NULL_requestBody_fails_when_HTTPHeaders_ReplaceHeaderNameValuePair_fails_2)
{
    /// arrange
    CHTTPAPIEXMocks mocks;
    auto httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);

    unsigned int httpStatusCode;
    HTTP_HEADERS_HANDLE requestHttpHeaders;
    BUFFER_HANDLE requestHttpBody;
    HTTP_HEADERS_HANDLE responseHttpHeaders;
    BUFFER_HANDLE responseHttpBody;
    createHttpObjects(&requestHttpHeaders, &requestHttpBody, &responseHttpHeaders, &responseHttpBody);
    mocks.ResetAllCalls();

    STRICT_EXPECTED_CALL(mocks, BUFFER_new()); /*because it makes a fake buffer*/
    STRICT_EXPECTED_CALL(mocks, BUFFER_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc()); /*because it makes fakes request headers*/
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    /*this is building the host and content-length for the http request headers*/
    STRICT_EXPECTED_CALL(mocks, BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Host", TEST_HOSTNAME))
        .IgnoreArgument(1)
        .SetReturn(HTTP_HEADERS_ERROR);

    /// act
    auto result = HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, NULL, NULL, &httpStatusCode, responseHttpHeaders, responseHttpBody);

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_ERROR, result);
    mocks.AssertActualAndExpectedCalls();

    ///destroy
    destroyHttpObjects(&requestHttpHeaders, &requestHttpBody, &responseHttpHeaders, &responseHttpBody);
    HTTPAPIEX_Destroy(httpapiexhandle);
}

/*Tests_SRS_HTTPAPIEX_02_010: [If any of the operations in SRS_HTTAPIEX_02_009 fails, then HTTPAPIEX_ExecuteRequest shall return HTTPAPIEX_ERROR.]*/
TEST_FUNCTION(HTTPAPIEX_ExecuteRequest_with_NULL_request_headers_and_NULL_requestBody_fails_when_HTTPHeaders_ReplaceHeaderNameValuePair_fails_3)
{
    /// arrange
    CHTTPAPIEXMocks mocks;
    auto httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);

    unsigned int httpStatusCode;
    HTTP_HEADERS_HANDLE requestHttpHeaders;
    BUFFER_HANDLE requestHttpBody;
    HTTP_HEADERS_HANDLE responseHttpHeaders;
    BUFFER_HANDLE responseHttpBody;
    createHttpObjects(&requestHttpHeaders, &requestHttpBody, &responseHttpHeaders, &responseHttpBody);
    mocks.ResetAllCalls();

    STRICT_EXPECTED_CALL(mocks, BUFFER_new()); /*because it makes a fake buffer*/
    STRICT_EXPECTED_CALL(mocks, BUFFER_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    whenShallHTTPHeaders_Alloc_fail = currentHTTPHeaders_Alloc_call + 1;
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc()); /*because it makes fakes request headers*/

    /// act
    auto result = HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, NULL, NULL, &httpStatusCode, responseHttpHeaders, responseHttpBody);

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_ERROR, result);
    mocks.AssertActualAndExpectedCalls();

    ///destroy
    destroyHttpObjects(&requestHttpHeaders, &requestHttpBody, &responseHttpHeaders, &responseHttpBody);
    HTTPAPIEX_Destroy(httpapiexhandle);
}

/*Tests_SRS_HTTPAPIEX_02_011: [If parameter requestHttpHeadersHandle is not NULL then HTTPAPIEX_ExecuteRequest shall create or update the following headers of the request:
	Host:{hostname}
	Content-Length:the size of the requestContent parameter, and shall use the so constructed HTTPHEADERS object to all calls to HTTPAPI_ExecuteRequest as parameter httpHeadersHandle.]
*/
TEST_FUNCTION(HTTPAPIEX_ExecuteRequest_with_non_NULL_request_headers_and_NULL_requestBody_succeeds)
{
    /// arrange
    CHTTPAPIEXMocks mocks;
    auto httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);

    unsigned int httpStatusCode;
    HTTP_HEADERS_HANDLE requestHttpHeaders;
    BUFFER_HANDLE requestHttpBody;
    HTTP_HEADERS_HANDLE responseHttpHeaders;
    BUFFER_HANDLE responseHttpBody;
    createHttpObjects(&requestHttpHeaders, &requestHttpBody, &responseHttpHeaders, &responseHttpBody);
    mocks.ResetAllCalls();

    STRICT_EXPECTED_CALL(mocks, BUFFER_new()); /*because it makes a fake buffer*/
    STRICT_EXPECTED_CALL(mocks, BUFFER_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    /*this is building the host and content-length for the http request headers*/
    STRICT_EXPECTED_CALL(mocks, BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Host", TEST_HOSTNAME))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Content-Length", "0"))
        .IgnoreArgument(1);

    /*this is getting the buffer content and buffer length to pass to httpapi_executerequest*/
    STRICT_EXPECTED_CALL(mocks, HTTPAPI_Init());
    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPAPI_CreateConnection(TEST_HOSTNAME));
    STRICT_EXPECTED_CALL(mocks, VECTOR_size(IGNORED_PTR_ARG)) /*this is passing the options*/ /*there are none saved in the regular sequences*/
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, BUFFER_u_char(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, HTTPAPI_ExecuteRequest(
        IGNORED_PTR_ARG,
        HTTPAPI_REQUEST_PATCH,
        TEST_RELATIVE_PATH,
        requestHttpHeaders,
        IGNORED_PTR_ARG,
        0,
        IGNORED_PTR_ARG,
        responseHttpHeaders,
        responseHttpBody))
        .IgnoreArgument(1)
        .IgnoreArgument(4)
        .IgnoreArgument(5)
        .IgnoreArgument(7)
        ;

    /// act
    auto result = HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, requestHttpHeaders, NULL, &httpStatusCode, responseHttpHeaders, responseHttpBody);

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_OK, result);
    mocks.AssertActualAndExpectedCalls();

    ///destroy
    destroyHttpObjects(&requestHttpHeaders, &requestHttpBody, &responseHttpHeaders, &responseHttpBody);
    HTTPAPIEX_Destroy(httpapiexhandle);
}

/*Tests_SRS_HTTPAPIEX_02_012: [If any of the operations required for SRS_HTTAPIEX_02_011 fails, then HTTPAPIEX_ExecuteRequest shall return HTTPAPIEX_ERROR.] */
TEST_FUNCTION(HTTPAPIEX_ExecuteRequest_with_non_NULL_request_headers_and_NULL_requestBody_fails_when_HTTPHeaders_ReplaceHeaderNameValuePair_fails_1)
{
    /// arrange
    CHTTPAPIEXMocks mocks;
    auto httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);

    unsigned int httpStatusCode;
    HTTP_HEADERS_HANDLE requestHttpHeaders;
    BUFFER_HANDLE requestHttpBody;
    HTTP_HEADERS_HANDLE responseHttpHeaders;
    BUFFER_HANDLE responseHttpBody;
    createHttpObjects(&requestHttpHeaders, &requestHttpBody, &responseHttpHeaders, &responseHttpBody);
    mocks.ResetAllCalls();

    STRICT_EXPECTED_CALL(mocks, BUFFER_new()); /*because it makes a fake buffer*/
    STRICT_EXPECTED_CALL(mocks, BUFFER_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    /*this is building the host and content-length for the http request headers*/
    STRICT_EXPECTED_CALL(mocks, BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Host", TEST_HOSTNAME))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Content-Length", "0"))
        .IgnoreArgument(1)
        .SetReturn(HTTP_HEADERS_ERROR);

    /// act
    auto result = HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, requestHttpHeaders, NULL, &httpStatusCode, responseHttpHeaders, responseHttpBody);

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_ERROR, result);
    mocks.AssertActualAndExpectedCalls();

    ///destroy
    destroyHttpObjects(&requestHttpHeaders, &requestHttpBody, &responseHttpHeaders, &responseHttpBody);
    HTTPAPIEX_Destroy(httpapiexhandle);
}

/*Tests_SRS_HTTPAPIEX_02_012: [If any of the operations required for SRS_HTTAPIEX_02_011 fails, then HTTPAPIEX_ExecuteRequest shall return HTTPAPIEX_ERROR.] */
TEST_FUNCTION(HTTPAPIEX_ExecuteRequest_with_non_NULL_request_headers_and_NULL_requestBody_fails_when_HTTPHeaders_ReplaceHeaderNameValuePair_fails_2)
{
    /// arrange
    CHTTPAPIEXMocks mocks;
    auto httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);

    unsigned int httpStatusCode;
    HTTP_HEADERS_HANDLE requestHttpHeaders;
    BUFFER_HANDLE requestHttpBody;
    HTTP_HEADERS_HANDLE responseHttpHeaders;
    BUFFER_HANDLE responseHttpBody;
    createHttpObjects(&requestHttpHeaders, &requestHttpBody, &responseHttpHeaders, &responseHttpBody);
    mocks.ResetAllCalls();

    STRICT_EXPECTED_CALL(mocks, BUFFER_new()); /*because it makes a fake buffer*/
    STRICT_EXPECTED_CALL(mocks, BUFFER_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    /*this is building the host and content-length for the http request headers*/
    STRICT_EXPECTED_CALL(mocks, BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Host", TEST_HOSTNAME))
        .IgnoreArgument(1)
        .SetReturn(HTTP_HEADERS_ERROR);

    /// act
    auto result = HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, requestHttpHeaders, NULL, &httpStatusCode, responseHttpHeaders, responseHttpBody);

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_ERROR, result);
    mocks.AssertActualAndExpectedCalls();

    ///destroy
    destroyHttpObjects(&requestHttpHeaders, &requestHttpBody, &responseHttpHeaders, &responseHttpBody);
    HTTPAPIEX_Destroy(httpapiexhandle);
}

/*Tests_SRS_HTTPAPIEX_02_012: [If any of the operations required for SRS_HTTAPIEX_02_011 fails, then HTTPAPIEX_ExecuteRequest shall return HTTPAPIEX_ERROR.] */
TEST_FUNCTION(HTTPAPIEX_ExecuteRequest_with_non_NULL_request_headers_and_NULL_requestBody_fails_when_fake_requestbody_creation_fails)
{
    /// arrange
    CHTTPAPIEXMocks mocks;
    auto httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);

    unsigned int httpStatusCode;
    HTTP_HEADERS_HANDLE requestHttpHeaders;
    BUFFER_HANDLE requestHttpBody;
    HTTP_HEADERS_HANDLE responseHttpHeaders;
    BUFFER_HANDLE responseHttpBody;
    createHttpObjects(&requestHttpHeaders, &requestHttpBody, &responseHttpHeaders, &responseHttpBody);
    mocks.ResetAllCalls();

    whenShallBUFFER_new_fail = currentBUFFER_new_call + 1;
    STRICT_EXPECTED_CALL(mocks, BUFFER_new()); /*because it makes a fake buffer*/

    /// act
    auto result = HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, requestHttpHeaders, NULL, &httpStatusCode, responseHttpHeaders, responseHttpBody);

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_ERROR, result);
    mocks.AssertActualAndExpectedCalls();

    ///destroy
    destroyHttpObjects(&requestHttpHeaders, &requestHttpBody, &responseHttpHeaders, &responseHttpBody);
    HTTPAPIEX_Destroy(httpapiexhandle);
}

/*Tests_SRS_HTTPAPIEX_02_014: [If requestContent is not NULL then its content and its size shall be used for parameters content and contentLength of HTTPAPI_ExecuteRequest.] */
TEST_FUNCTION(HTTPAPIEX_ExecuteRequest_with_non_NULL_request_headers_and_non_NULL_requestBody_succeeds)
{
    /// arrange
    CHTTPAPIEXMocks mocks;
    auto httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);

    unsigned int httpStatusCode;
    HTTP_HEADERS_HANDLE requestHttpHeaders;
    BUFFER_HANDLE requestHttpBody;
    HTTP_HEADERS_HANDLE responseHttpHeaders;
    BUFFER_HANDLE responseHttpBody;
    createHttpObjects(&requestHttpHeaders, &requestHttpBody, &responseHttpHeaders, &responseHttpBody);
    mocks.ResetAllCalls();

    /*this is building the host and content-length for the http request headers*/
    STRICT_EXPECTED_CALL(mocks, BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Host", TEST_HOSTNAME))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Content-Length", TOSTRING(TEST_BUFFER_SIZE)))
        .IgnoreArgument(1);

    /*this is getting the buffer content and buffer length to pass to httpapi_executerequest*/
    STRICT_EXPECTED_CALL(mocks, HTTPAPI_Init());
    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPAPI_CreateConnection(TEST_HOSTNAME));
    STRICT_EXPECTED_CALL(mocks, VECTOR_size(IGNORED_PTR_ARG)) /*this is passing the options*/ /*there are none saved in the regular sequences*/
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, BUFFER_u_char(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, HTTPAPI_ExecuteRequest(
        IGNORED_PTR_ARG,
        HTTPAPI_REQUEST_PATCH,
        TEST_RELATIVE_PATH,
        requestHttpHeaders,
        IGNORED_PTR_ARG,
        TEST_BUFFER_SIZE,
        IGNORED_PTR_ARG,
        responseHttpHeaders,
        responseHttpBody))
        .IgnoreArgument(1)
        .IgnoreArgument(4)
        .IgnoreArgument(5)
        .IgnoreArgument(7)
        .ValidateArgumentBuffer(5, TEST_BUFFER, TEST_BUFFER_SIZE)
        ;

    /// act
    auto result = HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, requestHttpHeaders, requestHttpBody, &httpStatusCode, responseHttpHeaders, responseHttpBody);

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_OK, result);
    mocks.AssertActualAndExpectedCalls();

    ///destroy
    destroyHttpObjects(&requestHttpHeaders, &requestHttpBody, &responseHttpHeaders, &responseHttpBody);
    HTTPAPIEX_Destroy(httpapiexhandle);
}

/*Tests_SRS_HTTPAPIEX_02_014: [If requestContent is not NULL then its content and its size shall be used for parameters content and contentLength of HTTPAPI_ExecuteRequest.] */
TEST_FUNCTION(HTTPAPIEX_ExecuteRequest_with_non_NULL_request_headers_and_non_NULL_requestBody_fails_when_HTTPHeaders_ReplaceHeaderNameValuePair_fails_1)
{
    /// arrange
    CHTTPAPIEXMocks mocks;
    auto httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);

    unsigned int httpStatusCode;
    HTTP_HEADERS_HANDLE requestHttpHeaders;
    BUFFER_HANDLE requestHttpBody;
    HTTP_HEADERS_HANDLE responseHttpHeaders;
    BUFFER_HANDLE responseHttpBody;
    createHttpObjects(&requestHttpHeaders, &requestHttpBody, &responseHttpHeaders, &responseHttpBody);
    mocks.ResetAllCalls();

    /*this is building the host and content-length for the http request headers*/
    STRICT_EXPECTED_CALL(mocks, BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Host", TEST_HOSTNAME))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Content-Length", TOSTRING(TEST_BUFFER_SIZE)))
        .IgnoreArgument(1)
        .SetReturn(HTTP_HEADERS_ERROR);

    /// act
    auto result = HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, requestHttpHeaders, requestHttpBody, &httpStatusCode, responseHttpHeaders, responseHttpBody);

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_ERROR, result);
    mocks.AssertActualAndExpectedCalls();

    ///destroy
    destroyHttpObjects(&requestHttpHeaders, &requestHttpBody, &responseHttpHeaders, &responseHttpBody);
    HTTPAPIEX_Destroy(httpapiexhandle);
}

/*Tests_SRS_HTTPAPIEX_02_014: [If requestContent is not NULL then its content and its size shall be used for parameters content and contentLength of HTTPAPI_ExecuteRequest.] */
TEST_FUNCTION(HTTPAPIEX_ExecuteRequest_with_non_NULL_request_headers_and_non_NULL_requestBody_fails_when_HTTPHeaders_ReplaceHeaderNameValuePair_fails_2)
{
    /// arrange
    CHTTPAPIEXMocks mocks;
    auto httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);

    unsigned int httpStatusCode;
    HTTP_HEADERS_HANDLE requestHttpHeaders;
    BUFFER_HANDLE requestHttpBody;
    HTTP_HEADERS_HANDLE responseHttpHeaders;
    BUFFER_HANDLE responseHttpBody;
    createHttpObjects(&requestHttpHeaders, &requestHttpBody, &responseHttpHeaders, &responseHttpBody);
    mocks.ResetAllCalls();

    /*this is building the host and content-length for the http request headers*/
    STRICT_EXPECTED_CALL(mocks, BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Host", TEST_HOSTNAME))
        .IgnoreArgument(1)
        .SetReturn(HTTP_HEADERS_ERROR);

    /// act
    auto result = HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, requestHttpHeaders, requestHttpBody, &httpStatusCode, responseHttpHeaders, responseHttpBody);

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_ERROR, result);
    mocks.AssertActualAndExpectedCalls();

    ///destroy
    destroyHttpObjects(&requestHttpHeaders, &requestHttpBody, &responseHttpHeaders, &responseHttpBody);
    HTTPAPIEX_Destroy(httpapiexhandle);
}

/*Tests_SRS_HTTPAPIEX_02_015: [If statusCode is NULL then HTTPAPIEX_ExecuteRequest shall not write in statusCode the HTTP status code, and it will use a temporary internal int for parameter statusCode to the calls of HTTPAPI_ExecuteRequest.] */
TEST_FUNCTION(HTTPAPIEX_ExecuteRequest_with_NULL_statusCode_succeeds)
{
    /// arrange
    CHTTPAPIEXMocks mocks;
    auto httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);

    HTTP_HEADERS_HANDLE requestHttpHeaders;
    BUFFER_HANDLE requestHttpBody;
    HTTP_HEADERS_HANDLE responseHttpHeaders;
    BUFFER_HANDLE responseHttpBody;
    createHttpObjects(&requestHttpHeaders, &requestHttpBody, &responseHttpHeaders, &responseHttpBody);
    mocks.ResetAllCalls();

    /*this is building the host and content-length for the http request headers*/
    STRICT_EXPECTED_CALL(mocks, BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Host", TEST_HOSTNAME))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Content-Length", TOSTRING(TEST_BUFFER_SIZE)))
        .IgnoreArgument(1);

    /*this is getting the buffer content and buffer length to pass to httpapi_executerequest*/
    STRICT_EXPECTED_CALL(mocks, HTTPAPI_Init());
    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPAPI_CreateConnection(TEST_HOSTNAME));
    STRICT_EXPECTED_CALL(mocks, VECTOR_size(IGNORED_PTR_ARG)) /*this is passing the options*/ /*there are none saved in the regular sequences*/
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, BUFFER_u_char(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, HTTPAPI_ExecuteRequest(
        IGNORED_PTR_ARG,
        HTTPAPI_REQUEST_PATCH,
        TEST_RELATIVE_PATH,
        requestHttpHeaders,
        IGNORED_PTR_ARG,
        TEST_BUFFER_SIZE,
        IGNORED_PTR_ARG,
        responseHttpHeaders,
        responseHttpBody))
        .IgnoreArgument(1)
        .IgnoreArgument(4)
        .IgnoreArgument(5)
        .IgnoreArgument(7)
        .ValidateArgumentBuffer(5, TEST_BUFFER, TEST_BUFFER_SIZE)
        ;

    /// act
    auto result = HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, requestHttpHeaders, requestHttpBody, NULL, responseHttpHeaders, responseHttpBody);

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_OK, result);
    mocks.AssertActualAndExpectedCalls();

    ///destroy
    destroyHttpObjects(&requestHttpHeaders, &requestHttpBody, &responseHttpHeaders, &responseHttpBody);
    HTTPAPIEX_Destroy(httpapiexhandle);
}

/*Tests_SRS_HTTPAPIEX_02_016: [If statusCode is not NULL then If statusCode is NULL then HTTPAPIEX_ExecuteRequest shall use it for parameter statusCode to the calls of HTTPAPI_ExecuteRequest.] */
TEST_FUNCTION(HTTPAPIEX_ExecuteRequest_with_non_NULL_statusCode_returns_that_code_to_the_caller_suceeds)
{
    /// arrange
    CHTTPAPIEXMocks mocks;
    auto httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);

    unsigned int httpStatusCode;
    unsigned int asGivenyHttpApi = 4567;
    HTTP_HEADERS_HANDLE requestHttpHeaders;
    BUFFER_HANDLE requestHttpBody;
    HTTP_HEADERS_HANDLE responseHttpHeaders;
    BUFFER_HANDLE responseHttpBody;
    createHttpObjects(&requestHttpHeaders, &requestHttpBody, &responseHttpHeaders, &responseHttpBody);
    mocks.ResetAllCalls();

    /*this is building the host and content-length for the http request headers*/
    STRICT_EXPECTED_CALL(mocks, BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Host", TEST_HOSTNAME))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Content-Length", TOSTRING(TEST_BUFFER_SIZE)))
        .IgnoreArgument(1);

    /*this is getting the buffer content and buffer length to pass to httpapi_executerequest*/
    STRICT_EXPECTED_CALL(mocks, HTTPAPI_Init());
    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPAPI_CreateConnection(TEST_HOSTNAME));
    STRICT_EXPECTED_CALL(mocks, VECTOR_size(IGNORED_PTR_ARG)) /*this is passing the options*/ /*there are none saved in the regular sequences*/
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, BUFFER_u_char(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, HTTPAPI_ExecuteRequest(
        IGNORED_PTR_ARG,
        HTTPAPI_REQUEST_PATCH,
        TEST_RELATIVE_PATH,
        requestHttpHeaders,
        IGNORED_PTR_ARG,
        TEST_BUFFER_SIZE,
        IGNORED_PTR_ARG,
        responseHttpHeaders,
        responseHttpBody))
        .IgnoreArgument(1)
        .IgnoreArgument(4)
        .IgnoreArgument(5)
        .IgnoreArgument(7)
        .ValidateArgumentBuffer(5, TEST_BUFFER, TEST_BUFFER_SIZE)
        .CopyOutArgumentBuffer(7, &asGivenyHttpApi, sizeof(asGivenyHttpApi))
        ;

    /// act
    auto result = HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, requestHttpHeaders, requestHttpBody, &httpStatusCode, responseHttpHeaders, responseHttpBody);

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_OK, result);
    ASSERT_ARE_EQUAL(int, 4567, (int)httpStatusCode);
    mocks.AssertActualAndExpectedCalls();

    ///destroy
    destroyHttpObjects(&requestHttpHeaders, &requestHttpBody, &responseHttpHeaders, &responseHttpBody);
    HTTPAPIEX_Destroy(httpapiexhandle);
}


TEST_FUNCTION(HTTPAPIEX_ExecuteRequest_cleans_when_originalhttpbody_is_NULL_and_response_headers_fails)
{
    /// arrange
    CHTTPAPIEXMocks mocks;
    auto httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);

    unsigned int httpStatusCode;
    HTTP_HEADERS_HANDLE requestHttpHeaders;
    BUFFER_HANDLE requestHttpBody;
    HTTP_HEADERS_HANDLE responseHttpHeaders;
    BUFFER_HANDLE responseHttpBody;
    createHttpObjects(&requestHttpHeaders, &requestHttpBody, &responseHttpHeaders, &responseHttpBody);
    mocks.ResetAllCalls();

    STRICT_EXPECTED_CALL(mocks, BUFFER_new()); /*because it makes a fake buffer*/
    STRICT_EXPECTED_CALL(mocks, BUFFER_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc());
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    /*this is building the host and content-length for the http request headers*/
    STRICT_EXPECTED_CALL(mocks, BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Host", TEST_HOSTNAME))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Content-Length", "0"))
        .IgnoreArgument(1);

    /*Becauyse it is creating fake response headers*/
    whenShallHTTPHeaders_Alloc_fail = currentHTTPHeaders_Alloc_call + 2;
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc());

    /// act
    auto result = HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, NULL, NULL, &httpStatusCode, NULL, responseHttpBody);

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_ERROR, result);
    mocks.AssertActualAndExpectedCalls();

    ///destroy
    destroyHttpObjects(&requestHttpHeaders, &requestHttpBody, &responseHttpHeaders, &responseHttpBody);
    HTTPAPIEX_Destroy(httpapiexhandle);
}

TEST_FUNCTION(HTTPAPIEX_ExecuteRequest_cleans_when_responsehttpbody_is_NULL_and_creating_a_fake_one_fails)
{
    /// arrange
    CHTTPAPIEXMocks mocks;
    auto httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);

    unsigned int httpStatusCode;
    HTTP_HEADERS_HANDLE requestHttpHeaders;
    BUFFER_HANDLE requestHttpBody;
    HTTP_HEADERS_HANDLE responseHttpHeaders;
    BUFFER_HANDLE responseHttpBody;
    createHttpObjects(&requestHttpHeaders, &requestHttpBody, &responseHttpHeaders, &responseHttpBody);
    mocks.ResetAllCalls();

    STRICT_EXPECTED_CALL(mocks, BUFFER_new()); /*because it makes a fake buffer*/
    STRICT_EXPECTED_CALL(mocks, BUFFER_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc());
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    /*this is building the host and content-length for the http request headers*/
    STRICT_EXPECTED_CALL(mocks, BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Host", TEST_HOSTNAME))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Content-Length", "0"))
        .IgnoreArgument(1);

    /*Becauyse it is creating fake response headers*/
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc());
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    whenShallBUFFER_new_fail = currentBUFFER_new_call + 2;
    STRICT_EXPECTED_CALL(mocks, BUFFER_new()); /*because it makes a fake buffer*/

    /// act
    auto result = HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, NULL, NULL, &httpStatusCode, NULL, NULL);

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_ERROR, result);
    mocks.AssertActualAndExpectedCalls();

    ///destroy
    destroyHttpObjects(&requestHttpHeaders, &requestHttpBody, &responseHttpHeaders, &responseHttpBody);
    HTTPAPIEX_Destroy(httpapiexhandle);
}

/*Tests_SRS_HTTPAPIEX_02_017: [If responseHeaders handle is NULL then HTTPAPIEX_ExecuteRequest shall create a temporary internal instance of HTTPHEADERS object and use that for responseHeaders parameter of HTTPAPI_ExecuteRequest call.] */
TEST_FUNCTION(HTTPAPIEX_ExecuteRequest_with_NULL_response_headers_suceeds)
{
    /// arrange
    CHTTPAPIEXMocks mocks;
    auto httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);

    unsigned int httpStatusCode;
    unsigned int asGivenyHttpApi = 4567;
    HTTP_HEADERS_HANDLE requestHttpHeaders;
    BUFFER_HANDLE requestHttpBody;
    HTTP_HEADERS_HANDLE responseHttpHeaders;
    BUFFER_HANDLE responseHttpBody;
    createHttpObjects(&requestHttpHeaders, &requestHttpBody, &responseHttpHeaders, &responseHttpBody);
    mocks.ResetAllCalls();

    /*this is building the host and content-length for the http request headers*/
    STRICT_EXPECTED_CALL(mocks, BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Host", TEST_HOSTNAME))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Content-Length", TOSTRING(TEST_BUFFER_SIZE)))
        .IgnoreArgument(1);

    /*Becauyse it is creating fake response headers*/
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc());
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    /*this is getting the buffer content and buffer length to pass to httpapi_executerequest*/
    STRICT_EXPECTED_CALL(mocks, HTTPAPI_Init());
    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPAPI_CreateConnection(TEST_HOSTNAME));
    STRICT_EXPECTED_CALL(mocks, VECTOR_size(IGNORED_PTR_ARG)) /*this is passing the options*/ /*there are none saved in the regular sequences*/
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, BUFFER_u_char(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, HTTPAPI_ExecuteRequest(
        IGNORED_PTR_ARG,
        HTTPAPI_REQUEST_PATCH,
        TEST_RELATIVE_PATH,
        requestHttpHeaders,
        IGNORED_PTR_ARG,
        TEST_BUFFER_SIZE,
        IGNORED_PTR_ARG,
        IGNORED_PTR_ARG,
        responseHttpBody))
        .IgnoreArgument(1)
        .IgnoreArgument(4)
        .IgnoreArgument(5)
        .IgnoreArgument(7)
        .IgnoreArgument(8)
        .ValidateArgumentBuffer(5, TEST_BUFFER, TEST_BUFFER_SIZE)
        .CopyOutArgumentBuffer(7, &asGivenyHttpApi, sizeof(asGivenyHttpApi))
        ;

    /// act
    auto result = HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, requestHttpHeaders, requestHttpBody, &httpStatusCode, NULL, responseHttpBody);

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_OK, result);
    ASSERT_ARE_EQUAL(int, 4567, (int)httpStatusCode);
    mocks.AssertActualAndExpectedCalls();

    ///destroy
    destroyHttpObjects(&requestHttpHeaders, &requestHttpBody, &responseHttpHeaders, &responseHttpBody);
    HTTPAPIEX_Destroy(httpapiexhandle);
}

/*Tests_SRS_HTTPAPIEX_02_018: [If creating the temporary http headers in SRS_HTTPAPIEX_02_017 fails then HTTPAPIEX_ExecuteRequest shall return HTTPAPIEX_ERROR.] */
TEST_FUNCTION(HTTPAPIEX_ExecuteRequest_with_NULL_response_headers_fails_when_HTTPHeaders_Alloc_fails)
{
    /// arrange
    CHTTPAPIEXMocks mocks;
    auto httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);

    unsigned int httpStatusCode;
    HTTP_HEADERS_HANDLE requestHttpHeaders;
    BUFFER_HANDLE requestHttpBody;
    HTTP_HEADERS_HANDLE responseHttpHeaders;
    BUFFER_HANDLE responseHttpBody;
    createHttpObjects(&requestHttpHeaders, &requestHttpBody, &responseHttpHeaders, &responseHttpBody);
    mocks.ResetAllCalls();

    /*this is building the host and content-length for the http request headers*/
    STRICT_EXPECTED_CALL(mocks, BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Host", TEST_HOSTNAME))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Content-Length", TOSTRING(TEST_BUFFER_SIZE)))
        .IgnoreArgument(1);

    /*Becauyse it is creating fake response headers*/
    whenShallHTTPHeaders_Alloc_fail = currentHTTPHeaders_Alloc_call + 1;
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc());

    /// act
    auto result = HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, requestHttpHeaders, requestHttpBody, &httpStatusCode, NULL, responseHttpBody);

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_ERROR, result);
    mocks.AssertActualAndExpectedCalls();

    ///destroy
    destroyHttpObjects(&requestHttpHeaders, &requestHttpBody, &responseHttpHeaders, &responseHttpBody);
    HTTPAPIEX_Destroy(httpapiexhandle);
}

/*Tests_SRS_HTTPAPIEX_02_019: [If responseHeaders is not NULL, then then HTTPAPIEX_ExecuteRequest shall use that object as parameter responseHeaders of HTTPAPI_ExecuteRequest call.] */
TEST_FUNCTION(HTTPAPIEX_ExecuteRequest_with_non_NULL_response_headers_suceeds)
{
    /// arrange
    CHTTPAPIEXMocks mocks;
    auto httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);

    unsigned int httpStatusCode;
    unsigned int asGivenyHttpApi = 4567;
    HTTP_HEADERS_HANDLE requestHttpHeaders;
    BUFFER_HANDLE requestHttpBody;
    HTTP_HEADERS_HANDLE responseHttpHeaders;
    BUFFER_HANDLE responseHttpBody;
    createHttpObjects(&requestHttpHeaders, &requestHttpBody, &responseHttpHeaders, &responseHttpBody);
    mocks.ResetAllCalls();

    /*this is building the host and content-length for the http request headers*/
    STRICT_EXPECTED_CALL(mocks, BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Host", TEST_HOSTNAME))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Content-Length", TOSTRING(TEST_BUFFER_SIZE)))
        .IgnoreArgument(1);

    /*this is getting the buffer content and buffer length to pass to httpapi_executerequest*/
    STRICT_EXPECTED_CALL(mocks, HTTPAPI_Init());
    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPAPI_CreateConnection(TEST_HOSTNAME));
    STRICT_EXPECTED_CALL(mocks, VECTOR_size(IGNORED_PTR_ARG)) /*this is passing the options*/ /*there are none saved in the regular sequences*/
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, BUFFER_u_char(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, HTTPAPI_ExecuteRequest(
        IGNORED_PTR_ARG,
        HTTPAPI_REQUEST_PATCH,
        TEST_RELATIVE_PATH,
        requestHttpHeaders,
        IGNORED_PTR_ARG,
        TEST_BUFFER_SIZE,
        IGNORED_PTR_ARG,
        responseHttpHeaders,
        responseHttpBody))
        .IgnoreArgument(1)
        .IgnoreArgument(5)
        .IgnoreArgument(7)
        .ValidateArgumentBuffer(5, TEST_BUFFER, TEST_BUFFER_SIZE)
        .CopyOutArgumentBuffer(7, &asGivenyHttpApi, sizeof(asGivenyHttpApi))
        ;

    /// act
    auto result = HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, requestHttpHeaders, requestHttpBody, &httpStatusCode, responseHttpHeaders, responseHttpBody);

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_OK, result);
    ASSERT_ARE_EQUAL(int, 4567, (int)httpStatusCode);
    mocks.AssertActualAndExpectedCalls();

    ///destroy
    destroyHttpObjects(&requestHttpHeaders, &requestHttpBody, &responseHttpHeaders, &responseHttpBody);
    HTTPAPIEX_Destroy(httpapiexhandle);
}

/*Tests_SRS_HTTPAPIEX_02_020: [If responseContent is NULL then HTTPAPIEX_ExecuteRequest shall create a temporary internal BUFFER object and use that as parameter responseContent of HTTPAPI_ExecuteRequest call.] */
TEST_FUNCTION(HTTPAPIEX_ExecuteRequest_with_NULL_response_body_suceeds)
{
    /// arrange
    CHTTPAPIEXMocks mocks;
    auto httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);

    unsigned int httpStatusCode;
    unsigned int asGivenyHttpApi = 4567;
    HTTP_HEADERS_HANDLE requestHttpHeaders;
    BUFFER_HANDLE requestHttpBody;
    HTTP_HEADERS_HANDLE responseHttpHeaders;
    BUFFER_HANDLE responseHttpBody;
    createHttpObjects(&requestHttpHeaders, &requestHttpBody, &responseHttpHeaders, &responseHttpBody);
    mocks.ResetAllCalls();

    /*this is building the host and content-length for the http request headers*/
    STRICT_EXPECTED_CALL(mocks, BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Host", TEST_HOSTNAME))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Content-Length", TOSTRING(TEST_BUFFER_SIZE)))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, BUFFER_new()); /*because it makes a fake response buffer*/
    STRICT_EXPECTED_CALL(mocks, BUFFER_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    /*this is getting the buffer content and buffer length to pass to httpapi_executerequest*/
    STRICT_EXPECTED_CALL(mocks, HTTPAPI_Init());
    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPAPI_CreateConnection(TEST_HOSTNAME));
    STRICT_EXPECTED_CALL(mocks, VECTOR_size(IGNORED_PTR_ARG)) /*this is passing the options*/ /*there are none saved in the regular sequences*/
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, BUFFER_u_char(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, HTTPAPI_ExecuteRequest(
        IGNORED_PTR_ARG,
        HTTPAPI_REQUEST_PATCH,
        TEST_RELATIVE_PATH,
        requestHttpHeaders,
        IGNORED_PTR_ARG,
        TEST_BUFFER_SIZE,
        IGNORED_PTR_ARG,
        responseHttpHeaders,
        IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(4)
        .IgnoreArgument(5)
        .IgnoreArgument(7)
        .IgnoreArgument(9)
        .ValidateArgumentBuffer(5, TEST_BUFFER, TEST_BUFFER_SIZE)
        .CopyOutArgumentBuffer(7, &asGivenyHttpApi, sizeof(asGivenyHttpApi))
        ;

    /// act
    auto result = HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, requestHttpHeaders, requestHttpBody, &httpStatusCode, responseHttpHeaders, NULL);

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_OK, result);
    ASSERT_ARE_EQUAL(int, 4567, (int)httpStatusCode);
    mocks.AssertActualAndExpectedCalls();

    ///destroy
    destroyHttpObjects(&requestHttpHeaders, &requestHttpBody, &responseHttpHeaders, &responseHttpBody);
    HTTPAPIEX_Destroy(httpapiexhandle);
}

/*Tests_SRS_HTTPAPIEX_02_021: [If creating the BUFFER_HANDLE in SRS_HTTPAPIEX_02_020 fails, then HTTPAPIEX_ExecuteRequest shall return HTTPAPIEX_ERROR.] */
TEST_FUNCTION(HTTPAPIEX_ExecuteRequest_with_NULL_fails_when_BUFFER_new_fails)
{
    /// arrange
    CHTTPAPIEXMocks mocks;
    auto httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);

    unsigned int httpStatusCode;
    HTTP_HEADERS_HANDLE requestHttpHeaders;
    BUFFER_HANDLE requestHttpBody;
    HTTP_HEADERS_HANDLE responseHttpHeaders;
    BUFFER_HANDLE responseHttpBody;
    createHttpObjects(&requestHttpHeaders, &requestHttpBody, &responseHttpHeaders, &responseHttpBody);
    mocks.ResetAllCalls();

    /*this is building the host and content-length for the http request headers*/
    STRICT_EXPECTED_CALL(mocks, BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Host", TEST_HOSTNAME))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Content-Length", TOSTRING(TEST_BUFFER_SIZE)))
        .IgnoreArgument(1);

    whenShallBUFFER_new_fail = currentBUFFER_new_call + 1;
    STRICT_EXPECTED_CALL(mocks, BUFFER_new()); /*because it makes a fake response buffer*/

    /// act
    auto result = HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, requestHttpHeaders, requestHttpBody, &httpStatusCode, responseHttpHeaders, NULL);

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_ERROR, result);
    mocks.AssertActualAndExpectedCalls();

    ///destroy
    destroyHttpObjects(&requestHttpHeaders, &requestHttpBody, &responseHttpHeaders, &responseHttpBody);
    HTTPAPIEX_Destroy(httpapiexhandle);
}

/*Tests_SRS_HTTPAPIEX_02_022: [If responseContent is not NULL then HTTPAPIEX_ExecuteRequest use that as parameter responseContent of HTTPAPI_ExecuteRequest call.] */
TEST_FUNCTION(HTTPAPIEX_ExecuteRequest_with_non_NULL_response_body_suceeds)
{
    /// arrange
    CHTTPAPIEXMocks mocks;
    auto httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);

    unsigned int httpStatusCode;
    unsigned int asGivenyHttpApi = 4567;
    HTTP_HEADERS_HANDLE requestHttpHeaders;
    BUFFER_HANDLE requestHttpBody;
    HTTP_HEADERS_HANDLE responseHttpHeaders;
    BUFFER_HANDLE responseHttpBody;
    createHttpObjects(&requestHttpHeaders, &requestHttpBody, &responseHttpHeaders, &responseHttpBody);
    mocks.ResetAllCalls();

    /*this is building the host and content-length for the http request headers*/
    STRICT_EXPECTED_CALL(mocks, BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Host", TEST_HOSTNAME))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Content-Length", TOSTRING(TEST_BUFFER_SIZE)))
        .IgnoreArgument(1);

    /*this is getting the buffer content and buffer length to pass to httpapi_executerequest*/
    STRICT_EXPECTED_CALL(mocks, HTTPAPI_Init());
    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPAPI_CreateConnection(TEST_HOSTNAME));
    STRICT_EXPECTED_CALL(mocks, VECTOR_size(IGNORED_PTR_ARG)) /*this is passing the options*/ /*there are none saved in the regular sequences*/
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, BUFFER_u_char(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, HTTPAPI_ExecuteRequest(
        IGNORED_PTR_ARG,
        HTTPAPI_REQUEST_PATCH,
        TEST_RELATIVE_PATH,
        requestHttpHeaders,
        IGNORED_PTR_ARG,
        TEST_BUFFER_SIZE,
        IGNORED_PTR_ARG,
        responseHttpHeaders,
        responseHttpBody))
        .IgnoreArgument(1)
        .IgnoreArgument(4)
        .IgnoreArgument(5)
        .IgnoreArgument(7)
        .ValidateArgumentBuffer(5, TEST_BUFFER, TEST_BUFFER_SIZE)
        .CopyOutArgumentBuffer(7, &asGivenyHttpApi, sizeof(asGivenyHttpApi))
        ;

    /// act
    auto result = HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, requestHttpHeaders, requestHttpBody, &httpStatusCode, responseHttpHeaders, responseHttpBody);

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_OK, result);
    ASSERT_ARE_EQUAL(int, 4567, (int)httpStatusCode);
    mocks.AssertActualAndExpectedCalls();

    ///destroy
    destroyHttpObjects(&requestHttpHeaders, &requestHttpBody, &responseHttpHeaders, &responseHttpBody);
    HTTPAPIEX_Destroy(httpapiexhandle);
}

/*Tests_SRS_HTTPAPIEX_02_023: [HTTPAPIEX_ExecuteRequest shall try to execute the HTTP call by ensuring the following API call sequence is respected:]*/
/*Tests_SRS_HTTPAPIEX_02_024: [If any point in the sequence fails, HTTPAPIEX_ExecuteRequest shall attempt to recover by going back to the previous step and retrying that step.]*/
/*Tests_SRS_HTTPAPIEX_02_025: [If the first step fails, then the sequence fails.]*/
/*Tests_SRS_HTTPAPIEX_02_026: [A step shall be retried at most once.]*/
/*Tests_SRS_HTTPAPIEX_02_027: [If a step has been retried then all subsequent steps shall be retried too.]*/
/*Tests_SRS_HTTPAPIEX_02_028: [HTTPAPIEX_ExecuteRequest shall return HTTPAPIEX_OK when a call to HTTPAPI_ExecuteRequest has been completed successfully.]*/
TEST_FUNCTION(HTTPAPIEX_ExecuteRequest_TestCase_S1) /*refer to httpapiex_retry_mechanism.vsdx*/
{
    /// arrange
    CHTTPAPIEXMocks mocks;
    auto httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);

    unsigned int httpStatusCode;
    unsigned int asGivenyHttpApi = 23;
    HTTP_HEADERS_HANDLE requestHttpHeaders;
    BUFFER_HANDLE requestHttpBody;
    HTTP_HEADERS_HANDLE responseHttpHeaders;
    BUFFER_HANDLE responseHttpBody;
    createHttpObjects(&requestHttpHeaders, &requestHttpBody, &responseHttpHeaders, &responseHttpBody);
    (void)HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, requestHttpHeaders, requestHttpBody, &httpStatusCode, responseHttpHeaders, responseHttpBody);
    mocks.ResetAllCalls();

    prepareHTTPAPIEX_ExecuteRequest(&mocks, &asGivenyHttpApi, requestHttpHeaders, responseHttpHeaders, responseHttpBody, HTTPAPI_OK);

    /// act
    auto result = HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, requestHttpHeaders, requestHttpBody, &httpStatusCode, responseHttpHeaders, responseHttpBody);

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_OK, result);
    ASSERT_ARE_EQUAL(int, 23, (int)httpStatusCode);
    mocks.AssertActualAndExpectedCalls();

    ///destroy
    destroyHttpObjects(&requestHttpHeaders, &requestHttpBody, &responseHttpHeaders, &responseHttpBody);
    HTTPAPIEX_Destroy(httpapiexhandle);
}

TEST_FUNCTION(HTTPAPIEX_ExecuteRequest_TestCase_S2) /*refer to httpapiex_retry_mechanism.vsdx*/
{
    /// arrange
    CHTTPAPIEXMocks mocks;
    auto httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);

    unsigned int httpStatusCode;
    unsigned int asGivenyHttpApi = 23;
    HTTP_HEADERS_HANDLE requestHttpHeaders;
    BUFFER_HANDLE requestHttpBody;
    HTTP_HEADERS_HANDLE responseHttpHeaders;
    BUFFER_HANDLE responseHttpBody;
    createHttpObjects(&requestHttpHeaders, &requestHttpBody, &responseHttpHeaders, &responseHttpBody);
    (void)HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, requestHttpHeaders, requestHttpBody, &httpStatusCode, responseHttpHeaders, responseHttpBody);
    mocks.ResetAllCalls();

    prepareHTTPAPIEX_ExecuteRequest(&mocks, &asGivenyHttpApi, requestHttpHeaders, responseHttpHeaders, responseHttpBody, HTTPAPI_ERROR);

    {
        STRICT_EXPECTED_CALL(mocks, HTTPAPI_CloseConnection(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, HTTPAPI_CreateConnection(TEST_HOSTNAME));
        STRICT_EXPECTED_CALL(mocks, VECTOR_size(IGNORED_PTR_ARG)) /*this is passing the options*/ /*there are none saved in the regular sequences*/
            .IgnoreArgument(1);

    }

    {
        STRICT_EXPECTED_CALL(mocks, BUFFER_length(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, BUFFER_u_char(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, HTTPAPI_ExecuteRequest(
            IGNORED_PTR_ARG,
            HTTPAPI_REQUEST_PATCH,
            TEST_RELATIVE_PATH,
            requestHttpHeaders,
            IGNORED_PTR_ARG,
            TEST_BUFFER_SIZE,
            IGNORED_PTR_ARG,
            responseHttpHeaders,
            responseHttpBody))
            .IgnoreArgument(1)
            .IgnoreArgument(4)
            .IgnoreArgument(5)
            .IgnoreArgument(7)
            .ValidateArgumentBuffer(5, TEST_BUFFER, TEST_BUFFER_SIZE)
            .CopyOutArgumentBuffer(7, &asGivenyHttpApi, sizeof(asGivenyHttpApi))
            ;
    }
    /// act
    auto result = HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, requestHttpHeaders, requestHttpBody, &httpStatusCode, responseHttpHeaders, responseHttpBody);

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_OK, result);
    ASSERT_ARE_EQUAL(int, 23, (int)httpStatusCode);
    mocks.AssertActualAndExpectedCalls();

    ///destroy
    destroyHttpObjects(&requestHttpHeaders, &requestHttpBody, &responseHttpHeaders, &responseHttpBody);
    HTTPAPIEX_Destroy(httpapiexhandle);
}

TEST_FUNCTION(HTTPAPIEX_ExecuteRequest_TestCase_S3) /*refer to httpapiex_retry_mechanism.vsdx*/
{
    /// arrange
    CHTTPAPIEXMocks mocks;
    auto httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);

    unsigned int httpStatusCode;
    unsigned int asGivenyHttpApi = 23;
    HTTP_HEADERS_HANDLE requestHttpHeaders;
    BUFFER_HANDLE requestHttpBody;
    HTTP_HEADERS_HANDLE responseHttpHeaders;
    BUFFER_HANDLE responseHttpBody;
    createHttpObjects(&requestHttpHeaders, &requestHttpBody, &responseHttpHeaders, &responseHttpBody);
    (void)HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, requestHttpHeaders, requestHttpBody, &httpStatusCode, responseHttpHeaders, responseHttpBody);
    mocks.ResetAllCalls();

    prepareHTTPAPIEX_ExecuteRequest(&mocks, &asGivenyHttpApi, requestHttpHeaders, responseHttpHeaders, responseHttpBody, HTTPAPI_ERROR);

    {
        STRICT_EXPECTED_CALL(mocks, HTTPAPI_CloseConnection(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, HTTPAPI_CreateConnection(TEST_HOSTNAME));
        STRICT_EXPECTED_CALL(mocks, VECTOR_size(IGNORED_PTR_ARG)) /*this is passing the options*/ /*there are none saved in the regular sequences*/
            .IgnoreArgument(1);
    }

    {
        STRICT_EXPECTED_CALL(mocks, BUFFER_length(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, BUFFER_u_char(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, HTTPAPI_ExecuteRequest(
            IGNORED_PTR_ARG,
            HTTPAPI_REQUEST_PATCH,
            TEST_RELATIVE_PATH,
            requestHttpHeaders,
            IGNORED_PTR_ARG,
            TEST_BUFFER_SIZE,
            IGNORED_PTR_ARG,
            responseHttpHeaders,
            responseHttpBody))
            .IgnoreArgument(1)
            .IgnoreArgument(4)
            .IgnoreArgument(5)
            .IgnoreArgument(7)
            .ValidateArgumentBuffer(5, TEST_BUFFER, TEST_BUFFER_SIZE)
            .CopyOutArgumentBuffer(7, &asGivenyHttpApi, sizeof(asGivenyHttpApi))
            .SetReturn(HTTPAPI_ERROR)
            ;
    }

    {
        STRICT_EXPECTED_CALL(mocks, HTTPAPI_CloseConnection(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, HTTPAPI_Deinit());
        STRICT_EXPECTED_CALL(mocks, HTTPAPI_Init())
            .SetReturn(HTTPAPI_OK);
    }

    {
        STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, HTTPAPI_CreateConnection(TEST_HOSTNAME));
        STRICT_EXPECTED_CALL(mocks, VECTOR_size(IGNORED_PTR_ARG)) /*this is passing the options*/ /*there are none saved in the regular sequences*/
            .IgnoreArgument(1);
    }

    {
        STRICT_EXPECTED_CALL(mocks, BUFFER_length(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, BUFFER_u_char(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, HTTPAPI_ExecuteRequest(
            IGNORED_PTR_ARG,
            HTTPAPI_REQUEST_PATCH,
            TEST_RELATIVE_PATH,
            requestHttpHeaders,
            IGNORED_PTR_ARG,
            TEST_BUFFER_SIZE,
            IGNORED_PTR_ARG,
            responseHttpHeaders,
            responseHttpBody))
            .IgnoreArgument(1)
            .IgnoreArgument(4)
            .IgnoreArgument(5)
            .IgnoreArgument(7)
            .ValidateArgumentBuffer(5, TEST_BUFFER, TEST_BUFFER_SIZE)
            .CopyOutArgumentBuffer(7, &asGivenyHttpApi, sizeof(asGivenyHttpApi))
            ;
    }

    /// act
    auto result = HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, requestHttpHeaders, requestHttpBody, &httpStatusCode, responseHttpHeaders, responseHttpBody);

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_OK, result);
    ASSERT_ARE_EQUAL(int, 23, (int)httpStatusCode);
    mocks.AssertActualAndExpectedCalls();

    ///destroy
    destroyHttpObjects(&requestHttpHeaders, &requestHttpBody, &responseHttpHeaders, &responseHttpBody);
    HTTPAPIEX_Destroy(httpapiexhandle);
}

TEST_FUNCTION(HTTPAPIEX_ExecuteRequest_TestCase_S4) /*refer to httpapiex_retry_mechanism.vsdx*/
{
    /// arrange
    CHTTPAPIEXMocks mocks;
    auto httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);

    unsigned int httpStatusCode;
    unsigned int asGivenyHttpApi = 23;
    HTTP_HEADERS_HANDLE requestHttpHeaders;
    BUFFER_HANDLE requestHttpBody;
    HTTP_HEADERS_HANDLE responseHttpHeaders;
    BUFFER_HANDLE responseHttpBody;
    createHttpObjects(&requestHttpHeaders, &requestHttpBody, &responseHttpHeaders, &responseHttpBody);
    (void)HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, requestHttpHeaders, requestHttpBody, &httpStatusCode, responseHttpHeaders, responseHttpBody);
    mocks.ResetAllCalls();

    prepareHTTPAPIEX_ExecuteRequest(&mocks, &asGivenyHttpApi, requestHttpHeaders, responseHttpHeaders, responseHttpBody, HTTPAPI_ERROR);

    {
        STRICT_EXPECTED_CALL(mocks, HTTPAPI_CloseConnection(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        whenShallHTTPAPI_CreateConnection_fail[0] = currentHTTPAPI_CreateConnection_call + 1;
        STRICT_EXPECTED_CALL(mocks, HTTPAPI_CreateConnection(TEST_HOSTNAME));
    }

    {
        STRICT_EXPECTED_CALL(mocks, HTTPAPI_Deinit());
        STRICT_EXPECTED_CALL(mocks, HTTPAPI_Init())
            .SetReturn(HTTPAPI_OK);
    }

    {
        STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, HTTPAPI_CreateConnection(TEST_HOSTNAME));
        STRICT_EXPECTED_CALL(mocks, VECTOR_size(IGNORED_PTR_ARG)) /*this is passing the options*/ /*there are none saved in the regular sequences*/
            .IgnoreArgument(1);
    }

    {
        STRICT_EXPECTED_CALL(mocks, BUFFER_length(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, BUFFER_u_char(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, HTTPAPI_ExecuteRequest(
            IGNORED_PTR_ARG,
            HTTPAPI_REQUEST_PATCH,
            TEST_RELATIVE_PATH,
            requestHttpHeaders,
            IGNORED_PTR_ARG,
            TEST_BUFFER_SIZE,
            IGNORED_PTR_ARG,
            responseHttpHeaders,
            responseHttpBody))
            .IgnoreArgument(1)
            .IgnoreArgument(4)
            .IgnoreArgument(5)
            .IgnoreArgument(7)
            .ValidateArgumentBuffer(5, TEST_BUFFER, TEST_BUFFER_SIZE)
            .CopyOutArgumentBuffer(7, &asGivenyHttpApi, sizeof(asGivenyHttpApi))
            ;
    }

    /// act
    auto result = HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, requestHttpHeaders, requestHttpBody, &httpStatusCode, responseHttpHeaders, responseHttpBody);

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_OK, result);
    ASSERT_ARE_EQUAL(int, 23, (int)httpStatusCode);
    mocks.AssertActualAndExpectedCalls();

    ///destroy
    destroyHttpObjects(&requestHttpHeaders, &requestHttpBody, &responseHttpHeaders, &responseHttpBody);
    HTTPAPIEX_Destroy(httpapiexhandle);
}

/*this and all subsequent tests... */
/*Tests_SRS_HTTPAPIEX_02_029: [Otherwise, HTTAPIEX_ExecuteRequest shall return HTTPAPIEX_RECOVERYFAILED.] */
TEST_FUNCTION(HTTPAPIEX_ExecuteRequest_TestCase_F1) /*refer to httpapiex_retry_mechanism.vsdx*/
{
    /// arrange
    CHTTPAPIEXMocks mocks;
    auto httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);

    unsigned int httpStatusCode;
    unsigned int asGivenyHttpApi = 23;
    HTTP_HEADERS_HANDLE requestHttpHeaders;
    BUFFER_HANDLE requestHttpBody;
    HTTP_HEADERS_HANDLE responseHttpHeaders;
    BUFFER_HANDLE responseHttpBody;
    createHttpObjects(&requestHttpHeaders, &requestHttpBody, &responseHttpHeaders, &responseHttpBody);
    (void)HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, requestHttpHeaders, requestHttpBody, &httpStatusCode, responseHttpHeaders, responseHttpBody);
    mocks.ResetAllCalls();

    prepareHTTPAPIEX_ExecuteRequest(&mocks, &asGivenyHttpApi, requestHttpHeaders, responseHttpHeaders, responseHttpBody, HTTPAPI_ERROR);

    {
        STRICT_EXPECTED_CALL(mocks, HTTPAPI_CloseConnection(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        whenShallHTTPAPI_CreateConnection_fail[0] = currentHTTPAPI_CreateConnection_call + 1;
        STRICT_EXPECTED_CALL(mocks, HTTPAPI_CreateConnection(TEST_HOSTNAME));
    }

    {
        STRICT_EXPECTED_CALL(mocks, HTTPAPI_Deinit());
        whenShallHTTPAPI_Init_fail[0] = currentHTTPAPI_Init_call + 1;
        STRICT_EXPECTED_CALL(mocks, HTTPAPI_Init());
    }

    /// act
    auto result = HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, requestHttpHeaders, requestHttpBody, &httpStatusCode, responseHttpHeaders, responseHttpBody);

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_RECOVERYFAILED, result);
    mocks.AssertActualAndExpectedCalls();

    ///destroy
    destroyHttpObjects(&requestHttpHeaders, &requestHttpBody, &responseHttpHeaders, &responseHttpBody);
    HTTPAPIEX_Destroy(httpapiexhandle);
}


/*Tests_SRS_HTTPAPIEX_02_029: [Otherwise, HTTAPIEX_ExecuteRequest shall return HTTPAPIEX_RECOVERYFAILED.] */
TEST_FUNCTION(HTTPAPIEX_ExecuteRequest_TestCase_F2) /*refer to httpapiex_retry_mechanism.vsdx*/
{
    /// arrange
    CHTTPAPIEXMocks mocks;
    auto httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);

    unsigned int httpStatusCode;
    unsigned int asGivenyHttpApi = 23;
    HTTP_HEADERS_HANDLE requestHttpHeaders;
    BUFFER_HANDLE requestHttpBody;
    HTTP_HEADERS_HANDLE responseHttpHeaders;
    BUFFER_HANDLE responseHttpBody;
    createHttpObjects(&requestHttpHeaders, &requestHttpBody, &responseHttpHeaders, &responseHttpBody);
    (void)HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, requestHttpHeaders, requestHttpBody, &httpStatusCode, responseHttpHeaders, responseHttpBody);
    mocks.ResetAllCalls();

    prepareHTTPAPIEX_ExecuteRequest(&mocks, &asGivenyHttpApi, requestHttpHeaders, responseHttpHeaders, responseHttpBody, HTTPAPI_ERROR);

    {
        STRICT_EXPECTED_CALL(mocks, HTTPAPI_CloseConnection(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        whenShallHTTPAPI_CreateConnection_fail[0] = currentHTTPAPI_CreateConnection_call + 1;
        STRICT_EXPECTED_CALL(mocks, HTTPAPI_CreateConnection(TEST_HOSTNAME));
    }

    {
        STRICT_EXPECTED_CALL(mocks, HTTPAPI_Deinit());
        STRICT_EXPECTED_CALL(mocks, HTTPAPI_Init());
    }

    {
        STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        whenShallHTTPAPI_CreateConnection_fail[1] = currentHTTPAPI_CreateConnection_call + 2;
        STRICT_EXPECTED_CALL(mocks, HTTPAPI_CreateConnection(TEST_HOSTNAME));

    }

    /*this is post final fail - sequence is reset to a clean start*/
    {        
        STRICT_EXPECTED_CALL(mocks, HTTPAPI_Deinit());
    }

    /// act
    auto result = HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, requestHttpHeaders, requestHttpBody, &httpStatusCode, responseHttpHeaders, responseHttpBody);

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_RECOVERYFAILED, result);
    mocks.AssertActualAndExpectedCalls();

    ///destroy
    destroyHttpObjects(&requestHttpHeaders, &requestHttpBody, &responseHttpHeaders, &responseHttpBody);
    HTTPAPIEX_Destroy(httpapiexhandle);
}

/*Tests_SRS_HTTPAPIEX_02_029: [Otherwise, HTTAPIEX_ExecuteRequest shall return HTTPAPIEX_RECOVERYFAILED.] */
TEST_FUNCTION(HTTPAPIEX_ExecuteRequest_TestCase_F3) /*refer to httpapiex_retry_mechanism.vsdx*/
{
    /// arrange
    CHTTPAPIEXMocks mocks;
    auto httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);

    unsigned int httpStatusCode;
    unsigned int asGivenyHttpApi = 23;
    HTTP_HEADERS_HANDLE requestHttpHeaders;
    BUFFER_HANDLE requestHttpBody;
    HTTP_HEADERS_HANDLE responseHttpHeaders;
    BUFFER_HANDLE responseHttpBody;
    createHttpObjects(&requestHttpHeaders, &requestHttpBody, &responseHttpHeaders, &responseHttpBody);
    (void)HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, requestHttpHeaders, requestHttpBody, &httpStatusCode, responseHttpHeaders, responseHttpBody);
    mocks.ResetAllCalls();

    prepareHTTPAPIEX_ExecuteRequest(&mocks, &asGivenyHttpApi, requestHttpHeaders, responseHttpHeaders, responseHttpBody, HTTPAPI_ERROR);

    {
        STRICT_EXPECTED_CALL(mocks, HTTPAPI_CloseConnection(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        whenShallHTTPAPI_CreateConnection_fail[0] = currentHTTPAPI_CreateConnection_call + 1;
        STRICT_EXPECTED_CALL(mocks, HTTPAPI_CreateConnection(TEST_HOSTNAME));
    }

    {
        STRICT_EXPECTED_CALL(mocks, HTTPAPI_Deinit());
        STRICT_EXPECTED_CALL(mocks, HTTPAPI_Init());
    }

    {
        STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, HTTPAPI_CreateConnection(TEST_HOSTNAME));
        STRICT_EXPECTED_CALL(mocks, VECTOR_size(IGNORED_PTR_ARG)) /*this is passing the options*/ /*there are none saved in the regular sequences*/
            .IgnoreArgument(1);

    }

    {
        STRICT_EXPECTED_CALL(mocks, BUFFER_length(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, BUFFER_u_char(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, HTTPAPI_ExecuteRequest(
            IGNORED_PTR_ARG,
            HTTPAPI_REQUEST_PATCH,
            TEST_RELATIVE_PATH,
            requestHttpHeaders,
            IGNORED_PTR_ARG,
            TEST_BUFFER_SIZE,
            IGNORED_PTR_ARG,
            responseHttpHeaders,
            responseHttpBody))
            .IgnoreArgument(1)
            .IgnoreArgument(4)
            .IgnoreArgument(5)
            .IgnoreArgument(7)
            .ValidateArgumentBuffer(5, TEST_BUFFER, TEST_BUFFER_SIZE)
            .CopyOutArgumentBuffer(7, &asGivenyHttpApi, sizeof(asGivenyHttpApi))
            .SetReturn(HTTPAPI_ERROR);
            ;
    }

    /*this is post final fail - sequence is reset to a clean start*/
    {
        STRICT_EXPECTED_CALL(mocks, HTTPAPI_CloseConnection(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, HTTPAPI_Deinit());
    }

    /// act
    auto result = HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, requestHttpHeaders, requestHttpBody, &httpStatusCode, responseHttpHeaders, responseHttpBody);

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_RECOVERYFAILED, result);
    mocks.AssertActualAndExpectedCalls();

    ///destroy
    destroyHttpObjects(&requestHttpHeaders, &requestHttpBody, &responseHttpHeaders, &responseHttpBody);
    HTTPAPIEX_Destroy(httpapiexhandle);
}

/*Tests_SRS_HTTPAPIEX_02_029: [Otherwise, HTTAPIEX_ExecuteRequest shall return HTTPAPIEX_RECOVERYFAILED.] */
TEST_FUNCTION(HTTPAPIEX_ExecuteRequest_TestCase_F4) /*refer to httpapiex_retry_mechanism.vsdx*/
{
    /// arrange
    CHTTPAPIEXMocks mocks;
    auto httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);

    unsigned int httpStatusCode;
    unsigned int asGivenyHttpApi = 23;
    HTTP_HEADERS_HANDLE requestHttpHeaders;
    BUFFER_HANDLE requestHttpBody;
    HTTP_HEADERS_HANDLE responseHttpHeaders;
    BUFFER_HANDLE responseHttpBody;
    createHttpObjects(&requestHttpHeaders, &requestHttpBody, &responseHttpHeaders, &responseHttpBody);
    (void)HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, requestHttpHeaders, requestHttpBody, &httpStatusCode, responseHttpHeaders, responseHttpBody);
    mocks.ResetAllCalls();

    prepareHTTPAPIEX_ExecuteRequest(&mocks, &asGivenyHttpApi, requestHttpHeaders, responseHttpHeaders, responseHttpBody, HTTPAPI_ERROR);

    {
        STRICT_EXPECTED_CALL(mocks, HTTPAPI_CloseConnection(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, HTTPAPI_CreateConnection(TEST_HOSTNAME));
        STRICT_EXPECTED_CALL(mocks, VECTOR_size(IGNORED_PTR_ARG)) /*this is passing the options*/ /*there are none saved in the regular sequences*/
            .IgnoreArgument(1);

    }

    {
        STRICT_EXPECTED_CALL(mocks, BUFFER_length(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, BUFFER_u_char(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, HTTPAPI_ExecuteRequest(
            IGNORED_PTR_ARG,
            HTTPAPI_REQUEST_PATCH,
            TEST_RELATIVE_PATH,
            requestHttpHeaders,
            IGNORED_PTR_ARG,
            TEST_BUFFER_SIZE,
            IGNORED_PTR_ARG,
            responseHttpHeaders,
            responseHttpBody))
            .IgnoreArgument(1)
            .IgnoreArgument(4)
            .IgnoreArgument(5)
            .IgnoreArgument(7)
            .ValidateArgumentBuffer(5, TEST_BUFFER, TEST_BUFFER_SIZE)
            .CopyOutArgumentBuffer(7, &asGivenyHttpApi, sizeof(asGivenyHttpApi))
            .SetReturn(HTTPAPI_ERROR);
        ;
    }

    {
        STRICT_EXPECTED_CALL(mocks, HTTPAPI_CloseConnection(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, HTTPAPI_Deinit());

        whenShallHTTPAPI_Init_fail[0] = currentHTTPAPI_Init_call + 1;
        STRICT_EXPECTED_CALL(mocks, HTTPAPI_Init());
    }

    /// act
    auto result = HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, requestHttpHeaders, requestHttpBody, &httpStatusCode, responseHttpHeaders, responseHttpBody);

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_RECOVERYFAILED, result);
    mocks.AssertActualAndExpectedCalls();

    ///destroy
    destroyHttpObjects(&requestHttpHeaders, &requestHttpBody, &responseHttpHeaders, &responseHttpBody);
    HTTPAPIEX_Destroy(httpapiexhandle);
}

/*Tests_SRS_HTTPAPIEX_02_029: [Otherwise, HTTAPIEX_ExecuteRequest shall return HTTPAPIEX_RECOVERYFAILED.] */
TEST_FUNCTION(HTTPAPIEX_ExecuteRequest_TestCase_F5) /*refer to httpapiex_retry_mechanism.vsdx*/
{
    /// arrange
    CHTTPAPIEXMocks mocks;
    auto httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);

    unsigned int httpStatusCode;
    unsigned int asGivenyHttpApi = 23;
    HTTP_HEADERS_HANDLE requestHttpHeaders;
    BUFFER_HANDLE requestHttpBody;
    HTTP_HEADERS_HANDLE responseHttpHeaders;
    BUFFER_HANDLE responseHttpBody;
    createHttpObjects(&requestHttpHeaders, &requestHttpBody, &responseHttpHeaders, &responseHttpBody);
    (void)HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, requestHttpHeaders, requestHttpBody, &httpStatusCode, responseHttpHeaders, responseHttpBody);
    mocks.ResetAllCalls();

    prepareHTTPAPIEX_ExecuteRequest(&mocks, &asGivenyHttpApi, requestHttpHeaders, responseHttpHeaders, responseHttpBody, HTTPAPI_ERROR);

    {
        STRICT_EXPECTED_CALL(mocks, HTTPAPI_CloseConnection(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, HTTPAPI_CreateConnection(TEST_HOSTNAME));
        STRICT_EXPECTED_CALL(mocks, VECTOR_size(IGNORED_PTR_ARG)) /*this is passing the options*/ /*there are none saved in the regular sequences*/
            .IgnoreArgument(1);

    }

    {
        STRICT_EXPECTED_CALL(mocks, BUFFER_length(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, BUFFER_u_char(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, HTTPAPI_ExecuteRequest(
            IGNORED_PTR_ARG,
            HTTPAPI_REQUEST_PATCH,
            TEST_RELATIVE_PATH,
            requestHttpHeaders,
            IGNORED_PTR_ARG,
            TEST_BUFFER_SIZE,
            IGNORED_PTR_ARG,
            responseHttpHeaders,
            responseHttpBody))
            .IgnoreArgument(1)
            .IgnoreArgument(4)
            .IgnoreArgument(5)
            .IgnoreArgument(7)
            .ValidateArgumentBuffer(5, TEST_BUFFER, TEST_BUFFER_SIZE)
            .CopyOutArgumentBuffer(7, &asGivenyHttpApi, sizeof(asGivenyHttpApi))
            .SetReturn(HTTPAPI_ERROR);
        ;
    }

    {
        STRICT_EXPECTED_CALL(mocks, HTTPAPI_CloseConnection(IGNORED_PTR_ARG))
            .IgnoreArgument(1); 
        STRICT_EXPECTED_CALL(mocks, HTTPAPI_Deinit());
        STRICT_EXPECTED_CALL(mocks, HTTPAPI_Init());
    }

    {
        STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        whenShallHTTPAPI_CreateConnection_fail[1] = currentHTTPAPI_CreateConnection_call + 1;
        STRICT_EXPECTED_CALL(mocks, HTTPAPI_CreateConnection(TEST_HOSTNAME));
    }

    {
        STRICT_EXPECTED_CALL(mocks, HTTPAPI_Deinit());
    }

    /// act
    auto result = HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, requestHttpHeaders, requestHttpBody, &httpStatusCode, responseHttpHeaders, responseHttpBody);

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_RECOVERYFAILED, result);
    mocks.AssertActualAndExpectedCalls();

    ///destroy
    destroyHttpObjects(&requestHttpHeaders, &requestHttpBody, &responseHttpHeaders, &responseHttpBody);
    HTTPAPIEX_Destroy(httpapiexhandle);
}

/*Tests_SRS_HTTPAPIEX_02_029: [Otherwise, HTTAPIEX_ExecuteRequest shall return HTTPAPIEX_RECOVERYFAILED.] */
TEST_FUNCTION(HTTPAPIEX_ExecuteRequest_TestCase_F6) /*refer to httpapiex_retry_mechanism.vsdx*/
{
    /// arrange
    CHTTPAPIEXMocks mocks;
    auto httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);

    unsigned int httpStatusCode;
    unsigned int asGivenyHttpApi = 23;
    HTTP_HEADERS_HANDLE requestHttpHeaders;
    BUFFER_HANDLE requestHttpBody;
    HTTP_HEADERS_HANDLE responseHttpHeaders;
    BUFFER_HANDLE responseHttpBody;
    createHttpObjects(&requestHttpHeaders, &requestHttpBody, &responseHttpHeaders, &responseHttpBody);
    (void)HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, requestHttpHeaders, requestHttpBody, &httpStatusCode, responseHttpHeaders, responseHttpBody);
    mocks.ResetAllCalls();

    prepareHTTPAPIEX_ExecuteRequest(&mocks, &asGivenyHttpApi, requestHttpHeaders, responseHttpHeaders, responseHttpBody, HTTPAPI_ERROR);

    {
        STRICT_EXPECTED_CALL(mocks, HTTPAPI_CloseConnection(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, HTTPAPI_CreateConnection(TEST_HOSTNAME));
        STRICT_EXPECTED_CALL(mocks, VECTOR_size(IGNORED_PTR_ARG)) /*this is passing the options*/ /*there are none saved in the regular sequences*/
            .IgnoreArgument(1);

    }

    {
        STRICT_EXPECTED_CALL(mocks, BUFFER_length(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, BUFFER_u_char(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, HTTPAPI_ExecuteRequest(
            IGNORED_PTR_ARG,
            HTTPAPI_REQUEST_PATCH,
            TEST_RELATIVE_PATH,
            requestHttpHeaders,
            IGNORED_PTR_ARG, 
            TEST_BUFFER_SIZE,
            IGNORED_PTR_ARG,
            responseHttpHeaders,
            responseHttpBody))
            .IgnoreArgument(1)
            .IgnoreArgument(4)
            .IgnoreArgument(5)
            .IgnoreArgument(7)
            .ValidateArgumentBuffer(5, TEST_BUFFER, TEST_BUFFER_SIZE)
            .CopyOutArgumentBuffer(7, &asGivenyHttpApi, sizeof(asGivenyHttpApi))
            .SetReturn(HTTPAPI_ERROR);
        ;
    }

    {
        STRICT_EXPECTED_CALL(mocks, HTTPAPI_CloseConnection(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, HTTPAPI_Deinit());
        STRICT_EXPECTED_CALL(mocks, HTTPAPI_Init());
    }

    {
        STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, HTTPAPI_CreateConnection(TEST_HOSTNAME));
        STRICT_EXPECTED_CALL(mocks, VECTOR_size(IGNORED_PTR_ARG)) /*this is passing the options*/ /*there are none saved in the regular sequences*/
            .IgnoreArgument(1);

    }

    {
        STRICT_EXPECTED_CALL(mocks, BUFFER_length(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, BUFFER_u_char(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, HTTPAPI_ExecuteRequest(
            IGNORED_PTR_ARG,
            HTTPAPI_REQUEST_PATCH,
            TEST_RELATIVE_PATH,
            requestHttpHeaders,
            IGNORED_PTR_ARG,
            TEST_BUFFER_SIZE,
            IGNORED_PTR_ARG,
            responseHttpHeaders,
            responseHttpBody))
            .IgnoreArgument(1)
            .IgnoreArgument(4)
            .IgnoreArgument(5)
            .IgnoreArgument(7)
            .ValidateArgumentBuffer(5, TEST_BUFFER, TEST_BUFFER_SIZE)
            .CopyOutArgumentBuffer(7, &asGivenyHttpApi, sizeof(asGivenyHttpApi))
            .SetReturn(HTTPAPI_ERROR);
        ;
    }

    {
        STRICT_EXPECTED_CALL(mocks, HTTPAPI_CloseConnection(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, HTTPAPI_Deinit());
    }

    /// act
    auto result = HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, requestHttpHeaders, requestHttpBody, &httpStatusCode, responseHttpHeaders, responseHttpBody);

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_RECOVERYFAILED, result);
    mocks.AssertActualAndExpectedCalls();

    ///destroy
    destroyHttpObjects(&requestHttpHeaders, &requestHttpBody, &responseHttpHeaders, &responseHttpBody);
    HTTPAPIEX_Destroy(httpapiexhandle);
}

/*Tests_SRS_HTTPAPIEX_02_032: [If parameter handle is NULL then HTTPAPIEX_SetOption shall return HTTPAPIEX_INVALID_ARG.] */
TEST_FUNCTION(HTTPAPIEX_SetOption_fails_with_NULL_handle)
{
    /// arrange
    CHTTPAPIEXMocks mocks;
    
    /// act
    auto result = HTTPAPIEX_SetOption(NULL, "someOption", (void*)42);

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_INVALID_ARG, result);
    mocks.AssertActualAndExpectedCalls();

    ///destroy
}

/*Tests_SRS_HTTPAPIEX_02_033: [If parameter optionName is NULL then HTTPAPIEX_SetOption shall return HTTPAPIEX_INVALID_ARG.] */
TEST_FUNCTION(HTTPAPIEX_SetOption_fails_with_NULL_optionName_fails)
{
    /// arrange
    CHTTPAPIEXMocks mocks;
    auto httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);
    mocks.ResetAllCalls();

    /// act
    auto result = HTTPAPIEX_SetOption(httpapiexhandle, NULL, (void*)42);

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_INVALID_ARG, result);
    mocks.AssertActualAndExpectedCalls();

    ///destroy
    HTTPAPIEX_Destroy(httpapiexhandle);
}

/*Tests_SRS_HTTPAPIEX_02_034: [If parameter value is NULL then HTTPAPIEX_SetOption shall return HTTPAPIEX_INVALID_ARG.] */
TEST_FUNCTION(HTTPAPIEX_SetOption_fails_with_NULL_value_fails)
{
    /// arrange
    CHTTPAPIEXMocks mocks;
    auto httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);
    mocks.ResetAllCalls();

    /// act
    auto result = HTTPAPIEX_SetOption(httpapiexhandle, "someOption", NULL);

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_INVALID_ARG, result);
    mocks.AssertActualAndExpectedCalls();

    ///destroy
    HTTPAPIEX_Destroy(httpapiexhandle);
}

/*Tests_SRS_HTTPAPIEX_02_037: [HTTPAPIEX_SetOption shall attempt to save the value of the option by calling HTTPAPI_CloneOption passing optionName and value, irrespective of the existence of a HTTPAPI_HANDLE] */
/*Tests_SRS_HTTPAPIEX_02_039: [If HTTPAPI_CloneOption returns HTTPAPI_OK then HTTPAPIEX_SetOption shall create or update the pair optionName/value.]*/
TEST_FUNCTION(HTTPAPIEX_SetOption_happy_path_without_httpapi_handle_succeeds)
{
    /// arrange
    CHTTPAPIEXMocks mocks;
    auto httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);
    mocks.ResetAllCalls();

    STRICT_EXPECTED_CALL(mocks, HTTPAPI_CloneOption("someOption", "333", IGNORED_PTR_ARG))  /*this asks lower HTTPAPI to create a clone of the option*/
        .IgnoreArgument(3);

    STRICT_EXPECTED_CALL(mocks, VECTOR_find_if(IGNORED_PTR_ARG, IGNORED_PTR_ARG, "someOption")) /*this is looking for optionName*/
        .IgnoreArgument(1)
        .IgnoreArgument(2);

    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(sizeof("someOption"))); /*this is creating a clone of the optionName*/

    STRICT_EXPECTED_CALL(mocks, VECTOR_push_back(IGNORED_PTR_ARG, IGNORED_PTR_ARG, 1)) /*this is adding the optionName, value*/
        .IgnoreArgument(1)
        .IgnoreArgument(2);

    /// act
    auto result = HTTPAPIEX_SetOption(httpapiexhandle, "someOption", "333");

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_OK, result);
    mocks.AssertActualAndExpectedCalls();

    ///destroy
    HTTPAPIEX_Destroy(httpapiexhandle);
}

/*Tests_SRS_HTTPAPIEX_02_037: [HTTPAPIEX_SetOption shall attempt to save the value of the option by calling HTTPAPI_CloneOption passing optionName and value, irrespective of the existence of a HTTPAPI_HANDLE] */
/*Tests_SRS_HTTPAPIEX_02_039: [If HTTPAPI_CloneOption returns HTTPAPI_OK then HTTPAPIEX_SetOption shall create or update the pair optionName/value.]*/
TEST_FUNCTION(HTTPAPIEX_SetOption_happy_path_without_httpapi_handle_can_save_2_options_succeeds)
{
    /// arrange
    CHTTPAPIEXMocks mocks;
    auto httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);
    mocks.ResetAllCalls();

    STRICT_EXPECTED_CALL(mocks, HTTPAPI_CloneOption("someOption1", (void*)"3", IGNORED_PTR_ARG))  /*this asks lower HTTPAPI to create a clone of the option*/
        .IgnoreArgument(3);

    STRICT_EXPECTED_CALL(mocks, VECTOR_find_if(IGNORED_PTR_ARG, IGNORED_PTR_ARG, "someOption1")) /*this is looking for hte option to device between update / create*/
        .IgnoreArgument(1)
        .IgnoreArgument(2);

    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(sizeof("someOption1"))); /*this is creating a clone of the optionName*/

    STRICT_EXPECTED_CALL(mocks, VECTOR_push_back(IGNORED_PTR_ARG, IGNORED_PTR_ARG, 1)) /*this is increasing the array of options by 1*/
        .IgnoreArgument(1)
        .IgnoreArgument(2);

    STRICT_EXPECTED_CALL(mocks, HTTPAPI_CloneOption("someOption2", (void*)"33", IGNORED_PTR_ARG))  /*this asks lower HTTPAPI to create a clone of the option*/
        .IgnoreArgument(3);

    STRICT_EXPECTED_CALL(mocks, VECTOR_find_if(IGNORED_PTR_ARG, IGNORED_PTR_ARG, "someOption2")) /*this is looking for hte option to device between update / create*/
        .IgnoreArgument(1)
        .IgnoreArgument(2);

    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(sizeof("someOption2"))); /*this is creating a clone of the optionName*/

    STRICT_EXPECTED_CALL(mocks, VECTOR_push_back(IGNORED_PTR_ARG, IGNORED_PTR_ARG, 1)) /*this is increasing the array of options by 1*/
        .IgnoreArgument(1)
        .IgnoreArgument(2);

    /// act
    auto result1 = HTTPAPIEX_SetOption(httpapiexhandle, "someOption1", (void*)"3");
    auto result2 = HTTPAPIEX_SetOption(httpapiexhandle, "someOption2", (void*)"33");

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_OK, result1);
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_OK, result2);
    mocks.AssertActualAndExpectedCalls();

    ///destroy
    HTTPAPIEX_Destroy(httpapiexhandle);
}

/*Tests_SRS_HTTPAPIEX_02_035: [HTTPAPIEX_ExecuteRequest shall pass all the saved options (see HTTPAPIEX_SetOption) to the newly create HTTPAPI_HANDLE in step 2 by calling HTTPAPI_SetOption.]*/
TEST_FUNCTION(HTTPAPIEX_SetOption_passes_saved_options_to_httpapi_succeeds)
{
    /// arrange
    CHTTPAPIEXMocks mocks;
    unsigned int httpStatusCode;
    HTTP_HEADERS_HANDLE requestHttpHeaders;
    BUFFER_HANDLE requestHttpBody;
    HTTP_HEADERS_HANDLE responseHttpHeaders;
    BUFFER_HANDLE responseHttpBody;
    createHttpObjects(&requestHttpHeaders, &requestHttpBody, &responseHttpHeaders, &responseHttpBody);

    auto httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);
    (void)HTTPAPIEX_SetOption(httpapiexhandle, "someOption1", (void*)"3");
    (void)HTTPAPIEX_SetOption(httpapiexhandle, "someOption2", (void*)"33");
    mocks.ResetAllCalls();

    setupAllCallBeforeHTTPsequence(&mocks);
    setupAllCallForHTTPsequence(&mocks, TEST_RELATIVE_PATH, requestHttpHeaders, requestHttpBody, responseHttpHeaders, responseHttpBody);

    STRICT_EXPECTED_CALL(mocks, VECTOR_element(IGNORED_PTR_ARG, 0))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, VECTOR_element(IGNORED_PTR_ARG, 1))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, HTTPAPI_SetOption(IGNORED_PTR_ARG, "someOption1", (void*)"3"))
        .IgnoreArgument(1)
        .IgnoreArgument(3);
    STRICT_EXPECTED_CALL(mocks, HTTPAPI_SetOption(IGNORED_PTR_ARG, "someOption2", (void*)"33"))
        .IgnoreArgument(1)
        .IgnoreArgument(3);

    /// act
    auto result = HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, requestHttpHeaders, requestHttpBody, &httpStatusCode, responseHttpHeaders, responseHttpBody);

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_OK, result);
    mocks.AssertActualAndExpectedCalls();

    ///destroy
    destroyHttpObjects(&requestHttpHeaders, &requestHttpBody, &responseHttpHeaders, &responseHttpBody);
    HTTPAPIEX_Destroy(httpapiexhandle);
}

/*Tests_SRS_HTTPAPIEX_02_036: [If setting the option fails, then the failure shall be ignored.] */
TEST_FUNCTION(HTTPAPIEX_SetOption_passes_saved_options_to_httpapi_ignores_errors_succeeds)
{
    /// arrange
    CHTTPAPIEXMocks mocks;
    unsigned int httpStatusCode;
    HTTP_HEADERS_HANDLE requestHttpHeaders;
    BUFFER_HANDLE requestHttpBody;
    HTTP_HEADERS_HANDLE responseHttpHeaders;
    BUFFER_HANDLE responseHttpBody;
    createHttpObjects(&requestHttpHeaders, &requestHttpBody, &responseHttpHeaders, &responseHttpBody);

    auto httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);
    (void)HTTPAPIEX_SetOption(httpapiexhandle, "someOption1", (void*)"3");
    (void)HTTPAPIEX_SetOption(httpapiexhandle, "someOption2", (void*)"33");
    mocks.ResetAllCalls();

    setupAllCallBeforeHTTPsequence(&mocks);
    setupAllCallForHTTPsequence(&mocks, TEST_RELATIVE_PATH, requestHttpHeaders, requestHttpBody, responseHttpHeaders, responseHttpBody); /*contains VECTOR_size*/
    
    STRICT_EXPECTED_CALL(mocks, VECTOR_element(IGNORED_PTR_ARG, 0))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, VECTOR_element(IGNORED_PTR_ARG, 1))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, HTTPAPI_SetOption(IGNORED_PTR_ARG, "someOption1", (void*)"3"))
        .IgnoreArgument(1)
        .IgnoreArgument(3)
        .SetReturn(HTTPAPI_ERROR);
    STRICT_EXPECTED_CALL(mocks, HTTPAPI_SetOption(IGNORED_PTR_ARG, "someOption2", (void*)"33"))
        .IgnoreArgument(1)
        .IgnoreArgument(3)
        .SetReturn(HTTPAPI_ERROR);

    /// act
    auto result = HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, requestHttpHeaders, requestHttpBody, &httpStatusCode, responseHttpHeaders, responseHttpBody);

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_OK, result);
    mocks.AssertActualAndExpectedCalls();

    ///destroy
    destroyHttpObjects(&requestHttpHeaders, &requestHttpBody, &responseHttpHeaders, &responseHttpBody);
    HTTPAPIEX_Destroy(httpapiexhandle);
}

/*Tests_SRS_HTTPAPIEX_02_041: [If creating or updating the pair optionName/value fails then shall return HTTPAPIEX_ERROR.] */
TEST_FUNCTION(HTTPAPIEX_SetOption_fails_when_VECTOR_push_back_fails)
{
    /// arrange
    CHTTPAPIEXMocks mocks;
    auto httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);
    mocks.ResetAllCalls();

    STRICT_EXPECTED_CALL(mocks, HTTPAPI_CloneOption("someOption2", (void*)"33", IGNORED_PTR_ARG))  /*this asks lower HTTPAPI to create a clone of the option*/
        .IgnoreArgument(3);
    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*this is freeing the clone created by HTTPAPI_CloneOption*/
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(sizeof("someOption2"))); /*this is creating a clone of the optionName*/
    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, VECTOR_find_if(IGNORED_PTR_ARG, IGNORED_PTR_ARG, "someOption2")) /*this is looking for hte option to device between update / create*/
        .IgnoreArgument(1)
        .IgnoreArgument(2);

    whenShallVECTOR_push_back_fail = currentVECTOR_push_back_call + 1;
    STRICT_EXPECTED_CALL(mocks, VECTOR_push_back(IGNORED_PTR_ARG, IGNORED_PTR_ARG, 1))
        .IgnoreArgument(1)
        .IgnoreArgument(2);

    /// act
    auto result = HTTPAPIEX_SetOption(httpapiexhandle, "someOption2", (void*)"33");

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_ERROR, result);
    mocks.AssertActualAndExpectedCalls();

    ///destroy
    HTTPAPIEX_Destroy(httpapiexhandle);
}

/*Tests_SRS_HTTPAPIEX_02_041: [If creating or updating the pair optionName/value fails then shall return HTTPAPIEX_ERROR.] */
TEST_FUNCTION(HTTPAPIEX_SetOption_fails_when_gballoc_fails_1)
{
    /// arrange
    CHTTPAPIEXMocks mocks;
    auto httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);
    mocks.ResetAllCalls();

    STRICT_EXPECTED_CALL(mocks, HTTPAPI_CloneOption("someOption2", (void*)"33", IGNORED_PTR_ARG))  /*this asks lower HTTPAPI to create a clone of the option*/
        .IgnoreArgument(3);
    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*this is freeing the clone created by HTTPAPI_CloneOption*/
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, VECTOR_find_if(IGNORED_PTR_ARG, IGNORED_PTR_ARG, "someOption2")) /*this is looking for hte option to device between update / create*/
        .IgnoreArgument(1)
        .IgnoreArgument(2);

    whenShallmalloc_fail = currentmalloc_call + 1;
    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(sizeof("someOption2"))); /*this is creating a clone of the optionName*/

    /// act
    auto result = HTTPAPIEX_SetOption(httpapiexhandle, "someOption2", (void*)"33");

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_ERROR, result);
    mocks.AssertActualAndExpectedCalls();

    ///destroy
    HTTPAPIEX_Destroy(httpapiexhandle);
}

/*Tests_SRS_HTTPAPIEX_02_041: [If creating or updating the pair optionName/value fails then shall return HTTPAPIEX_ERROR.] */
/*Tests_SRS_HTTPAPIEX_02_040: [For all other return values of HTTPAPI_SetOption, HTTPIAPIEX_SetOption shall return HTTPAPIEX_ERROR.] */
TEST_FUNCTION(HTTPAPIEX_SetOption_fails_when_HTTPAPI_SaveOption_fails_3)
{
    /// arrange
    CHTTPAPIEXMocks mocks;
    auto httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);
    mocks.ResetAllCalls();
    
    whenShallHTTPAPI_SaveOption_fail = currentHTTPAPI_SaveOption_call + 1;
    STRICT_EXPECTED_CALL(mocks, HTTPAPI_CloneOption("someOption2", (void*)"33", IGNORED_PTR_ARG))  /*this asks lower HTTPAPI to create a clone of the option*/
        .IgnoreArgument(3)
        .SetReturn(HTTPAPI_ALREADY_INIT); /*some random error code*/

    /// act
    auto result = HTTPAPIEX_SetOption(httpapiexhandle, "someOption2", (void*)"33");

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_ERROR, result);
    mocks.AssertActualAndExpectedCalls();

    ///destroy
    HTTPAPIEX_Destroy(httpapiexhandle);
}

/*Tests_SRS_HTTPAPIEX_02_038: [If HTTPAPI_CloneOption returns HTTPAPI_INVALID_ARG then HTTPAPIEX shall return HTTPAPIEX_INVALID_ARG.]*/
TEST_FUNCTION(HTTPAPIEX_SetOption_fails_when_HTTPAPI_SaveOption_fails_4)
{
    /// arrange
    CHTTPAPIEXMocks mocks;
    auto httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);
    mocks.ResetAllCalls();

    whenShallHTTPAPI_SaveOption_fail = currentHTTPAPI_SaveOption_call + 1;
    STRICT_EXPECTED_CALL(mocks, HTTPAPI_CloneOption("someOption2", (void*)"33", IGNORED_PTR_ARG))  /*this asks lower HTTPAPI to create a clone of the option*/
        .IgnoreArgument(3)
        .SetReturn(HTTPAPI_INVALID_ARG);

    /// act
    auto result = HTTPAPIEX_SetOption(httpapiexhandle, "someOption2", (void*)"33");

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_INVALID_ARG, result);
    mocks.AssertActualAndExpectedCalls();

    ///destroy
    HTTPAPIEX_Destroy(httpapiexhandle);
}

/*Tests_SRS_HTTPAPIEX_02_031: [If HTTPAPI_HANDLE exists then HTTPAPIEX_SetOption shall call HTTPAPI_SetOption passing the same optionName and value and shall return a value conforming to the below table:] */
TEST_FUNCTION(HTTPAPIEX_SetOption_passes_saved_options_to_existing_httpapi_handle_succeeds)
{
    /// arrange
    CHTTPAPIEXMocks mocks;
    auto httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);

    unsigned int httpStatusCode;
    HTTP_HEADERS_HANDLE requestHttpHeaders;
    BUFFER_HANDLE requestHttpBody;
    HTTP_HEADERS_HANDLE responseHttpHeaders;
    BUFFER_HANDLE responseHttpBody;
    createHttpObjects(&requestHttpHeaders, &requestHttpBody, &responseHttpHeaders, &responseHttpBody);
    mocks.ResetAllCalls();

    setupAllCallBeforeHTTPsequence(&mocks);
    setupAllCallForHTTPsequence(&mocks, TEST_RELATIVE_PATH, requestHttpHeaders, requestHttpBody, responseHttpHeaders, responseHttpBody);
    (void)HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, requestHttpHeaders, requestHttpBody, &httpStatusCode, responseHttpHeaders, responseHttpBody);

    STRICT_EXPECTED_CALL(mocks, HTTPAPI_CloneOption("someOption", "3", IGNORED_PTR_ARG))  /*this asks lower HTTPAPI to create a clone of the option*/
        .IgnoreArgument(3);

    STRICT_EXPECTED_CALL(mocks, VECTOR_find_if(IGNORED_PTR_ARG, IGNORED_PTR_ARG, "someOption")) /*this is looking for hte option to device between update / create*/
        .IgnoreArgument(1)
        .IgnoreArgument(2);

    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(sizeof("someOption"))); /*this is creating a clone of the optionName*/

    STRICT_EXPECTED_CALL(mocks, VECTOR_push_back(IGNORED_PTR_ARG, IGNORED_PTR_ARG, 1)) /*this is increasing the array of options by 1*/
        .IgnoreArgument(1)
        .IgnoreArgument(2);

    STRICT_EXPECTED_CALL(mocks, HTTPAPI_SetOption(IGNORED_PTR_ARG, "someOption", "3"))
        .IgnoreArgument(1);

    /// act
    auto result = HTTPAPIEX_SetOption(httpapiexhandle, "someOption", "3");

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_OK, result);
    mocks.AssertActualAndExpectedCalls();

    ///destroy
    destroyHttpObjects(&requestHttpHeaders, &requestHttpBody, &responseHttpHeaders, &responseHttpBody);
    HTTPAPIEX_Destroy(httpapiexhandle);
}

/*Tests_SRS_HTTPAPIEX_02_031: [If HTTPAPI_HANDLE exists then HTTPAPIEX_SetOption shall call HTTPAPI_SetOption passing the same optionName and value and shall return a value conforming to the below table:] */
TEST_FUNCTION(HTTPAPIEX_SetOption_passes_saved_options_to_existing_httpapi_handle_fails_1)
{
    /// arrange
    CHTTPAPIEXMocks mocks;
    auto httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);

    unsigned int httpStatusCode;
    HTTP_HEADERS_HANDLE requestHttpHeaders;
    BUFFER_HANDLE requestHttpBody;
    HTTP_HEADERS_HANDLE responseHttpHeaders;
    BUFFER_HANDLE responseHttpBody;
    createHttpObjects(&requestHttpHeaders, &requestHttpBody, &responseHttpHeaders, &responseHttpBody);
    mocks.ResetAllCalls();

    setupAllCallBeforeHTTPsequence(&mocks);
    setupAllCallForHTTPsequence(&mocks, TEST_RELATIVE_PATH, requestHttpHeaders, requestHttpBody, responseHttpHeaders, responseHttpBody);
    (void)HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, requestHttpHeaders, requestHttpBody, &httpStatusCode, responseHttpHeaders, responseHttpBody);

    STRICT_EXPECTED_CALL(mocks, HTTPAPI_CloneOption("someOption", "3", IGNORED_PTR_ARG))  /*this asks lower HTTPAPI to create a clone of the option*/
        .IgnoreArgument(3);

    STRICT_EXPECTED_CALL(mocks, VECTOR_find_if(IGNORED_PTR_ARG, IGNORED_PTR_ARG, "someOption")) /*this is looking for hte option to device between update / create*/
        .IgnoreArgument(1)
        .IgnoreArgument(2);

    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(sizeof("someOption"))); /*this is creating a clone of the optionName*/

    STRICT_EXPECTED_CALL(mocks, VECTOR_push_back(IGNORED_PTR_ARG, IGNORED_PTR_ARG, 1)) /*this is increasing the array of options by 1*/
        .IgnoreArgument(1)
        .IgnoreArgument(2);

    STRICT_EXPECTED_CALL(mocks, HTTPAPI_SetOption(IGNORED_PTR_ARG, "someOption", "3"))
        .IgnoreArgument(1)
        .SetReturn(HTTPAPI_INVALID_ARG);

    /// act
    auto result = HTTPAPIEX_SetOption(httpapiexhandle, "someOption", "3");

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_INVALID_ARG, result);
    mocks.AssertActualAndExpectedCalls();

    ///destroy
    destroyHttpObjects(&requestHttpHeaders, &requestHttpBody, &responseHttpHeaders, &responseHttpBody);
    HTTPAPIEX_Destroy(httpapiexhandle);
}

/*Tests_SRS_HTTPAPIEX_02_031: [If HTTPAPI_HANDLE exists then HTTPAPIEX_SetOption shall call HTTPAPI_SetOption passing the same optionName and value and shall return a value conforming to the below table:] */
TEST_FUNCTION(HTTPAPIEX_SetOption_passes_saved_options_to_existing_httpapi_handle_fails_2)
{
    /// arrange
    CHTTPAPIEXMocks mocks;
    auto httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);

    unsigned int httpStatusCode;
    HTTP_HEADERS_HANDLE requestHttpHeaders;
    BUFFER_HANDLE requestHttpBody;
    HTTP_HEADERS_HANDLE responseHttpHeaders;
    BUFFER_HANDLE responseHttpBody;
    createHttpObjects(&requestHttpHeaders, &requestHttpBody, &responseHttpHeaders, &responseHttpBody);
    mocks.ResetAllCalls();

    setupAllCallBeforeHTTPsequence(&mocks);
    setupAllCallForHTTPsequence(&mocks, TEST_RELATIVE_PATH, requestHttpHeaders, requestHttpBody, responseHttpHeaders, responseHttpBody);
    (void)HTTPAPIEX_ExecuteRequest(httpapiexhandle, HTTPAPI_REQUEST_PATCH, TEST_RELATIVE_PATH, requestHttpHeaders, requestHttpBody, &httpStatusCode, responseHttpHeaders, responseHttpBody);

    STRICT_EXPECTED_CALL(mocks, HTTPAPI_CloneOption("someOption", "3", IGNORED_PTR_ARG))  /*this asks lower HTTPAPI to create a clone of the option*/
        .IgnoreArgument(3);

    STRICT_EXPECTED_CALL(mocks, VECTOR_find_if(IGNORED_PTR_ARG, IGNORED_PTR_ARG, "someOption")) /*this is looking for hte option to device between update / create*/
        .IgnoreArgument(1)
        .IgnoreArgument(2);

    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(sizeof("someOption"))); /*this is creating a clone of the optionName*/

    STRICT_EXPECTED_CALL(mocks, VECTOR_push_back(IGNORED_PTR_ARG, IGNORED_PTR_ARG, 1)) /*this is increasing the array of options by 1*/
        .IgnoreArgument(1)
        .IgnoreArgument(2);

    STRICT_EXPECTED_CALL(mocks, HTTPAPI_SetOption(IGNORED_PTR_ARG, "someOption", "3"))
        .IgnoreArgument(1)
        .SetReturn(HTTPAPI_ALLOC_FAILED);

    /// act
    auto result = HTTPAPIEX_SetOption(httpapiexhandle, "someOption", "3");
    
    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_ERROR, result);
    mocks.AssertActualAndExpectedCalls();

    ///destroy
    destroyHttpObjects(&requestHttpHeaders, &requestHttpBody, &responseHttpHeaders, &responseHttpBody);
    HTTPAPIEX_Destroy(httpapiexhandle);
}

/*Tests_SRS_HTTPAPIEX_02_039: [If HTTPAPI_CloneOption returns HTTPAPI_OK then HTTPAPIEX_SetOption shall create or update the pair optionName/value.]*/
TEST_FUNCTION(HTTPAPIEX_SetOption_happy_path_on_update_option_value_succeeds)
{
    /// arrange
    CHTTPAPIEXMocks mocks;
    auto httpapiexhandle = HTTPAPIEX_Create(TEST_HOSTNAME);
    HTTPAPIEX_SetOption(httpapiexhandle, "someOption", "333");
    mocks.ResetAllCalls();

    STRICT_EXPECTED_CALL(mocks, HTTPAPI_CloneOption("someOption", "4", IGNORED_PTR_ARG))  /*this asks lower HTTPAPI to create a clone of the option*/
        .IgnoreArgument(3);

    STRICT_EXPECTED_CALL(mocks, VECTOR_find_if(IGNORED_PTR_ARG, IGNORED_PTR_ARG, "someOption")) /*this is looking for hte option to device between update / create*/
        .IgnoreArgument(1)
        .IgnoreArgument(2);

    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*this is free-ing the previos value*/
        .IgnoreArgument(1);

    /// act
    auto result = HTTPAPIEX_SetOption(httpapiexhandle, "someOption", "4");

    ///assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, HTTPAPIEX_OK, result);
    mocks.AssertActualAndExpectedCalls();

    ///destroy
    HTTPAPIEX_Destroy(httpapiexhandle);
}

/*Tests_SRS_HTTPAPIEX_02_043: [If parameter handle is NULL then HTTPAPIEX_Destroy shall take no action.] */
TEST_FUNCTION(HTTPAPIEX_Destroy_with_NULL_argument_does_nothing)
{
    CHTTPAPIEXMocks mocks;
   
    /// act
    HTTPAPIEX_Destroy(NULL);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///destroy
}
END_TEST_SUITE(httpapiex_unittests)
