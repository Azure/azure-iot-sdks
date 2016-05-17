// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client.Transport
{
    using System.Threading.Tasks;

    abstract class TransportHandler : DefaultDelegatingHandler
    {
        readonly ITransportSettings transportSettings;

        protected TransportHandler(ITransportSettings transportSettings)
        {
            this.transportSettings = transportSettings;
        }

        public override Task<Message> ReceiveAsync()
        {
            return this.ReceiveAsync(this.transportSettings.DefaultReceiveTimeout);
        }
    }
}