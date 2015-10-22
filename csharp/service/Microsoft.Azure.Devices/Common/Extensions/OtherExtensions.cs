// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Common.Extensions
{
    using System.Collections.Generic;
    using System.Linq;
    using System.Net.Http.Headers;

    public static class OtherExtensions
    {
        public static T GetValueOrDefault<T>(this IDictionary<string, object> dictionary, string key)
        {
            object o;
            if (dictionary.TryGetValue(key, out o) && o is T)
            {
                return (T)o;
            }

            return default(T);
        }

        public static string GetFirstValueOrNull(this HttpHeaders headers, string name)
        {
            IEnumerable<string> values = headers.GetValuesOrNull(name) ?? Enumerable.Empty<string>();
            return values.FirstOrDefault();
        }

        public static IEnumerable<string> GetValuesOrNull(this HttpHeaders headers, string name)
        {
            IEnumerable<string> values;
            headers.TryGetValues(name, out values);
            return values;
        }
    }
}
