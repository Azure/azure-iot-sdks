// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef JSONDECODER_H
#define JSONDECODER_H

#include "multitree.h"

#ifdef __cplusplus
#include <cstddef>
extern "C" {
#else
#include <stddef.h>
#endif

typedef enum JSON_DECODER_RESULT_TAG
{
    JSON_DECODER_OK,
    JSON_DECODER_INVALID_ARG,
    JSON_DECODER_PARSE_ERROR,
    JSON_DECODER_MULTITREE_FAILED,
    JSON_DECODER_ERROR
} JSON_DECODER_RESULT;

#include "azure_c_shared_utility/umock_c_prod.h"
MOCKABLE_FUNCTION(, JSON_DECODER_RESULT, JSONDecoder_JSON_To_MultiTree, char*, json, MULTITREE_HANDLE*, multiTreeHandle);

#ifdef __cplusplus
}
#endif

#endif /* JSONDECODER_H */
