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

namespace Amqp.Serialization
{
    using System;

    /// <summary>
    /// Specifies that the member is part of an AMQP serializable type.
    /// </summary>
    [AttributeUsage(AttributeTargets.Property | AttributeTargets.Field,
        AllowMultiple = false, Inherited = true)]
    public sealed class AmqpMemberAttribute : Attribute
    {
        int? order;

        /// <summary>
        /// Gets or sets the field name of the member.
        /// </summary>
        public string Name 
        {
            get; 
            set; 
        }

        /// <summary>
        /// Gets or sets the order of the member in the list when encoding type is
        /// set to list.
        /// </summary>
        public int Order 
        {
            get { return this.order.HasValue ? this.order.Value : 0; }

            set { this.order = value; }
        }

        internal int? InternalOrder
        {
            get { return this.order; }
        }
    }
}
