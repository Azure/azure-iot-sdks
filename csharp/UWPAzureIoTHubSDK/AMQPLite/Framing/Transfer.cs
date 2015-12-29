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

namespace Amqp.Framing
{
    using Amqp.Types;

    sealed class Transfer : DescribedList
    {
        public Transfer()
            : base(Codec.Transfer, 11)
        {
        }

        public bool HasDeliveryId
        {
            get { return this.Fields[1] != null; }
        }

        public uint Handle
        {
            get { return this.Fields[0] == null ? uint.MaxValue : (uint)this.Fields[0]; }
            set { this.Fields[0] = value; }
        }

        public uint DeliveryId
        {
            get { return this.Fields[1] == null ? uint.MinValue : (uint)this.Fields[1]; }
            set { this.Fields[1] = value; }
        }

        public byte[] DeliveryTag
        {
            get { return (byte[])this.Fields[2]; }
            set { this.Fields[2] = value; }
        }

        public uint MessageFormat
        {
            get { return this.Fields[3] == null ? uint.MinValue : (uint)this.Fields[3]; }
            set { this.Fields[3] = value; }
        }

        public bool Settled
        {
            get { return this.Fields[4] == null ? false : (bool)this.Fields[4]; }
            set { this.Fields[4] = value; }
        }

        public bool More
        {
            get { return this.Fields[5] == null ? false : (bool)this.Fields[5]; }
            set { this.Fields[5] = value; }
        }

        public ReceiverSettleMode RcvSettleMode
        {
            get { return this.Fields[6] == null ? ReceiverSettleMode.First : (ReceiverSettleMode)this.Fields[6]; }
            set { this.Fields[6] = (byte)value; }
        }

        public DeliveryState State
        {
            get { return (DeliveryState)this.Fields[7]; }
            set { this.Fields[7] = value; }
        }

        public bool Resume
        {
            get { return this.Fields[8] == null ? false : (bool)this.Fields[8]; }
            set { this.Fields[8] = value; }
        }

        public bool Aborted
        {
            get { return this.Fields[9] == null ? false : (bool)this.Fields[9]; }
            set { this.Fields[9] = value; }
        }

        public bool Batchable
        {
            get { return this.Fields[10] == null ? false : (bool)this.Fields[10]; }
            set { this.Fields[10] = value; }
        }

        public override string ToString()
        {
#if TRACE
            return this.GetDebugString(
                "transfer",
                new object[] { "handle", "delivery-id", "delivery-tag", "message-format", "settled", "more", "rcv-settle-mode", "state", "resume", "aborted", "batchable" },
                this.Fields);
#else
            return base.ToString();
#endif
        }
    }
}