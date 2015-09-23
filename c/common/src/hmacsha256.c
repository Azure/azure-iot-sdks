// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#include "gballoc.h"

#include "hmacsha256.h"
#include "hmac.h"
#include "strings.h"



HMACSHA256_RESULT HMACSHA256_ComputeHash(const unsigned char* key, size_t keyLen, const unsigned char* payload, size_t payloadLen, BUFFER_HANDLE hash)
{
    HMACSHA256_RESULT result;

    if (key == NULL ||
        keyLen == 0 ||
        payload == NULL ||
        payloadLen == 0 ||
        hash == NULL)
    {
        result = HMACSHA256_INVALID_ARG;
    }
    else
    {
        if ((BUFFER_enlarge(hash, 32) != 0) ||
            (hmac(SHA256, payload, payloadLen, key, keyLen, BUFFER_u_char(hash) ) != 0))
        {
            result = HMACSHA256_ERROR;
        }
        else
        {
            result = HMACSHA256_OK;
        }
    }

    return result;
}
