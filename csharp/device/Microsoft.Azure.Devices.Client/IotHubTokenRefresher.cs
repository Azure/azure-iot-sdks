// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client
{
    using System;
    using System.Threading.Tasks;
    using Microsoft.Azure.Amqp;

    sealed class IotHubTokenRefresher
    {
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
            this.SendCbsTokenLoopAsync(expiresAtUtc, timeout).Fork();
        }

        async Task SendCbsTokenLoopAsync(DateTime expiryTimeUtc, TimeSpan timeout)
        {
            bool continueSendingTokens = await WaitUntilNextTokenSendTime(expiryTimeUtc);

            if (!continueSendingTokens)
            {
                return;
            }

            try
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

                            continueSendingTokens = await WaitUntilNextTokenSendTime(expiresAtUtc);
                            if (!continueSendingTokens)
                            {
                                break;
                            }
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
            catch (Exception e)
            {
                if (Fx.IsFatal(e))
                {
                    throw;
                }

                // ignore other exceptions
            }
        }

        static async Task<bool> WaitUntilNextTokenSendTime(DateTime expiresAtUtc)
        {
            var waitTime = ComputeTokenRefreshWaitTime(expiresAtUtc);

            if (waitTime == TimeSpan.MaxValue || waitTime == TimeSpan.Zero)
            {
                return false;
            }

            await Task.Delay(waitTime);
            return true;
        }

        static TimeSpan ComputeTokenRefreshWaitTime(DateTime expiresAtUtc)
        {
            return expiresAtUtc == DateTime.MaxValue ? TimeSpan.MaxValue : expiresAtUtc.Subtract(RefreshTokenBuffer).Subtract(DateTime.UtcNow);
        }
    }
}
