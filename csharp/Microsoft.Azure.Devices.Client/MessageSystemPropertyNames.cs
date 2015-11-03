// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client
{
    static class MessageSystemPropertyNames
    {
        public const string MessageId = "message-id";

        public const string LockToken = "iothub-messagelocktoken";

        public const string SequenceNumber = "sequence-number";

        public const string To = "to";

        public const string EnqueuedTime = "iothub-enqueuedtime";

        public const string ExpiryTimeUtc = "absolute-expiry-time";

        public const string CorrelationId = "correlation-id";

        public const string DeliveryCount = "iothub-deliverycount";

        public const string UserId = "user-id";

        public const string Operation = "iothub-operation";

        public const string Ack = "iothub-ack";
    }
}