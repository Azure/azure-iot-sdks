// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "xio.h"
#include "tlsio.h"
#include "platform.h"

XIO_HANDLE dm_io_create(const char* iotHubName)
{
    TLSIO_CONFIG config = { iotHubName, 5684 };
    return xio_create(platform_get_default_tlsio(), &config, NULL);
}
