// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.Serialization;
using System.Text;
using System.Threading.Tasks;

namespace api_generator
{

    /// <summary>
    /// Class that maps an LWM2M object/resource to IoTHub property name
    /// </summary>
    [DataContract]
    class PropertyMapEntry
    {
        /// <summary>
        /// Property name used by the IoTHub service
        /// </summary>
        [DataMember]
        public string ServiceName = "";

        /// <summary>
        /// Name of the LWM2M object that contains the resource
        /// </summary>
        [DataMember]
        public string Object = "";

        /// <summary>
        /// Name of the LWM2M resource
        /// </summary>
        [DataMember]
        public string Resource = "";

        /// <summary>
        /// Property name used by the Api, optional.  Only needed if default name (Object_Resource) needs to be changed.
        /// </summary>
        [DataMember]
        public string ApiPropertyOverride = "";

        /// <summary>
        /// Was this property mapped to an OMA schema item?
        /// </summary>
        public bool Mapped = false;
    }


}
