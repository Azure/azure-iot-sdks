// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// This limited implementation of the System.Threading.Timer is necessary for PCL profile111.

namespace System.Threading
{
    using System.Threading;
    using System.Threading.Tasks;

    internal delegate void TimerCallback(object state);

    internal sealed class Timer : CancellationTokenSource, IDisposable
    {
        internal Timer(TimerCallback callback, object state, TimeSpan dueTime, TimeSpan period) :
            this(callback, state, (int)dueTime.TotalMilliseconds, (int)period.TotalMilliseconds)
        {
        }

        internal Timer(TimerCallback callback, object state, int dueTime, int period)
        {
            if (dueTime != -1 || period != -1)
            {
                throw new NotImplementedException("This stub implementation does not support this scenario.");
            }

            _callback = callback;
            _state = state;
        }

        internal bool Change(TimeSpan dueTime, TimeSpan period)
        {
            return this.Change((int)dueTime.TotalMilliseconds, (int)period.TotalMilliseconds);
        }

        internal bool Change(int dueTime, int period)
        {
            if (period != -1)
            {
                throw new NotImplementedException("This stub implementation does not support this scenario.");
            }

            // Cancel any previously running tasks (through Token).
            Cancel();

            Task.Delay(dueTime, Token).ContinueWith(
                (t, s) =>
                {
                    var tuple = (Tuple<TimerCallback, object>)s;
                    tuple.Item1(tuple.Item2);
                },
                Tuple.Create(_callback, _state),
                CancellationToken.None,
                TaskContinuationOptions.ExecuteSynchronously | TaskContinuationOptions.OnlyOnRanToCompletion,
                TaskScheduler.Default);

            return true;
        }

        public new void Dispose() { base.Cancel(); }

        // private data members
        private TimerCallback _callback;
        private object _state;
    }
}
