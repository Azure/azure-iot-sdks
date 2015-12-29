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
    /// Specifies that the type is serializable by the AMQP serializer.
    /// </summary>
    [AttributeUsage(AttributeTargets.Class | AttributeTargets.Struct,
        AllowMultiple = false, Inherited = true)]
    public sealed class AmqpContractAttribute : Attribute
    {
        /// <summary>
        /// Initializes a new instance of the AmqpContractAttribute class.
        /// </summary>
        public AmqpContractAttribute()
        {
            this.Encoding = EncodingType.List;
            this.Code = -1;
        }

        /// <summary>
        /// Gets or sets the descriptor name for the type.
        /// </summary>
        public string Name
        {
            get;
            set;
        }

        /// <summary>
        /// Gets or sets the descriptor code for the type.
        /// </summary>
        public long Code
        {
            get;
            set;
        }

        /// <summary>
        /// Gets or sets the encoding type for the type.
        /// </summary>
        public EncodingType Encoding
        {
            get;
            set;
        }

        internal ulong? InternalCode
        {
            get { return this.Code >= 0 ? (ulong?)this.Code : null; }
        }
    }
}