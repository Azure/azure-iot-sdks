// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client
{
    using System;
    using System.Threading.Tasks;

#if !WINDOWS_UWP
    using Microsoft.Azure.Amqp;
#endif

    sealed class IotHubTokenRefresher
    {
#if !WINDOWS_UWP
        static readonly TimeSpan RefreshTokenBuffer = TimeSpan.FromMinutes(2);
        static readonly TimeSpan RefreshTokenRetryInterval = TimeSpan.FromSeconds(30);
        static readonly string[] AccessRightsStringArray = AccessRightsHelper.AccessRightsToStringArray(AccessRights.DeviceConnect);

        readonly AmqpSession amqpSession;
        readonly IotHubConnectionString connectionString;
        readonly string audience;
        volatile bool taskCancelled;

        public IotHubTokenRefresher(AmqpSession amqpSession, IotHubConnectionString connectionString, string audience)
        {
            if (amqpSession == null)
            {
                throw new ArgumentNullException("amqpSession");
            }

            this.amqpSession = amqpSession;
            this.connectionString = connectionString;
            this.audience = audience;
            this.taskCancelled = false;
        }

        public void Cancel()
        {
            this.taskCancelled = true;
        }

        public async Task SendCbsTokenAsync(TimeSpan timeout)
        {
            // Send a Cbs Token right away and fork off a task to continuously do it
            var cbsLink = this.amqpSession.Connection.Extensions.Find<AmqpCbsLink>();
            var expiresAtUtc = await cbsLink.SendTokenAsync(
                this.connectionString,
                this.connectionString.AmqpEndpoint,
                this.audience,
                this.connectionString.AmqpEndpoint.AbsoluteUri,
                AccessRightsStringArray,
                timeout);

            this.SendCbsTokenLoopAsync(timeout).Fork();
        }

        async Task SendCbsTokenLoopAsync(TimeSpan timeout)
        {
            while (!this.amqpSession.IsClosing())
            {
                if (this.taskCancelled)
                {
                    break;
                }

                var cbsLink = this.amqpSession.Connection.Extensions.Find<AmqpCbsLink>();
                if (cbsLink != null)
                {
                    try
                    {
                        var expiresAtUtc = await cbsLink.SendTokenAsync(
                             this.connectionString,
                             this.connectionString.AmqpEndpoint,
                             this.audience,
                             this.connectionString.AmqpEndpoint.AbsoluteUri,
                             AccessRightsStringArray,
                             timeout);
                        await Task.Delay(RefreshTokenBuffer);
                    }
                    catch (Exception exception)
                    {
                        if (Fx.IsFatal(exception))
                        {
                            throw;
                        }

                        await Task.Delay(RefreshTokenRetryInterval);
                    }
                }
                else
                {
                    break;
                }
            }
        }
    }
#endif
}
