// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

import { EventEmitter } from 'events';
import { results, Message } from 'azure-iot-common';
import { Amqp as AmqpBase } from 'azure-iot-amqp-base';

import Client = require('./client');

declare class Amqp extends EventEmitter implements Client.Transport {
    constructor(config: Client.TransportConfigOptions, amqpBase?: AmqpBase);
    connect(done?: (err: Error, result?: results.Connected) => void): void;
    disconnect(done: (err: Error) => void): void;
    send(deviceId: string, message: Message, done?: (err: Error, result?: results.MessageEnqueued) => void): void;
    // not including deprecated getReceiver on purpose
    getFeedbackReceiver(done: (err: Error, receiver?: Client.ServiceReceiver) => void): void;
    getFileNotificationReceiver(done: (err: Error, receiver?: Client.ServiceReceiver) => void): void;

    on(type: 'disconnect', func: (err: Error) => void): this;
    // Required overload, not used during normal operation
    on(type: string, func: Function): this;
}

export = Amqp;
