// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var device = require('azure-iot-device');

var connectionString = '[IoT Device Connection String]';

var client = new device.Client(connectionString, new device.Https());

// Create a message and send it to the IoT Hub every second
setInterval(function(){
  var windSpeed = 10 + (Math.random() * 4); // range: [10, 14]
  var data = JSON.stringify({ deviceId: 'myFirstDevice', windSpeed: windSpeed });
  var message = new device.Message(data);
  message.properties.add('myproperty', 'myvalue');
  console.log("Sending message: " + message.getData());
  client.sendEvent(message, printResultFor('send'));
}, 1000);

// Monitor notifications from IoT Hub and print them in the console.
setInterval(function(){
  client.receive(function (err, res, msg) {
    if (!err && res.statusCode !== 204) {
      console.log('Received data: ' + msg.getData());
      client.complete(msg, printResultFor('complete'));
    }
    else if (err)
    {
      printResultFor('receive')(err, res);
    }
  });
}, 1000);

// Helper function to print results in the console
function printResultFor(op) {
  return function printResult(err, res) {
    if (err) console.log(op + ' error: ' + err.toString());
    if (res && (res.statusCode !== 204)) console.log(op + ' status: ' + res.statusCode + ' ' + res.statusMessage);
  };
}
