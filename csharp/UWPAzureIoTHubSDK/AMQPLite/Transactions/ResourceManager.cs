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

namespace Amqp.Transactions
{
    using System;
    using System.Collections.Generic;
    using System.Threading.Tasks;
    using System.Transactions;
    using Amqp;
    using Amqp.Framing;

    sealed class ResourceManager
    {
        static readonly ResourceManager instance = new ResourceManager();
        readonly Dictionary<Connection, Controller> controllers;
        readonly Dictionary<string, Enlistment> enlistments;

        ResourceManager()
        {
            this.controllers = new Dictionary<Connection, Controller>();
            this.enlistments = new Dictionary<string, Enlistment>(StringComparer.OrdinalIgnoreCase);
        }

        object SyncRoot { get { return this.enlistments; } }

        public static async Task<TransactionalState> GetTransactionalStateAsync(Link link)
        {
            Transaction txn = Transaction.Current;
            if (txn != null)
            {
                byte[] txnId = await instance.EnlistAsync(link, txn);
                return new TransactionalState() { TxnId = txnId };
            }

            return null;
        }

        async Task<byte[]> EnlistAsync(Link link, Transaction txn)
        {
            string id = txn.TransactionInformation.LocalIdentifier;
            Enlistment enlistment;
            lock (this.SyncRoot)
            {
                if (!this.enlistments.TryGetValue(id, out enlistment))
                {
                    enlistment = new Enlistment(this, txn);
                    this.enlistments.Add(id, enlistment);
                    txn.TransactionCompleted += this.OnTransactionCompleted;

                    if (!txn.EnlistPromotableSinglePhase(enlistment))
                    {
                        this.enlistments.Remove(id);
                        txn.TransactionCompleted -= this.OnTransactionCompleted;
                        throw new InvalidOperationException("DTC not supported");
                    }
                }
            }

            return await enlistment.EnlistAsync(link);
        }

        void OnTransactionCompleted(object sender, TransactionEventArgs e)
        {
            lock (this.SyncRoot)
            {
                this.enlistments.Remove(e.Transaction.TransactionInformation.LocalIdentifier);
            }
        }

        Controller GetOrCreateController(Link link)
        {
            Controller controller;
            lock (this.SyncRoot)
            {
                if (!this.controllers.TryGetValue(link.Session.Connection, out controller))
                {
                    Session session = new Session(link.Session.Connection);
                    controller = new Controller(session);
                    controller.Closed += this.OnControllerClosed;
                    this.controllers.Add(link.Session.Connection, controller);
                }
            }

            return controller;
        }

        void OnControllerClosed(AmqpObject obj, Error error)
        {
            var controller = (Controller)obj;
            bool removed;
            lock (this.SyncRoot)
            {
                removed = this.controllers.Remove(controller.Session.Connection);
            }

            if (removed)
            {
                controller.Session.Close(0);
            }
        }

        class Enlistment : IPromotableSinglePhaseNotification
        {
            static readonly TimeSpan rollbackTimeout = TimeSpan.FromMinutes(1);
            readonly ResourceManager owner;
            readonly Transaction transaction;
            readonly string transactionId;
            readonly object syncRoot;
            Controller controller;
            Task<byte[]> declareTask;
            byte[] txnid;

            public Enlistment(ResourceManager owner, Transaction transaction)
            {
                this.owner = owner;
                this.transaction = transaction;
                this.transactionId = this.transaction.TransactionInformation.LocalIdentifier;
                this.syncRoot = new object();
            }

            public async Task<byte[]> EnlistAsync(Link link)
            {
                if (this.txnid != null)
                {
                    return this.txnid;
                }

                lock (this.syncRoot)
                {
                    if (this.declareTask == null)
                    {
                        this.controller = this.owner.GetOrCreateController(link);
                        this.declareTask = this.controller.DeclareAsync();
                    }
                }

                return this.txnid = await this.declareTask;
            }

            void IPromotableSinglePhaseNotification.Initialize()
            {
            }

            void IPromotableSinglePhaseNotification.Rollback(SinglePhaseEnlistment singlePhaseEnlistment)
            {
                lock (this.syncRoot)
                {
                    if (this.txnid != null)
                    {
                        this.controller.DischargeAsync(this.txnid, true).ContinueWith(
                            (t, o) =>
                            {
                                var spe = (SinglePhaseEnlistment)o;
                                if (t.IsFaulted)
                                {
                                    spe.Aborted(t.Exception.InnerException);
                                }
                                else
                                {
                                    spe.Done();
                                }
                            },
                            singlePhaseEnlistment);
                    }
                }
            }

            void IPromotableSinglePhaseNotification.SinglePhaseCommit(SinglePhaseEnlistment singlePhaseEnlistment)
            {
                lock (this.syncRoot)
                {
                    if (this.txnid != null)
                    {
                        this.controller.DischargeAsync(this.txnid, false).ContinueWith(
                            (t, o) =>
                            {
                                var spe = (SinglePhaseEnlistment)o;
                                if (t.IsFaulted)
                                {
                                    spe.Aborted(t.Exception.InnerException);
                                }
                                else
                                {
                                    spe.Done();
                                }
                            },
                            singlePhaseEnlistment);
                    }
                }
            }

            byte[] ITransactionPromoter.Promote()
            {
                throw new TransactionPromotionException("DTC not supported");
            }
        }
    }
}
