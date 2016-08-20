// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

import { Client } from 'azure-iot-device';

export import Http = require('./lib/http');
export declare function clientFromConnectionString(connectionString: string): Client;
