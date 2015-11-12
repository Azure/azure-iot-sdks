// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client.Extensions
{
    using System;
    using System.Collections.Generic;
    using System.Globalization;
    using System.Linq;
    using System.Net;
    using System.Net.Http;
    using System.Net.Sockets;
    using System.Text;
    using System.Text.RegularExpressions;
#if !WINDOWS_UWP // Owin NuGet package is not compatible with UAP
    using Microsoft.Owin;
#endif

    delegate bool TryParse<TInput, TOutput>(TInput input, bool ignoreCase, out TOutput output);

    static class CommonExtensionMethods
    {
        const char ValuePairDelimiter = ';';
        const char ValuePairSeparator = '=';

        public static string EnsureStartsWith(this string value, char prefix)
        {
            if (value == null)
            {
                throw new ArgumentNullException("value");
            }

            if (value.Length == 0)
            {
                return prefix.ToString();
            }
            else
            {
                return value[0] == prefix ? value : prefix + value;
            }
        }

        public static string GetValueOrDefault(this IDictionary<string, string> map, string keyName)
        {
            string value;
            if (!map.TryGetValue(keyName, out value))
            {
                value = null;
            }

            return value;
        }

        public static IDictionary<string, string> ToDictionary(this string valuePairString, char kvpDelimiter, char kvpSeparator)
        {
            if (string.IsNullOrWhiteSpace(valuePairString))
            {
                throw new ArgumentException("Malformed Token");
            }

            IEnumerable<string[]> parts = valuePairString
                .Split(kvpDelimiter)
                .Where(p => p.Trim().Length > 0)
                .Select(p => p.Split(new char[] { kvpSeparator }, 2));

            if (parts.Any(p => p.Length != 2))
            {
                throw new FormatException("Malformed Token");
            }

            IDictionary<string, string> map = parts.ToDictionary(kvp => kvp[0], (kvp) => kvp[1], StringComparer.OrdinalIgnoreCase);

            return map;
        }

        public static bool TryGetIotHubName(this HttpRequestMessage requestMessage, out string iotHubName)
        {
            iotHubName = null;

            // header overrides the domain name, this is basically to help with testing so we don't have
            // to mess with the hosts file
            if (requestMessage.Headers.Contains(CustomHeaderConstants.HttpIotHubName))
            {
                iotHubName = requestMessage.Headers.GetValues(CustomHeaderConstants.HttpIotHubName).FirstOrDefault();
            }
            else
            {
                // {IotHubname}.[env-specific-sub-domain.]IotHub[-int].net
                //
                string[] hostNameParts = requestMessage.RequestUri.Host.Split('.');
                if (hostNameParts.Length < 3)
                {
                    return false;
                }
                iotHubName = hostNameParts[0];
            }

            return true;
        }

        public static string GetIotHubName(this HttpRequestMessage requestMessage)
        {
            string iotHubName = null;

            if (!TryGetIotHubName(requestMessage, out iotHubName))
            {
                throw new ArgumentException("Invalid request URI");
            }

            return iotHubName;
        }

#if !WINDOWS_UWP // Depends on Owin, not supported for UWP
        public static string GetMaskedClientIpAddress(this HttpRequestMessage requestMessage)
        {
            // note that this only works if we are hosted as an OWIN app
            if (requestMessage.Properties.ContainsKey("MS_OwinContext"))
            {
                OwinContext owinContext = requestMessage.Properties["MS_OwinContext"] as OwinContext;
                if (owinContext != null)
                {
                    string remoteIpAddress = owinContext.Request.RemoteIpAddress;

                    string maskedRemoteIpAddress = string.Empty;

                    IPAddress remoteIp = null;
                    IPAddress.TryParse(remoteIpAddress, out remoteIp);

                    if (null != remoteIp)
                    {
                        byte[] addressBytes = remoteIp.GetAddressBytes();
                        if (remoteIp.AddressFamily == AddressFamily.InterNetwork)
                        {
                            addressBytes[addressBytes.Length - 1] = 0xFF;
                            addressBytes[addressBytes.Length - 2] = 0xFF;
                            maskedRemoteIpAddress = new IPAddress(addressBytes).ToString();
                        }
                        else if (remoteIp.AddressFamily == AddressFamily.InterNetworkV6)
                        {
                            addressBytes[addressBytes.Length - 1] = 0xFF;
                            addressBytes[addressBytes.Length - 2] = 0xFF;
                            addressBytes[addressBytes.Length - 3] = 0xFF;
                            addressBytes[addressBytes.Length - 4] = 0xFF;
                            maskedRemoteIpAddress = new IPAddress(addressBytes).ToString();
                        }

                    }

                    return maskedRemoteIpAddress;
                }
            }
            return null;
        }
#endif

        public static void AppendKeyValuePairIfNotEmpty(this StringBuilder builder, string name, object value)
        {
            if (value != null)
            {
                builder.Append(name);
                builder.Append(ValuePairSeparator);
                builder.Append(value);
                builder.Append(ValuePairDelimiter);
            }
        }

        public static bool IsNullOrWhiteSpace(this string value)
        {
            return string.IsNullOrWhiteSpace(value);
        }

        public static string RemoveWhitespace(this string value)
        {
            return new string(value.Where(c => !char.IsWhiteSpace(c)).ToArray());
        }
    }
}
