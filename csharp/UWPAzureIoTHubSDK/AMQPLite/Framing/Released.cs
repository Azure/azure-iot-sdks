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
    /// The released outcome is a terminal delivery state.
    /// </summary>
    public sealed class Released : Outcome
    {
        /// <summary>
        /// Initializes a released object.
        /// </summary>
        public Released()
            : base(Codec.Released, 0)
        {
        }

#if TRACE
        /// <summary>
        /// Returns a string that represents the current released object.
        /// </summary>
        /// <returns></returns>
        public override string ToString()
        {
            return this.GetDebugString(
                "released",
                new object[0],
                this.Fields);
        }
#endif
    }
}