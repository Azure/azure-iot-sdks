// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client
{
    using Microsoft.Azure.Devices.Client.Extensions;
    using Microsoft.SPOT.Cryptoki;
    using System;
    using System.Security.Cryptography;
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

            DateTime expiresOn = DateTime.UtcNow.Add(timeToLive);
            TimeSpan secondsFromBaseTime = expiresOn.Subtract(SharedAccessSignatureConstants.EpochTime);
            return((uint)(secondsFromBaseTime.Ticks / ticksPerSecond)).ToString(); 
        }

        static string Sign(string requestString, string key)
        {
            using (Session openSession = new Session("", MechanismType.SHA256_HMAC))
            {
                CryptokiAttribute[] secretKey = new CryptokiAttribute[] 
                { 
                    new CryptokiAttribute(CryptokiAttribute.CryptokiType.Class, Utility.ConvertToBytes((int)CryptokiClass.SECRET_KEY)),
                    new CryptokiAttribute(CryptokiAttribute.CryptokiType.KeyType, Utility.ConvertToBytes((int)CryptoKey.KeyType.GENERIC_SECRET)),
                    new CryptokiAttribute(CryptokiAttribute.CryptokiType.Value, Convert.FromBase64String(key)) 
                }; 

                CryptoKey binKey = CryptoKey.LoadKey(openSession, secretKey);

                using (KeyedHashAlgorithm hmacOpenSSL = new KeyedHashAlgorithm(KeyedHashAlgorithmType.HMACSHA256, binKey))
                {
                    byte[] hmac1 = hmacOpenSSL.ComputeHash(Encoding.UTF8.GetBytes(requestString));
                    return Convert.ToBase64String(hmac1);
                }
            }
        }
    }
}
