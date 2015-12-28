// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var common = require('azure-iot-common');

/**
 * The Azure IoT Service SDK for Node.js allows applications to send messages
 * to devices and get feedback when they're delivered. It also supports
 * creating, removing, updating, and listing device identities registered with
 * an IoT hub.
 * @module azure-iothub
 */
module.exports = {
  Client: require('./lib/client.js'),
  Device: require('./lib/device.js'),
  Https: common.Https,
  Registry: require('./lib/registry.js')
};