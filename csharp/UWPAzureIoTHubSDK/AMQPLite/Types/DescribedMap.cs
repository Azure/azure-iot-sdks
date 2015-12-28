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
    using System.Collections;

    /// <summary>
    /// The DescribedMap class consist of a descriptor and an AMQP map.
    /// </summary>
    public abstract class DescribedMap : RestrictedDescribed
    {
        readonly Type keyType;
        Map map;

        /// <summary>
        /// Initializes the described map object.
        /// </summary>
        /// <param name="descriptor">The descriptor of the concrete described map class.</param>
        /// <param name="keyType">The allowed key type.</param>
        protected DescribedMap(Descriptor descriptor, Type keyType)
            : base(descriptor)
        {
            this.keyType = keyType;
            this.map = new Map();
        }

        /// <summary>
        /// Gets the map value.
        /// </summary>
        public Map Map { get { return this.map; } }

        /// <summary>
        /// Gets the object associated with the key.
        /// </summary>
        /// <param name="key">The key.</param>
        /// <returns></returns>
        public object this[object key]
        {
            get
            {
                Map.ValidateKeyType(this.keyType, key.GetType());
                return this.map[key];
            }

            set
            {
                Map.ValidateKeyType(this.keyType, key.GetType());
                this.map[key] = value;
            }
        }

        internal override void DecodeValue(ByteBuffer buffer)
        {
            this.map = Encoder.ReadMap(buffer, Encoder.ReadFormatCode(buffer));
        }

        internal override void EncodeValue(ByteBuffer buffer)
        {
            Encoder.WriteMap(buffer, this.map, true);
        }

        /// <summary>
        /// Returns a string that represents the current map.
        /// </summary>
        /// <returns></returns>
        public override string ToString()
        {
            return this.map.ToString();
        }
    }
}
