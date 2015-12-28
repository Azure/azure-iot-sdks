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
    using System;
    using System.Collections;
    using System.Collections.Generic;
    using System.IO;
    using System.Reflection;
    using System.Runtime.Serialization;
    using Amqp.Types;

    /// <summary>
    /// Serializes and deserializes an instance of an AMQP type.
    /// The descriptor (name and code) is scoped to and must be
    /// uniqueue within an instance of the serializer.
    /// When the static Serialize and Deserialize methods are called,
    /// the default instance is used.
    /// </summary>
    public sealed class AmqpSerializer
    {
        static readonly AmqpSerializer instance = new AmqpSerializer();
        readonly Dictionary<Type, SerializableType> typeCache;

        /// <summary>
        /// Initializes a new instance of the AmqpSerializer class.
        /// </summary>
        public AmqpSerializer()
        {
            this.typeCache = new Dictionary<Type, SerializableType>();
        }

        /// <summary>
        /// Serializes an instance of an AMQP type into a buffer.
        /// </summary>
        /// <param name="buffer">The buffer.</param>
        /// <param name="graph">The serializable AMQP object.</param>
        public static void Serialize(ByteBuffer buffer, object graph)
        {
            WriteObject(instance, buffer, graph);
        }

        /// <summary>
        /// Deserializes an instance of an AMQP type from a buffer.
        /// </summary>
        /// <typeparam name="T">The serializable type.</typeparam>
        /// <param name="buffer">The buffer to read from.</param>
        /// <returns></returns>
        public static T Deserialize<T>(ByteBuffer buffer)
        {
            return ReadObject<T, T>(instance, buffer);
        }

        /// <summary>
        /// Deserializes an instance of an AMQP type from a buffer.
        /// </summary>
        /// <typeparam name="T">The serializable type.</typeparam>
        /// <typeparam name="TAs">The return type of the deserialized object.</typeparam>
        /// <param name="buffer">The buffer to read from.</param>
        /// <returns></returns>
        public static TAs Deserialize<T, TAs>(ByteBuffer buffer)
        {
            return ReadObject<T, TAs>(instance, buffer);
        }

        /// <summary>
        /// Writes an serializable object into a buffer.
        /// </summary>
        /// <param name="buffer">The buffer to write.</param>
        /// <param name="graph">The serializable object.</param>
        public void WriteObject(ByteBuffer buffer, object graph)
        {
            WriteObject(this, buffer, graph);
        }

        /// <summary>
        /// Reads an serializable object from a buffer.
        /// </summary>
        /// <typeparam name="T">The type of the serializable object.</typeparam>
        /// <param name="buffer">The buffer to read.</param>
        /// <returns></returns>
        public T ReadObject<T>(ByteBuffer buffer)
        {
            return ReadObject<T, T>(this, buffer);
        }

        /// <summary>
        /// Reads an serializable object from a buffer.
        /// </summary>
        /// <typeparam name="T">The type of the serializable object.</typeparam>
        /// <typeparam name="TAs">The return type of the deserialized object.</typeparam>
        /// <param name="buffer">The buffer to read.</param>
        /// <returns></returns>
        public TAs ReadObject<T, TAs>(ByteBuffer buffer)
        {
            return ReadObject<T, TAs>(this, buffer);
        }

        internal SerializableType GetType(Type type)
        {
            return this.GetOrCompileType(type, false);
        }

        static void WriteObject(AmqpSerializer serializer, ByteBuffer buffer, object graph)
        {
            if (graph == null)
            {
                Encoder.WriteObject(buffer, null);
            }
            else
            {
                SerializableType type = serializer.GetType(graph.GetType());
                type.WriteObject(buffer, graph);
            }
        }

        static TAs ReadObject<T, TAs>(AmqpSerializer serializer, ByteBuffer buffer)
        {
            SerializableType type = serializer.GetType(typeof(T));
            return (TAs)type.ReadObject(buffer);
        }

        SerializableType GetOrCompileType(Type type, bool describedOnly)
        {
            SerializableType serialiableType = null;
            if (!this.typeCache.TryGetValue(type, out serialiableType))
            {
                serialiableType = this.CompileType(type, describedOnly);
                if (serialiableType != null)
                {
                    this.typeCache[type] = serialiableType;
                }
            }

            if (serialiableType == null)
            {
                throw new NotSupportedException(type.FullName);
            }

            return serialiableType;
        }

        SerializableType CompileType(Type type, bool describedOnly)
        {
            object[] typeAttributes = type.GetCustomAttributes(typeof(AmqpContractAttribute), false);
            if (typeAttributes.Length == 0)
            {
                if (describedOnly)
                {
                    return null;
                }
                else
                {
                    return CompileNonContractTypes(type);
                }
            }

            AmqpContractAttribute contractAttribute = (AmqpContractAttribute)typeAttributes[0];
            SerializableType baseType = null;
            if (type.BaseType != typeof(object))
            {
                baseType = this.CompileType(type.BaseType, true);
                if (baseType != null)
                {
                    if (baseType.Encoding != contractAttribute.Encoding)
                    {
                        throw new SerializationException(
                            Fx.Format("{0}.Encoding ({1}) is different from {2}.Encoding ({3})",
                                type.Name, contractAttribute.Encoding, type.BaseType.Name, baseType.Encoding));
                    }

                    this.typeCache[type.BaseType] = baseType;
                }
            }

            string descriptorName = contractAttribute.Name;
            ulong? descriptorCode = contractAttribute.InternalCode;
            if (descriptorName == null && descriptorCode == null)
            {
                descriptorName = type.FullName;
            }

            List<SerialiableMember> memberList = new List<SerialiableMember>();
            if (contractAttribute.Encoding == EncodingType.List && baseType != null)
            {
                memberList.AddRange(baseType.Members);
            }

            int lastOrder = memberList.Count + 1;
            MemberInfo[] memberInfos = type.GetMembers(BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.Instance);
            MethodAccessor onDeserialized = null;
            foreach (MemberInfo memberInfo in memberInfos)
            {
                if (memberInfo.DeclaringType != type)
                {
                    continue;
                }

                if (memberInfo.MemberType == MemberTypes.Field ||
                    memberInfo.MemberType == MemberTypes.Property)
                {
                    object[] memberAttributes = memberInfo.GetCustomAttributes(typeof(AmqpMemberAttribute), true);
                    if (memberAttributes.Length != 1)
                    {
                        continue;
                    }

                    AmqpMemberAttribute attribute = (AmqpMemberAttribute)memberAttributes[0];

                    SerialiableMember member = new SerialiableMember();
                    member.Name = attribute.Name ?? memberInfo.Name;
                    member.Order = attribute.InternalOrder ?? lastOrder++;
                    member.Accessor = MemberAccessor.Create(memberInfo, true);

                    // This will recursively resolve member types
                    Type memberType = memberInfo.MemberType == MemberTypes.Field ? ((FieldInfo)memberInfo).FieldType : ((PropertyInfo)memberInfo).PropertyType;
                    member.Type = GetType(memberType);

                    memberList.Add(member);
                }
                else if (memberInfo.MemberType == MemberTypes.Method)
                {
                    object[] memberAttributes = memberInfo.GetCustomAttributes(typeof(OnDeserializedAttribute), false);
                    if (memberAttributes.Length == 1)
                    {
                        onDeserialized = MethodAccessor.Create((MethodInfo)memberInfo);
                    }
                }
            }

            if (contractAttribute.Encoding == EncodingType.List)
            {
                memberList.Sort(MemberOrderComparer.Instance);
                int order = -1;
                foreach (SerialiableMember member in memberList)
                {
                    if (order > 0 && member.Order == order)
                    {
                        throw new SerializationException(Fx.Format("Duplicate Order {0} detected in {1}", order, type.Name));
                    }

                    order = member.Order;
                }
            }

            SerialiableMember[] members = memberList.ToArray();

            Dictionary<Type, SerializableType> knownTypes = null;
            foreach (object o in type.GetCustomAttributes(typeof(AmqpProvidesAttribute), false))
            {
                AmqpProvidesAttribute knownAttribute = (AmqpProvidesAttribute)o;
                if (knownAttribute.Type.GetCustomAttributes(typeof(AmqpContractAttribute), false).Length > 0)
                {
                    if (knownTypes == null)
                    {
                        knownTypes = new Dictionary<Type, SerializableType>();
                    }

                    // KnownType compilation is delayed and non-recursive to avoid circular references
                    knownTypes.Add(knownAttribute.Type, null);
                }
            }

            if (contractAttribute.Encoding == EncodingType.List)
            {
                return SerializableType.CreateDescribedListType(this, type, baseType, descriptorName, 
                    descriptorCode, members, knownTypes, onDeserialized);
            }
            else if (contractAttribute.Encoding == EncodingType.Map)
            {
                return SerializableType.CreateDescribedMapType(this, type, baseType, descriptorName,
                    descriptorCode, members, knownTypes, onDeserialized);
            }
            else
            {
                throw new NotSupportedException(contractAttribute.Encoding.ToString());
            }
        }

        SerializableType CompileNonContractTypes(Type type)
        {
            // built-in type
            Encode encoder;
            Decode decoder;
            if (Encoder.TryGetCodec(type, out encoder, out decoder))
            {
                return SerializableType.CreatePrimitiveType(type, encoder, decoder);
            }

            if (type == typeof(object))
            {
                return SerializableType.CreateObjectType(type);
            }

            if (typeof(IAmqpSerializable).IsAssignableFrom(type))
            {
                return SerializableType.CreateAmqpSerializableType(this, type);
            }

            if (typeof(Described).IsAssignableFrom(type))
            {
                return SerializableType.CreateAmqpDescribedType(this, type);
            }

            SerializableType nullable = this.CompileNullableTypes(type);
            if (nullable != null)
            {
                return nullable;
            }

            SerializableType collection = this.CompileCollectionTypes(type);
            if (collection != null)
            {
                return collection;
            }

            return null;
        }

        SerializableType CompileNullableTypes(Type type)
        {
            if (type.IsGenericType &&
                type.GetGenericTypeDefinition() == typeof(Nullable<>))
            {
                Type[] argTypes = type.GetGenericArguments();
                Fx.Assert(argTypes.Length == 1, "Nullable type must have one argument");
                return SerializableType.CreateNullableType(type, this.GetType(argTypes[0]));
            }

            return null;
        }

        SerializableType CompileCollectionTypes(Type type)
        {
            MemberAccessor keyAccessor = null;
            MemberAccessor valueAccessor = null;
            MethodAccessor addAccess = null;
            Type itemType = null;

            foreach (Type it in type.GetInterfaces())
            {
                if (it.IsGenericType)
                {
                    Type genericTypeDef = it.GetGenericTypeDefinition();
                    if (genericTypeDef == typeof(IDictionary<,>))
                    {
                        Type[] argTypes = it.GetGenericArguments();
                        itemType = typeof(KeyValuePair<,>).MakeGenericType(argTypes);
                        keyAccessor = MemberAccessor.Create(itemType.GetProperty("Key"), false);
                        valueAccessor = MemberAccessor.Create(itemType.GetProperty("Value"), false);
                        addAccess = MethodAccessor.Create(type.GetMethod("Add", argTypes));

                        return SerializableType.CreateGenericMapType(this, type, keyAccessor, valueAccessor, addAccess);
                    }
                    else if (genericTypeDef == typeof(IList<>))
                    {
                        Type[] argTypes = it.GetGenericArguments();
                        itemType = argTypes[0];
                        addAccess = MethodAccessor.Create(type.GetMethod("Add", argTypes));

                        return SerializableType.CreateGenericListType(this, type, itemType, addAccess);
                    }
                }
            }

            return null;
        }

        sealed class MemberOrderComparer : IComparer<SerialiableMember>
        {
            public static readonly MemberOrderComparer Instance = new MemberOrderComparer();

            public int Compare(SerialiableMember m1, SerialiableMember m2)
            {
                return m1.Order == m2.Order ? 0 : (m1.Order > m2.Order ? 1 : -1);
            }
        }
    }
}
