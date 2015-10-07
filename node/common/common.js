// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

module.exports = {
    endpoint: require('./lib/endpoint.js'),
    Https: require('./lib/https.js'),
    Message: require('./lib/message.js'),
    SimulatedHttps: require('./lib/https_simulated.js')
};