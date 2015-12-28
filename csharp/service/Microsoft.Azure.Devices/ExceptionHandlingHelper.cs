// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices
{
    using System;
    using System.Collections.Generic;
    using System.Net;
    using System.Net.Http;
    using System.Threading.Tasks;

    using Microsoft.Azure.Devices.Common.Exceptions;

    class ExceptionHandlingHelper
    {
        public static IDictionary<HttpStatusCode, Func<HttpResponseMessage, Task<Exception>>> GetDefaultErrorMapping()
        {
            var mappings = new Dictionary<HttpStatusCode, Func<HttpResponseMessage, Task<Exception>>>();

            mappings.Add(HttpStatusCode.NoContent, async (response) => new DeviceNotFoundException(await GetExceptionMessageAsync(response)));
            mappings.Add(HttpStatusCode.NotFound, async (response) => new DeviceNotFoundException(await GetExceptionMessageAsync(response)));
            mappings.Add(HttpStatusCode.Conflict, async (response) => new DeviceAlreadyExistsException(await GetExceptionMessageAsync(response)));
            mappings.Add(HttpStatusCode.BadRequest, async (response) => new ArgumentException(await GetExceptionMessageAsync(response)));
            mappings.Add(HttpStatusCode.Unauthorized, async (response) => new UnauthorizedException(await GetExceptionMessageAsync(response)));
            mappings.Add(HttpStatusCode.Forbidden, async (response) => new QuotaExceededException(await GetExceptionMessageAsync(response)));
            mappings.Add(HttpStatusCode.PreconditionFailed, async (response) => new DeviceMessageLockLostException(await GetExceptionMessageAsync(response)));
            mappings.Add(HttpStatusCode.RequestEntityTooLarge, async (response) => new MessageTooLargeException(await GetExceptionMessageAsync(response))); ;
            mappings.Add(HttpStatusCode.InternalServerError, async (response) => new ServerErrorException(await GetExceptionMessageAsync(response)));
            mappings.Add(HttpStatusCode.ServiceUnavailable, async (response) => new ServerBusyException(await GetExceptionMessageAsync(response)));

            return mappings;
        }

        public static Task<string> GetExceptionMessageAsync(HttpResponseMessage response)
        {
            // TODO: pradeepc - consider parsing to HttpError
            return response.Content.ReadAsStringAsync();
        }
    }
}
