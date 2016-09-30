// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Converters
{
    using System;
    using System.Collections.Generic;
    using System.Reflection;
    using Newtonsoft.Json;
    using Newtonsoft.Json.Linq;

    /// <summary>
    /// Converts <see cref="Twin"/> to Json
    /// </summary>
    public sealed class TwinJsonConverter : JsonConverter
    {
        const string DeviceIdJsonTag = "deviceId";
        const string ETagJsonTag = "etag";
        const string TagsJsonTag = "tags";
        const string PropertiesJsonTag = "properties";
        const string DesiredPropertiesJsonTag = "desired";
        const string ReportedPropertiesJsonTag = "reported";

        public override void WriteJson(JsonWriter writer, object value, JsonSerializer serializer)
        {
            if (value == null)
            {
                return;
            }

            Twin twin = value as Twin;

            if (twin == null)
            {
                throw new InvalidOperationException("Object passed is not of type Twin.");
            }

            writer.WriteStartObject();

            writer.WritePropertyName(DeviceIdJsonTag);
            writer.WriteValue(twin.DeviceId);

            writer.WritePropertyName(ETagJsonTag);
            writer.WriteValue(twin.ETag);

            if (twin.Tags != null && twin.Tags.Count > 0)
            {
                writer.WritePropertyName(TagsJsonTag);
                serializer.Serialize(writer, twin.Tags, typeof(IDictionary<string, object>));
            }

            if (twin.Properties?.Desired != null || twin.Properties?.Reported != null)
            {
                writer.WritePropertyName(PropertiesJsonTag);
                writer.WriteStartObject();
                if (twin.Properties.Desired != null)
                {
                    writer.WritePropertyName(DesiredPropertiesJsonTag);
                    serializer.Serialize(writer, twin.Properties.Desired, typeof(TwinCollection));
                }

                if (twin.Properties.Reported != null)
                {
                    writer.WritePropertyName(ReportedPropertiesJsonTag);
                    serializer.Serialize(writer, twin.Properties.Reported, typeof(TwinCollection));
                }

                writer.WriteEndObject();
            }

            writer.WriteEndObject();
        }

        public override object ReadJson(JsonReader reader, Type objectType, object existingValue, JsonSerializer serializer)
        {
            var twin = new Twin();

            if (reader.TokenType != JsonToken.StartObject)
            {
                return null;
            }

            while (reader.Read())
            {
                if (reader.TokenType == JsonToken.EndObject)
                {
                    break;
                }

                if (reader.TokenType != JsonToken.PropertyName)
                {
                    // TODO: validate that this code is not reached.
                    continue;
                }

                string propertyName = reader.Value as string;
                reader.Read();

                switch (propertyName)
                {
                    case DeviceIdJsonTag:
                        twin.DeviceId = reader.Value as string;
                        break;
                    case ETagJsonTag:
                        twin.ETag = reader.Value as string;
                        break;
                    case TagsJsonTag:
                        if (reader.TokenType != JsonToken.StartObject)
                        {
                            throw new InvalidOperationException("Tags Json not a Dictionary.");
                        }
                        twin.Tags = new TwinCollection(JToken.ReadFrom(reader) as JObject);
                        break;
                    case PropertiesJsonTag:
                        PopulatePropertiesForTwin(twin, reader, serializer);
                        break;
                    default:
                        break;
                }
            }

            return twin;
        }

        public override bool CanRead => true;

        public override bool CanWrite => true;

        public override bool CanConvert(Type objectType) => typeof(Twin).IsAssignableFrom(objectType);


        private static Dictionary<string, object> GetTagsForTwin(JsonReader reader)
        {
            if (reader.TokenType != JsonToken.StartObject)
            {
                return null;
            }

            Dictionary<string, object> dict = new Dictionary<string, object>();
            while (reader.Read())
            {
                if (reader.TokenType == JsonToken.EndObject)
                {
                    break;
                }

                string propertyName = reader.Value as string;
                reader.Read();

                if (reader.TokenType != JsonToken.StartObject)
                {
                    dict.Add(propertyName, reader.Value);
                }
                else
                {
                    dict.Add(propertyName, GetTagsForTwin(reader));
                }
            }

            return dict;
        }

        private static void PopulatePropertiesForTwin(Twin twin, JsonReader reader, JsonSerializer serializer)
        {
            if (twin == null)
            {
                throw new InvalidOperationException("Twin object null.");
            }

            if (reader.TokenType != JsonToken.StartObject)
            {
                twin.Properties.Desired = null;
                twin.Properties.Reported = null;
                return;
            }

            while (reader.Read())
            {
                if (reader.TokenType == JsonToken.EndObject)
                {
                    break;
                }

                string propertyName = reader.Value as string;
                reader.Read();

                switch (propertyName)
                {
                    case DesiredPropertiesJsonTag:
                        twin.Properties.Desired = new TwinCollection(JToken.ReadFrom(reader) as JObject);
                        break;
                    case ReportedPropertiesJsonTag:
                        twin.Properties.Reported = new TwinCollection(JToken.ReadFrom(reader) as JObject);
                        break;
                    default:
                        break;
                }
            }
        }
    }
}