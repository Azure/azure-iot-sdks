// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices
{
    /// <summary>
    /// Represents <see cref="Twin"/> properties
    /// </summary>
    public class TwinProperties
    {
        /// <summary>
        /// Initializes a new instance of <see cref="TwinProperties"/>
        /// </summary>
        public TwinProperties()
        {
            this.Desired = new TwinCollection();
            this.Reported = new TwinCollection();
        }

        /// <summary>
        /// Gets and sets the <see cref="Twin"/> desired properties.
        /// </summary>
        public TwinCollection Desired { get; set; }

        /// <summary>
        /// Gets and sets the <see cref="Twin"/> reported properties.
        /// </summary>
        public TwinCollection Reported { get; set; }
    }
}