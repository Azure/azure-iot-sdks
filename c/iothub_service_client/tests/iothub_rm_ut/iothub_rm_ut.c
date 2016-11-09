// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "testrunnerswitcher.h"
#include "umock_c.h"
#include "umocktypes_charptr.h"
#include "umock_c_negative_tests.h"

#define ENABLE_MOCKS
#include "azure_c_shared_utility/httpheaders.h"
#include "azure_c_shared_utility/httpapiex.h"
#include "azure_c_shared_utility/httpapiexsas.h"
#include "azure_c_shared_utility/strings.h"
#include "azure_c_shared_utility/singlylinkedlist.h"
#include "parson.h"
#include "azure_c_shared_utility/crt_abstractions.h"

MOCKABLE_FUNCTION(, JSON_Value*, json_parse_string, const char *, string);
MOCKABLE_FUNCTION(, const char*, json_object_get_string, const JSON_Object *, object, const char *, name);
MOCKABLE_FUNCTION(, JSON_Object*, json_value_get_object, const JSON_Value *, value);
MOCKABLE_FUNCTION(, double, json_object_get_number, const JSON_Object*, object, const char*, name);
MOCKABLE_FUNCTION(, char*, json_serialize_to_string, const JSON_Value*, value);
MOCKABLE_FUNCTION(, void, json_free_serialized_string, char*, string);
MOCKABLE_FUNCTION(, const char*, json_object_dotget_string, const JSON_Object*, object, const char*, name);
MOCKABLE_FUNCTION(, JSON_Status, json_object_set_string, JSON_Object*, object, const char*, name, const char*, string);
MOCKABLE_FUNCTION(, JSON_Status, json_object_dotset_string, JSON_Object*, object, const char*, name, const char*, string);
MOCKABLE_FUNCTION(, JSON_Value*, json_value_init_object);
MOCKABLE_FUNCTION(, JSON_Array*, json_array_get_array, const JSON_Array*, array, size_t, index);
MOCKABLE_FUNCTION(, JSON_Object*, json_array_get_object, const JSON_Array*, array, size_t, index);
MOCKABLE_FUNCTION(, JSON_Array*, json_value_get_array, const JSON_Value*, value);
MOCKABLE_FUNCTION(, size_t, json_array_get_count, const JSON_Array*, array);
MOCKABLE_FUNCTION(, JSON_Status, json_array_clear, JSON_Array*, array);
MOCKABLE_FUNCTION(, JSON_Status, json_object_clear, JSON_Object*, object);
MOCKABLE_FUNCTION(, void, json_value_free, JSON_Value *, value);
#undef ENABLE_MOCKS

static TEST_MUTEX_HANDLE g_testByTest;
static TEST_MUTEX_HANDLE g_dllByDll;

STRING_HANDLE my_STRING_construct(const char* psz)
{
    (void)psz;
    return (STRING_HANDLE)malloc(1);
}

void my_STRING_delete(STRING_HANDLE handle)
{
    free(handle);
}

HTTP_HEADERS_HANDLE my_HTTPHeaders_Alloc(void)
{
    return (HTTP_HEADERS_HANDLE)malloc(1);
}

void my_HTTPHeaders_Free(HTTP_HEADERS_HANDLE handle)
{
    free(handle);
}

BUFFER_HANDLE my_BUFFER_new(void)
{
    return (BUFFER_HANDLE)malloc(1);
}

BUFFER_HANDLE my_BUFFER_create(const unsigned char* source, size_t size)
{
    (void)source, size;
    return (BUFFER_HANDLE)malloc(1);
}

void my_BUFFER_delete(BUFFER_HANDLE handle)
{
    free(handle);
}

HTTPAPIEX_HANDLE my_HTTPAPIEX_Create(const char* hostName)
{
    (void)hostName;
    return (HTTPAPIEX_HANDLE)malloc(1);
}

void my_HTTPAPIEX_Destroy(HTTPAPIEX_HANDLE handle)
{
    free(handle);
}

HTTPAPIEX_SAS_HANDLE my_HTTPAPIEX_SAS_Create(STRING_HANDLE key, STRING_HANDLE uriResource, STRING_HANDLE keyName)
{
    (void)key, uriResource, keyName;
    return (HTTPAPIEX_SAS_HANDLE)malloc(1);
}

void my_HTTPAPIEX_SAS_Destroy(HTTPAPIEX_SAS_HANDLE handle)
{
    free(handle);
}

static void* my_gballoc_malloc(size_t size)
{
    return malloc(size);
}

static void my_gballoc_free(void* ptr)
{
    free(ptr);
}

static int my_mallocAndStrcpy_s(char** destination, const char* source)
{
    char* p = (char*)malloc(2);
    p[0] = source[0];
    p[1] = '\0';
    *destination = p;
    return 0;
}

typedef struct LIST_ITEM_INSTANCE_TAG
{
    const void* item;
    void* next;
} LIST_ITEM_INSTANCE;

typedef struct SINGLYLINKEDLIST_INSTANCE_TAG
{
    LIST_ITEM_INSTANCE* head;
} LIST_INSTANCE;

static SINGLYLINKEDLIST_HANDLE my_list_create(void)
{
    LIST_INSTANCE* result;

    result = (LIST_INSTANCE*)malloc(sizeof(LIST_INSTANCE));
    if (result != NULL)
    {
        result->head = NULL;
    }

    return result;
}

static void my_list_destroy(SINGLYLINKEDLIST_HANDLE list)
{
    if (list != NULL)
    {
        LIST_INSTANCE* list_instance = (LIST_INSTANCE*)list;

        while (list_instance->head != NULL)
        {
            LIST_ITEM_INSTANCE* current_item = list_instance->head;
            list_instance->head = (LIST_ITEM_INSTANCE*)current_item->next;
            free(current_item);
        }

        free(list_instance);
    }
}

static LIST_ITEM_HANDLE my_list_add(SINGLYLINKEDLIST_HANDLE list, const void* item)
{
    LIST_ITEM_INSTANCE* result;

    if ((list == NULL) ||
        (item == NULL))
    {
        result = NULL;
    }
    else
    {
        LIST_INSTANCE* list_instance = (LIST_INSTANCE*)list;
        result = (LIST_ITEM_INSTANCE*)malloc(sizeof(LIST_ITEM_INSTANCE));

        if (result == NULL)
        {
            result = NULL;
        }
        else
        {
            result->next = NULL;
            result->item = item;

            if (list_instance->head == NULL)
            {
                list_instance->head = result;
            }
            else
            {
                LIST_ITEM_INSTANCE* current = list_instance->head;
                while (current->next != NULL)
                {
                    current = (LIST_ITEM_INSTANCE*)current->next;
                }

                current->next = result;
            }
        }
    }

    return result;
}

static int my_list_remove(SINGLYLINKEDLIST_HANDLE list, LIST_ITEM_HANDLE item)
{
    int result;

    if ((list == NULL) ||
        (item == NULL))
    {
        result = __LINE__;
    }
    else
    {
        LIST_INSTANCE* list_instance = (LIST_INSTANCE*)list;
        LIST_ITEM_INSTANCE* current_item = list_instance->head;
        LIST_ITEM_INSTANCE* previous_item = NULL;

        while (current_item != NULL)
        {
            if (current_item == item)
            {
                if (previous_item != NULL)
                {
                    previous_item->next = current_item->next;
                }
                else
                {
                    list_instance->head = (LIST_ITEM_INSTANCE*)current_item->next;
                }

                free(current_item);

                break;
            }
            previous_item = current_item;
            current_item = (LIST_ITEM_INSTANCE*)current_item->next;
        }

        if (current_item == NULL)
        {
            result = __LINE__;
        }
        else
        {
            result = 0;
        }
    }

    return result;
}

static LIST_ITEM_HANDLE my_list_get_head_item(SINGLYLINKEDLIST_HANDLE list)
{
    LIST_ITEM_HANDLE result;

    if (list == NULL)
    {
        result = NULL;
    }
    else
    {
        LIST_INSTANCE* list_instance = (LIST_INSTANCE*)list;

        result = list_instance->head;
    }

    return result;
}

static LIST_ITEM_HANDLE my_list_get_next_item(LIST_ITEM_HANDLE item_handle)
{
    LIST_ITEM_HANDLE result;

    if (item_handle == NULL)
    {
        result = NULL;
    }
    else
    {
        result = (LIST_ITEM_HANDLE)((LIST_ITEM_INSTANCE*)item_handle)->next;
    }

    return result;
}

static const void* my_list_item_get_value(LIST_ITEM_HANDLE item_handle)
{
    const void* result;

    if (item_handle == NULL)
    {
        result = NULL;
    }
    else
    {
        result = ((LIST_ITEM_INSTANCE*)item_handle)->item;
    }

    return result;
}

static LIST_ITEM_HANDLE my_list_find(SINGLYLINKEDLIST_HANDLE list, LIST_MATCH_FUNCTION match_function, const void* match_context)
{
    LIST_ITEM_HANDLE result;

    if ((list == NULL) ||
        (match_function == NULL))
    {
        result = NULL;
    }
    else
    {
        LIST_INSTANCE* list_instance = (LIST_INSTANCE*)list;
        LIST_ITEM_INSTANCE* current = list_instance->head;

        while (current != NULL)
        {
            if (match_function((LIST_ITEM_HANDLE)current, match_context) == true)
            {
                break;
            }

            current = (LIST_ITEM_INSTANCE*)current->next;
        }

        if (current == NULL)
        {
            result = NULL;
        }
        else
        {
            result = current;
        }
    }

    return result;
}

#define ENABLE_MOCKS
#include "azure_c_shared_utility/gballoc.h"
#undef ENABLE_MOCKS

TEST_DEFINE_ENUM_TYPE(HTTPAPI_RESULT, HTTPAPI_RESULT_VALUES);
IMPLEMENT_UMOCK_C_ENUM_TYPE(HTTPAPI_RESULT, HTTPAPI_RESULT_VALUES);
TEST_DEFINE_ENUM_TYPE(HTTPAPIEX_RESULT, HTTPAPIEX_RESULT_VALUES);
IMPLEMENT_UMOCK_C_ENUM_TYPE(HTTPAPIEX_RESULT, HTTPAPIEX_RESULT_VALUES);
TEST_DEFINE_ENUM_TYPE(HTTP_HEADERS_RESULT, HTTP_HEADERS_RESULT_VALUES);
IMPLEMENT_UMOCK_C_ENUM_TYPE(HTTP_HEADERS_RESULT, HTTP_HEADERS_RESULT_VALUES);
TEST_DEFINE_ENUM_TYPE(HTTPAPI_REQUEST_TYPE, HTTPAPI_REQUEST_TYPE_VALUES);
IMPLEMENT_UMOCK_C_ENUM_TYPE(HTTPAPI_REQUEST_TYPE, HTTPAPI_REQUEST_TYPE_VALUES);

#ifdef _MSC_VER
#pragma warning(disable:4505)
#endif

#include "iothub_registrymanager.h"
#include "iothub_service_client_auth.h"

static const char* TEST_DEVICE_ID = "theDeviceId";
static const char* TEST_PRIMARYKEY = "thePrimaryKey";
static const char* TEST_SECONDARYKEY = "theSecondaryKey";
static const char* TEST_GENERATIONID = "theGenerationId";
static const char* TEST_ETAG = "theEtag";

static char* TEST_HOSTNAME = "theHostName";
static char* TEST_IOTHUBNAME = "theIotHubName";
static char* TEST_IOTHUBSUFFIX = "theIotHubSuffix";
static char* TEST_SHAREDACCESSKEY = "theSharedAccessKey";
static char* TEST_SHAREDACCESSKEYNAME = "theSharedAccessKeyName";

static IOTHUB_SERVICE_CLIENT_AUTH TEST_IOTHUB_SERVICE_CLIENT_AUTH;
static IOTHUB_SERVICE_CLIENT_AUTH_HANDLE TEST_IOTHUB_SERVICE_CLIENT_AUTH_HANDLE = &TEST_IOTHUB_SERVICE_CLIENT_AUTH;

static IOTHUB_REGISTRYMANAGER TEST_IOTHUB_REGISTRYMANAGER;
static IOTHUB_REGISTRYMANAGER_HANDLE TEST_IOTHUB_REGISTRYMANAGER_HANDLE = &TEST_IOTHUB_REGISTRYMANAGER;

static IOTHUB_REGISTRY_DEVICE_CREATE TEST_IOTHUB_REGISTRY_DEVICE_CREATE;
static IOTHUB_REGISTRY_DEVICE_UPDATE TEST_IOTHUB_REGISTRY_DEVICE_UPDATE;
static IOTHUB_DEVICE TEST_IOTHUB_DEVICE;
static IOTHUB_REGISTRY_STATISTICS TEST_IOTHUB_REGISTRY_STATISTICS;

static JSON_Value* TEST_JSON_VALUE = (JSON_Value*)0x5050;
static JSON_Object* TEST_JSON_OBJECT = (JSON_Object*)0x5151;
static JSON_Array* TEST_JSON_ARRAY = (JSON_Array*)0x5252;
static JSON_Status TEST_JSON_STATUS = 0;

static char* TEST_CHAR_PTR = "TestString";
static unsigned char* TEST_UNSIGNED_CHAR_PTR = (unsigned char*)"TestString";
static const char* TEST_CONST_CHAR_PTR = "TestConstChar";

static STRING_HANDLE TEST_STRING_HANDLE = (STRING_HANDLE)0x4242;
static BUFFER_HANDLE TEST_BUFFER_HANDLE = (BUFFER_HANDLE)0x4242;
static const SINGLYLINKEDLIST_HANDLE TEST_LIST_HANDLE = (SINGLYLINKEDLIST_HANDLE)0x4242;
static const LIST_ITEM_HANDLE TEST_LIST_ITEM_HANDLE = (LIST_ITEM_HANDLE)0x3434;

static const unsigned int httpStatusCodeOk = 200;
static const unsigned int httpStatusCodeBadRequest = 400;
static const unsigned int httpStatusCodeDeviceExists = 409;
static const HTTPAPIEX_HANDLE TEST_HTTPAPIEX_HANDLE = (HTTPAPIEX_HANDLE)0x4343;
static HTTPAPIEX_SAS_HANDLE TEST_HTTPAPIEX_SAS_HANDLE = (HTTPAPIEX_SAS_HANDLE)0x4444;
static const HTTP_HEADERS_HANDLE TEST_HTTP_HEADERS_HANDLE = (HTTP_HEADERS_HANDLE)0x4545;
static const HTTP_HEADERS_RESULT TEST_HTTP_HEADERS_RESULT = (HTTP_HEADERS_RESULT)0x1;
static HTTPAPIEX_RESULT TEST_HTTPAPIEX_RESULT = (HTTPAPIEX_RESULT)0x1;

static const char* TEST_DEVICE_JSON_KEY_DEVICE_NAME = "deviceId";
static const char* TEST_DEVICE_JSON_KEY_DEVICE_PRIMARY_KEY = "authentication.symmetricKey.primaryKey";
static const char* TEST_DEVICE_JSON_KEY_DEVICE_SECONDARY_KEY = "authentication.symmetricKey.secondaryKey";
static const char* TEST_DEVICE_JSON_KEY_DEVICE_GENERATION_ID = "generationId";
static const char* TEST_DEVICE_JSON_KEY_DEVICE_ETAG = "etag";
static const char* TEST_DEVICE_JSON_KEY_DEVICE_CONNECTIONSTATE = "connectionState";
static const char* TEST_DEVICE_JSON_KEY_DEVICE_CONNECTIONSTATEUPDATEDTIME = "connectionStateUpdatedTime";
static const char* TEST_DEVICE_JSON_KEY_DEVICE_STATUS = "status";
static const char* TEST_DEVICE_JSON_KEY_DEVICE_STATUSREASON = "statusReason";
static const char* TEST_DEVICE_JSON_KEY_DEVICE_STATUSUPDATEDTIME = "statusUpdatedTime";
static const char* TEST_DEVICE_JSON_KEY_DEVICE_LASTACTIVITYTIME = "lastActivityTime";
static const char* TEST_DEVICE_JSON_KEY_DEVICE_CLOUDTODEVICEMESSAGECOUNT = "cloudToDeviceMessageCount";
static const char* TEST_DEVICE_JSON_KEY_DEVICE_ISMANAGED = "isManaged";
static const char* TEST_DEVICE_JSON_KEY_DEVICE_CONFIGURATION = "configuration";
static const char* TEST_DEVICE_JSON_KEY_DEVICE_DEVICEROPERTIES = "deviceProperties";
static const char* TEST_DEVICE_JSON_KEY_DEVICE_SERVICEPROPERTIES = "serviceProperties";

static const char* TEST_DEVICE_JSON_KEY_TOTAL_DEVICECOUNT = "totalDeviceCount";
static const char* TEST_DEVICE_JSON_KEY_ENABLED_DEVICECCOUNT = "enabledDeviceCount";
static const char* TEST_DEVICE_JSON_KEY_DISABLED_DEVICECOUNT = "disabledDeviceCount";

static const char* TEST_CONNECTIONSTATEUPDATEDTIME = "0001-01-01T11:11:11";
static const char* TEST_STATUSREASON = "Because...";
static const char* TEST_STATUSUPDATEDTIME = "0001-01-01T22:22:22";
static const char* TEST_LASTACTIVITYTIME = "0001-01-01T33:33:33";
static const char* TEST_CLOUDTODEVICEMESSAGECOUNT = "42";
static const char* TEST_IS_MANAGED = "true";
static const char* TEST_CONFIGURATION = "theSecondaryKey";
static const char* TEST_DEVICEPROPERTIES = "theSecondaryKey";
static const char* TEST_SERVICEPROPERTIES = "theSecondaryKey";
static const char* TEST_DEVICE_JSON_DEFAULT_VALUE_ENABLED = "Enabled";
static const char* TEST_DEVICE_JSON_DEFAULT_VALUE_CONNECTED = "Connected";

static const char* TEST_HTTP_HEADER_KEY_AUTHORIZATION = "Authorization";
static const char* TEST_HTTP_HEADER_VAL_AUTHORIZATION = " ";
static const char* TEST_HTTP_HEADER_KEY_REQUEST_ID = "Request-Id";
static const char* TEST_HTTP_HEADER_VAL_REQUEST_ID = "1001";
static const char* TEST_HTTP_HEADER_KEY_USER_AGENT = "User-Agent";
static const char* TEST_HTTP_HEADER_VAL_USER_AGENT = "Microsoft.Azure.Devices/1.0.0";
static const char* TEST_HTTP_HEADER_KEY_ACCEPT = "Accept";
static const char* TEST_HTTP_HEADER_VAL_ACCEPT = "application/json";
static const char* TEST_HTTP_HEADER_KEY_CONTENT_TYPE = "Content-Type";
static const char* TEST_HTTP_HEADER_VAL_CONTENT_TYPE = "application/json; charset=utf-8";
static const char* TEST_HTTP_HEADER_KEY_IFMATCH = "If-Match";
static const char* TEST_HTTP_HEADER_VAL_IFMATCH = "*";

DEFINE_ENUM_STRINGS(UMOCK_C_ERROR_CODE, UMOCK_C_ERROR_CODE_VALUES)

static void on_umock_c_error(UMOCK_C_ERROR_CODE error_code)
{
    char temp_str[256];
    (void)snprintf(temp_str, sizeof(temp_str), "umock_c reported error :%s", ENUM_TO_STRING(UMOCK_C_ERROR_CODE, error_code));
    ASSERT_FAIL(temp_str);
}

BEGIN_TEST_SUITE(iothub_registrymanager_ut)

    TEST_SUITE_INITIALIZE(TestClassInitialize)
    {
        TEST_INITIALIZE_MEMORY_DEBUG(g_dllByDll);
        g_testByTest = TEST_MUTEX_CREATE();
        ASSERT_IS_NOT_NULL(g_testByTest);

        umock_c_init(on_umock_c_error);

        int result = umocktypes_charptr_register_types();
        ASSERT_ARE_EQUAL(int, 0, result);

        REGISTER_TYPE(HTTPAPI_RESULT, HTTPAPI_RESULT);
        REGISTER_TYPE(HTTPAPIEX_RESULT, HTTPAPIEX_RESULT);
        REGISTER_TYPE(HTTP_HEADERS_RESULT, HTTP_HEADERS_RESULT);
        REGISTER_TYPE(HTTPAPI_REQUEST_TYPE, HTTPAPI_REQUEST_TYPE);
        REGISTER_UMOCK_ALIAS_TYPE(VECTOR_HANDLE, void*);
        REGISTER_UMOCK_ALIAS_TYPE(const VECTOR_HANDLE, void*);
        REGISTER_UMOCK_ALIAS_TYPE(PREDICATE_FUNCTION, void*);
        REGISTER_UMOCK_ALIAS_TYPE(BUFFER_HANDLE, void*);
        REGISTER_UMOCK_ALIAS_TYPE(STRING_HANDLE, void*);
        REGISTER_UMOCK_ALIAS_TYPE(HTTP_HEADERS_HANDLE, void*);
        REGISTER_UMOCK_ALIAS_TYPE(HTTP_HANDLE, void*);
        REGISTER_UMOCK_ALIAS_TYPE(HTTPAPIEX_HANDLE, void*);
        REGISTER_UMOCK_ALIAS_TYPE(HTTPAPIEX_SAS_HANDLE, void*);

        REGISTER_UMOCK_ALIAS_TYPE(JSON_Value, void*);
        REGISTER_UMOCK_ALIAS_TYPE(JSON_Object, void*);
        REGISTER_UMOCK_ALIAS_TYPE(JSON_Status, int);
        REGISTER_UMOCK_ALIAS_TYPE(SINGLYLINKEDLIST_HANDLE, void*);
        REGISTER_UMOCK_ALIAS_TYPE(LIST_ITEM_HANDLE, void*);

        REGISTER_GLOBAL_MOCK_HOOK(gballoc_malloc, my_gballoc_malloc);
        REGISTER_GLOBAL_MOCK_FAIL_RETURN(gballoc_malloc, NULL);

        REGISTER_GLOBAL_MOCK_HOOK(gballoc_free, my_gballoc_free);

        REGISTER_GLOBAL_MOCK_HOOK(mallocAndStrcpy_s, my_mallocAndStrcpy_s);
        REGISTER_GLOBAL_MOCK_FAIL_RETURN(mallocAndStrcpy_s, 42);

        REGISTER_GLOBAL_MOCK_HOOK(STRING_construct, my_STRING_construct);
        REGISTER_GLOBAL_MOCK_FAIL_RETURN(STRING_construct, NULL);

        REGISTER_GLOBAL_MOCK_HOOK(STRING_delete, my_STRING_delete);

        REGISTER_GLOBAL_MOCK_HOOK(BUFFER_new, my_BUFFER_new);
        REGISTER_GLOBAL_MOCK_FAIL_RETURN(BUFFER_new, NULL);

        REGISTER_GLOBAL_MOCK_HOOK(BUFFER_create, my_BUFFER_create);
        REGISTER_GLOBAL_MOCK_FAIL_RETURN(BUFFER_create, NULL);
        
        REGISTER_GLOBAL_MOCK_HOOK(BUFFER_delete, my_BUFFER_delete);

        REGISTER_GLOBAL_MOCK_HOOK(singlylinkedlist_create, my_list_create);
        REGISTER_GLOBAL_MOCK_FAIL_RETURN(singlylinkedlist_create, NULL);

        REGISTER_GLOBAL_MOCK_HOOK(singlylinkedlist_add, my_list_add);
        REGISTER_GLOBAL_MOCK_FAIL_RETURN(singlylinkedlist_add, NULL);

        REGISTER_GLOBAL_MOCK_HOOK(singlylinkedlist_get_head_item, my_list_get_head_item);
        REGISTER_GLOBAL_MOCK_FAIL_RETURN(singlylinkedlist_get_head_item, NULL);

        REGISTER_GLOBAL_MOCK_HOOK(singlylinkedlist_remove, my_list_remove);
        REGISTER_GLOBAL_MOCK_FAIL_RETURN(singlylinkedlist_remove, __LINE__);

        REGISTER_GLOBAL_MOCK_HOOK(singlylinkedlist_get_next_item, my_list_get_next_item);
        REGISTER_GLOBAL_MOCK_FAIL_RETURN(singlylinkedlist_get_next_item, NULL);
        
        REGISTER_GLOBAL_MOCK_HOOK(singlylinkedlist_find, my_list_find);
        REGISTER_GLOBAL_MOCK_FAIL_RETURN(singlylinkedlist_find, NULL);

        REGISTER_GLOBAL_MOCK_HOOK(singlylinkedlist_item_get_value, my_list_item_get_value);

        REGISTER_GLOBAL_MOCK_HOOK(singlylinkedlist_destroy, my_list_destroy);
        
        REGISTER_GLOBAL_MOCK_HOOK(HTTPHeaders_Alloc, my_HTTPHeaders_Alloc);
        REGISTER_GLOBAL_MOCK_FAIL_RETURN(HTTPHeaders_Alloc, NULL);

        REGISTER_GLOBAL_MOCK_HOOK(HTTPHeaders_Free, my_HTTPHeaders_Free);
        REGISTER_GLOBAL_MOCK_RETURN(HTTPHeaders_AddHeaderNameValuePair, HTTP_HEADERS_OK);
        REGISTER_GLOBAL_MOCK_FAIL_RETURN(HTTPHeaders_AddHeaderNameValuePair, HTTP_HEADERS_ERROR);

        REGISTER_GLOBAL_MOCK_HOOK(HTTPAPIEX_Create, my_HTTPAPIEX_Create);
        REGISTER_GLOBAL_MOCK_FAIL_RETURN(HTTPAPIEX_Create, NULL);

        REGISTER_GLOBAL_MOCK_HOOK(HTTPAPIEX_Destroy, my_HTTPAPIEX_Destroy);

        REGISTER_GLOBAL_MOCK_HOOK(HTTPAPIEX_SAS_Create, my_HTTPAPIEX_SAS_Create);
        REGISTER_GLOBAL_MOCK_FAIL_RETURN(HTTPAPIEX_SAS_Create, NULL);

        REGISTER_GLOBAL_MOCK_HOOK(HTTPAPIEX_SAS_Destroy, my_HTTPAPIEX_SAS_Destroy);

        REGISTER_GLOBAL_MOCK_RETURN(HTTPAPIEX_SAS_ExecuteRequest, HTTPAPIEX_OK);
        REGISTER_GLOBAL_MOCK_FAIL_RETURN(HTTPAPIEX_SAS_ExecuteRequest, HTTPAPIEX_ERROR);

        REGISTER_GLOBAL_MOCK_RETURN(json_value_init_object, TEST_JSON_VALUE);
        REGISTER_GLOBAL_MOCK_FAIL_RETURN(json_value_init_object, NULL);

        REGISTER_GLOBAL_MOCK_RETURN(json_value_get_object, TEST_JSON_OBJECT);
        REGISTER_GLOBAL_MOCK_FAIL_RETURN(json_value_get_object, NULL);

        REGISTER_GLOBAL_MOCK_RETURN(json_object_get_string, TEST_CONST_CHAR_PTR);
        REGISTER_GLOBAL_MOCK_FAIL_RETURN(json_object_get_string, NULL);

        REGISTER_GLOBAL_MOCK_RETURN(json_object_get_number, 42);
        REGISTER_GLOBAL_MOCK_FAIL_RETURN(json_object_get_number, -1);

        REGISTER_GLOBAL_MOCK_RETURN(json_object_dotget_string, TEST_CONST_CHAR_PTR);
        REGISTER_GLOBAL_MOCK_FAIL_RETURN(json_object_dotget_string, NULL);

        REGISTER_GLOBAL_MOCK_RETURN(json_object_set_string, TEST_JSON_STATUS);
        REGISTER_GLOBAL_MOCK_FAIL_RETURN(json_object_set_string, -1);

        REGISTER_GLOBAL_MOCK_RETURN(json_object_dotset_string, TEST_JSON_STATUS);
        REGISTER_GLOBAL_MOCK_FAIL_RETURN(json_object_dotset_string, -1);

        REGISTER_GLOBAL_MOCK_RETURN(json_parse_string, TEST_JSON_VALUE);
        REGISTER_GLOBAL_MOCK_FAIL_RETURN(json_parse_string, NULL);

        REGISTER_GLOBAL_MOCK_RETURN(json_serialize_to_string, TEST_CHAR_PTR);
        REGISTER_GLOBAL_MOCK_FAIL_RETURN(json_serialize_to_string, NULL);

        REGISTER_GLOBAL_MOCK_RETURN(json_array_get_array, TEST_JSON_ARRAY);
        REGISTER_GLOBAL_MOCK_FAIL_RETURN(json_array_get_array, NULL);

        REGISTER_GLOBAL_MOCK_RETURN(json_array_get_object, TEST_JSON_OBJECT);
        REGISTER_GLOBAL_MOCK_FAIL_RETURN(json_array_get_object, NULL);

        REGISTER_GLOBAL_MOCK_RETURN(json_value_get_array, TEST_JSON_ARRAY);
        REGISTER_GLOBAL_MOCK_FAIL_RETURN(json_value_get_array, NULL);

        REGISTER_GLOBAL_MOCK_RETURN(json_object_clear, JSONSuccess);
        REGISTER_GLOBAL_MOCK_FAIL_RETURN(json_object_clear, JSONFailure);

        REGISTER_GLOBAL_MOCK_RETURN(json_array_clear, JSONSuccess);
        REGISTER_GLOBAL_MOCK_FAIL_RETURN(json_array_clear, JSONFailure);
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

        TEST_IOTHUB_SERVICE_CLIENT_AUTH.hostname = TEST_HOSTNAME;
        TEST_IOTHUB_SERVICE_CLIENT_AUTH.iothubName = TEST_IOTHUBNAME;
        TEST_IOTHUB_SERVICE_CLIENT_AUTH.iothubSuffix = TEST_IOTHUBSUFFIX;
        TEST_IOTHUB_SERVICE_CLIENT_AUTH.keyName = TEST_SHAREDACCESSKEYNAME;
        TEST_IOTHUB_SERVICE_CLIENT_AUTH.sharedAccessKey = TEST_SHAREDACCESSKEY;

        TEST_IOTHUB_REGISTRYMANAGER.hostname = TEST_HOSTNAME;
        TEST_IOTHUB_REGISTRYMANAGER.iothubName = TEST_IOTHUBNAME;
        TEST_IOTHUB_REGISTRYMANAGER.iothubSuffix = TEST_IOTHUBSUFFIX;
        TEST_IOTHUB_REGISTRYMANAGER.keyName = TEST_SHAREDACCESSKEYNAME;
        TEST_IOTHUB_REGISTRYMANAGER.sharedAccessKey = TEST_SHAREDACCESSKEY;

        TEST_IOTHUB_REGISTRY_DEVICE_CREATE.deviceId = TEST_DEVICE_ID;
        TEST_IOTHUB_REGISTRY_DEVICE_CREATE.primaryKey = TEST_PRIMARYKEY;
        TEST_IOTHUB_REGISTRY_DEVICE_CREATE.secondaryKey = TEST_SECONDARYKEY;

        TEST_IOTHUB_REGISTRY_DEVICE_UPDATE.deviceId = TEST_DEVICE_ID;
        TEST_IOTHUB_REGISTRY_DEVICE_UPDATE.primaryKey = TEST_PRIMARYKEY;
        TEST_IOTHUB_REGISTRY_DEVICE_UPDATE.secondaryKey = TEST_SECONDARYKEY;
        TEST_IOTHUB_REGISTRY_DEVICE_UPDATE.status = IOTHUB_DEVICE_STATUS_DISABLED;

        TEST_IOTHUB_DEVICE.deviceId = TEST_DEVICE_ID;
        TEST_IOTHUB_DEVICE.primaryKey = TEST_PRIMARYKEY;
        TEST_IOTHUB_DEVICE.secondaryKey = TEST_SECONDARYKEY;
        TEST_IOTHUB_DEVICE.status = IOTHUB_DEVICE_STATUS_DISABLED;
    }

    TEST_FUNCTION_CLEANUP(TestMethodCleanup)
    {
        umock_c_negative_tests_deinit();
        TEST_MUTEX_RELEASE(g_testByTest);
    }

    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_001: [ If the serviceClientHandle input parameter is NULL IoTHubRegistryManager_Create shall return NULL ] */
    TEST_FUNCTION(IoTHubRegistryManager_Create_return_null_if_input_parameter_serviceClientHandle_is_NULL)
    {
        ///arrange

        ///act
        IOTHUB_REGISTRYMANAGER_HANDLE result = IoTHubRegistryManager_Create(NULL);

        ///assert
        ASSERT_IS_NULL(result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    }

    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_084: [ If any member of the serviceClientHandle input parameter is NULL IoTHubRegistryManager_Create shall return NULL ] */
    TEST_FUNCTION(IoTHubRegistryManager_Create_return_null_if_input_parameter_serviceClientHandle_hostName_is_NULL)
    {
        // arrange
        TEST_IOTHUB_SERVICE_CLIENT_AUTH.hostname = NULL;

        // act
        IOTHUB_REGISTRYMANAGER_HANDLE result = IoTHubRegistryManager_Create(TEST_IOTHUB_SERVICE_CLIENT_AUTH_HANDLE);

        // assert
        ASSERT_IS_NULL(result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    }

    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_084: [ If any member of the serviceClientHandle input parameter is NULL IoTHubRegistryManager_Create shall return NULL ] */
    TEST_FUNCTION(IoTHubRegistryManager_Create_return_null_if_input_parameter_serviceClientHandle_iothubName_is_NULL)
    {
        // arrange
        TEST_IOTHUB_SERVICE_CLIENT_AUTH.iothubName = NULL;

        // act
        IOTHUB_REGISTRYMANAGER_HANDLE result = IoTHubRegistryManager_Create(TEST_IOTHUB_SERVICE_CLIENT_AUTH_HANDLE);

        // assert
        ASSERT_IS_NULL(result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    }

    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_084: [ If any member of the serviceClientHandle input parameter is NULL IoTHubRegistryManager_Create shall return NULL ] */
    TEST_FUNCTION(IoTHubRegistryManager_Create_return_null_if_input_parameter_serviceClientHandle_iothubSuffix_is_NULL)
    {
        // arrange
        TEST_IOTHUB_SERVICE_CLIENT_AUTH.iothubSuffix = NULL;

        // act
        IOTHUB_REGISTRYMANAGER_HANDLE result = IoTHubRegistryManager_Create(TEST_IOTHUB_SERVICE_CLIENT_AUTH_HANDLE);

        // assert
        ASSERT_IS_NULL(result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    }

    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_084: [ If any member of the serviceClientHandle input parameter is NULL IoTHubRegistryManager_Create shall return NULL ] */
    TEST_FUNCTION(IoTHubRegistryManager_Create_return_null_if_input_parameter_serviceClientHandle_keyName_is_NULL)
    {
        // arrange
        TEST_IOTHUB_SERVICE_CLIENT_AUTH.keyName = NULL;

        // act
        IOTHUB_REGISTRYMANAGER_HANDLE result = IoTHubRegistryManager_Create(TEST_IOTHUB_SERVICE_CLIENT_AUTH_HANDLE);

        // assert
        ASSERT_IS_NULL(result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    }

    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_084: [ If any member of the serviceClientHandle input parameter is NULL IoTHubRegistryManager_Create shall return NULL ] */
    TEST_FUNCTION(IoTHubRegistryManager_Create_return_null_if_input_parameter_serviceClientHandle_sharedAccessKey_is_NULL)
    {
        // arrange
        TEST_IOTHUB_SERVICE_CLIENT_AUTH.sharedAccessKey = NULL;

        // act
        IOTHUB_REGISTRYMANAGER_HANDLE result = IoTHubRegistryManager_Create(TEST_IOTHUB_SERVICE_CLIENT_AUTH_HANDLE);

        // assert
        ASSERT_IS_NULL(result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    }

    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_002: [ IoTHubRegistryManager_Create shall allocate memory for a new registry manager instance ] */
    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_004: [ If the allocation successful, IoTHubRegistryManager_Create shall create a IOTHUB_REGISTRYMANAGER_HANDLE from the given IOTHUB_REGISTRYMANAGER_AUTH_HANDLE and return with it ] */
    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_085: [ IoTHubRegistryManager_Create shall allocate memory and copy hostName to result->hostName by calling mallocAndStrcpy_s. ]*/
    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_087: [ IoTHubRegistryManager_Create shall allocate memory and copy iothubName to result->iothubName by calling mallocAndStrcpy_s. ]*/
    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_089: [ IoTHubRegistryManager_Create shall allocate memory and copy iothubSuffix to result->iothubSuffix by calling mallocAndStrcpy_s. ]*/
    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_091: [ IoTHubRegistryManager_Create shall allocate memory and copy sharedAccessKey to result->sharedAccessKey by calling mallocAndStrcpy_s. ]*/
    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_093: [ IoTHubRegistryManager_Create shall allocate memory and copy keyName to result->keyName by calling mallocAndStrcpy_s. ]*/
    TEST_FUNCTION(IoTHubRegistryManager_Create_happy_path)
    {
        // arrange
        STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreAllArguments();

        STRICT_EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreAllArguments();

        STRICT_EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreAllArguments();

        STRICT_EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreAllArguments();

        STRICT_EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreAllArguments();

        // act
        IOTHUB_REGISTRYMANAGER_HANDLE result = IoTHubRegistryManager_Create(TEST_IOTHUB_SERVICE_CLIENT_AUTH_HANDLE);

        // assert
        ASSERT_IS_NOT_NULL(result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        if (result != NULL)
        {
            free(result->hostname);
            free(result->iothubName);
            free(result->iothubSuffix);
            free(result->keyName);
            free(result->sharedAccessKey);
            free(result);
            result = NULL;
        }
    }

    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_003: [ If the allocation failed, IoTHubRegistryManager_Create shall return NULL ] */
    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_086: [ If the mallocAndStrcpy_s fails, IoTHubRegistryManager_Create shall do clean up and return NULL. ]*/
    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_088: [ If the mallocAndStrcpy_s fails, IoTHubRegistryManager_Create shall do clean up and return NULL. ]*/
    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_090: [ If the mallocAndStrcpy_s fails, IoTHubRegistryManager_Create shall do clean up and return NULL. ]*/
    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_092: [ If the mallocAndStrcpy_s fails, IoTHubRegistryManager_Create shall do clean up and return NULL. ]*/
    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_094: [ If the mallocAndStrcpy_s fails, IoTHubRegistryManager_Create shall do clean up and return NULL. ]*/
    TEST_FUNCTION(IoTHubRegistryManager_Create_non_happy_path)
    {
        // arrange
        int umockc_result = umock_c_negative_tests_init();
        ASSERT_ARE_EQUAL(int, 0, umockc_result);

        STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, (const char*)(TEST_IOTHUB_SERVICE_CLIENT_AUTH_HANDLE->hostname)))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, (const char*)(TEST_IOTHUB_SERVICE_CLIENT_AUTH_HANDLE->iothubName)))
            .IgnoreArgument(1);
        
        STRICT_EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, (const char*)(TEST_IOTHUB_SERVICE_CLIENT_AUTH_HANDLE->iothubSuffix)))
            .IgnoreArgument(1);
        
        STRICT_EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, (const char*)(TEST_IOTHUB_SERVICE_CLIENT_AUTH_HANDLE->sharedAccessKey)))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, (const char*)(TEST_IOTHUB_SERVICE_CLIENT_AUTH_HANDLE->keyName)))
            .IgnoreArgument(1);

        
        umock_c_negative_tests_snapshot();

        ///act
        for (size_t i = 0; i < umock_c_negative_tests_call_count(); i++)
        {
            /// arrange
            umock_c_negative_tests_reset();
            umock_c_negative_tests_fail_call(i);

            /// act
            IOTHUB_REGISTRYMANAGER_HANDLE result = IoTHubRegistryManager_Create(TEST_IOTHUB_SERVICE_CLIENT_AUTH_HANDLE);

            /// assert
            ASSERT_ARE_EQUAL(void_ptr, NULL, result);

            ///cleanup
        }
        umock_c_negative_tests_deinit();
    }
    
    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_005: [ If the registryManagerHandle input parameter is NULL IoTHubRegistryManager_Destroy shall return ] */
    TEST_FUNCTION(IoTHubRegistryManager_Destroy_return_if_input_parameter_registryManagerHandle_is_NULL)
    {
        // arrange

        // act
        IoTHubRegistryManager_Destroy(NULL);

        // assert
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    }

    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_006 : [ If the registryManagerHandle input parameter is not NULL IoTHubRegistryManager_Destroy shall free the memory of it and return ] */
    TEST_FUNCTION(IoTHubRegistryManager_Destroy_do_clean_up_and_return_if_input_parameter_registryManagerHandle_is_not_NULL)
    {
        // arrange
        IOTHUB_REGISTRYMANAGER_HANDLE handle = IoTHubRegistryManager_Create(TEST_IOTHUB_SERVICE_CLIENT_AUTH_HANDLE);

        umock_c_reset_all_calls();

        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        // act
        IoTHubRegistryManager_Destroy(handle);

        // assert
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    }
    
    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_007: [ IoTHubRegistryManager_CreateDevice shall verify the input parameters and if any of them are NULL then return IOTHUB_REGISTRYMANAGER_INVALID_ARG ]*/
    TEST_FUNCTION(IoTHubRegistryManager_CreateDevice_return_IOTHUB_REGISTRYMANAGER_INVALID_ARG_if_input_parameter_registryManagerHandle_is_NULL)
    {
        // arrange

        // act
        IOTHUB_DEVICE deviceInfo;
        (void)memset(&deviceInfo, 0, sizeof(IOTHUB_DEVICE));
        IOTHUB_REGISTRYMANAGER_RESULT result = IoTHubRegistryManager_CreateDevice(NULL, &TEST_IOTHUB_REGISTRY_DEVICE_CREATE, &deviceInfo);

        // assert
        ASSERT_ARE_EQUAL(int, IOTHUB_REGISTRYMANAGER_INVALID_ARG, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    }

    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_007: [ IoTHubRegistryManager_CreateDevice shall verify the input parameters and if any of them are NULL then return IOTHUB_REGISTRYMANAGER_INVALID_ARG ]*/
    TEST_FUNCTION(IoTHubRegistryManager_CreateDevice_return_IOTHUB_REGISTRYMANAGER_INVALID_ARG_if_input_parameter_deviceCreateInfo_is_NULL)
    {
        // arrange

        // act
        IOTHUB_DEVICE deviceInfo;
        (void)memset(&deviceInfo, 0, sizeof(IOTHUB_DEVICE));
        IOTHUB_REGISTRYMANAGER_RESULT result = IoTHubRegistryManager_CreateDevice(TEST_IOTHUB_REGISTRYMANAGER_HANDLE, NULL, &deviceInfo);

        // assert
        ASSERT_ARE_EQUAL(int, IOTHUB_REGISTRYMANAGER_INVALID_ARG, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    }

    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_007: [ IoTHubRegistryManager_CreateDevice shall verify the input parameters and if any of them are NULL then return IOTHUB_REGISTRYMANAGER_INVALID_ARG ]*/
    TEST_FUNCTION(IoTHubRegistryManager_CreateDevice_return_IOTHUB_REGISTRYMANAGER_INVALID_ARG_if_input_parameter_deviceInfo_is_NULL)
    {
        // arrange

        // act
        IOTHUB_REGISTRYMANAGER_RESULT result = IoTHubRegistryManager_CreateDevice(TEST_IOTHUB_REGISTRYMANAGER_HANDLE, &TEST_IOTHUB_REGISTRY_DEVICE_CREATE, NULL);

        // assert
        ASSERT_ARE_EQUAL(int, IOTHUB_REGISTRYMANAGER_INVALID_ARG, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    }

    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_008: [ IoTHubRegistryManager_CreateDevice shall verify the deviceCreateInfo->deviceId input parameter and if it is NULL then return IOTHUB_REGISTRYMANAGER_INVALID_ARG ]*/
    TEST_FUNCTION(IoTHubRegistryManager_CreateDevice_return_IOTHUB_REGISTRYMANAGER_INVALID_ARG_if_input_parameter_deviceCreateInfo_deviceId_is_NULL)
    {
        // arrange

        // act
        IOTHUB_DEVICE deviceInfo;
        (void)memset(&deviceInfo, 0, sizeof(IOTHUB_DEVICE));

        TEST_IOTHUB_REGISTRY_DEVICE_CREATE.deviceId = NULL;
        IOTHUB_REGISTRYMANAGER_RESULT result = IoTHubRegistryManager_CreateDevice(TEST_IOTHUB_REGISTRYMANAGER_HANDLE, &TEST_IOTHUB_REGISTRY_DEVICE_CREATE, &deviceInfo);

        // assert
        ASSERT_ARE_EQUAL(int, IOTHUB_REGISTRYMANAGER_INVALID_ARG, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    }

    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_009: [ IoTHubRegistryManager_CreateDevice shall verify the deviceCreateInfo->deviceId input parameter and if it contains space(s) then return IOTHUB_REGISTRYMANAGER_INVALID_ARG ] */
    TEST_FUNCTION(IoTHubRegistryManager_CreateDevice_return_IOTHUB_REGISTRYMANAGER_INVALID_ARG_if_input_parameter_deviceCreateInfo_deviceId_contains_space)
    {
        // arrange

        // act
        IOTHUB_DEVICE deviceInfo;
        (void)memset(&deviceInfo, 0, sizeof(IOTHUB_DEVICE));

        TEST_IOTHUB_REGISTRY_DEVICE_CREATE.deviceId = "aaa bbb";
        IOTHUB_REGISTRYMANAGER_RESULT result = IoTHubRegistryManager_CreateDevice(TEST_IOTHUB_REGISTRYMANAGER_HANDLE, &TEST_IOTHUB_REGISTRY_DEVICE_CREATE, &deviceInfo);

        // assert
        ASSERT_ARE_EQUAL(int, IOTHUB_REGISTRYMANAGER_INVALID_ARG, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    }

    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_010: [ IoTHubRegistryManager_CreateDevice shall create a flat "key1:value2,key2:value2..." JSON representation from the given deviceCreateInfo parameter using the following parson APIs: json_value_init_object, json_value_get_object, json_object_set_string, json_object_dotset_string ]*/
    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_012: [ IoTHubRegistryManager_CreateDevice shall set the following key/value pairs to default value: etag:null,connectionState:Disconnected,status:enabled,statusReason:null,connectionStateUpdatedTime:0001-01-01T00:00:00,statusUpdatedTime:0001-01-01T00:00:00,lastActivityTime:0001-01-01T00:00:00,authentication:{symmetricKey:{primaryKey:null,secondaryKey:null}} ]*/
    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_024: [ If the deviceInfo out parameter is not NULL IoTHubRegistryManager_CreateDevice shall save the received deviceInfo to the out parameter and return IOTHUB_REGISTRYMANAGER_OK ]*/
    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_100: [ IoTHubRegistryManager_CreateDevice shall do clean up before return ] */
    TEST_FUNCTION(IoTHubRegistryManager_CreateDevice_happy_path_status_code_200)
    {
        // arrange
        STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(json_value_init_object())
            .SetReturn(TEST_JSON_VALUE);

        STRICT_EXPECTED_CALL(json_value_get_object(TEST_JSON_VALUE))
            .SetReturn(TEST_JSON_OBJECT);

        STRICT_EXPECTED_CALL(json_object_set_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_NAME, TEST_DEVICE_ID));
        STRICT_EXPECTED_CALL(json_object_dotset_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_PRIMARY_KEY, TEST_PRIMARYKEY));
        STRICT_EXPECTED_CALL(json_object_dotset_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_SECONDARY_KEY, TEST_SECONDARYKEY));
        STRICT_EXPECTED_CALL(json_serialize_to_string(TEST_JSON_VALUE))
            .SetReturn(TEST_CHAR_PTR);

        STRICT_EXPECTED_CALL(BUFFER_create(IGNORED_PTR_ARG, IGNORED_NUM_ARG))
            .IgnoreAllArguments();

        STRICT_EXPECTED_CALL(json_free_serialized_string(TEST_CHAR_PTR));
        STRICT_EXPECTED_CALL(json_object_clear(TEST_JSON_OBJECT));
        STRICT_EXPECTED_CALL(json_value_free(TEST_JSON_VALUE));

        STRICT_EXPECTED_CALL(BUFFER_new());

        STRICT_EXPECTED_CALL(STRING_construct(TEST_HOSTNAME));
        STRICT_EXPECTED_CALL(STRING_construct(TEST_SHAREDACCESSKEY));
        STRICT_EXPECTED_CALL(STRING_construct(TEST_SHAREDACCESSKEYNAME));

        STRICT_EXPECTED_CALL(HTTPHeaders_Alloc());
        STRICT_EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, TEST_HTTP_HEADER_KEY_AUTHORIZATION, TEST_HTTP_HEADER_VAL_AUTHORIZATION))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, TEST_HTTP_HEADER_KEY_REQUEST_ID, TEST_HTTP_HEADER_VAL_REQUEST_ID))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, TEST_HTTP_HEADER_KEY_USER_AGENT, TEST_HTTP_HEADER_VAL_USER_AGENT))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, TEST_HTTP_HEADER_KEY_ACCEPT, TEST_HTTP_HEADER_VAL_ACCEPT))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, TEST_HTTP_HEADER_KEY_CONTENT_TYPE, TEST_HTTP_HEADER_VAL_CONTENT_TYPE))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(HTTPAPIEX_SAS_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreAllArguments();
        STRICT_EXPECTED_CALL(HTTPAPIEX_Create(TEST_HOSTNAME));

        STRICT_EXPECTED_CALL(HTTPAPIEX_SAS_ExecuteRequest(IGNORED_PTR_ARG, IGNORED_PTR_ARG, HTTPAPI_REQUEST_PUT, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(2)
            .IgnoreArgument(4)
            .IgnoreArgument(5)
            .IgnoreArgument(6)
            .IgnoreArgument(7)
            .IgnoreArgument(8)
            .IgnoreArgument(9)
            .CopyOutArgumentBuffer_statusCode(&httpStatusCodeOk, sizeof(httpStatusCodeOk))
            .SetReturn(HTTPAPIEX_OK);

        STRICT_EXPECTED_CALL(HTTPHeaders_Free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPAPIEX_Destroy(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPAPIEX_SAS_Destroy(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(BUFFER_u_char(IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .SetReturn(TEST_UNSIGNED_CHAR_PTR);

        STRICT_EXPECTED_CALL(json_parse_string(IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .SetReturn(TEST_JSON_VALUE);
        STRICT_EXPECTED_CALL(json_value_get_object(TEST_JSON_VALUE))
            .SetReturn(TEST_JSON_OBJECT);

        STRICT_EXPECTED_CALL(json_object_get_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_NAME))
            .SetReturn(TEST_DEVICE_ID);
        STRICT_EXPECTED_CALL(json_object_dotget_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_PRIMARY_KEY))
            .SetReturn(TEST_PRIMARYKEY);
        STRICT_EXPECTED_CALL(json_object_dotget_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_SECONDARY_KEY))
            .SetReturn(TEST_SECONDARYKEY);
        STRICT_EXPECTED_CALL(json_object_get_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_GENERATION_ID))
            .SetReturn(TEST_GENERATIONID);
        STRICT_EXPECTED_CALL(json_object_get_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_ETAG))
            .SetReturn(TEST_ETAG);

        STRICT_EXPECTED_CALL(json_object_get_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_CONNECTIONSTATE))
            .SetReturn(TEST_DEVICE_JSON_DEFAULT_VALUE_CONNECTED);
        STRICT_EXPECTED_CALL(json_object_get_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_CONNECTIONSTATEUPDATEDTIME))
            .SetReturn(TEST_CONNECTIONSTATEUPDATEDTIME);

        STRICT_EXPECTED_CALL(json_object_get_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_STATUS))
            .SetReturn(TEST_DEVICE_JSON_DEFAULT_VALUE_ENABLED);
        STRICT_EXPECTED_CALL(json_object_get_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_STATUSREASON))
            .SetReturn(TEST_STATUSREASON);
        STRICT_EXPECTED_CALL(json_object_get_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_STATUSUPDATEDTIME))
            .SetReturn(TEST_STATUSUPDATEDTIME);

        STRICT_EXPECTED_CALL(json_object_get_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_LASTACTIVITYTIME))
            .SetReturn(TEST_LASTACTIVITYTIME);
        STRICT_EXPECTED_CALL(json_object_get_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_CLOUDTODEVICEMESSAGECOUNT))
            .SetReturn(TEST_CLOUDTODEVICEMESSAGECOUNT);

        STRICT_EXPECTED_CALL(json_object_get_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_ISMANAGED))
            .SetReturn(TEST_IS_MANAGED);
        STRICT_EXPECTED_CALL(json_object_get_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_CONFIGURATION))
            .SetReturn(TEST_CONFIGURATION);
        STRICT_EXPECTED_CALL(json_object_get_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_DEVICEROPERTIES))
            .SetReturn(TEST_DEVICEPROPERTIES);
        STRICT_EXPECTED_CALL(json_object_get_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_SERVICEPROPERTIES))
            .SetReturn(TEST_SERVICEPROPERTIES);

        STRICT_EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreAllArguments();
        STRICT_EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreAllArguments();
        STRICT_EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreAllArguments();
        STRICT_EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreAllArguments();
        STRICT_EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreAllArguments();
        STRICT_EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreAllArguments();
        STRICT_EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreAllArguments();
        STRICT_EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreAllArguments();
        STRICT_EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreAllArguments();
        STRICT_EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreAllArguments();
        STRICT_EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreAllArguments();
        STRICT_EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreAllArguments();

        STRICT_EXPECTED_CALL(json_object_clear(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(json_value_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(BUFFER_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(BUFFER_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        // act
        IOTHUB_DEVICE deviceInfo;
        IOTHUB_REGISTRYMANAGER_RESULT result = IoTHubRegistryManager_CreateDevice(TEST_IOTHUB_REGISTRYMANAGER_HANDLE, &TEST_IOTHUB_REGISTRY_DEVICE_CREATE, &deviceInfo);

        // assert
        ASSERT_ARE_EQUAL(int, IOTHUB_REGISTRYMANAGER_OK, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        // cleanup
        free((void*)deviceInfo.deviceId);
        free((void*)deviceInfo.primaryKey);
        free((void*)deviceInfo.secondaryKey);
        free((void*)deviceInfo.generationId);
        free((void*)deviceInfo.eTag);
        free((void*)deviceInfo.connectionStateUpdatedTime);
        free((void*)deviceInfo.statusReason);
        free((void*)deviceInfo.statusUpdatedTime);
        free((void*)deviceInfo.lastActivityTime);
        free((void*)deviceInfo.configuration);
        free((void*)deviceInfo.deviceProperties);
        free((void*)deviceInfo.serviceProperties);
    }

    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_020: [ IoTHubRegistryManager_CreateDevice shall verify the received HTTP status code and if it is 409 then return IOTHUB_REGISTRYMANAGER_DEVICE_EXIST ]*/
    TEST_FUNCTION(IoTHubRegistryManager_CreateDevice_happy_path_status_code_409)
    {
        // arrange
        STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(json_value_init_object())
            .SetReturn(TEST_JSON_VALUE);

        STRICT_EXPECTED_CALL(json_value_get_object(TEST_JSON_VALUE))
            .SetReturn(TEST_JSON_OBJECT);

        STRICT_EXPECTED_CALL(json_object_set_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_NAME, TEST_DEVICE_ID));
        STRICT_EXPECTED_CALL(json_object_dotset_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_PRIMARY_KEY, TEST_PRIMARYKEY));
        STRICT_EXPECTED_CALL(json_object_dotset_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_SECONDARY_KEY, TEST_SECONDARYKEY));
        STRICT_EXPECTED_CALL(json_serialize_to_string(TEST_JSON_VALUE))
            .SetReturn(TEST_CHAR_PTR);

        STRICT_EXPECTED_CALL(BUFFER_create(IGNORED_PTR_ARG, IGNORED_NUM_ARG))
            .IgnoreAllArguments();

        STRICT_EXPECTED_CALL(json_free_serialized_string(TEST_CHAR_PTR));
        STRICT_EXPECTED_CALL(json_object_clear(IGNORED_NUM_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(json_value_free(IGNORED_NUM_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(BUFFER_new());

        STRICT_EXPECTED_CALL(STRING_construct(TEST_HOSTNAME));
        STRICT_EXPECTED_CALL(STRING_construct(TEST_SHAREDACCESSKEY));
        STRICT_EXPECTED_CALL(STRING_construct(TEST_SHAREDACCESSKEYNAME));

        STRICT_EXPECTED_CALL(HTTPHeaders_Alloc());
        STRICT_EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, TEST_HTTP_HEADER_KEY_AUTHORIZATION, TEST_HTTP_HEADER_VAL_AUTHORIZATION))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, TEST_HTTP_HEADER_KEY_REQUEST_ID, TEST_HTTP_HEADER_VAL_REQUEST_ID))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, TEST_HTTP_HEADER_KEY_USER_AGENT, TEST_HTTP_HEADER_VAL_USER_AGENT))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, TEST_HTTP_HEADER_KEY_ACCEPT, TEST_HTTP_HEADER_VAL_ACCEPT))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, TEST_HTTP_HEADER_KEY_CONTENT_TYPE, TEST_HTTP_HEADER_VAL_CONTENT_TYPE))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(HTTPAPIEX_SAS_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreAllArguments();
        STRICT_EXPECTED_CALL(HTTPAPIEX_Create(TEST_HOSTNAME));

        STRICT_EXPECTED_CALL(HTTPAPIEX_SAS_ExecuteRequest(IGNORED_PTR_ARG, IGNORED_PTR_ARG, HTTPAPI_REQUEST_PUT, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(2)
            .IgnoreArgument(4)
            .IgnoreArgument(5)
            .IgnoreArgument(6)
            .IgnoreArgument(7)
            .IgnoreArgument(8)
            .IgnoreArgument(9)
            .CopyOutArgumentBuffer_statusCode(&httpStatusCodeDeviceExists, sizeof(httpStatusCodeDeviceExists))
            .SetReturn(HTTPAPIEX_OK);

        STRICT_EXPECTED_CALL(HTTPHeaders_Free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPAPIEX_Destroy(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPAPIEX_SAS_Destroy(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(BUFFER_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(BUFFER_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        // act
        IOTHUB_DEVICE deviceInfo;
        (void)memset(&deviceInfo, 0, sizeof(IOTHUB_DEVICE));
        IOTHUB_REGISTRYMANAGER_RESULT result = IoTHubRegistryManager_CreateDevice(TEST_IOTHUB_REGISTRYMANAGER_HANDLE, &TEST_IOTHUB_REGISTRY_DEVICE_CREATE, &deviceInfo);

        // assert
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ASSERT_ARE_EQUAL(int, IOTHUB_REGISTRYMANAGER_DEVICE_EXIST, result);
    }

    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_021: [ IoTHubRegistryManager_CreateDevice shall verify the received HTTP status code and if it is greater than 300 then return IOTHUB_REGISTRYMANAGER_HTTP_STATUS_ERROR ]*/
    TEST_FUNCTION(IoTHubRegistryManager_CreateDevice_happy_path_status_code_400)
    {
        // arrange
        STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(json_value_init_object())
            .SetReturn(TEST_JSON_VALUE);

        STRICT_EXPECTED_CALL(json_value_get_object(TEST_JSON_VALUE))
            .SetReturn(TEST_JSON_OBJECT);

        STRICT_EXPECTED_CALL(json_object_set_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_NAME, TEST_DEVICE_ID));
        STRICT_EXPECTED_CALL(json_object_dotset_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_PRIMARY_KEY, TEST_PRIMARYKEY));
        STRICT_EXPECTED_CALL(json_object_dotset_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_SECONDARY_KEY, TEST_SECONDARYKEY));
        STRICT_EXPECTED_CALL(json_serialize_to_string(TEST_JSON_VALUE))
            .SetReturn(TEST_CHAR_PTR);

        STRICT_EXPECTED_CALL(BUFFER_create(IGNORED_PTR_ARG, IGNORED_NUM_ARG))
            .IgnoreAllArguments();

        STRICT_EXPECTED_CALL(json_free_serialized_string(TEST_CHAR_PTR));
        STRICT_EXPECTED_CALL(json_object_clear(IGNORED_NUM_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(json_value_free(IGNORED_NUM_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(BUFFER_new());

        STRICT_EXPECTED_CALL(STRING_construct(TEST_HOSTNAME));
        STRICT_EXPECTED_CALL(STRING_construct(TEST_SHAREDACCESSKEY));
        STRICT_EXPECTED_CALL(STRING_construct(TEST_SHAREDACCESSKEYNAME));

        STRICT_EXPECTED_CALL(HTTPHeaders_Alloc());
        STRICT_EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, TEST_HTTP_HEADER_KEY_AUTHORIZATION, TEST_HTTP_HEADER_VAL_AUTHORIZATION))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, TEST_HTTP_HEADER_KEY_REQUEST_ID, TEST_HTTP_HEADER_VAL_REQUEST_ID))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, TEST_HTTP_HEADER_KEY_USER_AGENT, TEST_HTTP_HEADER_VAL_USER_AGENT))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, TEST_HTTP_HEADER_KEY_ACCEPT, TEST_HTTP_HEADER_VAL_ACCEPT))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, TEST_HTTP_HEADER_KEY_CONTENT_TYPE, TEST_HTTP_HEADER_VAL_CONTENT_TYPE))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(HTTPAPIEX_SAS_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreAllArguments();
        STRICT_EXPECTED_CALL(HTTPAPIEX_Create(TEST_HOSTNAME));

        STRICT_EXPECTED_CALL(HTTPAPIEX_SAS_ExecuteRequest(IGNORED_PTR_ARG, IGNORED_PTR_ARG, HTTPAPI_REQUEST_PUT, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(2)
            .IgnoreArgument(4)
            .IgnoreArgument(5)
            .IgnoreArgument(6)
            .IgnoreArgument(7)
            .IgnoreArgument(8)
            .IgnoreArgument(9)
            .CopyOutArgumentBuffer_statusCode(&httpStatusCodeBadRequest, sizeof(httpStatusCodeBadRequest))
            .SetReturn(HTTPAPIEX_OK);

        STRICT_EXPECTED_CALL(HTTPHeaders_Free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPAPIEX_Destroy(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPAPIEX_SAS_Destroy(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(BUFFER_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(BUFFER_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        // act
        IOTHUB_DEVICE deviceInfo;
        (void)memset(&deviceInfo, 0, sizeof(IOTHUB_DEVICE));
        IOTHUB_REGISTRYMANAGER_RESULT result = IoTHubRegistryManager_CreateDevice(TEST_IOTHUB_REGISTRYMANAGER_HANDLE, &TEST_IOTHUB_REGISTRY_DEVICE_CREATE, &deviceInfo);

        // assert
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ASSERT_ARE_EQUAL(int, IOTHUB_REGISTRYMANAGER_HTTP_STATUS_ERROR, result);
    }

    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_013: [ IoTHubRegistryManager_CreateDevice shall return IOTHUB_REGISTRYMANAGER_ERROR_CREATING_JSON if the JSON creation failed  ] */
    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_019: [ If any of the HTTPAPI call fails IoTHubRegistryManager_CreateDevice shall fail and return IOTHUB_REGISTRYMANAGER_HTTPAPI_ERROR ] */
    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_023: [ If the JSON parsing failed, IoTHubRegistryManager_CreateDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR ]*/
    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_099: [ If any of the call fails during the HTTP creation IoTHubRegistryManager_CreateDevice shall fail and return IOTHUB_REGISTRYMANAGER_ERROR ] */
    TEST_FUNCTION(IoTHubRegistryManager_CreateDevice_non_happy_path)
    {
        ///arrange
        int umockc_result = umock_c_negative_tests_init();
        ASSERT_ARE_EQUAL(int, 0, umockc_result);

        STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(json_value_init_object())
            .SetReturn(TEST_JSON_VALUE);

        STRICT_EXPECTED_CALL(json_value_get_object(TEST_JSON_VALUE))
            .SetReturn(TEST_JSON_OBJECT);

        STRICT_EXPECTED_CALL(json_object_set_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_NAME, TEST_DEVICE_ID));
        STRICT_EXPECTED_CALL(json_object_dotset_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_PRIMARY_KEY, TEST_PRIMARYKEY));
        STRICT_EXPECTED_CALL(json_object_dotset_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_SECONDARY_KEY, TEST_SECONDARYKEY));
        STRICT_EXPECTED_CALL(json_serialize_to_string(TEST_JSON_VALUE))
            .SetReturn(TEST_CHAR_PTR);

        STRICT_EXPECTED_CALL(BUFFER_create(IGNORED_PTR_ARG, IGNORED_NUM_ARG))
            .IgnoreAllArguments();

        STRICT_EXPECTED_CALL(json_free_serialized_string(TEST_CHAR_PTR));
        STRICT_EXPECTED_CALL(json_object_clear(IGNORED_NUM_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(json_value_free(IGNORED_NUM_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(BUFFER_new());

        STRICT_EXPECTED_CALL(STRING_construct(TEST_HOSTNAME));
        STRICT_EXPECTED_CALL(STRING_construct(TEST_SHAREDACCESSKEY));
        STRICT_EXPECTED_CALL(STRING_construct(TEST_SHAREDACCESSKEYNAME));

        STRICT_EXPECTED_CALL(HTTPHeaders_Alloc());
        STRICT_EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, TEST_HTTP_HEADER_KEY_AUTHORIZATION, TEST_HTTP_HEADER_VAL_AUTHORIZATION))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, TEST_HTTP_HEADER_KEY_REQUEST_ID, TEST_HTTP_HEADER_VAL_REQUEST_ID))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, TEST_HTTP_HEADER_KEY_USER_AGENT, TEST_HTTP_HEADER_VAL_USER_AGENT))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, TEST_HTTP_HEADER_KEY_ACCEPT, TEST_HTTP_HEADER_VAL_ACCEPT))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, TEST_HTTP_HEADER_KEY_CONTENT_TYPE, TEST_HTTP_HEADER_VAL_CONTENT_TYPE))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(HTTPAPIEX_SAS_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreAllArguments();
        STRICT_EXPECTED_CALL(HTTPAPIEX_Create(TEST_HOSTNAME));

        STRICT_EXPECTED_CALL(HTTPAPIEX_SAS_ExecuteRequest(IGNORED_PTR_ARG, IGNORED_PTR_ARG, HTTPAPI_REQUEST_PUT, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(2)
            .IgnoreArgument(4)
            .IgnoreArgument(5)
            .IgnoreArgument(6)
            .IgnoreArgument(7)
            .IgnoreArgument(8)
            .IgnoreArgument(9)
            .CopyOutArgumentBuffer_statusCode(&httpStatusCodeBadRequest, sizeof(httpStatusCodeBadRequest))
            .SetReturn(HTTPAPIEX_OK);

        STRICT_EXPECTED_CALL(HTTPHeaders_Free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPAPIEX_Destroy(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPAPIEX_SAS_Destroy(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(BUFFER_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(BUFFER_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        umock_c_negative_tests_snapshot();

        ///act
        for (size_t i = 0; i < umock_c_negative_tests_call_count(); i++)
        {
            /// arrange
            umock_c_negative_tests_reset();
            umock_c_negative_tests_fail_call(i);

            /// act
            if (
                (i != 8) && /*json_free_serialized_string*/
                (i != 10) && /*json_value_free*/
                (i != 24) && /*HTTPHeaders_Free*/
                (i != 25) && /*HTTPAPIEX_Destroy*/
                (i != 26) && /*HTTPAPIEX_SAS_Destroy*/
                (i != 27) && /*STRING_delete*/
                (i != 28) && /*STRING_delete*/
                (i != 29) && /*STRING_delete*/
                (i != 30) && /*BUFFER_delete*/
                (i != 31) && /*BUFFER_delete*/
                (i != 32) /*gballoc_free*/
                )
            {
                IOTHUB_DEVICE deviceInfo;
                (void)memset(&deviceInfo, 0, sizeof(IOTHUB_DEVICE));
                IOTHUB_REGISTRYMANAGER_RESULT result = IoTHubRegistryManager_CreateDevice(TEST_IOTHUB_REGISTRYMANAGER_HANDLE, &TEST_IOTHUB_REGISTRY_DEVICE_CREATE, &deviceInfo);

                /// assert
                ASSERT_ARE_NOT_EQUAL(int, IOTHUB_REGISTRYMANAGER_OK, result);
            }

            ///cleanup
        }
        umock_c_negative_tests_deinit();
    }

    /* Test_SRS_IOTHUBREGISTRYMANAGER_12_025: [ IoTHubRegistryManager_GetDevice shall verify the registryManagerHandle and deviceId input parameters and if any of them are NULL then return IOTHUB_REGISTRYMANAGER_INVALID_ARG ] */
    TEST_FUNCTION(IoTHubRegistryManager_GetDevice_return_IOTHUB_REGISTRYMANAGER_INVALID_ARG_if_input_parameter_registryManagerHandle_is_NULL)
    {
        ///arrange

        ///act
        IOTHUB_REGISTRYMANAGER_RESULT result = IoTHubRegistryManager_GetDevice(NULL, TEST_DEVICE_ID, &TEST_IOTHUB_DEVICE);

        ///assert
        ASSERT_ARE_EQUAL(int, IOTHUB_REGISTRYMANAGER_INVALID_ARG, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    }

    /* Test_SRS_IOTHUBREGISTRYMANAGER_12_025: [ IoTHubRegistryManager_GetDevice shall verify the registryManagerHandle and deviceId input parameters and if any of them are NULL then return IOTHUB_REGISTRYMANAGER_INVALID_ARG ] */
    TEST_FUNCTION(IoTHubRegistryManager_GetDevice_return_IOTHUB_REGISTRYMANAGER_INVALID_ARG_if_input_parameter_deviceId_is_NULL)
    {
        ///arrange

        ///act
        IOTHUB_REGISTRYMANAGER_RESULT result = IoTHubRegistryManager_GetDevice(TEST_IOTHUB_REGISTRYMANAGER_HANDLE, NULL, &TEST_IOTHUB_DEVICE);

        ///assert
        ASSERT_ARE_EQUAL(int, IOTHUB_REGISTRYMANAGER_INVALID_ARG, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    }

    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_026: [ IoTHubRegistryManager_GetDevice shall create HTTP GET request URL using the given deviceId using the following format: url/devices/[deviceId]?api-version=2016-11-14  ]*/
    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_027: [ IoTHubRegistryManager_GetDevice shall add the following headers to the created HTTP GET request: authorization=sasToken,Request-Id=1001,Accept=application/json,Content-Type=application/json,charset=utf-8 ]*/
    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_028: [ IoTHubRegistryManager_GetDevice shall create an HTTPAPIEX_SAS_HANDLE handle by calling HTTPAPIEX_SAS_Create ]*/
    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_029: [ IoTHubRegistryManager_GetDevice shall create an HTTPAPIEX_HANDLE handle by calling HTTPAPIEX_Create ]*/
    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_030: [ IoTHubRegistryManager_GetDevice shall execute the HTTP GET request by calling HTTPAPIEX_ExecuteRequest ]*/
    TEST_FUNCTION(IoTHubRegistryManager_GetDevice_happy_path)
    {
        ///arrange
        STRICT_EXPECTED_CALL(BUFFER_new());

        STRICT_EXPECTED_CALL(STRING_construct(TEST_HOSTNAME));
        STRICT_EXPECTED_CALL(STRING_construct(TEST_SHAREDACCESSKEY));
        STRICT_EXPECTED_CALL(STRING_construct(TEST_SHAREDACCESSKEYNAME));

        STRICT_EXPECTED_CALL(HTTPHeaders_Alloc());
        STRICT_EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, TEST_HTTP_HEADER_KEY_AUTHORIZATION, TEST_HTTP_HEADER_VAL_AUTHORIZATION))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, TEST_HTTP_HEADER_KEY_REQUEST_ID, TEST_HTTP_HEADER_VAL_REQUEST_ID))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, TEST_HTTP_HEADER_KEY_USER_AGENT, TEST_HTTP_HEADER_VAL_USER_AGENT))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, TEST_HTTP_HEADER_KEY_ACCEPT, TEST_HTTP_HEADER_VAL_ACCEPT))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, TEST_HTTP_HEADER_KEY_CONTENT_TYPE, TEST_HTTP_HEADER_VAL_CONTENT_TYPE))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(HTTPAPIEX_SAS_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreAllArguments();
        STRICT_EXPECTED_CALL(HTTPAPIEX_Create(TEST_HOSTNAME));

        STRICT_EXPECTED_CALL(HTTPAPIEX_SAS_ExecuteRequest(IGNORED_PTR_ARG, IGNORED_PTR_ARG, HTTPAPI_REQUEST_GET, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(2)
            .IgnoreArgument(4)
            .IgnoreArgument(5)
            .IgnoreArgument(6)
            .IgnoreArgument(7)
            .IgnoreArgument(8)
            .IgnoreArgument(9)
            .CopyOutArgumentBuffer_statusCode(&httpStatusCodeOk, sizeof(httpStatusCodeOk))
            .SetReturn(HTTPAPIEX_OK);

        STRICT_EXPECTED_CALL(HTTPHeaders_Free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPAPIEX_Destroy(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPAPIEX_SAS_Destroy(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(BUFFER_u_char(IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .SetReturn(TEST_UNSIGNED_CHAR_PTR);

        STRICT_EXPECTED_CALL(json_parse_string(IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .SetReturn(TEST_JSON_VALUE);
        STRICT_EXPECTED_CALL(json_value_get_object(TEST_JSON_VALUE))
            .SetReturn(TEST_JSON_OBJECT);

        STRICT_EXPECTED_CALL(json_object_get_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_NAME))
            .SetReturn(TEST_DEVICE_ID);
        STRICT_EXPECTED_CALL(json_object_dotget_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_PRIMARY_KEY))
            .SetReturn(TEST_PRIMARYKEY);
        STRICT_EXPECTED_CALL(json_object_dotget_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_SECONDARY_KEY))
            .SetReturn(TEST_SECONDARYKEY);
        STRICT_EXPECTED_CALL(json_object_get_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_GENERATION_ID))
            .SetReturn(TEST_GENERATIONID);
        STRICT_EXPECTED_CALL(json_object_get_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_ETAG))
            .SetReturn(TEST_ETAG);

        STRICT_EXPECTED_CALL(json_object_get_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_CONNECTIONSTATE))
            .SetReturn(TEST_DEVICE_JSON_DEFAULT_VALUE_CONNECTED);
        STRICT_EXPECTED_CALL(json_object_get_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_CONNECTIONSTATEUPDATEDTIME))
            .SetReturn(TEST_CONNECTIONSTATEUPDATEDTIME);

        STRICT_EXPECTED_CALL(json_object_get_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_STATUS))
            .SetReturn(TEST_DEVICE_JSON_DEFAULT_VALUE_ENABLED);
        STRICT_EXPECTED_CALL(json_object_get_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_STATUSREASON))
            .SetReturn(TEST_STATUSREASON);
        STRICT_EXPECTED_CALL(json_object_get_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_STATUSUPDATEDTIME))
            .SetReturn(TEST_STATUSUPDATEDTIME);

        STRICT_EXPECTED_CALL(json_object_get_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_LASTACTIVITYTIME))
            .SetReturn(TEST_LASTACTIVITYTIME);
        STRICT_EXPECTED_CALL(json_object_get_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_CLOUDTODEVICEMESSAGECOUNT))
            .SetReturn(TEST_CLOUDTODEVICEMESSAGECOUNT);

        STRICT_EXPECTED_CALL(json_object_get_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_ISMANAGED))
            .SetReturn(TEST_IS_MANAGED);
        STRICT_EXPECTED_CALL(json_object_get_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_CONFIGURATION))
            .SetReturn(TEST_CONFIGURATION);
        STRICT_EXPECTED_CALL(json_object_get_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_DEVICEROPERTIES))
            .SetReturn(TEST_DEVICEPROPERTIES);
        STRICT_EXPECTED_CALL(json_object_get_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_SERVICEPROPERTIES))
            .SetReturn(TEST_SERVICEPROPERTIES);

        STRICT_EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreAllArguments();
        STRICT_EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreAllArguments();
        STRICT_EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreAllArguments();
        STRICT_EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreAllArguments();
        STRICT_EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreAllArguments();
        STRICT_EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreAllArguments();
        STRICT_EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreAllArguments();
        STRICT_EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreAllArguments();
        STRICT_EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreAllArguments();
        STRICT_EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreAllArguments();
        STRICT_EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreAllArguments();
        STRICT_EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreAllArguments();

        STRICT_EXPECTED_CALL(json_object_clear(IGNORED_NUM_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(json_value_free(IGNORED_NUM_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(BUFFER_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);


        ///act
        IOTHUB_DEVICE deviceInfo;
        IOTHUB_REGISTRYMANAGER_RESULT result = IoTHubRegistryManager_GetDevice(TEST_IOTHUB_REGISTRYMANAGER_HANDLE, TEST_DEVICE_ID, &deviceInfo);

        ///assert
        ASSERT_ARE_EQUAL(int, IOTHUB_REGISTRYMANAGER_OK, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        free((void*)deviceInfo.deviceId);
        free((void*)deviceInfo.primaryKey);
        free((void*)deviceInfo.secondaryKey);
        free((void*)deviceInfo.generationId);
        free((void*)deviceInfo.eTag);
        free((void*)deviceInfo.connectionStateUpdatedTime);
        free((void*)deviceInfo.statusReason);
        free((void*)deviceInfo.statusUpdatedTime);
        free((void*)deviceInfo.lastActivityTime);
        free((void*)deviceInfo.configuration);
        free((void*)deviceInfo.deviceProperties);
        free((void*)deviceInfo.serviceProperties);
    }

    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_031: [ If any of the HTTPAPI call fails IoTHubRegistryManager_GetDevice shall fail and return IOTHUB_REGISTRYMANAGER_ERROR ]*/
    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_033: [ IoTHubRegistryManager_GetDevice shall verify the received HTTP status code and if it is less or equal than 300 then try to parse the response JSON to deviceInfo for the following properties: deviceId, primaryKey, secondaryKey, generationId, eTag, connectionState, connectionstateUpdatedTime, status, statusReason, statusUpdatedTime, lastActivityTime, cloudToDeviceMessageCount ]*/
    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_034: [ If any of the property field above missing from the JSON the property value will not be populated ]*/
    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_035: [ If the JSON parsing failed, IoTHubRegistryManager_GetDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR ]*/
    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_036: [ If the received JSON is empty, IoTHubRegistryManager_GetDevice shall return IOTHUB_REGISTRYMANAGER_DEVICE_NOT_EXIST ]*/
    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_037: [ If the deviceInfo out parameter if not NULL IoTHubRegistryManager_GetDevice shall save the received deviceInfo to the out parameter and return IOTHUB_REGISTRYMANAGER_OK ]*/
    TEST_FUNCTION(IoTHubRegistryManager_GetDevice_non_happy_path)
    {
        ///arrange
        int umockc_result = umock_c_negative_tests_init();
        ASSERT_ARE_EQUAL(int, 0, umockc_result);

        STRICT_EXPECTED_CALL(BUFFER_new());

        STRICT_EXPECTED_CALL(STRING_construct(TEST_HOSTNAME));
        STRICT_EXPECTED_CALL(STRING_construct(TEST_SHAREDACCESSKEY));
        STRICT_EXPECTED_CALL(STRING_construct(TEST_SHAREDACCESSKEYNAME));

        STRICT_EXPECTED_CALL(HTTPHeaders_Alloc());
        STRICT_EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, TEST_HTTP_HEADER_KEY_AUTHORIZATION, TEST_HTTP_HEADER_VAL_AUTHORIZATION))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, TEST_HTTP_HEADER_KEY_REQUEST_ID, TEST_HTTP_HEADER_VAL_REQUEST_ID))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, TEST_HTTP_HEADER_KEY_USER_AGENT, TEST_HTTP_HEADER_VAL_USER_AGENT))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, TEST_HTTP_HEADER_KEY_ACCEPT, TEST_HTTP_HEADER_VAL_ACCEPT))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, TEST_HTTP_HEADER_KEY_CONTENT_TYPE, TEST_HTTP_HEADER_VAL_CONTENT_TYPE))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(HTTPAPIEX_SAS_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreAllArguments();
        STRICT_EXPECTED_CALL(HTTPAPIEX_Create(TEST_HOSTNAME));

        STRICT_EXPECTED_CALL(HTTPAPIEX_SAS_ExecuteRequest(IGNORED_PTR_ARG, IGNORED_PTR_ARG, HTTPAPI_REQUEST_GET, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(2)
            .IgnoreArgument(4)
            .IgnoreArgument(5)
            .IgnoreArgument(6)
            .IgnoreArgument(7)
            .IgnoreArgument(8)
            .IgnoreArgument(9)
            .CopyOutArgumentBuffer_statusCode(&httpStatusCodeOk, sizeof(httpStatusCodeOk))
            .SetReturn(HTTPAPIEX_OK);

        STRICT_EXPECTED_CALL(HTTPHeaders_Free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPAPIEX_Destroy(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPAPIEX_SAS_Destroy(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(BUFFER_u_char(IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .SetReturn(TEST_UNSIGNED_CHAR_PTR);

        STRICT_EXPECTED_CALL(json_parse_string(IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .SetReturn(TEST_JSON_VALUE);
        STRICT_EXPECTED_CALL(json_value_get_object(TEST_JSON_VALUE))
            .SetReturn(TEST_JSON_OBJECT);

        STRICT_EXPECTED_CALL(json_object_get_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_NAME))
            .SetReturn(TEST_DEVICE_ID);
        STRICT_EXPECTED_CALL(json_object_dotget_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_PRIMARY_KEY))
            .SetReturn(TEST_PRIMARYKEY);
        STRICT_EXPECTED_CALL(json_object_dotget_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_SECONDARY_KEY))
            .SetReturn(TEST_SECONDARYKEY);
        STRICT_EXPECTED_CALL(json_object_get_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_GENERATION_ID))
            .SetReturn(TEST_GENERATIONID);
        STRICT_EXPECTED_CALL(json_object_get_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_ETAG))
            .SetReturn(TEST_ETAG);

        STRICT_EXPECTED_CALL(json_object_get_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_CONNECTIONSTATE))
            .SetReturn(TEST_DEVICE_JSON_DEFAULT_VALUE_CONNECTED);
        STRICT_EXPECTED_CALL(json_object_get_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_CONNECTIONSTATEUPDATEDTIME))
            .SetReturn(TEST_CONNECTIONSTATEUPDATEDTIME);

        STRICT_EXPECTED_CALL(json_object_get_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_STATUS))
            .SetReturn(TEST_DEVICE_JSON_DEFAULT_VALUE_ENABLED);
        STRICT_EXPECTED_CALL(json_object_get_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_STATUSREASON))
            .SetReturn(TEST_STATUSREASON);
        STRICT_EXPECTED_CALL(json_object_get_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_STATUSUPDATEDTIME))
            .SetReturn(TEST_STATUSUPDATEDTIME);

        STRICT_EXPECTED_CALL(json_object_get_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_LASTACTIVITYTIME))
            .SetReturn(TEST_LASTACTIVITYTIME);
        STRICT_EXPECTED_CALL(json_object_get_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_CLOUDTODEVICEMESSAGECOUNT))
            .SetReturn(TEST_CLOUDTODEVICEMESSAGECOUNT);

        STRICT_EXPECTED_CALL(json_object_get_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_ISMANAGED))
            .SetReturn(TEST_IS_MANAGED);
        STRICT_EXPECTED_CALL(json_object_get_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_CONFIGURATION))
            .SetReturn(TEST_CONFIGURATION);
        STRICT_EXPECTED_CALL(json_object_get_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_DEVICEROPERTIES))
            .SetReturn(TEST_DEVICEPROPERTIES);
        STRICT_EXPECTED_CALL(json_object_get_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_SERVICEPROPERTIES))
            .SetReturn(TEST_SERVICEPROPERTIES);

        STRICT_EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreAllArguments();
        STRICT_EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreAllArguments();
        STRICT_EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreAllArguments();
        STRICT_EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreAllArguments();
        STRICT_EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreAllArguments();
        STRICT_EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreAllArguments();
        STRICT_EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreAllArguments();
        STRICT_EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreAllArguments();
        STRICT_EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreAllArguments();
        STRICT_EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreAllArguments();
        STRICT_EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreAllArguments();
        STRICT_EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreAllArguments();

        STRICT_EXPECTED_CALL(json_object_clear(IGNORED_NUM_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(json_value_free(IGNORED_NUM_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(BUFFER_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        umock_c_negative_tests_snapshot();

        ///act
        for (size_t i = 0; i < umock_c_negative_tests_call_count(); i++)
        {
            /// arrange
            IOTHUB_DEVICE* deviceInfo = (IOTHUB_DEVICE*)malloc(sizeof(IOTHUB_DEVICE));
            umock_c_reset_all_calls();

            umock_c_negative_tests_reset();
            umock_c_negative_tests_fail_call(i);
            /// act
            if (
                (i != 13) && /*HTTPHeaders_Free*/
                (i != 14) && /*HTTPAPIEX_Destroy*/
                (i != 15) && /*HTTPAPIEX_SAS_Destroy*/
                (i != 16) && /*STRING_delete*/
                (i != 17) && /*STRING_delete*/
                (i != 18) && /*STRING_delete*/
                (i != 19) && /*BUFFER_u_char*/
                (i != 22) && /*json_object_get_string*/
                (i != 23) && /*json_object_dotget_string*/
                (i != 24) && /*json_object_dotget_string*/
                (i != 25) && /*json_object_get_string*/
                (i != 26) && /*json_object_get_string*/
                (i != 27) && /*json_object_get_string*/
                (i != 28) && /*json_object_get_string*/
                (i != 29) && /*json_object_get_string*/
                (i != 30) && /*json_object_get_string*/
                (i != 31) && /*json_object_get_string*/
                (i != 32) && /*json_object_get_string*/
                (i != 33) && /*json_object_get_string*/
                (i != 34) && /*json_object_get_string*/
                (i != 35) && /*json_object_get_string*/
                (i != 36) && /*json_object_get_string*/
                (i != 37) && /*json_object_get_string*/
                (i != 51) && /*json_value_free*/
                (i != 52) /*BUFFER_delete*/
                )
            {
                IOTHUB_REGISTRYMANAGER_RESULT result = IoTHubRegistryManager_GetDevice(TEST_IOTHUB_REGISTRYMANAGER_HANDLE, TEST_DEVICE_ID, deviceInfo);

                /// assert
                ASSERT_ARE_NOT_EQUAL(int, IOTHUB_REGISTRYMANAGER_OK, result);
            }
            free(deviceInfo);
        }
        umock_c_negative_tests_deinit();
    }

    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_038: [ IoTHubRegistryManager_UpdateDevice shall verify the input parameters and if any of them are NULL then return IOTHUB_REGISTRYMANAGER_INVALID_ARG ] */
    TEST_FUNCTION(IoTHubRegistryManager_UpdateDevice_return_IOTHUB_REGISTRYMANAGER_INVALID_ARG_if_input_parameter_registryManagerHandle_is_NULL)
    {
        ///arrange

        ///act
        IOTHUB_REGISTRYMANAGER_RESULT result = IoTHubRegistryManager_UpdateDevice(NULL, &TEST_IOTHUB_REGISTRY_DEVICE_UPDATE);

        ///assert
        ASSERT_ARE_EQUAL(int, IOTHUB_REGISTRYMANAGER_INVALID_ARG, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    }

    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_038: [ IoTHubRegistryManager_UpdateDevice shall verify the input parameters and if any of them are NULL then return IOTHUB_REGISTRYMANAGER_INVALID_ARG ] */
    TEST_FUNCTION(IoTHubRegistryManager_UpdateDevice_return_IOTHUB_REGISTRYMANAGER_INVALID_ARG_if_input_parameter_deviceUpdate_is_NULL)
    {
        ///arrange

        ///act
        IOTHUB_REGISTRYMANAGER_RESULT result = IoTHubRegistryManager_UpdateDevice(TEST_IOTHUB_REGISTRYMANAGER_HANDLE, NULL);

        ///assert
        ASSERT_ARE_EQUAL(int, IOTHUB_REGISTRYMANAGER_INVALID_ARG, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    }

    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_039: [ IoTHubRegistryManager_UpdateDevice shall verify the deviceCreateInfo->deviceId input parameter and if it is NULL then return IOTHUB_REGISTRYMANAGER_INVALID_ARG ] */
    TEST_FUNCTION(IoTHubRegistryManager_UpdateDevice_return_IOTHUB_REGISTRYMANAGER_INVALID_ARG_if_input_parameter_deviceUpdate_deviceId_is_NULL)
    {
        ///arrange

        ///act
        TEST_IOTHUB_REGISTRY_DEVICE_UPDATE.deviceId = NULL;
        IOTHUB_REGISTRYMANAGER_RESULT result = IoTHubRegistryManager_UpdateDevice(TEST_IOTHUB_REGISTRYMANAGER_HANDLE, &TEST_IOTHUB_REGISTRY_DEVICE_UPDATE);

        ///assert
        ASSERT_ARE_EQUAL(int, IOTHUB_REGISTRYMANAGER_INVALID_ARG, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    }

    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_039: [ IoTHubRegistryManager_UpdateDevice shall verify the deviceCreateInfo->deviceId input parameter and if it is NULL then return IOTHUB_REGISTRYMANAGER_INVALID_ARG ] */
    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_106: [ IoTHubRegistryManager_UpdateDevice shall allocate memory for device info structure by calling malloc ] */
    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_107: [ IoTHubRegistryManager_UpdateDevice shall set the "symmetricKey" value to deviceCreateInfo->primaryKey and deviceCreateInfo->secondaryKey ] */
    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_041: [ IoTHubRegistryManager_UpdateDevice shall create a flat "key1:value2,key2:value2..." JSON representation from the given deviceCreateInfo parameter using the following parson APIs : json_value_init_object, json_value_get_object, json_object_set_string, json_object_dotset_string ] */
    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_101: [ IoTHubRegistryManager_UpdateDevice shall allocate memory for response buffer by calling BUFFER_new ] */
    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_043: [ IoTHubRegistryManager_UpdateDevice shall create an HTTP PUT request using the created JSON ] */
    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_044: [ IoTHubRegistryManager_UpdateDevice shall create an HTTP PUT request using the createdfollowing HTTP headers : authorization = sasToken, Request - Id = 1001, Accept = application / json, Content - Type = application / json, charset = utf - 8 ] */
    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_045: [ IoTHubRegistryManager_UpdateDevice shall create an HTTPAPIEX_SAS_HANDLE handle by calling HTTPAPIEX_SAS_Create ] */
    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_046: [ IoTHubRegistryManager_UpdateDevice shall create an HTTPAPIEX_HANDLE handle by calling HTTPAPIEX_Create ] */
    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_047: [ IoTHubRegistryManager_UpdateDevice shall execute the HTTP PUT request by calling HTTPAPIEX_ExecuteRequest ] */
    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_105: [ IoTHubRegistryManager_UpdateDevice shall do clean up before return ] */
    TEST_FUNCTION(IoTHubRegistryManager_UpdateDevice_happy_path)
    {
        // arrange
        STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(json_value_init_object())
            .SetReturn(TEST_JSON_VALUE);

        STRICT_EXPECTED_CALL(json_value_get_object(TEST_JSON_VALUE))
            .SetReturn(TEST_JSON_OBJECT);

        STRICT_EXPECTED_CALL(json_object_set_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_NAME, TEST_DEVICE_ID));
        STRICT_EXPECTED_CALL(json_object_dotset_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_PRIMARY_KEY, TEST_PRIMARYKEY));
        STRICT_EXPECTED_CALL(json_object_dotset_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_SECONDARY_KEY, TEST_SECONDARYKEY));
        STRICT_EXPECTED_CALL(json_serialize_to_string(TEST_JSON_VALUE))
            .SetReturn(TEST_CHAR_PTR);

        STRICT_EXPECTED_CALL(BUFFER_create(IGNORED_PTR_ARG, IGNORED_NUM_ARG))
            .IgnoreAllArguments();

        STRICT_EXPECTED_CALL(json_free_serialized_string(TEST_CHAR_PTR));
        STRICT_EXPECTED_CALL(json_object_clear(IGNORED_NUM_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(json_value_free(IGNORED_NUM_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(BUFFER_new());

        STRICT_EXPECTED_CALL(STRING_construct(TEST_HOSTNAME));
        STRICT_EXPECTED_CALL(STRING_construct(TEST_SHAREDACCESSKEY));
        STRICT_EXPECTED_CALL(STRING_construct(TEST_SHAREDACCESSKEYNAME));

        STRICT_EXPECTED_CALL(HTTPHeaders_Alloc());
        STRICT_EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, TEST_HTTP_HEADER_KEY_AUTHORIZATION, TEST_HTTP_HEADER_VAL_AUTHORIZATION))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, TEST_HTTP_HEADER_KEY_REQUEST_ID, TEST_HTTP_HEADER_VAL_REQUEST_ID))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, TEST_HTTP_HEADER_KEY_USER_AGENT, TEST_HTTP_HEADER_VAL_USER_AGENT))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, TEST_HTTP_HEADER_KEY_ACCEPT, TEST_HTTP_HEADER_VAL_ACCEPT))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, TEST_HTTP_HEADER_KEY_CONTENT_TYPE, TEST_HTTP_HEADER_VAL_CONTENT_TYPE))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, TEST_HTTP_HEADER_KEY_IFMATCH, TEST_HTTP_HEADER_VAL_IFMATCH))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(HTTPAPIEX_SAS_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreAllArguments();
        STRICT_EXPECTED_CALL(HTTPAPIEX_Create(TEST_HOSTNAME));

        STRICT_EXPECTED_CALL(HTTPAPIEX_SAS_ExecuteRequest(IGNORED_PTR_ARG, IGNORED_PTR_ARG, HTTPAPI_REQUEST_PUT, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(2)
            .IgnoreArgument(4)
            .IgnoreArgument(5)
            .IgnoreArgument(6)
            .IgnoreArgument(7)
            .IgnoreArgument(8)
            .IgnoreArgument(9)
            .CopyOutArgumentBuffer_statusCode(&httpStatusCodeOk, sizeof(httpStatusCodeOk))
            .SetReturn(HTTPAPIEX_OK);

        STRICT_EXPECTED_CALL(HTTPHeaders_Free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPAPIEX_Destroy(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPAPIEX_SAS_Destroy(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(BUFFER_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(BUFFER_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        // act
        IOTHUB_REGISTRYMANAGER_RESULT result = IoTHubRegistryManager_UpdateDevice(TEST_IOTHUB_REGISTRYMANAGER_HANDLE, &TEST_IOTHUB_REGISTRY_DEVICE_UPDATE);

        // assert
        ASSERT_ARE_EQUAL(int, IOTHUB_REGISTRYMANAGER_OK, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    }

    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_108: [ If the malloc fails, IoTHubRegistryManager_Create shall do clean up and return NULL ] */
    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_042: [ IoTHubRegistryManager_UpdateDevice shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR if the JSON creation failed  ] */
    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_102: [ If the BUFFER_new fails, IoTHubRegistryManager_UpdateDevice shall do clean up and return NULL. ] */
    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_103: [ If any of the call fails during the HTTP creation IoTHubRegistryManager_UpdateDevice shall fail and return IOTHUB_REGISTRYMANAGER_ERROR ] */
    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_104: [ If any of the HTTPAPI call fails IoTHubRegistryManager_UpdateDevice shall fail and return IOTHUB_REGISTRYMANAGER_HTTPAPI_ERROR ] */
    TEST_FUNCTION(IoTHubRegistryManager_UpdateDevice_non_happy_path)
    {
        ///arrange
        int umockc_result = umock_c_negative_tests_init();
        ASSERT_ARE_EQUAL(int, 0, umockc_result);

        // arrange
        STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(json_value_init_object())
            .SetReturn(TEST_JSON_VALUE);

        STRICT_EXPECTED_CALL(json_value_get_object(TEST_JSON_VALUE))
            .SetReturn(TEST_JSON_OBJECT);

        STRICT_EXPECTED_CALL(json_object_set_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_NAME, TEST_DEVICE_ID));
        STRICT_EXPECTED_CALL(json_object_dotset_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_PRIMARY_KEY, TEST_PRIMARYKEY));
        STRICT_EXPECTED_CALL(json_object_dotset_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_SECONDARY_KEY, TEST_SECONDARYKEY));
        STRICT_EXPECTED_CALL(json_serialize_to_string(TEST_JSON_VALUE))
            .SetReturn(TEST_CHAR_PTR);

        STRICT_EXPECTED_CALL(BUFFER_create(IGNORED_PTR_ARG, IGNORED_NUM_ARG))
            .IgnoreAllArguments();

        STRICT_EXPECTED_CALL(json_free_serialized_string(TEST_CHAR_PTR));
        STRICT_EXPECTED_CALL(json_object_clear(IGNORED_NUM_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(json_value_free(IGNORED_NUM_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(BUFFER_new());

        STRICT_EXPECTED_CALL(STRING_construct(TEST_HOSTNAME));
        STRICT_EXPECTED_CALL(STRING_construct(TEST_SHAREDACCESSKEY));
        STRICT_EXPECTED_CALL(STRING_construct(TEST_SHAREDACCESSKEYNAME));

        STRICT_EXPECTED_CALL(HTTPHeaders_Alloc());
        STRICT_EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, TEST_HTTP_HEADER_KEY_AUTHORIZATION, TEST_HTTP_HEADER_VAL_AUTHORIZATION))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, TEST_HTTP_HEADER_KEY_REQUEST_ID, TEST_HTTP_HEADER_VAL_REQUEST_ID))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, TEST_HTTP_HEADER_KEY_USER_AGENT, TEST_HTTP_HEADER_VAL_USER_AGENT))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, TEST_HTTP_HEADER_KEY_ACCEPT, TEST_HTTP_HEADER_VAL_ACCEPT))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, TEST_HTTP_HEADER_KEY_CONTENT_TYPE, TEST_HTTP_HEADER_VAL_CONTENT_TYPE))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, TEST_HTTP_HEADER_KEY_IFMATCH, TEST_HTTP_HEADER_VAL_IFMATCH))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(HTTPAPIEX_SAS_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreAllArguments();
        STRICT_EXPECTED_CALL(HTTPAPIEX_Create(TEST_HOSTNAME));

        STRICT_EXPECTED_CALL(HTTPAPIEX_SAS_ExecuteRequest(IGNORED_PTR_ARG, IGNORED_PTR_ARG, HTTPAPI_REQUEST_PUT, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(2)
            .IgnoreArgument(4)
            .IgnoreArgument(5)
            .IgnoreArgument(6)
            .IgnoreArgument(7)
            .IgnoreArgument(8)
            .IgnoreArgument(9)
            .CopyOutArgumentBuffer_statusCode(&httpStatusCodeOk, sizeof(httpStatusCodeOk))
            .SetReturn(HTTPAPIEX_OK);

        STRICT_EXPECTED_CALL(HTTPHeaders_Free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPAPIEX_Destroy(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPAPIEX_SAS_Destroy(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(BUFFER_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(BUFFER_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        umock_c_negative_tests_snapshot();

        ///act
        for (size_t i = 0; i < umock_c_negative_tests_call_count(); i++)
        {
            /// arrange
            umock_c_negative_tests_reset();
            umock_c_negative_tests_fail_call(i);

            /// act
            if (
                (i != 8) && /*json_free_serialized_string*/
                (i != 9) && /*json_object_clear*/
                (i != 10) && /*json_value_free*/
                (i != 25) && /*HTTPHeaders_Free*/
                (i != 26) && /*HTTPAPIEX_Destroy*/
                (i != 27) && /*HTTPAPIEX_SAS_Destroy*/
                (i != 28) && /*STRING_delete*/
                (i != 29) && /*STRING_delete*/
                (i != 30) && /*STRING_delete*/
                (i != 31) && /*BUFFER_delete*/
                (i != 32) && /*BUFFER_delete*/
                (i != 33) /*gballoc_free*/
                )
            {
                printf("i is = %zu\n", i);

                IOTHUB_REGISTRYMANAGER_RESULT result = IoTHubRegistryManager_UpdateDevice(TEST_IOTHUB_REGISTRYMANAGER_HANDLE, &TEST_IOTHUB_REGISTRY_DEVICE_UPDATE);

                /// assert
                ASSERT_ARE_NOT_EQUAL(int, IOTHUB_REGISTRYMANAGER_OK, result);
            }
            ///cleanup
        }
        umock_c_negative_tests_deinit();
    }

    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_052: [ IoTHubRegistryManager_DeleteDevice shall verify the input parameters and if any of them are NULL then return IOTHUB_REGISTRYMANAGER_INVALID_ARG ] */
    TEST_FUNCTION(IoTHubRegistryManager_DeleteDevice_return_IOTHUB_REGISTRYMANAGER_INVALID_ARG_if_input_parameter_registryManagerHandle_is_NULL)
    {
        ///arrange

        ///act
        IOTHUB_REGISTRYMANAGER_RESULT result = IoTHubRegistryManager_DeleteDevice(NULL, TEST_CONST_CHAR_PTR);

        ///assert
        ASSERT_ARE_EQUAL(int, IOTHUB_REGISTRYMANAGER_INVALID_ARG, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    }

    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_052: [ IoTHubRegistryManager_DeleteDevice shall verify the input parameters and if any of them are NULL then return IOTHUB_REGISTRYMANAGER_INVALID_ARG ] */
    TEST_FUNCTION(IoTHubRegistryManager_DeleteDevice_return_IOTHUB_REGISTRYMANAGER_INVALID_ARG_if_input_parameter_deviceId_is_NULL)
    {
        ///arrange

        ///act
        IOTHUB_REGISTRYMANAGER_RESULT result = IoTHubRegistryManager_DeleteDevice(TEST_IOTHUB_REGISTRYMANAGER_HANDLE, NULL);

        ///assert
        ASSERT_ARE_EQUAL(int, IOTHUB_REGISTRYMANAGER_INVALID_ARG, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    }

    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_053: [ IoTHubRegistryManager_DeleteDevice shall create HTTP DELETE request URL using the given deviceId using the following format : url / devices / [deviceId] ? api - version  ] */
    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_054: [ IoTHubRegistryManager_DeleteDevice shall add the following headers to the created HTTP GET request : authorization = sasToken, Request - Id = 1001, Accept = application / json, Content - Type = application / json, charset = utf - 8 ] */
    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_055: [ IoTHubRegistryManager_DeleteDevice shall create an HTTPAPIEX_SAS_HANDLE handle by calling HTTPAPIEX_SAS_Create ] */
    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_056: [ IoTHubRegistryManager_DeleteDevice shall create an HTTPAPIEX_HANDLE handle by calling HTTPAPIEX_Create ] */
    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_057: [ IoTHubRegistryManager_DeleteDevice shall execute the HTTP DELETE request by calling HTTPAPIEX_ExecuteRequest ] */
    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_058: [ IoTHubRegistryManager_DeleteDevice shall verify the received HTTP status code and if it is greater than 300 then return IOTHUB_REGISTRYMANAGER_HTTP_STATUS_ERROR ] */
    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_059: [ IoTHubRegistryManager_DeleteDevice shall verify the received HTTP status code and if it is less or equal than 300 then return IOTHUB_REGISTRYMANAGER_OK ] */
    TEST_FUNCTION(IoTHubRegistryManager_DeleteDevice_happy_path)
    {
        ///arrange
        STRICT_EXPECTED_CALL(STRING_construct(TEST_HOSTNAME));
        STRICT_EXPECTED_CALL(STRING_construct(TEST_SHAREDACCESSKEY));
        STRICT_EXPECTED_CALL(STRING_construct(TEST_SHAREDACCESSKEYNAME));

        STRICT_EXPECTED_CALL(HTTPHeaders_Alloc());
        STRICT_EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, TEST_HTTP_HEADER_KEY_AUTHORIZATION, TEST_HTTP_HEADER_VAL_AUTHORIZATION))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, TEST_HTTP_HEADER_KEY_REQUEST_ID, TEST_HTTP_HEADER_VAL_REQUEST_ID))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, TEST_HTTP_HEADER_KEY_USER_AGENT, TEST_HTTP_HEADER_VAL_USER_AGENT))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, TEST_HTTP_HEADER_KEY_ACCEPT, TEST_HTTP_HEADER_VAL_ACCEPT))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, TEST_HTTP_HEADER_KEY_CONTENT_TYPE, TEST_HTTP_HEADER_VAL_CONTENT_TYPE))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, TEST_HTTP_HEADER_KEY_IFMATCH, TEST_HTTP_HEADER_VAL_IFMATCH))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(HTTPAPIEX_SAS_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreAllArguments();
        STRICT_EXPECTED_CALL(HTTPAPIEX_Create(TEST_HOSTNAME));

        STRICT_EXPECTED_CALL(HTTPAPIEX_SAS_ExecuteRequest(IGNORED_PTR_ARG, IGNORED_PTR_ARG, HTTPAPI_REQUEST_DELETE, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(2)
            .IgnoreArgument(4)
            .IgnoreArgument(5)
            .IgnoreArgument(6)
            .IgnoreArgument(7)
            .IgnoreArgument(8)
            .IgnoreArgument(9)
            .CopyOutArgumentBuffer_statusCode(&httpStatusCodeOk, sizeof(httpStatusCodeOk))
            .SetReturn(HTTPAPIEX_OK);

        STRICT_EXPECTED_CALL(HTTPHeaders_Free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPAPIEX_Destroy(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPAPIEX_SAS_Destroy(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        ///act
        IOTHUB_REGISTRYMANAGER_RESULT result = IoTHubRegistryManager_DeleteDevice(TEST_IOTHUB_REGISTRYMANAGER_HANDLE, TEST_DEVICE_ID);

        ///assert
        ASSERT_ARE_EQUAL(int, IOTHUB_REGISTRYMANAGER_OK, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    }

    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_053: [ IoTHubRegistryManager_DeleteDevice shall create HTTP DELETE request URL using the given deviceId using the following format : url / devices / [deviceId] ? api - version  ] */
    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_054: [ IoTHubRegistryManager_DeleteDevice shall add the following headers to the created HTTP GET request : authorization = sasToken, Request - Id = 1001, Accept = application / json, Content - Type = application / json, charset = utf - 8 ] */
    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_055: [ IoTHubRegistryManager_DeleteDevice shall create an HTTPAPIEX_SAS_HANDLE handle by calling HTTPAPIEX_SAS_Create ] */
    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_056: [ IoTHubRegistryManager_DeleteDevice shall create an HTTPAPIEX_HANDLE handle by calling HTTPAPIEX_Create ] */
    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_057: [ IoTHubRegistryManager_DeleteDevice shall execute the HTTP DELETE request by calling HTTPAPIEX_ExecuteRequest ] */
    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_058: [ IoTHubRegistryManager_DeleteDevice shall verify the received HTTP status code and if it is greater than 300 then return IOTHUB_REGISTRYMANAGER_HTTP_STATUS_ERROR ] */
    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_059: [ IoTHubRegistryManager_DeleteDevice shall verify the received HTTP status code and if it is less or equal than 300 then return IOTHUB_REGISTRYMANAGER_OK ] */
    TEST_FUNCTION(IoTHubRegistryManager_DeleteDevice_non_happy_path)
    {
        ///arrange
        int umockc_result = umock_c_negative_tests_init();
        ASSERT_ARE_EQUAL(int, 0, umockc_result);

        STRICT_EXPECTED_CALL(STRING_construct(TEST_HOSTNAME));
        STRICT_EXPECTED_CALL(STRING_construct(TEST_SHAREDACCESSKEY));
        STRICT_EXPECTED_CALL(STRING_construct(TEST_SHAREDACCESSKEYNAME));

        STRICT_EXPECTED_CALL(HTTPHeaders_Alloc());
        STRICT_EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, TEST_HTTP_HEADER_KEY_AUTHORIZATION, TEST_HTTP_HEADER_VAL_AUTHORIZATION))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, TEST_HTTP_HEADER_KEY_REQUEST_ID, TEST_HTTP_HEADER_VAL_REQUEST_ID))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, TEST_HTTP_HEADER_KEY_USER_AGENT, TEST_HTTP_HEADER_VAL_USER_AGENT))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, TEST_HTTP_HEADER_KEY_ACCEPT, TEST_HTTP_HEADER_VAL_ACCEPT))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, TEST_HTTP_HEADER_KEY_CONTENT_TYPE, TEST_HTTP_HEADER_VAL_CONTENT_TYPE))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, TEST_HTTP_HEADER_KEY_IFMATCH, TEST_HTTP_HEADER_VAL_IFMATCH))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(HTTPAPIEX_SAS_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreAllArguments();
        STRICT_EXPECTED_CALL(HTTPAPIEX_Create(TEST_HOSTNAME));

        STRICT_EXPECTED_CALL(HTTPAPIEX_SAS_ExecuteRequest(IGNORED_PTR_ARG, IGNORED_PTR_ARG, HTTPAPI_REQUEST_DELETE, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(2)
            .IgnoreArgument(4)
            .IgnoreArgument(5)
            .IgnoreArgument(6)
            .IgnoreArgument(7)
            .IgnoreArgument(8)
            .IgnoreArgument(9)
            .CopyOutArgumentBuffer_statusCode(&httpStatusCodeOk, sizeof(httpStatusCodeOk))
            .SetReturn(HTTPAPIEX_OK);

        STRICT_EXPECTED_CALL(HTTPHeaders_Free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPAPIEX_Destroy(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPAPIEX_SAS_Destroy(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        umock_c_negative_tests_snapshot();

        ///act
        for (size_t i = 0; i < umock_c_negative_tests_call_count(); i++)
        {
            /// arrange
            umock_c_negative_tests_reset();
            umock_c_negative_tests_fail_call(i);

            /// act
            if (
                (i != 13) && /*HTTPHeaders_Free*/
                (i != 14) && /*HTTPAPIEX_Destroy*/
                (i != 15) && /*HTTPAPIEX_SAS_Destroy*/
                (i != 16) && /*STRING_delete*/
                (i != 17) && /*STRING_delete*/
                (i != 18) /*STRING_delete*/
                )
            {
                IOTHUB_REGISTRYMANAGER_RESULT result = IoTHubRegistryManager_DeleteDevice(TEST_IOTHUB_REGISTRYMANAGER_HANDLE, TEST_DEVICE_ID);

                /// assert
                ASSERT_ARE_NOT_EQUAL(int, IOTHUB_REGISTRYMANAGER_OK, result);
            }
            ///cleanup
        }
        umock_c_negative_tests_deinit();
    }

    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_060: [ IoTHubRegistryManager_GetDeviceList shall verify the input parameters and if any of them are NULL then return IOTHUB_REGISTRYMANAGER_INVALID_ARG ]*/
    TEST_FUNCTION(IoTHubRegistryManager_GetDeviceList_return_IOTHUB_REGISTRYMANAGER_INVALID_ARG_if_input_parameter_registryManagerHandle_is_NULL)
    {
        ///arrange
        SINGLYLINKEDLIST_HANDLE deviceList = singlylinkedlist_create();
        umock_c_reset_all_calls();

        ///act
        IOTHUB_REGISTRYMANAGER_RESULT result = IoTHubRegistryManager_GetDeviceList(NULL, 10, deviceList);

        ///assert
        ASSERT_ARE_EQUAL(int, IOTHUB_REGISTRYMANAGER_INVALID_ARG, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        singlylinkedlist_destroy(deviceList);
    }

    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_060: [ IoTHubRegistryManager_GetDeviceList shall verify the input parameters and if any of them are NULL then return IOTHUB_REGISTRYMANAGER_INVALID_ARG ]*/
    TEST_FUNCTION(IoTHubRegistryManager_GetDeviceList_return_IOTHUB_REGISTRYMANAGER_INVALID_ARG_if_input_parameter_deviceList_is_NULL)
    {
        ///arrange

        ///act
        IOTHUB_REGISTRYMANAGER_RESULT result = IoTHubRegistryManager_GetDeviceList(TEST_IOTHUB_REGISTRYMANAGER_HANDLE, 10, NULL);

        ///assert
        ASSERT_ARE_EQUAL(int, IOTHUB_REGISTRYMANAGER_INVALID_ARG, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    }

    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_061: [ IoTHubRegistryManager_GetDeviceList shall verify if the numberOfDevices input parameter is between 1 and 1000 and if it is not then return IOTHUB_REGISTRYMANAGER_INVALID_ARG ]*/
    TEST_FUNCTION(IoTHubRegistryManager_GetDeviceList_return_IOTHUB_REGISTRYMANAGER_INVALID_ARG_if_input_parameter_numberOfDevices_is_zero)
    {
        ///arrange
        SINGLYLINKEDLIST_HANDLE deviceList = singlylinkedlist_create();
        umock_c_reset_all_calls();

        ///act
        IOTHUB_REGISTRYMANAGER_RESULT result = IoTHubRegistryManager_GetDeviceList(TEST_IOTHUB_REGISTRYMANAGER_HANDLE, 0, deviceList);

        ///assert
        ASSERT_ARE_EQUAL(int, IOTHUB_REGISTRYMANAGER_INVALID_ARG, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        singlylinkedlist_destroy(deviceList);
    }

    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_061: [ IoTHubRegistryManager_GetDeviceList shall verify if the numberOfDevices input parameter is between 1 and 1000 and if it is not then return IOTHUB_REGISTRYMANAGER_INVALID_ARG ]*/
    TEST_FUNCTION(IoTHubRegistryManager_GetDeviceList_return_IOTHUB_REGISTRYMANAGER_INVALID_ARG_if_input_parameter_numberOfDevices_is_1001)
    {
        ///arrange
        SINGLYLINKEDLIST_HANDLE deviceList = singlylinkedlist_create();
        umock_c_reset_all_calls();

        ///act
        IOTHUB_REGISTRYMANAGER_RESULT result = IoTHubRegistryManager_GetDeviceList(TEST_IOTHUB_REGISTRYMANAGER_HANDLE, 1001, deviceList);

        ///assert
        ASSERT_ARE_EQUAL(int, IOTHUB_REGISTRYMANAGER_INVALID_ARG, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        singlylinkedlist_destroy(deviceList);
    }

    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_062: [ IoTHubRegistryManager_GetDeviceList shall create HTTP GET request for numberOfDevices using the follwoing format: url/devices/?top=[numberOfDevices]&api-version ]*/
    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_063: [ IoTHubRegistryManager_GetDeviceList shall add the following headers to the created HTTP GET request: authorization=sasToken,Request-Id=1001,Accept=application/json,Content-Type=application/json,charset=utf-8 ]*/
    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_064: [ IoTHubRegistryManager_GetDeviceList shall create an HTTPAPIEX_SAS_HANDLE handle by calling HTTPAPIEX_SAS_Create ]*/
    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_065: [ IoTHubRegistryManager_GetDeviceList shall create an HTTPAPIEX_HANDLE handle by calling HTTPAPIEX_Create ]*/
    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_066: [ IoTHubRegistryManager_GetDeviceList shall execute the HTTP GET request by calling HTTPAPIEX_ExecuteRequest ]*/
    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_067: [ IoTHubRegistryManager_GetDeviceList shall verify the received HTTP status code and if it is greater than 300 then return IOTHUB_REGISTRYMANAGER_ERROR ]*/
    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_068: [ IoTHubRegistryManager_GetDeviceList shall verify the received HTTP status code and if it is less or equal than 300 then try to parse the response JSON to deviceList ]*/
    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_069: [ IoTHubRegistryManager_GetDeviceList shall use the following parson APIs to parse the response JSON: json_parse_string, json_value_get_object, json_object_get_string, json_object_dotget_string  ]*/
    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_070: [ If any of the parson API fails, IoTHubRegistryManager_GetDeviceList shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR ]*/
    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_071: [ IoTHubRegistryManager_GetDeviceList shall populate the deviceList parameter with structures of type "IOTHUB_DEVICE" ]*/
    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_072: [ If populating the deviceList parameter fails IoTHubRegistryManager_GetDeviceList shall return IOTHUB_REGISTRYMANAGER_ERROR ]*/
    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_073: [ If populating the deviceList parameter successful IoTHubRegistryManager_GetDeviceList shall return IOTHUB_REGISTRYMANAGER_OK ]*/
    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_111: [ IoTHubRegistryManager_GetDeviceList shall do clean up before return ]*/
    TEST_FUNCTION(IoTHubRegistryManager_GetDeviceList_happy_path)
    {
        ///arrange
        SINGLYLINKEDLIST_HANDLE deviceList = singlylinkedlist_create();
        ASSERT_IS_NOT_NULL(deviceList);
        umock_c_reset_all_calls();

        STRICT_EXPECTED_CALL(BUFFER_new());

        STRICT_EXPECTED_CALL(STRING_construct(TEST_HOSTNAME));
        STRICT_EXPECTED_CALL(STRING_construct(TEST_SHAREDACCESSKEY));
        STRICT_EXPECTED_CALL(STRING_construct(TEST_SHAREDACCESSKEYNAME));

        STRICT_EXPECTED_CALL(HTTPHeaders_Alloc());
        STRICT_EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, TEST_HTTP_HEADER_KEY_AUTHORIZATION, TEST_HTTP_HEADER_VAL_AUTHORIZATION))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, TEST_HTTP_HEADER_KEY_REQUEST_ID, TEST_HTTP_HEADER_VAL_REQUEST_ID))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, TEST_HTTP_HEADER_KEY_USER_AGENT, TEST_HTTP_HEADER_VAL_USER_AGENT))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, TEST_HTTP_HEADER_KEY_ACCEPT, TEST_HTTP_HEADER_VAL_ACCEPT))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, TEST_HTTP_HEADER_KEY_CONTENT_TYPE, TEST_HTTP_HEADER_VAL_CONTENT_TYPE))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(HTTPAPIEX_SAS_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreAllArguments();
        STRICT_EXPECTED_CALL(HTTPAPIEX_Create(TEST_HOSTNAME));

        STRICT_EXPECTED_CALL(HTTPAPIEX_SAS_ExecuteRequest(IGNORED_PTR_ARG, IGNORED_PTR_ARG, HTTPAPI_REQUEST_GET, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(2)
            .IgnoreArgument(4)
            .IgnoreArgument(5)
            .IgnoreArgument(6)
            .IgnoreArgument(7)
            .IgnoreArgument(8)
            .IgnoreArgument(9)
            .CopyOutArgumentBuffer_statusCode(&httpStatusCodeOk, sizeof(httpStatusCodeOk))
            .SetReturn(HTTPAPIEX_OK);

        STRICT_EXPECTED_CALL(HTTPHeaders_Free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPAPIEX_Destroy(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPAPIEX_SAS_Destroy(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(BUFFER_u_char(IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .SetReturn(TEST_UNSIGNED_CHAR_PTR);
        STRICT_EXPECTED_CALL(json_parse_string(IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .SetReturn(TEST_JSON_VALUE);
        STRICT_EXPECTED_CALL(json_value_get_array(TEST_JSON_VALUE))
            .SetReturn(TEST_JSON_ARRAY);

        STRICT_EXPECTED_CALL(json_array_get_count(TEST_JSON_ARRAY))
            .SetReturn(1);

        STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(json_array_get_object(TEST_JSON_ARRAY, 0))
            .SetReturn(TEST_JSON_OBJECT);

        STRICT_EXPECTED_CALL(json_object_get_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_NAME))
            .SetReturn(TEST_DEVICE_ID);
        STRICT_EXPECTED_CALL(json_object_dotget_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_PRIMARY_KEY))
            .SetReturn(TEST_PRIMARYKEY);
        STRICT_EXPECTED_CALL(json_object_dotget_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_SECONDARY_KEY))
            .SetReturn(TEST_SECONDARYKEY);
        STRICT_EXPECTED_CALL(json_object_get_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_GENERATION_ID))
            .SetReturn(TEST_GENERATIONID);
        STRICT_EXPECTED_CALL(json_object_get_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_ETAG))
            .SetReturn(TEST_ETAG);
        STRICT_EXPECTED_CALL(json_object_get_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_CONNECTIONSTATE))
            .SetReturn(TEST_DEVICE_JSON_DEFAULT_VALUE_CONNECTED);
        STRICT_EXPECTED_CALL(json_object_get_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_CONNECTIONSTATEUPDATEDTIME))
            .SetReturn(TEST_CONNECTIONSTATEUPDATEDTIME);
        STRICT_EXPECTED_CALL(json_object_get_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_STATUS))
            .SetReturn(TEST_DEVICE_JSON_DEFAULT_VALUE_ENABLED);
        STRICT_EXPECTED_CALL(json_object_get_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_STATUSREASON))
            .SetReturn(TEST_STATUSREASON);
        STRICT_EXPECTED_CALL(json_object_get_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_STATUSUPDATEDTIME))
            .SetReturn(TEST_STATUSUPDATEDTIME);
        STRICT_EXPECTED_CALL(json_object_get_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_LASTACTIVITYTIME))
            .SetReturn(TEST_LASTACTIVITYTIME);
        STRICT_EXPECTED_CALL(json_object_get_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_CLOUDTODEVICEMESSAGECOUNT))
            .SetReturn(TEST_CLOUDTODEVICEMESSAGECOUNT);
        STRICT_EXPECTED_CALL(json_object_get_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_ISMANAGED))
            .SetReturn(TEST_IS_MANAGED);
        STRICT_EXPECTED_CALL(json_object_get_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_CONFIGURATION))
            .SetReturn(TEST_CONFIGURATION);
        STRICT_EXPECTED_CALL(json_object_get_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_DEVICEROPERTIES))
            .SetReturn(TEST_DEVICEPROPERTIES);
        STRICT_EXPECTED_CALL(json_object_get_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_SERVICEPROPERTIES))
            .SetReturn(TEST_SERVICEPROPERTIES);

        STRICT_EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreAllArguments();
        STRICT_EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreAllArguments();
        STRICT_EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreAllArguments();
        STRICT_EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreAllArguments();
        STRICT_EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreAllArguments();
        STRICT_EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreAllArguments();
        STRICT_EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreAllArguments();
        STRICT_EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreAllArguments();
        STRICT_EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreAllArguments();
        STRICT_EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreAllArguments();
        STRICT_EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreAllArguments();
        STRICT_EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreAllArguments();

        STRICT_EXPECTED_CALL(singlylinkedlist_add(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreAllArguments();

        STRICT_EXPECTED_CALL(json_object_clear(IGNORED_NUM_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(json_array_clear(IGNORED_NUM_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(json_value_free(IGNORED_NUM_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(BUFFER_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        ///act
        IOTHUB_REGISTRYMANAGER_RESULT result = IoTHubRegistryManager_GetDeviceList(TEST_IOTHUB_REGISTRYMANAGER_HANDLE, 10, deviceList);

        ///assert
        ASSERT_ARE_EQUAL(int, IOTHUB_REGISTRYMANAGER_OK, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        if (deviceList != NULL)
        {
            LIST_ITEM_HANDLE itemHandle = singlylinkedlist_get_head_item(deviceList);
            while (itemHandle != NULL)
            {
                IOTHUB_DEVICE* deviceInfo = (IOTHUB_DEVICE*)itemHandle->item;
                itemHandle = singlylinkedlist_get_next_item(itemHandle);

                if (deviceInfo->deviceId != NULL)
                    free((char*)deviceInfo->deviceId);
                if (deviceInfo->primaryKey != NULL)
                    free((char*)deviceInfo->primaryKey);
                if (deviceInfo->secondaryKey != NULL)
                    free((char*)deviceInfo->secondaryKey);
                if (deviceInfo->generationId != NULL)
                    free((char*)deviceInfo->generationId);
                if (deviceInfo->eTag != NULL)
                    free((char*)deviceInfo->eTag);
                if (deviceInfo->connectionStateUpdatedTime != NULL)
                    free((char*)deviceInfo->connectionStateUpdatedTime);
                if (deviceInfo->statusReason != NULL)
                    free((char*)deviceInfo->statusReason);
                if (deviceInfo->statusUpdatedTime != NULL)
                    free((char*)deviceInfo->statusUpdatedTime);
                if (deviceInfo->lastActivityTime != NULL)
                    free((char*)deviceInfo->lastActivityTime);
                if (deviceInfo->configuration != NULL)
                    free((char*)deviceInfo->configuration);
                if (deviceInfo->deviceProperties != NULL)
                    free((char*)deviceInfo->deviceProperties);
                if (deviceInfo->serviceProperties != NULL)
                    free((char*)deviceInfo->serviceProperties);
                free(deviceInfo);
            }
            singlylinkedlist_destroy(deviceList);
        }
    }

    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_110: [ If the BUFFER_new fails, IoTHubRegistryManager_GetDeviceList shall do clean up and return NULL ]*/
    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_070: [ If any of the parson API fails, IoTHubRegistryManager_GetDeviceList shall return IOTHUB_REGISTRYMANAGER_JSON_ERROR ]*/
    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_072: [ If populating the deviceList parameter fails IoTHubRegistryManager_GetDeviceList shall return IOTHUB_REGISTRYMANAGER_ERROR ]*/
    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_115: [ If any of the HTTPAPI call fails IoTHubRegistryManager_GetDeviceList shall fail and return IOTHUB_REGISTRYMANAGER_ERROR ]*/
    TEST_FUNCTION(IoTHubRegistryManager_GetDeviceList_non_happy_path)
    {
        ///arrange
        int umockc_result = umock_c_negative_tests_init();
        ASSERT_ARE_EQUAL(int, 0, umockc_result);

        STRICT_EXPECTED_CALL(BUFFER_new());

        STRICT_EXPECTED_CALL(STRING_construct(TEST_HOSTNAME));
        STRICT_EXPECTED_CALL(STRING_construct(TEST_SHAREDACCESSKEY));
        STRICT_EXPECTED_CALL(STRING_construct(TEST_SHAREDACCESSKEYNAME));

        STRICT_EXPECTED_CALL(HTTPHeaders_Alloc());
        STRICT_EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, TEST_HTTP_HEADER_KEY_AUTHORIZATION, TEST_HTTP_HEADER_VAL_AUTHORIZATION))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, TEST_HTTP_HEADER_KEY_REQUEST_ID, TEST_HTTP_HEADER_VAL_REQUEST_ID))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, TEST_HTTP_HEADER_KEY_USER_AGENT, TEST_HTTP_HEADER_VAL_USER_AGENT))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, TEST_HTTP_HEADER_KEY_ACCEPT, TEST_HTTP_HEADER_VAL_ACCEPT))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, TEST_HTTP_HEADER_KEY_CONTENT_TYPE, TEST_HTTP_HEADER_VAL_CONTENT_TYPE))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(HTTPAPIEX_SAS_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreAllArguments();
        STRICT_EXPECTED_CALL(HTTPAPIEX_Create(TEST_HOSTNAME));

        STRICT_EXPECTED_CALL(HTTPAPIEX_SAS_ExecuteRequest(IGNORED_PTR_ARG, IGNORED_PTR_ARG, HTTPAPI_REQUEST_GET, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(2)
            .IgnoreArgument(4)
            .IgnoreArgument(5)
            .IgnoreArgument(6)
            .IgnoreArgument(7)
            .IgnoreArgument(8)
            .IgnoreArgument(9)
            .CopyOutArgumentBuffer_statusCode(&httpStatusCodeOk, sizeof(httpStatusCodeOk))
            .SetReturn(HTTPAPIEX_OK);

        STRICT_EXPECTED_CALL(HTTPHeaders_Free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPAPIEX_Destroy(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPAPIEX_SAS_Destroy(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(BUFFER_u_char(IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .SetReturn(TEST_UNSIGNED_CHAR_PTR);
        STRICT_EXPECTED_CALL(json_parse_string(IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .SetReturn(TEST_JSON_VALUE);
        STRICT_EXPECTED_CALL(json_value_get_array(TEST_JSON_VALUE))
            .SetReturn(TEST_JSON_ARRAY);

        STRICT_EXPECTED_CALL(json_array_get_count(TEST_JSON_ARRAY))
            .SetReturn(1);

        STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(json_array_get_object(TEST_JSON_ARRAY, 0))
            .SetReturn(TEST_JSON_OBJECT);

        STRICT_EXPECTED_CALL(json_object_get_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_NAME))
            .SetReturn(TEST_DEVICE_ID);
        STRICT_EXPECTED_CALL(json_object_dotget_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_PRIMARY_KEY))
            .SetReturn(TEST_PRIMARYKEY);
        STRICT_EXPECTED_CALL(json_object_dotget_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_SECONDARY_KEY))
            .SetReturn(TEST_SECONDARYKEY);
        STRICT_EXPECTED_CALL(json_object_get_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_GENERATION_ID))
            .SetReturn(TEST_GENERATIONID);
        STRICT_EXPECTED_CALL(json_object_get_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_ETAG))
            .SetReturn(TEST_ETAG);
        STRICT_EXPECTED_CALL(json_object_get_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_CONNECTIONSTATE))
            .SetReturn(TEST_DEVICE_JSON_DEFAULT_VALUE_CONNECTED);
        STRICT_EXPECTED_CALL(json_object_get_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_CONNECTIONSTATEUPDATEDTIME))
            .SetReturn(TEST_CONNECTIONSTATEUPDATEDTIME);
        STRICT_EXPECTED_CALL(json_object_get_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_STATUS))
            .SetReturn(TEST_DEVICE_JSON_DEFAULT_VALUE_ENABLED);
        STRICT_EXPECTED_CALL(json_object_get_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_STATUSREASON))
            .SetReturn(TEST_STATUSREASON);
        STRICT_EXPECTED_CALL(json_object_get_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_STATUSUPDATEDTIME))
            .SetReturn(TEST_STATUSUPDATEDTIME);
        STRICT_EXPECTED_CALL(json_object_get_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_LASTACTIVITYTIME))
            .SetReturn(TEST_LASTACTIVITYTIME);
        STRICT_EXPECTED_CALL(json_object_get_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_CLOUDTODEVICEMESSAGECOUNT))
            .SetReturn(TEST_CLOUDTODEVICEMESSAGECOUNT);
        STRICT_EXPECTED_CALL(json_object_get_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_ISMANAGED))
            .SetReturn(TEST_IS_MANAGED);
        STRICT_EXPECTED_CALL(json_object_get_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_CONFIGURATION))
            .SetReturn(TEST_CONFIGURATION);
        STRICT_EXPECTED_CALL(json_object_get_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_DEVICEROPERTIES))
            .SetReturn(TEST_DEVICEPROPERTIES);
        STRICT_EXPECTED_CALL(json_object_get_string(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DEVICE_SERVICEPROPERTIES))
            .SetReturn(TEST_SERVICEPROPERTIES);

        STRICT_EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreAllArguments();
        STRICT_EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreAllArguments();
        STRICT_EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreAllArguments();
        STRICT_EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreAllArguments();
        STRICT_EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreAllArguments();
        STRICT_EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreAllArguments();
        STRICT_EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreAllArguments();
        STRICT_EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreAllArguments();
        STRICT_EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreAllArguments();
        STRICT_EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreAllArguments();
        STRICT_EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreAllArguments();
        STRICT_EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreAllArguments();

        STRICT_EXPECTED_CALL(singlylinkedlist_add(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreAllArguments();

        STRICT_EXPECTED_CALL(json_object_clear(IGNORED_NUM_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(json_array_clear(IGNORED_NUM_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(json_value_free(IGNORED_NUM_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(BUFFER_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        umock_c_negative_tests_snapshot();

        ///act
        size_t negative_call_count = umock_c_negative_tests_call_count();
        printf("negative_test_count=%zu", negative_call_count);
        for (size_t i = 0; i < negative_call_count; i++)
        {
            /// arrange
            SINGLYLINKEDLIST_HANDLE deviceList = singlylinkedlist_create();
            umock_c_reset_all_calls();

            umock_c_negative_tests_reset();
            umock_c_negative_tests_fail_call(i);

            /// act
            if (
                (i != 13) && /*HTTPHeaders_Free*/
                (i != 14) && /*HTTPAPIEX_Destroy*/
                (i != 15) && /*HTTPAPIEX_SAS_Destroy*/
                (i != 16) && /*STRING_delete*/
                (i != 17) && /*STRING_delete*/
                (i != 18) && /*STRING_delete*/
                (i != 22) && /*json_array_get_count*/
                (i != 25) && /*json_object_get_string*/
                (i != 26) && /*json_object_dotget_string*/
                (i != 27) && /*json_object_dotget_string*/
                (i != 28) && /*json_object_get_string*/
                (i != 29) && /*json_object_get_string*/
                (i != 30) && /*json_object_get_string*/
                (i != 31) && /*json_object_get_string*/
                (i != 32) && /*json_object_get_string*/
                (i != 33) && /*json_object_get_string*/
                (i != 34) && /*json_object_get_string*/
                (i != 35) && /*json_object_get_string*/
                (i != 36) && /*json_object_get_string*/
                (i != 37) && /*json_object_get_string*/
                (i != 38) && /*json_object_get_string*/
                (i != 39) && /*json_object_get_string*/
                (i != 40) && /*json_object_get_string*/
                (i != 56) && /*json_value_free*/
                (i != 57) /*BUFFER_delete*/
                )
            {
                IOTHUB_REGISTRYMANAGER_RESULT result = IoTHubRegistryManager_GetDeviceList(TEST_IOTHUB_REGISTRYMANAGER_HANDLE, 10, deviceList);

                /// assert
                ASSERT_ARE_NOT_EQUAL(int, IOTHUB_REGISTRYMANAGER_OK, result);
            }

            ///cleanup
            if (deviceList != NULL)
            {
                LIST_ITEM_HANDLE itemHandle = singlylinkedlist_get_head_item(deviceList);
                while (itemHandle != NULL)
                {
                    IOTHUB_DEVICE* deviceInfo = (IOTHUB_DEVICE*)itemHandle->item;
                    itemHandle = singlylinkedlist_get_next_item(itemHandle);

                    if (deviceInfo->deviceId != NULL)
                        free((char*)deviceInfo->deviceId);
                    if (deviceInfo->primaryKey != NULL)
                        free((char*)deviceInfo->primaryKey);
                    if (deviceInfo->secondaryKey != NULL)
                        free((char*)deviceInfo->secondaryKey);
                    if (deviceInfo->generationId != NULL)
                        free((char*)deviceInfo->generationId);
                    if (deviceInfo->eTag != NULL)
                        free((char*)deviceInfo->eTag);
                    if (deviceInfo->connectionStateUpdatedTime != NULL)
                        free((char*)deviceInfo->connectionStateUpdatedTime);
                    if (deviceInfo->statusReason != NULL)
                        free((char*)deviceInfo->statusReason);
                    if (deviceInfo->statusUpdatedTime != NULL)
                        free((char*)deviceInfo->statusUpdatedTime);
                    if (deviceInfo->lastActivityTime != NULL)
                        free((char*)deviceInfo->lastActivityTime);
                    if (deviceInfo->configuration != NULL)
                        free((char*)deviceInfo->configuration);
                    if (deviceInfo->deviceProperties != NULL)
                        free((char*)deviceInfo->deviceProperties);
                    if (deviceInfo->serviceProperties != NULL)
                        free((char*)deviceInfo->serviceProperties);
                    free(deviceInfo);
                }
                singlylinkedlist_destroy(deviceList);
            }


            //if (deviceList != NULL)
            //{
            //    LIST_INSTANCE* list_instance = (LIST_INSTANCE*)deviceList;

            //    while (list_instance->head != NULL)
            //    {
            //        LIST_ITEM_INSTANCE* current_item = list_instance->head;
            //        list_instance->head = current_item->next;

            //        IOTHUB_DEVICE* deviceInfo = (IOTHUB_DEVICE*)current_item;
            //        if (deviceInfo != NULL)

            //        //if (singlylinkedlist_item_get_value(current_item) != NULL)
            //        {
            //        //    IOTHUB_DEVICE* deviceInfo = (IOTHUB_DEVICE*)(singlylinkedlist_item_get_value(current_item));
            //            if (deviceInfo->deviceId != NULL)
            //                free((char*)deviceInfo->deviceId);
            //            if (deviceInfo->primaryKey != NULL)
            //                free((char*)deviceInfo->primaryKey);
            //            if (deviceInfo->secondaryKey != NULL)
            //                free((char*)deviceInfo->secondaryKey);
            //            if (deviceInfo->generationId != NULL)
            //                free((char*)deviceInfo->generationId);
            //            if (deviceInfo->eTag != NULL)
            //                free((char*)deviceInfo->eTag);
            //            if (deviceInfo->connectionStateUpdatedTime != NULL)
            //                free((char*)deviceInfo->connectionStateUpdatedTime);
            //            if (deviceInfo->statusReason != NULL)
            //                free((char*)deviceInfo->statusReason);
            //            if (deviceInfo->statusUpdatedTime != NULL)
            //                free((char*)deviceInfo->statusUpdatedTime);
            //            if (deviceInfo->lastActivityTime != NULL)
            //                free((char*)deviceInfo->lastActivityTime);
            //            if (deviceInfo->configuration != NULL)
            //                free((char*)deviceInfo->configuration);
            //            if (deviceInfo->deviceProperties != NULL)
            //                free((char*)deviceInfo->deviceProperties);
            //            if (deviceInfo->serviceProperties != NULL)
            //                free((char*)deviceInfo->serviceProperties);

            //        //    free((void*)singlylinkedlist_item_get_value(current_item));
            //        }
            //        free(current_item);
            //    }
            //    free(list_instance);
            //}
        }
        umock_c_negative_tests_deinit();

        ///cleanup
    }

#define AAA
#ifdef AAA
    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_074: [ IoTHubRegistryManager_GetStatistics shall verify the input parameters and if any of them are NULL then return IOTHUB_REGISTRYMANAGER_INVALID_ARG ]*/
    TEST_FUNCTION(IoTHubRegistryManager_GetStatistics_return_IOTHUB_REGISTRYMANAGER_INVALID_ARG_if_input_parameter_registryManagerHandle_is_NULL)
    {
        ///arrange

        ///act
        IOTHUB_REGISTRYMANAGER_RESULT result = IoTHubRegistryManager_GetStatistics(NULL, &TEST_IOTHUB_REGISTRY_STATISTICS);

        ///assert
        ASSERT_ARE_EQUAL(int, IOTHUB_REGISTRYMANAGER_INVALID_ARG, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
    }

    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_074: [ IoTHubRegistryManager_GetStatistics shall verify the input parameters and if any of them are NULL then return IOTHUB_REGISTRYMANAGER_INVALID_ARG ]*/
    TEST_FUNCTION(IoTHubRegistryManager_GetStatistics_return_IOTHUB_REGISTRYMANAGER_INVALID_ARG_if_input_parameter_registryStatistics_is_NULL)
    {
        ///arrange

        ///act
        IOTHUB_REGISTRYMANAGER_RESULT result = IoTHubRegistryManager_GetStatistics(TEST_IOTHUB_REGISTRYMANAGER_HANDLE, NULL);

        ///assert
        ASSERT_ARE_EQUAL(int, IOTHUB_REGISTRYMANAGER_INVALID_ARG, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    }

    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_112: [ IoTHubRegistryManager_GetStatistics shall allocate memory for response buffer by calling BUFFER_new ]*/
    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_075: [ IoTHubRegistryManager_GetStatistics shall create HTTP GET request for statistics using the following format: url/statistics/devices?api-version ]*/
    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_076: [ IoTHubRegistryManager_GetStatistics shall add the following headers to the created HTTP GET request: authorization=sasToken,Request-Id=1001,Accept=application/json,Content-Type=application/json,charset=utf-8 ]*/
    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_077: [ IoTHubRegistryManager_GetStatistics shall create an HTTPAPIEX_SAS_HANDLE handle by calling HTTPAPIEX_SAS_Create ]*/
    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_078: [ IoTHubRegistryManager_GetStatistics shall create an HTTPAPIEX_HANDLE handle by calling HTTPAPIEX_Create ]*/
    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_079: [ IoTHubRegistryManager_GetStatistics shall execute the HTTP GET request by calling HTTPAPIEX_ExecuteRequest ]*/
    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_080: [ IoTHubRegistryManager_GetStatistics shall verify the received HTTP status code and if it is greater than 300 then return IOTHUB_REGISTRYMANAGER_ERROR ]*/
    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_081: [ IoTHubRegistryManager_GetStatistics shall verify the received HTTP status code and if it is less or equal than 300 then use the following parson APIs to parse the response JSON to registry statistics structure: json_parse_string, json_value_get_object, json_object_get_string, json_object_dotget_string ]*/
    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_083: [ IoTHubRegistryManager_GetStatistics shall save the registry statistics to the out value and return IOTHUB_REGISTRYMANAGER_OK ]*/
    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_114: [ IoTHubRegistryManager_GetStatistics shall do clean up before return ]*/
    TEST_FUNCTION(IoTHubRegistryManager_GetStatistics_happy_path)
    {
        ///arrange
        STRICT_EXPECTED_CALL(BUFFER_new());

        STRICT_EXPECTED_CALL(STRING_construct(TEST_HOSTNAME));
        STRICT_EXPECTED_CALL(STRING_construct(TEST_SHAREDACCESSKEY));
        STRICT_EXPECTED_CALL(STRING_construct(TEST_SHAREDACCESSKEYNAME));

        STRICT_EXPECTED_CALL(HTTPHeaders_Alloc());
        STRICT_EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, TEST_HTTP_HEADER_KEY_AUTHORIZATION, TEST_HTTP_HEADER_VAL_AUTHORIZATION))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, TEST_HTTP_HEADER_KEY_REQUEST_ID, TEST_HTTP_HEADER_VAL_REQUEST_ID))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, TEST_HTTP_HEADER_KEY_USER_AGENT, TEST_HTTP_HEADER_VAL_USER_AGENT))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, TEST_HTTP_HEADER_KEY_ACCEPT, TEST_HTTP_HEADER_VAL_ACCEPT))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, TEST_HTTP_HEADER_KEY_CONTENT_TYPE, TEST_HTTP_HEADER_VAL_CONTENT_TYPE))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(HTTPAPIEX_SAS_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreAllArguments();
        STRICT_EXPECTED_CALL(HTTPAPIEX_Create(TEST_HOSTNAME));

        STRICT_EXPECTED_CALL(HTTPAPIEX_SAS_ExecuteRequest(IGNORED_PTR_ARG, IGNORED_PTR_ARG, HTTPAPI_REQUEST_GET, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(2)
            .IgnoreArgument(4)
            .IgnoreArgument(5)
            .IgnoreArgument(6)
            .IgnoreArgument(7)
            .IgnoreArgument(8)
            .IgnoreArgument(9)
            .CopyOutArgumentBuffer_statusCode(&httpStatusCodeOk, sizeof(httpStatusCodeOk))
            .SetReturn(HTTPAPIEX_OK);

        STRICT_EXPECTED_CALL(HTTPHeaders_Free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPAPIEX_Destroy(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPAPIEX_SAS_Destroy(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(BUFFER_u_char(IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .SetReturn(TEST_UNSIGNED_CHAR_PTR);

        STRICT_EXPECTED_CALL(json_parse_string(IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .SetReturn(TEST_JSON_VALUE);

        STRICT_EXPECTED_CALL(json_value_get_object(TEST_JSON_VALUE))
            .SetReturn(TEST_JSON_OBJECT);

        STRICT_EXPECTED_CALL(json_object_get_number(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_TOTAL_DEVICECOUNT));
        STRICT_EXPECTED_CALL(json_object_get_number(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_ENABLED_DEVICECCOUNT));
        STRICT_EXPECTED_CALL(json_object_get_number(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DISABLED_DEVICECOUNT));

        STRICT_EXPECTED_CALL(json_object_clear(IGNORED_NUM_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(json_value_free(IGNORED_NUM_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(BUFFER_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        ///act
        IOTHUB_REGISTRYMANAGER_RESULT result = IoTHubRegistryManager_GetStatistics(TEST_IOTHUB_REGISTRYMANAGER_HANDLE, &TEST_IOTHUB_REGISTRY_STATISTICS);

        ///assert
        ASSERT_ARE_EQUAL(int, IOTHUB_REGISTRYMANAGER_OK, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
    }

    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_113: [ If the BUFFER_new fails, IoTHubRegistryManager_GetStatistics shall do clean up and return NULL ]*/
    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_116: [ If any of the HTTPAPI call fails IoTHubRegistryManager_GetStatistics shall fail and return IOTHUB_REGISTRYMANAGER_ERROR ]*/
    /* Tests_SRS_IOTHUBREGISTRYMANAGER_12_082: [ If the parsing failed, IoTHubRegistryManager_GetStatistics shall return IOTHUB_REGISTRYMANAGER_ERROR ]*/
    TEST_FUNCTION(IoTHubRegistryManager_GetStatistics_non_happy_path)
    {
        ///arrange
        int umockc_result = umock_c_negative_tests_init();
        ASSERT_ARE_EQUAL(int, 0, umockc_result);

        STRICT_EXPECTED_CALL(BUFFER_new());

        STRICT_EXPECTED_CALL(STRING_construct(TEST_HOSTNAME));
        STRICT_EXPECTED_CALL(STRING_construct(TEST_SHAREDACCESSKEY));
        STRICT_EXPECTED_CALL(STRING_construct(TEST_SHAREDACCESSKEYNAME));

        STRICT_EXPECTED_CALL(HTTPHeaders_Alloc());
        STRICT_EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, TEST_HTTP_HEADER_KEY_AUTHORIZATION, TEST_HTTP_HEADER_VAL_AUTHORIZATION))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, TEST_HTTP_HEADER_KEY_REQUEST_ID, TEST_HTTP_HEADER_VAL_REQUEST_ID))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, TEST_HTTP_HEADER_KEY_USER_AGENT, TEST_HTTP_HEADER_VAL_USER_AGENT))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, TEST_HTTP_HEADER_KEY_ACCEPT, TEST_HTTP_HEADER_VAL_ACCEPT))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, TEST_HTTP_HEADER_KEY_CONTENT_TYPE, TEST_HTTP_HEADER_VAL_CONTENT_TYPE))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(HTTPAPIEX_SAS_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreAllArguments();
        STRICT_EXPECTED_CALL(HTTPAPIEX_Create(TEST_HOSTNAME));

        STRICT_EXPECTED_CALL(HTTPAPIEX_SAS_ExecuteRequest(IGNORED_PTR_ARG, IGNORED_PTR_ARG, HTTPAPI_REQUEST_GET, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(2)
            .IgnoreArgument(4)
            .IgnoreArgument(5)
            .IgnoreArgument(6)
            .IgnoreArgument(7)
            .IgnoreArgument(8)
            .IgnoreArgument(9)
            .CopyOutArgumentBuffer_statusCode(&httpStatusCodeOk, sizeof(httpStatusCodeOk))
            .SetReturn(HTTPAPIEX_OK);

        STRICT_EXPECTED_CALL(HTTPHeaders_Free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPAPIEX_Destroy(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(HTTPAPIEX_SAS_Destroy(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(BUFFER_u_char(IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .SetReturn(TEST_UNSIGNED_CHAR_PTR);

        STRICT_EXPECTED_CALL(json_parse_string(IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .SetReturn(TEST_JSON_VALUE);

        STRICT_EXPECTED_CALL(json_value_get_object(TEST_JSON_VALUE))
            .SetReturn(TEST_JSON_OBJECT);

        STRICT_EXPECTED_CALL(json_object_get_number(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_TOTAL_DEVICECOUNT));
        STRICT_EXPECTED_CALL(json_object_get_number(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_ENABLED_DEVICECCOUNT));
        STRICT_EXPECTED_CALL(json_object_get_number(TEST_JSON_OBJECT, TEST_DEVICE_JSON_KEY_DISABLED_DEVICECOUNT));

        STRICT_EXPECTED_CALL(json_object_clear(IGNORED_NUM_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(json_value_free(IGNORED_NUM_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(BUFFER_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        umock_c_negative_tests_snapshot();

        ///act
        for (size_t i = 0; i < umock_c_negative_tests_call_count(); i++)
        {
            /// arrange
            umock_c_negative_tests_reset();
            umock_c_negative_tests_fail_call(i);

            /// act
            if (
                (i != 13) && /*HTTPHeaders_Free*/
                (i != 14) && /*HTTPAPIEX_Destroy*/
                (i != 15) && /*HTTPAPIEX_SAS_Destroy*/
                (i != 16) && /*STRING_delete*/
                (i != 17) && /*STRING_delete*/
                (i != 18) && /*STRING_delete*/
                (i != 22) && /*json_object_get_number*/
                (i != 23) && /*json_object_get_number*/
                (i != 24) && /*json_object_get_number*/
                (i != 26) && /*json_value_free*/
                (i != 27) /*BUFFER_delete*/
                )
            {
                IOTHUB_REGISTRYMANAGER_RESULT result = IoTHubRegistryManager_GetStatistics(TEST_IOTHUB_REGISTRYMANAGER_HANDLE, &TEST_IOTHUB_REGISTRY_STATISTICS);

                /// assert
                if (result == IOTHUB_REGISTRYMANAGER_OK)
                    printf("ddd");
                ASSERT_ARE_NOT_EQUAL(int, IOTHUB_REGISTRYMANAGER_OK, result);
            }

            ///cleanup
        }
        umock_c_negative_tests_deinit();
    }
#endif
    END_TEST_SUITE(iothub_registrymanager_ut)
