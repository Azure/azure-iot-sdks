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

    sealed class Flow : DescribedList
    {
        public Flow()
            : base(Codec.Flow, 11)
        {
        }

        public bool HasHandle
        {
            get { return this.Fields[4] != null; }
        }

        public uint NextIncomingId
        {
            get { return this.Fields[0] == null ? uint.MinValue : (uint)this.Fields[0]; }
            set { this.Fields[0] = value; }
        }

        public uint IncomingWindow
        {
            get { return this.Fields[1] == null ? uint.MaxValue : (uint)this.Fields[1]; }
            set { this.Fields[1] = value; }
        }

        public uint NextOutgoingId
        {
            get { return this.Fields[2] == null ? uint.MinValue : (uint)this.Fields[2]; }
            set { this.Fields[2] = value; }
        }

        public uint OutgoingWindow
        {
            get { return this.Fields[3] == null ? uint.MaxValue : (uint)this.Fields[3]; }
            set { this.Fields[3] = value; }
        }

        public uint Handle
        {
            get { return this.Fields[4] == null ? uint.MaxValue : (uint)this.Fields[4]; }
            set { this.Fields[4] = value; }
        }

        public uint DeliveryCount
        {
            get { return this.Fields[5] == null ? uint.MinValue : (uint)this.Fields[5]; }
            set { this.Fields[5] = value; }
        }

        public uint LinkCredit
        {
            get { return this.Fields[6] == null ? uint.MinValue : (uint)this.Fields[6]; }
            set { this.Fields[6] = value; }
        }

        public uint Available
        {
            get { return this.Fields[7] == null ? uint.MinValue : (uint)this.Fields[7]; }
            set { this.Fields[7] = value; }
        }

        public bool Drain
        {
            get { return this.Fields[8] == null ? false : (bool)this.Fields[8]; }
            set { this.Fields[8] = value; }
        }

        public bool Echo
        {
            get { return this.Fields[9] == null ? false : (bool)this.Fields[9]; }
            set { this.Fields[9] = value; }
        }

        public Fields Properties
        {
            get { return Amqp.Types.Fields.From(this.Fields, 10); }
            set { this.Fields[10] = value; }
        }

        public override string ToString()
        {
#if TRACE
            return this.GetDebugString(
                "flow",
                new object[] { "next-in-id", "in-window", "next-out-id", "out-window", "handle", "delivery-count", "link-credit", "available", "drain", "echo", "properties" },
                this.Fields);
#else
            return base.ToString();
#endif
        }
    }
}