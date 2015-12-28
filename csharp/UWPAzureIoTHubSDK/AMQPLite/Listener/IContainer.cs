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
    using System.Security.Cryptography.X509Certificates;
    using Amqp.Framing;

    /// <summary>
    /// Represents an AMQP container.
    /// </summary>
    public interface IContainer
    {
        /// <summary>
        /// Gets the service certificate.
        /// </summary>
        X509Certificate2 ServiceCertificate { get; }

        /// <summary>
        /// Creates an AMQP message from the buffer. This is useful for brokers/listen applications
        /// to create lightweight message objects without paying the full serialization cost. 
        /// </summary>
        /// <param name="buffer">The serialized message.</param>
        /// <returns></returns>
        Message CreateMessage(ByteBuffer buffer);

        /// <summary>
        /// Creates an AMQP link for the attach request.
        /// </summary>
        /// <param name="connection">The connection where attach was received.</param>
        /// <param name="session">The session where attach was received.</param>
        /// <param name="attach">The received attach frame.</param>
        /// <returns></returns>
        Link CreateLink(ListenerConnection connection, ListenerSession session, Attach attach);

        /// <summary>
        /// Attaches the link. Brokers/listen applications should perform necessary validations (security,
        /// resource condition, etc). If return value is true, an attach is sent to peer by the library,
        /// otherwise the implementation is responsible to send attach to move the link to attached state.
        /// </summary>
        /// <param name="connection">The connection where attach was received.</param>
        /// <param name="session">The session where attach was received.</param>
        /// <param name="link">The link created in CreateLink call.</param>
        /// <param name="attach">The received attach frame.</param>
        /// <returns></returns>
        bool AttachLink(ListenerConnection connection, ListenerSession session, Link link, Attach attach);
    }
}
