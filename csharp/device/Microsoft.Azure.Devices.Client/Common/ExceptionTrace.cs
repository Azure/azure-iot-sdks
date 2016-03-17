// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client
{
    using System;
    using System.Diagnostics;
    using System.Reflection;
    using System.Diagnostics.CodeAnalysis;
    using System.Globalization;
    using System.Runtime.CompilerServices;
    using System.Runtime.Versioning;
    using System.Threading;
    using Microsoft.Azure.Devices.Client.Extensions;

    class ExceptionTrace
    {
        const ushort FailFastEventLogCategory = 6;
        readonly string eventSourceName;

        public ExceptionTrace(string eventSourceName)
        {
            this.eventSourceName = eventSourceName;
        }

        public Exception AsError(Exception exception, EventTraceActivity activity = null)
        {
            return TraceException<Exception>(exception, TraceEventType.Error, activity);
        }

        public Exception AsInformation(Exception exception, EventTraceActivity activity = null)
        {
            return TraceException<Exception>(exception, TraceEventType.Information, activity);
        }

        public Exception AsWarning(Exception exception, EventTraceActivity activity = null)
        {
            return TraceException<Exception>(exception, TraceEventType.Warning, activity);
        }

        public Exception AsVerbose(Exception exception, EventTraceActivity activity = null)
        {
            return TraceException<Exception>(exception, TraceEventType.Verbose, activity);
        }

        public ArgumentException Argument(string paramName, string message)
        {
            return TraceException<ArgumentException>(new ArgumentException(message, paramName), TraceEventType.Error);
        }

        public ArgumentNullException ArgumentNull(string paramName)
        {
            return TraceException<ArgumentNullException>(new ArgumentNullException(paramName), TraceEventType.Error);
        }

        public ArgumentNullException ArgumentNull(string paramName, string message)
        {
            return TraceException<ArgumentNullException>(new ArgumentNullException(paramName, message), TraceEventType.Error);
        }

        public ArgumentOutOfRangeException ArgumentOutOfRange(string paramName, object actualValue, string message)
        {
            return TraceException<ArgumentOutOfRangeException>(new ArgumentOutOfRangeException(paramName, actualValue, message), TraceEventType.Error);
        }

        // When throwing ObjectDisposedException, it is highly recommended that you use this ctor
        // [C#]
        // public ObjectDisposedException(string objectName, string message);
        // And provide null for objectName but meaningful and relevant message for message. 
        // It is recommended because end user really does not care or can do anything on the disposed object, commonly an internal or private object.
        public ObjectDisposedException ObjectDisposed(string message)
        {
            // pass in null, not disposedObject.GetType().FullName as per the above guideline
            return TraceException<ObjectDisposedException>(new ObjectDisposedException(null, message), TraceEventType.Error);
        }

        public void TraceHandled(Exception exception, string catchLocation, EventTraceActivity activity = null)
        {
#if !WINDOWS_UWP && !PCL // No Trace in UWP. Consider Debug.WriteLine
#if DEBUG
            Trace.WriteLine(string.Format(
                CultureInfo.InvariantCulture,
                "IotHub/TraceHandled ThreadID=\"{0}\" catchLocation=\"{1}\" exceptionType=\"{2}\" exception=\"{3}\"",
                Thread.CurrentThread.ManagedThreadId,
                catchLocation,
                exception.GetType(),
                exception.ToStringSlim()));
#endif
#endif
            ////MessagingClientEtwProvider.Provider.HandledExceptionWithFunctionName(
            ////    activity, catchLocation, exception.ToStringSlim(), string.Empty);

            this.BreakOnException(exception);
        }

        public void TraceUnhandled(Exception exception)
        {
            ////MessagingClientEtwProvider.Provider.EventWriteUnhandledException(this.eventSourceName + ": " + exception.ToStringSlim());
        }

#if !WINDOWS_UWP && !PCL // attribute does not exist in UWP
        [ResourceConsumption(ResourceScope.Process)]
#endif
        [Fx.Tag.SecurityNote(Critical = "Calls 'System.Runtime.Interop.UnsafeNativeMethods.IsDebuggerPresent()' which is a P/Invoke method",
            Safe = "Does not leak any resource, needed for debugging")]
#if !PCL
        public TException TraceException<TException>(TException exception, TraceEventType level, EventTraceActivity activity = null)
#else
        public TException TraceException<TException>(TException exception, Microsoft.Azure.Devices.Client.TraceEventType level, EventTraceActivity activity = null)
#endif
            where TException : Exception
        {

            if (!exception.Data.Contains(this.eventSourceName))
            {
                // Only trace if this is the first time an exception is thrown by this ExceptionTrace/EventSource.
                exception.Data[this.eventSourceName] = this.eventSourceName;

                switch (level)
                {
                    case TraceEventType.Critical:
                    case TraceEventType.Error:
#if !WINDOWS_UWP && !PCL
                        Trace.TraceError("An Exception is being thrown: {0}", GetDetailsForThrownException(exception));
#endif
                        ////if (MessagingClientEtwProvider.Provider.IsEnabled(
                        ////        EventLevel.Error,
                        ////        MessagingClientEventSource.Keywords.Client,
                        ////        MessagingClientEventSource.Channels.DebugChannel))
                        ////{
                        ////    MessagingClientEtwProvider.Provider.ThrowingExceptionError(activity, GetDetailsForThrownException(exception));
                        ////}
                         
                        break;
                    case TraceEventType.Warning:
#if !WINDOWS_UWP && !PCL
                        Trace.TraceWarning("An Exception is being thrown: {0}", GetDetailsForThrownException(exception));
#endif
                        ////if (MessagingClientEtwProvider.Provider.IsEnabled(
                        ////        EventLevel.Warning,
                        ////        MessagingClientEventSource.Keywords.Client,
                        ////        MessagingClientEventSource.Channels.DebugChannel))
                        ////{
                        ////    MessagingClientEtwProvider.Provider.ThrowingExceptionWarning(activity, GetDetailsForThrownException(exception));
                        ////}
                       
                        break;
                    default:
#if DEBUG
                        ////if (MessagingClientEtwProvider.Provider.IsEnabled(
                        ////        EventLevel.Verbose,
                        ////        MessagingClientEventSource.Keywords.Client,
                        ////        MessagingClientEventSource.Channels.DebugChannel))
                        ////{
                        ////    MessagingClientEtwProvider.Provider.ThrowingExceptionVerbose(activity, GetDetailsForThrownException(exception));
                        ////}
#endif

                        break;
                }
            }

            BreakOnException(exception);
            return exception;
        }

        public static string GetDetailsForThrownException(Exception e)
        {
            string details = e.GetType().ToString();

#if !WINDOWS_UWP && !PCL
            const int MaxStackFrames = 10;
            // Include the current callstack (this ensures we see the Stack in case exception is not output when caught)
            var stackTrace = new StackTrace();
            string stackTraceString = stackTrace.ToString();
            if (stackTrace.FrameCount > MaxStackFrames)
            {
                string[] frames = stackTraceString.Split(new[] { Environment.NewLine }, MaxStackFrames + 1, StringSplitOptions.RemoveEmptyEntries);
                stackTraceString = string.Join(Environment.NewLine, frames, 0, MaxStackFrames) + "...";
            }

            details += Environment.NewLine + stackTraceString;
#endif
            details += Environment.NewLine + "Exception ToString:" + Environment.NewLine;
            details += e.ToStringSlim();
            return details;
        }

        [SuppressMessage(FxCop.Category.Performance, FxCop.Rule.MarkMembersAsStatic, Justification = "CSDMain #183668")]
        [Fx.Tag.SecurityNote(Critical = "Calls into critical method UnsafeNativeMethods.IsDebuggerPresent and UnsafeNativeMethods.DebugBreak",
            Safe = "Safe because it's a no-op in retail builds.")]
        internal void BreakOnException(Exception exception)
        {
#if DEBUG
            if (Fx.BreakOnExceptionTypes != null)
            {
                foreach (Type breakType in Fx.BreakOnExceptionTypes)
                {
#if WINDOWS_UWP
                    if (breakType.GetTypeInfo().IsAssignableFrom(exception.GetType().GetTypeInfo()))
                    {
                        Debugger.Launch();
                    }
#elif !PCL
                    if (breakType.IsAssignableFrom(exception.GetType()))
                    {
                        // This is intended to "crash" the process so that a debugger can be attached.  If a managed
                        // debugger is already attached, it will already be able to hook these exceptions.  We don't
                        // want to simulate an unmanaged crash (DebugBreak) in that case.
                        if (!Debugger.IsAttached && !UnsafeNativeMethods.IsDebuggerPresent())
                        {
                            Debugger.Launch();
                        }
                    }
#endif
                }
            }
#endif
        }

        [MethodImpl(MethodImplOptions.NoInlining)]
        public void TraceFailFast(string message)
        {
////            EventLogger logger = null;
////#pragma warning disable 618
////            logger = new EventLogger(this.eventSourceName, Fx.Trace);
////#pragma warning restore 618
////            TraceFailFast(message, logger);
        }

        // Generate an event Log entry for failfast purposes
        // To force a Watson on a dev machine, do the following:
        // 1. Set \HKLM\SOFTWARE\Microsoft\PCHealth\ErrorReporting ForceQueueMode = 0 
        // 2. In the command environment, set COMPLUS_DbgJitDebugLaunchSetting=0
        ////[SuppressMessage(FxCop.Category.Performance, FxCop.Rule.MarkMembersAsStatic, Justification = "CSDMain #183668")]
        ////[MethodImpl(MethodImplOptions.NoInlining)]
        ////internal void TraceFailFast(string message, EventLogger logger)
        ////{
        ////    if (logger != null)
        ////    {
        ////        try
        ////        {
        ////            string stackTrace = null;
        ////            try
        ////            {
        ////                stackTrace = new StackTrace().ToString();
        ////            }
        ////            catch (Exception exception)
        ////            {
        ////                stackTrace = exception.Message;
        ////                if (Fx.IsFatal(exception))
        ////                {
        ////                    throw;
        ////                }
        ////            }
        ////            finally
        ////            {
        ////                logger.LogEvent(TraceEventType.Critical,
        ////                    FailFastEventLogCategory,
        ////                    (uint)EventLogEventId.FailFast,
        ////                    message,
        ////                    stackTrace);
        ////            }
        ////        }
        ////        catch (Exception ex)
        ////        {
        ////            logger.LogEvent(TraceEventType.Critical,
        ////                FailFastEventLogCategory,
        ////                (uint)EventLogEventId.FailFastException,
        ////                ex.ToString());
        ////            if (Fx.IsFatal(ex))
        ////            {
        ////                throw;
        ////            }
        ////        }
        ////    }
        ////}
    }
}
