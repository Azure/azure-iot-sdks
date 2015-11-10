// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Common.Data
{
    using System;
    using Microsoft.Azure.Devices.Common.Security;
    using Newtonsoft.Json;

    public sealed class SharedAccessSignatureAuthorizationRule : IEquatable<SharedAccessSignatureAuthorizationRule>
    {
        string primaryKey;
        string secondaryKey;

        [JsonProperty(PropertyName = "keyName")]
        public string KeyName { get; set; }

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

        [JsonProperty(PropertyName = "rights")]
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
    }
}
