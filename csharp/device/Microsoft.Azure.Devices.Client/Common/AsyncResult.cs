// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client
{
    using Microsoft.Azure.Devices.Client.Exceptions;
    using System;
    using System.Diagnostics;
    using System.Diagnostics.CodeAnalysis;
    using System.Threading;
#if !WINDOWS_UWP && !PCL
    using System.Transactions;
#endif

    // AsyncResult starts acquired; Complete releases.
    [Fx.Tag.SynchronizationPrimitive(Fx.Tag.BlocksUsing.ManualResetEvent, SupportsAsync = true, ReleaseMethod = "Complete")]
    [DebuggerStepThrough]
    [SuppressMessage("Microsoft.Design", "CA1001:TypesThatOwnDisposableFieldsShouldBeDisposable",
        Justification = "Uses custom scheme for cleanup")]
    abstract class AsyncResult : IAsyncResult
    {
        public const string DisablePrepareForRethrow = "DisablePrepareForRethrow";

        static AsyncCallback asyncCompletionWrapperCallback;
        AsyncCallback callback;
        bool completedSynchronously;
        bool endCalled;
        Exception exception;
        bool isCompleted;
        AsyncCompletion nextAsyncCompletion;
#if !WINDOWS_UWP && !PCL
        IAsyncResult deferredTransactionalResult;
        TransactionSignalScope transactionContext;
#endif
        object state;

        [Fx.Tag.SynchronizationObject]
        ManualResetEvent manualResetEvent;

        [Fx.Tag.SynchronizationObject(Blocking = false)]
        object thisLock;

#if DEBUG
        UncompletedAsyncResultMarker marker;
#endif

        protected AsyncResult(AsyncCallback callback, object state)
        {
            this.callback = callback;
            this.state = state;
            this.thisLock = new object();

#if DEBUG
            this.marker = new UncompletedAsyncResultMarker(this);
#endif
        }

        public object AsyncState
        {
            get
            {
                return this.state;
            }
        }

        public WaitHandle AsyncWaitHandle
        {
            get
            {
                if (this.manualResetEvent != null)
                {
                    return this.manualResetEvent;
                }

                lock (this.ThisLock)
                {
                    if (this.manualResetEvent == null)
                    {
                        this.manualResetEvent = new ManualResetEvent(isCompleted);
                    }
                }

                return this.manualResetEvent;
            }
        }

        public bool CompletedSynchronously
        {
            get
            {
                return this.completedSynchronously;
            }
        }

        public bool HasCallback
        {
            get
            {
                return this.callback != null;
            }
        }

        public bool IsCompleted
        {
            get
            {
                return this.isCompleted;
            }
        }

        // used in conjunction with PrepareAsyncCompletion to allow for finally blocks
        protected Action<AsyncResult, Exception> OnCompleting { get; set; }

        // Override this property to provide the ActivityId when completing with exception
        protected internal virtual EventTraceActivity Activity
        {
            get { return null; }
        }

#if !PCL
        // Override this property to change the trace level when completing with exception
        protected virtual TraceEventType TraceEventType
        {
            get { return TraceEventType.Verbose; }
        }
#endif
        protected object ThisLock
        {
            get
            {
                return this.thisLock;
            }
        }

        // subclasses like TraceAsyncResult can use this to wrap the callback functionality in a scope
        protected Action<AsyncCallback, IAsyncResult> VirtualCallback
        {
            get;
            set;
        }

        protected bool TryComplete(bool didCompleteSynchronously, Exception exception)
        {
            lock (this.ThisLock)
            {
                if (this.isCompleted)
                {
                    return false;
                }

                this.exception = exception;
                this.isCompleted = true;
            }

#if DEBUG
            this.marker.AsyncResult = null;
            this.marker = null;
#endif

            this.completedSynchronously = didCompleteSynchronously;
            if (this.OnCompleting != null)
            {
                // Allow exception replacement, like a catch/throw pattern.
                try
                {
                    this.OnCompleting(this, this.exception);
                }
                catch (Exception e)
                {
                    if (Fx.IsFatal(e))
                    {
                        throw;
                    }
                    this.exception = e;
                }
            }

            if (didCompleteSynchronously)
            {
                // If we completedSynchronously, then there's no chance that the manualResetEvent was created so
                // we don't need to worry about a race
                Fx.Assert(this.manualResetEvent == null, "No ManualResetEvent should be created for a synchronous AsyncResult.");
            }
            else
            {
                lock (this.ThisLock)
                {
                    if (this.manualResetEvent != null)
                    {
                        this.manualResetEvent.Set();
                    }
                }
            }

            if (this.callback != null)
            {
                try
                {
                    if (this.VirtualCallback != null)
                    {
                        this.VirtualCallback(this.callback, this);
                    }
                    else
                    {
                        this.callback(this);
                    }
                }
#pragma warning disable 1634
#pragma warning suppress 56500 // transferring exception to another thread
                catch (Exception e)
                {
                    if (Fx.IsFatal(e))
                    {
                        throw;
                    }

                    throw Fx.Exception.AsError(new CallbackException(CommonResources.AsyncCallbackThrewException, e));
                }
#pragma warning restore 1634
            }

            return true;
        }

        protected bool TryComplete(bool didcompleteSynchronously)
        {
            return this.TryComplete(didcompleteSynchronously, null);
        }

        protected void Complete(bool didCompleteSynchronously)
        {
            this.Complete(didCompleteSynchronously, null);
        }

        protected void Complete(bool didCompleteSynchronously, Exception e)
        {
            if (!this.TryComplete(didCompleteSynchronously, e))
            {
                throw Fx.Exception.AsError(new InvalidOperationException(CommonResources.GetString(CommonResources.AsyncResultCompletedTwice, this.GetType())));
            }
        }

        static void AsyncCompletionWrapperCallback(IAsyncResult result)
        {
            if (result == null)
            {
                throw Fx.Exception.AsError(new InvalidOperationException(CommonResources.InvalidNullAsyncResult));
            }
            if (result.CompletedSynchronously)
            {
                return;
            }

            AsyncResult thisPtr = (AsyncResult)result.AsyncState;

#if !WINDOWS_UWP && !PCL
            if (thisPtr.transactionContext != null && !thisPtr.transactionContext.Signal(result))
            {
                // The TransactionScope isn't cleaned up yet and can't be done on this thread.  Must defer
                // the callback (which is likely to attempt to commit the transaction) until later.
                return;
            }
#endif

            AsyncCompletion callback = thisPtr.GetNextCompletion();
            if (callback == null)
            {
                ThrowInvalidAsyncResult(result);
            }

            bool completeSelf = false;
            Exception completionException = null;
            try
            {
                completeSelf = callback(result);
            }
            catch (Exception e)
            {
                completeSelf = true;
                completionException = e;
            }

            if (completeSelf)
            {
                thisPtr.Complete(false, completionException);
            }
        }

        protected AsyncCallback PrepareAsyncCompletion(AsyncCompletion callback)
        {
#if !WINDOWS_UWP && !PCL
            if (this.transactionContext != null)
            {
                // It might be an old, leftover one, if an exception was thrown within the last using (PrepareTransactionalCall()) block.
                if (this.transactionContext.IsPotentiallyAbandoned)
                {
                    this.transactionContext = null;
                }
                else
                {
                    this.transactionContext.Prepared();
                }
            }
#endif
            this.nextAsyncCompletion = callback;
            if (AsyncResult.asyncCompletionWrapperCallback == null)
            {
                AsyncResult.asyncCompletionWrapperCallback = new AsyncCallback(AsyncCompletionWrapperCallback);
            }
            return AsyncResult.asyncCompletionWrapperCallback;
        }

#if !WINDOWS_UWP && !PCL
        protected IDisposable PrepareTransactionalCall(Transaction transaction)
        {
            if (this.transactionContext != null && !this.transactionContext.IsPotentiallyAbandoned)
            {
                ThrowInvalidAsyncResult("PrepareTransactionalCall should only be called as the object of non-nested using statements. If the Begin succeeds, Check/SyncContinue must be called before another PrepareTransactionalCall.");
            }
            return this.transactionContext = transaction == null ? null : new TransactionSignalScope(this, transaction);
        }
#endif

        protected bool CheckSyncContinue(IAsyncResult result)
        {
            AsyncCompletion dummy;
            return TryContinueHelper(result, out dummy);
        }

        protected bool SyncContinue(IAsyncResult result)
        {
            AsyncCompletion callback;
            if (TryContinueHelper(result, out callback))
            {
                return callback(result);
            }
            else
            {
                return false;
            }
        }

        bool TryContinueHelper(IAsyncResult result, out AsyncCompletion callback)
        {
            if (result == null)
            {
                throw Fx.Exception.AsError(new InvalidOperationException(CommonResources.InvalidNullAsyncResult));
            }

            callback = null;


            if (result.CompletedSynchronously)
            {
#if !WINDOWS_UWP && !PCL
                // Once we pass the check, we know that we own forward progress, so transactionContext is correct. Verify its state.
                if (this.transactionContext != null)
                {
                    if (this.transactionContext.State != TransactionSignalState.Completed)
                    {
                        ThrowInvalidAsyncResult("Check/SyncContinue cannot be called from within the PrepareTransactionalCall using block.");
                    }
                    else if (this.transactionContext.IsSignalled)
                    {
                        // This is most likely to happen when result.CompletedSynchronously registers differently here and in the callback, which
                        // is the fault of 'result'.
                        ThrowInvalidAsyncResult(result);
                    }
                }
#endif
            }
#if !WINDOWS_UWP && !PCL
            else if (object.ReferenceEquals(result, this.deferredTransactionalResult))
            {
                // The transactionContext may not be current if forward progress has been made via the callback. Instead,
                // use deferredTransactionalResult to see if we are supposed to execute a post-transaction callback.
                //
                // Once we pass the check, we know that we own forward progress, so transactionContext is correct. Verify its state.
                if (this.transactionContext == null || !this.transactionContext.IsSignalled)
                {
                    ThrowInvalidAsyncResult(result);
                }
            this.deferredTransactionalResult = null;
            }
#endif
            else
            {
                return false;
            }

            callback = GetNextCompletion();
            if (callback == null)
            {
                ThrowInvalidAsyncResult("Only call Check/SyncContinue once per async operation (once per PrepareAsyncCompletion).");
            }
            return true;
        }

        AsyncCompletion GetNextCompletion()
        {
            AsyncCompletion result = this.nextAsyncCompletion;
#if !WINDOWS_UWP && !PCL
            this.transactionContext = null;
#endif
            this.nextAsyncCompletion = null;
            return result;
        }

        protected static void ThrowInvalidAsyncResult(IAsyncResult result)
        {
            throw Fx.Exception.AsError(new InvalidOperationException(CommonResources.GetString(CommonResources.InvalidAsyncResultImplementation, result.GetType())));
        }

        protected static void ThrowInvalidAsyncResult(string debugText)
        {
            string message = CommonResources.InvalidAsyncResultImplementationGeneric;
            if (debugText != null)
            {
#if DEBUG
                message += " " + debugText;
#endif
            }
            throw Fx.Exception.AsError(new InvalidOperationException(message));
        }

        [Fx.Tag.Blocking(Conditional = "!asyncResult.isCompleted")]
        protected static TAsyncResult End<TAsyncResult>(IAsyncResult result)
            where TAsyncResult : AsyncResult
        {
            if (result == null)
            {
                throw Fx.Exception.ArgumentNull("result");
            }

            TAsyncResult asyncResult = result as TAsyncResult;

            if (asyncResult == null)
            {
                throw Fx.Exception.Argument("result", CommonResources.InvalidAsyncResult);
            }

            if (asyncResult.endCalled)
            {
                throw Fx.Exception.AsError(new InvalidOperationException(CommonResources.AsyncResultAlreadyEnded));
            }

            asyncResult.endCalled = true;

            if (!asyncResult.isCompleted)
            {
                lock (asyncResult.ThisLock)
                {
                    if (!asyncResult.isCompleted && asyncResult.manualResetEvent == null)
                    {
                        asyncResult.manualResetEvent = new ManualResetEvent(asyncResult.isCompleted);
                    }
                }
            }

            if (asyncResult.manualResetEvent != null)
            {
                asyncResult.manualResetEvent.WaitOne();
#if !WINDOWS_UWP && !PCL // Close does not exist in UWP
                asyncResult.manualResetEvent.Close();
#endif
            }

            if (asyncResult.exception != null)
            {
                // Trace before PrepareForRethrow to avoid weird callstack strings
#if !PCL                
                Fx.Exception.TraceException(asyncResult.exception, asyncResult.TraceEventType, asyncResult.Activity);
#else
                Fx.Exception.TraceException(asyncResult.exception, TraceEventType.Verbose, asyncResult.Activity);
#endif
                ExceptionDispatcher.Throw(asyncResult.exception);
            }

            return asyncResult;
        }

        enum TransactionSignalState
        {
            Ready = 0,
            Prepared,
            Completed,
            Abandoned,
        }

#if !WINDOWS_UWP && !PCL
        [Serializable]
        class TransactionSignalScope : SignalGateT<IAsyncResult>, IDisposable
        {
            [NonSerialized]
            TransactionScope transactionScope;
            [NonSerialized]
            AsyncResult parent;
            bool disposed;

            public TransactionSignalScope(AsyncResult result, Transaction transaction)
            {
                Fx.Assert(transaction != null, "Null Transaction provided to AsyncResult.TransactionSignalScope.");
                this.parent = result;
                this.transactionScope = Fx.CreateTransactionScope(transaction);
            }

            public TransactionSignalState State { get; private set; }

            public bool IsPotentiallyAbandoned
            {
                get
                {
                    return this.State == TransactionSignalState.Abandoned || (State == TransactionSignalState.Completed && !IsSignalled);
                }
            }

            public void Prepared()
            {
                if (State != TransactionSignalState.Ready)
                {
                    AsyncResult.ThrowInvalidAsyncResult("PrepareAsyncCompletion should only be called once per PrepareTransactionalCall.");
                }
                this.State = TransactionSignalState.Prepared;
            }

            protected virtual void Dispose(bool disposing)
            {
                if (disposing && !this.disposed)
                {
                    this.disposed = true;

                    if (this.State == TransactionSignalState.Ready)
                    {
                        this.State = TransactionSignalState.Abandoned;
                    }
                    else if (this.State == TransactionSignalState.Prepared)
                    {
                        this.State = TransactionSignalState.Completed;
                    }
                    else
                    {
                        AsyncResult.ThrowInvalidAsyncResult("PrepareTransactionalCall should only be called in a using. Dispose called multiple times.");
                    }

                    try
                    {
                        Fx.CompleteTransactionScope(ref this.transactionScope);
                    }
                    catch (Exception exception)
                    {
                        if (Fx.IsFatal(exception))
                        {
                            throw;
                        }

                        // Complete and Dispose are not expected to throw.  If they do it can mess up the AsyncResult state machine.
                        throw Fx.Exception.AsError(new InvalidOperationException(CommonResources.AsyncTransactionException));
                    }

                    // This will release the callback to run, or tell us that we need to defer the callback to Check/SyncContinue.
                    //
                    // It's possible to avoid this Interlocked when CompletedSynchronously is true, but we have no way of knowing that
                    // from here, and adding a way would add complexity to the AsyncResult transactional calling pattern. This
                    // unnecessary Interlocked only happens when: PrepareTransactionalCall is called with a non-null transaction,
                    // PrepareAsyncCompletion is reached, and the operation completes synchronously or with an exception.
                    IAsyncResult result;
                    if (this.State == TransactionSignalState.Completed && Unlock(out result))
                    {
                        if (this.parent.deferredTransactionalResult != null)
                        {
                            AsyncResult.ThrowInvalidAsyncResult(this.parent.deferredTransactionalResult);
                        }
                        this.parent.deferredTransactionalResult = result;
                    }
                }
            }

            void IDisposable.Dispose()
            {
                Dispose(true);
                GC.SuppressFinalize(this);
            }
        }
#endif

        // can be utilized by subclasses to write core completion code for both the sync and async paths
        // in one location, signalling chainable synchronous completion with the boolean result,
        // and leveraging PrepareAsyncCompletion for conversion to an AsyncCallback.
        // NOTE: requires that "this" is passed in as the state object to the asynchronous sub-call being used with a completion routine.
        protected delegate bool AsyncCompletion(IAsyncResult result);

#if DEBUG
        class UncompletedAsyncResultMarker
        {
            public UncompletedAsyncResultMarker(AsyncResult result)
            {
                AsyncResult = result;
            }

            [SuppressMessage(FxCop.Category.Performance, FxCop.Rule.AvoidUncalledPrivateCode,
                Justification = "Debug-only facility")]
            public AsyncResult AsyncResult { get; set; }
        }
#endif
    }

    // Use this as your base class for AsyncResult and you don't have to define the End method.
    abstract class AsyncResultT<TAsyncResult> : AsyncResult
        where TAsyncResult : AsyncResultT<TAsyncResult>
    {
        protected AsyncResultT(AsyncCallback callback, object state)
            : base(callback, state)
        {
        }

        public static TAsyncResult End(IAsyncResult asyncResult)
        {
            return AsyncResult.End<TAsyncResult>(asyncResult);
        }
    }
}
