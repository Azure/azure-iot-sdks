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

namespace Amqp
{
    using System;
    using Amqp.Framing;

    /// <summary>
    /// The exception that is thrown when an AMQP error occurs.
    /// </summary>
    public sealed class AmqpException : Exception
    {
        /// <summary>
        /// Initializes a new instance of the AmqpException class with the AMQP error.
        /// </summary>
        /// <param name="error">The AMQP error.</param>
        public AmqpException(Error error)
            : base(error.Description ?? error.Condition)
        {
            this.Error = error;
        }

        /// <summary>
        /// Initializes a new instance of the AmqpException class with the AMQP error.
        /// </summary>
        /// <param name="condition">The error condition.</param>
        /// <param name="description">The error description.</param>
        public AmqpException(string condition, string description)
            : this(new Error() { Condition = condition, Description = description })
        {
        }

        /// <summary>
        /// Gets the AMQP error stored in this exception.
        /// </summary>
        public Error Error
        {
            get;
            private set;
        }
    }
}