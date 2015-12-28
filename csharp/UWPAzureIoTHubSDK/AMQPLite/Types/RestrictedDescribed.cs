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

namespace Amqp.Types
{
    /// <summary>
    /// The RestrictedDescribed class is an AMQP described value, whose descriptor is
    /// restricted to symbol or ulong.
    /// </summary>
    public abstract class RestrictedDescribed : Described
    {
        readonly Descriptor descriptor;

        /// <summary>
        /// Initializes a described value.
        /// </summary>
        /// <param name="descriptor">The descriptor of the value.</param>
        protected RestrictedDescribed(Descriptor descriptor)
        {
            this.descriptor = descriptor;
        }

        /// <summary>
        /// Gets the descriptor.
        /// </summary>
        public Descriptor Descriptor
        {
            get { return this.descriptor; }
        }

        internal override void EncodeDescriptor(ByteBuffer buffer)
        {
            Encoder.WriteULong(buffer, this.descriptor.Code, true);
        }

        internal override void DecodeDescriptor(ByteBuffer buffer)
        {
            Encoder.ReadObject(buffer);
        }
    }
}
