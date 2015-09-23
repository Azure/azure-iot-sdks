// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef URLENCODE_H
#define URLENCODE_H

#include "strings.h"

#ifdef __cplusplus
extern "C" {
#endif

extern STRING_HANDLE URL_EncodeString(const char* textEncode);
extern STRING_HANDLE URL_Encode(STRING_HANDLE input);

#ifdef __cplusplus
}
#endif

#endif /* URLENCODE_H */

