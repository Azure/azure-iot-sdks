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
    using System.Threading;
    using Amqp.Framing;
    using Amqp.Types;

    /// <summary>
    /// A callback that is invoked when a message is received.
    /// </summary>
    /// <param name="receiver">The receiver link.</param>
    /// <param name="message">The received message.</param>
    public delegate void MessageCallback(ReceiverLink receiver, Message message);

    /// <summary>
    /// The ReceiverLink class represents a link that accepts incoming messages.
    /// </summary>
    public class ReceiverLink : Link
    {
#if DOTNET
        const int DefaultCredit = 200;
#else
        const int DefaultCredit = 20;
#endif
        // flow control
        SequenceNumber deliveryCount;
        int totalCredit;
        int credit;
        int restored;

        // received messages queue
        LinkedList receivedMessages;
        Delivery deliveryCurrent;

        // pending receivers
        LinkedList waiterList;
        MessageCallback onMessage;

        /// <summary>
        /// Initializes a receiver link.
        /// </summary>
        /// <param name="session">The session within which to create the link.</param>
        /// <param name="name">The link name.</param>
        /// <param name="adderss">The node address.</param>
        public ReceiverLink(Session session, string name, string adderss)
            : this(session, name, new Source() { Address = adderss }, null)
        {
        }

        /// <summary>
        /// Initializes a receiver link.
        /// </summary>
        /// <param name="session">The session within which to create the link.</param>
        /// <param name="name">The link name.</param>
        /// <param name="source">The source on attach that specifies the message source.</param>
        /// <param name="onAttached">The callback to invoke when an attach is received from peer.</param>
        public ReceiverLink(Session session, string name, Source source, OnAttached onAttached)
            : this(session, name, new Attach() { Source = source, Target = new Target() }, onAttached)
        {
        }

        /// <summary>
        /// Initializes a receiver link.
        /// </summary>
        /// <param name="session">The session within which to create the link.</param>
        /// <param name="name">The link name.</param>
        /// <param name="attach">The attach frame to send for this link.</param>
        /// <param name="onAttached">The callback to invoke when an attach is received from peer.</param>
        public ReceiverLink(Session session, string name, Attach attach, OnAttached onAttached)
            : base(session, name, onAttached)
        {
            this.totalCredit = -1;
            this.receivedMessages = new LinkedList();
            this.waiterList = new LinkedList();
            this.SendAttach(true, 0, attach);
        }

        /// <summary>
        /// Starts the message pump.
        /// </summary>
        /// <param name="credit">The link credit to issue.</param>
        /// <param name="onMessage">If specified, the callback to invoke when messages are received. If not specified, call Receive method to get the messages.</param>
        public void Start(int credit, MessageCallback onMessage = null)
        {
            this.onMessage = onMessage;
            this.SetCredit(credit, true);
        }

        /// <summary>
        /// Sets a credit on the link. A flow is sent to the peer to update link flow control state.
        /// </summary>
        /// <param name="credit">The new link credit.</param>
        /// <param name="autoRestore">If true, link credit is auto-restored when a message is accepted/rejected by the caller. If false, caller is responsible for manage link credits.</param>
        public void SetCredit(int credit, bool autoRestore = true)
        {
            uint dc;
            lock (this.ThisLock)
            {
                if (this.IsDetaching)
                {
                    return;
                }

                this.totalCredit = autoRestore ? credit : 0;
                this.credit = credit;
                this.restored = 0;
                dc = this.deliveryCount;
            }

            this.SendFlow(dc, (uint)credit);
        }

        /// <summary>
        /// Receives a message. The call is blocked for the timeout in seconds or a message is available.
        /// </summary>
        /// <param name="timeout">Number of seconds to wait.</param>
        /// <returns></returns>
        public Message Receive(int timeout = 60000)
        {
            return this.ReceiveInternal(null, timeout);
        }

        /// <summary>
        /// Accepts a message. It sends an accepted outcome to the peer.
        /// </summary>
        /// <param name="message">The message to accept.</param>
        public void Accept(Message message)
        {
            this.ThrowIfDetaching("Accept");
            this.DisposeMessage(message, new Accepted());
        }

        /// <summary>
        /// Releases a message. It sends a released outcome to the peer.
        /// </summary>
        /// <param name="message">The message to release.</param>
        public void Release(Message message)
        {
            this.ThrowIfDetaching("Release");
            this.DisposeMessage(message, new Released());
        }

        /// <summary>
        /// Rejects a message. It sends a rejected outcome to the peer.
        /// </summary>
        /// <param name="message">The message to reject.</param>
        /// <param name="error">The error, if any, for the rejection.</param>
        public void Reject(Message message, Error error = null)
        {
            this.ThrowIfDetaching("Reject");
            this.DisposeMessage(message, new Rejected() { Error = error });
        }

        internal override void OnFlow(Flow flow)
        {
        }

        internal override void OnTransfer(Delivery delivery, Transfer transfer, ByteBuffer buffer)
        {
            if (!transfer.More)
            {
                Waiter waiter;
                MessageCallback callback;
                lock (this.ThisLock)
                {
                    if (delivery == null)
                    {
                        // multi-transfer delivery
                        delivery = this.deliveryCurrent;
                        this.deliveryCurrent = null;
                        Fx.Assert(delivery != null, "Must have a delivery in the queue");
                        AmqpBitConverter.WriteBytes(delivery.Buffer, buffer.Buffer, buffer.Offset, buffer.Length);
                        delivery.Message = Message.Decode(delivery.Buffer);
                        delivery.Buffer = null;
                    }
                    else
                    {
                        // single tranfer delivery
                        this.OnDelivery(transfer.DeliveryId);
                        delivery.Message = Message.Decode(buffer);
                    }

                    callback = this.onMessage;
                    waiter = (Waiter)this.waiterList.First;
                    if (waiter != null)
                    {
                        this.waiterList.Remove(waiter);
                    }

                    if (waiter == null && callback == null)
                    {
                        this.receivedMessages.Add(new MessageNode() { Message = delivery.Message });
                        return;
                    }
                }

                while (waiter != null)
                {
                    if (waiter.Signal(delivery.Message))
                    {
                        this.OnDeliverMessage();
                        return;
                    }

                    lock (this.ThisLock)
                    {
                        waiter = (Waiter)this.waiterList.First;
                        if (waiter != null)
                        {
                            this.waiterList.Remove(waiter);
                        }
                        else if (callback == null)
                        {
                            this.receivedMessages.Add(new MessageNode() { Message = delivery.Message });
                            return;
                        }
                    }
                }

                Fx.Assert(waiter == null, "waiter must be null now");
                Fx.Assert(callback != null, "callback must not be null now");
                callback(this, delivery.Message);
                this.OnDeliverMessage();
            }
            else
            {
                lock (this.ThisLock)
                {
                    if (delivery == null)
                    {
                        delivery = this.deliveryCurrent;
                        Fx.Assert(delivery != null, "Must have a current delivery");
                        AmqpBitConverter.WriteBytes(delivery.Buffer, buffer.Buffer, buffer.Offset, buffer.Length);
                    }
                    else
                    {
                        this.OnDelivery(transfer.DeliveryId);
                        delivery.Buffer = new ByteBuffer(buffer.Length * 2, true);
                        AmqpBitConverter.WriteBytes(delivery.Buffer, buffer.Buffer, buffer.Offset, buffer.Length);
                        this.deliveryCurrent = delivery;
                    }
                }
            }
        }

        internal override void OnAttach(uint remoteHandle, Attach attach)
        {
            base.OnAttach(remoteHandle, attach);
            this.deliveryCount = attach.InitialDeliveryCount;
        }

        internal override void OnDeliveryStateChanged(Delivery delivery)
        {
        }

        /// <summary>
        /// Closes the receiver link.
        /// </summary>
        /// <param name="error">The error for the closure.</param>
        /// <returns></returns>
        protected override bool OnClose(Error error)
        {
            this.OnAbort(error);

            return base.OnClose(error);
        }

        /// <summary>
        /// Aborts the receiver link.
        /// </summary>
        /// <param name="error">The error for the abort.</param>
        protected override void OnAbort(Error error)
        {
            Waiter waiter;
            lock (this.ThisLock)
            {
                waiter = (Waiter)this.waiterList.Clear();
            }

            while (waiter != null)
            {
                waiter.Signal(null);
                waiter = (Waiter)waiter.Next;
            }
        }

        void OnDeliverMessage()
        {
            if (this.totalCredit > 0 &&
                Interlocked.Increment(ref this.restored) >= (this.totalCredit / 2))
            {
                this.SetCredit(this.totalCredit, true);
            }
        }

        internal Message ReceiveInternal(MessageCallback callback, int timeout = 60000)
        {
            this.ThrowIfDetaching("Receive");
            if (this.totalCredit < 0)
            {
                this.SetCredit(DefaultCredit, true);
            }

            Waiter waiter = null;
            lock (this.ThisLock)
            {
                MessageNode first = (MessageNode)this.receivedMessages.First;
                if (first != null)
                {
                    this.receivedMessages.Remove(first);
                    this.OnDeliverMessage();
                    return first.Message;
                }

                if (timeout == 0)
                {
                    return null;
                }

#if DOTNET || NETFX_CORE
                waiter = callback == null ? (Waiter)new SyncWaiter() : new AsyncWaiter(this, callback);
#else
                waiter = new SyncWaiter();
#endif
                this.waiterList.Add(waiter);
            }

            return waiter.Wait(timeout);
        }
        
        void DisposeMessage(Message message, Outcome outcome)
        {
            Delivery delivery = message.Delivery;
            if (delivery == null || delivery.Settled)
            {
                return;
            }

            DeliveryState state = outcome;
            bool settled = true;
#if DOTNET
            var txnState = Amqp.Transactions.ResourceManager.GetTransactionalStateAsync(this).Result;
            if (txnState != null)
            {
                txnState.Outcome = outcome;
                state = txnState;
                settled = false;
            }
#endif
            this.Session.DisposeDelivery(true, delivery, state, settled);
        }

        void OnDelivery(SequenceNumber deliveryId)
        {
            // called with lock held
            if (this.credit <= 0)
            {
                throw new AmqpException(ErrorCode.TransferLimitExceeded,
                    Fx.Format(SRAmqp.DeliveryLimitExceeded, deliveryId));
            }

            this.deliveryCount++;
            this.credit--;
        }

        sealed class MessageNode : INode
        {
            public Message Message { get; set; }

            public INode Previous { get; set; }

            public INode Next { get; set; }
        }

        abstract class Waiter : INode
        {
            public INode Previous { get; set; }

            public INode Next { get; set; }

            public abstract Message Wait(int timeout);

            public abstract bool Signal(Message message);
        }

        sealed class SyncWaiter : Waiter
        {
            readonly ManualResetEvent signal;
            Message message;
            bool expired;

            public SyncWaiter()
            {
                this.signal = new ManualResetEvent(false);
            }

            public override Message Wait(int timeout)
            {
                this.signal.WaitOne(timeout, false);
                lock (this)
                {
                    this.expired = this.message == null;
                    return this.message;
                }
            }

            public override bool Signal(Message message)
            {
                bool signaled = false;
                lock (this)
                {
                    if (!this.expired)
                    {
                        this.message = message;
                        signaled = true;
                    }
                }

                this.signal.Set();
                return signaled;
            }
        }

#if DOTNET || NETFX_CORE
        sealed class AsyncWaiter : Waiter
        {
            readonly static TimerCallback onTimer = OnTimer;
            readonly ReceiverLink link;
            readonly MessageCallback callback;
            Timer timer;
            int state;  // 0: created, 1: waiting, 2: signaled

            public AsyncWaiter(ReceiverLink link, MessageCallback callback)
            {
                this.link = link;
                this.callback = callback;
            }

            public override Message Wait(int timeout)
            {
                this.timer = new Timer(onTimer, this, timeout, -1);
                if (Interlocked.CompareExchange(ref this.state, 1, 0) != 0)
                {
                    // already signaled
                    this.timer.Dispose();
                }

                return null;
            }

            public override bool Signal(Message message)
            {
                int old = Interlocked.Exchange(ref this.state, 2);
                if (old != 2)
                {
                    if (old == 1)
                    {
                        this.timer.Dispose();
                    }

                    this.callback(this.link, message);
                    return true;
                }
                else
                {
                    return false;
                }
            }

            static void OnTimer(object state)
            {
                var thisPtr = (AsyncWaiter)state;
                thisPtr.Signal(null);
            }
        }
#endif
    }
}