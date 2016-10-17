// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client
{
    /// <summary>
    /// Transport types supported by DeviceClient - AMQP/TCP, HTTP 1.1, MQTT/TCP, AMQP/WS, MQTT/WS
    /// </summary>
    public enum TransportType
    {
        /// <summary>
        /// Advanced Message Queuing Protocol transport.
        /// Try Amqp over TCP first and fallback to Amqp over WebSocket if that fails
        /// </summary>
        Amqp = 0,

        /// <summary>
        /// HyperText Transfer Protocol version 1 transport.
        /// </summary>
        Http1 = 1,

        /// <summary>
        /// Advanced Message Queuing Protocol transport over WebSocket only.
        /// </summary>
        Amqp_WebSocket_Only = 2,

        /// <summary>
        /// Advanced Message Queuing Protocol transport over native TCP only
        /// </summary>
        Amqp_Tcp_Only = 3,

        /// <summary>
        /// Message Queuing Telemetry Transport.
        /// Try Mqtt over TCP first and fallback to Mqtt over WebSocket if that fails
        /// </summary>
        Mqtt = 4,

        /// <summary>
        /// Message Queuing Telemetry Transport over Websocket only.
        /// </summary>
        Mqtt_WebSocket_Only = 5,

        /// <summary>
        /// Message Queuing Telemetry Transport over native TCP only
        /// </summary>
        Mqtt_Tcp_Only = 6,
    }
}
