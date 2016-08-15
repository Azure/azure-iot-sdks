// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

import { EventEmitter } from 'events';
import { ReceiverLink as Amqp10ReceiverLink } from 'amqp10';
import { results } from 'azure-iot-common';

import AmqpMessage = require('./amqp_message');

declare class AmqpReceiver extends EventEmitter {
    constructor(amqpReceiver: Amqp10ReceiverLink);

    complete(message: AmqpMessage, done?: (err: Error, result?: results.MessageCompleted) => void): void;
    abandon(message: AmqpMessage, done?: (err: Error, result?: results.MessageAbandoned) => void): void;
    reject(message: AmqpMessage, done?: (err: Error, result?: results.MessageRejected) => void): void;
}

export = AmqpReceiver;
