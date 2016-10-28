// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client.Transport
{
    using System;
    using System.Collections.Generic;
    using System.Collections.ObjectModel;
    using System.Globalization;
    using System.IO;
    using System.Linq;
    using System.Net;
    using System.Net.Http;
    using System.Net.Http.Headers;
    using System.Threading;
    using System.Threading.Tasks;
    using Microsoft.Azure.Devices.Client.Exceptions;
    using Microsoft.Azure.Devices.Client.Extensions;
#if !WINDOWS_UWP && !PCL
    using System.Net.Http.Formatting;
    using System.Security.Cryptography.X509Certificates;
#endif

    sealed class HttpClientHelper : IHttpClientHelper
    {
        readonly Uri baseAddress;
        readonly IAuthorizationHeaderProvider authenticationHeaderProvider;
        readonly IReadOnlyDictionary<HttpStatusCode, Func<HttpResponseMessage, Task<Exception>>> defaultErrorMapping;
        readonly bool usingX509ClientCert = false;
        HttpClient httpClientObj;
        bool isDisposed;

        public HttpClientHelper(
            Uri baseAddress,
            IAuthorizationHeaderProvider authenticationHeaderProvider,
            IDictionary<HttpStatusCode, Func<HttpResponseMessage, Task<Exception>>> defaultErrorMapping,
            TimeSpan timeout,
            Action<HttpClient> preRequestActionForAllRequests
#if !WINDOWS_UWP && !PCL
            , X509Certificate2 clientCert
#endif
            )
        {
            this.baseAddress = baseAddress;
            this.authenticationHeaderProvider = authenticationHeaderProvider;
            this.defaultErrorMapping =
                new ReadOnlyDictionary<HttpStatusCode, Func<HttpResponseMessage, Task<Exception>>>(defaultErrorMapping);

#if !WINDOWS_UWP && !PCL
            WebRequestHandler handler = null;
            if (clientCert != null)
            {
                handler = new WebRequestHandler();
                handler.ClientCertificates.Add(clientCert);
                this.usingX509ClientCert = true;
            }

            this.httpClientObj = handler != null ? new HttpClient(handler) : new HttpClient();
#else
            this.httpClientObj = new HttpClient();
#endif

            this.httpClientObj.BaseAddress = this.baseAddress;
            this.httpClientObj.Timeout = timeout;
            this.httpClientObj.DefaultRequestHeaders.Accept.Add(new MediaTypeWithQualityHeaderValue(CommonConstants.MediaTypeForDeviceManagementApis));
            this.httpClientObj.DefaultRequestHeaders.ExpectContinue = false;
            if (preRequestActionForAllRequests != null)
            {
                preRequestActionForAllRequests(this.httpClientObj);
            }
        }

        public Task<T> GetAsync<T>(
            Uri requestUri,
            IDictionary<HttpStatusCode, Func<HttpResponseMessage, Task<Exception>>> errorMappingOverrides,
            IDictionary<string, string> customHeaders,
            CancellationToken cancellationToken)
        {
            return this.GetAsync<T>(requestUri, errorMappingOverrides, customHeaders, true, cancellationToken);
        }

        public async Task<T> GetAsync<T>(
            Uri requestUri,
            IDictionary<HttpStatusCode, Func<HttpResponseMessage, Task<Exception>>> errorMappingOverrides,
            IDictionary<string, string> customHeaders,
            bool throwIfNotFound,
            CancellationToken cancellationToken)
        {
            T result = default(T);

            if (throwIfNotFound)
            {
                await this.ExecuteAsync(
                    HttpMethod.Get,
                    new Uri(this.baseAddress, requestUri),
                    (requestMsg, token) => AddCustomHeaders(requestMsg, customHeaders),
                    async (message, token) => result = await ReadResponseMessageAsync<T>(message, token),
                    errorMappingOverrides,
                    cancellationToken);
            }
            else
            {
                await this.ExecuteAsync(
                   HttpMethod.Get,
                   new Uri(this.baseAddress, requestUri),
                   (requestMsg, token) => AddCustomHeaders(requestMsg, customHeaders),
                   message => message.IsSuccessStatusCode || message.StatusCode == HttpStatusCode.NotFound,
                   async (message, token) => result = message.StatusCode == HttpStatusCode.NotFound ? (default(T)) : await ReadResponseMessageAsync<T>(message, token),
                   errorMappingOverrides,
                   cancellationToken);
            }

            return result;
        }

#if WINDOWS_UWP || PCL
        public Task<T> PutAsync<T>(
#else
        public async Task<T> PutAsync<T>(
#endif
            Uri requestUri,
            T entity,
            PutOperationType operationType,
            IDictionary<HttpStatusCode, Func<HttpResponseMessage, Task<Exception>>> errorMappingOverrides,
            CancellationToken cancellationToken) where T: IETagHolder
        {
#if WINDOWS_UWP || PCL
            throw new NotImplementedException();
#else
            T result = default(T);

            await this.ExecuteAsync(
                    HttpMethod.Put,
                    new Uri(this.baseAddress, requestUri),
                    (requestMsg, token) =>
                    {
                        InsertEtag(requestMsg, entity, operationType);
                        requestMsg.Content = new ObjectContent<T>(entity, new JsonMediaTypeFormatter());
                        return Task.FromResult(0);
                    },
                    async (httpClient, token) => result = await ReadResponseMessageAsync<T>(httpClient, token),
                    errorMappingOverrides,
                    cancellationToken);

            return result;
#endif
        }

        static async Task<T> ReadResponseMessageAsync<T>(HttpResponseMessage message, CancellationToken token)
        {
            if (typeof(T) == typeof (HttpResponseMessage))
            {
                return (T) (object)message;
            }

#if WINDOWS_UWP || PCL
            // ReadAsAsync does not exist in UWP. Looks like all callers call this API 
            // with HttpResponseMessage, so it might be unreachable. If you reach it, implement deserialization from stream
            await message.Content.ReadAsStreamAsync();
            throw new NotImplementedException();
#else
            T entity = await message.Content.ReadAsAsync<T>(token);

            // Etag in the header is considered authoritative
            var eTagHolder = entity as IETagHolder;
            if (eTagHolder != null)
            {
                if (message.Headers.ETag != null && !string.IsNullOrWhiteSpace(message.Headers.ETag.Tag))
                {
                    // RDBug 3429280:Make the version field of Device object internal
                    eTagHolder.ETag = message.Headers.ETag.Tag;
                }
            }

            return entity;
#endif
        }

        static Task AddCustomHeaders(HttpRequestMessage requestMessage, IDictionary<string, string> customHeaders)
        {
            if (customHeaders != null)
            {
                foreach (var header in customHeaders)
                {
                    requestMessage.Headers.Add(header.Key, header.Value);
                }
            }

            return Task.FromResult(0);
        }

        static void InsertEtag(HttpRequestMessage requestMessage, IETagHolder entity, PutOperationType operationType)
        {
            if (operationType == PutOperationType.CreateEntity)
            {
                return;
            }

            if (operationType == PutOperationType.ForceUpdateEntity)
            {
                const string etag = "\"*\"";
                requestMessage.Headers.IfMatch.Add(new EntityTagHeaderValue(etag));
            }
            else
            {
                InsertEtag(requestMessage, entity);
            }
        }

        static void InsertEtag(HttpRequestMessage requestMessage, IETagHolder entity)
        {
            if (string.IsNullOrWhiteSpace(entity.ETag))
            {
                throw new ArgumentException("The entity does not have its ETag set.");
            }

            string etag = entity.ETag;

            if (!etag.StartsWith("\""))
            {
                etag = "\"" + etag;
            }

            if (!etag.EndsWith("\""))
            {
                etag = etag + "\"";
            }

            requestMessage.Headers.IfMatch.Add(new EntityTagHeaderValue(etag));
        }

        IDictionary<HttpStatusCode, Func<HttpResponseMessage, Task<Exception>>> MergeErrorMapping(
            IDictionary<HttpStatusCode, Func<HttpResponseMessage, Task<Exception>>> errorMappingOverrides)
        {
            var mergedMapping = this.defaultErrorMapping.ToDictionary(mapping => mapping.Key, mapping => mapping.Value);

            if (errorMappingOverrides != null)
            {
                foreach (var @override in errorMappingOverrides)
                {
                    mergedMapping[@override.Key] = @override.Value;
                }
            }

            return mergedMapping;
        }

        public Task PostAsync<T>(
            Uri requestUri, 
            T entity, 
            IDictionary<HttpStatusCode, Func<HttpResponseMessage, Task<Exception>>> errorMappingOverrides, 
            IDictionary<string, string> customHeaders, 
            CancellationToken cancellationToken)
        {
            return this.ExecuteAsync(
                HttpMethod.Post,
                new Uri(this.baseAddress, requestUri),
                (requestMsg, token) =>
                {
                    AddCustomHeaders(requestMsg, customHeaders);
                    if (entity != null)
                    {
                        if (typeof(T) == typeof(byte[]))
                        {
                            requestMsg.Content = new ByteArrayContent((byte[]) (object) entity);
                        }
                        else if (typeof(T) == typeof(string))
                        {
                            // only used to send batched messages on Http runtime
                            requestMsg.Content = new StringContent((string) (object)entity);
                            requestMsg.Content.Headers.ContentType = new MediaTypeHeaderValue(CommonConstants.BatchedMessageContentType);
                        }
                        else
                        {
#if WINDOWS_UWP || PCL
                            // System.Net.Http.Formatting does not exist in UWP. Need to find another way to create content
                            throw new NotImplementedException();
#else
                            requestMsg.Content = new ObjectContent<T>(entity, new JsonMediaTypeFormatter());
#endif
                        }
                    }

                    return Task.FromResult(0);
                },
                ReadResponseMessageAsync<HttpResponseMessage>,
                errorMappingOverrides,
                cancellationToken);
        }

        public async Task<T2> PostAsync<T1, T2>(
             Uri requestUri,
             T1 entity,
             IDictionary<HttpStatusCode, Func<HttpResponseMessage, Task<Exception>>> errorMappingOverrides,
             IDictionary<string, string> customHeaders,
             CancellationToken cancellationToken)
        {
            T2 result = default(T2);
            await this.PostAsyncHelper(
                requestUri,
                entity,
                errorMappingOverrides,
                customHeaders,
                async (message, token) => result = await ReadResponseMessageAsync<T2>(message, token),
                cancellationToken);

            return result;
        }

        Task PostAsyncHelper<T1>(
            Uri requestUri,
            T1 entity,
            IDictionary<HttpStatusCode, Func<HttpResponseMessage, Task<Exception>>> errorMappingOverrides,
            IDictionary<string, string> customHeaders,
            Func<HttpResponseMessage, CancellationToken, Task> processResponseMessageAsync,
            CancellationToken cancellationToken)
        {
            return this.ExecuteAsync(
                HttpMethod.Post,
                new Uri(this.baseAddress, requestUri),
                (requestMsg, token) =>
                {
                    AddCustomHeaders(requestMsg, customHeaders);
                    if (entity != null)
                    {
                        if (typeof(T1) == typeof(byte[]))
                        {
                            requestMsg.Content = new ByteArrayContent((byte[])(object)entity);
                        }
                        else if (typeof(T1) == typeof(string))
                        {
                            // only used to send batched messages on Http runtime
                            requestMsg.Content = new StringContent((string)(object)entity);
                            requestMsg.Content.Headers.ContentType = new MediaTypeHeaderValue(CommonConstants.BatchedMessageContentType);
                        }
                        else
                        {
#if WINDOWS_UWP || PCL
                            // System.Net.Http.Formatting does not exist in UWP. Need to find another way to create content
                            throw new NotImplementedException();
#else
                            requestMsg.Content = new ObjectContent<T1>(entity, new JsonMediaTypeFormatter());
#endif
                        }
                    }

                    return Task.FromResult(0);
                },
                processResponseMessageAsync,
                errorMappingOverrides,
                cancellationToken);
        }

        public Task DeleteAsync<T>(
            Uri requestUri,
            T entity,
            IDictionary<HttpStatusCode, Func<HttpResponseMessage, Task<Exception>>> errorMappingOverrides,
            IDictionary<string, string> customHeaders,
            CancellationToken cancellationToken) where T: IETagHolder
        {
            return this.ExecuteAsync(
                    HttpMethod.Delete,
                    new Uri(this.baseAddress, requestUri),
                    (requestMsg, token) =>
                    {
                        InsertEtag(requestMsg, entity);
                        AddCustomHeaders(requestMsg, customHeaders);
                        return TaskHelpers.CompletedTask;
                    },
                    null,
                    errorMappingOverrides,
                    cancellationToken);
        }

        Task ExecuteAsync(
            HttpMethod httpMethod,
            Uri requestUri,
            Func<HttpRequestMessage, CancellationToken, Task> modifyRequestMessageAsync,
            Func<HttpResponseMessage, CancellationToken, Task> processResponseMessageAsync,
            IDictionary<HttpStatusCode, Func<HttpResponseMessage, Task<Exception>>> errorMappingOverrides,
            CancellationToken cancellationToken)
        {
            return this.ExecuteAsync(
                httpMethod,
                requestUri,
                modifyRequestMessageAsync,
                message => message.IsSuccessStatusCode,
                processResponseMessageAsync,
                errorMappingOverrides,
                cancellationToken);
        }

        async Task ExecuteAsync(
            HttpMethod httpMethod,
            Uri requestUri,
            Func<HttpRequestMessage, CancellationToken, Task> modifyRequestMessageAsync,
            Func<HttpResponseMessage, bool> isSuccessful,
            Func<HttpResponseMessage, CancellationToken, Task> processResponseMessageAsync,
            IDictionary<HttpStatusCode, Func<HttpResponseMessage, Task<Exception>>> errorMappingOverrides,
            CancellationToken cancellationToken)
        {
            IDictionary<HttpStatusCode, Func<HttpResponseMessage, Task<Exception>>> mergedErrorMapping =
                this.MergeErrorMapping(errorMappingOverrides);

            using (var msg = new HttpRequestMessage(httpMethod, requestUri))
            {
                if (!this.usingX509ClientCert)
                {
                    msg.Headers.Add(HttpRequestHeader.Authorization.ToString(), this.authenticationHeaderProvider.GetAuthorizationHeader());
                }
#if !WINDOWS_UWP && !PCL
                msg.Headers.UserAgent.ParseAdd(Utils.GetClientVersion());
#endif
                if (modifyRequestMessageAsync != null) await modifyRequestMessageAsync(msg, cancellationToken);

                // TODO: pradeepc - find out the list of exceptions that HttpClient can throw.
                HttpResponseMessage responseMsg;
                try
                {
                    responseMsg = await this.httpClientObj.SendAsync(msg, cancellationToken);
                    if (responseMsg == null)
                    {
                        throw new InvalidOperationException("The response message was null when executing operation {0}.".FormatInvariant(httpMethod));
                    }

                    if (isSuccessful(responseMsg))
                    {
                        if (processResponseMessageAsync != null)
                        {
                            await processResponseMessageAsync(responseMsg, cancellationToken);
                        }
                    }                        
                }
                catch (AggregateException ex)
                {
                    var innerExceptions = ex.Flatten().InnerExceptions;
                    if (innerExceptions.Any(Fx.IsFatal))
                    {
                        throw;
                    }

                    // Apparently HttpClient throws AggregateException when a timeout occurs.
                    // TODO: pradeepc - need to confirm this with ASP.NET team
                    if (innerExceptions.Any(e => e is TimeoutException))
                    {
                        throw new IotHubCommunicationException(ex.Message, ex);
                    }

                    throw new IotHubException(ex.Message, ex);
                }
                catch (TimeoutException ex)
                {
                    throw new IotHubCommunicationException(ex.Message, ex);
                }
                catch (IOException ex)
                {
                    throw new IotHubCommunicationException(ex.Message, ex);
                }
                catch (HttpRequestException ex)
                {
                    throw new IotHubCommunicationException(ex.Message, ex);
                }
                catch (TaskCanceledException ex)
                {
                    // Unfortunately TaskCanceledException is thrown when HttpClient times out.
                    if (cancellationToken.IsCancellationRequested)
                    {
                        throw new IotHubException(ex.Message, ex);
                    }

                    throw new IotHubCommunicationException(string.Format(CultureInfo.InvariantCulture, "The {0} operation timed out.", httpMethod), ex);
                }
                catch (Exception ex)
                {
                    if (Fx.IsFatal(ex)) throw;

                    throw new IotHubException(ex.Message, ex);
                }

                if (!isSuccessful(responseMsg))
                {
                    Exception mappedEx = await MapToExceptionAsync(responseMsg, mergedErrorMapping);
                    throw mappedEx;
                }
            }
        }

        static async Task<Exception> MapToExceptionAsync(
            HttpResponseMessage response,
            IDictionary<HttpStatusCode, Func<HttpResponseMessage, Task<Exception>>> errorMapping)
        {
            Func<HttpResponseMessage, Task<Exception>> func;
            if (!errorMapping.TryGetValue(response.StatusCode, out func))
            {
                return new IotHubException(
                    await ExceptionHandlingHelper.GetExceptionMessageAsync(response),
                    isTransient: true);
            }

            var mapToExceptionFunc = errorMapping[response.StatusCode];
            var exception = mapToExceptionFunc(response);
            return await exception;
        }

        public void Dispose()
        {
            if (!this.isDisposed)
            {
                if (this.httpClientObj != null)
                {
                    this.httpClientObj.Dispose();
                    this.httpClientObj = null;
                }

                this.isDisposed = true;
            }
        }
    }
}