// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Common.Exceptions
{
    using System;
    using System.Runtime.Serialization;

    [Serializable]
    public sealed class DeviceInvalidResultCountException : IotHubException
    {
        public DeviceInvalidResultCountException(int maximumResultCount)
            : base("Number of device results must be between 0 and {0}".FormatInvariant(maximumResultCount))
        {
            this.MaximumResultCount = maximumResultCount;
        }

        DeviceInvalidResultCountException(SerializationInfo info, StreamingContext context)
            : base(info, context)
        {
            this.MaximumResultCount = info.GetInt32("MaximumResultCount");
        }

        internal int MaximumResultCount
        {
            get;
            private set;
        }

        /// <inheritdoc />
        public override void GetObjectData(SerializationInfo info, StreamingContext context)
        {
            base.GetObjectData(info, context);
            info.AddValue("MaximumResultCount", this.MaximumResultCount);
        }
    }
}
