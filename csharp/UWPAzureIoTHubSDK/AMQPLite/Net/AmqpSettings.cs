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
    /// <summary>
    /// Contains the AMQP settings for a connection.
    /// </summary>
    public class AmqpSettings
    {
        /// <summary>
        /// Gets or sets the open.max-frame-size field.
        /// </summary>
        public int MaxFrameSize
        {
            get;
            set;
        }

        /// <summary>
        /// Gets or sets the open.container-id field.
        /// </summary>
        public string ContainerId
        {
            get;
            set;
        }

        /// <summary>
        /// Gets or sets the open.hostname field.
        /// </summary>
        public string HostName
        {
            get;
            set;
        }

        /// <summary>
        /// Gets or sets the open.channel-max field.
        /// </summary>
        public ushort MaxSessionsPerConnection
        {
            get;
            set;
        }

        /// <summary>
        /// Gets or sets the open.idle-time-out field.
        /// </summary>
        public int IdleTimeout
        {
            get;
            set;
        }
    }
}
