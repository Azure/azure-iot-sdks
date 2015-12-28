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

    /// <summary>
    /// Defines the traces levels. Except Frame, levels are forward inclusive.
    /// For example, Information level includes the Error and Warning levels.
    /// </summary>
    public enum TraceLevel
    {
        /// <summary>
        /// Specifies that error events should be traced.
        /// </summary>
        Error = 0x01,

        /// <summary>
        /// Specifies that warning events should be traced.
        /// </summary>
        Warning = 0x03,

        /// <summary>
        /// Specifies that informational events should be traced.
        /// </summary>
        Information = 0x07,

        /// <summary>
        /// Specifies that verbose events should be traced.
        /// </summary>
        Verbose = 0x0F,

        /// <summary>
        /// Specifies that AMQP frames should be traced.
        /// </summary>
        Frame = 0x10
    }

    /// <summary>
    /// The callback to invoke to write traces.
    /// </summary>
    /// <param name="format"></param>
    /// <param name="args"></param>
    public delegate void WriteTrace(string format, params object[] args);

    /// <summary>
    /// The Trace class for writing traces.
    /// </summary>
    public static class Trace
    {
        /// <summary>
        /// Gets or sets the trace level.
        /// </summary>
        public static TraceLevel TraceLevel;

        /// <summary>
        /// Gets or sets the trace callback.
        /// </summary>
        public static WriteTrace TraceListener;

        /// <summary>
        /// Writes a debug trace.
        /// </summary>
        /// <param name="format">The format string.</param>
        /// <param name="args">The argument list.</param>
        [Conditional("DEBUG")]
        public static void Debug(string format, params object[] args)
        {
            if (TraceListener != null)
            {
                TraceListener(format, args);
            }
        }

        /// <summary>
        /// Writes a trace if the specified level is enabled.
        /// </summary>
        /// <param name="level">The trace level.</param>
        /// <param name="format">The content to trace.</param>
        [Conditional("TRACE")]
        public static void WriteLine(TraceLevel level, string format)
        {
            if (TraceListener != null && (level & TraceLevel) > 0)
            {
                TraceListener(format);
            }
        }

        /// <summary>
        /// Writes a trace if the specified level is enabled.
        /// </summary>
        /// <param name="level">The trace level.</param>
        /// <param name="format">The format string.</param>
        /// <param name="arg1">The first argument.</param>
        [Conditional("TRACE")]
        public static void WriteLine(TraceLevel level, string format, object arg1)
        {
            if (TraceListener != null && (level & TraceLevel) > 0)
            {
                TraceListener(format, arg1);
            }
        }

        /// <summary>
        /// Writes a trace if the specified level is enabled.
        /// </summary>
        /// <param name="level">The trace level.</param>
        /// <param name="format">The format string.</param>
        /// <param name="arg1">The first argument.</param>
        /// <param name="arg2">The second argument.</param>
        [Conditional("TRACE")]
        public static void WriteLine(TraceLevel level, string format, object arg1, object arg2)
        {
            if (TraceListener != null && (level & TraceLevel) > 0)
            {
                TraceListener(format, arg1, arg2);
            }
        }

        /// <summary>
        /// Writes a trace if the specified level is enabled.
        /// </summary>
        /// <param name="level">The trace level.</param>
        /// <param name="format">The format string.</param>
        /// <param name="arg1">The first argument.</param>
        /// <param name="arg2">The second argument.</param>
        /// <param name="arg3">The third argument.</param>
        [Conditional("TRACE")]
        public static void WriteLine(TraceLevel level, string format, object arg1, object arg2, object arg3)
        {
            if (TraceListener != null && (level & TraceLevel) > 0)
            {
                TraceListener(format, arg1, arg2, arg3);
            }
        }
    }
}