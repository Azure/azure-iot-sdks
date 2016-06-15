// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Common
{
    using System;
    using System.Reflection;

#if !WINDOWS_UWP
    /// <summary>
    /// Extended reflection support
    /// </summary>
    public static class MemberInfoExtensions
    {
        /// <summary>
        /// Given a member reference, find its fully qualified name (assembly, class, member)
        /// </summary>
        /// <param name="member">The member whose name is desired</param>
        /// <returns>The fully qualified member name, including assembly, namespace, class, and member</returns>
        public static string GetFullyQualifiedMemberName(this MemberInfo member)
        {
            if (member == null)
            {
                throw new ArgumentNullException("member");
            }

            Type type = member.DeclaringType;
            Assembly assembly = type.Assembly;
            string assemblyName = assembly.ManifestModule.Name;
            return "{0}!{1}.{2}".FormatInvariant(assemblyName, type.FullName, member.Name);
        }
    }
#endif
}
