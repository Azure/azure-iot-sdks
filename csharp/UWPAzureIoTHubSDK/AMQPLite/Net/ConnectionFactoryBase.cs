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
    using System.Diagnostics;

    /// <summary>
    /// The base class of connection factories.
    /// </summary>
    public class ConnectionFactoryBase
    {
        internal TcpSettings tcpSettings;
        internal AmqpSettings amqpSettings;

        /// <summary>
        /// Constructor to create a connection factory.
        /// </summary>
        protected ConnectionFactoryBase()
        {
            this.tcpSettings = new TcpSettings()
            {
                NoDelay = true
            };

            this.amqpSettings = new AmqpSettings()
            {
                MaxFrameSize = (int)Connection.DefaultMaxFrameSize,
                ContainerId = Process.GetCurrentProcess().ProcessName,
                IdleTimeout = int.MaxValue,
                MaxSessionsPerConnection = 8
            };
        }

        /// <summary>
        /// Gets the TCP settings on the factory.
        /// </summary>
        public TcpSettings TCP
        {
            get
            {
                return this.tcpSettings ?? (this.tcpSettings = new TcpSettings());
            }
        }

        /// <summary>
        /// Gets the AMQP settings on the factory.
        /// </summary>
        public AmqpSettings AMQP
        {
            get { return this.amqpSettings; }
        }
    }
}
