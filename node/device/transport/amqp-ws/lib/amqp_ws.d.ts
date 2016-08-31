// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

import { Client } from 'azure-iot-device';
import { Amqp }  from 'azure-iot-device-amqp';

declare class AmqpWs extends Amqp implements Client.Transport {
    constructor(config: Client.Config);
}

export = AmqpWs;
