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
    /// Outcome is a terminal delivery-state of a delivery.
    /// </summary>
    public abstract class Outcome : DeliveryState
    {
        /// <summary>
        /// Initializes the outcome object.
        /// </summary>
        /// <param name="descriptor">The descriptor of the concrete outcome class.</param>
        /// <param name="fieldCount">The number of fields of the concrete outcome class.</param>
        protected Outcome(Descriptor descriptor, int fieldCount)
            : base(descriptor, fieldCount)
        {
        }
    }
}
