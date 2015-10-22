//------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------

namespace Microsoft.Azure.Devices.Common.Interop
{
    using System;
    using System.Diagnostics;
    using System.Globalization;
    using System.Runtime.InteropServices;
    using System.Runtime.Versioning;
    using System.Security;
    using Microsoft.Win32.SafeHandles;

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
}
