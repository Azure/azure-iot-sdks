// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client
{
    using System;
    using System.Net;
    using Microsoft.Azure.Devices.Client.Extensions;
#if !NETMF
    using System.Collections.Generic;
    using PCLCrypto;
#endif
    using System.Globalization;
#if WINDOWS_UWP
    using Windows.Security.Cryptography;
    using Windows.Security.Cryptography.Core;
    using System.Runtime.InteropServices.WindowsRuntime;
    using Windows.Storage.Streams;
#endif

    using System.Text;

    sealed class SharedAccessSignatureBuilder
    {
        string key;

        public SharedAccessSignatureBuilder()
        {
#if NETMF
            this.TimeToLive = new TimeSpan(0, 20, 0);
#else
            this.TimeToLive = TimeSpan.FromMinutes(20);
#endif
        }

        public string KeyName { get; set; }

        public string Key
        {
            get
            {
                return this.key;
            }

            set
            {
#if !NETMF
                StringValidationHelper.EnsureBase64String(value, "Key");
#endif
                this.key = value;
            }
        }

        public string Target { get; set; }

        public TimeSpan TimeToLive { get; set; }

        public string ToSignature()
        {
            return BuildSignature(this.KeyName, this.Key, this.Target, this.TimeToLive);
        }

        static string BuildSignature(string keyName, string key, string target, TimeSpan timeToLive)
        {
            string expiresOn = BuildExpiresOn(timeToLive);
            string audience = WebUtility.UrlEncode(target);

#if !NETMF
            List<string> fields = new List<string>();
            fields.Add(audience);
            fields.Add(expiresOn);
#endif

            // Example string to be signed:
            // dh://myiothub.azure-devices.net/a/b/c?myvalue1=a
            // <Value for ExpiresOn>

#if NETMF
            string signature = Sign(audience + "\n" + expiresOn, key);
#else
            string signature = Sign(string.Join("\n", fields), key);
#endif

            // Example returned string:
            // SharedAccessSignature sr=ENCODED(dh://myiothub.azure-devices.net/a/b/c?myvalue1=a)&sig=<Signature>&se=<ExpiresOnValue>[&skn=<KeyName>]

#if NETMF
            var buffer = new StringBuilder();
            buffer.Append(SharedAccessSignatureConstants.SharedAccessSignature + " ");
            buffer.Append(SharedAccessSignatureConstants.AudienceFieldName + "=" + audience);
            buffer.Append("&" + SharedAccessSignatureConstants.SignatureFieldName + "=" + WebUtility.UrlEncode(signature));
            buffer.Append("&" + SharedAccessSignatureConstants.ExpiryFieldName + "=" + WebUtility.UrlEncode(expiresOn));

            if (!keyName.IsNullOrWhiteSpace())
            {
                buffer.Append("&" + SharedAccessSignatureConstants.KeyNameFieldName + "=" + WebUtility.UrlEncode(keyName));
            }
#else
            var buffer = new StringBuilder();
            buffer.AppendFormat(CultureInfo.InvariantCulture, "{0} {1}={2}&{3}={4}&{5}={6}",
                SharedAccessSignatureConstants.SharedAccessSignature,
                SharedAccessSignatureConstants.AudienceFieldName, audience,
                SharedAccessSignatureConstants.SignatureFieldName, WebUtility.UrlEncode(signature),
                SharedAccessSignatureConstants.ExpiryFieldName, WebUtility.UrlEncode(expiresOn));
            
            if (!keyName.IsNullOrWhiteSpace())
            {
                buffer.AppendFormat(CultureInfo.InvariantCulture, "&{0}={1}",
                    SharedAccessSignatureConstants.KeyNameFieldName, WebUtility.UrlEncode(keyName));
            }
#endif

            return buffer.ToString();
        }

        static string BuildExpiresOn(TimeSpan timeToLive)
        {
#if MF_FRAMEWORK_VERSION_V4_3
            // .NETMF < v4.4 had a know bug with DateTime.Kind: values were always created with DateTimeKind.Local 
            // this requires us to perform an extra step to make a DateTime to be in UTC, otherwise the expiry date will be calculated wrongly

            // the 'absolute' value is correct but DateTimeKind is Local (WRONG!)
            DateTime expiresOn = TimeZone.CurrentTimeZone.ToUniversalTime(DateTime.UtcNow.Add(timeToLive));
            TimeSpan secondsFromBaseTime = expiresOn.Subtract(TimeZone.CurrentTimeZone.ToUniversalTime(SharedAccessSignatureConstants.EpochTime));
            return (secondsFromBaseTime.Ticks / TimeSpan.TicksPerSecond).ToString();
#elif MF_FRAMEWORK_VERSION_V4_4
            DateTime expiresOn = DateTime.UtcNow.Add(timeToLive);
            TimeSpan secondsFromBaseTime = expiresOn.Subtract(SharedAccessSignatureConstants.EpochTime);
            return ((uint)(secondsFromBaseTime.Ticks / TimeSpan.TicksPerSecond)).ToString();
#else
            DateTime expiresOn = DateTime.UtcNow.Add(timeToLive);
            TimeSpan secondsFromBaseTime = expiresOn.Subtract(SharedAccessSignatureConstants.EpochTime);
            long seconds = Convert.ToInt64(secondsFromBaseTime.TotalSeconds, CultureInfo.InvariantCulture);
            return Convert.ToString(seconds, CultureInfo.InvariantCulture);
#endif
        }
#if NETMF
        static string Sign(string requestString, string key)
        {
            // computing SHA256 signature using a managed code library
            var hmac = SHA.computeHMAC_SHA256(Convert.FromBase64String(key), Encoding.UTF8.GetBytes(requestString));
            return Convert.ToBase64String(hmac);
        }
#else
        static string Sign(string requestString, string key)
        {
            var algorithm = WinRTCrypto.MacAlgorithmProvider.OpenAlgorithm(MacAlgorithm.HmacSha256);
            var hash = algorithm.CreateHash(Convert.FromBase64String(key));
            hash.Append(Encoding.UTF8.GetBytes(requestString));
            var mac = hash.GetValueAndReset();
            return Convert.ToBase64String(mac);
        }
#endif
    }
}
