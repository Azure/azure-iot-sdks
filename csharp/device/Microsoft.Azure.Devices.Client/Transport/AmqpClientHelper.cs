// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client.Transport
{
    using System;
    using Microsoft.Azure.Amqp;
    using Microsoft.Azure.Devices.Client.Exceptions;

    class AmqpClientHelper
    {
        public static Exception ToIotHubClientContract(Exception exception)
        {
            if (exception is TimeoutException)
            {
                return new IotHubCommunicationException(exception.Message);
            }
            else if (exception is UnauthorizedAccessException)
            {
                return new UnauthorizedException(exception.Message);
            }
            else
            {
                var amqpException = exception as AmqpException;
                if (amqpException != null)
                {
                    return AmqpErrorMapper.ToIotHubClientContract(amqpException.Error);
                }

                return exception;
            }
        }
    }
}
