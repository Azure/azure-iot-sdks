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
    /// The listener link provides non-blocking methods that can be used by brokers/listener
    /// applications.
    /// </summary>
    public class ListenerLink : Link
    {
        bool role;
        uint credit;
        object state;

        // send
        Action<int, object> onCredit;
        Action<Message, DeliveryState, bool, object> onDispose;

        // receive
        SequenceNumber deliveryCount;
        int delivered;
        Delivery deliveryCurrent;
        Action<ListenerLink, Message, DeliveryState, object> onMessage;

        /// <summary>
        /// Initializes a listener link object.
        /// </summary>
        /// <param name="session">The session.</param>
        /// <param name="attach">The received attach frame.</param>
        public ListenerLink(ListenerSession session, Attach attach)
            : base(session, attach.LinkName, null)
        {
            this.role = !attach.Role;
            this.SettleOnSend = attach.SndSettleMode == SenderSettleMode.Settled;
        }

        /// <summary>
        /// Gets the sender (false) or receiver (true) role of the link.
        /// </summary>
        public bool Role
        {
            get { return this.role; }
        }

        /// <summary>
        /// Gets the settled flag. If it is true, messages are sent settled.
        /// </summary>
        public bool SettleOnSend
        {
            get; internal set;
        }

        /// <summary>
        /// Gets the user state attached to the link when it is initialized.
        /// </summary>
        public object State
        {
            get { return this.state; }
        }

        /// <summary>
        /// Initializes the receiver state for the link.
        /// </summary>
        /// <param name="credit">The link credit.</param>
        /// <param name="onMessage">The callback to invoke for received messages.</param>
        /// <param name="state">The user state attached to the link.</param>
        public void InitializeReceiver(uint credit, Action<ListenerLink, Message, DeliveryState, object> onMessage, object state)
        {
            this.credit = credit;
            this.onMessage = onMessage;
            this.state = state;
        }

        /// <summary>
        /// Initializes the sender state for the link.
        /// </summary>
        /// <param name="onCredit">The callback to invoke when flow is received with more link credit.</param>
        /// <param name="onDispose">The callback to invoke when disposition is received.</param>
        /// <param name="state">The user state attached to the link.</param>
        public void InitializeSender(Action<int, object> onCredit, Action<Message, DeliveryState, bool, object> onDispose, object state)
        {
            this.onCredit = onCredit;
            this.onDispose = onDispose;
            this.state = state;
        }

        /// <summary>
        /// Sends a message. This call is non-blocking and it does not wait for acknowledgements.
        /// </summary>
        /// <param name="message"></param>
        /// <param name="buffer"></param>
        public void SendMessage(Message message, ByteBuffer buffer)
        {
            Delivery delivery = new Delivery()
            {
                Handle = this.Handle,
                Message = message,
                Buffer = buffer,
                Link = this,
                Settled = this.SettleOnSend
            };

            this.Session.SendDelivery(delivery);
            this.deliveryCount++;
        }

        /// <summary>
        /// Sends a disposition for the message.
        /// </summary>
        /// <param name="message">The message.</param>
        /// <param name="deliveryState">The delivery state to set on disposition.</param>
        /// <param name="settled">The settled flag on disposition.</param>
        public void DisposeMessage(Message message, DeliveryState deliveryState, bool settled)
        {
            Delivery delivery = message.Delivery;
            if (delivery == null || delivery.Settled)
            {
                return;
            }

            this.Session.DisposeDelivery(this.role, delivery, deliveryState, settled);
        }

        /// <summary>
        /// Completes the link attach request.
        /// </summary>
        /// <param name="attach">The attach to send back.</param>
        /// <param name="error">The error, if any, for the link.</param>
        public void CompleteAttach(Attach attach, Error error)
        {
            if (error != null)
            {
                this.SendAttach(!attach.Role, attach.InitialDeliveryCount, new Attach() { Target = null, Source = null });
            }
            else
            {
                this.SendAttach(!attach.Role, attach.InitialDeliveryCount, new Attach() { Target = attach.Target, Source = attach.Source });
            }

            base.OnAttach(attach.Handle, attach);

            if (error != null)
            {
                this.Close(0, error);
            }
            else
            {
                if (this.credit > 0)
                {
                    this.SendFlow(this.deliveryCount, credit);
                }
            }
        }

        internal override void OnAttach(uint remoteHandle, Attach attach)
        {
            var container = ((ListenerConnection)this.Session.Connection).Listener.Container;

            Error error = null;

            try
            {
                bool done = container.AttachLink((ListenerConnection)this.Session.Connection, (ListenerSession)this.Session, this, attach);
                if (!done)
                {
                    return;
                }
            }
            catch (AmqpException amqpException)
            {
                error = amqpException.Error;
            }
            catch (Exception exception)
            {
                error = new Error() { Condition = ErrorCode.InternalError, Description = exception.Message };
            }

            this.CompleteAttach(attach, error);
        }

        internal override void OnFlow(Flow flow)
        {
            if (this.onCredit != null)
            {
                var theirLimit = (SequenceNumber)(flow.DeliveryCount + flow.LinkCredit);
                var myLimit = (SequenceNumber)((uint)this.deliveryCount + this.credit);
                int delta = theirLimit - myLimit;
                if (delta > 0)
                {
                    this.onCredit(delta, this.state);
                }
            }
        }

        internal override void OnDeliveryStateChanged(Delivery delivery)
        {
            if (this.onDispose != null)
            {
                this.onDispose(delivery.Message, delivery.State, delivery.Settled, this.state);
            }
        }

        internal override void OnTransfer(Delivery delivery, Transfer transfer, ByteBuffer buffer)
        {
            if (delivery != null)
            {
                this.deliveryCount++;
                if (!transfer.More)
                {
                    // single transfer message - the most common case
                    delivery.Buffer = buffer;
                    this.DeliverMessage(delivery);
                }
                else
                {
                    delivery.Buffer = new ByteBuffer(buffer.Length * 2, true);
                    AmqpBitConverter.WriteBytes(delivery.Buffer, buffer.Buffer, buffer.Offset, buffer.Length);
                    this.deliveryCurrent = delivery;
                }
            }
            else
            {
                delivery = this.deliveryCurrent;
                if (!transfer.More)
                {
                    AmqpBitConverter.WriteBytes(delivery.Buffer, buffer.Buffer, buffer.Offset, buffer.Length);
                    this.deliveryCurrent = null;
                    this.DeliverMessage(delivery);
                }
                else
                {
                    AmqpBitConverter.WriteBytes(delivery.Buffer, buffer.Buffer, buffer.Offset, buffer.Length);
                }
            }
        }

        /// <summary>
        /// Aborts the link.
        /// </summary>
        /// <param name="error">The error.</param>
        protected override void OnAbort(Error error)
        {
        }

        void DeliverMessage(Delivery delivery)
        {
            var container = ((ListenerConnection)this.Session.Connection).Listener.Container;
            delivery.Message = container.CreateMessage(delivery.Buffer);
            this.onMessage(this, delivery.Message, delivery.State, this.state);
            if (this.delivered++ >= this.credit / 2)
            {
                this.delivered = 0;
                this.SendFlow(this.deliveryCount, this.credit);
            }
        }
    }
}
