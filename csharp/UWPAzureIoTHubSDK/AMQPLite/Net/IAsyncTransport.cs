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
    using System.Collections.Generic;
    using System.Threading.Tasks;
   
    /// <summary>
    /// Provides asynchronous I/O calls.
    /// </summary>
    public interface IAsyncTransport : ITransport
    {
        /// <summary>
        /// Sets a connection to the transport.
        /// </summary>
        /// <param name="connection"></param>
        void SetConnection(Connection connection);

        /// <summary>
        /// Sends one or a list of buffers. Only one of buffer and bufferList can be non-null.
        /// </summary>
        /// <param name="buffer">The buffer to send.</param>
        /// <param name="bufferList">The list of buffers to send.</param>
        /// <param name="listSize">Number of bytes of all buffers in the list.</param>
        /// <returns>true: pending, false: completed</returns>
        bool SendAsync(ByteBuffer buffer, IList<ArraySegment<byte>> bufferList, int listSize);

        /// <summary>
        /// Reads bytes into a buffer.
        /// </summary>
        /// <param name="buffer">The buffer to store data.</param>
        /// <param name="offset">The offset.</param>
        /// <param name="count">The number of bytes to read.</param>
        /// <returns></returns>
        Task<int> ReceiveAsync(byte[] buffer, int offset, int count);
    }
}
