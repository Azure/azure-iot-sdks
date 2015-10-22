// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Common.WebApi
{
    using System;
    using System.Collections.Generic;
    using System.Net;
    using System.Net.Http;
    using System.Web.Http;

    using Microsoft.Azure.Devices.Common.Exceptions;

    public sealed class ErrorMapper
    {
        static readonly IDictionary<Type, Func<HttpRequestMessage, Exception, HttpResponseMessage>> ErrorMappers
            = new Dictionary<Type, Func<HttpRequestMessage, Exception, HttpResponseMessage>>
              {
                  {typeof (HttpResponseException), (r, ex) => ((HttpResponseException) ex).Response},
                  {typeof (UnauthorizedAccessException), (r, ex) => r.CreateErrorResponse(HttpStatusCode.Unauthorized, CreateHttpError(r, ex))},
                  {typeof (IotHubSuspendedException), (r, ex) => r.CreateErrorResponse(HttpStatusCode.Forbidden, CreateHttpError(r, ex))},
                  {typeof (InvalidProtocolVersionException), (r, ex) => r.CreateErrorResponse(HttpStatusCode.BadRequest, CreateHttpError(r, ex))},
                  {typeof (DeviceNotFoundException), (r, ex) => r.CreateErrorResponse(HttpStatusCode.NotFound, CreateHttpError(r, ex))},
                  {typeof (IotHubNotFoundException), (r, ex) => r.CreateErrorResponse(HttpStatusCode.Unauthorized, CreateHttpError(r, ex))},
                  {typeof (DeviceAlreadyExistsException), (r, ex) => r.CreateErrorResponse(HttpStatusCode.Conflict, CreateHttpError(r, ex))},
                  {typeof (DeviceInvalidResultCountException), (r, ex) => r.CreateErrorResponse(HttpStatusCode.BadRequest, CreateHttpError(r, ex))},
                  {typeof (PreconditionFailedException), (r, ex) => r.CreateErrorResponse(HttpStatusCode.PreconditionFailed, CreateHttpError(r, ex))},
                  {typeof (InvalidOperationException), (r, ex) => r.CreateErrorResponse(HttpStatusCode.BadRequest, CreateHttpError(r, ex))},
                  {typeof (ArgumentException), (r, ex) => r.CreateErrorResponse(HttpStatusCode.BadRequest, CreateHttpError(r, ex))},
                  {typeof (ArgumentNullException), (r, ex) => r.CreateErrorResponse(HttpStatusCode.BadRequest, CreateHttpError(r, ex))},
                  {typeof (QuotaExceededException), (r, ex) => r.CreateErrorResponse(HttpStatusCode.Forbidden, CreateHttpError(r, ex))},
                  {typeof (FormatException), (r, ex) => r.CreateErrorResponse(HttpStatusCode.BadRequest, CreateHttpError(r, ex))},
                  {typeof (ServerErrorException), (r, ex) => r.CreateErrorResponse(HttpStatusCode.InternalServerError, CreateHttpError(r, ex))},
                  {typeof (MessageTooLargeException), (r, ex) => r.CreateErrorResponse(HttpStatusCode.RequestEntityTooLarge, CreateHttpError(r, ex))},
                  {typeof (DeviceMessageLockLostException), (r, ex) => r.CreateErrorResponse(HttpStatusCode.PreconditionFailed, CreateHttpError(r, ex))},
                  {typeof (JobQuotaExceededException), (r, ex) => r.CreateErrorResponse(HttpStatusCode.Forbidden, CreateHttpError(r, ex))},
                  {typeof (JobNotFoundException), (r, ex) => r.CreateErrorResponse(HttpStatusCode.NotFound, CreateHttpError(r, ex))},
                  {typeof (TimeoutException), (r, ex) => r.CreateErrorResponse(HttpStatusCode.ServiceUnavailable, CreateHttpError(r, ex))},

                  // 429 - Too Many Requests
                  {typeof (ThrottlingException), (r, ex) => r.CreateErrorResponse((HttpStatusCode)429, CreateHttpError(r, ex))}
              };

        public static HttpResponseMessage HandleException(HttpRequestMessage request, Exception ex,
            IDictionary<Type, Func<HttpRequestMessage, Exception, HttpResponseMessage>> additionalErrorMappings)
        {
            Func<HttpRequestMessage, Exception, HttpResponseMessage> mapper;
            HttpResponseMessage response;

            if (!ErrorMappers.TryGetValue(ex.GetType(), out mapper))
            {
                if (additionalErrorMappings != null)
                {
                    additionalErrorMappings.TryGetValue(ex.GetType(), out mapper);
                }
            }

            if (mapper != null)
            {
                response = mapper(request, ex);
            }
            else
            {
                response = MapServerError(request, ex);
            }

            if (request.Method == HttpMethod.Head)
            {
                response.Content = null;
            }

            return response;
        }

        static HttpResponseMessage MapServerError(HttpRequestMessage request, Exception ex)
        {
            return request.CreateErrorResponse(HttpStatusCode.InternalServerError, CreateHttpError(request, ex));
        }

        static HttpError CreateHttpError(HttpRequestMessage request, Exception exception)
        {
            bool dbgEnabled = request.GetConfiguration().IncludeErrorDetailPolicy == IncludeErrorDetailPolicy.Always;
            var httpError = new HttpError(exception, dbgEnabled);
            if (!dbgEnabled)
            {
                httpError.Message = exception.Message;       // Even if ErrorDetailPolicy is not Always, include the actual error message     
            }

            return httpError;
        }
    }
}
