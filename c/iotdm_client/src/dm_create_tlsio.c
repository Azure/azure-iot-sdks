// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "azure_c_shared_utility/xio.h"
#include "azure_c_shared_utility/tlsio.h"
#include "azure_c_shared_utility/platform.h"
#include "certs.h"

XIO_HANDLE dm_io_create(const char* iotHubName)
{
    TLSIO_CONFIG config = { iotHubName, 5684 };
    XIO_HANDLE h = xio_create(platform_get_default_tlsio(), &config, NULL);
    if (h != NULL)
    {   
        (void)xio_setoption(h, "TrustedCerts", certificates);
    }
    return h;
}
