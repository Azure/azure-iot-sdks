//  ------------------------------------------------------------------------------------
//  Copyright (c) Microsoft Corporation
//  All rights reserved. 
//  
//  Licensed under the Apache License, Version 2.0 (the ""License""); you may not use this 
//  file except in compliance with the License. You may obtain a copy of the License at 
//  http://www.apache.org/licenses/LICENSE-2.0  
//  
//  THIS CODE IS PROVIDED *AS IS* BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, 
//  EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION ANY IMPLIED WARRANTIES OR 
//  CONDITIONS OF TITLE, FITNESS FOR A PARTICULAR PURPOSE, MERCHANTABLITY OR 
//  NON-INFRINGEMENT. 
// 
//  See the Apache Version 2.0 License for specific language governing permissions and 
//  limitations under the License.
//  ------------------------------------------------------------------------------------

namespace Amqp
{
    using System;
    using System.Threading.Tasks;
    using Amqp.Framing;
    using Amqp.Types;

    class AsyncPump
    {
        readonly IAsyncTransport transport;

        public AsyncPump(IAsyncTransport transport)
        {
            this.transport = transport;
        }

        public void Start(Connection connection)
        {
            Task task = this.StartAsync(connection);
        }

        public async Task PumpAsync(Func<ProtocolHeader, bool> onHeader, Func<ByteBuffer, bool> onBuffer)
        {
            byte[] header = new byte[FixedWidth.ULong];

            if (onHeader != null)
            {
                // header
                await this.ReceiveBufferAsync(header, 0, FixedWidth.ULong);
                if (!onHeader(ProtocolHeader.Create(header, 0)))
                {
                    return;
                }
            }

            // frames
            while (true)
            {
                await this.ReceiveBufferAsync(header, 0, FixedWidth.UInt);
                int frameSize = AmqpBitConverter.ReadInt(header, 0);

                byte[] buffer = new byte[frameSize];
                Buffer.BlockCopy(header, 0, buffer, 0, FixedWidth.UInt);

                await this.ReceiveBufferAsync(buffer, FixedWidth.UInt, frameSize - FixedWidth.UInt);
                if (!onBuffer(new ByteBuffer(buffer, 0, frameSize, frameSize)))
                {
                    break;
                }
            }
        }

        async Task StartAsync(Connection connection)
        {
            try
            {
                await this.PumpAsync(connection.OnHeader, connection.OnFrame);
            }
            catch (Exception exception)
            {
                connection.OnIoException(exception);
            }

        }

        async Task ReceiveBufferAsync(byte[] buffer, int offset, int count)
        {
            while (count > 0)
            {
                int received = await this.transport.ReceiveAsync(buffer, offset, count);

                offset += received;
                count -= received;
            }
        }
    }
}
