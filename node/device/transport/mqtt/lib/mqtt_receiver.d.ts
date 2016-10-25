// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

import { EventEmitter } from 'events';
import { Client as MqttClient } from 'mqtt';
import { Receiver, Message } from 'azure-iot-common';

declare class MqttReceiver extends EventEmitter implements Receiver {
    // Want to have the same variables as the source
    // tslint:disable-next-line:variable-name
    constructor(mqttClient: MqttClient, topic_subscribe: string);

    on(type: 'message', func: (msg: Message) => void): this;
    on(type: 'errorReceived', func: (err: Error) => void): this;
    on(type: string, func: Function): this;
}

export = MqttReceiver;
