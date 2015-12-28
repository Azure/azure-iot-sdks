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
    using System.Threading;
    using Amqp.Framing;
    using Amqp.Types;

    /// <summary>
    /// A callback that is invoked when an outcome is received from peer for an outgoing message.
    /// </summary>
    /// <param name="message">The outgoing message.</param>
    /// <param name="outcome">The received message.</param>
    /// <param name="state">The user object specified in the Send method.</param>
    public delegate void OutcomeCallback(Message message, Outcome outcome, object state);

    /// <summary>
    /// The SenderLink represents a link that sends outgoing messages.
    /// </summary>
    public class SenderLink : Link
    {
        // flow control
        SequenceNumber deliveryCount;
        int credit;

        // outgoing queue
        SenderSettleMode settleMode;
        LinkedList outgoingList;
        bool writing;

        /// <summary>
        /// Initializes a sender link.
        /// </summary>
        /// <param name="session">The session within which to create the link.</param>
        /// <param name="name">The link name.</param>
        /// <param name="adderss">The node address.</param>
        public SenderLink(Session session, string name, string adderss)
            : this(session, name, new Target() { Address = adderss }, null)
        {
        }

        /// <summary>
        /// Initializes a sender link.
        /// </summary>
        /// <param name="session">The session within which to create the link.</param>
        /// <param name="name">The link name.</param>
        /// <param name="target">The target on attach that specifies the message target.</param>
        /// <param name="onAttached">The callback to invoke when an attach is received from peer.</param>
        public SenderLink(Session session, string name, Target target, OnAttached onAttached)
            : this(session, name, new Attach() { Source = new Source(), Target = target }, onAttached)
        {
        }

        /// <summary>
        /// Initializes a sender link.
        /// </summary>
        /// <param name="session">The session within which to create the link.</param>
        /// <param name="name">The link name.</param>
        /// <param name="attach">The attach frame to send for this link.</param>
        /// <param name="onAttached">The callback to invoke when an attach is received from peer.</param>
        public SenderLink(Session session, string name, Attach attach, OnAttached onAttached)
            : base(session, name, onAttached)
        {
            this.settleMode = attach.SndSettleMode;
            this.outgoingList = new LinkedList();
            this.SendAttach(false, this.deliveryCount, attach);
        }
        
        /// <summary>
        /// Sends a message and synchronously waits for an acknowledgement.
        /// </summary>
        /// <param name="message">The message to send.</param>
        /// <param name="millisecondsTimeout">The time in milliseconds to wait for the acknowledgement.</param>
        public void Send(Message message, int millisecondsTimeout = 60000)
        {
            ManualResetEvent acked = new ManualResetEvent(false);
            Outcome outcome = null;
            OutcomeCallback callback = (m, o, s) =>
            {
                outcome = o;
                acked.Set();
            };

            this.Send(message, callback, acked);

#if !COMPACT_FRAMEWORK
            bool signaled = acked.WaitOne(millisecondsTimeout, true);
#else
            bool signaled = acked.WaitOne(millisecondsTimeout, false);
#endif
            if (!signaled)
            {
                throw new TimeoutException();
            }

            if (outcome != null)
            {
                if (outcome.Descriptor.Code == Codec.Released.Code)
                {
                    Released released = (Released)outcome;
                    throw new AmqpException(ErrorCode.MessageReleased, null);
                }
                else if (outcome.Descriptor.Code == Codec.Rejected.Code)
                {
                    Rejected rejected = (Rejected)outcome;
                    throw new AmqpException(rejected.Error);
                }
            }
        }

        /// <summary>
        /// Sends a message asynchronously. If callback is null, the message is sent without requesting for an acknowledgement (best effort).
        /// </summary>
        /// <param name="message">The message to send.</param>
        /// <param name="callback">The callback to invoke when acknowledgement is received.</param>
        /// <param name="state">The object that is passed back to the outcome callback.</param>
        public void Send(Message message, OutcomeCallback callback, object state)
        {
            DeliveryState deliveryState = null;
#if DOTNET
            deliveryState = Amqp.Transactions.ResourceManager.GetTransactionalStateAsync(this).Result;
#endif
            this.Send(message, deliveryState, callback, state);
        }

        internal void Send(Message message, DeliveryState deliveryState, OutcomeCallback callback, object state)
        {
            this.ThrowIfDetaching("Send");
            Delivery delivery = new Delivery()
            {
                Message = message,
                Buffer = message.Encode(),
                State = deliveryState,
                Link = this,
                OnOutcome = callback,
                UserToken = state,
                Settled = this.settleMode == SenderSettleMode.Settled || callback == null
            };

            lock (this.ThisLock)
            {
                if (this.credit <= 0 || this.writing)
                {
                    this.outgoingList.Add(delivery);
                    return;
                }

                delivery.Tag = GetDeliveryTag(this.deliveryCount);
                this.credit--;
                this.deliveryCount++;
                this.writing = true;
            }

            this.WriteDelivery(delivery);
        }

        internal override void OnFlow(Flow flow)
        {
            Delivery delivery = null;
            lock (this.ThisLock)
            {
                this.credit = (flow.DeliveryCount + flow.LinkCredit) - this.deliveryCount;
                if (this.writing || this.credit <= 0 || this.outgoingList.First == null)
                {
                    return;
                }

                delivery = (Delivery)this.outgoingList.First;
                this.outgoingList.Remove(delivery);
                delivery.Tag = GetDeliveryTag(this.deliveryCount);
                this.credit--;
                this.deliveryCount++;
                this.writing = true;
            }

            this.WriteDelivery(delivery);
        }

        internal override void OnTransfer(Delivery delivery, Transfer transfer, ByteBuffer buffer)
        {
            throw new InvalidOperationException();
        }

        internal override void OnDeliveryStateChanged(Delivery delivery)
        {
            // some broker may not settle the message when sending dispositions
            if (!delivery.Settled)
            {
                this.Session.DisposeDelivery(false, delivery, new Accepted(), true);
            }

            if (delivery.OnOutcome != null)
            {
                Outcome outcome = delivery.State as Outcome;
#if DOTNET
                if (delivery.State != null && delivery.State is Amqp.Transactions.TransactionalState)
                {
                    outcome = ((Amqp.Transactions.TransactionalState)delivery.State).Outcome;
                }
#endif
                delivery.OnOutcome(delivery.Message, outcome, delivery.UserToken);
            }
        }

        /// <summary>
        /// Closes the sender link.
        /// </summary>
        /// <param name="error">The error for the closure.</param>
        /// <returns></returns>
        protected override bool OnClose(Error error)
        {
            this.OnAbort(error);

            return base.OnClose(error);
        }

        /// <summary>
        /// Aborts the sender link.
        /// </summary>
        /// <param name="error">The error for the abort.</param>
        protected override void OnAbort(Error error)
        {
            Delivery toRelease = null;
            while (true)
            {
                lock (this.ThisLock)
                {
                    if (this.writing)
                    {
                        // wait until write finishes (either all deliveries are handed over to session or no credit is available)
                    }
                    else
                    {
                        toRelease = (Delivery)this.outgoingList.Clear();
                        break;
                    }
                }
            }

            Delivery.ReleaseAll(toRelease, error);
        }

        static byte[] GetDeliveryTag(uint tag)
        {
            byte[] buffer = new byte[FixedWidth.UInt];
            AmqpBitConverter.WriteInt(buffer, 0, (int)tag);
            return buffer;
        }

        void WriteDelivery(Delivery delivery)
        {
            while (delivery != null)
            {
                delivery.Handle = this.Handle;
                bool settled = delivery.Settled;
                this.Session.SendDelivery(delivery);
                if (settled && delivery.OnOutcome != null)
                {
                    delivery.OnOutcome(delivery.Message, new Accepted(), delivery.UserToken);
                }

                lock (this.ThisLock)
                {
                    delivery = (Delivery)this.outgoingList.First;
                    if (delivery == null)
                    {
                        this.writing = false;
                    }
                    else if (this.credit > 0)
                    {
                        this.outgoingList.Remove(delivery);
                        delivery.Tag = GetDeliveryTag(this.deliveryCount);
                        this.credit--;
                        this.deliveryCount++;
                    }
                    else
                    {
                        delivery = null;
                        this.writing = false;
                    }
                }
            }
        }
    }
}