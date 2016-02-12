// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client.Transport.Mqtt
{
    using System.Threading.Tasks;
    using DotNetty.Transport.Channels;

    class SingleInstanceEventLoopGroup : IEventLoopGroup
    {
        readonly SingleThreadEventLoop eventLoop;

        public SingleInstanceEventLoopGroup()
        {
            this.eventLoop = new SingleThreadEventLoop();
        }

        public IEventLoop GetNext()
        {
            return this.eventLoop;
        }

        public Task ShutdownGracefullyAsync()
        {
            return this.eventLoop.ShutdownGracefullyAsync();
        }

        public Task TerminationCompletion
        {
            get
            {
                return this.eventLoop.TerminationCompletion;
            }
        }
    }
}