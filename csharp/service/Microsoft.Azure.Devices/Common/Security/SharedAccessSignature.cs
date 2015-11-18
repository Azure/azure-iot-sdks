// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Common.Security
{
    using System;
    using System.Collections.Generic;
    using System.Globalization;
    using System.Security.Cryptography;
    using System.Text;
    using System.Web;

    using Microsoft.Azure.Devices.Common.Data;
    
    public sealed class SharedAccessSignature : ISharedAccessSignatureCredential
    {
        readonly string iotHubName;
        readonly string signature;
        readonly string audience;
        readonly string encodedAudience;
        readonly string expiry;
        readonly string keyName;

        SharedAccessSignature(string iotHubName, DateTime expiresOn, string expiry, string keyName, string signature, string encodedAudience)
        {
            if (string.IsNullOrWhiteSpace(iotHubName))
            {
                throw new ArgumentNullException("iotHubName");
            }

            this.ExpiresOn = expiresOn;

            if (this.IsExpired())
            {
                throw new UnauthorizedAccessException("The specified SAS token is expired");
            }

            this.iotHubName = iotHubName;
            this.signature = signature;
            this.audience = HttpUtility.UrlDecode(encodedAudience);
            this.encodedAudience = encodedAudience;
            this.expiry = expiry;
            this.keyName = keyName ?? string.Empty;
        }

        public string IotHubName
        {
            get
            {
                return this.iotHubName;
            }
        }

        public DateTime ExpiresOn
        {
            get;
            private set;
        }

        public string KeyName
        {
            get
            {
                return this.keyName;
            }
        }

        public string Audience
        {
            get
            {
                return this.audience;
            }
        }

        public string Signature
        {
            get
            {
                return this.signature;
            }
        }

        public static SharedAccessSignature Parse(string iotHubName, string rawToken)
        {
            if (string.IsNullOrWhiteSpace(iotHubName))
            {
                throw new ArgumentNullException("iotHubName");
            }

            if (string.IsNullOrWhiteSpace(rawToken))
            {
                throw new ArgumentNullException("rawToken");
            }

            IDictionary<string, string> parsedFields = ExtractFieldValues(rawToken);

            string signature;
            if (!parsedFields.TryGetValue(SharedAccessSignatureConstants.SignatureFieldName, out signature))
            {
                throw new FormatException(string.Format(CultureInfo.InvariantCulture, "Missing field: {0}", SharedAccessSignatureConstants.SignatureFieldName));
            }

            string expiry;
            if (!parsedFields.TryGetValue(SharedAccessSignatureConstants.ExpiryFieldName, out expiry))
            {
                throw new FormatException(string.Format(CultureInfo.InvariantCulture, "Missing field: {0}", SharedAccessSignatureConstants.ExpiryFieldName));
            }

            // KeyName (skn) is optional.
            string keyName;
            parsedFields.TryGetValue(SharedAccessSignatureConstants.KeyNameFieldName, out keyName);

            string encodedAudience;
            if (!parsedFields.TryGetValue(SharedAccessSignatureConstants.AudienceFieldName, out encodedAudience))
            {
                throw new FormatException(string.Format(CultureInfo.InvariantCulture, "Missing field: {0}", SharedAccessSignatureConstants.AudienceFieldName));
            }

            return new SharedAccessSignature(iotHubName, SharedAccessSignatureConstants.EpochTime + TimeSpan.FromSeconds(double.Parse(expiry, CultureInfo.InvariantCulture)), expiry, keyName, signature, encodedAudience);
        }

        public static bool IsSharedAccessSignature(string rawSignature)
        {
            if (string.IsNullOrWhiteSpace(rawSignature))
            {
                return false;
            }

            IDictionary<string, string> parsedFields = ExtractFieldValues(rawSignature);
            string signature;
            bool isSharedAccessSignature = parsedFields.TryGetValue(SharedAccessSignatureConstants.SignatureFieldName, out signature);

            return isSharedAccessSignature;
        }

        public bool IsExpired()
        {
            return this.ExpiresOn + SharedAccessSignatureConstants.MaxClockSkew < DateTime.UtcNow;
        }

        public DateTime ExpiryTime()
        {
            return this.ExpiresOn + SharedAccessSignatureConstants.MaxClockSkew;
        }

        public void Authenticate(SharedAccessSignatureAuthorizationRule sasAuthorizationRule)
        {
            if (this.IsExpired())
            {
                throw new UnauthorizedAccessException("The specified SAS token is expired.");
            }

            if (sasAuthorizationRule.PrimaryKey != null)
            {
                string primareyKeyComputedSignature = this.ComputeSignature(Convert.FromBase64String(sasAuthorizationRule.PrimaryKey));
                if (string.Equals(this.signature, primareyKeyComputedSignature))
                {
                    return;
                }
            }

            if (sasAuthorizationRule.SecondaryKey != null)
            {
                string secondaryKeyComputedSignature = this.ComputeSignature(Convert.FromBase64String(sasAuthorizationRule.SecondaryKey));
                if (string.Equals(this.signature, secondaryKeyComputedSignature))
                {
                    return;
                }
            }

            throw new UnauthorizedAccessException("The specified SAS token has an invalid signature. It does not match either the primary or secondary key.");
        }

        public void Authorize(string iotHubHostName)
        {
            SecurityHelper.ValidateIotHubHostName(iotHubHostName, this.IotHubName);
        }

        public void Authorize(Uri targetAddress)
        {
            if (targetAddress == null)
            {
                throw new ArgumentNullException("targetAddress");
            }
            
            string target = targetAddress.Host + targetAddress.AbsolutePath;

            if (!target.StartsWith(this.audience.TrimEnd(new char[] { '/' }), StringComparison.OrdinalIgnoreCase))
            {
                throw new UnauthorizedAccessException("Invalid target audience");
            }
        }

        public string ComputeSignature(byte[] key)
        {
            List<string> fields = new List<string>();
            fields.Add(this.encodedAudience);
            fields.Add(this.expiry);

            using (var hmac = new HMACSHA256(key))
            {
                string value = string.Join("\n", fields);
                return Convert.ToBase64String(hmac.ComputeHash(Encoding.UTF8.GetBytes(value)));
            }
        }

        static IDictionary<string, string> ExtractFieldValues(string sharedAccessSignature)
        {
            string[] lines = sharedAccessSignature.Split();

            if (!string.Equals(lines[0].Trim(), SharedAccessSignatureConstants.SharedAccessSignature, StringComparison.Ordinal) || lines.Length != 2)
            {
                throw new FormatException("Malformed signature");
            }

            IDictionary<string, string> parsedFields = new Dictionary<string, string>(StringComparer.OrdinalIgnoreCase);
            string[] fields = lines[1].Trim().Split(new string[] { SharedAccessSignatureConstants.PairSeparator }, StringSplitOptions.None);

            foreach (string field in fields)
            {
                if (field != string.Empty)
                {
                    string[] fieldParts = field.Split(new string[]{ SharedAccessSignatureConstants.KeyValueSeparator }, StringSplitOptions.None);
                    if (string.Equals(fieldParts[0], SharedAccessSignatureConstants.AudienceFieldName, StringComparison.OrdinalIgnoreCase))
                    {
                        // We need to preserve the casing of the escape characters in the audience,
                        // so defer decoding the URL until later.
                        parsedFields.Add(fieldParts[0], fieldParts[1]);
                    }
                    else
                    {
                        parsedFields.Add(fieldParts[0], HttpUtility.UrlDecode(fieldParts[1]));
                    }
                }
            }

            return parsedFields;
        }
    }
}
