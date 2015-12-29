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

namespace Amqp.Listener
{
    using System.Threading;
    using Amqp.Framing;

    /// <summary>
    /// An AMQP connection used by the listener.
    /// </summary>
    public class ListenerConnection : Connection
    {
        readonly ConnectionListener listener;

        internal ListenerConnection(ConnectionListener listener, Address address, IAsyncTransport transport)
            : base(listener.AMQP, address, transport, null, null)
        {
            this.listener = listener;
        }

        internal ConnectionListener Listener
        {
            get { return this.listener; }
        }

        internal override void OnBegin(ushort remoteChannel, Begin begin)
        {
            // this sends a begin to the remote peer
            begin.RemoteChannel = remoteChannel;
            var session = new ListenerSession(this, begin);

            // this updates the local session state
            begin.RemoteChannel = session.Channel;
            base.OnBegin(remoteChannel, begin);
        }
    }
}
