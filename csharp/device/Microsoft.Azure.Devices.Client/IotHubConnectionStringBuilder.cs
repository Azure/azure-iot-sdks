// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client
{
    using System;
    using System.Text;
    using Microsoft.Azure.Devices.Client.Extensions;

#if !NETMF
    using System.Collections.Generic;
    using System.Globalization;
    using System.Text.RegularExpressions;
    using System.Net;
    using SharedAccessSignatureParser = Microsoft.Azure.Devices.Client.SharedAccessSignature;

#if !WINDOWS_UWP && !PCL
    using System.Security.Cryptography.X509Certificates;
#endif
#endif

    /// <summary>
    /// Builds a connection string for the IoT Hub service based on the properties populated by the user.
    /// </summary>
    public sealed class IotHubConnectionStringBuilder
    {
        const char ValuePairDelimiter = ';';
        const char ValuePairSeparator = '=';
        const string HostNameSeparator = ".";

#if !NETMF && !PCL
        static readonly RegexOptions regexOptions = RegexOptions.Compiled | RegexOptions.IgnoreCase;
#elif PCL
        static readonly RegexOptions regexOptions = RegexOptions.IgnoreCase;
#endif

#if !NETMF
        static readonly string HostNamePropertyName = nameof(HostName);
        static readonly string DeviceIdPropertyName = nameof(DeviceId);
        static readonly string SharedAccessKeyNamePropertyName = nameof(SharedAccessKeyName); 
        static readonly string SharedAccessKeyPropertyName = nameof(SharedAccessKey); 
        static readonly string SharedAccessSignaturePropertyName = nameof(SharedAccessSignature);
        static readonly string GatewayHostNamePropertyName = nameof(GatewayHostName);
        static readonly string X509CertPropertyName =  "X509Cert";
        static readonly Regex HostNameRegex = new Regex(@"[a-zA-Z0-9_\-\.]+$", regexOptions);
        static readonly Regex DeviceIdRegex = new Regex(@"^[A-Za-z0-9\-:.+%_#*?!(),=@;$']{1,128}$", regexOptions);
        static readonly Regex SharedAccessKeyNameRegex = new Regex(@"^[a-zA-Z0-9_\-@\.]+$", regexOptions);
        static readonly Regex SharedAccessKeyRegex = new Regex(@"^.+$", regexOptions);
        static readonly Regex SharedAccessSignatureRegex = new Regex(@"^.+$", regexOptions);
        static readonly Regex X509CertRegex = new Regex(@"^[true|false]+$", regexOptions);
#endif

        string hostName;
        string iotHubName;
        IAuthenticationMethod authenticationMethod;

        /// <summary>
        /// Initializes a new instance of the <see cref="IotHubConnectionStringBuilder"/> class.
        /// </summary>
        IotHubConnectionStringBuilder()
        {
        }

        /// <summary>
        /// Creates a connection string based on the hostname of the IoT Hub and the authentication method passed as a parameter.
        /// </summary>
        /// <param name="hostname">The fully-qualified DNS hostname of IoT Hub</param>
        /// <param name="authenticationMethod">The authentication method that is used</param>
        /// <returns>A new instance of the <see cref="IotHubConnectionStringBuilder"/> class with a populated connection string.</returns>
        public static IotHubConnectionStringBuilder Create(string hostname, IAuthenticationMethod authenticationMethod)
        {
            var iotHubConnectionStringBuilder = new IotHubConnectionStringBuilder()
            {
                HostName = hostname,
                AuthenticationMethod = authenticationMethod
            };

            iotHubConnectionStringBuilder.Validate();

            return iotHubConnectionStringBuilder;
        }

        /// <summary>
        /// Creates a connection string based on the hostname of the IoT Hub and the authentication method passed as a parameter.
        /// </summary>
        /// <param name="iotHubConnectionString">The connection string.</param>
        /// <returns>A new instance of the <see cref="IotHubConnectionStringBuilder"/> class with a populated connection string.</returns>
        public static IotHubConnectionStringBuilder Create(string iotHubConnectionString)
        {
            if (iotHubConnectionString.IsNullOrWhiteSpace())
            {
                throw new ArgumentNullException("iotHubConnectionString");
            }

            var iotHubConnectionStringBuilder = new IotHubConnectionStringBuilder();
            iotHubConnectionStringBuilder.Parse(iotHubConnectionString);
            iotHubConnectionStringBuilder.AuthenticationMethod = AuthenticationMethodFactory.GetAuthenticationMethod(iotHubConnectionStringBuilder);

            return iotHubConnectionStringBuilder;
        }

        /// <summary>
        /// Gets or sets the value of the fully-qualified DNS hostname of the IoT Hub service.
        /// </summary>
        public string HostName
        {
            get { return this.hostName; }
            set { this.SetHostName(value); }
        }

        /// <summary>
        /// Gets or sets the authentication method to be used with the IoT Hub service.
        /// </summary>
        public IAuthenticationMethod AuthenticationMethod {
            get { return this.authenticationMethod; }
            set { this.SetAuthenticationMethod(value); }
        }

        /// <summary>
        /// Gets the device identifier of the device connecting to the service.
        /// </summary>
        public string DeviceId { get; internal set; }

        /// <summary>
        /// Gets the shared acess key name used to connect the device to the IoT Hub service.
        /// </summary>
        public string SharedAccessKeyName { get; internal set; }

        /// <summary>
        /// Gets the shared access key used to connect to the IoT Hub service.
        /// </summary>
        public string SharedAccessKey { get; internal set; }

        /// <summary>
        /// Gets the optional name of the gateway to connect to
        /// </summary>
        public string GatewayHostName { get; internal set; }

        /// <summary>
        /// Gets the shared access signature used to connect to the IoT Hub service.
        /// </summary>
        public string SharedAccessSignature { get; internal set; }

        public bool UsingX509Cert { get; internal set; }

        internal string IotHubName 
        {
            get { return this.iotHubName; }
        }

#if !NETMF && !WINDOWS_UWP && !PCL
        internal X509Certificate2 Certificate { get; set; }
#endif

        internal IotHubConnectionString ToIotHubConnectionString()
        {
            this.Validate();
            return new IotHubConnectionString(this);
        }

#if !NETMF
        /// <summary>
        /// Produces the connection string based on the values of the <see cref="IotHubConnectionStringBuilder"/> instance properties.
        /// </summary>
        /// <returns>A properly formatted connection string.</returns>
        public override sealed string ToString()
        {
            this.Validate();

            StringBuilder stringBuilder = new StringBuilder();
            stringBuilder.AppendKeyValuePairIfNotEmpty(HostNamePropertyName, this.HostName);
            stringBuilder.AppendKeyValuePairIfNotEmpty(DeviceIdPropertyName, WebUtility.UrlEncode(this.DeviceId));
            stringBuilder.AppendKeyValuePairIfNotEmpty(SharedAccessKeyNamePropertyName, this.SharedAccessKeyName);
            stringBuilder.AppendKeyValuePairIfNotEmpty(SharedAccessKeyPropertyName, this.SharedAccessKey);
            stringBuilder.AppendKeyValuePairIfNotEmpty(SharedAccessSignaturePropertyName, this.SharedAccessSignature);
            stringBuilder.AppendKeyValuePairIfNotEmpty(X509CertPropertyName, this.UsingX509Cert);
            stringBuilder.AppendKeyValuePairIfNotEmpty(GatewayHostNamePropertyName, this.GatewayHostName);
            if (stringBuilder.Length > 0)
            {
                stringBuilder.Remove(stringBuilder.Length - 1, 1);
            }

            return stringBuilder.ToString();
        }
#endif

        void Parse(string iotHubConnectionString)
        {
#if NETMF
            if (iotHubConnectionString.IsNullOrWhiteSpace())
            {
                throw new ArgumentException("Malformed Token");
            }

            string[] parts = iotHubConnectionString.Split(ValuePairDelimiter);
            string[] values;

            foreach (string part in parts)
            {
                values = part.Split(ValuePairSeparator);

                if (part.IndexOf("HostName") > -1)
                {
                    // Host Name
                    this.HostName = values[1];
                }
                else if (part.IndexOf("DeviceId") > -1)
                {
                    // DeviceId
                    this.DeviceId = WebUtility.UrlDecode(values[1]);
                }
                else if (part.IndexOf("SharedAccessKeyName") > -1)
                {
                    // Shared Access Key Name 
                    this.SharedAccessKeyName = values[1];
                }
                else if (part.IndexOf("SharedAccessKey") > -1)
                {
                    // Shared Access Key
                    // need to handle this differently becuase shared access key may have special chars such as '=' which break the string split
                    this.SharedAccessKey = part.Substring(part.IndexOf('=') + 1);
                }
                else if (part.IndexOf("SharedAccessSignature") > -1)
                {
                    // Shared Access Signature
                    // need to handle this differently because shared access key may have special chars such as '=' which break the string split
                    this.SharedAccessSignature = part.Substring(part.IndexOf('=') + 1);
                }
                else if (part.IndexOf("GatewayHostName") > -1)
                {
                    // Gateway host name
                    this.GatewayHostName = values[1];
                }

            }
#else
            IDictionary<string, string> map = iotHubConnectionString.ToDictionary(ValuePairDelimiter, ValuePairSeparator);

            this.HostName = GetConnectionStringValue(map, HostNamePropertyName);
            this.DeviceId = WebUtility.UrlDecode(GetConnectionStringOptionalValue(map, DeviceIdPropertyName));
            this.SharedAccessKeyName = GetConnectionStringOptionalValue(map, SharedAccessKeyNamePropertyName);
            this.SharedAccessKey = GetConnectionStringOptionalValue(map, SharedAccessKeyPropertyName);
            this.SharedAccessSignature = GetConnectionStringOptionalValue(map, SharedAccessSignaturePropertyName);
            this.UsingX509Cert = GetConnectionStringOptionalValueOrDefault<bool>(map, X509CertPropertyName, GetX509, true);
            this.GatewayHostName = GetConnectionStringOptionalValue(map, GatewayHostNamePropertyName);
#endif
            this.Validate();
        }

        void Validate()
        {
            if (this.DeviceId.IsNullOrWhiteSpace())
            {
                throw new ArgumentException("DeviceId must be specified in connection string");
            }

            if (!(this.SharedAccessKey.IsNullOrWhiteSpace() ^ this.SharedAccessSignature.IsNullOrWhiteSpace()))
            {
#if !WINDOWS_UWP && !PCL && !NETMF
                if (!this.UsingX509Cert)
                {
#endif
                    throw new ArgumentException("Should specify either SharedAccessKey or SharedAccessSignature if X.509 certificate is not used");
#if !WINDOWS_UWP && !PCL && !NETMF
                }
#endif
            }

#if !WINDOWS_UWP && !PCL && !NETMF
            if ((this.UsingX509Cert || this.Certificate != null) &&
                (!this.SharedAccessKey.IsNullOrWhiteSpace() || !this.SharedAccessSignature.IsNullOrWhiteSpace()))
            {
                throw new ArgumentException("Should not specify either SharedAccessKey or SharedAccessSignature if X.509 certificate is used");
            }
#endif

            if (this.IotHubName.IsNullOrWhiteSpace())
            {
#if NETMF
                throw new ArgumentException("Missing IOT hub name");
#else
                throw new FormatException("Missing IOT hub name");
#endif
            }

            if (!this.SharedAccessKey.IsNullOrWhiteSpace())
            {
                Convert.FromBase64String(this.SharedAccessKey);
            }

#if !NETMF
            if (!string.IsNullOrWhiteSpace(this.SharedAccessSignature))
            {
                if (SharedAccessSignatureParser.IsSharedAccessSignature(this.SharedAccessSignature))
                {
                    SharedAccessSignatureParser.Parse(this.IotHubName, this.SharedAccessSignature);
                }
                else
                {
                    throw new ArgumentException("Invalid SharedAccessSignature (SAS)");
                }
            }

            ValidateFormat(this.HostName, HostNamePropertyName, HostNameRegex);
            ValidateFormat(this.DeviceId, DeviceIdPropertyName, DeviceIdRegex);
            ValidateFormatIfSpecified(this.SharedAccessKeyName, SharedAccessKeyNamePropertyName, SharedAccessKeyNameRegex);
            ValidateFormatIfSpecified(this.SharedAccessKey, SharedAccessKeyPropertyName, SharedAccessKeyRegex);
            ValidateFormatIfSpecified(this.SharedAccessSignature, SharedAccessSignaturePropertyName, SharedAccessSignatureRegex);
            ValidateFormatIfSpecified(this.GatewayHostName, GatewayHostNamePropertyName, HostNameRegex);
            ValidateFormatIfSpecified(this.UsingX509Cert.ToString(), X509CertPropertyName, X509CertRegex);
#endif
        }

        void SetHostName(string hostname)
        {
            if (hostname.IsNullOrWhiteSpace())
            {
                throw new ArgumentNullException("hostname");
            }
#if !NETMF
            ValidateFormat(hostname, HostNamePropertyName, HostNameRegex);
#endif

            this.hostName = hostname;
            this.SetIotHubName();
        }

        void SetIotHubName()
        {
            this.iotHubName = GetIotHubName(this.HostName);

            if (this.IotHubName.IsNullOrWhiteSpace())
            {
#if NETMF
                throw new ArgumentException("Missing IOT hub name");
#else
                throw new FormatException("Missing IOT hub name");
#endif
            }
        }

        void SetAuthenticationMethod(IAuthenticationMethod authMethod)
        {
            if (authMethod == null)
            {
                throw new ArgumentNullException("authMethod");
            }

            authMethod.Populate(this);
            this.authenticationMethod = authMethod;
            this.Validate();
        }

#if !NETMF
        static void ValidateFormat(string value, string propertyName, Regex regex)
        {
            if (!regex.IsMatch(value))
            {
                throw new ArgumentException(string.Format(CultureInfo.InvariantCulture, "The connection string has an invalid value for property: {0}", propertyName), "iotHubConnectionString");
            }
        }

        static void ValidateFormatIfSpecified(string value, string propertyName, Regex regex)
        {
            if (!string.IsNullOrEmpty(value))
            {
                ValidateFormat(value, propertyName, regex);
            }
        }

        static string GetConnectionStringValue(IDictionary<string, string> map, string propertyName)
        {
            string value;
            if (!map.TryGetValue(propertyName, out value))
            {
                throw new ArgumentException(string.Format(CultureInfo.InvariantCulture, "The connection string is missing the property: {0}", propertyName), "iotHubConnectionString");
            }

            return value;
        }

        static string GetConnectionStringOptionalValue(IDictionary<string, string> map, string propertyName)
        {
            string value;
            map.TryGetValue(propertyName, out value);
            return value;
        }

        static TValue GetConnectionStringOptionalValueOrDefault<TValue>(IDictionary<string, string> map, string propertyName, TryParse<string, TValue> tryParse, bool ignoreCase)
        {
            TValue value = default(TValue);
            string stringValue;
            if (map.TryGetValue(propertyName, out stringValue) && stringValue != null)
            {
                if (!tryParse(stringValue, ignoreCase, out value))
                {
                    throw new ArgumentException(string.Format(CultureInfo.InvariantCulture, "The connection string has an invalid value for property: {0}", propertyName), "iotHubConnectionString");
                }
            }

            return value;
        }
#endif
        static string GetIotHubName(string hostName)
        {
#if NETMF
            int index = hostName.IndexOf(HostNameSeparator);
#else
            int index = hostName.IndexOf(HostNameSeparator, StringComparison.OrdinalIgnoreCase);
#endif
            string iotHubName = index >= 0 ? hostName.Substring(0, index) : null;
            return iotHubName;
        }

        static bool GetX509(string input, bool ignoreCase, out bool usingX509Cert)
        {
            usingX509Cert = false;
            bool isMatch;

#if NETMF
            isMatch = (ignoreCase ? input.ToLower().CompareTo("true") : input.CompareTo("true")) == 0;
#else
            isMatch = string.Equals(input, "true", ignoreCase ? StringComparison.OrdinalIgnoreCase : StringComparison.Ordinal);
#endif

            if (isMatch)
            {
                usingX509Cert = true;
            }

            return true;
        }
    }
}