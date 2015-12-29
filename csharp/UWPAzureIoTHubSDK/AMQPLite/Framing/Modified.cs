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
    /// The Modified class defines the modified outcome.
    /// </summary>
    public sealed class Modified : Outcome
    {
        /// <summary>
        /// Initializes a modified outcome.
        /// </summary>
        public Modified()
            : base(Codec.Modified, 3)
        {
        }

        /// <summary>
        /// Gets or sets the delivery-failed field.
        /// </summary>
        public bool DeliveryFailed
        {
            get { return this.Fields[0] == null ? false : (bool)this.Fields[0]; }
            set { this.Fields[0] = value; }
        }

        /// <summary>
        /// Gets or sets the undeliverable-here field.
        /// </summary>
        public bool UndeliverableHere
        {
            get { return this.Fields[1] == null ? false : (bool)this.Fields[1]; }
            set { this.Fields[1] = value; }
        }

        /// <summary>
        /// Gets or sets the message-annotations field.
        /// </summary>
        public Fields MessageAnnotations
        {
            get { return Amqp.Types.Fields.From(this.Fields, 2); }
            set { this.Fields[2] = value; }
        }

#if TRACE
        /// <summary>
        /// Returns a string that represents the current modified object.
        /// </summary>
        /// <returns></returns>
        public override string ToString()
        {
            return this.GetDebugString(
                "modified",
                new object[] { "delivery-failed", "undeliverable-here", "message-annotations" },
                this.Fields);
#endif
        }
    }
}