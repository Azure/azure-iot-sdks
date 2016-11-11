// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client
{
    /// <summary>
    /// The data structure represent the Device Twin Method that is used for triggering an activity on the device
    /// </summary>
    internal sealed class Method
    {
        /// <summary>
        /// Constructor which uses the input byte array as the body
        /// </summary>
        /// <param name="methodName">a string containing the method name, e.g. 'Reboot'</param>
        /// <param name="methodPayload">a string containing the method data as a JSON document</param>
        internal Method(string methodName, string methodPayload)
        {
            this.Name = methodName;
            this.Payload = methodPayload;
        }

        /// <summary>
        /// Property indicating the method name for this instance
        /// </summary>
        internal string Name
        {
            get; private set;
        }

        /// <summary>
        /// Property containing entire payload message data.
        /// </summary>
        internal string Payload
        {
            get; private set;
        }

        /// <summary>
        /// contains the response of the device client application method handler.
        /// </summary>
        internal MethodStatusType Status
        {
            get; set;
        }

        /// <summary>
        /// contains the data which resulted from the method call. This property can be null.
        /// </summary>
        internal string Result
        {
            get; set;
        }

        /// <summary>
        /// the request ID for the transport layer
        /// </summary>
        internal string RequestId
        {
            get; set;
        }
    }
}

