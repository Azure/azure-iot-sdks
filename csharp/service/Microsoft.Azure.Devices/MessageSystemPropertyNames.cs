// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices
{
    /// <summary>
    /// The names of the system properties in the <see cref="Message"/> class.
    /// </summary>
    public static class MessageSystemPropertyNames
    {
        public const string MessageId = "message-id";

        public const string LockToken = "iothub-messagelocktoken";

        public const string SequenceNumber = "iothub-sequencenumber";

        public const string To = "to";

        public const string DeliveryCount = "iothub-deliverycount";

        public const string EnqueuedTime = "iothub-enqueuedtime";

        public const string ExpiryTimeUtc = "absolute-expiry-time";

        public const string CorrelationId = "correlation-id";

        public const string UserId = "user-id";

        public const string Operation = "iothub-operation";

        public const string Ack = "iothub-ack";

        public const string ConnectionDeviceId = "iothub-connection-device-id";

        public const string ConnectionDeviceGenerationId = "iothub-connection-auth-generation-id";

        public const string ConnectionAuthMethod = "iothub-connection-auth-method";
    }
}