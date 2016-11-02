// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices
{
    using System;
    using System.Collections;
    using System.Collections.Generic;
    using System.Dynamic;
    using Microsoft.Azure.Devices.Converters;
    using Newtonsoft.Json;
    using Newtonsoft.Json.Linq;

    /// <summary>
    /// Represents a collection of properties for <see cref="Twin"/>
    /// </summary>
    [JsonConverter(typeof(TwinCollectionJsonConverter))]
    public class TwinCollection : IEnumerable
    {
        private const string MetadataName = "$metadata";
        private const string LastUpdatedName = "$lastUpdated";
        private const string LastUpdatedVersionName = "$lastUpdatedVersion";
        private const string VersionName = "$version";

        private JObject jObject;
        private JObject metadata;

        /// <summary>
        /// Creates instance of <see cref="TwinCollection"/>
        /// </summary>
        public TwinCollection()
            : this(null)
        {
        }

        internal TwinCollection(JObject jObject)
        {
            this.jObject = jObject ?? new JObject();

            JToken metadataJToken;
            if (this.jObject.TryGetValue(MetadataName, out metadataJToken))
            {
                this.metadata = metadataJToken as JObject;
            }
        }

        private TwinCollection(JObject jObject, JObject metadata)
        {
            this.jObject = jObject ?? new JObject();
            this.metadata = metadata;
        }

        /// <summary>
        /// Gets the version of the <see cref="TwinCollection"/>
        /// </summary>
        public long Version
        {
            get
            {
                JToken versionToken;
                if (!this.jObject.TryGetValue(VersionName, out versionToken))
                {
                    return default(long);
                }

                return (long)versionToken;
            }
        }

        /// <summary>
        /// Gets the count of properties in the Collection
        /// </summary>
        public int Count
        {
            get
            {
                int count = this.jObject.Count;
                if (count > 0)
                {
                    // Metadata and Version should not count towards this value
                    JToken ignored;
                    if (this.jObject.TryGetValue(MetadataName, out ignored))
                    {
                        count--;
                    }

                    if (this.jObject.TryGetValue(VersionName, out ignored))
                    {
                        count--;
                    }
                }

                return count;
            }
        }

        internal JObject JObject => this.jObject;

        /// <summary>
        /// Property Indexer
        /// </summary>
        /// <param name="propertyName">Name of the property to get</param>
        /// <returns>Value for the given proprety name</returns>
        public dynamic this[string propertyName]
        {
            get
            {
                dynamic value;
                if (propertyName == MetadataName)
                {
                    return this.GetMetadata();
                }
                else if (propertyName == LastUpdatedName)
                {
                    return this.GetLastUpdated();
                }
                else if (propertyName == LastUpdatedVersionName)
                {
                    return this.GetLastUpdatedVersion();
                }
                else if (this.TryGetMemberInternal(propertyName, out value))
                {
                    return value;
                }
                else
                {
                    throw new ArgumentOutOfRangeException(nameof(propertyName));
                }
            }
            set { this.TrySetMemberInternal(propertyName, value); }
        }

        /// <inheritdoc />
        public override string ToString()
        {
            return this.jObject.ToString();
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
            return (DateTime)this.metadata[LastUpdatedName];
        }

        /// <summary>
        /// Gets the LastUpdatedVersion for this property
        /// </summary>
        /// <returns>LastUpdatdVersion if present, null otherwise</returns>
        public long? GetLastUpdatedVersion()
        {
            return (long?)this.metadata[LastUpdatedVersionName];
        }

        /// <summary>
        /// Gets the TwinProperties as a JSON string
        /// </summary>
        /// <param name="formatting">Optional. Formatting for the output JSON string.</param>
        /// <returns>JSON string</returns>
        public string ToJson(Formatting formatting = Formatting.None)
        {
            return JsonConvert.SerializeObject(this.jObject, formatting);
        }

        /// <summary>
        /// Determines whether the specified property is present
        /// </summary>
        /// <param name="propertyName">The property to locate</param>
        /// <returns>true if the specified property is present; otherwise, false</returns>
        public bool Contains(string propertyName)
        {
            JToken ignored;
            return this.jObject.TryGetValue(propertyName, out ignored);
        }

        /// <inheritdoc />
        public IEnumerator GetEnumerator()
        {
            foreach (KeyValuePair<string, JToken> kvp in this.jObject)
            {
                if (kvp.Key == MetadataName || kvp.Key == VersionName)
                {
                    continue;
                }

                yield return new KeyValuePair<string, dynamic>(kvp.Key, this[kvp.Key]);
            }
        }

        private bool TryGetMemberInternal(string propertyName, out object result)
        {
            JToken value;
            if (!this.jObject.TryGetValue(propertyName, out value))
            { 
                result = null;
                return false;
            }
            
            if (this.metadata?[propertyName] is JObject)
            {
                if (value is JValue)
                {
                    result = new TwinCollectionValue((JValue)value, (JObject)this.metadata[propertyName]);
                }
                else
                {
                    result = new TwinCollection(value as JObject, (JObject)this.metadata[propertyName]);
                }
            }
            else
            {
                // No metadata for this property, return as-is.
                result = value;
            }

            return true;
        }

        private bool TrySetMemberInternal(string propertyName, object value)
        {
            JToken valueJToken =  value == null ? null : JToken.FromObject(value);
            JToken ignored;
            if (this.jObject.TryGetValue(propertyName, out ignored))
            {
                this.jObject[propertyName] = valueJToken;
            }
            else
            {
                this.jObject.Add(propertyName, valueJToken);
            }

            return true;
        }
    }
}