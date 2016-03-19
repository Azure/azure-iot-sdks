// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client
{
    using System;

    // An AsyncResult that completes as soon as it is instantiated.
#if !WINDOWS_UWP && !PCL
    [Serializable]
#endif
    class CompletedAsyncResult : AsyncResult
    {
        public CompletedAsyncResult(AsyncCallback callback, object state)
            : base(callback, state)
        {
            Complete(true);
        }

        public CompletedAsyncResult(Exception exception, AsyncCallback callback, object state)
            : base(callback, state)
        {
            Complete(true, exception);
        }

        [Fx.Tag.GuaranteeNonBlocking]
        public static void End(IAsyncResult result)
        {
            Fx.AssertAndThrowFatal(result.IsCompleted, "CompletedAsyncResult was not completed!");
            AsyncResult.End<CompletedAsyncResult>(result);
        }
    }

#if !WINDOWS_UWP && !PCL
    [Serializable]
#endif
    class CompletedAsyncResultT<T> : AsyncResult
    {
        T data;

        public CompletedAsyncResultT(T data, AsyncCallback callback, object state)
            : base(callback, state)
        {
            this.data = data;
            Complete(true);
        }

        [Fx.Tag.GuaranteeNonBlocking]
        public static T End(IAsyncResult result)
        {
            Fx.AssertAndThrowFatal(result.IsCompleted, "CompletedAsyncResult<T> was not completed!");
            CompletedAsyncResultT<T> completedResult = AsyncResult.End<CompletedAsyncResultT<T>>(result);
            return completedResult.data;
        }
    }

#if !WINDOWS_UWP && !PCL
    [Serializable]
#endif
    class CompletedAsyncResultT2<TResult, TParameter> : AsyncResult
    {
        TResult resultData;
        TParameter parameter;

        public CompletedAsyncResultT2(TResult resultData, TParameter parameter, AsyncCallback callback, object state)
            : base(callback, state)
        {
            this.resultData = resultData;
            this.parameter = parameter;
            Complete(true);
        }

        [Fx.Tag.GuaranteeNonBlocking]
        public static TResult End(IAsyncResult result, out TParameter parameter)
        {
            Fx.AssertAndThrowFatal(result.IsCompleted, "CompletedAsyncResult<T> was not completed!");
            CompletedAsyncResultT2<TResult, TParameter> completedResult = AsyncResult.End<CompletedAsyncResultT2<TResult, TParameter>>(result);
            parameter = completedResult.parameter;
            return completedResult.resultData;
        }
    }
}
