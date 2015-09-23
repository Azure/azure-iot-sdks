// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef SASTOKEN_H
#define SASTOKEN_H

#include "strings.h"
#include "buffer_.h"

#ifdef __cplusplus
extern "C" {
#endif

extern STRING_HANDLE SASToken_Create(STRING_HANDLE key, STRING_HANDLE scope, STRING_HANDLE keyName, size_t expiry);

#ifdef __cplusplus
}
#endif

#endif /* SASTOKEN_H */
