// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices
{
    class ClientApiVersionHelper
    {
        const string ApiVersionQueryPrefix = "api-version=";
        const string ApiVersionGA = "2016-02-03";
        const string ApiVersionApril2016 = "2016-04-30";
        const string ApiVersionLatest = ApiVersionApril2016;

        public const string ApiVersionQueryString = ApiVersionQueryPrefix + ApiVersionLatest;
        public const string ApiVersionQueryStringGA = ApiVersionQueryPrefix + ApiVersionGA;
    }
}