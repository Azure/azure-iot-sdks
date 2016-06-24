// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.QueryExpressions
{
    using System;
    using System.Collections.Generic;
    using Microsoft.Azure.Devices.Common;
    using Newtonsoft.Json;
    using Newtonsoft.Json.Linq;
    using System.Reflection;
    internal class FilterExpressionConverter : JsonConverter
    {
        const string TypePropertyName = "type";
        const string FiltersPropertyName = "filters";
        const string LogicalExpressionTypeString = "logical";

        public override void WriteJson(JsonWriter writer, object value, JsonSerializer serializer)
        {
            serializer.Serialize(writer, value);
        }

        public override object ReadJson(JsonReader reader, Type objectType, object existingValue, JsonSerializer serializer)
        {
            if (reader.TokenType == JsonToken.Null)
            {
                return null;
            }

            JObject jObject = JObject.Load(reader);
            return this.ReadFilterExpression(jObject, serializer);
        }

        public override bool CanConvert(Type objectType)
        {
            return typeof(FilterExpression).IsAssignableFrom(objectType);
        }

        FilterExpression ReadFilterExpression(JToken jToken, JsonSerializer serializer)
        {
            if (jToken == null)
            {
                throw new ArgumentNullException(nameof(jToken));
            }

            var filterExpressionType = GetValue(jToken, TypePropertyName);
            if (filterExpressionType == null)
            {
                throw new ArgumentException(ApiResources.ParameterCannotBeNullOrEmpty.FormatInvariant("filter.type"));
            }

            if (filterExpressionType.ToString().ToLowerInvariant() == LogicalExpressionTypeString)
            {
                return this.ReadLogicalExpression(jToken, serializer);
            }
            else
            {
                return this.ReadComparisonExpression(jToken, serializer);
            }
        }

        LogicalExpression ReadLogicalExpression(JToken jToken, JsonSerializer serializer)
        {
            if (jToken == null)
            {
                throw new ArgumentNullException(nameof(jToken));
            }

            var target = jToken.ToObject<LogicalExpression>();
            target.FilterExpressions = new List<FilterExpression>();

            var filterTokens = GetValue(jToken, FiltersPropertyName);
            if (filterTokens == null)
            {
                return target;
            }

            IEnumerable<JToken> filters = ((JToken)filterTokens).Values<JToken>();
            foreach (JToken filter in filters)
            {
                target.FilterExpressions.Add(this.ReadFilterExpression(filter, serializer));
            }

            return target;
        }

        ComparisonExpression ReadComparisonExpression(JToken jToken, JsonSerializer serializer)
        {
            if (jToken == null)
            {
                throw new ArgumentNullException(nameof(jToken));
            }

            var target = new ComparisonExpression();
            serializer.Populate(jToken.CreateReader(), target);

            var arrayContent = target.Value as JArray;
            if (arrayContent != null)
            {
                var list = new List<object>();
                foreach (JToken j in arrayContent)
                {
                    var jv = j as JValue;
                    if (jv != null)
                    {
                        list.Add(jv.Value);
                    }
                }
                target.Value = list;
            }

            return target;
        }

        private static object GetValue(JToken jToken, string tokenName)
        {
            object value = null;
            var tokenDictionary = new Dictionary<string, object>(jToken.ToObject<IDictionary<string, object>>(), StringComparer.OrdinalIgnoreCase);
            tokenDictionary.TryGetValue(tokenName, out value);
            return value;
        }
    }
}
