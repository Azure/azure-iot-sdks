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

    /// <summary>
    /// Provides framework specific routines.
    /// </summary>
    public static class Fx
    {
        /// <summary>
        /// Asserts a condition is true.
        /// </summary>
        /// <param name="condition">A boolean value indicating the condition.</param>
        /// <param name="message">The error message if condition is not met.</param>
        [Conditional("DEBUG")]
        public static void Assert(bool condition, string message)
        {
            Debug.Assert(condition, message);
        }

        /// <summary>
        /// Formats a string from a format and an array of arguments.
        /// </summary>
        /// <param name="format">The format string.</param>
        /// <param name="args">The arguments.</param>
        /// <returns></returns>
        public static string Format(string format, params object[] args)
        {
            return string.Format(format, args);
        }

        /// <summary>
        /// Starts a new thread.
        /// </summary>
        /// <param name="threadStart">The thread start callback.</param>
        public static void StartThread(ThreadStart threadStart)
        {
            ThreadPool.QueueUserWorkItem(
                o => { ((ThreadStart)o)(); },
                threadStart);
        }
    }
}