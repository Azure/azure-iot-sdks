// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

import { Client } from 'azure-iot-device';
import Mqtt = require('./mqtt');

declare class MqttWs extends Mqtt implements Client.Transport {
    constructor(config: Client.Config);
}

export = MqttWs;
