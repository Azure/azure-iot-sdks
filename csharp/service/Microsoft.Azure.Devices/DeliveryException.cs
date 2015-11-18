// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices
{
    using System;
    using System.Runtime.Serialization;

    [Serializable]
    public class DeliveryException : Exception
    {
        public DeliveryFailureReason Reason { get; set; }

        public DeliveryException(string message)
            : base(message)
        {
        }

        public DeliveryException(SerializationInfo info, StreamingContext context)
            : base(info, context)
        {
            this.Reason = (DeliveryFailureReason)info.GetValue("Reason", typeof(DeliveryFailureReason));
        }

        /// <inheritdoc />
        public override void GetObjectData(SerializationInfo info, StreamingContext context)
        {
            base.GetObjectData(info, context);
            info.AddValue("Reason", this.Reason);
        }
    }
}
