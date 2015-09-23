// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client
{
    using System;
    using System.Net;
    using System.Collections.Generic;
    using System.Globalization;
#if WINDOWS_UWP
    using Windows.Security.Cryptography;
    using Windows.Security.Cryptography.Core;
    using System.Runtime.InteropServices.WindowsRuntime;
    using Windows.Storage.Streams;
#else
    using System.Security.Cryptography;
#endif
    using System.Text;
    

    sealed class SharedAccessSignatureBuilder
    {
        string key;

        public SharedAccessSignatureBuilder()
        {
            this.TimeToLive = TimeSpan.FromMinutes(20);
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
                StringValidationHelper.EnsureBase64String(value, "Key");
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
            List<string> fields = new List<string>();
            fields.Add(audience);
            fields.Add(expiresOn);

            // Example string to be signed:
            // dh://myiothub.azure-devices.net/a/b/c?myvalue1=a
            // <Value for ExpiresOn>

            string signature = Sign(string.Join("\n", fields), key);

            // Example returned string:
            // SharedAccessSignature sr=ENCODED(dh://myiothub.azure-devices.net/a/b/c?myvalue1=a)&sig=<Signature>&se=<ExpiresOnValue>[&skn=<KeyName>]

            var buffer = new StringBuilder();
            buffer.AppendFormat(CultureInfo.InvariantCulture, "{0} {1}={2}&{3}={4}&{5}={6}",
                SharedAccessSignatureConstants.SharedAccessSignature,
                SharedAccessSignatureConstants.AudienceFieldName, audience,
                SharedAccessSignatureConstants.SignatureFieldName, WebUtility.UrlEncode(signature),
                SharedAccessSignatureConstants.ExpiryFieldName, WebUtility.UrlEncode(expiresOn));
            
            if (!string.IsNullOrEmpty(keyName))
            {
                buffer.AppendFormat(CultureInfo.InvariantCulture, "&{0}={1}",
                    SharedAccessSignatureConstants.KeyNameFieldName, WebUtility.UrlEncode(keyName));
            }

            return buffer.ToString();
        }

        static string BuildExpiresOn(TimeSpan timeToLive)
        {
            DateTime expiresOn = DateTime.UtcNow.Add(timeToLive);
            TimeSpan secondsFromBaseTime = expiresOn.Subtract(SharedAccessSignatureConstants.EpochTime);
            long seconds = Convert.ToInt64(secondsFromBaseTime.TotalSeconds, CultureInfo.InvariantCulture);
            return Convert.ToString(seconds, CultureInfo.InvariantCulture);
        }

#if WINDOWS_UWP
        static string Sign(string requestString, string key)
        {
            var algo = MacAlgorithmProvider.OpenAlgorithm(MacAlgorithmNames.HmacSha256);
            var keyMaterial = Convert.FromBase64String(key).AsBuffer();
            var hash = algo.CreateHash(keyMaterial);
            hash.Append(CryptographicBuffer.ConvertStringToBinary(requestString, BinaryStringEncoding.Utf8));

            var sign = CryptographicBuffer.EncodeToBase64String(hash.GetValueAndReset());
            return sign;
        }
#else
        static string Sign(string requestString, string key)
        {
            using (var hmac = new HMACSHA256(Convert.FromBase64String(key)))
            {
                var sign = Convert.ToBase64String(hmac.ComputeHash(Encoding.UTF8.GetBytes(requestString)));
                return sign;
            }
        }
#endif
    }
}
