#Blob Requirements

##Overview

References

##Exposed API
```c
#define BLOB_RESULT_VALUES \
    BLOB_OK,               \
    BLOB_ERROR,            \
    BLOB_NOT_IMPLEMENTED,  \
    BLOB_HTTP_ERROR,       \
    BLOB_INVALID_ARG    

DEFINE_ENUM(BLOB_RESULT, BLOB_RESULT_VALUES)
    
    /**
    * @brief	Synchronously uploads a byte array to blob storage
    *
    * @param	SASURI	    The URI to use to upload data
    * @param	size		The size of the data to be uploaded (can be 0)
    * @param	source		A pointer to the byte array to be uploaded (can be NULL, but then size needs to be zero)
    *
    * @return	A @c BLOB_RESULT. BLOB_OK means the blob has been uploaded successfully. Any other value indicates an error
    */
    extern BLOB_RESULT Blob_UploadFromSASURI(const char* SASURI, const unsigned char* source, size_t size);
```

##Blob_UploadFromSASURI 
```c
BLOB_RESULT Blob_UploadFromSASURI(const char* SASURI, const unsigned char* source, size_t size)
```
`Blob_UploadFromSASURI` uploads as a Blob the array of bytes pointed to by `source` having size `size` by using HTTPAPI_EX module.

**SRS_BLOB_02_001: [** If `SASURI` is NULL then `Blob_UploadFromSASURI` shall fail and return `BLOB_INVALID_ARG`. **]**
**SRS_BLOB_02_002: [** If `source` is NULL and `size` is not zero then `Blob_UploadFromSASURI` shall fail and return `BLOB_INVALID_ARG`. **]**
**SRS_BLOB_02_003: [** If `size` is bigger or equal to 64M then `Blob_UploadFromSASURI` shall fail and return `BLOB_NOT_IMPLEMENTED`. **]**
**SRS_BLOB_02_004: [** `Blob_UploadFromSASURI` shall copy from `SASURI` the hostname to a new const char*. **]** 
**SRS_BLOB_02_005: [** If the hostname cannot be copied, then `Blob_UploadFromSASURI` shall fail and return `BLOB_ERROR`. **]**
**SRS_BLOB_02_006: [** `Blob_UploadFromSASURI` shall create a new `HTTPAPI_EX_HANDLE` by calling `HTTPAPIEX_Create` passing the hostname. **]**
**SRS_BLOB_02_007: [** If `HTTPAPIEX_Create` fails then `Blob_UploadFromSASURI` shall fail and return `BLOB_ERROR`. **]**

In order for `Blob_UploadFromSASURI` to call `HTTPAPIEX_ExecuteRequest` it will build several data structures:

**SRS_BLOB_02_008: [** `Blob_UploadFromSASURI` shall compute the relative path of the request from the `SASURI` parameter. **]**
**SRS_BLOB_02_009: [** `Blob_UploadFromSASURI` shall create an `HTTP_HEADERS_HANDLE` for the request HTTP headers carrying the following headers: **]**
- x-ms-blob-type: BlockBlob
**SRS_BLOB_02_010: [** `Blob_UploadFromSASURI` shall create a BUFFER_HANDLE from `source` and `size` parameters. **]**
**SRS_BLOB_02_011: [** If any of the previous steps related to building the `HTTPAPI_EX_ExecuteRequest` parameters fails, then `Blob_UploadFromSASURI` shall fail and return `BLOB_ERROR`. **]**
**SRS_BLOB_02_012: [** `Blob_UploadFromSASURI` shall call `HTTPAPIEX_ExecuteRequest` passing the parameters previously build. **]**
**SRS_BLOB_02_013: [** If `HTTPAPIEX_ExecuteRequest` fails, then `Blob_UploadFromSASURI` shall fail and return `BLOB_HTTP_ERROR`. **]**
**SRS_BLOB_02_014: [** If the `statusCode` returned by `HTTPAPIEX_ExecuteRequest` is bigger or equal to 300, then `Blob_UploadFromSASURI` shall fail and return `BLOB_HTTP_ERROR`. **]**
**SRS_BLOB_02_015: [** Otherwise, `HTTPAPIEX_ExecuteRequest` shall succeed and return `BLOB_OK`. **]**