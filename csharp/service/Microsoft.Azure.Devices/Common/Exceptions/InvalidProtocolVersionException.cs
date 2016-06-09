// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Common.Exceptions
{
    using System;
    using System.Runtime.Serialization;

#if !WINDOWS_UWP
    [Serializable]
#endif
    public class InvalidProtocolVersionException : IotHubException
    {
        public InvalidProtocolVersionException(string version)
            : base(!string.IsNullOrEmpty(version) ? "Invalid protocol version: " + version : "Protocol version is required. But, it was not provided")
        {
            this.RequestedVersion = version;
        }

#if !WINDOWS_UWP
        public InvalidProtocolVersionException(SerializationInfo info, StreamingContext context)
            : base(info, context)
        {
            this.RequestedVersion = info.GetString("RequestedVersion");
        }

        public override void GetObjectData(SerializationInfo info, StreamingContext context)
        {
            if (info == null)
            {
                throw new ArgumentException("info");
            }

            info.AddValue("RequestedVersion", this.RequestedVersion);

            base.GetObjectData(info, context);
        }
#endif
        public string RequestedVersion { get; private set; }
    }
}
