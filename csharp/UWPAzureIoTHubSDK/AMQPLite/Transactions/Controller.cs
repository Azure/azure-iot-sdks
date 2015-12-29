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
    using System.Threading.Tasks;
    using Amqp;
    using Amqp.Framing;

    sealed class Controller : SenderLink
    {
        public Controller(Session session)
            : base(session, GetName(), new Attach() { Target = new Coordinator(), Source = new Source() }, null)
        {
        }

        public Task<byte[]> DeclareAsync()
        {
            Message message = new Message(new Declare());
            TaskCompletionSource<byte[]> tcs = new TaskCompletionSource<byte[]>();
            this.Send(message, null, OnOutcome, tcs);
            return tcs.Task;
        }

        public Task DischargeAsync(byte[] txnId, bool fail)
        {
            Message message = new Message(new Discharge() { TxnId = txnId, Fail = fail });
            TaskCompletionSource<byte[]> tcs = new TaskCompletionSource<byte[]>();
            this.Send(message, null, OnOutcome, tcs);
            return tcs.Task;
        }

        static string GetName()
        {
            return "controller-link-" + Guid.NewGuid().ToString("N").Substring(0, 5);
        }

        static void OnOutcome(Message message, Outcome outcome, object state)
        {
            var tcs = (TaskCompletionSource<byte[]>)state;
            if (outcome.Descriptor.Code == Codec.Declared.Code)
            {
                tcs.SetResult(((Declared)outcome).TxnId);
            }
            else if (outcome.Descriptor.Code == Codec.Rejected.Code)
            {
                tcs.SetException(new AmqpException(((Rejected)outcome).Error));
            }
            else
            {
                tcs.SetCanceled();
            }
        }
   }
}
