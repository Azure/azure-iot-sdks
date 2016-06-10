// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace api_generator
{
    /// <summary>
    /// Class which defines a property to generate a resource for.
    /// </summary>
    class PropertyToGenerate
    {
        /// <summary>
        /// Name of the property.  This is the name that is exposed to API developers.  
        /// </summary>
        private string _propertyName = null;
        public string PropertyName
        {
            get
            {
                if (_propertyName != null)
                {
                    return _propertyName;
                }

                else
                {
                    return (ObjectName + "_" + ResourceName).Replace(" ", "");
                }

            }
            set
            {
                _propertyName = value;
            }
        }

        /// <summary>
        /// Name of the LWM2M object that contains the resource
        /// </summary>
        public string ObjectName;

        /// <summary>
        /// Name of the LWM2M Resosource
        /// </summary>
        public string ResourceName
        {
            get
            {
                return ResourceFromXml.Name;
            }
        }


        /// <summary>
        /// Resource definition for the resource.
        /// </summary>
        public LWM2MObjectItem ResourceFromXml;

        /// <summary>
        /// Convert the given LWM2M type to it's string representation, as you would use in a function name (e.g. GetTime)
        /// </summary>
        public static string PropertyTypeToNameString(LWM2MObjectItemType value)
        {
            switch (value)
            {
                case LWM2MObjectItemType.Boolean:
                    return "Bool";
                case LWM2MObjectItemType.String:
                    return "String";
                case LWM2MObjectItemType.Integer:
                    return "Integer";
                case LWM2MObjectItemType.Float:
                    return "Float";
                case LWM2MObjectItemType.Opaque:
                    return "Opaque";
                case LWM2MObjectItemType.Time:
                    return "Time";
                default:
                    return "UNDEFINED";
            }
        }

        /// <summary>
        /// Convert the given LWM2M type to a string representation of the type name you would use in code. -- e.g. String->char*
        /// </summary>
        public static string PropertyTypeToTypeCodeName(LWM2MObjectItemType value)
        {
            switch (value)
            {
                case LWM2MObjectItemType.Boolean:
                    return "bool";
                case LWM2MObjectItemType.String:
                    return "char*";
                case LWM2MObjectItemType.Integer:
                    return "int64_t";
                case LWM2MObjectItemType.Float:
                    return "double";
                case LWM2MObjectItemType.Opaque:
                    return "void*";
                case LWM2MObjectItemType.Time:
                    return "int64_t";
                default:
                    return "UNKONWN_OBJECT_TYPE";
            }
        }

        public string DefaultValue
        {
            get
            {
                switch (ResourceFromXml.Type)
                {
                    case LWM2MObjectItemType.Boolean:
                        return "false";
                    case LWM2MObjectItemType.String:
                        return "NULL";
                    case LWM2MObjectItemType.Integer:
                        return "0";
                    case LWM2MObjectItemType.Float:
                        return "0.0";
                    case LWM2MObjectItemType.Opaque:
                        return "NULL";
                    case LWM2MObjectItemType.Time:
                        return "0";
                    default:
                        return "UNKONWN_OBJECT_TYPE";
                }
            }
        }


        public string PropertyTypeCodeName
        {
            get
            {
                return PropertyTypeToTypeCodeName(ResourceFromXml.Type);
            }
        }

        /// <summary>
        /// Type of property, as a string
        /// </summary>
        public string PropertyTypeName
        {
            get
            {
                return PropertyTypeToNameString(ResourceFromXml.Type);
            }
        }

        /// <summary>
        /// Property type as LWM2MType_ string
        /// </summary>
        public string LWM2MTypeText
        {
            get
            {
                if (PropertyTypeName.ToUpper().Equals("TIME"))
                {
                    return "LWM2M_TYPE_INTEGER";
                }
                else
                {
                    return "LWM2M_TYPE_" + PropertyTypeName.ToUpper();
                }
            }
        }

        public LWM2MObjectItemType Type
        {
            get
            {
                return ResourceFromXml.Type;
            }
        }


        /// <summary>
        /// Returns true if property supports Write operation
        /// </summary>
        public bool CanWrite
        {
            get
            {
                switch (ResourceFromXml.Operations)
                {
                    case LWM2MObjectItemOperations.W:
                    case LWM2MObjectItemOperations.RW:
                    case LWM2MObjectItemOperations.WE:
                    case LWM2MObjectItemOperations.RWE:
                        return true;
                    default:
                        return false;
                }
            }
        }

        public string WriteCallbackSetterCodeName
        {
            get
            {
                return "IotHubClient_" + PropertyName + "_SetWriteCallback";
            }
        }

        public static string PropertyTypeToWritePrototype(LWM2MObjectItemType value)
        {
            return "IOTHUB_CLIENT_SET_" + PropertyTypeToNameString(value).ToUpper() + "PROP";
        }

        public string WriteCallbackPrototypeCodeName
        {
            get
            {
                return PropertyTypeToWritePrototype(ResourceFromXml.Type);
            }
        }


        /// <summary>
        /// Returns TRUE if property supports Read operation
        /// </summary>
        public bool CanRead
        {
            get
            {
                switch (ResourceFromXml.Operations)
                {
                    case LWM2MObjectItemOperations.R:
                    case LWM2MObjectItemOperations.RW:
                    case LWM2MObjectItemOperations.RE:
                    case LWM2MObjectItemOperations.RWE:
                        return true;
                    default:
                        return false;
                }
            }
        }

        public string  ReadCallbackSetterCodeName
        {
            get
            {
                return "IotHubClient_" + PropertyName + "_SetReadCallback";
            }
        }
        public static string PropertyTypeToReadPrototype(LWM2MObjectItemType value)
        {
            return "IOTHUB_CLIENT_GET_" + PropertyTypeToNameString(value).ToUpper() + "PROP";
        }
        public string PushApiCodeName
        {
            get
            {
                return "IotHubClient_Push_" + PropertyName;
            }

        }

        public string ReadCallbackPrototypeCodeName
        {
            get
            {
                return PropertyTypeToReadPrototype(ResourceFromXml.Type);
            }
        }

        /// <summary>
        /// Returns true if property supports execute operation
        /// </summary>
        public bool CanExecute
        {
            get
            {
                switch (ResourceFromXml.Operations)
                {
                    case LWM2MObjectItemOperations.E:
                    case LWM2MObjectItemOperations.RE:
                    case LWM2MObjectItemOperations.WE:
                    case LWM2MObjectItemOperations.RWE:
                        return true;
                    default:
                        return false;
                }

            }
        }

        public string ExecuteCallbackSetterCodeName
        {
            get
            {
                return "IotHubClient_" + PropertyName + "_SetExecuteCallback";
            }
        }

        public static string ExecuteCallbackPrototypeCodeName
        {
            get
            {
                return "IOTHUB_CLIENT_HANDLE_EXECUTE";

            }
        }

        /// <summary>
        /// LWM2M Object ID for the object that holds the property
        /// </summary>
        public string ObjectId;
        
        /// <summary>
        /// LWM2M Property ID for this property
        /// </summary>
        public string PropertyId
        {
            get
            {
                return ResourceFromXml.ID;
            }
        }

        public string SampleVariableName
        {
            get
            {
                return "propval_" + PropertyName.ToLower();
            }
        }

        public int PropertyIndex;

    }
}
