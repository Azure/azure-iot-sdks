// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client.Transport
{
    using System;
    using System.Threading;
    using System.Threading.Tasks;

    abstract class TransportHandler : DefaultDelegatingHandler
    {
        readonly ITransportSettings transportSettings;

        protected TransportHandler(ITransportSettings transportSettings)
        {
            this.transportSettings = transportSettings;
        }

        public override Task<Message> ReceiveAsync(CancellationToken cancellationToken)
        {
            return this.ReceiveAsync(this.transportSettings.DefaultReceiveTimeout, cancellationToken);
        }

        protected Task HandleTimeoutCancellation(Func<Task> func, CancellationToken token)
        {
            CancellationTokenRegistration ctr = token.Register(() => this.CloseAsync());

            var tcs = new TaskCompletionSource<bool>();

            func().ContinueWith(t =>
            {
                if (t.IsFaulted)
                {
                    if (token.IsCancellationRequested)
                    {
                        tcs.SetResult(false);
                    }
                    else
                    {
                        tcs.TrySetException(t.Exception);
                    }
                }
                else if (t.IsCanceled)
                {
                    if (token.IsCancellationRequested)
                    {
                        tcs.SetResult(false);
                    }
                    else
                    {
                        tcs.TrySetCanceled();
                    }
                }
                else
                {
                    tcs.SetResult(false);
                }
                ctr.Dispose();
            }, CancellationToken.None);

            return tcs.Task;
        }
    }
}