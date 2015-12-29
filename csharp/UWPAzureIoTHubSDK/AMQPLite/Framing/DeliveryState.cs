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
    /// The Messaging layer defines a concrete set of delivery states which can be used
    /// to indicate the state of the message at the receiver.
    /// </summary>
    public abstract class DeliveryState : DescribedList
    {
        /// <summary>
        /// Initializes the delivery state object.
        /// </summary>
        /// <param name="descriptor">The descriptor of the concrete delivery state class.</param>
        /// <param name="fieldCount">The number of fields of the concrete delivery state class.</param>
        protected DeliveryState(Descriptor descriptor, int fieldCount)
            : base(descriptor, fieldCount)
        {
        }
    }
}
