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

    /// <summary>
    /// The transport interface used by a connection for network I/O.
    /// </summary>
    public interface ITransport
    {
        /// <summary>
        /// Sends a buffer.
        /// </summary>
        /// <param name="buffer">The buffer to send.</param>
        void Send(ByteBuffer buffer);

        /// <summary>
        /// Receives a buffer.
        /// </summary>
        /// <param name="buffer">The buffer to store the received bytes.</param>
        /// <param name="offset">The start position.</param>
        /// <param name="count">The number of bytes to receive.</param>
        /// <returns></returns>
        int Receive(byte[] buffer, int offset, int count);

        /// <summary>
        /// Closes the transport.
        /// </summary>
        void Close();
    }
}