// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client
{
    using System;
    using System.Diagnostics;
    using System.Globalization;
    using System.Runtime.InteropServices;
    using System.Runtime.Versioning;
    using System.Security;
    using Microsoft.Win32.SafeHandles;

#if !WINDOWS_UWP
    [SecurityCritical]
    sealed class SafeEventLogWriteHandle : SafeHandleZeroOrMinusOneIsInvalid
    {
        // Note: RegisterEventSource returns 0 on failure
        [SecurityCritical]
        SafeEventLogWriteHandle() : base(true) { }

        [ResourceConsumption(ResourceScope.Machine)]
        [SecurityCritical]
        public static SafeEventLogWriteHandle RegisterEventSource(string uncServerName, string sourceName)
        {
            SafeEventLogWriteHandle retval = UnsafeNativeMethods.RegisterEventSource(uncServerName, sourceName);
            int error = Marshal.GetLastWin32Error();
            if (retval.IsInvalid)
            {
                Debug.Print("SafeEventLogWriteHandle::RegisterEventSource[" + uncServerName + ", " + sourceName + "] Failed. Last Error: " +
                    error.ToString(CultureInfo.InvariantCulture));
            }

            return retval;
        }

        [DllImport("advapi32", SetLastError = true)]
        [ResourceExposure(ResourceScope.None)]
        static extern bool DeregisterEventSource(IntPtr hEventLog);

        [SecurityCritical]
        protected override bool ReleaseHandle()
        {
            return DeregisterEventSource(this.handle);
        }
    }
#endif
}
