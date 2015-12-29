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

namespace Amqp.Sasl
{
    using Amqp.Framing;
    using Amqp.Types;

    sealed class SaslOutcome : DescribedList
    {
        public SaslOutcome()
            : base(Codec.SaslOutcome, 2)
        {
        }

        public SaslCode Code
        {
            get { return (SaslCode)this.Fields[0]; }
            set { this.Fields[0] = (byte)value; }
        }

        public byte[] AdditionalData
        {
            get { return (byte[])this.Fields[1]; }
            set { this.Fields[1] = value; }
        }

#if TRACE
        public override string ToString()
        {
            return this.GetDebugString(
                "sasl-outcome",
                new object[] { "code", "additional-data" },
                this.Fields);
        }
#endif
    }
}