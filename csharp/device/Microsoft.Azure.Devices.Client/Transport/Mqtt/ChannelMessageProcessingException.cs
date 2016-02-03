// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client.Transport.Mqtt
{
    using System;
    using DotNetty.Transport.Channels;

    public class ChannelMessageProcessingException : Exception
    {
        public ChannelMessageProcessingException(Exception innerException, IChannelHandlerContext context)
            : base(string.Empty, innerException)
        {
            this.Context = context;
        }

        public IChannelHandlerContext Context { get; private set; }
    }
}