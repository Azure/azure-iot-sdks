// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client
{
    static class FxTrace
    {
        const string EventSourceName = "Microsoft.IotHub";
        static ExceptionTrace exceptionTrace;

        public static ExceptionTrace Exception
        {
            get
            {
                if (exceptionTrace == null)
                {
                    // don't need a lock here since a true singleton is not required
                    exceptionTrace = new ExceptionTrace(EventSourceName);
                }

                return exceptionTrace;
            }
        }
    }
}
