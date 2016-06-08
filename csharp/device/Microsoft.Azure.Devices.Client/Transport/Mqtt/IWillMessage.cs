// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client.Transport.Mqtt
{
    using DotNetty.Codecs.Mqtt.Packets;

    public interface IWillMessage
    {
        Message Message { get; }

        QualityOfService QoS { get; set; }
    }
}