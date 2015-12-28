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
    using System.Reflection;
    using System.Reflection.Emit;
    using System.Runtime.Serialization;

    abstract class MemberAccessor
    {
        readonly Type type;
        Func<object, object> getter;
        Action<object, object> setter;

        protected MemberAccessor(Type type)
        {
            this.type = type;
        }

        public Type Type
        {
            get { return this.type; }
        }

        public static MemberAccessor Create(MemberInfo memberInfo, bool requiresSetter)
        {
            if (memberInfo.MemberType == MemberTypes.Field)
            {
                return new FieldMemberAccessor((FieldInfo)memberInfo);
            }
            else if (memberInfo.MemberType == MemberTypes.Property)
            {
                return new PropertyMemberAccessor((PropertyInfo)memberInfo, requiresSetter);
            }

            throw new NotSupportedException(memberInfo.MemberType.ToString());
        }

        public object Get(object container)
        {
            return this.getter(container);
        }

        public void Set(object container, object value)
        {
            this.setter(container, value);
        }

        static void EmitTypeConversion(ILGenerator generator, Type castType, bool isContainer)
        {
            if (castType == typeof(object))
            {
            }
            else if (castType.IsValueType)
            {
                generator.Emit(isContainer ? OpCodes.Unbox : OpCodes.Unbox_Any, castType);
            }
            else
            {
                generator.Emit(OpCodes.Castclass, castType);
            }
        }

        static void EmitCall(ILGenerator generator, MethodInfo method)
        {
            OpCode opcode = (method.IsStatic || method.DeclaringType.IsValueType) ? OpCodes.Call : OpCodes.Callvirt;
            generator.EmitCall(opcode, method, null);
        }

        static string GetAccessorName(bool isGetter, string name)
        {
            return (isGetter ? "get_" : "set_") + name;
        }

        sealed class FieldMemberAccessor : MemberAccessor
        {
            public FieldMemberAccessor(FieldInfo fieldInfo)
                : base(fieldInfo.FieldType)
            {
                this.InitializeGetter(fieldInfo);
                this.InitializeSetter(fieldInfo);
            }

            void InitializeGetter(FieldInfo fieldInfo)
            {
                DynamicMethod method = new DynamicMethod(GetAccessorName(true, fieldInfo.Name), typeof(object), new[] { typeof(object) }, true);
                ILGenerator generator = method.GetILGenerator();
                generator.Emit(OpCodes.Ldarg_0);
                EmitTypeConversion(generator, fieldInfo.DeclaringType, true);
                generator.Emit(OpCodes.Ldfld, fieldInfo);
                if (fieldInfo.FieldType.IsValueType)
                {
                    generator.Emit(OpCodes.Box, fieldInfo.FieldType);
                }

                generator.Emit(OpCodes.Ret);

                this.getter = (Func<object, object>)method.CreateDelegate(typeof(Func<object, object>));
            }

            void InitializeSetter(FieldInfo fieldInfo)
            {
                DynamicMethod method = new DynamicMethod(GetAccessorName(false, fieldInfo.Name), typeof(void), new[] { typeof(object), typeof(object) }, true);
                ILGenerator generator = method.GetILGenerator();
                generator.Emit(OpCodes.Ldarg_0);
                EmitTypeConversion(generator, fieldInfo.DeclaringType, true);
                generator.Emit(OpCodes.Ldarg_1);
                EmitTypeConversion(generator, fieldInfo.FieldType, false);
                generator.Emit(OpCodes.Stfld, fieldInfo);
                generator.Emit(OpCodes.Ret);

                this.setter = (Action<object, object>)method.CreateDelegate(typeof(Action<object, object>));
            }
        }

        sealed class PropertyMemberAccessor : MemberAccessor
        {
            public PropertyMemberAccessor(PropertyInfo propertyInfo, bool requiresSetter)
                : base(propertyInfo.PropertyType)
            {
                this.InitializeGetter(propertyInfo);
                this.InitializeSetter(propertyInfo, requiresSetter);
            }

            void InitializeGetter(PropertyInfo propertyInfo)
            {
                DynamicMethod method = new DynamicMethod(GetAccessorName(true, propertyInfo.Name), typeof(object), new[] { typeof(object) }, true);
                ILGenerator generator = method.GetILGenerator();
                generator.DeclareLocal(typeof(object)); 
                generator.Emit(OpCodes.Ldarg_0);
                EmitTypeConversion(generator, propertyInfo.DeclaringType, true);
                EmitCall(generator, propertyInfo.GetGetMethod(true));
                if (propertyInfo.PropertyType.IsValueType)
                {
                    generator.Emit(OpCodes.Box, propertyInfo.PropertyType);
                }

                generator.Emit(OpCodes.Ret);

                this.getter = (Func<object, object>)method.CreateDelegate(typeof(Func<object, object>));
            }

            void InitializeSetter(PropertyInfo propertyInfo, bool requiresSetter)
            {
                MethodInfo setMethod = propertyInfo.GetSetMethod(true);
                if (setMethod == null)
                {
                    if (requiresSetter)
                    {
                        throw new SerializationException("Property annotated with AmqpMemberAttribute must have a setter.");
                    }
                    else
                    {
                        return;
                    }
                }

                DynamicMethod method = new DynamicMethod(GetAccessorName(false, propertyInfo.Name), typeof(void), new[] { typeof(object), typeof(object) }, true);
                ILGenerator generator = method.GetILGenerator();
                generator.Emit(OpCodes.Ldarg_0);
                EmitTypeConversion(generator, propertyInfo.DeclaringType, true);
                generator.Emit(OpCodes.Ldarg_1);
                EmitTypeConversion(generator, propertyInfo.PropertyType, false);
                EmitCall(generator, setMethod);
                generator.Emit(OpCodes.Ret);

                this.setter = (Action<object, object>)method.CreateDelegate(typeof(Action<object, object>));
            }
        }
    }
}
