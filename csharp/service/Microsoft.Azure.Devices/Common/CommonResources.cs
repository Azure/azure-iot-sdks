// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Common
{
    sealed class CommonResources : Microsoft.Azure.Devices.Common.Resources
    {
        internal static string GetString(string value, params object[] args)
        {
            if (args != null && args.Length > 0)
            {
                for (int i = 0; i < args.Length; i++)
                {
                    string text = args[i] as string;
                    if (text != null && text.Length > 1024)
                    {
                        args[i] = text.Substring(0, 1021) + "...";
                    }
                }

                return string.Format(CommonResources.Culture, value, args);
            }

            return value;
        } 
    }
}
