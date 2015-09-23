// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client
{
    using System;
    using System.Collections.Generic;
    using System.Net;
    using System.Net.Http;
    using System.Threading;
    using System.Threading.Tasks;

    interface IHttpClientHelper : IDisposable
    {
        Task<T> GetAsync<T>(
            Uri requestUri,
            IDictionary<HttpStatusCode, Func<HttpResponseMessage, Task<Exception>>> errorMappingOverrides,
            IDictionary<string, string> customHeaders,
            CancellationToken cancellationToken);

        Task<T> GetAsync<T>(
            Uri requestUri,
            IDictionary<HttpStatusCode, Func<HttpResponseMessage, Task<Exception>>> errorMappingOverrides,
            IDictionary<string, string> customHeaders,
            bool throwIfNotFound,
            CancellationToken cancellationToken);

        Task<T> PutAsync<T>(
            Uri requestUri,
            T entity,
            PutOperationType operationType,
            IDictionary<HttpStatusCode, Func<HttpResponseMessage, Task<Exception>>> errorMappingOverrides,
            CancellationToken cancellationToken) where T: IETagHolder;

        Task PostAsync<T>(
            Uri requestUri, 
            T entity, 
            IDictionary<HttpStatusCode, Func<HttpResponseMessage, Task<Exception>>> errorMappingOverrides, 
            IDictionary<string, string> customHeaders, 
            CancellationToken cancellationToken);

        Task DeleteAsync<T>(
            Uri requestUri,
            T entity,
            IDictionary<HttpStatusCode, Func<HttpResponseMessage, Task<Exception>>> errorMappingOverrides,
            IDictionary<string, string> customHeaders, 
            CancellationToken cancellationToken) where T : IETagHolder;
    }
}
