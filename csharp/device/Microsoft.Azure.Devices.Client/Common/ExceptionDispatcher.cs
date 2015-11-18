// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client
{
    using System;
    using Microsoft.Azure.Devices.Client.Extensions;

    sealed class ExceptionDispatcher
    {
        public static void Throw(Exception exception)
        {
            exception.PrepareForRethrow();
            throw exception;
        }
    }
}
