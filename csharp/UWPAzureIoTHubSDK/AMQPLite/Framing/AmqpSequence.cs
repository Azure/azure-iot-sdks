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
    using System.Collections;
    using Amqp.Types;

    /// <summary>
    /// A sequence section contains an arbitrary number of structured data elements.
    /// </summary>
    public sealed class AmqpSequence : RestrictedDescribed
    {
        /// <summary>
        /// Initializes an AmqpSequence object.
        /// </summary>
        public AmqpSequence()
            : base(Codec.AmqpSequence)
        {
        }

        /// <summary>
        /// Gets or sets the list (data elements) in the section.
        /// </summary>
        public IList List
        {
            get;
            set;
        }

        internal override void EncodeValue(ByteBuffer buffer)
        {
            Encoder.WriteList(buffer, this.List, true);
        }

        internal override void DecodeValue(ByteBuffer buffer)
        {
            this.List = Encoder.ReadList(buffer, Encoder.ReadFormatCode(buffer));
        }
    }
}