// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var device = require('../device.js'); // require('azure-iot-device');

var connectionString = '[IoT Device Connection String]';

var client = new device.Client(connectionString, new device.Https());

// Create a message and send it to the IoT Hub.
var windSpeed = 10 + (Math.random() * 4); // range: [10, 14)
var data = JSON.stringify({ deviceId: 'myFirstDevice', windSpeed: windSpeed });
var message = new device.Message(data);
message.properties.add('myproperty', 'myvalue');
client.sendEvent(message, printResultFor('send'));

// If there are messages waiting for the client, grab the first one.
client.receive(function (err, res, msg) {
  printResultFor('receive')(err, res);
  if (!err && res.statusCode !== 204) {
    console.log('receive data: ' + msg.getData());
    client.complete(msg, printResultFor('complete'));
  }
});

function printResultFor(op) {
  return function printResult(err, res) {
    if (err) console.log(op + ' error: ' + err.toString());
    if (res) console.log(op + ' status: ' + res.statusCode + ' ' + res.statusMessage);
  };
}