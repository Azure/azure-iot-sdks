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
    /// <summary>
    /// Specifies that a type is serializable by the AMQP serializer.
    /// </summary>
    public interface IAmqpSerializable
    {
        /// <summary>
        /// Gets the encode size in bytes of an instance.
        /// </summary>
        int EncodeSize
        {
            get;
        }

        /// <summary>
        /// Encodes the serializable object into a buffer.
        /// </summary>
        /// <param name="buffer"></param>
        void Encode(ByteBuffer buffer);

        /// <summary>
        /// Decodes and initializes the serializable object from a buffer.
        /// </summary>
        /// <param name="buffer">The buffer to read.</param>
        void Decode(ByteBuffer buffer);
    }
}