// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client
{
    using System;
    using System.Text;
    using System.Threading.Tasks;
    using System.Net;
#if !WINDOWS_UWP
    using Microsoft.Azure.Amqp;
#endif
    using Microsoft.Azure.Devices.Client.Extensions;

    sealed class IotHubConnectionString : IAuthorizationHeaderProvider
#if !WINDOWS_UWP
        , ICbsTokenProvider
#endif
    {
        static readonly TimeSpan DefaultTokenTimeToLive = TimeSpan.FromHours(1);
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
#if WINDOWS_UWP
            this.HttpsEndpoint = new UriBuilder("https", builder.HostName).Uri;
#else
            this.HttpsEndpoint = new UriBuilder(Uri.UriSchemeHttps, builder.HostName).Uri;
#endif
#if !WINDOWS_UWP
            this.AmqpEndpoint = new UriBuilder(CommonConstants.AmqpsScheme, builder.HostName, AmqpConstants.DefaultSecurePort).Uri;
#endif
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

#if !WINDOWS_UWP
        public Uri AmqpEndpoint
        {
            get;
            private set;
        }
#endif
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
            stringBuilder.Append(this.SharedAccessKeyName ?? WebUtility.UrlEncode(this.DeviceId));
            stringBuilder.Append(UserSeparator);
            stringBuilder.Append("sas.");
            stringBuilder.Append(this.SharedAccessKeyName == null ? "" : "root.");
            stringBuilder.Append(this.IotHubName);

            return stringBuilder.ToString();
        }

        public string GetPassword()
        {
            string password;
            if (string.IsNullOrWhiteSpace(this.SharedAccessSignature))
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

#if !WINDOWS_UWP
        Task<CbsToken> ICbsTokenProvider.GetTokenAsync(Uri namespaceAddress, string appliesTo, string[] requiredClaims)
        {
            string tokenValue;
            CbsToken token;
            if (string.IsNullOrWhiteSpace(this.SharedAccessSignature))
            {
                TimeSpan timeToLive;
                tokenValue = this.BuildToken(out timeToLive);
                token = new CbsToken(tokenValue, CbsConstants.IotHubSasTokenType, DateTime.UtcNow.Add(timeToLive));
            }
            else
            {
                tokenValue = this.SharedAccessSignature;
                token = new CbsToken(tokenValue, CbsConstants.IotHubSasTokenType, DateTime.MaxValue);
            }

            return Task.FromResult(token);
        }
#endif
        public Uri BuildLinkAddress(string path)
        {
#if WINDOWS_UWP
            throw new NotImplementedException();
#else
            var builder = new UriBuilder(this.AmqpEndpoint)
            {
                Path = path,
            };

            return builder.Uri;
#endif
        }

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
                builder.Target = "{0}/devices/{1}".FormatInvariant(this.Audience, WebUtility.UrlEncode(this.DeviceId));
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
