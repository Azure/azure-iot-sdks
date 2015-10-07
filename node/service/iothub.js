// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var common = require('azure-iot-common');

module.exports = {
    Device: require('./lib/device.js'),
    Https: common.Https,
    Registry: require('./lib/registry.js')
};