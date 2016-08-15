// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

import { EventEmitter } from 'events';
import { Client as MqttClient } from 'mqtt';

declare class MqttReceiver extends EventEmitter {
    // Want to have the same variables as the source
    // tslint:disable-next-line:variable-name
    constructor(mqttClient: MqttClient, topic_subscribe: string);
}

export = MqttReceiver;
