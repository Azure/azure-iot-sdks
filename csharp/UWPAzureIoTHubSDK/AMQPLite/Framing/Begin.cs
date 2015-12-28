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

    sealed class Begin : DescribedList
    {
        public Begin()
            : base(Codec.Begin, 8)
        {
        }

        public ushort RemoteChannel
        {
            get { return this.Fields[0] == null ? ushort.MaxValue : (ushort)this.Fields[0]; }
            set { this.Fields[0] = value; }
        }

        public uint NextOutgoingId
        {
            get { return this.Fields[1] == null ? uint.MinValue : (uint)this.Fields[1]; }
            set { this.Fields[1] = value; }
        }

        public uint IncomingWindow
        {
            get { return this.Fields[2] == null ? uint.MaxValue : (uint)this.Fields[2]; }
            set { this.Fields[2] = value; }
        }

        public uint OutgoingWindow
        {
            get { return this.Fields[3] == null ? uint.MaxValue : (uint)this.Fields[3]; }
            set { this.Fields[3] = value; }
        }

        public uint HandleMax
        {
            get { return this.Fields[4] == null ? uint.MaxValue : (uint)this.Fields[4]; }
            set { this.Fields[4] = value; }
        }

        public Symbol[] OfferedCapabilities
        {
            get { return Codec.GetSymbolMultiple(this.Fields, 5); }
            set { this.Fields[5] = value; }
        }

        public Symbol[] DesiredCapabilities
        {
            get { return Codec.GetSymbolMultiple(this.Fields, 6); }
            set { this.Fields[6] = value; }
        }

        public Fields Properties
        {
            get { return Amqp.Types.Fields.From(this.Fields, 7); }
            set { this.Fields[7] = value; }
        }

        public override string ToString()
        {
#if TRACE
            return this.GetDebugString(
                "begin",
                new object[] { "remote-channel", "next-outgoing-id", "incoming-window", "outgoing-window", "handle-max", "offered-capabilities", "desired-capabilities", "properties" },
                this.Fields);
#else
            return base.ToString();
#endif
        }
    }
}