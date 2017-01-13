// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

import { EventEmitter } from 'events';
import { Stream } from 'stream';
import { Message, results, Receiver, TransportConfig } from 'azure-iot-common';

declare class Client extends EventEmitter {
    constructor(transport: Client.Transport, connStr: string, blobUploadClient: Client.BlobUpload);

    updateSharedAccessSignature(sharedAccessSignature: string, done: (err: Error, result?: results.SharedAccessSignatureUpdated) => void): void;
    open(done: (err: Error, result?: results.Connected) => void): void;
    sendEvent(message: Message, done: (err: Error, result?: results.MessageEnqueued) => void): void;
    sendEventBatch(messages: Message[], done: (err: Error, result?: results.MessageEnqueued) => void): void;
    close(done: (err: Error, result?: results.Disconnected) => void): void;
    setTransportOptions(options: {}, done: (err: Error, result?: results.TransportConfigured) => void): void;
    setOptions(options: {}, done: (err: Error) => void): void;
    complete(message: Message, done: (err: Error, result?: results.MessageCompleted) => void): void;
    reject(message: Message, done: (err: Error, results?: results.MessageRejected) => void): void;
    abandon(message: Message, done: (err: Error, results?: results.MessageAbandoned) => void): void;
    uploadToBlob(blobName: string, stream: Stream, steamLength: number, done: (err?: Error) => void): void;

    on(type: 'message', func: (msg: Message) => void): this;
    on(type: 'error', func: (err: Error) => void): this;
    on(type: 'disconnect', func: (result: results.Disconnected) => void): this;
    // Required operator, shouldn't be used during normal operation
    on(type: string, func: Function): this;

    static fromConnectionString(connStr: string, transport: Client.TransportCtor): Client;
    static fromSharedAccessSignature(sharedAccessSignature: string, transport: Client.TransportCtor): Client;
}

declare namespace Client {
    interface Config extends TransportConfig {
        hubName: string;
    }

    interface Transport {
        updateSharedAccessSignature(sharedAccessSignature: string, done: (err: Error, result?: results.SharedAccessSignatureUpdated) => void): void;
        getReceiver(func: (err: Error, receiver?: Receiver) => void): void;
        sendEvent(message: Message, done: (err: Error, result?: results.MessageEnqueued) => void): void;
        sendEventBatch(messages: Message[], done: (err: Error, result?: results.MessageEnqueued) => void): void;
        complete(message: Message, done: (err: Error, result?: results.MessageCompleted) => void): void;
        reject(message: Message, done: (err: Error, results?: results.MessageRejected) => void): void;
        abandon(message: Message, done: (err: Error, results?: results.MessageAbandoned) => void): void;
    }

    interface BlobUpload {
        uploadToBlob(blobName: string, stream: Stream, steamLength: number, done: (err?: Error) => void): void;
        updateSharedAccessSignature(sharedAccessSignature: string): void;
    }

    type TransportCtor = new(config: Config) => Transport;
}

export = Client;
