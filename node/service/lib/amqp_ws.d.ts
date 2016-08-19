// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

import Amqp = require('./amqp');
import Client = require('./client');

declare class AmqpWs extends Amqp implements Client.Transport {
    constructor(config: Client.TransportConfigOptions);
}

export = AmqpWs;
