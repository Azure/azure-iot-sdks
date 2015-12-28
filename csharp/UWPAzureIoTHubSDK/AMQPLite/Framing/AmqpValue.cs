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

    /// <summary>
    /// An amqp-value section contains a single AMQP value.
    /// </summary>
    public sealed class AmqpValue : RestrictedDescribed
    {
        object value;

        /// <summary>
        /// Initializes an AmqpValue object.
        /// </summary>
        public AmqpValue()
            : base(Codec.AmqpValue)
        {
        }

        /// <summary>
        /// Gets or sets the value in the body section.
        /// </summary>
        public object Value
        {
            get { return this.value; }
            set { this.value = value; }
        }

#if DOTNET
        ByteBuffer valueBuffer;

        internal T GetValue<T>()
        {
            if (this.value == null)
            {
                return default(T);
            }
            else if (typeof(T) == typeof(object) || typeof(T) == this.value.GetType())
            {
                return (T)this.value;
            }
            else
            {
                return Serialization.AmqpSerializer.Deserialize<T>(this.valueBuffer);
            }
        }

        internal override void EncodeValue(ByteBuffer buffer)
        {
            Serialization.AmqpSerializer.Serialize(buffer, this.value);
        }

        internal override void DecodeValue(ByteBuffer buffer)
        {
            int offset = buffer.Offset;
            this.value = Encoder.ReadObject(buffer);
            if (this.value is DescribedValue)
            {
                int count = buffer.Offset - offset;
                this.valueBuffer = new ByteBuffer(buffer.Buffer, offset, count, count);
            }
        }
#else
        internal override void EncodeValue(ByteBuffer buffer)
        {
            Encoder.WriteObject(buffer, this.value);
        }

        internal override void DecodeValue(ByteBuffer buffer)
        {
            this.value = Encoder.ReadObject(buffer);
        }
#endif
    }
}