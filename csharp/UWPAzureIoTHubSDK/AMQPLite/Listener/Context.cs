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
    using Amqp.Framing;

    /// <summary>
    /// Defines the property and methods of a message processor.
    /// </summary>
    public interface IMessageProcessor
    {
        /// <summary>
        /// Gets the link credit to issue to the client.
        /// </summary>
        int Credit { get; }

        /// <summary>
        /// Processes a received message.
        /// </summary>
        /// <param name="messageContext">Context of the received message.</param>
        void Process(MessageContext messageContext);
    }

    /// <summary>
    /// Defines the methods of a request processor.
    /// </summary>
    public interface IRequestProcessor
    {
        /// <summary>
        /// Processes a received request.
        /// </summary>
        /// <param name="requestContext">Context of the received request.</param>
        void Process(RequestContext requestContext);
    }

    /// <summary>
    /// The base class of request context.
    /// </summary>
    public abstract class Context
    {
        internal static Accepted Accepted = new Accepted();

        /// <summary>
        /// Initializes a context object.
        /// </summary>
        /// <param name="link">The link where the message was received.</param>
        /// <param name="message">The received message.</param>
        protected Context(ListenerLink link, Message message)
        {
            this.Link = link;
            this.Message = message;
        }

        /// <summary>
        /// Gets the link associated with the context.
        /// </summary>
        public ListenerLink Link
        {
            get;
            private set;
        }

        /// <summary>
        /// Gets the messages associated with the context.
        /// </summary>
        public Message Message
        {
            get;
            private set;
        }

        /// <summary>
        /// Disposes the request. If required, a disposition frame is sent to
        /// the peer to acknowledge the message.
        /// </summary>
        /// <param name="deliveryState">The delivery state to send.</param>
        protected void Dispose(DeliveryState deliveryState)
        {
            this.Link.DisposeMessage(this.Message, deliveryState, true);
        }
    }

    /// <summary>
    /// Provides the context to a message processor to process the received message.
    /// </summary>
    public class MessageContext : Context
    {
        internal MessageContext(ListenerLink link, Message message)
            : base(link, message)
        {
        }

        /// <summary>
        /// Accepts the message.
        /// </summary>
        public void Complete()
        {
            this.Dispose(Context.Accepted);
        }

        /// <summary>
        /// Rejects the message.
        /// </summary>
        /// <param name="error"></param>
        public void Complete(Error error)
        {
            this.Dispose(new Rejected() { Error = error });
        }
    }

    /// <summary>
    /// Provides the context to a request processor to process the received request.
    /// </summary>
    public class RequestContext : Context
    {
        readonly ListenerLink responseLink;

        internal RequestContext(ListenerLink requestLink, ListenerLink responseLink, Message request)
            : base(requestLink, request)
        {
            this.responseLink = responseLink;
        }

        /// <summary>
        /// Completes the request and sends the response message to the peer.
        /// </summary>
        /// <param name="response">The response message to send.</param>
        public void Complete(Message response)
        {
            if (response.Properties == null)
            {
                response.Properties = new Properties();
            }

            response.Properties.CorrelationId = this.Message.Properties.MessageId;
            this.responseLink.SendMessage(response, response.Encode());
        }
    }
}
