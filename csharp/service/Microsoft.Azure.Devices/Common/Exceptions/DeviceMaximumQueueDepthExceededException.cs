// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
namespace Microsoft.Azure.Devices.Common.Exceptions
{
    using System;
    using System.Runtime.Serialization;

    [Serializable]
    public sealed class DeviceMaximumQueueDepthExceededException : IotHubException
    {
        public DeviceMaximumQueueDepthExceededException(int maximumQueueDepth)
            : this(maximumQueueDepth, null)
        {            
        }

        public DeviceMaximumQueueDepthExceededException(int maximumQueueDepth, string trackingId)
            : base("Device Queue depth cannot exceed {0} messages".FormatInvariant(maximumQueueDepth), trackingId)
        {
            this.MaximumQueueDepth = maximumQueueDepth;
        }

        public DeviceMaximumQueueDepthExceededException(string message)
            : base(message)
        {
        }

        public DeviceMaximumQueueDepthExceededException(string message, Exception innerException)
            : base(message, innerException)
        {
        }

        DeviceMaximumQueueDepthExceededException(SerializationInfo info, StreamingContext context)
            : base(info, context)
        {
            this.MaximumQueueDepth = info.GetInt32("MaximumQueueDepth");
        }

        internal int MaximumQueueDepth
        {
            get;
            private set;
        }

        /// <inheritdoc />
        public override void GetObjectData(SerializationInfo info, StreamingContext context)
        {
            base.GetObjectData(info, context);
            info.AddValue("MaximumQueueDepth", this.MaximumQueueDepth);
        }
    }
}
