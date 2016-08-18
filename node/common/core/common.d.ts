// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

import { EventEmitter } from 'events';

export { anHourFromNow, encodeUriComponentStrict } from './lib/authorization';
export import ConnectionString = require('./lib/connection_string');
export import endpoint = require('./lib/endpoint');
export import errors = require('./lib/errors');
export import results = require('./lib/results');
export import Message = require('./lib/message');
export import SharedAccessSignature = require('./lib/shared_access_signature');

// Typescript only, used by other modules in azure-iot-sdk
export interface X509 {
    // https://nodejs.org/api/tls.html#tls_tls_connect_options_callback
    cert: string | string[] | Buffer | Buffer[];
    key: string | Buffer;
    passphrase?: string;
}

export interface Receiver extends EventEmitter {
    on(type: 'message', func: (msg: Message) => void): this;
    on(type: 'errorReceived', func: (err: Error) => void): this;

    on(type: string, func: Function): this;
}

export interface TransportConfig {
    host: string;
    deviceId: string;
    sharedAccessSignature?: string;
    x509?: X509;
}
