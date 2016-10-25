// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

import { Packet } from 'mqtt';
import { results, TransportConfig } from 'azure-iot-common';
import MqttReceiver = require('./mqtt_receiver');

declare class MqttBase {
    // There is currently no way for the parameter to be a whole TypeScript namespace
    // So there is no way to specify what the mqttprovider module should expose
    constructor(mqttprovider: any);
    connect(config: TransportConfig, done: (err: Error, ack?: Packet) => void): void;
    disconnect(done: () => void): void;
    publish(message: { data: { toString(): string }}, done: (err: Error, result?: results.MessageEnqueued) => void): void;
    getReceiver(done: (err: Error, receiver: MqttReceiver) => void): void;
}

export = MqttBase;
