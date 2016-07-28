// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.-
#if !PCL
namespace Microsoft.Azure.Devices.Client.Exceptions
{
    using Microsoft.Azure.Amqp;
    using Microsoft.Azure.Amqp.Encoding;

    static class IotHubAmqpErrorCode
    {
        // Properties
        public static readonly AmqpSymbol TimeoutName = AmqpConstants.Vendor + ":timeout";
        public static readonly AmqpSymbol StackTraceName = AmqpConstants.Vendor + ":stack-trace";

        // Error codes
        public static readonly AmqpSymbol DeadLetterName = AmqpConstants.Vendor + ":dead-letter";
        public const string DeadLetterReasonHeader = "DeadLetterReason";
        public const string DeadLetterErrorDescriptionHeader = "DeadLetterErrorDescription";
        public static readonly AmqpSymbol TimeoutError = AmqpConstants.Vendor + ":timeout";
        public static readonly AmqpSymbol MessageLockLostError = AmqpConstants.Vendor + ":message-lock-lost";
        public static readonly AmqpSymbol IotHubNotFoundError = AmqpConstants.Vendor + ":iot-hub-not-found-error";
        public static readonly AmqpSymbol ArgumentError = AmqpConstants.Vendor + ":argument-error";
        public static readonly AmqpSymbol ArgumentOutOfRangeError = AmqpConstants.Vendor + ":argument-out-of-range";
        public static readonly AmqpSymbol DeviceAlreadyExists = AmqpConstants.Vendor + ":device-already-exists";
        public static readonly AmqpSymbol DeviceContainerThrottled = AmqpConstants.Vendor + ":device-container-throttled";
        public static readonly AmqpSymbol PartitionNotFound = AmqpConstants.Vendor + ":partition-not-found";
        public static readonly AmqpSymbol IotHubSuspended = AmqpConstants.Vendor + ":iot-hub-suspended";
    }
}
#endif
