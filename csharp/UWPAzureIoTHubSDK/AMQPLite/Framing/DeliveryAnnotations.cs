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
    /// The delivery-annotations section is used for delivery-specific non-standard
    /// properties at the head of the message.
    /// </summary>
    public sealed class DeliveryAnnotations : DescribedMap
    {
        /// <summary>
        /// Initializes a delivery annotations section.
        /// </summary>
        public DeliveryAnnotations()
            : base(Codec.DeliveryAnnotations, typeof(Symbol))
        {
        }

#if TRACE
        /// <summary>
        /// Returns a string that represents the current delivery annotations object.
        /// </summary>
        /// <returns></returns>
        public override string ToString()
        {
            return "delivery-annotations:" + base.ToString();
        }
#endif
    }
}