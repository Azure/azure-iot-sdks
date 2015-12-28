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
    using System.Net.Sockets;

    /// <summary>
    /// Contains the TCP settings of a connection.
    /// </summary>
    public class TcpSettings
    {
        const int DefaultBufferSize = 8192;
        bool? noDelay;
        int? receiveBufferSize;
        int? receiveTimeout;
        int? sendBufferSize;
        int? sendTimeout;

        /// <summary>
        /// Specifies the LingerOption option of the TCP socket.
        /// </summary>
        public LingerOption LingerOption
        {
            get;
            set;
        }

        /// <summary>
        /// Specifies the NoDelay option of the TCP socket.
        /// </summary>
        public bool NoDelay
        {
            get { return this.noDelay ?? false; }
            set { this.noDelay = value; }
        }

        /// <summary>
        /// Specifies the ReceiveBufferSize option of the TCP socket.
        /// </summary>
        public int ReceiveBufferSize
        {
            get { return this.receiveBufferSize ?? DefaultBufferSize; }
            set { this.receiveBufferSize = value; }
        }

        /// <summary>
        /// Specifies the ReceiveTimeout option of the TCP socket.
        /// </summary>
        public int ReceiveTimeout
        {
            get { return this.receiveTimeout ?? 0; }
            set { this.receiveTimeout = value; }
        }

        /// <summary>
        /// Specifies the SendBufferSize option of the TCP socket.
        /// </summary>
        public int SendBufferSize
        {
            get { return this.sendBufferSize ?? DefaultBufferSize; }
            set { this.sendBufferSize = value; }
        }

        /// <summary>
        /// Specifies the SendTimeout option of the TCP socket.
        /// </summary>
        public int SendTimeout
        {
            get { return this.sendTimeout ?? 0; }
            set { this.sendTimeout = value; }
        }

        internal void Configure(Socket socket)
        {
            if (this.noDelay != null) socket.NoDelay = this.noDelay.Value;
            if (this.receiveBufferSize != null) socket.ReceiveBufferSize = this.receiveBufferSize.Value;
            if (this.receiveTimeout != null) socket.ReceiveTimeout = this.receiveTimeout.Value;
            if (this.sendBufferSize != null) socket.SendBufferSize = this.sendBufferSize.Value;
            if (this.sendTimeout != null) socket.SendTimeout = this.sendTimeout.Value;
            if (this.LingerOption != null) socket.LingerState = this.LingerOption;
        }
    }
}
