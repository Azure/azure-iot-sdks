#Blob Requirements

##Overview

References
[Operations on Block Blobs](https://msdn.microsoft.com/en-us/library/azure/ee691974.aspx)
[Put Block](https://msdn.microsoft.com/en-us/library/azure/dd135726.aspx)
[Put Block List](https://msdn.microsoft.com/en-us/library/azure/dd179467.aspx)

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
    extern BLOB_RESULT Blob_UploadFromSasUri(const char* SASURI, const unsigned char* source, size_t size, const unsigned int* httpStatus, BUFFER_HANDLE httpResponse);
```

##Blob_UploadFromSasUri 
```c
BLOB_RESULT Blob_UploadFromSasUri(const char* SASURI, const unsigned char* source, size_t size, const unsigned int* httpStatus, BUFFER_HANDLE httpResponse)
```
`Blob_UploadFromSasUri` uploads as a Blob the array of bytes pointed to by `source` having size `size` by using HTTPAPI_EX module.

**SRS_BLOB_02_001: [** If `SASURI` is NULL then `Blob_UploadFromSasUri` shall fail and return `BLOB_INVALID_ARG`. **]**
**SRS_BLOB_02_002: [** If `source` is NULL and `size` is not zero then `Blob_UploadFromSasUri` shall fail and return `BLOB_INVALID_ARG`. **]**
**SRS_BLOB_02_034: [** If size is bigger than 50000\*4\*1024\*1024 then `Blob_UploadFromSasUri` shall fail and return `BLOB_INVALID_ARG`. **]**

Steps to follow when size < 64MB:

**SRS_BLOB_02_004: [** `Blob_UploadFromSasUri` shall copy from `SASURI` the hostname to a new const char\*. **]** 
**SRS_BLOB_02_005: [** If the hostname cannot be determined, then `Blob_UploadFromSasUri` shall fail and return `BLOB_INVALID_ARG`. **]**
**SRS_BLOB_02_016: [** If the hostname copy cannot be made then then `Blob_UploadFromSasUri` shall fail and return `BLOB_ERROR` **]**
**SRS_BLOB_02_006: [** `Blob_UploadFromSasUri` shall create a new `HTTPAPI_EX_HANDLE` by calling `HTTPAPIEX_Create` passing the hostname. **]**
**SRS_BLOB_02_007: [** If `HTTPAPIEX_Create` fails then `Blob_UploadFromSasUri` shall fail and return `BLOB_ERROR`. **]**

In order for `Blob_UploadFromSasUri` to call `HTTPAPIEX_ExecuteRequest` it will build several data structures:

**SRS_BLOB_02_008: [** `Blob_UploadFromSasUri` shall compute the relative path of the request from the `SASURI` parameter. **]**
**SRS_BLOB_02_009: [** `Blob_UploadFromSasUri` shall create an `HTTP_HEADERS_HANDLE` for the request HTTP headers carrying the following headers: **]**
- x-ms-blob-type: BlockBlob

**SRS_BLOB_02_010: [** `Blob_UploadFromSasUri` shall create a BUFFER_HANDLE from `source` and `size` parameters. **]**
**SRS_BLOB_02_011: [** If any of the previous steps related to building the `HTTPAPI_EX_ExecuteRequest` parameters fails, then `Blob_UploadFromSasUri` shall fail and return `BLOB_ERROR`. **]**
**SRS_BLOB_02_012: [** `Blob_UploadFromSasUri` shall call `HTTPAPIEX_ExecuteRequest` passing the parameters previously build, `httpStatus` and `httpResponse` **]**
**SRS_BLOB_02_013: [** If `HTTPAPIEX_ExecuteRequest` fails, then `Blob_UploadFromSasUri` shall fail and return `BLOB_HTTP_ERROR`. **]**
**SRS_BLOB_02_015: [** Otherwise, `HTTPAPIEX_ExecuteRequest` shall succeed and return `BLOB_OK`. **]**

Steps to follow when size >= 64MB

Design considerations: Blob_UploadFromSasUri will break the souce into 4MB blocks.
These blocks have the IDs starting from 000000 and ending with 049999 (potentially). 
    Note: the URL encoding of the BASE64 of these numbers is the same as the BASE64 representation (therefore no URL encoding needed)
Blocks are uploaded serially by "Put Block" REST API. After all the blocks have been uploaded, a "Put Block List" is executed.

**SRS_BLOB_02_017: [** `Blob_UploadFromSasUri` shall copy from `SASURI` the hostname to a new const char\* **]**
**SRS_BLOB_02_018: [** `Blob_UploadFromSasUri` shall create a new `HTTPAPI_EX_HANDLE` by calling `HTTPAPIEX_Create` passing the hostname. **]**
**SRS_BLOB_02_019: [** `Blob_UploadFromSasUri` shall compute the base relative path of the request from the `SASURI` parameter. **]**
 
**SRS_BLOB_02_021: [** For every block of 4MB the following operations shall happen: **]**
  
1. **SRS_BLOB_02_020: [** `Blob_UploadFromSasUri` shall construct a BASE64 encoded string from the block ID (000000... 049999) **]**
2. **SRS_BLOB_02_022: [** `Blob_UploadFromSasUri` shall construct a new relativePath from following string: base relativePath + "&comp=block&blockid=BASE64 encoded string of blockId" **]**
3. **SRS_BLOB_02_023: [** `Blob_UploadFromSasUri` shall create a BUFFER_HANDLE from `source` and `size` parameters. **]**
4. **SRS_BLOB_02_024: [** `Blob_UploadFromSasUri` shall call `HTTPAPIEX_ExecuteRequest` with a PUT operation, passing `httpStatus` and `httpResponse`. **]**
5. **SRS_BLOB_02_025: [** If `HTTPAPIEX_ExecuteRequest` fails then `Blob_UploadFromSasUri` shall fail and return `BLOB_HTTP_ERROR`. **]**
6. **SRS_BLOB_02_026: [** Otherwise, if HTTP response code is >=300 then `Blob_UploadFromSasUri` shall succeed and return `BLOB_OK`. **]**
7. **SRS_BLOB_02_027: [** Otherwise `Blob_UploadFromSasUri` shall continue execution. **]**

**SRS_BLOB_02_028: [** `Blob_UploadFromSasUri` shall construct an XML string with the following content: **]**
```xml
<?xml version="1.0" encoding="utf-8"?>
<BlockList>
  <Latest>BASE64 encoding of the first 4MB block ID (MDAwMDAw)</Latest>
  <Latest>BASE64 encoding of the second 4MB block ID (MDAwMDAx)</Latest>
  ...
</BlockList>
```

**SRS_BLOB_02_029: [** `Blob_UploadFromSasUri` shall construct a new relativePath from following string: base relativePath + "&comp=blocklist" **]**
**SRS_BLOB_02_030: [** `Blob_UploadFromSasUri` shall call `HTTPAPIEX_ExecuteRequest` with a PUT operation, passing the new relativePath, `httpStatus` and `httpResponse` and the XML string as content. **]**
**SRS_BLOB_02_031: [** If `HTTPAPIEX_ExecuteRequest` fails then `Blob_UploadFromSasUri` shall fail and return `BLOB_HTTP_ERROR`. **]**
**SRS_BLOB_02_033: [** If any previous operation that doesn't have an explicit failure description fails then `Blob_UploadFromSasUri` shall fail and return `BLOB_ERROR` **]**  
**SRS_BLOB_02_032: [** Otherwise, `Blob_UploadFromSasUri` shall succeed and return `BLOB_OK`. **]**