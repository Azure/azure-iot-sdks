// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client.Transport.Mqtt
{
    using DotNetty.Codecs.Mqtt.Packets;

    public class WillMessage : IWillMessage
    {
        public Message Message { get; private set; }

        public QualityOfService QoS { get; set; }

        public WillMessage(QualityOfService qos, Message message)
        {
            this.QoS = qos;
            this.Message = message;
        }
    }
}