// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client
{
    static class SecurityConstants
    {
        /// <summary>
        /// The scheme name for Bearer token auth.
        /// </summary>
        public const string BearerTokenScheme = "Bearer";

        /// <summary>
        /// The scheme name to use for certificate authentication handler
        /// </summary>
        public const string CertificateScheme = "Certificate";

        /// <summary>
        /// The claim type for the authentication scheme.
        /// </summary>
        public const string AuthenticationScheme = "Scheme";

        /// <summary>
        /// The role capable of invoking ResourceProvider from CSM. 
        /// </summary>
        public const string ResourceProviderClientAccessRole = "ResourceProviderClientAccess";

        /// <summary>
        /// The role capable of invoking ResourceProvider from ACIS
        /// </summary>
        public const string ResourceProviderAdminAccessRole = "ResourceProviderAdminAccess";

        /// <summary>
        /// For use when giving permissions to both resource provider roles
        /// </summary>
        public const string ResourceProviderFullAccessRole = ResourceProviderAdminAccessRole + "," + ResourceProviderClientAccessRole;

        /// <summary>
        /// Authentication failed message
        /// </summary>
        public const string AuthenticationFailed = "Authentication failed for the request";

        /// <summary>
        /// Authorization failed message
        /// </summary>
        public const string AuthorizationFailed = "Authorization failed for the request";

        /// <summary>
        /// Missing certificate message
        /// </summary>
        public const string MissingCertificate = "Missing client certificate";

        /// <summary>
        /// Default size of device gateway and device keys
        /// </summary>
        public const int KeyLengthInBytes = 16;

        /// <summary>
        /// The name of the authentication challenge response header.
        /// </summary>
        public const string WwwAuthenticateHeader = "WWW-Authenticate";

        /// <summary>
        /// Default SaS Key name.
        /// </summary>
        public const string DefaultSaSKeyName = "owner";

        /// <summary>
        /// Admin SaS Key name.
        /// </summary>
        public const string AdminSaSKeyName = "admin";

        /// <summary>
        /// SaS Key length.
        /// </summary>
        public const int SaSKeyLength = 32;

        // Shared Access Key Constants

        /// <summary>
        /// Shared Access Key
        /// </summary>
        public const string SharedAccessKey = "SharedAccessKey";

        /// <summary>
        /// Shared Access Key Field Name
        /// </summary>
        public const string SharedAccessKeyFieldName = "sk";

        /// <summary>
        /// Shared Access Key Full Field Name
        /// </summary>
        public const string SharedAccessKeyFullFieldName = SharedAccessKey + " " + SharedAccessKeyFieldName + "=";
    }
}
