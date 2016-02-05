// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client.Transport.Mqtt
{
    using System;
    using DotNetty.Codecs.Mqtt.Packets;

    class MqttTransportSettings : ITransportSettings
    {
        readonly TransportType transportType;
        const bool DefaultCleanSession = false;
        const bool DefaultDeviceReceiveAckCanTimeout = false;
        const bool DefaultHasWill = false;
        const bool DefaultMaxOutboundRetransmissionEnforced = false;
        const int DefaultKeepAliveInSeconds = 300;
        const int DefaultMaxPendingInboundMessages = 50;
        const QualityOfService DefaultPublishToServerQoS = QualityOfService.AtLeastOnce;
        const QualityOfService DefaultReceivingQoS = QualityOfService.AtLeastOnce;
        static readonly TimeSpan DefaultConnectArrivalTimeout = TimeSpan.FromSeconds(300);
        static readonly TimeSpan DefaultDeviceReceiveAckTimeout = TimeSpan.FromSeconds(300);

        public MqttTransportSettings(TransportType transportType)
        {
            this.transportType = transportType;

            this.CleanSession = DefaultCleanSession;
            this.ConnectArrivalTimeout = DefaultConnectArrivalTimeout;
            this.DeviceReceiveAckCanTimeout = DefaultDeviceReceiveAckCanTimeout;
            this.DeviceReceiveAckTimeout = DefaultDeviceReceiveAckTimeout;
            this.DupPropertyName = "mqtt-dup";
            this.HasWill = DefaultHasWill;
            this.KeepAliveInSeconds = DefaultKeepAliveInSeconds;
            this.MaxOutboundRetransmissionEnforced = DefaultMaxOutboundRetransmissionEnforced;
            this.MaxPendingInboundMessages = DefaultMaxPendingInboundMessages;
            this.PublishToServerQoS = DefaultPublishToServerQoS;
            this.ReceivingQoS = DefaultReceivingQoS;
            this.QoSPropertyName = "mqtt-qos";
            this.RetainPropertyName = "mqtt-retain";
            this.WillMessage = null;
        }

        public bool DeviceReceiveAckCanTimeout { get; set; }

        public TimeSpan DeviceReceiveAckTimeout { get; set; }

        public QualityOfService PublishToServerQoS { get; set; }

        public QualityOfService ReceivingQoS { get; set; }

        public string RetainPropertyName { get; set; }

        public string DupPropertyName { get; set; }

        public string QoSPropertyName { get; set; }

        public bool MaxOutboundRetransmissionEnforced { get; set; }

        public int MaxPendingInboundMessages { get; set; }

        public TimeSpan ConnectArrivalTimeout { get; set; }

        public bool CleanSession { get; set; }

        public int KeepAliveInSeconds { get; set; }

        public bool HasWill { get; set; }

        public IWillMessage WillMessage { get; set; }

        public TransportType GetTransportType()
        {
            return this.transportType;
        }
    }
}