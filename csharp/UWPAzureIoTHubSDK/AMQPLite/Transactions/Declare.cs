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

namespace Amqp.Transactions
{
    using Amqp.Framing;
    using Amqp.Types;

    /// <summary>
    /// Message body for declaring a transaction id.
    /// </summary>
    public sealed class Declare : DescribedList
    {
        /// <summary>
        /// Initializes a declare object.
        /// </summary>
        public Declare()
            : base(Codec.Declare, 1)
        {
        }

        /// <summary>
        /// Gets or sets the global-id field.
        /// </summary>
        public object GlobalId
        {
            get { return this.Fields[0]; }
            set { this.Fields[0] = value; }
        }

#if TRACE
        /// <summary>
        /// Returns a string that represents the current object.
        /// </summary>
        /// <returns></returns>
        public override string ToString()
        {
            return this.GetDebugString(
                "declare",
                new object[] { "global-id" },
                this.Fields);
        }
#endif
    }
}