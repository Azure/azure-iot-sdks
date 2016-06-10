// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "azure_c_shared_utility/xio.h"
#include "azure_c_shared_utility/socketio.h"

XIO_HANDLE dm_io_create(const char* iotHubName)
{
    SOCKETIO_CONFIG config = { iotHubName, 5683 };
    return xio_create(socketio_get_interface_description(), &config, NULL);
}
