// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

import { Errors as AmqpErrors } from 'amqp10';

declare function translateError(message: string, amqpError: AmqpErrors.BaseError): Error;

export = translateError;
