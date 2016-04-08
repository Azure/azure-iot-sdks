// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client
{
    using System;
    using System.Threading.Tasks;
    using Microsoft.Azure.Amqp;
    using Microsoft.Azure.Devices.Client.Extensions;

    sealed class IotHubSingleTokenConnection : IotHubConnection
    {
        readonly IotHubScopeConnectionPool iotHubScopeConnectionPool;
        IotHubTokenRefresher iotHubTokenRefresher;

        public IotHubSingleTokenConnection(IotHubScopeConnectionPool iotHubScopeConnectionPool, IotHubConnectionString connectionString, AmqpTransportSettings amqpTransportSettings)
            :base(connectionString.HostName, connectionString.AmqpEndpoint.Port, amqpTransportSettings)
        {
            this.iotHubScopeConnectionPool = iotHubScopeConnectionPool;
            this.ConnectionString = connectionString;
            this.FaultTolerantSession = new FaultTolerantAmqpObject<AmqpSession>(this.CreateSessionAsync, this.CloseConnection);
        }

        public IotHubConnectionString ConnectionString { get; }

        public override Task CloseAsync()
        {
            return this.FaultTolerantSession.CloseAsync();
        }

        public override void SafeClose(Exception exception)
        {
            this.FaultTolerantSession.Close();
        }

        public override void Release(string doNotUse)
        {
            this.iotHubScopeConnectionPool.RemoveRef();
        }

        protected override async Task<AmqpSession> CreateSessionAsync(TimeSpan timeout)
        {
            var timeoutHelper = new TimeoutHelper(timeout);

            if (this.iotHubTokenRefresher != null)
            {
                this.iotHubTokenRefresher.Cancel();
            }

            AmqpSession amqpSession = await base.CreateSessionAsync(timeoutHelper.RemainingTime());
            this.iotHubTokenRefresher = new IotHubTokenRefresher(
                amqpSession, 
                this.ConnectionString, 
                this.ConnectionString.AmqpEndpoint.AbsoluteUri
                );

            // Send Cbs token for new connection first
            await this.iotHubTokenRefresher.SendCbsTokenAsync(timeoutHelper.RemainingTime());

            return amqpSession;
        }

        protected override Uri BuildLinkAddress(IotHubConnectionString doNotUse, string path)
        {
            return this.ConnectionString.BuildLinkAddress(path);
        }

        protected override string BuildAudience(IotHubConnectionString doNotUse, string donotUse2)
        {
            return string.Empty;
        }

        protected override async Task OpenLinkAsync(AmqpObject link, IotHubConnectionString doNotUse, string doNotUse2, TimeSpan timeout)
        {
            try
            {
                await link.OpenAsync(timeout);
            }
            catch (Exception exception)
            {
                if (exception.IsFatal())
                {
                    throw;
                }

                link.SafeClose(exception);

                throw;
            }
        }

        void CloseConnection(AmqpSession amqpSession)
        {
            // Closing the connection also closes any sessions.
            amqpSession.Connection.SafeClose();
            this.iotHubTokenRefresher.Cancel();
        }
    }
}