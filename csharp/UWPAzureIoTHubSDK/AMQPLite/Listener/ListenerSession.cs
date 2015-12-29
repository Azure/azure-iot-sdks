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
    using System;
    using Amqp.Framing;
    using Amqp.Types;

    /// <summary>
    /// An AMQP session used by the listener.
    /// </summary>
    public class ListenerSession : Session
    {
        internal ListenerSession(ListenerConnection connection, Begin begin)
            : base(connection, begin)
        {
        }

        internal override void OnAttach(Attach attach)
        {
            var connection = (ListenerConnection)this.Connection;
            Link link = connection.Listener.Container.CreateLink(connection, this, attach);
            base.OnAttach(attach);
        }
    }
}
