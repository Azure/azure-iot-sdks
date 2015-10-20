// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client
{
    using Microsoft.Azure.Devices.Client.Extensions;
    using System;
    using System.Text;

    sealed class SharedAccessSignatureBuilder
    {
        string key;

        public SharedAccessSignatureBuilder()
        {
            this.TimeToLive = new TimeSpan(0, 20, 0);
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
            string audience = HttpUtility.UrlEncode(target);

            // Example string to be signed:
            // dh://myiothub.azure-devices.net/a/b/c?myvalue1=a
            // <Value for ExpiresOn>

            string signature = Sign(audience + "\n" + expiresOn, key);

            // Example returned string:
            // SharedAccessSignature sr=ENCODED(dh://myiothub.azure-devices.net/a/b/c?myvalue1=a)&sig=<Signature>&se=<ExpiresOnValue>[&skn=<KeyName>]

            var buffer = new StringBuilder();
            buffer.Append(SharedAccessSignatureConstants.SharedAccessSignature + " ");
            buffer.Append(SharedAccessSignatureConstants.AudienceFieldName + "=" + audience);
            buffer.Append("&" + SharedAccessSignatureConstants.SignatureFieldName + "=" + HttpUtility.UrlEncode(signature));
            buffer.Append("&" + SharedAccessSignatureConstants.ExpiryFieldName + "=" + HttpUtility.UrlEncode(expiresOn));

            if (!keyName.IsNullOrWhiteSpace())
            {
                buffer.Append("&" + SharedAccessSignatureConstants.KeyNameFieldName + "=" + HttpUtility.UrlEncode(keyName));
            }

            return buffer.ToString();
        }

        static string BuildExpiresOn(TimeSpan timeToLive)
        {
            const long ticksPerSecond = 1000000000 / 100; // NetMF: 1 tick = 100 nano seconds

            // .NETMF < v4.4 had a know bug with DateTime.Kind: values were always created with DateTimeKind.Local 
            // this requires us to perform an extra step to make a DateTime to be in UTC, otherwise the expiry date will be calculated wrongly

#if MF_FRAMEWORK_VERSION_V4_2 || MF_FRAMEWORK_VERSION_V4_3
            // the 'absolute' value is correct but DateTimeKind is Local (WRONG!)
            DateTime expiresOn = TimeZone.CurrentTimeZone.ToUniversalTime(DateTime.UtcNow.Add(timeToLive));
            TimeSpan secondsFromBaseTime = expiresOn.Subtract(TimeZone.CurrentTimeZone.ToUniversalTime(SharedAccessSignatureConstants.EpochTime));
            return (secondsFromBaseTime.Ticks / ticksPerSecond).ToString();
#else
            DateTime expiresOn = DateTime.UtcNow.Add(timeToLive);
            TimeSpan secondsFromBaseTime = expiresOn.Subtract(SharedAccessSignatureConstants.EpochTime);
            return((uint)(secondsFromBaseTime.Ticks / ticksPerSecond)).ToString(); 
            return (secondsFromBaseTime.Ticks / ticksPerSecond).ToString();
#endif
        }

        static string Sign(string requestString, string key)
        {
            // computing SHA256 signature using a managed code library
            var hmac = ElzeKool.SHA.computeHMAC_SHA256(Convert.FromBase64String(key), Encoding.UTF8.GetBytes(requestString));
            return Convert.ToBase64String(hmac);

            // computing SHA256 signature using the .NET Micro Framework classes
            // this requires that the appropriate assemblies are compiled and available in the .NETMF build it's being used
            // required assemblies are: Microsoft.SPOT.Cryptoki and System.Security.Cryptography
            //using (Microsoft.SPOT.Cryptoki.Session openSession = new Microsoft.SPOT.Cryptoki.Session("", Microsoft.SPOT.Cryptoki.MechanismType.SHA256_HMAC))
            //{
            //    Microsoft.SPOT.Cryptoki.CryptokiAttribute[] secretKey = new Microsoft.SPOT.Cryptoki.CryptokiAttribute[] 
            //    { 
            //        new Microsoft.SPOT.Cryptoki.CryptokiAttribute(Microsoft.SPOT.Cryptoki.CryptokiAttribute.CryptokiType.Class, Microsoft.SPOT.Cryptoki.Utility.ConvertToBytes((int)Microsoft.SPOT.Cryptoki.CryptokiClass.SECRET_KEY)),
            //        new Microsoft.SPOT.Cryptoki.CryptokiAttribute(Microsoft.SPOT.Cryptoki.CryptokiAttribute.CryptokiType.KeyType, Microsoft.SPOT.Cryptoki.Utility.ConvertToBytes((int)System.Security.Cryptography.CryptoKey.KeyType.GENERIC_SECRET)),
            //        new Microsoft.SPOT.Cryptoki.CryptokiAttribute(Microsoft.SPOT.Cryptoki.CryptokiAttribute.CryptokiType.Value, Convert.FromBase64String(key)) 
            //    }; 

            //    System.Security.Cryptography.CryptoKey binKey = System.Security.Cryptography.CryptoKey.LoadKey(openSession, secretKey);

            //    using (System.Security.Cryptography.KeyedHashAlgorithm hmacOpenSSL = new System.Security.Cryptography.KeyedHashAlgorithm(System.Security.Cryptography.KeyedHashAlgorithmType.HMACSHA256, binKey))
            //    {
            //        byte[] hmac1 = hmacOpenSSL.ComputeHash(Encoding.UTF8.GetBytes(requestString));
            //        return Convert.ToBase64String(hmac1);
            //    }
            //}
        }
    }
}
