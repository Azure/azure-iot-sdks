// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

package com.microsoft.azure.iothub;

/**
 * The application-layer protocol used by the client to communicate with an IoT
 * Hub.
 */
public enum IotHubClientProtocol
{
    HTTPS, AMQPS, MQTT, AMQPS_WS
}
