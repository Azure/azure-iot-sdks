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
    /// A mapping from field name to value. The keys are restricted to be of type symbol.
    /// </summary>
    public class Fields : Map
    {
        /// <summary>
        /// Creates a field object from an object in the array at the specified index.
        /// </summary>
        /// <param name="array">The object array.</param>
        /// <param name="index">The index of the object in the array.</param>
        /// <returns></returns>
        public static Fields From(object[] array, int index)
        {
            object obj = array[index];
            if (obj != null)
            {
                Fields fields = array[index] as Fields;
                if (fields == null)
                {
                    fields = new Fields();
                    Map map = (Map)obj;
                    foreach (var key in map.Keys)
                    {
                        fields.Add(key, map[key]);
                    }

                    array[index] = fields;
                }

                return fields;
            }
            else
            {
                return null;
            }
        }

        internal override void CheckKeyType(Type keyType)
        {
            Map.ValidateKeyType(typeof(Symbol), keyType);
        }
    }
}
