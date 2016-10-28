// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client.Transport
{
    using System;
    using System.Collections.ObjectModel;
    using System.Diagnostics;
    using System.Linq;
    using System.Net.Sockets;
    using System.Threading;
    using System.Threading.Tasks;
    using Microsoft.Azure.Devices.Client.Exceptions;
    using Microsoft.Azure.Devices.Client.Extensions;

    /// <summary>
    /// Transport handler router. 
    /// Tries to open open transport in the order it was set. 
    /// If fails tries to open the next one, etc.
    /// </summary>
    class RoutingDelegatingHandler : DefaultDelegatingHandler
    {
        internal delegate IDelegatingHandler TransportHandlerFactory(IotHubConnectionString iotHubConnectionString, ITransportSettings transportSettings);

        readonly TransportHandlerFactory transportHandlerFactory;
        readonly IotHubConnectionString iotHubConnectionString;
        readonly ITransportSettings[] transportSettings;

        public RoutingDelegatingHandler(TransportHandlerFactory transportHandlerFactory, IotHubConnectionString iotHubConnectionString, ITransportSettings[] transportSettings)
        {
            this.transportHandlerFactory = transportHandlerFactory;
            this.iotHubConnectionString = iotHubConnectionString;
            this.transportSettings = transportSettings;
        }

        public override async Task OpenAsync(bool explicitOpen, CancellationToken cancellationToken)
        {
            await this.TryOpenPrioritizedTransportsAsync(explicitOpen, cancellationToken);
        }

        async Task TryOpenPrioritizedTransportsAsync(bool explicitOpen, CancellationToken cancellationToken)
        {
            Exception lastException = null;
            // Concrete Device Client creation was deferred. Use prioritized list of transports.
            foreach (ITransportSettings transportSetting in this.transportSettings)
            {
                if (cancellationToken.IsCancellationRequested)
                {
                    return;
                }

                try
                {
                    this.InnerHandler = this.transportHandlerFactory(this.iotHubConnectionString, transportSetting);

                    // Try to open a connection with this transport
                    await base.OpenAsync(explicitOpen, cancellationToken);
                }
                catch (Exception exception)
                {
                    try
                    {
                        if (this.InnerHandler != null)
                        {
                            await this.CloseAsync();
                        }
                    }
                    catch (Exception ex) when (!ex.IsFatal())
                    {
                        //ignore close failures    
                    }

                    if (!(exception is IotHubCommunicationException || exception is TimeoutException || exception is SocketException || exception is AggregateException))
                    {
                        throw;
                    }

                    var aggregateException = exception as AggregateException;
                    if (aggregateException != null)
                    {
                        ReadOnlyCollection<Exception> innerExceptions = aggregateException.Flatten().InnerExceptions;
                        if (!innerExceptions.Any(x => x is IotHubCommunicationException || x is SocketException || x is TimeoutException))
                        {
                            throw;
                        }
                    }

                    lastException = exception;

                    // open connection failed. Move to next transport type
                    continue;
                }

                return;
            }

            if (lastException != null)
            {
                throw new IotHubCommunicationException("Unable to open transport", lastException);
            }
        }

        //Everything below just for test purposese and won't go to public
        internal static readonly int[] Latencies = new int[1000 * 60 * 10];
        internal static SpinLock SpinLock = new SpinLock();
        
        public RoutingDelegatingHandler()
        {
            
        }
    }
}