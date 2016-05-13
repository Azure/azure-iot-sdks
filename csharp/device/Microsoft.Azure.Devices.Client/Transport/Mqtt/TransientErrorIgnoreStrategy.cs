// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client.Transport.Mqtt
{
    using System;
    using Microsoft.Azure.Devices.Client.Extensions;
    using Microsoft.Practices.EnterpriseLibrary.TransientFaultHandling;

    sealed class TransientErrorIgnoreStrategy : ITransientErrorDetectionStrategy
    {
        /// <summary>
        /// Always returns false.
        /// 
        /// </summary>
        /// <param name="ex">The exception.</param>
        /// <returns>
        /// Always false.
        /// </returns>
        public bool IsTransient(Exception ex)
        {
            return !ex.IsFatal();
        }
    }
}