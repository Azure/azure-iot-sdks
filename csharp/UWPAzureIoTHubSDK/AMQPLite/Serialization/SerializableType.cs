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
    using System.Runtime.Serialization;
    using Amqp.Types;

    abstract class SerializableType
    {
        readonly AmqpSerializer serializer;
        readonly Type type;
        readonly bool hasDefaultCtor;

        protected SerializableType(AmqpSerializer serializer, Type type)
        {
            this.serializer = serializer;
            this.type = type;
            this.hasDefaultCtor = type.GetConstructor(Type.EmptyTypes) != null;
        }

        public virtual EncodingType Encoding
        {
            get
            {
                throw new InvalidOperationException();
            }
        }

        public virtual SerialiableMember[] Members
        {
            get
            {
                throw new InvalidOperationException();
            }
        }

        public static SerializableType CreatePrimitiveType(Type type, Encode encoder, Decode decoder)
        {
            return new AmqpPrimitiveType(type, encoder, decoder);
        }

        public static SerializableType CreateObjectType(Type type)
        {
            return new AmqpObjectType(type);
        }

        public static SerializableType CreateNullableType(Type type, SerializableType argType)
        {
            return new NullableType(type, argType);
        }

        public static SerializableType CreateAmqpSerializableType(AmqpSerializer serializer, Type type)
        {
            return new AmqpSerializableType(serializer, type);
        }

        public static SerializableType CreateAmqpDescribedType(AmqpSerializer serializer, Type type)
        {
            return new AmqpDescribedType(serializer, type);
        }

        public static SerializableType CreateGenericListType(
            AmqpSerializer serializer,
            Type type,
            Type itemType,
            MethodAccessor addAccessor)
        {
            return new GenericListType(serializer, type, itemType, addAccessor);
        }

        public static SerializableType CreateGenericMapType(
            AmqpSerializer serializer,
            Type type,
            MemberAccessor keyAccessor,
            MemberAccessor valueAccessor,
            MethodAccessor addAccessor)
        {
            return new GenericMapType(serializer, type, keyAccessor, valueAccessor, addAccessor);
        }

        public static SerializableType CreateDescribedListType(
            AmqpSerializer serializer,
            Type type,
            SerializableType baseType,
            string descriptorName,
            ulong? descriptorCode,
            SerialiableMember[] members,
            Dictionary<Type, SerializableType> knownTypes,
            MethodAccessor onDesrialized)
        {
            return new DescribedListType(serializer, type, baseType, descriptorName,
                descriptorCode, members, knownTypes, onDesrialized);
        }

        public static SerializableType CreateDescribedMapType(
            AmqpSerializer serializer,
            Type type,
            SerializableType baseType,
            string descriptorName,
            ulong? descriptorCode,
            SerialiableMember[] members,
            Dictionary<Type, SerializableType> knownTypes,
            MethodAccessor onDesrialized)
        {
            return new DescribedMapType(serializer, type, baseType, descriptorName, descriptorCode,
                members, knownTypes, onDesrialized);
        }

        public virtual void ValidateType(SerializableType otherType)
        {
        }

        public abstract void WriteObject(ByteBuffer buffer, object graph);

        public abstract object ReadObject(ByteBuffer buffer);

        sealed class AmqpPrimitiveType : SerializableType
        {
            readonly Encode encoder;
            readonly Decode decoder;

            public AmqpPrimitiveType(Type type, Encode encoder, Decode decoder)
                : base(null, type)
            {
                this.encoder = encoder;
                this.decoder = decoder;
            }

            public override void WriteObject(ByteBuffer buffer, object value)
            {
                this.encoder(buffer, value, true);
            }

            public override object ReadObject(ByteBuffer buffer)
            {
                byte formatCode = Encoder.ReadFormatCode(buffer);
                return this.decoder(buffer, formatCode);
            }
        }

        sealed class AmqpObjectType : SerializableType
        {
            public AmqpObjectType(Type type)
                : base(null, type)
            {
            }

            public override void WriteObject(ByteBuffer buffer, object value)
            {
                Encoder.WriteObject(buffer, value);
            }

            public override object ReadObject(ByteBuffer buffer)
            {
                return Encoder.ReadObject(buffer);
            }
        }

        abstract class AmqpExtendedType : SerializableType
        {
            protected AmqpExtendedType(AmqpSerializer serializer, Type type)
                : base(serializer, type)
            {
            }

            public override void WriteObject(ByteBuffer buffer, object value)
            {
                if (value == null)
                {
                    Encoder.WriteObject(buffer, value);
                }
                else
                {
                    this.EncodeObject(buffer, value);
                }
            }

            public override object ReadObject(ByteBuffer buffer)
            {
                if (this.TryDecodeNull(buffer))
                {
                    return null;
                }

                object container = this.hasDefaultCtor ?
                    Activator.CreateInstance(this.type) :
                    FormatterServices.GetUninitializedObject(this.type);
                this.DecodeObject(buffer, container);
                return container;
            }

            protected bool TryDecodeNull(ByteBuffer buffer)
            {
                buffer.Validate(false, FixedWidth.FormatCode);
                byte formatCode = buffer.Buffer[buffer.Offset];
                if (formatCode == FormatCode.Null)
                {
                    buffer.Complete(FixedWidth.FormatCode);
                    return true;
                }
                else
                {
                    return false;
                }
            }

            protected abstract void EncodeObject(ByteBuffer buffer, object value);

            protected abstract void DecodeObject(ByteBuffer buffer, object value);
        }

        sealed class NullableType : AmqpExtendedType
        {
            SerializableType argType;

            public NullableType(Type type, SerializableType argType)
                : base(null, type)
            {
                this.argType = argType;
            }

            protected override void EncodeObject(ByteBuffer buffer, object value)
            {
                this.argType.WriteObject(buffer, value);
            }

            public override object ReadObject(ByteBuffer buffer)
            {
                if (this.TryDecodeNull(buffer))
                {
                    return null;
                }

                return this.argType.ReadObject(buffer);
            }

            protected override void DecodeObject(ByteBuffer buffer, object value)
            {
                throw new NotImplementedException();
            }
        }

        sealed class AmqpSerializableType : AmqpExtendedType
        {
            public AmqpSerializableType(AmqpSerializer serializer, Type type)
                : base(serializer, type)
            {
            }

            protected override void EncodeObject(ByteBuffer buffer, object value)
            {
                ((IAmqpSerializable)value).Encode(buffer);
            }

            protected override void DecodeObject(ByteBuffer buffer, object value)
            {
                ((IAmqpSerializable)value).Decode(buffer);
            }
        }

        sealed class AmqpDescribedType : AmqpExtendedType
        {
            public AmqpDescribedType(AmqpSerializer serializer, Type type)
                : base(serializer, type)
            {
            }

            protected override void EncodeObject(ByteBuffer buffer, object value)
            {
                ((Described)value).Encode(buffer);
            }

            protected override void DecodeObject(ByteBuffer buffer, object value)
            {
                ((Described)value).Decode(buffer);
            }
        }

        abstract class CollectionType : SerializableType
        {
            protected CollectionType(AmqpSerializer serializer, Type type)
                : base(serializer, type)
            {
            }

            public abstract int WriteMembers(ByteBuffer buffer, object container);

            public abstract void ReadMembers(ByteBuffer buffer, object container, ref int count);

            protected abstract bool WriteFormatCode(ByteBuffer buffer);

            protected abstract void Initialize(ByteBuffer buffer, byte formatCode,
                out int size, out int count, out int encodeWidth, out CollectionType effectiveType);

            public override void WriteObject(ByteBuffer buffer, object graph)
            {
                if (graph == null)
                {
                    Encoder.WriteObject(buffer, null);
                    return;
                }

                if (!this.WriteFormatCode(buffer))
                {
                    return;
                }

                int pos = buffer.WritePos;                  // remember the current position
                AmqpBitConverter.WriteULong(buffer, 0);     // reserve space for size and count

                int count = this.WriteMembers(buffer, graph);

                AmqpBitConverter.WriteInt(buffer.Buffer, pos, buffer.WritePos - pos - FixedWidth.UInt);
                AmqpBitConverter.WriteInt(buffer.Buffer, pos + FixedWidth.UInt, count);
            }

            public override object ReadObject(ByteBuffer buffer)
            {
                byte formatCode = Encoder.ReadFormatCode(buffer);
                if (formatCode == FormatCode.Null)
                {
                    return null;
                }

                int size;
                int count;
                int encodeWidth;
                CollectionType effectiveType;
                this.Initialize(buffer, formatCode, out size, out count, out encodeWidth, out effectiveType);
                int offset = buffer.Offset;

                object container = effectiveType.hasDefaultCtor ?
                    Activator.CreateInstance(effectiveType.type) :
                    FormatterServices.GetUninitializedObject(effectiveType.type);

                if (count > 0)
                {
                    effectiveType.ReadMembers(buffer, container, ref count);

                    if (count > 0)
                    {
                        // skip unknow members
                        buffer.Complete(size - (buffer.Offset - offset) - encodeWidth);
                    }
                }

                return container;
            }

            protected static void ReadSizeAndCount(ByteBuffer buffer, byte formatCode, out int size, out int count, out int width)
            {
                if (formatCode == FormatCode.List0)
                {
                    size = count = width = 0;
                }
                else if (formatCode == FormatCode.List8 || formatCode == FormatCode.Map8)
                {
                    width = FixedWidth.UByte;
                    size = AmqpBitConverter.ReadUByte(buffer);
                    count = AmqpBitConverter.ReadUByte(buffer);
                }
                else if (formatCode == FormatCode.List32 || formatCode == FormatCode.Map32)
                {
                    width = FixedWidth.UInt;
                    size = (int)AmqpBitConverter.ReadUInt(buffer);
                    count = (int)AmqpBitConverter.ReadUInt(buffer);
                }
                else
                {
                    throw new AmqpException(ErrorCode.InvalidField, Fx.Format(SRAmqp.AmqpInvalidFormatCode, formatCode, buffer.Offset));
                }

            }
        }

        sealed class GenericListType : CollectionType
        {
            readonly SerializableType itemType;
            readonly MethodAccessor addMethodAccessor;

            public GenericListType(AmqpSerializer serializer, Type type, Type itemType, MethodAccessor addAccessor)
                : base(serializer, type)
            {
                this.itemType = serializer.GetType(itemType);
                this.addMethodAccessor = addAccessor;
            }

            protected override bool WriteFormatCode(ByteBuffer buffer)
            {
                AmqpBitConverter.WriteUByte(buffer, FormatCode.List32);
                return true;
            }

            public override int WriteMembers(ByteBuffer buffer, object container)
            {
                int count = 0;
                foreach (object item in (IEnumerable)container)
                {
                    if (item == null)
                    {
                        Encoder.WriteObject(buffer, null);
                    }
                    else
                    {
                        SerializableType effectiveType = this.itemType;
                        if (item.GetType() != effectiveType.type)
                        {
                            effectiveType = this.serializer.GetType(item.GetType());
                        }

                        effectiveType.WriteObject(buffer, item);
                    }

                    count++;
                }

                return count;
            }

            protected override void Initialize(ByteBuffer buffer, byte formatCode,
                out int size, out int count, out int encodeWidth, out CollectionType effectiveType)
            {
                effectiveType = this;
                ReadSizeAndCount(buffer, formatCode, out size, out count, out encodeWidth);
            }

            public override void ReadMembers(ByteBuffer buffer, object container, ref int count)
            {
                for (; count > 0; count--)
                {
                    object value = this.itemType.ReadObject(buffer);
                    this.addMethodAccessor.Invoke(container, new object[] { value });
                }
            }
        }

        sealed class GenericMapType : CollectionType
        {
            readonly SerializableType keyType;
            readonly SerializableType valueType;
            readonly MemberAccessor keyAccessor;
            readonly MemberAccessor valueAccessor;
            readonly MethodAccessor addMethodAccessor;

            public GenericMapType(AmqpSerializer serializer, Type type, MemberAccessor keyAccessor,
                MemberAccessor valueAccessor, MethodAccessor addAccessor)
                : base(serializer, type)
            {
                this.keyType = this.serializer.GetType(keyAccessor.Type);
                this.valueType = this.serializer.GetType(valueAccessor.Type);
                this.keyAccessor = keyAccessor;
                this.valueAccessor = valueAccessor;
                this.addMethodAccessor = addAccessor;
            }

            protected override bool WriteFormatCode(ByteBuffer buffer)
            {
                AmqpBitConverter.WriteUByte(buffer, FormatCode.Map32);
                return true;
            }

            public override int WriteMembers(ByteBuffer buffer, object container)
            {
                int count = 0;
                foreach (object item in (IEnumerable)container)
                {
                    object key = this.keyAccessor.Get(item);
                    object value = this.valueAccessor.Get(item);
                    if (value != null)
                    {
                        this.keyType.WriteObject(buffer, key);

                        SerializableType effectiveType = this.valueType;
                        if (value.GetType() != effectiveType.type)
                        {
                            effectiveType = this.serializer.GetType(value.GetType());
                        }

                        effectiveType.WriteObject(buffer, value);

                        count += 2;
                    }
                }
                return count;
            }

            protected override void Initialize(ByteBuffer buffer, byte formatCode,
                out int size, out int count, out int encodeWidth, out CollectionType effectiveType)
            {
                effectiveType = this;
                ReadSizeAndCount(buffer, formatCode, out size, out count, out encodeWidth);
            }

            public override void ReadMembers(ByteBuffer buffer, object container, ref int count)
            {
                for (; count > 0; count -= 2)
                {
                    object key = this.keyType.ReadObject(buffer);
                    object value = this.valueType.ReadObject(buffer);
                    this.addMethodAccessor.Invoke(container, new object[] { key, value });
                }
            }
        }

        abstract class DescribedCompoundType : CollectionType
        {
            readonly DescribedCompoundType baseType;
            readonly Symbol descriptorName;
            readonly ulong? descriptorCode;
            readonly SerialiableMember[] members;
            readonly MethodAccessor onDeserialized;
            readonly KeyValuePair<Type, SerializableType>[] knownTypes;

            protected DescribedCompoundType(
                AmqpSerializer serializer,
                Type type,
                SerializableType baseType,
                string descriptorName,
                ulong? descriptorCode,
                SerialiableMember[] members,
                Dictionary<Type, SerializableType> knownTypes,
                MethodAccessor onDesrialized)
                : base(serializer, type)
            {
                this.baseType = (DescribedCompoundType)baseType;
                this.descriptorName = descriptorName;
                this.descriptorCode = descriptorCode;
                this.members = members;
                this.onDeserialized = onDesrialized;
                this.knownTypes = GetKnownTypes(knownTypes);
            }

            public override SerialiableMember[] Members
            {
                get { return this.members; }
            }

            protected abstract byte Code
            {
                get;
            }

            protected DescribedCompoundType BaseType
            {
                get { return this.baseType; }
            }

            protected override bool WriteFormatCode(ByteBuffer buffer)
            {
                AmqpBitConverter.WriteUByte(buffer, FormatCode.Described);
                if (this.descriptorCode != null)
                {
                    Encoder.WriteULong(buffer, this.descriptorCode.Value, true);
                }
                else
                {
                    Encoder.WriteSymbol(buffer, this.descriptorName, true);
                }

                AmqpBitConverter.WriteUByte(buffer, this.Code);
                return true;
            }

            protected override void Initialize(ByteBuffer buffer, byte formatCode,
                out int size, out int count, out int encodeWidth, out CollectionType effectiveType)
            {
                if (formatCode != FormatCode.Described)
                {
                    throw new AmqpException(ErrorCode.InvalidField, Fx.Format(SRAmqp.AmqpInvalidFormatCode, formatCode, buffer.Offset));
                }

                effectiveType = null;
                formatCode = Encoder.ReadFormatCode(buffer);
                ulong? code = null;
                Symbol symbol = default(Symbol);
                if (formatCode == FormatCode.ULong0)
                {
                    code = 0;
                }
                else if (formatCode == FormatCode.ULong || formatCode == FormatCode.SmallULong)
                {
                    code = Encoder.ReadULong(buffer, formatCode);
                }
                else if (formatCode == FormatCode.Symbol8 || formatCode == FormatCode.Symbol32)
                {
                    symbol = Encoder.ReadSymbol(buffer, formatCode);
                }

                if (this.AreEqual(this.descriptorCode, this.descriptorName, code, symbol))
                {
                    effectiveType = this;
                }
                else if (this.knownTypes != null)
                {
                    for (int i = 0; i < this.knownTypes.Length; ++i)
                    {
                        var kvp = this.knownTypes[i];
                        if (kvp.Value == null)
                        {
                            SerializableType knownType = this.serializer.GetType(kvp.Key);
                            this.knownTypes[i] = kvp = new KeyValuePair<Type, SerializableType>(kvp.Key, knownType);
                        }

                        DescribedCompoundType describedKnownType = (DescribedCompoundType)kvp.Value;
                        if (this.AreEqual(describedKnownType.descriptorCode, describedKnownType.descriptorName, code, symbol))
                        {
                            effectiveType = describedKnownType;
                            break;
                        }
                    }
                }

                if (effectiveType == null)
                {
                    throw new SerializationException(Fx.Format(SRAmqp.AmqpUnknownDescriptor, code != null ? code.ToString() : symbol.ToString(), this.type.Name));
                }

                formatCode = Encoder.ReadFormatCode(buffer);
                ReadSizeAndCount(buffer, formatCode, out size, out count, out encodeWidth);
            }

            protected void InvokeDeserialized(object container)
            {
                if (this.baseType != null)
                {
                    this.baseType.InvokeDeserialized(container);
                }

                if (this.onDeserialized != null)
                {
                    this.onDeserialized.Invoke(container, new object[] { default(StreamingContext) });
                }
            }
            
            static KeyValuePair<Type, SerializableType>[] GetKnownTypes(Dictionary<Type, SerializableType> types)
            {
                if (types == null || types.Count == 0)
                {
                    return null;
                }

                var kt = new KeyValuePair<Type, SerializableType>[types.Count];
                int i = 0;
                foreach (var kvp in types)
                {
                    kt[i++] = kvp;
                }

                return kt;
            }

            bool AreEqual(ulong? code1, Symbol symbol1, ulong? code2, Symbol symbol2)
            {
                if (code1 != null && code2 != null)
                {
                    return code1.Value == code2.Value;
                }

                if (symbol1 != null && symbol2 != null)
                {
                    return string.Equals((string)symbol1, (string)symbol2, StringComparison.Ordinal);
                }

                return false;
            }
        }

        sealed class DescribedListType : DescribedCompoundType
        {
            public DescribedListType(
                AmqpSerializer serializer,
                Type type,
                SerializableType baseType,
                string descriptorName,
                ulong? descriptorCode,
                SerialiableMember[] members,
                Dictionary<Type, SerializableType> knownTypes,
                MethodAccessor onDesrialized)
                : base(serializer, type, baseType, descriptorName, descriptorCode, members, knownTypes, onDesrialized)
            {
            }

            public override EncodingType Encoding
            {
                get
                {
                    return EncodingType.List;
                }
            }

            protected override byte Code
            {
                get { return FormatCode.List32; }
            }

            public override int WriteMembers(ByteBuffer buffer, object container)
            {
                foreach (SerialiableMember member in this.Members)
                {
                    object memberValue = member.Accessor.Get(container);
                    if (memberValue == null)
                    {
                        Encoder.WriteObject(buffer, null);
                    }
                    else
                    {
                        SerializableType effectiveType = member.Type;
                        if (memberValue.GetType() != effectiveType.type)
                        {
                            effectiveType = this.serializer.GetType(memberValue.GetType());
                        }

                        effectiveType.WriteObject(buffer, memberValue);
                    }
                }

                return this.Members.Length;
            }

            public override void ReadMembers(ByteBuffer buffer, object container, ref int count)
            {
                for (int i = 0; i < this.Members.Length && count > 0; ++i, --count)
                {
                    object value = this.Members[i].Type.ReadObject(buffer);
                    this.Members[i].Accessor.Set(container, value);
                }

                this.InvokeDeserialized(container);
            }
        }

        sealed class DescribedMapType : DescribedCompoundType
        {
            readonly Dictionary<string, SerialiableMember> membersMap;

            public DescribedMapType(
                AmqpSerializer serializer,
                Type type,
                SerializableType baseType,
                string descriptorName,
                ulong? descriptorCode,
                SerialiableMember[] members,
                Dictionary<Type, SerializableType> knownTypes,
                MethodAccessor onDesrialized)
                : base(serializer, type, baseType, descriptorName, descriptorCode, members, knownTypes, onDesrialized)
            {
                this.membersMap = new Dictionary<string, SerialiableMember>();
                foreach(SerialiableMember member in members)
                {
                    this.membersMap.Add(member.Name, member);
                }
            }

            public override EncodingType Encoding
            {
                get
                {
                    return EncodingType.Map;
                }
            }

            protected override byte Code
            {
                get { return FormatCode.Map32; }
            }

            public override int WriteMembers(ByteBuffer buffer, object container)
            {
                int count = 0;
                if (this.BaseType != null)
                {
                    this.BaseType.WriteMembers(buffer, container);
                }

                foreach (SerialiableMember member in this.Members)
                {
                    object memberValue = member.Accessor.Get(container);
                    if (memberValue != null)
                    {
                        Encoder.WriteSymbol(buffer, (Symbol)memberValue, true);

                        SerializableType effectiveType = member.Type;
                        if (memberValue.GetType() != effectiveType.type)
                        {
                            effectiveType = this.serializer.GetType(memberValue.GetType());
                        }

                        effectiveType.WriteObject(buffer, memberValue);
                        count += 2;
                    }
                }

                return count;
            }

            public override void ReadMembers(ByteBuffer buffer, object container, ref int count)
            {
                if (this.BaseType != null)
                {
                    this.BaseType.ReadMembers(buffer, container, ref count);
                }

                for (int i = 0; i < this.membersMap.Count && count > 0; ++i, count -= 2)
                {
                    Symbol key = Encoder.ReadSymbol(buffer, Encoder.ReadFormatCode(buffer));
                    SerialiableMember member;
                    if (this.membersMap.TryGetValue((string)key, out member))
                    {
                        object value = member.Type.ReadObject(buffer);
                        member.Accessor.Set(container, value);
                    }
                }

                this.InvokeDeserialized(container);
            }
        }
    }
}
