// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

import { Client } from 'azure-iot-device';

export import AmqpWs = require('./lib/amqp_ws');
export declare function clientFromConnectionString(connectionString: string): Client;
