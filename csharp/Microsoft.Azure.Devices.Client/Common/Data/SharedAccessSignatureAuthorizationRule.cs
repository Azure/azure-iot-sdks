// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client
{
    using System;
    using Newtonsoft.Json;

    sealed class SharedAccessSignatureAuthorizationRule : IEquatable<SharedAccessSignatureAuthorizationRule>
    {
        string primaryKey;
        string secondaryKey;

        public string KeyName { get; set; }

        // TODO: encrypt
        [JsonProperty(PropertyName = "primaryKey")]
        public string PrimaryKey
        {
            get
            {
                return this.primaryKey;
            }

            set
            {
                StringValidationHelper.EnsureNullOrBase64String(value, "PrimaryKey");
                this.primaryKey = value;
            }
        }

        // TODO: encrypt
        [JsonProperty(PropertyName = "secondaryKey")]
        public string SecondaryKey
        {
            get
            {
                return this.secondaryKey;
            }
            set
            {
                StringValidationHelper.EnsureNullOrBase64String(value, "SecondaryKey");
                this.secondaryKey = value;
            }
        }

        public AccessRights Rights { get; set; }

        public bool Equals(SharedAccessSignatureAuthorizationRule other)
        {
            if (other == null)
            {
                return false;
            }

            bool equals = string.Equals(this.KeyName, other.KeyName, StringComparison.OrdinalIgnoreCase) &&
                string.Equals(this.PrimaryKey, other.PrimaryKey, StringComparison.Ordinal) &&
                string.Equals(this.SecondaryKey, other.SecondaryKey, StringComparison.Ordinal) &&
                Equals(this.Rights, other.Rights);

            return equals;
        }

        public int GetHashCode(SharedAccessSignatureAuthorizationRule rule)
        {
            if (Object.ReferenceEquals(rule, null))
            {
                return 0;
            }

            int hashKeyName = rule.KeyName == null ? 0 : rule.KeyName.GetHashCode();

            int hashPrimaryKey = rule.PrimaryKey == null ? 0 : rule.PrimaryKey.GetHashCode();

            int hashSecondaryKey = rule.SecondaryKey == null ? 0 : rule.SecondaryKey.GetHashCode();

            int hashRights = rule.Rights.GetHashCode();

            return hashKeyName ^ hashPrimaryKey ^ hashSecondaryKey ^ hashRights;
        }
    }
}
