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

    abstract class MethodAccessor
    {
        delegate object MethodDelegate(object container, object[] parameters);

        static readonly Type[] delegateParamsType = { typeof(object), typeof(object[]) };
        bool isStatic;
        MethodDelegate methodDelegate;

        public static MethodAccessor Create(MethodInfo methodInfo)
        {
            return new TypeMethodAccessor(methodInfo);
        }

        public static MethodAccessor Create(ConstructorInfo constructorInfo)
        {
            return new ConstructorAccessor(constructorInfo);
        }

        public object Invoke(object[] parameters)
        {
            if (!this.isStatic)
            {
                throw new InvalidOperationException("Instance required to call an instance method.");
            }

            return this.Invoke(null, parameters);
        }

        public object Invoke(object container, object[] parameters)
        {
            if (this.isStatic && container != null)
            {
                throw new InvalidOperationException("Static method must be called with null instance.");
            }

            return this.methodDelegate(container, parameters);
        }

        Type[] GetParametersType(ParameterInfo[] paramsInfo)
        {
            Type[] paramsType = new Type[paramsInfo.Length];
            for (int i = 0; i < paramsInfo.Length; i++)
            {
                paramsType[i] = paramsInfo[i].ParameterType.IsByRef ? paramsInfo[i].ParameterType.GetElementType() : paramsInfo[i].ParameterType;
            }

            return paramsType;
        }

        void LoadArguments(ILGenerator generator, Type[] paramsType)
        {
            for (int i = 0; i < paramsType.Length; i++)
            {
                generator.Emit(OpCodes.Ldarg_1);
                switch (i)
                {
                    case 0: generator.Emit(OpCodes.Ldc_I4_0); break;
                    case 1: generator.Emit(OpCodes.Ldc_I4_1); break;
                    case 2: generator.Emit(OpCodes.Ldc_I4_2); break;
                    case 3: generator.Emit(OpCodes.Ldc_I4_3); break;
                    case 4: generator.Emit(OpCodes.Ldc_I4_4); break;
                    case 5: generator.Emit(OpCodes.Ldc_I4_5); break;
                    case 6: generator.Emit(OpCodes.Ldc_I4_6); break;
                    case 7: generator.Emit(OpCodes.Ldc_I4_7); break;
                    case 8: generator.Emit(OpCodes.Ldc_I4_8); break;
                    default: generator.Emit(OpCodes.Ldc_I4, i); break;
                }
                generator.Emit(OpCodes.Ldelem_Ref);
                if (paramsType[i].IsValueType)
                {
                    generator.Emit(OpCodes.Unbox_Any, paramsType[i]);
                }
                else if (paramsType[i] != typeof(object))
                {
                    generator.Emit(OpCodes.Castclass, paramsType[i]);
                }
            }
        }

        sealed class ConstructorAccessor : MethodAccessor
        {
            public ConstructorAccessor(ConstructorInfo constructorInfo)
            {
                this.isStatic = true;
                DynamicMethod method = new DynamicMethod("ctor_" + constructorInfo.DeclaringType.Name, typeof(object), delegateParamsType, true);
                Type[] paramsType = this.GetParametersType(constructorInfo.GetParameters());
                ILGenerator generator = method.GetILGenerator();
                this.LoadArguments(generator, paramsType);
                generator.Emit(OpCodes.Newobj, constructorInfo);
                if (constructorInfo.DeclaringType.IsValueType)
                {
                    generator.Emit(OpCodes.Box, constructorInfo.DeclaringType);
                }
                generator.Emit(OpCodes.Ret);

                this.methodDelegate = (MethodDelegate)method.CreateDelegate(typeof(MethodDelegate));
            }
        }

        sealed class TypeMethodAccessor : MethodAccessor
        {
            public TypeMethodAccessor(MethodInfo methodInfo)
            {
                Type[] paramsType = this.GetParametersType(methodInfo.GetParameters());
                DynamicMethod method = new DynamicMethod("method_" + methodInfo.Name, typeof(object), delegateParamsType, true);
                ILGenerator generator = method.GetILGenerator();
                if (!this.isStatic)
                {
                    generator.Emit(OpCodes.Ldarg_0);
                    if (methodInfo.DeclaringType.IsValueType)
                    {
                        generator.Emit(OpCodes.Unbox_Any, methodInfo.DeclaringType);
                    }
                    else
                    {
                        generator.Emit(OpCodes.Castclass, methodInfo.DeclaringType);
                    }
                }
                this.LoadArguments(generator, paramsType);
                if (methodInfo.IsFinal)
                {
                    generator.Emit(OpCodes.Call, methodInfo);
                }
                else
                {
                    generator.Emit(OpCodes.Callvirt, methodInfo);
                }

                if (methodInfo.ReturnType == typeof(void))
                {
                    generator.Emit(OpCodes.Ldnull);
                }
                else if (methodInfo.ReturnType.IsValueType)
                {
                    generator.Emit(OpCodes.Box, methodInfo.ReturnType);
                }

                generator.Emit(OpCodes.Ret);

                this.methodDelegate = (MethodDelegate)method.CreateDelegate(typeof(MethodDelegate));
            }
        }
    }
}
