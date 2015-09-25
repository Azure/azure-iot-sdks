// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

/**
 * The `azure-iot-device` module provides a means for devices to send events to and
 * receive notifications from an Azure IoT Hub.  The client handles
 * communication with the IoT Hub through a transport supplied by the caller
 * (e.g., [Https]{@linkcode module:adapters/https.Https}).
 *
 * @example
 * var Device = require('azure-iot-device');
 * var Client = Device.Client;
 * var Message = Device.Message;
 * var Https = Device.Https;
 *
 * function print(err, res) {
 *   if (err) console.log(err.toString());
 *   if (res) console.log(res.statusCode + ' ' + res.statusMessage);
 * }
 *
 * var config = {
 *   host: 'hostname',
 *   keyName: 'deviceId',
 *   key: 'password'
 * };
 *
 * var client = new Client(new Https(config));
 *
 * client.sendEvent(new Message('hello world'), print);
 *
 * client.receive (function (err, res, msg) {
 *   print(err, res);
 *   if (msg) console.log('Notification: ' + msg);
 *   if (!err && res.statusCode !== 204) {
 *     client.complete(msg, print);
 *   }
 * });
 *
 * @module azure-iot-device
 */

'use strict';

var common = require('azure-iot-common');

module.exports = {
    Client: require('./lib/client.js'),
    Https: common.Https,
    Message: common.Message
};