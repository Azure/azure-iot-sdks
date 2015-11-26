// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client
{
    using System;
    using System.IO;
    using System.Net;
    using System.Collections;
    using System.Text;
    
    using Microsoft.Azure.Devices.Client.Extensions;
    
    sealed class HttpClientHelper : IDisposable
    {
        readonly Uri baseAddress;
        readonly IAuthorizationHeaderProvider authenticationHeaderProvider;
        bool isDisposed;
        
        public HttpClientHelper(
            Uri baseAddress,
            IAuthorizationHeaderProvider authenticationHeaderProvider,
            TimeSpan timeout)
        {
            this.baseAddress = baseAddress;
            this.authenticationHeaderProvider = authenticationHeaderProvider;
        }

        public HttpWebResponse Get(
            string requestUri,
            Hashtable customHeaders)
        {
            return this.Get(requestUri, customHeaders, true);
        }

        public HttpWebResponse Get(
            string requestUri,
            Hashtable customHeaders,
            bool throwIfNotFound)
        {
            try
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
            catch (Exception)
            {
                if (throwIfNotFound)
                {
                    throw;
                }
                else
                {
                    return null;
                }
            }
        }

        static void AddCustomHeaders(HttpWebRequest requestMessage, Hashtable customHeaders)
        {
            foreach (var header in customHeaders.Keys)
            {
                requestMessage.Headers.Add(header as string, customHeaders[header] as string);
            }
        }

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

                if (entity == null)
                {
                    webRequest.ContentLength = 0;
                }
                else
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
                        webRequest.ContentLength = 0;
                    }
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
                    if (webResponse.StatusCode != HttpStatusCode.NoContent)
                    {
                        throw new WebException("", null, WebExceptionStatus.ReceiveFailure, webResponse);
                    }
                    // else it's a success and there's nothing to do but exit the method.
                }
            }
        }

        public void Dispose()
        {
            if (!this.isDisposed)
            {
                this.isDisposed = true;
            }
        }
    }
}