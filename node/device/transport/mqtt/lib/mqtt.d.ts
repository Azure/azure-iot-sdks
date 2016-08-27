// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

import { EventEmitter } from 'events';
import { Message, Receiver, results, X509 } from 'azure-iot-common';
import { Client } from 'azure-iot-device';

declare class Mqtt extends EventEmitter implements Client.Transport {
    constructor(config: Client.Config);

    connect(done: (err: Error, result: results.Connected) => void): void;
    disconnect(done: (err: Error, results: results.Disconnected) => void): void;
    setOptions(options: Mqtt.Options, done: (err?: Error) => void): void;

    updateSharedAccessSignature(sharedAccessSignature: string, done: (err: Error, result?: results.SharedAccessSignatureUpdated) => void): void;
    getReceiver(func: (err: Error, receiver?: Receiver) => void): void;
    sendEvent(message: Message, done: (err: Error, result?: results.MessageEnqueued) => void): void;
    sendEventBatch(messages: Message[], done: (err: Error, result?: results.MessageEnqueued) => void): void;
    complete(message: Message, done: (err: Error, result?: results.MessageCompleted) => void): void;
    reject(message: Message, done: (err: Error, results?: results.MessageRejected) => void): void;
    abandon(message: Message, done: (err: Error, results?: results.MessageAbandoned) => void): void;
    sendMethodResponse(response: Client.DeviceMethodResponse, done?: (err: Error) => void): void;

    on(type: 'disconnect', func: (err: Error) => void): this;

    on(type: string, func: Function): this;
}

declare namespace Mqtt {
    interface Options extends X509 {
    }
}

export = Mqtt;
