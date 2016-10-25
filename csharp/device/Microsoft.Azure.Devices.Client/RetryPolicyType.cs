using System;
using System.Collections.Generic;
using System.Text;

namespace Microsoft.Azure.Devices.Client
{
    /// <summary>
    /// Retry Strategy types supported by DeviceClient
    /// </summary>
    public enum RetryPolicyType
    {
        /// <summary>
        /// No retry.  A single attempt to connect then tries a single time to send if connection is established
        /// </summary>
        No_Retry = 0,

        /// <summary>
        /// A retry strategy with backoff parameters for calculating the exponential delay between retries. 
        /// </summary>
        Exponential_Backoff_With_Jitter = 1,
    }
}
