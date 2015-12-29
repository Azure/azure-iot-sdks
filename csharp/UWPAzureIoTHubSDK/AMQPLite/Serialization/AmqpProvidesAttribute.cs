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
    /// Specifies that the current type can be set to the provided type.
    /// </summary>
    [AttributeUsage(AttributeTargets.Class | AttributeTargets.Struct,
        Inherited = true, AllowMultiple = true)]
    public sealed class AmqpProvidesAttribute : Attribute
    {
        readonly Type type;

        /// <summary>
        /// Initializes the attribute object.
        /// </summary>
        /// <param name="type">The provided type.</param>
        public AmqpProvidesAttribute(Type type)
        {
            this.type = type;
        }

        /// <summary>
        /// Gets the provided type.
        /// </summary>
        public Type Type
        {
            get { return this.type; }
        }
    }
}