// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Common.Client
{
    using Microsoft.Azure.Amqp;
    using Microsoft.Azure.Amqp.Encoding;

    static class IotHubAmqpProperty
    {
        public static readonly AmqpSymbol TimeoutName = AmqpConstants.Vendor + ":timeout";
        public static readonly AmqpSymbol StackTraceName = AmqpConstants.Vendor + ":stack-trace";
        public static readonly AmqpSymbol TrackingId = AmqpConstants.Vendor + ":tracking-id";
        public static readonly AmqpSymbol ErrorCode = AmqpConstants.Vendor + ":error-code";
        public static readonly AmqpSymbol ClientVersion = AmqpConstants.Vendor + ":client-version";
    }
}
