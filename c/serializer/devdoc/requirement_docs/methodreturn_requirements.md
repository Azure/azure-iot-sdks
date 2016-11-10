# METHODRETURN requirements

METHODRETURN is a module that encapsulates the return from a Method. A Device Method return a 
a touple consisting of an int and a null terminated string. A METHODRETURN is immutable, that is,
once created, it can never be modified.

## References
[JSON value](http://www.json.org)

## APIs

```c
MOCKABLE_FUNCTION(, METHODRETURN_HANDLE, MethodReturn_Create, int, statusCode, const char*, jsonValue);
MOCKABLE_FUNCTION(, void, MethodReturn_Destroy, METHODRETURN_HANDLE, handle);
MOCKABLE_FUNCTION(, int, MethodReturn_GetStatusCode, METHODRETURN_HANDLE, handle);
MOCKABLE_FUNCTION(, const char*, MethodReturn_GetJsonValue, METHODRETURN_HANDLE, handle);
```


### MethodReturn_Create
```c
METHODRETURN_HANDLE MethodReturn_Create(int statusCode, const char* jsonValue)
```

`MethodReturn_Create` creates a Method Return type from a `statusCode` and a `jsonValue`. `jsonValue` can
be `NULL`.

**SRS_METHODRETURN_02_001: [** `MethodReturn_Create` shall create a non-NULL handle containing `statusCode` and a clone of `jsonValue`. **]**
**SRS_METHODRETURN_02_002: [** If any failure is encountered then `MethodReturn_Create` shall return `NULL` **]**

### MethodReturn_Destroy
```c
void MethodReturn_Destroy(METHODRETURN_HANDLE handle)
```

`MethodReturn_Destroy` frees all resources used by `handle`.

**SRS_METHODRETURN_02_003: [** If `handle` is `NULL` then `MethodReturn_Destroy` shall return. **]**
**SRS_METHODRETURN_02_004: [** Otherwise,  `MethodReturn_Destroy` shall free all used resources by `handle`. **]**

### MethodReturn_GetStatusCode
```c
int MethodReturn_GetStatusCode(METHODRETURN_HANDLE handle)
```

`MethodReturn_GetStatusCode` returns the stored status code for a method return.

**SRS_METHODRETURN_02_005: [** If `handle` is `NULL` then `MethodReturn_GetStatusCode` shall return `MIN_INT`+1. **]**
**SRS_METHODRETURN_02_006: [** Otherwise, `MethodReturn_GetStatusCode` shall return the stored status code. **]**

### MethodReturn_GetJsonValue
```c
const char* MethodReturn_GetJsonValue(METHODRETURN_HANDLE handle)
```

`MethodReturn_GetJsonValue` returns the stored JSON value for a method return.

**SRS_METHODRETURN_02_007: [** If `handle` is `NULL` then `MethodReturn_GetJsonValue` shall return `NULL`. **]**
**SRS_METHODRETURN_02_008: [** Otherwise, `MethodReturn_GetJsonValue` shall return the stored JSON value. **]**
