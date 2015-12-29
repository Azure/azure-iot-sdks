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
    using System;

    /// <summary>
    /// Defines symbolic values from a constrained domain.
    /// </summary>
    public class Symbol
    {
        string value;

        /// <summary>
        /// Initializes a symbol value.
        /// </summary>
        /// <param name="value">The string value./</param>
        public Symbol(string value)
        {
            if (value == null)
            {
                throw new ArgumentNullException("value");
            }

            this.value = value;
        }

        /// <summary>
        /// Converts a string value to a symbol implicitly.
        /// </summary>
        /// <param name="value">The string value.</param>
        /// <returns></returns>
        public static implicit operator Symbol(string value)
        {
            return value == null ? null : new Symbol(value);
        }

        /// <summary>
        /// Converts a symbol to a string value implicitly.
        /// </summary>
        /// <param name="value">the symbol value.</param>
        /// <returns></returns>
        public static implicit operator string(Symbol value)
        {
            return value == null ? null : value.value;
        }

        /// <summary>
        /// Compares equality of an object with the current symbol.
        /// </summary>
        /// <param name="obj">The object to compare.</param>
        /// <returns></returns>
        public override bool Equals(object obj)
        {
            Symbol other = obj as Symbol;
            return other != null ? this.value.Equals(other.value) : false;
        }

        /// <summary>
        /// Gets the hash code of the symbol object.
        /// </summary>
        /// <returns></returns>
        public override int GetHashCode()
        {
            return this.value.GetHashCode();
        }

        /// <summary>
        /// Returns a string that represents the current map object.
        /// </summary>
        /// <returns></returns>
        public override string ToString()
        {
            return this.value.ToString();
        }
    }
}
