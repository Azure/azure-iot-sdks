// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices
{
    // TODO : we should just throw typed exceptions rather than this
    public enum DeliveryFailureReason
    {
        TimeToLiveExpired,
        DeliveryCountExceeded,
    }
}
