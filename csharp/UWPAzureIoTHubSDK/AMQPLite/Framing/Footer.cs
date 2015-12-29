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
    using System;

    /// <summary>
    /// The Footer class represents the footer section of a message.
    /// </summary>
    public sealed class Footer : DescribedMap
    {
        /// <summary>
        /// Initializes a footer section.
        /// </summary>
        public Footer()
            : base(Codec.Footer, typeof(Symbol))
        {
        }

#if TRACE
        /// <summary>
        /// Returns a string that represents the current footer object.
        /// </summary>
        /// <returns></returns>
        public override string ToString()
        {
            return "footer:" + base.ToString();
        }
#endif
    }
}