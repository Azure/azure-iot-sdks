// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#include "azure_c_shared_utility/gballoc.h"

#include "blob.h"

#include "azure_c_shared_utility/httpapiex.h"
#include "azure_c_shared_utility/iot_logging.h"

BLOB_RESULT Blob_UploadFromSasUri(const char* SASURI, const unsigned char* source, size_t size, unsigned int* httpStatus, BUFFER_HANDLE httpResponse)
{
    BLOB_RESULT result;
    /*Codes_SRS_BLOB_02_001: [ If SASURI is NULL then Blob_UploadFromSasUri shall fail and return BLOB_INVALID_ARG. ]*/
    if (SASURI == NULL)
    {
        LogError("parameter SASURI is NULL");
        result = BLOB_INVALID_ARG;
    }
    else
    {
        /*Codes_SRS_BLOB_02_002: [ If source is NULL and size is not zero then Blob_UploadFromSasUri shall fail and return BLOB_INVALID_ARG. ]*/
        if (
            (size > 0) &&
            (source == NULL)
           )
        {
            LogError("combination of source = %p and size = %zd is invalid", source, size);
            result = BLOB_INVALID_ARG;
        }
        /*Codes_SRS_BLOB_02_003: [ If size is bigger or equal to 64M then Blob_UploadFromSasUri shall fail and return BLOB_NOT_IMPLEMENTED. ]*/
        else if (size >= 64 * 1024 * 1024)
        {
            LogError("upload of files greater than 64M is not implemented");
            result = BLOB_NOT_IMPLEMENTED;
        }
        else
        {
            /*Codes_SRS_BLOB_02_004: [ Blob_UploadFromSasUri shall copy from SASURI the hostname to a new const char*. ]*/
            /*to find the hostname, the following logic is applied:*/
            /*the hostname starts at the first character after "://"*/
            /*the hostname ends at the first character before the next "/" after "://"*/
            const char* hostnameBegin = strstr(SASURI, "://");
            if (hostnameBegin == NULL)
            {
                /*Codes_SRS_BLOB_02_005: [ If the hostname cannot be determined, then Blob_UploadFromSasUri shall fail and return BLOB_INVALID_ARG. ]*/
                LogError("hostname cannot be determined");
                result = BLOB_INVALID_ARG;
            }
            else
            {
                hostnameBegin += 3; /*have to skip 3 characters which are "://"*/
                const char* hostnameEnd = strchr(hostnameBegin, '/');
                if (hostnameEnd == NULL)
                {
                    /*Codes_SRS_BLOB_02_005: [ If the hostname cannot be determined, then Blob_UploadFromSasUri shall fail and return BLOB_INVALID_ARG. ]*/
                    LogError("hostname cannot be determined");
                    result = BLOB_INVALID_ARG;
                }
                else
                {
                    size_t hostnameSize = hostnameEnd - hostnameBegin;
                    char* hostname = (char*)malloc(hostnameSize + 1); /*+1 because of '\0' at the end*/
                    if (hostname == NULL)
                    {
                        /*Codes_SRS_BLOB_02_016: [ If the hostname copy cannot be made then then Blob_UploadFromSasUri shall fail and return BLOB_ERROR ]*/
                        LogError("oom - out of memory");
                        result = BLOB_ERROR;
                    }
                    else
                    {
                        HTTPAPIEX_HANDLE httpApiExHandle;
                        memcpy(hostname, hostnameBegin, hostnameSize);
                        hostname[hostnameSize] = '\0';

                        /*Codes_SRS_BLOB_02_006: [ Blob_UploadFromSasUri shall create a new HTTPAPI_EX_HANDLE by calling HTTPAPIEX_Create passing the hostname. ]*/
                        httpApiExHandle = HTTPAPIEX_Create(hostname);
                        if (httpApiExHandle == NULL)
                        {
                            /*Codes_SRS_BLOB_02_007: [ If HTTPAPIEX_Create fails then Blob_UploadFromSasUri shall fail and return BLOB_ERROR. ]*/
                            LogError("unable to create a HTTPAPIEX_HANDLE");
                            result = BLOB_ERROR;
                        }
                        else
                        {
                            /*Codes_SRS_BLOB_02_008: [ Blob_UploadFromSasUri shall compute the relative path of the request from the SASURI parameter. ]*/
                            const char* relativePath = hostnameEnd; /*this is where the relative path begins in the SasUri*/

                            /*Codes_SRS_BLOB_02_010: [ Blob_UploadFromSasUri shall create a BUFFER_HANDLE from source and size parameters. ]*/
                            BUFFER_HANDLE requestBuffer = BUFFER_create(source, size);
                            if (requestBuffer == NULL)
                            {
                                /*Codes_SRS_BLOB_02_011: [ If any of the previous steps related to building the HTTPAPI_EX_ExecuteRequest parameters fails, then Blob_UploadFromSasUri shall fail and return BLOB_ERROR. ]*/
                                LogError("unable to BUFFER_create");
                                result = BLOB_ERROR;
                            }
                            else
                            {
                                /*Codes_SRS_BLOB_02_009: [ Blob_UploadFromSasUri shall create an HTTP_HEADERS_HANDLE for the request HTTP headers carrying the following headers: ]*/
                                HTTP_HEADERS_HANDLE requestHttpHeaders = HTTPHeaders_Alloc();
                                if (requestHttpHeaders == NULL)
                                {
                                    /*Codes_SRS_BLOB_02_011: [ If any of the previous steps related to building the HTTPAPI_EX_ExecuteRequest parameters fails, then Blob_UploadFromSasUri shall fail and return BLOB_ERROR. ]*/
                                    LogError("unable to HTTPHeaders_Alloc");
                                    result = BLOB_ERROR;
                                }
                                else
                                {
                                    if (HTTPHeaders_AddHeaderNameValuePair(requestHttpHeaders, "x-ms-blob-type", "BlockBlob") != HTTP_HEADERS_OK)
                                    {
                                        /*Codes_SRS_BLOB_02_011: [ If any of the previous steps related to building the HTTPAPI_EX_ExecuteRequest parameters fails, then Blob_UploadFromSasUri shall fail and return BLOB_ERROR. ]*/
                                        LogError("unable to HTTPHeaders_AddHeaderNameValuePair");
                                        result = BLOB_ERROR;
                                    }
                                    else
                                    {
                                        /*Codes_SRS_BLOB_02_012: [ Blob_UploadFromSasUri shall call HTTPAPIEX_ExecuteRequest passing the parameters previously build, httpStatus and httpResponse ]*/
                                        if (HTTPAPIEX_ExecuteRequest(httpApiExHandle, HTTPAPI_REQUEST_PUT, relativePath, requestHttpHeaders, requestBuffer, httpStatus, NULL, httpResponse) != HTTPAPIEX_OK)
                                        {
                                            /*Codes_SRS_BLOB_02_013: [ If HTTPAPIEX_ExecuteRequest fails, then Blob_UploadFromSasUri shall fail and return BLOB_HTTP_ERROR. ]*/
                                            LogError("failed to HTTPAPIEX_ExecuteRequest");
                                            result = BLOB_HTTP_ERROR;
                                        }
                                        else
                                        {
                                            /*Codes_SRS_BLOB_02_015: [ Otherwise, HTTPAPIEX_ExecuteRequest shall succeed and return BLOB_OK. ]*/
                                            result = BLOB_OK;
                                        }
                                    }
                                    HTTPHeaders_Free(requestHttpHeaders);
                                }
                                BUFFER_delete(requestBuffer);
                            }
                            HTTPAPIEX_Destroy(httpApiExHandle);
                        }
                        free(hostname);
                    }
                }
            }
        }
    }
    return result;
}
