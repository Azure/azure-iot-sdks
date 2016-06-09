// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices
{
    using System;
    using System.Threading;

    class IOThreadTimerSlim
    {
        Timer timer;
        Action<object> callback;
        object callbackState;

        private void CreateTimer()
        {
            this.timer = new Timer((obj) => callback(obj), callbackState, TimeSpan.FromMilliseconds(-1), TimeSpan.FromMilliseconds(-1));
        }

        public IOThreadTimerSlim(Action<object> callback, object callbackState, bool isTypicallyCanceledShortlyAfterBeingSet)
        {
            this.callback = callback;
            this.callbackState = callbackState;
            CreateTimer();
        }

        public void Set(TimeSpan timeFromNow)
        {
            if (timer == null)
            {
                CreateTimer();
            }
            timer.Change(timeFromNow, TimeSpan.FromMilliseconds(-1));
        }

        public bool Cancel()
        {
            timer.Dispose();
            timer = null;
            return true;
        }
    }
}
