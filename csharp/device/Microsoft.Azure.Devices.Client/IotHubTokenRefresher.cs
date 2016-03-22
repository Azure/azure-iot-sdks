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

        readonly AmqpSession amqpSession;
        readonly IotHubConnectionString connectionString;
        readonly string audience;
        readonly IOThreadTimer refreshTokenTimer;
        readonly TimeSpan operationTimeout;
        readonly string[] accessRightsStringArray;

        public IotHubTokenRefresher(AmqpSession amqpSession, IotHubConnectionString connectionString, string audience, TimeSpan operationTimeout, AccessRights accessRights)
        {
            if (amqpSession == null)
            {
                throw new ArgumentNullException("amqpSession");
            }

            this.amqpSession = amqpSession;
            this.connectionString = connectionString;
            this.audience = audience;
            this.refreshTokenTimer = new IOThreadTimer(s => ((IotHubTokenRefresher)s).OnLinkRefreshTokenAsync(), this, false);
            this.operationTimeout = operationTimeout;
            this.accessRightsStringArray = AccessRightsHelper.AccessRightsToStringArray(accessRights);
        }

        public void Cancel()
        {
            this.refreshTokenTimer.Cancel();
        }

        public async Task SendCbsTokenAsync(TimeSpan timeout)
        {
            if (!this.amqpSession.IsClosing())
            {
                var cbsLink = this.amqpSession.Connection.Extensions.Find<AmqpCbsLink>();
                if (cbsLink != null)
                {
                    var expiresAtUtc = await cbsLink.SendTokenAsync(
                        this.connectionString,
                        this.connectionString.AmqpEndpoint,
                        this.audience,
                        this.connectionString.AmqpEndpoint.AbsoluteUri,
                        this.accessRightsStringArray,
                        timeout);
                    this.ScheduleLinkTokenRefresh(expiresAtUtc);
                }
                else
                {
                    // TODO: This is an error
                }
            }
        }

        void ScheduleLinkTokenRefresh(DateTime expiresAtUtc)
        {
            if (expiresAtUtc == DateTime.MaxValue)
            {
                return;
            }

            TimeSpan timeFromNow = expiresAtUtc.Subtract(RefreshTokenBuffer).Subtract(DateTime.UtcNow);
            if (timeFromNow > TimeSpan.Zero)
            {
                this.refreshTokenTimer.Set(timeFromNow);
            }
        }

        async void OnLinkRefreshTokenAsync()
        {
            try
            {
                await this.SendCbsTokenAsync(this.operationTimeout);
            }
            catch (Exception exception)
            {
                if (Fx.IsFatal(exception))
                {
                    throw;
                }

                this.refreshTokenTimer.Set(RefreshTokenRetryInterval);
            }
        }
    }
#endif
}
