// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client
{
    using System;
    using System.Collections.Generic;
    using System.Collections.ObjectModel;
    using System.Diagnostics;
    using System.Diagnostics.CodeAnalysis;
    using System.Globalization;
    using System.Reflection;
    using System.Runtime.CompilerServices;
#if !WINDOWS_UWP && !PCL
    using System.Runtime.ConstrainedExecution;
#endif
    using System.Runtime.InteropServices;
    using System.Security;
    using System.Threading;
#if !WINDOWS_UWP && !PCL
    using System.Transactions;
#endif
#if !PCL
    using Microsoft.Win32;
#endif
    using Microsoft.Azure.Devices.Client.Exceptions;

    static class Fx
    {
        // This is only used for EventLog Source therefore matching EventLog source rather than ETL source
        const string DefaultEventSource = "Microsoft.IotHub";

#if DEBUG
        const string SBRegistryKey = @"SOFTWARE\Microsoft\IotHub\v2.0";
        const string AssertsFailFastName = "AssertsFailFast";
        const string BreakOnExceptionTypesName = "BreakOnExceptionTypes";
        const string FastDebugName = "FastDebug";

        static bool breakOnExceptionTypesRetrieved;
        static Type[] breakOnExceptionTypesCache;
#endif

#if UNUSED
        [Fx.Tag.SecurityNote(Critical = "This delegate is called from within a ConstrainedExecutionRegion, must not be settable from PT code")]
        [SecurityCritical]
        static ExceptionHandler asynchronousThreadExceptionHandler;
#endif // UNUSED

        static ExceptionTrace exceptionTrace;

        ////static DiagnosticTrace diagnosticTrace;

        ////[Fx.Tag.SecurityNote(Critical = "Accesses SecurityCritical field EtwProvider",
        ////    Safe = "Doesn't leak info\\resources")]
        ////[SuppressMessage(FxCop.Category.ReliabilityBasic, FxCop.Rule.UseNewGuidHelperRule,
        ////    Justification = "This is a method that creates ETW provider passing Guid Provider ID.")]
        ////static DiagnosticTrace InitializeTracing()
        ////{
        ////    DiagnosticTrace trace = new DiagnosticTrace(DefaultEventSource, DiagnosticTrace.DefaultEtwProviderId);
        ////    return trace;
        ////}

#if UNUSED
        public static ExceptionHandler AsynchronousThreadExceptionHandler
        {
            [Fx.Tag.SecurityNote(Critical = "access critical field", Safe = "ok for get-only access")]
            [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
            get
            {
                return Fx.asynchronousThreadExceptionHandler;
            }

            [Fx.Tag.SecurityNote(Critical = "sets a critical field")]
            [SecurityCritical]
            set
            {
                Fx.asynchronousThreadExceptionHandler = value;
            }
        }
#endif // UNUSED

        public static ExceptionTrace Exception
        {
            get
            {
                if (exceptionTrace == null)
                {
                    //need not be a true singleton. No locking needed here.
                    exceptionTrace = new ExceptionTrace(DefaultEventSource);
                }

                return exceptionTrace;
            }
        }
        
        // Do not call the parameter "message" or else FxCop thinks it should be localized.
        [Conditional("DEBUG")]
        public static void Assert(bool condition, string description)
        {
            if (!condition)
            {
                Assert(description);
            }
        }

        [Conditional("DEBUG")]
        public static void Assert(string description)
        {
            Debug.Assert(false, description);
        }

        public static void AssertAndThrow(bool condition, string description)
        {
            if (!condition)
            {
                AssertAndThrow(description);
            }
        }

        public static void AssertIsNotNull(object objectMayBeNull, string description)
        {
            if (objectMayBeNull == null)
            {
                Fx.AssertAndThrow(description);
            }
        }

        [MethodImpl(MethodImplOptions.NoInlining)]
        public static Exception AssertAndThrow(string description)
        {
            Fx.Assert(description);
            throw Fx.Exception.AsError(new AssertionFailedException(description));
        }

        public static void AssertAndThrowFatal(bool condition, string description)
        {
            if (!condition)
            {
                AssertAndThrowFatal(description);
            }
        }

        [MethodImpl(MethodImplOptions.NoInlining)]
        public static Exception AssertAndThrowFatal(string description)
        {
            Fx.Assert(description);
            throw Fx.Exception.AsError(new FatalException(description));
        }

        internal static bool FailFastInProgress { get; private set; }

        public static bool IsFatal(Exception exception)
        {
            while (exception != null)
            {
                // FYI, CallbackException is-a FatalException
                if (exception is FatalException ||
#if WINDOWS_UWP || PCL
                    exception is OutOfMemoryException ||
#else
                    (exception is OutOfMemoryException && !(exception is InsufficientMemoryException)) ||
                    exception is ThreadAbortException ||
                    exception is AccessViolationException ||
#endif
                    exception is SEHException)
                {
                    return true;
                }

                // These exceptions aren't themselves fatal, but since the CLR uses them to wrap other exceptions,
                // we want to check to see whether they've been used to wrap a fatal exception.  If so, then they
                // count as fatal.
                if (exception is TypeInitializationException ||
                    exception is TargetInvocationException)
                {
                    exception = exception.InnerException;
                }
                else if (exception is AggregateException)
                {
                    // AggregateExceptions have a collection of inner exceptions, which may themselves be other
                    // wrapping exceptions (including nested AggregateExceptions).  Recursively walk this
                    // hierarchy.  The (singular) InnerException is included in the collection.
                    ReadOnlyCollection<Exception> innerExceptions = ((AggregateException)exception).InnerExceptions;
                    foreach (Exception innerException in innerExceptions)
                    {
                        if (IsFatal(innerException))
                        {
                            return true;
                        }
                    }

                    break;
                }
                else if (exception is NullReferenceException)
                {
                    ////MessagingClientEtwProvider.Provider.EventWriteNullReferenceErrorOccurred(exception.ToString());
                    break;
                }
                else
                {
                    break;
                }
            }

            return false;
        }

#if !WINDOWS_UWP && !PCL
        // If the transaction has aborted then we switch over to a new transaction
        // which we will immediately abort after setting Transaction.Current
        public static TransactionScope CreateTransactionScope(Transaction transaction)
        {
            try
            {
                return transaction == null ? null : new TransactionScope(transaction);
            }
            catch (TransactionAbortedException)
            {
                CommittableTransaction tempTransaction = new CommittableTransaction();
                try
                {
                    return new TransactionScope(tempTransaction.Clone());
                }
                finally
                {
                    tempTransaction.Rollback();
                }
            }
        }

        public static void CompleteTransactionScope(ref TransactionScope scope)
        {
            TransactionScope localScope = scope;
            if (localScope != null)
            {
                scope = null;
                try
                {
                    localScope.Complete();
                }
                finally
                {
                    localScope.Dispose();
                }
            }
        }
#endif

#if UNUSED
        public static AsyncCallback ThunkCallback(AsyncCallback callback)
        {
            return (new AsyncThunk(callback)).ThunkFrame;
        }

        public static WaitCallback ThunkCallback(WaitCallback callback)
        {
            return (new WaitThunk(callback)).ThunkFrame;
        }

        public static TimerCallback ThunkCallback(TimerCallback callback)
        {
            return (new TimerThunk(callback)).ThunkFrame;
        }

        public static WaitOrTimerCallback ThunkCallback(WaitOrTimerCallback callback)
        {
            return (new WaitOrTimerThunk(callback)).ThunkFrame;
        }

        public static SendOrPostCallback ThunkCallback(SendOrPostCallback callback)
        {
            return (new SendOrPostThunk(callback)).ThunkFrame;
        }
#endif

#if !WINDOWS_UWP && !PCL
        [Fx.Tag.SecurityNote(Critical = "Construct the unsafe object IOCompletionThunk")]
        [SecurityCritical]
        public static IOCompletionCallback ThunkCallback(IOCompletionCallback callback)
        {
            return (new IOCompletionThunk(callback)).ThunkFrame;
        }
#endif
        #if DEBUG

        internal static Type[] BreakOnExceptionTypes
        {
            get
            {
                if (!Fx.breakOnExceptionTypesRetrieved)
                {
                    object value;
                    if (TryGetDebugSwitch(Fx.BreakOnExceptionTypesName, out value))
                    {
                        string[] typeNames = value as string[];
                        if (typeNames != null && typeNames.Length > 0)
                        {
                            List<Type> types = new List<Type>(typeNames.Length);
                            for (int i = 0; i < typeNames.Length; i++)
                            {
                                types.Add(Type.GetType(typeNames[i], false));
                            }
                            if (types.Count != 0)
                            {
                                Fx.breakOnExceptionTypesCache = types.ToArray();
                            }
                        }
                    }
                    Fx.breakOnExceptionTypesRetrieved = true;
                }
                return Fx.breakOnExceptionTypesCache;
            }
        }

        static bool TryGetDebugSwitch(string name, out object value)
        {
#if WINDOWS_UWP || PCL
            // No registry access in UWP
            value = null;
            return false;
#else
            value = null;
            try
            {
                RegistryKey key = Registry.LocalMachine.OpenSubKey(Fx.SBRegistryKey);
                if (key != null)
                {
                    using (key)
                    {
                        value = key.GetValue(name);
                    }
                }
            }
            catch (SecurityException)
            {
                // This debug-only code shouldn't trace.
            }
            return value != null;
#endif
        }

#endif // DEBUG
        [SuppressMessage(FxCop.Category.Design, FxCop.Rule.DoNotCatchGeneralExceptionTypes,
            Justification = "Don't want to hide the exception which is about to crash the process.")]
        [Fx.Tag.SecurityNote(Miscellaneous = "Must not call into PT code as it is called within a CER.")]
#if !WINDOWS_UWP && !PCL
        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
#endif
        static void TraceExceptionNoThrow(Exception exception)
        {
            try
            {
                // This call exits the CER.  However, when still inside a catch, normal ThreadAbort is prevented.
                // Rude ThreadAbort will still be allowed to terminate processing.
                Fx.Exception.TraceUnhandled(exception);
            }
            catch
            {
                // This empty catch is only acceptable because we are a) in a CER and b) processing an exception
                // which is about to crash the process anyway.
            }
        }

        [SuppressMessage(FxCop.Category.Design, FxCop.Rule.DoNotCatchGeneralExceptionTypes,
            Justification = "Don't want to hide the exception which is about to crash the process.")]
        [SuppressMessage(FxCop.Category.ReliabilityBasic, FxCop.Rule.IsFatalRule,
            Justification = "Don't want to hide the exception which is about to crash the process.")]
        [Fx.Tag.SecurityNote(Miscellaneous = "Must not call into PT code as it is called within a CER.")]
#if !WINDOWS_UWP && !PCL
        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
#endif
        static bool HandleAtThreadBase(Exception exception)
        {
            // This area is too sensitive to do anything but return.
            if (exception == null)
            {
                Fx.Assert("Null exception in HandleAtThreadBase.");
                return false;
            }

            TraceExceptionNoThrow(exception);

#if UNUSED
            try
            {
                ExceptionHandler handler = Fx.AsynchronousThreadExceptionHandler;
                return handler == null ? false : handler.HandleException(exception);
            }
            catch (Exception secondException)
            {
                // Don't let a new exception hide the original exception.
                TraceExceptionNoThrow(secondException);
            }
#endif // UNUSED

            return false;
        }

#if UNUSED
        abstract class Thunk<T> where T : class
        {
            [Fx.Tag.SecurityNote(Critical = "Make these safe to use in SecurityCritical contexts.")]
            [SecurityCritical]
            T callback;

            [Fx.Tag.SecurityNote(Critical = "Accesses critical field.", Safe = "Data provided by caller.")]
            protected Thunk(T callback)
            {
                this.callback = callback;
            }

            internal T Callback
            {
                [Fx.Tag.SecurityNote(Critical = "Accesses critical field.", Safe = "Data is not privileged.")]
                get
                {
                    return this.callback;
                }
            }
        }

        sealed class TimerThunk : Thunk<TimerCallback>
        {
            public TimerThunk(TimerCallback callback)
                : base(callback)
            {
            }

            public TimerCallback ThunkFrame
            {
                get
                {
                    return new TimerCallback(UnhandledExceptionFrame);
                }
            }

            [Fx.Tag.SecurityNote(Critical = "Calls PrepareConstrainedRegions which has a LinkDemand",
                Safe = "Guaranteed not to call into PT user code from the finally.")]
            void UnhandledExceptionFrame(object state)
            {
                RuntimeHelpers.PrepareConstrainedRegions();
                try
                {
                    Callback(state);
                }
                catch (Exception exception)
                {
                    if (!Fx.HandleAtThreadBase(exception))
                    {
                        throw;
                    }
                }
            }
        }

        sealed class WaitOrTimerThunk : Thunk<WaitOrTimerCallback>
        {
            public WaitOrTimerThunk(WaitOrTimerCallback callback)
                : base(callback)
            {
            }

            public WaitOrTimerCallback ThunkFrame
            {
                get
                {
                    return new WaitOrTimerCallback(UnhandledExceptionFrame);
                }
            }

            [Fx.Tag.SecurityNote(Critical = "Calls PrepareConstrainedRegions which has a LinkDemand",
                Safe = "Guaranteed not to call into PT user code from the finally.")]
            void UnhandledExceptionFrame(object state, bool timedOut)
            {
                RuntimeHelpers.PrepareConstrainedRegions();
                try
                {
                    Callback(state, timedOut);
                }
                catch (Exception exception)
                {
                    if (!Fx.HandleAtThreadBase(exception))
                    {
                        throw;
                    }
                }
            }
        }

        sealed class WaitThunk : Thunk<WaitCallback>
        {
            public WaitThunk(WaitCallback callback)
                : base(callback)
            {
            }

            public WaitCallback ThunkFrame
            {
                get
                {
                    return new WaitCallback(UnhandledExceptionFrame);
                }
            }

            [Fx.Tag.SecurityNote(Critical = "Calls PrepareConstrainedRegions which has a LinkDemand",
                Safe = "Guaranteed not to call into PT user code from the finally.")]
            void UnhandledExceptionFrame(object state)
            {
                RuntimeHelpers.PrepareConstrainedRegions();
                try
                {
                    Callback(state);
                }
                catch (Exception exception)
                {
                    if (!Fx.HandleAtThreadBase(exception))
                    {
                        throw;
                    }
                }
            }
        }

        sealed class AsyncThunk : Thunk<AsyncCallback>
        {
            public AsyncThunk(AsyncCallback callback)
                : base(callback)
            {
            }

            public AsyncCallback ThunkFrame
            {
                get
                {
                    return new AsyncCallback(UnhandledExceptionFrame);
                }
            }

            [Fx.Tag.SecurityNote(Critical = "Calls PrepareConstrainedRegions which has a LinkDemand",
                Safe = "Guaranteed not to call into PT user code from the finally.")]
            void UnhandledExceptionFrame(IAsyncResult result)
            {
                RuntimeHelpers.PrepareConstrainedRegions();
                try
                {
                    Callback(result);
                }
                catch (Exception exception)
                {
                    if (!Fx.HandleAtThreadBase(exception))
                    {
                        throw;
                    }
                }
            }
        }

        public abstract class ExceptionHandler
        {
            [Fx.Tag.SecurityNote(Miscellaneous = "Must not call into PT code as it is called within a CER.")]
            public abstract bool HandleException(Exception exception);
        }
#endif // UNUSED

#if !WINDOWS_UWP && !PCL
        // This can't derive from Thunk since T would be unsafe.
        [Fx.Tag.SecurityNote(Critical = "unsafe object")]
        [SecurityCritical]
        unsafe sealed class IOCompletionThunk
        {
            [Fx.Tag.SecurityNote(Critical = "Make these safe to use in SecurityCritical contexts.")]
            IOCompletionCallback callback;

            [Fx.Tag.SecurityNote(Critical = "Accesses critical field.", Safe = "Data provided by caller.")]
            public IOCompletionThunk(IOCompletionCallback callback)
            {
                this.callback = callback;
            }

            public IOCompletionCallback ThunkFrame
            {
                [Fx.Tag.SecurityNote(Safe = "returns a delegate around the safe method UnhandledExceptionFrame")]
                get
                {
                    return new IOCompletionCallback(UnhandledExceptionFrame);
                }
            }

            [Fx.Tag.SecurityNote(Critical = "Accesses critical field, calls PrepareConstrainedRegions which has a LinkDemand",
                Safe = "Delegates can be invoked, guaranteed not to call into PT user code from the finally.")]
            void UnhandledExceptionFrame(uint error, uint bytesRead, NativeOverlapped* nativeOverlapped)
            {
                RuntimeHelpers.PrepareConstrainedRegions();
                try
                {
                    this.callback(error, bytesRead, nativeOverlapped);
                }
                catch (Exception exception)
                {
                    if (!Fx.HandleAtThreadBase(exception))
                    {
                        throw;
                    }
                }
            }
        }
#endif

#if UNUSED
        sealed class SendOrPostThunk : Thunk<SendOrPostCallback>
        {
            public SendOrPostThunk(SendOrPostCallback callback)
                : base(callback)
            {
            }

            public SendOrPostCallback ThunkFrame
            {
                get
                {
                    return new SendOrPostCallback(UnhandledExceptionFrame);
                }
            }

            [Fx.Tag.SecurityNote(Critical = "Calls PrepareConstrainedRegions which has a LinkDemand",
                Safe = "Guaranteed not to call into PT user code from the finally.")]
            void UnhandledExceptionFrame(object state)
            {
                RuntimeHelpers.PrepareConstrainedRegions();
                try
                {
                    Callback(state);
                }
                catch (Exception exception)
                {
                    if (!Fx.HandleAtThreadBase(exception))
                    {
                        throw;
                    }
                }
            }
        }
#endif // UNUSED

        public static class Tag
        {
            public enum CacheAttrition
            {
                None,
                ElementOnTimer,

                // A finalizer/WeakReference based cache, where the elements are held by WeakReferences (or hold an
                // inner object by a WeakReference), and the weakly-referenced object has a finalizer which cleans the
                // item from the cache.
                ElementOnGC,

                // A cache that provides a per-element token, delegate, interface, or other piece of context that can
                // be used to remove the element (such as IDisposable).
                ElementOnCallback,

                FullPurgeOnTimer,
                FullPurgeOnEachAccess,
                PartialPurgeOnTimer,
                PartialPurgeOnEachAccess,
            }

            public enum Location
            {
                InProcess,
                OutOfProcess,
                LocalSystem,
                LocalOrRemoteSystem, // as in a file that might live on a share
                RemoteSystem,
            }

            public enum SynchronizationKind
            {
                LockStatement,
                MonitorWait,
                MonitorExplicit,
                InterlockedNoSpin,
                InterlockedWithSpin,

                // Same as LockStatement if the field type is object.
                FromFieldType,
            }

            [Flags]
            public enum BlocksUsing
            {
                MonitorEnter,
                MonitorWait,
                ManualResetEvent,
                AutoResetEvent,
                AsyncResult,
                IAsyncResult,
                PInvoke,
                InputQueue,
                ThreadNeutralSemaphore,
                PrivatePrimitive,
                OtherInternalPrimitive,
                OtherFrameworkPrimitive,
                OtherInterop,
                Other,

                NonBlocking, // For use by non-blocking SynchronizationPrimitives such as IOThreadScheduler
            }

            public static class Strings
            {
                internal const string ExternallyManaged = "externally managed";
                internal const string AppDomain = "AppDomain";
                internal const string DeclaringInstance = "instance of declaring class";
                internal const string Unbounded = "unbounded";
                internal const string Infinite = "infinite";
            }

            [AttributeUsage(AttributeTargets.Field | AttributeTargets.Method | AttributeTargets.Constructor,
                AllowMultiple = true, Inherited = false)]
            [Conditional("CODE_ANALYSIS")]
            public sealed class ExternalResourceAttribute : Attribute
            {
                readonly Location location;
                readonly string description;

                public ExternalResourceAttribute(Location location, string description)
                {
                    this.location = location;
                    this.description = description;
                }

                public Location Location
                {
                    get
                    {
                        return this.location;
                    }
                }

                public string Description
                {
                    get
                    {
                        return this.description;
                    }
                }
            }

            [AttributeUsage(AttributeTargets.Field)]
            [Conditional("CODE_ANALYSIS")]
            public sealed class CacheAttribute : Attribute
            {
                readonly Type elementType;
                readonly CacheAttrition cacheAttrition;

                public CacheAttribute(Type elementType, CacheAttrition cacheAttrition)
                {
                    Scope = Strings.DeclaringInstance;
                    SizeLimit = Strings.Unbounded;
                    Timeout = Strings.Infinite;

                    if (elementType == null)
                    {
                        throw Fx.Exception.ArgumentNull("elementType");
                    }

                    this.elementType = elementType;
                    this.cacheAttrition = cacheAttrition;
                }

                public Type ElementType
                {
                    get
                    {
                        return this.elementType;
                    }
                }

                public CacheAttrition CacheAttrition
                {
                    get
                    {
                        return this.cacheAttrition;
                    }
                }

                public string Scope { get; set; }
                public string SizeLimit { get; set; }
                public string Timeout { get; set; }
            }

            [AttributeUsage(AttributeTargets.Field)]
            [Conditional("CODE_ANALYSIS")]
            public sealed class QueueAttribute : Attribute
            {
                readonly Type elementType;

                public QueueAttribute(Type elementType)
                {
                    Scope = Strings.DeclaringInstance;
                    SizeLimit = Strings.Unbounded;

                    if (elementType == null)
                    {
                        throw Fx.Exception.ArgumentNull("elementType");
                    }

                    this.elementType = elementType;
                }

                public Type ElementType
                {
                    get
                    {
                        return this.elementType;
                    }
                }

                public string Scope { get; set; }
                public string SizeLimit { get; set; }
                public bool StaleElementsRemovedImmediately { get; set; }
                public bool EnqueueThrowsIfFull { get; set; }
            }

            // Set on a class when that class uses lock (this) - acts as though it were on a field
            //     private object this;
            [AttributeUsage(AttributeTargets.Field | AttributeTargets.Class, Inherited = false)]
            [Conditional("CODE_ANALYSIS")]
            public sealed class SynchronizationObjectAttribute : Attribute
            {
                public SynchronizationObjectAttribute()
                {
                    Blocking = true;
                    Scope = Strings.DeclaringInstance;
                    Kind = SynchronizationKind.FromFieldType;
                }

                public bool Blocking
                {
                    get;
                    set;
                }
                public string Scope
                {
                    get;
                    set;
                }
                public SynchronizationKind Kind
                {
                    get;
                    set;
                }
            }

            [AttributeUsage(AttributeTargets.Class | AttributeTargets.Struct, Inherited = true)]
            [Conditional("CODE_ANALYSIS")]
            public sealed class SynchronizationPrimitiveAttribute : Attribute
            {
                readonly BlocksUsing blocksUsing;

                public SynchronizationPrimitiveAttribute(BlocksUsing blocksUsing)
                {
                    this.blocksUsing = blocksUsing;
                }

                public BlocksUsing BlocksUsing
                {
                    get
                    {
                        return this.blocksUsing;
                    }
                }

                public bool SupportsAsync
                {
                    get;
                    set;
                }
                public bool Spins
                {
                    get;
                    set;
                }
                public string ReleaseMethod
                {
                    get;
                    set;
                }
            }

            [AttributeUsage(AttributeTargets.Method | AttributeTargets.Constructor, Inherited = false)]
            [Conditional("CODE_ANALYSIS")]
            public sealed class BlockingAttribute : Attribute
            {
                public BlockingAttribute()
                {
                }

                public string CancelMethod
                {
                    get;
                    set;
                }
                public Type CancelDeclaringType
                {
                    get;
                    set;
                }
                public string Conditional
                {
                    get;
                    set;
                }
            }

            // Sometime a method will call a conditionally-blocking method in such a way that it is guaranteed
            // not to block (i.e. the condition can be Asserted false).  Such a method can be marked as
            // GuaranteeNonBlocking as an assertion that the method doesn't block despite calling a blocking method.
            //
            // Methods that don't call blocking methods and aren't marked as Blocking are assumed not to block, so
            // they do not require this attribute.
            [AttributeUsage(AttributeTargets.Method | AttributeTargets.Constructor, Inherited = false)]
            [Conditional("CODE_ANALYSIS")]
            public sealed class GuaranteeNonBlockingAttribute : Attribute
            {
                public GuaranteeNonBlockingAttribute()
                {
                }
            }

            [AttributeUsage(AttributeTargets.Method | AttributeTargets.Constructor, Inherited = false)]
            [Conditional("CODE_ANALYSIS")]
            public sealed class NonThrowingAttribute : Attribute
            {
                public NonThrowingAttribute()
                {
                }
            }

            [AttributeUsage(AttributeTargets.Method | AttributeTargets.Constructor, Inherited = false)]
            [Conditional("CODE_ANALYSIS")]
            public sealed class InheritThrowsAttribute : Attribute
            {
                public InheritThrowsAttribute()
                {
                }

                public Type FromDeclaringType
                {
                    get;
                    set;
                }
                public string From
                {
                    get;
                    set;
                }
            }

            [AttributeUsage(AttributeTargets.Assembly | AttributeTargets.Module | AttributeTargets.Class |
                AttributeTargets.Struct | AttributeTargets.Enum | AttributeTargets.Constructor | AttributeTargets.Method |
                AttributeTargets.Property | AttributeTargets.Field | AttributeTargets.Event | AttributeTargets.Interface |
                AttributeTargets.Delegate, AllowMultiple = false, Inherited = false)]
            [Conditional("CODE_ANALYSIS")]
            public sealed class SecurityNoteAttribute : Attribute
            {
                public SecurityNoteAttribute()
                {
                }

                public string Critical
                {
                    get;
                    set;
                }
                public string Safe
                {
                    get;
                    set;
                }
                public string Miscellaneous
                {
                    get;
                    set;
                }
            }
        }
    }
}
