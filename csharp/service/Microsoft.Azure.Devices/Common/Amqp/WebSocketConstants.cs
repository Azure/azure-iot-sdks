﻿// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Amqp.Transport
{
    internal static class WebSocketConstants
    {
        internal const string Scheme = "wss://";
        internal const string UriSuffix = "/$iothub/websocket";
        internal const string SecurePort = "443";

        internal static class SubProtocols
        {
            public const string Amqpwsb10 = "AMQPWSB10";
        }
    }
}