// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

import { EventEmitter } from 'events';
import { results, Message, Receiver } from 'azure-iot-common';
import { AmqpMessage } from 'azure-iot-amqp-base';

declare class Client extends EventEmitter {
    constructor(transport: Client.Transport);
    open(done: (err: Error, result?: results.Connected) => void): void;
    close(done: (err: Error) => void): void;
    send(deviceId: string, message: Message | Message.BufferConvertible, done: (err: Error, result?: results.MessageEnqueued) => void): void;
    getFeedbackReceiver(done: (err: Error, receiver?: Client.ServiceReceiver) => void): void;
    getFileNotificationReceiver(done: (err: Error, receiver?: Client.ServiceReceiver) => void): void;

    on(type: 'disconnect', func: (reason: results.Disconnected) => void ): this;
    // Required overload, not used during normal operation
    on(type: string, func: Function): this;

    static fromConnectionString(connStr: string, transportCtr?: Client.TransportCtor): Client;
    static fromSharedAccessSignature(sharedAccessSignature: string, tranposrtCtr?: Client.TransportCtor): Client;
}

declare namespace Client {
    interface TransportConfigOptions {
        host: string;
        hubName: string;
        keyName: string;
        sharedAccessSignature: string;
    }
    interface ServerMessage extends AmqpMessage {
    }
    interface ServiceReceiver extends Receiver {
        complete(message: ServerMessage, done?: (err: Error, result?: results.MessageCompleted) => void): void;
        abandon(message: ServerMessage, done?: (err: Error, result?: results.MessageAbandoned) => void): void;
        reject(message: ServerMessage, done?: (err: Error, result?: results.MessageRejected) => void): void;
    }
    interface Transport {
        connect(done?: (err: Error, result?: results.Connected) => void): void;
        disconnect(done: (err: Error) => void): void;
        send(deviceId: string, message: Message, done?: (err: Error, result?: results.MessageEnqueued) => void): void;
        getFeedbackReceiver(done: (err: Error, receiver?: ServiceReceiver) => void): void;
        getFileNotificationReceiver(done: (err: Error, receiver?: ServiceReceiver) => void): void;
    }

    type TransportCtor = new(config: Client.TransportConfigOptions) => Client.Transport;
}

export = Client;
