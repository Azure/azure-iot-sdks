// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices
{
    using System;
    using Microsoft.CSharp.RuntimeBinder;
    using Newtonsoft.Json.Linq;

    /// <summary>
    /// Represents a property value in <see cref="TwinCollection"/>
    /// </summary>
    public class TwinCollectionValue : JValue
    {
        const string metadataName = "$metadata";
        const string lastUpdatedName = "$lastUpdated";
        const string lastUpdatedVersionName = "$lastUpdatedVersion";

        private readonly JObject metadata;

        internal TwinCollectionValue(JValue jValue, JObject metadata)
            : base(jValue)
        {
            this.metadata = metadata;
        }

        /// <summary>
        /// Gets the value for the given property name
        /// </summary>
        /// <param name="propertyName">Property Name to lookup</param>
        /// <returns>Value if present</returns>
        public dynamic this[string propertyName]
        {
            get
            {
                if (propertyName == metadataName)
                {
                    return this.GetMetadata();
                }

                if (propertyName == lastUpdatedName)
                {
                    return this.GetLastUpdated();
                }

                if (propertyName == lastUpdatedVersionName)
                {
                    return this.GetLastUpdatedVersion();
                }

                throw new RuntimeBinderException($"'Newtonsoft.Linq.JValue' does not contain a definition for '{propertyName}'.");
            }
        }

        /// <summary>
        /// Gets the Metadata for this property
        /// </summary>
        /// <returns>Metadata instance representing the metadata for this property</returns>
        public Metadata GetMetadata()
        {
            return new Metadata(this.GetLastUpdated(), this.GetLastUpdatedVersion());
        }

        /// <summary>
        /// Gets the LastUpdated time for this property
        /// </summary>
        /// <returns>DateTime instance representing the LastUpdated time for this property</returns>
        public DateTime GetLastUpdated()
        {
            return (DateTime)this.metadata[lastUpdatedName];
        }

        /// <summary>
        /// Gets the LastUpdatedVersion for this property
        /// </summary>
        /// <returns>LastUpdatdVersion if present, null otherwise</returns>
        public long? GetLastUpdatedVersion()
        {
            return (long?)this.metadata[lastUpdatedVersionName];
        }
    }
}