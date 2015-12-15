// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var device = require('azure-iot-device');

// String containing Hostname, Device Id & Device Key in the following formats:
//  "HostName=<iothub_host_name>;DeviceId=<device_id>;SharedAccessKey=<device_key>"
var connectionString = '[IoT Hub device connection string]';

var client = device.Client.fromConnectionString(connectionString, device.Amqp);

// Create a message and send it to the IoT Hub every second
setInterval(function(){
  var windSpeed = 10 + (Math.random() * 4); // range: [10, 14]
  var data = JSON.stringify({ deviceId: 'myFirstDevice', windSpeed: windSpeed });
  var message = new device.Message(data);
  message.properties.add('myproperty', 'myvalue');
  console.log("Sending message: " + message.getData());
  client.sendEvent(message, printResultFor('send'));
}, 1000);

client.getReceiver(function (err, receiver)
{
  receiver.on('message', function (msg) {
    console.log('Id: ' + msg.properties.messageId + ' Body: ' + msg.body);
    receiver.complete(msg, function() {
      console.log('completed');
    });
    // receiver.reject(msg, function() {
    //   console.log('rejected');
    // });
    // receiver.abandon(msg, function() {
    //   console.log('abandoned');
    // });
  });
  receiver.on('errorReceived', function(err)
  {
    console.warn(err);
  });
});

// Helper function to print results in the console
function printResultFor(op) {
  return function printResult(err, res) {
    if (err) console.log(op + ' error: ' + err.toString());
    if (res) console.log(op + ' status: ' + res);
  };
}
