// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

import { Message, Receiver, results } from 'azure-iot-common';
import { Http as HttpBase } from 'azure-iot-http-base';
import { Client } from 'azure-iot-device';

declare class HttpReceiver implements Receiver {
    constructor(config: Client.Config, httpHelper: HttpBase);

    receive(): void;
    setOptions(opts: HttpReceiver.Options): void;
    updateSharedAccessSignature(sharedAccessSignature: string): void;
    complete(message: Message, done: (err: Error, result?: results.MessageCompleted) => void): void;
    reject(message: Message, done: (err: Error, results?: results.MessageRejected) => void): void;
    abandon(message: Message, done: (err: Error, results?: results.MessageAbandoned) => void): void;


    on(type: 'message', func: (msg: Message) => void): this;
    on(type: 'errorReceived', func: (err: Error) => void): this;

    on(type: string, func: Function): this;
}

declare namespace HttpReceiver {
    interface Options {
        interval?: number;
        at?: Date;
        cron?: string;
        drain?: boolean;
    }
}

export = HttpReceiver;
