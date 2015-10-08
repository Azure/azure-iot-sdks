// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client
{
    using Microsoft.Azure.Devices.Client.Extensions;
    using System;
    using System.Text;

    public sealed class IotHubConnectionStringBuilder
    {
        const char ValuePairDelimiter = ';';
        const char ValuePairSeparator = '=';
        const string HostNameSeparator = ".";


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
            if (iotHubConnectionString.IsNullOrWhiteSpace())
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
            //stringBuilder.AppendKeyValuePairIfNotEmpty(HostNamePropertyName, this.HostName);
            //stringBuilder.AppendKeyValuePairIfNotEmpty(DeviceIdPropertyName, WebUtility.UrlEncode(this.DeviceId));
            //stringBuilder.AppendKeyValuePairIfNotEmpty(SharedAccessKeyNamePropertyName, this.SharedAccessKeyName);
            //stringBuilder.AppendKeyValuePairIfNotEmpty(SharedAccessKeyPropertyName, this.SharedAccessKey);
            //stringBuilder.AppendKeyValuePairIfNotEmpty(SharedAccessSignaturePropertyName, this.SharedAccessSignature);
            if (stringBuilder.Length > 0)
            {
                stringBuilder.Remove(stringBuilder.Length - 1, 1);
            }

            return stringBuilder.ToString();
        }

        void Parse(string iotHubConnectionString)
        {
            if (iotHubConnectionString == null || iotHubConnectionString == "")
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
                    this.DeviceId = HttpUtility.UrlDecode(values[1]);
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
                    // need to handle this differently becuase shared access key may have special chars such as '=' which break the string split
                    this.SharedAccessSignature = part.Substring(part.IndexOf('=') + 1);
                }
            }

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

            if (this.IotHubName.IsNullOrWhiteSpace())
            {
                throw new ArgumentException("Missing IOT hub name");
            }

            if (!this.SharedAccessKey.IsNullOrWhiteSpace())
            {
                Convert.FromBase64String(this.SharedAccessKey);
            }

            //if (SharedAccessSignatureParser.IsSharedAccessSignature(this.SharedAccessSignature))
            //{
            //    SharedAccessSignatureParser.Parse(this.IotHubName, this.SharedAccessSignature);
            //}
        }

        void SetHostName(string hostname)
        {
            if (hostname.IsNullOrWhiteSpace())
            {
                throw new ArgumentNullException("hostname");
            }

            this.hostName = hostname;
            this.SetIotHubName();
        }

        void SetIotHubName()
        {
            this.iotHubName = GetIotHubName(this.HostName);

            if (this.IotHubName.IsNullOrWhiteSpace())
            {
                throw new ArgumentException("Missing IOT hub name");
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

        static string GetIotHubName(string hostName)
        {
            int index = hostName.IndexOf(HostNameSeparator);
            string iotHubName = index >= 0 ? hostName.Substring(0, index) : null;
            return iotHubName;
        }
    }
}
