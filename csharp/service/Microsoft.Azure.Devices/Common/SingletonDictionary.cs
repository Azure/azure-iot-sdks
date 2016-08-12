// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Common
{
    using System;
    using System.Collections.Concurrent;
    using System.Threading.Tasks;

    abstract class SingletonDictionary<TKey, TValue> : IDisposable
    {
        readonly ConcurrentDictionary<TKey, TaskCompletionSource<TValue>> dictionary;
        volatile bool disposed;

        public SingletonDictionary()
        {
            this.dictionary = new ConcurrentDictionary<TKey, TaskCompletionSource<TValue>>();
        }

        public void Dispose()
        {
            if (!this.disposed)
            {
                this.disposed = true;
                foreach (var kvp in this.dictionary)
                {
                    if (kvp.Value.Task.Status == TaskStatus.RanToCompletion)
                    {
                        OnSafeClose(kvp.Value.Task.Result);
                    }
                }
            }
        }

        public async Task<TValue> GetOrCreate(TKey key, TimeSpan timeout)
        {
            var timeoutHelper = new TimeoutHelper(timeout);

            while (!this.disposed && timeoutHelper.RemainingTime() > TimeSpan.Zero)
            {
                TaskCompletionSource<TValue> tcs;
                if (this.dictionary.TryGetValue(key, out tcs))
                {
                    return await tcs.Task.ConfigureAwait(false);
                }

                tcs = new TaskCompletionSource<TValue>();
                if (this.dictionary.TryAdd(key, tcs))
                {
                    this.CreateValue(key, tcs, timeout).Fork();
                }
            }

            if (this.disposed)
            {
                throw new ObjectDisposedException(this.GetType().Name);
            }
            else
            {
                throw new TimeoutException("Timed out trying to create {0}".FormatInvariant(this.GetType().Name));
            }
        }

        protected abstract Task<TValue> OnCreateAsync(TKey key, TimeSpan timeout);

        protected abstract void OnSafeClose(TValue value);

        protected bool Invalidate(TKey key)
        {
            TaskCompletionSource<TValue> tcs;
            return this.dictionary.TryRemove(key, out tcs);
        }

        async Task CreateValue(TKey key, TaskCompletionSource<TValue> tcs, TimeSpan timeout)
        {
            try
            {
                TValue value = await OnCreateAsync(key, timeout).ConfigureAwait(false);
                tcs.SetResult(value);

                if (this.disposed)
                {
                    OnSafeClose(value);
                }
            }
            catch (Exception ex)
            {
                if (Fx.IsFatal(ex))
                {
                    throw;
                }

                this.dictionary.TryRemove(key, out tcs);
                tcs.SetException(ex);

                // Prevents unobserved task exception from occurring in case no one is
                // awaiting this particular task.
                tcs.Task.ContinueWith(_ => { }, TaskContinuationOptions.OnlyOnFaulted).Fork();
            }
        }
    }
}
