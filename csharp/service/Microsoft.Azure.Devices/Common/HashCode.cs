// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Common
{
    static class HashCode
    {
        public static int CombineHashCodes(int h1, int h2)
        {
            return ((h1 << 5) + h1) ^ h2;
        }

        public static int CombineHashCodes(int h1, int h2, int h3)
        {
            return CombineHashCodes(CombineHashCodes(h1, h2), h3);
        }

        public static int CombineHashCodes(int h1, int h2, int h3, int h4)
        {
            return CombineHashCodes(CombineHashCodes(h1, h2), CombineHashCodes(h3, h4));
        }

        public static int SafeGet<T>(T value)
            where T : class
        {
            if (value != null)
            {
                return value.GetHashCode();
            }

            return 0;
        }

        ////public static int CombineHashCodes(int h1, int h2, int h3, int h4, int h5)
        ////{
        ////    return CombineHashCodes(CombineHashCodes(h1, h2, h3, h4), h5);
        ////}

        ////public static int CombineHashCodes(int h1, int h2, int h3, int h4, int h5, int h6)
        ////{
        ////    return CombineHashCodes(CombineHashCodes(h1, h2, h3, h4), CombineHashCodes(h5, h6));
        ////}
                 
        public static int CombineHashCodes(int h1, int h2, int h3, int h4, int h5, int h6, int h7)
        {
            return CombineHashCodes(CombineHashCodes(h1, h2, h3, h4), CombineHashCodes(h5, h6, h7));
        }

        ////public static int CombineHashCodes(int h1, int h2, int h3, int h4, int h5, int h6, int h7, int h8)
        ////{
        ////    return CombineHashCodes(CombineHashCodes(h1, h2, h3, h4), CombineHashCodes(h5, h6, h7, h8));
        ////}
    }
}
