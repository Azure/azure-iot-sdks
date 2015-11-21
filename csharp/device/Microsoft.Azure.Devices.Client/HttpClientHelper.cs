// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client
{
    using System;
    using System.IO;
    using System.Net;
#if !WINDOWS_UWP && !NETMF
    using System.Net.Http.Formatting;
#endif

#if NETMF
    using System.Collections;
    using System.Text;
#else
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
#endif

    using Microsoft.Azure.Devices.Client.Exceptions;
    using Microsoft.Azure.Devices.Client.Extensions;

#if NETMF
    sealed class HttpClientHelper : IDisposable
#else
    sealed class HttpClientHelper : IHttpClientHelper
#endif
    {
        readonly Uri baseAddress;
        readonly IAuthorizationHeaderProvider authenticationHeaderProvider;

#if !NETMF
        readonly IReadOnlyDictionary<HttpStatusCode, Func<HttpResponseMessage, Task<Exception>>> defaultErrorMapping;
        HttpClient httpClientObj;
#endif
        bool isDisposed;

        public HttpClientHelper(
            Uri baseAddress,
            IAuthorizationHeaderProvider authenticationHeaderProvider,
#if NETMF
            TimeSpan timeout)
#else
            IDictionary<HttpStatusCode, Func<HttpResponseMessage, Task<Exception>>> defaultErrorMapping,
            TimeSpan timeout,
            Action<HttpClient> preRequestActionForAllRequests)
#endif
        {
            this.baseAddress = baseAddress;
            this.authenticationHeaderProvider = authenticationHeaderProvider;

#if !NETMF
            this.defaultErrorMapping =
                new ReadOnlyDictionary<HttpStatusCode, Func<HttpResponseMessage, Task<Exception>>>(defaultErrorMapping);

            this.httpClientObj = new HttpClient();
            this.httpClientObj.BaseAddress = this.baseAddress;
            this.httpClientObj.Timeout = timeout;
            this.httpClientObj.DefaultRequestHeaders.Accept.Add(new MediaTypeWithQualityHeaderValue(CommonConstants.MediaTypeForDeviceManagementApis));
            this.httpClientObj.DefaultRequestHeaders.ExpectContinue = false; 
            if (preRequestActionForAllRequests != null)
            {
                preRequestActionForAllRequests(this.httpClientObj);
            }
#endif
        }

#if NETMF
        public HttpWebResponse Get(
            string requestUri,
            Hashtable customHeaders)
        {
            return this.Get(requestUri, customHeaders, true);
        }
#else
        public Task<T> GetAsync<T>(
            Uri requestUri,
            IDictionary<HttpStatusCode, Func<HttpResponseMessage, Task<Exception>>> errorMappingOverrides,
            IDictionary<string, string> customHeaders,
            CancellationToken cancellationToken)
        {
            return this.GetAsync<T>(requestUri, errorMappingOverrides, customHeaders, true, cancellationToken);
        }
#endif

#if NETMF
        public HttpWebResponse Get(
            string requestUri,
            Hashtable customHeaders,
            bool throwIfNotFound)
        {
            if (throwIfNotFound)
            {

                using (var webRequest = (HttpWebRequest)WebRequest.Create(new Uri(this.baseAddress.OriginalString + requestUri)))
                {
                    webRequest.Method = "GET";

                    // add authorization header
                    webRequest.Headers.Add("Authorization", this.authenticationHeaderProvider.GetAuthorizationHeader());

                    // add custom headers
                    AddCustomHeaders(webRequest, customHeaders);

                    // perform request and get response
                    // don't close the WebResponse here because we may need to read the response stream later 
                    var webResponse = webRequest.GetResponse() as HttpWebResponse;

                    // message received
                    return webResponse;
                }
            }

            return null;
        }
#else
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
#endif

#if !NETMF
        public async Task<T> PutAsync<T>(
            Uri requestUri,
            T entity,
            PutOperationType operationType,
            IDictionary<HttpStatusCode, Func<HttpResponseMessage, Task<Exception>>> errorMappingOverrides,
            CancellationToken cancellationToken) where T: IETagHolder
        {
#if WINDOWS_UWP
            await Task.CompletedTask; // to suppress async warning
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
#endif
        
        static async Task<T> ReadResponseMessageAsync<T>(HttpResponseMessage message, CancellationToken token)
        {
            if (typeof(T) == typeof (HttpResponseMessage))
            {
                return (T) (object)message;
            }

#if WINDOWS_UWP
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

#if NETMF
        static void AddCustomHeaders(HttpWebRequest requestMessage, Hashtable customHeaders)
        {
            foreach (var header in customHeaders.Keys)
            {
                requestMessage.Headers.Add(header as string, customHeaders[header] as string);
            }
        }
#else

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
#endif

#if NETMF
        static void InsertEtag(HttpWebRequest requestMessage, IETagHolder entity, PutOperationType operationType)
        {
            if (operationType == PutOperationType.CreateEntity)
            {
                return;
            }

            if (operationType == PutOperationType.ForceUpdateEntity)
            {
                const string etag = "\"*\"";
                requestMessage.Headers.Add("IfMatch", etag);
            }
            else
            {
                InsertEtag(requestMessage, entity);
            }
        }
#else
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
#endif

#if NETMF
        static void InsertEtag(HttpWebRequest requestMessage, IETagHolder entity)
        {
            if (entity.ETag.IsNullOrWhiteSpace())
            {
                throw new ArgumentException("The entity does not have its ETag set.");
            }

            string etag = entity.ETag;

            if (etag.IndexOf("\"") != 0)
            {
                etag = "\"" + etag;
            }

            if (etag.LastIndexOf("\"") != etag.Length)
            {
                etag = etag + "\"";
            }

            requestMessage.Headers.Add("IfMatch", etag);
        }
#else
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
#endif

#if !NETMF
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
#endif

#if NETMF
        public void Post(
            string requestUri,
            object entity,
            Hashtable customHeaders)
        {
            using (var webRequest = (HttpWebRequest)WebRequest.Create(new Uri(this.baseAddress.OriginalString + requestUri)))
            {
                //webRequest.ProtocolVersion = HttpVersion.Version11;
                webRequest.KeepAlive = true;
                webRequest.Method = "POST";

                // add authorization header
                webRequest.Headers.Add("Authorization", this.authenticationHeaderProvider.GetAuthorizationHeader());

                // add custom headers
                AddCustomHeaders(webRequest, customHeaders);
                webRequest.AllowWriteStreamBuffering = true;

                if (entity != null)
                {
                    if (entity.GetType().Equals(typeof(MemoryStream)))
                    {
                        // need to set these before getting the request stream
                        webRequest.ContentLength = ((Stream)entity).Length;

                        int totalBytes = 0;
                        using (var requestStream = webRequest.GetRequestStream())
                        {
                            var buffer = new byte[256];
                            var bytesRead = 0;

                            while ((bytesRead = ((Stream)entity).Read(buffer, 0, 256)) > 0)
                            {
                                requestStream.Write(buffer, 0, bytesRead);

                                totalBytes += bytesRead;
                            }
                        }

                    }
                    else if (entity.GetType().Equals(typeof(string)))
                    {

                        var buffer = Encoding.UTF8.GetBytes(entity as string);

                        // need to set these before getting the request stream
                        webRequest.ContentLength = buffer.Length;
                        webRequest.ContentType = CommonConstants.BatchedMessageContentType;

                        int bytesSent = 0;

                        using (var requestStream = webRequest.GetRequestStream())
                        {
                            var chunkBytes = 0;

                            while (bytesSent < buffer.Length)
                            {
                                // calculate bytes count for this chunk
                                chunkBytes = (buffer.Length - bytesSent) < 256 ? (buffer.Length - bytesSent) : 256;

                                // write chunk
                                requestStream.Write(buffer, bytesSent, chunkBytes);

                                // update counter
                                bytesSent += chunkBytes;
                            }
                        }

                    }
                    else
                    {
                        // TODO
                        // requestMsg.Content = new ObjectContent<T>(entity, new JsonMediaTypeFormatter());
                        webRequest.ContentLength = 0;
                    }
                }
                else
                {
                    webRequest.ContentLength = 0;
                }

                // perform request and get response
                using (var webResponse = webRequest.GetResponse() as HttpWebResponse)
                {
                    if (webResponse.StatusCode == HttpStatusCode.NoContent)
                    {
                        // success!
                        return;
                    }
                    else
                    {
                        throw new WebException("", null, WebExceptionStatus.ReceiveFailure, webResponse);
                    }
                }
            }
        }
#else
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
#if WINDOWS_UWP
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
#endif

#if NETMF
        public void Delete(
            string requestUri,
            IETagHolder etag,
            Hashtable customHeaders)
        {
            using (var webRequest = (HttpWebRequest)WebRequest.Create(new Uri(this.baseAddress.OriginalString + requestUri)))
            {
                webRequest.Method = "DELETE";

                // add authorization header
                webRequest.Headers.Add("Authorization", this.authenticationHeaderProvider.GetAuthorizationHeader());

                // add custom headers
                AddCustomHeaders(webRequest, customHeaders);

                // add ETag header
                InsertEtag(webRequest, etag);

                // perform request and get response
                using (var webResponse = webRequest.GetResponse() as HttpWebResponse)
                {
                    if (webResponse.StatusCode == HttpStatusCode.NoContent)
                    {
                        // success!
                        return;
                    }
                    else
                    {
                        throw new WebException("", null, WebExceptionStatus.ReceiveFailure, webResponse);
                    }
                }
            }
        }
#else
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
#endif

#if !NETMF
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
                msg.Headers.Add(HttpRequestHeader.Authorization.ToString(), this.authenticationHeaderProvider.GetAuthorizationHeader());

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
#endif

        public void Dispose()
        {
            if (!this.isDisposed)
            {
#if !NETMF

                if (this.httpClientObj != null)
                {
                    this.httpClientObj.Dispose();
                    this.httpClientObj = null;
                }
#endif

                this.isDisposed = true;
            }
        }
    }
}