// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

import { Packet } from 'mqtt';
import { X509, results } from 'azure-iot-common';
import MqttReceiver = require('./mqtt_receiver');

declare class Mqtt {
    // There is currently no way for the parameter to be a whole TypeScript namespace
    constructor(mqttprovider: any);
    connect(config: Mqtt.ConnectOptions, done: (err: Error, ack?: Packet) => void): void;
    disconnect(done: () => void): void;
    publish(message: { data: { toString(): string }}, done: (err: Error, result?: results.MessageEnqueued) => void): void;
    getReceiver(done: (err: Error, receiver: MqttReceiver) => void): void;
}

declare namespace Mqtt {
    interface ConnectOptions {
        host: string;
        deviceId: string;
        sharedAccessSignature?: string;
        x509?: X509;
    }
}

export = Mqtt;
