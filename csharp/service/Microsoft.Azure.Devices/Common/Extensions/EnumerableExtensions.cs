// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Common
{
    using System;
    using System.Collections;
    using System.Collections.Generic;
    using System.Linq;

    static class EnumerableExtensions
    {
        /// <summary>
        /// Avoid allocating if possible (if ICollection is supported)
        /// </summary>
        public static bool AnySlim<T>(this IEnumerable<T> enumerable)
        {
            var collection = enumerable as ICollection;
            if (collection != null)
            {
                return collection.Count != 0;
            }

            return enumerable.Any();
        }

        /// <summary>
        /// If the underlying type already supports an IList implementation then don't allocate a new List.
        /// </summary>
        public static IList<T> ToListSlim<T>(this IEnumerable<T> enumerable, bool requireMutable = false)
        {
            var list = enumerable as IList<T>;
            if (list != null && !(requireMutable && list.IsReadOnly))
            {
                return list;
            }

            return enumerable.ToList();
        }
    }
}
