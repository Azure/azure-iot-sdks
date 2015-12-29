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
    using System;

    /// <summary>
    /// A described value consist of a descriptor and a value. Both of them are valid
    /// AMQP types.
    /// </summary>
    public class DescribedValue : Described
    {
        object descriptor;
        object value;

        /// <summary>
        /// Initializes a described value.
        /// </summary>
        /// <param name="descriptor">The descriptor object.</param>
        /// <param name="value">The value object.</param>
        public DescribedValue(object descriptor, object value)
        {
            this.descriptor = descriptor;
            this.value = value;
        }

        /// <summary>
        /// Gets the descriptor object.
        /// </summary>
        public object Descriptor
        {
            get { return this.descriptor; }
        }

        /// <summary>
        /// Gets the value object.
        /// </summary>
        public object Value
        {
            get { return this.value; }
        }

        internal override void EncodeDescriptor(ByteBuffer buffer)
        {
            Encoder.WriteObject(buffer, this.descriptor);
        }

        internal override void EncodeValue(ByteBuffer buffer)
        {
            Encoder.WriteObject(buffer, this.value);
        }

        internal override void DecodeDescriptor(ByteBuffer buffer)
        {
            this.descriptor = Encoder.ReadObject(buffer);
        }

        internal override void DecodeValue(ByteBuffer buffer)
        {
            this.value = Encoder.ReadObject(buffer);
        }

#if TRACE
        /// <summary>
        /// Returns a string that represents the current object.
        /// </summary>
        /// <returns></returns>
        public override string ToString()
        {
            return this.value == null ? "nil" : this.value.ToString();
        }
#endif
    }
}
