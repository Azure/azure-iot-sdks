// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

export { anHourFromNow, encodeUriComponentStrict } from './lib/authorization';
export import ConnectionString = require('./lib/connection_string');
export import endpoint = require('./lib/endpoint');
export import errors = require('./lib/errors');
export import results = require('./lib/results');
export import Message = require('./lib/message');
export import SharedAccessSignature = require('./lib/shared_access_signature');

// Typescript only, used by other modules in azure-iot-sdk
interface X509 {
    // https://nodejs.org/api/tls.html#tls_tls_connect_options_callback
    cert: string | string[] | Buffer | Buffer[];
    key: string | Buffer;
    passphrase?: string;    
}
