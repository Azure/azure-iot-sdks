// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

import { Policy } from 'amqp10';

import { Message } from 'azure-iot-common';
import { results } from 'azure-iot-common';

import AmqpReceiver = require('./amqp_receiver');

declare class Amqp {
    constructor(autoSettleMessage: boolean, sdkVersionString: string);

    connect(uri: string, sslOptions: Policy.Options.SSLOptions, done?: (err: Error, result?: results.Connected) => void): void;
    setDisconnectHandler(disconnectCallback: (msg: string) => void): void;
    disconnect(done?: (err: Error) => void): void;
    send(message: Message, endpoint: string, to: string, done?: (err: Error, result?: results.MessageEnqueued) => void): void;
    getReceiver(endpoint: string, done: (err: Error, receiver?: AmqpReceiver) => void): void;
}

export = Amqp;
