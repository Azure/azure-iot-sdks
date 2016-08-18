// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

import { EventEmitter } from 'events';
import { Receiver, Message, results, X509 } from 'azure-iot-common';
import { Client } from 'azure-iot-device';

declare class Amqp extends EventEmitter implements Client.Transport {
    constructor(config: Client.Config);

    connect(done: (err: Error, result?: results.Connected) => void): void;
    disconnect(done?: (err: Error) => void): void;
    updateSharedAccessSignature(sharedAccessSignature: string, done: (err: Error, result?: results.SharedAccessSignatureUpdated) => void): void;
    getReceiver(func: (err: Error, receiver?: Receiver) => void): void;
    sendEvent(message: Message, done: (err: Error, result?: results.MessageEnqueued) => void): void;
    sendEventBatch(messages: Message[], done: (err: Error, result?: results.MessageEnqueued) => void): void;
    complete(message: Message, done: (err: Error, result?: results.MessageCompleted) => void): void;
    reject(message: Message, done: (err: Error, results?: results.MessageRejected) => void): void;
    abandon(message: Message, done: (err: Error, results?: results.MessageAbandoned) => void): void;
    setOption(options: Amqp.Options, done: () => void): void;
}

declare namespace Amqp {
    interface Options extends X509 {
        certFile?: string;
        keyFile?: string;
    }
}

export = Amqp;
