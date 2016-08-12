// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "azure_c_shared_utility/crt_abstractions.h"
#include <stddef.h>

size_t sizeof_bool(void)
{
    return sizeof(bool);
}
