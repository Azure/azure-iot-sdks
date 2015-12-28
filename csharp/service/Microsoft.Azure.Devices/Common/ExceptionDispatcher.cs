// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Common
{
    using System;

    sealed class ExceptionDispatcher
    {
        public static void Throw(Exception exception)
        {
            exception.PrepareForRethrow();
            throw exception;
        }
    }
}
