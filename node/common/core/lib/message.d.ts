// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

import Properties = require('./properties');

declare class Message {
    properties: Properties;
    messageId: string;
    to: string;
    expiryTimeUtc: Date;
    lockToken: string;
    correlationId: string;
    userId: string;

    constructor(data: Message.BufferConvertible);

    getData(): Message.BufferConvertible;
    getBytes(): Buffer;
}

declare namespace Message {
    type BufferConvertible = Buffer | String | any[] | ArrayBuffer;
}

export = Message;
