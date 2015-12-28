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

    sealed class Received : DeliveryState
    {
        public Received()
            : base(Codec.Received, 2)
        {
        }

        public uint SectionNumber
        {
            get { return this.Fields[0] == null ? uint.MinValue : (uint)this.Fields[0]; }
            set { this.Fields[0] = value; }
        }

        public ulong SectionOffset
        {
            get { return this.Fields[1] == null ? ulong.MinValue : (ulong)this.Fields[1]; }
            set { this.Fields[1] = value; }
        }

        public override string ToString()
        {
#if TRACE
            return this.GetDebugString(
                "received",
                new object[] { "section-number", "section-offset" },
                this.Fields);
#else
            return base.ToString();
#endif
        }
    }
}