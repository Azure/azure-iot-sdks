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
    /// Defines the details of an error.
    /// </summary>
    public sealed class Error : DescribedList
    {
        /// <summary>
        /// Initializes an error object.
        /// </summary>
        public Error()
            : base(Codec.Error, 3)
        {
        }

        /// <summary>
        /// Gets or sets a symbolic value indicating the error condition.
        /// </summary>
        public Symbol Condition
        {
            get { return (Symbol)this.Fields[0]; }
            set { this.Fields[0] = value; }
        }

        /// <summary>
        /// Gets or sets the descriptive text about the error condition.
        /// </summary>
        public string Description
        {
            get { return (string)this.Fields[1]; }
            set { this.Fields[1] = value; }
        }

        /// <summary>
        /// Gets or sets the map carrying information about the error condition.
        /// </summary>
        public Fields Info
        {
            get { return Amqp.Types.Fields.From(this.Fields, 2); }
            set { this.Fields[2] = value; }
        }

#if TRACE
        /// <summary>
        /// Returns a string that represents the current error object.
        /// </summary>
        /// <returns></returns>
        public override string ToString()
        {
            return this.GetDebugString(
                "error",
                new object[] { "condition", "description", "fields" },
                this.Fields);
        }
#endif
    }
}