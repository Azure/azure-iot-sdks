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
    using Amqp.Types;

    static class TxnCapabilities
    {
        public static readonly Symbol LocalTransactions = "amqp:local-transactions";
        public static readonly Symbol DistributedTxn = "amqp:distributed-transactions";
        public static readonly Symbol PrototableTransactions = "amqp:prototable-transactions";
        public static readonly Symbol MultiTxnsPerSsn = "amqp:multi-txns-per-ssn";
        public static readonly Symbol MultiSsnsPerTxn = "amqp:multi-ssns-per-txn";
    }
}