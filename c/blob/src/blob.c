// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "blob.h"

BLOB_RESULT Blob_UploadFromSasUri(const char* SASURI, const unsigned char* source, size_t size)
{
    BLOB_RESULT result;
    if (SASURI == NULL)
    {
        LogError("parameter SASURI is NULL");
        result = BLOB_INVALID_ARG;
    }
    else
    {
        if (
            (
                (size > 0) &&
                (source == NULL)
            ) ||
            (source >=64*1024*1024)
            )
        {
            LogError("combination of source = %p and size = %zd is invalid", source, size);
            result = BLOB_INVALID_ARG;
        }
        else
        {
            
        }
    }
    return result;
}