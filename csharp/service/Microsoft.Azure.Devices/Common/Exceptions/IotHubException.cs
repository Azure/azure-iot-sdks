// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Common.Exceptions
{
    using System;
    using System.Runtime.Serialization;

#if !WINDOWS_UWP
    [Serializable]
#endif
    public class IotHubException : Exception
    {
        const string IsTransientValueSerializationStoreName = "IotHubException-IsTransient";
        const string TrackingIdSerializationStoreName = "IoTHubException-TrackingId";
        const string ErrorCodeName = "ErrorCode";

        public bool IsTransient { get; private set; }

        public string TrackingId { get; set; }

        public IotHubException(string message)
            : this(message, false)
        {
        }

        public IotHubException(string message, string trackingId)
            : this(message, false, trackingId)
        {
        }

        public IotHubException(string message, bool isTransient, string trackingId)
            : this(message, null, isTransient, trackingId)
        {
        }

        public IotHubException(string message, bool isTransient)
            : this(message, null, isTransient, trackingId: string.Empty)
        {
        }

        public IotHubException(Exception innerException)
            : base(string.Empty, innerException)
        {
        }

        public IotHubException(string message, Exception innerException)
            : this(message, innerException, false, string.Empty)
        {
        }

        protected IotHubException(string message, Exception innerException, bool isTransient)
            : this(message, innerException, isTransient, trackingId: string.Empty)
        {
        }

        protected IotHubException(string message, Exception innerException, bool isTransient, string trackingId)
            : base(message, innerException)
        {
            this.IsTransient = isTransient;
            this.TrackingId = trackingId;
        }

#if !WINDOWS_UWP
        protected IotHubException(SerializationInfo info, StreamingContext context)
            : base(info, context)
        {
            if (info != null)
            {
                this.IsTransient = info.GetBoolean(IsTransientValueSerializationStoreName);
                this.TrackingId = info.GetString(TrackingIdSerializationStoreName);
            }
        }

        public override void GetObjectData(SerializationInfo info, StreamingContext context)
        {
            base.GetObjectData(info, context);
            info.AddValue(IsTransientValueSerializationStoreName, this.IsTransient);
            info.AddValue(TrackingIdSerializationStoreName, this.TrackingId);
        }
#endif
        public ErrorCode Code
        {
            get;
            private set;
        }
    }
}
