// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client
{
    using System;
    using System.Text;
    using System.Net;

#if !NETMF && !PCL
    using Microsoft.Azure.Amqp;
#endif

#if !NETMF
    using System.Threading.Tasks;
#endif

    using Microsoft.Azure.Devices.Client.Extensions;

    sealed class IotHubConnectionString : IAuthorizationHeaderProvider
#if !NETMF && !PCL
        , ICbsTokenProvider
#endif
    {
#if NETMF
        static readonly TimeSpan DefaultTokenTimeToLive = new TimeSpan(1, 0, 0);
#else
        static readonly TimeSpan DefaultTokenTimeToLive = TimeSpan.FromHours(1);
#endif
        const string UserSeparator = "@";

        public IotHubConnectionString(IotHubConnectionStringBuilder builder)
        {
            if (builder == null)
            {
                throw new ArgumentNullException("builder");
            }

            this.Audience = builder.HostName;
            this.HostName = builder.GatewayHostName == null || builder.GatewayHostName == "" ? builder.HostName : builder.GatewayHostName;
            this.SharedAccessKeyName = builder.SharedAccessKeyName;
            this.SharedAccessKey = builder.SharedAccessKey;
            this.SharedAccessSignature = builder.SharedAccessSignature; 
            this.IotHubName = builder.IotHubName;
            this.DeviceId = builder.DeviceId;

#if WINDOWS_UWP || PCL
            this.HttpsEndpoint = new UriBuilder("https", this.HostName).Uri;
#elif !NETMF
            this.HttpsEndpoint = new UriBuilder(Uri.UriSchemeHttps, this.HostName).Uri;
#elif NETMF
            this.HttpsEndpoint = new Uri("https://" + this.HostName);
#endif

#if !NETMF && !PCL
            this.AmqpEndpoint = new UriBuilder(CommonConstants.AmqpsScheme, this.HostName, AmqpConstants.DefaultSecurePort).Uri;
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

#if !NETMF && !PCL
        public Uri AmqpEndpoint
        {
            get;
            private set;
        }
#endif

        public string Audience
        {
            get;
            private set;
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

#if !NETMF && !PCL
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
#if NETMF || PCL
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
#if NETMF
                builder.Target = this.Audience + "/devices/" + WebUtility.UrlEncode(this.DeviceId);
#else
                builder.Target = "{0}/devices/{1}".FormatInvariant(this.Audience, WebUtility.UrlEncode(this.DeviceId));
#endif
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
