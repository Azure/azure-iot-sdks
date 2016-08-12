// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client.Transport.Mqtt
{
    using System;

    class MqttIotHubAdapterFactory 
    {
        readonly MqttTransportSettings settings;

        public MqttIotHubAdapterFactory(MqttTransportSettings settings)
        {
            this.settings = settings;
        }

        public MqttIotHubAdapter Create(
            Action onConnected, 
            Action<Message> onMessageReceived,
            Action<Exception> onError, 
            IotHubConnectionString iotHubConnectionString, 
            MqttTransportSettings mqttTransportSettings)
        {
            IWillMessage willMessage = mqttTransportSettings.HasWill ? this.settings.WillMessage : null;
            
            return new MqttIotHubAdapter(
                iotHubConnectionString.DeviceId,
                iotHubConnectionString.HostName,
                mqttTransportSettings.ClientCertificate != null ? null : iotHubConnectionString.GetPassword(),
                mqttTransportSettings,
                willMessage,
                onConnected,
                onMessageReceived,
                onError);
        }
    }
}