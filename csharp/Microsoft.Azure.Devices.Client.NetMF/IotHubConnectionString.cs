// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client
{
    using Microsoft.Azure.Devices.Client.Extensions;
    using System;
    using System.Text;

    sealed class IotHubConnectionString : IAuthorizationHeaderProvider
//#if !WINDOWS_UWP
//        , ICbsTokenProvider
//#endif
    {
        static readonly TimeSpan DefaultTokenTimeToLive = new TimeSpan(1, 0, 0);
        const string UserSeparator = "@";

        public IotHubConnectionString(IotHubConnectionStringBuilder builder)
        {
            if (builder == null)
            {
                throw new ArgumentNullException("builder");
            }

            this.HostName = builder.HostName;
            this.SharedAccessKeyName = builder.SharedAccessKeyName;
            this.SharedAccessKey = builder.SharedAccessKey;
            this.SharedAccessSignature = builder.SharedAccessSignature; 
            this.IotHubName = builder.IotHubName;
            this.DeviceId = builder.DeviceId;

            // NetMF doesn't implement UriBuilder
            this.HttpsEndpoint = new Uri("https://" + builder.HostName);
            //this.AmqpEndpoint = "amqps://" + this.HostName + ":" + AmqpConstants.DefaultSecurePort;
        }

        public string IotHubName
        {
            get;
            private set;
        }

        public string DeviceId
        {
            get;
            private set;
        }

        public string HostName
        {
            get;
            private set;
        }

        public Uri HttpsEndpoint
        {
            get;
            private set;
        }

        public string AmqpEndpoint
        {
            get;
            private set;
        }

        public string Audience
        {
            get { return this.HostName; }
        }

        public string SharedAccessKeyName
        {
            get;
            private set;
        }

        public string SharedAccessKey
        {
            get;
            private set;
        }

        public string SharedAccessSignature
        {
            get;
            private set;
        }

        public string GetUser()
        {
            var stringBuilder = new StringBuilder();
            stringBuilder.Append(this.SharedAccessKeyName ?? HttpUtility.UrlEncode(this.DeviceId));
            stringBuilder.Append(UserSeparator);
            stringBuilder.Append("sas.");
            stringBuilder.Append(this.SharedAccessKeyName == null ? "" : "root.");
            stringBuilder.Append(this.IotHubName);

            return stringBuilder.ToString();
        }

        public string GetPassword()
        {
            string password;
            if (this.SharedAccessSignature.IsNullOrWhiteSpace())
            {
                TimeSpan timeToLive;
                password = this.BuildToken(out timeToLive);
            }
            else
            {
                password = this.SharedAccessSignature;
            }

            return password;
        }

        public string GetAuthorizationHeader()
        {
            return this.GetPassword();
        }

        //#if !WINDOWS_UWP
        //        Task<CbsToken> ICbsTokenProvider.GetTokenAsync(Uri namespaceAddress, string appliesTo, string[] requiredClaims)
        //        {
        //            string tokenValue;
        //            CbsToken token;
        //            if (string.IsNullOrWhiteSpace(this.SharedAccessSignature))
        //            {
        //                TimeSpan timeToLive;
        //                tokenValue = this.BuildToken(out timeToLive);
        //                token = new CbsToken(tokenValue, CbsConstants.IotHubSasTokenType, DateTime.UtcNow.Add(timeToLive));
        //            }
        //            else
        //            {
        //                tokenValue = this.SharedAccessSignature;
        //                token = new CbsToken(tokenValue, CbsConstants.IotHubSasTokenType, DateTime.MaxValue);
        //            }

        //            return Task.FromResult(token);
        //        }
        //#endif
        //        public Uri BuildLinkAddress(string path)
        //        {
        //#if WINDOWS_UWP
        //            throw new NotImplementedException();
        //#else
        //            var builder = new UriBuilder(this.AmqpEndpoint)
        //            {
        //                Path = path,
        //            };

        //            return builder.Uri;
        //#endif
        //        }

        public static IotHubConnectionString Parse(string connectionString)
        {
            var builder = IotHubConnectionStringBuilder.Create(connectionString);
            return builder.ToIotHubConnectionString();
        }

        string BuildToken(out TimeSpan ttl)
        {
            var builder = new SharedAccessSignatureBuilder()
            {
                Key = this.SharedAccessKey,
                TimeToLive = DefaultTokenTimeToLive,
            };

            if (this.SharedAccessKeyName == null)
            {
                builder.Target = this.Audience + "/devices/" + HttpUtility.UrlEncode(this.DeviceId);
            }
            else
            {
                builder.KeyName = this.SharedAccessKeyName;
                builder.Target = this.Audience;
            }

            ttl = builder.TimeToLive;

            return builder.ToSignature();
        }
    }
}
