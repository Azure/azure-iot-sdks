// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client
{
    using System;
    using System.Reflection;
    using System.Runtime.CompilerServices;
    using System.Security;
#if !WINDOWS_UWP
    using System.Security.Permissions;
#endif

    static class PartialTrustHelpers
    {
#if !WINDOWS_UWP
        [Fx.Tag.SecurityNote(Critical = "used in a security-sensitive decision")]
        [SecurityCritical]
        static Type aptca;
#endif
        internal static bool ShouldFlowSecurityContext
        {
            [Fx.Tag.SecurityNote(Critical = "used in a security-sensitive decision")]
            [SecurityCritical]
            get
            {
#if WINDOWS_UWP
                throw new NotImplementedException();
#else
                if (AppDomain.CurrentDomain.IsHomogenous)
                {
                    return false;
                }

                return SecurityManager.CurrentThreadRequiresSecurityContextCapture();
#endif
            }
        }

        [Fx.Tag.SecurityNote(Critical = "used in a security-sensitive decision")]
        [SecurityCritical]
        internal static bool IsInFullTrust()
        {
#if WINDOWS_UWP
            throw new NotImplementedException();
#else
            if (AppDomain.CurrentDomain.IsHomogenous)
            {
                return AppDomain.CurrentDomain.IsFullyTrusted;
            }
            else
            {
                if (!SecurityManager.CurrentThreadRequiresSecurityContextCapture())
                {
                    return true;
                }

                try
                {
                    DemandForFullTrust();
                    return true;
                }
                catch (SecurityException)
                {
                    return false;
                }
            }
#endif
        }

        [Fx.Tag.SecurityNote(Critical = "used in a security-sensitive decision")]
        [SecurityCritical]
        internal static bool UnsafeIsInFullTrust()
        {
#if WINDOWS_UWP
            throw new NotImplementedException();
#else
            if (AppDomain.CurrentDomain.IsHomogenous)
            {
                return AppDomain.CurrentDomain.IsFullyTrusted;
            }
            else
            {
                return !SecurityManager.CurrentThreadRequiresSecurityContextCapture();
            }
#endif
        }

#if !WINDOWS_UWP
        [Fx.Tag.SecurityNote(Critical = "Captures security context with identity flow suppressed, " +
            "this requires satisfying a LinkDemand for infrastructure.")]
        [SecurityCritical]
        internal static SecurityContext CaptureSecurityContextNoIdentityFlow()
        {
            // capture the security context but never flow windows identity
            if (SecurityContext.IsWindowsIdentityFlowSuppressed())
            {
                return SecurityContext.Capture();
            }
            else
            {
                using (SecurityContext.SuppressFlowWindowsIdentity())
                {
                    return SecurityContext.Capture();
                }
            }
        }
#endif
        [Fx.Tag.SecurityNote(Critical = "used in a security-sensitive decision")]
        [SecurityCritical]
        internal static bool IsTypeAptca(Type type)
        {
#if WINDOWS_UWP
            throw new NotImplementedException();
#else
            Assembly assembly = type.Assembly;
            return IsAssemblyAptca(assembly) || !IsAssemblySigned(assembly);
#endif
        }

        [Fx.Tag.SecurityNote(Critical = "used in a security-sensitive decision")]
        [SecurityCritical]
#if !WINDOWS_UWP
        [PermissionSet(SecurityAction.Demand, Name = "FullTrust")]
#endif
        [MethodImpl(MethodImplOptions.NoInlining)]
        static void DemandForFullTrust()
        {
        }

        [Fx.Tag.SecurityNote(Critical = "used in a security-sensitive decision")]
        [SecurityCritical]
        static bool IsAssemblyAptca(Assembly assembly)
        {
#if WINDOWS_UWP
            throw new NotImplementedException();
#else
            if (aptca == null)
            {
                aptca = typeof(AllowPartiallyTrustedCallersAttribute);
            }
            return assembly.GetCustomAttributes(aptca, false).Length > 0;
#endif
        }

        [Fx.Tag.SecurityNote(Critical = "used in a security-sensitive decision")]
        [SecurityCritical]
#if !WINDOWS_UWP
        [FileIOPermission(SecurityAction.Assert, Unrestricted = true)]
#endif
        static bool IsAssemblySigned(Assembly assembly)
        {
            byte[] publicKeyToken = assembly.GetName().GetPublicKeyToken();
            return publicKeyToken != null & publicKeyToken.Length > 0;
        }

#if !WINDOWS_UWP
        [Fx.Tag.SecurityNote(Critical = "used in a security-sensitive decision")]
        [SecurityCritical]
        internal static bool CheckAppDomainPermissions(PermissionSet permissions)
        {

            return AppDomain.CurrentDomain.IsHomogenous &&
                   permissions.IsSubsetOf(AppDomain.CurrentDomain.PermissionSet);
        }
#endif

        [Fx.Tag.SecurityNote(Critical = "used in a security-sensitive decision")]
        [SecurityCritical]
        internal static bool HasEtwPermissions()
        {
#if WINDOWS_UWP
            throw new NotImplementedException();
#else
            //Currently unrestricted permissions are required to create Etw provider. 
            PermissionSet permissions = new PermissionSet(PermissionState.Unrestricted);
            return CheckAppDomainPermissions(permissions);
#endif
        }

    }
}
