// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Common.Tracing
{
    using System;
    using System.Diagnostics;

    /// <summary>
    /// This class holds the ActivityId that would be set on the thread for ETW during the trace. 
    /// </summary>
    internal class EventTraceActivity
    {
        static EventTraceActivity empty;

        public EventTraceActivity()
            : this(Guid.NewGuid())
        {
        }

        public EventTraceActivity(Guid activityId)
        {
            this.ActivityId = activityId;
        }

        public static EventTraceActivity Empty
        {
            get
            {
                if (empty == null)
                {
                    empty = new EventTraceActivity(Guid.Empty);
                }

                return empty;
            }
        }

        public static string Name
        {
            get { return "E2EActivity"; }
        }

        // this field is passed as reference to native code. 
        public Guid ActivityId;

        public static EventTraceActivity CreateFromThread()
        {
            Guid id = Trace.CorrelationManager.ActivityId;
            if (id == Guid.Empty)
            {
                return EventTraceActivity.Empty;
            }

            return new EventTraceActivity(id);
        }
    }
}
