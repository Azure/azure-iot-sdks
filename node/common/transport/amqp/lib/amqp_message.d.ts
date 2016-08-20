// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

import { Message } from 'azure-iot-common';

declare interface AmqpMessage {
    properties: AmqpMessage.Properties;
    body?: Buffer;
}

declare namespace AmqpMessage {
    export function fromMessage(message: Message): AmqpMessage;

    interface Dictionary<T> {
        [key: string]: T;
    }

    interface Properties {
        to?: string;
        absoluteExpiryTime?: Date;
        messageId?: string;
        applicationProperties: Dictionary<string>;
    }
}

export = AmqpMessage;
