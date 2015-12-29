//  ------------------------------------------------------------------------------------
//  Copyright (c) Microsoft Corporation
//  All rights reserved. 
//  
//  Licensed under the Apache License, Version 2.0 (the ""License""); you may not use this 
//  file except in compliance with the License. You may obtain a copy of the License at 
//  http://www.apache.org/licenses/LICENSE-2.0  
//  
//  THIS CODE IS PROVIDED *AS IS* BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, 
//  EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION ANY IMPLIED WARRANTIES OR 
//  CONDITIONS OF TITLE, FITNESS FOR A PARTICULAR PURPOSE, MERCHANTABLITY OR 
//  NON-INFRINGEMENT. 
// 
//  See the Apache Version 2.0 License for specific language governing permissions and 
//  limitations under the License.
//  ------------------------------------------------------------------------------------

namespace System.Threading
{
    using global::Windows.System.Threading;

    delegate void TimerCallback(object state);

    class Timer
    {
        readonly TimerCallback callback;
        readonly object state;
        readonly ThreadPoolTimer timer;

        public Timer(TimerCallback callback, object state, int dueTime, int period)
        {
            this.callback = callback;
            this.state = state;
            this.timer = ThreadPoolTimer.CreatePeriodicTimer(this.OnTimer, TimeSpan.FromMilliseconds(dueTime));
        }

        public void Dispose()
        {
            this.timer.Cancel();
        }

        void OnTimer(ThreadPoolTimer t)
        {
            this.callback(this.state);
        }
    }
}
