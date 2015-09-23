// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client
{
    using System;
    using System.Collections.Generic;
    using System.Globalization;
    using System.Linq.Expressions;
    using System.Text;
    using System.Text.RegularExpressions;
    using System.Net;
    using Microsoft.Azure.Devices.Client.Extensions;
    using SharedAccessSignatureParser = Microsoft.Azure.Devices.Client.SharedAccessSignature;

    public sealed class IotHubConnectionStringBuilder
    {
        const char ValuePairDelimiter = ';';
        const char ValuePairSeparator = '=';
        const string HostNameSeparator = ".";

        static readonly RegexOptions regexOptions = RegexOptions.Compiled | RegexOptions.IgnoreCase;

        static readonly string HostNamePropertyName = ((MemberExpression)((Expression<Func<IotHubConnectionStringBuilder, string>>)(_ => _.HostName)).Body).Member.Name; // todo: replace with nameof()
        static readonly string DeviceIdPropertyName = ((MemberExpression)((Expression<Func<IotHubConnectionStringBuilder, string>>)(_ => _.DeviceId)).Body).Member.Name; // todo: replace with nameof()
        static readonly string SharedAccessKeyNamePropertyName = ((MemberExpression)((Expression<Func<IotHubConnectionStringBuilder, string>>)(_ => _.SharedAccessKeyName)).Body).Member.Name; // todo: replace with nameof()
        static readonly string SharedAccessKeyPropertyName = ((MemberExpression)((Expression<Func<IotHubConnectionStringBuilder, string>>)(_ => _.SharedAccessKey)).Body).Member.Name; // todo: replace with nameof()
        static readonly string SharedAccessSignaturePropertyName = ((MemberExpression)((Expression<Func<IotHubConnectionStringBuilder, string>>)(_ => _.SharedAccessSignature)).Body).Member.Name; // todo: replace with nameof();
        static readonly Regex HostNameRegex = new Regex(@"[a-zA-Z0-9_\-\.]+$", regexOptions);
        static readonly Regex DeviceIdRegex = new Regex(@"^[A-Za-z0-9\-:.+%_#*?!(),=@;$']{1,128}$", regexOptions);
        static readonly Regex SharedAccessKeyNameRegex = new Regex(@"^[a-zA-Z0-9_\-@\.]+$", regexOptions);
        static readonly Regex SharedAccessKeyRegex = new Regex(@"^.+$", regexOptions);
        static readonly Regex SharedAccessSignatureRegex = new Regex(@"^.+$", regexOptions);

        string hostName;
        string iotHubName;
        IAuthenticationMethod authenticationMethod;

        IotHubConnectionStringBuilder()
        {
        }

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

        public static IotHubConnectionStringBuilder Create(string iotHubConnectionString)
        {
            if (string.IsNullOrWhiteSpace(iotHubConnectionString))
            {
                throw new ArgumentNullException("iotHubConnectionString");
            }

            var iotHubConnectionStringBuilder = new IotHubConnectionStringBuilder();
            iotHubConnectionStringBuilder.Parse(iotHubConnectionString);
            iotHubConnectionStringBuilder.AuthenticationMethod = AuthenticationMethodFactory.GetAuthenticationMethod(iotHubConnectionStringBuilder);

            return iotHubConnectionStringBuilder;
        }

        public string HostName
        {
            get { return this.hostName; }
            set { this.SetHostName(value); }
        }

        public IAuthenticationMethod AuthenticationMethod {
            get { return this.authenticationMethod; }
            set { this.SetAuthenticationMethod(value); }
        }

        public string DeviceId { get; internal set; }

        public string SharedAccessKeyName { get; internal set; }

        public string SharedAccessKey { get; internal set; }

        public string SharedAccessSignature { get; internal set; }

        internal string IotHubName 
        {
            get { return this.iotHubName; }
        }

        internal IotHubConnectionString ToIotHubConnectionString()
        {
            this.Validate();
            return new IotHubConnectionString(this);
        }

        public override sealed string ToString()
        {
            this.Validate();

            StringBuilder stringBuilder = new StringBuilder();
            stringBuilder.AppendKeyValuePairIfNotEmpty(HostNamePropertyName, this.HostName);
            stringBuilder.AppendKeyValuePairIfNotEmpty(DeviceIdPropertyName, WebUtility.UrlEncode(this.DeviceId));
            stringBuilder.AppendKeyValuePairIfNotEmpty(SharedAccessKeyNamePropertyName, this.SharedAccessKeyName);
            stringBuilder.AppendKeyValuePairIfNotEmpty(SharedAccessKeyPropertyName, this.SharedAccessKey);
            stringBuilder.AppendKeyValuePairIfNotEmpty(SharedAccessSignaturePropertyName, this.SharedAccessSignature);
            if (stringBuilder.Length > 0)
            {
                stringBuilder.Remove(stringBuilder.Length - 1, 1);
            }

            return stringBuilder.ToString();
        }

        void Parse(string iotHubConnectionString)
        {
            IDictionary<string, string> map = iotHubConnectionString.ToDictionary(ValuePairDelimiter, ValuePairSeparator);

            this.HostName = GetConnectionStringValue(map, HostNamePropertyName);
            this.DeviceId = WebUtility.UrlDecode(GetConnectionStringOptionalValue(map, DeviceIdPropertyName));
            this.SharedAccessKeyName = GetConnectionStringOptionalValue(map, SharedAccessKeyNamePropertyName);
            this.SharedAccessKey = GetConnectionStringOptionalValue(map, SharedAccessKeyPropertyName);
            this.SharedAccessSignature = GetConnectionStringOptionalValue(map, SharedAccessSignaturePropertyName);

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
                throw new ArgumentException("Should specify either SharedAccessKey or SharedAccessSignature");
            }

            if (string.IsNullOrWhiteSpace(this.IotHubName))
            {
                throw new FormatException("Missing IOT hub name");
            }

            if (!this.SharedAccessKey.IsNullOrWhiteSpace())
            {
                Convert.FromBase64String(this.SharedAccessKey);
            }

            if (SharedAccessSignatureParser.IsSharedAccessSignature(this.SharedAccessSignature))
            {
                SharedAccessSignatureParser.Parse(this.IotHubName, this.SharedAccessSignature);
            }
            
            ValidateFormat(this.HostName, HostNamePropertyName, HostNameRegex);
            ValidateFormat(this.DeviceId, DeviceIdPropertyName, DeviceIdRegex);
            ValidateFormatIfSpecified(this.SharedAccessKeyName, SharedAccessKeyNamePropertyName, SharedAccessKeyNameRegex);
            ValidateFormatIfSpecified(this.SharedAccessKey, SharedAccessKeyPropertyName, SharedAccessKeyRegex);
            ValidateFormatIfSpecified(this.SharedAccessSignature, SharedAccessSignaturePropertyName, SharedAccessSignatureRegex);
        }

        void SetHostName(string hostname)
        {
            if (string.IsNullOrWhiteSpace(hostname))
            {
               throw new ArgumentNullException("hostname");
            }

            ValidateFormat(hostname, HostNamePropertyName, HostNameRegex);
            this.hostName = hostname;
            this.SetIotHubName();
        }

        void SetIotHubName()
        {
            this.iotHubName = GetIotHubName(this.HostName);

            if (string.IsNullOrWhiteSpace(this.IotHubName))
            {
                throw new FormatException("Missing IOT hub name");
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

        static string GetIotHubName(string hostName)
        {
            int index = hostName.IndexOf(HostNameSeparator, StringComparison.OrdinalIgnoreCase);
            string iotHubName = index >= 0 ? hostName.Substring(0, index) : null;
            return iotHubName;
        }
    }
}