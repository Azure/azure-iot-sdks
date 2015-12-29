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
    /// The descriptor of a described type.
    /// </summary>
    public sealed class Descriptor
    {
        /// <summary>
        /// The descriptor name.
        /// </summary>
        public readonly string Name;

        /// <summary>
        /// The descriptor code.
        /// </summary>
        public readonly ulong Code;

        /// <summary>
        /// Initializes a descriptor object.
        /// </summary>
        /// <param name="code">The descriptor code.</param>
        /// <param name="name">The descriptor name.</param>
        public Descriptor(ulong code, string name)
        {
            this.Code = code;
            this.Name = name;
        }
    }
}