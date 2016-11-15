# METHODRETURN requirements

METHODRETURN is a module that encapsulates the return from a Method. A Device Method return a 
a touple consisting of an int and a null terminated string. A METHODRETURN is immutable, that is,
once created, it can never be modified.

## References
[JSON value](http://www.json.org)

## APIs

```c

typedef struct METHODRETURN_DATA_TAG
{
    const int statusCode;
    const char* jsonValue;
}METHODRETURN_DATA;

extern METHODRETURN_HANDLE MethodReturn_Create(int statusCode, const char* jsonValue);
extern void, MethodReturn_Destroy(METHODRETURN_HANDLE handle);
extern const METHODRETURN_DATA* MethodReturn_GetReturn(METHODRETURN_HANDLE handle);
```


### MethodReturn_Create
```c
METHODRETURN_HANDLE MethodReturn_Create(int statusCode, const char* jsonValue)
```

`MethodReturn_Create` creates a Method Return type from a `statusCode` and a `jsonValue`. `jsonValue` can
be `NULL`.

**SRS_METHODRETURN_02_001: [** `MethodReturn_Create` shall create a non-NULL handle containing `statusCode` and a clone of `jsonValue`. **]**

**SRS_METHODRETURN_02_009: [** If `jsonValue` is not a JSON value then `MethodReturn_Create` shall fail and return `NULL`. **]**

**SRS_METHODRETURN_02_002: [** If any failure is encountered then `MethodReturn_Create` shall return `NULL` **]**

### MethodReturn_Destroy
```c
void MethodReturn_Destroy(METHODRETURN_HANDLE handle)
```

`MethodReturn_Destroy` frees all resources used by `handle`.

**SRS_METHODRETURN_02_003: [** If `handle` is `NULL` then `MethodReturn_Destroy` shall return. **]**

**SRS_METHODRETURN_02_004: [** Otherwise,  `MethodReturn_Destroy` shall free all used resources by `handle`. **]**
 
 ### MethodReturn_GetReturn
 ```c
 const METHODRETURN_DATA* MethodReturn_GetReturn(METHODRETURN_HANDLE handle)
 ```

 `MethodReturn_GetReturn` returns the return value (made of an `int` and a JSON value as string) from a Method.

 **SRS_METHODRETURN_02_010: [** If `handle` is `NULL` then `MethodReturn_GetReturn` shall fail and return `NULL`. **]**
 
 **SRS_METHODRETURN_02_011: [** Otherwise, `MethodReturn_GetReturn` shall return a non-`NULL` const pointer to a `METHODRETURN_DATA`. **]** 
