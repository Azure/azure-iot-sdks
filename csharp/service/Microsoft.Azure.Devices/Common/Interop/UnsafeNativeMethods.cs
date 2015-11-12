//------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------

namespace Microsoft.Azure.Devices.Common.Interop
{
    using System;
    using System.Diagnostics.CodeAnalysis;
    using System.Diagnostics.Eventing;
    using System.Runtime.InteropServices;
    using System.Runtime.Versioning;
    using System.Security;
    using System.Text;
    using System.Threading;
    using Microsoft.Win32.SafeHandles;

    [SuppressUnmanagedCodeSecurity]    
    static class UnsafeNativeMethods
    {
        public const string KERNEL32 = "kernel32.dll";
        public const String ADVAPI32 = "advapi32.dll";
        public const string WS2_32 = "ws2_32.dll";

        public const int ERROR_SUCCESS = 0;
        public const int ERROR_INVALID_HANDLE = 6;
        public const int ERROR_OUTOFMEMORY = 14;
        public const int ERROR_MORE_DATA = 234;
        public const int ERROR_ARITHMETIC_OVERFLOW = 534;
        public const int ERROR_NOT_ENOUGH_MEMORY = 8;
        public const int ERROR_OPERATION_ABORTED = 995;
        public const int ERROR_IO_PENDING = 997;
        public const int ERROR_NO_SYSTEM_RESOURCES = 1450;

        public const int STATUS_PENDING = 0x103;

        // socket errors
        public const int WSAACCESS = 10013;
        public const int WSAEMFILE = 10024;
        public const int WSAEMSGSIZE = 10040;
        public const int WSAEADDRINUSE = 10048;
        public const int WSAEADDRNOTAVAIL = 10049;
        public const int WSAENETDOWN = 10050;
        public const int WSAENETUNREACH = 10051;
        public const int WSAENETRESET = 10052;
        public const int WSAECONNABORTED = 10053;
        public const int WSAECONNRESET = 10054;
        public const int WSAENOBUFS = 10055;
        public const int WSAESHUTDOWN = 10058;
        public const int WSAETIMEDOUT = 10060;
        public const int WSAECONNREFUSED = 10061;
        public const int WSAEHOSTDOWN = 10064;
        public const int WSAEHOSTUNREACH = 10065;

        [StructLayout(LayoutKind.Explicit, Size = 16)]
        public struct EventData
        {
            [FieldOffset(0)]
            internal UInt64 DataPointer;
            [FieldOffset(8)]
            internal uint Size;
            [FieldOffset(12)]
            internal int Reserved;
        }       

        //[SuppressMessage(FxCop.Category.Interoperability, FxCop.Rule.MarkBooleanPInvokeArgumentsWithMarshalAs, Justification = "Opened as CSDMain #183080.")]
        //[SuppressMessage(FxCop.Category.Security, FxCop.Rule.ReviewSuppressUnmanagedCodeSecurityUsage,
        //    Justification = "This PInvoke call has been reviewed")]
        [DllImport(KERNEL32, BestFitMapping = false, CharSet = CharSet.Auto)]
        [ResourceExposure(ResourceScope.Process)]
        [SecurityCritical]
        public static extern SafeWaitHandle CreateWaitableTimer(IntPtr mustBeZero, bool manualReset, string timerName);

        //[SuppressMessage(FxCop.Category.Interoperability, FxCop.Rule.MarkBooleanPInvokeArgumentsWithMarshalAs, Justification = "Opened as CSDMain #183080.")]
        //[SuppressMessage(FxCop.Category.Security, FxCop.Rule.ReviewSuppressUnmanagedCodeSecurityUsage,
        //    Justification = "This PInvoke call has been reviewed")]
        [DllImport(KERNEL32, ExactSpelling = true)]
        [ResourceExposure(ResourceScope.None)]
        [SecurityCritical]
        public static extern bool SetWaitableTimer(SafeWaitHandle handle, ref long dueTime, int period, IntPtr mustBeZero, IntPtr mustBeZeroAlso, bool resume);

        //[SuppressMessage(FxCop.Category.Security, FxCop.Rule.ReviewSuppressUnmanagedCodeSecurityUsage,
        //    Justification = "This PInvoke call has been reviewed")]
        [DllImport(KERNEL32, SetLastError = true)]
        [ResourceExposure(ResourceScope.None)]
        [SecurityCritical]
        public static extern int QueryPerformanceCounter(out long time);

        //[SuppressMessage(FxCop.Category.Security, FxCop.Rule.ReviewSuppressUnmanagedCodeSecurityUsage,
        //    Justification = "This PInvoke call has been reviewed")]
        [DllImport(KERNEL32, SetLastError = false)]
        [ResourceExposure(ResourceScope.None)]
        [SecurityCritical]
        public static extern uint GetSystemTimeAdjustment(
            [Out] out int adjustment,
            [Out] out uint increment,
            [Out] out uint adjustmentDisabled
            );

        [DllImport(KERNEL32, SetLastError = true)]
        [ResourceExposure(ResourceScope.None)]
        [SecurityCritical]
        public static extern void GetSystemTimeAsFileTime([Out] out long time);

        [DllImport(WS2_32, SetLastError = true)]
        internal static unsafe extern int WSARecv
        (
            IntPtr handle, WSABuffer* buffers, int bufferCount, out int bytesTransferred,
            ref int socketFlags,
            NativeOverlapped* nativeOverlapped,
            IntPtr completionRoutine);

        [DllImport(WS2_32, SetLastError = true)]
        internal static unsafe extern bool WSAGetOverlappedResult(
            IntPtr socketHandle,
            NativeOverlapped* overlapped,
            out int bytesTransferred,
            bool wait,
            out uint flags);

        ////[DllImport(KERNEL32, SetLastError = true)]
        ////[return: MarshalAs(UnmanagedType.Bool)]
        ////[ResourceExposure(ResourceScope.None)]
        ////[SecurityCritical]
        ////static extern bool GetComputerNameEx
        ////    (
        ////    [In] ComputerNameFormat nameType,
        ////    [In, Out, MarshalAs(UnmanagedType.LPTStr)] StringBuilder lpBuffer,
        ////    [In, Out] ref int size
        ////    );

        ////[SecurityCritical]
        ////internal static string GetComputerName(ComputerNameFormat nameType)
        ////{
        ////    int length = 0;
        ////    if (!GetComputerNameEx(nameType, null, ref length))
        ////    {
        ////        int error = Marshal.GetLastWin32Error();

        ////        if (error != ERROR_MORE_DATA)
        ////        {
        ////            throw Fx.Exception.AsError(new System.ComponentModel.Win32Exception(error));
        ////        }
        ////    }

        ////    if (length < 0)
        ////    {
        ////        Fx.AssertAndThrow("GetComputerName returned an invalid length: " + length);
        ////    }

        ////    StringBuilder stringBuilder = new StringBuilder(length);
        ////    if (!GetComputerNameEx(nameType, stringBuilder, ref length))
        ////    {
        ////        int error = Marshal.GetLastWin32Error();
        ////        throw Fx.Exception.AsError(new System.ComponentModel.Win32Exception(error));
        ////    }

        ////    return stringBuilder.ToString();
        ////}

        [DllImport(KERNEL32)]
        [ResourceExposure(ResourceScope.None)]
        [SecurityCritical]
        internal static extern bool IsDebuggerPresent();

        [DllImport(KERNEL32)]
        [ResourceExposure(ResourceScope.Process)]
        [SecurityCritical]
        internal static extern void DebugBreak();

        [DllImport(KERNEL32, CharSet = CharSet.Unicode)]
        [ResourceExposure(ResourceScope.Process)]
        [SecurityCritical]
        internal static extern void OutputDebugString(string lpOutputString);

        //
        // Callback
        //
        [SecurityCritical]
        internal unsafe delegate void EtwEnableCallback(
            [In] ref Guid sourceId,
            [In] int isEnabled,
            [In] byte level,
            [In] long matchAnyKeywords,
            [In] long matchAllKeywords,
            [In] void* filterData,
            [In] void* callbackContext
            );

        [DllImport(ADVAPI32, ExactSpelling = true, EntryPoint = "EventRegister", CharSet = System.Runtime.InteropServices.CharSet.Unicode)]
        [SecurityCritical]
        internal static extern unsafe uint EventRegister(
                    [In] ref Guid providerId,
                    [In]EtwEnableCallback enableCallback,
                    [In]void* callbackContext,
                    [In][Out]ref long registrationHandle
                    );

        [DllImport(ADVAPI32, ExactSpelling = true, EntryPoint = "EventUnregister", CharSet = System.Runtime.InteropServices.CharSet.Unicode)]
        [SecurityCritical]
        internal static extern uint EventUnregister([In] long registrationHandle);

        [DllImport(ADVAPI32, ExactSpelling = true, EntryPoint = "EventWrite", CharSet = System.Runtime.InteropServices.CharSet.Unicode)]
        [SecurityCritical]
        internal static extern unsafe uint EventWrite(
                [In] long registrationHandle,
                [In] ref EventDescriptor eventDescriptor,
                [In] uint userDataCount,
                [In] EventData* userData
                );

        [DllImport(ADVAPI32, ExactSpelling = true, EntryPoint = "EventWriteTransfer", CharSet = System.Runtime.InteropServices.CharSet.Unicode)]
        [SecurityCritical]
        internal static extern unsafe uint EventWriteTransfer(
                [In] long registrationHandle,
                [In] ref EventDescriptor eventDescriptor,
                [In] ref Guid activityId,
                [In] ref Guid relatedActivityId,
                [In] uint userDataCount,
                [In] EventData* userData
                );

        [DllImport(ADVAPI32, ExactSpelling = true, EntryPoint = "EventWriteString", CharSet = System.Runtime.InteropServices.CharSet.Unicode)]
        [SecurityCritical]
        internal static extern unsafe uint EventWriteString(
                [In] long registrationHandle,
                [In] byte level,
                [In] long keywords,
                [In] char* message
                );

        [DllImport(ADVAPI32, ExactSpelling = true, EntryPoint = "EventActivityIdControl", CharSet = System.Runtime.InteropServices.CharSet.Unicode)]
        [SecurityCritical]
        internal static extern unsafe uint EventActivityIdControl([In] int ControlCode, [In][Out] ref Guid ActivityId);

        [DllImport(ADVAPI32, CharSet = System.Runtime.InteropServices.CharSet.Unicode, SetLastError = true)]
        [ResourceExposure(ResourceScope.None)]
        [SecurityCritical]
        internal static extern bool ReportEvent(SafeHandle hEventLog, ushort type, ushort category,
                                                uint eventID, byte[] userSID, ushort numStrings, uint dataLen, HandleRef strings,
                                                byte[] rawData);

        [DllImport(ADVAPI32, CharSet = System.Runtime.InteropServices.CharSet.Unicode, SetLastError = true)]
        [ResourceExposure(ResourceScope.Machine)]
        [SecurityCritical]
        internal static extern SafeEventLogWriteHandle RegisterEventSource(string uncServerName, string sourceName);

        // NOTE: a macro in win32
        internal unsafe static bool HasOverlappedIoCompleted(
            NativeOverlapped* overlapped)
        {
            return overlapped->InternalLow != (IntPtr)STATUS_PENDING;
        }

        [StructLayout(LayoutKind.Sequential)]
        internal struct WSABuffer
        {
            public int length;
            public IntPtr buffer;
        }
    }
}
