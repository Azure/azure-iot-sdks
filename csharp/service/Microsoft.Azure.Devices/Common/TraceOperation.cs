// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Common
{
    enum TraceOperation
    {
        Initialize = 0,
        Create = 1,
        Delete = 2,
        Add = 3,
        Remove = 4,
        Open = 5,
        Close = 6,
        Send = 7,
        Receive = 8,
        Connect = 9,
        Accept = 10,
        Execute = 11,
        Bind = 12,
        Attach = 13,
        Abort = 14,
        Flow = 15,
        ActiveLinkRegistered = 16,
        ActiveLinkUpdated = 17,
        ActiveLinkExpired = 18,
        ActiveLinkRefreshed = 19,
    }
}
