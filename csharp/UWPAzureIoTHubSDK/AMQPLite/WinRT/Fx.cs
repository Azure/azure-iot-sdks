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

namespace Amqp
{
    using System.Diagnostics;
    using System.Threading;

    // Framework specific routines
    public static class Fx
    {
        [Conditional("DEBUG")]
        public static void Assert(bool condition, string message)
        {
            Debug.Assert(condition, message);
        }

        public static bool WaitOne(this WaitHandle waithandle, int msTimeout, bool unused)
        {
            return waithandle.WaitOne(msTimeout);
        }

        public static string Format(string format, params object[] args)
        {
            return string.Format(format, args);
        }

#if WINDOWS_PHONE
        public static void StartThread(ThreadStart threadStart)
        {
            ThreadPool.QueueUserWorkItem(
                o => { ((ThreadStart)o)(); },
                threadStart);
        }
#else
        public delegate void ThreadStart();

        public static void StartThread(ThreadStart threadStart)
        {
            System.Threading.Tasks.Task.Factory.StartNew(o => ((ThreadStart)o)(), threadStart);
        }
#endif
    }
}