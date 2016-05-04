// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices
{
    using System;
    using System.Text.RegularExpressions;
    using Microsoft.Azure.Devices.Common;
    using Newtonsoft.Json;

    /// <summary>
    /// X509 client certificate thumbprints of the device
    /// </summary>
    public sealed class X509Thumbprint
    {
        static readonly Regex ThumbprintRegex = new Regex(@"^([A-Fa-f0-9]{2}){20}$", RegexOptions.Compiled | RegexOptions.IgnoreCase);
        /// <summary>
        /// X509 client certificate primary thumbprint
        /// </summary>
        [JsonProperty(PropertyName = "primaryThumbprint")]
        public string PrimaryThumbprint { get; set; }

        /// <summary>
        /// X509 client certificate secondary thumbprint
        /// </summary>
        [JsonProperty(PropertyName = "secondaryThumbprint")]
        public string SecondaryThumbprint { get; set; }

        /// <summary>
        /// Checks if contents are valid
        /// </summary>
        /// <param name="throwArgumentException"></param>
        /// <returns>bool</returns>
        public bool IsValid(bool throwArgumentException)
        {
            if (!this.IsEmpty())
            {
                bool primaryThumbprintIsValid = IsValidThumbprint(this.PrimaryThumbprint);
                bool secondaryThumbprintIsValid = IsValidThumbprint(this.SecondaryThumbprint);

                if (primaryThumbprintIsValid)
                {
                    if (secondaryThumbprintIsValid || string.IsNullOrWhiteSpace(this.SecondaryThumbprint))
                    {
                        return true;
                    }
                    else
                    {
                        if (throwArgumentException)
                        {
                            throw new ArgumentException(ApiResources.StringIsNotThumbprint.FormatInvariant(this.SecondaryThumbprint), "Secondary Thumbprint");
                        }

                        return false;
                    }
                }
                else if (secondaryThumbprintIsValid)
                {
                    if (string.IsNullOrWhiteSpace(this.PrimaryThumbprint))
                    {
                        return true;
                    }
                    else
                    {
                        if (throwArgumentException)
                        {
                            throw new ArgumentException(ApiResources.StringIsNotThumbprint.FormatInvariant(this.PrimaryThumbprint), "PrimaryThumbprint");
                        }

                        return false;
                    }
                }

                if (throwArgumentException)
                {
                    if (string.IsNullOrEmpty(this.SecondaryThumbprint))
                    {
                        throw new ArgumentException(ApiResources.StringIsNotThumbprint.FormatInvariant(this.PrimaryThumbprint), "Primary Thumbprint");
                    }
                    else if (string.IsNullOrEmpty(this.PrimaryThumbprint))
                    {
                        throw new ArgumentException(ApiResources.StringIsNotThumbprint.FormatInvariant(this.SecondaryThumbprint), "Secondary Thumbprint");
                    }
                    else
                    {
                        throw new ArgumentException(ApiResources.StringIsNotThumbprint.FormatInvariant(this.PrimaryThumbprint), "Primary Thumbprint");
                    }
                }
            }

            return false;
        }

        /// <summary>
        /// Checks if the thumbprints are populated
        /// </summary>
        /// <returns>bool</returns>
        public bool IsEmpty()
        {
            return string.IsNullOrWhiteSpace(this.PrimaryThumbprint) && string.IsNullOrWhiteSpace(this.SecondaryThumbprint);
        }

        static bool IsValidThumbprint(string thumbprint)
        {
            return thumbprint != null && ThumbprintRegex.IsMatch(thumbprint);
        }
    }
}